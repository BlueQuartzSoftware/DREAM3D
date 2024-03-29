/*
 * Your License or Copyright can go here
 */

#include "GenerateFZQuaternions.h"

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#endif

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"

#include "EbsdLib/LaueOps/LaueOps.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

#include "EbsdLib/Core/EbsdLibConstants.h"

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
    std::vector<LaueOps::Pointer> ops = LaueOps::GetAllOrientationOps();
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

      if(phase < m_NumPhases && calcIPF && m_CrystalStructures[phase] < EbsdLib::CrystalStructure::LaueGroupEnd)
      {
        QuatD q = QuatD(m_Quats[index], m_Quats[index + 1], m_Quats[index + 2], m_Quats[index + 3]); // Makes a copy into q
        int32_t xtal = static_cast<int32_t>(m_CrystalStructures[phase]);                             // get the Laue Group
        q = ops[xtal]->getFZQuat(q);
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
{
  initialize();
  m_OrientationOps = LaueOps::GetAllOrientationOps();
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
  std::vector<std::string> names = LaueOps::GetLaueNames();
  std::vector<QString> choices;
  for(const auto& name : names)
  {
    choices.push_back(QString::fromStdString(name));
  }

  choices.pop_back(); // Remove the last name because we don't need it.

  std::vector<QString> linkedProps = {"GoodVoxelsArrayPath"};
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Apply to Good Elements Only (Bad Elements Will Be Black)", UseGoodVoxels, FilterParameter::Category::Parameter, GenerateFZQuaternions, linkedProps));
  parameters.push_back(SeparatorFilterParameter::Create("Element Data", FilterParameter::Category::RequiredArray));

  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 4, AttributeMatrix::Type::Any, IGeometry::Type::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Quaternions", QuatsArrayPath, FilterParameter::Category::RequiredArray, GenerateFZQuaternions, req));
  }
  DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Float, 3, AttributeMatrix::Category::Any);

  req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Category::Any);
  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", CellPhasesArrayPath, FilterParameter::Category::RequiredArray, GenerateFZQuaternions, req));
  req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Bool, 1, AttributeMatrix::Category::Any);
  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Mask", GoodVoxelsArrayPath, FilterParameter::Category::RequiredArray, GenerateFZQuaternions, req));

  parameters.push_back(SeparatorFilterParameter::Create("Ensemble Data", FilterParameter::Category::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::UInt32, 1, AttributeMatrix::Category::Ensemble);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Crystal Structures", CrystalStructuresArrayPath, FilterParameter::Category::RequiredArray, GenerateFZQuaternions, req));
  }
  parameters.push_back(SeparatorFilterParameter::Create("Element Data", FilterParameter::Category::CreatedArray));

  DataArrayCreationFilterParameter::RequirementType dacReq;
  parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Created FZ Quaternions", FZQuatsArrayPath, FilterParameter::Category::CreatedArray, GenerateFZQuaternions, dacReq));
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
  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>>(this, getCellPhasesArrayPath(), cDims);
  if(nullptr != m_CellPhasesPtr.lock())
  {
    m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
  }
  if(getErrorCode() >= 0)
  {
    dataArraypaths.push_back(getCellPhasesArrayPath());
  }

  cDims[0] = 4;
  m_QuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>>(this, getQuatsArrayPath(), cDims);
  if(nullptr != m_QuatsPtr.lock())
  {
    m_Quats = m_QuatsPtr.lock()->getPointer(0);
  }
  if(getErrorCode() >= 0)
  {
      dataArraypaths.push_back(getQuatsArrayPath());
  }

  cDims[0] = 1;
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>>(this, getCrystalStructuresArrayPath(), cDims);
  if(nullptr != m_CrystalStructuresPtr.lock())
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  }

  cDims[0] = 4;
  m_FZQuatsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>>(this, getFZQuatsArrayPath(), 0, cDims, "", DataArrayID31);
  if(nullptr != m_FZQuatsPtr.lock())
  {
    m_FZQuats = m_FZQuatsPtr.lock()->getPointer(0);
  }
  if(getErrorCode() >= 0)
  {
      dataArraypaths.push_back(getFZQuatsArrayPath());
  }


  if(getUseGoodVoxels())
  {
    // The good voxels array is optional, If it is available we are going to use it, otherwise we are going to create it
    cDims[0] = 1;
    m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>>(this, getGoodVoxelsArrayPath(), cDims);
    if(nullptr != m_GoodVoxelsPtr.lock())
    {
      m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0);
    }
    if(getErrorCode() >= 0)
    {
      dataArraypaths.push_back(getGoodVoxelsArrayPath());
    }
  }
  else
  {
    m_GoodVoxels = nullptr;
  }

  getDataContainerArray()->validateNumberOfTuples(this, dataArraypaths);
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
  if(true)
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
QString GenerateFZQuaternions::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateFZQuaternions::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateFZQuaternions::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateFZQuaternions::getGroupName() const
{
  return SIMPL::FilterGroups::ProcessingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid GenerateFZQuaternions::getUuid() const
{
  return QUuid("{9a6677a6-b9e5-5fee-afa2-27e868cab8ca}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateFZQuaternions::getSubGroupName() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateFZQuaternions::getHumanLabel() const
{
  return "Reduce Orientations to Fundamental Zone";
}

// -----------------------------------------------------------------------------
GenerateFZQuaternions::Pointer GenerateFZQuaternions::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<GenerateFZQuaternions> GenerateFZQuaternions::New()
{
  struct make_shared_enabler : public GenerateFZQuaternions
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString GenerateFZQuaternions::getNameOfClass() const
{
  return QString("GenerateFZQuaternions");
}

// -----------------------------------------------------------------------------
QString GenerateFZQuaternions::ClassName()
{
  return QString("GenerateFZQuaternions");
}

// -----------------------------------------------------------------------------
void GenerateFZQuaternions::setCellPhasesArrayPath(const DataArrayPath& value)
{
  m_CellPhasesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenerateFZQuaternions::getCellPhasesArrayPath() const
{
  return m_CellPhasesArrayPath;
}

// -----------------------------------------------------------------------------
void GenerateFZQuaternions::setQuatsArrayPath(const DataArrayPath& value)
{
  m_QuatsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenerateFZQuaternions::getQuatsArrayPath() const
{
  return m_QuatsArrayPath;
}

// -----------------------------------------------------------------------------
void GenerateFZQuaternions::setCrystalStructuresArrayPath(const DataArrayPath& value)
{
  m_CrystalStructuresArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenerateFZQuaternions::getCrystalStructuresArrayPath() const
{
  return m_CrystalStructuresArrayPath;
}

// -----------------------------------------------------------------------------
void GenerateFZQuaternions::setUseGoodVoxels(bool value)
{
  m_UseGoodVoxels = value;
}

// -----------------------------------------------------------------------------
bool GenerateFZQuaternions::getUseGoodVoxels() const
{
  return m_UseGoodVoxels;
}

// -----------------------------------------------------------------------------
void GenerateFZQuaternions::setGoodVoxelsArrayPath(const DataArrayPath& value)
{
  m_GoodVoxelsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenerateFZQuaternions::getGoodVoxelsArrayPath() const
{
  return m_GoodVoxelsArrayPath;
}

// -----------------------------------------------------------------------------
void GenerateFZQuaternions::setFZQuatsArrayPath(const DataArrayPath& value)
{
  m_FZQuatsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenerateFZQuaternions::getFZQuatsArrayPath() const
{
  return m_FZQuatsArrayPath;
}
