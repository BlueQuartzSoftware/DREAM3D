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

#include "ExtractFlaggedFeatures.h"


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

#include "Sampling/SamplingFilters/CropVolume.h"

#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExtractFlaggedFeatures::ExtractFlaggedFeatures() :
  AbstractFilter(),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_FlaggedFeaturesArrayPath("", "" , ""),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_FlaggedFeaturesArrayName(""),
  m_FlaggedFeatures(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExtractFlaggedFeatures::~ExtractFlaggedFeatures()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractFlaggedFeatures::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("FeatureIds", "FeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeatureIdsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Flagged Features", "FlaggedFeaturesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFlaggedFeaturesArrayPath(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractFlaggedFeatures::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFlaggedFeaturesArrayPath(reader->readDataArrayPath("FlaggedFeaturesArrayPath", getFlaggedFeaturesArrayPath() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ExtractFlaggedFeatures::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(FlaggedFeaturesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractFlaggedFeatures::dataCheck()
{
  setErrorCondition(0);

  QVector<size_t> dims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() < 0) { return; }

  ImageGeom::Pointer image = getDataContainerArray()->getDataContainer(getFeatureIdsArrayPath().getDataContainerName())->getPrereqGeometry<ImageGeom, AbstractFilter>(this);
  if(getErrorCondition() < 0 || NULL == image.get()) { return; }

  m_FlaggedFeaturesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getFlaggedFeaturesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FlaggedFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FlaggedFeatures = m_FlaggedFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractFlaggedFeatures::preflight()
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
void ExtractFlaggedFeatures::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  size_t totalFeatures = m_FlaggedFeaturesPtr.lock()->getNumberOfTuples();

  find_feature_bounds();

  QString newDCName = "";
  CropVolume::Pointer cropVol = CropVolume::New();
  for(size_t i = 1; i < totalFeatures; i++)
  {
    if(m_FlaggedFeatures[i] == true)
    {
      newDCName.clear();
      newDCName = "Feature_" + QString::number(i);
      cropVol->setDataContainerArray(getDataContainerArray());
      cropVol->setNewDataContainerName(newDCName);
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

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Minimum Size Filter Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractFlaggedFeatures::find_feature_bounds()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  //int64_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  int64_t totalFeatures = m_FlaggedFeaturesPtr.lock()->getNumberOfTuples();
  size_t udims[3] = {0, 0, 0};
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);
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

  QVector<size_t> cDims(1, 6);
  boundsPtr = Int32ArrayType::CreateArray(totalFeatures, cDims, "Bounds");
  m_FeatureBounds = boundsPtr->getPointer(0);
  boundsPtr->initializeWithValue(-1);

  size_t kstride, jstride, count;
  int featureShift, feature;

  for (DimType k = 0; k < dims[2]; k++)
  {
    kstride = static_cast<int>( dims[0] * dims[1] * k );
    for (DimType j = 0; j < dims[1]; j++)
    {
      jstride = static_cast<int>( dims[0] * j );
      for (DimType i = 0; i < dims[0]; i++)
      {
        count = kstride + jstride + i;
        feature = m_FeatureIds[count];
        featureShift = 6 * feature;
        if(m_FeatureBounds[featureShift] == -1 || m_FeatureBounds[featureShift] > i)  { m_FeatureBounds[featureShift] = i; }
        if(m_FeatureBounds[featureShift + 1] == -1 || m_FeatureBounds[featureShift + 1] < i)  { m_FeatureBounds[featureShift + 1] = i; }
        if(m_FeatureBounds[featureShift + 2] == -1 || m_FeatureBounds[featureShift + 2] > j)  { m_FeatureBounds[featureShift + 2] = j; }
        if(m_FeatureBounds[featureShift + 3] == -1 || m_FeatureBounds[featureShift + 3] < j)  { m_FeatureBounds[featureShift + 3] = j; }
        if(m_FeatureBounds[featureShift + 4] == -1 || m_FeatureBounds[featureShift + 4] > k)  { m_FeatureBounds[featureShift + 4] = k; }
        if(m_FeatureBounds[featureShift + 5] == -1 || m_FeatureBounds[featureShift + 5] < k)  { m_FeatureBounds[featureShift + 5] = k; }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ExtractFlaggedFeatures::newFilterInstance(bool copyFilterParameters)
{
  ExtractFlaggedFeatures::Pointer filter = ExtractFlaggedFeatures::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExtractFlaggedFeatures::getCompiledLibraryName()
{ return Sampling::SamplingBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExtractFlaggedFeatures::getGroupName()
{ return DREAM3D::FilterGroups::SamplingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExtractFlaggedFeatures::getSubGroupName()
{ return DREAM3D::FilterSubGroups::CropCutFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExtractFlaggedFeatures::getHumanLabel()
{ return "Extract Flagged Features (Rogues Gallery)"; }

