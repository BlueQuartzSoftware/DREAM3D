/* ============================================================================
* Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
* Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
* Copyright (c) 2012 Joseph B. Kleingers (Student Research Assistant)
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
* Neither the name of Michael A. Groeber, Michael A. Jackson, Joseph B. Kleingers,
* the US Air Force, BlueQuartz Software nor the names of its contributors may be
* used to endorse or promote products derived from this software without specific
* prior written permission.
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

#include "DREAM3DApplication.h"

#include "Applications/DREAM3D/DREAM3D_UI.h"

#include <QtGui/QFileOpenEvent>
#include <iostream>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DApplication::DREAM3DApplication(int & argc, char ** argv) :
  QApplication(argc, argv)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DApplication::~DREAM3DApplication()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DREAM3DApplication::event(QEvent* event)
{
  if (event->type() == QEvent::FileOpen)
  {
    QFileOpenEvent* openEvent = static_cast<QFileOpenEvent*>(event);
    QString filePath = openEvent->file();

    // Create new DREAM3D_UI instance, and register it
    DREAM3D_UI* newInstance = new DREAM3D_UI(NULL);
    registerDREAM3DWindow(newInstance);

    // Open the pipeline in a new window
    newInstance->openNewPipeline(filePath, true, true, true);

    return true;
  }

  return QApplication::event(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QWidget*> DREAM3DApplication::getDREAM3DWindowList()
{
  return m_DREAM3DWidgetList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::registerDREAM3DWindow(QWidget* widget)
{
  m_DREAM3DWidgetList.push_back(widget);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::unregisterDREAM3DWindow(QWidget* widget)
{
  m_DREAM3DWidgetList.removeAll(widget);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DApplication::exitTriggered()
{
  bool shouldReallyClose = true;
  for (int i = 0; i < m_DREAM3DWidgetList.size(); i++)
  {
    QWidget* dream3dWindow = m_DREAM3DWidgetList.at(i);
    if (NULL != dream3dWindow)
    {
      if (dream3dWindow->close() == false)
      {
        shouldReallyClose = false;
      }
    }
  }

  if (shouldReallyClose == true)
  {
    dream3dApp->quit();
  }
}






