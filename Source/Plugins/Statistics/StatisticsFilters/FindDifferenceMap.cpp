/*
 * Your License or Copyright can go here
 */

#include "FindDifferenceMap.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/TemplateHelpers.hpp"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "Statistics/StatisticsConstants.h"

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

/**
 * @brief The FindDifferenceMapImpl class implements a threaded algorithm that computes the difference map
 * between two arrays
 */
template<typename DataType>
class FindDifferenceMapImpl
{
  public:
    FindDifferenceMapImpl(IDataArray::Pointer firstArray, IDataArray::Pointer secondArray, IDataArray::Pointer differenceMap) :
      m_FirstArray(firstArray),
      m_SecondArray(secondArray),
      m_DifferenceMap(differenceMap)
    {}
    virtual ~FindDifferenceMapImpl() {}

    void generate(size_t start, size_t end) const
    {
      typename DataArray<DataType>::Pointer firstArrayPtr = boost::dynamic_pointer_cast<DataArray<DataType> >(m_FirstArray);
      typename DataArray<DataType>::Pointer secondArrayPtr = boost::dynamic_pointer_cast<DataArray<DataType> >(m_SecondArray);
      typename DataArray<DataType>::Pointer differenceMapPtr = boost::dynamic_pointer_cast<DataArray<DataType> >(m_DifferenceMap);

      DataType* firstArray = firstArrayPtr->getPointer(0);
      DataType* secondArray = secondArrayPtr->getPointer(0);
      DataType* differenceMap = differenceMapPtr->getPointer(0);

      int32_t numComps = firstArrayPtr->getNumberOfComponents();

      for (size_t i = start; i < end; i++)
      {
        for (int32_t j = 0; j < numComps; j++)
        {
          differenceMap[numComps * i + j] = firstArray[numComps * i + j] - secondArray[numComps * i + j];
        }
      }
    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      generate(r.begin(), r.end());
    }
#endif

  private:
    IDataArray::Pointer m_FirstArray;
    IDataArray::Pointer m_SecondArray;
    IDataArray::Pointer m_DifferenceMap;
};

/**
 * @brief The ExecuteFindDifferenceMap class wraps the FindDifferenceMapImpl class so that the
 * EXECUTE_TEMPLATE macro can be used in the main execute() of the Filter
 */
template<typename DataType>
class ExecuteFindDifferenceMap
{
  public:
    ExecuteFindDifferenceMap() {}
    virtual ~ExecuteFindDifferenceMap() {}

    bool operator()(IDataArray::Pointer p)
    {
      return (TemplateHelpers::CanDynamicCast<DataArray<DataType> >()(p));
    }

    void Execute(IDataArray::Pointer firstArrayPtr, IDataArray::Pointer secondArrayPtr, IDataArray::Pointer differenceMapPtr)
    {
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
      tbb::task_scheduler_init init;
      bool doParallel = true;
#endif

      size_t numTuples = firstArrayPtr->getNumberOfTuples();

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS

      if (doParallel == true)
      {
        tbb::parallel_for(tbb::blocked_range<size_t>(0, numTuples), FindDifferenceMapImpl<DataType>(firstArrayPtr, secondArrayPtr, differenceMapPtr), tbb::auto_partitioner());
      }
      else
#endif
      {
        FindDifferenceMapImpl<DataType> serial(firstArrayPtr, secondArrayPtr, differenceMapPtr);
        serial.generate(0, numTuples);
      }
    }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindDifferenceMap::FindDifferenceMap() :
  AbstractFilter(),
  m_FirstInputArrayPath("", "", ""),
  m_SecondInputArrayPath("", "", ""),
  m_DifferenceMapArrayPath("", "", "DifferenceMap"),
  m_FirstInputArray(NULL),
  m_SecondInputArray(NULL),
  m_DifferenceMap(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindDifferenceMap::~FindDifferenceMap()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindDifferenceMap::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    DataArraySelectionFilterParameter::DataStructureRequirements req;
    parameters.push_back(DataArraySelectionFilterParameter::New("First Attribute Array", "FirstInputArrayPath", getFirstInputArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::DataStructureRequirements req;
    parameters.push_back(DataArraySelectionFilterParameter::New("Second Attribute Array", "SecondInputArrayPath", getSecondInputArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArrayCreationFilterParameter::DataStructureRequirements req;
    parameters.push_back(DataArrayCreationFilterParameter::New("Difference Map", "DifferenceMapArrayPath", getDifferenceMapArrayPath(), FilterParameter::CreatedArray, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindDifferenceMap::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFirstInputArrayPath(reader->readDataArrayPath("FirstInputArrayPath", getFirstInputArrayPath()));
  setSecondInputArrayPath(reader->readDataArrayPath("SecondInputArrayPath", getSecondInputArrayPath()));
  setDifferenceMapArrayPath(reader->readDataArrayPath("DifferenceMapArrayPath", getDifferenceMapArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindDifferenceMap::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FirstInputArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(SecondInputArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(DifferenceMapArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename DataType>
void validateArrayTypes(AbstractFilter* filter, QVector<IDataArray::Pointer> ptrArray)
{
  for (QVector<IDataArray::Pointer>::iterator it = ptrArray.begin(); it != ptrArray.end(); ++it)
  {
    if (TemplateHelpers::CanDynamicCast<DataArray<bool> >()(*it))
    {
      filter->setErrorCondition(-90000);
      QString ss = QObject::tr("Selected Attribute Arrays cannot be of type bool");
      filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
      return;
    }
    if (!TemplateHelpers::CanDynamicCast<DataArray<DataType> >()(*it))
    {
      filter->setErrorCondition(-90001);
      QString ss = QObject::tr("Selected Attribute Arrays must all be of the same type");
      filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename DataType>
void warnOnUnsignedTypes(AbstractFilter* filter, IDataArray::Pointer ptr)
{
  if (TemplateHelpers::CanDynamicCast<DataArray<uint8_t> >()(ptr))
  {
    QString ss = QObject::tr("Selected Attribute Arrays are of type uint8_t. Using unsigned integer types may result in underflow leading to extremely large values!");
    filter->notifyWarningMessage(filter->getHumanLabel(), ss, -90004);
  }
  if (TemplateHelpers::CanDynamicCast<DataArray<uint16_t> >()(ptr))
  {
    QString ss = QObject::tr("Selected Attribute Arrays are of type uint16_t. Using unsigned integer types may result in underflow leading to extremely large values!");
    filter->notifyWarningMessage(filter->getHumanLabel(), ss, -90004);
  }
  if (TemplateHelpers::CanDynamicCast<DataArray<uint32_t> >()(ptr))
  {
    QString ss = QObject::tr("Selected Attribute Arrays are of type uint32_t. Using unsigned integer types may result in underflow leading to extremely large values!");
    filter->notifyWarningMessage(filter->getHumanLabel(), ss, -90004);
  }
  if (TemplateHelpers::CanDynamicCast<DataArray<uint64_t> >()(ptr))
  {
    QString ss = QObject::tr("Selected Attribute Arrays are of type uint64_t. Using unsigned integer types may result in underflow leading to extremely large values!");
    filter->notifyWarningMessage(filter->getHumanLabel(), ss, -90004);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindDifferenceMap::dataCheck()
{
  setErrorCondition(0);

  QVector<IDataArray::Pointer> dataArrays;

  m_FirstInputArrayPtr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getFirstInputArrayPath());
  if (getErrorCondition() >= 0) { dataArrays.push_back(m_FirstInputArrayPtr.lock()); }

  m_SecondInputArrayPtr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getSecondInputArrayPath());
  if (getErrorCondition() >= 0) { dataArrays.push_back(m_SecondInputArrayPtr.lock()); }

  if (dataArrays.size() > 0) { EXECUTE_FUNCTION_TEMPLATE(this, validateArrayTypes, dataArrays[0], this, dataArrays) }

  if (getErrorCondition() < 0) { return; }

  if (dataArrays.size() > 0) { EXECUTE_FUNCTION_TEMPLATE(this, warnOnUnsignedTypes, dataArrays[0], this, dataArrays[0]) }

  // Safe to check array component dimensions since we won't get here if the pointers are null
  if (m_FirstInputArrayPtr.lock()->getComponentDimensions() != m_SecondInputArrayPtr.lock()->getComponentDimensions())
  {
    setErrorCondition(-90003);
    QString ss = QObject::tr("Selected Attribute Arrays must have the same component dimensions");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if (getErrorCondition() < 0) { return; }

  // At this point we have two valid arrays of the same type and component dimensions, so we
  // are safe to make the output array with the correct type and component dimensions
  QVector<size_t> cDims = m_FirstInputArrayPtr.lock()->getComponentDimensions();
  m_DifferenceMapPtr = TemplateHelpers::CreateNonPrereqArrayFromArrayType()(this, getDifferenceMapArrayPath(), cDims, m_FirstInputArrayPtr.lock());
  if (getErrorCondition() >= 0) { dataArrays.push_back(m_DifferenceMapPtr.lock()); }

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrays);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindDifferenceMap::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindDifferenceMap::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  EXECUTE_TEMPLATE(this, ExecuteFindDifferenceMap, m_FirstInputArrayPtr.lock(), m_FirstInputArrayPtr.lock(), m_SecondInputArrayPtr.lock(), m_DifferenceMapPtr.lock())

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindDifferenceMap::newFilterInstance(bool copyFilterParameters)
{
  FindDifferenceMap::Pointer filter = FindDifferenceMap::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindDifferenceMap::getCompiledLibraryName()
{ return StatisticsConstants::StatisticsBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindDifferenceMap::getGroupName()
{ return DREAM3D::FilterGroups::StatisticsFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindDifferenceMap::getHumanLabel()
{ return "Find Difference Map"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindDifferenceMap::getSubGroupName()
{ return DREAM3D::FilterSubGroups::MiscFilters; }

