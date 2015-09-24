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


#include "HelpDialog.h"

#include <iostream>

#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QTextStream>
#include <QtCore/QUrl>

#include "Applications/DREAM3D/DREAM3DApplication.h"
#include "moc_HelpDialog.cpp"
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HelpDialog::HelpDialog(QWidget* parent) :
  QDialog(parent)
{
  this->setupUi(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HelpDialog::~HelpDialog()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HelpDialog::setContentFile(QUrl sourceLocation)
{
  QDir aPluginDir = QDir(dream3dApp->applicationDirPath());
  QString aPluginDirStr = aPluginDir.absolutePath();
  QString thePath;
#if defined(Q_OS_WIN)
  if (aPluginDir.cd("Help") )
  {
    thePath = aPluginDir.absolutePath();
  }
  else if (aPluginDir.cd("../Help") )
  {
    thePath = aPluginDir.absolutePath();
  }
#elif defined(Q_OS_MAC)
  if (aPluginDir.dirName() == "MacOS")
  {
    aPluginDir.cdUp();
    aPluginDir.cdUp();
    aPluginDir.cdUp();
  }
  // aPluginDir.cd("Plugins");
  thePath = aPluginDir.absolutePath() + "/Help";
#else
  if (aPluginDir.cd("Help"))
  {
    thePath = aPluginDir.absolutePath();
  }
#endif
  thePath = QString("file:///").append(thePath).append("/").append(sourceLocation.toString());

//  qDebug() << "Help File Path:" << thePath() << "\n";

  helpBrowser->setSource(QUrl(thePath));
  // Set the Home Page File
  m_HomeUrl = QUrl(thePath);
  this->show();
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HelpDialog::on_backBtn_clicked()
{
  if (helpBrowser->isBackwardAvailable() )
  {
    helpBrowser->backward();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HelpDialog::on_forwardBtn_clicked()
{
  if (helpBrowser->isForwardAvailable() )
  {
    helpBrowser->forward();
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HelpDialog::on_reloadBtn_clicked()
{
  helpBrowser->reload();
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HelpDialog::on_homeBtn_clicked()
{
  helpBrowser->setSource(m_HomeUrl);
}
