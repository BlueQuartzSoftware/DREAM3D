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


#include "AddOrientationNoise.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"
#include "OrientationLib/OrientationMath/OrientationMath.h"

#include "SyntheticBuilding/SyntheticBuildingConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AddOrientationNoise::AddOrientationNoise() :
  AbstractFilter(),
  m_Magnitude(1.0f),
  m_CellEulerAnglesArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::ElementAttributeMatrixName, DREAM3D::CellData::EulerAngles),
  m_CellEulerAngles(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AddOrientationNoise::~AddOrientationNoise()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddOrientationNoise::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Magnitude of Orientation Noise", "Magnitude", FilterParameterWidgetType::DoubleWidget, getMagnitude(), FilterParameter::Uncategorized, "Degrees"));
  parameters.push_back(SeparatorFilterParameter::New("Required Information", FilterParameter::Uncategorized));
  parameters.push_back(FilterParameter::New("Element Euler Angles", "CellEulerAnglesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCellEulerAnglesArrayPath(), FilterParameter::Uncategorized, ""));
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddOrientationNoise::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellEulerAnglesArrayPath(reader->readDataArrayPath("CellEulerAnglesArrayPath", getCellEulerAnglesArrayPath() ) );
  setMagnitude( reader->readValue("Magnitude", getMagnitude()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AddOrientationNoise::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(CellEulerAnglesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(Magnitude)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddOrientationNoise::dataCheck()
{
  setErrorCondition(0);

  QVector<size_t> cDims(1, 3);
  m_CellEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCellEulerAnglesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() < 0) { return; }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddOrientationNoise::preflight()
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
void AddOrientationNoise::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  m_Magnitude = m_Magnitude * DREAM3D::Constants::k_Pi / 180.0f;

  add_orientation_noise();

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "AddOrientationNoises Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void  AddOrientationNoise::add_orientation_noise()
{
  notifyStatusMessage(getHumanLabel(), "Adding Orientation Noise");
  DREAM3D_RANDOMNG_NEW()

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getCellEulerAnglesArrayPath().getDataContainerName());

  float g[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
  float newg[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
  float rot[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
  float w = 0.0f, n1 = 0.0f, n2 = 0.0f, n3 = 0.0f;
  size_t totalPoints = m->getGeometryAs<ImageGeom>()->getNumberOfElements();
  for (size_t i = 0; i < totalPoints; ++i)
  {
    float ea1 = m_CellEulerAngles[3 * i + 0];
    float ea2 = m_CellEulerAngles[3 * i + 1];
    float ea3 = m_CellEulerAngles[3 * i + 2];
    FOrientArrayType om(9, 0.0);
    FOrientTransformsType::eu2om(FOrientArrayType(&(m_CellEulerAngles[3 * i]), 3), om);
    om.toGMatrix(g);
    n1 = static_cast<float>( rg.genrand_res53() );
    n2 = static_cast<float>( rg.genrand_res53() );
    n3 = static_cast<float>( rg.genrand_res53() );
    w = static_cast<float>( rg.genrand_res53() );
    w = 2.0 * (w - 0.5);
    w = (m_Magnitude * w);
    OrientationMath::AxisAngletoMat(w, n1, n2, n3, rot);
    MatrixMath::Multiply3x3with3x3(g, rot, newg);
    OrientationMath::MattoEuler(newg, ea1, ea2, ea3);
    m_CellEulerAngles[3 * i + 0] = ea1;
    m_CellEulerAngles[3 * i + 1] = ea2;
    m_CellEulerAngles[3 * i + 2] = ea3;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer AddOrientationNoise::newFilterInstance(bool copyFilterParameters)
{
  AddOrientationNoise::Pointer filter = AddOrientationNoise::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AddOrientationNoise::getCompiledLibraryName()
{ return SyntheticBuildingConstants::SyntheticBuildingBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AddOrientationNoise::getGroupName()
{ return DREAM3D::FilterGroups::SyntheticBuildingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AddOrientationNoise::getSubGroupName()
{ return DREAM3D::FilterSubGroups::CrystallographyFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AddOrientationNoise::getHumanLabel()
{ return "Add Orientation Noise"; }
