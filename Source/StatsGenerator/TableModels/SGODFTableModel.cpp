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
#include "SGODFTableModel.h"
#include <iostream>

#include <QApplication>
#include <QtGui/QStyleOptionComboBox>
#include <QtGui/QAbstractItemDelegate>

#include "StatsGenerator/Delegates/SGODFItemDelegate.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SGODFTableModel::SGODFTableModel(QObject* parent) :
QAbstractTableModel(parent),
m_RowCount(0)
{
  m_ColumnCount = ColumnCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SGODFTableModel::~SGODFTableModel()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::ItemFlags SGODFTableModel::flags(const QModelIndex &index) const
{
  //  std::cout << "SGODFTableModel::flags" << std::endl;
  if (!index.isValid())
  {
    return Qt::NoItemFlags;
  }
  Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
  if (index.isValid())
  {
  //  theFlags |= Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    int col = index.column();
    if (col == Texture)
    {
      theFlags = Qt::ItemIsEnabled;
    }
    else if (col == Weight)
    {
      theFlags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    else if (col == Sigma)
    {
      theFlags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }

  }
  return theFlags;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant SGODFTableModel::data(const QModelIndex &index, qint32 role) const
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
      case Texture:
      {
        comboBox.currentText = QString("101");
        const QString header = headerData(Texture, Qt::Horizontal, Qt::DisplayRole).toString();
        if (header.length() > comboBox.currentText.length()) comboBox.currentText = header;
        break;
      }
      case Weight:
      {
        comboBox.currentText = QString("10001");
        const QString header = headerData(Weight, Qt::Horizontal, Qt::DisplayRole).toString();
        if (header.length() > comboBox.currentText.length()) comboBox.currentText = header;
        break;
      }
      case Sigma:
      {
        comboBox.currentText = QString("10001");
        const QString header = headerData(Sigma, Qt::Horizontal, Qt::DisplayRole).toString();
        if (header.length() > comboBox.currentText.length()) comboBox.currentText = header;
        break;
      }
      default:
        Q_ASSERT(false);
    }
    QFontMetrics fontMetrics(data(index, Qt::FontRole) .value<QFont > ());
    comboBox.fontMetrics = fontMetrics;
    QSize size(fontMetrics.width(comboBox.currentText), fontMetrics.height());
    return qApp->style()->sizeFromContents(QStyle::CT_ComboBox, &comboBox, size);
  }
  else if (role == Qt::TextAlignmentRole)
  {
    return int(Qt::AlignRight | Qt::AlignVCenter);
  }
  else if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    int col = index.column();
    if (col == Texture)
    {
      return QVariant(m_Textures[index.row()]);
    }
    else if (col == Weight)
    {
      return QVariant(m_Weights[index.row()]);
    }
    else if (col == Sigma)
    {
      return QVariant(m_Sigmas[index.row()]);
    }
  }

  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant SGODFTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    switch(section)
    {
      case Texture:
        return QVariant(QString("Texture"));
        break;
      case Weight:
        return QVariant(QString("Weight"));
        break;
      case Sigma:
        return QVariant(QString("Sigma"));
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
int SGODFTableModel::rowCount(const QModelIndex &index) const
{
  return index.isValid() ? 0 : m_RowCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SGODFTableModel::columnCount(const QModelIndex &index) const
{
  return index.isValid() ? 0 : m_ColumnCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SGODFTableModel::setHeaderData(int col, Qt::Orientation o, const QVariant& var, int role)
{
  return false;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SGODFTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  // std::cout << "SGODFTableModel::setData " << value.toString().toStdString() << std::endl;
  if (!index.isValid() || role != Qt::EditRole || index.row() < 0 || index.row() >= m_Textures.count() || index.column() < 0 || index.column()
      >= m_ColumnCount)
  {
    return false;
  }
  bool ok;
  qint32 row = index.row();
  qint32 col = index.column();
  switch(col)
  {
    case Texture:
      m_Textures[row] = value.toString();
      break;
    case Weight:
      m_Weights[row] = value.toDouble(&ok);
      break;
    case Sigma:
      m_Sigmas[row] = value.toDouble(&ok);
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
bool SGODFTableModel::insertRows(int row, int count, const QModelIndex& index)
{
  QString texture("Unknown");
  double weight = 5.0;
  double sigma = 0.0;

  QString c("blue");

  beginInsertRows(QModelIndex(), row, row + count - 1);
  for (int i = 0; i < count; ++i)
  {
    m_Textures.append(texture);
    m_Weights.append(weight);
    m_Sigmas.append(sigma);
    m_RowCount = m_Textures.count();
  }
  endInsertRows();
  emit dataChanged(index, index);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SGODFTableModel::removeRows(int row, int count, const QModelIndex& index)
{
  if (count < 1)
  {
    return true;
  } // No Rows to remove
  beginRemoveRows(QModelIndex(), row, row + count - 1);
  for (int i = 0; i < count; ++i)
  {
    m_Textures.remove(row);
    m_Weights.remove(row);
    m_Sigmas.remove(row);
    m_RowCount = m_Textures.count();
  }
  endRemoveRows();
  emit dataChanged(index, index);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<double > SGODFTableModel::getData(int col)
{

  switch(col)
  {
    case Weight:
      return m_Weights;break;
    case Sigma:
      return m_Sigmas;break;
    default:
      Q_ASSERT(false);
  }
  return QVector<double > ();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double SGODFTableModel::getDataValue(int col, int row)
{
  switch(col)
  {
    case Weight:
      return m_Weights[row];break;
    case Sigma:
      return m_Sigmas[row]; break;
    default:
      Q_ASSERT(false);
  }
  return 0.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGODFTableModel::setColumnData(int col, QVector<double> &data)
{
  switch(col)
  {
    case Weight:
      m_Weights = data;break;
    case Sigma:
      m_Sigmas = data; break;
    default:
      Q_ASSERT(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAbstractItemDelegate* SGODFTableModel::getItemDelegate()
{
  return new SGODFItemDelegate;
}

#define ADD_INITIAL_ROW_VALUE(name, weight, sigma)\
    insertRow(rowCount());\
    QModelIndex textureIndex = index(rowCount() - 1, SGODFTableModel::Texture);\
    setData(textureIndex, QVariant(QString(name)), Qt::EditRole);\
    QModelIndex weightIndex = index(rowCount() - 1, SGODFTableModel::Weight);\
    setData(weightIndex, QVariant(weight), Qt::EditRole);\
    QModelIndex sigmaIndex = index(rowCount() - 1, SGODFTableModel::Sigma);\
    setData(sigmaIndex, QVariant(sigma), Qt::EditRole);\

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGODFTableModel::setInitialValues()
{
  size_t size = Texture::Count;

  ADD_INITIAL_ROW_VALUE("Random", 1.0, 0.0);

  for (size_t i = 0; i < size; ++i) {
    ADD_INITIAL_ROW_VALUE(Texture::Names[i], Texture::Weights[i], Texture::Sigmas[i]);
  }
}
