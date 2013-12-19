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

#include "FindFeatureReferenceCAxisMisorientations.h"

#include <limits>


#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/GenericFilters/FindCellQuats.h"
#include "DREAM3DLib/StatisticsFilters/FindAvgCAxes.h"
#include "DREAM3DLib/StatisticsFilters/FindEuclideanDistMap.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFeatureReferenceCAxisMisorientations::FindFeatureReferenceCAxisMisorientations() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CellPhases(NULL),
  m_QuatsArrayName(DREAM3D::CellData::Quats),
  m_Quats(NULL),
  m_AvgCAxesArrayName(DREAM3D::FeatureData::AvgCAxes),
  m_AvgCAxes(NULL),
  m_FeatureReferenceCAxisMisorientationsArrayName(DREAM3D::CellData::FeatureReferenceCAxisMisorientations),
  m_FeatureReferenceCAxisMisorientations(NULL),
  m_FeatureAvgCAxisMisorientationsArrayName(DREAM3D::FeatureData::FeatureAvgCAxisMisorientations),
  m_FeatureAvgCAxisMisorientations(NULL),
  m_FeatureStdevCAxisMisorientationsArrayName(DREAM3D::FeatureData::FeatureStdevCAxisMisorientations),
  m_FeatureStdevCAxisMisorientations(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFeatureReferenceCAxisMisorientations::~FindFeatureReferenceCAxisMisorientations()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureReferenceCAxisMisorientations::setupFilterParameters()
{
}

// -----------------------------------------------------------------------------
void FindFeatureReferenceCAxisMisorientations::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int FindFeatureReferenceCAxisMisorientations::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureReferenceCAxisMisorientations::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, FindFeatureReferenceCAxisMisorientations>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix* cellFeatureAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix* cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }


  QVector<size_t> dims(1, 1);
  m_FeatureIdsPtr = cellAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_FeatureIdsArrayName, -300, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_CellPhasesPtr = cellAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_CellPhasesArrayName, -300, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_FeatureAvgCAxisMisorientationsPtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this, m_FeatureAvgCAxisMisorientationsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureAvgCAxisMisorientationsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureAvgCAxisMisorientations = m_FeatureAvgCAxisMisorientationsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_FeatureStdevCAxisMisorientationsPtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this, m_FeatureStdevCAxisMisorientationsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureStdevCAxisMisorientationsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureStdevCAxisMisorientations = m_FeatureStdevCAxisMisorientationsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_FeatureReferenceCAxisMisorientationsPtr = cellAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this, m_FeatureReferenceCAxisMisorientationsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureReferenceCAxisMisorientationsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureReferenceCAxisMisorientations = m_FeatureReferenceCAxisMisorientationsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 3;
  m_AvgCAxesPtr = cellFeatureAttrMat->getPrereqArray<DataArray<float>, AbstractFilter>(this, m_AvgCAxesArrayName, -303, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgCAxesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgCAxes = m_AvgCAxesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 4;
  m_QuatsPtr = cellAttrMat->getPrereqArray<DataArray<float>, AbstractFilter>(this, m_QuatsArrayName, -303, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_QuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Quats = m_QuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureReferenceCAxisMisorientations::preflight()
{
  dataCheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureReferenceCAxisMisorientations::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();
  size_t totalFeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();

  int avgMisoComps = 3;
  QVector<size_t> dims(1, avgMisoComps);
  FloatArrayType::Pointer avgmisoPtr = FloatArrayType::CreateArray(totalFeatures, dims, "AvgMiso_Temp");
  avgmisoPtr->initializeWithZeros();
  float* avgmiso = avgmisoPtr->getPointer(0);

  QuatF q1;
  QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);
  typedef DataArray<unsigned int> XTalType;

  float w;
  size_t udims[3] = {0, 0, 0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef uint32_t DimType;
  uint32_t maxUInt32 = std::numeric_limits<uint32_t>::max();
  // We have more points than can be allocated on a 32 bit machine. Assert Now.
  if(totalPoints > maxUInt32)
  {
    setErrorCondition(-666);
    ss.str("");
    ss << "The volume is too large for a 32 bit machine. Try reducing the input volume size. Total Voxels: " << totalPoints;
    notifyErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
    return;
  }
#else
  typedef int64_t DimType;
#endif

  DimType xPoints = static_cast<DimType>(udims[0]);
  DimType yPoints = static_cast<DimType>(udims[1]);
  DimType zPoints = static_cast<DimType>(udims[2]);

  DimType point;

  float g1[3][3];
  float g1t[3][3];
  float caxis[3] = {0, 0, 1};
  float c1[3];
  float AvgCAxis[3];
  size_t index = 0;

  for (DimType col = 0; col < xPoints; col++)
  {
    for (DimType row = 0; row < yPoints; row++)
    {
      for (DimType plane = 0; plane < zPoints; plane++)
      {
        point = (plane * xPoints * yPoints) + (row * xPoints) + col;
        if (m_FeatureIds[point] > 0 && m_CellPhases[point] > 0)
        {
          QuaternionMathF::Copy(quats[point], q1);
          OrientationMath::QuattoMat(q1, g1);
          //transpose the g matricies so when caxis is multiplied by it
          //it will give the sample direction that the caxis is along
          MatrixMath::Transpose3x3(g1, g1t);
          MatrixMath::Multiply3x3with3x1(g1t, caxis, c1);
          //normalize so that the magnitude is 1
          MatrixMath::Normalize3x1(c1);

          AvgCAxis[0] = m_AvgCAxes[3 * m_FeatureIds[point]];
          AvgCAxis[1] = m_AvgCAxes[3 * m_FeatureIds[point] + 1];
          AvgCAxis[2] = m_AvgCAxes[3 * m_FeatureIds[point] + 2];
          //normalize so that the magnitude is 1
          MatrixMath::Normalize3x1(AvgCAxis);
          w = GeometryMath::CosThetaBetweenVectors(c1, AvgCAxis);
          DREAM3DMath::boundF(w, -1, 1);
          w = acosf(w);
          w = w * DREAM3D::Constants::k_180OverPi;
          if(w > 90.0) { w = 180.0 - w; }

          m_FeatureReferenceCAxisMisorientations[point] = w;
          index = m_FeatureIds[point] * avgMisoComps;
          avgmiso[index]++;
          avgmiso[index + 1] += w;
        }
        if (m_FeatureIds[point] == 0 || m_CellPhases[point] == 0)
        {
          m_FeatureReferenceCAxisMisorientations[point] = 0;
        }
      }
    }
  }

  for (size_t i = 1; i < totalFeatures; i++)
  {
    if (i % 1000 == 0)
    {
      QString ss = QObject::tr("Working On Feature %1 of %2").arg(i).arg(totalFeatures);
      notifyStatusMessage(getHumanLabel(), ss);
    }
    index = i * avgMisoComps;
    m_FeatureAvgCAxisMisorientations[i] = avgmiso[index + 1] / avgmiso[index];
    if(avgmiso[index] == 0) { m_FeatureAvgCAxisMisorientations[i] = 0.0; }
  }


  size_t gNum;
  for (size_t j = 0; j < totalPoints; j++)
  {
    gNum = m_FeatureIds[j];
    avgmiso[(gNum * avgMisoComps) + 2] += ((m_FeatureReferenceCAxisMisorientations[j] - m_FeatureAvgCAxisMisorientations[gNum]) * (m_FeatureReferenceCAxisMisorientations[j] - m_FeatureAvgCAxisMisorientations[gNum]));
  }

  for (size_t i = 1; i < totalFeatures; i++)
  {
    index = i * avgMisoComps;
    m_FeatureStdevCAxisMisorientations[i] = sqrtf((1 / avgmiso[index]) * avgmiso[index + 2]);
  }

  notifyStatusMessage(getHumanLabel(), "Completed");
}

