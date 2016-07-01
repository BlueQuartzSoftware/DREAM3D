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

#include "SGODFTableModel.h"
#include <iostream>

#include <QtWidgets/QStyleOptionComboBox>
#include <QtWidgets/QAbstractItemDelegate>

#include "Applications/SIMPLView/SIMPLViewApplication.h"

#include "StatsGenerator/Widgets/Delegates/SGODFItemDelegate.h"
// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_SGODFTableModel.cpp"
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SGODFTableModel::SGODFTableModel(QObject* parent) :
  QAbstractTableModel(parent),
  m_RowCount(0),
  m_CrystalStructure(Ebsd::CrystalStructure::Cubic_High)
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
Qt::ItemFlags SGODFTableModel::flags(const QModelIndex& index) const
{
  //  qDebug() << "SGODFTableModel::flags" << "\n";
  if (!index.isValid())
  {
    return Qt::NoItemFlags;
  }
  Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
  if (index.isValid())
  {
    int col = index.column();
    if (col < SGODFTableModel::ColumnCount)
    {
      theFlags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
  }
  return theFlags;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant SGODFTableModel::data(const QModelIndex& index, qint32 role) const
{

  if (!index.isValid())
  {
    return QVariant();
  }

  if (role == Qt::SizeHintRole)
  {
    QStyleOptionComboBox comboBox;
    comboBox.currentText = QString("10001");
    if (index.column() < SGODFTableModel::ColumnCount)
    {
      const QString header = headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();
      if (header.length() > comboBox.currentText.length()) { comboBox.currentText = header; }
    }
    else
    {
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
    if (col == Euler1)
    {
      return QVariant(m_Euler1s[index.row()]);
    }
    else if (col == Euler2)
    {
      return QVariant(m_Euler2s[index.row()]);
    }
    else if (col == Euler3)
    {
      return QVariant(m_Euler3s[index.row()]);
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
      case Euler1:
        return QVariant(QString("Euler 1"));
        break;
      case Euler2:
        return QVariant(QString("Euler 2"));
        break;
      case Euler3:
        return QVariant(QString("Euler 3"));
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
int SGODFTableModel::rowCount(const QModelIndex& index) const
{
  return index.isValid() ? 0 : m_RowCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SGODFTableModel::columnCount(const QModelIndex& index) const
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
bool SGODFTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  // qDebug() << "SGODFTableModel::setData " << value.toString() << "\n";
  if (!index.isValid() || role != Qt::EditRole || index.row() < 0
      || index.row() >= m_Euler1s.count()
      || index.column() < 0 || index.column()
      >= m_ColumnCount)
  {
    return false;
  }
  bool ok;
  qint32 row = index.row();
  qint32 col = index.column();
  switch(col)
  {
    case Euler1:
      m_Euler1s[row] = value.toFloat(&ok);
      break;
    case Euler2:
      m_Euler2s[row] = value.toFloat(&ok);
      break;
    case Euler3:
      m_Euler3s[row] = value.toFloat(&ok);
      break;
    case Weight:
      m_Weights[row] = value.toFloat(&ok);
      break;
    case Sigma:
      m_Sigmas[row] = value.toFloat(&ok);
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
  float e1 = 0.0;
  float e2 = 0.0;
  float e3 = 0.0;
  float weight = 1.0;
  float sigma = 1.0;

  QString c("blue");

  beginInsertRows(QModelIndex(), row, row + count - 1);
  for (int i = 0; i < count; ++i)
  {
    m_Euler1s.append(e1);
    m_Euler2s.append(e2);
    m_Euler3s.append(e3);
    m_Weights.append(weight);
    m_Sigmas.append(sigma);
    m_RowCount = m_Euler1s.count();
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
    m_Euler1s.remove(row);
    m_Euler2s.remove(row);
    m_Euler3s.remove(row);
    m_Weights.remove(row);
    m_Sigmas.remove(row);
    m_RowCount = m_Euler1s.count();
  }
  endRemoveRows();
  emit dataChanged(index, index);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<float > SGODFTableModel::getData(int col)
{

  switch(col)
  {
    case Euler1:
      return m_Euler1s;
      break;
    case Euler2:
      return m_Euler2s;
      break;
    case Euler3:
      return m_Euler3s;
      break;
    case Weight:
      return m_Weights;
      break;
    case Sigma:
      return m_Sigmas;
      break;
    default:
      Q_ASSERT(false);
  }
  return QVector<float > ();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float SGODFTableModel::getDataValue(int col, int row)
{
  switch(col)
  {
    case Euler1:
      return m_Euler1s[row];
      break;
    case Euler2:
      return m_Euler2s[row];
      break;
    case Euler3:
      return m_Euler3s[row];
      break;
    case Weight:
      return m_Weights[row];
      break;
    case Sigma:
      return m_Sigmas[row];
      break;
    default:
      Q_ASSERT(false);
  }
  return 0.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGODFTableModel::setColumnData(int col, QVector<float>& data)
{
  switch(col)
  {
    case Euler1:
      m_Euler1s = data;
      break;
    case Euler2:
      m_Euler2s = data;
      break;
    case Euler3:
      m_Euler3s = data;
      break;
    case Weight:
      m_Weights = data;
      break;
    case Sigma:
      m_Sigmas = data;
      break;
    default:
      Q_ASSERT(false);
  }
  m_RowCount = data.size();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGODFTableModel::setRowData(int row, float e1, float e2, float e3, float weight, float sigma)
{
  m_Euler1s[row] = e1;
  m_Euler2s[row] = e2;
  m_Euler3s[row] = e3;
  m_Weights[row] = weight;
  m_Sigmas[row] = sigma;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAbstractItemDelegate* SGODFTableModel::getItemDelegate()
{
  return new SGODFItemDelegate(m_CrystalStructure);
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

  }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
  void SGODFTableModel::setTableData(QVector<float> e1, QVector<float> e2, QVector<float> e3, QVector<float> weights, QVector<float> sigmas)
  {
    qint32 count = e1.count();

    qint32 row = 0;
    // Remove all the current rows in the table model
    removeRows(0, rowCount());
    QModelIndex topLeft;
    QModelIndex botRight;
    if (count >= 1)
    {
      // Now mass insert the data to the table then emit that the data has changed
      beginInsertRows(QModelIndex(), row, row + count - 1);
      m_Euler1s = e1;
      m_Euler2s = e2;
      m_Euler3s = e3;
      m_Weights = weights;
      m_Sigmas = sigmas;
      m_RowCount = count;
      endInsertRows();
      createIndex(0, 0);
      createIndex(count - 1, ColumnCount);
    }
    emit dataChanged(topLeft, botRight);
  }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
  void SGODFTableModel::setCrystalStructure(unsigned int value)
  {
    if (m_CrystalStructure != value)
    {
      this->m_CrystalStructure = value;
    }
  }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
  unsigned int SGODFTableModel::getCrystalStructure()
  {
    return m_CrystalStructure;
  }



