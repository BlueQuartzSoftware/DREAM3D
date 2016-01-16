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


#include "MaskCountDecision.h"

#include <QtCore/QJsonDocument>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"

#include "moc_MaskCountDecision.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MaskCountDecision::MaskCountDecision() :
  AbstractDecisionFilter(),
  m_MaskArrayPath("", "", ""),
  m_NumberOfTrues(0),
  m_Mask(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MaskCountDecision::~MaskCountDecision()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MaskCountDecision::setupFilterParameters()
{
  FilterParameterVector parameters = getFilterParameters();
  DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Bool, 1, DREAM3D::Defaults::AnyAttributeMatrix, DREAM3D::Defaults::AnyGeometry);
  parameters.push_back(DataArraySelectionFilterParameter::New("Mask", "MaskArrayPath", getMaskArrayPath(), FilterParameter::RequiredArray, req));
  parameters.push_back(IntFilterParameter::New("Number of True Instances", "NumberOfTrues", getNumberOfTrues(), FilterParameter::Parameter, 0));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MaskCountDecision::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  AbstractDecisionFilter::readFilterParameters(reader, index);
  reader->openFilterGroup(this, index);
  setMaskArrayPath(reader->readDataArrayPath("MaskArrayPath", getMaskArrayPath()));
  setNumberOfTrues(reader->readValue("NumberOfTrues", getNumberOfTrues()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MaskCountDecision::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  AbstractDecisionFilter::writeFilterParameters(writer, index);
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(MaskArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(NumberOfTrues)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MaskCountDecision::dataCheck()
{
  setErrorCondition(0);

  QVector<size_t> cDims(1, 1);

  m_MaskPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getMaskArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if (NULL != m_MaskPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Mask = m_MaskPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MaskCountDecision::preflight()
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
void MaskCountDecision::execute()
{
  setErrorCondition(0);
  dataCheck();
  if (getErrorCondition() < 0) { return; }

  size_t numTuples = m_MaskPtr.lock()->getNumberOfTuples();

  size_t trueCount = 0;
  bool dm = false;

  for (size_t i = 0; i < numTuples; i++)
  {
    if (m_NumberOfTrues < 0 && !m_Mask[i])
    {
      emit decisionMade(dm);
      return;
    }
    if (m_Mask[i]) { trueCount++; }
    if (trueCount >= m_NumberOfTrues)
    {
      dm = true;
      emit decisionMade(dm);
      return;
    }
  }

  emit decisionMade(dm);

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MaskCountDecision::extractProperties(const QJsonDocument &jsonDoc)
{
  QJsonObject json = jsonDoc.object();
  QJsonValue jvalue = json.value(this->getNameOfClass());
  if (jvalue.isUndefined()) {
    return;
  }

  {
    QJsonValue propValue = jvalue.toObject()["MaxGrains"];
    if (!propValue.isUndefined())
    {
      setNumberOfTrues(propValue.toInt());
    }
  }

  {
    QJsonValue propValue = jvalue.toObject()["FeatureIdsArrayPath"];
    if (!propValue.isUndefined())
    {
      QJsonObject jObj = propValue.toObject();
      m_MaskArrayPath.readJson(jObj);
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer MaskCountDecision::newFilterInstance(bool copyFilterParameters)
{
  MaskCountDecision::Pointer filter = MaskCountDecision::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MaskCountDecision::getCompiledLibraryName()
{ return Core::CoreBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MaskCountDecision::getGroupName()
{ return DREAM3D::FilterGroups::CoreFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MaskCountDecision::getSubGroupName()
{ return DREAM3D::FilterSubGroups::MiscFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MaskCountDecision::getHumanLabel()
{ return "Mask Count Decision"; }
