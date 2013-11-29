#include "FindMicroTextureRegions.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/GenericFilters/FindFeaturePhases.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindMicroTextureRegions::FindMicroTextureRegions() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_MicroTextureRegionNumCellsArrayName(DREAM3D::FeatureData::MicroTextureRegionNumCells),
  m_MicroTextureRegionFractionOccupiedArrayName(DREAM3D::FeatureData::MicroTextureRegionFractionOccupied),
  m_MicroTextureRegionNumCells(NULL),
  m_MicroTextureRegionFractionOccupied(NULL),
  m_FeatureIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindMicroTextureRegions::~FindMicroTextureRegions()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindMicroTextureRegions::setupFilterParameters()
{
}
// -----------------------------------------------------------------------------
void FindMicroTextureRegions::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int FindMicroTextureRegions::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindMicroTextureRegions::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, FindMicroTextureRegions>(getDataContainerName(), false, this);
  if(getErrorCondition() < 0) { return; }

  QVector<int> dims(1, 1);
  // Cell Data
  m_FeatureIdsPtr = m->getPrereqArray<int32_t, AbstractFilter>(this, m_CellAttributeMatrixName,  m_FeatureIdsArrayName, -300, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Feature Data
  m_MicroTextureRegionNumCellsPtr = m->createNonPrereqArray<int32_t, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_MicroTextureRegionNumCellsArrayName, 0, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_MicroTextureRegionNumCellsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_MicroTextureRegionNumCells = m_MicroTextureRegionNumCellsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_MicroTextureRegionFractionOccupiedPtr = m->createNonPrereqArray<float, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_MicroTextureRegionFractionOccupiedArrayName, 0, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_MicroTextureRegionFractionOccupiedPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_MicroTextureRegionFractionOccupied = m_MicroTextureRegionFractionOccupiedPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindMicroTextureRegions::preflight()
{

  dataCheck(true, 1, 1, 1);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindMicroTextureRegions::execute()
{
  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(getDataContainerName(), false, this);
  if(getErrorCondition() < 0) { return; }
  setErrorCondition(0);

  int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();
  size_t totalFeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  dataCheck(false, totalPoints, totalFeatures, 0);
  if (getErrorCondition() < 0)
  {
    return;
  }

  find_microtextureregions();
  notifyStatusMessage("FindMicroTextureRegions Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindMicroTextureRegions::find_microtextureregions()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();
  size_t nummicrotextureregions = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();

  DataArray<double>::Pointer m_MicroTextureRegionCounts = DataArray<double>::CreateArray(nummicrotextureregions, "MicroTextureRegionCounts");
  DataArray<double>::Pointer m_MicroTextureRegionXMaxs = DataArray<double>::CreateArray(nummicrotextureregions, "MicroTextureRegionCounts");
  DataArray<double>::Pointer m_MicroTextureRegionYMaxs = DataArray<double>::CreateArray(nummicrotextureregions, "MicroTextureRegionCounts");
  DataArray<double>::Pointer m_MicroTextureRegionZMaxs = DataArray<double>::CreateArray(nummicrotextureregions, "MicroTextureRegionCounts");
  DataArray<double>::Pointer m_MicroTextureRegionXMins = DataArray<double>::CreateArray(nummicrotextureregions, "MicroTextureRegionCounts");
  DataArray<double>::Pointer m_MicroTextureRegionYMins = DataArray<double>::CreateArray(nummicrotextureregions, "MicroTextureRegionCounts");
  DataArray<double>::Pointer m_MicroTextureRegionZMins = DataArray<double>::CreateArray(nummicrotextureregions, "MicroTextureRegionCounts");

  double* microtextureregioncounts = m_MicroTextureRegionCounts->getPointer(0);
  double* microtextureregionxmins = m_MicroTextureRegionXMins->getPointer(0);
  double* microtextureregionxmaxs = m_MicroTextureRegionXMaxs->getPointer(0);
  double* microtextureregionymins = m_MicroTextureRegionYMins->getPointer(0);
  double* microtextureregionymaxs = m_MicroTextureRegionYMaxs->getPointer(0);
  double* microtextureregionzmins = m_MicroTextureRegionZMins->getPointer(0);
  double* microtextureregionzmaxs = m_MicroTextureRegionZMaxs->getPointer(0);

  float xPoints = m->getXPoints();
  float yPoints = m->getYPoints();
  float zPoints = m->getZPoints();
  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();

  // Initialize every element to 0.0 or max Dimension
  for (size_t i = 0; i < nummicrotextureregions * 1; i++)
  {
    microtextureregioncounts[i] = 0.0f;
    microtextureregionxmins[i] = xRes * (float)xPoints;
    microtextureregionxmaxs[i] = 0.0f;
    microtextureregionymins[i] = yRes * (float)yPoints;
    microtextureregionymaxs[i] = 0.0f;
    microtextureregionzmins[i] = zRes * (float)zPoints;
    microtextureregionzmaxs[i] = 0.0f;
  }

  for (size_t j = 0; j < totalPoints; j++)
  {
    int mtnum = m_FeatureIds[j];
    microtextureregioncounts[mtnum]++;
  }

  for (size_t i = 1; i < nummicrotextureregions; i++)
  {
    m_MicroTextureRegionNumCells[i] = static_cast<int32_t>( microtextureregioncounts[i] );
  }

  float x, y, z;
  size_t zStride, yStride;
  for(size_t i = 0; i < zPoints; i++)
  {
    zStride = i * xPoints * yPoints;
    for (size_t j = 0; j < yPoints; j++)
    {
      yStride = j * xPoints;
      for(size_t k = 0; k < xPoints; k++)
      {
        int mtnum = m_FeatureIds[zStride + yStride + k];
        x = float(k) * xRes;
        y = float(j) * yRes;
        z = float(i) * zRes;
        if (x > microtextureregionxmaxs[mtnum]) { microtextureregionxmaxs[mtnum] = x; }
        if (y > microtextureregionymaxs[mtnum]) { microtextureregionymaxs[mtnum] = y; }
        if (z > microtextureregionzmaxs[mtnum]) { microtextureregionzmaxs[mtnum] = z; }
        if (x < microtextureregionxmins[mtnum]) { microtextureregionxmins[mtnum] = x; }
        if (y < microtextureregionymins[mtnum]) { microtextureregionymins[mtnum] = y; }
        if (z < microtextureregionzmins[mtnum]) { microtextureregionzmins[mtnum] = z; }
      }
    }
  }

  for (size_t i = 1; i < nummicrotextureregions; i++)
  {
    float xlength = (microtextureregionxmaxs[i] - microtextureregionxmins[i]) + xRes;
    float ylength = (microtextureregionymaxs[i] - microtextureregionymins[i]) + yRes;
    if (zPoints == 1)
    {
      float zlength = (microtextureregionzmaxs[i] - microtextureregionzmins[i]) + zRes;
      float prismvolume = xlength * ylength * zlength;
      m_MicroTextureRegionFractionOccupied[i] = ((float)m_MicroTextureRegionNumCells[i] * xRes * yRes * zRes) / prismvolume;
    }
    else
    {
      float rectanglevolume = xlength * ylength;
      m_MicroTextureRegionFractionOccupied[i] = ((float)m_MicroTextureRegionNumCells[i] * xRes * yRes) / rectanglevolume;
    }
  }
}

