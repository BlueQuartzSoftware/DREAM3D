/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "H5VoxelFileReader.h"

#include <limits>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5VoxelFileReader::H5VoxelFileReader() :
  FileReader(),
  m_InputFile(""),
  m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_PhaseTypesArrayName(DREAM3D::EnsembleData::PhaseTypes),
  m_GrainIds(NULL),
  m_CellPhases(NULL),
  m_CellEulerAngles(NULL),
  m_CrystalStructures(NULL),
  m_PhaseTypes(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5VoxelFileReader::~H5VoxelFileReader()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5VoxelFileReader::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Input File");
    option->setPropertyName("InputFile");
    option->setWidgetType(FilterParameter::InputFileWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5VoxelFileReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setInputFile( reader->readValue( "InputFile", getInputFile() ) );
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5VoxelFileReader::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("InputFile", getInputFile() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5VoxelFileReader::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  
  VolumeDataContainer* m = getVolumeDataContainer();

  if (getInputFile().empty() == true)
  {
    ss << ClassName() << " needs the Input File Set and it was not.";
    setErrorCondition(-387);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }
  else if (QFileInfo::exists(getInputFile()) == false)
  {
    ss << "The input file does not exist.";
    setErrorCondition(-388);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }

  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, CellEulerAngles, float, FloatArrayType, 0, voxels, 3)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, int32_t, Int32ArrayType, 1, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, int32_t, Int32ArrayType, 0, voxels, 1)

  typedef DataArray<unsigned int> XTalStructArrayType;
  typedef DataArray<unsigned int> PTypeArrayType;
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellEnsembleData, CrystalStructures, unsigned int, XTalStructArrayType, Ebsd::CrystalStructure::UnknownCrystalStructure, ensembles, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellEnsembleData, PhaseTypes, unsigned int, PTypeArrayType, DREAM3D::PhaseType::PrimaryPhase, ensembles, 1)

  int err = 0;
  H5VoxelReader::Pointer reader = H5VoxelReader::New();
  reader->setFileName(getInputFile());
  int64_t dims[3];
  float spacing[3];
  float origin[3];
  err = reader->getSizeResolutionOrigin(dims, spacing, origin);

    /* Sanity check what we are trying to load to make sure it can fit in our address space.
     * Note that this does not guarantee the user has enough left, just that the
     * size of the volume can fit in the address space of the program
     */
#if   (CMP_SIZEOF_SSIZE_T==4)
    int64_t max = std::numeric_limits<size_t>::max();
#else
    int64_t max = std::numeric_limits<int64_t>::max();
#endif
    if(dims[0] * dims[1] * dims[2] > max)
    {
      err = -1;
      QTextStream s;
      s << "The total number of elements '" << (dims[0] * dims[1] * dims[2]) << "' is greater than this program can hold. Try the 64 bit version.";
      setErrorCondition(err);
      addErrorMessage(getHumanLabel(), s.str(), -1);
      return;
    }

    if(dims[0] > max || dims[1] > max || dims[2] > max)
    {
      err = -1;
      QTextStream s;
      s << "One of the dimensions is greater than the max index for this sysem. Try the 64 bit version.";
      s << " dim[0]=" << dims[0] << "  dim[1]=" << dims[1] << "  dim[2]=" << dims[2];
      setErrorCondition(err);
      addErrorMessage(getHumanLabel(), s.str(), -1);
      return;
    }
    /* ************ End Sanity Check *************************** */
    size_t dcDims[3] =
    { dims[0], dims[1], dims[2] };
    m->setDimensions(dcDims);
    m->setResolution(spacing);
    m->setOrigin(origin);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5VoxelFileReader::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5VoxelFileReader::execute()
{
  if(NULL == getVolumeDataContainer())
  {
    
    ss << "DataContainer Pointer was NULL and Must be valid." << __FILE__ << "(" << __LINE__<<")";
    addErrorMessage(getHumanLabel(), ss.str(), -1);
    setErrorCondition(-1);
  }

  int err = 0;
  H5VoxelReader::Pointer reader = H5VoxelReader::New();
  reader->setFileName(getInputFile());
  int64_t volDims[3];
  float spacing[3];
  float origin[3];
  err = reader->getSizeResolutionOrigin(volDims, spacing, origin);
  if(err < 0)
  {
    PipelineMessage em (getHumanLabel(), "Error Reading the Dimensions, Origin and Scaling values from the HDF5 Voxel File", -1);
    addErrorMessage(em);
  }
  size_t dcDims[3] = {volDims[0], volDims[1], volDims[2]};
  getVolumeDataContainer()->setDimensions(dcDims);
  getVolumeDataContainer()->setResolution(spacing);
  getVolumeDataContainer()->setOrigin(origin);

  size_t totalpoints = volDims[0] * volDims[1] * volDims[2];
  // Create an DataArray to hold the data
  DataArray<int>::Pointer grainIds = DataArray<int>::CreateArray(totalpoints, DREAM3D::CellData::GrainIds);
  DataArray<int>::Pointer phases = DataArray<int>::CreateArray(totalpoints, DREAM3D::CellData::Phases);
  DataArray<float>::Pointer eulers = DataArray<float>::CreateArray(totalpoints*3, DREAM3D::CellData::EulerAngles);
  eulers->SetNumberOfComponents(3);

  QString arrayname = "GrainID";
  err = reader->readScalarData<int>(arrayname, grainIds->GetPointer(0));
  if(err < 0)
  {
    setErrorCondition(err);
    PipelineMessage em (getHumanLabel(), "Error Reading the GrainIDs from the .h5voxel file.", err);
    addErrorMessage(em);
    grainIds = DataArray<int>::NullPointer();
  }
  arrayname = "PhaseID";
  err = reader->readScalarData<int>(arrayname, phases->GetPointer(0));
  if(err < 0)
  {
    setErrorCondition(err);
    PipelineMessage em (getHumanLabel(), "Error Reading the Phases from the .h5voxel file.", err);
    addErrorMessage(em);
    grainIds = DataArray<int>::NullPointer();
  }
  arrayname = "Euler Angles";
  err = reader->readScalarData<float>(arrayname, eulers->GetPointer(0));
  if(err < 0)
  {
    setErrorCondition(err);
    PipelineMessage em (getHumanLabel(), "Error Reading the Euler Angles from the .h5voxel file.", err);
    addErrorMessage(em);
    grainIds = DataArray<int>::NullPointer();
  }

  getVolumeDataContainer()->addCellData(DREAM3D::CellData::GrainIds, grainIds);
  getVolumeDataContainer()->addCellData(DREAM3D::CellData::Phases, phases);
  getVolumeDataContainer()->addCellData(DREAM3D::CellData::EulerAngles, eulers);

  size_t gnum = 0;
  size_t maxId = 0;
  for(size_t i= 0; i < totalpoints; i++)
  {
    gnum = size_t(grainIds->GetValue(i));
    if(gnum > maxId) maxId = gnum;
  }
  getVolumeDataContainer()->resizeFieldDataArrays(maxId+1);

  QVector<unsigned int> crystruct;
  QVector<unsigned int> phaseType;

  arrayname = "CrystalStructure";
  err = reader->readFieldData<unsigned int, uint32_t>(arrayname, crystruct);
  if(err < 0)
  {
    PipelineMessage em (getHumanLabel(), "H5VoxelReader Error Reading the Crystal Structure Field Data", err);
    addErrorMessage(em);
  }
  arrayname = "PhaseType";
  err = reader->readFieldData<unsigned int, uint32_t>(arrayname, phaseType);
  if(err < 0)
  {
    PipelineMessage em (getHumanLabel(), "H5VoxelReader Error Reading the Phase Type Data", err);
    addErrorMessage(em);
  }

  DataArray<unsigned int>::Pointer crystructs = DataArray<unsigned int>::CreateArray(crystruct.size(), DREAM3D::EnsembleData::CrystalStructures);
  DataArray<unsigned int>::Pointer phaseTypes = DataArray<unsigned int>::CreateArray(phaseType.size(), DREAM3D::EnsembleData::PhaseTypes);

  for (size_t i = 0; i < crystruct.size(); i++)
  {
    crystructs->SetValue(i,crystruct[i]);
    phaseTypes->SetValue(i,phaseType[i]);
  }
  getVolumeDataContainer()->addCellEnsembleData(DREAM3D::EnsembleData::CrystalStructures, crystructs);
  getVolumeDataContainer()->addCellEnsembleData(DREAM3D::EnsembleData::PhaseTypes, phaseTypes);
}
