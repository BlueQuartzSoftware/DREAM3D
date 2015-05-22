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

BookmarksModel* BookmarksModel::self = 0;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksModel::BookmarksModel(QObject *parent) :
QAbstractItemModel(parent)
{
  QVector<QVariant> vector;
  vector.push_back("Name");
  vector.push_back("Path");
  rootItem = new BookmarksItem(vector);
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
BookmarksModel* BookmarksModel::NewInstanceFromFile(QString filePath)
{
  QFileInfo fi(filePath);
  if (fi.exists() & fi.isFile())
  {
    // Erase the old content
    delete self;
    self = NULL;

    DREAM3DSettings prefs(filePath);

    prefs.beginGroup("DockWidgetSettings");
    QJsonObject modelObj = prefs.value("Bookmarks Model", QJsonObject());
    prefs.endGroup();

    self = BookmarksTreeView::FromJsonObject(modelObj);
  }

  return self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int BookmarksModel::columnCount(const QModelIndex &parent) const
{
  return rootItem->columnCount();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex BookmarksModel::sibling(const QModelIndex &currentIndex)
{
  if (currentIndex.column() == Name)
  {
    return index(currentIndex.row(), Path, currentIndex.parent());
  }
  else
  {
    return index(currentIndex.row(), Name, currentIndex.parent());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant BookmarksModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role != Qt::DisplayRole && role != Qt::EditRole)
    return QVariant();

  BookmarksItem *item = getItem(index);

  return item->data(index.column());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::ItemFlags BookmarksModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
    return 0;

  Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

  BookmarksItem* item = getItem(index);
  QString name = item->data(Name).toString();
  if (item->data(Path).toString().isEmpty())
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
BookmarksItem *BookmarksModel::getItem(const QModelIndex &index) const
{
  if (index.isValid()) {
    BookmarksItem *item = static_cast<BookmarksItem*>(index.internalPointer());
    if (item)
      return item;
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
    return rootItem->data(section);

  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex BookmarksModel::index(int row, int column, const QModelIndex &parent) const
{
  if (parent.isValid() && parent.column() != 0)
    return QModelIndex();

  BookmarksItem *parentItem = getItem(parent);

  BookmarksItem *childItem = parentItem->child(row);
  if (childItem)
    return createIndex(row, column, childItem);
  else
    return QModelIndex();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksModel::insertRows(int position, int rows, const QModelIndex &parent)
{
  BookmarksItem *parentItem = getItem(parent);
  bool success;

  beginInsertRows(parent, position, position + rows - 1);
  success = parentItem->insertChildren(position, rows, rootItem->columnCount());
  endInsertRows();

  return success;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksModel::removeRows(int position, int rows, const QModelIndex &parent)
{
  BookmarksItem *parentItem = getItem(parent);
  bool success = true;

  beginRemoveRows(parent, position, position + rows - 1);
  success = parentItem->removeChildren(position, rows);
  endRemoveRows();

  return success;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex BookmarksModel::parent(const QModelIndex &index) const
{
  if (!index.isValid())
    return QModelIndex();

  BookmarksItem *childItem = getItem(index);
  BookmarksItem *parentItem = childItem->parent();

  if (parentItem == rootItem)
    return QModelIndex();

  return createIndex(parentItem->childNumber(), 0, parentItem);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int BookmarksModel::rowCount(const QModelIndex &parent) const
{
  BookmarksItem *parentItem = getItem(parent);

  return parentItem->childCount();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksModel::setData(const QModelIndex &index, const QVariant &value)
{
  BookmarksItem *item = getItem(index);
  bool result = item->setData(index.column(), value);

  if (result)
    emit dataChanged(index, index);

  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksModel::setNeedsToBeExpanded(const QModelIndex &index, bool value)
{
  BookmarksItem *item = getItem(index);
  item->setNeedsToBeExpanded(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksModel::needsToBeExpanded(const QModelIndex &index)
{
  BookmarksItem *item = getItem(index);
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




