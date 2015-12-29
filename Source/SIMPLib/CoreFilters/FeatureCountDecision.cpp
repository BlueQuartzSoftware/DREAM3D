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


#include "FeatureCountDecision.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "moc_FeatureCountDecision.cpp"
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FeatureCountDecision::FeatureCountDecision() :
  AbstractDecisionFilter(),
  m_FeatureIdsArrayPath(" "," "," "),
  m_MaxGrains(0),
  m_FeatureIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FeatureCountDecision::~FeatureCountDecision()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureCountDecision::setupFilterParameters()
{
  FilterParameterVector parameters = getFilterParameters();
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Int32, 1, DREAM3D::AttributeMatrixType::CellEnsemble, DREAM3D::Defaults::AnyGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Feature Ids", "FeatureIdsArrayPath", getFeatureIdsArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(IntFilterParameter::New("Max Grains", "MaxGrains", getMaxGrains(), FilterParameter::Parameter, 0));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureCountDecision::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  AbstractDecisionFilter::readFilterParameters(reader, index);
  reader->openFilterGroup(this, index);
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  setMaxGrains(reader->readValue("MaxGrains", getMaxGrains()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FeatureCountDecision::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  AbstractDecisionFilter::writeFilterParameters(writer, index);
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(MaxGrains)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureCountDecision::dataCheck()
{
  setErrorCondition(0);
  if(getErrorCondition() < 0) { return; }

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

  QVector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if (NULL != m_FeatureIdsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {  m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureCountDecision::preflight()
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
void FeatureCountDecision::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  // Assumes a SINGLE Phase. This WILL BREAK if there is more than 1 phase.
  qDebug() << "MaxGrains: " << m_MaxGrains;
  qDebug() << "FeatureIds: " << m_FeatureIds[1];
  bool dm = true;
  if (m_MaxGrains < m_FeatureIds[1])
  {
    dm = false;
  }
  emit decisionMade(dm);

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FeatureCountDecision::newFilterInstance(bool copyFilterParameters)
{
  FeatureCountDecision::Pointer filter = FeatureCountDecision::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FeatureCountDecision::getCompiledLibraryName()
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FeatureCountDecision::getGroupName()
{
  return DREAM3D::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FeatureCountDecision::getSubGroupName()
{
  return DREAM3D::FilterSubGroups::MiscFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FeatureCountDecision::getHumanLabel()
{ return "Feature Count Decision"; }
