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
#ifndef SGBETAITEMDELEGATE_H_
#define SGBETAITEMDELEGATE_H_

#include <iostream>

#include <QtCore/QModelIndex>
#include <QtGui/QComboBox>
#include <QtGui/QPainter>
#include <QtGui/QStyleOptionViewItemV4>
#include <QtGui/QLineEdit>
#include <QtGui/QDoubleValidator>
#include <QtGui/QStyledItemDelegate>

#include "DREAM3DLib/Common/StatsGen.h"
#include "QtSupport/ColorComboPicker.h"
#include "StatsGenerator/TableModels/SGBetaTableModel.h"

/**
 * @class SGBetaItemDelegate SGBetaItemDelegate.h StatsGenerator/SGBetaItemDelegate.h
 * @brief This class creates the appropriate Editor Widget for the Tables
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Dec 28, 2010
 * @version 1.0
 */
class SGBetaItemDelegate : public QStyledItemDelegate
{
  Q_OBJECT

  public:
    explicit SGBetaItemDelegate(QObject *parent = 0) :
      QStyledItemDelegate(parent)
    {
    }

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
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
      QLineEdit* alpha;
      QLineEdit* beta;
      QDoubleValidator* alphaValidator;
      QDoubleValidator* betaValidator;
      QComboBox* colorCombo;

      qint32 col = index.column();
      switch(col)
      {
        case SGBetaTableModel::BinNumber:
          return NULL;
          break;

        case SGBetaTableModel::Alpha:
          alpha = new QLineEdit(parent);
          alpha->setFrame(false);
          alphaValidator = new QDoubleValidator(alpha);
          alphaValidator->setDecimals(6);
          alpha->setValidator(alphaValidator);
          return alpha;
        case SGBetaTableModel::Beta:
          beta = new QLineEdit(parent);
          beta->setFrame(false);
          betaValidator = new QDoubleValidator(beta);
          betaValidator->setDecimals(6);
          beta->setValidator(betaValidator);
          return beta;
        case SGBetaTableModel::LineColor:
          colorCombo = new ColorComboPicker(parent);
          return colorCombo;
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
   //   bool ok = false;
      if (col == SGBetaTableModel::Alpha || col == SGBetaTableModel::Beta)
      {
   //     double value = index.model()->data(index).toFloat(&ok);
        QLineEdit* lineEdit = qobject_cast<QLineEdit* > (editor);
        Q_ASSERT(lineEdit);
        lineEdit->setText(index.model()->data(index).toString());
      }
      else if (col == SGBetaTableModel::LineColor)
      {
        QString state = index.model()->data(index).toString();
        ColorComboPicker* comboBox = qobject_cast<ColorComboPicker* > (editor);
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
      //  std::cout << "SGBetaItemDelegate::setModelData" << std::endl;
      qint32 col = index.column();
      //  bool ok = false;
      if (col == SGBetaTableModel::Alpha || col == SGBetaTableModel::Beta)
      {
        QLineEdit* lineEdit = qobject_cast<QLineEdit* > (editor);
        Q_ASSERT(lineEdit);
        bool ok = false;
        double v = lineEdit->text().toFloat(&ok);
        model->setData(index, v);
      }
      else if (col == SGBetaTableModel::LineColor)
      {
        ColorComboPicker *comboBox = qobject_cast<ColorComboPicker* > (editor);
        Q_ASSERT(comboBox);
        model->setData(index, comboBox->currentText());
      }
      else QStyledItemDelegate::setModelData(editor, model, index);

    }

  private:
    QModelIndex m_Index;
};

#endif /* SGBETAITEMDELEGATE_H_ */
