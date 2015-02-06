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
#ifndef _SGODFITEMDELEGATE_H_
#define _SGODFITEMDELEGATE_H_

#include <iostream>

#include <QtCore/QModelIndex>
#include <QtWidgets/QComboBox>
#include <QtGui/QPainter>
#include <QtWidgets/QStyleOptionViewItemV4>
#include <QtWidgets/QLineEdit>
#include <QtGui/QDoubleValidator>
#include <QtWidgets/QStyledItemDelegate>

#include "EbsdLib/EbsdConstants.h"

#include "DREAM3DLib/Common/StatsGen.hpp"
#include "StatsGenerator/TableModels/SGODFTableModel.h"

/**
 * @class SGODFItemDelegate SGODFItemDelegate.h StatsGenerator/SGODFItemDelegate.h
 * @brief This class creates the appropriate Editor Widget for the Tables
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Dec 28, 2010
 * @version 1.0
 */
class SGODFItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

  public:
    explicit SGODFItemDelegate(unsigned int crystalStructure, QObject* parent = 0) :
      QStyledItemDelegate(parent),
      m_CrystalStructure(crystalStructure)
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
      QDoubleValidator* validator;
      unsigned int sigma_top = 1;
      if ( Ebsd::CrystalStructure::Cubic_High == m_CrystalStructure )
      {
        sigma_top = 18;
      }
      else if ( Ebsd::CrystalStructure::Hexagonal_High == m_CrystalStructure )
      {
        sigma_top = 36;
      }
      qint32 col = index.column();
      if (col == SGODFTableModel::Sigma)
      {
        editor = new QLineEdit(parent);
        editor->setFrame(false);
        validator = new QDoubleValidator(editor);
        validator->setDecimals(0);
        validator->setBottom(1);
        validator->setTop(sigma_top);
        editor->setValidator(validator);
        return editor;
      }
      else if (col < SGODFTableModel::ColumnCount)
      {
        editor = new QLineEdit(parent);
        editor->setFrame(false);
        validator = new QDoubleValidator(editor);
        validator->setDecimals(4);
        editor->setValidator(validator);
        return editor;
      }
      return QStyledItemDelegate::createEditor(parent, option, index);
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void setEditorData(QWidget* editor, const QModelIndex& index) const
    {
      qint32 col = index.column();
      if (col < SGODFTableModel::ColumnCount)
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
      qint32 col = index.column();
      if (col < SGODFTableModel::ColumnCount)
      {
        QLineEdit* lineEdit = qobject_cast<QLineEdit* > (editor);
        Q_ASSERT(lineEdit);
        bool ok = false;
        double v = lineEdit->text().toFloat(&ok);
        model->setData(index, v);
      }
      else { QStyledItemDelegate::setModelData(editor, model, index); }
    }

  private:
    QModelIndex m_Index;
    unsigned int m_CrystalStructure;

};

#endif /* _SGODFITEMDELEGATE_H_ */

