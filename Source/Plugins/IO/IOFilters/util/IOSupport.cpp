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

#include "IOSupport.h"

#include "H5Support/QH5Lite.h"
#include "H5Support/QH5Utilities.h"

#include "SIMPLib/HDF5/VTKH5Constants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IOSupport::IOSupport()
: Observable()
, m_ErrorCondition(0)
, m_DataContainer(nullptr)
, m_DataContainerType(DREAM3D::DataContainerType::UnknownDataContainer)
, m_HdfFileId(-1)
, m_HdfGroupId(-1)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IOSupport::~IOSupport()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString IOSupport::getHumanLabel()
{
  return QString("IOSupport");
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IOSupport::addErrorMessage(PipelineMessage& msg)
{
  m_PipelineMessages.push_back(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IOSupport::addErrorMessage(const QString& filterHumanLabel, const QString& errorDescription, int errorCode)
{
  PipelineMessage em(getNameOfClass(), errorDescription, errorCode, PipelineMessage::Error);
  em.setFilterHumanLabel(getHumanLabel());
  m_PipelineMessages.push_back(em);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IOSupport::addErrorMessages(QVector<PipelineMessage> msgVector)
{
  for (QVector<PipelineMessage>::size_type i = 0; i < msgVector.size(); ++i)
  {
    m_PipelineMessages.push_back(msgVector[i]);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IOSupport::addWarningMessage(PipelineMessage& msg)
{
  msg.setFilterHumanLabel(getHumanLabel());
  msg.setFilterClassName(getNameOfClass());
  m_PipelineMessages.push_back(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IOSupport::addWarningMessage(const QString& filterName, const QString& warnDescription, int warnCode)
{
  PipelineMessage em(getNameOfClass(), warnDescription, warnCode, PipelineMessage::Warning);
  em.setFilterHumanLabel(getHumanLabel());
  em.setFilterClassName(getNameOfClass());
  m_PipelineMessages.push_back(em);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IOSupport::addWarningMessages(QVector<PipelineMessage> msgVector)
{
  for (QVector<PipelineMessage>::size_type i = 0; i < msgVector.size(); ++i)
  {
    m_PipelineMessages.push_back(msgVector[i]);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IOSupport::removeErrorMessage(PipelineMessage msg)
{
  for (QVector<PipelineMessage>::iterator iter = m_PipelineMessages.begin(); iter != m_PipelineMessages.end(); ++iter)
  {
    if (*iter == msg)
    {
      m_PipelineMessages.erase(iter);
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IOSupport::removeErrorMessage(int index)
{
  int count = 0;

  for (QVector<PipelineMessage>::iterator iter = m_PipelineMessages.begin(); iter != m_PipelineMessages.end(); ++iter)
  {
    if (count == index)
    {
      m_PipelineMessages.erase(iter);
      return;
    }
    count++;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IOSupport::removeErrorMessages(int start, int end)
{
  int count = 0;

  for (QVector<PipelineMessage>::iterator iter = m_PipelineMessages.begin(); iter != m_PipelineMessages.end(); ++iter)
  {
    if (count == start)
    {
      while (count <= end)
      {
        iter = m_PipelineMessages.erase(iter);
        count++;
      }
      return;
    }
    count++;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IOSupport::clearErrorMessages()
{
  QVector<PipelineMessage>::iterator iter = m_PipelineMessages.begin();

  while ( iter != m_PipelineMessages.end() )
  {
    iter = m_PipelineMessages.erase(iter);
  }
}
#endif
