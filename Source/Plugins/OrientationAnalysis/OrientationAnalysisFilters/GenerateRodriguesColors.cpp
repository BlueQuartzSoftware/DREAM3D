/* ============================================================================
 * Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
 * Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
 * contributors may be used to endorse or promote products derived from this software
 * without specific prior written permission.
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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "GenerateRodriguesColors.h"

#include <QtCore/QTextStream>

#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Math/MatrixMath.h"
#include "SIMPLib/Utilities/ColorTable.h"

#include "EbsdLib/Core/Orientation.hpp"
#include "EbsdLib/Core/OrientationTransformation.hpp"
#include "EbsdLib/LaueOps/CubicLowOps.h"
#include "EbsdLib/LaueOps/CubicOps.h"
#include "EbsdLib/LaueOps/HexagonalLowOps.h"
#include "EbsdLib/LaueOps/HexagonalOps.h"
#include "EbsdLib/LaueOps/MonoclinicOps.h"
#include "EbsdLib/LaueOps/OrthoRhombicOps.h"
#include "EbsdLib/LaueOps/TetragonalLowOps.h"
#include "EbsdLib/LaueOps/TetragonalOps.h"
#include "EbsdLib/LaueOps/TriclinicOps.h"
#include "EbsdLib/LaueOps/TrigonalLowOps.h"
#include "EbsdLib/LaueOps/TrigonalOps.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

#include "EbsdLib/Core/EbsdLibConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateRodriguesColors::GenerateRodriguesColors() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateRodriguesColors::~GenerateRodriguesColors() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateRodriguesColors::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  std::vector<QString> linkedProps = {"GoodVoxelsArrayPath"};
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Apply to Good Voxels Only (Bad Voxels Will Be Black)", UseGoodVoxels, FilterParameter::Category::Parameter, GenerateRodriguesColors, linkedProps));

  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::Defaults::AnyPrimitive, 1, AttributeMatrix::Category::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Cell Phases", CellPhasesArrayPath, FilterParameter::Category::RequiredArray, GenerateRodriguesColors, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::Defaults::AnyPrimitive, 1, AttributeMatrix::Category::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Crystal Structures", CrystalStructuresArrayPath, FilterParameter::Category::RequiredArray, GenerateRodriguesColors, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::Defaults::AnyPrimitive, 3, AttributeMatrix::Category::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Cell Euler Angles", CellEulerAnglesArrayPath, FilterParameter::Category::RequiredArray, GenerateRodriguesColors, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::Defaults::AnyPrimitive, 1, AttributeMatrix::Category::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Mask", GoodVoxelsArrayPath, FilterParameter::Category::RequiredArray, GenerateRodriguesColors, req));
  }

  parameters.push_back(SIMPL_NEW_STRING_FP("CellRodriguesColors", CellRodriguesColorsArrayName, FilterParameter::Category::CreatedArray, GenerateRodriguesColors));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateRodriguesColors::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setUseGoodVoxels(reader->readValue("UseGoodVoxels", getUseGoodVoxels()));
  setCellRodriguesColorsArrayName(reader->readString("CellRodriguesColorsArrayName", getCellRodriguesColorsArrayName()));
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath()));
  setCellEulerAnglesArrayPath(reader->readDataArrayPath("CellEulerAnglesArrayPath", getCellEulerAnglesArrayPath()));
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath()));
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateRodriguesColors::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateRodriguesColors::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  DataArrayPath tempPath;

  std::vector<size_t> dims(1, 1);
  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>>(this, getCellPhasesArrayPath(), dims);
  if(nullptr != m_CellPhasesPtr.lock())
  {
    m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  // typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>>(this, getCrystalStructuresArrayPath(), dims);
  if(nullptr != m_CrystalStructuresPtr.lock())
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 3;
  m_CellEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>>(this, getCellEulerAnglesArrayPath(), dims);
  if(nullptr != m_CellEulerAnglesPtr.lock())
  {
    m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(m_CellEulerAnglesArrayPath.getDataContainerName(), m_CellEulerAnglesArrayPath.getAttributeMatrixName(), getCellRodriguesColorsArrayName());
  m_CellRodriguesColorsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>>(this, tempPath, 0, dims);
  if(nullptr != m_CellRodriguesColorsPtr.lock())
  {
    m_CellRodriguesColors = m_CellRodriguesColorsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  // The good voxels array is optional, If it is available we are going to use it, otherwise we are going to create it
  dims[0] = 1;
  if(getUseGoodVoxels())
  {
    // The good voxels array is optional, If it is available we are going to use it, otherwise we are going to create it
    dims[0] = 1;
    m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>>(this, getGoodVoxelsArrayPath(), dims);
    if(nullptr != m_GoodVoxelsPtr.lock())
    {
      m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  else
  {
    m_GoodVoxels = nullptr;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateRodriguesColors::execute()
{
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  int64_t totalPoints = m_CellEulerAnglesPtr.lock()->getNumberOfTuples();

  bool missingGoodVoxels = true;
  if(nullptr != m_GoodVoxels)
  {
    missingGoodVoxels = false;
  }

  // Create 1 of every type of Ops class. This condenses the code below
  QVector<LaueOps::Pointer> ops;
  ops.push_back(HexagonalOps::New());
  ops.push_back(CubicOps::New());
  ops.push_back(HexagonalLowOps::New());
  ops.push_back(CubicLowOps::New());
  ops.push_back(TriclinicOps::New());
  ops.push_back(MonoclinicOps::New());
  ops.push_back(OrthoRhombicOps::New());
  ops.push_back(TetragonalLowOps::New());
  ops.push_back(TetragonalOps::New());
  ops.push_back(TrigonalLowOps::New());
  ops.push_back(TrigonalOps::New());

  int phase;
  size_t index = 0;
  SIMPL::Rgb argb = 0x00000000;

  // Write the IPF Coloring Cell Data
  for(int64_t i = 0; i < totalPoints; i++)
  {
    phase = m_CellPhases[i];
    index = i * 3;
    m_CellRodriguesColors[index] = 0;
    m_CellRodriguesColors[index + 1] = 0;
    m_CellRodriguesColors[index + 2] = 0;

    // Make sure we are using a valid Euler Angles with valid crystal symmetry
    if((missingGoodVoxels || m_GoodVoxels[i]) && m_CrystalStructures[phase] < EbsdLib::CrystalStructure::LaueGroupEnd)
    {
      OrientationF rod = OrientationTransformation::eu2ro<OrientationF, OrientationF>(OrientationF(m_CellEulerAngles + index, 3));

      argb = ops[m_CrystalStructures[phase]]->generateRodriguesColor(rod[0], rod[1], rod[2]);
      m_CellRodriguesColors[index] = RgbColor::dRed(argb);
      m_CellRodriguesColors[index + 1] = RgbColor::dGreen(argb);
      m_CellRodriguesColors[index + 2] = RgbColor::dBlue(argb);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GenerateRodriguesColors::newFilterInstance(bool copyFilterParameters) const
{
  GenerateRodriguesColors::Pointer filter = GenerateRodriguesColors::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateRodriguesColors::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateRodriguesColors::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateRodriguesColors::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateRodriguesColors::getGroupName() const
{
  return SIMPL::FilterGroups::ProcessingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid GenerateRodriguesColors::getUuid() const
{
  return QUuid("{626f6feb-68bc-537a-8f5b-c6062e72236f}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateRodriguesColors::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::CrystallographyFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateRodriguesColors::getHumanLabel() const
{
  return "Generate Rodrigues Colors";
}

// -----------------------------------------------------------------------------
GenerateRodriguesColors::Pointer GenerateRodriguesColors::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<GenerateRodriguesColors> GenerateRodriguesColors::New()
{
  struct make_shared_enabler : public GenerateRodriguesColors
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString GenerateRodriguesColors::getNameOfClass() const
{
  return QString("GenerateRodriguesColors");
}

// -----------------------------------------------------------------------------
QString GenerateRodriguesColors::ClassName()
{
  return QString("GenerateRodriguesColors");
}

// -----------------------------------------------------------------------------
void GenerateRodriguesColors::setCellPhasesArrayPath(const DataArrayPath& value)
{
  m_CellPhasesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenerateRodriguesColors::getCellPhasesArrayPath() const
{
  return m_CellPhasesArrayPath;
}

// -----------------------------------------------------------------------------
void GenerateRodriguesColors::setCrystalStructuresArrayPath(const DataArrayPath& value)
{
  m_CrystalStructuresArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenerateRodriguesColors::getCrystalStructuresArrayPath() const
{
  return m_CrystalStructuresArrayPath;
}

// -----------------------------------------------------------------------------
void GenerateRodriguesColors::setCellEulerAnglesArrayPath(const DataArrayPath& value)
{
  m_CellEulerAnglesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenerateRodriguesColors::getCellEulerAnglesArrayPath() const
{
  return m_CellEulerAnglesArrayPath;
}

// -----------------------------------------------------------------------------
void GenerateRodriguesColors::setGoodVoxelsArrayPath(const DataArrayPath& value)
{
  m_GoodVoxelsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenerateRodriguesColors::getGoodVoxelsArrayPath() const
{
  return m_GoodVoxelsArrayPath;
}

// -----------------------------------------------------------------------------
void GenerateRodriguesColors::setCellRodriguesColorsArrayName(const QString& value)
{
  m_CellRodriguesColorsArrayName = value;
}

// -----------------------------------------------------------------------------
QString GenerateRodriguesColors::getCellRodriguesColorsArrayName() const
{
  return m_CellRodriguesColorsArrayName;
}

// -----------------------------------------------------------------------------
void GenerateRodriguesColors::setUseGoodVoxels(bool value)
{
  m_UseGoodVoxels = value;
}

// -----------------------------------------------------------------------------
bool GenerateRodriguesColors::getUseGoodVoxels() const
{
  return m_UseGoodVoxels;
}
