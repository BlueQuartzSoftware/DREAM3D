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

#include "FindLargestCrossSections.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "Statistics/StatisticsConstants.h"
#include "Statistics/StatisticsVersion.h"

// Include the MOC generated file for this class
#include "moc_FindLargestCrossSections.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindLargestCrossSections::FindLargestCrossSections() :
  AbstractFilter(),
  m_Plane(0),
  m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds),
  m_LargestCrossSectionsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::LargestCrossSections),
  m_FeatureIds(NULL),
  m_LargestCrossSections(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindLargestCrossSections::~FindLargestCrossSections()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLargestCrossSections::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Plane of Interest");
    parameter->setPropertyName("Plane");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(FindLargestCrossSections, this, Plane));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(FindLargestCrossSections, this, Plane));

    QVector<QString> choices;
    choices.push_back("XY");
    choices.push_back("XZ");
    choices.push_back("YZ");
    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, SIMPL::AttributeMatrixType::Cell, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, FindLargestCrossSections, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::CreatedArray));
  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(SIMPL::AttributeMatrixType::CellFeature, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Largest Cross Sections", LargestCrossSectionsArrayPath, FilterParameter::CreatedArray, FindLargestCrossSections, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void FindLargestCrossSections::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setLargestCrossSectionsArrayPath(reader->readDataArrayPath("LargestCrossSectionsArrayPath", getLargestCrossSectionsArrayPath()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setPlane( reader->readValue("Plane", getPlane()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLargestCrossSections::initialize()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLargestCrossSections::dataCheck()
{
  setErrorCondition(0);

  QVector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_LargestCrossSectionsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, getLargestCrossSectionsArrayPath(), 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_LargestCrossSectionsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_LargestCrossSections = m_LargestCrossSectionsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  ImageGeom::Pointer image = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());
  if(getErrorCondition() < 0) { return; }

  size_t dims[3] = { 0, 0, 0 };
  image->getDimensions(dims);

  if (dims[0] <= 1 || dims[1] <= 1 || dims[2] <= 1)
  {
    QString ss = QObject::tr("Image Geometry is not 3D.  The dimensions are (%1, %2, %3)").arg(dims[0]).arg(dims[1]).arg(dims[2]);
    setErrorCondition(-3012);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLargestCrossSections::preflight()
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
void FindLargestCrossSections::find_crosssections()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  size_t numfeatures = m_LargestCrossSectionsPtr.lock()->getNumberOfTuples();

  DoubleArrayType::Pointer m_FeatureCounts = DoubleArrayType::CreateArray(numfeatures, "_INTERNAL_USE_ONLY_FeatureCounts");
  double* featurecounts = m_FeatureCounts->getPointer(0);

  size_t outPlane = 0, inPlane1 = 0, inPlane2 = 0;
  float res_scalar = 0.0f, area = 0.0f;
  size_t stride1 = 0, stride2 = 0, stride3 = 0;
  size_t istride = 0, jstride = 0, kstride = 0;
  size_t point = 0, gnum = 0;

  if (m_Plane == 0)
  {
    outPlane = m->getGeometryAs<ImageGeom>()->getZPoints();
    inPlane1 = m->getGeometryAs<ImageGeom>()->getXPoints();
    inPlane2 = m->getGeometryAs<ImageGeom>()->getYPoints();
    res_scalar = m->getGeometryAs<ImageGeom>()->getXRes() * m->getGeometryAs<ImageGeom>()->getYRes();
    stride1 = inPlane1 * inPlane2;
    stride2 = 1;
    stride3 = inPlane1;
  }
  if (m_Plane == 1)
  {
    outPlane = m->getGeometryAs<ImageGeom>()->getYPoints();
    inPlane1 = m->getGeometryAs<ImageGeom>()->getXPoints();
    inPlane2 = m->getGeometryAs<ImageGeom>()->getZPoints();
    res_scalar = m->getGeometryAs<ImageGeom>()->getXRes() * m->getGeometryAs<ImageGeom>()->getZRes();
    stride1 = inPlane1;
    stride2 = 1;
    stride3 = inPlane1 * inPlane2;
  }
  if (m_Plane == 2)
  {
    outPlane = m->getGeometryAs<ImageGeom>()->getXPoints();
    inPlane1 = m->getGeometryAs<ImageGeom>()->getYPoints();
    inPlane2 = m->getGeometryAs<ImageGeom>()->getZPoints();
    res_scalar = m->getGeometryAs<ImageGeom>()->getYRes() * m->getGeometryAs<ImageGeom>()->getZRes();
    stride1 = 1;
    stride2 = inPlane1;
    stride3 = inPlane1 * inPlane2;
  }
  for (size_t i = 0; i < outPlane; i++)
  {
    for (size_t g = 0; g < numfeatures * 1; g++)
    {
      featurecounts[g] = 0.0f;
    }
    istride = i * stride1;
    for (size_t j = 0; j < inPlane1; j++)
    {
      jstride = j * stride2;
      for (size_t k = 0; k < inPlane2; k++)
      {
        kstride = k * stride3;
        point = istride + jstride + kstride;
        gnum = m_FeatureIds[point];
        featurecounts[gnum]++;
      }
    }
    for (size_t g = 1; g < numfeatures; g++)
    {
      area = featurecounts[g] * res_scalar;
      if (area > m_LargestCrossSections[g]) { m_LargestCrossSections[g] = area; }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLargestCrossSections::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  find_crosssections();

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindLargestCrossSections::newFilterInstance(bool copyFilterParameters)
{
  FindLargestCrossSections::Pointer filter = FindLargestCrossSections::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindLargestCrossSections::getCompiledLibraryName()
{
  return StatisticsConstants::StatisticsBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindLargestCrossSections::getBrandingString()
{
  return "Statistics";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindLargestCrossSections::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  Statistics::Version::Major() << "." << Statistics::Version::Minor() << "." << Statistics::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindLargestCrossSections::getGroupName()
{ return SIMPL::FilterGroups::StatisticsFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindLargestCrossSections::getSubGroupName()
{ return SIMPL::FilterSubGroups::MorphologicalFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindLargestCrossSections::getHumanLabel()
{ return "Find Feature Largest Cross-Section Areas"; }
