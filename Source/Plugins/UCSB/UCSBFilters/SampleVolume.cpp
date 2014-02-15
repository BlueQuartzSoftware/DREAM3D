/*
 * Your License or Copyright Information can go here
 */

#include "SampleVolume.h"
#include "DREAM3DLib/Common/Observer.h"

#include "DREAM3DLib/IOFilters/DataContainerReader.h"
#include "DREAM3DLib/SamplingFilters/CropVolume.h"
#include "DREAM3DLib/IOFilters/DataContainerWriter.h"

//reader
#include "H5Support/H5Utilities.h"
#include "H5Support/H5Lite.h"
#include "MXA/Utilities/MXAFileInfo.h"
#include "DREAM3DLib/IOFilters/VoxelDataContainerReader.h"
#include "DREAM3DLib/IOFilters/SurfaceMeshDataContainerReader.h"
#include "DREAM3DLib/IOFilters/SolidMeshDataContainerReader.h"
#include "DREAM3DLib/FilterParameters/H5FilterParametersReader.h"
#include "DREAM3DLib/Common/FilterManager.h"
#include "H5Support/HDF5ScopedFileSentinel.h"

//random numbers
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SampleVolume::SampleVolume() :
AbstractFilter(),
//file reader
m_InputFile(""),

//file writting
m_OutputDirectory(""),
m_FileName(""),
m_WriteXdmf(false),

//cropping
m_NumberVolumes(NULL)
{
  m_BoxSize.x = 10;
  m_BoxSize.y = 10;
  m_BoxSize.z = 10;
  m_Dims.x = 0;
  m_Dims.y = 0;
  m_Dims.z = 0;
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SampleVolume::~SampleVolume()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleVolume::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Input File");
    option->setPropertyName("InputFile");
    option->setWidgetType(FilterParameter::InputFileWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Number of Volumes");
    option->setPropertyName("NumberVolumes");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Sampling Size");
    option->setPropertyName("BoxSize");
    option->setWidgetType(FilterParameter::IntVec3Widget);
    option->setValueType("IntVec3Widget_t");
    option->setUnits("voxels");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Sample Dimensions");
    option->setPropertyName("Dims");
    option->setWidgetType(FilterParameter::IntVec3Widget);
    option->setValueType("IntVec3Widget_t");
    option->setUnits("voxels");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output Directory");
    option->setPropertyName("OutputDirectory");
    option->setWidgetType(FilterParameter::OutputPathWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output File Prefix");
    option->setPropertyName("FileName");
    option->setWidgetType(FilterParameter::StringWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Xdmf (ParaView Compatible File)");
    option->setPropertyName("WriteXdmf");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleVolume::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);

  //reader
  setInputFile( reader->readValue("InputFile", getInputFile() ) );

  //cropping
  setNumberVolumes( reader->readValue( "NumberVolumes", getNumberVolumes() ) );
  setBoxSize( reader->readValue( "BoxSize", getBoxSize() ) );
  setDims( reader->readValue( "Dims", getDims() ) );

  //writer
  setOutputDirectory( reader->readValue( "OutputDirectory", getOutputDirectory() ) );
  setFileName( reader->readValue( "FileName", getFileName() ) );
  setWriteXdmf( reader->readValue( "WriteXdmf", getWriteXdmf() ) );

  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SampleVolume::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)

{
  //reader
  writer->openFilterGroup(this, index);
  writer->writeValue("InputFile", getInputFile() );

  //cropping
  writer->writeValue("NumberVolumes", getNumberVolumes() );
  writer->writeValue("BoxSize", getBoxSize() );
  writer->writeValue("Dims", getDims() );

  //writer
  writer->writeValue("OutputDirectory", getOutputDirectory() );
  writer->writeValue("FileName", getFileName() );
  writer->writeValue("WriteXdmf", getWriteXdmf() );
  writer->closeFilterGroup();

  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleVolume::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  int32_t err = 0;

  //check for output director
  if (m_OutputDirectory.empty() == true)
  {
    setErrorCondition(-1003);
    addErrorMessage(getHumanLabel(), "Output Directory is Not set correctly", -382);
  }

  if (getInputFile().empty() == true)
  {
    ss << ClassName() << " needs the Input File Set and it was not.";
    setErrorCondition(-387);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }
  else if (MXAFileInfo::exists(getInputFile()) == false)
  {
    ss << "The input file does not exist.";
    setErrorCondition(-388);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }
  else
  {
    // Read the Meta Data and Array names from the file
    hid_t fileId = H5Utilities::openFile(m_InputFile, true); // Open the file Read Only
    if(fileId < 0)
    {
      ss.str("");
      ss << ": Error opening input file '" << m_InputFile << "'";
      setErrorCondition(-150);
      addErrorMessage(getHumanLabel(), ss.str(), err);
      return;
    }

    // This will make sure if we return early from this method that the HDF5 File is properly closed.
    HDF5ScopedFileSentinel scopedFileSentinel(&fileId, true);

      /* READ THE VOXEL DATA TO THE HDF5 FILE */
      if (getVoxelDataContainer() != NULL)
      {
        VoxelDataContainerReader::Pointer voxelReader = VoxelDataContainerReader::New();
        voxelReader->setHdfFileId(fileId);
        voxelReader->setVoxelDataContainer(getVoxelDataContainer());
        voxelReader->setObservers(getObservers());
        ss.str("");
        ss << getMessagePrefix() << " |--> Reading Voxel Data ";
        voxelReader->setMessagePrefix(ss.str());
        voxelReader->preflight();
        if (voxelReader->getErrorCondition() < 0)
        {
          setErrorCondition(voxelReader->getErrorCondition());
          addErrorMessage(getHumanLabel(), "The voxel data was not available in the data file.", getErrorCondition());
        }
      }
  }


  FilterPipeline::Pointer pipeline = buildPipeline(0, 0, m_BoxSize.x-1, 0, m_BoxSize.y-1, 0, m_BoxSize.z-1);
  pipeline->preflightPipeline();
  err = pipeline->getErrorCondition();
  if(err<0)
  {
    setErrorCondition(-1);
    addErrorMessage(getHumanLabel(), "Errors in sub-pipeline", -382);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleVolume::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleVolume::execute()
{
  int err = 0;
  setErrorCondition(err);
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", getErrorCondition());
    return;
  }
  setErrorCondition(0);

  /* Place all your code to execute your filter here. */
/*
  //get size of sample
  size_t udims[3] =
  { 0, 0, 0 };
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  { static_cast<DimType>(udims[0]), static_cast<DimType>(udims[1]), static_cast<DimType>(udims[2]), };
  const int xMax = dims[0]-m_BoxSize.x;
  const int yMax = dims[1]-m_BoxSize.y;
  const int zMax = dims[2]-m_BoxSize.z;
*/
  const int xMax = m_Dims.x-m_BoxSize.x;
  const int yMax = m_Dims.y-m_BoxSize.y;
  const int zMax = m_Dims.z-m_BoxSize.z;

  //set up generation
  const int rangeMin = 0;
  typedef boost::uniform_int<int> NumberDistribution;
  typedef boost::mt19937 RandomNumberGenerator;
  typedef boost::variate_generator<RandomNumberGenerator&, NumberDistribution> Generator;

  NumberDistribution xDist(0, xMax);
  NumberDistribution yDist(0, yMax);
  NumberDistribution zDist(0, zMax);
  RandomNumberGenerator generator;
  Generator numberGeneratorX(generator, xDist);
  Generator numberGeneratorY(generator, yDist);
  Generator numberGeneratorZ(generator, zDist);
  generator.seed(static_cast<boost::uint32_t>( MXA::getMilliSeconds() )); // seed with the current time

  //generate random numbers
  std::vector<int> xCord;
  std::vector<int> yCord;
  std::vector<int> zCord;

  for(int i=0; i<m_NumberVolumes; i++)
  {
    xCord.push_back(numberGeneratorX());
    yCord.push_back(numberGeneratorY());
    zCord.push_back(numberGeneratorZ());
  }

  //generate volumes
  for(int i=0; i<m_NumberVolumes; i++)
  {
    int x1, x2, y1, y2, z1, z2;//bounding box
    x1=xCord[i];
    y1=zCord[i];
    z1=yCord[i];
    x2=x1+m_BoxSize.x-1;
    y2=y1+m_BoxSize.y-1;
    z2=z1+m_BoxSize.z-1;

    FilterPipeline::Pointer pipeline = buildPipeline(i+1, x1, x2, y1, y2, z1, z2);
    pipeline->preflightPipeline();
    err = pipeline->getErrorCondition();
    if(err<0)
    {
      setErrorCondition(err);
      notifyErrorMessage("Error in subpipeline", err);
    }
    pipeline->run();
    err = pipeline->getErrorCondition();
    if(err<0)
    {
      setErrorCondition(err);
      notifyErrorMessage("Error in subpipeline", err);
    }
    pipeline->clear();
  }

  /* Let the GUI know we are done with this filter */
   notifyStatusMessage("Complete");
}

FilterPipeline::Pointer SampleVolume::buildPipeline(int append, int x1, int x2, int y1, int y2, int z1, int z2)
{
  //make pipeline
  FilterPipeline::Pointer pipeline = FilterPipeline::New();

  //set up reader
  DataContainerReader::Pointer reader = DataContainerReader::New();
  reader->setInputFile(m_InputFile);
  reader->setReadVoxelData(true);
  reader->setReadAllArrays(true);
  reader->setObservers(getObservers());
  pipeline->pushBack(reader);

  //set up crop
  CropVolume::Pointer crop = CropVolume::New();
  crop->setXMin(x1);
  crop->setYMin(y1);
  crop->setZMin(z1);
  crop->setXMax(x2);
  crop->setYMax(y2);
  crop->setZMax(z2);
  crop->setRenumberGrains(false);
  crop->setUpdateOrigin(true);
  crop->setObservers(getObservers());
  pipeline->pushBack(crop);

  //set up writer
  std::stringstream ss;
  ss<<"_"<<append;
  DataContainerWriter::Pointer writer = DataContainerWriter::New();
  std::string dream_3d_file = m_OutputDirectory + m_FileName + ss.str();
  writer->setOutputFile(dream_3d_file);
  writer->setWriteVoxelData(true);
  writer->setWriteSurfaceMeshData(false);
  writer->setWriteXdmfFile(m_WriteXdmf);
  writer->setObservers(getObservers());
  pipeline->pushBack(writer);

  return pipeline;
}
