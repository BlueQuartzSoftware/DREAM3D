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

#include <memory>

#include "FindMicroTextureRegions.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindMicroTextureRegions::FindMicroTextureRegions()
: m_CellFeatureAttributeMatrixName(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, "")
, m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds)
, m_MicroTextureRegionNumCellsArrayName("MicroTextureRegionNumCells")
, m_MicroTextureRegionFractionOccupiedArrayName("MicroTextureRegionFractionOccupied")
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindMicroTextureRegions::~FindMicroTextureRegions() = default;
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindMicroTextureRegions::setupFilterParameters()
{
  FilterParameterVectorType parameters;

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
  clearErrorCode();
  clearWarningCode();

  std::vector<size_t> dims(1, 1);
  // Cell Data
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(),
                                                                                                        dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock())                                                                        /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() < 0)
  {
    return;
  }

  ImageGeom::Pointer image = getDataContainerArray()->getDataContainer(getFeatureIdsArrayPath().getDataContainerName())->getPrereqGeometry<ImageGeom, AbstractFilter>(this);
  if(getErrorCode() < 0 || nullptr == image.get())
  {
    return;
  }

  // Feature Data
  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getMicroTextureRegionNumCellsArrayName());
  m_MicroTextureRegionNumCellsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, 0, dims);                               /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_MicroTextureRegionNumCellsPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_MicroTextureRegionNumCells = m_MicroTextureRegionNumCellsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getMicroTextureRegionFractionOccupiedArrayName());
  m_MicroTextureRegionFractionOccupiedPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
      this, tempPath, 0, dims);                                       /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_MicroTextureRegionFractionOccupiedPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
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
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
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
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  size_t nummicrotextureregions = m_MicroTextureRegionNumCellsPtr.lock()->getNumberOfTuples();

  DataArray<double>::Pointer m_MicroTextureRegionCounts = DataArray<double>::CreateArray(nummicrotextureregions, "MicroTextureRegionCounts", true);
  DataArray<double>::Pointer m_MicroTextureRegionXMaxs = DataArray<double>::CreateArray(nummicrotextureregions, "MicroTextureRegionCounts", true);
  DataArray<double>::Pointer m_MicroTextureRegionYMaxs = DataArray<double>::CreateArray(nummicrotextureregions, "MicroTextureRegionCounts", true);
  DataArray<double>::Pointer m_MicroTextureRegionZMaxs = DataArray<double>::CreateArray(nummicrotextureregions, "MicroTextureRegionCounts", true);
  DataArray<double>::Pointer m_MicroTextureRegionXMins = DataArray<double>::CreateArray(nummicrotextureregions, "MicroTextureRegionCounts", true);
  DataArray<double>::Pointer m_MicroTextureRegionYMins = DataArray<double>::CreateArray(nummicrotextureregions, "MicroTextureRegionCounts", true);
  DataArray<double>::Pointer m_MicroTextureRegionZMins = DataArray<double>::CreateArray(nummicrotextureregions, "MicroTextureRegionCounts", true);

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
  FloatVec3Type spacing = m->getGeometryAs<ImageGeom>()->getSpacing();

  // Initialize every element to 0.0 or max Dimension
  for(size_t i = 0; i < nummicrotextureregions * 1; i++)
  {
    microtextureregioncounts[i] = 0.0f;
    microtextureregionxmins[i] = spacing[0] * static_cast<float>(xPoints);
    microtextureregionxmaxs[i] = 0.0f;
    microtextureregionymins[i] = spacing[1] * static_cast<float>(yPoints);
    microtextureregionymaxs[i] = 0.0f;
    microtextureregionzmins[i] = spacing[2] * static_cast<float>(zPoints);
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
        x = float(k) * spacing[0];
        y = float(j) * spacing[1];
        z = float(i) * spacing[2];
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
    float xlength = (microtextureregionxmaxs[i] - microtextureregionxmins[i]) + spacing[0];
    float ylength = (microtextureregionymaxs[i] - microtextureregionymins[i]) + spacing[1];
    if(zPoints == 1)
    {
      float zlength = (microtextureregionzmaxs[i] - microtextureregionzmins[i]) + spacing[2];
      float prismvolume = xlength * ylength * zlength;
      m_MicroTextureRegionFractionOccupied[i] = ((float)m_MicroTextureRegionNumCells[i] * spacing[0] * spacing[1] * spacing[2]) / prismvolume;
    }
    else
    {
      float rectanglevolume = xlength * ylength;
      m_MicroTextureRegionFractionOccupied[i] = ((float)m_MicroTextureRegionNumCells[i] * spacing[0] * spacing[1]) / rectanglevolume;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindMicroTextureRegions::newFilterInstance(bool copyFilterParameters) const
{
  FindMicroTextureRegions::Pointer filter = FindMicroTextureRegions::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindMicroTextureRegions::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindMicroTextureRegions::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindMicroTextureRegions::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindMicroTextureRegions::getGroupName() const
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid FindMicroTextureRegions::getUuid() const
{
  return QUuid("{90f8e3b1-2460-5862-95a1-a9e06f5ee75e}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindMicroTextureRegions::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MorphologicalFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindMicroTextureRegions::getHumanLabel() const
{
  return "Find MicroTexture Regions";
}

// -----------------------------------------------------------------------------
FindMicroTextureRegions::Pointer FindMicroTextureRegions::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<FindMicroTextureRegions> FindMicroTextureRegions::New()
{
  struct make_shared_enabler : public FindMicroTextureRegions
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString FindMicroTextureRegions::getNameOfClass() const
{
  return QString("FindMicroTextureRegions");
}

// -----------------------------------------------------------------------------
QString FindMicroTextureRegions::ClassName()
{
  return QString("FindMicroTextureRegions");
}

// -----------------------------------------------------------------------------
void FindMicroTextureRegions::setCellFeatureAttributeMatrixName(const DataArrayPath& value)
{
  m_CellFeatureAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindMicroTextureRegions::getCellFeatureAttributeMatrixName() const
{
  return m_CellFeatureAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void FindMicroTextureRegions::setFeatureIdsArrayPath(const DataArrayPath& value)
{
  m_FeatureIdsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindMicroTextureRegions::getFeatureIdsArrayPath() const
{
  return m_FeatureIdsArrayPath;
}

// -----------------------------------------------------------------------------
void FindMicroTextureRegions::setMicroTextureRegionNumCellsArrayName(const QString& value)
{
  m_MicroTextureRegionNumCellsArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindMicroTextureRegions::getMicroTextureRegionNumCellsArrayName() const
{
  return m_MicroTextureRegionNumCellsArrayName;
}

// -----------------------------------------------------------------------------
void FindMicroTextureRegions::setMicroTextureRegionFractionOccupiedArrayName(const QString& value)
{
  m_MicroTextureRegionFractionOccupiedArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindMicroTextureRegions::getMicroTextureRegionFractionOccupiedArrayName() const
{
  return m_MicroTextureRegionFractionOccupiedArrayName;
}
