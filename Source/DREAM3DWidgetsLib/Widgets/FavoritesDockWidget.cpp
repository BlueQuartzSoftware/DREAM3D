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
#include <QtCore/QFileInfoList>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QUrl>
#include <QtCore/QDebug>

#include <QtWidgets/QAction>
#include <QtWidgets/QMessageBox>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QMenu>

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/FilterFactory.hpp"
#include "DREAM3DLib/FilterParameters/JsonFilterParametersReader.h"

#include "DREAM3DWidgetsLib/Widgets/FilterListDockWidget.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FavoritesDockWidget::FavoritesDockWidget(QWidget* parent) :
  QDockWidget(parent),
  m_DeleteAction(NULL)
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

  QString css(" QToolTip {\
              border: 2px solid #434343;\
      padding: 2px;\
  border-radius: 3px;\
opacity: 255;\
  background-color: #FFFFFF;\
}");
  filterLibraryTree->setStyleSheet(css);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FavoritesDockWidget::configureFilterLibraryTree()
{
  FilterLibraryTreeWidget* filterLibraryTree = getFilterLibraryTreeWidget();
  if(filterLibraryTree)
  {

    QItemSelectionModel* selectionModel = filterLibraryTree->selectionModel();
    QModelIndex index = selectionModel->model()->index(0, 0);
    filterLibraryTree->setCurrentIndex(index);
  }
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
  fileExtension.append("*.json");
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
      //  qDebug() << fi.absoluteFilePath() << "\n";
      // Add a tree widget item for this  Group
      //  qDebug() << fi.absoluteFilePath();
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
    QString itemName;
    if (itemInfo.suffix().compare("ini") == 0 || itemInfo.suffix().compare("txt") == 0)
    {
      QSettings itemPref(itemFilePath, QSettings::IniFormat);
      itemPref.beginGroup(DREAM3D::Settings::PipelineBuilderGroup);
      itemName = itemPref.value(DREAM3D::Settings::PipelineName).toString();
      itemPref.endGroup();
    }
    else if (itemInfo.suffix().compare("json") == 0)
    {
      itemName = JsonFilterParametersReader::ReadNameOfPipelineFromFile(itemFilePath);
    }
    // Add tree widget for this Prebuilt Pipeline
    QTreeWidgetItem* itemWidget = new QTreeWidgetItem(currentDirItem, itemType);
    itemWidget->setText(0, itemName);
    itemWidget->setIcon(0, QIcon(iconFileName));
    itemWidget->setData(0, Qt::UserRole, QVariant(itemInfo.absoluteFilePath()));
    if(allowEditing == true)
    {
      itemWidget->setFlags(itemWidget->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled);
    }
    QString htmlFormattedString = generateHtmlFilterListFromPipelineFile(itemInfo.absoluteFilePath());
    itemWidget->setToolTip(0, htmlFormattedString);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FavoritesDockWidget::generateHtmlFilterListFromPipelineFile(QString path)
{
  QSettings prefs(path, QSettings::IniFormat);

  prefs.beginGroup(DREAM3D::Settings::PipelineBuilderGroup);
  bool ok = false;
  int filterCount = prefs.value("Number_Filters").toInt(&ok);
  QString name = prefs.value("Name").toString();
  QString dVers = prefs.value("DREAM3D_Version").toString();
  if(dVers.isEmpty() == true)
  {
    dVers = prefs.value("Version").toString();
  }
  prefs.endGroup();
  if (false == ok) {filterCount = 0;}

  QString html;
  QTextStream ss(&html);
  ss << "<html><head></head>\n";
  ss << "<body>\n";

  // A table for the summary items
  ss << "<table cellpadding=\"2\" cellspacing=\"0\" border=\"0\">\n";
  ss << "<tbody>\n";
  ss << "	<tr><th align=\"right\">Pipeline Name:</th><td>" << name << "</td></tr>\n";
  ss << "	<tr><th align=\"right\">Filter Count:</th><td>" << filterCount << "</td></tr>\n";
  ss << "	<tr><th align=\"right\">DREAM3D Version:</th><td>" << dVers << "</td></tr>\n";
  ss << "</tbody>\n";
  ss << "</table>\n";
  ss << "<p></p>\n";

  // Start the table of the Pipeline
  ss << "<table cellpadding=\"2\" cellspacing=\"0\" border=\"0\" width=\"300px\">\n";
  ss << "<tbody>\n";
  ss << "<tr bgcolor=\"#A2E99C\"><th>Index</th><th>Filter Group</th><th>Filter Name</th></tr>\n";

  FilterManager* filtManager = FilterManager::Instance();
  char rowColor = 0;
  QString red("#FFAAAA");
  QString odd("#FFFFFF");
  QString even("#B0E4FF");
  QString color = odd;
  QString unknownFilter("Unknown");
  bool unknownFilters = false;

  for (int i = 0; i < filterCount; ++i)
  {
    if (rowColor == 0) { rowColor = 1; color = odd; }
    else { rowColor = 0; color = even; }
    QString gName = QString::number(i);
    prefs.beginGroup(gName);
    QString item = prefs.value("Filter_Name", "").toString();
    prefs.endGroup();

    IFilterFactory::Pointer factory = filtManager->getFactoryForFilter(item);
    if(factory.get() != NULL)
    {
      AbstractFilter::Pointer filter = factory->create();
      if(NULL != filter.get())
      {
        AbstractFilter::Pointer filter = factory->create();
        ss << "<tr bgcolor=\"" << color << "\"><td>" << i << "</td><td>" << filter->getGroupName() << "</td><td>" << item << "</td></tr>\n";
      }
    }
    else
    {
      color = red;
      ss << "<tr bgcolor=\"" << color << "\"><td>" << i << "</td><td>" << unknownFilter << "</td><td>" << item << "</td></tr>\n";
      unknownFilters = true;
    }
  }


  if(unknownFilters)
  {
    color = red;
    ss << "<tr bgcolor=\"" << color << "\"><th colspan=\"3\">There are filters in the pipeline that the currently running version of DREAM3D does not know about. This ";
    ss << "can happen if you are missing plugins that contain the filters or if the pipeline was created in a prior version ";
    ss << "of DREAM3D in which case those filters may have been renamed. Please consult the DREAM3D documentation for more details ";
    ss << "or ask the individual who gave you the pipeline file for more details.</th></tr>\n";
  }
  ss << "</tbody></table>\n";

  ss << "</body></html>";

  return html;
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
void FavoritesDockWidget::on_filterLibraryTree_itemClicked( QTreeWidgetItem* item, int column )
{
#if 0
  QString favoritePath = item->data(0, Qt::UserRole).toString();
  QStringList filterList = generateFilterListFromPipelineFile(favoritePath);
  if(filterList.size() > 0)
  {
    emit filterListGenerated(filterList, false);
  }
#endif
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
    emit pipelineFileActivated(pipelinePath, 0);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FavoritesDockWidget::on_filterLibraryTree_itemChanged(QTreeWidgetItem* item, int column)
{
#if 0
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
#endif
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FavoritesDockWidget::on_filterLibraryTree_currentItemChanged(QTreeWidgetItem* item, QTreeWidgetItem* previous )
{
  if (m_DeleteAction != NULL)
  {
    if (item == NULL)
    {
      m_DeleteAction->setVisible(false);
    }
    else if (item->type() == FilterLibraryTreeWidget::Node_Item_Type)
    {
      m_DeleteAction->setText("Delete Favorite Folder");
      m_DeleteAction->setVisible(true);
    }
    else
    {
      m_DeleteAction->setText("Delete Favorite");
      m_DeleteAction->setVisible(true);
    }
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
    Q_UNUSED(reply);
  }

  filterLibraryTree->blockSignals(true);
  item->setData(0, Qt::UserRole, QVariant(newPath));
  filterLibraryTree->blockSignals(false);

  return newPath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FavoritesDockWidget::actionAddFavoriteFolder_triggered()
{
  addFavorite(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FavoritesDockWidget::actionAddFavorite_triggered()
{
  addFavorite(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QTreeWidgetItem* FavoritesDockWidget::getSelectedParentTreeItem()
{
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

  return selection;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FavoritesDockWidget::addFavorite(bool folder)
{
  QString favoriteTitle = displayNewFavoriteDialog();

  if(favoriteTitle.isEmpty() == true)
  {
    return;
  }

  QTreeWidgetItem* selection = getSelectedParentTreeItem();

  bool allowEditing = true;

  QString newPrefPath;
  QIcon icon;
  FilterLibraryTreeWidget::ItemType itemType = FilterLibraryTreeWidget::Unknown_Item_Type;
  if(folder)
  {
    itemType = FilterLibraryTreeWidget::Node_Item_Type;
    icon = QIcon(":/folder_blue.png");
    QString parentPath = selection->data(0, Qt::UserRole).toString();
    newPrefPath = parentPath + QDir::separator() + favoriteTitle + QDir::separator(); // Generate the proper path to the favorite
    QDir dir(parentPath); // Get the QDir for the parent path
    bool created = dir.mkpath(favoriteTitle); // Try to actually create the folder
    if (false == created)
    {
      QMessageBox::StandardButton reply;
      reply = QMessageBox::critical(this, QObject::tr("Folder Creation Error"),
                                    QObject::tr("Error creating folder '%1'.").arg(parentPath + "/" + favoriteTitle),
                                    QMessageBox::Ok);
      Q_UNUSED(reply);
      return; // Bail out now instead of adding it to the tree
    }

  }
  else
  {
    itemType = FilterLibraryTreeWidget::Leaf_Item_Type;
    icon = QIcon(":/text.png");
    QString path = selection->data(0, Qt::UserRole).toString(); // Get the selected item's path
    newPrefPath = path + QDir::separator() + favoriteTitle + ".ini"; // Generate the proper path to the favorite
  }

  addFavoriteTreeItem(selection, favoriteTitle, icon, itemType, newPrefPath, allowEditing);

  if(false == folder)
  {
    // Signal out to the PipelineViewWidget to save the current pipeline to the path & filename
    emit pipelineNeedsToBeSaved(newPrefPath, favoriteTitle);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FavoritesDockWidget::displayNewFavoriteDialog()
{
  QString favoriteTitle;
  QTreeWidgetItem* parentItem = getSelectedParentTreeItem();
  QString parentPath = parentItem->data(0, Qt::UserRole).toString(); // Get the selected item's path

  AddFavoriteWidget* addfavoriteDialog = new AddFavoriteWidget("Name of Favorite", parentPath, this);

  connect(addfavoriteDialog, SIGNAL(fireRemoveExistingFavorite(QString)), this, SLOT(removeFavorite(QString)));

    addfavoriteDialog->exec(); // Show the dialog
    if(addfavoriteDialog->getBtnClicked()) // the user clicked the OK button, now check what they typed
    {
      favoriteTitle = addfavoriteDialog->getFavoriteName();
    }
    else
    {
      favoriteTitle = "";
    }

  return favoriteTitle;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FavoritesDockWidget::addFavoriteTreeItem(QTreeWidgetItem* selection,
                                              QString& favoriteTitle,
                                              QIcon icon,
                                              FilterLibraryTreeWidget::ItemType itemType,
                                              QString favoritePath,
                                              bool allowEditing)
{

  filterLibraryTree->blockSignals(true);
  // Add a new Item to the Tree
  QTreeWidgetItem* itemWidget = new QTreeWidgetItem(selection, itemType);
  itemWidget->setText(0, favoriteTitle);
  itemWidget->setIcon(0, icon);
  itemWidget->setData(0, Qt::UserRole, QVariant(favoritePath));
  if(allowEditing == true)
  {
    itemWidget->setFlags(itemWidget->flags() | Qt::ItemIsEditable);
  }
  filterLibraryTree->sortItems(0, Qt::AscendingOrder);
  filterLibraryTree->blockSignals(false);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FavoritesDockWidget::actionUpdateFavorite_triggered()
{
  // Lets get the name of the favorite
  QTreeWidgetItem* item = filterLibraryTree->currentItem();

  QString name = item->text(0);
  QString filePath = item->data(0, Qt::UserRole).toString();

  emit pipelineNeedsToBeSaved(filePath, name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FavoritesDockWidget::removeFavorite(QString favoritePath)
{
  QFileInfo fileInfo(favoritePath);
  QString fileName = fileInfo.baseName();
  //QTreeWidgetItem* item = filterLibraryTree->findChild<QTreeWidgetItem*>(fileName);
  QList<QTreeWidgetItem*> list = filterLibraryTree->findItems(fileName, Qt::MatchFixedString);
  removeFavorite(list.first());
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
    if (item->type() == FilterLibraryTreeWidget::Node_Item_Type)
    {
      bool didRemove = removeDir(filePath);
      if(didRemove == false)
      {
        QMessageBox::warning ( this, QString::fromLatin1("Pipeline Save Error"),
                               QString::fromLatin1("There was an error removing the existing Pipeline folder.") );
        return;
      }
    }
    else
    {
      bool didRemove = file.remove();
      if(didRemove == false)
      {
        QMessageBox::warning ( this, QString::fromLatin1("Pipeline Save Error"),
                               QString::fromLatin1("There was an error removing the existing Pipeline file.") );
        return;
      }
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
bool FavoritesDockWidget::removeDir(const QString& dirName)
{
  bool result = true;
  QDir dir(dirName);

  if (dir.exists(dirName))
  {
    Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst))
    {
      if (info.isDir())
      {
        result = removeDir(info.absoluteFilePath());
      }
      else
      {
        result = QFile::remove(info.absoluteFilePath());
      }

      if (!result)
      {
        return result;
      }
    }
    result = dir.rmdir(dirName);
  }

  return result;
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
