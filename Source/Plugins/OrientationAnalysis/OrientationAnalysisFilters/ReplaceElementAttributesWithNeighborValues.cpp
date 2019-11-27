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
#include <memory>

#include "ReplaceElementAttributesWithNeighborValues.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/Common/TemplateHelpers.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatFilterParameter.h"
#include "SIMPLib/FilterParameters/MultiDataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

namespace Detail
{
static const int LessThan = 0;
static const int GreaterThan = 1;

template <typename T> class LessThanComparison
{
public:
  using Self = LessThanComparison<T>;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer()
  {
    return Pointer(static_cast<Self*>(nullptr));
  }

  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  QString getNameOfClass() const
  {
    return QString("LessThanComparison<T>");
  }
  /**
   * @brief Returns the name of the class for AbstractMessage
   */
  static QString ClassName()
  {
    return QString("LessThanComparison<T>");
  }

  static Pointer New()
  {
    Pointer sharedPtr(new(Self));
    return sharedPtr;
  }
  virtual ~LessThanComparison() = default;

  virtual bool compare(T a, T b)
  {
    return a < b;
  }
  virtual bool compare1(T a, T b)
  {
    return a >= b;
  }
  virtual bool compare2(T a, T b)
  {
    return a > b;
  }

protected:
  LessThanComparison() = default;
};

template <typename T> class GreaterThanComparison : public LessThanComparison<T>
{
public:
  using Self = GreaterThanComparison<T>;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer()
  {
    return Pointer(static_cast<Self*>(nullptr));
  }

  static Pointer New()
  {
    Pointer sharedPtr(new(Self));
    return sharedPtr;
  }
  /**
   * @brief Returns the name of the class for GreaterThanComparison<T>
   */
  /**
   * @brief Returns the name of the class for GreaterThanComparison<T>
   */
  QString getNameOfClass() const
  {
    return QString("_SUPERGreaterThanComparison<T>");
  }

  /**
   * @brief Returns the name of the class for GreaterThanComparison<T>
   */
  QString ClassName()
  {
    return QString("_SUPERGreaterThanComparison<T>");
  }

  ~GreaterThanComparison() override = default;

  bool compare(T a, T b) override
  {
    return a > b;
  }
  bool compare1(T a, T b) override
  {
    return a <= b;
  }
  bool compare2(T a, T b) override
  {
    return a < b;
  }

protected:
  GreaterThanComparison() = default;
};

template <typename T> void ExecuteTemplate(ReplaceElementAttributesWithNeighborValues* filter, IDataArray::Pointer inArrayPtr)
{
  using DataArrayType = DataArray<T>;
  using DataArrayPointerType = typename DataArrayType::Pointer;

  DataArrayPath dataArrayPath = filter->getConfidenceIndexArrayPath();
  DataContainer::Pointer m = filter->getDataContainerArray()->getDataContainer(dataArrayPath.getDataContainerName());
  size_t totalPoints = inArrayPtr->getNumberOfTuples();

  SizeVec3Type udims = m->getGeometryAs<ImageGeom>()->getDimensions();
  int64_t dims[3] = {
      static_cast<int64_t>(udims[0]), static_cast<int64_t>(udims[1]), static_cast<int64_t>(udims[2]),
  };

  bool good = true;
  int64_t neighbor = 0;
  int64_t column = 0, row = 0, plane = 0;

  int64_t neighpoints[6] = {0, 0, 0, 0, 0, 0};
  neighpoints[0] = static_cast<int64_t>(-dims[0] * dims[1]);
  neighpoints[1] = static_cast<int64_t>(-dims[0]);
  neighpoints[2] = static_cast<int64_t>(-1);
  neighpoints[3] = static_cast<int64_t>(1);
  neighpoints[4] = static_cast<int64_t>(dims[0]);
  neighpoints[5] = static_cast<int64_t>(dims[0] * dims[1]);

  QVector<int64_t> bestNeighbor(totalPoints, -1);

  size_t count = 0;
  float best = 0.0f;
  bool keepGoing = true;

  typename Detail::LessThanComparison<T>::Pointer comp = Detail::LessThanComparison<T>::New();
  if(filter->getSelectedComparison() == Detail::GreaterThan)
  {
    comp = Detail::GreaterThanComparison<T>::New();
  }

  DataArrayPointerType inData = std::dynamic_pointer_cast<DataArrayType>(inArrayPtr);
  DataArrayType& data = *inData;

  float thresholdValue = filter->getMinConfidence();

  QString attrMatName = dataArrayPath.getAttributeMatrixName();
  QVector<DataArrayPath> ignoredPaths = filter->getIgnoredDataArrayPaths();
  QList<QString> voxelArrayNames = m->getAttributeMatrix(attrMatName)->getAttributeArrayNames();
  for(const auto& dataArrayPath : ignoredPaths)
  {
    voxelArrayNames.removeAll(dataArrayPath.getDataArrayName());
  }

  while(keepGoing)
  {
    keepGoing = false;
    count = 0;
    if(filter->getCancel())
    {
      break;
    }

    int64_t progIncrement = static_cast<int64_t>(totalPoints / 50);
    int64_t prog = 1;
    int64_t progressInt = 0;
    for(size_t i = 0; i < totalPoints; i++)
    {
      if(comp->compare(data[i], thresholdValue))
      {
        column = i % dims[0];
        row = (i / dims[0]) % dims[1];
        plane = i / (dims[0] * dims[1]);
        count++;
        best = data[i];
        for(int64_t j = 0; j < 6; j++)
        {
          good = true;
          neighbor = int64_t(i) + neighpoints[j];
          if(j == 0 && plane == 0)
          {
            good = false;
          }
          if(j == 5 && plane == (dims[2] - 1))
          {
            good = false;
          }
          if(j == 1 && row == 0)
          {
            good = false;
          }
          if(j == 4 && row == (dims[1] - 1))
          {
            good = false;
          }
          if(j == 2 && column == 0)
          {
            good = false;
          }
          if(j == 3 && column == (dims[0] - 1))
          {
            good = false;
          }
          if(good)
          {
            if(comp->compare1(data[neighbor], thresholdValue) && comp->compare2(data[neighbor], best))
            {
              best = data[neighbor];
              bestNeighbor[i] = neighbor;
            }
          }
        }
      }
      if(int64_t(i) > prog)
      {
        progressInt = static_cast<int64_t>(((float)i / totalPoints) * 100.0f);
        QString ss = QObject::tr("Processing Data Current Loop (%1) Progress: %2% Complete").arg(count).arg(progressInt);
        filter->notifyStatusMessage(ss);
        prog = prog + progIncrement;
      }
    }

    if(filter->getCancel())
    {
      break;
    }

    progIncrement = static_cast<int64_t>(totalPoints / 50);
    prog = 1;
    progressInt = 0;
    for(size_t i = 0; i < totalPoints; i++)
    {
      if(int64_t(i) > prog)
      {
        progressInt = static_cast<int64_t>(((float)i / totalPoints) * 100.0f);
        QString ss = QObject::tr("Processing Data Current Loop (%1) || Transferring Cell Data: %2% Complete").arg(count).arg(progressInt);
        filter->notifyStatusMessage(ss);
        prog = prog + progIncrement;
      }

      neighbor = bestNeighbor[i];
      if(neighbor != -1)
      {
        for(const auto& arrayName : voxelArrayNames)
        {
          IDataArray::Pointer p = m->getAttributeMatrix(attrMatName)->getAttributeArray(arrayName);
          p->copyTuple(neighbor, i);
        }
      }
    }
    if(filter->getLoop() && count > 0)
    {
      keepGoing = true;
    }
  }
}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReplaceElementAttributesWithNeighborValues::ReplaceElementAttributesWithNeighborValues()
: m_MinConfidence(0.1f)
, m_ConfidenceIndexArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::ConfidenceIndex)
, m_SelectedComparison(Detail::LessThan)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReplaceElementAttributesWithNeighborValues::~ReplaceElementAttributesWithNeighborValues() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReplaceElementAttributesWithNeighborValues::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SIMPL_NEW_FLOAT_FP("Threshold Value", MinConfidence, FilterParameter::Parameter, ReplaceElementAttributesWithNeighborValues));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Comparison Operator");
    parameter->setPropertyName("SelectedComparison");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(ReplaceElementAttributesWithNeighborValues, this, SelectedComparison));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(ReplaceElementAttributesWithNeighborValues, this, SelectedComparison));

    QVector<QString> choices;
    choices.push_back("<");
    choices.push_back(">");
    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }

  parameters.push_back(SIMPL_NEW_BOOL_FP("Loop Until Gone", Loop, FilterParameter::Parameter, ReplaceElementAttributesWithNeighborValues));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));

  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::Defaults::AnyPrimitive, 1, AttributeMatrix::Category::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Comparison Array", ConfidenceIndexArrayPath, FilterParameter::RequiredArray, ReplaceElementAttributesWithNeighborValues, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReplaceElementAttributesWithNeighborValues::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setConfidenceIndexArrayPath(reader->readDataArrayPath("ConfidenceIndexArrayPath", getConfidenceIndexArrayPath()));
  setMinConfidence(reader->readValue("MinConfidence", getMinConfidence()));
  setLoop(reader->readValue("Loop", false));
  setSelectedComparison(reader->readValue("SelectedComparison", getSelectedComparison()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReplaceElementAttributesWithNeighborValues::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReplaceElementAttributesWithNeighborValues::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getConfidenceIndexArrayPath().getDataContainerName());

  m_InArrayPtr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getConfidenceIndexArrayPath());
  if(getErrorCode() < 0)
  {
    return;
  }

  std::vector<size_t> cDims = m_InArrayPtr.lock()->getComponentDimensions();
  if(cDims.size() != 1 && cDims.at(0) != 1)
  {
    QString ss = QObject::tr("The number of components must be 1.");
    setErrorCondition(-5655, ss);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReplaceElementAttributesWithNeighborValues::preflight()
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
void ReplaceElementAttributesWithNeighborValues::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  // This function is templated over the primitive type so we are going to use
  // the macros provided in TemplateHelpers.hpp to cut down on the amount of
  // redundant code
  EXECUTE_FUNCTION_TEMPLATE(this, Detail::ExecuteTemplate, m_InArrayPtr.lock(), this, m_InArrayPtr.lock());


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ReplaceElementAttributesWithNeighborValues::newFilterInstance(bool copyFilterParameters) const
{
  ReplaceElementAttributesWithNeighborValues::Pointer filter = ReplaceElementAttributesWithNeighborValues::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReplaceElementAttributesWithNeighborValues::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReplaceElementAttributesWithNeighborValues::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReplaceElementAttributesWithNeighborValues::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReplaceElementAttributesWithNeighborValues::getGroupName() const
{
  return SIMPL::FilterGroups::ProcessingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid ReplaceElementAttributesWithNeighborValues::getUuid() const
{
  return QUuid("{17410178-4e5f-58b9-900e-8194c69200ab}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReplaceElementAttributesWithNeighborValues::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::CleanupFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReplaceElementAttributesWithNeighborValues::getHumanLabel() const
{
  return "Replace Element Attributes with Neighbor (Threshold)";
}

// -----------------------------------------------------------------------------
ReplaceElementAttributesWithNeighborValues::Pointer ReplaceElementAttributesWithNeighborValues::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ReplaceElementAttributesWithNeighborValues> ReplaceElementAttributesWithNeighborValues::New()
{
  struct make_shared_enabler : public ReplaceElementAttributesWithNeighborValues
  {

  private:
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ReplaceElementAttributesWithNeighborValues::getNameOfClass() const
{
  return QString("ReplaceElementAttributesWithNeighborValues");
}

// -----------------------------------------------------------------------------
QString ReplaceElementAttributesWithNeighborValues::ClassName()
{
  return QString("ReplaceElementAttributesWithNeighborValues");
}

// -----------------------------------------------------------------------------
void ReplaceElementAttributesWithNeighborValues::setMinConfidence(float value)
{
  m_MinConfidence = value;
}

// -----------------------------------------------------------------------------
float ReplaceElementAttributesWithNeighborValues::getMinConfidence() const
{
  return m_MinConfidence;
}

// -----------------------------------------------------------------------------
void ReplaceElementAttributesWithNeighborValues::setLoop(bool value)
{
  m_Loop = value;
}

// -----------------------------------------------------------------------------
bool ReplaceElementAttributesWithNeighborValues::getLoop() const
{
  return m_Loop;
}

// -----------------------------------------------------------------------------
void ReplaceElementAttributesWithNeighborValues::setConfidenceIndexArrayPath(const DataArrayPath& value)
{
  m_ConfidenceIndexArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ReplaceElementAttributesWithNeighborValues::getConfidenceIndexArrayPath() const
{
  return m_ConfidenceIndexArrayPath;
}

// -----------------------------------------------------------------------------
void ReplaceElementAttributesWithNeighborValues::setSelectedComparison(int value)
{
  m_SelectedComparison = value;
}

// -----------------------------------------------------------------------------
int ReplaceElementAttributesWithNeighborValues::getSelectedComparison() const
{
  return m_SelectedComparison;
}

// -----------------------------------------------------------------------------
void ReplaceElementAttributesWithNeighborValues::setIgnoredDataArrayPaths(const QVector<DataArrayPath>& value)
{
  m_IgnoredDataArrayPaths = value;
}

// -----------------------------------------------------------------------------
QVector<DataArrayPath> ReplaceElementAttributesWithNeighborValues::getIgnoredDataArrayPaths() const
{
  return m_IgnoredDataArrayPaths;
}
