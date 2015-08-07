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
#include "NeighborCICorrelation.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/FilterParameters/DoubleFilterParameter.h"
#include "DREAM3DLib/FilterParameters/ChoiceFilterParameter.h"
#include "DREAM3DLib/FilterParameters/BooleanFilterParameter.h"
#include "DREAM3DLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/SeparatorFilterParameter.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"


namespace Detail
{
  static const int LessThan = 0;
  static const int GreaterThan = 1;

  class LessThanComparison
  {
    public:
      DREAM3D_SHARED_POINTERS(LessThanComparison)
      DREAM3D_STATIC_NEW_MACRO(LessThanComparison)
      DREAM3D_TYPE_MACRO(LessThanComparison)
      virtual ~LessThanComparison(){}

      virtual bool compare(float a, float b) { return a < b; }
      virtual bool compare1(float a, float b) { return a >= b; }
      virtual bool compare2(float a, float b) { return a > b; }

    protected:
      LessThanComparison(){}


  };

  class GreaterThanComparison : public LessThanComparison
  {
    public:
      DREAM3D_SHARED_POINTERS(GreaterThanComparison)
      DREAM3D_STATIC_NEW_MACRO(GreaterThanComparison)
      DREAM3D_TYPE_MACRO_SUPER(GreaterThanComparison, LessThanComparison)
      virtual ~GreaterThanComparison() {}

      virtual bool compare(float a, float b) { return a > b; }
      virtual bool compare1(float a, float b) { return a <= b; }
      virtual bool compare2(float a, float b) { return a < b; }

    protected:
      GreaterThanComparison(){}

  };

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
NeighborCICorrelation::NeighborCICorrelation() :
  AbstractFilter(),
  m_MinConfidence(0.1f),
  m_Loop(false),
  m_ConfidenceIndexArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::ConfidenceIndex),
  m_SelectedComparison(Detail::LessThan),
  m_ConfidenceIndex(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
NeighborCICorrelation::~NeighborCICorrelation()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NeighborCICorrelation::setupFilterParameters()
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
  parameters.push_back(DataArraySelectionFilterParameter::New("Comparison Array", "ConfidenceIndexArrayPath", getConfidenceIndexArrayPath(), FilterParameter::RequiredArray));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NeighborCICorrelation::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setConfidenceIndexArrayPath(reader->readDataArrayPath("ConfidenceIndexArrayPath", getConfidenceIndexArrayPath() ) );
  setMinConfidence( reader->readValue("MinConfidence", getMinConfidence()) );
  setLoop( reader->readValue("Loop", false) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int NeighborCICorrelation::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(ConfidenceIndexArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(MinConfidence)
  DREAM3D_FILTER_WRITE_PARAMETER(Loop)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NeighborCICorrelation::dataCheck()
{
  setErrorCondition(0);

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getConfidenceIndexArrayPath().getDataContainerName());

  QVector<size_t> cDims(1, 1);
  m_ConfidenceIndexPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getConfidenceIndexArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ConfidenceIndexPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_ConfidenceIndex = m_ConfidenceIndexPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NeighborCICorrelation::preflight()
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
void NeighborCICorrelation::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_ConfidenceIndexArrayPath.getDataContainerName());
  size_t totalPoints = m_ConfidenceIndexPtr.lock()->getNumberOfTuples();

  size_t udims[3] = { 0, 0, 0 };
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  bool good = true;
  DimType neighbor = 0;
  DimType column = 0, row = 0, plane = 0;

  DimType neighpoints[6] = { 0, 0, 0, 0, 0, 0 };
  neighpoints[0] = static_cast<DimType>(-dims[0] * dims[1]);
  neighpoints[1] = static_cast<DimType>(-dims[0]);
  neighpoints[2] = static_cast<DimType>(-1);
  neighpoints[3] = static_cast<DimType>(1);
  neighpoints[4] = static_cast<DimType>(dims[0]);
  neighpoints[5] = static_cast<DimType>(dims[0] * dims[1]);

  QVector<DimType> bestNeighbor(totalPoints, -1);

  size_t count = 0;
  float best = 0.0f;
  bool keepGoing = true;

  Detail::LessThanComparison::Pointer comp = Detail::LessThanComparison::New();
  if(m_SelectedComparison == Detail::GreaterThan)
  {
    comp = Detail::GreaterThanComparison::New();
  }


  while (keepGoing == true)
  {
    keepGoing = false;
    count = 0;
    if (getCancel()) { break; }

    DimType progIncrement = static_cast<DimType>(totalPoints / 50);
    DimType prog = 1;
    int64_t progressInt = 0;
    for (size_t i = 0; i < totalPoints; i++)
    {
      if(comp->compare(m_ConfidenceIndex[i], m_MinConfidence))
      {
        column = i % dims[0];
        row = (i / dims[0]) % dims[1];
        plane = i / (dims[0] * dims[1]);
        count++;
        best = m_ConfidenceIndex[i];
        for (DimType j = 0; j < 6; j++)
        {
          good = true;
          neighbor = DimType(i) + neighpoints[j];
          if (j == 0 && plane == 0) { good = false; }
          if (j == 5 && plane == (dims[2] - 1)) { good = false; }
          if (j == 1 && row == 0) { good = false; }
          if (j == 4 && row == (dims[1] - 1)) { good = false; }
          if (j == 2 && column == 0) { good = false; }
          if (j == 3 && column == (dims[0] - 1)) { good = false; }
          if (good == true)
          {
            if (comp->compare1(m_ConfidenceIndex[neighbor], m_MinConfidence) && comp->compare2(m_ConfidenceIndex[neighbor], best) )
            {
              best = m_ConfidenceIndex[neighbor];
              bestNeighbor[i] = neighbor;
            }
          }
        }
      }
      if (DimType(i) > prog)
      {
        progressInt = static_cast<int64_t>(((float)i / totalPoints) * 100.0f);
        QString ss = QObject::tr("|| Processing Data Current Loop (%1) Progress: %2% Complete").arg(count).arg(progressInt);
        notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
        prog = prog + progIncrement;
      }
    }
    QString attrMatName = m_ConfidenceIndexArrayPath.getAttributeMatrixName();
    QList<QString> voxelArrayNames = m->getAttributeMatrix(attrMatName)->getAttributeArrayNames();

    if (getCancel()) { break; }

    progIncrement = static_cast<DimType>(totalPoints / 50);
    prog = 1;
    progressInt = 0;
    for (size_t i = 0; i < totalPoints; i++)
    {
      if (DimType(i) > prog)
      {
        progressInt = static_cast<int64_t>(((float)i / totalPoints) * 100.0f);
        QString ss = QObject::tr("|| Processing Data Current Loop (%1) || Transferring Cell Data: %2% Complete").arg(count).arg(progressInt);
        notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
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
    if (m_Loop == true && count > 0) { keepGoing = true; }
  }

  if (getCancel()) { return; }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer NeighborCICorrelation::newFilterInstance(bool copyFilterParameters)
{
  NeighborCICorrelation::Pointer filter = NeighborCICorrelation::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString NeighborCICorrelation::getCompiledLibraryName()
{ return OrientationAnalysisConstants::OrientationAnalysisBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString NeighborCICorrelation::getGroupName()
{ return DREAM3D::FilterGroups::ProcessingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString NeighborCICorrelation::getSubGroupName()
{ return DREAM3D::FilterSubGroups::CleanupFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString NeighborCICorrelation::getHumanLabel()
{ return "Neighbor CI Correlation"; }
