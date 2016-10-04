/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include "ASCIIDataModel.h"

#include <QtGui/QColor>
#include <QtGui/QFont>

#include "SIMPLib/Common/Constants.h"

#include "ASCIIDataItem.h"

ASCIIDataModel* ASCIIDataModel::self = nullptr;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ASCIIDataModel::ASCIIDataModel(QObject* parent)
: QAbstractTableModel(parent)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ASCIIDataModel::~ASCIIDataModel()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ASCIIDataModel* ASCIIDataModel::Instance()
{
  if(self == nullptr)
  {
    self = new ASCIIDataModel();
  }

  return self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ASCIIDataModel::columnCount(const QModelIndex& parent) const
{
  return m_HorizontalHeaders.size();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant ASCIIDataModel::data(const QModelIndex& index, int role) const
{
  if(!index.isValid())
  {
    return QVariant();
  }

  if(role == Qt::DisplayRole)
  {
    if(index.row() == 0)
    {
      QString dataType = m_ColumnDataType[index.column()];
      return dataType;
    }
    else
    {
      QModelIndex actualIndex = this->index(index.row() - 1, index.column());
      ASCIIDataItem* item = getItem(actualIndex);
      return item->data(index.column());
    }
  }
  else if(role == Qt::FontRole)
  {
    if(index.row() == 0)
    {
      QFont font;
      font.setItalic(true);
      return font;
    }
  }
  else if(role == Qt::TextAlignmentRole)
  {
    return Qt::AlignCenter;
  }
  else if(role == Qt::BackgroundRole)
  {

    if(columnHasErrors(index.column()) == true)
    {
      return QColor(255, 191, 193);
    }
    else
    {
      return QVariant();
    }
  }

  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::ItemFlags ASCIIDataModel::flags(const QModelIndex& index) const
{
  if(!index.isValid())
  {
    return 0;
  }

  Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);
  return (defaultFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ASCIIDataItem* ASCIIDataModel::getItem(const QModelIndex& index) const
{
  if(index.isValid())
  {
    ASCIIDataItem* item = m_TableItems[index.row()];
    if(item)
    {
      return item;
    }
  }
  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant ASCIIDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(section < 0)
  {
    return QVariant();
  }

  if(section < m_HorizontalHeaders.size() && orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    return m_HorizontalHeaders[section];
  }
  else if(section < m_VerticalHeaders.size() && orientation == Qt::Vertical && role == Qt::DisplayRole)
  {
    if(section == 0)
    {
      return "Data Type";
    }
    else
    {
      return m_VerticalHeaders[section - 1];
    }
  }

  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ASCIIDataModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
  if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    m_HorizontalHeaders[section] = value.toString();
    emit headerDataChanged(Qt::Horizontal, section, section);
    return true;
  }
  else if(orientation == Qt::Vertical && role == Qt::DisplayRole)
  {
    m_VerticalHeaders[section] = value.toString();
    emit headerDataChanged(Qt::Vertical, section, section);
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ASCIIDataModel::insertRows(int position, int rows, const QModelIndex& parent)
{
  Q_UNUSED(parent)
  beginInsertRows(QModelIndex(), position, position + rows - 1);
  m_TableItems.insert(position, rows, new ASCIIDataItem(QVector<QVariant>(m_HorizontalHeaders.size(), "")));
  m_VerticalHeaders.insert(position, rows, "");
  endInsertRows();
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ASCIIDataModel::removeRows(int position, int rows, const QModelIndex& parent)
{
  Q_UNUSED(parent)
  beginRemoveRows(QModelIndex(), position, position + rows - 1);
  m_TableItems.remove(position, rows);
  m_VerticalHeaders.remove(position, rows);
  endRemoveRows();
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ASCIIDataModel::insertColumns(int position, int columns, const QModelIndex& parent)
{
  Q_UNUSED(parent)
  beginInsertColumns(QModelIndex(), position, position + columns - 1);
  m_HorizontalHeaders.insert(position, columns, "");
  m_ColumnDataType.insert(position, columns, SIMPL::TypeNames::Double);
  m_ColumnHasErrors.insert(position, columns, false);
  for(int i = 0; i < m_TableItems.size(); i++)
  {
    m_TableItems[i]->insertColumns(position, columns);
  }
  endInsertColumns();
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ASCIIDataModel::removeColumns(int position, int columns, const QModelIndex& parent)
{
  Q_UNUSED(parent)
  beginRemoveColumns(QModelIndex(), position, position + columns - 1);
  m_HorizontalHeaders.remove(position, columns);
  m_ColumnDataType.remove(position, columns);
  m_ColumnHasErrors.remove(position, columns);
  for(int i = 0; i < m_TableItems.size(); i++)
  {
    m_TableItems[i]->removeColumns(position, columns);
  }
  endRemoveColumns();
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ASCIIDataModel::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent)
  return m_TableItems.size();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ASCIIDataModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  int row = index.row();

  if(index.column() >= m_HorizontalHeaders.size())
  {
    return false;
  }

  bool result = m_TableItems[row]->setData(index.column(), value);

  if(result)
  {
    emit dataChanged(index, index);
  }

  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList ASCIIDataModel::originalStrings()
{
  QStringList lines;
  for(int i = 0; i < m_TableItems.size(); i++)
  {
    lines.push_back(m_TableItems[i]->originalString());
  }
  return lines;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ASCIIDataModel::originalString(const int row) const
{
  return m_TableItems[row]->originalString();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ASCIIDataModel::setOriginalString(const int row, const QString& value)
{
  m_TableItems[row]->setOriginalString(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ASCIIDataModel::columnDataType(const int column) const
{
  return m_ColumnDataType[column];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ASCIIDataModel::setColumnDataType(const int column, const QString& type)
{
  m_ColumnDataType[column] = type;

  QModelIndex index = this->index(0, column);
  emit dataChanged(index, index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ASCIIDataModel::columnHasErrors(const int column) const
{
  return m_ColumnHasErrors[column];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ASCIIDataModel::setColumnHasErrors(const int column, const bool& value)
{
  m_ColumnHasErrors[column] = value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ASCIIDataModel::clear()
{
  if(rowCount() > 0)
  {
    removeRows(0, rowCount());
    m_VerticalHeaders.clear();
  }

  if(columnCount() > 0)
  {
    removeColumns(0, columnCount());
    m_HorizontalHeaders.clear();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ASCIIDataModel::clearContents()
{
  if(rowCount() > 0 && columnCount() > 0)
  {
    for(int row = 0; row < rowCount(); row++)
    {
      for(int col = 0; col < columnCount(); col++)
      {
        QModelIndex index = this->index(row, col);
        setData(index, "", Qt::DisplayRole);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ASCIIDataModel::clearHeaders(Qt::Orientation orientation)
{
  if(orientation == Qt::Horizontal)
  {
    for(int i = 0; i < m_HorizontalHeaders.size(); i++)
    {
      m_HorizontalHeaders[i] = "";
    }
    emit headerDataChanged(Qt::Horizontal, 0, m_HorizontalHeaders.size() - 1);
  }
  else if(orientation == Qt::Vertical)
  {
    for(int i = 0; i < m_VerticalHeaders.size(); i++)
    {
      m_VerticalHeaders[i] = "";
    }
    emit headerDataChanged(Qt::Vertical, 0, m_VerticalHeaders.size() - 1);
  }
}
