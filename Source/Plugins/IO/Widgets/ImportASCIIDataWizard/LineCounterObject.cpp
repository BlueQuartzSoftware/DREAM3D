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

#include "LineCounterObject.h"

#include <QtCore/QTextStream>
#include <QtCore/QFile>

#include "SIMPLib/SIMPLibTypes.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LineCounterObject::LineCounterObject(const QString& filePath, QObject* parent)
: QObject(parent)
, m_FilePath(filePath)
, m_NumOfLines(0)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LineCounterObject::LineCounterObject(const QString& filePath, int numLines, QObject* parent)
: QObject(parent)
, m_FilePath(filePath)
, m_NumOfLines(numLines)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LineCounterObject::~LineCounterObject()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LineCounterObject::run()
{
  // Validate that the file is an ASCII file
  int64_t bufferSize = 262144;
  char* buffer;
  int64_t result;

  // Obtain the file size
  if(m_FilePath.isEmpty())
  {
    m_NumOfLines = -1;
    emit finished();
    return;
  }
  QFile qFile(m_FilePath);
  int64_t fileSize = qFile.size();

  // Open the file
  if(qFile.open(QIODevice::ReadOnly) == false)
  {
    QString errorStr = "Error: Unable to open file \"" + m_FilePath + "\"";
    fputs(errorStr.toStdString().c_str(), stderr);
    return;
  }

  int64_t actualSize;
  if(fileSize <= bufferSize)
  {
    actualSize = fileSize;
  }
  else
  {
    actualSize = bufferSize;
  }

  // Allocate the buffer
  buffer = (char*)malloc(sizeof(char) * actualSize);
  if(buffer == nullptr)
  {
    QString errorStr = "Error: Unable to allocate memory to read in data from \"" + m_FilePath + "\"";
    fputs(errorStr.toStdString().c_str(), stderr);
    return;
  }

  int64_t currentByte = 0;
  while(qFile.atEnd() == false)
  {
    // Copy the file contents into the buffer
    result = qFile.read(buffer, actualSize);

    // Check the buffer for new lines and carriage returns
    int64_t fiveThresh = fileSize / 20.0;
    int64_t currentThresh = fiveThresh;
    for(int i = 0; i < result; i++)
    {
      currentByte++;
      if(currentByte > currentThresh)
      {
        double progress = static_cast<double>(currentByte) / static_cast<double>(fileSize) * 100;
        emit progressUpdateGenerated(progress);
        currentThresh = currentThresh + fiveThresh;
      }

      char currentChar = buffer[i];

      if(currentChar == '\n')
      {
        m_NumOfLines++;
      }
      else if(currentChar == '\r' && i + 1 < actualSize && buffer[i + 1] == '\n')
      {
        m_NumOfLines++;
        i++;
      }
    }
  }

  // Count the last line
  m_NumOfLines++;

  // Close the file and free the memory from the buffer
  qFile.close();
  free(buffer);

  emit finished();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LineCounterObject::getNumberOfLines()
{
  return m_NumOfLines;
}
