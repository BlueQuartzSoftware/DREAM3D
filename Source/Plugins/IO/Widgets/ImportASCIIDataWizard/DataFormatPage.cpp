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

#include "DataFormatPage.h"

#include <QtCore/QFile>

#include "ImportASCIIDataWizard.h"
#include "ASCIIDataModel.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataFormatPage::DataFormatPage(const QString &inputFilePath, QWidget* parent) :
  QWizardPage(parent),
  m_InputFilePath(inputFilePath),
  m_DataModel(new ASCIIDataModel())
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataFormatPage::~DataFormatPage()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::setupGui()
{
  dataView->setModel(m_DataModel);
  dataView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  registerField("startRow", startRowSpin);
  registerField("hasHeaders", hasHeadersCheckBox);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::showEvent(QShowEvent* event)
{
  m_DataModel->clearContents();

  QVector<QString> lines = ImportASCIIDataWizard::ReadLines(m_InputFilePath, startRowSpin->value(), ImportASCIIDataWizard::TotalPreviewLines);

  int vHeaderIndex = startRowSpin->value();
  bool isDelimited = field("isDelimited").toBool();
  if (isDelimited == true)
  {
    for (int i = 0; i < lines.size(); i++)
    {
      QString line = lines[i];

      int row = m_DataModel->rowCount();
      m_DataModel->insertRow(row);

      QModelIndex index = m_DataModel->index(row, 0);
      m_DataModel->setData(index, line, Qt::DisplayRole);

      m_DataModel->setHeaderData(row, Qt::Vertical, QString::number(vHeaderIndex), Qt::DisplayRole);
      vHeaderIndex++;
    }
  }
  else
  {
    for (int i = 0; i < lines.size(); i++)
    {
      QString line = lines[i];

      int row = m_DataModel->rowCount();
      m_DataModel->insertRow(row);

      line = line.replace('\t', ' ');
      QStringList columnDataList = line.split(' ', QString::SkipEmptyParts);
      int column = m_DataModel->columnCount();

      if (column != columnDataList.size())
      {
        m_DataModel->insertColumn(column);
      }

      for (int j = 0; j < columnDataList.size(); j++)
      {
        QString columnData = columnDataList[j];
        QModelIndex index = m_DataModel->index(row, column);
        m_DataModel->setData(index, columnData, Qt::DisplayRole);
      }

      m_DataModel->setHeaderData(row, Qt::Vertical, QString::number(vHeaderIndex), Qt::DisplayRole);
      vHeaderIndex++;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_startRowSpin_valueChanged(int value)
{
  m_DataModel->clearContents();

  QVector<QString> lines = ImportASCIIDataWizard::ReadLines(m_InputFilePath, value, ImportASCIIDataWizard::TotalPreviewLines);

  for (int i = 0; i < lines.size(); i++)
  {
    QString line = lines[i];

    int row = m_DataModel->rowCount();
    m_DataModel->insertRow(row);

    // This is the first screen, so everything automatically goes into one column for now
    QModelIndex index = m_DataModel->index(row, 0);

    m_DataModel->setData(index, line, Qt::DisplayRole);
    m_DataModel->setHeaderData(row, Qt::Vertical, QString::number(i + 1), Qt::DisplayRole);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataFormatPage::nextId() const
{
  // There is no "Next" page
  return -1;
}
