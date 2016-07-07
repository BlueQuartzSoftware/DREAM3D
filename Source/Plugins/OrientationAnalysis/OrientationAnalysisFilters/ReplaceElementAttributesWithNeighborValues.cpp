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
#include "ReplaceElementAttributesWithNeighborValues.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/TemplateHelpers.hpp"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"


namespace Detail
{
  static const int LessThan = 0;
  static const int GreaterThan = 1;

  template<typename T>
  class LessThanComparison
  {
    public:
      SIMPL_SHARED_POINTERS(LessThanComparison<T>)
      SIMPL_STATIC_NEW_MACRO(LessThanComparison<T>)
      SIMPL_TYPE_MACRO(LessThanComparison<T>)
      virtual ~LessThanComparison() {}

      virtual bool compare(T a, T b) { return a < b; }
      virtual bool compare1(T a, T b) { return a >= b; }
      virtual bool compare2(T a, T b) { return a > b; }

    protected:
      LessThanComparison() {}


  };

  template<typename T>
  class GreaterThanComparison : public LessThanComparison<T>
  {
    public:
      SIMPL_SHARED_POINTERS(GreaterThanComparison<T>)
      SIMPL_STATIC_NEW_MACRO(GreaterThanComparison<T>)
      SIMPL_TYPE_MACRO_SUPER(GreaterThanComparison<T>, LessThanComparison<T>)
      virtual ~GreaterThanComparison() {}

      virtual bool compare(T a, T b) { return a > b; }
      virtual bool compare1(T a, T b) { return a <= b; }
      virtual bool compare2(T a, T b) { return a < b; }

    protected:
      GreaterThanComparison() {}

  };

  template<typename T>
  void ExecuteTemplate(ReplaceElementAttributesWithNeighborValues* filter, IDataArray::Pointer inArrayPtr)
  {
    typedef DataArray<T> DataArrayType;
    typedef typename DataArrayType::Pointer DataArrayPointerType;


    DataArrayPath dataArrayPath = filter->getConfidenceIndexArrayPath();
    DataContainer::Pointer m = filter->getDataContainerArray()->getDataContainer(dataArrayPath.getDataContainerName());
    size_t totalPoints = inArrayPtr->getNumberOfTuples();

    size_t udims[3] = { 0, 0, 0 };
    m->getGeometryAs<ImageGeom>()->getDimensions(udims);
    int64_t dims[3] =
    {
      static_cast<int64_t>(udims[0]),
      static_cast<int64_t>(udims[1]),
      static_cast<int64_t>(udims[2]),
    };

    bool good = true;
    int64_t neighbor = 0;
    int64_t column = 0, row = 0, plane = 0;

    int64_t neighpoints[6] = { 0, 0, 0, 0, 0, 0 };
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

    while (keepGoing == true)
    {
      keepGoing = false;
      count = 0;
      if (filter->getCancel()) { break; }

      int64_t progIncrement = static_cast<int64_t>(totalPoints / 50);
      int64_t prog = 1;
      int64_t progressInt = 0;
      for (size_t i = 0; i < totalPoints; i++)
      {
        if(comp->compare(data[i], thresholdValue))
        {
          column = i % dims[0];
          row = (i / dims[0]) % dims[1];
          plane = i / (dims[0] * dims[1]);
          count++;
          best = data[i];
          for (int64_t j = 0; j < 6; j++)
          {
            good = true;
            neighbor = int64_t(i) + neighpoints[j];
            if (j == 0 && plane == 0) { good = false; }
            if (j == 5 && plane == (dims[2] - 1)) { good = false; }
            if (j == 1 && row == 0) { good = false; }
            if (j == 4 && row == (dims[1] - 1)) { good = false; }
            if (j == 2 && column == 0) { good = false; }
            if (j == 3 && column == (dims[0] - 1)) { good = false; }
            if (good == true)
            {
              if (comp->compare1(data[neighbor], thresholdValue) && comp->compare2(data[neighbor], best) )
              {
                best = data[neighbor];
                bestNeighbor[i] = neighbor;
              }
            }
          }
        }
        if (int64_t(i) > prog)
        {
          progressInt = static_cast<int64_t>(((float)i / totalPoints) * 100.0f);
          QString ss = QObject::tr("|| Processing Data Current Loop (%1) Progress: %2% Complete").arg(count).arg(progressInt);
          filter->notifyStatusMessage(filter->getMessagePrefix(), filter->getHumanLabel(), ss);
          prog = prog + progIncrement;
        }
      }
      QString attrMatName = dataArrayPath.getAttributeMatrixName();
      QList<QString> voxelArrayNames = m->getAttributeMatrix(attrMatName)->getAttributeArrayNames();

      if (filter->getCancel()) { break; }

      progIncrement = static_cast<int64_t>(totalPoints / 50);
      prog = 1;
      progressInt = 0;
      for (size_t i = 0; i < totalPoints; i++)
      {
        if (int64_t(i) > prog)
        {
          progressInt = static_cast<int64_t>(((float)i / totalPoints) * 100.0f);
          QString ss = QObject::tr("|| Processing Data Current Loop (%1) || Transferring Cell Data: %2% Complete").arg(count).arg(progressInt);
          filter->notifyStatusMessage(filter->getMessagePrefix(), filter->getHumanLabel(), ss);
          prog = prog + progIncrement;
        }

        neighbor = bestNeighbor[i];
        if (neighbor != -1)
        {
          for (QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
          {
            IDataArray::Pointer p = m->getAttributeMatrix(attrMatName)->getAttributeArray(*iter);
            p->copyTuple(neighbor, i);
          }
        }
      }
      if (filter->getLoop() == true && count > 0) { keepGoing = true; }
    }


  }

}



// Include the MOC generated file for this class
#include "moc_ReplaceElementAttributesWithNeighborValues.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReplaceElementAttributesWithNeighborValues::ReplaceElementAttributesWithNeighborValues() :
  AbstractFilter(),
  m_MinConfidence(0.1f),
  m_Loop(false),
  m_ConfidenceIndexArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::ConfidenceIndex),
  m_SelectedComparison(Detail::LessThan)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReplaceElementAttributesWithNeighborValues::~ReplaceElementAttributesWithNeighborValues()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReplaceElementAttributesWithNeighborValues::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(DoubleFilterParameter::New("Threshold Value", "MinConfidence", getMinConfidence(), FilterParameter::Parameter));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Comparison Operator");
    parameter->setPropertyName("SelectedComparison");

    QVector<QString> choices;
    choices.push_back("<");
    choices.push_back(">");
    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }

  parameters.push_back(BooleanFilterParameter::New("Loop Until Gone", "Loop", getLoop(), FilterParameter::Parameter));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));

  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::Defaults::AnyPrimitive, 1, SIMPL::AttributeMatrixObjectType::Any);
    parameters.push_back(DataArraySelectionFilterParameter::New("Comparison Array", "ConfidenceIndexArrayPath", getConfidenceIndexArrayPath(), FilterParameter::RequiredArray, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReplaceElementAttributesWithNeighborValues::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setConfidenceIndexArrayPath(reader->readDataArrayPath("ConfidenceIndexArrayPath", getConfidenceIndexArrayPath() ) );
  setMinConfidence( reader->readValue("MinConfidence", getMinConfidence()) );
  setLoop( reader->readValue("Loop", false) );
  setSelectedComparison(reader->readValue("SelectedComparison", getSelectedComparison()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ReplaceElementAttributesWithNeighborValues::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(ConfidenceIndexArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(MinConfidence)
  SIMPL_FILTER_WRITE_PARAMETER(Loop)
  SIMPL_FILTER_WRITE_PARAMETER(SelectedComparison)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
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
  setErrorCondition(0);

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getConfidenceIndexArrayPath().getDataContainerName());

  m_InArrayPtr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getConfidenceIndexArrayPath());
  if(getErrorCondition() < 0) { return; }


  QVector<size_t> cDims = m_InArrayPtr.lock()->getComponentDimensions();
  if(cDims.size() != 1 && cDims.at(0) != 1)
  {
    QString ss = QObject::tr("The number of components must be 1.");
    setErrorCondition(-5655);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
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
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  // This function is templated over the primitive type so we are going to use
  // the macros provided in TemplateHelpers.hpp to cut down on the amount of
  // redundant code
  EXECUTE_FUNCTION_TEMPLATE(this, Detail::ExecuteTemplate, m_InArrayPtr.lock(), this, m_InArrayPtr.lock());

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ReplaceElementAttributesWithNeighborValues::newFilterInstance(bool copyFilterParameters)
{
  ReplaceElementAttributesWithNeighborValues::Pointer filter = ReplaceElementAttributesWithNeighborValues::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReplaceElementAttributesWithNeighborValues::getCompiledLibraryName()
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReplaceElementAttributesWithNeighborValues::getBrandingString()
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReplaceElementAttributesWithNeighborValues::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReplaceElementAttributesWithNeighborValues::getGroupName()
{ return SIMPL::FilterGroups::ProcessingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReplaceElementAttributesWithNeighborValues::getSubGroupName()
{ return SIMPL::FilterSubGroups::CleanupFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReplaceElementAttributesWithNeighborValues::getHumanLabel()
{ return "Replace Element Attributes with Neighbor (Threshold)"; }
