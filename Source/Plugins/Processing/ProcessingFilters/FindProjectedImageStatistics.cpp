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
#include "FindProjectedImageStatistics.h"

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#endif

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/TemplateHelpers.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/SIMPLibMath.h"

#include "Processing/ProcessingConstants.h"
#include "Processing/ProcessingVersion.h"

/**
 * @brief The CalcProjectedStatsImpl class implements a templated threaded algorithm for
 * determining the projected image statistics of a given volume.
 */
template <typename T>
class CalcProjectedStatsImpl
{

public:
  CalcProjectedStatsImpl(T* data, float* min, float* max, float* avg, float* std, float* var, int32_t* startPoints, size_t stride, size_t depth)
  : m_Data(data)
  , m_Min(min)
  , m_Max(max)
  , m_Avg(avg)
  , m_Std(std)
  , m_Var(var)
  , m_StartPoints(startPoints)
  , m_Stride(stride)
  , m_Depth(depth)
  {
  }
  virtual ~CalcProjectedStatsImpl() = default;

  void convert(size_t start, size_t end) const
  {
    int32_t point = 0, newPoint = 0;
    T val = static_cast<T>(0);
    for(size_t i = start; i < end; i++)
    {
      point = m_StartPoints[i];
      m_Min[point] = m_Data[point];
      m_Max[point] = m_Data[point];
      m_Avg[point] = m_Data[point];
      for(size_t j = 0; j < m_Depth; j++)
      {
        newPoint = static_cast<int32_t>(point + (j * m_Stride));
        val = m_Data[newPoint];
        if(val < m_Min[point])
        {
          m_Min[point] = val;
        }
        if(val > m_Max[point])
        {
          m_Max[point] = val;
        }
        m_Avg[point] += val;
      }
      m_Avg[point] /= m_Depth;
      for(size_t j = 0; j < m_Depth; j++)
      {
        newPoint = static_cast<int32_t>(point + (j * m_Stride));
        val = m_Data[newPoint];
        m_Min[newPoint] = m_Min[point];
        m_Max[newPoint] = m_Max[point];
        m_Avg[newPoint] = m_Avg[point];
        m_Std[point] += ((val - m_Avg[point]) * (val - m_Avg[point]));
      }
      m_Std[point] /= m_Depth;
      m_Var[point] = m_Std[point];
      m_Std[point] = sqrt(m_Std[point]);
      for(size_t j = 0; j < m_Depth; j++)
      {
        newPoint = point + (j * m_Stride);
        m_Std[newPoint] = m_Std[point];
        m_Var[newPoint] = m_Var[point];
      }
    }
  }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  void operator()(const tbb::blocked_range<size_t>& r) const
  {
    convert(r.begin(), r.end());
  }
#endif
private:
  T* m_Data;
  float* m_Min;
  float* m_Max;
  float* m_Avg;
  float* m_Std;
  float* m_Var;
  int32_t* m_StartPoints;
  size_t m_Stride;
  size_t m_Depth;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindProjectedImageStatistics::FindProjectedImageStatistics() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindProjectedImageStatistics::~FindProjectedImageStatistics() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindProjectedImageStatistics::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Plane of Interest");
    parameter->setPropertyName("Plane");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(FindProjectedImageStatistics, this, Plane));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(FindProjectedImageStatistics, this, Plane));

    std::vector<QString> choices;
    choices.push_back("XY");
    choices.push_back("XZ");
    choices.push_back("YZ");
    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Category::Parameter);
    parameters.push_back(parameter);
  }
  parameters.push_back(SeparatorFilterParameter::Create("Cell Data", FilterParameter::Category::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::Defaults::AnyPrimitive, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    std::vector<QString> daTypes;
    daTypes.push_back(SIMPL::TypeNames::Int8);
    daTypes.push_back(SIMPL::TypeNames::Int16);
    daTypes.push_back(SIMPL::TypeNames::Int32);
    daTypes.push_back(SIMPL::TypeNames::Int64);
    daTypes.push_back(SIMPL::TypeNames::UInt8);
    daTypes.push_back(SIMPL::TypeNames::UInt16);
    daTypes.push_back(SIMPL::TypeNames::UInt32);
    daTypes.push_back(SIMPL::TypeNames::UInt64);
    daTypes.push_back(SIMPL::TypeNames::Float);
    daTypes.push_back(SIMPL::TypeNames::Double);
    req.daTypes = daTypes;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Attribute Array to Quantify", SelectedArrayPath, FilterParameter::Category::RequiredArray, FindProjectedImageStatistics, req));
  }
  parameters.push_back(SeparatorFilterParameter::Create("Cell Data", FilterParameter::Category::CreatedArray));
  parameters.push_back(
      SIMPL_NEW_DA_WITH_LINKED_AM_FP("Projected Image Min", ProjectedImageMinArrayName, SelectedArrayPath, SelectedArrayPath, FilterParameter::Category::CreatedArray, FindProjectedImageStatistics));
  parameters.push_back(
      SIMPL_NEW_DA_WITH_LINKED_AM_FP("Projected Image Max", ProjectedImageMaxArrayName, SelectedArrayPath, SelectedArrayPath, FilterParameter::Category::CreatedArray, FindProjectedImageStatistics));
  parameters.push_back(
      SIMPL_NEW_DA_WITH_LINKED_AM_FP("Projected Image Avg", ProjectedImageAvgArrayName, SelectedArrayPath, SelectedArrayPath, FilterParameter::Category::CreatedArray, FindProjectedImageStatistics));
  parameters.push_back(
      SIMPL_NEW_DA_WITH_LINKED_AM_FP("Projected Image Std", ProjectedImageStdArrayName, SelectedArrayPath, SelectedArrayPath, FilterParameter::Category::CreatedArray, FindProjectedImageStatistics));
  parameters.push_back(
      SIMPL_NEW_DA_WITH_LINKED_AM_FP("Projected Image Var", ProjectedImageVarArrayName, SelectedArrayPath, SelectedArrayPath, FilterParameter::Category::CreatedArray, FindProjectedImageStatistics));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void FindProjectedImageStatistics::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setProjectedImageVarArrayName(reader->readString("ProjectedImageVarArrayName", getProjectedImageVarArrayName()));
  setProjectedImageStdArrayName(reader->readString("ProjectedImageStdArrayName", getProjectedImageStdArrayName()));
  setProjectedImageAvgArrayName(reader->readString("ProjectedImageAvgArrayName", getProjectedImageAvgArrayName()));
  setProjectedImageMaxArrayName(reader->readString("ProjectedImageMaxArrayName", getProjectedImageMaxArrayName()));
  setProjectedImageMinArrayName(reader->readString("ProjectedImageMinArrayName", getProjectedImageMinArrayName()));
  setSelectedArrayPath(reader->readDataArrayPath("SelectedArrayPath", getSelectedArrayPath()));
  setPlane(reader->readValue("Plane", getPlane()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindProjectedImageStatistics::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindProjectedImageStatistics::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  DataArrayPath tempPath;

  m_InDataPtr = getDataContainerArray()->getPrereqIDataArrayFromPath(this, getSelectedArrayPath());
  if(nullptr != m_InDataPtr.lock())
  {
    if(TemplateHelpers::CanDynamicCast<BoolArrayType>()(m_InDataPtr.lock()))
    {
      QString ss = QObject::tr("Selected array cannot be of type bool.  The path is %1").arg(getSelectedArrayPath().serialize());
      setErrorCondition(-11001, ss);
    }
  }

  std::vector<size_t> cDims(1, 1);
  tempPath.update(getSelectedArrayPath().getDataContainerName(), getSelectedArrayPath().getAttributeMatrixName(), getProjectedImageMinArrayName());
  m_ProjectedImageMinPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>>(this, tempPath, 0, cDims);
  if(nullptr != m_ProjectedImageMinPtr.lock())
  {
    m_ProjectedImageMin = m_ProjectedImageMinPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getSelectedArrayPath().getDataContainerName(), getSelectedArrayPath().getAttributeMatrixName(), getProjectedImageMaxArrayName());
  m_ProjectedImageMaxPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>>(this, tempPath, 0, cDims);
  if(nullptr != m_ProjectedImageMaxPtr.lock())
  {
    m_ProjectedImageMax = m_ProjectedImageMaxPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getSelectedArrayPath().getDataContainerName(), getSelectedArrayPath().getAttributeMatrixName(), getProjectedImageAvgArrayName());
  m_ProjectedImageAvgPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>>(this, tempPath, 0, cDims);
  if(nullptr != m_ProjectedImageAvgPtr.lock())
  {
    m_ProjectedImageAvg = m_ProjectedImageAvgPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getSelectedArrayPath().getDataContainerName(), getSelectedArrayPath().getAttributeMatrixName(), getProjectedImageStdArrayName());
  m_ProjectedImageStdPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>>(this, tempPath, 0, cDims);
  if(nullptr != m_ProjectedImageStdPtr.lock())
  {
    m_ProjectedImageStd = m_ProjectedImageStdPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getSelectedArrayPath().getDataContainerName(), getSelectedArrayPath().getAttributeMatrixName(), getProjectedImageVarArrayName());
  m_ProjectedImageVarPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>>(this, tempPath, 0, cDims);
  if(nullptr != m_ProjectedImageVarPtr.lock())
  {
    m_ProjectedImageVar = m_ProjectedImageVarPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  ImageGeom::Pointer image = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom>(this, getSelectedArrayPath().getDataContainerName());
  if(getErrorCode() < 0)
  {
    return;
  }

  if(image->getXPoints() <= 1 || image->getYPoints() <= 1 || image->getZPoints() <= 1)
  {
    setErrorCondition(-999, "The Image Geometry is not 3D and cannot be run through this Filter");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindProjectedImageStatistics::execute()
{
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getSelectedArrayPath().getDataContainerName());

  SizeVec3Type geoDims = m->getGeometryAs<ImageGeom>()->getDimensions();

  Int32ArrayType::Pointer startingPoints = Int32ArrayType::CreateArray(0, std::string("_INTERNAL_USE_ONLY_startingPoints"), true);
  int32_t* startPoints = nullptr;
  size_t stride = 0, yStride = 0;
  size_t count = 0;
  size_t depth = 0;
  if(m_Plane == 0)
  {
    startingPoints->resizeTuples(geoDims[0] * geoDims[1]);
    startPoints = startingPoints->getPointer(0);
    stride = geoDims[0] * geoDims[1];
    depth = geoDims[2];
    for(size_t i = 0; i < geoDims[1]; i++)
    {
      yStride = i * geoDims[0];
      for(size_t j = 0; j < geoDims[0]; j++)
      {
        startPoints[count] = yStride + j;
        count++;
      }
    }
  }
  if(m_Plane == 1)
  {
    startingPoints->resizeTuples(geoDims[0] * geoDims[2]);
    startPoints = startingPoints->getPointer(0);
    stride = geoDims[0];
    depth = geoDims[1];
    for(size_t i = 0; i < geoDims[2]; i++)
    {
      yStride = i * geoDims[0] * geoDims[1];
      for(size_t j = 0; j < geoDims[0]; j++)
      {
        startPoints[count] = yStride + j;
        count++;
      }
    }
  }
  if(m_Plane == 2)
  {
    startingPoints->resizeTuples(geoDims[1] * geoDims[2]);
    startPoints = startingPoints->getPointer(0);
    stride = 1;
    depth = geoDims[0];
    for(size_t i = 0; i < geoDims[2]; i++)
    {
      yStride = i * geoDims[0] * geoDims[1];
      for(size_t j = 0; j < geoDims[1]; j++)
      {
        startPoints[count] = yStride + (j * geoDims[0]);
        count++;
      }
    }
  }

  if(nullptr == startPoints)
  {
    QString ss = QObject::tr("Unable to establish starting location for supplied plane. The plane is %1").arg(m_Plane);
    setErrorCondition(-11001, ss);
    return;
  }

  if(TemplateHelpers::CanDynamicCast<Int8ArrayType>()(m_InDataPtr.lock()))
  {
    Int8ArrayType::Pointer cellArray = std::dynamic_pointer_cast<Int8ArrayType>(m_InDataPtr.lock());
    int8_t* cPtr = cellArray->getPointer(0);
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
    tbb::parallel_for(tbb::blocked_range<size_t>(0, count),
                      CalcProjectedStatsImpl<int8_t>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth),
                      tbb::auto_partitioner());

#else
    CalcProjectedStatsImpl<int8_t> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
    serial.convert(0, count);
#endif
  }
  else if(TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(m_InDataPtr.lock()))
  {
    UInt8ArrayType::Pointer cellArray = std::dynamic_pointer_cast<UInt8ArrayType>(m_InDataPtr.lock());
    uint8_t* cPtr = cellArray->getPointer(0);
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
    tbb::parallel_for(tbb::blocked_range<size_t>(0, count),
                      CalcProjectedStatsImpl<uint8_t>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth),
                      tbb::auto_partitioner());

#else
    CalcProjectedStatsImpl<uint8_t> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
    serial.convert(0, count);
#endif
  }
  else if(TemplateHelpers::CanDynamicCast<Int16ArrayType>()(m_InDataPtr.lock()))
  {
    Int16ArrayType::Pointer cellArray = std::dynamic_pointer_cast<Int16ArrayType>(m_InDataPtr.lock());
    int16_t* cPtr = cellArray->getPointer(0);
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
    tbb::parallel_for(tbb::blocked_range<size_t>(0, count),
                      CalcProjectedStatsImpl<int16_t>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth),
                      tbb::auto_partitioner());
#else
    CalcProjectedStatsImpl<int16_t> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
    serial.convert(0, count);
#endif
  }
  else if(TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(m_InDataPtr.lock()))
  {
    UInt16ArrayType::Pointer cellArray = std::dynamic_pointer_cast<UInt16ArrayType>(m_InDataPtr.lock());
    uint16_t* cPtr = cellArray->getPointer(0);
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
    tbb::parallel_for(tbb::blocked_range<size_t>(0, count),
                      CalcProjectedStatsImpl<uint16_t>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth),
                      tbb::auto_partitioner());
#else
    CalcProjectedStatsImpl<uint16_t> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
    serial.convert(0, count);
#endif
  }
  else if(TemplateHelpers::CanDynamicCast<Int32ArrayType>()(m_InDataPtr.lock()))
  {
    Int32ArrayType::Pointer cellArray = std::dynamic_pointer_cast<Int32ArrayType>(m_InDataPtr.lock());
    int32_t* cPtr = cellArray->getPointer(0);
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
    tbb::parallel_for(tbb::blocked_range<size_t>(0, count),
                      CalcProjectedStatsImpl<int32_t>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth),
                      tbb::auto_partitioner());

#else
    CalcProjectedStatsImpl<int32_t> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
    serial.convert(0, count);
#endif
  }
  else if(TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(m_InDataPtr.lock()))
  {
    UInt32ArrayType::Pointer cellArray = std::dynamic_pointer_cast<UInt32ArrayType>(m_InDataPtr.lock());
    uint32_t* cPtr = cellArray->getPointer(0);
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
    tbb::parallel_for(tbb::blocked_range<size_t>(0, count),
                      CalcProjectedStatsImpl<uint32_t>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth),
                      tbb::auto_partitioner());

#else
    CalcProjectedStatsImpl<uint32_t> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
    serial.convert(0, count);
#endif
  }
  else if(TemplateHelpers::CanDynamicCast<Int64ArrayType>()(m_InDataPtr.lock()))
  {
    Int64ArrayType::Pointer cellArray = std::dynamic_pointer_cast<Int64ArrayType>(m_InDataPtr.lock());
    int64_t* cPtr = cellArray->getPointer(0);
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
    tbb::parallel_for(tbb::blocked_range<size_t>(0, count),
                      CalcProjectedStatsImpl<int64_t>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth),
                      tbb::auto_partitioner());

#else
    CalcProjectedStatsImpl<int64_t> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
    serial.convert(0, count);
#endif
  }
  else if(TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(m_InDataPtr.lock()))
  {
    UInt64ArrayType::Pointer cellArray = std::dynamic_pointer_cast<UInt64ArrayType>(m_InDataPtr.lock());
    uint64_t* cPtr = cellArray->getPointer(0);
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
    tbb::parallel_for(tbb::blocked_range<size_t>(0, count),
                      CalcProjectedStatsImpl<uint64_t>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth),
                      tbb::auto_partitioner());
#else
    CalcProjectedStatsImpl<uint64_t> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
    serial.convert(0, count);
#endif
  }
  else if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(m_InDataPtr.lock()))
  {
    FloatArrayType::Pointer cellArray = std::dynamic_pointer_cast<FloatArrayType>(m_InDataPtr.lock());
    float* cPtr = cellArray->getPointer(0);
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
    tbb::parallel_for(tbb::blocked_range<size_t>(0, count),
                      CalcProjectedStatsImpl<float>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth),
                      tbb::auto_partitioner());
#else
    CalcProjectedStatsImpl<float> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
    serial.convert(0, count);
#endif
  }
  else if(TemplateHelpers::CanDynamicCast<DoubleArrayType>()(m_InDataPtr.lock()))
  {
    DoubleArrayType::Pointer cellArray = std::dynamic_pointer_cast<DoubleArrayType>(m_InDataPtr.lock());
    double* cPtr = cellArray->getPointer(0);
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
    tbb::parallel_for(tbb::blocked_range<size_t>(0, count),
                      CalcProjectedStatsImpl<double>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth),
                      tbb::auto_partitioner());
#else
    CalcProjectedStatsImpl<double> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
    serial.convert(0, count);
#endif
  }
  else
  {
    QString ss = QObject::tr("Selected array is of unsupported type. The type is %1").arg(m_InDataPtr.lock()->getTypeAsString());
    setErrorCondition(-11001, ss);
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindProjectedImageStatistics::newFilterInstance(bool copyFilterParameters) const
{
  FindProjectedImageStatistics::Pointer filter = FindProjectedImageStatistics::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindProjectedImageStatistics::getCompiledLibraryName() const
{
  return ProcessingConstants::ProcessingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindProjectedImageStatistics::getBrandingString() const
{
  return "Processing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindProjectedImageStatistics::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Processing::Version::Major() << "." << Processing::Version::Minor() << "." << Processing::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindProjectedImageStatistics::getGroupName() const
{
  return SIMPL::FilterGroups::ProcessingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid FindProjectedImageStatistics::getUuid() const
{
  return QUuid("{577dfdf6-02f8-5284-b45b-e31f5392a191}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindProjectedImageStatistics::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::ImageFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindProjectedImageStatistics::getHumanLabel() const
{
  return "Find Projected Image Statistics";
}

// -----------------------------------------------------------------------------
FindProjectedImageStatistics::Pointer FindProjectedImageStatistics::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<FindProjectedImageStatistics> FindProjectedImageStatistics::New()
{
  struct make_shared_enabler : public FindProjectedImageStatistics
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString FindProjectedImageStatistics::getNameOfClass() const
{
  return QString("FindProjectedImageStatistics");
}

// -----------------------------------------------------------------------------
QString FindProjectedImageStatistics::ClassName()
{
  return QString("FindProjectedImageStatistics");
}

// -----------------------------------------------------------------------------
void FindProjectedImageStatistics::setSelectedArrayPath(const DataArrayPath& value)
{
  m_SelectedArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindProjectedImageStatistics::getSelectedArrayPath() const
{
  return m_SelectedArrayPath;
}

// -----------------------------------------------------------------------------
void FindProjectedImageStatistics::setPlane(unsigned int value)
{
  m_Plane = value;
}

// -----------------------------------------------------------------------------
unsigned int FindProjectedImageStatistics::getPlane() const
{
  return m_Plane;
}

// -----------------------------------------------------------------------------
void FindProjectedImageStatistics::setProjectedImageMinArrayName(const QString& value)
{
  m_ProjectedImageMinArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindProjectedImageStatistics::getProjectedImageMinArrayName() const
{
  return m_ProjectedImageMinArrayName;
}

// -----------------------------------------------------------------------------
void FindProjectedImageStatistics::setProjectedImageMaxArrayName(const QString& value)
{
  m_ProjectedImageMaxArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindProjectedImageStatistics::getProjectedImageMaxArrayName() const
{
  return m_ProjectedImageMaxArrayName;
}

// -----------------------------------------------------------------------------
void FindProjectedImageStatistics::setProjectedImageAvgArrayName(const QString& value)
{
  m_ProjectedImageAvgArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindProjectedImageStatistics::getProjectedImageAvgArrayName() const
{
  return m_ProjectedImageAvgArrayName;
}

// -----------------------------------------------------------------------------
void FindProjectedImageStatistics::setProjectedImageStdArrayName(const QString& value)
{
  m_ProjectedImageStdArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindProjectedImageStatistics::getProjectedImageStdArrayName() const
{
  return m_ProjectedImageStdArrayName;
}

// -----------------------------------------------------------------------------
void FindProjectedImageStatistics::setProjectedImageVarArrayName(const QString& value)
{
  m_ProjectedImageVarArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindProjectedImageStatistics::getProjectedImageVarArrayName() const
{
  return m_ProjectedImageVarArrayName;
}
