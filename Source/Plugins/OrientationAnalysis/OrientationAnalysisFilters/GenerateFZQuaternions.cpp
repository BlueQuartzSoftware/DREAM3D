/*
 * Your License or Copyright can go here
 */

#include "GenerateFZQuaternions.h"

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"

#include "OrientationLib/LaueOps/LaueOps.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

#include "EbsdLib/EbsdConstants.h"

/* Create Enumerations to allow the created Attribute Arrays to take part in renaming */
enum createdPathID : RenameDataPath::DataID_t
{
  DataArrayID30 = 30,
  DataArrayID31 = 31,
};

/**
 * @brief The GenerateFZQuatsImpl class implements a threaded algorithm that computes the IPF
 * colors for each element in a geometry
 */
class GenerateFZQuatsImpl
{
public:
  GenerateFZQuatsImpl(GenerateFZQuaternions* filter, float* quats, int32_t* phases, uint32_t* crystalStructures, int32_t numPhases, bool* goodVoxels, float* fzQuats)
  : m_Filter(filter)
  , m_Quats(quats)
  , m_CellPhases(phases)
  , m_CrystalStructures(crystalStructures)
  , m_NumPhases(numPhases)
  , m_GoodVoxels(goodVoxels)
  , m_FZQuats(fzQuats)
  {
  }

  virtual ~GenerateFZQuatsImpl() = default;

  /**
   * @brief convert
   * @param start
   * @param end
   */
  void convert(size_t start, size_t end) const
  {
    QVector<LaueOps::Pointer> ops = LaueOps::getOrientationOpsQVector();
    int32_t phase = 0;
    bool calcIPF = false;
    size_t index = 0;
    for(size_t i = start; i < end; i++)
    {
      phase = m_CellPhases[i];

      calcIPF = true;
      if(nullptr != m_GoodVoxels)
      {
        calcIPF = m_GoodVoxels[i];
      }

      // Sanity check the phase data to make sure we do not walk off the end of the array
      if(phase >= m_NumPhases)
      {
        m_Filter->incrementPhaseWarningCount();
      }

      // Initialize the output to zero. There really isn't a good value to use.
      index = i * 4;
      m_FZQuats[index] = 0.0f;
      m_FZQuats[index + 1] = 0.0f;
      m_FZQuats[index + 2] = 0.0f;
      m_FZQuats[index + 3] = 0.0f;

      if(phase < m_NumPhases && calcIPF && m_CrystalStructures[phase] < Ebsd::CrystalStructure::LaueGroupEnd)
      {
        QuatType q = QuatType(m_Quats[index], m_Quats[index + 1], m_Quats[index + 2], m_Quats[index + 3]);
        int32_t xtal = static_cast<int32_t>(m_CrystalStructures[phase]);
        ops[xtal]->getFZQuat(q);
        m_FZQuats[index] = q.x();
        m_FZQuats[index + 1] = q.y();
        m_FZQuats[index + 2] = q.z();
        m_FZQuats[index + 3] = q.w();
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
  GenerateFZQuaternions* m_Filter = nullptr;
  float* m_Quats = nullptr;
  int32_t* m_CellPhases = nullptr;
  unsigned int* m_CrystalStructures = nullptr;
  int32_t m_NumPhases = 0;
  bool* m_GoodVoxels = nullptr;
  float* m_FZQuats = nullptr;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateFZQuaternions::GenerateFZQuaternions()
: m_CellPhasesArrayPath("", "", "")
, m_QuatsArrayPath("", "", "")
, m_CrystalStructuresArrayPath("", "", "")
, m_UseGoodVoxels(false)
, m_GoodVoxelsArrayPath("", "", "")
{
  initialize();
  m_OrientationOps = LaueOps::getOrientationOpsQVector();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateFZQuaternions::~GenerateFZQuaternions() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFZQuaternions::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFZQuaternions::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  QVector<QString> choices = QVector<QString>::fromStdVector(LaueOps::GetLaueNames());
  choices.pop_back(); // Remove the last name because we don't need it.

  QStringList linkedProps("GoodVoxelsArrayPath");
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Apply to Good Elements Only (Bad Elements Will Be Black)", UseGoodVoxels, FilterParameter::Parameter, GenerateFZQuaternions, linkedProps));
  parameters.push_back(SeparatorFilterParameter::New("Element Data", FilterParameter::RequiredArray));

  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 4, AttributeMatrix::Type::Any, IGeometry::Type::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Quaternions", QuatsArrayPath, FilterParameter::RequiredArray, GenerateFZQuaternions, req));
  }
  DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Float, 3, AttributeMatrix::Category::Any);

  req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Category::Any);
  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", CellPhasesArrayPath, FilterParameter::RequiredArray, GenerateFZQuaternions, req));
  req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Bool, 1, AttributeMatrix::Category::Any);
  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Mask", GoodVoxelsArrayPath, FilterParameter::RequiredArray, GenerateFZQuaternions, req));

  parameters.push_back(SeparatorFilterParameter::New("Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::UInt32, 1, AttributeMatrix::Category::Ensemble);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Crystal Structures", CrystalStructuresArrayPath, FilterParameter::RequiredArray, GenerateFZQuaternions, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Element Data", FilterParameter::CreatedArray));

  DataArrayCreationFilterParameter::RequirementType dacReq;
  parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Created FZ Quaternions", FZQuatsArrayPath, FilterParameter::CreatedArray, GenerateFZQuaternions, dacReq));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFZQuaternions::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  initialize();

  QVector<DataArrayPath> dataArraypaths;

  std::vector<size_t> cDims(1, 1);
  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CellPhasesPtr.lock())                                                                         /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() >= 0)
  {
    dataArraypaths.push_back(getCellPhasesArrayPath());
  }

  cDims[0] = 4;
  m_QuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getQuatsArrayPath(), cDims);
  if(nullptr != m_QuatsPtr.lock())
  {
    m_Quats = m_QuatsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 1;
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(),
                                                                                                                    cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CrystalStructuresPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 4;
  m_FZQuatsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, getFZQuatsArrayPath(), 0, cDims, "", DataArrayID31);
  if(nullptr != m_FZQuatsPtr.lock())
  {
    m_FZQuats = m_FZQuatsPtr.lock()->getPointer(0);
  }

  if(getUseGoodVoxels())
  {
    // The good voxels array is optional, If it is available we are going to use it, otherwise we are going to create it
    cDims[0] = 1;
    m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getGoodVoxelsArrayPath(),
                                                                                                       cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_GoodVoxelsPtr.lock())                                                                      /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCode() >= 0)
    {
      dataArraypaths.push_back(getGoodVoxelsArrayPath());
    }
  }
  else
  {
    m_GoodVoxels = nullptr;
  }

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArraypaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFZQuaternions::preflight()
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
void GenerateFZQuaternions::execute()
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

  m_PhaseWarningCount = 0;
  size_t totalPoints = m_QuatsPtr.lock()->getNumberOfTuples();
  int32_t numPhases = static_cast<int32_t>(m_CrystalStructuresPtr.lock()->getNumberOfTuples());

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  if(doParallel)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints), GenerateFZQuatsImpl(this, m_Quats, m_CellPhases, m_CrystalStructures, numPhases, m_GoodVoxels, m_FZQuats), tbb::auto_partitioner());
  }
  else
#endif
  {
    GenerateFZQuatsImpl serial(this, m_Quats, m_CellPhases, m_CrystalStructures, numPhases, m_GoodVoxels, m_FZQuats);
    serial.convert(0, totalPoints);
  }

  if(m_PhaseWarningCount > 0)
  {
    QString ss = QObject::tr("The Ensemble Phase information only references %2 phase(s) but %1 cell(s) had a phase value greater than %2. \
This indicates a problem with the input cell phase data. DREAM.3D will give INCORRECT RESULTS.")
                     .arg(m_PhaseWarningCount)
                     .arg(numPhases - 1);
    setErrorCondition(-49000, ss);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFZQuaternions::incrementPhaseWarningCount()
{
  ++m_PhaseWarningCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GenerateFZQuaternions::newFilterInstance(bool copyFilterParameters) const
{
  GenerateFZQuaternions::Pointer filter = GenerateFZQuaternions::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateFZQuaternions::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateFZQuaternions::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateFZQuaternions::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateFZQuaternions::getGroupName() const
{
  return SIMPL::FilterGroups::ProcessingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid GenerateFZQuaternions::getUuid()
{
  return QUuid("{9a6677a6-b9e5-5fee-afa2-27e868cab8ca}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateFZQuaternions::getSubGroupName() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateFZQuaternions::getHumanLabel() const
{
  return "Reduce Orientations to Fundamental Zone";
}
