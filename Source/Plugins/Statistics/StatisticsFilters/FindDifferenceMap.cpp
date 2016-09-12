/*
 * Your License or Copyright can go here
 */

#include "FindDifferenceMap.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/TemplateHelpers.hpp"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "Statistics/StatisticsConstants.h"
#include "Statistics/StatisticsVersion.h"

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
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
      typename DataArray<DataType>::Pointer firstArrayPtr = std::dynamic_pointer_cast<DataArray<DataType> >(m_FirstArray);
      typename DataArray<DataType>::Pointer secondArrayPtr = std::dynamic_pointer_cast<DataArray<DataType> >(m_SecondArray);
      typename DataArray<DataType>::Pointer differenceMapPtr = std::dynamic_pointer_cast<DataArray<DataType> >(m_DifferenceMap);

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

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
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
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
      tbb::task_scheduler_init init;
      bool doParallel = true;
#endif

      size_t numTuples = firstArrayPtr->getNumberOfTuples();

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS

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

// Include the MOC generated file for this class
#include "moc_FindDifferenceMap.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindDifferenceMap::FindDifferenceMap() :
  AbstractFilter(),
  m_FirstInputArrayPath("", "", ""),
  m_SecondInputArrayPath("", "", ""),
  m_DifferenceMapArrayPath("", "", "DifferenceMap"),
  m_FirstInputArray(nullptr),
  m_SecondInputArray(nullptr),
  m_DifferenceMap(nullptr)
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
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("First Attribute Array", FirstInputArrayPath, FilterParameter::RequiredArray, FindDifferenceMap, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Second Attribute Array", SecondInputArrayPath, FilterParameter::RequiredArray, FindDifferenceMap, req));
  }
  {
    DataArrayCreationFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Difference Map", DifferenceMapArrayPath, FilterParameter::CreatedArray, FindDifferenceMap, req));
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
    filter->notifyWarningMessage(filter->getHumanLabel(), ss, -90005);
  }
  if (TemplateHelpers::CanDynamicCast<DataArray<uint32_t> >()(ptr))
  {
    QString ss = QObject::tr("Selected Attribute Arrays are of type uint32_t. Using unsigned integer types may result in underflow leading to extremely large values!");
    filter->notifyWarningMessage(filter->getHumanLabel(), ss, -90006);
  }
  if (TemplateHelpers::CanDynamicCast<DataArray<uint64_t> >()(ptr))
  {
    QString ss = QObject::tr("Selected Attribute Arrays are of type uint64_t. Using unsigned integer types may result in underflow leading to extremely large values!");
    filter->notifyWarningMessage(filter->getHumanLabel(), ss, -90007);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindDifferenceMap::initialize()
{

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
{
  return StatisticsConstants::StatisticsBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindDifferenceMap::getBrandingString()
{
  return "Statistics";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindDifferenceMap::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  Statistics::Version::Major() << "." << Statistics::Version::Minor() << "." << Statistics::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindDifferenceMap::getGroupName()
{ return SIMPL::FilterGroups::StatisticsFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindDifferenceMap::getHumanLabel()
{ return "Find Difference Map"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindDifferenceMap::getSubGroupName()
{ return SIMPL::FilterSubGroups::MiscFilters; }

