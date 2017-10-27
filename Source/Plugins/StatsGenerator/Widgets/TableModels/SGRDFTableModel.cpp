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

#include "SGRDFTableModel.h"

#include <iostream>

#include <QtWidgets/QAbstractItemDelegate>
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyleOptionComboBox>

#include "StatsGenerator/Widgets/Delegates/SGRDFItemDelegate.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_SGRDFTableModel.cpp"
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SGRDFTableModel::SGRDFTableModel(QObject* parent)
: QAbstractTableModel(parent)
, m_RowCount(0)
{
  m_ColumnCount = ColumnCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SGRDFTableModel::~SGRDFTableModel()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::ItemFlags SGRDFTableModel::flags(const QModelIndex& index) const
{
  //  qDebug() << "SGRDFTableModel::flags" << "\n";
  if(!index.isValid())
  {
    return Qt::NoItemFlags;
  }
  Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
  if(index.isValid())
  {
    int col = index.column();
    if(col < SGRDFTableModel::ColumnCount)
    {
      theFlags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
  }
  return theFlags;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant SGRDFTableModel::data(const QModelIndex& index, qint32 role) const
{

  if(!index.isValid())
  {
    return QVariant();
  }

  if(role == Qt::SizeHintRole)
  {
    QStyleOptionComboBox comboBox;
    comboBox.currentText = QString("10001");
    if(index.column() < SGRDFTableModel::ColumnCount)
    {
      const QString header = headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();
      if(header.length() > comboBox.currentText.length())
      {
        comboBox.currentText = header;
      }
    }
    else
    {
      Q_ASSERT(false);
    }
    QFontMetrics fontMetrics(data(index, Qt::FontRole).value<QFont>());
    comboBox.fontMetrics = fontMetrics;
    QSize size(fontMetrics.width(comboBox.currentText), fontMetrics.height());
    return qApp->style()->sizeFromContents(QStyle::CT_ComboBox, &comboBox, size);
  }
  else if(role == Qt::TextAlignmentRole)
  {
    return int(Qt::AlignRight | Qt::AlignVCenter);
  }
  else if(role == Qt::DisplayRole || role == Qt::EditRole)
  {
    int col = index.column();
    if(col == Frequency)
    {
      return QVariant(m_Frequencies[index.row()]);
    }
  }

  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant SGRDFTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    switch(section)
    {
    case Frequency:
      return QVariant(QString("Normalized Fequency"));
      break;
    default:
      break;
    }
  }
  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SGRDFTableModel::rowCount(const QModelIndex& index) const
{
  return index.isValid() ? 0 : m_RowCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SGRDFTableModel::columnCount(const QModelIndex& index) const
{
  return index.isValid() ? 0 : m_ColumnCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SGRDFTableModel::setHeaderData(int col, Qt::Orientation o, const QVariant& var, int role)
{
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SGRDFTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  // qDebug() << "SGRDFTableModel::setData " << value.toString() << "\n";
  if(!index.isValid() || role != Qt::EditRole || index.row() < 0 || index.row() >= m_Frequencies.count() || index.column() < 0 || index.column() >= m_ColumnCount)
  {
    return false;
  }
  bool ok;
  qint32 row = index.row();
  qint32 col = index.column();
  switch(col)
  {
  case Frequency:
    m_Frequencies[row] = value.toFloat(&ok);
    break;
  default:
    Q_ASSERT(false);
  }

  emit dataChanged(index, index);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SGRDFTableModel::insertRows(int row, int count, const QModelIndex& index)
{
  int freq = 0;
  beginInsertRows(QModelIndex(), row, row + count - 1);
  for(int i = 0; i < count; ++i)
  {
    m_Frequencies.append(freq);
    m_RowCount = m_Frequencies.count();
  }
  endInsertRows();
  emit dataChanged(index, index);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SGRDFTableModel::removeRows(int row, int count, const QModelIndex& index)
{
  if(count < 1)
  {
    return true;
  } // No Rows to remove
  beginRemoveRows(QModelIndex(), row, row + count - 1);
  for(int i = 0; i < count; ++i)
  {
    m_Frequencies.remove(row);
    m_RowCount = m_Frequencies.count();
  }
  endRemoveRows();
  emit dataChanged(index, index);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<float> SGRDFTableModel::getData(int col)
{

  switch(col)
  {
  case Frequency:
    return m_Frequencies;
    break;
  default:
    Q_ASSERT(false);
  }
  return QVector<float>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SGRDFTableModel::getDataValue(int col, int row)
{
  switch(col)
  {
  case Frequency:
    return m_Frequencies[row];
    break;
  default:
    Q_ASSERT(false);
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGRDFTableModel::setColumnData(int col, QVector<float>& data)
{
  switch(col)
  {
  case Frequency:
    m_Frequencies = data;
    break;
  default:
    Q_ASSERT(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGRDFTableModel::setRowData(int row, int freq)
{
  m_Frequencies[row] = freq;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAbstractItemDelegate* SGRDFTableModel::getItemDelegate()
{
  return new SGRDFItemDelegate();
}

#define ADD_INITIAL_ROW_VALUE(name, weight, sigma)                                                                                                                                                     \
  insertRow(rowCount());                                                                                                                                                                               \
  QModelIndex textureIndex = index(rowCount() - 1, SGRDFTableModel::Texture);                                                                                                                          \
  setData(textureIndex, QVariant(QString(name)), Qt::EditRole);                                                                                                                                        \
  QModelIndex weightIndex = index(rowCount() - 1, SGRDFTableModel::Weight);                                                                                                                            \
  setData(weightIndex, QVariant(weight), Qt::EditRole);                                                                                                                                                \
  QModelIndex sigmaIndex = index(rowCount() - 1, SGRDFTableModel::Sigma);                                                                                                                              \
  setData(sigmaIndex, QVariant(sigma), Qt::EditRole);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGRDFTableModel::setInitialValues()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGRDFTableModel::setTableData(QVector<float> freqs)
{
  qint32 count = freqs.count();

  qint32 row = 0;
  // Remove all the current rows in the table model
  removeRows(0, rowCount());
  QModelIndex topLeft;
  QModelIndex botRight;
  if(count >= 1)
  {
    // Now mass insert the data to the table then emit that the data has changed
    beginInsertRows(QModelIndex(), row, row + count - 1);
    m_Frequencies = freqs;
    m_RowCount = count;
    endInsertRows();
    createIndex(0, 0);
    createIndex(count - 1, ColumnCount);
  }
  emit dataChanged(topLeft, botRight);
}
