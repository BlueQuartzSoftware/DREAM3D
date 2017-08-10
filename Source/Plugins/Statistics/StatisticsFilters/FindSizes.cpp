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

#include "FindSizes.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Math/SIMPLibMath.h"

#include "Statistics/StatisticsConstants.h"
#include "Statistics/StatisticsVersion.h"

// Include the MOC generated file for this class
#include "moc_FindSizes.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSizes::FindSizes()
  : AbstractFilter()
  , m_FeatureAttributeMatrixName("", "", "")
  , m_FeatureIdsArrayPath("", "", SIMPL::CellData::FeatureIds)
  , m_VolumesArrayName(SIMPL::FeatureData::Volumes)
  , m_EquivalentDiametersArrayName(SIMPL::FeatureData::EquivalentDiameters)
  , m_NumElementsArrayName(SIMPL::FeatureData::NumElements)
  , m_SaveElementSizes(false)
  , m_FeatureIds(nullptr)
  , m_Volumes(nullptr)
  , m_EquivalentDiameters(nullptr)
  , m_NumElements(nullptr)
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
  FilterParameterVector parameters;
  parameters.push_back(SIMPL_NEW_BOOL_FP("Save Element Sizes", SaveElementSizes, FilterParameter::Parameter, FindSizes));
  parameters.push_back(SeparatorFilterParameter::New("Element Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Any, IGeometry::Type::Any);
    AttributeMatrix::Types amTypes = { AttributeMatrix::Type::Vertex, AttributeMatrix::Type::Edge,
                                  AttributeMatrix::Type::Face, AttributeMatrix::Type::Cell };
    req.amTypes = amTypes;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, FindSizes, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Feature Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req =
        AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::Any, IGeometry::Type::Any);
     AttributeMatrix::Types amTypes = { AttributeMatrix::Type::VertexFeature, AttributeMatrix::Type::EdgeFeature,
                                  AttributeMatrix::Type::FaceFeature, AttributeMatrix::Type::CellFeature };
    req.amTypes = amTypes;
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Feature Attribute Matrix", FeatureAttributeMatrixName, FilterParameter::RequiredArray, FindSizes, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Equivalent Diameters", EquivalentDiametersArrayName, FilterParameter::CreatedArray, FindSizes));
  parameters.push_back(SIMPL_NEW_STRING_FP("Number of Elements", NumElementsArrayName, FilterParameter::CreatedArray, FindSizes));
  parameters.push_back(SIMPL_NEW_STRING_FP("Volumes", VolumesArrayName, FilterParameter::CreatedArray, FindSizes));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFeatureAttributeMatrixName(reader->readDataArrayPath("CellFeatureAttributeMatrixName", getFeatureAttributeMatrixName())); // Note the String name differs, this is for historical reasons
  setNumElementsArrayName(reader->readString("NumCellsArrayName", getNumElementsArrayName())); // Note the String name differs, this is for historical reasons
  setEquivalentDiametersArrayName(reader->readString("EquivalentDiametersArrayName", getEquivalentDiametersArrayName()));
  setVolumesArrayName(reader->readString("VolumesArrayName", getVolumesArrayName()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  // SaveElementSizes was introduced AFTER 6.3.29 and would ONLY show up in the JSON based pipelines. This
  // method should only be reading from an HDF5 based pipeline.
  //setSaveElementSizes(reader->readValue("SaveElementSizes", getSaveElementSizes()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);
  DataArrayPath tempPath;

  QVector<IDataArray::Pointer> dataArrays;

  IGeometry::Pointer igeom = getDataContainerArray()->getPrereqGeometryFromDataContainer<IGeometry, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

  QVector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(),
                                                                                                        cDims);
  if(nullptr != m_FeatureIdsPtr.lock().get())
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  }

  tempPath.update(getFeatureAttributeMatrixName().getDataContainerName(), getFeatureAttributeMatrixName().getAttributeMatrixName(), getVolumesArrayName());
  m_VolumesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0,
                                                                                                                cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_VolumesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Volumes = m_VolumesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getFeatureAttributeMatrixName().getDataContainerName(), getFeatureAttributeMatrixName().getAttributeMatrixName(), getEquivalentDiametersArrayName());
  m_EquivalentDiametersPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
        this, tempPath, 0, cDims);                       /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_EquivalentDiametersPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_EquivalentDiameters = m_EquivalentDiametersPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getFeatureAttributeMatrixName().getDataContainerName(), getFeatureAttributeMatrixName().getAttributeMatrixName(), getNumElementsArrayName());
  m_NumElementsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0,
                                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_NumElementsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_NumElements = m_NumElementsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(getErrorCondition() < 0) { return; }

  size_t numElements = igeom->getNumberOfElements();

  if(m_FeatureIdsPtr.lock()->getNumberOfTuples() != numElements)
  {
    setErrorCondition(-10200);
    QString ss = QObject::tr("The number of tuples for the DataArray %1 is %2 and for Geometry %3 is %4. The number of tuples must match.")
        .arg(m_FeatureIdsPtr.lock()->getName()).arg(m_FeatureIdsPtr.lock()->getNumberOfTuples()).arg(igeom->getGeometryTypeAsString()).arg(numElements);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::preflight()
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
void FindSizes::findSizesImage(ImageGeom::Pointer image)
{
  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  size_t numfeatures = m_VolumesPtr.lock()->getNumberOfTuples();

  DataArray<float>::Pointer m_FeatureCounts = DataArray<float>::CreateArray(numfeatures, "_INTERNAL_USE_ONLY_FeatureCounts");
  m_FeatureCounts->initializeWithZeros();
  float* featurecounts = m_FeatureCounts->getPointer(0);

  float rad = 0.0f;
  float diameter = 0.0f;
  float res_scalar = 0.0f;

  for(size_t j = 0; j < totalPoints; j++)
  {
    int32_t gnum = m_FeatureIds[j];
    featurecounts[gnum]++;
  }

  if(image->getXPoints() == 1 || image->getYPoints() == 1 || image->getZPoints() == 1)
  {
    if(image->getXPoints() == 1)
    {
      res_scalar = image->getYRes() * image->getZRes();
    }
    else if(image->getYPoints() == 1)
    {
      res_scalar = image->getXRes() * image->getZRes();
    }
    else if(image->getZPoints() == 1)
    {
      res_scalar = image->getXRes() * image->getYRes();
    }
    for(size_t i = 1; i < numfeatures; i++)
    {
      m_NumElements[i] = static_cast<int32_t>(featurecounts[i]);
      m_Volumes[i] = (featurecounts[i] * res_scalar);
      rad = m_Volumes[i] / SIMPLib::Constants::k_Pi;
      diameter = (2 * sqrtf(rad));
      m_EquivalentDiameters[i] = diameter;
    }
  }
  else
  {
    res_scalar = image->getXRes() * image->getYRes() * image->getZRes();
    float vol_term = (4.0f / 3.0f) * SIMPLib::Constants::k_Pi;
    for(size_t i = 1; i < numfeatures; i++)
    {
      m_NumElements[i] = static_cast<int32_t>(featurecounts[i]);
      m_Volumes[i] = (featurecounts[i] * res_scalar);
      rad = m_Volumes[i] / vol_term;
      diameter = 2.0f * powf(rad, 0.3333333333f);
      m_EquivalentDiameters[i] = diameter;
    }
  }

  if(m_SaveElementSizes)
  {
    if(!image->getElementSizes())
    {
      int32_t err = image->findElementSizes();
      if(err < 0)
      {
        setErrorCondition(err);
        QString ss = QObject::tr("Error computing Element sizes for Geometry type %1").arg(image->getGeometryTypeAsString());
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::findSizesUnstructured(IGeometry::Pointer igeom)
{
  if(!igeom->getElementSizes())
  {
    int32_t err = igeom->findElementSizes();
    if(err < 0)
    {
      setErrorCondition(err);
      QString ss = QObject::tr("Error computing Element sizes for Geometry type %1").arg(igeom->getGeometryTypeAsString());
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
  }

  FloatArrayType::Pointer elemSizes = igeom->getElementSizes();
  float* sizes = elemSizes->getPointer(0);

  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  size_t numfeatures = m_VolumesPtr.lock()->getNumberOfTuples();

  DataArray<float>::Pointer m_FeatureCounts = DataArray<float>::CreateArray(numfeatures, "_INTERNAL_USE_ONLY_FeatureCounts");
  m_FeatureCounts->initializeWithZeros();
  float* featurecounts = m_FeatureCounts->getPointer(0);

  float rad = 0.0f;
  float diameter = 0.0f;

  for(size_t j = 0; j < totalPoints; j++)
  {
    int32_t gnum = m_FeatureIds[j];
    featurecounts[gnum]++;
    m_Volumes[gnum] += sizes[j];
  }
  float vol_term = (4.0f / 3.0f) * SIMPLib::Constants::k_Pif;
  for(size_t i = 1; i < numfeatures; i++)
  {
    m_NumElements[i] = static_cast<int32_t>(featurecounts[i]);
    rad = m_Volumes[i] / vol_term;
    diameter = 2.0f * powf(rad, 0.3333333333f);
    m_EquivalentDiameters[i] = diameter;
  }

  if(!m_SaveElementSizes)
  {
    igeom->deleteElementSizes();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::findSizes(IGeometry::Pointer igeom)
{
  // If the geometry is an Image, it may be 2D;
  // if so, call the specialized findSizesImage() function
  if(std::dynamic_pointer_cast<ImageGeom>(igeom))
  {
    ImageGeom::Pointer image = std::dynamic_pointer_cast<ImageGeom>(igeom);

    findSizesImage(image);
  }
  else
  {
    findSizesUnstructured(igeom);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSizes::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());
  IGeometry::Pointer igeom = m->getGeometry();

  findSizes(igeom);

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindSizes::newFilterInstance(bool copyFilterParameters)
{
  FindSizes::Pointer filter = FindSizes::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSizes::getCompiledLibraryName()
{
  return StatisticsConstants::StatisticsBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSizes::getBrandingString()
{
  return "Statistics";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSizes::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << Statistics::Version::Major() << "." << Statistics::Version::Minor() << "." << Statistics::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSizes::getGroupName()
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSizes::getSubGroupName()
{
  return SIMPL::FilterSubGroups::MorphologicalFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSizes::getHumanLabel()
{
  return "Find Feature Sizes";
}
