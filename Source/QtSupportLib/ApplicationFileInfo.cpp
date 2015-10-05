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

#include "ApplicationFileInfo.h"

#include <QtCore/QDir>

#include <QtWidgets/QApplication>

#include "Applications/DREAM3D/DREAM3DApplication.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ApplicationFileInfo::ApplicationFileInfo()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ApplicationFileInfo::~ApplicationFileInfo()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ApplicationFileInfo::GenerateFileSystemPath(QString pathEnding)
{
  QString appPath = dream3dApp->applicationDirPath();

  QDir dir = QDir(appPath);

#if defined(Q_OS_WIN)

#elif defined(Q_OS_MAC)
  if (dir.dirName() == "MacOS")
  {
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
  }
#else
  // We are on Linux - I think
  QFileInfo fi(dir.absolutePath() + pathEnding);
  if (fi.exists() == false)
  {
    // The help file does not exist at the default location because we are probably running from the build tree.
    // Try up one more directory
    dir.cdUp();
  }
#endif

#if defined(Q_OS_WIN) || defined (Q_OS_MAC)
  QFileInfo fi(dir.absolutePath() + pathEnding);
  if (fi.exists() == false)
  {
    // The help file does not exist at the default location because we are probably running from visual studio.
    // Try up one more directory
    dir.cdUp();
  }
#endif

  QString filePath = dir.absolutePath() + pathEnding;
  filePath = QDir::toNativeSeparators(filePath);
  return filePath;
}



