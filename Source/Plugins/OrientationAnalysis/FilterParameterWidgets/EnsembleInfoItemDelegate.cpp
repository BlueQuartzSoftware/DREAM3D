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

#include "EnsembleInfoItemDelegate.h"

#include <limits>

#include <QtCore/QAbstractItemModel>
#include <QtCore/QDebug>
#include <QtGui/QDoubleValidator>
#include <QtGui/QPainter>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QStyleOptionViewItemV4>

#include "EnsembleInfoTableModel.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
//#include "moc_EnsembleInfoItemDelegate.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnsembleInfoItemDelegate::EnsembleInfoItemDelegate(int numPhases, QObject* parent)
: QStyledItemDelegate(parent)
, m_CrystalStructureList(EnsembleInfo::CrystalStructureStrings())
, m_NumberOfPhases(numPhases)

{
  PhaseType::getPhaseTypeStrings(m_PhaseTypeList);
  m_PhaseTypeList.pop_back(); // Pop "Unknown" PhaseType
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnsembleInfoItemDelegate::~EnsembleInfoItemDelegate()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoItemDelegate::setNumberOfPhases(int n)
{
  m_NumberOfPhases = n;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QStyledItemDelegate::paint(painter, option, index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoItemDelegate::setCrystalStructureList(QStringList structures)
{
  m_CrystalStructureList = structures;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoItemDelegate::setPhaseTypeList(QStringList phases)
{
  m_PhaseTypeList = phases;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* EnsembleInfoItemDelegate::createEditor(QWidget* widgetParent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QComboBox* operatorCombo = nullptr;
  QLineEdit* lineEdit = nullptr;

  EnsembleInfoTableModel* tableModel = qobject_cast<EnsembleInfoTableModel*>(parent());

  QStringList operators;
  operators << (SIMPL::Comparison::Strings::LessThan) << (SIMPL::Comparison::Strings::GreaterThan) << (SIMPL::Comparison::Strings::Equal);

  // QComboBox* phaseCombo = nullptr;
  QStringList phases;
  for(int i = 0; i < m_NumberOfPhases; ++i)
  {
    phases << QString::number(i + 1);
  }
  qint32 col = index.column();
  switch(col)
  {
  case EnsembleInfoTableModel::CrystalStructure:
    operatorCombo = new QComboBox(widgetParent);
    operatorCombo->addItems(m_CrystalStructureList);
    operatorCombo->setAutoFillBackground(true);
    if(tableModel)
    {
      connect(operatorCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(crystalStructureChangedData(int)));
    }
    return operatorCombo;
  case EnsembleInfoTableModel::PhaseType:
  {
    operatorCombo = new QComboBox(widgetParent);
    operatorCombo->addItems(m_PhaseTypeList);
    operatorCombo->setAutoFillBackground(true);
    if(tableModel)
    {
      connect(operatorCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(phaseTypeChangedData(int)));
    }
    return operatorCombo;
  }
  case EnsembleInfoTableModel::PhaseName:
  {
    lineEdit = new QLineEdit(widgetParent);
    lineEdit->setFrame(false);
    QVariant var = index.model()->data(index);
    lineEdit->setText(var.toString());
    if(tableModel)
    {
      connect(lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(phaseNameChangedData(const QString&)));
    }
    return lineEdit;
  }
  //        case EnsembleInfoTableModel::FeaturePhaseValue:
  //          phaseCombo = new QComboBox(parent);
  //          phaseCombo->addItems(phases);
  //          phaseCombo->setAutoFillBackground(true);
  //          return phaseCombo;
  default:
    break;
  }
  return QStyledItemDelegate::createEditor(widgetParent, option, index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
  qint32 col = index.column();
  const QAbstractItemModel* model = index.model();
  QVariant variant = model->data(index);

  // bool ok = false;
  if(col == EnsembleInfoTableModel::CrystalStructure)
  {
    QString objName = QString::number(index.row()) + "," + QString::number(EnsembleInfoTableModel::CrystalStructure);
    QString state = variant.toString();
    QComboBox* comboBox = qobject_cast<QComboBox*>(editor);
    Q_ASSERT(comboBox);
    comboBox->setObjectName(objName);
    comboBox->setCurrentText(state);
  }
  else if(col == EnsembleInfoTableModel::PhaseType)
  {
    QString objName = QString::number(index.row()) + "," + QString::number(EnsembleInfoTableModel::PhaseType);
    QString state = variant.toString();
    QComboBox* comboBox = qobject_cast<QComboBox*>(editor);
    Q_ASSERT(comboBox);
    comboBox->setObjectName(objName);
    comboBox->setCurrentText(state);
    //    QVariant var = index.model()->data(index);
    //    double d = var.toDouble();
    //    qDebug() << lineEdit->text();
    //    lineEdit->setText(QString::number(d));
  }
  else if(col == EnsembleInfoTableModel::PhaseName)
  {
    QString objName = QString::number(index.row()) + "," + QString::number(EnsembleInfoTableModel::PhaseName);
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    Q_ASSERT(lineEdit);
    lineEdit->setObjectName(objName);
  }
  else
  {
    QStyledItemDelegate::setEditorData(editor, index);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
  qint32 col = index.column();
  if(col == EnsembleInfoTableModel::CrystalStructure)
  {
    QComboBox* comboBox = qobject_cast<QComboBox*>(editor);
    Q_ASSERT(comboBox);
    model->setData(index, comboBox->currentIndex());
  }
  else if(col == EnsembleInfoTableModel::PhaseType)
  {
    QComboBox* comboBox = qobject_cast<QComboBox*>(editor);
    Q_ASSERT(comboBox);
    model->setData(index, comboBox->currentIndex());
  }
  else if(col == EnsembleInfoTableModel::PhaseName)
  {
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    Q_ASSERT(lineEdit);
    model->setData(index, lineEdit->text());
  }
  else
  {
    QStyledItemDelegate::setModelData(editor, model, index);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoItemDelegate::crystalStructureChangedData(int i)
{
  EnsembleInfoTableModel* tableModel = qobject_cast<EnsembleInfoTableModel*>(parent());
  QWidget* w = qobject_cast<QWidget*>(sender());
  if(tableModel && w)
  {
    QString objName = w->objectName();
    QStringList tokens = objName.split(',');
    int row = tokens[0].toInt();
    int col = tokens[1].toInt();
    QModelIndex index = tableModel->index(row, col);
    setModelData(w, tableModel, index);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoItemDelegate::phaseTypeChangedData(int i)
{
  EnsembleInfoTableModel* tableModel = qobject_cast<EnsembleInfoTableModel*>(parent());
  QWidget* w = qobject_cast<QWidget*>(sender());
  if(tableModel && w)
  {
    QString objName = w->objectName();
    QStringList tokens = objName.split(',');
    int row = tokens[0].toInt();
    int col = tokens[1].toInt();
    // EnsembleInfoTableModel::FeatureName
    QModelIndex index = tableModel->index(row, col);

    setModelData(w, tableModel, index);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoItemDelegate::phaseNameChangedData(const QString& text)
{
  EnsembleInfoTableModel* tableModel = qobject_cast<EnsembleInfoTableModel*>(parent());
  QWidget* w = qobject_cast<QWidget*>(sender());
  if(tableModel && w)
  {
    QString objName = w->objectName();
    QStringList tokens = objName.split(',');
    int row = tokens[0].toInt();
    int col = tokens[1].toInt();
    // EnsembleInfoTableModel::FeatureName
    QModelIndex index = tableModel->index(row, col);

    setModelData(w, tableModel, index);
  }
}
