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

#include "FindNumFeatures.h"

#include <sstream>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNumFeatures::FindNumFeatures() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellEnsembleAttributeMatrixPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, ""),
  m_FeaturePhasesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_FeaturePhases(NULL),
  m_NumFeaturesArrayName(DREAM3D::EnsembleData::NumFeatures),
  m_NumFeatures(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNumFeatures::~FindNumFeatures()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNumFeatures::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
  parameters.push_back(FilterParameter::New("Feature Phases Array Path", "FeaturePhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget,"DataArrayPath", true));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
  parameters.push_back(FilterParameter::New("Create Cell Ensemble Data in AttributeMatrix", "CellEnsembleAttributeMatrixPath", FilterParameterWidgetType::AttributeMatrixSelectionWidget,"DataArrayPath", true));
  parameters.push_back(FilterParameter::New("Number of Features Array Name", "NumFeaturesArrayName", FilterParameterWidgetType::StringWidget,"QString", true));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNumFeatures::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setFeaturePhasesArrayPath( reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() ) );
  setCellEnsembleAttributeMatrixPath(reader->readDataArrayPath("CellEnsembleAttributeMatrixPath", getCellEnsembleAttributeMatrixPath() ) );
  setNumFeaturesArrayName( reader->readString("NumFeaturesArrayName", getNumFeaturesArrayName() ) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindNumFeatures::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() );
  writer->writeValue("CellEnsembleAttributeMatrixPath", getCellEnsembleAttributeMatrixPath() );
  writer->writeValue("NumFeaturesArrayName", getNumFeaturesArrayName() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNumFeatures::dataCheck()
{
  setErrorCondition(0);

  AttributeMatrix::Pointer cellEnsembleAttrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath<VolumeDataContainer, AbstractFilter>(this, getCellEnsembleAttributeMatrixPath(), -304);
  if(getErrorCondition() < 0) { return; }
 
  QVector<size_t> dims(1, 1);
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, m_FeaturePhasesArrayPath, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_NumFeaturesPtr = cellEnsembleAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, m_NumFeaturesArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NumFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NumFeatures = m_NumFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNumFeatures::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNumFeatures::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  size_t totalFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();
  size_t totalEnsembles = m_NumFeaturesPtr.lock()->getNumberOfTuples();

  for(size_t i = 1; i < totalEnsembles; i++)
  {
    m_NumFeatures[i] = 0;
  }
  for(size_t i = 1; i < totalFeatures; i++)
  {
    m_NumFeatures[m_FeaturePhases[i]]++;
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindNumFeatures::newFilterInstance(bool copyFilterParameters)
{
  /*
  */
  FindNumFeatures::Pointer filter = FindNumFeatures::New();
  if(true == copyFilterParameters)
  {
    filter->setFeaturePhasesArrayPath(getFeaturePhasesArrayPath());
    filter->setCellEnsembleAttributeMatrixPath(getCellEnsembleAttributeMatrixPath());
    filter->setNumFeaturesArrayName(getNumFeaturesArrayName());
  }
  return filter;
}
