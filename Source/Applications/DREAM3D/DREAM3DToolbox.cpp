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

#include "StandardDREAM3DApplication.h"
#include "DREAM3DMenuItems.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_DREAM3DToolbox.cpp"

DREAM3DToolbox* DREAM3DToolbox::self = NULL;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DToolbox::DREAM3DToolbox(QWidget* parent, Qt::WindowFlags flags) :
QMainWindow(parent, flags)
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
DREAM3DToolbox* DREAM3DToolbox::Instance(QWidget* parent, Qt::WindowFlags flags)
{
  if (self == NULL)
  {
    self = new DREAM3DToolbox(parent, flags);
  }

  return self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DToolbox::setupGui()
{
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  // Connect the library and list widgets so that the "double-click to add" feature works
  connect(filterLibraryWidget, SIGNAL(filterItemDoubleClicked(const QString&)),
    dream3dApp, SLOT(addFilter(const QString&)));

  connect(filterListWidget, SIGNAL(filterItemDoubleClicked(const QString&)),
    dream3dApp, SLOT(addFilter(const QString&)));

  connect(bookmarksWidget, SIGNAL(pipelineFileActivated(const QString&, const bool&, const bool&)),
    dream3dApp, SLOT(newInstanceFromFile(const QString&, const bool&, const bool&)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DToolbox::readSettings()
{
  QSharedPointer<DREAM3DSettings> prefs = QSharedPointer<DREAM3DSettings>(new DREAM3DSettings());
  DREAM3DMenuItems* menuItems = DREAM3DMenuItems::Instance();

  prefs->beginGroup("ToolboxSettings");

  setVisible(prefs->value(objectName(), true).toBool());

  bool showBookmarks = prefs->value(bookmarksWidget->objectName(), true).toBool();
  menuItems->getActionShowBookmarks()->setChecked(showBookmarks);
  if (showBookmarks == false)
  {
    tabWidget->removeTab(tabWidget->indexOf(bookmarksTab));
  }

  bool showFilterList = prefs->value(filterListWidget->objectName(), true).toBool();
  menuItems->getActionShowFilterList()->setChecked(showFilterList);
  if (showFilterList == false)
  {
    tabWidget->removeTab(tabWidget->indexOf(filterListTab));
  }

  bool showFilterLibrary = prefs->value(filterLibraryWidget->objectName(), true).toBool();
  menuItems->getActionShowFilterLibrary()->setChecked(showFilterLibrary);
  if (showFilterLibrary == false)
  {
    tabWidget->removeTab(tabWidget->indexOf(filterLibraryTab));
  }

  // Have the toolbox write its settings to the prefs file
  readWindowSettings(prefs.data());

  // Read dock widget settings
  prefs->beginGroup("Bookmarks Widget");
  bookmarksWidget->readSettings(prefs.data());
  if (prefs->value("PrebuiltsRead", QVariant(false)).toBool() == false)
  {
    bookmarksWidget->readPrebuiltPipelines();
    prefs->setValue("PrebuiltsRead", true);
  }
  prefs->endGroup();

  prefs->beginGroup("Filter List Widget");
  filterListWidget->readSettings(prefs.data());
  prefs->endGroup();

  prefs->beginGroup("Filter Library Widget");
  filterLibraryWidget->readSettings(prefs.data());
  prefs->endGroup();

  prefs->endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DToolbox::readWindowSettings(DREAM3DSettings* prefs)
{
  bool ok = false;
  prefs->beginGroup("WindowSettings");
  QStringList childGroups = prefs->childGroups();
  if (prefs->contains(QString("MainWindowGeometry")))
  {
    QByteArray geo_data = prefs->value("MainWindowGeometry", QByteArray(""));
    ok = restoreGeometry(geo_data);
    if (!ok)
    {
      qDebug() << "Error Restoring the Window Geometry" << "\n";
    }
  }

  // Remove the tabs from the interface
  tabWidget->removeTab(tabWidget->indexOf(filterListTab));
  tabWidget->removeTab(tabWidget->indexOf(filterLibraryTab));
  tabWidget->removeTab(tabWidget->indexOf(bookmarksTab));

  // Get the new tab indices (if there is no data, put them in [1]List, [2]Library, [3]Bookmarks order)
  int filterListIndex = prefs->value("Filter List Index", QVariant(0)).toInt();
  int filterLibraryIndex = prefs->value("Filter Library Index", QVariant(1)).toInt();
  int bookmarksIndex = prefs->value("Bookmarks Index", QVariant(2)).toInt();
  int currentIndex = prefs->value("Current Index", QVariant(0)).toInt();

  // Re-add the tabs in the order that they were in from last launch (if index is less than 0, it was hidden during last launch)
  if (filterListIndex >= 0)
  {
    tabWidget->insertTab(filterListIndex, filterListTab, "Filter List");
  }
  if (filterLibraryIndex >= 0)
  {
    tabWidget->insertTab(filterLibraryIndex, filterLibraryTab, "Filter Library");
  }
  if (bookmarksIndex >= 0)
  {
    tabWidget->insertTab(bookmarksIndex, bookmarksTab, "Bookmarks");
  }

  tabWidget->setCurrentIndex(currentIndex);

  prefs->endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DToolbox::writeSettings()
{
  QSharedPointer<DREAM3DSettings> prefs = QSharedPointer<DREAM3DSettings>(new DREAM3DSettings());
  DREAM3DMenuItems* menuItems = DREAM3DMenuItems::Instance();

  prefs->beginGroup("ToolboxSettings");

  prefs->setValue(objectName(), menuItems->getActionShowToolbox()->isChecked());
  prefs->setValue(bookmarksWidget->objectName(), menuItems->getActionShowBookmarks()->isChecked());
  prefs->setValue(filterListWidget->objectName(), menuItems->getActionShowFilterList()->isChecked());
  prefs->setValue(filterLibraryWidget->objectName(), menuItems->getActionShowFilterLibrary()->isChecked());

  // Have the toolbox write its settings to the prefs file
  writeWindowSettings(prefs.data());

  // Write dock widget settings
  prefs->beginGroup("Bookmarks Widget");
  bookmarksWidget->writeSettings(prefs.data());
  prefs->endGroup();

  prefs->beginGroup("Filter List Widget");
  filterListWidget->writeSettings(prefs.data());
  prefs->endGroup();

  prefs->beginGroup("Filter Library Widget");
  filterLibraryWidget->writeSettings(prefs.data());
  prefs->endGroup();

  prefs->endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DToolbox::changeEvent(QEvent* event)
{
  if (event->type() == QEvent::ActivationChange)
  {
    emit toolboxChangedState();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DToolbox::closeEvent(QCloseEvent* event)
{
  DREAM3DMenuItems* menuItems = DREAM3DMenuItems::Instance();
  QAction* action = menuItems->getActionShowToolbox();
  action->setChecked(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DToolbox::writeWindowSettings(DREAM3DSettings* prefs)
{
  prefs->beginGroup("WindowSettings");

  QByteArray geo_data = saveGeometry();
  prefs->setValue(QString("MainWindowGeometry"), geo_data);

  prefs->setValue("Current Index", QVariant(tabWidget->currentIndex()));
  prefs->setValue("Filter List Index", QVariant(tabWidget->indexOf(filterListTab)));
  prefs->setValue("Filter Library Index", QVariant(tabWidget->indexOf(filterLibraryTab)));
  prefs->setValue("Bookmarks Index", QVariant(tabWidget->indexOf(bookmarksTab)));

  prefs->endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DToolbox::actionShowFilterLibrary_triggered(bool enabled)
{
  if (enabled == true)
  {
    tabWidget->addTab(filterLibraryTab, "Filter Library");
  }
  else
  {
    tabWidget->removeTab(tabWidget->indexOf(filterLibraryTab));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DToolbox::actionShowFilterList_triggered(bool enabled)
{
  if (enabled == true)
  {
    tabWidget->addTab(filterListTab, "Filter List");
  }
  else
  {
    tabWidget->removeTab(tabWidget->indexOf(filterListTab));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DToolbox::actionShowBookmarks_triggered(bool enabled)
{
  if (enabled == true)
  {
    tabWidget->addTab(bookmarksTab, "Bookmarks");
  }
  else
  {
    tabWidget->removeTab(tabWidget->indexOf(bookmarksTab));
  }
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DToolbox::setCurrentTab(ToolboxTabs tab)
{
  if (tab == DREAM3DToolbox::FilterList)
  {
    tabWidget->setCurrentWidget(filterListTab);
  }
  else if (tab == DREAM3DToolbox::FilterLibrary)
  {
    tabWidget->setCurrentWidget(filterLibraryTab);
  }
  else if (tab == DREAM3DToolbox::Bookmarks)
  {
    tabWidget->setCurrentWidget(bookmarksTab);
  }
}


