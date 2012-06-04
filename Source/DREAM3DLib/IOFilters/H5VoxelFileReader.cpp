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
m_GrainIds(NULL),
m_CellPhases(NULL),
m_CellEulerAngles(NULL)
{
	setupFilterOptions();
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
void H5VoxelFileReader::setupFilterOptions()
{
  std::vector<FilterOption::Pointer> options;
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Input File");
    option->setPropertyName("InputFile");
    option->setWidgetType(FilterOption::InputFileWidget);
    option->setValueType("string");
    options.push_back(option);
  }
  setFilterOptions(options);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5VoxelFileReader::writeFilterOptions(AbstractFilterOptionsWriter* writer)
{
  writer->writeValue("InputFile", getInputFile() );
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5VoxelFileReader::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  DataContainer* m = getDataContainer();

  if (getInputFile().empty() == true)
  {
    std::stringstream ss;
    ss << ClassName() << " needs the Input File Set and it was not.";
    setErrorMessage(ss.str());
    setErrorCondition(-387);
  }

  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, CellEulerAngles, ss, float, FloatArrayType, 0, voxels, 3);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, ss, int32_t, Int32ArrayType, 1, voxels, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, int32_t, Int32ArrayType, 0, voxels, 1);

  typedef DataArray<unsigned int> XTalStructArrayType;
  CREATE_NON_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, unsigned int, XTalStructArrayType, Ebsd::CrystalStructure::Cubic, ensembles, 1);
}

void H5VoxelFileReader::preflight()
{
  dataCheck(true, 1, 1, 1);
}
void H5VoxelFileReader::execute()
{
  if(NULL == getDataContainer())
  {
    std::stringstream ss;
    ss << "DataContainer Pointer was NULL and Must be valid." << __FILE__ << "(" << __LINE__<<")";
    setErrorMessage(ss.str());
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
    setErrorMessage("Error Reading the Dimensions, Origin and Scaling values from the HDF5 Voxel File");
  }
  size_t dcDims[3] = {volDims[0], volDims[1], volDims[2]};
  getDataContainer()->setDimensions(dcDims);
  getDataContainer()->setResolution(spacing);
  getDataContainer()->setOrigin(origin);

  size_t totalpoints = volDims[0] * volDims[1] * volDims[2];
  // Create an DataArray to hold the data
  DataArray<int>::Pointer grainIds = DataArray<int>::CreateArray(totalpoints, DREAM3D::CellData::GrainIds);
  DataArray<int>::Pointer phases = DataArray<int>::CreateArray(totalpoints, DREAM3D::CellData::Phases);
  DataArray<float>::Pointer eulers = DataArray<float>::CreateArray(totalpoints*3, DREAM3D::CellData::EulerAngles);
  
  std::string arrayname = "GrainID";
  err = reader->readScalarData<int>(arrayname, grainIds->GetPointer(0));
  if(err < 0)
  {
    setErrorCondition(err);
    setErrorMessage("Error Reading the GrainIDs from the .h5voxel file.");
    grainIds = DataArray<int>::NullPointer();
  }
  arrayname = "PhaseID";
  err = reader->readScalarData<int>(arrayname, phases->GetPointer(0));
  if(err < 0)
  {
    setErrorCondition(err);
    setErrorMessage("Error Reading the Phases from the .h5voxel file.");
    grainIds = DataArray<int>::NullPointer();
  }
  arrayname = "Euler Angles";
  err = reader->readScalarData<float>(arrayname, eulers->GetPointer(0));
  if(err < 0)
  {
    setErrorCondition(err);
    setErrorMessage("Error Reading the Euler Angles from the .h5voxel file.");
    grainIds = DataArray<int>::NullPointer();
  }

  getDataContainer()->addCellData(DREAM3D::CellData::GrainIds, grainIds);
  getDataContainer()->addCellData(DREAM3D::CellData::Phases, phases);
  getDataContainer()->addCellData(DREAM3D::CellData::EulerAngles, eulers);

  size_t gnum = 0;
  size_t maxId = 0;
  for(size_t i= 0; i < totalpoints; i++)
  {
	  gnum = size_t(grainIds->GetValue(i));
	  if(gnum > maxId) maxId = gnum;
  }
  getDataContainer()->resizeFieldDataArrays(maxId+1);

  std::vector<unsigned int> crystruct;
  std::vector<unsigned int> phaseType;

  arrayname = "CrystalStructure";
  err = reader->readFieldData<unsigned int, uint32_t>(arrayname, crystruct);
  if(err < 0)
  {
      setErrorMessage("H5VoxelReader Error Reading the Crystal Structure Field Data");
  }
  arrayname = "PhaseType";
  err = reader->readFieldData<unsigned int, uint32_t>(arrayname, phaseType);
  if(err < 0)
  {
      setErrorMessage("H5VoxelReader Error Reading the Phase Type Data");
  }

  DataArray<unsigned int>::Pointer crystructs = DataArray<unsigned int>::CreateArray(crystruct.size(), DREAM3D::EnsembleData::CrystalStructures);
  DataArray<unsigned int>::Pointer phaseTypes = DataArray<unsigned int>::CreateArray(phaseType.size(), DREAM3D::EnsembleData::PhaseTypes);

  for (size_t i = 0; i < crystruct.size(); i++)
  {
	  crystructs->SetValue(i,crystruct[i]);
	  phaseTypes->SetValue(i,phaseType[i]);
  }
  getDataContainer()->addEnsembleData(DREAM3D::EnsembleData::CrystalStructures, crystructs);
  getDataContainer()->addEnsembleData(DREAM3D::EnsembleData::PhaseTypes, phaseTypes);
}
