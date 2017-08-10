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

#include "FindProjectedImageStatistics.h"

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/TemplateHelpers.hpp"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
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
template <typename T> class CalcProjectedStatsImpl
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
  virtual ~CalcProjectedStatsImpl()
  {
  }

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

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
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

// Include the MOC generated file for this class
#include "moc_FindProjectedImageStatistics.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindProjectedImageStatistics::FindProjectedImageStatistics()
: AbstractFilter()
, m_SelectedArrayPath("", "", "")
, m_Plane(0)
, m_ProjectedImageMinArrayName(SIMPL::CellData::ProjectedImageMin)
, m_ProjectedImageMaxArrayName(SIMPL::CellData::ProjectedImageMax)
, m_ProjectedImageAvgArrayName(SIMPL::CellData::ProjectedImageAvg)
, m_ProjectedImageStdArrayName(SIMPL::CellData::ProjectedImageStd)
, m_ProjectedImageVarArrayName(SIMPL::CellData::ProjectedImageVar)
, m_InData(nullptr)
, m_ProjectedImageMin(nullptr)
, m_ProjectedImageMax(nullptr)
, m_ProjectedImageAvg(nullptr)
, m_ProjectedImageStd(nullptr)
, m_ProjectedImageVar(nullptr)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindProjectedImageStatistics::~FindProjectedImageStatistics()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindProjectedImageStatistics::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Plane of Interest");
    parameter->setPropertyName("Plane");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(FindProjectedImageStatistics, this, Plane));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(FindProjectedImageStatistics, this, Plane));

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
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::Defaults::AnyPrimitive, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    QVector<QString> daTypes;
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
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Attribute Array to Quantify", SelectedArrayPath, FilterParameter::RequiredArray, FindProjectedImageStatistics, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Projected Image Min", ProjectedImageMinArrayName, FilterParameter::CreatedArray, FindProjectedImageStatistics));
  parameters.push_back(SIMPL_NEW_STRING_FP("Projected Image Max", ProjectedImageMaxArrayName, FilterParameter::CreatedArray, FindProjectedImageStatistics));
  parameters.push_back(SIMPL_NEW_STRING_FP("Projected Image Avg", ProjectedImageAvgArrayName, FilterParameter::CreatedArray, FindProjectedImageStatistics));
  parameters.push_back(SIMPL_NEW_STRING_FP("Projected Image Std", ProjectedImageStdArrayName, FilterParameter::CreatedArray, FindProjectedImageStatistics));
  parameters.push_back(SIMPL_NEW_STRING_FP("Projected Image Var", ProjectedImageVarArrayName, FilterParameter::CreatedArray, FindProjectedImageStatistics));
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
  setErrorCondition(0);
  setWarningCondition(0);

  DataArrayPath tempPath;

  m_InDataPtr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getSelectedArrayPath());
  if(nullptr != m_InDataPtr.lock())
  {
    if(TemplateHelpers::CanDynamicCast<BoolArrayType>()(m_InDataPtr.lock()))
    {
      QString ss = QObject::tr("Selected array cannot be of type bool.  The path is %1").arg(getSelectedArrayPath().serialize());
      setErrorCondition(-11001);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
  }

  QVector<size_t> cDims(1, 1);
  tempPath.update(getSelectedArrayPath().getDataContainerName(), getSelectedArrayPath().getAttributeMatrixName(), getProjectedImageMinArrayName());
  m_ProjectedImageMinPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
      this, tempPath, 0, cDims);                     /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_ProjectedImageMinPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_ProjectedImageMin = m_ProjectedImageMinPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getSelectedArrayPath().getDataContainerName(), getSelectedArrayPath().getAttributeMatrixName(), getProjectedImageMaxArrayName());
  m_ProjectedImageMaxPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
      this, tempPath, 0, cDims);                     /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_ProjectedImageMaxPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_ProjectedImageMax = m_ProjectedImageMaxPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getSelectedArrayPath().getDataContainerName(), getSelectedArrayPath().getAttributeMatrixName(), getProjectedImageAvgArrayName());
  m_ProjectedImageAvgPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
      this, tempPath, 0, cDims);                     /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_ProjectedImageAvgPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_ProjectedImageAvg = m_ProjectedImageAvgPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getSelectedArrayPath().getDataContainerName(), getSelectedArrayPath().getAttributeMatrixName(), getProjectedImageStdArrayName());
  m_ProjectedImageStdPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
      this, tempPath, 0, cDims);                     /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_ProjectedImageStdPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_ProjectedImageStd = m_ProjectedImageStdPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getSelectedArrayPath().getDataContainerName(), getSelectedArrayPath().getAttributeMatrixName(), getProjectedImageVarArrayName());
  m_ProjectedImageVarPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
      this, tempPath, 0, cDims);                     /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_ProjectedImageVarPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_ProjectedImageVar = m_ProjectedImageVarPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  ImageGeom::Pointer image = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getSelectedArrayPath().getDataContainerName());
  if(getErrorCondition() < 0)
  {
    return;
  }

  if(image->getXPoints() <= 1 || image->getYPoints() <= 1 || image->getZPoints() <= 1)
  {
    setErrorCondition(-999);
    notifyErrorMessage(getHumanLabel(), "The Image Geometry is not 3D and cannot be run through this Filter", getErrorCondition());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindProjectedImageStatistics::preflight()
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
void FindProjectedImageStatistics::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getSelectedArrayPath().getDataContainerName());

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  size_t xP = 0, yP = 0, zP = 0;
  m->getGeometryAs<ImageGeom>()->getDimensions(xP, yP, zP);

  Int32ArrayType::Pointer startingPoints = Int32ArrayType::CreateArray(0, "_INTERNAL_USE_ONLY_startingPoints");
  int32_t* startPoints = nullptr;
  size_t stride = 0, yStride = 0;
  size_t count = 0;
  size_t depth = 0;
  if(m_Plane == 0)
  {
    startingPoints->resize(xP * yP);
    startPoints = startingPoints->getPointer(0);
    stride = xP * yP;
    depth = zP;
    for(size_t i = 0; i < yP; i++)
    {
      yStride = i * xP;
      for(size_t j = 0; j < xP; j++)
      {
        startPoints[count] = yStride + j;
        count++;
      }
    }
  }
  if(m_Plane == 1)
  {
    startingPoints->resize(xP * zP);
    startPoints = startingPoints->getPointer(0);
    stride = xP;
    depth = yP;
    for(size_t i = 0; i < zP; i++)
    {
      yStride = i * xP * yP;
      for(size_t j = 0; j < xP; j++)
      {
        startPoints[count] = yStride + j;
        count++;
      }
    }
  }
  if(m_Plane == 2)
  {
    startingPoints->resize(yP * zP);
    startPoints = startingPoints->getPointer(0);
    stride = 1;
    depth = xP;
    for(size_t i = 0; i < zP; i++)
    {
      yStride = i * xP * yP;
      for(size_t j = 0; j < yP; j++)
      {
        startPoints[count] = yStride + (j * xP);
        count++;
      }
    }
  }

  if(nullptr == startPoints)
  {
    QString ss = QObject::tr("Unable to establish starting location for supplied plane. The plane is %1").arg(m_Plane);
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if(TemplateHelpers::CanDynamicCast<Int8ArrayType>()(m_InDataPtr.lock()))
  {
    Int8ArrayType::Pointer cellArray = std::dynamic_pointer_cast<Int8ArrayType>(m_InDataPtr.lock());
    int8_t* cPtr = cellArray->getPointer(0);
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    if(doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, count),
                        CalcProjectedStatsImpl<int8_t>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth),
                        tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcProjectedStatsImpl<int8_t> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
      serial.convert(0, count);
    }
  }
  else if(TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(m_InDataPtr.lock()))
  {
    UInt8ArrayType::Pointer cellArray = std::dynamic_pointer_cast<UInt8ArrayType>(m_InDataPtr.lock());
    uint8_t* cPtr = cellArray->getPointer(0);
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    if(doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, count),
                        CalcProjectedStatsImpl<uint8_t>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth),
                        tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcProjectedStatsImpl<uint8_t> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
      serial.convert(0, count);
    }
  }
  else if(TemplateHelpers::CanDynamicCast<Int16ArrayType>()(m_InDataPtr.lock()))
  {
    Int16ArrayType::Pointer cellArray = std::dynamic_pointer_cast<Int16ArrayType>(m_InDataPtr.lock());
    int16_t* cPtr = cellArray->getPointer(0);
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    if(doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, count),
                        CalcProjectedStatsImpl<int16_t>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth),
                        tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcProjectedStatsImpl<int16_t> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
      serial.convert(0, count);
    }
  }
  else if(TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(m_InDataPtr.lock()))
  {
    UInt16ArrayType::Pointer cellArray = std::dynamic_pointer_cast<UInt16ArrayType>(m_InDataPtr.lock());
    uint16_t* cPtr = cellArray->getPointer(0);
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    if(doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, count),
                        CalcProjectedStatsImpl<uint16_t>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth),
                        tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcProjectedStatsImpl<uint16_t> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
      serial.convert(0, count);
    }
  }
  else if(TemplateHelpers::CanDynamicCast<Int32ArrayType>()(m_InDataPtr.lock()))
  {
    Int32ArrayType::Pointer cellArray = std::dynamic_pointer_cast<Int32ArrayType>(m_InDataPtr.lock());
    int32_t* cPtr = cellArray->getPointer(0);
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    if(doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, count),
                        CalcProjectedStatsImpl<int32_t>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth),
                        tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcProjectedStatsImpl<int32_t> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
      serial.convert(0, count);
    }
  }
  else if(TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(m_InDataPtr.lock()))
  {
    UInt32ArrayType::Pointer cellArray = std::dynamic_pointer_cast<UInt32ArrayType>(m_InDataPtr.lock());
    uint32_t* cPtr = cellArray->getPointer(0);
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    if(doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, count),
                        CalcProjectedStatsImpl<uint32_t>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth),
                        tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcProjectedStatsImpl<uint32_t> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
      serial.convert(0, count);
    }
  }
  else if(TemplateHelpers::CanDynamicCast<Int64ArrayType>()(m_InDataPtr.lock()))
  {
    Int64ArrayType::Pointer cellArray = std::dynamic_pointer_cast<Int64ArrayType>(m_InDataPtr.lock());
    int64_t* cPtr = cellArray->getPointer(0);
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    if(doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, count),
                        CalcProjectedStatsImpl<int64_t>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth),
                        tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcProjectedStatsImpl<int64_t> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
      serial.convert(0, count);
    }
  }
  else if(TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(m_InDataPtr.lock()))
  {
    UInt64ArrayType::Pointer cellArray = std::dynamic_pointer_cast<UInt64ArrayType>(m_InDataPtr.lock());
    uint64_t* cPtr = cellArray->getPointer(0);
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    if(doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, count),
                        CalcProjectedStatsImpl<uint64_t>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth),
                        tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcProjectedStatsImpl<uint64_t> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
      serial.convert(0, count);
    }
  }
  else if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(m_InDataPtr.lock()))
  {
    FloatArrayType::Pointer cellArray = std::dynamic_pointer_cast<FloatArrayType>(m_InDataPtr.lock());
    float* cPtr = cellArray->getPointer(0);
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    if(doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, count),
                        CalcProjectedStatsImpl<float>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth),
                        tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcProjectedStatsImpl<float> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
      serial.convert(0, count);
    }
  }
  else if(TemplateHelpers::CanDynamicCast<DoubleArrayType>()(m_InDataPtr.lock()))
  {
    DoubleArrayType::Pointer cellArray = std::dynamic_pointer_cast<DoubleArrayType>(m_InDataPtr.lock());
    double* cPtr = cellArray->getPointer(0);
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    if(doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, count),
                        CalcProjectedStatsImpl<double>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth),
                        tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcProjectedStatsImpl<double> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
      serial.convert(0, count);
    }
  }
  else
  {
    QString ss = QObject::tr("Selected array is of unsupported type. The type is %1").arg(m_InDataPtr.lock()->getTypeAsString());
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindProjectedImageStatistics::newFilterInstance(bool copyFilterParameters)
{
  FindProjectedImageStatistics::Pointer filter = FindProjectedImageStatistics::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindProjectedImageStatistics::getCompiledLibraryName()
{
  return ProcessingConstants::ProcessingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindProjectedImageStatistics::getBrandingString()
{
  return "Processing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindProjectedImageStatistics::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << Processing::Version::Major() << "." << Processing::Version::Minor() << "." << Processing::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindProjectedImageStatistics::getGroupName()
{
  return SIMPL::FilterGroups::ProcessingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindProjectedImageStatistics::getSubGroupName()
{
  return SIMPL::FilterSubGroups::ImageFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindProjectedImageStatistics::getHumanLabel()
{
  return "Find Projected Image Statistics";
}
