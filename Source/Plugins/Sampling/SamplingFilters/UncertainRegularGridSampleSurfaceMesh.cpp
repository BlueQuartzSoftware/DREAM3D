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

#include "UncertainRegularGridSampleSurfaceMesh.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/DataArrays/DynamicListArray.hpp"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/Math/GeometryMath.h"
#include "SIMPLib/Math/SIMPLibRandom.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "Sampling/SamplingConstants.h"
#include "Sampling/SamplingVersion.h"

enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID21 = 21,

  DataContainerID = 1
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UncertainRegularGridSampleSurfaceMesh::UncertainRegularGridSampleSurfaceMesh()
: m_DataContainerName(SIMPL::Defaults::ImageDataContainerName)
, m_CellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName)
, m_XPoints(0)
, m_YPoints(0)
, m_ZPoints(0)
, m_FeatureIdsArrayName(SIMPL::CellData::FeatureIds)
{
  m_Spacing[0] = 1.0f;
  m_Spacing[1] = 1.0f;
  m_Spacing[2] = 1.0f;

  m_Origin[0] = 0.0f;
  m_Origin[1] = 0.0f;
  m_Origin[2] = 0.0f;
  m_Uncertainty[0] = 0.1f;
  m_Uncertainty[1] = 0.1f;
  m_Uncertainty[2] = 0.1f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UncertainRegularGridSampleSurfaceMesh::~UncertainRegularGridSampleSurfaceMesh() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UncertainRegularGridSampleSurfaceMesh::setupFilterParameters()
{
  SampleSurfaceMesh::setupFilterParameters();
  FilterParameterVectorType parameters = getFilterParameters();
  parameters.push_back(SIMPL_NEW_INTEGER_FP("X Points", XPoints, FilterParameter::Parameter, UncertainRegularGridSampleSurfaceMesh));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Y Points", YPoints, FilterParameter::Parameter, UncertainRegularGridSampleSurfaceMesh));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Z Points", ZPoints, FilterParameter::Parameter, UncertainRegularGridSampleSurfaceMesh));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Spacing", Spacing, FilterParameter::Parameter, UncertainRegularGridSampleSurfaceMesh));

  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Origin", Origin, FilterParameter::Parameter, UncertainRegularGridSampleSurfaceMesh));

  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Uncertainty", Uncertainty, FilterParameter::Parameter, UncertainRegularGridSampleSurfaceMesh));

  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Data Container", DataContainerName, FilterParameter::CreatedArray, UncertainRegularGridSampleSurfaceMesh));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_AM_WITH_LINKED_DC_FP("Cell Attribute Matrix", CellAttributeMatrixName, DataContainerName, FilterParameter::CreatedArray, UncertainRegularGridSampleSurfaceMesh));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Feature Ids", FeatureIdsArrayName, DataContainerName, CellAttributeMatrixName, FilterParameter::CreatedArray, UncertainRegularGridSampleSurfaceMesh));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UncertainRegularGridSampleSurfaceMesh::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  SampleSurfaceMesh::readFilterParameters(reader, index);
  reader->openFilterGroup(this, index);
  setXPoints(reader->readValue("XPoints", getXPoints()));
  setYPoints(reader->readValue("YPoints", getYPoints()));
  setZPoints(reader->readValue("ZPoints", getZPoints()));
  setSpacing(reader->readFloatVec3("Spacing", getSpacing()));
  setOrigin(reader->readFloatVec3("Origin", getOrigin()));
  setUncertainty(reader->readFloatVec3("Uncertainty", getUncertainty()));
  setDataContainerName(reader->readDataArrayPath("DataContainerName", getDataContainerName()));
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName()));
  setFeatureIdsArrayName(reader->readString("FeatureIdsArrayName", getFeatureIdsArrayName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UncertainRegularGridSampleSurfaceMesh::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UncertainRegularGridSampleSurfaceMesh::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  DataArrayPath tempPath;

  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getDataContainerName(), DataContainerID);
  if(getErrorCode() < 0)
  {
    return;
  }

  ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
  m->setGeometry(image);

  // Set the Dimensions, Spacing and Origin of the output data container
  m->getGeometryAs<ImageGeom>()->setDimensions(m_XPoints, m_YPoints, m_ZPoints);
  m->getGeometryAs<ImageGeom>()->setSpacing(m_Spacing);
  m->getGeometryAs<ImageGeom>()->setOrigin(m_Origin);

  std::vector<size_t> tDims(3, 0);
  tDims[0] = m_XPoints;
  tDims[1] = m_YPoints;
  tDims[2] = m_ZPoints;

  AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix(this, getCellAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell, AttributeMatrixID21);
  if(getErrorCode() < 0 || nullptr == cellAttrMat.get())
  {
    return;
  }

  std::vector<size_t> cDims(1, 1);
  tempPath.update(getDataContainerName().getDataContainerName(), getCellAttributeMatrixName(), getFeatureIdsArrayName());
  m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, 0, cDims);              /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UncertainRegularGridSampleSurfaceMesh::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  SampleSurfaceMesh::preflight();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexGeom::Pointer UncertainRegularGridSampleSurfaceMesh::generate_points()
{
  VertexGeom::Pointer points = VertexGeom::CreateGeometry((m_XPoints * m_YPoints * m_ZPoints), "Points");

  SIMPL_RANDOMNG_NEW()

  int64_t count = 0;
  float coords[3] = {0.0f, 0.0f, 0.0f};
  for(int64_t k = 0; k < m_ZPoints; k++)
  {
    float randomZ = 2.0f * static_cast<float>(rg.genrand_res53()) - 1.0f;
    for(int64_t j = 0; j < m_YPoints; j++)
    {
      float randomY = 2.0f * static_cast<float>(rg.genrand_res53()) - 1.0f;
      for(int64_t i = 0; i < m_XPoints; i++)
      {
        float randomX = 2.0f * static_cast<float>(rg.genrand_res53()) - 1.0f;
        coords[0] = ((float(i) + 0.5f) * m_Spacing[0]) + (m_Uncertainty[0] * randomX) + m_Origin[0];
        coords[1] = ((float(j) + 0.5f) * m_Spacing[1]) + (m_Uncertainty[1] * randomY) + m_Origin[1];
        coords[2] = ((float(k) + 0.5f) * m_Spacing[2]) + (m_Uncertainty[2] * randomZ) + m_Origin[2];
        points->setCoords(count, coords);
        count++;
      }
    }
  }

  return points;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UncertainRegularGridSampleSurfaceMesh::assign_points(Int32ArrayType::Pointer iArray)
{
  int32_t* ids = iArray->getPointer(0);
  for(int64_t i = 0; i < (m_XPoints * m_YPoints * m_ZPoints); i++)
  {
    m_FeatureIds[i] = ids[i];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UncertainRegularGridSampleSurfaceMesh::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  SIMPL_RANDOMNG_NEW()

  m->getGeometryAs<ImageGeom>()->setDimensions(m_XPoints, m_YPoints, m_ZPoints);
  m->getGeometryAs<ImageGeom>()->setOrigin(m_Origin);
  m->getGeometryAs<ImageGeom>()->setSpacing(m_Spacing);

  SampleSurfaceMesh::execute();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer UncertainRegularGridSampleSurfaceMesh::newFilterInstance(bool copyFilterParameters) const
{
  UncertainRegularGridSampleSurfaceMesh::Pointer filter = UncertainRegularGridSampleSurfaceMesh::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString UncertainRegularGridSampleSurfaceMesh::getCompiledLibraryName() const
{
  return SamplingConstants::SamplingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString UncertainRegularGridSampleSurfaceMesh::getBrandingString() const
{
  return "Sampling";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString UncertainRegularGridSampleSurfaceMesh::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Sampling::Version::Major() << "." << Sampling::Version::Minor() << "." << Sampling::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString UncertainRegularGridSampleSurfaceMesh::getGroupName() const
{
  return SIMPL::FilterGroups::SamplingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid UncertainRegularGridSampleSurfaceMesh::getUuid() const
{
  return QUuid("{75cfeb9b-cd4b-5a20-a344-4170b39bbfaf}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString UncertainRegularGridSampleSurfaceMesh::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::ResolutionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString UncertainRegularGridSampleSurfaceMesh::getHumanLabel() const
{
  return "Sample Triangle Geometry on Uncertain Regular Grid";
}

// -----------------------------------------------------------------------------
UncertainRegularGridSampleSurfaceMesh::Pointer UncertainRegularGridSampleSurfaceMesh::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<UncertainRegularGridSampleSurfaceMesh> UncertainRegularGridSampleSurfaceMesh::New()
{
  struct make_shared_enabler : public UncertainRegularGridSampleSurfaceMesh
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString UncertainRegularGridSampleSurfaceMesh::getNameOfClass() const
{
  return QString("UncertainRegularGridSampleSurfaceMesh");
}

// -----------------------------------------------------------------------------
QString UncertainRegularGridSampleSurfaceMesh::ClassName()
{
  return QString("UncertainRegularGridSampleSurfaceMesh");
}

// -----------------------------------------------------------------------------
void UncertainRegularGridSampleSurfaceMesh::setDataContainerName(const DataArrayPath& value)
{
  m_DataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath UncertainRegularGridSampleSurfaceMesh::getDataContainerName() const
{
  return m_DataContainerName;
}

// -----------------------------------------------------------------------------
void UncertainRegularGridSampleSurfaceMesh::setCellAttributeMatrixName(const QString& value)
{
  m_CellAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString UncertainRegularGridSampleSurfaceMesh::getCellAttributeMatrixName() const
{
  return m_CellAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void UncertainRegularGridSampleSurfaceMesh::setXPoints(int value)
{
  m_XPoints = value;
}

// -----------------------------------------------------------------------------
int UncertainRegularGridSampleSurfaceMesh::getXPoints() const
{
  return m_XPoints;
}

// -----------------------------------------------------------------------------
void UncertainRegularGridSampleSurfaceMesh::setYPoints(int value)
{
  m_YPoints = value;
}

// -----------------------------------------------------------------------------
int UncertainRegularGridSampleSurfaceMesh::getYPoints() const
{
  return m_YPoints;
}

// -----------------------------------------------------------------------------
void UncertainRegularGridSampleSurfaceMesh::setZPoints(int value)
{
  m_ZPoints = value;
}

// -----------------------------------------------------------------------------
int UncertainRegularGridSampleSurfaceMesh::getZPoints() const
{
  return m_ZPoints;
}

// -----------------------------------------------------------------------------
void UncertainRegularGridSampleSurfaceMesh::setSpacing(const FloatVec3Type& value)
{
  m_Spacing = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type UncertainRegularGridSampleSurfaceMesh::getSpacing() const
{
  return m_Spacing;
}

// -----------------------------------------------------------------------------
void UncertainRegularGridSampleSurfaceMesh::setOrigin(const FloatVec3Type& value)
{
  m_Origin = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type UncertainRegularGridSampleSurfaceMesh::getOrigin() const
{
  return m_Origin;
}

// -----------------------------------------------------------------------------
void UncertainRegularGridSampleSurfaceMesh::setUncertainty(const FloatVec3Type& value)
{
  m_Uncertainty = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type UncertainRegularGridSampleSurfaceMesh::getUncertainty() const
{
  return m_Uncertainty;
}

// -----------------------------------------------------------------------------
void UncertainRegularGridSampleSurfaceMesh::setFeatureIdsArrayName(const QString& value)
{
  m_FeatureIdsArrayName = value;
}

// -----------------------------------------------------------------------------
QString UncertainRegularGridSampleSurfaceMesh::getFeatureIdsArrayName() const
{
  return m_FeatureIdsArrayName;
}
