/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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


#include "FindRelativeMotionBetweenSlices.h"

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/TemplateHelpers.hpp"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "DREAM3DLib/FilterParameters/IntFilterParameter.h"
#include "DREAM3DLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/StringFilterParameter.h"
#include "DREAM3DLib/FilterParameters/ChoiceFilterParameter.h"
#include "DREAM3DLib/FilterParameters/SeparatorFilterParameter.h"
#include "DREAM3DLib/Math/MatrixMath.h"

#include "Processing/ProcessingConstants.h"

/**
 * @brief The CalcRelativeMotion class implements a templated threaded algorithm for
 * determining the relative motion between a series of slices through a 3D volume.
 */
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
      int32_t patchPoint = 0, comparePoint = 0;
      float val = 0.0f, minVal = 0.0f;
      for (size_t i = start; i < end; i++)
      {
        if(m_ValidPoints[i] == true)
        {
          minVal = std::numeric_limits<float>::max();
          for (size_t j = 0; j < m_NumSearchPoints; j++)
          {
            val = 0;
            for (size_t k = 0; k < m_NumPatchPoints; k++)
            {
              patchPoint = i + m_PatchPoints[k];
              comparePoint = patchPoint + m_SearchPoints[4 * j];
              val += float((m_Data[patchPoint] - m_Data[comparePoint])) * float((m_Data[patchPoint] - m_Data[comparePoint]));
            }
            if (val < minVal)
            {
              minVal = val;
              m_MotionDirection[3 * i + 0] = m_SearchPoints[4 * j + 1];
              m_MotionDirection[3 * i + 1] = m_SearchPoints[4 * j + 2];
              m_MotionDirection[3 * i + 2] = m_SearchPoints[4 * j + 3];
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
  m_PSize1(0),
  m_PSize2(0),
  m_SSize1(0),
  m_SSize2(0),
  m_SliceStep(0),
  m_MotionDirectionArrayName(DREAM3D::CellData::MotionDirection),
  m_InData(NULL),
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
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Plane of Interest");
    parameter->setPropertyName("Plane");

    QVector<QString> choices;
    choices.push_back("XY");
    choices.push_back("XZ");
    choices.push_back("YZ");
    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  parameters.push_back(IntFilterParameter::New("Patch Size 1", "PSize1", getPSize1(), FilterParameter::Parameter));
  parameters.push_back(IntFilterParameter::New("Patch Size 2", "PSize2", getPSize2(), FilterParameter::Parameter));
  parameters.push_back(IntFilterParameter::New("Search Distance 1", "SSize1", getSSize1(), FilterParameter::Parameter));
  parameters.push_back(IntFilterParameter::New("Search Distance 2", "SSize2", getSSize2(), FilterParameter::Parameter));
  parameters.push_back(IntFilterParameter::New("Slice Step", "SliceStep", getSliceStep(), FilterParameter::Parameter));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::Defaults::AnyPrimitive, 1, DREAM3D::AttributeMatrixType::Cell, DREAM3D::GeometryType::ImageGeometry);
    QVector<QString> daTypes;
    daTypes.push_back(DREAM3D::TypeNames::Int8);
    daTypes.push_back(DREAM3D::TypeNames::Int16);
    daTypes.push_back(DREAM3D::TypeNames::Int32);
    daTypes.push_back(DREAM3D::TypeNames::Int64);
    daTypes.push_back(DREAM3D::TypeNames::UInt8);
    daTypes.push_back(DREAM3D::TypeNames::UInt16);
    daTypes.push_back(DREAM3D::TypeNames::UInt32);
    daTypes.push_back(DREAM3D::TypeNames::UInt64);
    daTypes.push_back(DREAM3D::TypeNames::Float);
    daTypes.push_back(DREAM3D::TypeNames::Double);
    req.daTypes = daTypes;
    parameters.push_back(DataArraySelectionFilterParameter::New("Attribute Array to Track Motion", "SelectedArrayPath", getSelectedArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Motion Direction", "MotionDirectionArrayName", getMotionDirectionArrayName(), FilterParameter::CreatedArray));
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
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(MotionDirectionArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(SelectedArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(Plane)
  DREAM3D_FILTER_WRITE_PARAMETER(PSize1)
  DREAM3D_FILTER_WRITE_PARAMETER(PSize2)
  DREAM3D_FILTER_WRITE_PARAMETER(SSize1)
  DREAM3D_FILTER_WRITE_PARAMETER(SSize2)
  DREAM3D_FILTER_WRITE_PARAMETER(SliceStep)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindRelativeMotionBetweenSlices::dataCheck()
{
  setErrorCondition(0);

  DataArrayPath tempPath;
  QString ss;

  m_InDataPtr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getSelectedArrayPath());
  if( NULL != m_InDataPtr.lock())
  {
    if (TemplateHelpers::CanDynamicCast<BoolArrayType>()(m_InDataPtr.lock()))
    {
      QString ss = QObject::tr("Selected array cannot be of type bool.  The path is %1").arg(getSelectedArrayPath().serialize());
      setErrorCondition(-11001);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
  }

  QVector<size_t> cDims(1, 3);
  tempPath.update(m_SelectedArrayPath.getDataContainerName(), m_SelectedArrayPath.getAttributeMatrixName(), getMotionDirectionArrayName() );
  m_MotionDirectionPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_MotionDirectionPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_MotionDirection = m_MotionDirectionPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  ImageGeom::Pointer image = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getSelectedArrayPath().getDataContainerName());
  if(getErrorCondition() < 0) { return; }

  if (image->getXPoints() <= 1 || image->getYPoints() <= 1 || image->getZPoints() <= 1)
  {
    ss = QObject::tr("The Image Geometry is not 3D and cannot be run through this filter. The dimensions are (%1,%2,%3)").arg(image->getXPoints()).arg(image->getYPoints()).arg(image->getZPoints());
    setErrorCondition(-999);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if (getPSize1() <= 0 || getPSize2() <= 0)
  {
    ss = QObject::tr("The patch dimensions (%1, %2) must both be positive numbers").arg(getPSize1()).arg(getPSize2());
    setErrorCondition(-5555);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if (getSSize1() <= 0 || getSSize2() <= 0)
  {
    ss = QObject::tr("The search dimensions (%1, %2) must both be positive numbers").arg(getSSize1()).arg(getSSize2());
    setErrorCondition(-5555);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if (getPlane() == 0)
  {
    if (getSliceStep() >= static_cast<int64_t>(image->getZPoints()))
    {
      ss = QObject::tr("The Image Geometry extent (%1) is smaller than the supplied slice step (%2)").arg(image->getZPoints()).arg(getSliceStep());
      setErrorCondition(-5556);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
  }

  if (getPlane() == 1)
  {
    if (getSliceStep() >= static_cast<int64_t>(image->getYPoints()))
    {
      ss = QObject::tr("The Image Geometry Y extent (%1) is smaller than the supplied slice step (%2)").arg(image->getYPoints()).arg(getSliceStep());
      setErrorCondition(-5556);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
  }

  if (getPlane() == 2)
  {
    if (getSliceStep() >= static_cast<int64_t>(image->getXPoints()))
    {
      ss = QObject::tr("The Image Geometry X extent (%1) is smaller than the supplied slice step (%2)").arg(image->getXPoints()).arg(getSliceStep());
      setErrorCondition(-5556);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindRelativeMotionBetweenSlices::preflight()
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
void FindRelativeMotionBetweenSlices::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_SelectedArrayPath.getDataContainerName());
  ImageGeom::Pointer image = m->getGeometryAs<ImageGeom>();

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  int64_t xP = static_cast<int64_t>(image->getXPoints());
  int64_t yP = static_cast<int64_t>(image->getYPoints());
  int64_t zP = static_cast<int64_t>(image->getZPoints());
  size_t totalPoints = xP * yP * zP;

  int32_t buffer1 = (m_PSize1 / 2) + (m_SSize1 / 2);
  int32_t buffer2 = (m_PSize2 / 2) + (m_SSize1 / 2);

  QVector<size_t> cDims(1, 4);
  Int32ArrayType::Pointer patchPointsPtr = Int32ArrayType::CreateArray((m_PSize1 * m_PSize2), "_INTERNAL_USE_ONLY_patchPoints");
  Int32ArrayType::Pointer searchPointsPtr = Int32ArrayType::CreateArray((m_SSize1 * m_SSize2), cDims, "_INTERNAL_USE_ONLY_searchPoints");
  BoolArrayType::Pointer validPointsPtr = BoolArrayType::CreateArray(totalPoints, "_INTERNAL_USE_ONLY_validPoints");
  validPointsPtr->initializeWithValue(false);
  int32_t* patchPoints = patchPointsPtr->getPointer(0);
  int32_t* searchPoints = searchPointsPtr->getPointer(0);
  bool* validPoints = validPointsPtr->getPointer(0);
  int64_t yStride = 0, zStride = 0;
  size_t count = 0;
  size_t numPatchPoints = 0, numSearchPoints = 0;

  if (m_Plane == 0)
  {
    for (int32_t j = -(m_PSize2 / 2); j < (m_PSize2 / 2); j++)
    {
      yStride = j * xP;
      for (int32_t i = -(m_PSize1 / 2); i < (m_PSize1 / 2); i++)
      {
        patchPoints[count] = yStride + i;
        count++;
      }
    }
    numPatchPoints = count;
    count = 0;
    for (int32_t j = -(m_SSize2 / 2); j <= (m_SSize2 / 2); j++)
    {
      yStride = j * xP;
      for (int32_t i = -(m_SSize1 / 2); i <= (m_SSize1 / 2); i++)
      {
        searchPoints[4 * count] = (m_SliceStep * xP * yP) + yStride + i;
        searchPoints[4 * count + 1] = i;
        searchPoints[4 * count + 2] = j;
        searchPoints[4 * count + 3] = m_SliceStep;
        count++;
      }
    }
    numSearchPoints = count;
    for (int64_t k = 0; k < zP - m_SliceStep; k++)
    {
      zStride = k * xP * yP;
      for (int64_t j = buffer2; j < (yP - buffer2); j++)
      {
        yStride = j * xP;
        for (int64_t i = buffer1; i < (xP - buffer1); i++)
        {
          validPoints[zStride + yStride + i] = true;
        }
      }
    }
  }

  if (m_Plane == 1)
  {
    for (int32_t j = -(m_PSize2 / 2); j < (m_PSize2 / 2); j++)
    {
      yStride = (j * xP * yP);
      for (int32_t i = -(m_PSize1 / 2); i < (m_PSize1 / 2); i++)
      {
        patchPoints[count] = yStride + i;
        count++;
      }
    }
    numPatchPoints = count;
    count = 0;
    for (int32_t j = -(m_SSize2 / 2); j <= (m_SSize2 / 2); j++)
    {
      yStride = (j * xP * yP);
      for (int32_t i = -(m_SSize1 / 2); i <= (m_SSize1 / 2); i++)
      {
        searchPoints[count] = (m_SliceStep * xP) + yStride + i;
        searchPoints[4 * count + 1] = i;
        searchPoints[4 * count + 2] = m_SliceStep;
        searchPoints[4 * count + 3] = j;
        count++;
      }
    }
    numSearchPoints = count;
    for (int64_t k = buffer2; k < (zP - buffer2); k++)
    {
      zStride = k * xP * yP;
      for (int64_t j = 0; j < yP - m_SliceStep; j++)
      {
        yStride = j * xP;
        for (int64_t i = buffer1; i < (xP - buffer1); i++)
        {
          validPoints[zStride + yStride + i] = true;
        }
      }
    }
  }

  if (m_Plane == 2)
  {
    for (int32_t j = -(m_PSize2 / 2); j < (m_PSize2 / 2); j++)
    {
      yStride = (j * xP * yP);
      for (int32_t i = -(m_PSize1 / 2); i < (m_PSize1 / 2); i++)
      {
        patchPoints[count] = yStride + (i * xP);
        count++;
      }
    }
    numPatchPoints = count;
    count = 0;
    for (int32_t j = -(m_SSize2 / 2); j <= (m_SSize2 / 2); j++)
    {
      yStride = (j * xP * yP);
      for (int32_t i = -(m_SSize1 / 2); i <= (m_SSize1 / 2); i++)
      {
        searchPoints[count] = (m_SliceStep) + yStride + (i * xP);
        searchPoints[4 * count + 1] = m_SliceStep;
        searchPoints[4 * count + 2] = i;
        searchPoints[4 * count + 3] = j;
        count++;
      }
    }
    numSearchPoints = count;
    for (int64_t k = buffer2; k < (zP - buffer2); k++)
    {
      zStride = k * xP * yP;
      for (int64_t j = buffer1; j < (yP - buffer1); j++)
      {
        yStride = j * xP;
        for (int64_t i = 0; i < xP - m_SliceStep; i++)
        {
          validPoints[zStride + yStride + i] = true;
        }
      }
    }
  }

  if (NULL == patchPoints || NULL == searchPoints || NULL == validPoints)
  {
    QString ss = QObject::tr("Unable to establish search space for supplied parameters");
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if (TemplateHelpers::CanDynamicCast<Int8ArrayType>()(m_InDataPtr.lock()))
  {
    Int8ArrayType::Pointer cellArray = boost::dynamic_pointer_cast<Int8ArrayType>(m_InDataPtr.lock());
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
  else if (TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(m_InDataPtr.lock()))
  {
    UInt8ArrayType::Pointer cellArray = boost::dynamic_pointer_cast<UInt8ArrayType>(m_InDataPtr.lock());
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
  else if (TemplateHelpers::CanDynamicCast<Int16ArrayType>()(m_InDataPtr.lock()))
  {
    Int16ArrayType::Pointer cellArray = boost::dynamic_pointer_cast<Int16ArrayType>(m_InDataPtr.lock());
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
  else if (TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(m_InDataPtr.lock()))
  {
    UInt16ArrayType::Pointer cellArray = boost::dynamic_pointer_cast<UInt16ArrayType>(m_InDataPtr.lock());
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
  else if (TemplateHelpers::CanDynamicCast<Int32ArrayType>()(m_InDataPtr.lock()))
  {
    Int32ArrayType::Pointer cellArray = boost::dynamic_pointer_cast<Int32ArrayType>(m_InDataPtr.lock());
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
  else if (TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(m_InDataPtr.lock()))
  {
    UInt32ArrayType::Pointer cellArray = boost::dynamic_pointer_cast<UInt32ArrayType>(m_InDataPtr.lock());
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
  else if (TemplateHelpers::CanDynamicCast<Int64ArrayType>()(m_InDataPtr.lock()))
  {
    Int64ArrayType::Pointer cellArray = boost::dynamic_pointer_cast<Int64ArrayType>(m_InDataPtr.lock());
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
  else if (TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(m_InDataPtr.lock()))
  {
    UInt64ArrayType::Pointer cellArray = boost::dynamic_pointer_cast<UInt64ArrayType>(m_InDataPtr.lock());
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
  else if (TemplateHelpers::CanDynamicCast<FloatArrayType>()(m_InDataPtr.lock()))
  {
    FloatArrayType::Pointer cellArray = boost::dynamic_pointer_cast<FloatArrayType>(m_InDataPtr.lock());
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
  else if (TemplateHelpers::CanDynamicCast<DoubleArrayType>()(m_InDataPtr.lock()))
  {
    DoubleArrayType::Pointer cellArray = boost::dynamic_pointer_cast<DoubleArrayType>(m_InDataPtr.lock());
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
  else
  {
    QString ss = QObject::tr("Selected array is of unsupported type. The type is %1").arg(m_InDataPtr.lock()->getTypeAsString());
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  float v[3];
  float xRes = m->getGeometryAs<ImageGeom>()->getXRes();
  float yRes = m->getGeometryAs<ImageGeom>()->getYRes();
  float zRes = m->getGeometryAs<ImageGeom>()->getZRes();
  for (size_t i = 0; i < totalPoints; i++)
  {
    v[0] = m_MotionDirection[3 * i + 0] * xRes;
    v[1] = m_MotionDirection[3 * i + 1] * yRes;
    v[2] = m_MotionDirection[3 * i + 2] * zRes;
    MatrixMath::Normalize3x1(v);
    m_MotionDirection[3 * i + 0] = v[0];
    m_MotionDirection[3 * i + 1] = v[1];
    m_MotionDirection[3 * i + 2] = v[2];
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindRelativeMotionBetweenSlices::newFilterInstance(bool copyFilterParameters)
{
  FindRelativeMotionBetweenSlices::Pointer filter = FindRelativeMotionBetweenSlices::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindRelativeMotionBetweenSlices::getCompiledLibraryName()
{ return ProcessingConstants::ProcessingBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindRelativeMotionBetweenSlices::getGroupName()
{ return DREAM3D::FilterGroups::ProcessingFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindRelativeMotionBetweenSlices::getSubGroupName()
{ return DREAM3D::FilterSubGroups::ImageFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindRelativeMotionBetweenSlices::getHumanLabel()
{ return "Find Relative Motion Between Slices"; }

