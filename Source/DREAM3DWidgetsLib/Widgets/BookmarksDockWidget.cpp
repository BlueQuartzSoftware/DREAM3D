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

#include "BookmarksDockWidget.h"

#include <QtCore/QFileInfo>
#include <QtCore/QFileInfoList>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QUrl>
#include <QtCore/QDebug>
#include <QtCore/QDirIterator>

#include <QtWidgets/QAction>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QFileDialog>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QMenu>

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/FilterFactory.hpp"
#include "DREAM3DLib/FilterParameters/JsonFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/QFilterParametersReader.h"

#include "QtSupportLib/FavoritesChangedDialog.h"

#include "DREAM3DWidgetsLib/Widgets/FilterListDockWidget.h"

enum ErrorCodes {
  UNRECOGNIZED_EXT = -1
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksDockWidget::BookmarksDockWidget(QWidget* parent) :
  QDockWidget(parent),
  m_DeleteAction(NULL),
  m_RenameAction(NULL),
  m_OpenDialogLastDirectory("")
{
  setupUi(this);
  setupGui();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksDockWidget::~BookmarksDockWidget()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::connectFilterList(FilterListDockWidget* filterListWidget)
{
  connect(this, SIGNAL(filterListGenerated(const QStringList&, bool)),
          filterListWidget, SLOT(updateFilterList(const QStringList&, bool) ) );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::setupGui()
{
  //// Clear out the default stuff
  //filterLibraryTree->clear();

  //QDir pipelinesDir = findPipelinesDirectory();
  //if (pipelinesDir.exists() && pipelinesDir.entryList().isEmpty() == false)
  //{
  //  FavoritesChangedDialog* dialog = new FavoritesChangedDialog(this);
  //  connect(dialog, SIGNAL(exportBtnPressed(QString)), this, SLOT(convertPipelines(QString)));
  //  dialog->exec();
  //}

  QString css(" QToolTip {\
              border: 2px solid #434343;\
      padding: 2px;\
  border-radius: 3px;\
opacity: 255;\
  background-color: #FFFFFF;\
}");
  filterLibraryTree->setStyleSheet(css);

  connect(filterLibraryTree, SIGNAL(itemWasDropped(QTreeWidgetItem*, QString&, QIcon, FilterLibraryTreeWidget::ItemType, QString, bool, bool, bool)), this, SLOT(addTreeItem(QTreeWidgetItem*, QString&, QIcon, FilterLibraryTreeWidget::ItemType, QString, bool, bool, bool)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::configureFilterLibraryTree()
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
FilterLibraryTreeWidget* BookmarksDockWidget::getFilterLibraryTreeWidget()
{
  return filterLibraryTree;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QDir BookmarksDockWidget::findPipelinesDirectory()
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
void BookmarksDockWidget::convertPipelines(QString newDirectory)
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
void BookmarksDockWidget::addPipelinesRecursively(QDir currentDir, QTreeWidgetItem* currentDirItem, QString iconFileName,
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
QString BookmarksDockWidget::generateHtmlFilterListFromPipelineFile(QString path)
{
  int filterCount = 0;
  QString name;
  QString dVers;
  FilterPipeline::Pointer pipeline;

  QFileInfo fi(path);
  if(fi.suffix().compare("ini") == 0)
  {
    QSettings prefs(path, QSettings::IniFormat);
    prefs.beginGroup(DREAM3D::Settings::PipelineBuilderGroup);
    bool ok = false;
    name = prefs.value("Name").toString();
    dVers = prefs.value("DREAM3D_Version").toString();
    if(dVers.isEmpty() == true)
    {
      dVers = prefs.value("Version").toString();
    }
    prefs.endGroup();
    if (false == ok) {filterCount = 0;}

    pipeline = QFilterParametersReader::ReadPipelineFromFile(path, QSettings::IniFormat, NULL);
  }
  else if (fi.suffix().compare(".json") == 0)
  {
    pipeline = JsonFilterParametersReader::ReadPipelineFromFile(path, NULL);
    name = JsonFilterParametersReader::ReadNameOfPipelineFromFile(path, NULL);
  }

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
  if(NULL == pipeline.get())
  {
    ss << "<b>Unkonwn Pipeline File format for file " << path << "</b>";
    ss << "</tbody></table>\n";
    ss << "</body></html>";
    return html;
  }
  // Start the table of the Pipeline
  ss << "<table cellpadding=\"2\" cellspacing=\"0\" border=\"0\" width=\"300px\">\n";
  ss << "<tbody>\n";
  ss << "<tr bgcolor=\"#A2E99C\"><th>Index</th><th>Filter Group</th><th>Filter Name</th></tr>\n";

//  FilterManager* filtManager = FilterManager::Instance();
  char rowColor = 0;
  QString red("#FFAAAA");
  QString odd("#FFFFFF");
  QString even("#B0E4FF");
  QString color = odd;
  bool unknownFilters = false;

  FilterPipeline::FilterContainerType filters = pipeline->getFilterContainer();
  filterCount = filters.size();
  for (int i = 0; i < filterCount; ++i)
  {
    if (rowColor == 0) { rowColor = 1; color = odd; }
    else { rowColor = 0; color = even; }

    AbstractFilter::Pointer filter = filters.at(i);

    if(NULL != filter.get())
    {
      ss << "<tr bgcolor=\"" << color << "\"><td>" << i << "</td><td>" << filter->getGroupName() << "</td><td>" << filter->getHumanLabel() << "</td></tr>\n";
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
QStringList BookmarksDockWidget::generateFilterListFromPipelineFile(QString path)
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
void BookmarksDockWidget::on_filterLibraryTree_itemClicked( QTreeWidgetItem* item, int column )
{
#if 0
  QString favoritePath = item->data(1, Qt::UserRole).toString();
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
void BookmarksDockWidget::on_filterLibraryTree_itemDoubleClicked( QTreeWidgetItem* item, int column )
{
  QString pipelinePath = item->data(1, Qt::UserRole).toString();
  if (item->type() == FilterLibraryTreeWidget::Node_Item_Type)
  {
    return; // The user double clicked a folder, so don't do anything
  }
  if (pipelinePath.isEmpty() == false)
  {
    emit pipelineNeedsToBeCleared();
    emit pipelineFileActivated(pipelinePath, 0);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::on_filterLibraryTree_itemChanged(QTreeWidgetItem* item, int column)
{
  if (column == 0)
  {
    // Set the internal data with the new name
    item->setData(0, Qt::UserRole, item->text(0));

    writeAndUpdateWidget();
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::on_filterLibraryTree_currentItemChanged(QTreeWidgetItem* item, QTreeWidgetItem* previous )
{
  if (m_DeleteAction != NULL)
  {
    if (item == NULL)
    {
      m_DeleteAction->setVisible(false);
    }
    else if (item->type() == FilterLibraryTreeWidget::Node_Item_Type)
    {
      m_DeleteAction->setText("Remove Folder");
      m_DeleteAction->setVisible(true);

      m_RenameAction->setText("Rename Folder");
      m_RenameAction->setVisible(true);
    }
    else
    {
      m_DeleteAction->setText("Remove Pipeline");
      m_DeleteAction->setVisible(true);

      m_RenameAction->setText("Rename Pipeline");
      m_RenameAction->setVisible(true);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksDockWidget::checkFavoriteTitle(QString favoritePath, QString newFavoriteTitle, QTreeWidgetItem* item)
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
bool BookmarksDockWidget::hasIllegalFavoriteName(QString favoritePath, QString newFavoriteTitle, QTreeWidgetItem* item)
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
bool BookmarksDockWidget::hasDuplicateFavorites(QList<QTreeWidgetItem*> favoritesList, QString favoritePath, QString newFavoriteTitle, QTreeWidgetItem* item)
{
  QSettings favoritePrefs(favoritePath, QSettings::IniFormat);
  QString displayText = "";

  for (int i = 0; i < favoritesList.size(); i++)
  {
    QSettings currentItemPrefs(favoritesList[i]->data(1, Qt::UserRole).toString(), QSettings::IniFormat);
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
QString BookmarksDockWidget::writeNewFavoriteFilePath(QString newFavoriteTitle, QString favoritePath, QTreeWidgetItem* item)
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
void BookmarksDockWidget::m_ActionNewFolder_triggered()
{
  QTreeWidgetItem* parent = getSelectedParentTreeItem();
  QString parentName = parent->text(0);
  QString parentTreePath = parent->data(2, Qt::UserRole).toString();

  addFolder("New Folder", parentName, parentTreePath, true);

  // Resize the bookmarks widget header to contents
  filterLibraryTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

  writeAndUpdateWidget();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::m_ActionAddPipeline_triggered()
{
  QString proposedDir = m_OpenDialogLastDirectory;
  QList<QString> newPrefPaths;

  newPrefPaths = QFileDialog::getOpenFileNames(this, tr("Choose Pipeline File(s)"),
    proposedDir, tr("Json File (*.json);;Dream3d File (*.dream3d);;Text File (*.txt);;Ini File (*.ini);;All Files (*.*)"));
  if (true == newPrefPaths.isEmpty()) { return; }

  addPipelines(newPrefPaths);

  // Resize the bookmarks widget header to contents
  filterLibraryTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

  writeAndUpdateWidget();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QTreeWidgetItem* BookmarksDockWidget::getSelectedParentTreeItem()
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
void BookmarksDockWidget::addFolder(QString name, QString parentName, QString parentTreePath, bool editState)
{
  if (parentName.isEmpty() && parentTreePath.isEmpty())
  {
    QTreeWidgetItem* parent = filterLibraryTree->invisibleRootItem();
    addTreeItem(parent, name, QIcon(":/folder_blue.png"), FilterLibraryTreeWidget::Node_Item_Type, "", true, editState, false);
  }

  QList<QTreeWidgetItem*> list = filterLibraryTree->findItems(parentName, Qt::MatchExactly | Qt::MatchRecursive, 0);

  for (int i = 0; i < list.size(); i++)
  {
    QTreeWidgetItem* potentialParent = list[i];
    if (potentialParent->data(2, Qt::UserRole).toString() == parentTreePath)
    {
      addTreeItem(potentialParent, name, QIcon(":/folder_blue.png"), FilterLibraryTreeWidget::Node_Item_Type, "", true, editState, false);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::addPipelines(QList<QString> newPaths)
{
  QTreeWidgetItem* selection = getSelectedParentTreeItem();

  for (int i = 0; i < newPaths.size(); i++)
  {
    QString newPrefPath = newPaths[i];
    newPrefPath = QDir::toNativeSeparators(newPrefPath);
    QFileInfo fi(newPrefPath);
    QString fileTitle = fi.baseName();
    int err = addTreeItem(selection, fileTitle, QIcon(":/text.png"), FilterLibraryTreeWidget::Leaf_Item_Type, newPrefPath, true, false, false);
    if (err >= 0)
    {
      emit updateStatusBar("The pipeline '" + fileTitle + "' has been added successfully.");
      selection->setExpanded(true);
    }
    else if (err == UNRECOGNIZED_EXT)
    {
      emit updateStatusBar("The pipeline '" + fileTitle + "' could not be added, because the pipeline file extension was not recognized.");
    }
    else
    {
      emit updateStatusBar("The pipeline '" + fileTitle + "' could not be added.  An unknown error has occurred.  Please contact the DREAM3D developers.");
    }
  }

  if (newPaths.size() > 0)
  {
    // Cache the directory from the last path added
    m_OpenDialogLastDirectory = newPaths[newPaths.size() - 1];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int BookmarksDockWidget::addTreeItem(QTreeWidgetItem* selection,
  QString& favoriteTitle,
  QIcon icon,
  FilterLibraryTreeWidget::ItemType itemType,
  QString favoritePath,
  bool allowEditing,
  bool editState,
  bool isExpanded)
{

  QFileInfo fileInfo(favoritePath);
  QString ext = fileInfo.completeSuffix();
  if (fileInfo.isFile() && ext != "dream3d" && ext != "json" && ext != "ini" && ext != "txt")
  {
    return UNRECOGNIZED_EXT;
  }

  filterLibraryTree->blockSignals(true);
  // Add a new Item to the Tree
  QTreeWidgetItem* itemWidget = new QTreeWidgetItem(selection, itemType);
  itemWidget->setText(0, favoriteTitle);
  itemWidget->setText(1, favoritePath);
  itemWidget->setIcon(0, icon);
  itemWidget->setFlags(Qt::NoItemFlags);
  itemWidget->setData(0, Qt::UserRole, favoriteTitle);
  itemWidget->setData(1, Qt::UserRole, QVariant(favoritePath));
  itemWidget->setData(2, Qt::UserRole, getTreePathFromItem(itemWidget));
  filterLibraryTree->sortItems(0, Qt::AscendingOrder);
  filterLibraryTree->blockSignals(false);

  if (allowEditing == true)
  {
    filterLibraryTree->blockSignals(true);
    itemWidget->setFlags(Qt::ItemIsEditable);
    filterLibraryTree->blockSignals(false);
  }

  if (itemType == FilterLibraryTreeWidget::Node_Item_Type)
  {
    filterLibraryTree->blockSignals(true);
    itemWidget->setFlags(itemWidget->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
    itemWidget->setExpanded(isExpanded);
    filterLibraryTree->blockSignals(false);
    if (editState == true)
    {
      filterLibraryTree->editItem(itemWidget);
    }
  }
  else if (itemType == FilterLibraryTreeWidget::Leaf_Item_Type)
  {
    filterLibraryTree->blockSignals(true);
    itemWidget->setFlags(itemWidget->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
    filterLibraryTree->blockSignals(false);
  }

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::m_ActionUpdatePipeline_triggered()
{
  // Lets get the name of the favorite
  QTreeWidgetItem* item = filterLibraryTree->currentItem();

  QString name = item->text(0);
  QString filePath = item->data(1, Qt::UserRole).toString();

  emit pipelineNeedsToBeSaved(filePath, name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::removeBookmark(QString bookmarkName, QString bookmarkPath, QString bookmarkTreePath)
{
  QList<QTreeWidgetItem*> list = filterLibraryTree->findItems(bookmarkName, Qt::MatchExactly | Qt::MatchRecursive, 0);

  for (int i = 0; i < list.size(); i++)
  {
    QTreeWidgetItem* item = list[i];
    QString itemName = item->text(0);
    QString itemFilePath = item->data(1, Qt::UserRole).toString();
    QString itemTreePath = item->data(2, Qt::UserRole).toString();

    if (itemName == bookmarkName && itemFilePath == bookmarkPath && itemTreePath == bookmarkTreePath)
    {
      removeBookmark(item);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::removeBookmark(QTreeWidgetItem* item)
{
  //Remove favorite, graphically, from the DREAM3D interface
  filterLibraryTree->removeItemWidget(item, 0);
  filterLibraryTree->removeItemWidget(item, 1);
  delete item;

  // Write these changes out to the preferences file
  emit fireWriteSettings();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::renameBookmark(QString oldName, QString newName, QString filePath, QString treePath)
{
  QList<QTreeWidgetItem*> list = filterLibraryTree->findItems(oldName, Qt::MatchExactly | Qt::MatchRecursive, 0);

  for (int i = 0; i < list.size(); i++)
  {
    QTreeWidgetItem* item = list[i];
    QString itemName = item->text(0);
    QString itemFilePath = item->data(1, Qt::UserRole).toString();
    QString itemTreePath = item->data(2, Qt::UserRole).toString();

    if (itemName == oldName && itemFilePath == filePath && itemTreePath == treePath)
    {
      item->setText(0, newName);
      item->setData(0, Qt::UserRole, newName);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksDockWidget::removeDir(const QString& dirName)
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
void BookmarksDockWidget::m_ActionRemovePipeline_triggered()
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
    QString name = item->data(0, Qt::UserRole).toString();
    QString path = item->data(1, Qt::UserRole).toString();
    QString treePath = item->data(2, Qt::UserRole).toString();

    removeBookmark(item);

    // Resize the bookmarks widget header to contents
    filterLibraryTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    writeAndUpdateWidget();
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::m_ActionRenamePipeline_triggered()
{
  QTreeWidgetItem* item = filterLibraryTree->currentItem();
  filterLibraryTree->editItem(item, 0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::m_ActionAddToPipelineView_triggered()
{
  QTreeWidgetItem* item = filterLibraryTree->currentItem();

  QString pipelinePath = item->data(1, Qt::UserRole).toString();
  if (pipelinePath.isEmpty() == false)
  {
    QFileInfo fi(pipelinePath);
    if (fi.exists() == false) { return; }
    emit pipelineFileActivated(fi.absoluteFilePath(), Append);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::m_ActionShowInFileSystem_triggered()
{
  QTreeWidgetItem* item = filterLibraryTree->currentItem();
  if(item)
  {
    QString pipelinePath = item->data(1, Qt::UserRole).toString();

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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::readSettings()
{
#if defined (Q_OS_MAC)
  QSettings::Format format = QSettings::NativeFormat;
#else
  QSettings::Format format = QSettings::IniFormat;
#endif

    QSettings prefs(format, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());

    QString filePath = prefs.fileName();

    readTreeSettings(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::readTreeSettings(QSettings& prefs)
{
  prefs.beginGroup("DockWidgetSettings");

  bool b = prefs.value(objectName(), false).toBool();
  setHidden(b);

  QByteArray headerState = prefs.value("BookmarksHeaderState").toByteArray();
  filterLibraryTree->header()->restoreState(headerState);

  int size = prefs.beginReadArray("Bookmarks");
  for (int i = 0; i < size; i++)
  {
    prefs.setArrayIndex(i);

    QString title = prefs.value("Title").toString();
    QString path = prefs.value("Path").toString();
    QString parentTreePath = prefs.value("Parent_Tree_Path").toString();
    bool isExpanded = prefs.value("IsExpanded", false).toBool();

    path = QDir::toNativeSeparators(path);

    QTreeWidgetItem* parentItem = getItemFromTreePath(parentTreePath);

    QIcon icon;
    FilterLibraryTreeWidget::ItemType itemType = FilterLibraryTreeWidget::Unknown_Item_Type;
    if (path.isEmpty())
    {
      // This is a folder
      addTreeItem(parentItem, title, QIcon(":/folder_blue.png"), FilterLibraryTreeWidget::Node_Item_Type, path, true, false, isExpanded);
    }
    else
    {
      // This is a pipeline
      addTreeItem(parentItem, title, QIcon(":/text.png"), FilterLibraryTreeWidget::Leaf_Item_Type, path, true, false, isExpanded);
    }
  }
  prefs.endArray();

  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::readSettings(QMainWindow* main, QSettings& prefs)
{
  main->restoreDockWidget(this);

  readTreeSettings(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::writeSettings()
{
#if defined (Q_OS_MAC)
  QSettings::Format format = QSettings::NativeFormat;
#else
  QSettings::Format format = QSettings::IniFormat;
#endif

  QSettings prefs(format, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());

  QString fileName = prefs.fileName();

  writeSettings(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::writeSettings(QSettings& prefs)
{
  prefs.beginGroup("DockWidgetSettings");

  prefs.setValue(objectName(), isHidden());

  prefs.setValue("BookmarksHeaderState", filterLibraryTree->header()->saveState());

  QTreeWidgetItemIterator iter(filterLibraryTree);

  // Delete out old content
  prefs.beginGroup("Bookmarks");
  prefs.remove("");
  prefs.endGroup();

  // Write new content
  prefs.beginWriteArray("Bookmarks");
  int i = 0;
  while (*iter)
  {
    QTreeWidgetItem* currentItem = *iter;

    prefs.setArrayIndex(i);
    prefs.setValue("Title", currentItem->text(0));
    prefs.setValue("Path", currentItem->data(1, Qt::UserRole).toString());
    prefs.setValue("IsExpanded", currentItem->isExpanded());

    if (NULL != currentItem->parent())
    {
      QString treePath = getTreePathFromItem(currentItem->parent());
      prefs.setValue("Parent_Tree_Path", treePath);
    }
    else
    {
      prefs.setValue("Parent_Tree_Path", "");
    }

    ++iter;
    i++;
  }
  prefs.endArray();

  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::updateWidget()
{
  // Clear the tree
  filterLibraryTree->clear();

  // Read Settings
  readSettings();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString BookmarksDockWidget::getTreePathFromItem(QTreeWidgetItem* item)
{
  QTreeWidgetItem* parentItem = item->parent();
  QString treePath = "/" + item->text(0);

  while (NULL != parentItem)
  {
    treePath.prepend(parentItem->text(0));
    treePath.prepend("/");
    parentItem = parentItem->parent();
  }

  return treePath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QTreeWidgetItem* BookmarksDockWidget::getItemFromTreePath(QString treePath)
{
  QList<QString> stringList = deserializeTreePath(treePath);
  
  if (stringList.size() <= 0)
  {
    return filterLibraryTree->invisibleRootItem();
  }

  QList<QTreeWidgetItem*> possibleItems = filterLibraryTree->findItems(stringList.back(), Qt::MatchExactly | Qt::MatchRecursive);
  stringList.pop_back();

  QTreeWidgetItem* possibleItem = NULL;
  for (int i = 0; i < possibleItems.size(); i++)
  {
    possibleItem = possibleItems[i];
    QTreeWidgetItem* parentItem = possibleItem->parent();
    for (int j = stringList.size() - 1; j >= 0; j--)
    {
      QString parentString = stringList[j];
      if (NULL == parentItem || parentItem->text(0) != parentString)
      {
        possibleItem = NULL;
        break;
      }
      parentItem = parentItem->parent();
    }
  }

  return possibleItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> BookmarksDockWidget::deserializeTreePath(QString treePath)
{
  QList<QString> list;
  int currentIndex = 0;
  int spaceIndex = 0;
  QString strPart = "";

  while (spaceIndex >= 0 && treePath.isEmpty() == false)
  {
    spaceIndex = treePath.indexOf('/');
    strPart = treePath.left(spaceIndex);
    strPart = strPart.simplified();
    if (strPart != "")
    {
      list.push_back(strPart);
    }
    treePath = treePath.remove(currentIndex, spaceIndex + 1);
  }

  return list;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::writeAndUpdateWidget()
{
  // Update settings in the file, since this is used by new DREAM3D instances
  writeSettings();

  emit settingsUpdated();
}



