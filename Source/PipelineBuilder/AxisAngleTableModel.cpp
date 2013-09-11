/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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

#include "AxisAngleTableModel.h"

#include <iostream>

#include <QApplication>
#include <QtGui/QStyleOptionComboBox>
#include <QtGui/QAbstractItemDelegate>

#include "AxisAngleItemDelegate.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AxisAngleTableModel::AxisAngleTableModel(QObject* parent) :
QAbstractTableModel(parent),
m_RowCount(0)
{
  m_ColumnCount = ColumnCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AxisAngleTableModel::~AxisAngleTableModel()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::ItemFlags AxisAngleTableModel::flags(const QModelIndex &index) const
{
  //  qDebug() << "AxisAngleTableModel::flags" << "\n";
  if (!index.isValid())
  {
    return Qt::NoItemFlags;
  }
  Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
  if (index.isValid())
  {
  //  theFlags |= Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    int col = index.column();
    if (col == Angle)
    {
      theFlags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    else if (col == Axis)
    {
      theFlags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }

  }
  return theFlags;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant AxisAngleTableModel::data(const QModelIndex &index, qint32 role) const
{

  if (!index.isValid())
  {
    return QVariant();
  }

  if (role == Qt::SizeHintRole)
  {
    QStyleOptionFrame comboBox;
    QLineEdit contents("1");
    switch(index.column())
    {
      case Angle:
      {
        contents.setText( QString("11.") );
        const QString header = headerData(Angle, Qt::Horizontal, Qt::DisplayRole).toString();
        if (header.length() > contents.text().length()) contents.text() = header;
        break;
      }
      case Axis:
      {
        contents.setText( QString("1,1,1") );
        const QString header = headerData(Axis, Qt::Horizontal, Qt::DisplayRole).toString();
        if (header.length() > contents.text().length()) contents.text() = header;
        break;
      }
      default:
        Q_ASSERT(false);
    }
    QFontMetrics fontMetrics(data(index, Qt::FontRole) .value<QFont > ());
    comboBox.fontMetrics = fontMetrics;
    QSize size(fontMetrics.width(contents.text()), fontMetrics.height());
    return qApp->style()->sizeFromContents(QStyle::CT_ComboBox, &comboBox, size);
  }
  else if (role == Qt::TextAlignmentRole)
  {
    return int(Qt::AlignRight | Qt::AlignVCenter);
  }
  else if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    int col = index.column();
    if (col == Angle)
    {
      return QVariant(m_Angles[index.row()]);
    }
    else if (col == Axis)
    {
      return QVariant(m_Axis[index.row()]);
    }
  }

  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant AxisAngleTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    switch(section)
    {
      case Angle:
        return QVariant(QString("Angle (w)"));
        break;
      case Axis:
        return QVariant(QString("Axis <h,k,l>"));
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
int AxisAngleTableModel::rowCount(const QModelIndex &index) const
{
  return index.isValid() ? 0 : m_RowCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AxisAngleTableModel::columnCount(const QModelIndex &index) const
{
  return index.isValid() ? 0 : m_ColumnCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AxisAngleTableModel::setHeaderData(int col, Qt::Orientation o, const QVariant& var, int role)
{
  return false;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AxisAngleTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  // qDebug() << "AxisAngleTableModel::setData " << value.toString().toStdString() << "\n";
  if (!index.isValid() || role != Qt::EditRole || index.row() < 0 || index.row() >= m_Angles.count() || index.column() < 0 || index.column()
      >= m_ColumnCount)
  {
    return false;
  }
  bool ok;
  qint32 row = index.row();
  qint32 col = index.column();
  switch(col)
  {
    case Angle:
      m_Angles[row] = value.toFloat(&ok);
      break;
    case Axis:
      m_Axis[row] = value.toString();
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
bool AxisAngleTableModel::insertRows(int row, int count, const QModelIndex& index)
{
  QString axis("<0,0,1>");
  float angle = 0.0;

  beginInsertRows(QModelIndex(), row, row + count - 1);
  for (int i = 0; i < count; ++i)
  {
    m_Angles.append(angle);
    m_Axis.append(axis);
    m_RowCount = m_Angles.count();
  }
  endInsertRows();
  emit dataChanged(index, index);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AxisAngleTableModel::removeRows(int row, int count, const QModelIndex& index)
{
  if (count < 1)
  {
    return true;
  } // No Rows to remove
  beginRemoveRows(QModelIndex(), row, row + count - 1);
  for (int i = 0; i < count; ++i)
  {
    m_Angles.remove(row);
    m_Axis.remove(row);
    m_RowCount = m_Angles.count();
  }
  endRemoveRows();
  emit dataChanged(index, index);
  return true;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<float > AxisAngleTableModel::getData(int col)
{
  if (col == Angle) {
      return m_Angles;
  }
  else if (col == Axis) {
    int count = rowCount();
    QVector<float> values;
    float h = 0.0;
    float k = 0.0;
    float l = 0.0;
    int err = 0;
    for (int r = 0; r < count; ++r)
    {
      err = parseHKLRow(r, h,k,l);
      if (err >= 0) {
        values.push_back(h);
        values.push_back(k);
        values.push_back(l);
      }
    }
    return values;
  }
  return QVector<float > ();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AxisAngleTableModel::parseHKLRow(int row, float &h, float &k, float &l)
{
  QString hklStr = m_Axis[row];
  hklStr.chop(1); // remove the ">" charater from the end;
  hklStr.remove(0, 1); // Remove the front "<" character
  bool ok = false;
  h = hklStr.section(',', 0,0).toFloat(&ok);
  k = hklStr.section(',', 1,1).toFloat(&ok);
  l = hklStr.section(',', 2,2).toFloat(&ok);
  if (ok) { return 0; }
  return -1;
}


#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float AxisAngleTableModel::getDataValue(int col, int row)
{
  switch(col)
  {
    case Angle:
      return m_Angles[row];
    case Weight:
      return m_Weights[row];
    case Axis:
//      return m_Axis[row];
    default:
      Q_ASSERT(false);
  }
  return 0.0;
}
#endif
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AxisAngleTableModel::setColumnData(int col, QVector<float> &data)
{
  switch(col)
  {
    case Angle:
      m_Angles = data; break;
    case Axis:
//      m_Axis = data; break;
    default:
      Q_ASSERT(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AxisAngleTableModel::setTableData(QVector<float> angles, QVector<float> axis)
{
  qint32 count = angles.count();

  qint32 row = 0;
  // Remove all the current rows in the table model
  removeRows(0, rowCount());
  QModelIndex topLeft;
  QModelIndex botRight;

  if (count > 0) {
    // Now mass insert the data to the table then emit that the data has changed
    beginInsertRows(QModelIndex(), row, row + count - 1);
    m_Angles = angles;
    m_Axis.clear();
    int h,k,l;
    for (int i = 0; i < axis.size(); ++i)
    {
      h = axis[i];
      k = axis[++i];
      l = axis[++i];
      QString status = QString("<%1,%2,%3>").arg(h).arg(k).arg(l);
      m_Axis.push_back(status);
    }

    m_RowCount = count;
    endInsertRows();
    QModelIndex topLeft = createIndex(0, 0);
    QModelIndex botRight = createIndex(count-1, ColumnCount);
  }
  emit dataChanged(topLeft, botRight);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AxisAngleTableModel::setRowData(int row, float angle, QString axis)
{
   m_Angles[row] = angle;
   m_Axis[row] = QString::fromStdString(axis);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAbstractItemDelegate* AxisAngleTableModel::getItemDelegate()
{
  return new AxisAngleItemDelegate;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AxisAngleTableModel::setInitialValues()
{

}

