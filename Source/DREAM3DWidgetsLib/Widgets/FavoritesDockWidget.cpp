/* ============================================================================
* Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
* Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "FavoritesDockWidget.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QDebug>

#include <QtGui/QMessageBox>
#include <QtGui/QDesktopServices>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QMenu>

#include "DREAM3DWidgetsLib/Widgets/AddFavoriteWidget.h"
#include "DREAM3DWidgetsLib/Widgets/FilterListDockWidget.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FavoritesDockWidget::FavoritesDockWidget(QWidget* parent) :
  QDockWidget(parent)
{
  setupUi(this);
  setupGui();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FavoritesDockWidget::~FavoritesDockWidget()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FavoritesDockWidget::connectFilterList(FilterListDockWidget* filterListWidget)
{
  connect(this, SIGNAL(filterListGenerated(const QStringList&, bool)),
          filterListWidget, SLOT(updateFilterList(const QStringList&, bool) ) );

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FavoritesDockWidget::setupGui()
{
  // Clear out the default stuff
  filterLibraryTree->clear();

  readPipelines();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterLibraryTreeWidget* FavoritesDockWidget::getFilterLibraryTreeWidget()
{
  return filterLibraryTree;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QDir FavoritesDockWidget::findPipelinesDirectory()
{

  // Get the location of the PReferences file. The Favorites are stored in a directory located at that level.
  QString dirName("DREAM3D_Favorites");
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
  QString favPath = parentPath + QDir::separator() + dirName;
  QDir pipelinesDir = QDir(favPath);
  return pipelinesDir;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FavoritesDockWidget::readPipelines()
{
  QDir pipelinesDir = findPipelinesDirectory();

  FilterLibraryTreeWidget::ItemType itemType = FilterLibraryTreeWidget::Leaf_Item_Type;
  QString iconFileName(":/text.png");
  bool allowEditing = true;
  QStringList fileExtension;
  fileExtension.append("*.txt");
  fileExtension.append("*.ini");

  // Need to add the path to the favorites directory to the root item since we may use this later on.
  filterLibraryTree->invisibleRootItem()->setData(0, Qt::UserRole, QVariant(pipelinesDir.absolutePath()));


  // Now block signals and load up all the pipelines in the folder
  filterLibraryTree->blockSignals(true);
  addPipelinesRecursively(pipelinesDir, filterLibraryTree->invisibleRootItem(), iconFileName, allowEditing, fileExtension, itemType);
  // Sort the Favorite Tree by name(?)
  filterLibraryTree->sortItems(0, Qt::AscendingOrder);
  filterLibraryTree->blockSignals(false);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FavoritesDockWidget::addPipelinesRecursively(QDir currentDir, QTreeWidgetItem* currentDirItem, QString iconFileName,
                                                  bool allowEditing, QStringList filters, FilterLibraryTreeWidget::ItemType itemType)
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
      qDebug() << fi.absoluteFilePath() << "\n";
      // Add a tree widget item for this  Group
      qDebug() << fi.absoluteFilePath();
      nextDirItem = new QTreeWidgetItem(currentDirItem, FilterLibraryTreeWidget::Node_Item_Type);
      nextDirItem->setText(0, fi.baseName());
      nextDirItem->setIcon(0, QIcon(":/folder_blue.png"));
      nextDirItem->setData(0, Qt::UserRole, QVariant(fi.absoluteFilePath() ) );
      addPipelinesRecursively( QDir( fi.absoluteFilePath() ), nextDirItem, iconFileName, allowEditing, filters, itemType );   // Recursive call
    }
  }

  QFileInfoList itemList = currentDir.entryInfoList(filters);
  foreach(QFileInfo itemInfo, itemList)
  {
    QString itemFilePath = itemInfo.absoluteFilePath();
    QSettings itemPref(itemFilePath, QSettings::IniFormat);
    itemPref.beginGroup(DREAM3D::Settings::PipelineBuilderGroup);
    QString itemName = itemPref.value("Name").toString();
    //itemPref.setValue("DREAM3D_Version", QString::fromStdString(DREAM3DLib::Version::Package()));
    itemPref.endGroup();
    //qDebug() << itemInfo.absoluteFilePath() << "\n";
    // Add tree widget for this Prebuilt Pipeline
    QTreeWidgetItem* itemWidget = new QTreeWidgetItem(currentDirItem, itemType);
    itemWidget->setText(0, itemName);
    itemWidget->setIcon(0, QIcon(iconFileName));
    itemWidget->setData(0, Qt::UserRole, QVariant(itemInfo.absoluteFilePath()));
    if(allowEditing == true)
    {
      itemWidget->setFlags(itemWidget->flags() | Qt::ItemIsEditable);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FavoritesDockWidget::on_filterLibraryTree_itemClicked( QTreeWidgetItem* item, int column )
{
  QString favoritePath = item->data(0, Qt::UserRole).toString();
  QStringList filterList = generateFilterListFromPipelineFile(favoritePath);
  emit filterListGenerated(filterList, false);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList FavoritesDockWidget::generateFilterListFromPipelineFile(QString path)
{

  QStringList filterNames;
  QSettings prefs(path, QSettings::IniFormat);

  prefs.beginGroup(DREAM3D::Settings::PipelineBuilderGroup);
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
void FavoritesDockWidget::on_filterLibraryTree_itemChanged(QTreeWidgetItem* item, int column)
{
  if (NULL != item->parent() )
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
    newFavoritePrefs.beginGroup(DREAM3D::Settings::PipelineBuilderGroup);
    newFavoritePrefs.setValue("Name", item->text(0) );
    newFavoritePrefs.endGroup();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FavoritesDockWidget::checkFavoriteTitle(QString favoritePath, QString newFavoriteTitle, QTreeWidgetItem* item)
{
  // Put all children (favorites) in a list
  QList<QTreeWidgetItem*> favoritesList;
  int numOfChildren = item->parent()->childCount();
  for (int i = 0; i < numOfChildren; i++)
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
bool FavoritesDockWidget::hasIllegalFavoriteName(QString favoritePath, QString newFavoriteTitle, QTreeWidgetItem* item)
{
  QSettings favoritePrefs(favoritePath, QSettings::IniFormat);
  QString displayText = "";

  if ( newFavoriteTitle.contains(QRegExp("[^a-zA-Z_-\\d ]")) )
  {
    displayText = "The title that was chosen has illegal characters.\n\nNames can only have:\n\tLetters\n\tNumbers\n\tUnderscores\n\tDashes";
    displayText = displayText + "\n\nNo spaces allowed";

    // Change the GUI back to the old name
    favoritePrefs.beginGroup(DREAM3D::Settings::PipelineBuilderGroup);
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
bool FavoritesDockWidget::hasDuplicateFavorites(QList<QTreeWidgetItem*> favoritesList, QString favoritePath, QString newFavoriteTitle, QTreeWidgetItem* item)
{
  QSettings favoritePrefs(favoritePath, QSettings::IniFormat);
  QString displayText = "";

  for (int i = 0; i < favoritesList.size(); i++)
  {
    QSettings currentItemPrefs(favoritesList[i]->data(0, Qt::UserRole).toString(), QSettings::IniFormat);
    currentItemPrefs.beginGroup(DREAM3D::Settings::PipelineBuilderGroup);

    // If the new title matches one of the other favorite titles
    if (currentItemPrefs.value("Name").toString() == newFavoriteTitle)
    {
      displayText = "A favorite that has this title already exists in the Favorites list.\n\n";

      // Change the GUI back to the old name
      favoritePrefs.beginGroup(DREAM3D::Settings::PipelineBuilderGroup);
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
QString FavoritesDockWidget::writeNewFavoriteFilePath(QString newFavoriteTitle, QString favoritePath, QTreeWidgetItem* item)
{
  QFileInfo original(favoritePath);
  QString newPath = original.canonicalPath() + QDir::separator() + newFavoriteTitle;
  if (!original.completeSuffix().isEmpty())
  { newPath += "." + original.completeSuffix(); }

  newPath = QDir::toNativeSeparators(newPath);

  QFile f(favoritePath);
  bool success = f.rename(newPath);
  if (false == success)
  {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::critical(this, QObject::tr("Error renaming a file"),
                                  QObject::tr("Error renaming file '%1' to '%2'.").arg(favoritePath).arg(newPath),
                                  QMessageBox::Ok);
  }

  filterLibraryTree->blockSignals(true);
  item->setData(0, Qt::UserRole, QVariant(newPath));
  filterLibraryTree->blockSignals(false);

  return newPath;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FavoritesDockWidget::on_filterLibraryTree_currentItemChanged(QTreeWidgetItem* item, QTreeWidgetItem* previous )
{
  on_filterLibraryTree_itemClicked(item, 0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FavoritesDockWidget::on_filterLibraryTree_itemDoubleClicked( QTreeWidgetItem* item, int column )
{
  QString pipelinePath = item->data(0, Qt::UserRole).toString();
  if (item->type() == FilterLibraryTreeWidget::Node_Item_Type)
  {
    return; // The user double clicked a folder, so don't do anything
  }
  if (pipelinePath.isEmpty() == false)
  {
    emit pipelineFileActivated(pipelinePath, QSettings::IniFormat, false);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FavoritesDockWidget::actionAddFavoriteFolder_triggered()
{
  QString favoriteTitle;
  AddFavoriteWidget* addfavoriteDialog = new AddFavoriteWidget("Name of Folder", this);
  bool done = false;
  bool cancel = false;
  while (done == false)
  {
    addfavoriteDialog->exec(); // Show the dialog
    if(addfavoriteDialog->getBtnClicked()) // the user clicked the OK button, now check what they typed
    {
      favoriteTitle = addfavoriteDialog->getFavoriteName();
      if ( favoriteTitle.contains(QRegExp("[^a-zA-Z_-\\d ]")) )
      {
        QString displayText = "The name of the Favorite that was chosen has illegal characters.\n\nNames can only have:\n\tLetters\n\tNumbers\n\tUnderscores\n\tDashes";
        displayText = displayText + "\n\nNo special charaters allowed due to file system restrictions";
        // Display error message
        int reply = QMessageBox::critical(this, tr("Rename Favorite"), tr(displayText.toLatin1().data()),
                                          QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
        if(reply == QMessageBox::Cancel) {
          done = true;
          cancel = true;
        }
      }
      else { done = true; }
    }
    else
    {
      done = true; // The user clicked the cancel button
    }
  }
  if (true == cancel) { return; } // The user cancelled the whole thing, just return

  QTreeWidgetItem* currentDirItem = filterLibraryTree->currentItem();
  QString path = currentDirItem->data(0, Qt::UserRole).toString();
  QFileInfo fi(path);
  // If the user selected a file instead of a folder then get the path of the file, not including the filename
  if(fi.isFile() == true)
  {
    currentDirItem = currentDirItem->parent(); // We need to step up one level to get the parent item which should be a folder
  }

  QString folderPath = path + QDir::separator() + favoriteTitle + QDir::separator();

  QDir dir(path);

  bool created = dir.mkpath(favoriteTitle);
  if (false == created)
  {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::critical(this, QObject::tr("Folder Creation Error"),
                                  QObject::tr("Error creating folder '%1'.").arg(path + "/" + favoriteTitle),
                                  QMessageBox::Ok);
  }

  filterLibraryTree->blockSignals(true);
  QTreeWidgetItem* nextDirItem = new QTreeWidgetItem(currentDirItem, FilterLibraryTreeWidget::Node_Item_Type);
  nextDirItem->setText(0, favoriteTitle);
  nextDirItem->setData(0, Qt::UserRole, QVariant(folderPath) );
  nextDirItem->setIcon(0, QIcon(":/folder_blue.png"));
  filterLibraryTree->blockSignals(false);
  //Nothing to add as this was a brand new folder
  //FIXME: We should figure out how to plae the folder alphabetically, or reload the favorites maybe?

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FavoritesDockWidget::actionAddFavorite_triggered()
{
  QString favoriteTitle;
  AddFavoriteWidget* addfavoriteDialog = new AddFavoriteWidget("Name of Favorite", this);
  addfavoriteDialog->setModal(true);
  bool done = false;
  bool cancel = false;
  while (done == false)
  {
    addfavoriteDialog->exec(); // Show the dialog
    if(addfavoriteDialog->getBtnClicked()) // the user clicked the OK button, now check what they typed
    {
      favoriteTitle = addfavoriteDialog->getFavoriteName();
      if ( favoriteTitle.contains(QRegExp("[^a-zA-Z_-\\d\\s]")) )
      {
        QString displayText = "The name of the Favorite that was chosen has illegal characters.\n\nNames can only have:\n\tLetters\n\tNumbers\n\tUnderscores\n\tDashes";
        displayText = displayText + "\n\nNo special charaters allowed due to file system restrictions";
        // Display error message
        int reply = QMessageBox::critical(this, tr("Rename Favorite"), tr(displayText.toLatin1().data()),
                                          QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
        if(reply == QMessageBox::Cancel) {
          done = true;
          cancel = true;
        }
      }
      else { done = true; }
    }
    else
    {
      done = true; // The user clicked the cancel button
    }
  }
  if (true == cancel) { return; } // The user cancelled the whole thing, just return

  filterLibraryTree->blockSignals(true);
  QTreeWidgetItem* selection = filterLibraryTree->currentItem();

    // Sanity check to make sure we actually have selected a folder to add a favorite into the tree. If the user has
  // selected an actual favorite item, get it's parent which MUST be a folder
  if(NULL != selection && selection->type() == FilterLibraryTreeWidget::Leaf_Item_Type)
  {
    selection = selection->parent();
    // We can _still_ get a null item (for some odd reason) so check that next
  }

  // Make sure we have a valid selection otherwise select the root item
  if (NULL == selection)
  {
    selection = filterLibraryTree->invisibleRootItem();
  }

  bool allowEditing = true;

  QString path = selection->data(0, Qt::UserRole).toString();
  QString newPrefPath = path + QDir::separator() + favoriteTitle + ".ini";

  // Add a new Item to the Tree
  QTreeWidgetItem* itemWidget = new QTreeWidgetItem(selection, FilterLibraryTreeWidget::Leaf_Item_Type);
  itemWidget->setText(0, favoriteTitle);
  itemWidget->setIcon(0, QIcon(":/text.png"));
  itemWidget->setData(0, Qt::UserRole, QVariant(newPrefPath));
  if(allowEditing == true)
  {
    itemWidget->setFlags(itemWidget->flags() | Qt::ItemIsEditable);
  }
  filterLibraryTree->sortItems(0, Qt::AscendingOrder);
  filterLibraryTree->blockSignals(false);
  // Signal out to the PipelineViewWidget to save the current pipeline to the path & filename
  emit pipelineNeedsToBeSaved(newPrefPath, favoriteTitle);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FavoritesDockWidget::actionUpdateFavorite_triggered()
{
  // Lets get the name of the favorite
  QTreeWidgetItem* item = filterLibraryTree->currentItem();
  // QTreeWidgetItem* parent = filterLibraryTree->currentItem()->parent();

  QString name = item->text(0);
  QString filePath = item->data(0, Qt::UserRole).toString();
  //  QFileInfo filePathInfo = QFileInfo(filePath);
  //  QString name = filePathInfo.baseName();

  emit pipelineNeedsToBeSaved(filePath, name);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FavoritesDockWidget::removeFavorite(QTreeWidgetItem* item)
{
  QString filePath = item->data(0, Qt::UserRole).toString();

  QFile file(filePath);

  QFileInfo filePathInfo = QFileInfo(filePath);
  if(filePathInfo.exists() == true)
  {
    bool didRemove = file.remove();
    if(didRemove == false)
    {
      QMessageBox::warning ( this, QString::fromAscii("Pipeline Save Error"),
                             QString::fromAscii("There was an error removing the existing Pipeline file. The pipeline was NOT removed") );
      return;
    }
  }


  //Remove favorite, graphically, from the DREAM3D interface
  filterLibraryTree->removeItemWidget(item, 0);
  delete item;

  // Write these changes out to the preferences file
  emit fireWriteSettings();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FavoritesDockWidget::actionRemoveFavorite_triggered()
{
  QTreeWidgetItem* item = filterLibraryTree->currentItem();
  //QTreeWidgetItem* parent = filterLibraryTree->currentItem()->parent();

  QMessageBox msgBox;
  msgBox.setText("Are you sure that you want to remove \"" + item->text(0) + "\"?");
  msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
  msgBox.setDefaultButton(QMessageBox::Yes);
  int ret = msgBox.exec();

  if (ret == QMessageBox::Yes)
  {
    removeFavorite(item);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FavoritesDockWidget::actionRenameFavorite_triggered()
{
  QTreeWidgetItem* item = filterLibraryTree->currentItem();
  filterLibraryTree->editItem(item, 0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FavoritesDockWidget::actionAppendFavorite_triggered()
{
  QTreeWidgetItem* item = filterLibraryTree->currentItem();

  QString pipelinePath = item->data(0, Qt::UserRole).toString();
  if (pipelinePath.isEmpty() == false)
  {
    QFileInfo fi(pipelinePath);
    if (fi.exists() == false) { return; }
    emit pipelineFileActivated(fi.absoluteFilePath(), QSettings::IniFormat, true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FavoritesDockWidget::actionShowInFileSystem_triggered()
{
  QTreeWidgetItem* item = filterLibraryTree->currentItem();
  if(item)
  {
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
}
