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

#include "SGMDFTableModel.h"

#include <iostream>
#include <array>

#include <QtCore/QTextStream>

#include <QtWidgets/QAbstractItemDelegate>
#include <QtWidgets/QApplication>

#include "SIMPLib/Common/QtBackwardCompatibilityMacro.h"

#include "SyntheticBuilding/Gui/Widgets/Delegates/SGMDFItemDelegate.h"

std::array<float, 3> parseHKLRow(const QString& row)
{

  QString hklStr = row;
  float h = std::numeric_limits<float>::infinity();
  float k = std::numeric_limits<float>::infinity();
  float l = std::numeric_limits<float>::infinity();
  hklStr.chop(1);      // remove the ">" charater from the end;
  hklStr.remove(0, 1); // Remove the front "<" character
  bool ok = false;
  h = hklStr.section(',', 0, 0).toFloat(&ok);
  if(!ok)
  {
    h = std::numeric_limits<float>::infinity();
  }
  k = hklStr.section(',', 1, 1).toFloat(&ok);
  if(!ok)
  {
    k = std::numeric_limits<float>::infinity();
  }
  l = hklStr.section(',', 2, 2).toFloat(&ok);
  if(!ok)
  {
    l = std::numeric_limits<float>::infinity();
  }

  return {h,k,l};
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SGMDFTableModel::SGMDFTableModel(QObject* parent)
: QAbstractTableModel(parent)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SGMDFTableModel::~SGMDFTableModel() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::ItemFlags SGMDFTableModel::flags(const QModelIndex& index) const
{
  //  qDebug() << "SGMDFTableModel::flags" << "\n";
  if(!index.isValid())
  {
    return Qt::NoItemFlags;
  }
  Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
  if(index.isValid())
  {
    //  theFlags |= Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    int col = index.column();
    if(col == Angle)
    {
      theFlags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    else if(col == Weight)
    {
      theFlags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    else if(col == Axis)
    {
      theFlags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
  }
  return theFlags;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant SGMDFTableModel::data(const QModelIndex& index, qint32 role) const
{

  if(!index.isValid())
  {
    return QVariant();
  }

  if(role == Qt::SizeHintRole)
  {
    QStyleOptionFrame comboBox;
    QLineEdit contents("1");
    switch(index.column())
    {
    case Angle: {
      contents.setText(QString("11."));
      const QString header = headerData(Angle, Qt::Horizontal, Qt::DisplayRole).toString();
      if(header.length() > contents.text().length())
      {
        contents.text() = header;
      }
      break;
    }
    case Axis: {
      contents.setText(QString("1,1,1"));
      const QString header = headerData(Axis, Qt::Horizontal, Qt::DisplayRole).toString();
      if(header.length() > contents.text().length())
      {
        contents.text() = header;
      }
      break;
    }
    case Weight: {
      contents.setText(QString("11."));
      const QString header = headerData(Weight, Qt::Horizontal, Qt::DisplayRole).toString();
      if(header.length() > contents.text().length())
      {
        contents.text() = header;
      }
      break;
    }
    default:
      Q_ASSERT(false);
    }
    QFontMetrics fontMetrics(data(index, Qt::FontRole).value<QFont>());
    comboBox.fontMetrics = fontMetrics;
    QSize size(fontMetrics.QFONTMETRICS_WIDTH(contents.text()), fontMetrics.height());
    return qApp->style()->sizeFromContents(QStyle::CT_ComboBox, &comboBox, size);
  }
  if(role == Qt::TextAlignmentRole)
  {
    return int(Qt::AlignRight | Qt::AlignVCenter);
  }
  if(role == Qt::DisplayRole || role == Qt::EditRole)
  {
    int col = index.column();
    if(col == Angle)
    {
      return QVariant(m_Angles[index.row()]);
    }
    if(col == Weight)
    {
      return QVariant(m_Weights[index.row()]);
    }
    if(col == Axis)
    {
      QLocale loc = QLocale::system();
      QChar decimalPoint = loc.decimalPoint();
      QChar separatorPoint = ',';
      if(decimalPoint == ',')
      {
        separatorPoint = '.';
      }
      int32_t row = index.row();

      QString display;

      QTextStream out(&display);
      out << "<" << loc.toString(m_Axis[row * 3]) << separatorPoint << " " << loc.toString(m_Axis[row * 3 + 1]) << separatorPoint << " " << loc.toString(m_Axis[row * 3 + 2]) << ">";
      return QVariant(display);
    }
  }

  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant SGMDFTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    switch(section)
    {
    case Angle:
      return QVariant(QString("Angle\n(w) Degrees"));
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
int SGMDFTableModel::rowCount(const QModelIndex& parent) const
{
  return parent.isValid() ? 0 : m_RowCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SGMDFTableModel::columnCount(const QModelIndex& parent) const
{
  return parent.isValid() ? 0 : m_ColumnCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SGMDFTableModel::setHeaderData(int col, Qt::Orientation orientation, const QVariant& data, int role)
{
  std::ignore = col;
  std::ignore = orientation;
  std::ignore = data;
  std::ignore = role;
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SGMDFTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  // qDebug() << "SGMDFTableModel::setData " << value.toString() << "\n";
  if(!index.isValid() || role != Qt::EditRole || index.row() < 0 || index.row() >= m_Angles.count() || index.column() < 0 || index.column() >= m_ColumnCount)
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
    {
    std::array<float, 3> hkl = parseHKLRow(value.toString());
    m_Axis[row*3] = hkl[0];
    m_Axis[row*3+1] = hkl[1];
    m_Axis[row*3+2] = hkl[2];
    break;
    }
  case Weight:
    m_Weights[row] = value.toFloat(&ok);
    break;

  default:
    Q_ASSERT(false);
  }

  Q_EMIT dataChanged(index, index);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SGMDFTableModel::insertRows(int row, int count, const QModelIndex& parent)
{
  // QString axis("<0,0,1>");
  float weight = 50000.0; // Use a heavy default weight to show the user a major change in the MDF
  float angle = 0.0;

  beginInsertRows(QModelIndex(), row, row + count - 1);
  for(int i = 0; i < count; ++i)
  {
    m_Angles.append(angle);
    m_Weights.append(weight);
    m_Axis.append(0.0F);
    m_Axis.append(0.0F);
    m_Axis.append(1.0F);
    m_RowCount = m_Angles.count();
  }
  endInsertRows();
  Q_EMIT dataChanged(parent, parent);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SGMDFTableModel::removeRows(int row, int count, const QModelIndex& parent)
{
  if(count < 1)
  {
    return true;
  } // No Rows to remove
  beginRemoveRows(QModelIndex(), row, row + count - 1);
  for(int i = 0; i < count; ++i)
  {
    m_Angles.remove(row);
    m_Weights.remove(row);
    m_Axis.remove(row * 3);
    m_Axis.remove(row * 3);
    m_Axis.remove(row * 3);
    m_RowCount = m_Angles.count();
  }
  endRemoveRows();
  Q_EMIT dataChanged(parent, parent);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<float> SGMDFTableModel::getData(int col)
{
  std::vector<float> data;
  if(col == Angle)
  {
    return std::vector<float>(m_Angles.begin(), m_Angles.end());
  }
  if(col == Weight)
  {
    return std::vector<float>(m_Weights.begin(), m_Weights.end());
  }
  if(col == Axis)
  {
    return std::vector<float>(m_Axis.begin(), m_Axis.end());
  }
  return data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGMDFTableModel::setColumnData(int col, const QVector<float>& data)
{
  switch(col)
  {
  case Angle:
    m_Angles = data;
    break;
  case Weight:
    m_Weights = data;
    break;
  case Axis:
  //      m_Axis = data; break;
  default:
    Q_ASSERT(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGMDFTableModel::setTableData(const QVector<float>& angles, const QVector<float>& axis, const QVector<float>& weights)
{
  qint32 count = angles.count();

  qint32 row = 0;
  // Remove all the current rows in the table model
  removeRows(0, rowCount());
  QModelIndex topLeft;
  QModelIndex botRight;

  if(count > 0)
  {
    // Now mass insert the data to the table then Q_EMIT that the data has changed
    beginInsertRows(QModelIndex(), row, row + count - 1);
    m_Angles = angles;
    m_Weights = weights;
    m_Axis = axis;
#if 0
    m_Axis.clear();
    float h = 0.0f, k = 0.0f, l = 0.0f;
    for(int i = 0; i < axis.size(); ++i)
    {
      h = axis[i];
      k = axis[++i];
      l = axis[++i];
      QString status = QString("<%1,%2,%3>").arg(h).arg(k).arg(l);
      m_Axis.push_back(status);
    }
#endif

    m_RowCount = count;
    endInsertRows();
    createIndex(0, 0);
    createIndex(count - 1, ColumnCount);
  }
  Q_EMIT dataChanged(topLeft, botRight);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGMDFTableModel::setRowData(int row, float angle, float* const axis, float weight)
{
  m_Angles[row] = angle;
  m_Axis[row * 3] = axis[0];
  m_Axis[row * 3 + 1] = axis[1];
  m_Axis[row * 3 + 2] = axis[2];
  m_Weights[row] = weight;
}

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
