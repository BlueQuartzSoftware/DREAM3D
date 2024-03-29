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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "ExtractFlaggedFeatures.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "Sampling/SamplingConstants.h"
#include "Sampling/SamplingFilters/CropImageGeometry.h"
#include "Sampling/SamplingVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExtractFlaggedFeatures::ExtractFlaggedFeatures() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExtractFlaggedFeatures::~ExtractFlaggedFeatures() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractFlaggedFeatures::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SeparatorFilterParameter::Create("Cell Data", FilterParameter::Category::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::Category::RequiredArray, ExtractFlaggedFeatures, req));
  }
  parameters.push_back(SeparatorFilterParameter::Create("Cell Feature Data", FilterParameter::Category::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Bool, 1, AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);

    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Flagged Features", FlaggedFeaturesArrayPath, FilterParameter::Category::RequiredArray, ExtractFlaggedFeatures, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractFlaggedFeatures::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFlaggedFeaturesArrayPath(reader->readDataArrayPath("FlaggedFeaturesArrayPath", getFlaggedFeaturesArrayPath()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractFlaggedFeatures::initialize()
{
  m_BoundsPtr = Int32ArrayType::NullPointer();
  m_FeatureBounds = nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractFlaggedFeatures::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom>(this, getFeatureIdsArrayPath().getDataContainerName());

  std::vector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>>(this, getFeatureIdsArrayPath(), cDims);
  if(nullptr != m_FeatureIdsPtr.lock())
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_FlaggedFeaturesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>>(this, getFlaggedFeaturesArrayPath(), cDims);
  if(nullptr != m_FlaggedFeaturesPtr.lock())
  {
    m_FlaggedFeatures = m_FlaggedFeaturesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractFlaggedFeatures::find_feature_bounds()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  size_t totalFeatures = m_FlaggedFeaturesPtr.lock()->getNumberOfTuples();
  SizeVec3Type udims = m->getGeometryAs<ImageGeom>()->getDimensions();

  int64_t dims[3] = {
      static_cast<int64_t>(udims[0]),
      static_cast<int64_t>(udims[1]),
      static_cast<int64_t>(udims[2]),
  };

  std::vector<size_t> cDims(1, 6);
  m_BoundsPtr = Int32ArrayType::CreateArray(totalFeatures, cDims, "_INTERNAL_USE_ONLY_Bounds", true);
  m_FeatureBounds = m_BoundsPtr->getPointer(0);
  m_BoundsPtr->initializeWithValue(-1);

  int64_t kstride = 0, jstride = 0, count = 0;
  int64_t featureShift = 0;
  int32_t feature = 0;

  for(int64_t k = 0; k < dims[2]; k++)
  {
    kstride = dims[0] * dims[1] * k;
    for(int64_t j = 0; j < dims[1]; j++)
    {
      jstride = dims[0] * j;
      for(int64_t i = 0; i < dims[0]; i++)
      {
        count = kstride + jstride + i;
        feature = m_FeatureIds[count];
        featureShift = 6 * feature;
        if(m_FeatureBounds[featureShift] == -1 || m_FeatureBounds[featureShift] > i)
        {
          m_FeatureBounds[featureShift] = i;
        }
        if(m_FeatureBounds[featureShift + 1] == -1 || m_FeatureBounds[featureShift + 1] < i)
        {
          m_FeatureBounds[featureShift + 1] = i;
        }
        if(m_FeatureBounds[featureShift + 2] == -1 || m_FeatureBounds[featureShift + 2] > j)
        {
          m_FeatureBounds[featureShift + 2] = j;
        }
        if(m_FeatureBounds[featureShift + 3] == -1 || m_FeatureBounds[featureShift + 3] < j)
        {
          m_FeatureBounds[featureShift + 3] = j;
        }
        if(m_FeatureBounds[featureShift + 4] == -1 || m_FeatureBounds[featureShift + 4] > k)
        {
          m_FeatureBounds[featureShift + 4] = k;
        }
        if(m_FeatureBounds[featureShift + 5] == -1 || m_FeatureBounds[featureShift + 5] < k)
        {
          m_FeatureBounds[featureShift + 5] = k;
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractFlaggedFeatures::execute()
{
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  size_t totalFeatures = m_FlaggedFeaturesPtr.lock()->getNumberOfTuples();

  find_feature_bounds();

  QString newDCName = "";
  CropImageGeometry::Pointer cropVol = CropImageGeometry::New();
  for(size_t i = 1; i < totalFeatures; i++)
  {
    if(m_FlaggedFeatures[i])
    {
      newDCName.clear();
      newDCName = "Feature_" + QString::number(i);
      cropVol->setDataContainerArray(getDataContainerArray());
      cropVol->setNewDataContainerName(DataArrayPath(newDCName, "", ""));
      cropVol->setCellAttributeMatrixPath(m_FeatureIdsArrayPath);
      cropVol->setXMin(m_FeatureBounds[6 * i]);
      cropVol->setXMax(m_FeatureBounds[6 * i + 1]);
      cropVol->setYMin(m_FeatureBounds[6 * i + 2]);
      cropVol->setYMax(m_FeatureBounds[6 * i + 3]);
      cropVol->setZMin(m_FeatureBounds[6 * i + 4]);
      cropVol->setZMax(m_FeatureBounds[6 * i + 5]);
      cropVol->setRenumberFeatures(false);
      cropVol->setSaveAsNewDataContainer(true);
      cropVol->setUpdateOrigin(true);
      cropVol->execute();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ExtractFlaggedFeatures::newFilterInstance(bool copyFilterParameters) const
{
  ExtractFlaggedFeatures::Pointer filter = ExtractFlaggedFeatures::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExtractFlaggedFeatures::getCompiledLibraryName() const
{
  return SamplingConstants::SamplingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExtractFlaggedFeatures::getBrandingString() const
{
  return "Sampling";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExtractFlaggedFeatures::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Sampling::Version::Major() << "." << Sampling::Version::Minor() << "." << Sampling::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExtractFlaggedFeatures::getGroupName() const
{
  return SIMPL::FilterGroups::SamplingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid ExtractFlaggedFeatures::getUuid() const
{
  return QUuid("{e0555de5-bdc6-5bea-ba2f-aacfbec0a022}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExtractFlaggedFeatures::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::CropCutFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExtractFlaggedFeatures::getHumanLabel() const
{
  return "Extract Flagged Features (Rogues Gallery)";
}

// -----------------------------------------------------------------------------
ExtractFlaggedFeatures::Pointer ExtractFlaggedFeatures::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ExtractFlaggedFeatures> ExtractFlaggedFeatures::New()
{
  struct make_shared_enabler : public ExtractFlaggedFeatures
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ExtractFlaggedFeatures::getNameOfClass() const
{
  return QString("ExtractFlaggedFeatures");
}

// -----------------------------------------------------------------------------
QString ExtractFlaggedFeatures::ClassName()
{
  return QString("ExtractFlaggedFeatures");
}

// -----------------------------------------------------------------------------
void ExtractFlaggedFeatures::setFeatureIdsArrayPath(const DataArrayPath& value)
{
  m_FeatureIdsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ExtractFlaggedFeatures::getFeatureIdsArrayPath() const
{
  return m_FeatureIdsArrayPath;
}

// -----------------------------------------------------------------------------
void ExtractFlaggedFeatures::setFlaggedFeaturesArrayPath(const DataArrayPath& value)
{
  m_FlaggedFeaturesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ExtractFlaggedFeatures::getFlaggedFeaturesArrayPath() const
{
  return m_FlaggedFeaturesArrayPath;
}
