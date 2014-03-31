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

#include "FindNeighborhoods.h"


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNeighborhoods::FindNeighborhoods() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_NeighborhoodListArrayName(DREAM3D::FeatureData::NeighborhoodList),
  m_NumNeighborsArrayName(DREAM3D::FeatureData::NumNeighbors),
  m_MultiplesOfAverage(1.0f),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_FeaturePhases(NULL),
  m_CentroidsArrayName(DREAM3D::FeatureData::Centroids),
  m_Centroids(NULL),
  m_EquivalentDiametersArrayName(DREAM3D::FeatureData::EquivalentDiameters),
  m_EquivalentDiameters(NULL),
  m_NeighborhoodsArrayName(DREAM3D::FeatureData::Neighborhoods),
  m_Neighborhoods(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNeighborhoods::~FindNeighborhoods()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Multiples Of Average Diameter", "MultiplesOfAverage", FilterParameterWidgetType::DoubleWidget,"float", false));

  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void FindNeighborhoods::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setMultiplesOfAverage( reader->readValue("MultiplesOfAverage", getMultiplesOfAverage()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindNeighborhoods::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("MultiplesOfAverage", getMultiplesOfAverage() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  AttributeMatrix::Pointer cellFeatureAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }

  // Feature Data
  // Do this whole block FIRST otherwise the side effect is that a call to m->getNumCellFeatureTuples will = 0
  // because we are just creating an empty NeighborList object.
  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  m_NeighborhoodList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>* >
      (m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getAttributeArray(m_NeighborhoodListArrayName).get());
  if(m_NeighborhoodList == NULL)
  {
    NeighborList<int>::Pointer neighborhoodlistPtr = NeighborList<int>::New();
    neighborhoodlistPtr->setName(m_NeighborhoodListArrayName);
    neighborhoodlistPtr->resize(cellFeatureAttrMat->getNumTuples());
    neighborhoodlistPtr->setNumNeighborsArrayName(m_NeighborhoodsArrayName);
    m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->addAttributeArray(m_NeighborhoodListArrayName, neighborhoodlistPtr);

    if (neighborhoodlistPtr.get() == NULL)
    {
      QString ss = QObject::tr("NeighborhoodLists Array Not Initialized at Beginning of FindNeighbors Filter");
      setErrorCondition(-308);
    }
    m_NeighborhoodList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>* >
        (m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getAttributeArray(m_NeighborhoodListArrayName).get());
  }

  QVector<size_t> dims(1, 1);
  m_EquivalentDiametersPtr = cellFeatureAttrMat->getPrereqArray<DataArray<float>, AbstractFilter>(this, m_EquivalentDiametersArrayName, -302, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_EquivalentDiametersPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_EquivalentDiameters = m_EquivalentDiametersPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_FeaturePhasesPtr = cellFeatureAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_FeaturePhasesArrayName, -304, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_NeighborhoodsPtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, m_NeighborhoodsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NeighborhoodsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Neighborhoods = m_NeighborhoodsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 3;
  m_CentroidsPtr = cellFeatureAttrMat->getPrereqArray<DataArray<float>, AbstractFilter>(this, m_CentroidsArrayName, -305, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CentroidsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Centroids = m_CentroidsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  find_neighborhoods();
  notifyStatusMessage(getHumanLabel(), "FindNeighborhoods Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNeighborhoods::find_neighborhoods()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  float x, y, z;
  //  float xn, yn, zn;
  //  float dx, dy, dz;

  QVector<QVector<int> > neighborhoodlist;
  QVector<float> criticalDistance;

  //int64_t totalPoints = m->getTotalPoints();
  size_t totalFeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  // size_t totalEnsembles = 0;
  dataCheck();

  neighborhoodlist.resize(totalFeatures);
  criticalDistance.resize(totalFeatures);

  float aveDiam = 0.0f;
  for (size_t i = 1; i < totalFeatures; i++)
  {
    m_Neighborhoods[i] = 0;
    aveDiam += m_EquivalentDiameters[i];
    criticalDistance[i] = m_EquivalentDiameters[i] * m_MultiplesOfAverage;
  }
  aveDiam /= totalFeatures;
  for (size_t i = 1; i < totalFeatures; i++)
  {
    criticalDistance[i] /= aveDiam;
  }

  float m_OriginX, m_OriginY, m_OriginZ;
  m->getOrigin(m_OriginX, m_OriginY, m_OriginZ);
  size_t udims[3] = {0, 0, 0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif

  int xbin, ybin, zbin;
  //int bin, bin1, bin2;
  QVector<size_t> bins(3*totalFeatures, 0);
  for (size_t i = 1; i < totalFeatures; i++)
  {
    x = m_Centroids[3 * i];
    y = m_Centroids[3 * i + 1];
    z = m_Centroids[3 * i + 2];
    xbin = int((x - m_OriginX) / aveDiam);
    ybin = int((y - m_OriginY) / aveDiam);
    zbin = int((z - m_OriginZ) / aveDiam);
    bins[3*i] = xbin;
    bins[3*i+1] = ybin;
    bins[3*i+2] = zbin;
  }

  int bin1x, bin2x, bin1y, bin2y, bin1z, bin2z;
  int dBinX, dBinY, dBinZ;
  int criticalDistance1, criticalDistance2;

  for (size_t i = 1; i < totalFeatures; i++)
  {
    if (i % 1000 == 0)
    {

      QString ss = QObject::tr("Working On Feature %1 of %2").arg(i).arg(totalFeatures);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    }
    x = m_Centroids[3 * i];
    y = m_Centroids[3 * i + 1];
    z = m_Centroids[3 * i + 2];
    bin1x = bins[3*i];
    bin1y = bins[3*i+1];
    bin1z = bins[3*i+2];
    criticalDistance1 = criticalDistance[i];

    for (size_t j = i + 1; j < totalFeatures; j++)
    {
      bin2x = bins[3*j];
      bin2y = bins[3*j+1];
      bin2z = bins[3*j+2];
      criticalDistance2 = criticalDistance[j];

      dBinX = abs(bin2x - bin1x);
      dBinY = abs(bin2y - bin1y);
      dBinZ = abs(bin2z - bin1z);

      if (dBinX < criticalDistance1 && dBinY < criticalDistance1 && dBinZ < criticalDistance1)
      {
        m_Neighborhoods[i]++;
        neighborhoodlist[i].push_back(j);
      }

      if (dBinX < criticalDistance2 && dBinY < criticalDistance2 && dBinZ < criticalDistance2)
      {
        m_Neighborhoods[j]++;
        neighborhoodlist[j].push_back(i);
      }
    }
  }
  for (size_t i = 1; i < totalFeatures; i++)
  {
    // Set the vector for each list into the NeighborhoodList Object
    NeighborList<int>::SharedVectorType sharedNeiLst(new std::vector<int>);
    sharedNeiLst->assign(neighborhoodlist[i].begin(), neighborhoodlist[i].end());
    m_NeighborhoodList->setList(static_cast<int>(i), sharedNeiLst);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindNeighborhoods::newFilterInstance(bool copyFilterParameters)
{
  /*
  * MultiplesOfAverage
  */
  FindNeighborhoods::Pointer filter = FindNeighborhoods::New();
  if(true == copyFilterParameters)
  {
    filter->setMultiplesOfAverage( getMultiplesOfAverage() );
  }
  return filter;
}
