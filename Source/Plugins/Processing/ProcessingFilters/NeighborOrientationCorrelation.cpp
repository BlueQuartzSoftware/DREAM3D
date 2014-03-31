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

#include "NeighborOrientationCorrelation.h"


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"
#include "DREAM3DLib/Math/OrientationMath.h"



#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
NeighborOrientationCorrelation::NeighborOrientationCorrelation() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellEnsembleAttributeMatrixName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_MisorientationTolerance(5),
  m_MinConfidence(0.1),
  m_Level(6),
  m_ConfidenceIndexArrayName(DREAM3D::CellData::ConfidenceIndex),
  m_ConfidenceIndex(NULL),
  m_QuatsArrayName(DREAM3D::CellData::Quats),
  m_Quats(NULL),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CellPhases(NULL),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CrystalStructures(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsVector();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
NeighborOrientationCorrelation::~NeighborOrientationCorrelation()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NeighborOrientationCorrelation::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Minimum Confidence Index", "MinConfidence", FilterParameterWidgetType::DoubleWidget,"float", false));
  parameters.push_back(FilterParameter::New("Misorientation Tolerance", "MisorientationTolerance", FilterParameterWidgetType::DoubleWidget,"float", false, "Degrees"));
  parameters.push_back(FilterParameter::New("Cleanup Level", "Level", FilterParameterWidgetType::IntWidget,"int", false));

  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void NeighborOrientationCorrelation::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setMinConfidence( reader->readValue("MinConfidence", getMinConfidence()) );
  setMisorientationTolerance( reader->readValue("MisorientationTolerance", getMisorientationTolerance()) );
  setLevel( reader->readValue("Level", getLevel()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int NeighborOrientationCorrelation::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("MisorientationTolerance", getMisorientationTolerance() );
  writer->writeValue("MinConfidence", getMinConfidence() );
  writer->writeValue("Level", getLevel() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NeighborOrientationCorrelation::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  AttributeMatrix::Pointer cellEnsembleAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -301);
  if(getErrorCondition() < 0 || NULL == cellAttrMat.get() ) { return; }

  QVector<size_t> dims(1, 1);
  m_ConfidenceIndexPtr = cellAttrMat->getPrereqArray<DataArray<float>, AbstractFilter>(this, m_ConfidenceIndexArrayName, -301, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ConfidenceIndexPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_ConfidenceIndex = m_ConfidenceIndexPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_CellPhasesPtr = cellAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_CellPhasesArrayName, -302, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = cellEnsembleAttrMat->getPrereqArray<DataArray<unsigned int>, AbstractFilter>(this, m_CrystalStructuresArrayName, -304, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 4;
  m_QuatsPtr = cellAttrMat->getPrereqArray<DataArray<float>, AbstractFilter>(this, m_QuatsArrayName, -301, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_QuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Quats = m_QuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NeighborOrientationCorrelation::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NeighborOrientationCorrelation::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();

  m_MisorientationTolerance = m_MisorientationTolerance * DREAM3D::Constants::k_Pi / 180.0;

  size_t udims[3] = {0, 0, 0};
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

  int currentLevel = 6;

  size_t count = 1;
  int best;
  int good = 1;
  int good2 = 1;
  int neighbor;
  int neighbor2;
  DimType column, row, plane;


  int neighpoints[6];
  neighpoints[0] = static_cast<int>(-dims[0] * dims[1]);
  neighpoints[1] = static_cast<int>(-dims[0]);
  neighpoints[2] = static_cast<int>(-1);
  neighpoints[3] = static_cast<int>(1);
  neighpoints[4] = static_cast<int>(dims[0]);
  neighpoints[5] = static_cast<int>(dims[0] * dims[1]);

  float w = 10000.0;
  QuatF q1;
  QuatF q2;
  float n1, n2, n3;
  unsigned int phase1, phase2;

  QVector<int> neighborDiffCount(totalPoints, 0);
  QVector<int> neighborSimCount(6, 0);
  QVector<int> bestNeighbor(totalPoints, -1);
  QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);

  while(currentLevel > m_Level)
  {
    for (int64_t i = 0; i < totalPoints; i++)
    {
      if(m_ConfidenceIndex[i] < m_MinConfidence)
      {
        count = 0;
        column = i % dims[0];
        row = (i / dims[0]) % dims[1];
        plane = i / (dims[0] * dims[1]);
        for (DimType j = 0; j < 6; j++)
        {
          good = 1;
          neighbor = i + neighpoints[j];
          if (j == 0 && plane == 0) { good = 0; }
          if (j == 5 && plane == (dims[2] - 1)) { good = 0; }
          if (j == 1 && row == 0) { good = 0; }
          if (j == 4 && row == (dims[1] - 1)) { good = 0; }
          if (j == 2 && column == 0) { good = 0; }
          if (j == 3 && column == (dims[0] - 1)) { good = 0; }
          if (good == 1)
          {
            phase1 = m_CrystalStructures[m_CellPhases[i]];
            QuaternionMathF::Copy(quats[i], q1);

            phase2 = m_CrystalStructures[m_CellPhases[neighbor]];
            QuaternionMathF::Copy(quats[neighbor], q2);

            if (m_CellPhases[i] == m_CellPhases[neighbor] && m_CellPhases[i] > 0) { w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3); }
            if (w > m_MisorientationTolerance)
            {
              neighborDiffCount[i]++;
            }
            for (DimType k = j + 1; k < 6; k++)
            {
              good2 = 1;
              neighbor2 = i + neighpoints[k];
              if (k == 0 && plane == 0) { good2 = 0; }
              if (k == 5 && plane == (dims[2] - 1)) { good2 = 0; }
              if (k == 1 && row == 0) { good2 = 0; }
              if (k == 4 && row == (dims[1] - 1)) { good2 = 0; }
              if (k == 2 && column == 0) { good2 = 0; }
              if (k == 3 && column == (dims[0] - 1)) { good2 = 0; }
              if (good2 == 1)
              {
                phase1 = m_CrystalStructures[m_CellPhases[neighbor2]];
                QuaternionMathF::Copy(quats[neighbor2], q1);

                phase2 = m_CrystalStructures[m_CellPhases[neighbor]];
                QuaternionMathF::Copy(quats[neighbor], q2);
                if (m_CellPhases[neighbor2] == m_CellPhases[neighbor] && m_CellPhases[neighbor2] > 0) { w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3); }
                if (w < m_MisorientationTolerance)
                {
                  neighborSimCount[j]++;
                  neighborSimCount[k]++;
                }
              }
            }
          }
        }
        for (DimType j = 0; j < 6; j++)
        {
          best = 0;
          good = 1;
          neighbor = i + neighpoints[j];
          if (j == 0 && plane == 0) { good = 0; }
          if (j == 5 && plane == (dims[2] - 1)) { good = 0; }
          if (j == 1 && row == 0) { good = 0; }
          if (j == 4 && row == (dims[1] - 1)) { good = 0; }
          if (j == 2 && column == 0) { good = 0; }
          if (j == 3 && column == (dims[0] - 1)) { good = 0; }
          if (good == 1)
          {
            if(neighborSimCount[j] > best)
            {
              best = neighborSimCount[j];
              bestNeighbor[i] = neighbor;
            }
            neighborSimCount[j] = 0;
          }
        }
      }
    }
    QList<QString> voxelArrayNames = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArrayNameList();
    for (int64_t j = 0; j < totalPoints; j++)
    {
      neighbor = bestNeighbor[j];
      if (neighbor != -1)
      {
        for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
        {
          QString name = *iter;
          IDataArray::Pointer p = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArray(*iter);
          p->copyTuple(neighbor, j);
        }
      }
    }
    currentLevel = currentLevel - 1;
  }

// If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Filling Bad Data Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer NeighborOrientationCorrelation::newFilterInstance(bool copyFilterParameters)
{
  /*
  * MisorientationTolerance
  * MinConfidence
  * Level
  */
  NeighborOrientationCorrelation::Pointer filter = NeighborOrientationCorrelation::New();
  if(true == copyFilterParameters)
  {
    filter->setMinConfidence( getMinConfidence() );
    filter->setMisorientationTolerance( getMisorientationTolerance() );
    filter->setLevel( getLevel() );
  }
  return filter;
}
