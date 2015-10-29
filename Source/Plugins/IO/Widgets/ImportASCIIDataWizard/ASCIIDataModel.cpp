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

#include "ASCIIDataModel.h"
#include "ASCIIDataItem.h"

ASCIIDataModel* ASCIIDataModel::self = NULL;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ASCIIDataModel::ASCIIDataModel(QObject* parent) :
QAbstractTableModel(parent)
{
  connect(this, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(updateModel(const QModelIndex&, const QModelIndex&)));
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
  if (self == NULL)
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
  if (!index.isValid())
  {
    return QVariant();
  }

  ASCIIDataItem* item = getItem(index);

  if (role == Qt::DisplayRole)
  {
    return item->data(index.column());
  }
  else
  {
    return QVariant();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::ItemFlags ASCIIDataModel::flags(const QModelIndex& index) const
{
  if (!index.isValid())
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
  if (index.isValid())
  {
    ASCIIDataItem* item = m_TableItems[index.row()];
    if (item)
    {
      return item;
    }
  }
  return NULL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant ASCIIDataModel::headerData(int section, Qt::Orientation orientation,
  int role) const
{
  if (section < 0)
  {
    return QVariant();
  }

  if (section < m_HorizontalHeaders.size() && orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    return m_HorizontalHeaders[section];
  }
  else if (section < m_VerticalHeaders.size() && orientation == Qt::Vertical && role == Qt::DisplayRole)
  {
    return m_VerticalHeaders[section];
  }

  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ASCIIDataModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant & value, int role)
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    m_HorizontalHeaders[section] = value.toString();
    return true;
  }
  else if (orientation == Qt::Vertical && role == Qt::DisplayRole)
  {
    m_VerticalHeaders[section] = value.toString();
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
  for (int i = 0; i < m_TableItems.size(); i++)
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
  for (int i = 0; i < m_TableItems.size(); i++)
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

  if (index.column() >= m_HorizontalHeaders.size())
  {
    return false;
  }

  bool result = m_TableItems[row]->setData(index.column(), value);

  if (result)
  {
    emit dataChanged(index, index);
  }

  return result;
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
void ASCIIDataModel::clear()
{
  if (rowCount() > 0)
  {
    removeRows(0, rowCount());
    m_VerticalHeaders.clear();
  }

  if (columnCount() > 0)
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
  if (rowCount() > 0 && columnCount() > 0)
  {
    for (int row = 0; row < rowCount(); row++)
    {
      for (int col = 0; col < columnCount(); col++)
      {
        QModelIndex index = this->index(row, col);
        setData(index, "", Qt::DisplayRole);
      }
    }
  }
}




