/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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


#include "ComparisonSelectionTableModel.h"

#include <QtCore/QDebug>
#include <QtWidgets/QStyleOptionComboBox>
#include <QtWidgets/QAbstractItemDelegate>

#include "Applications/DREAM3D/DREAM3DApplication.h"

#include "ComparisonSelectionItemDelegate.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_ComparisonSelectionTableModel.cpp"

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
    m_ColumnCount = ColumnCount; // this comes from the enumerator in the header file
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
Qt::ItemFlags ComparisonSelectionTableModel::flags(const QModelIndex& index) const
{
  //  qDebug() << "ComparisonSelectionTableModel::flags" << "\n";
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
//    else if ( col == FeaturePhaseValue)
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
QVariant ComparisonSelectionTableModel::data(const QModelIndex& index, qint32 role) const
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
//      case FeaturePhaseValue:
//      {
//        comboBox.currentText = QString("1");
//        const QString header = headerData(FeaturePhaseValue, Qt::Horizontal, Qt::DisplayRole).toString();
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
    return dream3dApp->style()->sizeFromContents(QStyle::CT_ComboBox, &comboBox, size);
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
//    else if (col == FeaturePhaseValue)
//    {
//      return QVariant(m_FeaturePhaseValues[index.row()]);
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
      case FeatureName:
        return QVariant(QString("Array"));
        break;
      case FeatureValue:
        return QVariant(QString("Threshold Value"));
        break;
      case FeatureOperator:
        return QVariant(QString("Operator"));
        break;
//      case FeaturePhaseValue:
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
int ComparisonSelectionTableModel::rowCount(const QModelIndex& index) const
{
  return index.isValid() ? 0 : m_RowCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ComparisonSelectionTableModel::columnCount(const QModelIndex& index) const
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
bool ComparisonSelectionTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
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
    default:
      Q_ASSERT(false);
  }

  emit dataChanged(index, index);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ComparisonSelectionTableModel::insertRows(int row, int count, const QModelIndex& index)
{
  if (m_PossibleFeatures.size() < 1) { return false;}
  QString featureName = m_PossibleFeatures.at(0);
  float featureValue = 0.0f;
  QString featureOperator = ">";
  //int featurePhaseValue = 1;

  beginInsertRows(QModelIndex(), row, row + count - 1);
  for (int i = 0; i < count; ++i)
  {
    m_FeatureNames.append(featureName);
    m_FeatureValues.append(featureValue);
    m_FeatureOperators.append(featureOperator);
    //m_FeaturePhaseValues.append(featurePhaseValue);
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
bool ComparisonSelectionTableModel::removeRows(int row, int count, const QModelIndex& index)
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
    //  m_FeaturePhaseValues.remove(row);
    m_RowCount = m_FeatureNames.count();
  }
  endRemoveRows();
  emit
  dataChanged(index, index);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionTableModel::setTableData(QVector<QString> featureNames,
                                                 QVector<float> featureValues,
                                                 QVector<int> featureOperators)
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

  m_FeatureOperators.resize(featureOperators.size());
  for(int i = 0; i < m_FeatureOperators.size(); ++i)
  {
    if (featureOperators[i] == DREAM3D::Comparison::Operator_LessThan) { m_FeatureOperators[i] = (DREAM3D::Comparison::Strings::LessThan); }
    if (featureOperators[i] == DREAM3D::Comparison::Operator_GreaterThan) { m_FeatureOperators[i] = (DREAM3D::Comparison::Strings::GreaterThan); }
    if (featureOperators[i] == DREAM3D::Comparison::Operator_Equal) { m_FeatureOperators[i] = (DREAM3D::Comparison::Strings::Equal); }
  }
  m_RowCount = count;
  endInsertRows();
  QModelIndex topLeft = createIndex(0, 0);
  QModelIndex botRight = createIndex(count - 1, ColumnCount);
  emit dataChanged(topLeft, botRight);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionTableModel::setTableData(ComparisonInputs& comps)
{
  qint32 count = comps.size();
  qint32 row = 0;
  // Remove all the current rows in the table model
  removeRows(0, rowCount());
  // Check to make sure we have data to insert.
  if (count == 0) { return; }
  // Now mass insert the data to the table then emit that the data has changed
  beginInsertRows(QModelIndex(), row, row + count - 1);

  m_FeatureNames.resize(count);
  m_FeatureValues.resize(count);
  m_FeatureOperators.resize(count);
  for(int i = 0; i < comps.size(); ++i)
  {
    m_FeatureNames[i] = comps[i].attributeArrayName;
    m_FeatureValues[i] = comps[i].compValue;
    if (comps[i].compOperator == DREAM3D::Comparison::Operator_LessThan) { m_FeatureOperators[i] = (DREAM3D::Comparison::Strings::LessThan); }
    if (comps[i].compOperator == DREAM3D::Comparison::Operator_GreaterThan) { m_FeatureOperators[i] = (DREAM3D::Comparison::Strings::GreaterThan); }
    if (comps[i].compOperator == DREAM3D::Comparison::Operator_Equal) { m_FeatureOperators[i] = (DREAM3D::Comparison::Strings::Equal); }
  }
  m_RowCount = count;
  endInsertRows();
  QModelIndex topLeft = createIndex(0, 0);
  QModelIndex botRight = createIndex(count - 1, ColumnCount);
  emit dataChanged(topLeft, botRight);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionTableModel::getTableData( QVector<QString>& featureNames, QVector<float>& featureValues,  QVector<int>& featureOperators)
{
  featureNames = m_FeatureNames;
  featureValues = m_FeatureValues;
  featureOperators.resize(m_FeatureOperators.size());
  for(int i = 0; i < m_FeatureOperators.size(); ++i)
  {
    if (m_FeatureOperators[i].compare((DREAM3D::Comparison::Strings::LessThan)) == 0) { featureOperators[i] = DREAM3D::Comparison::Operator_LessThan; }
    else if (m_FeatureOperators[i].compare((DREAM3D::Comparison::Strings::GreaterThan)) == 0) { featureOperators[i] = DREAM3D::Comparison::Operator_GreaterThan; }
    else if (m_FeatureOperators[i].compare((DREAM3D::Comparison::Strings::Equal)) == 0) { featureOperators[i] = DREAM3D::Comparison::Operator_Equal; }
    else { featureOperators[i] = DREAM3D::Comparison::Operator_Unknown; }
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
  return new ComparisonSelectionItemDelegate(m_PossibleFeatures, m_NumberOfPhases, this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionTableModel::setPossibleFeatures(QStringList features)
{
  m_PossibleFeatures = features;

  qint32 count = m_FeatureNames.count();
  for(qint32 i = 0; i < count; ++i)
  {
    if (m_PossibleFeatures.contains(m_FeatureNames.at(i)) == false && m_PossibleFeatures.count() > 0)
    {
      m_FeatureNames[i] = m_PossibleFeatures[0]; // Just set it to the first index
    }
  }

  ComparisonSelectionItemDelegate* dlg = qobject_cast<ComparisonSelectionItemDelegate*>(getItemDelegate());
  if (dlg)
  {
    dlg->setFeatureList(m_PossibleFeatures);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionTableModel::setPossibleFeatures(QList<QString>& features)
{
  m_PossibleFeatures.clear();
  for(QList<QString>::iterator iter = features.begin(); iter != features.end(); ++iter)
  {
    m_PossibleFeatures << (*iter);
  }

  // Now check to make sure any selected array names are either still there or if they have been
  // removed we need to default select the first array name
  qint32 count = m_FeatureNames.count();
  for(qint32 i = 0; i < count; ++i)
  {
    if (m_PossibleFeatures.contains(m_FeatureNames.at(i)) == false && m_PossibleFeatures.count() > 0)
    {
      m_FeatureNames[i] = m_PossibleFeatures[0]; // Just set it to the first index
    }
  }


  ComparisonSelectionItemDelegate* dlg = qobject_cast<ComparisonSelectionItemDelegate*>(getItemDelegate());
  if (dlg)
  {
    dlg->setFeatureList(m_PossibleFeatures);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList ComparisonSelectionTableModel::getPossibleFeatures()
{
  return m_PossibleFeatures;
}
