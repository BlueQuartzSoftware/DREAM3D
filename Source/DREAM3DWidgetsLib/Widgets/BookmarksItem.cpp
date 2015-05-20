/* ============================================================================
* Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
* Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
* Copyright (c) 2012 Joseph B. Kleingers (Student Research Assistant)
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
* Neither the name of Michael A. Groeber, Michael A. Jackson, Joseph B. Kleingers,
* the US Air Force, BlueQuartz Software nor the names of its contributors may be
* used to endorse or promote products derived from this software without specific
* prior written permission.
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

#include "BookmarksItem.h"

#include <QStringList>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksItem::BookmarksItem(const QVector<QVariant> &data, BookmarksItem *parent)
{
  parentItem = parent;
  itemData = data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksItem::~BookmarksItem()
{
  qDeleteAll(childItems);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksItem *BookmarksItem::child(int number)
{
  return childItems.value(number);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int BookmarksItem::childCount() const
{
  return childItems.count();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int BookmarksItem::childNumber() const
{
  if (parentItem)
    return parentItem->childItems.indexOf(const_cast<BookmarksItem*>(this));

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int BookmarksItem::columnCount() const
{
  return itemData.count();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant BookmarksItem::data(int column) const
{
  return itemData.value(column);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksItem::insertChildren(int position, int count, int columns)
{
  if (position < 0 || position > childItems.size())
    return false;

  for (int row = 0; row < count; ++row) {
    QVector<QVariant> data(columns);
    BookmarksItem *item = new BookmarksItem(data, this);
    childItems.insert(position, item);
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksItem::insertColumns(int position, int columns)
{
  if (position < 0 || position > itemData.size())
    return false;

  for (int column = 0; column < columns; ++column)
    itemData.insert(position, QVariant());

  foreach(BookmarksItem *child, childItems)
    child->insertColumns(position, columns);

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksItem *BookmarksItem::parent()
{
  return parentItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksItem::removeChildren(int position, int count)
{
  if (position < 0 || position + count > childItems.size())
    return false;

  for (int row = 0; row < count; ++row)
    delete childItems.takeAt(position);

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksItem::removeColumns(int position, int columns)
{
  if (position < 0 || position + columns > itemData.size())
    return false;

  for (int column = 0; column < columns; ++column)
    itemData.remove(position);

  foreach(BookmarksItem *child, childItems)
    child->removeColumns(position, columns);

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksItem::setData(int column, const QVariant &value)
{
  if (column < 0 || column >= itemData.size())
    return false;

  itemData[column] = value;
  return true;
}