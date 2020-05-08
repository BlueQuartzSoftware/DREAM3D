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

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/PreflightUpdatedValueFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/Math/GeometryMath.h"
#include "SIMPLib/Math/SIMPLibRandom.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "Sampling/SamplingConstants.h"
#include "Sampling/SamplingVersion.h"

#define INIT_SYNTH_VOLUME_CHECK(var, errCond)                                                                                                                                                          \
  if(m_##var <= 0)                                                                                                                                                                                     \
  {                                                                                                                                                                                                    \
    QString ss = QObject::tr("%1 must be positive").arg(#var);                                                                                                                                         \
    setErrorCondition(errCond, ss);                                                                                                                                                                    \
  }

enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID21 = 21,

  DataContainerID = 1
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RegularGridSampleSurfaceMesh::RegularGridSampleSurfaceMesh()
: m_DataContainerName(SIMPL::Defaults::ImageDataContainerName)
, m_CellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName)
, m_FeatureIdsArrayName(SIMPL::CellData::FeatureIds)
{
  m_Dimensions[0] = 128;
  m_Dimensions[1] = 128;
  m_Dimensions[2] = 128;

  m_Spacing[0] = 1.0f;
  m_Spacing[1] = 1.0f;
  m_Spacing[2] = 1.0f;

  m_Origin[0] = 0.0f;
  m_Origin[1] = 0.0f;
  m_Origin[2] = 0.0f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RegularGridSampleSurfaceMesh::~RegularGridSampleSurfaceMesh() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegularGridSampleSurfaceMesh::setupFilterParameters()
{
  SampleSurfaceMesh::setupFilterParameters();
  FilterParameterVectorType parameters = getFilterParameters();

  parameters.push_back(SIMPL_NEW_INT_VEC3_FP("Dimensions (Voxels)", Dimensions, FilterParameter::Parameter, RegularGridSampleSurfaceMesh));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Spacing", Spacing, FilterParameter::Parameter, RegularGridSampleSurfaceMesh));
  parameters.back()->setLegacyPropertyName("Resolution");

  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Origin", Origin, FilterParameter::Parameter, RegularGridSampleSurfaceMesh));

  QVector<QString> choices = IGeometry::GetAllLengthUnitStrings();
  parameters.push_back(SIMPL_NEW_CHOICE_FP("Length Units (For Description Only)", LengthUnit, FilterParameter::Parameter, RegularGridSampleSurfaceMesh, choices, false));
  PreflightUpdatedValueFilterParameter::Pointer param = SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP("Box Size in Length Units", BoxDimensions, FilterParameter::Parameter, RegularGridSampleSurfaceMesh);
  param->setReadOnly(true);
  parameters.push_back(param);

  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Data Container", DataContainerName, FilterParameter::CreatedArray, RegularGridSampleSurfaceMesh));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_AM_WITH_LINKED_DC_FP("Cell Attribute Matrix", CellAttributeMatrixName, DataContainerName, FilterParameter::CreatedArray, RegularGridSampleSurfaceMesh));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Feature Ids", FeatureIdsArrayName, DataContainerName, CellAttributeMatrixName, FilterParameter::CreatedArray, RegularGridSampleSurfaceMesh));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegularGridSampleSurfaceMesh::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  SampleSurfaceMesh::readFilterParameters(reader, index);
  reader->openFilterGroup(this, index);
  setDimensions(reader->readIntVec3("Dimensions", getDimensions()));
  setSpacing(reader->readFloatVec3("Spacing", getSpacing()));
  setOrigin(reader->readFloatVec3("Origin", getOrigin()));
  setDataContainerName(reader->readDataArrayPath("DataContainerName", getDataContainerName()));
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName()));
  setFeatureIdsArrayName(reader->readString("FeatureIdsArrayName", getFeatureIdsArrayName()));
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
  clearErrorCode();
  clearWarningCode();
  DataArrayPath tempPath;

  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer(this, getDataContainerName(), DataContainerID);
  if(getErrorCode() < 0)
  {
    return;
  }

  ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
  m->setGeometry(image);

  // Sanity Check the Dimensions and Spacing
  INIT_SYNTH_VOLUME_CHECK(Dimensions[0], -5000);
  INIT_SYNTH_VOLUME_CHECK(Dimensions[1], -5001);
  INIT_SYNTH_VOLUME_CHECK(Dimensions[2], -5002);
  INIT_SYNTH_VOLUME_CHECK(Spacing[0], -5003);
  INIT_SYNTH_VOLUME_CHECK(Spacing[1], -5004);
  INIT_SYNTH_VOLUME_CHECK(Spacing[2], -5005);

  // Set the Dimensions, Spacing and Origin of the output data container
  image->setDimensions(static_cast<size_t>(m_Dimensions[0]), static_cast<size_t>(m_Dimensions[1]), static_cast<size_t>(m_Dimensions[2]));
  image->setSpacing(m_Spacing);
  image->setOrigin(m_Origin);
  image->setUnits(static_cast<IGeometry::LengthUnit>(m_LengthUnit));

  std::vector<size_t> tDims = {static_cast<size_t>(m_Dimensions[0]), static_cast<size_t>(m_Dimensions[1]), static_cast<size_t>(m_Dimensions[2])};
  AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix(this, getCellAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell, AttributeMatrixID21);
  if(getErrorCode() < 0 || nullptr == cellAttrMat.get())
  {
    return;
  }

  std::vector<size_t> cDims(1, 1);
  tempPath.update(getDataContainerName().getDataContainerName(), getCellAttributeMatrixName(), getFeatureIdsArrayName());
  m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>>(this, tempPath, 0, cDims);
  if(nullptr != m_FeatureIdsPtr.lock())
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexGeom::Pointer RegularGridSampleSurfaceMesh::generate_points()
{
  VertexGeom::Pointer points = VertexGeom::CreateGeometry((m_Dimensions[0] * m_Dimensions[1] * m_Dimensions[2]), "_INTERNAL_USE_ONLY_points");

  int64_t count = 0;
  float coords[3] = {0.0f, 0.0f, 0.0f};

  for(int32_t k = 0; k < m_Dimensions[2]; k++)
  {
    float f_k = static_cast<float>(k) + 0.5f;
    for(int32_t j = 0; j < m_Dimensions[1]; j++)
    {
      float f_j = static_cast<float>(j) + 0.5f;
      for(int32_t i = 0; i < m_Dimensions[0]; i++)
      {
        float f_i = static_cast<float>(i) + 0.5f;
        coords[0] = f_i * m_Spacing[0] + m_Origin[0];
        coords[1] = f_j * m_Spacing[1] + m_Origin[1];
        coords[2] = f_k * m_Spacing[2] + m_Origin[2];
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
  size_t totalPoints = m_Dimensions[0] * m_Dimensions[1] * m_Dimensions[2];
  for(size_t i = 0; i < totalPoints; i++)
  {
    m_FeatureIds[i] = ids[i];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegularGridSampleSurfaceMesh::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  SIMPL_RANDOMNG_NEW()

  SampleSurfaceMesh::execute();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RegularGridSampleSurfaceMesh::getBoxDimensions()
{
  QString lengthUnit = IGeometry::LengthUnitToString(static_cast<IGeometry::LengthUnit>(m_LengthUnit));
  QString desc;
  QTextStream ss(&desc);

  ss << "X Range: " << m_Origin[0] << " to " << (m_Origin[0] + (m_Dimensions[0] * m_Spacing[0])) << " (Delta: " << (m_Dimensions[0] * m_Spacing[0]) << ") " << lengthUnit << "\n";
  ss << "Y Range: " << m_Origin[1] << " to " << (m_Origin[1] + (m_Dimensions[1] * m_Spacing[1])) << " (Delta: " << (m_Dimensions[1] * m_Spacing[1]) << ") " << lengthUnit << "\n";
  ss << "Z Range: " << m_Origin[2] << " to " << (m_Origin[2] + (m_Dimensions[2] * m_Spacing[2])) << " (Delta: " << (m_Dimensions[2] * m_Spacing[2]) << ") " << lengthUnit << "\n";

  float vol = (m_Dimensions[0] * m_Spacing[0]) * (m_Dimensions[1] * m_Spacing[1]) * (m_Dimensions[2] * m_Spacing[2]);
  QLocale usa(QLocale::English, QLocale::UnitedStates);

  ss << "Volume: " << usa.toString(vol) << " " << lengthUnit << "s ^3"
     << "\n";
  return desc;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RegularGridSampleSurfaceMesh::getCompiledLibraryName() const
{
  return SamplingConstants::SamplingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RegularGridSampleSurfaceMesh::getBrandingString() const
{
  return "Sampling";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RegularGridSampleSurfaceMesh::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Sampling::Version::Major() << "." << Sampling::Version::Minor() << "." << Sampling::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RegularGridSampleSurfaceMesh::getGroupName() const
{
  return SIMPL::FilterGroups::SamplingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid RegularGridSampleSurfaceMesh::getUuid() const
{
  return QUuid("{0df3da89-9106-538e-b1a9-6bbf1cf0aa92}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RegularGridSampleSurfaceMesh::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::ResolutionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RegularGridSampleSurfaceMesh::getHumanLabel() const
{
  return "Sample Triangle Geometry on Regular Grid";
}

// -----------------------------------------------------------------------------
RegularGridSampleSurfaceMesh::Pointer RegularGridSampleSurfaceMesh::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<RegularGridSampleSurfaceMesh> RegularGridSampleSurfaceMesh::New()
{
  struct make_shared_enabler : public RegularGridSampleSurfaceMesh
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString RegularGridSampleSurfaceMesh::getNameOfClass() const
{
  return QString("RegularGridSampleSurfaceMesh");
}

// -----------------------------------------------------------------------------
QString RegularGridSampleSurfaceMesh::ClassName()
{
  return QString("RegularGridSampleSurfaceMesh");
}

// -----------------------------------------------------------------------------
void RegularGridSampleSurfaceMesh::setDataContainerName(const DataArrayPath& value)
{
  m_DataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath RegularGridSampleSurfaceMesh::getDataContainerName() const
{
  return m_DataContainerName;
}

// -----------------------------------------------------------------------------
void RegularGridSampleSurfaceMesh::setCellAttributeMatrixName(const QString& value)
{
  m_CellAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString RegularGridSampleSurfaceMesh::getCellAttributeMatrixName() const
{
  return m_CellAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void RegularGridSampleSurfaceMesh::setLengthUnit(int32_t value)
{
  m_LengthUnit = value;
}

// -----------------------------------------------------------------------------
int32_t RegularGridSampleSurfaceMesh::getLengthUnit() const
{
  return m_LengthUnit;
}

// -----------------------------------------------------------------------------
void RegularGridSampleSurfaceMesh::setDimensions(const IntVec3Type& value)
{
  m_Dimensions = value;
}

// -----------------------------------------------------------------------------
IntVec3Type RegularGridSampleSurfaceMesh::getDimensions() const
{
  return m_Dimensions;
}

// -----------------------------------------------------------------------------
void RegularGridSampleSurfaceMesh::setSpacing(const FloatVec3Type& value)
{
  m_Spacing = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type RegularGridSampleSurfaceMesh::getSpacing() const
{
  return m_Spacing;
}

// -----------------------------------------------------------------------------
void RegularGridSampleSurfaceMesh::setOrigin(const FloatVec3Type& value)
{
  m_Origin = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type RegularGridSampleSurfaceMesh::getOrigin() const
{
  return m_Origin;
}

// -----------------------------------------------------------------------------
void RegularGridSampleSurfaceMesh::setFeatureIdsArrayName(const QString& value)
{
  m_FeatureIdsArrayName = value;
}

// -----------------------------------------------------------------------------
QString RegularGridSampleSurfaceMesh::getFeatureIdsArrayName() const
{
  return m_FeatureIdsArrayName;
}
