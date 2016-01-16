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

#include "BookmarksToolboxWidget.h"

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
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QMenu>

#include "Applications/DREAM3D/DREAM3DApplication.h"

#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"
#include "SIMPLib/FilterParameters/QFilterParametersReader.h"

#include "QtSupportLib/BookmarkMissingDialog.h"

#include "DREAM3DWidgetsLib/Widgets/FilterListToolboxWidget.h"
#include "DREAM3DWidgetsLib/Widgets/BookmarksModel.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_BookmarksToolboxWidget.cpp"

enum ErrorCodes
{
  UNRECOGNIZED_EXT = -1
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksToolboxWidget::BookmarksToolboxWidget(QWidget* parent) :
  QWidget(parent),
  m_OpenDialogLastDirectory("")
{
  setupUi(this);
  setupGui();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksToolboxWidget::~BookmarksToolboxWidget()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksToolboxWidget::connectFilterList(FilterListToolboxWidget* filterListWidget)
{
  connect(this, SIGNAL(filterListGenerated(const QStringList&, bool)),
          filterListWidget, SLOT(updateFilterList(const QStringList&, bool) ) );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksToolboxWidget::setupGui()
{
  QString css(" QToolTip {\
              border: 2px solid #434343;\
      padding: 2px;\
  border-radius: 3px;\
opacity: 255;\
  background-color: #FFFFFF;\
}");
  bookmarksTreeView->setStyleSheet(css);

  connect(bookmarksTreeView, SIGNAL(itemWasDropped(QModelIndex, QString&, QIcon, QString, int, bool, bool, bool)), this, SLOT(addTreeItem(QModelIndex, QString&, QIcon, QString, int, bool, bool, bool)));

  bookmarksTreeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksTreeView* BookmarksToolboxWidget::getBookmarksTreeView()
{
  return bookmarksTreeView;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QDir BookmarksToolboxWidget::findV4FavoritesDirectory()
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString BookmarksToolboxWidget::generateHtmlFilterListFromPipelineFile(QString path)
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
    name = prefs.value("Name", QString("")).toString();
    dVers = prefs.value("DREAM3D_Version", QString("")).toString();
    if(dVers.isEmpty() == true)
    {
      dVers = prefs.value("Version", QString("")).toString();
    }
    prefs.endGroup();
    if (false == ok) {filterCount = 0;}

    pipeline = QFilterParametersReader::ReadPipelineFromFile(path, QSettings::IniFormat, NULL);
    filterCount = pipeline->getFilterContainer().size();
  }
  else if (fi.suffix().compare("json") == 0)
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
QStringList BookmarksToolboxWidget::generateFilterListFromPipelineFile(QString path)
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
void BookmarksToolboxWidget::readPrebuiltPipelines()
{
  QDir pipelinesDir = findPipelinesDirectory();
  QString pPath = pipelinesDir.absolutePath();
  BookmarksModel* model = BookmarksModel::Instance();

  FilterLibraryTreeWidget::ItemType itemType = FilterLibraryTreeWidget::Leaf_Item_Type;
  QString iconFileName(":/text.png");
  bool allowEditing = false;
  QStringList fileExtension;
  fileExtension.append("*.txt");
  fileExtension.append("*.ini");
  fileExtension.append("*.json");

  // Add top-level folder and then load up all the pipelines into the folder
  QString dirName = "Prebuilt Pipelines";
  addTreeItem(QModelIndex(), dirName, QIcon(":/folder_blue.png"), QString(), 0, true, false, false);
  QModelIndex index = model->index(0, BookmarksItem::Name, QModelIndex());
  addPipelinesRecursively(pipelinesDir, index, iconFileName, allowEditing, fileExtension, itemType);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksToolboxWidget::addPipelinesRecursively(QDir currentDir, QModelIndex parent, QString iconFileName,
  bool allowEditing, QStringList filters, FilterLibraryTreeWidget::ItemType itemType)
{
  BookmarksModel* model = BookmarksModel::Instance();

  QModelIndex nextIndex;

  // Get a list of all the directories
  QFileInfoList dirList = currentDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
  if (dirList.size() > 0)
  {
    foreach(QFileInfo fi, dirList)
    {
      // At this point we have the first level of directories and we want to do 2 things:
      // 1.Create an entry in the tree widget with this name
      // 2.drop into the directory and look for all the .txt files and add entries for those items.
      //qDebug() << fi.absoluteFilePath() << "\n";
      // Add a tree widget item for this  Group
      //qDebug() << fi.absoluteFilePath();
      int row = model->rowCount(parent);
      QString baseName = fi.baseName();
      addTreeItem(parent, baseName, QIcon(":/folder_blue.png"), QString(), row, true, false, false);
      nextIndex = model->index(row, BookmarksItem::Name, parent);
      addPipelinesRecursively(QDir(fi.absoluteFilePath()), nextIndex, iconFileName, allowEditing, filters, itemType);   // Recursive call
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
      QString dVers;
      JsonFilterParametersReader::ReadNameOfPipelineFromFile(itemFilePath, itemName, dVers, NULL);
    }

    // Add tree widget for this Prebuilt Pipeline
    int row = model->rowCount(parent);
    addTreeItem(parent, itemName, QIcon(iconFileName), itemInfo.absoluteFilePath(), row, true, false, false);
    nextIndex = model->index(row, BookmarksItem::Name, parent);

    QString htmlFormattedString = generateHtmlFilterListFromPipelineFile(itemInfo.absoluteFilePath());
    model->setData(nextIndex, htmlFormattedString, Qt::ToolTipRole);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QDir BookmarksToolboxWidget::findPipelinesDirectory()
{
  QString dirName("PrebuiltPipelines");

  QString appPath = dream3dApp->applicationDirPath();
  QDir pipelinesDir = QDir(appPath);
#if defined(Q_OS_WIN)
  QFileInfo fi(pipelinesDir.absolutePath() + QDir::separator() + dirName);
  if (fi.exists() == false)
  {
    // The help file does not exist at the default location because we are probably running from visual studio.
    // Try up one more directory
    pipelinesDir.cdUp();
  }
#elif defined(Q_OS_MAC)
  if (pipelinesDir.dirName() == "MacOS")
  {
    pipelinesDir.cdUp();
    pipelinesDir.cdUp();
    pipelinesDir.cdUp();
  }
#else
  // We are on Linux - I think
  QFileInfo fi(pipelinesDir.absolutePath() + QDir::separator() + dirName);
  // qDebug() << fi.absolutePath();
  // Look for the "PrebuiltPipelines" directory in the current app directory
  if (fi.exists() == false)
  {
    // Try up one more directory
    pipelinesDir.cdUp();
  }

#endif

  pipelinesDir = pipelinesDir.absolutePath() + QDir::separator() + dirName;
  return pipelinesDir;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksToolboxWidget::on_bookmarksTreeView_clicked(const QModelIndex& index)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksToolboxWidget::on_bookmarksTreeView_doubleClicked(const QModelIndex& index)
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
      connect(dialog, SIGNAL(locateBtnPressed()), dream3dApp, SLOT(on_actionLocateFile_triggered()));
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
        model->getFileSystemWatcher()->addPath(path);
      }
      emit pipelineFileActivated(pipelinePath, true, true);
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString BookmarksToolboxWidget::writeNewFavoriteFilePath(QString newFavoriteTitle, QString favoritePath, QTreeWidgetItem* item)
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
void BookmarksToolboxWidget::addBookmark(const QString& filePath, const QModelIndex& parent)
{
  BookmarksModel* model = BookmarksModel::Instance();
  QFileInfo fi(filePath);
  QString fileTitle = fi.baseName();
  int err = addTreeItem(parent, fileTitle, QIcon(":/text.png"), filePath, model->rowCount(parent), true, false, false);
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int BookmarksToolboxWidget::addTreeItem(QModelIndex parent,
                                     QString& favoriteTitle,
                                     QIcon icon,
                                     QString favoritePath,
                                     int insertIndex,
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
  model->insertRow(insertIndex, parent);
  QModelIndex nameIndex = model->index(insertIndex, BookmarksItem::Name, parent);
  model->setData(nameIndex, favoriteTitle, Qt::DisplayRole);
  QModelIndex pathIndex = model->index(insertIndex, BookmarksItem::Path, parent);
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
bool BookmarksToolboxWidget::removeDir(const QString& dirName)
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
void BookmarksToolboxWidget::readSettings(DREAM3DSettings* prefs)
{
  bool b = prefs->value(objectName(), QVariant(false)).toBool();
  setHidden(b);

  QByteArray headerState = prefs->value("Horizontal Header State", QByteArray());
  bookmarksTreeView->header()->restoreState(headerState);

  BookmarksModel* model = BookmarksModel::Instance();
  if (model->isEmpty())
  {
    // Check the new bookmarks settings file first
    QSharedPointer<DREAM3DSettings> bookmarksPrefs = QSharedPointer<DREAM3DSettings>(new DREAM3DSettings(getBookmarksPrefsPath()));

    if (bookmarksPrefs->contains("Bookmarks"))
    {
      bookmarksPrefs->beginGroup("Bookmarks");
      model = BookmarksModel::NewInstance(bookmarksPrefs.data());
      bookmarksPrefs->endGroup();
    }
    else
    {
      // If no bookmarks were found in the new location, check the old location
      prefs->beginGroup("DockWidgetSettings");
      prefs->beginGroup("Bookmarks Dock Widget");
      model = BookmarksModel::NewInstance(prefs);
      prefs->endGroup();
      prefs->endGroup();
    }
  }

  bookmarksTreeView->setModel(model);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksToolboxWidget::writeSettings()
{
  DREAM3DSettings* prefs = new DREAM3DSettings();
  writeSettings(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksToolboxWidget::writeSettings(DREAM3DSettings* prefs)
{
  prefs->setValue(objectName(), isHidden());

  prefs->setValue("Horizontal Header State", bookmarksTreeView->header()->saveState());

  QSharedPointer<DREAM3DSettings> bookmarksPrefs = QSharedPointer<DREAM3DSettings>(new DREAM3DSettings(getBookmarksPrefsPath()));

  bookmarksPrefs->beginGroup("Bookmarks");

  QJsonObject modelObj = bookmarksTreeView->toJsonObject();
  bookmarksPrefs->setValue("Bookmarks Model", modelObj);

  bookmarksPrefs->endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> BookmarksToolboxWidget::deserializeTreePath(QString treePath)
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
QString BookmarksToolboxWidget::getBookmarksPrefsPath()
{
  DREAM3DSettings prefs;
  QFileInfo fi(prefs.fileName());
  QString parentPath = fi.absolutePath();

  QString appName = QCoreApplication::applicationName();
  if (appName.isEmpty())
  {
    appName = QString("Application");
  }

#if defined (Q_OS_MAC)

  QString domain = QCoreApplication::organizationDomain();
  if (domain.isEmpty())
  {
    domain = QString("Domain");
  }
  QStringList tokens = domain.split(".");
  QStringListIterator iter(tokens);
  iter.toBack();
  domain = QString("");
  while (iter.hasPrevious())
  {
    domain = domain + iter.previous() + QString(".");
  }


  QString bookmarksPrefsPath = parentPath + "/" + domain + appName + "_BookmarksModel.json";
#else
  QString bookmarksPrefsPath = QDir::toNativeSeparators(parentPath + "/" + appName + "_BookmarksModel.json");
#endif

  return bookmarksPrefsPath;
}



