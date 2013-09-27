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

#include "FindSlipTransmissionMetrics.h"


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"
#include "DREAM3DLib/StatisticsFilters/FindNeighbors.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSlipTransmissionMetrics::FindSlipTransmissionMetrics() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_F1ArrayName(DREAM3D::FieldData::F1),
  m_F1sptArrayName(DREAM3D::FieldData::F1spt),
  m_F7ArrayName(DREAM3D::FieldData::F7),
  m_mPrimeArrayName(DREAM3D::FieldData::mPrime),
  m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
  m_AvgQuatsArrayName(DREAM3D::FieldData::AvgQuats),
  m_FieldPhases(NULL),
  m_AvgQuats(NULL),
  m_F1(NULL),
  m_F1spt(NULL),
  m_F7(NULL),
  m_mPrime(NULL),
  m_NeighborList(NULL),
  m_CrystalStructures(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsVector();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSlipTransmissionMetrics::~FindSlipTransmissionMetrics()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSlipTransmissionMetrics::setupFilterParameters()
{

}
// -----------------------------------------------------------------------------
void FindSlipTransmissionMetrics::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindSlipTransmissionMetrics::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSlipTransmissionMetrics::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  GET_PREREQ_DATA(m, DREAM3D, CellFieldData, AvgQuats, -301, float, FloatArrayType, fields, 4)
  GET_PREREQ_DATA(m, DREAM3D, CellFieldData, FieldPhases, -302, int32_t, Int32ArrayType, fields, 1)

  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  m_NeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>*>(m->getCellFieldData(DREAM3D::FieldData::NeighborList).get());
  if(m_NeighborList == NULL)
  {
    QString ss = QObject::tr("NeighborLists Array Not Initialized correctly");
    setErrorCondition(-305);
    addErrorMessage(getHumanLabel(), ss, -305);
  }

  NeighborList<float>::Pointer f1Ptr = NeighborList<float>::New();
  f1Ptr->SetName(DREAM3D::FieldData::F1);
  f1Ptr->Resize(fields);
  m->addCellFieldData(DREAM3D::FieldData::F1, f1Ptr);
  if (f1Ptr.get() == NULL)
  {
    QString ss = QObject::tr("F1 Array Not Initialized At Beginning of FindSlipTransmissionMetrics Filter");
    setErrorCondition(-308);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  NeighborList<float>::Pointer f1sptPtr = NeighborList<float>::New();
  f1sptPtr->SetName(DREAM3D::FieldData::F1spt);
  f1sptPtr->Resize(fields);
  m->addCellFieldData(DREAM3D::FieldData::F1spt, f1sptPtr);
  if (f1sptPtr.get() == NULL)
  {
    QString ss = QObject::tr("F1spt Array Not Initialized At Beginning of FindSlipTransmissionMetrics Filter");
    setErrorCondition(-308);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  NeighborList<float>::Pointer f7Ptr = NeighborList<float>::New();
  f7Ptr->SetName(DREAM3D::FieldData::F7);
  f7Ptr->Resize(fields);
  m->addCellFieldData(DREAM3D::FieldData::F7, f7Ptr);
  if (f7Ptr.get() == NULL)
  {
    QString ss = QObject::tr("F7 Array Not Initialized At Beginning of FindSlipTransmissionMetrics Filter");
    setErrorCondition(-308);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  NeighborList<float>::Pointer mPrimePtr = NeighborList<float>::New();
  mPrimePtr->SetName(DREAM3D::FieldData::mPrime);
  mPrimePtr->Resize(fields);
  m->addCellFieldData(DREAM3D::FieldData::mPrime, mPrimePtr);
  if (mPrimePtr.get() == NULL)
  {
    QString ss = QObject::tr("mPrime Array Not Initialized At Beginning of FindSlipTransmissionMetrics Filter");
    setErrorCondition(-308);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, CellEnsembleData, CrystalStructures, -305, unsigned int, XTalStructArrayType, ensembles, 1)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSlipTransmissionMetrics::preflight()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  dataCheck(true, 1,1 ,1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSlipTransmissionMetrics::execute()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  int64_t totalPoints = m->getTotalPoints();
  int64_t totalFields = m->getNumCellFieldTuples();
  dataCheck(false, totalPoints, totalFields, m->getNumCellEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  // But since a pointer is difficult to use operators with we will now create a
  // reference variable to the pointer with the correct variable name that allows
  // us to use the same syntax as the "vector of vectors"
  NeighborList<int>& neighborlist = *m_NeighborList;

  QVector<QVector<float> > F1lists;
  QVector<QVector<float> > F1sptlists;
  QVector<QVector<float> > F7lists;
  QVector<QVector<float> > mPrimelists;

  float mprime, F1, F1spt, F7;
  int nname;
  QuatF q1;
  QuatF q2;
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);


  float LD[3];
  LD[0] = 0;
  LD[1] = 0;
  LD[2] = 1;

  F1lists.resize(totalFields);
  F1sptlists.resize(totalFields);
  F7lists.resize(totalFields);
  mPrimelists.resize(totalFields);
  for (int i = 1; i < totalFields; i++)
  {
    F1lists[i].fill(0.0f, neighborlist[i].size());
    F1sptlists[i].fill(0.0f, neighborlist[i].size());
    F7lists[i].fill(0.0f, neighborlist[i].size());
    mPrimelists[i].fill(0.0f, neighborlist[i].size());
    for (size_t j = 0; j < neighborlist[i].size(); j++)
    {
      nname = neighborlist[i][j];

      QuaternionMathF::Copy(avgQuats[i], q1);
      QuaternionMathF::Copy(avgQuats[nname], q1);
//      for (int k = 0; k < 5; k++)
//      {
//        q1[k] = m_AvgQuats[5 * i + k];
//        q2[k] = m_AvgQuats[5 * nname + k];
//      }
      if(m_CrystalStructures[m_FieldPhases[i]] == m_CrystalStructures[m_FieldPhases[nname]] && m_FieldPhases[i] > 0)
      {
        m_OrientationOps[m_CrystalStructures[m_FieldPhases[i]]]->getmPrime(q1, q2, LD, mprime);
        m_OrientationOps[m_CrystalStructures[m_FieldPhases[i]]]->getF1(q1, q2, LD, true, F1);
        m_OrientationOps[m_CrystalStructures[m_FieldPhases[i]]]->getF1spt(q1, q2, LD, true, F1spt);
        m_OrientationOps[m_CrystalStructures[m_FieldPhases[i]]]->getF7(q1, q2, LD, true, F7);
      }
      else
      {
        mprime = 0;
        F1 = 0;
        F1spt = 0;
        F7 = 0;
      }
      mPrimelists[i][j] = mprime;
      F1lists[i][j] = F1;
      F1sptlists[i][j] = F1spt;
      F7lists[i][j] = F7;
    }
  }

  // We do this to create new set of List objects
  dataCheck(false, m->getNumCellTuples(), m->getNumCellFieldTuples(), m->getNumCellEnsembleTuples());

  for (int64_t i = 1; i < totalFields; i++)
  {
    // Set the vector for each list into the NeighborList Object
    NeighborList<float>::SharedVectorType f1L(new std::vector<float>);
    f1L->assign(F1lists[i].begin(), F1lists[i].end());
    m_F1->setList(static_cast<int>(i), f1L);

    // Set the vector for each list into the NeighborList Object
    NeighborList<float>::SharedVectorType f1sptL(new std::vector<float>);
    f1sptL->assign(F1sptlists[i].begin(), F1sptlists[i].end());
    m_F1spt->setList(static_cast<int>(i), f1sptL);

    // Set the vector for each list into the NeighborList Object
    NeighborList<float>::SharedVectorType f7L(new std::vector<float>);
    f7L->assign(F7lists[i].begin(), F7lists[i].end());
    m_F7->setList(static_cast<int>(i), f7L);

    // Set the vector for each list into the NeighborList Object
    NeighborList<float>::SharedVectorType mPrimeL(new std::vector<float>);
    mPrimeL->assign(mPrimelists[i].begin(), mPrimelists[i].end());
    m_mPrime->setList(static_cast<int>(i), mPrimeL);
  }

  notifyStatusMessage("Completed");
}
