/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
#include "PipelineBuilderWidget.h"



#include "H5Support/HDF5ScopedFileSentinel.h"

//-- Qt Includes
#include <QtCore/QString>
#include <QtCore/QSet>
#include <QtCore/QDebug>
#include <QtCore/QTime>
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QUrl>
#include <QtCore/QThread>
#include <QtCore/QFileInfoList>
#include <QtCore/QPropertyAnimation>
#include <QtCore/QResource>

#include <QtGui/QFileDialog>
#include <QtGui/QCloseEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QColor>
#include <QtGui/QBrush>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QKeySequence>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QDesktopServices>
#include <QtGui/QScrollArea>
#include <QtGui/QScrollBar>
#include <QtGui/QStyledItemDelegate>
#include <QtGui/QPainter>

#include "QtSupport/HelpDialog.h"
#include "QtSupport/DREAM3DHelpUrlGenerator.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DREAM3DFilters.h"
#include "DREAM3DLib/HDF5/H5FilterParametersReader.h"
#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"

#include "QFilterWidget.h"
#include "AddFavoriteWidget.h"
#include "DREAM3DFileDragMessageBox.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineBuilderWidget::PipelineBuilderWidget(QMenu* pipelineMenu, FilterManager *fm, QWidget *parent) :
  DREAM3DPluginFrame(parent),
  m_FilterPipeline(NULL),
  m_MenuPipeline(NULL),
  m_WorkerThread(NULL),
  m_DocErrorTabsIsOpen(false),
  m_HelpDialog(NULL),
  m_FilterManager(fm)
{
  m_OpenDialogLastDirectory = QDir::homePath();
  setPipelineMenu(pipelineMenu);
  setupUi(this);
  setupContextualMenus();
  setupGui();
  checkIOFiles();

  // Initialize filterList right-click menu
  initFilterListMenu();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineBuilderWidget::~PipelineBuilderWidget()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::onFilterListCustomContextMenuRequested(const QPoint& pos)
{
  m_FilterListPosition = pos;
  QListWidgetItem* item = filterList->itemAt(pos);
  if(NULL != item)
  {
    m_FilterMenu.exec( filterList->mapToGlobal(pos) );
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::initFilterListMenu()
{
  m_actionFilterHelp = new QAction(this);
  m_actionFilterHelp->setObjectName(QString::fromUtf8("actionWidgetHelp"));
  m_actionFilterHelp->setText(QApplication::translate("QFilterWidget", "Show Filter Help", 0, QApplication::UnicodeUTF8));
  connect(m_actionFilterHelp, SIGNAL(triggered()),
          this, SLOT( actionFilterListHelp_triggered() ) );
  m_FilterMenu.addAction(m_actionFilterHelp);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::setPipelineMenu(QMenu* menuPipeline)
{
  this->m_MenuPipeline = menuPipeline;

  QAction* separator = NULL;

  QAction* actionAddFavorite = new QAction(m_MenuPipeline);
  actionAddFavorite->setObjectName(QString::fromUtf8("actionAddFavorite"));
  actionAddFavorite->setText(QApplication::translate("DREAM3D_UI", "Add Favorite", 0, QApplication::UnicodeUTF8));
  menuPipeline->addAction(actionAddFavorite);
  QKeySequence actionAddFavKeySeq(Qt::CTRL + Qt::Key_Plus);
  actionAddFavorite->setShortcut(actionAddFavKeySeq);
  connect(actionAddFavorite, SIGNAL(triggered()),
          this, SLOT( actionAddFavorite_triggered() ) );
  m_FavoriteItemActions << actionAddFavorite;
  m_FavoriteCategoryActions << actionAddFavorite;

  QAction* actionUpdateFavorite = new QAction(m_MenuPipeline);
  actionUpdateFavorite->setObjectName(QString::fromUtf8("actionUpdateFavorite"));
  actionUpdateFavorite->setText(QApplication::translate("DREAM3D_UI", "Update Favorite", 0, QApplication::UnicodeUTF8));
  menuPipeline->addAction(actionUpdateFavorite);
  //QKeySequence actionAddFavKeySeq(Qt::CTRL + Qt::Key_Plus);
  //actionUpdateFavorite->setShortcut(actionAddFavKeySeq);
  connect(actionUpdateFavorite, SIGNAL(triggered()),
          this, SLOT( actionUpdateFavorite_triggered() ) );
  m_FavoriteItemActions << actionUpdateFavorite;

  QAction* actionRenameFavorite = new QAction(m_MenuPipeline);
  actionRenameFavorite->setObjectName(QString::fromUtf8("actionRenameFavorite"));
  actionRenameFavorite->setText(QApplication::translate("DREAM3D_UI", "Rename ...", 0, QApplication::UnicodeUTF8));
  menuPipeline->addAction(actionRenameFavorite);
  QKeySequence actionRenameFavKeySeq(Qt::CTRL + Qt::Key_R);
  actionRenameFavorite->setShortcut(actionRenameFavKeySeq);
  connect(actionRenameFavorite, SIGNAL(triggered()),
          this, SLOT( actionRenameFavorite_triggered() ) );
  m_FavoriteItemActions << actionRenameFavorite;


  QAction* actionAppendFavorite = new QAction(m_MenuPipeline);
  actionAppendFavorite->setObjectName(QString::fromUtf8("actionAppendFavorite"));
  actionAppendFavorite->setText(QApplication::translate("DREAM3D_UI", "Append to Pipeline", 0, QApplication::UnicodeUTF8));
  menuPipeline->addAction(actionAppendFavorite);
  QKeySequence actionAppendFavKeySeq(Qt::CTRL + Qt::Key_A);

  actionAppendFavorite->setShortcut(actionAppendFavKeySeq);
  connect(actionAppendFavorite, SIGNAL(triggered()),
          this, SLOT( actionAppendFavorite_triggered() ) );
  m_FavoriteItemActions << actionAppendFavorite;
  m_PrebuiltItemActions << actionAppendFavorite;


  menuPipeline->addSeparator();
  m_FavoriteItemActions << separator;

  QAction* actionRemoveFavorite = new QAction(m_MenuPipeline);
  actionRemoveFavorite->setObjectName(QString::fromUtf8("actionRemoveFavorite"));
  actionRemoveFavorite->setText(QApplication::translate("DREAM3D_UI", "Remove Favorite", 0, QApplication::UnicodeUTF8));
  menuPipeline->addAction(actionRemoveFavorite);
  QKeySequence actionRemoveFavKeySeq(Qt::CTRL + Qt::Key_Minus);
  actionRemoveFavorite->setShortcut(actionRemoveFavKeySeq);
  connect(actionRemoveFavorite, SIGNAL(triggered()),
          this, SLOT( actionRemoveFavorite_triggered() ) );
  m_FavoriteItemActions << actionRemoveFavorite;
  menuPipeline->addSeparator();


  QAction* actionClearPipeline = new QAction(m_MenuPipeline);
  actionClearPipeline->setObjectName(QString::fromUtf8("actionClearPipeline"));
  actionClearPipeline->setText(QApplication::translate("DREAM3D_UI", "Clear Pipeline", 0, QApplication::UnicodeUTF8));
  menuPipeline->addAction(actionClearPipeline);
  QKeySequence actionClearKeySeq(Qt::CTRL + Qt::Key_Delete);
  actionClearPipeline->setShortcut(actionClearKeySeq);
  connect(actionClearPipeline, SIGNAL(triggered()),
          this, SLOT( actionClearPipeline_triggered() ) );


  QAction* actionShowInFileSystem = new QAction(this);
  actionShowInFileSystem->setObjectName(QString::fromUtf8("actionShowInFileSystem"));
  // Handle the naming based on what OS we are currently running...
#if defined(Q_OS_WIN)

  actionShowInFileSystem->setText(QApplication::translate("DREAM3D_UI", "Show in Windows Explorer", 0, QApplication::UnicodeUTF8));
#elif defined(Q_OS_MAC)
  actionShowInFileSystem->setText(QApplication::translate("DREAM3D_UI", "Show in Finder", 0, QApplication::UnicodeUTF8));
#else
  actionShowInFileSystem->setText(QApplication::translate("DREAM3D_UI", "Show in File System", 0, QApplication::UnicodeUTF8));
#endif

  connect(actionShowInFileSystem, SIGNAL(triggered()),
          this, SLOT( actionShowInFileSystem_triggered() ) );
  m_FavoriteItemActions << separator;
  m_FavoriteItemActions << actionShowInFileSystem;
  m_PrebuiltItemActions << separator;
  m_PrebuiltItemActions << actionShowInFileSystem;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::setupContextualMenus()
{
  // Create favorites action list and add to tree
  filterLibraryTree->setActionList(PipelineTreeWidget::Favorite_Item_Type, m_FavoriteItemActions);

  // Create prebuilt action list and add to tree
  filterLibraryTree->setActionList(PipelineTreeWidget::Prebuilt_Item_Type, m_PrebuiltItemActions);

  // Create library action list and add to tree

  // Create favorite-category action list and add to tree
  filterLibraryTree->setActionList(PipelineTreeWidget::Favorite_Category_Item_Type, m_FavoriteCategoryActions);

  // Create prebuilt-category action list and add to tree
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMenu* PipelineBuilderWidget::getPipelineMenu()
{
  return m_MenuPipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::extractPipelineFromFile(const QString &filePath)
{
  hid_t fid = QH5Utilities::openFile( filePath );
  if (fid <= 0)
  {
    //Present a error dialog
    QMessageBox::critical(this, "Error Opening DREAM3D File", QString("Error opening file ") + filePath, QMessageBox::Ok, QMessageBox::Ok);
    return;
  }
  HDF5ScopedFileSentinel sentinel(&fid, true);
  fid = *(sentinel.getFileId());
  int err = readPipelineFromFile(fid);
  if (err < 0)
  {
    QMessageBox::critical(this, "Error Extracting Pipeline from DREAM3D file.",
                          "An error occured with extracting the pipeline from the DREAM3D file.",QMessageBox::Ok, QMessageBox::Ok);
    return;
  }
  FilterPipeline::FilterContainerType filters = m_PipelineFromFile->getFilterContainer();

  FilterPipeline::FilterContainerType::iterator iter = filters.begin();

  for (; iter != filters.end(); iter++)
  {

    QString filterName = (*iter)->getNameOfClass();
  //  qDebug() << QTime::currentTime() << " Creating Filter: " << (filterName);
    QFilterWidget* w = m_PipelineViewWidget->addFilter( (filterName) );
   // qDebug() << QTime::currentTime() << " Loading GUI Values: " << (filterName);
    if(w) {
      QTime qtime = QTime::currentTime();
      m_PipelineViewWidget->preflightPipeline();
    //  qDebug() << "ms: " << qtime.msecsTo(QTime::currentTime());
      qtime = QTime::currentTime();
      w->getGuiParametersFromFilter( (*iter).get() );
     // qDebug() << "ms: " << qtime.msecsTo(QTime::currentTime());
    }
  }
  // One last preflight to get the changes introduced by the last filter
 // m_PipelineViewWidget->preflightPipeline();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::addDREAM3DReaderFilter(const QString &filePath)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::openPipelineFile(const QString &filePath)
{
  QFileInfo fi (filePath);
  QString extension = fi.suffix();
  if ( extension == "ini" || extension == "txt")
  {
    QSettings prefs(filePath, QSettings::IniFormat, this);
    readSettings(prefs, true);
  }
  else if (extension == "dream3d")
  {
    DREAM3DFileDragMessageBox* msgBox = new DREAM3DFileDragMessageBox(this);
    msgBox->setFilePath(filePath);
    connect(msgBox, SIGNAL(fireExtractPipelineFromFile(const QString &)), this, SLOT(extractPipelineFromFile(const QString &)));
    connect(msgBox, SIGNAL(fireAddDREAM3DReaderFilter(const QString &)), this, SLOT(addDREAM3DReaderFilter(const QString &)));
    msgBox->exec();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PipelineBuilderWidget::readPipelineFromFile(hid_t fileId)
{
  int err = 0;
  m_PipelineFromFile->clear();

  H5FilterParametersReader::Pointer reader = H5FilterParametersReader::New();

  // HDF5: Open the "Pipeline" Group
  hid_t pipelineGroupId = H5Gopen(fileId, DREAM3D::HDF5::PipelineGroupName.toLatin1().data(), H5P_DEFAULT);
  reader->setGroupId(pipelineGroupId);

  // Use QH5Lite to ask how many "groups" are in the "Pipeline Group"
  QList<QString> groupList;
  err = QH5Utilities::getGroupObjects(pipelineGroupId, H5Utilities::H5Support_GROUP, groupList);

  // Loop over the items getting the "ClassName" attribute from each group
  QString classNameStr = "";
  for (int i=0; i<groupList.size(); i++)
  {

    QString iStr = QString::number(i);
    err = QH5Lite::readStringAttribute(pipelineGroupId, iStr, "ClassName", classNameStr);

    // Instantiate a new filter using the FilterFactory based on the value of the className attribute
    IFilterFactory::Pointer ff = m_FilterManager->getFactoryForFilter(classNameStr);
    if (NULL != ff)
    {
      AbstractFilter::Pointer filter = ff->create();
      if(NULL != ff)
      {
        // Read the parameters
        filter->readFilterParameters( reader.get(), i );

        // Add filter to m_PipelineFromFile
        m_PipelineFromFile->pushBack(filter);
      }
    }
    else
    {
      QMessageBox::critical(this, "Error Adding Filter to Pipeline",
                            QString("The filter was not known to DREAM3D:") + (classNameStr),
                            QMessageBox::Ok, QMessageBox::Ok);
    }

  }

  H5Gclose(pipelineGroupId);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::readSettings(QSettings &prefs, bool shouldClear)
{
  prefs.beginGroup(Detail::PipelineBuilderGroup);

  //  bool ok = false;
  splitter_1->restoreState(prefs.value("splitter_1").toByteArray());
  splitter_2->restoreState(prefs.value("splitter_2").toByteArray());
  prefs.endGroup();

  readSettings(prefs, m_PipelineViewWidget, shouldClear);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::readSettings(QSettings &prefs, PipelineViewWidget* viewWidget, bool shouldClear)
{
  // Clear Any Existing Pipeline
  if (shouldClear)
  {
    m_PipelineViewWidget->clearWidgets();
  }

  prefs.beginGroup(Detail::PipelineBuilderGroup);

  bool ok = false;
  int filterCount = prefs.value("Number_Filters").toInt(&ok);
  prefs.endGroup();

  if (false == ok) {filterCount = 0;}
  for (int i = 0; i < filterCount; ++i)
  {
    QString gName = QString::number(i);

    prefs.beginGroup(gName);

    QString filterName = prefs.value("Filter_Name", "").toString();

    QFilterWidget* w = viewWidget->addFilter(filterName); // This will set the variable m_SelectedFilterWidget

    if(w) {
      m_PipelineViewWidget->preflightPipeline();
      w->blockSignals(true);
      w->readOptions(prefs);
      w->blockSignals(false);
    }
    prefs.endGroup();
  }
  // One last preflight to get the changes introduced by the last filter
  m_PipelineViewWidget->preflightPipeline();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::readFavoritePipelines()
{
  QString prefFile;
  {
#if defined (Q_OS_MAC)
    QSettings prefs(QSettings::NativeFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
    QString extension = ".ini";
#else
    QSettings prefs(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
    QString extension = ".ini";
#endif
    prefFile = prefs.fileName();
  }

  QFileInfo prefFileInfo = QFileInfo(prefFile);
  QString parentPath = prefFileInfo.path();
  QString favPath = parentPath + QDir::separator() + "DREAM3D_Favorites";
  QDir favDir = QDir(favPath);

  QStringList filters;
  filters << "*.ini";
  QFileInfoList dirList = favDir.entryInfoList(filters);
  foreach(QFileInfo fi, dirList)
  {
    QString favFilePath = fi.absoluteFilePath();
    QSettings favPref(favFilePath, QSettings::IniFormat);
    QString favName;
    {
      favPref.beginGroup(Detail::FavoriteConfig);
      favName = favPref.value("Name").toString();
      favPref.endGroup();
      // This next code section is here to move from the old "favorite_config" to the newer Detail::PipelineBuilderGroup ini group
      if (favName.isEmpty() == false)
      {
        favPref.beginGroup(Detail::PipelineBuilderGroup);
        favPref.setValue("Name", favName);
        favPref.endGroup();
        favPref.remove("favorite_config"); // Now that we transfered the value, remove the old value
      }
    }

    {
      favPref.beginGroup(Detail::PipelineBuilderGroup);
      favName = favPref.value("Name").toString();
      favPref.endGroup();
    }

    // Add a tree widget item for this favorite
    QTreeWidgetItem* favoriteItem = new QTreeWidgetItem(m_favorites, PipelineTreeWidget::Favorite_Item_Type);
    favoriteItem->setText(0, favName);
    favoriteItem->setIcon(0, QIcon(":/bullet_ball_yellow.png"));
    favoriteItem->setData(0, Qt::UserRole, QVariant(favFilePath));
    favoriteItem->setFlags(favoriteItem->flags() | Qt::ItemIsEditable);
    //   m_favoritesMap[favoriteItem] = favFilePath;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::readPrebuiltPipelines()
{
  QString appPath = qApp->applicationDirPath();
  QDir prebuiltDir = QDir(appPath);
#if defined(Q_OS_WIN)

#elif defined(Q_OS_MAC)
  if (prebuiltDir.dirName() == "MacOS")
  {
    prebuiltDir.cdUp();
    prebuiltDir.cdUp();
    prebuiltDir.cdUp();
  }
#else
  // We are on Linux - I think
  prebuiltDir.cdUp();
#endif

#if defined(Q_OS_WIN)
  QFileInfo fi( prebuiltDir.absolutePath() + QDir::separator() + "PrebuiltPipelines");
  if (fi.exists() == false)
  {
    // The help file does not exist at the default location because we are probably running from visual studio.
    // Try up one more directory
    prebuiltDir.cdUp();
  }
#endif
  prebuiltDir = prebuiltDir.absolutePath() + QDir::separator() + "PrebuiltPipelines";

  // So Now we have the top level Directory for the Prebuilts
  addFiltersRecursively(prebuiltDir, m_prebuilts);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::addFiltersRecursively(QDir currentDir, QTreeWidgetItem* currentDirItem)
{
  QTreeWidgetItem* nextDirItem;

  // Get a list of all the directories
  QFileInfoList dirList = currentDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
  if ( dirList.size() > 0 )
  {
    foreach(QFileInfo fi, dirList)
    {
      // At this point we have the first level of directories and we want to do 2 things:
      // 1.Create an entry in the tree widget with this name
      // 2.drop into the directory and look for all the .txt files and add entries for those items.
      //qDebug() << fi.absoluteFilePath() << "\n";
      // Add a tree widget item for this Prebuilt Group
      nextDirItem = new QTreeWidgetItem(currentDirItem);
      nextDirItem->setText(0, fi.baseName());
      addFiltersRecursively( QDir( fi.absoluteFilePath() ), nextDirItem );   // Recursive call
    }
  }

  QStringList filters;
  filters << "*.txt";
  QFileInfoList pblist = currentDir.entryInfoList(filters);
  foreach(QFileInfo pbinfo, pblist)
  {
    QString pbFilePath = pbinfo.absoluteFilePath();
    QSettings pbPref(pbFilePath, QSettings::IniFormat);
    pbPref.beginGroup(Detail::PipelineBuilderGroup);
    QString pbName = pbPref.value("Name").toString();
    pbPref.endGroup();
    //qDebug() << pbinfo.absoluteFilePath() << "\n";
    // Add tree widget for this Prebuilt Pipeline
    QTreeWidgetItem* prebuiltItem = new QTreeWidgetItem(currentDirItem, PipelineTreeWidget::Prebuilt_Item_Type);
    prebuiltItem->setText(0, pbName);
    prebuiltItem->setIcon(0, QIcon(":/bullet_ball_blue.png"));
    prebuiltItem->setData(0, Qt::UserRole, QVariant(pbinfo.absoluteFilePath()));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::writeSettings(QSettings &prefs)
{

  prefs.setValue("splitter_1", splitter_1->saveState());
  prefs.setValue("splitter_2", splitter_2->saveState());

  /*
  prefs.beginGroup(Detail::FavoritePipelines);
  prefs.clear();
  prefs.setValue( "count", m_favoritesMap.size() );

  int index = 0;
  QMapIterator<QString, QString> i(m_favoritesMap);
  while (i.hasNext()) {
    i.next();
    prefs.setValue( QString::number(index) + "_Favorite_Name", i.key() );
    prefs.setValue( QString::number(index) + "_Favorite_File", i.value() );
    index++;
  }
  prefs.endGroup();
*/
  writeSettings(prefs, m_PipelineViewWidget);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::savePipeline(QSettings &prefs) {
  writeSettings(prefs, m_PipelineViewWidget);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::writeSettings(QSettings &prefs, PipelineViewWidget* viewWidget)
{
  prefs.beginGroup(Detail::PipelineBuilderGroup);
  qint32 count = m_PipelineViewWidget->filterCount();
  QFileInfo fi(prefs.fileName());

  prefs.setValue("Number_Filters", count);
  prefs.setValue("Name", fi.baseName()); // Put a default value in here
  prefs.endGroup();

  for(qint32 i = 0; i < count; ++i)
  {
    QFilterWidget* fw = viewWidget->filterWidgetAt(i);
    if (fw)
    {
      QString groupName = QString::number(i);
      prefs.beginGroup(groupName);
      fw->writeOptions(prefs);
      prefs.endGroup();
    }
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList) {
    w->setEnabled(b);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::setupGui()
{
  m_PipelineFromFile = FilterPipeline::New();

  connect(m_PipelineViewWidget, SIGNAL(pipelineHasErrorsSignal()), this, SLOT(disableGoBtn()));
  connect(m_PipelineViewWidget, SIGNAL(pipelineHasNoErrors()), this, SLOT(enableGoBtn()));

  filterLibraryTree->setEditTriggers(QAbstractItemView::SelectedClicked);
  m_hasErrors = false;
  m_hasWarnings = false;

  // Get the QFilterWidget Manager Instance
  FilterWidgetManager::Pointer fm = FilterWidgetManager::Instance();

  QSet<QString> groupNames = fm->getGroupNames();

  QTreeWidgetItem* library = new QTreeWidgetItem(filterLibraryTree);
  library->setText(0, Detail::Library);
  library->setIcon(0, QIcon(":/cubes.png"));

  m_prebuilts = new QTreeWidgetItem(filterLibraryTree, PipelineTreeWidget::Prebuilt_Category_Item_Type);
  m_prebuilts->setText(0, Detail::PrebuiltPipelines);
  m_prebuilts->setIcon(0, QIcon(":/flag_blue_scroll.png"));
  m_prebuilts->setExpanded(true);

  blockSignals(true);
  m_favorites = new QTreeWidgetItem(filterLibraryTree, PipelineTreeWidget::Favorite_Category_Item_Type);
  m_favorites->setText(0, Detail::FavoritePipelines);
  m_favorites->setIcon(0, QIcon(":/flash.png"));
  blockSignals(false);

  m_favorites->setExpanded(true);

  //  qDebug() << "Groups Found: " << "\n";
  for(QSet<QString>::iterator iter = groupNames.begin(); iter != groupNames.end(); ++iter)
  {
    //   qDebug() << *iter << "\n";
    QString iconName(":/");
    iconName.append( (*iter));
    iconName.append("_Icon.png");
    // Validate the icon is in the resource system
    QFileInfo iconInfo(iconName);
    if (iconInfo.exists() == false)
    {
      iconName = ":/Plugin_Icon.png"; // Switch to our generic icon for Plugins that do not provide their own
    }

    QIcon icon(iconName);
    QTreeWidgetItem* filterGroup = new QTreeWidgetItem(library);
    filterGroup->setText(0, (*iter));
    filterGroup->setIcon(0, icon);
    QSet<QString> subGroupNames = fm->getSubGroupNames(*iter);
    for(QSet<QString>::iterator iter2 = subGroupNames.begin(); iter2 != subGroupNames.end(); ++iter2)
    {
      QTreeWidgetItem* filterSubGroup = new QTreeWidgetItem(filterGroup);
      filterSubGroup->setText(0, (*iter2));
    }
  }
  library->setExpanded(true);

  errorTableWidget->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
  errorTableWidget->horizontalHeader()->setResizeMode(1, QHeaderView::Interactive);
  errorTableWidget->horizontalHeader()->resizeSection(1, 250);
  errorTableWidget->horizontalHeader()->setResizeMode(2, QHeaderView::ResizeToContents);
  errorTableWidget->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);

  m_PipelineViewWidget->setErrorsTextArea(errorTableWidget);

  // Connect the PipelineViewWidget Signals to slots
  connect(m_PipelineViewWidget, SIGNAL(pipelineFileDropped(const QString&)),
          this, SLOT(openPipelineFile(const QString& )) );


  m_DocErrorTabsIsOpen = false;

  toggleDocs->setChecked(false);
  showErrors->setChecked(false);

  m_HelpDialog = new HelpDialog(this);
  m_HelpDialog->setWindowModality(Qt::NonModal);

  readPrebuiltPipelines();

  filterLibraryTree->blockSignals(true);
  readFavoritePipelines();
  filterLibraryTree->blockSignals(false);

  on_filterLibraryTree_itemClicked(library, 0);

  m_PipelineViewWidget->setScrollArea(m_PipelineViewScrollArea);

  connect(m_PipelineViewWidget, SIGNAL(preflightHasMessage(PipelineMessage)),
          this, SLOT(addMessage(PipelineMessage)) );

  //
  filterList->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(filterList, SIGNAL(customContextMenuRequested(const QPoint&)),
          this, SLOT(onFilterListCustomContextMenuRequested(const QPoint&)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::disableGoBtn()
{
  m_GoBtn->setEnabled(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::enableGoBtn()
{
  m_GoBtn->setEnabled(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::on_filterLibraryTree_itemClicked( QTreeWidgetItem* item, int column )
{
  // Get the QFilterWidget Manager Instance
  FilterWidgetManager::Pointer fm = FilterWidgetManager::Instance();
  FilterWidgetManager::Collection factories;
  QTreeWidgetItem* parent = item->parent();
  while(NULL != parent)
  {
    if (NULL == parent->parent() )
    {
      break;
    }
    parent = parent->parent();
  }
  if (parent == NULL) {
    return;
  }

  QString itemText = parent->text(0);

#if 0
  if (itemText.compare(Detail::Library) == 0)
  {
    factories = fm->getFactories();
    updateFilterGroupList(factories);
  }
#endif

  if (itemText.compare(Detail::PrebuiltPipelines) == 0)
  {
    //QString prebuiltName = item->text(0);
    QString prebuiltPath = item->data(0, Qt::UserRole).toString();
    QStringList filterList = generateFilterListFromPipelineFile(prebuiltPath);
    populateFilterList(filterList);
  }
  else if(itemText.compare(Detail::FavoritePipelines) == 0)
  {
    //QString favoriteName = item->text(0);
    QString favoritePath = item->data(0, Qt::UserRole).toString();
    QStringList filterList = generateFilterListFromPipelineFile(favoritePath);
    populateFilterList(filterList);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::on_filterLibraryTree_itemChanged( QTreeWidgetItem* item, int column )
{
  if (NULL != item->parent() && item->parent()->text(0).compare(Detail::FavoritePipelines) == 0)
  {
    QString favoritePath = item->data(0, Qt::UserRole).toString();
    QString newFavoriteTitle = item->text(0);

    // Check favorite title for illegal characters and duplicate favorite names
    if ( checkFavoriteTitle(favoritePath, newFavoriteTitle, item) )
    {
      return;
    }

    // Create the new file path and write to it
    QString newPath = writeNewFavoriteFilePath(item->text(0), favoritePath, item);

    // Set Name in preferences group
    QSettings newFavoritePrefs(newPath, QSettings::IniFormat);
    newFavoritePrefs.beginGroup(Detail::PipelineBuilderGroup);
    newFavoritePrefs.setValue("Name", item->text(0) );
    newFavoritePrefs.endGroup();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineBuilderWidget::checkFavoriteTitle(QString favoritePath, QString newFavoriteTitle, QTreeWidgetItem* item)
{
  // Put all children (favorites) in a list
  QList<QTreeWidgetItem*> favoritesList;
  int numOfChildren = item->parent()->childCount();
  for (int i=0; i<numOfChildren; i++)
  {
    favoritesList.append( item->parent()->child(i) );
  }

  // Check for illegal characters and duplicate favorite names
  if ( hasIllegalFavoriteName(favoritePath, newFavoriteTitle, item) ||
       hasDuplicateFavorites(favoritesList, favoritePath, newFavoriteTitle, item) )
  {
    return true;
  }
  else
  {
    return false;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineBuilderWidget::hasDuplicateFavorites(QList<QTreeWidgetItem*> favoritesList, QString favoritePath, QString newFavoriteTitle, QTreeWidgetItem* item)
{
  QSettings favoritePrefs(favoritePath, QSettings::IniFormat);
  QString displayText = "";

  for (int i=0; i<favoritesList.size(); i++)
  {
    QSettings currentItemPrefs(favoritesList[i]->data(0, Qt::UserRole).toString(), QSettings::IniFormat);
    currentItemPrefs.beginGroup(Detail::PipelineBuilderGroup);

    // If the new title matches one of the other favorite titles
    if (currentItemPrefs.value("Name").toString() == newFavoriteTitle)
    {
      displayText = "A favorite that has this title already exists in the Favorites list.\n\n";

      // Change the GUI back to the old name
      favoritePrefs.beginGroup(Detail::PipelineBuilderGroup);
      filterLibraryTree->blockSignals(true);
      item->setText(0, favoritePrefs.value("Name").toString() );
      filterLibraryTree->blockSignals(false);
      favoritePrefs.endGroup();

      // Display error message
      QMessageBox::critical(this, tr("Rename Favorite"), tr(displayText.toLatin1().data()),
                            QMessageBox::Ok, QMessageBox::Ok);

      return true;
    }
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineBuilderWidget::hasIllegalFavoriteName(QString favoritePath, QString newFavoriteTitle, QTreeWidgetItem* item)
{
  QSettings favoritePrefs(favoritePath, QSettings::IniFormat);
  QString displayText = "";

  if ( newFavoriteTitle.contains(QRegExp("[^a-zA-Z_-\\d]")) )
  {
    displayText = "The title that was chosen has illegal characters.\n\nNames can only have:\n\tLetters\n\tNumbers\n\tUnderscores\n\tDashes";
    displayText = displayText + "\n\nNo spaces allowed";

    // Change the GUI back to the old name
    favoritePrefs.beginGroup(Detail::PipelineBuilderGroup);
    filterLibraryTree->blockSignals(true);
    item->setText(0, favoritePrefs.value("Name").toString() );
    filterLibraryTree->blockSignals(false);
    favoritePrefs.endGroup();

    // Display error message
    QMessageBox::critical(this, tr("Rename Favorite"), tr(displayText.toLatin1().data()),
                          QMessageBox::Ok, QMessageBox::Ok);

    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineBuilderWidget::writeNewFavoriteFilePath(QString newFavoriteTitle, QString favoritePath, QTreeWidgetItem* item)
{
  QFileInfo original(favoritePath);
  QString newPath = original.canonicalPath() + QDir::separator() + newFavoriteTitle;
  if (!original.completeSuffix().isEmpty())
    newPath += "." + original.completeSuffix();

  newPath = QDir::toNativeSeparators(newPath);

  QFile f(favoritePath);
  bool success = f.rename(newPath);
  if (false == success)
  {
    qDebug() << "Failed";
  }

  filterLibraryTree->blockSignals(true);
  item->setData(0, Qt::UserRole, QVariant(newPath));
  filterLibraryTree->blockSignals(false);

  return newPath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::on_filterLibraryTree_currentItemChanged(QTreeWidgetItem* item, QTreeWidgetItem* previous )
{
  // Get the QFilterWidget Manager Instance
  FilterWidgetManager::Pointer fm = FilterWidgetManager::Instance();
  FilterWidgetManager::Collection factories;

  //If the user clicks on Detail::Library, display all
  if ( item->text(0).compare(Detail::Library) == 0)
  {
    factories = fm->getFactories();
    updateFilterGroupList(factories);
  }
  else if (NULL != item->parent() && item->parent()->text(0).compare(Detail::Library) == 0)
  {
    factories = fm->getFactories(item->text(0));
    updateFilterGroupList(factories);
  }
  else if (NULL != item->parent() && NULL != item->parent()->parent() && item->parent()->parent()->text(0).compare(Detail::Library) == 0)
  {
    factories = fm->getFactories(item->parent()->text(0), item->text(0));
    updateFilterGroupList(factories);
  }
  else
  {
    // Update filter list with preview of current item
    on_filterLibraryTree_itemClicked(item, 0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::on_filterLibraryTree_itemDoubleClicked( QTreeWidgetItem* item, int column )
{
  QTreeWidgetItem* parent = item->parent();

  while(NULL != parent)
  {
    if (NULL == parent->parent() )
    {
      break;
    }
    parent = parent->parent();
  }
  if (parent == NULL) {
    return;
  }

  QString itemText = parent->text(0);
  if (m_PipelineViewWidget->isEnabled() == false) { return; }
  if (itemText.compare(Detail::PrebuiltPipelines) == 0
      || itemText.compare(Detail::FavoritePipelines) == 0 )
  {
    QString pipelinePath = item->data(0, Qt::UserRole).toString();
    if (pipelinePath.isEmpty() == false)
    {
      loadPipelineFileIntoPipelineView(pipelinePath);
    }
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::updateFilterGroupList(FilterWidgetManager::Collection &factories)
{
  // Clear all the current items from the list
  filterList->clear();
  filterList->setSortingEnabled(true);

  for (FilterWidgetManager::Collection::iterator factory = factories.begin(); factory != factories.end(); ++factory)
  {
    IFilterWidgetFactory::Pointer wigFactory = factory.value();
    if (NULL == wigFactory.get() ) {
      continue;
    }
    QString humanName = (wigFactory->getFilterHumanLabel());
    QString iconName(":/");
    iconName.append( (wigFactory->getFilterGroup()));
    iconName.append("_Icon.png");

    // Validate the icon is in the resource system
    QFileInfo iconInfo(iconName);
    if (iconInfo.exists() == false)
    {
      iconName = ":/Plugin_Icon.png"; // Switch to our generic icon for Plugins that do not provide their own
    }

    QIcon icon(iconName);
    // Create the QListWidgetItem and add it to the filterList
    QListWidgetItem* filterItem = new QListWidgetItem(icon, humanName, filterList);
    // Set an "internal" QString that is the name of the filter. We need this value
    // when the item is clicked in order to retreive the Filter Widget from the
    // filter widget manager.
    QString filterName = (factory.key());
    filterItem->setData( Qt::UserRole, filterName);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::on_filterList_currentItemChanged ( QListWidgetItem * item, QListWidgetItem * previous )
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::on_filterSearch_textChanged (const QString& text)
{

  FilterWidgetManager::Pointer fm = FilterWidgetManager::Instance();
  FilterWidgetManager::Collection factories = fm->getFactories(); // Get all the Factories
  QTreeWidgetItem* item = filterLibraryTree->currentItem();
  if (item->parent() == NULL && item->text(0).compare(Detail::Library) == 0)
  {
    factories = fm->getFactories();
  }
  else if (item->parent() != NULL && item->parent()->text(0).compare(Detail::Library) == 0)
  {
    factories = fm->getFactories(item->text(0));
  }
  else if (item->parent()->parent() != NULL && item->parent()->parent()->text(0).compare(Detail::Library) == 0)
  {
    factories = fm->getFactories(item->parent()->text(0), item->text(0));
  }

  // Nothing was in the search Feature so just reset to what was listed before
  if (text.isEmpty() == true)
  {
    updateFilterGroupList(factories);
    return;
  }

  // The user is typing something in the search box so lets search the filter class name and human label
  filterList->clear();
  for (FilterWidgetManager::Collection::iterator factory = factories.begin(); factory != factories.end(); ++factory)
  {
    IFilterWidgetFactory::Pointer wigFactory = factory.value();
    if (NULL == wigFactory.get() ) {
      continue;
    }
    QString humanName = (wigFactory->getFilterHumanLabel());
    bool match = false;
    if (humanName.contains(text, Qt::CaseInsensitive) == true)
    {
      match = true;
    }
    QString filterName = (factory.key());
    if (filterName.contains(text, Qt::CaseInsensitive) == true)
    {
      match = true;
    }
    // Nothing matched the string
    if (false == match) { continue; }

    QString iconName(":/");
    iconName.append( (wigFactory->getFilterGroup()));
    iconName.append("_Icon.png");

    // Validate the icon is in the resource system
    QFileInfo iconInfo(iconName);
    if (iconInfo.exists() == false)
    {
      iconName = ":/Plugin_Icon.png"; // Switch to our generic icon for Plugins that do not provide their own
    }

    QIcon icon(iconName);
    // Create the QListWidgetItem and add it to the filterList
    QListWidgetItem* filterItem = new QListWidgetItem(icon, humanName, filterList);
    // Set an "internal" QString that is the name of the filter. We need this value
    // when the item is clicked in order to retreive the Filter Widget from the
    // filter widget manager.
    filterItem->setData( Qt::UserRole, filterName);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::on_filterList_itemDoubleClicked( QListWidgetItem* item )
{
  if (m_PipelineViewWidget->isEnabled() == false) { return; }
  m_PipelineViewWidget->addFilter(item->data(Qt::UserRole).toString());
  m_PipelineViewWidget->preflightPipeline();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::on_errorTableWidget_itemClicked( QTableWidgetItem* item )
{
  qDebug() << item->text().toLatin1().data() << "\n";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::actionFilterListHelp_triggered()
{
  QListWidgetItem* listItem = filterList->itemAt(m_FilterListPosition);
  if (NULL == listItem) { return; }
  FilterWidgetManager::Pointer wm = FilterWidgetManager::Instance();

  IFilterWidgetFactory::Pointer wf = wm->getFactoryForFilter(listItem->data(Qt::UserRole).toString());
  if (NULL == wf) { return;}

  DREAM3DHelpUrlGenerator::generateAndOpenHTMLUrl( listItem->data(Qt::UserRole).toString().toLower(), this );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::on_toggleDocs_clicked()
{
  DREAM3DHelpUrlGenerator::generateAndOpenHTMLUrl("index", this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::on_showErrors_clicked()
{
  m_DocErrorTabsIsOpen = !m_DocErrorTabsIsOpen;
  docErrorTabs->setHidden(m_DocErrorTabsIsOpen);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::actionClearPipeline_triggered()
{
  // Clear Any Existing Pipeline
  m_PipelineViewWidget->clearWidgets();
  clearMessagesTable();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::clearMessagesTable()
{
  QTableWidget* errorTableWidget = m_PipelineViewWidget->getTableWidget();
  QStringList m_PipelineErrorList = m_PipelineViewWidget->getPipelineErrorList();

  // clear all the error messages
  m_PipelineErrorList.clear();

  for (int i=0; i<errorTableWidget->rowCount(); ++i) {
    errorTableWidget->removeRow(i);
  }
  errorTableWidget->setRowCount(0);
}

// -----------------------------------------------------------------------------
// This method MUST be implemented as it is a pure virtual in the super class
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::checkIOFiles()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::on_m_GoBtn_clicked()
{


  if (m_GoBtn->text().compare("Cancel") == 0)
  {
    if(m_FilterPipeline!= NULL)
    {
      //qDebug() << "canceling from GUI...." << "\n";
      emit cancelPipeline();
    }
    return;
  }

  // Save the preferences file NOW in case something happens
  emit fireWriteSettings();

  m_PipelineViewWidget->setEnabled(false);
  clearMessagesTable();

  m_hasErrors = false;
  m_hasWarnings = false;
  if (m_WorkerThread != NULL)
  {
    m_WorkerThread->wait(); // Wait until the thread is complete
    delete m_WorkerThread; // Kill the thread
    m_WorkerThread = NULL;
  }
  m_WorkerThread = new QThread(); // Create a new Thread Resource

  m_FilterPipeline = new QFilterPipeline(NULL);

  // Move the PipelineBuilder object into the thread that we just created.
  m_FilterPipeline->moveToThread(m_WorkerThread);

  //
  qint32 count = m_PipelineViewWidget->filterCount();
  for(qint32 i = 0; i < count; ++i)
  {
    QFilterWidget* fw = m_PipelineViewWidget->filterWidgetAt(i);
    if (fw)
    {
      m_FilterPipeline->pushBack(fw->getFilter(false));
    }
  }

  /* Connect the signal 'started()' from the QThread to the 'run' slot of the
   * PipelineBuilder object. Since the PipelineBuilder object has been moved to another
   * thread of execution and the actual QThread lives in *this* thread then the
   * type of connection will be a Queued connection.
   */
  // When the thread starts its event loop, start the PipelineBuilder going
  connect(m_WorkerThread, SIGNAL(started()),
          m_FilterPipeline, SLOT(run()));

  // When the PipelineBuilder ends then tell the QThread to stop its event loop
  connect(m_FilterPipeline, SIGNAL(finished() ),
          m_WorkerThread, SLOT(quit()) );

  // When the QThread finishes, tell this object that it has finished.
  connect(m_WorkerThread, SIGNAL(finished()),
          this, SLOT( pipelineComplete() ) );

  // If the use clicks on the "Cancel" button send a message to the PipelineBuilder object
  // We need a Direct Connection so the
  connect(this, SIGNAL(cancelPipeline() ),
          m_FilterPipeline, SLOT (on_CancelWorker() ) , Qt::DirectConnection);

  // Send progress messages from PipelineBuilder to this object for display
  qRegisterMetaType<PipelineMessage>();
  connect(m_FilterPipeline, SIGNAL (firePipelineMessage(PipelineMessage)),
          this, SLOT(addMessage(PipelineMessage) ));



  setWidgetListEnabled(false);
  emit pipelineStarted();
  m_WorkerThread->start();
  m_GoBtn->setText("Cancel");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::pipelineComplete()
{
  // qDebug() << "PipelineBuilderWidget::PipelineBuilder_Finished()" << "\n";
  if (m_hasErrors) {
    displayDialogBox(("Pipeline Errors"),
                     ("Errors occurred during processing.\nPlease check the error table for more information."),
                     QMessageBox::Critical);
  }
  m_GoBtn->setText("Go");
  m_PipelineViewWidget->setEnabled(true);
  setWidgetListEnabled(true);
  this->m_progressBar->setValue(0);
  emit pipelineEnded();
  checkIOFiles();
  m_FilterPipeline->deleteLater();
  this->statusBar()->showMessage(QString("Pipeline Complete"));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::pipelineProgress(int val)
{
  this->m_progressBar->setValue( val );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::addMessage(PipelineMessage msg)
{
  // Create error hyperlink
  QLabel* hyperlinkLabel = createHyperlinkLabel(msg);

  QColor msgColor;
  switch(msg.getMessageType())
  {
    case PipelineMessage::Error:
      m_hasErrors = true;
      msgColor.setRed(255);
      msgColor.setGreen(191);
      msgColor.setBlue(193);
    {
      QBrush msgBrush(msgColor);

      QString msgDesc = (msg.getMessageText());
      int msgCode = msg.getMessageCode();

      int rc = errorTableWidget->rowCount();

      errorTableWidget->insertRow(rc);

      QString msgPrefix = (msg.getMessagePrefix());
      QTableWidgetItem* filterNameWidgetItem = new QTableWidgetItem(msgPrefix);
      filterNameWidgetItem->setTextAlignment(Qt::AlignCenter);
      QTableWidgetItem* descriptionWidgetItem = new QTableWidgetItem(msgDesc);
      QTableWidgetItem* codeWidgetItem = new QTableWidgetItem(QString::number(msgCode));
      codeWidgetItem->setTextAlignment(Qt::AlignCenter);

      filterNameWidgetItem->setBackground(msgBrush);
      descriptionWidgetItem->setBackground(msgBrush);
      codeWidgetItem->setBackground(msgBrush);

      if (hyperlinkLabel == NULL)
      {
        errorTableWidget->setItem(rc, 0, filterNameWidgetItem);
      }
      else
      {
        errorTableWidget->setCellWidget(rc, 0, hyperlinkLabel);
      }
      errorTableWidget->setItem(rc, 1, descriptionWidgetItem);
      errorTableWidget->setItem(rc, 2, codeWidgetItem);
    }
      break;

    case PipelineMessage::Warning:
      m_hasWarnings = true;
      msgColor.setRed(251);
      msgColor.setGreen(254);
      msgColor.setBlue(137);

    {
      QBrush msgBrush(msgColor);

      QString msgName = (msg.getMessagePrefix());
      QString msgDesc = (msg.getMessageText());
      int msgCode = msg.getMessageCode();

      QTableWidget* msgTableWidget = m_PipelineViewWidget->getTableWidget();

      int rc = msgTableWidget->rowCount();

      msgTableWidget->insertRow(rc);

      QTableWidgetItem* filterNameWidgetItem = new QTableWidgetItem(msgName);
      filterNameWidgetItem->setTextAlignment(Qt::AlignCenter);
      QTableWidgetItem* descriptionWidgetItem = new QTableWidgetItem(msgDesc);
      QTableWidgetItem* codeWidgetItem = new QTableWidgetItem(QString::number(msgCode));
      codeWidgetItem->setTextAlignment(Qt::AlignCenter);

      filterNameWidgetItem->setBackground(msgBrush);
      descriptionWidgetItem->setBackground(msgBrush);
      codeWidgetItem->setBackground(msgBrush);

      if (hyperlinkLabel == NULL)
      {
        msgTableWidget->setItem(rc, 0, filterNameWidgetItem);
      }
      else
      {
        msgTableWidget->setCellWidget(rc, 0, hyperlinkLabel);
      }
      msgTableWidget->setItem(rc, 1, descriptionWidgetItem);
      msgTableWidget->setItem(rc, 2, codeWidgetItem);
    }
      break;

    case PipelineMessage::StatusValue:
      this->m_progressBar->setValue(msg.getProgressValue());
      break;
    case PipelineMessage::StatusMessage:
      if(NULL != this->statusBar())
      {
        QString s = (msg.getMessagePrefix());
        s = s.append(" ").append(msg.getMessageText().toLatin1().data());
        this->statusBar()->showMessage(s);
      }
      break;
    case PipelineMessage::StatusMessageAndValue:
      this->m_progressBar->setValue(msg.getProgressValue());
      if(NULL != this->statusBar())
      {
        QString s = (msg.getMessagePrefix());
        s = s.append(" ").append(msg.getMessageText().toLatin1().data());
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
void PipelineBuilderWidget::addProgressMessage(QString message)
{
  if (NULL != this->statusBar()) {
    this->statusBar()->showMessage(message);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::addFavorite(QString favoriteTitle)
{
  //Remove all spaces and illegal characters from favorite name
  favoriteTitle = favoriteTitle.trimmed();
  favoriteTitle = favoriteTitle.remove(" ");
  favoriteTitle = favoriteTitle.remove(QRegExp("[^a-zA-Z_\\d\\s]"));


#if defined (Q_OS_MAC)
  QSettings prefs(QSettings::NativeFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
  QString extension = ".ini";
#else
  QSettings prefs(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
  QString extension = ".ini";
#endif

  QString prefFile = prefs.fileName();
  QFileInfo prefFileInfo = QFileInfo(prefFile);
  QString parentPath = prefFileInfo.path();
  QDir parentPathDir = QDir(parentPath);

  if(parentPathDir.mkpath(parentPath))
  {
    QString newParentPrefPath = parentPath + "/DREAM3D_Favorites";
    QString newPrefPath = newParentPrefPath + "/" + favoriteTitle + extension;

    newPrefPath = QDir::toNativeSeparators(newPrefPath);
    newParentPrefPath = QDir::toNativeSeparators(newParentPrefPath);

    QDir newParentPrefPathDir = QDir(newParentPrefPath);

    if(newParentPrefPathDir.mkpath(newParentPrefPath))
    {
      QSettings newPrefs(newPrefPath, QSettings::IniFormat);
      newPrefs.beginGroup(Detail::PipelineBuilderGroup);
      newPrefs.setValue("Name", favoriteTitle);
      newPrefs.endGroup();
      writeSettings(newPrefs, m_PipelineViewWidget);
    }
  }

  QList<QTreeWidgetItem*> items = m_favorites->takeChildren();
  int count = items.count();
  for(int i = 0; i < count; ++i)
  {
    delete items.at(i);
  }
  filterLibraryTree->blockSignals(true);
  readFavoritePipelines();
  filterLibraryTree->blockSignals(false);
  // Tell everyone to save their preferences NOW instead of waiting until the app quits
  emit fireWriteSettings();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::actionAddFavorite_triggered() {
  AddFavoriteWidget* addfavoriteDialog = new AddFavoriteWidget(this);
  addfavoriteDialog->exec();

  if(addfavoriteDialog->getBtnClicked())
  {

    QString favoriteTitle = addfavoriteDialog->getFavoriteName();
    addFavorite(favoriteTitle);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::actionUpdateFavorite_triggered()
{
  // Lets get the name of the favorite
  QTreeWidgetItem* item = filterLibraryTree->currentItem();
  QTreeWidgetItem* parent = filterLibraryTree->currentItem()->parent();

  QString filePath = item->data(0, Qt::UserRole).toString();
  QFileInfo filePathInfo = QFileInfo(filePath);
  // QString fileParentPath = filePathInfo.path();
  QString fileName = filePathInfo.baseName();


  if (NULL != parent && parent->text(0).compare(Detail::FavoritePipelines) == 0 )
  {
    removeFavorite(item);
    item = NULL;
    addFavorite(fileName);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::removeFavorite(QTreeWidgetItem* item)
{
  QString filePath = item->data(0, Qt::UserRole).toString();
  QFileInfo filePathInfo = QFileInfo(filePath);
  QString fileParentPath = filePathInfo.path();
  QString fileName = filePathInfo.fileName();
  QDir fileParentPathDir = QDir(fileParentPath);

  //Remove favorite's pref file
  fileParentPathDir.remove(fileName);

  //Remove favorite, graphically, from the DREAM3D interface
  filterLibraryTree->removeItemWidget(item, 0);
  delete item;

  // Write these changes out to the preferences file
  emit fireWriteSettings();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::actionRemoveFavorite_triggered()
{
  QTreeWidgetItem* item = filterLibraryTree->currentItem();
  QTreeWidgetItem* parent = filterLibraryTree->currentItem()->parent();

  QMessageBox msgBox;
  msgBox.setText("Are you sure that you want to remove \"" + item->text(0) + "\"?");
  msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
  msgBox.setDefaultButton(QMessageBox::Yes);
  int ret = msgBox.exec();

  if (NULL != parent && parent->text(0).compare(Detail::FavoritePipelines) == 0 && ret == QMessageBox::Yes)
  {
    removeFavorite(item);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::actionRenameFavorite_triggered()
{
  QTreeWidgetItem* item = filterLibraryTree->currentItem();

  if (NULL != item->parent() && item->parent()->text(0).compare(Detail::FavoritePipelines) == 0)
  {
    filterLibraryTree->editItem(item, 0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::actionAppendFavorite_triggered()
{
  QTreeWidgetItem* item = filterLibraryTree->currentItem();

  QString pipelinePath = item->data(0, Qt::UserRole).toString();
  if (pipelinePath.isEmpty() == false)
  {
    QFileInfo fi(pipelinePath);
    if (fi.exists() == false) { return; }
    QSettings prefs(pipelinePath, QSettings::IniFormat);
    readSettings(prefs, m_PipelineViewWidget, false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::actionShowInFileSystem_triggered()
{
  QTreeWidgetItem* item = filterLibraryTree->currentItem();
  QString pipelinePath = item->data(0, Qt::UserRole).toString();

  QFileInfo pipelinePathInfo(pipelinePath);
  QString pipelinePathDir = pipelinePathInfo.path();

  QString s("file://");
#if defined(Q_OS_WIN)
  s = s + "/"; // Need the third slash on windows because file paths start with a drive letter
#elif defined(Q_OS_MAC)

#else
  // We are on Linux - I think

#endif
  s = s + pipelinePathDir;
  QDesktopServices::openUrl(s);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::loadPipelineFileIntoPipelineView(QString path)
{
  QFileInfo fi(path);
  if (fi.exists() == false) { return; }
  QSettings prefs(path, QSettings::IniFormat);
  readSettings(prefs, m_PipelineViewWidget, true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList PipelineBuilderWidget::generateFilterListFromPipelineFile(QString path)
{

  QStringList filterNames;
  QSettings prefs(path, QSettings::IniFormat);

  prefs.beginGroup(Detail::PipelineBuilderGroup);
  bool ok = false;
  int filterCount = prefs.value("Number_Filters").toInt(&ok);
  prefs.endGroup();
  if (false == ok) {filterCount = 0;}
  for (int i = 0; i < filterCount; ++i)
  {
    QString gName = QString::number(i);
    prefs.beginGroup(gName);
    QString filterName = prefs.value("Filter_Name", "").toString();
    filterNames.push_back(filterName);
    prefs.endGroup();
  }
  return filterNames;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::populateFilterList(QStringList filterNames)
{
  FilterWidgetManager::Pointer fm = FilterWidgetManager::Instance();

  // Clear all the current items from the list
  filterList->clear();
  filterList->setSortingEnabled(false);

  for(int i = 0; i < filterNames.size(); ++i)
  {
    QString filterName = filterNames[i];
    IFilterWidgetFactory::Pointer wigFactory = fm->getFactoryForFilter(filterName);
    if (NULL == wigFactory.get() )
    {
      continue;
    }
    QString humanName = (wigFactory->getFilterHumanLabel());
    QString iconName(":/");
    iconName.append( (wigFactory->getFilterGroup()));
    iconName.append("_Icon.png");

    // Validate the icon is in the resource system
    QFileInfo iconInfo(iconName);
    if (iconInfo.exists() == false)
    {
      iconName = ":/Plugin_Icon.png"; // Switch to our generic icon for Plugins that do not provide their own
    }

    QIcon icon(iconName);
    // Create the QListWidgetItem and add it to the filterList
    QListWidgetItem* filterItem = new QListWidgetItem(icon, humanName, filterList);
    // Set an "internal" QString that is the name of the filter. We need this value
    // when the item is clicked in order to retreive the Filter Widget from the
    // filter widget manager.
    filterItem->setData( Qt::UserRole, filterName);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QLabel* PipelineBuilderWidget::createHyperlinkLabel(PipelineMessage msg)
{
  QString filterClassName = (msg.getFilterClassName() );
  QString filterHumanLabel = (msg.getFilterHumanLabel() );
  QString msgPrefix = (msg.getMessagePrefix());

  if ( filterClassName.isEmpty() || filterHumanLabel.isEmpty() )
  {
    return NULL;
  }

  QUrl filterURL = DREAM3DHelpUrlGenerator::generateHTMLUrl( filterClassName.toLower() );
  QString filterHTMLText("<a href=\"");
  filterHTMLText.append(filterURL.toString()).append("\">").append(filterHumanLabel).append("</a>");

  QLabel* hyperlinkLabel = new QLabel(filterHTMLText);
  hyperlinkLabel->setTextFormat(Qt::RichText);
  hyperlinkLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
  hyperlinkLabel->setOpenExternalLinks(true);

  return hyperlinkLabel;
}
