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

#include "StatsGenTableModel.h"
#include <iostream>

#include <QApplication>
#include <QtGui/QStyleOptionComboBox>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenTableModel::StatsGenTableModel(QObject* parent) :
QAbstractTableModel(parent),
m_ColumnCount(5),
m_RowCount(0)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenTableModel::~StatsGenTableModel()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::ItemFlags StatsGenTableModel::flags(const QModelIndex &index) const
{
  //  std::cout << "StatsGenTableModel::flags" << std::endl;
    if (! index.isValid())
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
    else if (col == NumGrains)
    {
      theFlags = Qt::ItemIsEnabled;
    }
    else if (col == Mu)
    {
      theFlags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    else if (col == Sigma)
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
QVariant StatsGenTableModel::data(const QModelIndex &index, qint32 role) const
{

  if (! index.isValid())
  {
    return QVariant();
  }

  if (role == Qt::SizeHintRole) {
      QStyleOptionComboBox comboBox;

      switch (index.column())
      {
          case BinNumber:
          {
              comboBox.currentText = QString("1");
              const QString header = headerData(BinNumber,
                      Qt::Horizontal, Qt::DisplayRole).toString();
              if (header.length() > comboBox.currentText.length())
                  comboBox.currentText = header;
              break;
          }
          case NumGrains:
          {
              comboBox.currentText = QString("1");
              const QString header = headerData(BinNumber,
                      Qt::Horizontal, Qt::DisplayRole).toString();
              if (header.length() > comboBox.currentText.length())
                  comboBox.currentText = header;
              break;
          }
          case Mu:
          {
            comboBox.currentText = QString("11");
              const QString header = headerData(BinNumber,
                      Qt::Horizontal, Qt::DisplayRole).toString();
              if (header.length() > comboBox.currentText.length())
                  comboBox.currentText = header;
              break;
          }
          case Sigma:
          {
            comboBox.currentText = QString("11");
              const QString header = headerData(BinNumber,
                      Qt::Horizontal, Qt::DisplayRole).toString();
              if (header.length() > comboBox.currentText.length())
                  comboBox.currentText = header;
              break;
          }
          case LineColor:
          {
            comboBox.currentText = QString("Dark Blue");
            const QString header = headerData(BinNumber, Qt::Horizontal, Qt::DisplayRole).toString();
            if (header.length() > comboBox.currentText.length())
            {
              comboBox.currentText = header;
            }
            break;
          }
          default: Q_ASSERT(false);
      }
      QFontMetrics fontMetrics(data(index, Qt::FontRole)
                               .value<QFont>());
      comboBox.fontMetrics = fontMetrics;
      QSize size(fontMetrics.width(comboBox.currentText),
                 fontMetrics.height());
      return qApp->style()->sizeFromContents(QStyle::CT_ComboBox,
                                             &comboBox, size);
  }
  else if (role == Qt::TextAlignmentRole) {
    return int(Qt::AlignRight | Qt::AlignVCenter);
  }
  else if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    int col = index.column();
    if (col == BinNumber)
    {
     return QVariant(m_BinNumbers[index.row()]);
    }
    else if (col == NumGrains)
    {
      return QVariant(m_NumGrains[index.row()]);
    }
    else if (col == Mu)
    {
      return QVariant(m_Mu[index.row()]);
    }
    else if (col == Sigma)
    {
      return QVariant(m_Sigma[index.row()]);
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
QVariant  StatsGenTableModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    switch(section)
    {
      case BinNumber:
        return QVariant(QString("Bin"));
        break;
      case NumGrains:
        return QVariant(QString("Num Grains"));
        break;
      case Mu:
        return QVariant(QString("Average"));
        break;
      case Sigma:
        return QVariant(QString("Std Dev"));
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
int StatsGenTableModel::rowCount(const QModelIndex &index) const
{
  return index.isValid() ? 0 : m_RowCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsGenTableModel::columnCount(const QModelIndex &index) const
{
  return index.isValid() ? 0 : m_ColumnCount;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool StatsGenTableModel::setData ( const QModelIndex & index, const QVariant & value, int role)
{
 // std::cout << "StatsGenTableModel::setData " << value.toString().toStdString() << std::endl;
  if (!index.isValid() || role != Qt::EditRole ||
      index.row() < 0 || index.row() >= m_BinNumbers.count() ||
      index.column() < 0 || index.column() >= m_ColumnCount) {
      return false;
  }
  bool ok;
  qint32 row = index.row();
  qint32 col = index.column();
  switch (col) {
    case BinNumber:
      m_BinNumbers[row] = value.toInt(&ok);
      break;
    case NumGrains:
      m_NumGrains[row] = value.toInt(&ok);
      break;
    case Mu:
      m_Mu[row] = value.toDouble(&ok);
      break;
    case Sigma:
      m_Sigma[row] = value.toDouble(&ok);
      break;
    case LineColor:
      m_Colors[row] = value.toString();
      break;
    default: Q_ASSERT(false);

  }

  emit dataChanged(index, index);
  return true;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool StatsGenTableModel::insertRows(int row, int count, const QModelIndex& index)
{
  qint32 binNum = 0;
  qint32 nGrains = 0;
  double mu = 1.0;
  double sigma = 0.5;
  QString c ("blue");

  beginInsertRows(QModelIndex(), row, row + count - 1);
  for (int i = 0; i < count; ++i) {
    m_BinNumbers.append(binNum);
    m_NumGrains.append(nGrains);
    m_Mu.append(mu);
    m_Sigma.append(sigma);
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
bool StatsGenTableModel::removeRows(int row, int count, const QModelIndex& index)
{
  if (count < 1) { return true; } // No Rows to remove
    beginRemoveRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; ++i) {
      m_BinNumbers.remove(row);
      m_NumGrains.remove(row);
      m_Mu.remove(row);
      m_Sigma.remove(row);
      m_Colors.remove(row);
      m_RowCount = m_BinNumbers.count();
    }
    endRemoveRows();
    emit dataChanged(index, index);
    return true;
}




#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenTableModel::addRow(qint32 binNum, qint32 nGrains, double mu, double sigma, QColor color)
{
  std::cout << "StatsGenTableModel::addRow" << std::endl;
  m_BinNumbers.append(binNum);
  m_NumGrains.append(nGrains);
  m_Mu.append(mu);
  m_Sigma.append(sigma);
  m_Colors.append(color);
  m_RowCount = m_BinNumbers.count();
  emit layoutChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenTableModel::deleteRow(qint32 row)
{
  m_BinNumbers.remove(row);
  m_NumGrains.remove(row);
  m_Mu.remove(row);
  m_Sigma.remove(row);
  m_Colors.remove(row);
  m_RowCount = m_BinNumbers.count();
  emit layoutChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenTableModel::updateRow(qint32 row, qint32 binNum, qint32 nGrains, double mu, double sigma, QColor color)
{
  std::cout << "StatsGenTableModel::updateRow" << std::endl;
  m_BinNumbers[row] = binNum;
  m_NumGrains[row] = nGrains;
  m_Mu[row] = mu;
  m_Sigma[row] = sigma;
  m_Colors[row] = color;
  QModelIndex index0 = createIndex(row, 0);
  QModelIndex index1 = createIndex(row, m_ColumnCount);
  emit dataChanged(index0, index1);
}
#endif

