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
#include "TupleTableWidget.h"

#include <QtCore/QMetaProperty>

#include <QtWidgets/QMessageBox>
#include <QtWidgets/QTableWidgetItem>

#include "TupleTableItemDelegate.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TupleTableWidget::TupleTableWidget(QWidget* parent) :
  QWidget(parent)
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TupleTableWidget::~TupleTableWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TupleTableWidget::setupGui()
{
  // Set the item delegate so that we can only enter 'double' values into the table
  TupleTableItemDelegate* dlg = new TupleTableItemDelegate(tupleTable);
  tupleTable->setItemDelegate(dlg);

  tupleTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

  // Set Icons
  QIcon addIcon = QIcon(QString(":/add2.png"));
  QIcon deleteIcon = QIcon(QString(":/delete2.png"));
  addTupleBtn->setIcon(addIcon);
  deleteTupleBtn->setIcon(deleteIcon);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<size_t> TupleTableWidget::getData()
{
  int cCount = tupleTable->columnCount();
  QVector<size_t> data(cCount, 0);

  for (int col = 0; col < cCount; col++)
  {
    QTableWidgetItem* item = tupleTable->item(0, col);
    if (NULL == item)
    {
      return QVector<size_t>();
    }
    data[col] = item->data(Qt::DisplayRole).toInt();
  }

  return data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TupleTableWidget::addTupleDimensions(QVector<size_t> tupleDims)
{
  for (int i = 0; i < tupleDims.size(); i++)
  {
    addColumn(tupleDims[i]);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TupleTableWidget::addColumn(int value)
{
  int col = tupleTable->columnCount();

  // If we are adding the first column, add the first row too.
  if (col <= 0)
  {
    tupleTable->insertRow(0);
  }

  tupleTable->insertColumn(col);

  QTableWidgetItem* item = new QTableWidgetItem(QString::number(value));
  tupleTable->setItem(0, col, item);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TupleTableWidget::on_addTupleBtn_pressed()
{
  addColumn(1);

  emit tupleDimsChanged(getData());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TupleTableWidget::on_deleteTupleBtn_pressed()
{
  int currentColumn = tupleTable->currentColumn();

  if (currentColumn >= 0)
  {
    tupleTable->removeColumn(tupleTable->currentColumn());
  }

  if (tupleTable->columnCount() == 0)
  {
    tupleTable->removeRow(0);
  }

  emit tupleDimsChanged(getData());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TupleTableWidget::on_tupleTable_itemChanged(QTableWidgetItem* item)
{
  emit tupleDimsChanged(getData());
}



