/* ============================================================================
* Copyright (c) 2018 BlueQuartz Software, LLC
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
*    United States Air Force Prime Contract FA8650-15-D-5231
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "FindConnectedFeatures.h"

#include <QtCore/QSet>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "Generic/GenericConstants.h"
#include "Generic/GenericVersion.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindConnectedFeatures::FindConnectedFeatures() :
  AbstractFilter(),
  m_NeighborListArrayPath(DataArrayPath("", "", "")),
  m_ConnectedFeaturesArrayName("ConnectedFeatures"),
  m_SelectedFeatureId(0),
  m_SelectedPhaseId(0),
  m_AlgorithmType(0)
{
  initialize();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindConnectedFeatures::~FindConnectedFeatures()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindConnectedFeatures::initialize()
{
  setErrorCondition(0);
  setWarningCondition(0);
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindConnectedFeatures::setupFilterParameters()
{
  FilterParameterVector parameters;

  QStringList linkedProps;
  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Algorithm Type");
    parameter->setPropertyName("AlgorithmType");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(FindConnectedFeatures, this, AlgorithmType));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(FindConnectedFeatures, this, AlgorithmType));
    QVector<QString> choices;
    choices.push_back("Features");
    choices.push_back("Phases");
    parameter->setChoices(choices);
    QStringList linkedProps;
    linkedProps << "SelectedFeatureId"
                << "SelectedPhaseId"
                << "PhasesArrayPath";
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }

  parameters.push_back(SIMPL_NEW_INTEGER_FP("Selected Feature Id", SelectedFeatureId, FilterParameter::Parameter, FindConnectedFeatures, 0));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Selected Phase Id", SelectedPhaseId, FilterParameter::Parameter, FindConnectedFeatures, 1));

  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Category::Feature);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", PhasesArrayPath, FilterParameter::RequiredArray, FindConnectedFeatures, req, 1));
  }

  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::NeighborList, 1, AttributeMatrix::Category::Feature);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Neighbor List", NeighborListArrayPath, FilterParameter::RequiredArray, FindConnectedFeatures, req));
  }

  parameters.push_back(SIMPL_NEW_STRING_FP("Connected Features Array Name", ConnectedFeaturesArrayName, FilterParameter::CreatedArray, FindConnectedFeatures));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindConnectedFeatures::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  int err = 0;
  AttributeMatrix::Pointer nListAM = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getNeighborListArrayPath(), err);
  if (err < 0 || nListAM == AttributeMatrix::NullPointer())
  {
    QString ss = QObject::tr("Could not get the attribute matrix of the selected neighbor list.");
    setErrorCondition(-30000);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QVector<size_t> dims(1, 1);
  m_NeighborList = nListAM->getPrereqArray<NeighborList<int>, AbstractFilter>(this, getNeighborListArrayPath().getDataArrayName(), err, dims);
  std::shared_ptr<NeighborList<int> > nList = m_NeighborList.lock();
  if (nList == nullptr)
  {
    QString ss = QObject::tr("The selected neighbor list no longer exists, because a previous filter may have deleted it.  Please run the 'Find Feature Neighbors' filter before this filter"
                             "to remake the neighbor list.");
    setErrorCondition(-30001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if (m_AlgorithmType == static_cast<int>(AlgorithmTypeString::Feature))
  {
    if (getInPreflight() == false)
    {
      if (m_SelectedFeatureId < 0 || m_SelectedFeatureId >= nList->getSize())
      {
        QString ss = QObject::tr("The selected feature id was not found in the neighbor list.");
        setErrorCondition(-30002);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }
    }
  }
  else if (m_AlgorithmType == static_cast<int>(AlgorithmTypeString::Phase))
  {
    m_PhasesArrayPtr = getDataContainerArray()->getPrereqArrayFromPath<Int32ArrayType, AbstractFilter>(this, getPhasesArrayPath(), dims);
    if(nullptr != m_PhasesArrayPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_PhasesArray = m_PhasesArrayPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */

    if (getInPreflight() == false)
    {
      if (m_SelectedPhaseId < 0)
      {
        QString ss = QObject::tr("The selected phase must be a positive integer.");
        setErrorCondition(-30003);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }
    }
  }
  else
  {
    QString ss = QObject::tr("Algorithm type is set to an index that is out of bounds.  Please select a valid algorithm type.");
    setErrorCondition(-30004);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  m_ConnectedFeaturesArrayPtr = nListAM->createNonPrereqArray<DataArray<bool>, AbstractFilter, bool>(
      this, getConnectedFeaturesArrayName(), false, dims);   /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_ConnectedFeaturesArrayPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_ConnectedFeaturesArray = m_ConnectedFeaturesArrayPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindConnectedFeatures::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindConnectedFeatures::execute()
{
  initialize();
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  if (m_AlgorithmType == static_cast<int>(AlgorithmTypeString::Feature))
  {
    std::shared_ptr<NeighborList<int> > nList = m_NeighborList.lock();
    BoolArrayType::Pointer connectedFeaturesArray = m_ConnectedFeaturesArrayPtr.lock();

    // Get the list of neighbors for the given feature id
    NeighborList<int>::SharedVectorType featureNeighborList = nList->getList(m_SelectedFeatureId);
    if (featureNeighborList.get() == nullptr)
    {
      QString ss = QObject::tr("There was a problem getting the list of neighbors for feature id %1.").arg(m_SelectedFeatureId);
      setErrorCondition(-30005);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    // Set all the neighbor feature ids as connected features in the connected features array
    for (std::vector<int>::size_type i = 0; i < featureNeighborList->size(); i++)
    {
      int neighborId = featureNeighborList->at(i);
      connectedFeaturesArray->setValue(neighborId, true);
    }
  }
  else if (m_AlgorithmType == static_cast<int>(AlgorithmTypeString::Phase))
  {
    Int32ArrayType::Pointer phasesArray = m_PhasesArrayPtr.lock();
    QSet<int> featureIdSet;

    // Get the list of feature ids for the given phase
    for (size_t i = 0; i < phasesArray->getNumberOfTuples(); i++)
    {
      int32_t phaseId = phasesArray->getValue(i);
      if (phaseId == m_SelectedPhaseId)
      {
        featureIdSet.insert(i);
      }
    }

    // For each feature id, get the list of neighbor feature ids.  Set all the neighbor feature ids, that are not part of the selected phase, as connected features in the connected features array
    QList<int> featureList = featureIdSet.toList();
    for (int i = 0; i < featureList.size(); i++)
    {
      std::shared_ptr<NeighborList<int> > nList = m_NeighborList.lock();
      BoolArrayType::Pointer connectedFeaturesArray = m_ConnectedFeaturesArrayPtr.lock();

      NeighborList<int>::SharedVectorType featureNeighborList = nList->getList(featureList[i]);
      if (featureNeighborList.get() == nullptr)
      {
        QString ss = QObject::tr("There was a problem getting the list of neighbors for feature id %1.").arg(featureList[i]);
        setErrorCondition(-30005);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }

      for (std::vector<int>::size_type i = 0; i < featureNeighborList->size(); i++)
      {
        int neighborId = featureNeighborList->at(i);

        if (phasesArray->getValue(neighborId) != m_SelectedPhaseId)
        {
          connectedFeaturesArray->setValue(neighborId, true);
        }
      }
    }
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindConnectedFeatures::newFilterInstance(bool copyFilterParameters)
{
  FindConnectedFeatures::Pointer filter = FindConnectedFeatures::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindConnectedFeatures::getCompiledLibraryName()
{ return GenericConstants::GenericBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindConnectedFeatures::getBrandingString()
{
  return "Generic";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindConnectedFeatures::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  Generic::Version::Major() << "." << Generic::Version::Minor() << "." << Generic::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindConnectedFeatures::getGroupName()
{ return SIMPL::FilterGroups::Generic; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid FindConnectedFeatures::getUuid()
{
  return QUuid("{7bb2ec70-2042-5260-bba5-73457b16df3f}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindConnectedFeatures::getSubGroupName()
{ return "Generic"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindConnectedFeatures::getHumanLabel()
{ return "Find Connected Features"; }

