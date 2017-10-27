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

#include "EnsembleInfoTableModel.h"

#include <QtCore/QDebug>
#include <QtWidgets/QAbstractItemDelegate>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QStyleOptionComboBox>

#include "EnsembleInfoItemDelegate.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
//#include "moc_EnsembleInfoTableModel.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnsembleInfoTableModel::EnsembleInfoTableModel(bool showOperators, QObject* parent)
: QAbstractTableModel(parent)
, m_RowCount(0)
, m_NumberOfPhases(1)
{
  if(showOperators)
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
EnsembleInfoTableModel::~EnsembleInfoTableModel()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::ItemFlags EnsembleInfoTableModel::flags(const QModelIndex& index) const
{
  //  qDebug() << "EnsembleInfoTableModel::flags" << "\n";
  if(!index.isValid())
  {
    return Qt::NoItemFlags;
  }
  Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
  if(index.isValid())
  {
    // theFlags |= Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    int col = index.column();
    if(col == CrystalStructure || col == PhaseType || col == PhaseName)
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
QVariant EnsembleInfoTableModel::data(const QModelIndex& index, qint32 role) const
{

  if(!index.isValid())
  {
    return QVariant();
  }

  if(role == Qt::SizeHintRole)
  {
    QStyleOptionComboBox comboBox;
    QLineEdit lineEdit;

    switch(index.column())
    {
    case CrystalStructure:
    {
      comboBox.currentText = QString("Hexagonal-High");
      const QString header = headerData(CrystalStructure, Qt::Horizontal, Qt::DisplayRole).toString();
      if(header.length() > comboBox.currentText.length())
      {
        comboBox.currentText = header;
      }
      break;
    }
    case PhaseType:
    {
      comboBox.currentText = QString("Primary Phase");
      const QString header = headerData(PhaseType, Qt::Horizontal, Qt::DisplayRole).toString();
      if(header.length() > comboBox.currentText.length())
      {
        comboBox.currentText = header;
      }
      break;
    }
    case PhaseName:
    {
      lineEdit.setText("Phase Name");
      const QString header = headerData(PhaseType, Qt::Horizontal, Qt::DisplayRole).toString();
      if(header.length() > lineEdit.text().length())
      {
        lineEdit.setText(header);
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
    QFontMetrics fontMetrics(data(index, Qt::FontRole).value<QFont>());
    comboBox.fontMetrics = fontMetrics;
    QSize size(fontMetrics.width(comboBox.currentText), fontMetrics.height());
    // FIXME: Is this size correct? Can we get a more accurate size without using QApplication
    return size;

    // return qApp->style()->sizeFromContents(QStyle::CT_ComboBox, &comboBox, size);
  }
  else if(role == Qt::TextAlignmentRole)
  {
    return int(Qt::AlignRight | Qt::AlignVCenter);
  }
  else if(role == Qt::DisplayRole || role == Qt::EditRole)
  {
    int col = index.column();
    if(col == CrystalStructure)
    {
      EnsembleInfo info = m_EnsembleInfo;
      QString structure = EnsembleInfo::CrystalStructureToStr(info.getCrystalStructure(index.row()));
      return QVariant(structure);
    }
    else if(col == PhaseType)
    {
      EnsembleInfo info = m_EnsembleInfo;
      QString phase = PhaseType::getPhaseTypeString(info.getPhaseType(index.row()));
      return QVariant(phase);
    }
    else if(col == PhaseName)
    {
      EnsembleInfo info = m_EnsembleInfo;
      QString phaseName = info.getPhaseName(index.row());
      return QVariant(phaseName);
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
QVariant EnsembleInfoTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    switch(section)
    {
    case CrystalStructure:
      return QVariant(QString("Crystal Structure"));
      break;
    case PhaseType:
      return QVariant(QString("Phase Type"));
      break;
    case PhaseName:
      return QVariant(QString("Phase Name"));
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
int EnsembleInfoTableModel::rowCount(const QModelIndex& index) const
{
  return index.isValid() ? 0 : m_RowCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EnsembleInfoTableModel::columnCount(const QModelIndex& index) const
{
  return index.isValid() ? 0 : m_ColumnCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool EnsembleInfoTableModel::setHeaderData(int col, Qt::Orientation o, const QVariant& var, int role)
{
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool EnsembleInfoTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if(!index.isValid() || role != Qt::EditRole || index.row() < 0 || index.row() >= m_EnsembleInfo.size() || index.column() < 0 || index.column() >= m_ColumnCount)
  {
    return false;
  }
  bool ok;
  qint32 row = index.row();
  qint32 col = index.column();
  switch(col)
  {
  case CrystalStructure:
    m_EnsembleInfo.setCrystalStructure(row, static_cast<EnsembleInfo::CrystalStructure>(value.toInt()));
    break;
  case PhaseType:
    m_EnsembleInfo.setPhaseType(row, static_cast<PhaseType::Type>(value.toInt(&ok)));
    break;
  case PhaseName:
    m_EnsembleInfo.setPhaseName(row, value.toString());
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
bool EnsembleInfoTableModel::insertRows(int row, int count, const QModelIndex& index)
{
  EnsembleInfo::CrystalStructure crystalStructure = EnsembleInfo::CrystalStructure::Hexagonal_High;
  PhaseType::Type phaseType = PhaseType::Type::Primary;
  QString phaseName = "Phase Name";

  beginInsertRows(QModelIndex(), row, row + count - 1);
  for(int i = 0; i < count; ++i)
  {
    m_EnsembleInfo.addValues(crystalStructure, phaseType, phaseName);
    m_RowCount = m_EnsembleInfo.size();
  }
  endInsertRows();
  emit dataChanged(index, index);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool EnsembleInfoTableModel::removeRows(int row, int count, const QModelIndex& index)
{
  if(count < 1)
  {
    return true;
  } // No Rows to remove
  beginRemoveRows(QModelIndex(), row, row + count - 1);
  for(int i = 0; i < count; ++i)
  {
    m_EnsembleInfo.remove(row);
    m_RowCount = m_EnsembleInfo.size();
  }
  endRemoveRows();
  emit dataChanged(index, index);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoTableModel::setTableData(QVector<int> crystalStructures, QVector<int> phaseTypes, QVector<QString> phaseNames)
{
  qint32 count = crystalStructures.count();
  qint32 row = 0;
  // Remove all the current rows in the table model
  removeRows(0, rowCount());
  // Check to make sure we have data to insert.
  if(count == 0)
  {
    return;
  }
  // Now mass insert the data to the table then emit that the data has changed
  beginInsertRows(QModelIndex(), row, row + count - 1);

  for(int i = 0; i < count; i++)
  {
    EnsembleInfo::CrystalStructure crystalStruct = static_cast<EnsembleInfo::CrystalStructure>(crystalStructures[i]);
    PhaseType::Type phase = static_cast<PhaseType::Type>(phaseTypes[i]);
    QString name = phaseNames[i];
    m_EnsembleInfo.addValues(crystalStruct, phase, name);
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
void EnsembleInfoTableModel::setTableData(EnsembleInfo& ensemble)
{
  qint32 count = ensemble.size();
  qint32 row = 0;
  // Remove all the current rows in the table model
  removeRows(0, rowCount());
  // Check to make sure we have data to insert.
  if(count == 0)
  {
    return;
  }
  // Now mass insert the data to the table then emit that the data has changed
  beginInsertRows(QModelIndex(), row, row + count - 1);

  m_EnsembleInfo.clear();
  m_EnsembleInfo = ensemble;
  m_RowCount = count;
  endInsertRows();
  QModelIndex topLeft = createIndex(0, 0);
  QModelIndex botRight = createIndex(count - 1, ColumnCount);
  emit dataChanged(topLeft, botRight);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoTableModel::getTableData(QVector<int>& crystalStructures, QVector<int>& phaseTypes, QVector<QString>& phaseNames)
{
  QVector<int> structures;
  QVector<int> phases;
  QVector<QString> names;

  int count = m_EnsembleInfo.size();
  EnsembleInfo::CrystalStructureType* crystalStructurePtr = m_EnsembleInfo.getCrystalStructureArray()->getPointer(0);
  PhaseType::EnumType* phaseTypePtr = m_EnsembleInfo.getPhaseTypeArray()->getPointer(0);
  StringDataArray::Pointer phaseNamePtr = m_EnsembleInfo.getPhaseNameArray();

  for(int i = 0; i < count; i++)
  {
    structures.push_back(static_cast<int>(crystalStructurePtr[i]));
    phases.push_back(static_cast<int>(phaseTypePtr[i]));
    names.push_back(phaseNamePtr->getValue(i));
  }

  crystalStructures = structures;
  phaseTypes = phases;
  phaseNames = names;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoTableModel::setNumberOfPhases(int n)
{
  m_NumberOfPhases = n;
  EnsembleInfoItemDelegate* dlg = qobject_cast<EnsembleInfoItemDelegate*>(getItemDelegate());
  if(dlg)
  {
    dlg->setNumberOfPhases(n);
    delete dlg;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAbstractItemDelegate* EnsembleInfoTableModel::getItemDelegate()
{
  return new EnsembleInfoItemDelegate(m_NumberOfPhases, this);
}
