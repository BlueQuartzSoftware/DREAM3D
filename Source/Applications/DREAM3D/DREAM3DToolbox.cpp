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
#include "DREAM3DToolbox.h"

#include "DREAM3DApplication.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_DREAM3DToolbox.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DToolbox::DREAM3DToolbox(QWidget* parent) :
QWidget(parent)
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DToolbox::~DREAM3DToolbox()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DToolbox::setupGui()
{
  readSettings();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DToolbox::readSettings()
{
  DREAM3DSettings prefs;

  prefs.beginGroup("ToolboxSettings");

  setVisible(prefs.value(objectName(), true).toBool());

  // Have the toolbox write its settings to the prefs file
  readWindowSettings(prefs);

  // Read dock widget settings
  prefs.beginGroup("Bookmarks Dock Widget");
  bookmarksWidget->readSettings(prefs);
  prefs.endGroup();

  prefs.beginGroup("Prebuilts Dock Widget");
  prebuiltsWidget->readSettings(prefs);
  prefs.endGroup();

  prefs.beginGroup("Filter List Dock Widget");
  filterListWidget->readSettings(prefs);
  prefs.endGroup();

  prefs.beginGroup("Filter Library Dock Widget");
  filterLibraryWidget->readSettings(prefs);
  prefs.endGroup();

  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DToolbox::readWindowSettings(DREAM3DSettings& prefs)
{
  bool ok = false;
  prefs.beginGroup("WindowSettings");
  if (prefs.contains(QString("MainWindowGeometry")))
  {
    QByteArray geo_data = prefs.value("MainWindowGeometry", "").toByteArray();
    ok = restoreGeometry(geo_data);
    if (!ok)
    {
      qDebug() << "Error Restoring the Window Geometry" << "\n";
    }
  }

  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DToolbox::writeSettings()
{
  DREAM3DSettings prefs;

  prefs.beginGroup("ToolboxSettings");

  prefs.setValue(objectName(), isVisible());

  // Have the toolbox write its settings to the prefs file
  writeWindowSettings(prefs);

  // Write dock widget settings
  prefs.beginGroup("Bookmarks Widget");
  bookmarksWidget->writeSettings(prefs);
  prefs.endGroup();

  prefs.beginGroup("Prebuilts Widget");
  prebuiltsWidget->writeSettings(prefs);
  prefs.endGroup();

  prefs.beginGroup("Filter List Widget");
  filterListWidget->writeSettings(prefs);
  prefs.endGroup();

  prefs.beginGroup("Filter Library Widget");
  filterLibraryWidget->writeSettings(prefs);
  prefs.endGroup();

  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DToolbox::closeEvent(QCloseEvent* event)
{
  QAction* action = dream3dApp->getShowToolboxAction();
  action->setChecked(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DToolbox::writeWindowSettings(DREAM3DSettings& prefs)
{
  prefs.beginGroup("WindowSettings");

  QByteArray geo_data = saveGeometry();
  prefs.setValue(QString("MainWindowGeometry"), geo_data);

  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksToolboxWidget* DREAM3DToolbox::getBookmarksWidget()
{
  return bookmarksWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PrebuiltsToolboxWidget* DREAM3DToolbox::getPrebuiltsWidget()
{
  return prebuiltsWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterListToolboxWidget* DREAM3DToolbox::getFilterListWidget()
{
  return filterListWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterLibraryToolboxWidget* DREAM3DToolbox::getFilterLibraryWidget()
{
  return filterLibraryWidget;
}


