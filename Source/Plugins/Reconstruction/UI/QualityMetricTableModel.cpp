/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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

#include "QualityMetricTableModel.h"


#include <QApplication>
#include <QtGui/QStyleOptionComboBox>
#include <QtGui/QAbstractItemDelegate>

#include "QualityMetricItemDelegate.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QualityMetricTableModel::QualityMetricTableModel(QObject* parent) :
QAbstractTableModel(parent),
m_RowCount(0)
{
  m_ColumnCount = ColumnCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QualityMetricTableModel::~QualityMetricTableModel()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::ItemFlags QualityMetricTableModel::flags(const QModelIndex &index) const
{
  //  std::cout << "QualityMetricTableModel::flags" << std::endl;
  if (!index.isValid())
  {
    return Qt::NoItemFlags;
  }
  Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
  if (index.isValid())
  {
    theFlags |= Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    int col = index.column();
    if (col == FieldName)
    {
      theFlags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    else if (col == FieldValue)
    {
      theFlags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    else if (col == FieldOperator)
    {
      theFlags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }

  }
  return theFlags;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant QualityMetricTableModel::data(const QModelIndex &index, qint32 role) const
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
      case FieldName:
      {
        comboBox.currentText = QString("Confidence Index");
        const QString header = headerData(FieldName, Qt::Horizontal, Qt::DisplayRole).toString();
        if (header.length() > comboBox.currentText.length())
        {
          comboBox.currentText = header;
        }
        break;
      }
      case FieldValue:
      {
        comboBox.currentText = QString("0.005");
        const QString header = headerData(FieldName, Qt::Horizontal, Qt::DisplayRole).toString();
        if (header.length() > comboBox.currentText.length())
          {comboBox.currentText = header;}
        break;
      }
      case FieldOperator:
      {
        comboBox.currentText = QString(" > ");
        const QString header = headerData(FieldOperator, Qt::Horizontal, Qt::DisplayRole).toString();
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
    return qApp->style()->sizeFromContents(QStyle::CT_ComboBox, &comboBox, size);
  }
  else if (role == Qt::TextAlignmentRole)
  {
    return int(Qt::AlignRight | Qt::AlignVCenter);
  }
  else if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    int col = index.column();
    if (col == FieldName)
    {
      return QVariant(m_FieldNames[index.row()]);
    }
    else if (col == FieldValue)
    {
      return QVariant(m_FieldValues[index.row()]);
    }
    else if (col == FieldOperator)
    {
      return QVariant(m_FieldOperators[index.row()]);
    }
  }

  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant QualityMetricTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    switch(section)
    {
      case FieldName:
        return QVariant(QString("Field"));
        break;
      case FieldValue:
        return QVariant(QString("Value"));
        break;
      case FieldOperator:
        return QVariant(QString("Filter"));
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
int QualityMetricTableModel::rowCount(const QModelIndex &index) const
{
  return index.isValid() ? 0 : m_RowCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QualityMetricTableModel::columnCount(const QModelIndex &index) const
{
  return index.isValid() ? 0 : m_ColumnCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QualityMetricTableModel::setHeaderData(int col, Qt::Orientation o, const QVariant& var, int role)
{
  return false;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QualityMetricTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  // std::cout << "QualityMetricTableModel::setData " << value.toString().toStdString() << std::endl;
  if (!index.isValid() || role != Qt::EditRole || index.row() < 0 || index.row() >= m_FieldNames.count() || index.column() < 0 || index.column()
      >= m_ColumnCount)
  {
    return false;
  }
  bool ok;
  qint32 row = index.row();
  qint32 col = index.column();
  switch(col)
  {
    case FieldName:
      m_FieldNames[row] = value.toString();
      break;
    case FieldValue:
      m_FieldValues[row] = value.toFloat(&ok);
      break;
    case FieldOperator:
      m_FieldOperators[row] =  value.toString();
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
bool QualityMetricTableModel::insertRows(int row, int count, const QModelIndex& index)
{
  QString binNum = m_PossibleFields.at(0);
  float alpha = 0.0f;
  QString c = ">";

  beginInsertRows(QModelIndex(), row, row + count - 1);
  for (int i = 0; i < count; ++i)
  {
    m_FieldNames.append(binNum);
    m_FieldValues.append(alpha);
    m_FieldOperators.append(c);
    m_RowCount = m_FieldNames.count();
  }
  endInsertRows();
  emit
  dataChanged(index, index);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QualityMetricTableModel::removeRows(int row, int count, const QModelIndex& index)
{
  if (count < 1)
  {
    return true;
  } // No Rows to remove
  beginRemoveRows(QModelIndex(), row, row + count - 1);
  for (int i = 0; i < count; ++i)
  {
    m_FieldNames.remove(row);
    m_FieldValues.remove(row);
    m_FieldOperators.remove(row);
    m_RowCount = m_FieldNames.count();
  }
  endRemoveRows();
  emit
  dataChanged(index, index);
  return true;
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<float > QualityMetricTableModel::getData(int col)
{

  switch(col)
  {
    case FieldName:
      return m_FieldValues;break;
    case FieldValue:
      return m_K;break;
    case FieldOperator:
      return m_Beta;break;
    default:
      Q_ASSERT(false);
  }
  return QVector<float > ();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float QualityMetricTableModel::getDataValue(int col, int row)
{
  switch(col)
  {
    case FieldName:
      return m_FieldValues[row];break;
    case FieldValue:
      return m_K[row];break;
    case FieldOperator:
      return m_Beta[row];break;
    default:
      Q_ASSERT(false);
  }
  return 0.0f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QualityMetricTableModel::setColumnData(int col, QVector<float> &data)
{
  switch(col)
  {
    case FieldName:
      m_FieldValues = data;break;
    case FieldValue:
      m_K = data;break;
    case FieldOperator:
      m_Beta = data;break;
    default:
      Q_ASSERT(false);
  }
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QualityMetricTableModel::setTableData( QVector<QString> bins, QVector<float> data, QVector<QString> colors)
{
  qint32 count = bins.count();
  qint32 row = 0;
  // Remove all the current rows in the table model
  removeRows(0, rowCount());

  // Now mass insert the data to the table then emit that the data has changed
  beginInsertRows(QModelIndex(), row, row + count - 1);
  m_FieldNames = bins;
  m_FieldValues = data;
  m_FieldOperators = colors;
  m_RowCount = count;
  endInsertRows();
  QModelIndex topLeft = createIndex(0, 0);
  QModelIndex botRight = createIndex(count-1, ColumnCount);
  emit dataChanged(topLeft, botRight);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QualityMetricTableModel::getTableData( QVector<QString> &fieldNames, QVector<float> &fieldValues, QVector<QString> &fieldOperators)
{
  fieldNames = m_FieldNames;
  fieldValues = m_FieldValues;
  fieldOperators = m_FieldOperators;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAbstractItemDelegate* QualityMetricTableModel::getItemDelegate()
{
  return new QualityMetricItemDelegate(m_PossibleFields);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QualityMetricTableModel::setPossibleFields(QStringList fields)
{
  m_PossibleFields = fields;
  QualityMetricItemDelegate* dlg = qobject_cast<QualityMetricItemDelegate*>(getItemDelegate());
  if (dlg)
  {
    dlg->setFieldList(m_PossibleFields);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList QualityMetricTableModel::getPossibleFields()
{
  return m_PossibleFields;
}
