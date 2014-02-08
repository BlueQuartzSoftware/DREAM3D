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

#include "FindFeatureNeighborCAxisMisalignments.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFeatureNeighborCAxisMisalignments::FindFeatureNeighborCAxisMisalignments()  :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellEnsembleAttributeMatrixName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_NeighborListArrayName(DREAM3D::FeatureData::NeighborList),
  m_CAxisMisalignmentListArrayName(DREAM3D::FeatureData::CAxisMisalignmentList),
  m_AvgQuatsArrayName(DREAM3D::FeatureData::AvgQuats),
  m_FindAvgMisals(false),
  m_AvgQuats(NULL),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_FeaturePhases(NULL),
  m_AvgCAxisMisalignmentsArrayName(DREAM3D::FeatureData::AvgCAxisMisalignments),
  m_AvgCAxisMisalignments(NULL),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CrystalStructures(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsVector();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFeatureNeighborCAxisMisalignments::~FindFeatureNeighborCAxisMisalignments()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureNeighborCAxisMisalignments::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Find Average C-Axis Misalignments");
    option->setPropertyName("FindAvgMisals");
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
void FindFeatureNeighborCAxisMisalignments::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setFindAvgMisals( reader->readValue("UseFindAvgMisals", getFindAvgMisals()) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindFeatureNeighborCAxisMisalignments::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("FindAvgMisals", getFindAvgMisals() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureNeighborCAxisMisalignments::dataCheck()
{
  setErrorCondition(0);
  QString ss;
  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  AttributeMatrix::Pointer cellEnsembleAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer cellFeatureAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }

  QVector<size_t> dims(1, 4);
  // Feature Data
  m_AvgQuatsPtr = cellFeatureAttrMat->getPrereqArray<DataArray<float>, AbstractFilter>(this, m_AvgQuatsArrayName, -301, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  m_FeaturePhasesPtr = cellFeatureAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_FeaturePhasesArrayName, -303, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // New Feature Data
  if(m_FindAvgMisals == true)
  {
  m_AvgCAxisMisalignmentsPtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this, m_AvgCAxisMisalignmentsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgCAxisMisalignmentsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgCAxisMisalignments = m_AvgCAxisMisalignmentsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  // Ensemble Data
  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = cellEnsembleAttrMat->getPrereqArray<DataArray<unsigned int>, AbstractFilter>(this, m_CrystalStructuresArrayName, -305, dims)
                           ; /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureNeighborCAxisMisalignments::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureNeighborCAxisMisalignments::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  size_t totalFeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();

  // But since a pointer is difficult to use operators with we will now create a
  // reference variable to the pointer with the correct variable name that allows
  // us to use the same syntax as the "vector of vectors"
  NeighborList<int>& neighborlist = *m_NeighborList;

  std::vector<std::vector<float> > misalignmentlists;

  float w;
  float g1[3][3];
  float g2[3][3];
  float g1t[3][3];
  float g2t[3][3];
  float c1[3];
  float c2[3];
  float caxis[3] = {0, 0, 1};
  size_t hexneighborlistsize = 0;
  QuatF q1;
  QuatF q2;
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  unsigned int phase1, phase2;

  //float radToDeg = 180.0 / DREAM3D::Constants::k_Pi;

  size_t nname;
  // float nsa;
  misalignmentlists.resize(totalFeatures);
  for (size_t i = 1; i < totalFeatures; i++)
  {
    QuaternionMathF::Copy(avgQuats[i], q1);
    phase1 = m_CrystalStructures[m_FeaturePhases[i]];
    OrientationMath::QuattoMat(q1, g1);
    //transpose the g matrix so when caxis is multiplied by it
    //it will give the sample direction that the caxis is along
    MatrixMath::Transpose3x3(g1, g1t);
    MatrixMath::Multiply3x3with3x1(g1t, caxis, c1);
    //normalize so that the dot product can be taken below without
    //dividing by the magnitudes (they would be 1)
    MatrixMath::Normalize3x1(c1);
    misalignmentlists[i].resize(neighborlist[i].size(), -1.0);
    for (size_t j = 0; j < neighborlist[i].size(); j++)
    {
      w = 10000.0;
      nname = neighborlist[i][j];
      phase2 = m_CrystalStructures[m_FeaturePhases[nname]];
      hexneighborlistsize = neighborlist[i].size();
      if (phase1 == phase2 && (phase1 == Ebsd::CrystalStructure::Hexagonal_High) )
      {
        QuaternionMathF::Copy(avgQuats[nname], q2);
        OrientationMath::QuattoMat(q2, g2);
        //transpose the g matrix so when caxis is multiplied by it
        //it will give the sample direction that the caxis is along
        MatrixMath::Transpose3x3(g2, g2t);
        MatrixMath::Multiply3x3with3x1(g2t, caxis, c2);
        //normalize so that the dot product can be taken below without
        //dividing by the magnitudes (they would be 1)
        MatrixMath::Normalize3x1(c2);

        w = GeometryMath::CosThetaBetweenVectors(c1, c2);
        DREAM3DMath::boundF(w, -1, 1);
        w = acosf(w);
        if (w > (DREAM3D::Constants::k_Pi / 2)) w = DREAM3D::Constants::k_Pi - w;

        misalignmentlists[i][j] = w * DREAM3D::Constants::k_180OverPi;
        if (m_FindAvgMisals == true) m_AvgCAxisMisalignments[i] += misalignmentlists[i][j];
      }
      else
      {
        if (m_FindAvgMisals == true) hexneighborlistsize--;
        misalignmentlists[i][j] = -100.0f;
      }
    }
    if (m_FindAvgMisals == true)
    {
      if (hexneighborlistsize > 0) m_AvgCAxisMisalignments[i] /= hexneighborlistsize;
      else m_AvgCAxisMisalignments[i] = -100.0f;
      hexneighborlistsize = 0;
    }
  }

  // We do this to create new set of MisalignmentList objects
  totalFeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  dataCheck();

  for (size_t i = 1; i < totalFeatures; i++)
  {
    // Set the vector for each list into the NeighborList Object
    NeighborList<float>::SharedVectorType misaL(new std::vector<float>);
    misaL->assign(misalignmentlists[i].begin(), misalignmentlists[i].end());
    m_CAxisMisalignmentList->setList(static_cast<int>(i), misaL);
  }

  notifyStatusMessage(getHumanLabel(), "FindFeatureNeighborCAxisMisalignments Completed");
}
