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

#include "QtSupportLib/BookmarkMissingDialog.h"

#include "DREAM3DWidgetsLib/Widgets/FilterListDockWidget.h"
#include "DREAM3DWidgetsLib/Widgets/BookmarksModel.h"

enum ErrorCodes {
  UNRECOGNIZED_EXT = -1
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksDockWidget::BookmarksDockWidget(QWidget* parent) :
  QDockWidget(parent),
  m_RenameAction(NULL),
  m_DeleteAction(NULL),
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
  QString css(" QToolTip {\
              border: 2px solid #434343;\
      padding: 2px;\
  border-radius: 3px;\
opacity: 255;\
  background-color: #FFFFFF;\
}");
  bookmarksTreeView->setStyleSheet(css);

  connect(bookmarksTreeView, SIGNAL(itemWasDropped(QModelIndex, QString&, QIcon, QString, bool, bool, bool)), this, SLOT(addTreeItem(QModelIndex, QString&, QIcon, QString, bool, bool, bool)));

  // Resize the bookmarks widget header to contents
  bookmarksTreeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksTreeView* BookmarksDockWidget::getBookmarksTreeView()
{
  return bookmarksTreeView;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QDir BookmarksDockWidget::findV4FavoritesDirectory()
{
  // Get the location of the Preferences file. The Favorites are stored in a directory located at that level.
  QString dirName("DREAM3D_Favorites");

  DREAM3DSettings prefs;
  QString prefFile = prefs.fileName();

  QFileInfo prefFileInfo = QFileInfo(prefFile);
  QString parentPath = prefFileInfo.path();
  QString favPath = parentPath + QDir::separator() + dirName;
  QDir pipelinesDir = QDir(favPath);
  return pipelinesDir;
}

//// -----------------------------------------------------------------------------
////
//// -----------------------------------------------------------------------------
//void BookmarksDockWidget::convertPipelines(QString newDirectory)
//{
//  QDir pipelinesDir = findPipelinesDirectory();
//
//  FilterLibraryTreeWidget::ItemType itemType = FilterLibraryTreeWidget::Leaf_Item_Type;
//  QString iconFileName(":/text.png");
//  bool allowEditing = true;
//  QStringList fileExtension;
//  fileExtension.append("*.txt");
//  fileExtension.append("*.ini");
//  fileExtension.append("*.json");
//
//  // Now block signals and load up all the pipelines in the folder
//  bookmarksTreeView->blockSignals(true);
//  //addPipelinesRecursively(pipelinesDir, bookmarksTreeView->invisibleRootItem(), iconFileName, allowEditing, fileExtension, itemType);
//  // Sort the Favorite Tree by name(?)
//  bookmarksTreeView->sortByColumn(Name, Qt::AscendingOrder);
//  bookmarksTreeView->blockSignals(false);
//}

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
    DREAM3DSettings prefs(path);
    prefs.beginGroup(DREAM3D::Settings::PipelineBuilderGroup);
    bool ok = false;
    name = prefs.value("Name", "").toString();
    dVers = prefs.value("DREAM3D_Version", "").toString();
    if(dVers.isEmpty() == true)
    {
      dVers = prefs.value("Version", "").toString();
    }
    prefs.endGroup();
    if (false == ok) {filterCount = 0;}

    pipeline = QFilterParametersReader::ReadPipelineFromFile(path, QSettings::IniFormat, NULL);
    filterCount = pipeline->getFilterContainer().size();
  }
  else if (fi.suffix().compare(".json") == 0)
  {
    pipeline = JsonFilterParametersReader::ReadPipelineFromFile(path, NULL);
    JsonFilterParametersReader::ReadNameOfPipelineFromFile(path, name, dVers, NULL);
    filterCount = pipeline->getFilterContainer().size();
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
void BookmarksDockWidget::on_bookmarksTreeView_clicked(const QModelIndex & index)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::on_bookmarksTreeView_doubleClicked(const QModelIndex & index)
{
  BookmarksModel* model = BookmarksModel::Instance();

  QModelIndex nameIndex = model->index(index.row(), BookmarksItem::Name, index.parent());
  QModelIndex pathIndex = model->index(index.row(), BookmarksItem::Path, index.parent());

  QString pipelinePath = pathIndex.data().toString();
  if (pipelinePath.isEmpty())
  {
    return; // The user double clicked a folder, so don't do anything
  }
  else
  {
    bool itemHasErrors = model->data(pathIndex, Qt::UserRole).value<bool>();
    QString path = pathIndex.data().toString();
    QFileInfo fi(path);
    if (fi.exists() == false)
    {
      bookmarksTreeView->blockSignals(true);
      BookmarkMissingDialog* dialog = new BookmarkMissingDialog(this, Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowTitleHint);
      connect(dialog, SIGNAL(locateBtnPressed()), this, SLOT(m_ActionLocateFile_triggered()));
      dialog->setBookmarkName(nameIndex.data().toString());
      dialog->exec();
      delete dialog;
      bookmarksTreeView->blockSignals(false);
    }
    else
    {
      if (itemHasErrors == true)
      {
        // Set the itemHasError variable, and have the watcher monitor the file again
        model->setData(nameIndex, false, Qt::UserRole);
        model->getFileSystemWatcher()->addPath(pathIndex.data().toString());
      }
      emit pipelineFileActivated(pipelinePath, true);
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::on_bookmarksTreeView_currentIndexChanged(const QModelIndex & current, const QModelIndex & previous)
{
  BookmarksModel* model = BookmarksModel::Instance();
  QString path = model->index(current.row(), BookmarksItem::Path, current.parent()).data().toString();

  if (m_DeleteAction != NULL)
  {
    if (current.isValid() == false)
    {
      m_DeleteAction->setVisible(false);
    }
    else if (path.isEmpty())
    {
      m_DeleteAction->setText("Remove Folder");
      m_DeleteAction->setVisible(true);

      m_RenameAction->setText("Rename Folder");
      m_RenameAction->setVisible(true);
    }
    else
    {
      m_DeleteAction->setText("Remove Bookmark");
      m_DeleteAction->setVisible(true);

      m_RenameAction->setText("Rename Bookmark");
      m_RenameAction->setVisible(true);
    }
  }
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

  bookmarksTreeView->blockSignals(true);
  item->setData(0, Qt::UserRole, QVariant(newPath));
  bookmarksTreeView->blockSignals(false);

  return newPath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::m_ActionLocateFile_triggered()
{
  BookmarksModel* model = BookmarksModel::Instance();
  QModelIndex current = getBookmarksTreeView()->currentIndex();

  QModelIndex nameIndex = model->index(current.row(), BookmarksItem::Name, current.parent());
  QModelIndex pathIndex = model->index(current.row(), BookmarksItem::Path, current.parent());

  QFileInfo fi(pathIndex.data().toString());
  QString restrictions;
  if (fi.completeSuffix() == "json")
  {
    restrictions = "Json File (*.json)";
  }
  else if (fi.completeSuffix() == "dream3d")
  {
    restrictions = "Dream3d File(*.dream3d)";
  }
  else if (fi.completeSuffix() == "txt")
  {
    restrictions = "Text File (*.txt)";
  }
  else
  {
    restrictions = "Ini File (*.ini)";
  }

  QString filePath = QFileDialog::getOpenFileName(this, tr("Locate Pipeline File"),
    pathIndex.data().toString(), tr(restrictions.toStdString().c_str()));
  if (true == filePath.isEmpty()) { return; }

  filePath = QDir::toNativeSeparators(filePath);

  // Set the new path into the item
  model->setData(pathIndex, filePath, Qt::DisplayRole);

  // Change item back to default look and functionality
  model->setData(nameIndex, false, Qt::UserRole);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::m_ActionNewFolder_triggered()
{
  BookmarksModel* model = BookmarksModel::Instance();

  QModelIndex parent = getSelectedParentTreeItem();
  QString parentName = model->index(parent.row(), BookmarksItem::Name, parent.parent()).data().toString();

  QString name = "New Folder";

  addTreeItem(parent, name, QIcon(":/folder_blue.png"), "", true, true, false);
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

  QModelIndex parent = getSelectedParentTreeItem();

  for (int i = 0; i < newPrefPaths.size(); i++)
  {
    QString newPrefPath = newPrefPaths[i];
    newPrefPath = QDir::toNativeSeparators(newPrefPath);
    QFileInfo fi(newPrefPath);
    QString fileTitle = fi.baseName();
    int err = addTreeItem(parent, fileTitle, QIcon(":/text.png"), newPrefPath, true, false, false);
    if (err >= 0)
    {
      emit updateStatusBar("The pipeline '" + fileTitle + "' has been added successfully.");
      bookmarksTreeView->expand(parent);
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

  if (newPrefPaths.size() > 0)
  {
    // Cache the directory from the last path added
    m_OpenDialogLastDirectory = newPrefPaths[newPrefPaths.size() - 1];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex BookmarksDockWidget::getSelectedParentTreeItem()
{
  BookmarksModel* model = BookmarksModel::Instance();
  QModelIndex index = bookmarksTreeView->currentIndex();

  // Sanity check to make sure we actually have selected a folder to add a favorite into the tree. If the user has
  // selected an actual favorite item, get it's parent which MUST be a folder
  if(index.isValid() && model->index(index.row(), BookmarksItem::Path, index.parent()).data().toString().isEmpty() == false)
  {
    index = index.parent();
  }

  // Make sure we have a valid selection otherwise select the root item
  if (index.isValid() == false)
  {
    index = QModelIndex();
  }

  return index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int BookmarksDockWidget::addTreeItem(QModelIndex parent,
  QString& favoriteTitle,
  QIcon icon,
  QString favoritePath,
  bool allowEditing,
  bool editState,
  bool isExpanded)
{

  favoritePath = QDir::toNativeSeparators(favoritePath);

  QFileInfo fileInfo(favoritePath);
  QString ext = fileInfo.completeSuffix();
  if (fileInfo.isFile() && ext != "dream3d" && ext != "json" && ext != "ini" && ext != "txt")
  {
    return UNRECOGNIZED_EXT;
  }

  BookmarksModel* model = BookmarksModel::Instance();

  bookmarksTreeView->blockSignals(true);
  // Add a new Item to the Tree
  int rowPos = model->rowCount(parent);
  model->insertRow(rowPos, parent);
  QModelIndex nameIndex = model->index(rowPos, BookmarksItem::Name, parent);
  model->setData(nameIndex, favoriteTitle, Qt::DisplayRole);
  QModelIndex pathIndex = model->index(rowPos, BookmarksItem::Path, parent);
  model->setData(pathIndex, favoritePath, Qt::DisplayRole);
  model->setData(nameIndex, icon, Qt::DecorationRole);

  bookmarksTreeView->sortByColumn(BookmarksItem::Name, Qt::AscendingOrder);
  bookmarksTreeView->blockSignals(false);

  if (favoritePath.isEmpty())
  {
    // This is a node
    bookmarksTreeView->blockSignals(true);
    if (isExpanded)
    {
      bookmarksTreeView->expand(nameIndex);
      bookmarksTreeView->setExpanded(nameIndex, true);
      bookmarksTreeView->setExpanded(pathIndex, true);
    }
    else
    {
      bookmarksTreeView->setExpanded(nameIndex, false);
      bookmarksTreeView->setExpanded(pathIndex, false);
    }
    bookmarksTreeView->blockSignals(false);
    if (editState == true)
    {
      bookmarksTreeView->edit(nameIndex);
    }
  }
  else
  {
    model->getFileSystemWatcher()->addPath(favoritePath);
  }

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::m_ActionUpdatePipeline_triggered()
{
  BookmarksModel* model = BookmarksModel::Instance();

  // Lets get the name of the favorite
  QModelIndex index = bookmarksTreeView->currentIndex();

  QString name = model->index(index.row(), BookmarksItem::Name, index.parent()).data().toString();
  QString filePath = model->index(index.row(), BookmarksItem::Path, index.parent()).data().toString();

  emit pipelineNeedsToBeSaved(filePath, name);
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
  BookmarksModel* model = BookmarksModel::Instance();

  QModelIndex index = bookmarksTreeView->currentIndex();
  QString name = model->index(index.row(), BookmarksItem::Name, index.parent()).data().toString();
  QString path = model->index(index.row(), BookmarksItem::Path, index.parent()).data().toString();

  QMessageBox msgBox;
  msgBox.setText("Are you sure that you want to remove \"" + name + "\"?");
  msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
  msgBox.setDefaultButton(QMessageBox::Yes);
  int ret = msgBox.exec();

  if (ret == QMessageBox::Yes)
  {
    //Remove favorite, graphically, from the DREAM3D interface
    model->removeRow(index.row(), index.parent());

    // Write these changes out to the preferences file
    emit fireWriteSettings();
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::m_ActionRenamePipeline_triggered()
{
  QModelIndex index = bookmarksTreeView->currentIndex();
  bookmarksTreeView->edit(index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::m_ActionShowInFileSystem_triggered()
{
  BookmarksModel* model = BookmarksModel::Instance();

  QModelIndex index = bookmarksTreeView->currentIndex();
  if(index.isValid())
  {
    QString pipelinePath = model->index(index.row(), BookmarksItem::Path, index.parent()).data().toString();

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
void BookmarksDockWidget::readSettings(QMainWindow* main, DREAM3DSettings& prefs)
{
  main->restoreDockWidget(this);

  bool b = prefs.value(objectName(), false).toBool();
  setHidden(b);

  QByteArray headerState = prefs.value("BookmarksHeaderState", QByteArray()).toByteArray();
  bookmarksTreeView->header()->restoreState(headerState);

  QString path = prefs.fileName();

  BookmarksModel* model = BookmarksModel::Instance();
  if (model->isEmpty())
  {
    model = BookmarksModel::NewInstanceFromFile(path);
  }

  bookmarksTreeView->setModel(model);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::writeSettings()
{
  DREAM3DSettings prefs;

  QString fileName = prefs.fileName();

  writeSettings(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksDockWidget::writeSettings(DREAM3DSettings& prefs)
{
  prefs.setValue(objectName(), isHidden());

  prefs.setValue("BookmarksHeaderState", bookmarksTreeView->header()->saveState());

  QJsonObject modelObj = bookmarksTreeView->toJsonObject();

  prefs.setValue("Bookmarks Model", modelObj);
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



