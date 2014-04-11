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

#include "FindRelativeMotionBetweenSlices.h"

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Common/Constants.h"

template<typename T>
class CalcRelativeMotion
{

  public:
    CalcRelativeMotion(T* data, float* motionDir, int32_t* patchPoints, int32_t* searchPoints, bool* validPoints, size_t numPP, size_t numSP) :
      m_Data(data),
      m_MotionDirection(motionDir),
      m_PatchPoints(patchPoints),
      m_SearchPoints(searchPoints),
      m_ValidPoints(validPoints),
      m_NumPatchPoints(numPP),
      m_NumSearchPoints(numSP)
    {}
    virtual ~CalcRelativeMotion() {}

    void convert(size_t start, size_t end) const
    {
      int32_t patchPoint, comparePoint;
      float val, minVal;
      for (size_t i = start; i < end; i++)
      {
        if(m_ValidPoints[i] == true)
        {
          minVal = 10000000000.0;
          for(size_t j = 0; j < m_NumSearchPoints; j++)
          {
            val = 0;
            for(size_t k = 0; k < m_NumPatchPoints; k++)
            {
              patchPoint = i + m_PatchPoints[k];
              comparePoint = patchPoint + m_SearchPoints[4*j];
              val += float((m_Data[patchPoint]-m_Data[comparePoint]))*float((m_Data[patchPoint]-m_Data[comparePoint]));
            }
            if(val < minVal)
            {
              minVal = val;
              m_MotionDirection[3*i+0] = m_SearchPoints[4*j+1];
              m_MotionDirection[3*i+1] = m_SearchPoints[4*j+2];
              m_MotionDirection[3*i+2] = m_SearchPoints[4*j+3];
            }
          }
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
    float* m_MotionDirection;
    int32_t* m_PatchPoints;
    int32_t* m_SearchPoints;
    bool* m_ValidPoints;
    size_t m_NumPatchPoints;
    size_t m_NumSearchPoints;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindRelativeMotionBetweenSlices::FindRelativeMotionBetweenSlices() :
  AbstractFilter(),
  m_SelectedArrayPath("", "", ""),
  m_Plane(0),
  m_MotionDirectionArrayName(DREAM3D::CellData::MotionDirection),
  m_MotionDirection(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindRelativeMotionBetweenSlices::~FindRelativeMotionBetweenSlices()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindRelativeMotionBetweenSlices::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Cell Array To Track Motion", "SelectedArrayPath", FilterParameterWidgetType::DataArraySelectionWidget,"DataArrayPath", false));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Plane of Interest");
    parameter->setPropertyName("Plane");
    parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    parameter->setValueType("unsigned int");
    QVector<QString> choices;
    choices.push_back("XY");
    choices.push_back("XZ");
    choices.push_back("YZ");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }
  parameters.push_back(FilterParameter::New("Patch Size 1 (Voxels)", "PSize1", FilterParameterWidgetType::IntWidget,"int", false));
  parameters.push_back(FilterParameter::New("Patch Size 2 (Voxels)", "PSize2", FilterParameterWidgetType::IntWidget,"int", false));
  parameters.push_back(FilterParameter::New("Search Distance 1 (Voxels)", "SSize1", FilterParameterWidgetType::IntWidget,"int", false));
  parameters.push_back(FilterParameter::New("Search Distance 2 (Voxels)", "SSize2", FilterParameterWidgetType::IntWidget,"int", false));
  parameters.push_back(FilterParameter::New("Slice Step (Voxels)", "SliceStep", FilterParameterWidgetType::IntWidget,"int", false));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
  parameters.push_back(FilterParameter::New("MotionDirection", "MotionDirectionArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void FindRelativeMotionBetweenSlices::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setMotionDirectionArrayName(reader->readString("MotionDirectionArrayName", getMotionDirectionArrayName() ) );
  setSelectedArrayPath( reader->readDataArrayPath( "SelectedArrayPath", getSelectedArrayPath() ) );
  setPlane( reader->readValue("Plane", getPlane()));
  setPSize1( reader->readValue("PSize1", getPSize1()));
  setPSize2( reader->readValue("PSize2", getPSize2()));
  setSSize1( reader->readValue("SSize1", getSSize1()));
  setSSize2( reader->readValue("SSize2", getSSize2()));
  setSliceStep( reader->readValue("SliceStep", getSliceStep()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindRelativeMotionBetweenSlices::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("MotionDirectionArrayName", getMotionDirectionArrayName() );
  writer->writeValue("SelectedArrayPath", getSelectedArrayPath() );
  writer->writeValue("Plane", getPlane());
  writer->writeValue("PSize1", getPSize1());
  writer->writeValue("PSize2", getPSize2());
  writer->writeValue("SSize1", getSSize1());
  writer->writeValue("SSize2", getSSize2());
  writer->writeValue("SliceStep", getSliceStep());
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindRelativeMotionBetweenSlices::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  QVector<size_t> dims(1, 3);
  tempPath.update(m_SelectedArrayPath.getDataContainerName(), m_SelectedArrayPath.getAttributeMatrixName(), getMotionDirectionArrayName() );
  m_MotionDirectionPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_MotionDirectionPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_MotionDirection = m_MotionDirectionPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindRelativeMotionBetweenSlices::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindRelativeMotionBetweenSlices::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_SelectedArrayPath.getDataContainerName());

  if(m->getXPoints() <= 1 && m->getYPoints() <= 1 && m->getZPoints() <= 1)
  {
    setErrorCondition(-999);
    notifyErrorMessage(getHumanLabel(), "The volume is not 3D and cannot be run through this filter", -999);
    return;
  }

  QString ss;

  QString dcName = m_SelectedArrayPath.getDataContainerName();
  QString amName = m_SelectedArrayPath.getAttributeMatrixName();
  QString daName = m_SelectedArrayPath.getDataArrayName();

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
  size_t totalPoints = xP*yP*zP;

  int32_t buffer1 = (m_PSize1/2)+(m_SSize1/2);
  int32_t buffer2 = (m_PSize2/2)+(m_SSize1/2);

  QVector<size_t> cDims(1, 4);
  Int32ArrayType::Pointer patchPointsPtr = Int32ArrayType::CreateArray((m_PSize1*m_PSize2), "patchPoints");
  Int32ArrayType::Pointer searchPointsPtr = Int32ArrayType::CreateArray((m_SSize1*m_SSize2), cDims, "searchPoints");
  BoolArrayType::Pointer validPointsPtr = BoolArrayType::CreateArray(totalPoints, "validPoints");
  validPointsPtr->initializeWithValue(false);
  int32_t* patchPoints = patchPointsPtr->getPointer(0);
  int32_t* searchPoints = searchPointsPtr->getPointer(0);
  bool* validPoints = validPointsPtr->getPointer(0);
  size_t yStride, zStride;
  size_t count = 0;
  size_t numPatchPoints, numSearchPoints;
  if(m_Plane == 0)
  {
    for(int j = -(m_PSize2/2); j < (m_PSize2/2); j++)
    {
      yStride = (j*xP);
      for(int i = -(m_PSize1/2); i < (m_PSize1/2); i++)
      {
        patchPoints[count] = yStride+i;
        count++;
      }
    }
    numPatchPoints = count;
    count = 0;
    for(int j = -(m_SSize2/2); j <= (m_SSize2/2); j++)
    {
      yStride = (j*xP);
      for(int i = -(m_SSize1/2); i <= (m_SSize1/2); i++)
      {
        searchPoints[4*count] = (m_SliceStep*xP*yP)+yStride+i;
        searchPoints[4*count+1] = i;
        searchPoints[4*count+2] = j;
        searchPoints[4*count+3] = m_SliceStep;
        count++;
      }
    }
    numSearchPoints = count;
    for(int k=0;k<zP-m_SliceStep;k++)
    {
      zStride = k*xP*yP;
      for(int j=buffer2;j<(yP-buffer2);j++)
      {
        yStride = j*xP;
        for(int i=buffer1;i<(xP-buffer1);i++)
        {
          validPoints[zStride+yStride+i] = true;
        }
      }
    }
  }
  if(m_Plane == 1)
  {
    for(int j = -(m_PSize2/2); j < (m_PSize2/2); j++)
    {
      yStride = (j*xP*yP);
      for(int i = -(m_PSize1/2); i < (m_PSize1/2); i++)
      {
        patchPoints[count] = yStride+i;
        count++;
      }
    }
    numPatchPoints = count;
    count = 0;
    for(int j = -(m_SSize2/2); j <= (m_SSize2/2); j++)
    {
      yStride = (j*xP*yP);
      for(int i = -(m_SSize1/2); i <= (m_SSize1/2); i++)
      {
        searchPoints[count] = (m_SliceStep*xP)+yStride+i;
        searchPoints[4*count+1] = i;
        searchPoints[4*count+2] = m_SliceStep;
        searchPoints[4*count+3] = j;
        count++;
      }
    }
    numSearchPoints = count;
    for(int k=buffer2;k<(zP-buffer2);k++)
    {
      zStride = k*xP*yP;
      for(int j=0;j<yP-m_SliceStep;j++)
      {
        yStride = j*xP;
        for(int i=buffer1;i<(xP-buffer1);i++)
        {
          validPoints[zStride+yStride+i] = true;
        }
      }
    }
  }
  if(m_Plane == 2)
  {
    for(int j = -(m_PSize2/2); j < (m_PSize2/2); j++)
    {
      yStride = (j*xP*yP);
      for(int i = -(m_PSize1/2); i < (m_PSize1/2); i++)
      {
        patchPoints[count] = yStride+(i*xP);
        count++;
      }
    }
    numPatchPoints = count;
    count = 0;
    for(int j = -(m_SSize2/2); j <= (m_SSize2/2); j++)
    {
      yStride = (j*xP*yP);
      for(int i = -(m_SSize1/2); i <= (m_SSize1/2); i++)
      {
        searchPoints[count] = (m_SliceStep)+yStride+(i*xP);
        searchPoints[4*count+1] = m_SliceStep;
        searchPoints[4*count+2] = i;
        searchPoints[4*count+3] = j;
        count++;
      }
    }
    numSearchPoints = count;
    for(int k=buffer2;k<(zP-buffer2);k++)
    {
      zStride = k*xP*yP;
      for(int j=buffer1;j<(yP-buffer1);j++)
      {
        yStride = j*xP;
        for(int i=0;i<xP-m_SliceStep;i++)
        {
          validPoints[zStride+yStride+i] = true;
        }
      }
    }
  }

  QString dType = inputData->getTypeAsString();
  if (dType.compare("int8_t") == 0)
  {
    DataArray<int8_t>* cellArray = DataArray<int8_t>::SafePointerDownCast(inputData.get());
    if (NULL == cellArray) return;
    int8_t* cPtr = cellArray->getPointer(0);
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints), CalcRelativeMotion<int8_t>(cPtr, m_MotionDirection, patchPoints, searchPoints, validPoints, numPatchPoints, numSearchPoints), tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcRelativeMotion<int8_t> serial(cPtr, m_MotionDirection, patchPoints, searchPoints, validPoints, numPatchPoints, numSearchPoints);
      serial.convert(0, totalPoints);
    }
  }
  else if (dType.compare("uint8_t") == 0)
  {
    DataArray<uint8_t>* cellArray = DataArray<uint8_t>::SafePointerDownCast(inputData.get());
    if (NULL == cellArray) return;
    uint8_t* cPtr = cellArray->getPointer(0);
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints), CalcRelativeMotion<uint8_t>(cPtr, m_MotionDirection, patchPoints, searchPoints, validPoints, numPatchPoints, numSearchPoints), tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcRelativeMotion<uint8_t> serial(cPtr, m_MotionDirection, patchPoints, searchPoints, validPoints, numPatchPoints, numSearchPoints);
      serial.convert(0, totalPoints);
    }
  }
  else if (dType.compare("int16_t") == 0)
  {
    DataArray<int16_t>* cellArray = DataArray<int16_t>::SafePointerDownCast(inputData.get());
    if (NULL == cellArray) return;
    int16_t* cPtr = cellArray->getPointer(0);
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints), CalcRelativeMotion<int16_t>(cPtr, m_MotionDirection, patchPoints, searchPoints, validPoints, numPatchPoints, numSearchPoints), tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcRelativeMotion<int16_t> serial(cPtr, m_MotionDirection, patchPoints, searchPoints, validPoints, numPatchPoints, numSearchPoints);
      serial.convert(0, totalPoints);
    }
  }
  else if (dType.compare("uint16_t") == 0)
  {
    DataArray<uint16_t>* cellArray = DataArray<uint16_t>::SafePointerDownCast(inputData.get());
    if (NULL == cellArray) return;
    uint16_t* cPtr = cellArray->getPointer(0);
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints), CalcRelativeMotion<uint16_t>(cPtr, m_MotionDirection, patchPoints, searchPoints, validPoints, numPatchPoints, numSearchPoints), tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcRelativeMotion<uint16_t> serial(cPtr, m_MotionDirection, patchPoints, searchPoints, validPoints, numPatchPoints, numSearchPoints);
      serial.convert(0, totalPoints);
    }
  }
  else if (dType.compare("int32_t") == 0)
  {
    DataArray<int32_t>* cellArray = DataArray<int32_t>::SafePointerDownCast(inputData.get());
    if (NULL == cellArray) return;
    int32_t* cPtr = cellArray->getPointer(0);
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints), CalcRelativeMotion<int32_t>(cPtr, m_MotionDirection, patchPoints, searchPoints, validPoints, numPatchPoints, numSearchPoints), tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcRelativeMotion<int32_t> serial(cPtr, m_MotionDirection, patchPoints, searchPoints, validPoints, numPatchPoints, numSearchPoints);
      serial.convert(0, totalPoints);
    }
  }
  else if (dType.compare("uint32_t") == 0)
  {
    DataArray<uint32_t>* cellArray = DataArray<uint32_t>::SafePointerDownCast(inputData.get());
    if (NULL == cellArray) return;
    uint32_t* cPtr = cellArray->getPointer(0);
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints), CalcRelativeMotion<uint32_t>(cPtr, m_MotionDirection, patchPoints, searchPoints, validPoints, numPatchPoints, numSearchPoints), tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcRelativeMotion<uint32_t> serial(cPtr, m_MotionDirection, patchPoints, searchPoints, validPoints, numPatchPoints, numSearchPoints);
      serial.convert(0, totalPoints);
    }
  }
  else if (dType.compare("int64_t") == 0)
  {
    DataArray<int64_t>* cellArray = DataArray<int64_t>::SafePointerDownCast(inputData.get());
    if (NULL == cellArray) return;
    int64_t* cPtr = cellArray->getPointer(0);
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints), CalcRelativeMotion<int64_t>(cPtr, m_MotionDirection, patchPoints, searchPoints, validPoints, numPatchPoints, numSearchPoints), tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcRelativeMotion<int64_t> serial(cPtr, m_MotionDirection, patchPoints, searchPoints, validPoints, numPatchPoints, numSearchPoints);
      serial.convert(0, totalPoints);
    }
  }
  else if (dType.compare("uint64_t") == 0)
  {
    DataArray<uint64_t>* cellArray = DataArray<uint64_t>::SafePointerDownCast(inputData.get());
    if (NULL == cellArray) return;
    uint64_t* cPtr = cellArray->getPointer(0);
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints), CalcRelativeMotion<uint64_t>(cPtr, m_MotionDirection, patchPoints, searchPoints, validPoints, numPatchPoints, numSearchPoints), tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcRelativeMotion<uint64_t> serial(cPtr, m_MotionDirection, patchPoints, searchPoints, validPoints, numPatchPoints, numSearchPoints);
      serial.convert(0, totalPoints);
    }
  }
  else if (dType.compare("float") == 0)
  {
    DataArray<float>* cellArray = DataArray<float>::SafePointerDownCast(inputData.get());
    if (NULL == cellArray) return;
    float* cPtr = cellArray->getPointer(0);
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints), CalcRelativeMotion<float>(cPtr, m_MotionDirection, patchPoints, searchPoints, validPoints, numPatchPoints, numSearchPoints), tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcRelativeMotion<float> serial(cPtr, m_MotionDirection, patchPoints, searchPoints, validPoints, numPatchPoints, numSearchPoints);
      serial.convert(0, totalPoints);
    }
  }
  else if (dType.compare("double") == 0)
  {
    DataArray<double>* cellArray = DataArray<double>::SafePointerDownCast(inputData.get());
    if (NULL == cellArray) return;
    double* cPtr = cellArray->getPointer(0);
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints), CalcRelativeMotion<double>(cPtr, m_MotionDirection, patchPoints, searchPoints, validPoints, numPatchPoints, numSearchPoints), tbb::auto_partitioner());
    }
    else
#endif
    {
      CalcRelativeMotion<double> serial(cPtr, m_MotionDirection, patchPoints, searchPoints, validPoints, numPatchPoints, numSearchPoints);
      serial.convert(0, totalPoints);
    }
  }
  else if (dType.compare("bool") == 0)
  {
    ss = QObject::tr("Selected array with path: '%1' cannot be of type bool").arg(getSelectedArrayPath().getDataArrayName());
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  float v[3];
  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();
  for(size_t i=0; i<totalPoints; i++)
  {
    v[0] = m_MotionDirection[3*i+0]*xRes;
    v[1] = m_MotionDirection[3*i+1]*yRes;
    v[2] = m_MotionDirection[3*i+2]*zRes;
    MatrixMath::Normalize3x1(v);
    m_MotionDirection[3*i+0] = v[0];
    m_MotionDirection[3*i+1] = v[1];
    m_MotionDirection[3*i+2] = v[2];
  }

  notifyStatusMessage(getHumanLabel(), "FindRelativeMotionBetweenSlices Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindRelativeMotionBetweenSlices::newFilterInstance(bool copyFilterParameters)
{
  FindRelativeMotionBetweenSlices::Pointer filter = FindRelativeMotionBetweenSlices::New();
  if(true == copyFilterParameters)
  {
    filter->setFilterParameters(getFilterParameters() );
    
    //Loop over each Filter Parameter that is registered to the filter either through this class or a parent class
    // and copy the value from the current instance of the object into the "new" instance that was just created
    QVector<FilterParameter::Pointer> options = getFilterParameters(); // Get the current set of filter parameters
    for (QVector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
    {
      FilterParameter* parameter = (*iter).get();
      if (parameter->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0 )
      {
        continue; // Skip this type of filter parameter as it has nothing to do with anything in the filter.
      }
      // Get the property from the current instance of the filter
      QVariant var = property(parameter->getPropertyName().toLatin1().constData());
      bool ok = filter->setProperty(parameter->getPropertyName().toLatin1().constData(), var);
      if(false == ok)
      {
        QString ss = QString("Error occurred transferring the Filter Parameter '%1' in Filter '%2' to the filter instance. The pipeline may run but the underlying filter will NOT be using the values from the GUI."
                             " Please report this issue to the developers of this filter.").arg(parameter->getPropertyName()).arg(filter->getHumanLabel());
        Q_ASSERT_X(ok, __FILE__, ss.toLatin1().constData());
      }

      if(parameter->isConditional() == true)
      {
        QVariant cond = property(parameter->getConditionalProperty().toLatin1().constData() );
        ok = filter->setProperty(parameter->getConditionalProperty().toLatin1().constData(), cond);
        if(false == ok)
        {
          QString ss = QString("%1::newFilterInstance()\nError occurred transferring the Filter Parameter '%2' in Filter '%3' to the filter instance. "
                              " The filter parameter has a conditional property '%4'. The transfer of this property from the old filter to the new filter failed."
                              " Please report this issue to the developers of this filter.").arg(filter->getNameOfClass())
                              .arg(parameter->getPropertyName())
                              .arg(filter->getHumanLabel())
                              .arg(parameter->getConditionalProperty());
          Q_ASSERT_X(ok, __FILE__, ss.toLatin1().constData());
        }
      }
    }
  }
  return filter;
}
