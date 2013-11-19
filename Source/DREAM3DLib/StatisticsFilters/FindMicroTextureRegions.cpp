#include "FindMicroTextureRegions.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/GenericFilters/FindFeaturePhases.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindMicroTextureRegions::FindMicroTextureRegions() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
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

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QVector<int> dims(1, 1);
  // Cell Data
  GET_PREREQ_DATA(m, DREAM3D, CellData, FeatureIds, -300, int32_t, Int32ArrayType, voxels, dims)

  // Feature Data
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellFeatureData, MicroTextureRegionNumCells, int32_t, Int32ArrayType, 0, features, dims)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellFeatureData, MicroTextureRegionFractionOccupied, float, FloatArrayType, 0, features, dims)

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindMicroTextureRegions::preflight()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    addErrorMessage(getHumanLabel(), "The VolumeDataContainer Object with the specific name " + getDataContainerName() + " was not available.", getErrorCondition());
    return;
  }

  dataCheck(true, 1, 1, 1);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindMicroTextureRegions::execute()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  dataCheck(false, m->getTotalPoints(), m->getNumCellFeatureTuples(), m->getNumCellEnsembleTuples());
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

  int64_t totalPoints = m->getTotalPoints();

  size_t nummicrotextureregions = m->getNumCellFeatureTuples();

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

