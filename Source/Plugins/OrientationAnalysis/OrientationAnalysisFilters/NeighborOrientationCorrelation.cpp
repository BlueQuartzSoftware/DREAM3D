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


#include "NeighborOrientationCorrelation.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "DREAM3DLib/FilterParameters/DoubleFilterParameter.h"
#include "DREAM3DLib/FilterParameters/IntFilterParameter.h"
#include "DREAM3DLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/SeparatorFilterParameter.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
NeighborOrientationCorrelation::NeighborOrientationCorrelation() :
  AbstractFilter(),
  m_MisorientationTolerance(5.0f),
  m_MinConfidence(0.1f),
  m_Level(6),
  m_ConfidenceIndexArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::ConfidenceIndex),
  m_CellPhasesArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Phases),
  m_CrystalStructuresArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_QuatsArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Quats),
  m_ConfidenceIndex(NULL),
  m_Quats(NULL),
  m_CellPhases(NULL),
  m_CrystalStructures(NULL)
{
  m_OrientationOps = SpaceGroupOps::getOrientationOpsQVector();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
NeighborOrientationCorrelation::~NeighborOrientationCorrelation()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NeighborOrientationCorrelation::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(DoubleFilterParameter::New("Minimum Confidence Index", "MinConfidence", getMinConfidence(), FilterParameter::Parameter));
  parameters.push_back(DoubleFilterParameter::New("Misorientation Tolerance (Degrees)", "MisorientationTolerance", getMisorientationTolerance(), FilterParameter::Parameter));
  parameters.push_back(IntFilterParameter::New("Cleanup Level", "Level", getLevel(), FilterParameter::Parameter));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  parameters.push_back(DataArraySelectionFilterParameter::New("Confidence Index", "ConfidenceIndexArrayPath", getConfidenceIndexArrayPath(), FilterParameter::RequiredArray));
  parameters.push_back(DataArraySelectionFilterParameter::New("Phases", "CellPhasesArrayPath", getCellPhasesArrayPath(), FilterParameter::RequiredArray));
  parameters.push_back(DataArraySelectionFilterParameter::New("Quaternions", "QuatsArrayPath", getQuatsArrayPath(), FilterParameter::RequiredArray));
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  parameters.push_back(DataArraySelectionFilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", getCrystalStructuresArrayPath(), FilterParameter::RequiredArray));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NeighborOrientationCorrelation::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setQuatsArrayPath(reader->readDataArrayPath("QuatsArrayPath", getQuatsArrayPath() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath() ) );
  setConfidenceIndexArrayPath(reader->readDataArrayPath("ConfidenceIndexArrayPath", getConfidenceIndexArrayPath() ) );
  setMinConfidence( reader->readValue("MinConfidence", getMinConfidence()) );
  setMisorientationTolerance( reader->readValue("MisorientationTolerance", getMisorientationTolerance()) );
  setLevel( reader->readValue("Level", getLevel()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int NeighborOrientationCorrelation::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(QuatsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CellPhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(ConfidenceIndexArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(MisorientationTolerance)
  DREAM3D_FILTER_WRITE_PARAMETER(MinConfidence)
  DREAM3D_FILTER_WRITE_PARAMETER(Level)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NeighborOrientationCorrelation::dataCheck()
{
  setErrorCondition(0);

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getConfidenceIndexArrayPath().getDataContainerName());

  QVector<DataArrayPath> dataArrayPaths;

  QVector<size_t> cDims(1, 1);
  m_ConfidenceIndexPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getConfidenceIndexArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ConfidenceIndexPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_ConfidenceIndex = m_ConfidenceIndexPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getConfidenceIndexArrayPath()); }

  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getCellPhasesArrayPath()); }

  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getCrystalStructuresArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 4;
  m_QuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getQuatsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_QuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Quats = m_QuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getQuatsArrayPath()); }

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NeighborOrientationCorrelation::preflight()
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
void NeighborOrientationCorrelation::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_ConfidenceIndexArrayPath.getDataContainerName());
  size_t totalPoints = m_ConfidenceIndexPtr.lock()->getNumberOfTuples();

  m_MisorientationTolerance = m_MisorientationTolerance * DREAM3D::Constants::k_Pi / 180.0f;

  size_t udims[3] = { 0, 0, 0 };
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);

  int64_t dims[3] =
  {
    static_cast<int64_t>(udims[0]),
    static_cast<int64_t>(udims[1]),
    static_cast<int64_t>(udims[2]),
  };

  size_t count = 1;
  int32_t best = 0;
  bool good = true;
  bool good2 = true;
  int64_t neighbor = 0;
  int64_t neighbor2 = 0;
  int64_t column = 0, row = 0, plane = 0;

  int64_t neighpoints[6] = { 0, 0, 0, 0, 0, 0 };
  neighpoints[0] = static_cast<int64_t>(-dims[0] * dims[1]);
  neighpoints[1] = static_cast<int64_t>(-dims[0]);
  neighpoints[2] = static_cast<int64_t>(-1);
  neighpoints[3] = static_cast<int64_t>(1);
  neighpoints[4] = static_cast<int64_t>(dims[0]);
  neighpoints[5] = static_cast<int64_t>(dims[0] * dims[1]);

  float w = std::numeric_limits<float>::max();
  QuatF q1 = QuaternionMathF::New();
  QuatF q2 = QuaternionMathF::New();
  float n1 = 0.0f, n2 = 0.0f, n3 = 0.0f;
  uint32_t phase1 = 0, phase2 = 0;

  QVector<int32_t> neighborDiffCount(totalPoints, 0);
  QVector<int32_t> neighborSimCount(6, 0);
  QVector<int64_t> bestNeighbor(totalPoints, -1);
  QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);

  int32_t startLevel = 6;
  for (int32_t currentLevel = startLevel; currentLevel > m_Level; currentLevel--)
  {
    if (getCancel()) { break; }

    int64_t progIncrement = static_cast<int64_t>(totalPoints / 100);
    int64_t prog = 1;
    int64_t progressInt = 0;
    for (size_t i = 0; i < totalPoints; i++)
    {
      if (int64_t(i) > prog)
      {
        progressInt = static_cast<int64_t>(((float)i / totalPoints) * 100.0f);
        QString ss = QObject::tr("Level %1 of %2 || Processing Data %3%").arg((startLevel - currentLevel) + 1).arg(startLevel - m_Level).arg(progressInt);
        notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
        prog = prog + progIncrement;
      }

      if (m_ConfidenceIndex[i] < m_MinConfidence)
      {
        count = 0;
        column = i % dims[0];
        row = (i / dims[0]) % dims[1];
        plane = i / (dims[0] * dims[1]);
        for (int32_t j = 0; j < 6; j++)
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
            phase1 = m_CrystalStructures[m_CellPhases[i]];
            QuaternionMathF::Copy(quats[i], q1);

            phase2 = m_CrystalStructures[m_CellPhases[neighbor]];
            QuaternionMathF::Copy(quats[neighbor], q2);

            if (m_CellPhases[i] == m_CellPhases[neighbor] && m_CellPhases[i] > 0) { w = m_OrientationOps[phase1]->getMisoQuat(q1, q2, n1, n2, n3); }
            if (w > m_MisorientationTolerance)
            {
              neighborDiffCount[i]++;
            }
            for (int32_t k = j + 1; k < 6; k++)
            {
              good2 = true;
              neighbor2 = int64_t(i) + neighpoints[k];
              if (k == 0 && plane == 0) { good2 = false; }
              if (k == 5 && plane == (dims[2] - 1)) { good2 = false; }
              if (k == 1 && row == 0) { good2 = false; }
              if (k == 4 && row == (dims[1] - 1)) { good2 = false; }
              if (k == 2 && column == 0) { good2 = false; }
              if (k == 3 && column == (dims[0] - 1)) { good2 = false; }
              if (good2 == true)
              {
                phase1 = m_CrystalStructures[m_CellPhases[neighbor2]];
                QuaternionMathF::Copy(quats[neighbor2], q1);

                phase2 = m_CrystalStructures[m_CellPhases[neighbor]];
                QuaternionMathF::Copy(quats[neighbor], q2);
                if (m_CellPhases[neighbor2] == m_CellPhases[neighbor] && m_CellPhases[neighbor2] > 0)
                {
                  w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
                }
                if (w < m_MisorientationTolerance)
                {
                  neighborSimCount[j]++;
                  neighborSimCount[k]++;
                }
              }
            }
          }
        }
        for (int32_t j = 0; j < 6; j++)
        {
          best = 0;
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
            if (neighborSimCount[j] > best)
            {
              best = neighborSimCount[j];
              bestNeighbor[i] = neighbor;
            }
            neighborSimCount[j] = 0;
          }
        }
      }
    }
    QString attrMatName = m_ConfidenceIndexArrayPath.getAttributeMatrixName();
    QList<QString> voxelArrayNames = m->getAttributeMatrix(attrMatName)->getAttributeArrayNames();

    if (getCancel()) { return; }

    progIncrement = static_cast<int64_t>(totalPoints / 100);
    prog = 1;
    progressInt = 0;
    for (size_t i = 0; i < totalPoints; i++)
    {
      if (int64_t(i) > prog)
      {
        progressInt = static_cast<int64_t>(((float)i / totalPoints) * 100.0f);
        QString ss = QObject::tr("Level %1 of %2 || Copying Data %3%").arg((startLevel - currentLevel) + 2).arg(startLevel - m_Level).arg(progressInt);
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
    currentLevel = currentLevel - 1;
  }

  if (getCancel()) { return; }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer NeighborOrientationCorrelation::newFilterInstance(bool copyFilterParameters)
{
  NeighborOrientationCorrelation::Pointer filter = NeighborOrientationCorrelation::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString NeighborOrientationCorrelation::getCompiledLibraryName()
{ return OrientationAnalysisConstants::OrientationAnalysisBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString NeighborOrientationCorrelation::getGroupName()
{ return DREAM3D::FilterGroups::ProcessingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString NeighborOrientationCorrelation::getSubGroupName()
{ return DREAM3D::FilterSubGroups::CleanupFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString NeighborOrientationCorrelation::getHumanLabel()
{ return "Neighbor Orientation Correlation"; }
