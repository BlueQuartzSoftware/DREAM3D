/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "FileWriter.h"



#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileWriter::FileWriter() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_OutputFile("")
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileWriter::~FileWriter()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FileWriter::writeHeader()
{
  setErrorCondition(-1);
  PipelineMessage em (getHumanLabel(), "FileWriter should be subclassed and functionality implemented there", -1);
  emit filterGeneratedMessage(em);
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FileWriter::writeFile()
{
  setErrorCondition(-1);
  PipelineMessage em (getHumanLabel(), "FileWriter should be subclassed and functionality implemented there", -1);
  emit filterGeneratedMessage(em);
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileWriter::execute()
{
  setErrorCondition(0);

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(m_OutputFile);
  QString parentPath = fi.path();
  QDir dir;
  if(!dir.mkpath(parentPath))
  {
    setErrorCondition(-200);
    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath);
    PipelineMessage em(getHumanLabel(), ss, getErrorCondition(), PipelineMessage::Error);
    emit filterGeneratedMessage(em);
    return;
  }

  int err = writeHeader();
  if (err < 0)
  {
    QString ss = QObject::tr("Error Writing the Header portion of the file");
    setErrorCondition(err);
    PipelineMessage em(getHumanLabel(), ss, getErrorCondition(), PipelineMessage::Error);
    emit filterGeneratedMessage(em);
    return;
  }
  err = writeFile();
  if (err < 0)
  {
    QString ss = QObject::tr("Error Writing the data to the file");
    setErrorCondition(err);
    PipelineMessage em(getHumanLabel(), ss, getErrorCondition(), PipelineMessage::Error);
    emit filterGeneratedMessage(em);
    return;
  }
}



