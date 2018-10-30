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

#include "AlignSections.h"

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/atomic.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/task_group.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/tick_count.h>
#endif

#include "Reconstruction/ReconstructionConstants.h"
#include "Reconstruction/ReconstructionVersion.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/TemplateHelpers.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Utilities/FileSystemPathHelper.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T> void initializeArrayValues(IDataArray::Pointer p, size_t index)
{

  typename DataArray<T>::Pointer ptr = std::dynamic_pointer_cast<DataArray<T>>(p);
  T var = static_cast<T>(0);
  ptr->initializeTuple(index, &var);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class AlignSectionsTransferDataImpl
{
public:
  AlignSectionsTransferDataImpl() = delete;
  AlignSectionsTransferDataImpl(const AlignSectionsTransferDataImpl&) = default; // Copy Constructor Not Implemented

  AlignSectionsTransferDataImpl(AlignSections* filter, size_t* dims, const std::vector<int64_t>& xshifts, const std::vector<int64_t>& yshifts, int32_t voxelArrayIndex)
  : m_Filter(filter)
  , m_Dims(dims)
  , m_xshifts(xshifts)
  , m_yshifts(yshifts)
  , m_VoxelArrayIndex(voxelArrayIndex)
  {
  }

  ~AlignSectionsTransferDataImpl() = default;

  void operator()() const
  {
    DataContainer::Pointer m = m_Filter->getDataContainerArray()->getDataContainer(m_Filter->getDataContainerName());
    QVector<QString> voxelArrayNames = m->getAttributeMatrix(m_Filter->getCellAttributeMatrixName())->getAttributeArrayNames().toVector();
    size_t progIncrement = m_Dims[2] / 50;
    size_t prog = 1;
    size_t slice = 0;

    for(size_t i = 1; i < m_Dims[2]; i++)
    {
      if(i > prog)
      {
        prog = prog + progIncrement;
        m_Filter->updateProgress(progIncrement);
      }
      if(m_Filter->getCancel() == true)
      {
        return;
      }
      slice = (m_Dims[2] - 1) - i;
      for(size_t l = 0; l < m_Dims[1]; l++)
      {
        for(size_t n = 0; n < m_Dims[0]; n++)
        {
          int64_t xspot = 0, yspot = 0;
          int64_t newPosition = 0;
          int64_t currentPosition = 0;
          if(m_yshifts[i] >= 0)
          {
            yspot = static_cast<int64_t>(l);
          }
          else if(m_yshifts[i] < 0)
          {
            yspot = static_cast<int64_t>(m_Dims[1]) - 1 - static_cast<int64_t>(l);
          }
          if(m_xshifts[i] >= 0)
          {
            xspot = static_cast<int64_t>(n);
          }
          else if(m_xshifts[i] < 0)
          {
            xspot = static_cast<int64_t>(m_Dims[0]) - 1 - static_cast<int64_t>(n);
          }
          newPosition = (slice * m_Dims[0] * m_Dims[1]) + (yspot * m_Dims[0]) + xspot;
          currentPosition = (slice * m_Dims[0] * m_Dims[1]) + ((yspot + m_yshifts[i]) * m_Dims[0]) + (xspot + m_xshifts[i]);
          if((yspot + m_yshifts[i]) >= 0 && (yspot + m_yshifts[i]) <= static_cast<int64_t>(m_Dims[1]) - 1 && (xspot + m_xshifts[i]) >= 0 &&
             (xspot + m_xshifts[i]) <= static_cast<int64_t>(m_Dims[0]) - 1)
          {
            IDataArray::Pointer p = m->getAttributeMatrix(m_Filter->getCellAttributeMatrixName())->getAttributeArray(voxelArrayNames[m_VoxelArrayIndex]);
            p->copyTuple(static_cast<size_t>(currentPosition), static_cast<size_t>(newPosition));
          }
          if((yspot + m_yshifts[i]) < 0 || (yspot + m_yshifts[i]) > static_cast<int64_t>(m_Dims[1] - 1) || (xspot + m_xshifts[i]) < 0 || (xspot + m_xshifts[i]) > static_cast<int64_t>(m_Dims[0]) - 1)
          {
            IDataArray::Pointer p = m->getAttributeMatrix(m_Filter->getCellAttributeMatrixName())->getAttributeArray(voxelArrayNames[m_VoxelArrayIndex]);
            EXECUTE_FUNCTION_TEMPLATE(m_Filter, initializeArrayValues, p, p, newPosition)
          }
        }
      }
    }
  }

private:
  AlignSections* m_Filter = nullptr;
  size_t* m_Dims = nullptr;
  std::vector<int64_t> m_xshifts;
  std::vector<int64_t> m_yshifts;
  int32_t m_VoxelArrayIndex = 0;

  void operator=(const AlignSectionsTransferDataImpl&) = delete; // Move assignment Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSections::AlignSections()
: m_DataContainerName(SIMPL::Defaults::ImageDataContainerName)
, m_CellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName)
, m_WriteAlignmentShifts(false)
, m_AlignmentShiftFileName("")
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSections::~AlignSections() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSections::setupFilterParameters()
{
  FilterParameterVector parameters;
  QStringList linkedProps("AlignmentShiftFileName");
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Write Alignment Shift File", WriteAlignmentShifts, FilterParameter::Parameter, AlignSections, linkedProps));
  parameters.push_back(SIMPL_NEW_OUTPUT_FILE_FP("Alignment File", AlignmentShiftFileName, FilterParameter::Parameter, AlignSections, "", "*.txt"));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSections::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setAlignmentShiftFileName(reader->readString("AlignmentShiftFileName", getAlignmentShiftFileName()));
  setWriteAlignmentShifts(reader->readValue("WriteAlignmentShifts", getWriteAlignmentShifts()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSections::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSections::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);
  DataArrayPath tempPath;

  ImageGeom::Pointer image = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getDataContainerName());
  if(getErrorCondition() < 0)
  {
    return;
  }

  if(image->getXPoints() <= 1 || image->getYPoints() <= 1 || image->getZPoints() <= 1)
  {
    QString ss =
        QObject::tr("The Image Geometry is not 3D and cannot be run through this filter. The dimensions are (%1,%2,%3)").arg(image->getXPoints()).arg(image->getYPoints()).arg(image->getZPoints());
    setErrorCondition(-3010);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  tempPath.update(getDataContainerName(), getCellAttributeMatrixName(), "");
  getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, tempPath, -301);

  if(true == m_WriteAlignmentShifts)
  {
    FileSystemPathHelper::CheckOutputFile(this, "Output Alignment Shifts File", getAlignmentShiftFileName(), true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSections::preflight()
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
void AlignSections::find_shifts(std::vector<int64_t>& xshifts, std::vector<int64_t>& yshifts)
{
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSections::updateProgress(size_t p)
{
  m_Progress += p;
  int32_t progressInt = static_cast<int>((static_cast<float>(m_Progress) / static_cast<float>(m_TotalProgress)) * 100.0f);
  QString ss = QObject::tr("Transferring Cell Data %1%").arg(progressInt);
  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSections::execute()
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

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());

  size_t dims[3] = {0, 0, 0};
  std::tie(dims[0], dims[1], dims[2]) = m->getGeometryAs<ImageGeom>()->getDimensions();

  std::vector<int64_t> xshifts(dims[2], 0);
  std::vector<int64_t> yshifts(dims[2], 0);

  find_shifts(xshifts, yshifts);

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  // The idea for this parallel section is to parallelize over each Data Array that
  // will need it's data adjusted. This should go faster than before by about 2x.
  // Better speed up could be achieved if we had better data locality.
  if(doParallel == true)
  {
    std::shared_ptr<tbb::task_group> g(new tbb::task_group);
    QVector<QString> voxelArrayNames = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArrayNames().toVector();
    m_TotalProgress = voxelArrayNames.size() * dims[2]; // Total number of slices to update
    // Create all the tasks
    for(int32_t voxelArray = 0; voxelArray < voxelArrayNames.size(); voxelArray++)
    {
      g->run(AlignSectionsTransferDataImpl(this, dims, xshifts, yshifts, voxelArray));
    }
    // Wait for them to complete.
    g->wait();
    
  }
  else
#endif
  {
#if 0
  // This code is actually a bit slower than the code below.... 
    QVector<QString> voxelArrayNames = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArrayNames().toVector();
    for(int32_t voxelArray = 0; voxelArray < voxelArrayNames.size(); voxelArray++)
    {
      AlignSectionsTransferDataImpl(this, dims, xshifts, yshifts, voxelArray)();
    }
#else
    QList<QString> voxelArrayNames = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArrayNames();
    size_t progIncrement = dims[2] / 100;
    size_t prog = 1;
    size_t progressInt = 0;
    size_t slice = 0;

    for(size_t i = 1; i < dims[2]; i++)
    {
      if(i > prog)
      {
        progressInt = static_cast<int>((static_cast<float>(i) / static_cast<float>(dims[2])) * 100.0f);
        QString ss = QObject::tr("Transferring Cell Data %1%").arg(progressInt);
        notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
        prog = prog + progIncrement;
      }
      if(getCancel())
      {
        return;
      }
      slice = (dims[2] - 1) - i;
      for(size_t l = 0; l < dims[1]; l++)
      {
        for(size_t n = 0; n < dims[0]; n++)
        {
          int64_t xspot = 0, yspot = 0;
          int64_t newPosition = 0;
          int64_t currentPosition = 0;
          if(yshifts[i] >= 0)
          {
            yspot = static_cast<int64_t>(l);
          }
          else if(yshifts[i] < 0)
          {
            yspot = static_cast<int64_t>(dims[1]) - 1 - static_cast<int64_t>(l);
          }
          if(xshifts[i] >= 0)
          {
            xspot = static_cast<int64_t>(n);
          }
          else if(xshifts[i] < 0)
          {
            xspot = static_cast<int64_t>(dims[0]) - 1 - static_cast<int64_t>(n);
          }
          newPosition = (slice * dims[0] * dims[1]) + (yspot * dims[0]) + xspot;
          currentPosition = (slice * dims[0] * dims[1]) + ((yspot + yshifts[i]) * dims[0]) + (xspot + xshifts[i]);
          if((yspot + yshifts[i]) >= 0 && (yspot + yshifts[i]) <= static_cast<int64_t>(dims[1]) - 1 && (xspot + xshifts[i]) >= 0 && (xspot + xshifts[i]) <= static_cast<int64_t>(dims[0]) - 1)
          {
            for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
            {
              IDataArray::Pointer p = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArray(*iter);
              p->copyTuple(static_cast<size_t>(currentPosition), static_cast<size_t>(newPosition));
            }
          }
          if((yspot + yshifts[i]) < 0 || (yspot + yshifts[i]) > static_cast<int64_t>(dims[1] - 1) || (xspot + xshifts[i]) < 0 || (xspot + xshifts[i]) > static_cast<int64_t>(dims[0]) - 1)
          {
            for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
            {
              IDataArray::Pointer p = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArray(*iter);
              EXECUTE_FUNCTION_TEMPLATE(this, initializeArrayValues, p, p, newPosition)
            }
          }
        }
      }
    }
#endif
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer AlignSections::newFilterInstance(bool copyFilterParameters) const
{
  AlignSections::Pointer filter = AlignSections::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSections::getCompiledLibraryName() const
{
  return ReconstructionConstants::ReconstructionBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSections::getBrandingString() const
{
  return "Reconstruction";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSections::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Reconstruction::Version::Major() << "." << Reconstruction::Version::Minor() << "." << Reconstruction::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSections::getGroupName() const
{
  return SIMPL::FilterGroups::ReconstructionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid AlignSections::getUuid()
{
  return QUuid("{fc882470-6aa7-5fd1-8041-ffd14ba8ad9b}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSections::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::AlignmentFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSections::getHumanLabel() const
{
  return "Align Sections";
}
