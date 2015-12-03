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



#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QDebug>

#include "MacDREAM3DApplication.h"
#include "StandardDREAM3DApplication.h"
#include "DREAM3D_UI.h"
#include "BrandedStrings.h"

#ifdef Q_WS_X11
#include <QPlastiqueStyle>
#endif

#include <clocale>

int main(int argc, char* argv[])
{
#ifdef Q_OS_X11
  // Using motif style gives us test failures (and its ugly).
  // Using cleanlooks style gives us errors when using valgrind (Trolltech's bug #179200)
  // let's just use plastique for now
  QApplication::setStyle(new QPlastiqueStyle);
#endif

#ifdef Q_OS_WIN
  // Some where Visual Studio wants to set the Current Working Directory (cwd)
  // to the subfolder BUILD/Applications/DREAM3D instead of our true binary
  // directory where everything is built. This wreaks havoc on the prebuilt
  // pipelines not being able to find the data folder. This _should_ fix things
  // for debugging and hopefully NOT effect any type of release
  QFileInfo fi(argv[0]);
  QString absPathExe = fi.absolutePath();
  QString cwd = QDir::currentPath();

  if (absPathExe != cwd)
  {
    QDir::setCurrent(absPathExe);
    qDebug() << "setting cwd: " << absPathExe;
  }
  cwd = QDir::currentPath();
  qDebug() << "        cwd: " << cwd;
#endif

#if defined (Q_OS_MAC)
  MacDREAM3DApplication qtapp(argc, argv);
#else
  StandardDREAM3DApplication qtapp(argc, argv);
#endif

  QCoreApplication::setOrganizationDomain(BrandedStrings::OrganizationDomain);
  QCoreApplication::setOrganizationName(BrandedStrings::OrganizationName);
  QCoreApplication::setApplicationName("DREAM3D");

#if defined (Q_OS_MAC)
  dream3dApp->setQuitOnLastWindowClosed(false);
#endif

  QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

  setlocale(LC_NUMERIC, "C");

  if (!qtapp.initialize(argc, argv))
  {
    return 1;
  }

  // Open pipeline if DREAM3D was opened from a compatible file
  if (argc == 2)
  {
    char* two = argv[1];
    QString filePath = QString::fromLatin1(two);
    if (!filePath.isEmpty())
    {
      qtapp.newInstanceFromFile(filePath, true, true);
    }
  }
  else
  {
    DREAM3D_UI* ui = qtapp.getNewDREAM3DInstance();
    ui->show();
  }

  int err = qtapp.exec();
  return err;
}

