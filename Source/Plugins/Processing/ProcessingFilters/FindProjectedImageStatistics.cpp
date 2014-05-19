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

#include "FindProjectedImageStatistics.h"

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

template<typename T>
class CalcProjectedStatsImpl
{

  public:
    CalcProjectedStatsImpl(T* data, float* min, float* max, float* avg, float* std, float* var, int32_t* startPoints, size_t stride, size_t depth) :
      m_Data(data),
      m_Min(min),
      m_Max(max),
      m_Avg(avg),
      m_Std(std),
      m_Var(var),
      m_StartPoints(startPoints),
      m_Stride(stride),
      m_Depth(depth)
    {}
    virtual ~CalcProjectedStatsImpl() {}

    void convert(size_t start, size_t end) const
    {
      int32_t point, newPoint;
      T val;
      for (size_t i = start; i < end; i++)
      {
        point = m_StartPoints[i];
        m_Min[point] = m_Data[point];
        m_Max[point] = m_Data[point];
        m_Avg[point] = m_Data[point];
        for(size_t j = 0; j < m_Depth; j++)
        {
          newPoint = point + (j * m_Stride);
          val = m_Data[newPoint];
          if(val < m_Min[point]) { m_Min[point] = val; }
          if(val > m_Max[point]) { m_Max[point] = val; }
          m_Avg[point] += val;
        }
        m_Avg[point] /= m_Depth;
        for(size_t j = 0; j < m_Depth; j++)
        {
          newPoint = point + (j * m_Stride);
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

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
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
FindProjectedImageStatistics::FindProjectedImageStatistics() :
  AbstractFilter(),
  m_SelectedArrayPath("", "", ""),
  m_Plane(0),
  m_ProjectedImageMinArrayName(DREAM3D::CellData::ProjectedImageMin),
  m_ProjectedImageMaxArrayName(DREAM3D::CellData::ProjectedImageMax),
  m_ProjectedImageAvgArrayName(DREAM3D::CellData::ProjectedImageAvg),
  m_ProjectedImageStdArrayName(DREAM3D::CellData::ProjectedImageStd),
  m_ProjectedImageVarArrayName(DREAM3D::CellData::ProjectedImageVar),
  m_ProjectedImageMin(NULL),
  m_ProjectedImageMax(NULL),
  m_ProjectedImageAvg(NULL),
  m_ProjectedImageStd(NULL),
  m_ProjectedImageVar(NULL)
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
  parameters.push_back(FilterParameter::New("Cell Array To Quantify", "SelectedArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", false));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Plane of Interest");
    parameter->setPropertyName("Plane");
    parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    //parameter->setValueType("unsigned int");
    QVector<QString> choices;
    choices.push_back("XY");
    choices.push_back("XZ");
    choices.push_back("YZ");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
  parameters.push_back(FilterParameter::New("ProjectedImageMin", "ProjectedImageMinArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  parameters.push_back(FilterParameter::New("ProjectedImageMax", "ProjectedImageMaxArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  parameters.push_back(FilterParameter::New("ProjectedImageAvg", "ProjectedImageAvgArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  parameters.push_back(FilterParameter::New("ProjectedImageStd", "ProjectedImageStdArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  parameters.push_back(FilterParameter::New("ProjectedImageVar", "ProjectedImageVarArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void FindProjectedImageStatistics::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setProjectedImageVarArrayName(reader->readString("ProjectedImageVarArrayName", getProjectedImageVarArrayName() ) );
  setProjectedImageStdArrayName(reader->readString("ProjectedImageStdArrayName", getProjectedImageStdArrayName() ) );
  setProjectedImageAvgArrayName(reader->readString("ProjectedImageAvgArrayName", getProjectedImageAvgArrayName() ) );
  setProjectedImageMaxArrayName(reader->readString("ProjectedImageMaxArrayName", getProjectedImageMaxArrayName() ) );
  setProjectedImageMinArrayName(reader->readString("ProjectedImageMinArrayName", getProjectedImageMinArrayName() ) );
  setSelectedArrayPath( reader->readDataArrayPath( "SelectedArrayPath", getSelectedArrayPath() ) );
  setPlane( reader->readValue("Plane", getPlane()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindProjectedImageStatistics::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("ProjectedImageVarArrayName", getProjectedImageVarArrayName() );
  writer->writeValue("ProjectedImageStdArrayName", getProjectedImageStdArrayName() );
  writer->writeValue("ProjectedImageAvgArrayName", getProjectedImageAvgArrayName() );
  writer->writeValue("ProjectedImageMaxArrayName", getProjectedImageMaxArrayName() );
  writer->writeValue("ProjectedImageMinArrayName", getProjectedImageMinArrayName() );
  writer->writeValue("SelectedArrayPath", getSelectedArrayPath() );
  writer->writeValue("Plane", getPlane());
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindProjectedImageStatistics::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  QVector<size_t> dims(1, 1);
  tempPath.update(getSelectedArrayPath().getDataContainerName(), getSelectedArrayPath().getAttributeMatrixName(), getProjectedImageMinArrayName() );
  m_ProjectedImageMinPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ProjectedImageMinPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_ProjectedImageMin = m_ProjectedImageMinPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getSelectedArrayPath().getDataContainerName(), getSelectedArrayPath().getAttributeMatrixName(), getProjectedImageMaxArrayName() );
  m_ProjectedImageMaxPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ProjectedImageMaxPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_ProjectedImageMax = m_ProjectedImageMaxPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getSelectedArrayPath().getDataContainerName(), getSelectedArrayPath().getAttributeMatrixName(), getProjectedImageAvgArrayName() );
  m_ProjectedImageAvgPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ProjectedImageAvgPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_ProjectedImageAvg = m_ProjectedImageAvgPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getSelectedArrayPath().getDataContainerName(), getSelectedArrayPath().getAttributeMatrixName(), getProjectedImageStdArrayName() );
  m_ProjectedImageStdPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ProjectedImageStdPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_ProjectedImageStd = m_ProjectedImageStdPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getSelectedArrayPath().getDataContainerName(), getSelectedArrayPath().getAttributeMatrixName(), getProjectedImageVarArrayName() );
  m_ProjectedImageVarPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ProjectedImageVarPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_ProjectedImageVar = m_ProjectedImageVarPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindProjectedImageStatistics::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindProjectedImageStatistics::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  QString dcName = m_SelectedArrayPath.getDataContainerName();
  QString amName = m_SelectedArrayPath.getAttributeMatrixName();
  QString daName = m_SelectedArrayPath.getDataArrayName();

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(dcName);

  if(m->getXPoints() <= 1 && m->getYPoints() <= 1 && m->getZPoints() <= 1)
  {
    setErrorCondition(-999);
    notifyErrorMessage(getHumanLabel(), "The volume is not 3D and cannot be run through this filter", -999);
    return;
  }

  QString ss;

  // We the AttributeMatrix, now lets try to get the AttributeArray object and rename it if it exists
  IDataArray::Pointer inputData = getDataContainerArray()->getDataContainer(dcName)->getAttributeMatrix(amName)->getAttributeArray(daName);
  if (NULL == inputData.get())
  {
    ss = QObject::tr("Selected array with path: '%1' does not exist in the Data Container. Was it spelled correctly?").arg(getSelectedArrayPath().getDataArrayName());
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  size_t xP, yP, zP;
  m->getDimensions(xP, yP, zP);

  Int32ArrayType::Pointer startingPoints = Int32ArrayType::CreateArray(0, "startingPoints");
  int32_t* startPoints;
  size_t stride, yStride;
  size_t count = 0;
  size_t depth;
  QVector<size_t> cDims(1, 1);
  if(m_Plane == 0)
  {
    startingPoints->resize(xP * yP);
    startPoints = startingPoints->getPointer(0);
    stride = xP * yP;
    depth = zP;
    for(int i = 0; i < yP; i++)
    {
      yStride = i * xP;
      for(int j = 0; j < xP; j++)
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
    for(int i = 0; i < zP; i++)
    {
      yStride = i * xP * yP;
      for(int j = 0; j < xP; j++)
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
    for(int i = 0; i < zP; i++)
    {
      yStride = i * xP * yP;
      for(int j = 0; j < yP; j++)
      {
        startPoints[count] = yStride + (j * xP);
        count++;
      }
    }
  }

  QString dType = inputData->getTypeAsString();
  if (dType.compare("int8_t") == 0)
  {
    DataArray<int8_t>* cellArray = DataArray<int8_t>::SafePointerDownCast(inputData.get());
    if (NULL == cellArray) { return; }
    int8_t* cPtr = cellArray->getPointer(0);
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, count), CalcProjectedStatsImpl<int8_t>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth), tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcProjectedStatsImpl<int8_t> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
      serial.convert(0, count);
    }
  }
  else if (dType.compare("uint8_t") == 0)
  {
    DataArray<uint8_t>* cellArray = DataArray<uint8_t>::SafePointerDownCast(inputData.get());
    if (NULL == cellArray) { return; }
    uint8_t* cPtr = cellArray->getPointer(0);
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, count), CalcProjectedStatsImpl<uint8_t>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth), tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcProjectedStatsImpl<uint8_t> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
      serial.convert(0, count);
    }
  }
  else if (dType.compare("int16_t") == 0)
  {
    DataArray<int16_t>* cellArray = DataArray<int16_t>::SafePointerDownCast(inputData.get());
    if (NULL == cellArray) { return; }
    int16_t* cPtr = cellArray->getPointer(0);
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, count), CalcProjectedStatsImpl<int16_t>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth), tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcProjectedStatsImpl<int16_t> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
      serial.convert(0, count);
    }
  }
  else if (dType.compare("uint16_t") == 0)
  {
    DataArray<uint16_t>* cellArray = DataArray<uint16_t>::SafePointerDownCast(inputData.get());
    if (NULL == cellArray) { return; }
    uint16_t* cPtr = cellArray->getPointer(0);
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, count), CalcProjectedStatsImpl<uint16_t>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth), tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcProjectedStatsImpl<uint16_t> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
      serial.convert(0, count);
    }
  }
  else if (dType.compare("int32_t") == 0)
  {
    DataArray<int32_t>* cellArray = DataArray<int32_t>::SafePointerDownCast(inputData.get());
    if (NULL == cellArray) { return; }
    int32_t* cPtr = cellArray->getPointer(0);
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, count), CalcProjectedStatsImpl<int32_t>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth), tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcProjectedStatsImpl<int32_t> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
      serial.convert(0, count);
    }
  }
  else if (dType.compare("uint32_t") == 0)
  {
    DataArray<uint32_t>* cellArray = DataArray<uint32_t>::SafePointerDownCast(inputData.get());
    if (NULL == cellArray) { return; }
    uint32_t* cPtr = cellArray->getPointer(0);
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, count), CalcProjectedStatsImpl<uint32_t>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth), tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcProjectedStatsImpl<uint32_t> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
      serial.convert(0, count);
    }
  }
  else if (dType.compare("int64_t") == 0)
  {
    DataArray<int64_t>* cellArray = DataArray<int64_t>::SafePointerDownCast(inputData.get());
    if (NULL == cellArray) { return; }
    int64_t* cPtr = cellArray->getPointer(0);
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, count), CalcProjectedStatsImpl<int64_t>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth), tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcProjectedStatsImpl<int64_t> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
      serial.convert(0, count);
    }
  }
  else if (dType.compare("uint64_t") == 0)
  {
    DataArray<uint64_t>* cellArray = DataArray<uint64_t>::SafePointerDownCast(inputData.get());
    if (NULL == cellArray) { return; }
    uint64_t* cPtr = cellArray->getPointer(0);
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, count), CalcProjectedStatsImpl<uint64_t>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth), tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcProjectedStatsImpl<uint64_t> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
      serial.convert(0, count);
    }
  }
  else if (dType.compare("float") == 0)
  {
    DataArray<float>* cellArray = DataArray<float>::SafePointerDownCast(inputData.get());
    if (NULL == cellArray) { return; }
    float* cPtr = cellArray->getPointer(0);
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, count), CalcProjectedStatsImpl<float>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth), tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcProjectedStatsImpl<float> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
      serial.convert(0, count);
    }
  }
  else if (dType.compare("double") == 0)
  {
    DataArray<double>* cellArray = DataArray<double>::SafePointerDownCast(inputData.get());
    if (NULL == cellArray) { return; }
    double* cPtr = cellArray->getPointer(0);
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, count), CalcProjectedStatsImpl<double>(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth), tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcProjectedStatsImpl<double> serial(cPtr, m_ProjectedImageMin, m_ProjectedImageMax, m_ProjectedImageAvg, m_ProjectedImageStd, m_ProjectedImageVar, startPoints, stride, depth);
      serial.convert(0, count);
    }
  }
  else if (dType.compare("bool") == 0)
  {
    ss = QObject::tr("Selected array with path: '%1' cannot be of type bool").arg(getSelectedArrayPath().getDataArrayName());
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  notifyStatusMessage(getHumanLabel(), "FindProjectedImageStatistics Completed");
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
