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

#include "QualityMetricTableModel.h"


#include <QApplication>
#include <QtGui/QStyleOptionComboBox>
#include <QtGui/QAbstractItemDelegate>

#include "QualityMetricItemDelegate.h"

#include "IOFiltersWidgets/moc_QualityMetricTableModel.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QualityMetricTableModel::QualityMetricTableModel(QObject* parent) :
  QAbstractTableModel(parent),
  m_RowCount(0),
  m_NumberOfPhases(1)
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
Qt::ItemFlags QualityMetricTableModel::flags(const QModelIndex& index) const
{
  //  qDebug() << "QualityMetricTableModel::flags" << "\n";
  if (!index.isValid())
  {
    return Qt::NoItemFlags;
  }
  Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
  if (index.isValid())
  {
    // theFlags |= Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    int col = index.column();
    if (col == FeatureName || col == FeatureValue || col == FeatureOperator)
    {
      theFlags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    else if ( col == FeaturePhaseValue)
    {
      if (m_NumberOfPhases > 1)
      {
        theFlags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
      }
    }
  }
  return theFlags;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant QualityMetricTableModel::data(const QModelIndex& index, qint32 role) const
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
      case FeatureName:
      {
        comboBox.currentText = QString("Confidence Index");
        const QString header = headerData(FeatureName, Qt::Horizontal, Qt::DisplayRole).toString();
        if (header.length() > comboBox.currentText.length())
        {
          comboBox.currentText = header;
        }
        break;
      }
      case FeatureValue:
      {
        comboBox.currentText = QString("0.005");
        const QString header = headerData(FeatureName, Qt::Horizontal, Qt::DisplayRole).toString();
        if (header.length() > comboBox.currentText.length())
        {comboBox.currentText = header;}
        break;
      }
      case FeatureOperator:
      {
        comboBox.currentText = QString(" > ");
        const QString header = headerData(FeatureOperator, Qt::Horizontal, Qt::DisplayRole).toString();
        if (header.length() > comboBox.currentText.length())
        {
          comboBox.currentText = header;
        }
        break;
      }
      case FeaturePhaseValue:
      {
        comboBox.currentText = QString("1");
        const QString header = headerData(FeaturePhaseValue, Qt::Horizontal, Qt::DisplayRole).toString();
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
    if (col == FeatureName)
    {
      return QVariant(m_FeatureNames[index.row()]);
    }
    else if (col == FeatureValue)
    {
      return QVariant(m_FeatureValues[index.row()]);
    }
    else if (col == FeatureOperator)
    {
      return QVariant(m_FeatureOperators[index.row()]);
    }
    else if (col == FeaturePhaseValue)
    {
      return QVariant(m_FeaturePhaseValues[index.row()]);
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
      case FeatureName:
        return QVariant(QString("Feature"));
        break;
      case FeatureValue:
        return QVariant(QString("Value"));
        break;
      case FeatureOperator:
        return QVariant(QString("Filter"));
        break;
      case FeaturePhaseValue:
        return QVariant(QString("Phase"));
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
int QualityMetricTableModel::rowCount(const QModelIndex& index) const
{
  return index.isValid() ? 0 : m_RowCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QualityMetricTableModel::columnCount(const QModelIndex& index) const
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
bool QualityMetricTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  // qDebug() << "QualityMetricTableModel::setData " << value.toString().toStdString() << "\n";
  if (!index.isValid() || role != Qt::EditRole || index.row() < 0 || index.row() >= m_FeatureNames.count() || index.column() < 0 || index.column()
      >= m_ColumnCount)
  {
    return false;
  }
  bool ok;
  qint32 row = index.row();
  qint32 col = index.column();
  switch(col)
  {
    case FeatureName:
      m_FeatureNames[row] = value.toString();
      break;
    case FeatureValue:
      m_FeatureValues[row] = value.toFloat(&ok);
      break;
    case FeatureOperator:
      m_FeatureOperators[row] =  value.toString();
      break;
    case FeaturePhaseValue:
      m_FeaturePhaseValues[row] =  value.toInt(&ok);
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
  if (m_PossibleFeatures.size() < 1) { return false;}
  QString featureName = m_PossibleFeatures.at(0);
  float featureValue = 0.0f;
  QString featureOperator = ">";
  int featurePhaseValue = 1;

  beginInsertRows(QModelIndex(), row, row + count - 1);
  for (int i = 0; i < count; ++i)
  {
    m_FeatureNames.append(featureName);
    m_FeatureValues.append(featureValue);
    m_FeatureOperators.append(featureOperator);
    m_FeaturePhaseValues.append(featurePhaseValue);
    m_RowCount = m_FeatureNames.count();
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
    m_FeatureNames.remove(row);
    m_FeatureValues.remove(row);
    m_FeatureOperators.remove(row);
    m_FeaturePhaseValues.remove(row);
    m_RowCount = m_FeatureNames.count();
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
    case FeatureName:
      return m_FeatureValues;
      break;
    case FeatureValue:
      return m_K;
      break;
    case FeatureOperator:
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
float QualityMetricTableModel::getDataValue(int col, int row)
{
  switch(col)
  {
    case FeatureName:
      return m_FeatureValues[row];
      break;
    case FeatureValue:
      return m_K[row];
      break;
    case FeatureOperator:
      return m_Beta[row];
      break;
    default:
      Q_ASSERT(false);
  }
  return 0.0f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QualityMetricTableModel::setColumnData(int col, QVector<float>& data)
{
  switch(col)
  {
    case FeatureName:
      m_FeatureValues = data;
      break;
    case FeatureValue:
      m_K = data;
      break;
    case FeatureOperator:
      m_Beta = data;
      break;
    default:
      Q_ASSERT(false);
  }
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QualityMetricTableModel::setTableData( QVector<QString> featureNames, QVector<float> featureValues,  QVector<QString> featureOperators, QVector<int> featurePhaseValues)
{
  qint32 count = featureNames.count();
  qint32 row = 0;
  // Remove all the current rows in the table model
  removeRows(0, rowCount());
  // Check to make sure we have data to insert.
  if (count == 0) { return; }
  // Now mass insert the data to the table then emit that the data has changed
  beginInsertRows(QModelIndex(), row, row + count - 1);
  m_FeatureNames = featureNames;
  m_FeatureValues = featureValues;
  m_FeatureOperators = featureOperators;
  m_FeaturePhaseValues = featurePhaseValues;
  m_RowCount = count;
  endInsertRows();
  QModelIndex topLeft = createIndex(0, 0);
  QModelIndex botRight = createIndex(count - 1, ColumnCount);
  emit dataChanged(topLeft, botRight);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QualityMetricTableModel::getTableData( QVector<QString>& featureNames, QVector<float>& featureValues,  QVector<QString>& featureOperators, QVector<int>& featurePhaseValues)
{
  featureNames = m_FeatureNames;
  featureValues = m_FeatureValues;
  featureOperators = m_FeatureOperators;
  featurePhaseValues = m_FeaturePhaseValues;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QualityMetricTableModel::setNumberOfPhases(int n)
{
  m_NumberOfPhases = n;
  QualityMetricItemDelegate* dlg = qobject_cast<QualityMetricItemDelegate*>(getItemDelegate());
  if (dlg)
  {
    dlg->setNumberOfPhases(n);
    delete dlg;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAbstractItemDelegate* QualityMetricTableModel::getItemDelegate()
{
  return new QualityMetricItemDelegate(m_PossibleFeatures, m_NumberOfPhases);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QualityMetricTableModel::setPossibleFeatures(QStringList features)
{
  m_PossibleFeatures = features;
  QualityMetricItemDelegate* dlg = qobject_cast<QualityMetricItemDelegate*>(getItemDelegate());
  if (dlg)
  {
    dlg->setFeatureList(m_PossibleFeatures);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList QualityMetricTableModel::getPossibleFeatures()
{
  return m_PossibleFeatures;
}
