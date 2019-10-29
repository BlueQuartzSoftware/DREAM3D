/*
 * Your License or Copyright can go here
 */

#include <memory>

#include "GenerateOrientationMatrixTranspose.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>

/* Create Enumerations to allow the created Attribute Arrays to take part in renaming */
enum createdPathID : RenameDataPath::DataID_t
{
  DataArrayID30 = 30,
  DataArrayID31 = 31,
};

#endif

class GenerateOrientationMatrixTransposeImpl
{
public:
  GenerateOrientationMatrixTransposeImpl(GenerateOrientationMatrixTranspose* filter, float* outputRod)
  : m_Filter(filter)
  , m_Output(outputRod)
  {
  }
  GenerateOrientationMatrixTransposeImpl(const GenerateOrientationMatrixTransposeImpl&) = default;           // Copy Constructor
  GenerateOrientationMatrixTransposeImpl(GenerateOrientationMatrixTransposeImpl&&) = delete;                 // Move Constructor Not Implemented
  GenerateOrientationMatrixTransposeImpl& operator=(const GenerateOrientationMatrixTransposeImpl&) = delete; // Copy Assignment Not Implemented
  GenerateOrientationMatrixTransposeImpl& operator=(GenerateOrientationMatrixTransposeImpl&&) = delete;      // Move Assignment Not Implemented

  virtual ~GenerateOrientationMatrixTransposeImpl() = default;

  void convert(size_t start, size_t end) const
  {
    for(size_t i = start; i < end; i++)
    {
      if(m_Filter->getCancel())
      {
        return;
      }

      m_Output[i * 4] = m_Output[i * 4];
      m_Output[i * 4 + 1] = m_Output[i * 4 + 3];
      m_Output[i * 4 + 2] = m_Output[i * 4 + 6];
      m_Output[i * 4 + 3] = m_Output[i * 4 + 1];
      m_Output[i * 4 + 4] = m_Output[i * 4 + 4];
      m_Output[i * 4 + 5] = m_Output[i * 4 + 7];
      m_Output[i * 4 + 6] = m_Output[i * 4 + 2];
      m_Output[i * 4 + 7] = m_Output[i * 4 + 5];
      m_Output[i * 4 + 8] = m_Output[i * 4 + 8];
    }
  }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  void operator()(const tbb::blocked_range<size_t>& r) const
  {
    convert(r.begin(), r.end());
  }
#endif
private:
  GenerateOrientationMatrixTranspose* m_Filter = nullptr;
  float* m_Output;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateOrientationMatrixTranspose::GenerateOrientationMatrixTranspose()
: m_DeleteOriginalData(true)
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateOrientationMatrixTranspose::~GenerateOrientationMatrixTranspose() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateOrientationMatrixTranspose::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateOrientationMatrixTranspose::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  DataArraySelectionFilterParameter::RequirementType dasReq;
  std::vector<std::vector<size_t>> comp;
  comp.push_back(std::vector<size_t>(1, 9));
  dasReq.componentDimensions = comp;
  dasReq.daTypes = { SIMPL::TypeNames::Float };
  
  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Quaternion Array", OrientationMatrixDataArrayPath, FilterParameter::Parameter, GenerateOrientationMatrixTranspose, dasReq));
  DataArrayCreationFilterParameter::RequirementType dacReq;
  parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Output Data Array Path", OutputDataArrayPath, FilterParameter::CreatedArray, GenerateOrientationMatrixTranspose, dacReq));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Delete Original Data", DeleteOriginalData, FilterParameter::Parameter, GenerateOrientationMatrixTranspose));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateOrientationMatrixTranspose::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  std::vector<size_t> cDims(1, 1);
  cDims[0] = 9;
  m_OrientationMatrixPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getOrientationMatrixDataArrayPath(), cDims);
  if(nullptr != m_OrientationMatrixPtr.lock())
  {
    m_OrientationMatrix = m_OrientationMatrixPtr.lock()->getPointer(0);
  }

  cDims[0] = 9;
  m_OutputOrientationMatrixPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, getOutputDataArrayPath(), 0, cDims, "", DataArrayID31);
  if(nullptr != m_OutputOrientationMatrixPtr.lock())
  {
    m_OutputOrientationMatrix = m_OutputOrientationMatrixPtr.lock()->getPointer(0);
  }

  if(getDeleteOriginalData() && getInPreflight())
  {
    AttributeMatrix::Pointer am = getDataContainerArray()->getAttributeMatrix(getOrientationMatrixDataArrayPath());
    if(am)
    {
      am->removeAttributeArray(getOrientationMatrixDataArrayPath().getDataArrayName());
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateOrientationMatrixTranspose::preflight()
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
void GenerateOrientationMatrixTranspose::execute()
{
  initialize();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  if(getCancel())
  {
    return;
  }

  size_t totalPoints = m_OrientationMatrixPtr.lock()->getNumberOfTuples();

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  if(doParallel)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints), GenerateOrientationMatrixTransposeImpl(this, m_OutputOrientationMatrix), tbb::auto_partitioner());
  }
  else
#endif
  {
    GenerateOrientationMatrixTransposeImpl serial(this, m_OutputOrientationMatrix);
    serial.convert(0, totalPoints);
  }

  /* Do not forget to remove the original array if requested */
  if(getDeleteOriginalData())
  {
    getDataContainerArray()->getAttributeMatrix(getOrientationMatrixDataArrayPath())->removeAttributeArray(getOrientationMatrixDataArrayPath().getDataArrayName());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GenerateOrientationMatrixTranspose::newFilterInstance(bool copyFilterParameters) const
{
  GenerateOrientationMatrixTranspose::Pointer filter = GenerateOrientationMatrixTranspose::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateOrientationMatrixTranspose::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateOrientationMatrixTranspose::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateOrientationMatrixTranspose::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateOrientationMatrixTranspose::getGroupName() const
{
  return SIMPL::FilterGroups::ProcessingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateOrientationMatrixTranspose::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::CrystallographyFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateOrientationMatrixTranspose::getHumanLabel() const
{
  return "Generate Orientation Matrix Transpose";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid GenerateOrientationMatrixTranspose::getUuid() const
{
  return QUuid("{ec58f4fe-8e51-527e-9536-8b6f185684be}");
}

// -----------------------------------------------------------------------------
GenerateOrientationMatrixTranspose::Pointer GenerateOrientationMatrixTranspose::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<GenerateOrientationMatrixTranspose> GenerateOrientationMatrixTranspose::New()
{
  struct make_shared_enabler : public GenerateOrientationMatrixTranspose
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString GenerateOrientationMatrixTranspose::getNameOfClass() const
{
  return QString("_SUPERGenerateOrientationMatrixTranspose");
}

// -----------------------------------------------------------------------------
QString GenerateOrientationMatrixTranspose::ClassName()
{
  return QString("_SUPERGenerateOrientationMatrixTranspose");
}

// -----------------------------------------------------------------------------
void GenerateOrientationMatrixTranspose::setOrientationMatrixDataArrayPath(const DataArrayPath& value)
{
  m_OrientationMatrixDataArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenerateOrientationMatrixTranspose::getOrientationMatrixDataArrayPath() const
{
  return m_OrientationMatrixDataArrayPath;
}

// -----------------------------------------------------------------------------
void GenerateOrientationMatrixTranspose::setOutputDataArrayPath(const DataArrayPath& value)
{
  m_OutputDataArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenerateOrientationMatrixTranspose::getOutputDataArrayPath() const
{
  return m_OutputDataArrayPath;
}

// -----------------------------------------------------------------------------
void GenerateOrientationMatrixTranspose::setDeleteOriginalData(bool value)
{
  m_DeleteOriginalData = value;
}

// -----------------------------------------------------------------------------
bool GenerateOrientationMatrixTranspose::getDeleteOriginalData() const
{
  return m_DeleteOriginalData;
}
