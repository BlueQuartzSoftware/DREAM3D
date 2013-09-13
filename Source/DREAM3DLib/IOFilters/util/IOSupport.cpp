/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
 #include "IOSupport.h"

#include "H5Support/QH5Utilities.h"
#include "H5Support/QH5Lite.h"

#include "DREAM3DLib/HDF5/VTKH5Constants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IOSupport::IOSupport() :
Observable(),
m_ErrorCondition(0),
m_Cancel(false),
  m_DataContainer(NULL),
  m_HdfFileId(-1)
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IOSupport::addErrorMessage(PipelineMessage &msg)
{
  m_PipelineMessages.push_back(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IOSupport::addErrorMessage(const QString &filterHumanLabel, const QString &errorDescription, int errorCode)
{
  PipelineMessage em(getNameOfClass(), errorDescription, errorCode, PipelineMessage::Error);
  em.setFilterHumanLabel(getHumanLabel());
  m_PipelineMessages.push_back(em);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IOSupport::addErrorMessages(QVector<PipelineMessage> msgVector) {
  for (QVector<PipelineMessage>::size_type i=0; i < msgVector.size(); ++i) {
    m_PipelineMessages.push_back(msgVector[i]);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IOSupport::addWarningMessage(PipelineMessage &msg)
{
  msg.setFilterHumanLabel(getHumanLabel());
  msg.setFilterClassName(getNameOfClass());
  m_PipelineMessages.push_back(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IOSupport::addWarningMessage(const QString &filterName, const QString &warnDescription, int warnCode)
{
  PipelineMessage em(getNameOfClass(), warnDescription, warnCode, PipelineMessage::Warning);
  em.setFilterHumanLabel(getHumanLabel());
  em.setFilterClassName(getNameOfClass());
  m_PipelineMessages.push_back(em);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IOSupport::addWarningMessages(QVector<PipelineMessage> msgVector) {
  for (QVector<PipelineMessage>::size_type i=0; i < msgVector.size(); ++i) {
    m_PipelineMessages.push_back(msgVector[i]);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IOSupport::removeErrorMessage(PipelineMessage msg) {
  for (QVector<PipelineMessage>::iterator iter = m_PipelineMessages.begin(); iter!=m_PipelineMessages.end(); ++iter) {
    if (*iter == msg) {
      m_PipelineMessages.erase(iter);
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IOSupport::removeErrorMessage(int index) {
  int count = 0;

  for (QVector<PipelineMessage>::iterator iter = m_PipelineMessages.begin(); iter!=m_PipelineMessages.end(); ++iter) {
    if (count == index) {
      m_PipelineMessages.erase(iter);
      return;
    }
    count++;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IOSupport::removeErrorMessages(int start, int end) {
  int count = 0;

  for (QVector<PipelineMessage>::iterator iter = m_PipelineMessages.begin(); iter!=m_PipelineMessages.end(); ++iter) {
    if (count == start) {
      while (count <= end) {
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
void IOSupport::clearErrorMessages() {
  QVector<PipelineMessage>::iterator iter = m_PipelineMessages.begin();

  while ( iter != m_PipelineMessages.end() ) {
    iter = m_PipelineMessages.erase(iter);
  }
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int IOSupport::createVtkObjectGroup(const QString &hdfGroupPath, const char* vtkDataObjectType)
{
  // qDebug() << "   vtkH5DataWriter::WritePoints()" << "\n";
  herr_t err = QH5Utilities::createGroupsFromPath(hdfGroupPath, getHdfFileId());
  if (err < 0)
  {
    qDebug() << "Error creating HDF Group " << hdfGroupPath << "\n";
  }
  err = QH5Lite::writeStringAttribute(getHdfFileId(), hdfGroupPath, H5_VTK_DATA_OBJECT, vtkDataObjectType );
  if(err < 0)
  {
    qDebug() << "Error writing string attribute to HDF Group " << hdfGroupPath << "\n";
  }
  return err;
}

