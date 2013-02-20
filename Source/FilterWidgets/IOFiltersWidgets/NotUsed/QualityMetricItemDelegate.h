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


#ifndef QUALITYMETRICITEMDELEGATE_H_
#define QUALITYMETRICITEMDELEGATE_H_
#include <iostream>


#include <QtCore/QModelIndex>
#include <QtCore/QStringList>
#include <QtGui/QComboBox>
#include <QtGui/QPainter>
#include <QtGui/QStyleOptionViewItemV4>
#include <QtGui/QLineEdit>
#include <QtGui/QDoubleValidator>
#include <QtGui/QStyledItemDelegate>

#include "QualityMetricTableModel.h"

class QualityMetricItemDelegate : public QStyledItemDelegate
{
  Q_OBJECT

  public:
    explicit QualityMetricItemDelegate(QStringList possibleFields, int numPhases, QObject *parent = 0) :
      QStyledItemDelegate(parent),
      m_FieldList(possibleFields),
      m_NumberOfPhases(numPhases)
    {
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void setNumberOfPhases(int n) { m_NumberOfPhases = n; }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
      QStyledItemDelegate::paint(painter, option, index);
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void setFieldList(QStringList fields) {
      m_FieldList = fields;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
      QLineEdit* fieldValue = NULL;
      QDoubleValidator* fieldValueValidator = NULL;
      QComboBox* operatorCombo = NULL;

      QStringList operators;
      operators << ">" << "<" << "=";

      QComboBox* phaseCombo = NULL;
      QStringList phases;
      for(int i = 0; i < m_NumberOfPhases; ++i)
      {
        phases << QString::number(i + 1);
      }

      qint32 col = index.column();
      switch(col)
      {
        case QualityMetricTableModel::FieldName:
          operatorCombo = new QComboBox(parent);
          operatorCombo->addItems(m_FieldList);
          operatorCombo->setAutoFillBackground(true);
          return operatorCombo;
          break;
        case QualityMetricTableModel::FieldValue:
          fieldValue = new QLineEdit(parent);
          fieldValue->setFrame(false);
          fieldValueValidator = new QDoubleValidator(fieldValue);
          fieldValueValidator->setDecimals(6);
          fieldValue->setValidator(fieldValueValidator);
          return fieldValue;
        case QualityMetricTableModel::FieldOperator:
          operatorCombo = new QComboBox(parent);
          operatorCombo->addItems(operators);
          operatorCombo->setAutoFillBackground(true);
          return operatorCombo;
        case QualityMetricTableModel::FieldPhaseValue:
          phaseCombo = new QComboBox(parent);
          phaseCombo->addItems(phases);
          phaseCombo->setAutoFillBackground(true);
          return phaseCombo;
        default:
          break;
      }
      return QStyledItemDelegate::createEditor(parent, option, index);
    }



    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void setEditorData(QWidget *editor, const QModelIndex &index) const
    {
      qint32 col = index.column();
     // bool ok = false;
      if (col == QualityMetricTableModel::FieldName)
      {
        QString state = index.model()->data(index).toString();
        QComboBox* comboBox = qobject_cast<QComboBox* > (editor);
        Q_ASSERT(comboBox);
        comboBox->setCurrentIndex(comboBox->findText(state));
      }
      else if (col == QualityMetricTableModel::FieldValue )
      {
        QLineEdit* lineEdit = qobject_cast<QLineEdit* > (editor);
        Q_ASSERT(lineEdit);
        lineEdit->setText(index.model()->data(index).toString());
      }
      else if (col == QualityMetricTableModel::FieldOperator)
      {
        QString state = index.model()->data(index).toString();
        QComboBox* comboBox = qobject_cast<QComboBox* > (editor);
        Q_ASSERT(comboBox);
        comboBox->setCurrentIndex(comboBox->findText(state));
      }
      else if (col == QualityMetricTableModel::FieldPhaseValue)
      {
        QString state = index.model()->data(index).toString();
        QComboBox* comboBox = qobject_cast<QComboBox* > (editor);
        Q_ASSERT(comboBox);
        comboBox->setCurrentIndex(comboBox->findText(state));
      }

      else QStyledItemDelegate::setEditorData(editor, index);
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
    {
      //  std::cout << "QualityMetricItemDelegate::setModelData" << std::endl;
      qint32 col = index.column();
      //  bool ok = false;
      if (col == QualityMetricTableModel::FieldName)
      {
        QComboBox *comboBox = qobject_cast<QComboBox* > (editor);
        Q_ASSERT(comboBox);
        model->setData(index, comboBox->currentText());
      }
      else if (col == QualityMetricTableModel::FieldValue)
      {
        QLineEdit* lineEdit = qobject_cast<QLineEdit* > (editor);
        Q_ASSERT(lineEdit);
        bool ok = false;
        double v = lineEdit->text().toFloat(&ok);
        model->setData(index, v);
      }
      else if (col == QualityMetricTableModel::FieldOperator)
      {
        QComboBox *comboBox = qobject_cast<QComboBox* > (editor);
        Q_ASSERT(comboBox);
        model->setData(index, comboBox->currentText());
      }
      else if (col == QualityMetricTableModel::FieldPhaseValue)
      {
        QComboBox *comboBox = qobject_cast<QComboBox* > (editor);
        Q_ASSERT(comboBox);
        model->setData(index, comboBox->currentText());
      }
      else QStyledItemDelegate::setModelData(editor, model, index);
    }

  private:
    QModelIndex m_Index;
//    QWidget* m_Widget;
//    QAbstractItemModel* m_Model;
    QStringList m_FieldList;
    int m_NumberOfPhases;

};

#include "IOFiltersWidgets/moc_QualityMetricItemDelegate.cxx"
#endif /*  */

