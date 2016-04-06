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

#ifndef _sgmdfitemdelegate_h_
#define _sgmdfitemdelegate_h_

#include <iostream>

#include <QtCore/QModelIndex>
#include <QtWidgets/QComboBox>
#include <QtGui/QPainter>
#include <QtWidgets/QStyleOptionViewItemV4>
#include <QtWidgets/QLineEdit>
#include <QtGui/QDoubleValidator>
#include <QtWidgets/QStyledItemDelegate>

#include "StatsGenerator/Widgets/TableModels/SGMDFTableModel.h"

/**
 * @class SGMDFItemDelegate SGMDFItemDelegate.h StatsGenerator/SGMDFItemDelegate.h
 * @brief This class creates the appropriate Editor Widget for the Tables
 *
 * @date Dec 28, 2010
 * @version 1.0
 */
class SGMDFItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

  public:
    explicit SGMDFItemDelegate(QObject* parent = 0) :
      QStyledItemDelegate(parent)
    {
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
      QStyledItemDelegate::paint(painter, option, index);
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
      QLineEdit* editor;
      QDoubleValidator* weightValidator;

      qint32 col = index.column();
      switch(col)
      {
        case SGMDFTableModel::Angle:
          editor = new QLineEdit(parent);
          editor->setFrame(false);
          weightValidator = new QDoubleValidator(editor);
          weightValidator->setDecimals(4);
          editor->setValidator(weightValidator);
          return editor;
        case SGMDFTableModel::Axis:
          editor = new QLineEdit(parent);
          editor->setFrame(false);
          return editor;
        case SGMDFTableModel::Weight:
          editor = new QLineEdit(parent);
          editor->setFrame(false);
          weightValidator = new QDoubleValidator(editor);
          weightValidator->setDecimals(4);
          editor->setValidator(weightValidator);
          return editor;

        default:
          break;
      }
      return QStyledItemDelegate::createEditor(parent, option, index);
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void setEditorData(QWidget* editor, const QModelIndex& index) const
    {
      qint32 col = index.column();
      if (col == SGMDFTableModel::Angle
          || col == SGMDFTableModel::Weight
          || col == SGMDFTableModel::Axis)
      {
        QLineEdit* lineEdit = qobject_cast<QLineEdit* > (editor);
        Q_ASSERT(lineEdit);
        lineEdit->setText(index.model()->data(index).toString());
      }
      else { QStyledItemDelegate::setEditorData(editor, index); }
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
    {
      //  qDebug() << "SGMDFItemDelegate::setModelData" << "\n";
      qint32 col = index.column();
      //  bool ok = false;
      if (col == SGMDFTableModel::Angle || col == SGMDFTableModel::Weight)
      {
        QLineEdit* lineEdit = qobject_cast<QLineEdit* > (editor);
        Q_ASSERT(lineEdit);
        bool ok = false;
        double v = lineEdit->text().toFloat(&ok);
        model->setData(index, v);
      }
      else if (col == SGMDFTableModel::Axis)
      {
        QLineEdit* lineEdit = qobject_cast<QLineEdit* > (editor);
        Q_ASSERT(lineEdit);
        model->setData(index, lineEdit->text());
      }
      else { QStyledItemDelegate::setModelData(editor, model, index); }
    }

  private:
    QModelIndex m_Index;
    // QWidget* m_Widget;
    //  QAbstractItemModel* m_Model;

};


#endif /* SGMDFITEMDELEGATE_H_ */

