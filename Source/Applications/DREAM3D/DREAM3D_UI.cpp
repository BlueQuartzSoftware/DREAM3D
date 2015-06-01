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

#include "DREAM3D_UI.h"

//-- Qt Includes
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QThread>
#include <QtCore/QFileInfoList>
#include <QtCore/QDateTime>
#include <QtCore/QProcess>
#include <QtCore/QMimeData>
#include <QtCore/QDirIterator>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QListWidget>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QScrollBar>

//-- DREAM3D Includes
#include "DREAM3DLib/DREAM3DVersion.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/FilterParameters/QFilterParametersWriter.h"

#include "QtSupportLib/ApplicationAboutBoxDialog.h"
#include "QtSupportLib/QRecentFileList.h"
#include "QtSupportLib/DREAM3DQtMacros.h"
#include "QtSupportLib/DREAM3DPluginFrame.h"
#include "QtSupportLib/HelpDialog.h"
#include "QtSupportLib/DREAM3DHelpUrlGenerator.h"

#include "DREAM3DWidgetsLib/FilterWidgetManager.h"
#include "DREAM3DWidgetsLib/UpdateCheck.h"
#include "DREAM3DWidgetsLib/UpdateCheckData.h"
#include "DREAM3DWidgetsLib/Widgets/DREAM3DUpdateCheckDialog.h"
#include "DREAM3DWidgetsLib/Widgets/PipelineViewWidget.h"
#include "DREAM3DWidgetsLib/Widgets/FilterLibraryDockWidget.h"
#include "DREAM3DWidgetsLib/Widgets/PrebuiltPipelinesDockWidget.h"
#include "DREAM3DWidgetsLib/Widgets/DREAM3DUserManualDialog.h"


#include "Applications/DREAM3D/DREAM3DConstants.h"
#include "Applications/DREAM3D/AboutDREAM3D.h"
#include "Applications/DREAM3D/AboutPlugins.h"
#include "Applications/DREAM3D/DREAM3Dv5Wizard.h"


// Initialize private static member variable
QString DREAM3D_UI::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3D_UI::DREAM3D_UI(QWidget* parent) :
  QMainWindow(parent),
  m_WorkerThread(NULL),
  //  m_PluginActionGroup(NULL),
  //  m_PluginPrefsActionGroup(NULL),
  m_ActivePlugin(NULL),
  //  m_PluginToolBar(NULL),
  m_HelpDialog(NULL),
  m_UpdateCheckThread(NULL),
  m_FilterListBtn(NULL),
  m_FilterLibraryBtn(NULL),
  m_FavoritesBtn(NULL),
  m_PrebuiltBtn(NULL),
  m_IssuesBtn(NULL),
  m_ShouldRestart(false),
  m_OpenedFilePath(""),
  m_ActionAddPipeline(NULL),
  m_ActionUpdatePipeline(NULL),
  m_ActionRenamePipeline(NULL),
  m_ActionLocateFile(NULL),
  m_ActionAddToPipelineView(NULL),
  m_ActionNewFolder(NULL),
  m_ActionRemovePipeline(NULL),
  m_ActionShowInFileSystem(NULL),
  m_ActionClearPipeline(NULL)
{
  m_OpenDialogLastDirectory = QDir::homePath();

  // Update first run
  updateFirstRun();

  // Register all of the Filters we know about - the rest will be loaded through plugins
  //  which all should have been loaded by now.
  m_FilterManager = FilterManager::Instance();
  //m_FilterManager->RegisterKnownFilters(m_FilterManager);

  // Register all the known filterWidgets
  m_FilterWidgetManager = FilterWidgetManager::Instance();
  m_FilterWidgetManager->RegisterKnownFilterWidgets();

  // Calls the Parent Class to do all the Widget Initialization that were created
  // using the QDesigner program
  setupUi(this);

  // Do our own widget initializations
  setupGui();

  this->setAcceptDrops(true);

  // Read various settings
  readSettings();

  // Set window modified to false
  setWindowModified(false);

  // If all DREAM3D windows are closed, disable menus
  connect(qApp, SIGNAL(lastWindowClosed()), this, SLOT(disableMenuItems()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3D_UI::~DREAM3D_UI()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::updateFirstRun()
{
  DREAM3DSettings prefs;
  QString filePath = prefs.fileName();
  QFileInfo fi(filePath);

  if (fi.exists() == false)
  {
    prefs.setValue("First Run", true);
  }
  else
  {
    prefs.setValue("First Run", false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::checkFirstRun()
{
  // Launch v5.2 dialog box if this is the first run of v5.2
  DREAM3DSettings prefs;
  bool firstRun = prefs.value("First Run", true).toBool();
  if (firstRun == true)
  {
    // This is the first run of DREAM3D v5.2, so we need to show the splash screen
    DREAM3Dv5Wizard* wizard = new DREAM3Dv5Wizard(this, Qt::WindowTitleHint);
    wizard->exec();

    bool value = wizard->isBookmarkBtnChecked();
    if (value == true)
    {
      BookmarksModel* model = BookmarksModel::Instance();

      model->insertRow(0, QModelIndex());
      QModelIndex nameIndex = model->index(0, Name, QModelIndex());
      model->setData(nameIndex, "DREAM3D v4 Favorites", Qt::DisplayRole);
      model->setData(nameIndex, QIcon(":/folder_blue.png"), Qt::DecorationRole);

      QDir favoritesDir = bookmarksDockWidget->findV4FavoritesDirectory();
      QString favoritesPath = favoritesDir.path();
      QFileInfo fi(favoritesPath);

      if (fi.exists() && favoritesPath.isEmpty() == false)
      {
        QDirIterator iter(favoritesPath, QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
        while (iter.hasNext())
        {
          QString path = iter.next();
          model->addFileToTree(path, nameIndex);
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::resizeEvent ( QResizeEvent* event )
{
  QMainWindow::resizeEvent(event);

  emit parentResized();

  // We need to write the window settings so that any new windows will open with these window settings
  DREAM3DSettings prefs;
  writeWindowSettings(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_actionNew_triggered()
{
  PluginManager* pluginManager = PluginManager::Instance();
  QVector<IDREAM3DPlugin*> plugins = pluginManager->getPluginsVector();

  DREAM3D_UI* newInstance = new DREAM3D_UI(NULL);
  newInstance->setLoadedPlugins(plugins);
  newInstance->setWindowTitle("[*]Untitled Pipeline - DREAM3D");
  newInstance->setAttribute(Qt::WA_DeleteOnClose);
  newInstance->move(this->x() + 45, this->y() + 45);

  connectSignalsSlots(newInstance);

  newInstance->show();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_actionOpen_triggered()
{
  QString proposedDir = m_OpenDialogLastDirectory;
  QString filePath = QFileDialog::getOpenFileName(this, tr("Open Pipeline"),
    proposedDir, tr("Json File (*.json);;Dream3d File (*.dream3d);;Text File (*.txt);;Ini File (*.ini);;All Files (*.*)"));
  if (true == filePath.isEmpty()) { return; }

  filePath = QDir::toNativeSeparators(filePath);
  QFileInfo fi(filePath);

  // Open the new pipeline
  openNewPipeline(filePath, true);

  // Cache the last directory on old instance
  m_OpenDialogLastDirectory = fi.path();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::openNewPipeline(const QString &filePath, const bool &setOpenedFilePath)
{
  QFileInfo fi(filePath);
  QRecentFileList* list = QRecentFileList::instance();

  // If there are filters in the pipeline view, create a new window
  if (pipelineViewWidget->filterCount() > 0)
  {
    PluginManager* pluginManager = PluginManager::Instance();
    QVector<IDREAM3DPlugin*> plugins = pluginManager->getPluginsVector();

    // Create new DREAM3D instance
    DREAM3D_UI* newInstance = new DREAM3D_UI(NULL);
    newInstance->setLoadedPlugins(plugins);

    // Read Pipeline
    int err = newInstance->getPipelineViewWidget()->openPipeline(filePath, 0, setOpenedFilePath);

    if (setOpenedFilePath == false)
    {
      newInstance->setWindowTitle("[*]Untitled Pipeline - DREAM3D");
    }

    // Set Current File Path
    if (err >= 0)
    {
      // Cache the last directory on new instance
      newInstance->setOpenDialogLastDirectory(fi.path());

      // Remove filepath from wherever it is in the list
      list->removeFile(filePath);

      // Add file path to the recent files list for both instances
      if (list->fileList().size() >= 7)
      {
        list->popBack();
      }
      list->addFile(filePath);

      // Show the new instance
      newInstance->setAttribute(Qt::WA_DeleteOnClose);
      newInstance->move(this->x() + 45, this->y() + 45);

      connectSignalsSlots(newInstance);

      newInstance->show();
    }
    else
    {
      // Show error message on old DREAM3D instance
      QString errorMessage = newInstance->getPipelineViewWidget()->getStatusBar()->currentMessage();
      pipelineViewWidget->getStatusBar()->showMessage(errorMessage);

      // Delete new DREAM3D instance
      delete newInstance;
    }
  }
  // The pipeline view is empty, so open the pipeline in the pipeline view
  else
  {
    int err = pipelineViewWidget->openPipeline(filePath, 0, setOpenedFilePath);
    setOpenDialogLastDirectory(fi.path());

    if (err >= 0)
    {
      // Add file path to the recent files list for both instances
      if (list->fileList().size() == 7)
      {
        list->popBack();
      }
      list->addFile(filePath);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_actionSave_triggered()
{
  if (isWindowModified() == true)
  {
    QString filePath;
    if (m_OpenedFilePath.isEmpty())
    {
      // When the file hasn't been saved before, the same functionality as a "Save As" occurs...
      on_actionSaveAs_triggered();
      return;
    }
    else
    {
      filePath = m_OpenedFilePath;
    }

    // Fix the separators
    filePath = QDir::toNativeSeparators(filePath);

    // Write the pipeline
    pipelineViewWidget->writePipeline(filePath);

    // Set window title and save flag
    QFileInfo prefFileInfo = QFileInfo(filePath);
    setWindowTitle("[*]" + prefFileInfo.baseName() + " - DREAM3D");
    setWindowModified(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_actionSaveAs_triggered()
{
  QString proposedFile = m_OpenDialogLastDirectory + QDir::separator() + "Untitled.json";
  QString filePath = QFileDialog::getSaveFileName(this, tr("Save Pipeline To File"),
    proposedFile,
    tr("Json File (*.json);;DREAM3D File (*.dream3d);;All Files (*.*)"));
  if (true == filePath.isEmpty()) { return; }

  filePath = QDir::toNativeSeparators(filePath);

  //If the filePath already exists - delete it so that we get a clean write to the file
  QFileInfo fi(filePath);
  if (fi.suffix().isEmpty())
  {
    filePath.append(".json");
    fi.setFile(filePath);
  }

  // Write the pipeline
  int err = pipelineViewWidget->writePipeline(filePath);

  if (err >= 0)
  {
    // Set window title and save flag
    QFileInfo prefFileInfo = QFileInfo(filePath);
    setWindowTitle("[*]" + prefFileInfo.baseName() + " - DREAM3D");
    setWindowModified(false);

    m_OpenedFilePath = filePath;
  }

  // Cache the last directory
  m_OpenDialogLastDirectory = fi.path();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_actionExit_triggered()
{
#if defined (Q_OS_WIN)
  this->close();
#else
  qApp->closeAllWindows();
  qApp->quit();
#endif
}

// -----------------------------------------------------------------------------
//  Called when the main window is closed.
// -----------------------------------------------------------------------------
void DREAM3D_UI::closeEvent(QCloseEvent* event)
{
  QMessageBox::StandardButton choice = checkDirtyDocument();
  if (choice == QMessageBox::Cancel)
  {
    event->ignore();
    return;
  }

  disconnectSignalsSlots();

  writeSettings();
  clearPipeline();
  event->accept();

  if (m_ShouldRestart == true)
  {
    // Restart DREAM3D
    QProcess::startDetached(QApplication::applicationFilePath());
    qApp->quit();
  }
}

// -----------------------------------------------------------------------------
//  Read our settings from a file
// -----------------------------------------------------------------------------
void DREAM3D_UI::readSettings()
{
  DREAM3DSettings prefs;

  // Have the pipeline builder read its settings from the prefs file
  readWindowSettings(prefs);
  readVersionSettings(prefs);

  prefs.beginGroup("DockWidgetSettings");

  // Read dock widget settings
  bookmarksDockWidget->readSettings(this, prefs);
  prebuiltPipelinesDockWidget->readSettings(this, prefs);
  filterListDockWidget->readSettings(this, prefs);
  filterLibraryDockWidget->readSettings(this, prefs);
  issuesDockWidget->readSettings(this, prefs);

  prefs.endGroup();

  QRecentFileList::instance()->readList(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::readWindowSettings(DREAM3DSettings& prefs)
{
  QString filePath = prefs.fileName();
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

  if (prefs.contains(QString("MainWindowState")))
  {
    std::cout << "Reading State of Main Window" << std::endl;
    QByteArray layout_data = prefs.value("MainWindowState", "").toByteArray();
    restoreState(layout_data);
  }

  QByteArray splitterGeometry = prefs.value("Splitter_Geometry", "").toByteArray();
  splitter->restoreGeometry(splitterGeometry);
  QByteArray splitterSizes = prefs.value("Splitter_Sizes", "").toByteArray();
  splitter->restoreState(splitterSizes);

  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::readDockWidgetSettings(DREAM3DSettings& prefs, QDockWidget* dw)
{
  restoreDockWidget(dw);

  QString name = dw->objectName();
  bool b = prefs.value(dw->objectName(), false).toBool();
  dw->setHidden(b);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::readVersionSettings(DREAM3DSettings& prefs)
{

}

// -----------------------------------------------------------------------------
//  Write our Prefs to file
// -----------------------------------------------------------------------------
void DREAM3D_UI::writeSettings()
{
  DREAM3DSettings prefs;

  // Have the pipeline builder write its settings to the prefs file
  writeWindowSettings(prefs);
  // Have the version check widet write its preferences.
  writeVersionCheckSettings(prefs);

  prefs.beginGroup("DockWidgetSettings");

  // Write dock widget settings
  bookmarksDockWidget->writeSettings(prefs);
  prebuiltPipelinesDockWidget->writeSettings(prefs);
  filterListDockWidget->writeSettings(prefs);
  filterLibraryDockWidget->writeSettings(prefs);
  issuesDockWidget->writeSettings(prefs);

  prefs.endGroup();

  QRecentFileList::instance()->writeList(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::writeVersionCheckSettings(DREAM3DSettings& prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::writeWindowSettings(DREAM3DSettings& prefs)
{
  prefs.beginGroup("WindowSettings");
  QByteArray geo_data = saveGeometry();
  QByteArray layout_data = saveState();
  prefs.setValue(QString("MainWindowGeometry"), geo_data);
  prefs.setValue(QString("MainWindowState"), layout_data);

  QByteArray splitterGeometry = splitter->saveGeometry();
  QByteArray splitterSizes = splitter->saveState();
  prefs.setValue(QString("Splitter_Geometry"), splitterGeometry);
  prefs.setValue(QString("Splitter_Sizes"), splitterSizes);

  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::writeDockWidgetSettings(DREAM3DSettings& prefs, QDockWidget* dw)
{
  prefs.setValue(dw->objectName(), dw->isHidden() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::checkForUpdatesAtStartup()
{
  DREAM3DUpdateCheckDialog* d = new DREAM3DUpdateCheckDialog(this);
  if ( d->getAutomaticallyBtn()->isChecked() )
  {
    DREAM3DSettings updatePrefs;

    updatePrefs.beginGroup( DREAM3DUpdateCheckDialog::getUpdatePreferencesGroup() );
    QDate lastUpdateCheckDate = updatePrefs.value(DREAM3DUpdateCheckDialog::getUpdateCheckKey(), "").toDate();
    updatePrefs.endGroup();

    QDate systemDate;
    QDate currentDateToday = systemDate.currentDate();

    QDate dailyThreshold = lastUpdateCheckDate.addDays(1);
    QDate weeklyThreshold = lastUpdateCheckDate.addDays(7);
    QDate monthlyThreshold = lastUpdateCheckDate.addMonths(1);

    if ( (d->getHowOftenComboBox()->currentIndex() == DREAM3DUpdateCheckDialog::UpdateCheckDaily && currentDateToday >= dailyThreshold)
         || (d->getHowOftenComboBox()->currentIndex() == DREAM3DUpdateCheckDialog::UpdateCheckWeekly && currentDateToday >= weeklyThreshold)
         || (d->getHowOftenComboBox()->currentIndex() == DREAM3DUpdateCheckDialog::UpdateCheckMonthly && currentDateToday >= monthlyThreshold) )
    {
      m_UpdateCheck = QSharedPointer<UpdateCheck>(new UpdateCheck(this));

      connect(m_UpdateCheck.data(), SIGNAL( LatestVersion(UpdateCheckData*) ),
              this, SLOT( versionCheckReply(UpdateCheckData*) ) );

      m_UpdateCheck->checkVersion(DREAM3D::UpdateWebsite::UpdateWebSite);
    }
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::setupGui()
{
  // Automatically check for updates at startup if the user has indicated that preference before
  checkForUpdatesAtStartup();

  m_HelpDialog = new HelpDialog(this);
  m_HelpDialog->setWindowModality(Qt::NonModal);

  pipelineViewWidget->setScrollArea(pipelineViewScrollArea);

  // Stretch Factors
  splitter->setStretchFactor(0, 0);
  splitter->setStretchFactor(1, 1);

  pipelineViewScrollArea->verticalScrollBar()->setSingleStep(5);

  // Make the connections between the gui elements
  //QRecentFileList* recentsList = QRecentFileList::instance();

  // Hook up the signals from the various docks to the PipelineViewWidget that will either add a filter
  // or load an entire pipeline into the view
  connectSignalsSlots();

  pipelineViewWidget->setStatusBar(statusbar);

  // This will set the initial list of filters in the filterListDockWidget
  // Tell the Filter Library that we have more Filters (potentially)
  filterLibraryDockWidget->refreshFilterGroups();

  // Set the IssuesDockWidget as a PipelineMessageObserver Object.
  pipelineViewWidget->setPipelineMessageObserver(issuesDockWidget);

  setupViewMenu();
  setupPipelineContextMenu();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::disconnectSignalsSlots()
{
  QRecentFileList* recentsList = QRecentFileList::instance();
  disconnect(filterLibraryDockWidget, SIGNAL(filterItemDoubleClicked(const QString&)),
    pipelineViewWidget, SLOT(addFilter(const QString&)));

  disconnect(filterListDockWidget, SIGNAL(filterItemDoubleClicked(const QString&)),
    pipelineViewWidget, SLOT(addFilter(const QString&)));

  disconnect(prebuiltPipelinesDockWidget, SIGNAL(pipelineFileActivated(const QString&, const bool &)),
    this, SLOT(openNewPipeline(const QString&, const bool &)));

  disconnect(bookmarksDockWidget, SIGNAL(pipelineFileActivated(const QString&, const bool &)),
    this, SLOT(openNewPipeline(const QString&, const bool &)));

  disconnect(bookmarksDockWidget, SIGNAL(pipelineNeedsToBeSaved(const QString&, const QString&)),
    pipelineViewWidget, SLOT(updateFavorite(const QString&, const QString&)));

  disconnect(recentsList, SIGNAL(fileListChanged(const QString &)),
    this, SLOT(updateRecentFileList(const QString &)));

  disconnect(pipelineViewWidget, SIGNAL(filterInputWidgetChanged(FilterInputWidget*)),
    this, SLOT(setFilterInputWidget(FilterInputWidget*)));

  disconnect(pipelineViewWidget, SIGNAL(noFilterWidgetsInPipeline()),
    this, SLOT(clearFilterInputWidget()));

  disconnect(pipelineViewWidget, SIGNAL(filterParameterChanged()),
    this, SLOT(markDocumentAsDirty()));

  disconnect(bookmarksDockWidget, SIGNAL(updateStatusBar(const QString&)),
    this, SLOT(setStatusBarMessage(const QString&)));
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::connectSignalsSlots()
{
  QRecentFileList* recentsList = QRecentFileList::instance();

  connect(filterLibraryDockWidget, SIGNAL(filterItemDoubleClicked(const QString&)),
          pipelineViewWidget, SLOT(addFilter(const QString&)) );

  connect(filterListDockWidget, SIGNAL(filterItemDoubleClicked(const QString&)),
          pipelineViewWidget, SLOT(addFilter(const QString&)) );

  connect(prebuiltPipelinesDockWidget, SIGNAL(pipelineFileActivated(const QString&, const bool &)),
    this, SLOT(openNewPipeline(const QString&, const bool &)));

  connect(bookmarksDockWidget, SIGNAL(pipelineFileActivated(const QString&, const bool &)),
    this, SLOT(openNewPipeline(const QString&, const bool &)));

  connect(bookmarksDockWidget, SIGNAL(pipelineNeedsToBeSaved(const QString&, const QString&)),
    pipelineViewWidget, SLOT(updateFavorite(const QString&, const QString&)));

  connect(recentsList, SIGNAL(fileListChanged(const QString &)),
    this, SLOT(updateRecentFileList(const QString &)));

  connect(pipelineViewWidget, SIGNAL(filterInputWidgetChanged(FilterInputWidget*)),
    this, SLOT(setFilterInputWidget(FilterInputWidget*)));

  connect(pipelineViewWidget, SIGNAL(noFilterWidgetsInPipeline()),
    this, SLOT(clearFilterInputWidget()));

  connect(pipelineViewWidget, SIGNAL(filterParameterChanged()),
    this, SLOT(markDocumentAsDirty()));

  connect(bookmarksDockWidget, SIGNAL(updateStatusBar(const QString&)),
    this, SLOT(setStatusBarMessage(const QString&)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::connectSignalsSlots(DREAM3D_UI* other)
{
  connect(bookmarksDockWidget->getBookmarksTreeView(), SIGNAL(collapsed(const QModelIndex&)),
    other->getBookmarksDockWidget()->getBookmarksTreeView(), SLOT(collapse(const QModelIndex&)));

  connect(other->getBookmarksDockWidget()->getBookmarksTreeView(), SIGNAL(collapsed(const QModelIndex&)),
    bookmarksDockWidget->getBookmarksTreeView(), SLOT(collapse(const QModelIndex&)));

  connect(bookmarksDockWidget->getBookmarksTreeView(), SIGNAL(expanded(const QModelIndex&)),
    other->getBookmarksDockWidget()->getBookmarksTreeView(), SLOT(expand(const QModelIndex&)));

  connect(other->getBookmarksDockWidget()->getBookmarksTreeView(), SIGNAL(expanded(const QModelIndex&)),
    bookmarksDockWidget->getBookmarksTreeView(), SLOT(expand(const QModelIndex&)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::setupPipelineItemMenu()
{
  // Normal items
  QList<QAction*> favoriteItemActions;

  favoriteItemActions << m_ActionAddPipeline;
  favoriteItemActions << m_ActionRenamePipeline;
  favoriteItemActions << m_ActionUpdatePipeline;

  {
    QAction* separator = new QAction(this);
    separator->setSeparator(true);
    favoriteItemActions << separator;
  }

  favoriteItemActions << m_ActionRemovePipeline;

  {
    QAction* separator = new QAction(this);
    separator->setSeparator(true);
    favoriteItemActions << separator;
  }

  favoriteItemActions << m_ActionNewFolder;

  {
    QAction* separator = new QAction(this);
    separator->setSeparator(true);
    favoriteItemActions << separator;
  }

  favoriteItemActions << m_ActionShowInFileSystem;

  // Items with errors
  QList<QAction*> errorItemActions;

  errorItemActions << m_ActionLocateFile;
  {
    QAction* separator = new QAction(this);
    separator->setSeparator(true);
    errorItemActions << separator;
  }
  errorItemActions << m_ActionRemovePipeline;

  bookmarksDockWidget->getBookmarksTreeView()->setLeafActionList(favoriteItemActions);
  bookmarksDockWidget->getBookmarksTreeView()->setLeafErrorActionList(errorItemActions);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::setupFolderMenu()
{
  QList<QAction*> favoriteCategoryActions;

  favoriteCategoryActions << m_ActionAddPipeline;
  favoriteCategoryActions << m_ActionRenamePipeline;

  {
    QAction* separator = new QAction(this);
    separator->setSeparator(true);
    favoriteCategoryActions << separator;
  }

  favoriteCategoryActions << m_ActionRemovePipeline;

  {
    QAction* separator = new QAction(this);
    separator->setSeparator(true);
    favoriteCategoryActions << separator;
  }

  favoriteCategoryActions << m_ActionNewFolder;

  bookmarksDockWidget->getBookmarksTreeView()->setNodeActionList(favoriteCategoryActions);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::setupDefaultMenu()
{
  QList<QAction*> favoriteDefaultActions;

  favoriteDefaultActions << m_ActionAddPipeline;

  {
    QAction* separator = new QAction(this);
    separator->setSeparator(true);
    favoriteDefaultActions << separator;
  }

  favoriteDefaultActions << m_ActionNewFolder;


  bookmarksDockWidget->getBookmarksTreeView()->setDefaultActionList(favoriteDefaultActions);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::setupPrebuiltsMenu()
{
  // Prebuilt Pipeline Items
  QList<QAction*> prebuiltItemActions;

  prebuiltItemActions << m_ActionShowInFileSystem;

  prebuiltPipelinesDockWidget->getFilterLibraryTreeWidget()->setLeafActionList(prebuiltItemActions);

  // Prebuilt Folders
  QList<QAction*> prebuildCategoryActions;

  prebuildCategoryActions << m_ActionShowInFileSystem;

  prebuiltPipelinesDockWidget->getFilterLibraryTreeWidget()->setNodeActionList(prebuildCategoryActions);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::setupPipelineViewMenu()
{
  QList<QAction*> pipelineViewActions;

  pipelineViewActions << m_ActionAddPipeline;

  {
    QAction* separator = new QAction(this);
    separator->setSeparator(true);
    pipelineViewActions << separator;
  }

  pipelineViewActions << m_ActionNewFolder;

  {
    QAction* separator = new QAction(this);
    separator->setSeparator(true);
    pipelineViewActions << separator;
  }

  pipelineViewActions << m_ActionClearPipeline;

  pipelineViewWidget->setContextMenuActions(pipelineViewActions);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::setupPipelineContextMenu()
{
  // Initialize the menus
  initializeMenuActions();

  // Setup contextual menu for pipelines
  setupPipelineItemMenu();

  // Setup contextual menu for folders
  setupFolderMenu();

  // Setup the default menu
  setupDefaultMenu();

  // Setup the prebuilts menu
  setupPrebuiltsMenu();

  // Setup Pipeline View Widget menu
  setupPipelineViewMenu();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::initializeMenuActions()
{
  /* m_ActionAddPipeline */
  m_ActionAddPipeline = new QAction(menuBookmarks);
  m_ActionAddPipeline->setObjectName(QString::fromUtf8("m_ActionAddPipeline"));
  m_ActionAddPipeline->setText(QApplication::translate("DREAM3D_UI", "Add Bookmark", 0));
  menuBookmarks->addAction(m_ActionAddPipeline);
  QKeySequence m_ActionAddPipelineKeySeq(Qt::CTRL + Qt::Key_Plus);
  m_ActionAddPipeline->setShortcut(m_ActionAddPipelineKeySeq);
  connect(m_ActionAddPipeline, SIGNAL(triggered()),
    bookmarksDockWidget, SLOT(m_ActionAddPipeline_triggered()));

  /* m_ActionRenamePipeline */
  m_ActionRenamePipeline = new QAction(this);
  m_ActionRenamePipeline->setObjectName(QString::fromUtf8("m_ActionRenamePipeline"));
  m_ActionRenamePipeline->setText(QApplication::translate("DREAM3D_UI", "Rename Pipeline", 0));
  bookmarksDockWidget->setRenameAction(m_ActionRenamePipeline);
  connect(m_ActionRenamePipeline, SIGNAL(triggered()),
    bookmarksDockWidget, SLOT(m_ActionRenamePipeline_triggered()));

  /* m_ActionUpdatePipeline */
  m_ActionUpdatePipeline = new QAction(this);
  m_ActionUpdatePipeline->setObjectName(QString::fromUtf8("m_ActionUpdatePipeline"));
  m_ActionUpdatePipeline->setText(QApplication::translate("DREAM3D_UI", "Update Bookmark", 0));
  connect(m_ActionUpdatePipeline, SIGNAL(triggered()),
    bookmarksDockWidget, SLOT(m_ActionUpdatePipeline_triggered()));

  {
    menuBookmarks->addSeparator();
  }

  /* m_ActionRemovePipeline */
  m_ActionRemovePipeline = new QAction(this);
  m_ActionRemovePipeline->setObjectName(QString::fromUtf8("m_ActionRemovePipeline"));
  m_ActionRemovePipeline->setText(QApplication::translate("DREAM3D_UI", "Remove Bookmark", 0));
  bookmarksDockWidget->setDeleteAction(m_ActionRemovePipeline);
  connect(m_ActionRemovePipeline, SIGNAL(triggered()),
    bookmarksDockWidget, SLOT(m_ActionRemovePipeline_triggered()));

  {
    menuBookmarks->addSeparator();
  }

  /* m_ActionNewFolder */
  m_ActionNewFolder = new QAction(menuBookmarks);
  m_ActionNewFolder->setObjectName(QString::fromUtf8("m_ActionNewFolder"));
  m_ActionNewFolder->setText(QApplication::translate("DREAM3D_UI", "New Folder", 0));
  menuBookmarks->addAction(m_ActionNewFolder);
  QKeySequence m_ActionNewFolderKeySeq(Qt::CTRL + Qt::SHIFT + Qt::Key_Plus);
  m_ActionNewFolder->setShortcut(m_ActionNewFolderKeySeq);
  connect(m_ActionNewFolder, SIGNAL(triggered()),
    bookmarksDockWidget, SLOT(m_ActionNewFolder_triggered()));

  /* m_ActionClearPipeline */
  m_ActionClearPipeline = new QAction(menuPipeline);
  m_ActionClearPipeline->setObjectName(QString::fromUtf8("m_ActionClearPipeline"));
  m_ActionClearPipeline->setText(QApplication::translate("DREAM3D_UI", "Clear Pipeline", 0));
  menuPipeline->addAction(m_ActionClearPipeline);
  QKeySequence actionClearKeySeq(Qt::CTRL + Qt::Key_Escape);
  m_ActionClearPipeline->setShortcut(actionClearKeySeq);
  connect(m_ActionClearPipeline, SIGNAL(triggered()),
    this, SLOT(clearPipeline()));




  /* m_ActionLocateFile */
  m_ActionLocateFile = new QAction(this);
  m_ActionLocateFile->setObjectName(QString::fromUtf8("m_ActionLocateFile"));
  m_ActionLocateFile->setText(QApplication::translate("DREAM3D_UI", "Locate Bookmark...", 0));
  connect(m_ActionLocateFile, SIGNAL(triggered()),
    bookmarksDockWidget, SLOT(m_ActionLocateFile_triggered()));

  /* m_ActionShowInFileSystem */
  m_ActionShowInFileSystem = new QAction(this);
  m_ActionShowInFileSystem->setObjectName(QString::fromUtf8("m_ActionShowInFileSystem"));
  // Handle the naming based on what OS we are currently running...
#if defined(Q_OS_WIN)
  m_ActionShowInFileSystem->setText(QApplication::translate("DREAM3D_UI", "Show in Windows Explorer", 0));
#elif defined(Q_OS_MAC)
  m_ActionShowInFileSystem->setText(QApplication::translate("DREAM3D_UI", "Show in Finder", 0));
#else
  m_ActionShowInFileSystem->setText(QApplication::translate("DREAM3D_UI", "Show in File System", 0));
#endif
  connect(m_ActionShowInFileSystem, SIGNAL(triggered()),
    bookmarksDockWidget, SLOT(m_ActionShowInFileSystem_triggered()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::setupViewMenu()
{
  //  m_FilterListBtn = new QToolButton(this);
  //  makeStatusBarButton("Filters", filterListDockWidget, m_FilterListBtn, 0);
  menuView->removeAction(actionShow_Filter_List);
  delete actionShow_Filter_List;
  actionShow_Filter_List = filterListDockWidget->toggleViewAction();
  actionShow_Filter_List->setText("Filter List");

  menuView->addAction(actionShow_Filter_List);
  connect(actionShow_Filter_List, SIGNAL(triggered(bool)),
          this, SLOT(on_actionShow_Filter_List_triggered(bool)) );
  //  connect(m_FilterListBtn, SIGNAL(toggled(bool)),
  //          this, SLOT(on_actionShow_Filter_List_triggered(bool)) );


  //  m_FilterLibraryBtn = new QToolButton(this);
  //  makeStatusBarButton("Filter Library", filterLibraryDockWidget, m_FilterLibraryBtn, 1);
  menuView->removeAction(actionShow_Filter_Library);
  delete actionShow_Filter_Library;
  actionShow_Filter_Library = filterLibraryDockWidget->toggleViewAction();
  actionShow_Filter_Library->setText("Filter Library");
  menuView->addAction(actionShow_Filter_Library);
  connect(actionShow_Filter_Library, SIGNAL(triggered(bool)),
          this, SLOT(on_actionShow_Filter_Library_triggered(bool)) );
  //  connect(m_FilterLibraryBtn, SIGNAL(toggled(bool)),
  //          this, SLOT(on_actionShow_Filter_Library_triggered(bool)) );

  //  m_FavoritesBtn = new QToolButton(this);
  //  makeStatusBarButton("Favorites", BookmarksDockWidget, m_FavoritesBtn, 2);
  menuView->removeAction(actionShow_Favorites);
  delete actionShow_Favorites;
  actionShow_Favorites = bookmarksDockWidget->toggleViewAction();
  actionShow_Favorites->setText("Favorite Pipelines");
  menuView->addAction(actionShow_Favorites);
  connect(actionShow_Favorites, SIGNAL(triggered(bool)),
          this, SLOT(on_actionShow_Favorites_triggered(bool)) );
  //  connect(m_FavoritesBtn, SIGNAL(toggled(bool)),
  //          this, SLOT(on_actionShow_Favorites_triggered(bool)) );

  //  m_PrebuiltBtn = new QToolButton(this);
  //  makeStatusBarButton("Prebuilt", prebuiltPipelinesDockWidget, m_PrebuiltBtn, 3);
  menuView->removeAction(actionShow_Prebuilt_Pipelines);
  delete actionShow_Prebuilt_Pipelines;
  actionShow_Prebuilt_Pipelines = prebuiltPipelinesDockWidget->toggleViewAction();
  actionShow_Prebuilt_Pipelines->setText("Prebuilt Pipelines");
  menuView->addAction(actionShow_Prebuilt_Pipelines);
  connect(actionShow_Prebuilt_Pipelines, SIGNAL(triggered(bool)),
          this, SLOT(on_actionShow_Prebuilt_Pipelines_triggered(bool)) );
  //  connect(m_PrebuiltBtn, SIGNAL(toggled(bool)),
  //          this, SLOT(on_actionShow_Prebuilt_Pipelines_triggered(bool)) );


  //  m_IssuesBtn = new QToolButton(this);
  //  makeStatusBarButton("Issues", issuesDockWidget, m_IssuesBtn, 4);
  menuView->removeAction(actionShow_Issues);
  delete actionShow_Issues;
  actionShow_Issues = issuesDockWidget->toggleViewAction();
  actionShow_Issues->setText("Show Warnings/Errors");
  menuView->addAction(actionShow_Issues);
  connect(actionShow_Issues, SIGNAL(triggered(bool)),
          this, SLOT(on_actionShow_Issues_triggered(bool)) );
  //  connect(m_IssuesBtn, SIGNAL(toggled(bool)),
  //          this, SLOT(on_actionShow_Issues_triggered(bool)) );

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::makeStatusBarButton(QString text, QDockWidget* dockWidget, QToolButton* btn, int index)
{
  btn->setText(text);
  btn->setCheckable(true);
  btn->setChecked(!dockWidget->isHidden());
  statusBar()->insertPermanentWidget(index, btn, 0);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::setLoadedPlugins(QVector<IDREAM3DPlugin*> plugins)
{
  m_LoadedPlugins = plugins;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_pipelineViewWidget_pipelineFileDropped(QString& file, const bool &setOpenedFilePath)
{
  if (setOpenedFilePath == true)
  {
    QFileInfo fi(file);

    m_OpenedFilePath = file;
    on_pipelineViewWidget_pipelineTitleUpdated(fi.baseName());
    setWindowFilePath(file);
    setWindowModified(false);
  }
  else
  {
    setWindowModified(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_pipelineViewWidget_pipelineChanged()
{
  setWindowModified(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_pipelineViewWidget_pipelineTitleUpdated(QString title)
{
  setWindowTitle(QString("[*]") + title + " - DREAM3D");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_pipelineViewWidget_pipelineIssuesCleared()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_pipelineViewWidget_pipelineHasNoErrors()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_actionPlugin_Information_triggered()
{
  AboutPlugins dialog(this);
  dialog.exec();

  // Write cache on exit
  dialog.writePluginCache();

  /* If any of the load checkboxes were changed, display a dialog warning
   * the user that they must restart DREAM3D to see the changes.
   */
  if (dialog.getLoadPreferencesDidChange() == true)
  {
    QMessageBox msgBox;
    msgBox.setText("DREAM3D must be restarted to allow these changes to take effect.");
    msgBox.setInformativeText("Restart?");
    msgBox.setWindowTitle("Restart Needed");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int choice = msgBox.exec();

    if (choice == QMessageBox::Yes)
    {
      // Set Restart Flag and Begin closing DREAM3D
      m_ShouldRestart = true;
      this->close();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_actionCheck_For_Updates_triggered()
{
  DREAM3DUpdateCheckDialog* d = new DREAM3DUpdateCheckDialog(this);

  d->setCurrentVersion((DREAM3DLib::Version::Complete()));
  d->setUpdateWebSite(DREAM3D::UpdateWebsite::UpdateWebSite);
  d->setApplicationName("DREAM3D");

  // Read from the DREAM3DSettings Pref file the information that we need
  DREAM3DSettings prefs;
  prefs.beginGroup(DREAM3D::UpdateWebsite::VersionCheckGroupName);
  QDateTime dateTime = prefs.value(DREAM3D::UpdateWebsite::LastVersionCheck, QDateTime::currentDateTime()).toDateTime();
  d->setLastCheckDateTime(dateTime);
  prefs.endGroup();

  // Now display the dialog box
  d->exec();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::dragEnterEvent(QDragEnterEvent* e)
{
  const QMimeData* dat = e->mimeData();
  QList<QUrl> urls = dat->urls();
  QString file = urls.count() ? urls[0].toLocalFile() : QString();
  QDir parent(file);
  this->m_OpenDialogLastDirectory = parent.dirName();
  QFileInfo fi(file );
  QString ext = fi.suffix();
  if (fi.exists() && fi.isFile() && ( ext.compare("mxa") || ext.compare("h5") || ext.compare("hdf5") ) )
  {
    e->accept();
  }
  else
  {
    e->ignore();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::dropEvent(QDropEvent* e)
{
  const QMimeData* dat = e->mimeData();
  QList<QUrl> urls = dat->urls();
  QString file = urls.count() ? urls[0].toLocalFile() : QString();
  QDir parent(file);
  this->m_OpenDialogLastDirectory = parent.dirName();
  QFileInfo fi(file );
  QString ext = fi.suffix();
  file = QDir::toNativeSeparators(file);
  if (fi.exists() && fi.isFile() )
  {
    //TODO: INSERT Drop Event CODE HERE
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMessageBox::StandardButton DREAM3D_UI::checkDirtyDocument()
{

  if (this->isWindowModified() == true)
  {
    int r = QMessageBox::warning(this, tr("DREAM.3D"),
                                 tr("The Pipeline has been modified.\nDo you want to save your changes?"),
                                 QMessageBox::Save | QMessageBox::Default,
                                 QMessageBox::Discard,
                                 QMessageBox::Cancel | QMessageBox::Escape);
    if (r == QMessageBox::Save)
    {
      on_actionSave_triggered();
      return QMessageBox::Save;
    }
    else if (r == QMessageBox::Discard)
    {
      return QMessageBox::Discard;
    }
    else if (r == QMessageBox::Cancel)
    {
      return QMessageBox::Cancel;
    }
  }

  return QMessageBox::Ignore;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::updateRecentFileList(const QString &file)
{
  // Clear the Recent Items Menu
  this->menu_RecentFiles->clear();

  // Get the list from the static object
  QStringList files = QRecentFileList::instance()->fileList();
  foreach (QString file, files)
  {
    QAction* action = new QAction(this->menu_RecentFiles);
    action->setText(QRecentFileList::instance()->parentAndFileName(file));
    action->setData(file);
    action->setVisible(true);
    this->menu_RecentFiles->addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(openRecentFile()));
  }

  this->menu_RecentFiles->addSeparator();
  this->menu_RecentFiles->addAction(actionClearRecentFiles);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::openRecentFile()
{
  QAction* action = qobject_cast<QAction*>(sender());
  if (action)
  {
    //qDebug() << "Opening Recent file: " << action->data().toString() << "\n";
    QString filePath = action->data().toString();
    int err = getPipelineViewWidget()->openPipeline(filePath, 0, true);

    if (err >= 0)
    {
      QFileInfo fileInfo(filePath);
      setWindowTitle("[*]" + fileInfo.baseName() + " - DREAM3D");
      setOpenedFilePath(filePath);
      setWindowModified(false);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_startPipelineBtn_clicked()
{

  if (startPipelineBtn->text().compare("Cancel") == 0)
  {
    qDebug() << "canceling from GUI...." << "\n";
    emit pipelineCanceled();
    //    m_WorkerThread->wait(); // Wait until the thread is complete
    //    if (m_WorkerThread->isFinished() == true)
    //    {
    //      delete m_WorkerThread;
    //      m_WorkerThread = NULL;
    //    }
    return;
  }

  if (m_WorkerThread != NULL)
  {
    m_WorkerThread->wait(); // Wait until the thread is complete
    if (m_WorkerThread->isFinished() == true)
    {
      delete m_WorkerThread;
      m_WorkerThread = NULL;
    }
  }
  m_WorkerThread = new QThread(); // Create a new Thread Resource


  // Clear out the Issues Table
  issuesDockWidget->clearIssues();

  // Ask the PipelineViewWidget to create a FilterPipeline Object
  m_PipelineInFlight = pipelineViewWidget->getCopyOfFilterPipeline();

  // Give the pipeline one last chance to preflight and get all the latest values from the GUI
  int err = m_PipelineInFlight->preflightPipeline();
  if(err < 0)
  {
    m_PipelineInFlight = FilterPipeline::NullPointer();
    return;
  }

  // Save the preferences file NOW in case something happens
  writeSettings();


  pipelineViewWidget->setEnabled(false);

  // Move the FilterPipeline object into the thread that we just created.
  m_PipelineInFlight->moveToThread(m_WorkerThread);

  // Allow the GUI to receive messages - We are only interested in the progress messages
  m_PipelineInFlight->addMessageReceiver(this);

  /* Connect the signal 'started()' from the QThread to the 'run' slot of the
   * PipelineBuilder object. Since the PipelineBuilder object has been moved to another
   * thread of execution and the actual QThread lives in *this* thread then the
   * type of connection will be a Queued connection.
   */
  // When the thread starts its event loop, start the PipelineBuilder going
  connect(m_WorkerThread, SIGNAL(started()),
          m_PipelineInFlight.get(), SLOT(run()));

  // When the PipelineBuilder ends then tell the QThread to stop its event loop
  connect(m_PipelineInFlight.get(), SIGNAL(pipelineFinished() ),
          m_WorkerThread, SLOT(quit()) );

  // When the QThread finishes, tell this object that it has finished.
  connect(m_WorkerThread, SIGNAL(finished()),
          this, SLOT( pipelineDidFinish() ) );

  // If the use clicks on the "Cancel" button send a message to the PipelineBuilder object
  // We need a Direct Connection so the
  connect(this, SIGNAL(pipelineCanceled() ),
          m_PipelineInFlight.get(), SLOT (cancelPipeline() ), Qt::DirectConnection);



  emit pipelineStarted();
  m_WorkerThread->start();
  startPipelineBtn->setText("Cancel");

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::populateMenus(QObject* plugin)
{

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::processPipelineMessage(const PipelineMessage& msg)
{
  switch(msg.getType())
  {
    case PipelineMessage::ProgressValue:
      this->m_ProgressBar->setValue(msg.getProgressValue());
      break;
    case PipelineMessage::StatusMessage:
      if(NULL != this->statusBar())
      {
        QString s = (msg.getPrefix());
        s = s.append(" ").append(msg.getText().toLatin1().data());
        this->statusBar()->showMessage(s);
      }
      break;
    case PipelineMessage::StatusMessageAndProgressValue:
      this->m_ProgressBar->setValue(msg.getProgressValue());
      if(NULL != this->statusBar())
      {
        QString s = (msg.getPrefix());
        s = s.append(" ").append(msg.getText().toLatin1().data());
        this->statusBar()->showMessage(s);
      }
      break;

    default:
      return;
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::pipelineDidFinish()
{
  std::cout << "DREAM3D_UI::pipelineDidFinish()" << std::endl;
  m_PipelineInFlight = FilterPipeline::NullPointer();// This _should_ remove all the filters and deallocate them
  startPipelineBtn->setText("Go");
  m_ProgressBar->setValue(0);
  pipelineViewWidget->setEnabled(true);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::displayHelp(QString file)
{
  m_HelpDialog->setContentFile(file);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_actionAbout_DREAM3D_triggered()
{
  AboutDREAM3D d(this);
  d.exec();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_actionShowIndex_triggered()
{
  // Generate help page
  QUrl helpURL = DREAM3DHelpUrlGenerator::generateHTMLUrl("index");
  DREAM3DUserManualDialog::LaunchHelpDialog(helpURL);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::versionCheckReply(UpdateCheckData* dataObj)
{
  DREAM3DUpdateCheckDialog* d = new DREAM3DUpdateCheckDialog(this);
  d->setCurrentVersion((DREAM3DLib::Version::Complete()));
  d->setApplicationName("DREAM3D");

  if ( dataObj->hasUpdate() && !dataObj->hasError() )
  {
    QString message = dataObj->getMessageDescription();
    QLabel* feedbackTextLabel = d->getFeedbackTextLabel();
    d->toSimpleUpdateCheckDialog();
    feedbackTextLabel->setText(message);
    d->getCurrentVersionLabel()->setText( dataObj->getAppString() );
    d->setCurrentVersion( dataObj->getAppString() );
    d->getLatestVersionLabel()->setText( dataObj->getServerString() );
    d->exec();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::clearPipeline()
{
  // Clear the filter input widget
  clearFilterInputWidget();

  pipelineViewWidget->clearWidgets();
  setWindowModified(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_actionClearRecentFiles_triggered()
{
  // Clear the Recent Items Menu
  this->menu_RecentFiles->clear();

  // Clear the actual list
  QRecentFileList* recents = QRecentFileList::instance();
  recents->clear();

  // Write out the empty list
  DREAM3DSettings prefs;
  recents->writeList(prefs);

  this->menu_RecentFiles->addSeparator();
  this->menu_RecentFiles->addAction(actionClearRecentFiles);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_actionShow_Filter_Library_triggered(bool b)
{
  updateAndSyncDockWidget(actionShow_Filter_Library, filterLibraryDockWidget, m_FilterLibraryBtn, b);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_actionShow_Filter_List_triggered(bool b)
{
  updateAndSyncDockWidget(actionShow_Filter_List, filterListDockWidget, m_FilterListBtn, b);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_actionShow_Prebuilt_Pipelines_triggered(bool b)
{
  updateAndSyncDockWidget(actionShow_Prebuilt_Pipelines, prebuiltPipelinesDockWidget, m_PrebuiltBtn, b);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_actionShow_Favorites_triggered(bool b)
{
  updateAndSyncDockWidget(actionShow_Favorites, bookmarksDockWidget, m_FavoritesBtn, b);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_actionShow_Issues_triggered(bool b)
{
  updateAndSyncDockWidget(actionShow_Issues, issuesDockWidget, m_IssuesBtn, b);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::updateAndSyncDockWidget(QAction* action, QDockWidget* dock, QToolButton* btn, bool b)
{
  if(m_FilterListBtn == NULL || m_FilterLibraryBtn == NULL || m_FavoritesBtn == NULL || m_PrebuiltBtn == NULL || m_IssuesBtn == NULL) { return; }

  action->blockSignals(true);
  dock->blockSignals(true);
  btn->blockSignals(true);

  action->setChecked(b);
  btn->toggle();
  dock->setVisible(b);
#if 0
  if(b == false)
  {
    QString text = action->text().replace("Show", "Hide");
    action->setText(text);
  }
  else
  {
    QString text = action->text().replace("Hide", "Show");
    action->setText(text);
  }
#endif
  action->blockSignals(false);
  dock->blockSignals(false);
  btn->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineViewWidget* DREAM3D_UI::getPipelineViewWidget()
{
  return pipelineViewWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksDockWidget* DREAM3D_UI::getBookmarksDockWidget()
{
  return bookmarksDockWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::setOpenedFilePath(const QString &filePath)
{
  m_OpenedFilePath = filePath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::setOpenDialogLastDirectory(const QString &path)
{
  m_OpenDialogLastDirectory = path;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::setFilterInputWidget(FilterInputWidget* widget)
{
  // Clear the filter input widget
  clearFilterInputWidget();

  // Set the widget into the frame
  fiwFrameVLayout->addWidget(widget);
  widget->show();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::clearFilterInputWidget()
{
  QLayoutItem* item = fiwFrameVLayout->takeAt(0);
  if (item)
  {
    QWidget* w = item->widget();
    if (w)
    {
      w->hide();
      w->setParent(NULL);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::disableMenuItems()
{
  menuBookmarks->setDisabled(true);
  menuView->setDisabled(true);
  actionSave->setDisabled(true);
  actionSaveAs->setDisabled(true);
  actionPlugin_Information->setDisabled(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::markDocumentAsDirty()
{
  setWindowModified(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::setStatusBarMessage(const QString &msg)
{
  statusbar->showMessage(msg);
}



