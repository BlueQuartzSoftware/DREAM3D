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

#include "NeighborOrientationCorrelation.h"

#include <vector>

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/atomic.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/task_group.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/tick_count.h>
#endif

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

class NeighborOrientationCorrelationTransferDataImpl
{
public:
  NeighborOrientationCorrelationTransferDataImpl() = delete;
  NeighborOrientationCorrelationTransferDataImpl(const NeighborOrientationCorrelationTransferDataImpl&) = default; // Copy Constructor Not Implemented

  NeighborOrientationCorrelationTransferDataImpl(NeighborOrientationCorrelation* filter, size_t totalPoints, const std::vector<int64_t>& bestNeighbor, AttributeMatrix* attrMat,
                                                 int32_t voxelArrayIndex)
  : m_Filter(filter)
  , m_TotalPoints(totalPoints)
  , m_BestNeighbor(bestNeighbor)
  , m_AttrMat(attrMat)
  , m_VoxelArrayIndex(voxelArrayIndex)
  {
  }

  ~NeighborOrientationCorrelationTransferDataImpl() = default;

  void operator()() const
  {

    QVector<QString> voxelArrayNames = m_AttrMat->getAttributeArrayNames().toVector();

    size_t progIncrement = static_cast<size_t>(m_TotalPoints / 50);
    size_t prog = 1;
    for(size_t i = 0; i < m_TotalPoints; i++)
    {
      if(i > prog)
      {
        prog = prog + progIncrement;
        m_Filter->updateProgress(progIncrement);
      }
      int64_t neighbor = m_BestNeighbor[i];
      if(neighbor != -1)
      {
        IDataArray::Pointer p = m_AttrMat->getAttributeArray(voxelArrayNames[m_VoxelArrayIndex]);
        p->copyTuple(neighbor, i);
      }
    }
  }

private:
  NeighborOrientationCorrelation* m_Filter = nullptr;
  size_t m_TotalPoints = 0;
  std::vector<int64_t> m_BestNeighbor;
  AttributeMatrix* m_AttrMat = nullptr;
  int32_t m_VoxelArrayIndex = 0;

  void operator=(const NeighborOrientationCorrelationTransferDataImpl&) = delete; // Move assignment Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
NeighborOrientationCorrelation::NeighborOrientationCorrelation()
: m_MisorientationTolerance(5.0f)
, m_MinConfidence(0.1f)
, m_Level(6)
, m_ConfidenceIndexArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::ConfidenceIndex)
, m_CellPhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Phases)
, m_CrystalStructuresArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::CrystalStructures)
, m_QuatsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Quats)
, m_ConfidenceIndex(nullptr)
, m_Quats(nullptr)
, m_CellPhases(nullptr)
, m_CrystalStructures(nullptr)
{
  m_OrientationOps = LaueOps::getOrientationOpsQVector();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
NeighborOrientationCorrelation::~NeighborOrientationCorrelation() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NeighborOrientationCorrelation::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SIMPL_NEW_FLOAT_FP("Minimum Confidence Index", MinConfidence, FilterParameter::Parameter, NeighborOrientationCorrelation));
  parameters.push_back(SIMPL_NEW_FLOAT_FP("Misorientation Tolerance (Degrees)", MisorientationTolerance, FilterParameter::Parameter, NeighborOrientationCorrelation));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Cleanup Level", Level, FilterParameter::Parameter, NeighborOrientationCorrelation));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));

  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Confidence Index", ConfidenceIndexArrayPath, FilterParameter::RequiredArray, NeighborOrientationCorrelation, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", CellPhasesArrayPath, FilterParameter::RequiredArray, NeighborOrientationCorrelation, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 4, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Quaternions", QuatsArrayPath, FilterParameter::RequiredArray, NeighborOrientationCorrelation, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt32, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Crystal Structures", CrystalStructuresArrayPath, FilterParameter::RequiredArray, NeighborOrientationCorrelation, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NeighborOrientationCorrelation::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setQuatsArrayPath(reader->readDataArrayPath("QuatsArrayPath", getQuatsArrayPath()));
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath()));
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath()));
  setConfidenceIndexArrayPath(reader->readDataArrayPath("ConfidenceIndexArrayPath", getConfidenceIndexArrayPath()));
  setMinConfidence(reader->readValue("MinConfidence", getMinConfidence()));
  setMisorientationTolerance(reader->readValue("MisorientationTolerance", getMisorientationTolerance()));
  setLevel(reader->readValue("Level", getLevel()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NeighborOrientationCorrelation::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NeighborOrientationCorrelation::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getConfidenceIndexArrayPath().getDataContainerName());

  QVector<DataArrayPath> dataArrayPaths;

  QVector<size_t> cDims(1, 1);
  m_ConfidenceIndexPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getConfidenceIndexArrayPath(),
                                                                                                           cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_ConfidenceIndexPtr.lock())                                                                       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_ConfidenceIndex = m_ConfidenceIndexPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrayPaths.push_back(getConfidenceIndexArrayPath());
  }

  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CellPhasesPtr.lock())                                                                         /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrayPaths.push_back(getCellPhasesArrayPath());
  }

  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getCrystalStructuresArrayPath(),
                                                                                                                cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CrystalStructuresPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 4;
  m_QuatsPtr =
      getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getQuatsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_QuatsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Quats = m_QuatsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrayPaths.push_back(getQuatsArrayPath());
  }

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
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  m_Progress = 0;
  m_TotalProgress = 0;

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_ConfidenceIndexArrayPath.getDataContainerName());
  size_t totalPoints = m_ConfidenceIndexPtr.lock()->getNumberOfTuples();

  float misorientationToleranceR = m_MisorientationTolerance * static_cast<float>(SIMPLib::Constants::k_PiOver180);

  size_t udims[3] = {0, 0, 0};
  std::tie(udims[0], udims[1], udims[2]) = m->getGeometryAs<ImageGeom>()->getDimensions();

  int64_t dims[3] = {
      static_cast<int64_t>(udims[0]), static_cast<int64_t>(udims[1]), static_cast<int64_t>(udims[2]),
  };

  size_t count = 1;
  int32_t best = 0;
  bool good = true;
  bool good2 = true;
  int64_t neighbor = 0;
  int64_t neighbor2 = 0;
  int64_t column = 0, row = 0, plane = 0;

  int64_t neighpoints[6] = {0, 0, 0, 0, 0, 0};
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

  std::vector<int32_t> neighborDiffCount(totalPoints, 0);
  std::vector<int32_t> neighborSimCount(6, 0);
  std::vector<int64_t> bestNeighbor(totalPoints, -1);
  QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);

  const int32_t startLevel = 6;
  for(int32_t currentLevel = startLevel; currentLevel > m_Level; currentLevel--)
  {
    if(getCancel())
    {
      break;
    }

    int64_t progIncrement = static_cast<int64_t>(totalPoints / 100);
    int64_t prog = 1;
    int64_t progressInt = 0;
    for(size_t i = 0; i < totalPoints; i++)
    {
      if(int64_t(i) > prog)
      {
        progressInt = static_cast<int64_t>((static_cast<float>(i) / totalPoints) * 100.0f);
        QString ss = QObject::tr("Level %1 of %2 || Processing Data %3%").arg((startLevel - currentLevel) + 1).arg(startLevel - m_Level).arg(progressInt);
        notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
        prog = prog + progIncrement;
      }

      if(m_ConfidenceIndex[i] < m_MinConfidence)
      {
        count = 0;
        column = static_cast<int64_t>(i % dims[0]);
        row = (i / dims[0]) % dims[1];
        plane = i / (dims[0] * dims[1]);
        for(size_t j = 0; j < 6; j++)
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
            phase1 = m_CrystalStructures[m_CellPhases[i]];
            QuaternionMathF::Copy(quats[i], q1);

            phase2 = m_CrystalStructures[m_CellPhases[neighbor]];
            QuaternionMathF::Copy(quats[neighbor], q2);

            if(m_CellPhases[i] == m_CellPhases[neighbor] && m_CellPhases[i] > 0)
            {
              w = m_OrientationOps[phase1]->getMisoQuat(q1, q2, n1, n2, n3);
            }
            if(w > misorientationToleranceR)
            {
              neighborDiffCount[i]++;
            }
            for(size_t k = j + 1; k < 6; k++)
            {
              good2 = true;
              neighbor2 = int64_t(i) + neighpoints[k];
              if(k == 0 && plane == 0)
              {
                good2 = false;
              }
              if(k == 5 && plane == (dims[2] - 1))
              {
                good2 = false;
              }
              if(k == 1 && row == 0)
              {
                good2 = false;
              }
              if(k == 4 && row == (dims[1] - 1))
              {
                good2 = false;
              }
              if(k == 2 && column == 0)
              {
                good2 = false;
              }
              if(k == 3 && column == (dims[0] - 1))
              {
                good2 = false;
              }
              if(good2)
              {
                phase1 = m_CrystalStructures[m_CellPhases[neighbor2]];
                QuaternionMathF::Copy(quats[neighbor2], q1);

                phase2 = m_CrystalStructures[m_CellPhases[neighbor]];
                QuaternionMathF::Copy(quats[neighbor], q2);
                if(m_CellPhases[neighbor2] == m_CellPhases[neighbor] && m_CellPhases[neighbor2] > 0)
                {
                  w = m_OrientationOps[phase1]->getMisoQuat(q1, q2, n1, n2, n3);
                }
                if(w < misorientationToleranceR)
                {
                  neighborSimCount[j]++;
                  neighborSimCount[k]++;
                }
              }
            }
          }
        }
        for(size_t j = 0; j < 6; j++)
        {
          best = 0;
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
            if(neighborSimCount[j] > best)
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

    if(getCancel())
    {
      return;
    }
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
    tbb::task_scheduler_init init;
    bool doParallel = true;
#endif

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
    // The idea for this parallel section is to parallelize over each Data Array that
    // will need it's data adjusted. This should go faster than before by about 2x.
    // Better speed up could be achieved if we had better data locality.
    m_Progress = 0;
    m_TotalProgress = 0;
    if(doParallel)
    {
      std::shared_ptr<tbb::task_group> g(new tbb::task_group);
      AttributeMatrix* attrMat = m->getAttributeMatrix(attrMatName).get();
      QVector<QString> voxelArrayNames = m->getAttributeMatrix(attrMatName)->getAttributeArrayNames().toVector();
      m_TotalProgress = voxelArrayNames.size() * totalPoints; // Total number of points to update
      // Create and run all the tasks
      for(int32_t voxelArray = 0; voxelArray < voxelArrayNames.size(); voxelArray++)
      {
        g->run(NeighborOrientationCorrelationTransferDataImpl(this, totalPoints, bestNeighbor, attrMat, voxelArray));
      }
      // Wait for them to complete.
      g->wait();
      
    }
    else
#endif
    {
      QList<QString> voxelArrayNames = m->getAttributeMatrix(attrMatName)->getAttributeArrayNames();
      progIncrement = static_cast<int64_t>(totalPoints / 100);
      prog = 1;
      progressInt = 0;
      for(size_t i = 0; i < totalPoints; i++)
      {
        if(int64_t(i) > prog)
        {
          progressInt = static_cast<int64_t>(((float)i / totalPoints) * 100.0f);
          QString ss = QObject::tr("Level %1 of %2 || Copying Data %3%").arg((startLevel - currentLevel) + 2).arg(startLevel - m_Level).arg(progressInt);
          notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
          prog = prog + progIncrement;
        }
        neighbor = bestNeighbor[i];
        if(neighbor != -1)
        {
          for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
          {
            IDataArray::Pointer p = m->getAttributeMatrix(attrMatName)->getAttributeArray(*iter);
            p->copyTuple(neighbor, i);
          }
        }
      }
    }

    currentLevel = currentLevel - 1;
    m_CurrentLevel = currentLevel;
  }

  if(getCancel())
  {
    return;
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NeighborOrientationCorrelation::updateProgress(size_t p)
{
  m_Progress += p;
  int32_t progressInt = static_cast<int>((static_cast<float>(m_Progress) / static_cast<float>(m_TotalProgress)) * 100.0f);
  QString ss = QObject::tr("Level %1 of %2 || Copying Data %3%").arg((6 - m_CurrentLevel) + 2).arg(6 - m_Level).arg(progressInt);
  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer NeighborOrientationCorrelation::newFilterInstance(bool copyFilterParameters) const
{
  NeighborOrientationCorrelation::Pointer filter = NeighborOrientationCorrelation::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString NeighborOrientationCorrelation::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString NeighborOrientationCorrelation::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString NeighborOrientationCorrelation::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString NeighborOrientationCorrelation::getGroupName() const
{
  return SIMPL::FilterGroups::ProcessingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid NeighborOrientationCorrelation::getUuid()
{
  return QUuid("{6427cd5e-0ad2-5a24-8847-29f8e0720f4f}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString NeighborOrientationCorrelation::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::CleanupFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString NeighborOrientationCorrelation::getHumanLabel() const
{
  return "Neighbor Orientation Correlation";
}
