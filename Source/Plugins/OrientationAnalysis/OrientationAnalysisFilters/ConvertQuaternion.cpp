/*
 * Your License or Copyright can go here
 */

#include "ConvertQuaternion.h"

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#endif

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

/* Create Enumerations to allow the created Attribute Arrays to take part in renaming */
enum createdPathID : RenameDataPath::DataID_t
{
  DataArrayID30 = 30,
  DataArrayID31 = 31,
};

class ConvertQuaternionImpl
{
public:
  ConvertQuaternionImpl(ConvertQuaternion* filter, float* inputQuat, float* outputQuat)
  : m_Filter(filter)
  , m_Input(inputQuat)
  , m_Output(outputQuat)
  {
  }
  ConvertQuaternionImpl(const ConvertQuaternionImpl&) = default;           // Copy Constructor
  ConvertQuaternionImpl(ConvertQuaternionImpl&&) = delete;                 // Move Constructor Not Implemented
  ConvertQuaternionImpl& operator=(const ConvertQuaternionImpl&) = delete; // Copy Assignment Not Implemented
  ConvertQuaternionImpl& operator=(ConvertQuaternionImpl&&) = delete;      // Move Assignment Not Implemented

  virtual ~ConvertQuaternionImpl() = default;

  void convert(size_t start, size_t end) const
  {
    // Let's assume k_ToScalarVector which means the incoming quaternions are Vector-Scalar
    // <x,y,z> w  ---> w <x,y,z>
    std::array<size_t, 4> mapping = {{1, 2, 3, 0}};

    if(m_Filter->getConversionType() == ConvertQuaternion::k_ToVectorScalar)
    {
      // w <x,y,z>  ---> <x,y,z> w
      mapping = {{3, 0, 1, 2}};
    }

    std::array<float, 4> temp = {0.0f, 0.0f, 0.0f, 0.0f};
    for(size_t i = start; i < end; i++)
    {
      if(m_Filter->getCancel())
      {
        return;
      }

      temp[mapping[0]] = m_Input[i * 4];
      temp[mapping[1]] = m_Input[i * 4 + 1];
      temp[mapping[2]] = m_Input[i * 4 + 2];
      temp[mapping[3]] = m_Input[i * 4 + 3];

      m_Output[i * 4] = temp[0];
      m_Output[i * 4 + 1] = temp[1];
      m_Output[i * 4 + 2] = temp[2];
      m_Output[i * 4 + 3] = temp[3];
    }
  }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  void operator()(const tbb::blocked_range<size_t>& r) const
  {
    convert(r.begin(), r.end());
  }
#endif
private:
  ConvertQuaternion* m_Filter = nullptr;
  float* m_Input;
  float* m_Output;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertQuaternion::ConvertQuaternion()
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertQuaternion::~ConvertQuaternion() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertQuaternion::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertQuaternion::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  DataArraySelectionFilterParameter::RequirementType dasReq;
  std::vector<std::vector<size_t>> comp;
  comp.push_back(std::vector<size_t>(1, 4));
  dasReq.componentDimensions = comp;
  dasReq.daTypes = {SIMPL::TypeNames::Float};
  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Quaternion Array", QuaternionDataArrayPath, FilterParameter::Category::Parameter, ConvertQuaternion, dasReq, {DataArrayID30}));
  DataArrayCreationFilterParameter::RequirementType dacReq;
  parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Output Data Array Path", OutputDataArrayPath, FilterParameter::Category::CreatedArray, ConvertQuaternion, dacReq, {DataArrayID31}));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Delete Original Data", DeleteOriginalData, FilterParameter::Category::Parameter, ConvertQuaternion));
  std::vector<QString> choices = {"To Scalar Vector ( w, [x, y, z] )", "To Vector Scalar ( [x, y, z], w )"};
  parameters.push_back(SIMPL_NEW_CHOICE_FP("Conversion Type", ConversionType, FilterParameter::Category::Parameter, ConvertQuaternion, choices, false));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertQuaternion::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  std::vector<size_t> cDims(1, 1);
  cDims[0] = 4;
  m_QuaternionsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>>(this, getQuaternionDataArrayPath(), cDims);
  if(nullptr != m_QuaternionsPtr.lock())
  {
    m_Quaternions = m_QuaternionsPtr.lock()->getPointer(0);
  }

  cDims[0] = 4;
  m_OutputQuaternionsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>>(this, getOutputDataArrayPath(), 0, cDims, "", DataArrayID31);
  if(nullptr != m_OutputQuaternionsPtr.lock())
  {
    m_OutputQuaternions = m_OutputQuaternionsPtr.lock()->getPointer(0);
  }

  if(getDeleteOriginalData() && getInPreflight())
  {
    AttributeMatrix::Pointer am = getDataContainerArray()->getAttributeMatrix(getQuaternionDataArrayPath());
    if(am)
    {
      am->removeAttributeArray(getQuaternionDataArrayPath().getDataArrayName());
    }
  }

  if(m_ConversionType < 0 || m_ConversionType > 1)
  {
    QString ss = QObject::tr("The conversion type must be either 0 (ToScalarVector) or 1 (ToVectorScalar). DREAM.3D expects Quaternions to be  in the Vector-Scalar form, i.e., <x,y,z> w");
    setErrorCondition(-52500, ss);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertQuaternion::execute()
{
  initialize();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  size_t totalPoints = m_QuaternionsPtr.lock()->getNumberOfTuples();

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  if(true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints), ConvertQuaternionImpl(this, m_Quaternions, m_OutputQuaternions), tbb::auto_partitioner());
  }
  else
#endif
  {
    ConvertQuaternionImpl serial(this, m_Quaternions, m_OutputQuaternions);
    serial.convert(0, totalPoints);
  }

  /* Do not forget to remove the original array if requested */
  if(getDeleteOriginalData())
  {
    getDataContainerArray()->getAttributeMatrix(getQuaternionDataArrayPath())->removeAttributeArray(getQuaternionDataArrayPath().getDataArrayName());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ConvertQuaternion::newFilterInstance(bool copyFilterParameters) const
{
  ConvertQuaternion::Pointer filter = ConvertQuaternion::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ConvertQuaternion::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ConvertQuaternion::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ConvertQuaternion::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ConvertQuaternion::getGroupName() const
{
  return SIMPL::FilterGroups::ProcessingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ConvertQuaternion::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::ConversionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ConvertQuaternion::getHumanLabel() const
{
  return "Convert Quaternion Order";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid ConvertQuaternion::getUuid() const
{
  return QUuid("{439e31b7-3198-5d0d-aef6-65a9e9c1a016}");
}

// -----------------------------------------------------------------------------
ConvertQuaternion::Pointer ConvertQuaternion::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ConvertQuaternion> ConvertQuaternion::New()
{
  struct make_shared_enabler : public ConvertQuaternion
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ConvertQuaternion::getNameOfClass() const
{
  return QString("ConvertQuaternion");
}

// -----------------------------------------------------------------------------
QString ConvertQuaternion::ClassName()
{
  return QString("ConvertQuaternion");
}

// -----------------------------------------------------------------------------
void ConvertQuaternion::setQuaternionDataArrayPath(const DataArrayPath& value)
{
  m_QuaternionDataArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ConvertQuaternion::getQuaternionDataArrayPath() const
{
  return m_QuaternionDataArrayPath;
}

// -----------------------------------------------------------------------------
void ConvertQuaternion::setOutputDataArrayPath(const DataArrayPath& value)
{
  m_OutputDataArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ConvertQuaternion::getOutputDataArrayPath() const
{
  return m_OutputDataArrayPath;
}

// -----------------------------------------------------------------------------
void ConvertQuaternion::setDeleteOriginalData(bool value)
{
  m_DeleteOriginalData = value;
}

// -----------------------------------------------------------------------------
bool ConvertQuaternion::getDeleteOriginalData() const
{
  return m_DeleteOriginalData;
}

// -----------------------------------------------------------------------------
void ConvertQuaternion::setConversionType(int32_t value)
{
  m_ConversionType = value;
}

// -----------------------------------------------------------------------------
int32_t ConvertQuaternion::getConversionType() const
{
  return m_ConversionType;
}
