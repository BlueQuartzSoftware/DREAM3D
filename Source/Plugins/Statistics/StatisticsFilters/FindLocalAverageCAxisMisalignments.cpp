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
#include "DREAM3DLib/GenericFilters/FindFeaturePhases.h"
#include "Statistics/StatisticsFilters/FindNeighbors.h"

//#include "Statistics/StatisticsFilters/FindNeighbors.h"

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
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_CellParentIdsArrayName(DREAM3D::CellData::ParentIds),
  m_FeatureParentIdsArrayName(DREAM3D::FeatureData::ParentIds),
  m_FeatureParentIds(NULL),
  m_AvgCAxisMisalignmentsArrayName(DREAM3D::FeatureData::AvgCAxisMisalignments),
  m_AvgCAxisMisalignments(NULL),
  m_NumFeaturesPerParentArrayName(DREAM3D::FeatureData::NumFeaturesPerParent),
  m_NumFeaturesPerParent(NULL),
  m_AvgParentAvgCAxisMisalignmentsArrayName(DREAM3D::FeatureData::AvgParentAvgCAxisMisalignments),
  m_AvgParentAvgCAxisMisalignments(NULL),
  m_NeighborListArrayName(DREAM3D::FeatureData::NeighborList),
  m_CAxisMisalignmentListArrayName(DREAM3D::FeatureData::CAxisMisalignmentList),
  m_CalcUnbiasedAvg(false),
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
    option->setHumanLabel("Calculate Unbiased Average C-Axis Misorientation");
    option->setPropertyName("CalcUnbiasedAvg");
    option->setWidgetType(FilterParameterWidgetType::DoubleWidget);
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
	  m_NeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>*>(neighborListPtr.get());
	}

	IDataArray::Pointer misalignmentPtr = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getAttributeArray(m_CAxisMisalignmentListArrayName);
	if(NULL == misalignmentPtr.get())
	{
	  NeighborList<float>::Pointer misalignmentListPtr = NeighborList<float>::New();
	  misalignmentListPtr->setName(m_CAxisMisalignmentListArrayName);
	  misalignmentListPtr->resize(cellFeatureAttrMat->getNumTuples());
	  m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->addAttributeArray(m_CAxisMisalignmentListArrayName, misalignmentListPtr);
	  m_CAxisMisalignmentList = misalignmentListPtr.get();
	  if (misalignmentListPtr.get() == NULL)
	  {
		ss = QObject::tr("MisalignmentLists Array Not Initialized correctly");
		setErrorCondition(-308);
		notifyErrorMessage(getHumanLabel(), ss, -308);
	  }
	}
	else
	{
	  m_CAxisMisalignmentList = NeighborList<float>::SafeObjectDownCast<IDataArray*, NeighborList<float>*>(misalignmentPtr.get());
	  m_CAxisMisalignmentList->resize(cellFeatureAttrMat->getNumTuples());
	}
  }
  m_FeatureParentIdsPtr = cellFeatureAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_FeatureParentIdsArrayName, -302, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureParentIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureParentIds = m_FeatureParentIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_AvgCAxisMisalignmentsPtr = cellFeatureAttrMat->getPrereqArray<DataArray<float>, AbstractFilter>(this, m_AvgCAxisMisalignmentsArrayName, -302, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgCAxisMisalignmentsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgCAxisMisalignments = m_AvgCAxisMisalignmentsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // New Feature Data
  m_NumFeaturesPerParentPtr = newCellFeatureAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, float>(this, m_NumFeaturesPerParentArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NumFeaturesPerParentPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NumFeaturesPerParent = m_NumFeaturesPerParentPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_AvgParentAvgCAxisMisalignmentsPtr = newCellFeatureAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this, m_AvgParentAvgCAxisMisalignmentsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgParentAvgCAxisMisalignmentsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgParentAvgCAxisMisalignments = m_AvgParentAvgCAxisMisalignmentsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

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
  dataCheck();
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

  std::vector<std::vector<int> > afterNeighborList;
  std::vector<std::vector<float> > afterCAxisMisalignmentList;

  for(int i=0;i<numFeatures;i++)
  {
	if (m_CalcUnbiasedAvg == true)
	{
	  NeighborList<int>& neighborlist = *m_NeighborList;
	  NeighborList<float>& caxismisalignmentList = *m_CAxisMisalignmentList;

	  for(int i=0;i<numFeatures;i++)
	  {
		for (size_t j = 0; j < neighborlist[i].size(); j++)
		{
		  afterNeighborList[i][j] = neighborlist[i][j];
		  afterCAxisMisalignmentList[i][j] = caxismisalignmentList[i][j];
		}
	  } 
	}	
  }

  for(int i=0;i<numFeatures;i++)
  {
	int parentid = m_FeatureParentIds[i];
	if (m_CalcUnbiasedAvg == false)
	{
	  m_NumFeaturesPerParent[parentid]++;
      m_AvgParentAvgCAxisMisalignments[parentid] += m_AvgCAxisMisalignments[i];
	}
	else if (m_CalcUnbiasedAvg == true)
	{
	  for (size_t j = 0; j < afterNeighborList[i].size(); j++)
	  {
		if (m_FeatureParentIds[afterNeighborList[i][j]] == m_FeatureParentIds[i])
		{
		  m_NumFeaturesPerParent[parentid]++;
		  m_AvgParentAvgCAxisMisalignments[parentid] += m_AvgCAxisMisalignments[i];
		}
	  }
	}
  }

  for(int i=0;i<newNumFeatures;i++)
  {
    m_AvgParentAvgCAxisMisalignments[i] /= m_NumFeaturesPerParent[i];
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}
