/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#include "InitializeSyntheticVolume.h"

#include "DREAM3DLib/IOFilters/DataContainerReader.h"
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InitializeSyntheticVolume::InitializeSyntheticVolume() :
AbstractFilter(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_CellPhasesArrayName(DREAM3D::CellData::Phases),
m_ShapeTypesArrayName(DREAM3D::EnsembleData::ShapeTypes),
m_InputFile(""),
m_XVoxels(0),
m_YVoxels(0),
m_ZVoxels(0),
m_XRes(1.0f),
m_YRes(1.0f),
m_ZRes(1.0f),
m_GrainIds(NULL),
m_CellPhases(NULL)
{
  setupFilterOptions();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InitializeSyntheticVolume::~InitializeSyntheticVolume()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeSyntheticVolume::setupFilterOptions()
{
 std::vector<FilterOption::Pointer> options;
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Statistics File");
    option->setPropertyName("InputFile");
    option->setWidgetType(FilterOption::InputFileWidget);
    option->setValueType("string");
    options.push_back(option);
  }
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("X Voxels");
    option->setPropertyName("XVoxels");
    option->setWidgetType(FilterOption::IntWidget);
    option->setValueType("int");
    options.push_back(option);
  }
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Y Voxels");
    option->setPropertyName("YVoxels");
    option->setWidgetType(FilterOption::IntWidget);
    option->setValueType("int");
    options.push_back(option);
  }
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Z Voxels");
    option->setPropertyName("ZVoxels");
    option->setWidgetType(FilterOption::IntWidget);
    option->setValueType("int");
    options.push_back(option);
  }
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("X Res");
    option->setPropertyName("XRes");
    option->setWidgetType(FilterOption::DoubleWidget);
    option->setValueType("float");
    options.push_back(option);
  }
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Y Res");
    option->setPropertyName("YRes");
    option->setWidgetType(FilterOption::DoubleWidget);
    option->setValueType("float");
    options.push_back(option);
  }
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Z Res");
    option->setPropertyName("ZRes");
    option->setWidgetType(FilterOption::DoubleWidget);
    option->setValueType("float");
    options.push_back(option);
  }
  setFilterOptions(options);
}
// -----------------------------------------------------------------------------
void InitializeSyntheticVolume::writeFilterOptions(AbstractFilterOptionsWriter* writer)
{
  writer->writeValue("InputFile", getInputFile() );
  writer->writeValue("X Voxels", getXVoxels() );
  writer->writeValue("Y Voxels", getYVoxels() );
  writer->writeValue("Z Voxels", getZVoxels() );
  writer->writeValue("X Res", getXRes() );
  writer->writeValue("Y Res", getYRes() );
  writer->writeValue("Z Res", getZRes() );
}

#define INIT_SYNTH_VOLUME_CHECK(var, errCond) \
if (m_##var <= 0) { ss << ":" <<  #var << " must be a value > 0\n"; setErrorCondition(errCond); }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeSyntheticVolume::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  DataContainer* m = getDataContainer();

  //Cell Data
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, int32_t, Int32ArrayType, -1, voxels, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, ss, int32_t, Int32ArrayType, 0, voxels, 1);

  if(m_InputFile.empty() == true)
  {
    ss << ": The intput file must be set before executing this filter.\n";
    setErrorCondition(-800);
    addErrorMessage(getNameOfClass(), ss.str(), -800);
  }

  INIT_SYNTH_VOLUME_CHECK(XVoxels, -5000);
  INIT_SYNTH_VOLUME_CHECK(YVoxels, -5001);
  INIT_SYNTH_VOLUME_CHECK(ZVoxels, -5002);
  INIT_SYNTH_VOLUME_CHECK(XRes, -5003);
  INIT_SYNTH_VOLUME_CHECK(YRes, -5004);
  INIT_SYNTH_VOLUME_CHECK(ZRes, -5005);



  addRequiredEnsembleData(DREAM3D::EnsembleData::ShapeTypes);
  if (m_ShapeTypes.get() ==  NULL || m_ShapeTypes->GetNumberOfTuples() == 0)
  {
    ss << ": No ShapeTypes have been set and a shape type for each phase.\n";
    setErrorCondition(-801);
    addErrorMessage(getNameOfClass(), ss.str(), -801);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeSyntheticVolume::preflight()
{
  UInt32ArrayType::Pointer shapeTypes = UInt32ArrayType::CreateArray(1, DREAM3D::EnsembleData::ShapeTypes);
  getDataContainer()->addEnsembleData(DREAM3D::EnsembleData::ShapeTypes, shapeTypes);

  dataCheck(true, 1, 1, 1);

  DataContainerReader::Pointer read_data = DataContainerReader::New();
  read_data->setInputFile(m_InputFile);
  read_data->setReadCellData(false);
  read_data->setReadFieldData(false);
  read_data->setReadEnsembleData(true);
  read_data->setDataContainer(getDataContainer());
  read_data->preflight();
  if (read_data->getErrorCondition() < 0)
  {
    setErrorCondition(read_data->getErrorCondition());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeSyntheticVolume::execute()
{
  setErrorCondition(0);
  DataContainer* m = getDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  DataContainerReader::Pointer read_data = DataContainerReader::New();
  read_data->setInputFile(m_InputFile);
  read_data->setReadCellData(false);
  read_data->setReadFieldData(false);
  read_data->setReadEnsembleData(true);
  read_data->setDataContainer(m);
  read_data->execute();

  m->setDimensions(m_XVoxels, m_YVoxels, m_ZVoxels);
  m->setResolution(m_XRes, m_YRes, m_ZRes);
  m->addEnsembleData(DREAM3D::EnsembleData::ShapeTypes, m_ShapeTypes);

  int64_t totalPoints = m->getTotalPoints();
  int totalFields = m->getNumFieldTuples();
  int totalEnsembles = m_ShapeTypes->GetNumberOfTuples();

  // Check to make sure we have all of our data arrays available or make them available.
  dataCheck(false, totalPoints, totalFields, totalEnsembles);
  if (getErrorCondition() < 0)
  {
    return;
  }

  // If there is an error set this to something negative and also set a message
 notifyStatusMessage("InitializeSyntheticVolume Complete");
}

