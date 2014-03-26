/*
 * Your License or Copyright Information can go here
 */

#include "SampleVolume.h"

#include "MXA/Utilities/MXAFileInfo.h"
#include "DREAM3DLib/Common/Observer.h"
#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/SamplingFilters/CropVolume.h"
#include "CropSurfaceMesh.h"
#include "DREAM3DLib/IOFilters/DataContainerWriter.h"
#include "DREAM3DLib/IOFilters/DataContainerReader.h"
#include "DREAM3DLib/IOFilters/VoxelDataContainerReader.h"
#include "DREAM3DLib/IOFilters/SurfaceMeshDataContainerReader.h"
#include "DREAM3DLib/IOFilters/SolidMeshDataContainerReader.h"
#include "DREAM3DLib/FilterParameters/H5FilterParametersReader.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SampleVolume::SampleVolume() :
AbstractFilter(),
m_NumberVolumes(0),
m_OutputDirectory(""),
m_FileName(""),
m_CopySurfaceMesh(true),
m_WriteXdmf(false),
m_UpdateOrigin(false)
{
  m_BoxSize.x = 10;
  m_BoxSize.y = 50;
  m_BoxSize.z = 50;
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
    option->setHumanLabel("Copy Surface Mesh");
    option->setPropertyName("CopySurfaceMesh");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
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
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Update Origin");
    option->setPropertyName("UpdateOrigin");
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
  setNumberVolumes( reader->readValue( "NumberVolumes", getNumberVolumes() ) );
  setBoxSize( reader->readValue( "BoxSize", getBoxSize() ) );
  setOutputDirectory( reader->readValue( "OutputDirectory", getOutputDirectory() ) );
  setFileName( reader->readValue( "FileName", getFileName() ) );
  setWriteXdmf( reader->readValue( "WriteXdmf", getWriteXdmf() ) );
  setUpdateOrigin( reader->readValue( "UpdateOrigin", getUpdateOrigin() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SampleVolume::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)

{
  writer->openFilterGroup(this, index);
  writer->writeValue("NumberVolumes", getNumberVolumes() );
  writer->writeValue("BoxSize", getBoxSize() );
  writer->writeValue("OutputDirectory", getOutputDirectory() );
  writer->writeValue("FileName", getFileName() );
  writer->writeValue("WriteXdmf", getWriteXdmf() );
  writer->writeValue("UpdateOrigin", getUpdateOrigin() );
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
  VoxelDataContainer* m = getVoxelDataContainer();

  //check for output director
  if (m_OutputDirectory.empty() == true)
  {
    setErrorCondition(-1003);
    addErrorMessage(getHumanLabel(), "Output Directory is Not set correctly", -382);
  }

  //make sure box fits within sample
  if (m_BoxSize.x > static_cast<int64_t>(m->getXPoints()))
  {
    ss.str("");
    ss << "The X sampling size you have entered of " << m_BoxSize.x << " is greater than your sample X size of " << static_cast<int64_t>(m->getXPoints());
    addErrorMessage(getHumanLabel(), ss.str(), -5001);
    setErrorCondition(-5001);
  }
  if (m_BoxSize.y > static_cast<int64_t>(m->getYPoints()))
  {
    ss.str("");
    ss << "The Y sampling size you have entered of " << m_BoxSize.y << " is greater than your sample Y size of " << static_cast<int64_t>(m->getXPoints());
    addErrorMessage(getHumanLabel(), ss.str(), -5002);
    setErrorCondition(-5002);
  }
  if (m_BoxSize.z > static_cast<int64_t>(m->getZPoints()))
  {
    ss.str("");
    ss << "The Z sampling size you have entered of " << m_BoxSize.z << " is greater than your sample Z size of " << static_cast<int64_t>(m->getXPoints());
    addErrorMessage(getHumanLabel(), ss.str(), -5003);
    setErrorCondition(-5003);
  }

  if(getErrorCondition()>0)
  {
    SubFilterPipeline::Pointer pipeline = buildPipeline(0, 0, 0, 0);
    err = pipeline->preflightPipeline();
    if(err<0)
    {
      setErrorCondition(-1);
      addErrorMessage(getHumanLabel(), "Errors in sub-pipeline", err);
    }
  }
}

void SampleVolume::dataCheckSurfaceMesh(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if(NULL == sm)
  {
    addErrorMessage(getHumanLabel(), "SurfaceMeshDataContainer is missing", -383);
    setErrorCondition(-383);
  }
  else
  {
    // We MUST have Nodes
    if(sm->getVertices().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", -384);
      setErrorCondition(-384);
    }

    // We MUST have Triangles defined also.
    if(sm->getFaces().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", -385);
      setErrorCondition(-385);
    }
    else
    {
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleVolume::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  if(m_CopySurfaceMesh)dataCheckSurfaceMesh(true, 1, 1, 1);
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

  //set bounds for valid sampling locations
  const int xMax = dims[0]-m_BoxSize.x;
  const int yMax = dims[1]-m_BoxSize.y;
  const int zMax = dims[2]-m_BoxSize.z;

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
    int x1, y1, z1;
    x1=xCord[i];
    y1=yCord[i];
    z1=zCord[i];

    // In theory, the subpipeline was already constucted and preflight run on it so
    // there is no need to rerun it here. Just let it fly and see what happens.
    SubFilterPipeline::Pointer pipeline = buildPipeline(i+1, x1, y1, z1);

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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SubFilterPipeline::Pointer SampleVolume::buildPipeline(int append, int x, int y, int z)
{
  //make a subpipeline and populate with empty SurfaceMesh and Solid DataContainers
  SubFilterPipeline::Pointer pipeline = SubFilterPipeline::New();
  SolidMeshDataContainer::Pointer solid = SolidMeshDataContainer::New();
  pipeline->setSolidMeshDataContainer(solid);

  // Get the Current VoxelDataContainer;
  VoxelDataContainer* m = getVoxelDataContainer();

  // Create a new one for the sub pipeline to use
  VoxelDataContainer::Pointer copy = VoxelDataContainer::New();
  // Get a copy of the observers from the current VoxelDataContainer
  copy->setObservers(m->getObservers() );
  // Most likely our error messages are going to go to a console somewhere. It is probably best
  // to sanity check the values in the actual preflight of this filter where you can figure out
  // if the user entries are going to work.


  // Copy out the Dims, Origin, Resolution from our current Voxel Data Container to the new one
  size_t dims[3] = { 0,0,0};
  m->getDimensions(dims);
  copy->setDimensions(dims);

  float origin[3] = {0.0f, 0.0f, 0.0f};
  m->getOrigin(origin);
  copy->setOrigin(origin);

  float res[3] = { 0.0f, 0.0f, 0.0f};
  m->getResolution(res);
  copy->setResolution(res);

  // Now get a list of all the CellArrayNames to make deep copies of
  std::list<std::string> arrayNames = m->getCellArrayNameList();
  for (std::list<std::string>::iterator iter = arrayNames.begin(); iter != arrayNames.end(); ++iter)
  {
    std::string name = *iter;
    IDataArray::Pointer p = m->getCellData(name);
    IDataArray::Pointer dataCopy = p->deepCopy();
    copy->addCellData(name, dataCopy);
  }
  // Now get a list of all the Field Array Names to make deep copies of
  arrayNames = m->getFieldArrayNameList();
  for (std::list<std::string>::iterator iter = arrayNames.begin(); iter != arrayNames.end(); ++iter)
  {
    std::string name = *iter;
    IDataArray::Pointer p = m->getFieldData(name);
    IDataArray::Pointer dataCopy = p->deepCopy();
    copy->addFieldData(name, dataCopy);
  }

// Now get a list of all the Ensemble Array Names to make deep copies of
  arrayNames = m->getEnsembleArrayNameList();
  for (std::list<std::string>::iterator iter = arrayNames.begin(); iter != arrayNames.end(); ++iter)
  {
    std::string name = *iter;
    IDataArray::Pointer p = m->getEnsembleData(name);
    IDataArray::Pointer dataCopy = p->deepCopy();
    copy->addEnsembleData(name, dataCopy);
  }

  pipeline->setVoxelDataContainer(copy);

  ///copy surface mesh
  if(m_CopySurfaceMesh)
  {
    // Get the Current VoxelDataContainer;
    SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();

    // Create a new one for the sub pipeline to use
    SurfaceMeshDataContainer::Pointer smCopy = SurfaceMeshDataContainer::New();
    // Get a copy of the observers from the current VoxelDataContainer
    smCopy->setObservers(sm->getObservers() );
    // Most likely our error messages are going to go to a console somewhere. It is probably best
    // to sanity check the values in the actual preflight of this filter where you can figure out
    // if the user entries are going to work.

    // Now get a list of all the Vertex Array Names to make deep copies of
    std::list<std::string> arrayNames = sm->getPointArrayNameList();
    for (std::list<std::string>::iterator iter = arrayNames.begin(); iter != arrayNames.end(); ++iter)
    {
      std::string name = *iter;
      IDataArray::Pointer p = sm->getVertexData(name);
      IDataArray::Pointer dataCopy = p->deepCopy();
      smCopy->addVertexData(name, dataCopy);
    }
    // Now get a list of all the Face Array Names to make deep copies of
    arrayNames = sm->getFaceArrayNameList();
    for (std::list<std::string>::iterator iter = arrayNames.begin(); iter != arrayNames.end(); ++iter)
    {
      std::string name = *iter;
      IDataArray::Pointer p = sm->getFaceData(name);
      IDataArray::Pointer dataCopy = p->deepCopy();
      smCopy->addFaceData(name, dataCopy);
    }

  // Now get a list of all the Ensemble Array Names to make deep copies of
    arrayNames = sm->getEnsembleArrayNameList();
    for (std::list<std::string>::iterator iter = arrayNames.begin(); iter != arrayNames.end(); ++iter)
    {
      std::string name = *iter;
      IDataArray::Pointer p = sm->getEnsembleData(name);
      IDataArray::Pointer dataCopy = p->deepCopy();
      smCopy->addEnsembleData(name, dataCopy);
    }

    //copy faces
    DREAM3D::SurfaceMesh::FaceListPointer_t pTriagnles = sm->getFaces();
    if(0!=pTriagnles)
    {
      DREAM3D::SurfaceMesh::FaceListPointer_t copyTriangles = boost::dynamic_pointer_cast<DREAM3D::SurfaceMesh::FaceList_t>(pTriagnles->deepCopy());
      smCopy->setFaces(copyTriangles);
    }

    //copy vertices
    DREAM3D::SurfaceMesh::VertListPointer_t pNodes = sm->getVertices();
    if(0!=pNodes)
    {
      DREAM3D::SurfaceMesh::VertListPointer_t copyNodes = boost::dynamic_pointer_cast<DREAM3D::SurfaceMesh::VertList_t>(pNodes->deepCopy());
      smCopy->setVertices(copyNodes);
    }

    //dont need to copy edge or field data since it will be deleted by surface mesh crop anyway
    pipeline->setSurfaceMeshDataContainer(smCopy);

    //set up surface mesh crop
    CropSurfaceMesh::Pointer smCrop = CropSurfaceMesh::New();

    smCrop->setXMin(x);
    smCrop->setYMin(y);
    smCrop->setZMin(z);
    smCrop->setXMax(x+m_BoxSize.x-1);
    smCrop->setYMax(y+m_BoxSize.y-1);
    smCrop->setZMax(z+m_BoxSize.z-1);
    smCrop->setUpdateOrigin(m_UpdateOrigin);
    smCrop->setObservers(getObservers());
    pipeline->pushBack(smCrop);
  }
  else
  {
    SurfaceMeshDataContainer::Pointer smNew = SurfaceMeshDataContainer::New();
    pipeline->setSurfaceMeshDataContainer(smNew);
  }

  //set up volume crop
  CropVolume::Pointer crop = CropVolume::New();
  crop->setXMin(x);
  crop->setYMin(y);
  crop->setZMin(z);
  crop->setXMax(x+m_BoxSize.x-1);
  crop->setYMax(y+m_BoxSize.y-1);
  crop->setZMax(z+m_BoxSize.z-1);
  crop->setRenumberGrains(false);
  crop->setUpdateOrigin(m_UpdateOrigin);
  crop->setObservers(getObservers());
  pipeline->pushBack(crop);

  //set up writer
  std::stringstream ss;
  ss<<"_"<<append;
  DataContainerWriter::Pointer writer = DataContainerWriter::New();
  std::string dream_3d_file = m_OutputDirectory + "/" + m_FileName + ss.str();
  writer->setOutputFile(MXAFileInfo::toNativeSeparators(dream_3d_file));
  writer->setWriteVoxelData(true);
  writer->setWriteSurfaceMeshData(m_CopySurfaceMesh);
  writer->setWriteXdmfFile(m_WriteXdmf);
  writer->setObservers(getObservers());
  pipeline->pushBack(writer);

  return pipeline;
}
