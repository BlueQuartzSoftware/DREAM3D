/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "FindMicroTextureRegions.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/SIMPLibMath.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

// Include the MOC generated file for this class
#include "moc_FindMicroTextureRegions.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindMicroTextureRegions::FindMicroTextureRegions()
: AbstractFilter()
, m_CellFeatureAttributeMatrixName(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, "")
, m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds)
, m_MicroTextureRegionNumCellsArrayName("MicroTextureRegionNumCells")
, m_MicroTextureRegionFractionOccupiedArrayName("MicroTextureRegionFractionOccupied")
, m_MicroTextureRegionNumCells(nullptr)
, m_MicroTextureRegionFractionOccupied(nullptr)
, m_FeatureIds(nullptr)
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
  FilterParameterVector parameters;

  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::Defaults::AnyPrimitive, 1, AttributeMatrix::Category::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("FeatureIds", FeatureIdsArrayPath, FilterParameter::RequiredArray, FindMicroTextureRegions, req));
  }
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req =
        AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::CellFeature, IGeometry::Type::Unknown);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Cell Feature Attribute Matrix Name", CellFeatureAttributeMatrixName, FilterParameter::RequiredArray, FindMicroTextureRegions, req));
  }

  parameters.push_back(SIMPL_NEW_STRING_FP("MicroTextureRegionNumCells", MicroTextureRegionNumCellsArrayName, FilterParameter::CreatedArray, FindMicroTextureRegions));
  parameters.push_back(SIMPL_NEW_STRING_FP("MicroTextureRegionFractionOccupied", MicroTextureRegionFractionOccupiedArrayName, FilterParameter::CreatedArray, FindMicroTextureRegions));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void FindMicroTextureRegions::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellFeatureAttributeMatrixName(reader->readDataArrayPath("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName()));
  setMicroTextureRegionFractionOccupiedArrayName(reader->readString("MicroTextureRegionFractionOccupiedArrayName", getMicroTextureRegionFractionOccupiedArrayName()));
  setMicroTextureRegionNumCellsArrayName(reader->readString("MicroTextureRegionNumCellsArrayName", getMicroTextureRegionNumCellsArrayName()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindMicroTextureRegions::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindMicroTextureRegions::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);
  setWarningCondition(0);

  QVector<size_t> dims(1, 1);
  // Cell Data
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(),
                                                                                                        dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock().get())                                                                  /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() < 0)
  {
    return;
  }

  ImageGeom::Pointer image = getDataContainerArray()->getDataContainer(getFeatureIdsArrayPath().getDataContainerName())->getPrereqGeometry<ImageGeom, AbstractFilter>(this);
  if(getErrorCondition() < 0 || nullptr == image.get())
  {
    return;
  }

  // Feature Data
  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getMicroTextureRegionNumCellsArrayName());
  m_MicroTextureRegionNumCellsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, 0, dims);                               /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_MicroTextureRegionNumCellsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_MicroTextureRegionNumCells = m_MicroTextureRegionNumCellsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getMicroTextureRegionFractionOccupiedArrayName());
  m_MicroTextureRegionFractionOccupiedPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
      this, tempPath, 0, dims);                                       /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_MicroTextureRegionFractionOccupiedPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_MicroTextureRegionFractionOccupied = m_MicroTextureRegionFractionOccupiedPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindMicroTextureRegions::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindMicroTextureRegions::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  find_microtextureregions();
  notifyStatusMessage(getHumanLabel(), "FindMicroTextureRegions Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindMicroTextureRegions::find_microtextureregions()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  size_t nummicrotextureregions = m_MicroTextureRegionNumCellsPtr.lock()->getNumberOfTuples();

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

  float xPoints = m->getGeometryAs<ImageGeom>()->getXPoints();
  float yPoints = m->getGeometryAs<ImageGeom>()->getYPoints();
  float zPoints = m->getGeometryAs<ImageGeom>()->getZPoints();
  float xRes = m->getGeometryAs<ImageGeom>()->getXRes();
  float yRes = m->getGeometryAs<ImageGeom>()->getYRes();
  float zRes = m->getGeometryAs<ImageGeom>()->getZRes();

  // Initialize every element to 0.0 or max Dimension
  for(size_t i = 0; i < nummicrotextureregions * 1; i++)
  {
    microtextureregioncounts[i] = 0.0f;
    microtextureregionxmins[i] = xRes * (float)xPoints;
    microtextureregionxmaxs[i] = 0.0f;
    microtextureregionymins[i] = yRes * (float)yPoints;
    microtextureregionymaxs[i] = 0.0f;
    microtextureregionzmins[i] = zRes * (float)zPoints;
    microtextureregionzmaxs[i] = 0.0f;
  }

  for(size_t j = 0; j < totalPoints; j++)
  {
    int mtnum = m_FeatureIds[j];
    microtextureregioncounts[mtnum]++;
  }

  for(size_t i = 1; i < nummicrotextureregions; i++)
  {
    m_MicroTextureRegionNumCells[i] = static_cast<int32_t>(microtextureregioncounts[i]);
  }

  float x, y, z;
  size_t zStride, yStride;
  for(size_t i = 0; i < zPoints; i++)
  {
    zStride = i * xPoints * yPoints;
    for(size_t j = 0; j < yPoints; j++)
    {
      yStride = j * xPoints;
      for(size_t k = 0; k < xPoints; k++)
      {
        int mtnum = m_FeatureIds[zStride + yStride + k];
        x = float(k) * xRes;
        y = float(j) * yRes;
        z = float(i) * zRes;
        if(x > microtextureregionxmaxs[mtnum])
        {
          microtextureregionxmaxs[mtnum] = x;
        }
        if(y > microtextureregionymaxs[mtnum])
        {
          microtextureregionymaxs[mtnum] = y;
        }
        if(z > microtextureregionzmaxs[mtnum])
        {
          microtextureregionzmaxs[mtnum] = z;
        }
        if(x < microtextureregionxmins[mtnum])
        {
          microtextureregionxmins[mtnum] = x;
        }
        if(y < microtextureregionymins[mtnum])
        {
          microtextureregionymins[mtnum] = y;
        }
        if(z < microtextureregionzmins[mtnum])
        {
          microtextureregionzmins[mtnum] = z;
        }
      }
    }
  }

  for(size_t i = 1; i < nummicrotextureregions; i++)
  {
    float xlength = (microtextureregionxmaxs[i] - microtextureregionxmins[i]) + xRes;
    float ylength = (microtextureregionymaxs[i] - microtextureregionymins[i]) + yRes;
    if(zPoints == 1)
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindMicroTextureRegions::newFilterInstance(bool copyFilterParameters)
{
  FindMicroTextureRegions::Pointer filter = FindMicroTextureRegions::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindMicroTextureRegions::getCompiledLibraryName()
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindMicroTextureRegions::getBrandingString()
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindMicroTextureRegions::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindMicroTextureRegions::getGroupName()
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindMicroTextureRegions::getSubGroupName()
{
  return SIMPL::FilterSubGroups::MorphologicalFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindMicroTextureRegions::getHumanLabel()
{
  return "Find MicroTexture Regions";
}
