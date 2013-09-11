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

#include "ComparisonSelectionTableModel.h"


#include <QApplication>
#include <QtGui/QStyleOptionComboBox>
#include <QtGui/QAbstractItemDelegate>

#include "ComparisonSelectionItemDelegate.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonSelectionTableModel::ComparisonSelectionTableModel(bool showOperators, QObject* parent) :
QAbstractTableModel(parent),
m_RowCount(0),
m_NumberOfPhases(1)
{
  if (showOperators)
  {
    m_ColumnCount = ColumnCount;
  }
  else
  {
    m_ColumnCount = 1;
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonSelectionTableModel::~ComparisonSelectionTableModel()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::ItemFlags ComparisonSelectionTableModel::flags(const QModelIndex &index) const
{
  //  std::cout << "ComparisonSelectionTableModel::flags" << std::endl;
  if (!index.isValid())
  {
    return Qt::NoItemFlags;
  }
  Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
  if (index.isValid())
  {
    // theFlags |= Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    int col = index.column();
    if (col == FieldName || col == FieldValue || col == FieldOperator)
    {
      theFlags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
//    else if ( col == FieldPhaseValue)
//    {
//      if (m_NumberOfPhases > 1) {
//        theFlags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
//      }
//    }
  }
  return theFlags;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant ComparisonSelectionTableModel::data(const QModelIndex &index, qint32 role) const
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
//      case FieldPhaseValue:
//      {
//        comboBox.currentText = QString("1");
//        const QString header = headerData(FieldPhaseValue, Qt::Horizontal, Qt::DisplayRole).toString();
//        if (header.length() > comboBox.currentText.length())
//        {
//          comboBox.currentText = header;
//        }
//        break;
//      }
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
//    else if (col == FieldPhaseValue)
//    {
//      return QVariant(m_FieldPhaseValues[index.row()]);
//    }
  }

  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant ComparisonSelectionTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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
//      case FieldPhaseValue:
//        return QVariant(QString("Phase"));
//        break;
      default:
        break;
    }

  }
  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ComparisonSelectionTableModel::rowCount(const QModelIndex &index) const
{
  return index.isValid() ? 0 : m_RowCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ComparisonSelectionTableModel::columnCount(const QModelIndex &index) const
{
  return index.isValid() ? 0 : m_ColumnCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ComparisonSelectionTableModel::setHeaderData(int col, Qt::Orientation o, const QVariant& var, int role)
{
  return false;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ComparisonSelectionTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  // std::cout << "ComparisonSelectionTableModel::setData " << value.toString().toStdString() << std::endl;
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
//    case FieldPhaseValue:
//      m_FieldPhaseValues[row] =  value.toInt(&ok);
//      break;
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
bool ComparisonSelectionTableModel::insertRows(int row, int count, const QModelIndex& index)
{
  if (m_PossibleFields.size() < 1) { return false;}
  QString fieldName = m_PossibleFields.at(0);
  float fieldValue = 0.0f;
  QString fieldOperator = ">";
  //int fieldPhaseValue = 1;

  beginInsertRows(QModelIndex(), row, row + count - 1);
  for (int i = 0; i < count; ++i)
  {
    m_FieldNames.append(fieldName);
    m_FieldValues.append(fieldValue);
    m_FieldOperators.append(fieldOperator);
    //m_FieldPhaseValues.append(fieldPhaseValue);
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
bool ComparisonSelectionTableModel::removeRows(int row, int count, const QModelIndex& index)
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
  //  m_FieldPhaseValues.remove(row);
    m_RowCount = m_FieldNames.count();
  }
  endRemoveRows();
  emit
  dataChanged(index, index);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionTableModel::setTableData(QVector<QString> fieldNames,
                                                 QVector<float> fieldValues,
                                                 QVector<int> fieldOperators)
{
  qint32 count = fieldNames.count();
  qint32 row = 0;
  // Remove all the current rows in the table model
  removeRows(0, rowCount());
  // Check to make sure we have data to insert.
  if (count == 0) { return; }
  // Now mass insert the data to the table then emit that the data has changed
  beginInsertRows(QModelIndex(), row, row + count - 1);
  m_FieldNames = fieldNames;
  m_FieldValues = fieldValues;

  m_FieldOperators.resize(fieldOperators.size());
  for(int i = 0; i < m_FieldOperators.size(); ++i)
  {
    if (fieldOperators[i] == DREAM3D::Comparison::Operator_LessThan) m_FieldOperators[i] = QString::fromStdString(DREAM3D::Comparison::Strings::LessThan);
    if (fieldOperators[i] == DREAM3D::Comparison::Operator_GreaterThan) m_FieldOperators[i] = QString::fromStdString(DREAM3D::Comparison::Strings::GreaterThan);
    if (fieldOperators[i] == DREAM3D::Comparison::Operator_Equal) m_FieldOperators[i] = QString::fromStdString(DREAM3D::Comparison::Strings::Equal);
  }
  m_RowCount = count;
  endInsertRows();
  QModelIndex topLeft = createIndex(0, 0);
  QModelIndex botRight = createIndex(count-1, ColumnCount);
  emit dataChanged(topLeft, botRight);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionTableModel::getTableData( QVector<QString> &fieldNames, QVector<float> &fieldValues,  QVector<int> &fieldOperators)
{
  fieldNames = m_FieldNames;
  fieldValues = m_FieldValues;
  fieldOperators.resize(m_FieldOperators.size());
  for(int i = 0; i < m_FieldOperators.size(); ++i)
  {
    if (m_FieldOperators[i].compare(QString::fromStdString(DREAM3D::Comparison::Strings::LessThan)) == 0) fieldOperators[i] = DREAM3D::Comparison::Operator_LessThan;
    else if (m_FieldOperators[i].compare(QString::fromStdString(DREAM3D::Comparison::Strings::GreaterThan)) == 0) fieldOperators[i] = DREAM3D::Comparison::Operator_GreaterThan;
    else if (m_FieldOperators[i].compare(QString::fromStdString(DREAM3D::Comparison::Strings::Equal)) == 0) fieldOperators[i] = DREAM3D::Comparison::Operator_Equal;
    else fieldOperators[i] = DREAM3D::Comparison::Operator_Unknown;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionTableModel::setNumberOfPhases(int n)
{
  m_NumberOfPhases = n;
  ComparisonSelectionItemDelegate* dlg = qobject_cast<ComparisonSelectionItemDelegate*>(getItemDelegate());
  if (dlg)
  {
    dlg->setNumberOfPhases(n);
    delete dlg;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAbstractItemDelegate* ComparisonSelectionTableModel::getItemDelegate()
{
  return new ComparisonSelectionItemDelegate(m_PossibleFields, m_NumberOfPhases);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionTableModel::setPossibleFields(QStringList fields)
{
  m_PossibleFields = fields;

  qint32 count = m_FieldNames.count();
  for(qint32 i = 0; i < count; ++i)
  {
    if (m_PossibleFields.contains(m_FieldNames.at(i)) == false)
    {
      m_FieldNames[i] = m_PossibleFields[0]; // Just set it to the first index
    }
  }

  ComparisonSelectionItemDelegate* dlg = qobject_cast<ComparisonSelectionItemDelegate*>(getItemDelegate());
  if (dlg)
  {
    dlg->setFieldList(m_PossibleFields);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionTableModel::setPossibleFields(QList<QString> &fields)
{
  m_PossibleFields.clear();
  for(QList<QString>::iterator iter = fields.begin(); iter != fields.end(); ++iter)
  {
    m_PossibleFields << QString::fromStdString(*iter);
  }

  // Now check to make sure any selected array names are either still there or if they have been
  // removed we need to default select the first array name
  qint32 count = m_FieldNames.count();
  for(qint32 i = 0; i < count; ++i)
  {
    if (m_PossibleFields.contains(m_FieldNames.at(i)) == false && m_PossibleFields.count() > 0)
    {
      m_FieldNames[i] = m_PossibleFields[0]; // Just set it to the first index
    }
  }


  ComparisonSelectionItemDelegate* dlg = qobject_cast<ComparisonSelectionItemDelegate*>(getItemDelegate());
  if (dlg)
  {
    dlg->setFieldList(m_PossibleFields);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList ComparisonSelectionTableModel::getPossibleFields()
{
  return m_PossibleFields;
}
