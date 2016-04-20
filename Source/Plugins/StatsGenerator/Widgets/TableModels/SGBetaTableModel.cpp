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


#include "SGBetaTableModel.h"
#include <iostream>

#include <QtWidgets/QStyleOptionComboBox>
#include <QtWidgets/QAbstractItemDelegate>

#include "Applications/SIMPLView/SIMPLViewApplication.h"

#include "StatsGenerator/Widgets/Delegates/SGBetaItemDelegate.h"
// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_SGBetaTableModel.cpp"
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SGBetaTableModel::SGBetaTableModel(QObject* parent) :
  SGAbstractTableModel(parent), m_RowCount(0)
{
  m_ColumnCount = ColumnCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SGBetaTableModel::~SGBetaTableModel()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::ItemFlags SGBetaTableModel::flags(const QModelIndex& index) const
{
  //  qDebug() << "SGBetaTableModel::flags" << "\n";
  if (!index.isValid())
  {
    return Qt::NoItemFlags;
  }
  Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
  if (index.isValid())
  {
    theFlags |= Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    int col = index.column();
    if (col == BinNumber)
    {
      theFlags = Qt::ItemIsEnabled;
    }
    else if (col == Alpha)
    {
      theFlags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    else if (col == Beta)
    {
      theFlags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    else if (col == LineColor)
    {
      theFlags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
  }
  return theFlags;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant SGBetaTableModel::data(const QModelIndex& index, qint32 role) const
{

  if (!index.isValid())
  {
    return QVariant();
  }

  if (role == Qt::SizeHintRole)
  {
    QStyleOptionComboBox comboBox;

    switch(index.column())
    {
      case BinNumber:
      {
        comboBox.currentText = QString("101");
        const QString header = headerData(BinNumber, Qt::Horizontal, Qt::DisplayRole).toString();
        if (header.length() > comboBox.currentText.length()) { comboBox.currentText = header; }
        break;
      }
      case Alpha:
      {
        comboBox.currentText = QString("10001");
        const QString header = headerData(BinNumber, Qt::Horizontal, Qt::DisplayRole).toString();
        if (header.length() > comboBox.currentText.length()) { comboBox.currentText = header; }
        break;
      }
      case Beta:
      {
        comboBox.currentText = QString("10001");
        const QString header = headerData(BinNumber, Qt::Horizontal, Qt::DisplayRole).toString();
        if (header.length() > comboBox.currentText.length()) { comboBox.currentText = header; }
        break;
      }
      case LineColor:
      {
        comboBox.currentText = QString("Dark Blue     ");
        const QString header = headerData(BinNumber, Qt::Horizontal, Qt::DisplayRole).toString();
        if (header.length() > comboBox.currentText.length())
        {
          comboBox.currentText = header;
        }
        break;
      }
      default:
        Q_ASSERT(false);
    }
    QFontMetrics fontMetrics(data(index, Qt::FontRole) .value<QFont > ());
    comboBox.fontMetrics = fontMetrics;
    QSize size(fontMetrics.width(comboBox.currentText), fontMetrics.height());
    return dream3dApp->style()->sizeFromContents(QStyle::CT_ComboBox, &comboBox, size);
  }
  else if (role == Qt::TextAlignmentRole)
  {
    return int(Qt::AlignRight | Qt::AlignVCenter);
  }
  else if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    int col = index.column();
    if (col == BinNumber)
    {
      return QVariant(m_BinNumbers[index.row()]);
    }
    else if (col == Alpha)
    {
      return QVariant(m_Alpha[index.row()]);
    }
    else if (col == Beta)
    {
      return QVariant(m_Beta[index.row()]);
    }
    else if (col == LineColor)
    {
      return QVariant(m_Colors[index.row()]);
    }
  }

  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant SGBetaTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    switch(section)
    {
      case BinNumber:
        return QVariant(QString("Bin"));
        break;
      case Alpha:
        return QVariant(QString("Alpha"));
        break;
      case Beta:
        return QVariant(QString("Beta"));
        break;
      case LineColor:
        return QVariant(QString("Color"));
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
int SGBetaTableModel::rowCount(const QModelIndex& index) const
{
  return index.isValid() ? 0 : m_RowCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SGBetaTableModel::columnCount(const QModelIndex& index) const
{
  return index.isValid() ? 0 : m_ColumnCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SGBetaTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  // qDebug() << "SGBetaTableModel::setData " << value.toString() << "\n";
  if (!index.isValid() || role != Qt::EditRole || index.row() < 0 || index.row() >= m_BinNumbers.count() || index.column() < 0 || index.column()
      >= m_ColumnCount)
  {
    return false;
  }
  bool ok;
  qint32 row = index.row();
  qint32 col = index.column();
  switch(col)
  {
    case BinNumber:
      m_BinNumbers[row] = value.toFloat(&ok);
      break;
    case Alpha:
      m_Alpha[row] = value.toFloat(&ok);
      break;
    case Beta:
      m_Beta[row] = value.toFloat(&ok);
      break;
    case LineColor:
      m_Colors[row] = value.toString();
      break;
    default:
      Q_ASSERT(false);

  }

  emit
  dataChanged(index, index);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SGBetaTableModel::insertRows(int row, int count, const QModelIndex& index)
{
  qint32 binNum = 0;
  float alpha = 5.0;
  float beta = 5.0;

  QString c("blue");

  beginInsertRows(QModelIndex(), row, row + count - 1);
  for (int i = 0; i < count; ++i)
  {
    m_BinNumbers.append(binNum);
    m_Alpha.append(alpha);
    m_Beta.append(beta);
    m_Colors.append(c);
    m_RowCount = m_BinNumbers.count();
  }
  endInsertRows();
  emit dataChanged(index, index);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SGBetaTableModel::removeRows(int row, int count, const QModelIndex& index)
{
  if (count < 1)
  {
    return true;
  } // No Rows to remove
  beginRemoveRows(QModelIndex(), row, row + count - 1);
  for (int i = 0; i < count; ++i)
  {
    m_BinNumbers.remove(row);
    m_Alpha.remove(row);
    m_Beta.remove(row);
    m_Colors.remove(row);
    m_RowCount = m_BinNumbers.count();
  }
  endRemoveRows();
  emit
  dataChanged(index, index);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<float > SGBetaTableModel::getData(int col)
{

  switch(col)
  {
    case Alpha:
      return m_Alpha;
      break;
    case Beta:
      return m_Beta;
      break;
    default:
      Q_ASSERT(false);
  }
  return QVector<float > ();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float SGBetaTableModel::getDataValue(int col, int row)
{
  switch(col)
  {
    case Alpha:
      return m_Alpha[row];
      break;
    case Beta:
      return m_Beta[row];
      break;
    default:
      Q_ASSERT(false);
  }
  return 0.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGBetaTableModel::setColumnData(int col, QVector<float>& data)
{
  switch(col)
  {
    case Alpha:
      m_Alpha = data;
      break;
    case Beta:
      m_Beta = data;
      break;
    default:
      Q_ASSERT(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAbstractItemDelegate* SGBetaTableModel::getItemDelegate()
{
  return new SGBetaItemDelegate;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGBetaTableModel::setTableData(QVector<float> bins, QVector<QVector<float> > data, QVector<QString> colors)
{
  qint32 count = bins.count();
  // Now make sure we _really_ have the correct count because the number of
  // bins may NOT really reflect what is in the 'data' vectors. This discrepency
  // can happen if not all of the data was written to the stats file
  for(int i = 0; i < data.count(); ++i)
  {
    if (data[i].count() < count) { count = data[i].count(); }
  }
  qint32 row = 0;
  // Remove all the current rows in the table model
  removeRows(0, rowCount());
  if(data.size() > 1)
  {
    int offset = row + count - 1;
    if (offset < 0) { offset = 0;}
    // Now mass insert the data to the table then emit that the data has changed
    beginInsertRows(QModelIndex(), row, offset);
    m_BinNumbers = bins;
    m_Alpha = data[0];
    m_Beta = data[1];
    m_Colors = colors;
    m_RowCount = count;
    endInsertRows();
    QModelIndex topLeft = createIndex(0, 0);
    offset = count - 1;
    if (offset < 0) { offset = 0;}
    QModelIndex botRight = createIndex(offset, ColumnCount);
    emit dataChanged(topLeft, botRight);
  }
}



