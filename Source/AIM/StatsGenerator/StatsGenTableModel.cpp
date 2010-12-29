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
int StatsGenTableModel::rowCount(const QModelIndex &index) const
{
  return m_RowCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsGenTableModel::columnCount(const QModelIndex &index) const
{
  return m_ColumnCount;
}

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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant StatsGenTableModel::data(const QModelIndex &index, qint32 role) const
{

  if (! index.isValid())
  {
    return QVariant();
  }

  if (role == Qt::TextAlignmentRole) {
    return int(Qt::AlignRight | Qt::AlignVCenter);
  }
  else if (role == Qt::DisplayRole)
  {

    int col = index.column();
    if (col == 0)
    {
     return QVariant(m_BinNumbers[index.row()]);
    }
    else if (col == 1)
    {
      return QVariant(m_NumGrains[index.row()]);
    }
    else if (col == 2)
    {
      return QVariant(m_Mu[index.row()]);
    }
    else if (col == 3)
    {
      return QVariant(m_Sigma[index.row()]);
    }
    else if (col == 4)
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
      case 0:
        return QVariant(QString("Bin"));
        break;
      case 1:
        return QVariant(QString("Num Grains"));
        break;
      case 2:
        return QVariant(QString("Average"));
        break;
      case 3:
        return QVariant(QString("Std Dev"));
        break;
      case 4:
        return QVariant(QString("Color"));
        break;
      default:
        break;
    }

  }
  return QVariant();
}

