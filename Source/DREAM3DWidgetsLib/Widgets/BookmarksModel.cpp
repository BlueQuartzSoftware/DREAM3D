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

#include <QtWidgets>

#include "DREAM3DWidgetsLib/Widgets/BookmarksModel.h"
#include "DREAM3DWidgetsLib/Widgets/BookmarksTreeView.h"

#include "QtSupportLib/DREAM3DSettings.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_BookmarksModel.cpp"

BookmarksModel* BookmarksModel::self = NULL;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksModel::BookmarksModel(QObject* parent) :
  QAbstractItemModel(parent)
{
  QVector<QVariant> vector;
  vector.push_back("Name");
  vector.push_back("Path");
  rootItem = new BookmarksItem(vector);

  m_Watcher = new QFileSystemWatcher(this);

  connect(this, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(updateModel(const QModelIndex&, const QModelIndex&)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksModel::~BookmarksModel()
{
  delete rootItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksModel* BookmarksModel::Instance()
{
  if (self == NULL)
  {
    self = new BookmarksModel();
  }

  return self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksModel* BookmarksModel::NewInstance(DREAM3DSettings* prefs)
{
  // Erase the old content
  if (self)
  {
    delete self;
    self = NULL;
  }

  QJsonObject modelObj = prefs->value("Bookmarks Model", QJsonObject());
  self = BookmarksTreeView::FromJsonObject(modelObj);

  return self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksModel::setFileSystemWatcher(QFileSystemWatcher* watcher)
{
  m_Watcher = watcher;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFileSystemWatcher* BookmarksModel::getFileSystemWatcher()
{
  return m_Watcher;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksModel::updateRowState(const QString& path)
{
  QFileInfo fi(path);
  if (fi.exists() == false)
  {
    QModelIndexList indexList = findIndexByPath(path);
    for (int i = 0; i < indexList.size(); i++)
    {
      QModelIndex nameIndex = index(indexList[i].row(), BookmarksItem::Name, indexList[i].parent());

      // Set the itemHasError variable
      setData(nameIndex, true, Qt::UserRole);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int BookmarksModel::columnCount(const QModelIndex& parent) const
{
  return rootItem->columnCount();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex BookmarksModel::sibling(int row, int column, const QModelIndex& currentIndex) const
{
  if (currentIndex.column() == BookmarksItem::Name)
  {
    return index(currentIndex.row(), BookmarksItem::Path, currentIndex.parent());
  }
  else
  {
    return index(currentIndex.row(), BookmarksItem::Name, currentIndex.parent());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant BookmarksModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
  {
    return QVariant();
  }

  BookmarksItem* item = getItem(index);

  if (role == Qt::DisplayRole)
  {
    return item->data(index.column());
  }
  else if (role == Qt::UserRole)
  {
    return item->getItemHasErrors();
  }
  else if (role == Qt::BackgroundRole)
  {
    if (item->getItemHasErrors() == true)
    {
      return QColor(235, 110, 110);
    }
    else
    {
      return QVariant();
    }
  }
  else if (role == Qt::ForegroundRole)
  {
    if (item->getItemHasErrors() == true)
    {
      return QColor(240, 240, 240);
    }
    else
    {
      return QColor(Qt::black);
    }
  }
  else if (role == Qt::ToolTipRole)
  {
    QString tooltip = "'" + this->index(index.row(), BookmarksItem::Path, index.parent()).data().toString() + "' was not found on the file system.\nYou can either locate the file or delete the entry from the table.";

    if (item->getItemHasErrors() == true)
    {
      return tooltip;
    }
    else
    {
      return item->getItemTooltip();
    }
  }
  else if (role == Qt::DecorationRole)
  {
    QModelIndex nameIndex = this->index(index.row(), BookmarksItem::Name, index.parent());
    if (nameIndex == index)
    {
      BookmarksItem* item = getItem(index);
      return item->getIcon();
    }
    else
    {
      return QVariant();
    }
  }
  else
  {
    return QVariant();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::ItemFlags BookmarksModel::flags(const QModelIndex& index) const
{
  if (!index.isValid())
  { return 0; }

  Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

  BookmarksItem* item = getItem(index);
  QString name = item->data(BookmarksItem::Name).toString();
  if (item->data(BookmarksItem::Path).toString().isEmpty())
  {
    // This is a node
    return (defaultFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
  }
  else
  {
    // This is a leaf
    return (defaultFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksItem* BookmarksModel::getItem(const QModelIndex& index) const
{
  if (index.isValid())
  {
    BookmarksItem* item = static_cast<BookmarksItem*>(index.internalPointer());
    if (item)
    { return item; }
  }
  return rootItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant BookmarksModel::headerData(int section, Qt::Orientation orientation,
                                    int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
  { return rootItem->data(section); }

  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex BookmarksModel::index(int row, int column, const QModelIndex& parent) const
{
  if (parent.isValid() && parent.column() != 0)
  { return QModelIndex(); }

  BookmarksItem* parentItem = getItem(parent);

  BookmarksItem* childItem = parentItem->child(row);
  if (childItem)
  { return createIndex(row, column, childItem); }
  else
  { return QModelIndex(); }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksModel::insertRows(int position, int rows, const QModelIndex& parent)
{
  BookmarksItem* parentItem = getItem(parent);
  bool success;

  beginInsertRows(parent, position, position + rows - 1);
  success = parentItem->insertChildren(position, rows, rootItem->columnCount());
  endInsertRows();

  return success;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksModel::removeRows(int position, int rows, const QModelIndex& parent)
{
  BookmarksItem* parentItem = getItem(parent);
  bool success = true;

  beginRemoveRows(parent, position, position + rows - 1);
  success = parentItem->removeChildren(position, rows);
  endRemoveRows();

  return success;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksModel::moveRows(const QModelIndex & sourceParent, int sourceRow, int count, const QModelIndex & destinationParent, int destinationChild)
{
  beginMoveRows(sourceParent, sourceRow, sourceRow + count - 1, destinationParent, destinationChild);

  BookmarksItem* srcParentItem = getItem(sourceParent);
  BookmarksItem* destParentItem = getItem(destinationParent);

  for (int i = sourceRow; i < sourceRow + count; i++)
  {
    QModelIndex srcIndex = index(i, BookmarksItem::Name, sourceParent);
    BookmarksItem* srcItem = getItem(srcIndex);

    destParentItem->insertChild(destinationChild, srcItem);
    srcItem->setParent(destParentItem);
    srcParentItem->removeChild(i);
  }

  endMoveRows();

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex BookmarksModel::parent(const QModelIndex& index) const
{
  if (!index.isValid())
  { return QModelIndex(); }

  BookmarksItem* childItem = getItem(index);
  BookmarksItem* parentItem = childItem->parent();

  if (parentItem == rootItem)
  { return QModelIndex(); }

  return createIndex(parentItem->childNumber(), 0, parentItem);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int BookmarksModel::rowCount(const QModelIndex& parent) const
{
  BookmarksItem* parentItem = getItem(parent);

  return parentItem->childCount();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  BookmarksItem* item = getItem(index);
  bool result = false;

  if (role == Qt::UserRole)
  {
    result = item->setItemHasErrors(value.value<bool>());
  }
  else if (role == Qt::DecorationRole)
  {
    result = item->setIcon(value.value<QIcon>());
  }
  else if (role == Qt::ToolTipRole)
  {
    result = item->setItemTooltip(value.toString());
  }
  else
  {
    result = item->setData(index.column(), value);
  }

  if (result)
  { emit dataChanged(index, index); }

  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksModel::setNeedsToBeExpanded(const QModelIndex& index, bool value)
{
  BookmarksItem* item = getItem(index);
  item->setNeedsToBeExpanded(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksModel::needsToBeExpanded(const QModelIndex& index)
{
  BookmarksItem* item = getItem(index);
  return item->needsToBeExpanded();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksItem* BookmarksModel::getRootItem()
{
  return rootItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksModel::isEmpty()
{
  if (rowCount(QModelIndex()) <= 0)
  {
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksModel::addFileToTree(QString& path, QModelIndex& specifiedParent)
{
  path = QDir::toNativeSeparators(path);

  QFileInfo fi(path);

  int rowPos = self->rowCount(specifiedParent);
  self->insertRow(rowPos, specifiedParent);
  QModelIndex newNameIndex = self->index(rowPos, BookmarksItem::Name, specifiedParent);

  if (fi.isFile())
  {
    QString name = fi.baseName();
    self->setData(newNameIndex, name, Qt::DisplayRole);
  }
  else
  {
    QDir dir(path);
    self->setData(newNameIndex, dir.dirName(), Qt::DisplayRole);
  }

  if (fi.isFile())
  {
    QModelIndex newPathIndex = self->index(rowPos, BookmarksItem::Path, specifiedParent);
    self->setData(newPathIndex, path, Qt::DisplayRole);
    self->setData(newNameIndex, QIcon(":/text.png"), Qt::DecorationRole);
  }
  else
  {
    self->setData(newNameIndex, QIcon(":/folder_blue.png"), Qt::DecorationRole);

    QStringList filters;
    filters << "*.dream3d" << "*.ini" << "*.txt" << "*.json";
    QDirIterator iter(path, filters, QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    while (iter.hasNext())
    {
      QString nextPath = iter.next();
      addFileToTree(nextPath, newNameIndex);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList BookmarksModel::getFilePaths()
{
  return getFilePaths(rootItem);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList BookmarksModel::getFilePaths(BookmarksItem* item)
{
  QStringList list;
  if (item != rootItem && item->childCount() <= 0)
  {
    QString filePath = item->data(BookmarksItem::Path).toString();
    if (filePath.isEmpty() == false)
    {
      list.append(filePath);
    }
    return list;
  }

  for (int i = 0; i < item->childCount(); i++)
  {
    list.append(getFilePaths(item->child(i)));
  }

  return list;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndexList BookmarksModel::findIndexByPath(QString filePath)
{
  QModelIndexList list;
  for (int i = 0; i < rootItem->childCount(); i++)
  {
    QModelIndex child = index(i, BookmarksItem::Path, QModelIndex());
    if (rowCount(child) <= 0 && child.data().toString() == filePath)
    {
      list.append(child);
    }

    for (int j = 0; j < rowCount(child); j++)
    {
      QModelIndexList subList = findIndexByPath(child, filePath);
      list.append(subList);
    }
  }

  return list;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndexList BookmarksModel::findIndexByPath(const QModelIndex& current, QString filePath)
{
  QModelIndex actual = index(current.row(), BookmarksItem::Name, current.parent());

  QModelIndexList list;
  for (int i = 0; i < rowCount(actual); i++)
  {
    QModelIndex pathIndex = index(i, BookmarksItem::Path, actual);

    if (rowCount(pathIndex) <= 0 && pathIndex.data().toString() == filePath)
    {
      list.append(pathIndex);
    }

    QModelIndexList subList = findIndexByPath(pathIndex, filePath);
    list.append(subList);
  }

  return list;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksModel::updateModel(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
  if (topLeft.isValid())
  {
    QString path = index(topLeft.row(), BookmarksItem::Path, topLeft.parent()).data().toString();
    QFileInfo fi(path);
    if (NULL != m_Watcher && path.isEmpty() == false && fi.exists())
    {
      m_Watcher->addPath(path);
    }
  }
  else if (bottomRight.isValid())
  {
    QString path = index(bottomRight.row(), BookmarksItem::Path, bottomRight.parent()).data().toString();
    QFileInfo fi(path);
    if (NULL != m_Watcher && path.isEmpty() == false && fi.exists())
    {
      m_Watcher->addPath(path);
    }
  }
}




