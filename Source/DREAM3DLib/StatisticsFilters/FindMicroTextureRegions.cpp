#include "FindMicroTextureRegions.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindMicroTextureRegions::FindMicroTextureRegions() :
AbstractFilter(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_MicroTextureRegionNumCellsArrayName(DREAM3D::FieldData::MicroTextureRegionNumCells),
m_MicroTextureRegionFractionOccupiedArrayName(DREAM3D::FieldData::MicroTextureRegionFractionOccupied),
m_MicroTextureRegionNumCells(NULL),
m_MicroTextureRegionFractionOccupied(NULL),
m_GrainIds(NULL)
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
void FindMicroTextureRegions::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  
  VolumeDataContainer* m = getVolumeDataContainer();

  // Cell Data

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, -300, int32_t, Int32ArrayType, voxels, 1)

  // Field Data

  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, MicroTextureRegionNumCells, int32_t, Int32ArrayType, 0, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, MicroTextureRegionFractionOccupied, float, FloatArrayType, 0, fields, 1)

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
  VolumeDataContainer* m = getVolumeDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
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
  VolumeDataContainer* m = getVolumeDataContainer();
  int64_t totalPoints = m->getTotalPoints();

  size_t nummicrotextureregions = m->getNumFieldTuples();

  DataArray<double>::Pointer m_MicroTextureRegionCounts = DataArray<double>::CreateArray(nummicrotextureregions, "MicroTextureRegionCounts");
  DataArray<double>::Pointer m_MicroTextureRegionXMaxs = DataArray<double>::CreateArray(nummicrotextureregions, "MicroTextureRegionCounts");
  DataArray<double>::Pointer m_MicroTextureRegionYMaxs = DataArray<double>::CreateArray(nummicrotextureregions, "MicroTextureRegionCounts");
  DataArray<double>::Pointer m_MicroTextureRegionZMaxs = DataArray<double>::CreateArray(nummicrotextureregions, "MicroTextureRegionCounts");
  DataArray<double>::Pointer m_MicroTextureRegionXMins = DataArray<double>::CreateArray(nummicrotextureregions, "MicroTextureRegionCounts");
  DataArray<double>::Pointer m_MicroTextureRegionYMins = DataArray<double>::CreateArray(nummicrotextureregions, "MicroTextureRegionCounts");
  DataArray<double>::Pointer m_MicroTextureRegionZMins = DataArray<double>::CreateArray(nummicrotextureregions, "MicroTextureRegionCounts");

  double* microtextureregioncounts = m_MicroTextureRegionCounts->GetPointer(0);
  double* microtextureregionxmins = m_MicroTextureRegionXMins->GetPointer(0);
  double* microtextureregionxmaxs = m_MicroTextureRegionXMaxs->GetPointer(0);
  double* microtextureregionymins = m_MicroTextureRegionYMins->GetPointer(0);
  double* microtextureregionymaxs = m_MicroTextureRegionYMaxs->GetPointer(0);
  double* microtextureregionzmins = m_MicroTextureRegionZMins->GetPointer(0);
  double* microtextureregionzmaxs = m_MicroTextureRegionZMaxs->GetPointer(0);

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
    int mtnum = m_GrainIds[j];
    microtextureregioncounts[mtnum]++;
  }

  for (size_t i = 1; i < nummicrotextureregions; i++)
  {
    m_MicroTextureRegionNumCells[i] = static_cast<int32_t>( microtextureregioncounts[i] );
  }

  float x, y, z;
  size_t zStride, yStride;
  for(size_t i=0;i<zPoints;i++)
  {
  zStride = i*xPoints*yPoints;
    for (size_t j=0;j<yPoints;j++)
    {
      yStride = j*xPoints;
      for(size_t k=0;k<xPoints;k++)
      {
       int mtnum = m_GrainIds[zStride+yStride+k];
        x = float(k) * xRes;
        y = float(j) * yRes;
        z = float(i) * zRes;
        if (x > microtextureregionxmaxs[mtnum]) microtextureregionxmaxs[mtnum] = x;
        if (y > microtextureregionymaxs[mtnum]) microtextureregionymaxs[mtnum] = y;
        if (z > microtextureregionzmaxs[mtnum]) microtextureregionzmaxs[mtnum] = z;
        if (x < microtextureregionxmins[mtnum]) microtextureregionxmins[mtnum] = x;
        if (y < microtextureregionymins[mtnum]) microtextureregionymins[mtnum] = y;
        if (z < microtextureregionzmins[mtnum]) microtextureregionzmins[mtnum] = z;
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

