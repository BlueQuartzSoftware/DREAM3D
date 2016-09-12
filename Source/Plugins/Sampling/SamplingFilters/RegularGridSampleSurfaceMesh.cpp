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

#include "RegularGridSampleSurfaceMesh.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Math/GeometryMath.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Utilities/SIMPLibRandom.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"

#include "Sampling/SamplingConstants.h"
#include "Sampling/SamplingVersion.h"

#include "moc_RegularGridSampleSurfaceMesh.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RegularGridSampleSurfaceMesh::RegularGridSampleSurfaceMesh() :
  SampleSurfaceMesh(),
  m_DataContainerName(SIMPL::Defaults::ImageDataContainerName),
  m_CellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName),
  m_XPoints(0),
  m_YPoints(0),
  m_ZPoints(0),
  m_FeatureIdsArrayName(SIMPL::CellData::FeatureIds),
  m_FeatureIds(nullptr)
{
  m_Resolution.x = 1.0f;
  m_Resolution.y = 1.0f;
  m_Resolution.z = 1.0f;

  m_Origin.x = 0.0f;
  m_Origin.y = 0.0f;
  m_Origin.z = 0.0f;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RegularGridSampleSurfaceMesh::~RegularGridSampleSurfaceMesh()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegularGridSampleSurfaceMesh::setupFilterParameters()
{
  FilterParameterVector parameters = getFilterParameters();
  parameters.push_back(SIMPL_NEW_INTEGER_FP("X Points", XPoints, FilterParameter::Parameter, RegularGridSampleSurfaceMesh));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Y Points", YPoints, FilterParameter::Parameter, RegularGridSampleSurfaceMesh));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Z Points", ZPoints, FilterParameter::Parameter, RegularGridSampleSurfaceMesh));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Resolution", Resolution, FilterParameter::Parameter, RegularGridSampleSurfaceMesh));

  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Origin", Origin, FilterParameter::Parameter, RegularGridSampleSurfaceMesh));

  parameters.push_back(SIMPL_NEW_STRING_FP("Data Container", DataContainerName, FilterParameter::CreatedArray, RegularGridSampleSurfaceMesh));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Attribute Matrix", CellAttributeMatrixName, FilterParameter::CreatedArray, RegularGridSampleSurfaceMesh));
  parameters.push_back(SIMPL_NEW_STRING_FP("Feature Ids", FeatureIdsArrayName, FilterParameter::CreatedArray, RegularGridSampleSurfaceMesh));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegularGridSampleSurfaceMesh::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  SampleSurfaceMesh::readFilterParameters(reader, index);
  reader->openFilterGroup(this, index);
  setXPoints(reader->readValue("XPoints", getXPoints()));
  setYPoints(reader->readValue("YPoints", getYPoints()));
  setZPoints(reader->readValue("ZPoints", getZPoints()));
  setResolution(reader->readFloatVec3("Resolution", getResolution()));
  setOrigin(reader->readFloatVec3("Origin", getOrigin()));
  setDataContainerName(reader->readString("DataContainerName", getDataContainerName()));
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName() ) );
  setFeatureIdsArrayName(reader->readString("FeatureIdsArrayName", getFeatureIdsArrayName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegularGridSampleSurfaceMesh::initialize()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegularGridSampleSurfaceMesh::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getDataContainerName());
  if (getErrorCondition() < 0) { return; }

  ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
  m->setGeometry(image);

  // Set the Dimensions, Resolution and Origin of the output data container
  m->getGeometryAs<ImageGeom>()->setDimensions(m_XPoints, m_YPoints, m_ZPoints);
  m->getGeometryAs<ImageGeom>()->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
  m->getGeometryAs<ImageGeom>()->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);

  QVector<size_t> tDims(3, 0);
  tDims[0] = m_XPoints;
  tDims[1] = m_YPoints;
  tDims[2] = m_ZPoints;
  AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, SIMPL::AttributeMatrixType::Cell);
  if (getErrorCondition() < 0 || nullptr == cellAttrMat.get()) { return; }

  QVector<size_t> cDims(1, 1);
  tempPath.update(getDataContainerName(), getCellAttributeMatrixName(), getFeatureIdsArrayName() );
  m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( nullptr != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegularGridSampleSurfaceMesh::preflight()
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
VertexGeom::Pointer RegularGridSampleSurfaceMesh::generate_points()
{
  VertexGeom::Pointer points = VertexGeom::CreateGeometry((m_XPoints * m_YPoints * m_ZPoints), "_INTERNAL_USE_ONLY_points");

  int64_t count = 0;
  float coords[3] = { 0.0f, 0.0f, 0.0f };
  for (int64_t k = 0; k < m_ZPoints; k++)
  {
    for (int64_t j = 0; j < m_YPoints; j++)
    {
      for (int64_t i = 0; i < m_XPoints; i++)
      {
        coords[0] = (float(i) + 0.5f) * m_Resolution.x + m_Origin.x;
        coords[1] = (float(j) + 0.5f) * m_Resolution.y + m_Origin.y;
        coords[2] = (float(k) + 0.5f) * m_Resolution.z + m_Origin.z;
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
void RegularGridSampleSurfaceMesh::assign_points(Int32ArrayType::Pointer iArray)
{
  int32_t* ids = iArray->getPointer(0);
  for (int64_t i = 0; i < (m_XPoints * m_YPoints * m_ZPoints); i++)
  {
    m_FeatureIds[i] = ids[i];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegularGridSampleSurfaceMesh::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  SIMPL_RANDOMNG_NEW()

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());

  m->getGeometryAs<ImageGeom>()->setDimensions(m_XPoints, m_YPoints, m_ZPoints);
  m->getGeometryAs<ImageGeom>()->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);
  m->getGeometryAs<ImageGeom>()->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);

  SampleSurfaceMesh::execute();

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RegularGridSampleSurfaceMesh::newFilterInstance(bool copyFilterParameters)
{
  RegularGridSampleSurfaceMesh::Pointer filter = RegularGridSampleSurfaceMesh::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RegularGridSampleSurfaceMesh::getCompiledLibraryName()
{
  return SamplingConstants::SamplingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RegularGridSampleSurfaceMesh::getBrandingString()
{
  return "Sampling";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RegularGridSampleSurfaceMesh::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  Sampling::Version::Major() << "." << Sampling::Version::Minor() << "." << Sampling::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RegularGridSampleSurfaceMesh::getGroupName()
{ return SIMPL::FilterGroups::SamplingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RegularGridSampleSurfaceMesh::getSubGroupName()
{ return SIMPL::FilterSubGroups::ResolutionFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RegularGridSampleSurfaceMesh::getHumanLabel()
{ return "Sample Triangle Geometry on Regular Grid"; }
