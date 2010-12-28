/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "SGItemDelegate.h"

#include <iostream>


#include <QtGui/QComboBox>
#include <QtGui/QPainter>
#include <QtGui/QStyleOptionViewItemV4>
#include <QtGui/QDoubleSpinBox>


#include "StatsGenTableModel.h"
#include "AIM/Common/Qt/ColorComboPicker.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
        // adjusted gives a 3 pixel right margin
void SGItemDelegate::paint(QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
  QStyledItemDelegate::paint(painter, option, index);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* SGItemDelegate::createEditor(QWidget *parent,
                                      const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const
{
  QDoubleSpinBox* spin;
  QDoubleSpinBox* spin2;
  QComboBox* colorCombo;

  qint32 col = index.column();
  switch(col)
  {
    case StatsGenTableModel::BinNumber:
      return NULL;
      break;
    case StatsGenTableModel::NumGrains:
      return NULL;
      break;
    case StatsGenTableModel::Mu:
      spin = new QDoubleSpinBox(parent);
      spin->setRange(0.0, 1000);
      spin->setDecimals(5);
      spin->setSingleStep(0.1);
      return spin;
    case StatsGenTableModel::Sigma:
      spin2 = new QDoubleSpinBox(parent);
      spin2->setRange(0.0, 1000);
      spin2->setDecimals(5);
      spin2->setSingleStep(0.1);
      return spin2;
    case StatsGenTableModel::LineColor:
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
void SGItemDelegate::setEditorData(QWidget *editor,
                                   const QModelIndex &index) const
{
  qint32 col = index.column();
  bool ok = false;
  if (col == StatsGenTableModel::Mu || col == StatsGenTableModel::Sigma)
  {
    double value = index.model()->data(index).toDouble(&ok);
    QDoubleSpinBox* spinBox = qobject_cast<QDoubleSpinBox* > (editor);
    Q_ASSERT(spinBox);
    spinBox->setValue(value);
  }
  else if (col == StatsGenTableModel::LineColor)
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
void SGItemDelegate::setModelData(QWidget *editor,
                                  QAbstractItemModel *model,
                                  const QModelIndex &index) const
{
//  std::cout << "SGItemDelegate::setModelData" << std::endl;
  qint32 col = index.column();
//  bool ok = false;
  if (col == StatsGenTableModel::Mu || col == StatsGenTableModel::Sigma)
  {
    QDoubleSpinBox* spinBox = qobject_cast<QDoubleSpinBox* > (editor);
    Q_ASSERT(spinBox);
    spinBox->interpretText();
    model->setData(index, spinBox->value());
  }
  else if (col == StatsGenTableModel::LineColor)
  {
    ColorComboPicker *comboBox = qobject_cast<ColorComboPicker* > (editor);
    Q_ASSERT(comboBox);
    model->setData(index, comboBox->currentText());
  }
  else QStyledItemDelegate::setModelData(editor, model, index);
}
