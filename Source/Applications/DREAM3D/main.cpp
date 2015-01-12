/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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


#include <QtGui/QApplication>
#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QDebug>


#include "BrandedInitializer.h"

#ifdef Q_WS_X11
#include <QPlastiqueStyle>
#endif

#include <clocale>

int main(int argc, char* argv[])
{
#ifdef Q_WS_X11
  // Using motif style gives us test failures (and its ugly).
  // Using cleanlooks style gives us errors when using valgrind (Trolltech's bug #179200)
  // let's just use plastique for now
  QApplication::setStyle(new QPlastiqueStyle);
#endif

#ifdef Q_WS_WIN
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

  QCoreApplication::setApplicationName("DREAM3D_V5");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setOrganizationName("BlueQuartz Software");

  QApplication qtapp(argc, argv);
#if defined( Q_WS_MAC )
  //Needed for typical Mac program behavior.
  qtapp.setQuitOnLastWindowClosed( true );
#endif //APPLE

  setlocale(LC_NUMERIC, "C");
  BrandedInitializer d3dInitializer;
  if (!d3dInitializer.initialize(argc, argv))
  {
    return 1;
  }

  return qtapp.exec();
}

