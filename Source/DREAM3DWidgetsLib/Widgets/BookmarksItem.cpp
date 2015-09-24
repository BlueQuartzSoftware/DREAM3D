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
#include "BookmarksItem.h"

#include <QtGui/QColor>
#include <QtCore/QStringList>


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksItem::BookmarksItem(const QVector<QVariant>& data, BookmarksItem* parent) :
  m_ItemData(data),
  m_ParentItem(parent),
  m_ItemHasErrors(false),
  m_ItemTooltip(""),
  m_NeedsToBeExpanded(false),
  m_Icon(QIcon())
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksItem::~BookmarksItem()
{
  qDeleteAll(m_ChildItems);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString BookmarksItem::TopLevelString()
{
  return QString::fromLatin1("[Top Level]");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksItem* BookmarksItem::child(int number)
{
  return m_ChildItems.value(number);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int BookmarksItem::childCount() const
{
  return m_ChildItems.count();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int BookmarksItem::childNumber() const
{
  if (m_ParentItem)
  { return m_ParentItem->m_ChildItems.indexOf(const_cast<BookmarksItem*>(this)); }

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int BookmarksItem::columnCount() const
{
  return m_ItemData.count();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant BookmarksItem::data(int column) const
{
  return m_ItemData.value(column);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksItem::insertChildren(int position, int count, int columns)
{
  if (position < 0 || position > m_ChildItems.size())
  { return false; }

  for (int row = 0; row < count; ++row)
  {
    QVector<QVariant> data(columns);
    BookmarksItem* item = new BookmarksItem(data, this);
    m_ChildItems.insert(position, item);
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksItem::insertColumns(int position, int columns)
{
  if (position < 0 || position > m_ItemData.size())
  { return false; }

  for (int column = 0; column < columns; ++column)
  { m_ItemData.insert(position, QVariant()); }

  foreach(BookmarksItem* child, m_ChildItems)
  { child->insertColumns(position, columns); }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksItem* BookmarksItem::parent()
{
  return m_ParentItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksItem::removeChildren(int position, int count)
{
  if (position < 0 || position + count > m_ChildItems.size())
  { return false; }

  for (int row = 0; row < count; ++row)
  { delete m_ChildItems.takeAt(position); }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksItem::removeColumns(int position, int columns)
{
  if (position < 0 || position + columns > m_ItemData.size())
  { return false; }

  for (int column = 0; column < columns; ++column)
  { m_ItemData.remove(position); }

  foreach(BookmarksItem* child, m_ChildItems)
  { child->removeColumns(position, columns); }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksItem::setData(int column, const QVariant& value)
{
  if (column < 0 || column >= m_ItemData.size())
  { return false; }

  m_ItemData[column] = value;
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksItem::getItemHasErrors()
{
  return m_ItemHasErrors;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksItem::setItemHasErrors(const bool& value)
{
  m_ItemHasErrors = value;
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString BookmarksItem::getItemTooltip()
{
  return m_ItemTooltip;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksItem::setItemTooltip(const QString& value)
{
  m_ItemTooltip = value;
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon BookmarksItem::getIcon()
{
  return m_Icon;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksItem::setIcon(const QIcon& icon)
{
  m_Icon = icon;
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BookmarksItem::needsToBeExpanded()
{
  return m_NeedsToBeExpanded;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksItem::setNeedsToBeExpanded(bool value)
{
  m_NeedsToBeExpanded = value;
}
