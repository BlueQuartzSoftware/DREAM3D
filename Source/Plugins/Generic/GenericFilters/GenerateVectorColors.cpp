/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include "GenerateVectorColors.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "DREAM3DLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/StringFilterParameter.h"
#include "DREAM3DLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "DREAM3DLib/FilterParameters/SeparatorFilterParameter.h"

#include "DREAM3DLib/Utilities/ColorTable.h"
#include "OrientationLib/OrientationMath/OrientationMath.h"

#include "Generic/GenericConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateVectorColors::GenerateVectorColors() :
  AbstractFilter(),
  m_VectorsArrayPath("", "", ""),
  m_GoodVoxelsArrayPath("", "", ""),
  m_CellVectorColorsArrayName(DREAM3D::CellData::VectorColor),
  m_UseGoodVoxels(false),
  m_Vectors(NULL),
  m_GoodVoxels(NULL),
  m_CellVectorColors(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateVectorColors::~GenerateVectorColors()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateVectorColors::setupFilterParameters()
{
  FilterParameterVector parameters;
  QStringList linkedProps("GoodVoxelsArrayPath");
  parameters.push_back(LinkedBooleanFilterParameter::New("Apply to Good Voxels Only (Bad Voxels Will Be Black)", "UseGoodVoxels", getUseGoodVoxels(), linkedProps, FilterParameter::Parameter));
  parameters.push_back(SeparatorFilterParameter::New("Element Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::Float, 3, DREAM3D::AttributeMatrixObjectType::Element);
    parameters.push_back(DataArraySelectionFilterParameter::New("Vector Attribute Array", "VectorsArrayPath", getVectorsArrayPath(), FilterParameter::RequiredArray, req));
  }

  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::Bool, 1, DREAM3D::AttributeMatrixObjectType::Element);
    parameters.push_back(DataArraySelectionFilterParameter::New("Mask", "GoodVoxelsArrayPath", getGoodVoxelsArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Element Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Vector Colors", "CellVectorColorsArrayName", getCellVectorColorsArrayName(), FilterParameter::CreatedArray));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateVectorColors::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setUseGoodVoxels(reader->readValue("UseGoodVoxels", getUseGoodVoxels() ) );
  setCellVectorColorsArrayName(reader->readString("CellVectorColorsArrayName", getCellVectorColorsArrayName() ) );
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath() ) );
  setVectorsArrayPath(reader->readDataArrayPath("VectorsArrayPath", getVectorsArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GenerateVectorColors::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(UseGoodVoxels)
  DREAM3D_FILTER_WRITE_PARAMETER(CellVectorColorsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(GoodVoxelsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(VectorsArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateVectorColors::dataCheck()
{
  setErrorCondition(0);

  DataArrayPath tempPath;

  QVector<DataArrayPath> dataArrayPaths;

  QVector<size_t> cDims(1, 3);
  m_VectorsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getVectorsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_VectorsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Vectors = m_VectorsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getVectorsArrayPath()); };

  tempPath.update(getVectorsArrayPath().getDataContainerName(), getVectorsArrayPath().getAttributeMatrixName(), getCellVectorColorsArrayName() );
  m_CellVectorColorsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellVectorColorsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellVectorColors = m_CellVectorColorsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // The good voxels array is optional, If it is available we are going to use it, otherwise we are going to ignore it
  if (getUseGoodVoxels() == true)
  {
    // The good voxels array is optional, If it is available we are going to use it, otherwise we are going to create it
    cDims[0] = 1;
    m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getGoodVoxelsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_GoodVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getGoodVoxelsArrayPath()); };
  }
  else
  {
    m_GoodVoxels = NULL;
  }

  getDataContainerArray()->validateNumberOfTuples(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateVectorColors::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateVectorColors::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  size_t totalPoints = m_VectorsPtr.lock()->getNumberOfTuples();

  bool missingGoodVoxels = true;
  if (NULL != m_GoodVoxels)
  {
    missingGoodVoxels = false;
  }

  size_t index = 0;

  //DREAM3D::Rgb argb = 0x00000000;

  // Write the IPF Coloring Cell Data
  for (size_t i = 0; i < totalPoints; i++)
  {
    index = i * 3;
    m_CellVectorColors[index] = 0;
    m_CellVectorColors[index + 1] = 0;
    m_CellVectorColors[index + 2] = 0;

    float dir[3] = { 0.0f, 0.0f, 0.0f };
    float r = 0, g = 0, b = 0;
    DREAM3D::Rgb argb;
    if (missingGoodVoxels == true || m_GoodVoxels[i] == true)
    {
      dir[0] = m_Vectors[index + 0];
      dir[1] = m_Vectors[index + 1];
      dir[2] = m_Vectors[index + 2];
      MatrixMath::Normalize3x1(dir);
      if (dir[2] < 0) { MatrixMath::Multiply3x1withConstant(dir, -1); }
      float trend = atan2f(dir[1], dir[0]) * (180.0 / DREAM3D::Constants::k_Pi);
      float plunge = acosf(dir[2]) * (180.0 / DREAM3D::Constants::k_Pi);
      if (trend < 0.0) { trend += 360.0; }
      if (trend <= 120.0)
      {
        r = 255.0 * ((120.0 - trend) / 120.0);
        g = 255.0 * (trend / 120.0);
        b = 0.0;
      }
      if (trend > 120.0 && trend <= 240.0)
      {
        trend -= 120.0;
        r = 0.0;
        g = 255.0 * ((120.0 - trend) / 120.0);
        b = 255.0 * (trend / 120.0);
      }
      if (trend > 240.0 && trend < 360.0)
      {
        trend -= 240.0;
        r = 255.0 * (trend / 120.0);
        g = 0.0;
        b = 255.0 * ((120.0 - trend) / 120.0);
      }
      float deltaR = 255.0 - r;
      float deltaG = 255.0 - g;
      float deltaB = 255.0 - b;
      r += (deltaR * ((90.0 - plunge) / 90.0));
      g += (deltaG * ((90.0 - plunge) / 90.0));
      b += (deltaB * ((90.0 - plunge) / 90.0));
      if (r > 255.0) { r = 255.0; }
      if (g > 255.0) { g = 255.0; }
      if (b > 255.0) { b = 255.0; }
      argb = RgbColor::dRgb(r, g, b, 255);
      m_CellVectorColors[index] = RgbColor::dRed(argb);
      m_CellVectorColors[index + 1] = RgbColor::dGreen(argb);
      m_CellVectorColors[index + 2] = RgbColor::dBlue(argb);
    }
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GenerateVectorColors::newFilterInstance(bool copyFilterParameters)
{
  GenerateVectorColors::Pointer filter = GenerateVectorColors::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateVectorColors::getCompiledLibraryName()
{ return GenericConstants::GenericBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateVectorColors::getGroupName()
{ return DREAM3D::FilterGroups::GenericFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateVectorColors::getSubGroupName()
{ return DREAM3D::FilterSubGroups::CrystallographyFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateVectorColors::getHumanLabel()
{ return "Generate Vector Colors"; }
