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


#include "ComparisonSelectionItemDelegate.h"

#include <limits>

#include <QtCore/QAbstractItemModel>
#include <QtWidgets/QStyleOptionViewItemV4>
#include <QtWidgets/QLineEdit>
#include <QtGui/QDoubleValidator>
#include <QtWidgets/QComboBox>
#include <QtGui/QPainter>
#include <QtCore/QDebug>

#include "DREAM3DWidgetsLib/FilterParameterWidgets/ComparisonSelectionTableModel.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_ComparisonSelectionItemDelegate.cpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonSelectionItemDelegate::ComparisonSelectionItemDelegate(QStringList possibleFeatures, int numPhases, QObject* parent) :
  QStyledItemDelegate(parent),
  m_FeatureList(possibleFeatures),
  m_NumberOfPhases(numPhases)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonSelectionItemDelegate::~ComparisonSelectionItemDelegate()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionItemDelegate::setNumberOfPhases(int n) { m_NumberOfPhases = n; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QStyledItemDelegate::paint(painter, option, index);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionItemDelegate::setFeatureList(QStringList features)
{
  m_FeatureList = features;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* ComparisonSelectionItemDelegate::createEditor(QWidget* widgetParent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QLineEdit* featureValue = NULL;
  QDoubleValidator* featureValueValidator = NULL;
  QComboBox* operatorCombo = NULL;

  ComparisonSelectionTableModel* tableModel = qobject_cast<ComparisonSelectionTableModel*>(parent());

  QStringList operators;
  operators << (DREAM3D::Comparison::Strings::LessThan)
            << (DREAM3D::Comparison::Strings::GreaterThan)
            << (DREAM3D::Comparison::Strings::Equal);

  //QComboBox* phaseCombo = NULL;
  QStringList phases;
  for(int i = 0; i < m_NumberOfPhases; ++i)
  {
    phases << QString::number(i + 1);
  }
  bool ok = false;
  qint32 col = index.column();
  switch(col)
  {
    case ComparisonSelectionTableModel::FeatureName:
      operatorCombo = new QComboBox(widgetParent);
      operatorCombo->addItems(m_FeatureList);
      operatorCombo->setAutoFillBackground(true);
      if(tableModel)
      {
        connect(operatorCombo, SIGNAL(currentIndexChanged(int)),
                this, SLOT(arrayNameChangedData(int)));
      }
      return operatorCombo;
    case ComparisonSelectionTableModel::FeatureValue:
    {
      featureValue = new QLineEdit(widgetParent);
      featureValue->setFrame(false);
      featureValueValidator = new QDoubleValidator(featureValue);
      featureValueValidator->setRange(-1.0f * std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), 6);
      featureValue->setValidator(featureValueValidator);
      QVariant var = index.model()->data(index);
      featureValue->setText(QString::number(var.toDouble(&ok), 'g', 6));
      if (tableModel)
      {
        connect(featureValue, SIGNAL(textChanged(const QString&)),
                this, SLOT(valueChangedData(const QString&)));
      }
      return featureValue;
    }
    case ComparisonSelectionTableModel::FeatureOperator:
      operatorCombo = new QComboBox(widgetParent);
      operatorCombo->addItems(operators);
      operatorCombo->setAutoFillBackground(true);
      if (tableModel)
      {
        connect(operatorCombo, SIGNAL(currentIndexChanged(int)),
                this, SLOT(operatorChangedData(int)));
      }
      return operatorCombo;
    //        case ComparisonSelectionTableModel::FeaturePhaseValue:
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
void ComparisonSelectionItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
  qint32 col = index.column();
  // bool ok = false;
  if (col == ComparisonSelectionTableModel::FeatureName)
  {
    QString objName = QString::number(index.row()) + "," + QString::number(ComparisonSelectionTableModel::FeatureName);
    QString state = index.model()->data(index).toString();
    QComboBox* comboBox = qobject_cast<QComboBox* > (editor);
    Q_ASSERT(comboBox);
    comboBox->setObjectName(objName);
    comboBox->setCurrentIndex(comboBox->findText(state));
  }
  else if (col == ComparisonSelectionTableModel::FeatureValue )
  {
    QString objName = QString::number(index.row()) + "," + QString::number(ComparisonSelectionTableModel::FeatureValue);
    QLineEdit* lineEdit = qobject_cast<QLineEdit* > (editor);
    Q_ASSERT(lineEdit);
    lineEdit->setObjectName(objName);
//    QVariant var = index.model()->data(index);
//    double d = var.toDouble();
//    qDebug() << lineEdit->text();
//    lineEdit->setText(QString::number(d));
  }
  else if (col == ComparisonSelectionTableModel::FeatureOperator)
  {
    QString objName = QString::number(index.row()) + "," + QString::number(ComparisonSelectionTableModel::FeatureOperator);
    QString state = index.model()->data(index).toString();
    QComboBox* comboBox = qobject_cast<QComboBox* > (editor);
    Q_ASSERT(comboBox);
    comboBox->setObjectName(objName);
    comboBox->setCurrentIndex(comboBox->findText(state));
  }
  else { QStyledItemDelegate::setEditorData(editor, index); }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
  qint32 col = index.column();
  if (col == ComparisonSelectionTableModel::FeatureName)
  {
    QComboBox* comboBox = qobject_cast<QComboBox* > (editor);
    Q_ASSERT(comboBox);
    model->setData(index, comboBox->currentText());
  }
  else if (col == ComparisonSelectionTableModel::FeatureValue)
  {
    QLineEdit* lineEdit = qobject_cast<QLineEdit* > (editor);
    Q_ASSERT(lineEdit);
    bool ok = false;
    double v = lineEdit->text().toFloat(&ok);
    model->setData(index, v);
  }
  else if (col == ComparisonSelectionTableModel::FeatureOperator)
  {
    QComboBox* comboBox = qobject_cast<QComboBox* > (editor);
    Q_ASSERT(comboBox);
    model->setData(index, comboBox->currentText());
  }
  else { QStyledItemDelegate::setModelData(editor, model, index); }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionItemDelegate::arrayNameChangedData(int i)
{
  ComparisonSelectionTableModel* tableModel = qobject_cast<ComparisonSelectionTableModel*>(parent());
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
void ComparisonSelectionItemDelegate::operatorChangedData(int i)
{
  ComparisonSelectionTableModel* tableModel = qobject_cast<ComparisonSelectionTableModel*>(parent());
  QWidget* w = qobject_cast<QWidget*>(sender());
  if (tableModel && w)
  {
    QString objName = w->objectName();
    QStringList tokens = objName.split(',');
    int row = tokens[0].toInt();
    int col = tokens[1].toInt();
    //ComparisonSelectionTableModel::FeatureName
    QModelIndex index = tableModel->index(row, col);

    setModelData(w, tableModel, index);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionItemDelegate::valueChangedData(const QString& text)
{
  ComparisonSelectionTableModel* tableModel = qobject_cast<ComparisonSelectionTableModel*>(parent());
  QWidget* w = qobject_cast<QWidget*>(sender());
  if (tableModel && w)
  {
    QString objName = w->objectName();
    QStringList tokens = objName.split(',');
    int row = tokens[0].toInt();
    int col = tokens[1].toInt();
    //ComparisonSelectionTableModel::FeatureName
    QModelIndex index = tableModel->index(row, col);

    setModelData(w, tableModel, index);
  }
}
