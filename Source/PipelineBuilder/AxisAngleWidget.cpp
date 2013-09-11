/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#include "AxisAngleWidget.h"

//-- C++ Includes
#include <iostream>
#include <fstream>

#include <QtGui/QFileDialog>


#include "AxisAngleTableModel.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AxisAngleWidget::AxisAngleWidget(QWidget *parent) :
  QWidget(parent),
  m_TableModel(NULL)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AxisAngleWidget::~AxisAngleWidget()
{
  if (NULL != m_TableModel)
  {
    m_TableModel->deleteLater();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AxisAngleWidget::setupGui()
{
  m_TableModel = new AxisAngleTableModel;
  m_TableModel->setInitialValues();
  m_TableView->setModel(m_TableModel);
  QAbstractItemDelegate* aid = m_TableModel->getItemDelegate();
  m_TableView->setItemDelegate(aid);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AxisAngleWidget::on_addRow_clicked()
{
  if (!m_TableModel->insertRow(m_TableModel->rowCount())) return;
  m_TableView->resizeColumnsToContents();
  m_TableView->scrollToBottom();
  m_TableView->setFocus();
  QModelIndex index = m_TableModel->index(m_TableModel->rowCount() - 1, 0);
  m_TableView->setCurrentIndex(index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AxisAngleWidget::on_deleteRow_clicked()
{
  QItemSelectionModel *selectionModel = m_TableView->selectionModel();
  if (!selectionModel->hasSelection()) return;
  QModelIndex index = selectionModel->currentIndex();
  if (!index.isValid()) return;
  m_TableModel->removeRow(index.row(), index.parent());
  if (m_TableModel->rowCount() > 0)
  {
    m_TableView->resizeColumnsToContents();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<AxisAngleInput_t> AxisAngleWidget::getAxisAngleRotations()
{
  QVector<AxisAngleInput_t> comps;
  if (m_TableModel == NULL) { return comps; }

  int filterCount = m_TableModel->rowCount();
  QVector<float> angles = m_TableModel->getData(AxisAngleTableModel::Angle);
  QVector<float> axis = m_TableModel->getData(AxisAngleTableModel::Axis);

  for(int i = 0; i < filterCount; ++i)
  {
    AxisAngleInput_t comp;
    comp.angle = angles[i];
    comp.h = axis[i*3 + 0];
    comp.k = axis[i*3 + 1];
    comp.l = axis[i*3 + 2];
    comps.push_back(comp);
  }
  return comps;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AxisAngleTableModel* AxisAngleWidget::getTableModel()
{
  return m_TableModel;
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AxisAngleWidget::on_loadData_clicked()
{
  QString proposedFile = m_OpenDialogLastDirectory;
  QString file = QFileDialog::getOpenFileName(this, tr("Open Axis ODF File"), proposedFile, tr("Text Document (*.txt)"));
  if(true == file.isEmpty())
  {
    return;
  }
  else
  {
    size_t numMisorients = 0;
    QString filename = file.toStdString();
    std::ifstream inFile;
    inFile.open(filename.c_str());

    inFile >> numMisorients;

    float angle, weight;
    QString axis, n1, n2, n3;
    for(size_t i = 0; i < numMisorients; i++)
    {
      inFile >> angle >> n1 >> n2 >> n3 >> weight;

      axis = std::string("<" + n1 + "," + n2 + "," + n3 + ">");

      if (!m_TableModel->insertRow(m_TableModel->rowCount())) return;
      int row = m_TableModel->rowCount() - 1;
      m_TableModel->setRowData(row, angle, axis);

      m_TableView->resizeColumnsToContents();
      m_TableView->scrollToBottom();
      m_TableView->setFocus();
      QModelIndex index = m_TableModel->index(m_TableModel->rowCount() - 1, 0);
      m_TableView->setCurrentIndex(index);
    }
  }
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AxisAngleWidget::readOptions(QSettings &prefs, QString name)
{
  int count = prefs.beginReadArray(name);

  QVector<float> angles(count);
  QVector<float> axis(count * 3);
  bool ok = false;
  for(int i = 0; i < count; ++i)
  {
    prefs.setArrayIndex(i);
    angles[i] = prefs.value("Angle").toFloat(&ok);
    axis[i*3 + 0] = prefs.value("H").toFloat(&ok);
    axis[i*3 + 1] = prefs.value("K").toFloat(&ok);
    axis[i*3 + 2] = prefs.value("L").toFloat(&ok);
  }
  prefs.endArray();

  m_TableModel->setTableData(angles, axis);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AxisAngleWidget::writeOptions(QSettings &prefs, QString name)
{
  int count = m_TableModel->rowCount();
  QVector<float> angles = m_TableModel->getData(AxisAngleTableModel::Angle);
  QVector<float> axis = m_TableModel->getData(AxisAngleTableModel::Axis);
  prefs.beginWriteArray(name, count);
  for(int i = 0; i < count; ++i)
  {
    prefs.setArrayIndex(i);
    prefs.setValue("Angle", QString::number(angles[i]));
    prefs.setValue("H", QString::number(axis[i*3 + 0]));
    prefs.setValue("K", QString::number(axis[i*3 + 1]));
    prefs.setValue("L", QString::number(axis[i*3 + 2]));
  }
  prefs.endArray();
}


