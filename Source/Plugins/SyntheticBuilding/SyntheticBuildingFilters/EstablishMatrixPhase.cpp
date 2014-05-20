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

#include "EstablishMatrixPhase.h"

#include <map>


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

#include "DREAM3DLib/StatsData/MatrixStatsData.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EstablishMatrixPhase::EstablishMatrixPhase() :
  AbstractFilter(),
  m_OutputCellAttributeMatrixName(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, ""),
  m_OutputCellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_InputStatsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::Statistics),
  m_InputPhaseTypesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::PhaseTypes),
  m_FeatureIds(NULL),
  m_CellPhases(NULL),
  m_FeaturePhases(NULL),
  m_PhaseTypesArrayName(DREAM3D::EnsembleData::PhaseTypes),
  m_PhaseTypes(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EstablishMatrixPhase::~EstablishMatrixPhase()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishMatrixPhase::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Statistics Array", "InputStatsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getInputStatsArrayPath(), true));
  parameters.push_back(FilterParameter::New("Phase Types Array", "InputPhaseTypesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getInputPhaseTypesArrayPath(), true));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Cell Attribute Matrix Name", "OutputCellAttributeMatrixName", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getOutputCellAttributeMatrixName(), true));
  parameters.push_back(FilterParameter::New("Cell Feature Attribute Matrix Name", "OutputCellFeatureAttributeMatrixName", FilterParameterWidgetType::StringWidget, getOutputCellFeatureAttributeMatrixName(), true));
  parameters.push_back(FilterParameter::New("Feature Ids Array Name", "FeatureIdsArrayName", FilterParameterWidgetType::StringWidget, getFeatureIdsArrayName(), true));
  parameters.push_back(FilterParameter::New("Cell Phases Array Name", "CellPhasesArrayName", FilterParameterWidgetType::StringWidget, getCellPhasesArrayName(), true));
  parameters.push_back(FilterParameter::New("Feature Phases Array Name", "FeaturePhasesArrayName", FilterParameterWidgetType::StringWidget, getFeaturePhasesArrayName(), true));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishMatrixPhase::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setOutputCellAttributeMatrixName( reader->readDataArrayPath("OutputCellAttributeMatrixName", getOutputCellAttributeMatrixName() ) );
  setOutputCellFeatureAttributeMatrixName( reader->readString("OutputCellFeatureAttributeMatrixName", getOutputCellFeatureAttributeMatrixName() ) );
  setFeatureIdsArrayName( reader->readString("FeatureIdsArrayName", getFeatureIdsArrayName() ) );
  setCellPhasesArrayName( reader->readString("CellPhasesArrayName", getCellPhasesArrayName() ) );
  setFeaturePhasesArrayName( reader->readString("FeaturePhasesArrayName", getFeaturePhasesArrayName() ) );
  setInputStatsArrayPath(reader->readDataArrayPath("InputStatsArrayPath", getInputStatsArrayPath() ) );
  setInputPhaseTypesArrayPath(reader->readDataArrayPath("InputPhaseTypesArrayPath", getInputPhaseTypesArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EstablishMatrixPhase::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("OutputCellAttributeMatrixName", getOutputCellAttributeMatrixName() );
  writer->writeValue("OutputCellFeatureAttributeMatrixName", getOutputCellFeatureAttributeMatrixName() );
  writer->writeValue("FeatureIdsArrayName", getFeatureIdsArrayName() );
  writer->writeValue("CellPhasesArrayName", getCellPhasesArrayName() );
  writer->writeValue("FeaturePhasesArrayName", getFeaturePhasesArrayName() );
  writer->writeValue("InputStatsArrayPath", getInputStatsArrayPath() );
  writer->writeValue("InputPhaseTypesArrayPath", getInputPhaseTypesArrayPath() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishMatrixPhase::updateFeatureInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishMatrixPhase::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getOutputCellAttributeMatrixName().getDataContainerName());
  if(getErrorCondition() < 0) { return; }
  //Input Ensemble Data That we require
  typedef DataArray<unsigned int> PhaseTypeArrayType;

  QVector<size_t> dims(1, 1);
  m_PhaseTypesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getInputPhaseTypesArrayPath(), dims);
  if( NULL != m_PhaseTypesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_PhaseTypes = m_PhaseTypesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_StatsDataArray = getDataContainerArray()->getPrereqArrayFromPath<StatsDataArray, AbstractFilter>(this, getInputStatsArrayPath(), dims);
  if(m_StatsDataArray.lock() == NULL)
  {
    QString ss = QObject::tr("Stats Array Not Initialized correctly");
    setErrorCondition(-308);
    notifyErrorMessage(getHumanLabel(), ss, -308);
  }

  dims[0] = 1;
  //Cell Data
  tempPath.update(getOutputCellAttributeMatrixName().getDataContainerName(), getOutputCellAttributeMatrixName().getAttributeMatrixName(), getFeatureIdsArrayName() );
  m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, -1, dims); /* Assigns the shared_ptr<>(this, tempPath, -1, dims); Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getOutputCellAttributeMatrixName().getDataContainerName(), getOutputCellAttributeMatrixName().getAttributeMatrixName(), getCellPhasesArrayName() );
  m_CellPhasesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, dims); /* Assigns the shared_ptr<>(this, tempPath, 0, dims); Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  QVector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer cellFeatureAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getOutputCellFeatureAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::CellFeature);
  if(getErrorCondition() < 0) { return; }

  //Feature Data
  tempPath.update(getOutputCellAttributeMatrixName().getDataContainerName(), getOutputCellFeatureAttributeMatrixName(), getFeaturePhasesArrayName() );
  m_FeaturePhasesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishMatrixPhase::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishMatrixPhase::execute()
{
  int err = 0;
  setErrorCondition(err);
  DREAM3D_RANDOMNG_NEW()

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  establish_matrix();

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "EstablishMatrixPhases Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void  EstablishMatrixPhase::establish_matrix()
{
  notifyStatusMessage(getHumanLabel(), "Establishing Matrix");
  DREAM3D_RANDOMNG_NEW()

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getOutputCellAttributeMatrixName().getDataContainerName());

  StatsDataArray& statsDataArray = *(m_StatsDataArray.lock());

  size_t udims[3] =
  { 0, 0, 0 };
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  sizex = dims[0] * m->getXRes();
  sizey = dims[1] * m->getYRes();
  sizez = dims[2] * m->getZRes();
  totalvol = sizex * sizey * sizez;

  int64_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  size_t currentnumfeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();
  size_t numensembles = m_PhaseTypesPtr.lock()->getNumberOfTuples();
  QVector<size_t> tDims(1, 1);
  if(currentnumfeatures == 0)
  {
    m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->resizeAttributeArrays(tDims);
    updateFeatureInstancePointers();
    currentnumfeatures = 1;
  }
  firstMatrixFeature = currentnumfeatures;
  float random;
  float totalmatrixfractions = 0.0f;

  for (size_t i = 1; i < numensembles; ++i)
  {
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::MatrixPhase)
    {
      MatrixStatsData* mp = MatrixStatsData::SafePointerDownCast(statsDataArray[i].get());
      matrixphases.push_back(i);
      matrixphasefractions.push_back(mp->getPhaseFraction());
      totalmatrixfractions = totalmatrixfractions + mp->getPhaseFraction();
    }
  }
  for (size_t i = 0; i < matrixphases.size(); i++)
  {
    matrixphasefractions[i] = matrixphasefractions[i] / totalmatrixfractions;
    if(i > 0) { matrixphasefractions[i] = matrixphasefractions[i] + matrixphasefractions[i - 1]; }
  }
  size_t j = 0;
  for (size_t i = 0; i < static_cast<size_t>(totalPoints); ++i)
  {
    if(m_FeatureIds[i] <= 0)
    {
      random = static_cast<float>( rg.genrand_res53() );
      j = 0;
      while(random > matrixphasefractions[j])
      {
        j++;
      }
      if(m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getNumTuples() <= (firstMatrixFeature + j))
      {
        tDims[0] = (firstMatrixFeature + j) + 1;
        m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->resizeAttributeArrays(tDims);
        updateFeatureInstancePointers();
      }
      m_FeatureIds[i] = (firstMatrixFeature + j);
      m_CellPhases[i] = matrixphases[j];
      m_FeaturePhases[(firstMatrixFeature + j)] = matrixphases[j];
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer EstablishMatrixPhase::newFilterInstance(bool copyFilterParameters)
{
  EstablishMatrixPhase::Pointer filter = EstablishMatrixPhase::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}
