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

#include "FindSizes.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/GenericFilters/FindFeaturePhases.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSizes::FindSizes() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_VolumesArrayName(DREAM3D::FeatureData::Volumes),
  m_Volumes(NULL),
  m_EquivalentDiametersArrayName(DREAM3D::FeatureData::EquivalentDiameters),
  m_EquivalentDiameters(NULL),
  m_NumCellsArrayName(DREAM3D::FeatureData::NumCells),
  m_NumCells(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSizes::~FindSizes()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::setupFilterParameters()
{
}
// -----------------------------------------------------------------------------
void FindSizes::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int FindSizes::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, FindSizes>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }

  QVector<int> dims(1, 1);
  m_FeatureIdsPtr = attrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_FeatureIdsArrayName, -300, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_VolumesPtr = attrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this, m_CellFeatureAttributeMatrixName,  m_VolumesArrayName, 0, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_VolumesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Volumes = m_VolumesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_EquivalentDiametersPtr = attrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this, m_CellFeatureAttributeMatrixName,  m_EquivalentDiametersArrayName, 0, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_EquivalentDiametersPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_EquivalentDiameters = m_EquivalentDiametersPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_NumCellsPtr = attrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, m_CellFeatureAttributeMatrixName,  m_NumCellsArrayName, 0, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NumCellsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NumCells = m_NumCellsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::preflight()
{

  dataCheck(true, 1, 1, 1);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::execute()
{
  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }
  setErrorCondition(0);

  int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();
  size_t totalFeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  dataCheck(false, totalPoints, totalFeatures, 0);
  if (getErrorCondition() < 0)
  {
    return;
  }


  if(m->getXPoints() > 1 && m->getYPoints() > 1 && m->getZPoints() > 1) { find_sizes(); }
  if(m->getXPoints() == 1 || m->getYPoints() == 1 || m->getZPoints() == 1) { find_sizes2D(); }
  notifyStatusMessage("FindSizes Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::find_sizes()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  int64_t totalPoints = m->getTotalPoints();

  float radcubed;
  float diameter;

  size_t numfeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();

  DataArray<double>::Pointer m_FeatureCounts = DataArray<double>::CreateArray(numfeatures, "FeatureCounts");
  double* featurecounts = m_FeatureCounts->getPointer(0);

  // Initialize every element to 0.0
  for (size_t i = 0; i < numfeatures * 1; i++)
  {
    featurecounts[i] = 0.0f;
  }

  for (int64_t j = 0; j < totalPoints; j++)
  {
    int gnum = m_FeatureIds[j];
    featurecounts[gnum]++;
  }
  float res_scalar = m->getXRes() * m->getYRes() * m->getZRes();
  float vol_term = static_cast<double>( (4.0 / 3.0) * DREAM3D::Constants::k_Pi );
  for (size_t i = 1; i < numfeatures; i++)
  {
    m_NumCells[i] = static_cast<int32_t>( featurecounts[i] );
    m_Volumes[i] = (featurecounts[i] * res_scalar);
    radcubed = m_Volumes[i] / vol_term;
    diameter = 2.0f * powf(radcubed, 0.3333333333f);
    m_EquivalentDiameters[i] = diameter;
  }
}

void FindSizes::find_sizes2D()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  int64_t totalPoints = m->getTotalPoints();

  float radsquared;
  float diameter;
  size_t numfeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();

  DataArray<float>::Pointer m_FeatureCounts = DataArray<float>::CreateArray(numfeatures, "FeatureCounts");
  float* featurecounts = m_FeatureCounts->getPointer(0);

  for (size_t i = 0; i < numfeatures; i++)
  {
    featurecounts[i] = 0.0f;
  }
  for (int64_t j = 0; j < totalPoints; j++)
  {
    int gnum = m_FeatureIds[j];
    featurecounts[gnum]++;
  }
  float res_scalar = 0;
  if(m->getXPoints() == 1) { res_scalar = m->getYRes() * m->getZRes(); }
  else if(m->getYPoints() == 1) { res_scalar = m->getXRes() * m->getZRes(); }
  else if(m->getZPoints() == 1) { res_scalar = m->getXRes() * m->getYRes(); }
  for (size_t i = 1; i < numfeatures; i++)
  {
    m_NumCells[i] = static_cast<int32_t>( featurecounts[i] );
    m_Volumes[i] = (featurecounts[i] * res_scalar);
    radsquared = m_Volumes[i] / DREAM3D::Constants::k_Pi;
    diameter = (2 * sqrt(radsquared));
    m_EquivalentDiameters[i] = diameter;
  }
}


