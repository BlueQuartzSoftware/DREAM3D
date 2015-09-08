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
#include <QtWidgets/QFileDialog>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QListWidget>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QScrollBar>

#ifdef DREAM3D_USE_QtWebEngine
#include "Applications/Common/DREAM3DUserManualDialog.h"
#else
#include <QtWidgets/QMessageBox>
#include <QtGui/QDesktopServices>
#include "QtSupportLib/DREAM3DHelpUrlGenerator.h"
#endif


//-- DREAM3D Includes
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/DocRequestManager.h"
#include "SIMPLib/FilterParameters/QFilterParametersWriter.h"
#include "SIMPLib/Plugin/PluginManager.h"

#include "QtSupportLib/QRecentFileList.h"
#include "QtSupportLib/SIMPLQtMacros.h"
#include "QtSupportLib/SIMPLPluginFrame.h"

#include "DREAM3DWidgetsLib/FilterWidgetManager.h"
#include "DREAM3DWidgetsLib/UpdateCheck.h"
#include "DREAM3DWidgetsLib/UpdateCheckData.h"
#include "DREAM3DWidgetsLib/Widgets/DREAM3DUpdateCheckDialog.h"
#include "DREAM3DWidgetsLib/Widgets/PipelineViewWidget.h"
#include "DREAM3DWidgetsLib/Widgets/FilterLibraryDockWidget.h"
#include "DREAM3DWidgetsLib/Widgets/PrebuiltPipelinesDockWidget.h"


#include "Applications/DREAM3D/DREAM3DConstants.h"
#include "Applications/DREAM3D/DREAM3Dv6Wizard.h"
#include "Applications/DREAM3D/DREAM3DApplication.h"
#include "Applications/DREAM3D/DREAM3DMenu.h"


// Initialize private static member variable
QString DREAM3D_UI::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3D_UI::DREAM3D_UI(QWidget* parent) :
  QMainWindow(parent),
  m_WorkerThread(NULL),
  m_ActivePlugin(NULL),
  m_UpdateCheckThread(NULL),
  m_FilterManager(NULL),
  m_FilterWidgetManager(NULL),
#if !defined(Q_OS_MAC)
  m_InstanceMenu(NULL),
#endif
  m_ShouldRestart(false),
  m_OpenedFilePath("")
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

  // Set up the menu
  QMenu* viewMenu = createViewMenu();
#if !defined(Q_OS_MAC)
  // Create the menu
  m_InstanceMenu = new DREAM3DMenu();
  m_InstanceMenu->setViewMenu(viewMenu);
  setMenuBar(m_InstanceMenu->getMenuBar());
#endif
  dream3dApp->registerDREAM3DWindow(this, viewMenu);

  // Do our own widget initializations
  setupGui();

  this->setAcceptDrops(true);

  // Read various settings
  readSettings();

  // Set window modified to false
  setWindowModified(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3D_UI::~DREAM3D_UI()
{
  dream3dApp->unregisterDREAM3DWindow(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::updateFirstRun()
{
  DREAM3DSettings prefs;
  QString filePath = prefs.fileName();
  QFileInfo fi(filePath);

  if (prefs.contains("First Run") == false)
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
  // Launch v6.0 dialog box if this is the first run of v6.0
  DREAM3DSettings prefs;
  bool firstRun = prefs.value("First Run", true).toBool();
  if (firstRun == true)
  {
    // This is the first run of DREAM3D v6.0, so we need to show the v6.0 wizard
    DREAM3Dv6Wizard* wizard = new DREAM3Dv6Wizard(this, Qt::WindowTitleHint);
    wizard->exec();

    bool value = wizard->isBookmarkBtnChecked();
    if (value == true)
    {
      BookmarksModel* model = BookmarksModel::Instance();

      model->insertRow(0, QModelIndex());
      QModelIndex nameIndex = model->index(0, BookmarksItem::Name, QModelIndex());
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
bool DREAM3D_UI::savePipeline()
{
  if (isWindowModified() == true)
  {
    QString filePath;
    if (m_OpenedFilePath.isEmpty())
    {
      // When the file hasn't been saved before, the same functionality as a "Save As" occurs...
      bool didSave = savePipelineAs();
      return didSave;
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

    // Add file to the recent files list
    QRecentFileList* list = QRecentFileList::instance();
    list->addFile(filePath);
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DREAM3D_UI::savePipelineAs()
{
  QString proposedFile = m_OpenDialogLastDirectory + QDir::separator() + "Untitled.json";
  QString filePath = QFileDialog::getSaveFileName(this, tr("Save Pipeline To File"),
                                                  proposedFile,
                                                  tr("Json File (*.json);;DREAM.3D File (*.dream3d);;All Files (*.*)"));
  if (true == filePath.isEmpty()) { return false; }

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
    setWindowTitle("[*]" + fi.baseName() + " - DREAM3D");
    setWindowModified(false);

    // Add file to the recent files list
    QRecentFileList* list = QRecentFileList::instance();
    list->addFile(filePath);

    m_OpenedFilePath = filePath;
  }
  else
  {
    return false;
  }

  // Cache the last directory
  m_OpenDialogLastDirectory = fi.path();

  QMessageBox* bookmarkMsgBox = new QMessageBox(this);
  bookmarkMsgBox->setWindowTitle("Pipeline Saved");
  bookmarkMsgBox->setText("The pipeline has been saved.");
  bookmarkMsgBox->setInformativeText("Would you also like to bookmark this pipeline?");
  bookmarkMsgBox->setStandardButtons(QMessageBox::No | QMessageBox::Yes);
  bookmarkMsgBox->setDefaultButton(QMessageBox::Yes);
  int ret = bookmarkMsgBox->exec();

  if (ret == QMessageBox::Yes)
  {
    emit bookmarkNeedsToBeAdded(filePath, QModelIndex());
  }

  delete bookmarkMsgBox;
  bookmarkMsgBox = NULL;

  return true;
}

// -----------------------------------------------------------------------------
//  Called when the main window is closed.
// -----------------------------------------------------------------------------
void DREAM3D_UI::closeEvent(QCloseEvent* event)
{
  if (dream3dApp->isCurrentlyRunning(this) == true)
  {
//    QMessageBox* runningPipelineBox = new QMessageBox();
//    runningPipelineBox->setWindowTitle("Pipeline Is Running");
//    runningPipelineBox->setText("There is a pipeline currently running.\nWould you like to stop the pipeline?");
//    QPushButton* stopPipelineBtn = new QPushButton("Stop Pipeline", runningPipelineBox);
//    runningPipelineBox->addButton(stopPipelineBtn, QMessageBox::YesRole);
//    runningPipelineBox->setStandardButtons(QMessageBox::Cancel);
//    runningPipelineBox->setIcon(QMessageBox::Warning);
//    runningPipelineBox->exec();
//    if (runningPipelineBox->clickedButton() == stopPipelineBtn)
//    {
//      // Cancel the pipeline
//      on_startPipelineBtn_clicked();
//      delete runningPipelineBox;
//    }
//    else
//    {
//      event->ignore();
//      delete runningPipelineBox;
//      return;
//    }

    QMessageBox* runningPipelineBox = new QMessageBox();
    runningPipelineBox->setWindowTitle("Pipeline Is Running");
    runningPipelineBox->setText("There is a pipeline currently running.\nPlease cancel the running pipeline and try again.");
    runningPipelineBox->setStandardButtons(QMessageBox::Ok);
    runningPipelineBox->setIcon(QMessageBox::Warning);
    runningPipelineBox->exec();
    event->ignore();
    return;
  }

  QMessageBox::StandardButton choice = checkDirtyDocument();
  if (choice == QMessageBox::Cancel)
  {
    event->ignore();
    return;
  }

  disconnectSignalsSlots();

  writeSettings();
  clearPipeline();
  dream3dApp->unregisterDREAM3DWindow(this);

  event->accept();
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
  prefs.beginGroup("Bookmarks Dock Widget");
  bookmarksDockWidget->readSettings(this, prefs);
  prefs.endGroup();

  prefs.beginGroup("Prebuilts Dock Widget");
  prebuiltPipelinesDockWidget->readSettings(this, prefs);
  prefs.endGroup();

  prefs.beginGroup("Filter List Dock Widget");
  filterListDockWidget->readSettings(this, prefs);
  prefs.endGroup();

  prefs.beginGroup("Filter Library Dock Widget");
  filterLibraryDockWidget->readSettings(this, prefs);
  prefs.endGroup();

  prefs.beginGroup("Issues Dock Widget");
  issuesDockWidget->readSettings(this, prefs);
  prefs.endGroup();

  prefs.endGroup();

  QRecentFileList::instance()->readList(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::readWindowSettings(DREAM3DSettings& prefs)
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

  if (prefs.contains(QString("MainWindowState")))
  {
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
  prefs.beginGroup("Bookmarks Dock Widget");
  bookmarksDockWidget->writeSettings(prefs);
  prefs.endGroup();

  prefs.beginGroup("Prebuilts Dock Widget");
  prebuiltPipelinesDockWidget->writeSettings(prefs);
  prefs.endGroup();

  prefs.beginGroup("Filter List Dock Widget");
  filterListDockWidget->writeSettings(prefs);
  prefs.endGroup();

  prefs.beginGroup("Filter Library Dock Widget");
  filterLibraryDockWidget->writeSettings(prefs);
  prefs.endGroup();

  prefs.beginGroup("Issues Dock Widget");
  issuesDockWidget->writeSettings(prefs);
  prefs.endGroup();

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

      connect(m_UpdateCheck.data(), SIGNAL( latestVersion(UpdateCheckData*) ),
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

  pipelineViewWidget->setScrollArea(pipelineViewScrollArea);

  // Stretch Factors
  splitter->setStretchFactor(0, 0);
  splitter->setStretchFactor(1, 1);
  splitter->setOpaqueResize(true);

  pipelineViewScrollArea->verticalScrollBar()->setSingleStep(5);

  // Hook up the signals from the various docks to the PipelineViewWidget that will either add a filter
  // or load an entire pipeline into the view
  connectSignalsSlots();

  pipelineViewWidget->setStatusBar(statusbar);

  // This will set the initial list of filters in the filterListDockWidget
  // Tell the Filter Library that we have more Filters (potentially)
  filterLibraryDockWidget->refreshFilterGroups();

  // Set the IssuesDockWidget as a PipelineMessageObserver Object.
  pipelineViewWidget->setPipelineMessageObserver(issuesDockWidget);

  m_ProgressBar->hide();
//  horizontalLayout_2->removeWidget(m_ProgressBar);
//  horizontalLayout_2->removeWidget(startPipelineBtn);
//  horizontalLayout_2->addSpacerItem(progressSpacer);
//  horizontalLayout_2->addWidget(startPipelineBtn);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::disconnectSignalsSlots()
{
  disconnect(filterLibraryDockWidget, SIGNAL(filterItemDoubleClicked(const QString&)),
             pipelineViewWidget, SLOT(addFilter(const QString&)));

  disconnect(filterListDockWidget, SIGNAL(filterItemDoubleClicked(const QString&)),
             pipelineViewWidget, SLOT(addFilter(const QString&)));

  disconnect(prebuiltPipelinesDockWidget, SIGNAL(pipelineFileActivated(const QString&, const bool&, const bool&)),
             dream3dApp, SLOT(newInstanceFromFile(const QString&, const bool&, const bool&)));

  disconnect(bookmarksDockWidget, SIGNAL(pipelineFileActivated(const QString&, const bool&, const bool&)),
             dream3dApp, SLOT(newInstanceFromFile(const QString&, const bool&, const bool&)));

  disconnect(this, SIGNAL(bookmarkNeedsToBeAdded(const QString&, const QModelIndex&)),
             bookmarksDockWidget, SLOT(addBookmark(const QString&, const QModelIndex&)));

  disconnect(pipelineViewWidget, SIGNAL(filterInputWidgetChanged(FilterInputWidget*)),
             this, SLOT(setFilterInputWidget(FilterInputWidget*)));

  disconnect(pipelineViewWidget, SIGNAL(noFilterWidgetsInPipeline()),
             this, SLOT(clearFilterInputWidget()));

  disconnect(pipelineViewWidget, SIGNAL(filterInputWidgetEdited()),
             this, SLOT(markDocumentAsDirty()));

  disconnect(bookmarksDockWidget, SIGNAL(updateStatusBar(const QString&)),
             this, SLOT(setStatusBarMessage(const QString&)));
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::connectSignalsSlots()
{
  // If we are running Mac OS X, implement Close Window shortcut.
#if defined(Q_OS_MAC)
  QAction* m_ActionCloseWindow = new QAction(this);
  m_ActionCloseWindow->setObjectName(QString::fromUtf8("m_ActionCloseWindow"));
  m_ActionCloseWindow->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
  connect(m_ActionCloseWindow, SIGNAL(triggered()), dream3dApp, SLOT(on_actionCloseWindow_triggered()));
  addAction(m_ActionCloseWindow);
#endif

  connect(filterLibraryDockWidget, SIGNAL(filterItemDoubleClicked(const QString&)),
          pipelineViewWidget, SLOT(addFilter(const QString&)) );

  DocRequestManager* docRequester = DocRequestManager::Instance();

  connect(docRequester, SIGNAL(showFilterDocs(const QString&)),
          this, SLOT(showFilterHelp(const QString&)) );

  connect(docRequester, SIGNAL(showFilterDocUrl(const QUrl &)),
          this, SLOT(showFilterHelpUrl(const QUrl &)));

  connect(filterListDockWidget, SIGNAL(filterItemDoubleClicked(const QString&)),
          pipelineViewWidget, SLOT(addFilter(const QString&)) );

  connect(prebuiltPipelinesDockWidget, SIGNAL(pipelineFileActivated(const QString&, const bool&, const bool&)),
          dream3dApp, SLOT(newInstanceFromFile(const QString&, const bool&, const bool&)));

  connect(bookmarksDockWidget, SIGNAL(pipelineFileActivated(const QString&, const bool&, const bool&)),
          dream3dApp, SLOT(newInstanceFromFile(const QString&, const bool&, const bool&)));

  connect(this, SIGNAL(bookmarkNeedsToBeAdded(const QString&, const QModelIndex&)),
          bookmarksDockWidget, SLOT(addBookmark(const QString&, const QModelIndex&)));

  connect(pipelineViewWidget, SIGNAL(filterInputWidgetChanged(FilterInputWidget*)),
          this, SLOT(setFilterInputWidget(FilterInputWidget*)));

  connect(pipelineViewWidget, SIGNAL(noFilterWidgetsInPipeline()),
          this, SLOT(clearFilterInputWidget()));

  connect(pipelineViewWidget, SIGNAL(filterInputWidgetEdited()),
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
void DREAM3D_UI::setLoadedPlugins(QVector<ISIMPLibPlugin*> plugins)
{
  m_LoadedPlugins = plugins;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::on_pipelineViewWidget_pipelineOpened(QString& file, const bool& setOpenedFilePath, const bool& changeTitle)
{
  if (setOpenedFilePath == true)
  {
    m_OpenedFilePath = file;
    setWindowFilePath(file);
  }

  if (changeTitle == true)
  {
    QFileInfo fi(file);
    setWindowTitle(QString("[*]") + fi.baseName() + " - DREAM3D");
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
      if (savePipeline() == true)
      {
        return QMessageBox::Save;
      }
      else
      {
        return QMessageBox::Cancel;
      }
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
void DREAM3D_UI::on_startPipelineBtn_clicked()
{

  if (startPipelineBtn->text().compare("Cancel") == 0)
  {
    qDebug() << "canceling from GUI...." << "\n";
    emit pipelineCanceled();

    // Enable FilterListDockWidget signals - resume adding filters
    filterListDockWidget->blockSignals(false);

    // Enable FilterLibraryDockWidget signals - resume adding filters
    filterLibraryDockWidget->blockSignals(false);

    return;
  }

  m_ProgressBar->show();

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
    issuesDockWidget->displayCachedMessages();
    return;
  }

  // Save the preferences file NOW in case something happens
  writeSettings();

  // Connect signals and slots between DREAM3D_UI and each PipelineFilterWidget
  for (int i = 0; i < pipelineViewWidget->filterCount(); i++)
  {
    PipelineFilterWidget* w = pipelineViewWidget->filterWidgetAt(i);

    if (NULL != w)
    {
      connect(this, SIGNAL(pipelineStarted()), w, SLOT(toRunningState()));
      connect(this, SIGNAL(pipelineCanceled()), w, SLOT(toIdleState()));
      connect(this, SIGNAL(pipelineFinished()), w, SLOT(toIdleState()));
    }
  }

  // Connect signals and slots between DREAM3D_UI and PipelineViewWidget
  connect(this, SIGNAL(pipelineStarted()), pipelineViewWidget, SLOT(toRunningState()));
  connect(this, SIGNAL(pipelineCanceled()), pipelineViewWidget, SLOT(toIdleState()));
  connect(this, SIGNAL(pipelineFinished()), pipelineViewWidget, SLOT(toIdleState()));

  // Connect signals and slots between DREAM3D_UI and DREAM3DApplication
  connect(this, SIGNAL(pipelineStarted()), dream3dApp, SLOT(toPipelineRunningState()));
  connect(this, SIGNAL(pipelineCanceled()), dream3dApp, SLOT(toPipelineIdleState()));
  connect(this, SIGNAL(pipelineFinished()), dream3dApp, SLOT(toPipelineIdleState()));

  // Block FilterListDockWidget signals, so that we can't add filters to the view while running the pipeline
  filterListDockWidget->blockSignals(true);

  // Block FilterLibraryDockWidget signals, so that we can't add filters to the view while running the pipeline
  filterLibraryDockWidget->blockSignals(true);

  // Move the FilterPipeline object into the thread that we just created.
  m_PipelineInFlight->moveToThread(m_WorkerThread);

  // Allow the GUI to receive messages - We are only interested in the progress messages
  m_PipelineInFlight->addMessageReceiver(this);

  // Clear the Error table
  issuesDockWidget->clearIssues();

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

  // Add in a connection to clear the Error/Warnings table when the thread starts
//  connect(m_WorkerThread, SIGNAL(started()),
//          issuesDockWidget, SLOT( clearIssues() ) );

  // Tell the Error/Warnings Table that we are finished and to display any cached messages
  connect(m_WorkerThread, SIGNAL(finished()),
          issuesDockWidget, SLOT( displayCachedMessages() ) );

  // If the use clicks on the "Cancel" button send a message to the PipelineBuilder object
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
  m_PipelineInFlight = FilterPipeline::NullPointer();// This _should_ remove all the filters and deallocate them
  startPipelineBtn->setText("Go");
  m_ProgressBar->setValue(0);

  m_ProgressBar->hide();

  // Re-enable FilterListDockWidget signals - resume adding filters
  filterListDockWidget->blockSignals(false);

  // Re-enable FilterLibraryDockWidget signals - resume adding filters
  filterLibraryDockWidget->blockSignals(false);

  emit pipelineFinished();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::versionCheckReply(UpdateCheckData* dataObj)
{
  DREAM3DUpdateCheckDialog* d = new DREAM3DUpdateCheckDialog(this);
  d->setCurrentVersion((SIMPLib::Version::Complete()));
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
void DREAM3D_UI::showFilterHelp(const QString& className)
{
  // Launch the dialog
#ifdef DREAM3D_USE_QtWebEngine
    DREAM3DUserManualDialog::LaunchHelpDialog(className);
#else
  QUrl helpURL = DREAM3DHelpUrlGenerator::generateHTMLUrl(className.toLower());

  bool didOpen = QDesktopServices::openUrl(helpURL);
  if(false == didOpen)
  {
    QMessageBox msgBox;
    msgBox.setText(QString("Error Opening Help File"));
    msgBox.setInformativeText(QString::fromLatin1("DREAM3D could not open the help file path ") + helpURL.path());
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
  }
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::showFilterHelpUrl(const QUrl& helpURL)
{
#ifdef DREAM3D_USE_QtWebEngine
  DREAM3DUserManualDialog::LaunchHelpDialog(helpURL);
#else
  bool didOpen = QDesktopServices::openUrl(helpURL);
  if(false == didOpen)
  {
    QMessageBox msgBox;
    msgBox.setText(QString("Error Opening Help File"));
    msgBox.setInformativeText(QString::fromLatin1("DREAM3D could not open the help file path ") + helpURL.path());
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
  }
#endif
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
void DREAM3D_UI::updateAndSyncDockWidget(QAction* action, QDockWidget* dock, bool b)
{
  action->blockSignals(true);
  dock->blockSignals(true);

  action->setChecked(b);
  dock->setVisible(b);

  action->blockSignals(false);
  dock->blockSignals(false);
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
PrebuiltPipelinesDockWidget* DREAM3D_UI::getPrebuiltsDockWidget()
{
  return prebuiltPipelinesDockWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterListDockWidget* DREAM3D_UI::getFilterListDockWidget()
{
  return filterListDockWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterLibraryDockWidget* DREAM3D_UI::getFilterLibraryDockWidget()
{
  return filterLibraryDockWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IssuesDockWidget* DREAM3D_UI::getIssuesDockWidget()
{
  return issuesDockWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::setOpenedFilePath(const QString& filePath)
{
  m_OpenedFilePath = filePath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::setOpenDialogLastDirectory(const QString& path)
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
void DREAM3D_UI::markDocumentAsDirty()
{
  setWindowModified(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::setStatusBarMessage(const QString& msg)
{
  statusbar->showMessage(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::changeEvent(QEvent* event)
{
  if (event->type() == QEvent::ActivationChange)
  {
    emit dream3dWindowChangedState(this);
  }
}

#if !defined(Q_OS_MAC)
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3D_UI::setDREAM3DMenu(DREAM3DMenu* menu)
{
  m_InstanceMenu = menu;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DMenu* DREAM3D_UI::getDREAM3DMenu()
{
  return m_InstanceMenu;
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMenu* DREAM3D_UI::createViewMenu()
{
  QMenu* menuView = new QMenu(this);
  menuView->setTitle(QApplication::translate("DREAM3D_UI", "View", 0));
  menuView->setObjectName(QStringLiteral("menuView"));

  QAction* actionShowFilterList = filterListDockWidget->toggleViewAction();
  actionShowFilterList->setText("Filter List");
  menuView->addAction(actionShowFilterList);
  connect(actionShowFilterList, SIGNAL(triggered(bool)), dream3dApp, SLOT(on_actionShowFilterList_triggered(bool)) );


  QAction* actionShowFilterLibrary = filterLibraryDockWidget->toggleViewAction();
  actionShowFilterLibrary->setText("Filter Library");
  menuView->addAction(actionShowFilterLibrary);
  connect(actionShowFilterLibrary, SIGNAL(triggered(bool)), dream3dApp, SLOT(on_actionShowFilterLibrary_triggered(bool)) );


  QAction* actionShowBookmarks = bookmarksDockWidget->toggleViewAction();
  actionShowBookmarks->setText("Bookmarks");
  menuView->addAction(actionShowBookmarks);
  connect(actionShowBookmarks, SIGNAL(triggered(bool)), dream3dApp, SLOT(on_actionShowBookmarks_triggered(bool)) );


  QAction* actionShowPrebuiltPipelines = prebuiltPipelinesDockWidget->toggleViewAction();
  actionShowPrebuiltPipelines->setText("Prebuilt Pipelines");
  menuView->addAction(actionShowPrebuiltPipelines);
  connect(actionShowPrebuiltPipelines, SIGNAL(triggered(bool)), dream3dApp, SLOT(on_actionShowPrebuiltPipelines_triggered(bool)) );

  QAction* actionShowIssues = issuesDockWidget->toggleViewAction();
  actionShowIssues->setText("Show Warnings/Errors");
  menuView->addAction(actionShowIssues);
  connect(actionShowIssues, SIGNAL(triggered(bool)), dream3dApp, SLOT(on_actionShowIssues_triggered(bool)) );

  return menuView;
}



