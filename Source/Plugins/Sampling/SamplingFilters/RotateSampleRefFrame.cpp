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

#include <cmath>
#include <memory>

#include "RotateSampleRefFrame.h"

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range3d.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include <QtCore/QTextStream>

#include <Eigen/Dense>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DynamicTableFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLIb/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/GeometryMath.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "OrientationLib/Core/Orientation.hpp"
#include "OrientationLib/Core/OrientationTransformation.hpp"
#include "OrientationLib/Core/Quaternion.hpp"

#include "Sampling/SamplingConstants.h"
#include "Sampling/SamplingVersion.h"

namespace
{
struct RotateArgs
{
  int64_t xp = 0;
  int64_t yp = 0;
  int64_t zp = 0;
  float xRes = 0.0f;
  float yRes = 0.0f;
  float zRes = 0.0f;
  int64_t xpNew = 0;
  int64_t ypNew = 0;
  int64_t zpNew = 0;
  float xResNew = 0.0f;
  float yResNew = 0.0f;
  float zResNew = 0.0f;
  float xMinNew = 0.0f;
  float yMinNew = 0.0f;
  float zMinNew = 0.0f;
};

using Matrix3fR = Eigen::Matrix<float, 3, 3, Eigen::RowMajor>;

// Requires table to be 3 x 3
Matrix3fR tableToMatrix(const std::vector<std::vector<double>>& table)
{
  Matrix3fR matrix;

  for(size_t i = 0; i < table.size(); i++)
  {
    const auto& row = table[i];
    for(size_t j = 0; j < row.size(); j++)
    {
      matrix(i, j) = row[j];
    }
  }

  return matrix;
}

// rotationAngle in degrees
void axisAngleToRotationMatrix(float rotationAngle, const FloatVec3Type& rotationAxis, float rotMat[3][3])
{
  float rotAngle = rotationAngle * SIMPLib::Constants::k_Pi / 180.0;

  OrientationTransformation::ax2om<OrientationF, OrientationF>(OrientationF(rotationAxis[0], rotationAxis[1], rotationAxis[2], rotAngle)).toGMatrix(rotMat);
}

void determineMinMax(const float rotMat[3][3], const FloatVec3Type& spacing, size_t col, size_t row, size_t plane, float& xMin, float& xMax, float& yMin, float& yMax, float& zMin, float& zMax)
{
  float coords[3] = {0.0f, 0.0f, 0.0f};
  float newcoords[3] = {0.0f, 0.0f, 0.0f};

  coords[0] = static_cast<float>(col * spacing[0]);
  coords[1] = static_cast<float>(row * spacing[1]);
  coords[2] = static_cast<float>(plane * spacing[2]);
  MatrixMath::Multiply3x3with3x1(rotMat, coords, newcoords);

  xMin = std::min(newcoords[0], xMin);
  xMax = std::max(newcoords[0], xMax);

  yMin = std::min(newcoords[1], yMin);
  yMax = std::max(newcoords[1], yMax);

  zMin = std::min(newcoords[2], zMin);
  zMax = std::max(newcoords[2], zMax);
}

float determineSpacing(const FloatVec3Type& spacing, const float axisNew[3])
{
  const float xAxis[3] = {1.0f, 0.0f, 0.0f};
  const float yAxis[3] = {0.0f, 1.0f, 0.0f};
  const float zAxis[3] = {0.0f, 0.0f, 1.0f};

  float xAngle = std::abs(GeometryMath::CosThetaBetweenVectors(xAxis, axisNew));
  float yAngle = std::abs(GeometryMath::CosThetaBetweenVectors(yAxis, axisNew));
  float zAngle = std::abs(GeometryMath::CosThetaBetweenVectors(zAxis, axisNew));

  std::array<float, 3> axes = {xAngle, yAngle, zAngle};

  auto iter = std::max_element(axes.cbegin(), axes.cend());

  size_t index = std::distance(axes.cbegin(), iter);

  return spacing[index];
}

RotateArgs createRotateParams(const ImageGeom& imageGeom, const float rotMat[3][3])
{
  const SizeVec3Type origDims = imageGeom.getDimensions();
  const FloatVec3Type spacing = imageGeom.getSpacing();
  const FloatVec3Type origin = imageGeom.getOrigin();

  float xMin = std::numeric_limits<float>::max();
  float xMax = std::numeric_limits<float>::min();
  float yMin = std::numeric_limits<float>::max();
  float yMax = std::numeric_limits<float>::min();
  float zMin = std::numeric_limits<float>::max();
  float zMax = std::numeric_limits<float>::min();

  const std::vector<std::vector<size_t>> coords{{0, 0, 0},
                                                {origDims[0] - 1, 0, 0},
                                                {0, origDims[1] - 1, 0},
                                                {origDims[0] - 1, origDims[1] - 1, 0},
                                                {0, 0, origDims[2] - 1},
                                                {origDims[0] - 1, 0, origDims[2] - 1},
                                                {0, origDims[1] - 1, origDims[2] - 1},
                                                {origDims[0] - 1, origDims[1] - 1, origDims[2] - 1}};

  for(const auto& item : coords)
  {
    determineMinMax(rotMat, spacing, item[0], item[1], item[2], xMin, xMax, yMin, yMax, zMin, zMax);
  }

  const float xAxis[3] = {1.0f, 0.0f, 0.0f};
  const float yAxis[3] = {0.0f, 1.0f, 0.0f};
  const float zAxis[3] = {0.0f, 0.0f, 1.0f};

  float xAxisNew[3] = {0.0f, 0.0f, 0.0f};
  float yAxisNew[3] = {0.0f, 0.0f, 0.0f};
  float zAxisNew[3] = {0.0f, 0.0f, 0.0f};

  MatrixMath::Multiply3x3with3x1(rotMat, xAxis, xAxisNew);
  MatrixMath::Multiply3x3with3x1(rotMat, yAxis, yAxisNew);
  MatrixMath::Multiply3x3with3x1(rotMat, zAxis, zAxisNew);

  float xResNew = determineSpacing(spacing, xAxisNew);
  float yResNew = determineSpacing(spacing, yAxisNew);
  float zResNew = determineSpacing(spacing, zAxisNew);

  MeshIndexType xpNew = static_cast<int64_t>(std::nearbyint((xMax - xMin) / xResNew) + 1);
  MeshIndexType ypNew = static_cast<int64_t>(std::nearbyint((yMax - yMin) / yResNew) + 1);
  MeshIndexType zpNew = static_cast<int64_t>(std::nearbyint((zMax - zMin) / zResNew) + 1);

  RotateArgs params;

  params.xp = origDims[0];
  params.xRes = spacing[0];
  params.yp = origDims[1];
  params.yRes = spacing[1];
  params.zp = origDims[2];
  params.zRes = spacing[2];

  params.xpNew = xpNew;
  params.xResNew = xResNew;
  params.xMinNew = xMin;
  params.ypNew = ypNew;
  params.yResNew = yResNew;
  params.yMinNew = yMin;
  params.zpNew = zpNew;
  params.zResNew = zResNew;
  params.zMinNew = zMin;

  return params;
}

void updateGeometry(ImageGeom& imageGeom, const RotateArgs& params)
{
  FloatVec3Type origin = imageGeom.getOrigin();

  imageGeom.setSpacing(params.xResNew, params.yResNew, params.zResNew);
  imageGeom.setDimensions(params.xpNew, params.ypNew, params.zpNew);
  origin[0] += params.xMinNew;
  origin[1] += params.yMinNew;
  origin[2] += params.zMinNew;
  imageGeom.setOrigin(origin);
}

/**
 * @brief The RotateSampleRefFrameImpl class implements a threaded algorithm to do the
 * actual computation of the rotation by applying the rotation to each Euler angle
 */
class SampleRefFrameRotator
{
  DataArray<int64_t>::Pointer newIndicesPtr;
  float rotMatrixInv[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  bool m_SliceBySlice = false;
  const RotateArgs* m_params;

public:
  SampleRefFrameRotator(DataArray<int64_t>::Pointer newindices, const RotateArgs* args, const float rotMat[3][3], bool sliceBySlice)
  : newIndicesPtr(newindices)
  , m_SliceBySlice(sliceBySlice)
  , m_params(args)
  {
    // We have to inline the 3x3 Maxtrix transpose here because of the "const" nature of the 'convert' function
    MatrixMath::Transpose3x3(rotMat, rotMatrixInv);
  }

  ~SampleRefFrameRotator() = default;

  void convert(int64_t zStart, int64_t zEnd, int64_t yStart, int64_t yEnd, int64_t xStart, int64_t xEnd) const
  {
    int64_t* newindicies = newIndicesPtr->getPointer(0);

    for(int64_t k = zStart; k < zEnd; k++)
    {
      int64_t ktot = (m_params->xpNew * m_params->ypNew) * k;
      for(int64_t j = yStart; j < yEnd; j++)
      {
        int64_t jtot = (m_params->xpNew) * j;
        for(int64_t i = xStart; i < xEnd; i++)
        {
          int64_t index = ktot + jtot + i;
          newindicies[index] = -1;

          float coords[3] = {0.0f, 0.0f, 0.0f};
          float coordsNew[3] = {0.0f, 0.0f, 0.0f};

          coords[0] = (static_cast<float>(i) * m_params->xResNew) + m_params->xMinNew;
          coords[1] = (static_cast<float>(j) * m_params->yResNew) + m_params->yMinNew;
          coords[2] = (static_cast<float>(k) * m_params->zResNew) + m_params->zMinNew;

          MatrixMath::Multiply3x3with3x1(rotMatrixInv, coords, coordsNew);

          int64_t colOld = static_cast<int64_t>(std::nearbyint(coordsNew[0] / m_params->xRes));
          int64_t rowOld = static_cast<int64_t>(std::nearbyint(coordsNew[1] / m_params->yRes));
          int64_t planeOld = static_cast<int64_t>(std::nearbyint(coordsNew[2] / m_params->zRes));

          if(m_SliceBySlice)
          {
            planeOld = k;
          }

          if(colOld >= 0 && colOld < m_params->xp && rowOld >= 0 && rowOld < m_params->yp && planeOld >= 0 && planeOld < m_params->zp)
          {
            newindicies[index] = (m_params->xp * m_params->yp * planeOld) + (m_params->xp * rowOld) + colOld;
          }
        }
      }
    }
  }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  void operator()(const tbb::blocked_range3d<int64_t, int64_t, int64_t>& r) const
  {
    convert(r.pages().begin(), r.pages().end(), r.rows().begin(), r.rows().end(), r.cols().begin(), r.cols().end());
  }
#endif
};

} // namespace

struct RotateSampleRefFrame::Impl
{
  float m_RotationMatrix[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  RotateArgs m_Params;

  void reset()
  {
    float* ptr = &m_RotationMatrix[0][0];

    std::fill(ptr, ptr + 9, 0.0f);

    m_Params = RotateArgs();
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RotateSampleRefFrame::RotateSampleRefFrame()
: m_CellAttributeMatrixPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, "")
, p_Impl(std::make_unique<Impl>())
{
  std::vector<std::vector<double>> defaultTable{{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};
  m_RotationTable.setTableData(defaultTable);
  m_RotationTable.setDynamicRows(false);
  m_RotationTable.setDynamicCols(false);
  m_RotationTable.setDefaultColCount(3);
  m_RotationTable.setDefaultRowCount(3);
  m_RotationTable.setMinCols(3);
  m_RotationTable.setMinRows(3);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RotateSampleRefFrame::~RotateSampleRefFrame() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Rotation Representation");
    parameter->setPropertyName("RotationRepresentation");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(RotateSampleRefFrame, this, RotationRepresentation));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(RotateSampleRefFrame, this, RotationRepresentation));
    QVector<QString> choices{"Axis Angle", "Rotation Matrix"};
    parameter->setChoices(choices);
    QStringList linkedProps{"RotationAngle", "RotationAxis", "RotationTable"};
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Category::Parameter);
    parameters.push_back(parameter);
  }

  // Axis Angle Parameters

  parameters.push_back(SIMPL_NEW_FLOAT_FP("Rotation Angle (Degrees)", RotationAngle, FilterParameter::Parameter, RotateSampleRefFrame, 0));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Rotation Axis (ijk)", RotationAxis, FilterParameter::Parameter, RotateSampleRefFrame, 0));

  // Rotation Matrix Parameters

  parameters.push_back(SIMPL_NEW_DYN_TABLE_FP("Rotation Matrix", RotationTable, FilterParameter::Parameter, RotateSampleRefFrame, 1));

  // Required Arrays

  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Cell Attribute Matrix", CellAttributeMatrixPath, FilterParameter::RequiredArray, RotateSampleRefFrame, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellAttributeMatrixPath(reader->readDataArrayPath("CellAttributeMatrixPath", getCellAttributeMatrixPath()));
  setRotationAxis(reader->readFloatVec3("RotationAxis", getRotationAxis()));
  setRotationAngle(reader->readValue("RotationAngle", getRotationAngle()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  p_Impl->reset();

  if(getErrorCode() < 0)
  {
    return;
  }

  if(!isRotationRepresentationValid(m_RotationRepresentation))
  {
    QString ss = QObject::tr("Invalid rotation representation");
    setErrorCondition(-12005, ss);
    return;
  }

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getCellAttributeMatrixPath().getDataContainerName());
  getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getCellAttributeMatrixPath(), -301);
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getCellAttributeMatrixPath().getDataContainerName());

  if(m == nullptr)
  {
    QString ss = QObject::tr("Failed to get DataContainer '%1'").arg(getCellAttributeMatrixPath().getDataContainerName());
    setErrorCondition(-12011, ss);
    return;
  }

  ImageGeom::Pointer imageGeom = m->getGeometryAs<ImageGeom>();

  if(imageGeom == nullptr)
  {
    QString ss = QObject::tr("Failed to get Image Geometry from '%1'").arg(getCellAttributeMatrixPath().getDataContainerName());
    setErrorCondition(-12012, ss);
    return;
  }

  const RotationRepresentation representation = getRotationRepresentationEnum();

  switch(representation)
  {
  case RotationRepresentation::AxisAngle:
  {
    const Eigen::Vector3f rotationAxis(m_RotationAxis.data());
    float norm = rotationAxis.norm();
    if(!SIMPLibMath::closeEnough(rotationAxis.norm(), 1.0f, 0.00001f))
    {
      QString ss = QObject::tr("Axis angle must be normalized (norm is %1)").arg(norm);
      setErrorCondition(-12006, ss);
      return;
    }
    axisAngleToRotationMatrix(m_RotationAngle, m_RotationAxis, p_Impl->m_RotationMatrix);
  }
  break;
  case RotationRepresentation::RotationMatrix:
  {
    auto rotationMatrixTable = m_RotationTable.getTableData();

    if(rotationMatrixTable.size() != 3)
    {
      QString ss = QObject::tr("Rotation Matrix must be 3 x 3");
      setErrorCondition(-12001, ss);
      return;
    }

    for(const auto& row : rotationMatrixTable)
    {
      if(row.size() != 3)
      {
        QString ss = QObject::tr("Rotation Matrix must be 3 x 3");
        setErrorCondition(-12001, ss);
        return;
      }
    }

    Matrix3fR rotationMatrix = tableToMatrix(rotationMatrixTable);

    float determinant = rotationMatrix.determinant();

    if(!SIMPLibMath::closeEnough(determinant, 1.0f, 0.00001f))
    {
      QString ss = QObject::tr("Rotation Matrix must have a determinant of 1 (is %1)").arg(determinant);
      setErrorCondition(-12002, ss);
      return;
    }

    Matrix3fR transpose = rotationMatrix.transpose();
    Matrix3fR inverse = rotationMatrix.inverse();

    if(!transpose.isApprox(inverse))
    {
      QString ss = QObject::tr("Rotation Matrix's inverse and transpose must be equal");
      setErrorCondition(-12003, ss);
      return;
    }

    Eigen::Map<Matrix3fR>(&p_Impl->m_RotationMatrix[0][0], rotationMatrix.rows(), rotationMatrix.cols()) = rotationMatrix;
  }
  break;
  default:
  {
    QString ss = QObject::tr("Invalid rotation representation");
    setErrorCondition(-12004, ss);
    return;
  }
  }

  p_Impl->m_Params = createRotateParams(*imageGeom, p_Impl->m_RotationMatrix);

  updateGeometry(*imageGeom, p_Impl->m_Params);

  // Resize attribute matrix

  std::vector<size_t> tDims(3);
  tDims[0] = p_Impl->m_Params.xpNew;
  tDims[1] = p_Impl->m_Params.ypNew;
  tDims[2] = p_Impl->m_Params.zpNew;
  QString attrMatName = getCellAttributeMatrixPath().getAttributeMatrixName();
  m->getAttributeMatrix(attrMatName)->resizeAttributeArrays(tDims);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true);              // Set the fact that we are preflighting.
  emit preflightAboutToExecute();    // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck();                       // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted();          // We are done preflighting this filter
  setInPreflight(false);             // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateSampleRefFrame::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getCellAttributeMatrixPath().getDataContainerName());

  if(m == nullptr)
  {
    QString ss = QObject::tr("Failed to get DataContainer '%1'").arg(getCellAttributeMatrixPath().getDataContainerName());
    setErrorCondition(-12010, ss);
    return;
  }

  int64_t newNumCellTuples = p_Impl->m_Params.xpNew * p_Impl->m_Params.ypNew * p_Impl->m_Params.zpNew;

  DataArray<int64_t>::Pointer newIndiciesPtr = DataArray<int64_t>::CreateArray(newNumCellTuples, "_INTERNAL_USE_ONLY_RotateSampleRef_NewIndicies", true);
  newIndiciesPtr->initializeWithValue(-1);
  int64_t* newindicies = newIndiciesPtr->getPointer(0);

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  tbb::parallel_for(tbb::blocked_range3d<int64_t, int64_t, int64_t>(0, p_Impl->m_Params.zpNew, 0, p_Impl->m_Params.ypNew, 0, p_Impl->m_Params.xpNew),
                    SampleRefFrameRotator(newIndiciesPtr, &p_Impl->m_Params, p_Impl->m_RotationMatrix, m_SliceBySlice), tbb::auto_partitioner());
#else
  {
    SampleRefFrameRotator serial(newIndiciesPtr, &p_Impl->m_Params, p_Impl->m_RotationMatrix, m_SliceBySlice);
    serial.convert(0, p_Impl->m_Params.zpNew, 0, p_Impl->m_Params.ypNew, 0, p_Impl->m_Params.xpNew);
  }
#endif

  // This could technically be parallelized also where each thread takes an array to adjust. Except
  // that the DataContainer is NOT thread safe or re-entrant so that would actually be a BAD idea.

  QString attrMatName = getCellAttributeMatrixPath().getAttributeMatrixName();
  QList<QString> voxelArrayNames = m->getAttributeMatrix(attrMatName)->getAttributeArrayNames();

  for(const auto& attrArrayName : voxelArrayNames)
  {
    IDataArray::Pointer p = m->getAttributeMatrix(attrMatName)->getAttributeArray(attrArrayName);

    // Make a copy of the 'p' array that has the same name. When placed into
    // the data container this will over write the current array with
    // the same name.

    IDataArray::Pointer data = p->createNewArray(newNumCellTuples, p->getComponentDimensions(), p->getName());
    int64_t newIndicies_I = 0;
    for(size_t i = 0; i < static_cast<size_t>(newNumCellTuples); i++)
    {
      newIndicies_I = newindicies[i];
      if(newIndicies_I >= 0)
      {
        if(!data->copyFromArray(i, p, newIndicies_I, 1))
        {
          QString ss = QObject::tr("copyFromArray Failed: ");
          QTextStream out(&ss);
          out << "Source Array Name: " << p->getName() << " Source Tuple Index: " << newIndicies_I << "\n";
          out << "Dest Array Name: " << data->getName() << "  Dest. Tuple Index: " << i << "\n";
          setErrorCondition(-11004, ss);
          return;
        }
      }
      else
      {
        int var = 0;
        data->initializeTuple(i, &var);
      }
    }
    m->getAttributeMatrix(attrMatName)->insertOrAssign(data);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RotateSampleRefFrame::newFilterInstance(bool copyFilterParameters) const
{
  RotateSampleRefFrame::Pointer filter = RotateSampleRefFrame::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RotateSampleRefFrame::getCompiledLibraryName() const
{
  return SamplingConstants::SamplingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RotateSampleRefFrame::getBrandingString() const
{
  return "Sampling";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RotateSampleRefFrame::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Sampling::Version::Major() << "." << Sampling::Version::Minor() << "." << Sampling::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RotateSampleRefFrame::getGroupName() const
{
  return SIMPL::FilterGroups::SamplingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid RotateSampleRefFrame::getUuid() const
{
  return QUuid("{e25d9b4c-2b37-578c-b1de-cf7032b5ef19}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RotateSampleRefFrame::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::RotationTransformationFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RotateSampleRefFrame::getHumanLabel() const
{
  return "Rotate Sample Reference Frame";
}

// -----------------------------------------------------------------------------
RotateSampleRefFrame::Pointer RotateSampleRefFrame::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<RotateSampleRefFrame> RotateSampleRefFrame::New()
{
  struct make_shared_enabler : public RotateSampleRefFrame
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString RotateSampleRefFrame::getNameOfClass() const
{
  return QString("RotateSampleRefFrame");
}

// -----------------------------------------------------------------------------
QString RotateSampleRefFrame::ClassName()
{
  return QString("RotateSampleRefFrame");
}

// -----------------------------------------------------------------------------
void RotateSampleRefFrame::setCellAttributeMatrixPath(const DataArrayPath& value)
{
  m_CellAttributeMatrixPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath RotateSampleRefFrame::getCellAttributeMatrixPath() const
{
  return m_CellAttributeMatrixPath;
}

// -----------------------------------------------------------------------------
void RotateSampleRefFrame::setRotationAxis(const FloatVec3Type& value)
{
  m_RotationAxis = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type RotateSampleRefFrame::getRotationAxis() const
{
  return m_RotationAxis;
}

// -----------------------------------------------------------------------------
void RotateSampleRefFrame::setRotationAngle(float value)
{
  m_RotationAngle = value;
}

// -----------------------------------------------------------------------------
float RotateSampleRefFrame::getRotationAngle() const
{
  return m_RotationAngle;
}

// -----------------------------------------------------------------------------
void RotateSampleRefFrame::setSliceBySlice(bool value)
{
  m_SliceBySlice = value;
}

// -----------------------------------------------------------------------------
bool RotateSampleRefFrame::getSliceBySlice() const
{
  return m_SliceBySlice;
}

// -----------------------------------------------------------------------------
void RotateSampleRefFrame::setRotationTable(const DynamicTableData& value)
{
  m_RotationTable = value;
}

// -----------------------------------------------------------------------------
DynamicTableData RotateSampleRefFrame::getRotationTable() const
{
  return m_RotationTable;
}

// -----------------------------------------------------------------------------
void RotateSampleRefFrame::setRotationRepresentation(int value)
{
  m_RotationRepresentation = value;
}

// -----------------------------------------------------------------------------
int RotateSampleRefFrame::getRotationRepresentation() const
{
  return m_RotationRepresentation;
}

// -----------------------------------------------------------------------------
RotateSampleRefFrame::RotationRepresentation RotateSampleRefFrame::getRotationRepresentationEnum() const
{
  return static_cast<RotationRepresentation>(m_RotationRepresentation);
}

// -----------------------------------------------------------------------------
void RotateSampleRefFrame::setRotationRepresentationEnum(RotationRepresentation value)
{
  m_RotationRepresentation = static_cast<int>(value);
}

// -----------------------------------------------------------------------------
bool RotateSampleRefFrame::isRotationRepresentationValid(int value)
{
  return (value >= 0) && (value <= 1);
}
