/* ============================================================================
 * Copyright (c) 2014 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2014 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ComparisonSelectionItemDelegate.h"

#include <QtCore/QDebug>
#include <QtCore/QAbstractItemModel>
#include <QtGui/QStyleOptionViewItemV4>
#include <QtGui/QLineEdit>
#include <QtGui/QDoubleValidator>
#include <QtGui/QComboBox>
#include <QtGui/QPainter>

#include "DREAM3DWidgetsLib/moc_ComparisonSelectionItemDelegate.cpp"


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
QWidget* ComparisonSelectionItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QLineEdit* featureValue = NULL;
  QDoubleValidator* featureValueValidator = NULL;
  QComboBox* operatorCombo = NULL;

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

  qint32 col = index.column();
  switch(col)
  {
    case ComparisonSelectionTableModel::FeatureName:
      operatorCombo = new QComboBox(parent);
      operatorCombo->addItems(m_FeatureList);
      operatorCombo->setAutoFillBackground(true);
      return operatorCombo;
      break;
    case ComparisonSelectionTableModel::FeatureValue:
      featureValue = new QLineEdit(parent);
      featureValue->setFrame(false);
      featureValueValidator = new QDoubleValidator(featureValue);
      featureValueValidator->setDecimals(6);
      featureValue->setValidator(featureValueValidator);
      return featureValue;
    case ComparisonSelectionTableModel::FeatureOperator:
      operatorCombo = new QComboBox(parent);
      operatorCombo->addItems(operators);
      operatorCombo->setAutoFillBackground(true);
      return operatorCombo;
      //        case ComparisonSelectionTableModel::FeaturePhaseValue:
      //          phaseCombo = new QComboBox(parent);
      //          phaseCombo->addItems(phases);
      //          phaseCombo->setAutoFillBackground(true);
      //          return phaseCombo;
    default:
      break;
  }
  return QStyledItemDelegate::createEditor(parent, option, index);
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
    QString state = index.model()->data(index).toString();
    QComboBox* comboBox = qobject_cast<QComboBox* > (editor);
    Q_ASSERT(comboBox);
    comboBox->setCurrentIndex(comboBox->findText(state));
  }
  else if (col == ComparisonSelectionTableModel::FeatureValue )
  {
    QLineEdit* lineEdit = qobject_cast<QLineEdit* > (editor);
    Q_ASSERT(lineEdit);
    lineEdit->setText(index.model()->data(index).toString());
  }
  else if (col == ComparisonSelectionTableModel::FeatureOperator)
  {
    QString state = index.model()->data(index).toString();
    QComboBox* comboBox = qobject_cast<QComboBox* > (editor);
    Q_ASSERT(comboBox);
    comboBox->setCurrentIndex(comboBox->findText(state));
  }
  //      else if (col == ComparisonSelectionTableModel::FeaturePhaseValue)
  //      {
  //        QString state = index.model()->data(index).toString();
  //        QComboBox* comboBox = qobject_cast<QComboBox* > (editor);
  //        Q_ASSERT(comboBox);
  //        comboBox->setCurrentIndex(comboBox->findText(state));
  //      }

  else { QStyledItemDelegate::setEditorData(editor, index); }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
  //  qDebug() << "ComparisonSelectionItemDelegate::setModelData" << "\n";
  qint32 col = index.column();
  //  bool ok = false;
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
  //      else if (col == ComparisonSelectionTableModel::FeaturePhaseValue)
  //      {
  //        QComboBox *comboBox = qobject_cast<QComboBox* > (editor);
  //        Q_ASSERT(comboBox);
  //        model->setData(index, comboBox->currentText());
  //      }
  else { QStyledItemDelegate::setModelData(editor, model, index); }
}
