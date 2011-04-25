/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "SGMDFTableModel.h"

#include <iostream>

#include <QApplication>
#include <QtGui/QStyleOptionComboBox>
#include <QtGui/QAbstractItemDelegate>

#include "StatsGenerator/Delegates/SGMDFItemDelegate.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SGMDFTableModel::SGMDFTableModel(QObject* parent) :
QAbstractTableModel(parent),
m_RowCount(0)
{
  m_ColumnCount = ColumnCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SGMDFTableModel::~SGMDFTableModel()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::ItemFlags SGMDFTableModel::flags(const QModelIndex &index) const
{
  //  std::cout << "SGMDFTableModel::flags" << std::endl;
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
    else if (col == Weight)
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
QVariant SGMDFTableModel::data(const QModelIndex &index, qint32 role) const
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
      case Weight:
      {
        contents.setText( QString("11.") );
        const QString header = headerData(Weight, Qt::Horizontal, Qt::DisplayRole).toString();
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
    else if (col == Weight)
    {
      return QVariant(m_Weights[index.row()]);
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
QVariant SGMDFTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    switch(section)
    {
      case Angle:
        return QVariant(QString("Angle\n(w)"));
        break;
      case Weight:
        return QVariant(QString("Weight\n(MRD)"));
        break;
      case Axis:
        return QVariant(QString("Axis\n(<h,k,l>)"));
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
int SGMDFTableModel::rowCount(const QModelIndex &index) const
{
  return index.isValid() ? 0 : m_RowCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SGMDFTableModel::columnCount(const QModelIndex &index) const
{
  return index.isValid() ? 0 : m_ColumnCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SGMDFTableModel::setHeaderData(int col, Qt::Orientation o, const QVariant& var, int role)
{
  return false;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SGMDFTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  // std::cout << "SGMDFTableModel::setData " << value.toString().toStdString() << std::endl;
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
      m_Angles[row] = value.toDouble(&ok);
      break;
    case Axis:
      m_Axis[row] = value.toString();
      break;
    case Weight:
      m_Weights[row] = value.toDouble(&ok);
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
bool SGMDFTableModel::insertRows(int row, int count, const QModelIndex& index)
{
  QString axis("<0,0,1>");
  double weight = 0.0;
  double angle = 0.0;

  beginInsertRows(QModelIndex(), row, row + count - 1);
  for (int i = 0; i < count; ++i)
  {
    m_Angles.append(angle);
    m_Weights.append(weight);
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
bool SGMDFTableModel::removeRows(int row, int count, const QModelIndex& index)
{
  if (count < 1)
  {
    return true;
  } // No Rows to remove
  beginRemoveRows(QModelIndex(), row, row + count - 1);
  for (int i = 0; i < count; ++i)
  {
    m_Angles.remove(row);
    m_Weights.remove(row);
    m_Axis.remove(row);
    m_RowCount = m_Angles.count();
  }
  endRemoveRows();
  emit dataChanged(index, index);
  return true;
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<double > SGMDFTableModel::getData(int col)
{

  switch(col)
  {
    case Angle:
      return m_Angles;
    case Weight:
      return m_Weights;
    case Axis:
      return m_Axis;
    default:
      Q_ASSERT(false);
  }
  return QVector<double > ();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double SGMDFTableModel::getDataValue(int col, int row)
{
  switch(col)
  {
    case Angle:
      return m_Angles[row];
    case Weight:
      return m_Weights[row];
    case Axis:
      return m_Axis[row];
    default:
      Q_ASSERT(false);
  }
  return 0.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGMDFTableModel::setColumnData(int col, QVector<double> &data)
{
  switch(col)
  {
    case Angle:
      m_Angles = data; break;
    case Weight:
      m_Weights = data;break;
    case Axis:
      m_Axis = data; break;
    default:
      Q_ASSERT(false);
  }
}
#endif
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAbstractItemDelegate* SGMDFTableModel::getItemDelegate()
{
  return new SGMDFItemDelegate;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGMDFTableModel::setInitialValues()
{

}

