/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2014 Dr. Joseph C. Tucker (UES, Inc.)
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
 * Neither the name of Joseph C. Tucker, Michael A. Groeber, Michael A. Jackson,
 * UES, Inc., the US Air Force, BlueQuartz Software nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *                   FA8650-07-D-5800 and FA8650-10-D-5226
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "FindLocalAverageCAxisMisalignments.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1

#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindLocalAverageCAxisMisalignments::FindLocalAverageCAxisMisalignments() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_NewCellFeatureAttributeMatrixName(DREAM3D::Defaults::NewCellFeatureAttributeMatrixName),
  m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellEnsembleAttributeMatrixName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_NeighborListArrayName(DREAM3D::FeatureData::NeighborList),
  m_CAxisMisalignmentListArrayName(DREAM3D::FeatureData::CAxisMisalignmentList),
  m_CalcUnbiasedAvg(false),
  m_CalcBiasedAvg(false),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_CellParentIdsArrayName(DREAM3D::CellData::ParentIds),
  m_CellParentIds(NULL),
  m_FeatureParentIdsArrayName(DREAM3D::FeatureData::ParentIds),
  m_FeatureParentIds(NULL),
  m_NumFeaturesPerParentArrayName(DREAM3D::FeatureData::NumFeaturesPerParent),
  m_NumFeaturesPerParent(NULL),
  m_AvgCAxisMisalignmentsArrayName(DREAM3D::FeatureData::AvgCAxisMisalignments),
  m_AvgCAxisMisalignments(NULL),
  m_LocalCAxisMisalignmentsArrayName(DREAM3D::FeatureData::LocalCAxisMisalignments),
  m_LocalCAxisMisalignments(NULL),
  m_UnbiasedLocalCAxisMisalignmentsArrayName(DREAM3D::FeatureData::UnbiasedLocalCAxisMisalignments),
  m_UnbiasedLocalCAxisMisalignments(NULL),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CrystalStructures(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsVector();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindLocalAverageCAxisMisalignments::~FindLocalAverageCAxisMisalignments()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLocalAverageCAxisMisalignments::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Calculate Unbiased Local C-Axis Misalignments");
    option->setPropertyName("CalcUnbiasedAvg");
    option->setWidgetType(FilterParameterWidgetType::BooleanWidget);
    option->setValueType("bool");
    option->setUnits("");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Calculate Local C-Axis Misalignments");
    option->setPropertyName("CalcBiasedAvg");
    option->setWidgetType(FilterParameterWidgetType::BooleanWidget);
    option->setValueType("bool");
    option->setUnits("");
    parameters.push_back(option);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLocalAverageCAxisMisalignments::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setCalcUnbiasedAvg( reader->readValue("CalcUnbiasedAvg", getCalcUnbiasedAvg()) );
  setCalcBiasedAvg( reader->readValue("CalcBiasedAvg", getCalcBiasedAvg()) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindLocalAverageCAxisMisalignments::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  writer->writeValue("CalcUnbiasedAvg", getCalcUnbiasedAvg() );
  writer->writeValue("CalcBiasedAvg", getCalcBiasedAvg() );
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLocalAverageCAxisMisalignments::dataCheck()
{
  setErrorCondition(0);
  QString ss;
  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -301);
  if(getErrorCondition() < 0 || NULL == cellAttrMat.get() ) { return; }
  AttributeMatrix::Pointer cellFeatureAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), -302);
  if(getErrorCondition() < 0 || NULL == cellFeatureAttrMat.get()) { return; }
  AttributeMatrix::Pointer newCellFeatureAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getNewCellFeatureAttributeMatrixName(), -302);
  if(getErrorCondition() < 0 || NULL == newCellFeatureAttrMat.get()) { return; }

  AttributeMatrix::Pointer cellEnsembleAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), -303);
  if(getErrorCondition() < 0 || NULL == cellEnsembleAttrMat.get()) { return; }


  // Cell Data
  QVector<size_t> dims(1, 1);
  m_FeatureIdsPtr = cellAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_FeatureIdsArrayName, -301, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_CellParentIdsPtr = cellAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_CellParentIdsArrayName, -304, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellParentIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellParentIds = m_CellParentIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Feature Data
  if ( m_CalcUnbiasedAvg == true)
  {
	m_UnbiasedLocalCAxisMisalignmentsPtr = newCellFeatureAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this, m_UnbiasedLocalCAxisMisalignmentsArrayName, 0.0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
	if( NULL != m_UnbiasedLocalCAxisMisalignmentsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
	{ m_UnbiasedLocalCAxisMisalignments = m_UnbiasedLocalCAxisMisalignmentsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
    IDataArray::Pointer neighborListPtr = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getAttributeArray(m_NeighborListArrayName);
    if (NULL == neighborListPtr.get())
    {
      ss = QObject::tr("NeighborLists are not available and are required for this filter to run. A filter that generates NeighborLists needs to be placed before this filter in the pipeline.");
      setErrorCondition(-305);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
    else
    {
      m_NeighborList = boost::dynamic_pointer_cast< Int32NeighborListType >(neighborListPtr);
    }

    IDataArray::Pointer misalignmentPtr = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getAttributeArray(m_CAxisMisalignmentListArrayName);
    if(NULL == misalignmentPtr.get())
    {
      m_CAxisMisalignmentList = FloatNeighborListType::New();
      m_CAxisMisalignmentList->setName(m_CAxisMisalignmentListArrayName);
      m_CAxisMisalignmentList->resize(cellFeatureAttrMat->getNumTuples());
      m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->addAttributeArray(m_CAxisMisalignmentListArrayName, m_CAxisMisalignmentList);
    }
    else
    {
      m_CAxisMisalignmentList = boost::dynamic_pointer_cast< FloatNeighborListType >(misalignmentPtr);
      m_CAxisMisalignmentList->resize(cellFeatureAttrMat->getNumTuples());
    }
  }

  if (m_CalcBiasedAvg == true)
  {
	m_AvgCAxisMisalignmentsPtr = cellFeatureAttrMat->getPrereqArray<DataArray<float>, AbstractFilter>(this, m_AvgCAxisMisalignmentsArrayName, -302, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
	if( NULL != m_AvgCAxisMisalignmentsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
	{ m_AvgCAxisMisalignments = m_AvgCAxisMisalignmentsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
	m_LocalCAxisMisalignmentsPtr = newCellFeatureAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this, m_LocalCAxisMisalignmentsArrayName, 0.0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
	if( NULL != m_LocalCAxisMisalignmentsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
	{ m_LocalCAxisMisalignments = m_LocalCAxisMisalignmentsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  m_FeatureParentIdsPtr = cellFeatureAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_FeatureParentIdsArrayName, -302, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureParentIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureParentIds = m_FeatureParentIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // New Feature Data
  m_NumFeaturesPerParentPtr = newCellFeatureAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, float>(this, m_NumFeaturesPerParentArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NumFeaturesPerParentPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NumFeaturesPerParent = m_NumFeaturesPerParentPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Ensemble Data
  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = cellEnsembleAttrMat->getPrereqArray<DataArray<unsigned int>, AbstractFilter>(this,  m_CrystalStructuresArrayName, -305, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLocalAverageCAxisMisalignments::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLocalAverageCAxisMisalignments::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  size_t numFeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  size_t newNumFeatures = m->getAttributeMatrix(getNewCellFeatureAttributeMatrixName())->getNumTuples();

  QVector<int32_t> NumUnbiasedFeaturesPerParent(numFeatures,0);

  if(m_CalcUnbiasedAvg == true)
  {
    NeighborList<int>& neighborlist = *m_NeighborList;
    NeighborList<float>& caxismisalignmentList = *m_CAxisMisalignmentList;
    for(int i=1;i<numFeatures;i++)
    {
      int parentid = m_FeatureParentIds[i];
      for (size_t j = 0; j < neighborlist[i].size(); j++)
      {
        if (m_FeatureParentIds[neighborlist[i][j]] == m_FeatureParentIds[i])
        {
          m_UnbiasedLocalCAxisMisalignments[parentid] += caxismisalignmentList[i][j];
		  NumUnbiasedFeaturesPerParent[parentid]++;

        }
      }
    }
  }

  if (m_CalcBiasedAvg == true)
  {
    for(int i=1;i<numFeatures;i++)
    {
      int parentid = m_FeatureParentIds[i];
      m_NumFeaturesPerParent[parentid]++;
      m_LocalCAxisMisalignments[parentid] += m_AvgCAxisMisalignments[i];
    }
  }

  for(int i=1;i<newNumFeatures;i++)
  {
    if (m_CalcBiasedAvg == true) m_LocalCAxisMisalignments[i] /= m_NumFeaturesPerParent[i];
    if (m_CalcUnbiasedAvg == true)
    {
      if(NumUnbiasedFeaturesPerParent[i] > 0) m_UnbiasedLocalCAxisMisalignments[i] /= NumUnbiasedFeaturesPerParent[i];
      else m_UnbiasedLocalCAxisMisalignments[i] = 0.0f;
    }
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindLocalAverageCAxisMisalignments::newFilterInstance(bool copyFilterParameters)
{
  /*
  * CalcUnbiasedAvg
  * CalcBiasedAvg
  */
  FindLocalAverageCAxisMisalignments::Pointer filter = FindLocalAverageCAxisMisalignments::New();
  if(true == copyFilterParameters)
  {
    filter->setCalcUnbiasedAvg( getCalcUnbiasedAvg() );
    filter->setCalcBiasedAvg( getCalcBiasedAvg() );
  }
  return filter;
}
