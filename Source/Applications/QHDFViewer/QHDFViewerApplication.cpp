/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "QHDFViewerApplication.h"
#include <string>
#include <iostream>
#include "QHDFViewer.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QHDFViewerApplication::QHDFViewerApplication(int &argc, char **argv)
    : QApplication(argc, argv)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QHDFViewerApplication::~QHDFViewerApplication()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QHDFViewerApplication::event(QEvent *ev)
{
    bool eaten;
    bool fileIsOpen = false;
    QString f;
    switch (ev->type())
    {
    case QEvent::FileOpen:
      // Set it true first since, if it's a Qt 3 form, the messagebox from convert will fire the timer.
      f = static_cast<QFileOpenEvent *>(ev)->file();
   //   std::cout << "Open File: " << f.toStdString() << std::endl;

      foreach (QWidget *widget, qApp->topLevelWidgets())
      {
        QHDFViewer *mainWin = qobject_cast<QHDFViewer *>(widget);
        if (NULL != mainWin && mainWin->getCurrentFile().compare(f) == 0)
        {
           fileIsOpen = true;
        }
        if (NULL != mainWin && mainWin->getCurrentFile().isEmpty() == true)
        {
          mainWin->openHDF5File(f);
          mainWin->show();
          fileIsOpen = true;
        }
      }

      if (fileIsOpen == true)
      {
        eaten = true;
      }
      else
      {
        QHDFViewer* mainWindow = new QHDFViewer;
        mainWindow->openHDF5File(f);
        mainWindow->show();
        eaten = true;
      }
        break;
    case QEvent::Close: {

    }
    default:
        eaten = QApplication::event(ev);
        break;
    }
    return eaten;
}
