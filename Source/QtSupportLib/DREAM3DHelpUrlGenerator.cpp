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

#include "DREAM3DHelpUrlGenerator.h"

#include <QtCore/QDir>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QMessageBox>

#include "Applications/DREAM3D/DREAM3DApplication.h"

namespace Detail
{
  QString Dream3DHelpPath = "/Help/DREAM3D/";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DHelpUrlGenerator::DREAM3DHelpUrlGenerator()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DHelpUrlGenerator::~DREAM3DHelpUrlGenerator()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUrl DREAM3DHelpUrlGenerator::generateHTMLUrl(QString htmlName)
{
  QString appPath = dream3dApp->applicationDirPath();

  QDir helpDir = QDir(appPath);
  QString s("file://");

#if defined(Q_OS_WIN)
  s = s + "/"; // Need the third slash on windows because file paths start with a drive letter
#elif defined(Q_OS_MAC)
  if (helpDir.dirName() == "MacOS")
  {
    helpDir.cdUp();
    helpDir.cdUp();
    helpDir.cdUp();
  }
#else
  // We are on Linux - I think
  QFileInfo fi( helpDir.absolutePath() + Detail::Dream3DHelpPath + htmlName + ".html");
  if (fi.exists() == false)
  {
    // The help file does not exist at the default location because we are probably running from the build tree.
    // Try up one more directory
    helpDir.cdUp();
  }
#endif



#if defined(Q_OS_WIN) || defined (Q_OS_MAC)
  QFileInfo fi( helpDir.absolutePath() + Detail::Dream3DHelpPath + htmlName + ".html");
  if (fi.exists() == false)
  {
    // The help file does not exist at the default location because we are probably running from Visual Studio or Xcode
    // Try up one more directory
    helpDir.cdUp();
  }
#endif

  s = s + helpDir.absolutePath() + Detail::Dream3DHelpPath + htmlName + ".html";
  return QUrl(s);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DHelpUrlGenerator::openHTMLUrl(QUrl URL, QWidget* parent)
{
  bool didOpen = QDesktopServices::openUrl(URL);
  if(false == didOpen)
  {
    QMessageBox::critical(parent, "Error Opening Help File",
                          QString::fromLatin1("DREAM3D could not open the help file path ") + URL.path(),
                          QMessageBox::Ok, QMessageBox::Ok);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DHelpUrlGenerator::generateAndOpenHTMLUrl(QString helpName, QWidget* parent)
{
  QUrl URL = generateHTMLUrl(helpName);
  openHTMLUrl(URL, parent);
}

