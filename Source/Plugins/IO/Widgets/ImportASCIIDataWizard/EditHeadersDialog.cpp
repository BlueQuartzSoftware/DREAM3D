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

#include "EditHeadersDialog.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

#include "ASCIIDataModel.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EditHeadersDialog::EditHeadersDialog(QWidget* parent) :
QDialog(parent)
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EditHeadersDialog::~EditHeadersDialog()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EditHeadersDialog::setupGui()
{
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  ASCIIDataModel* model = ASCIIDataModel::Instance();

  // Ensure the headers storage vector is the correct size
  m_Headers.resize(model->columnCount());

  for (int i = 0; i < model->columnCount(); i++)
  {
    QLabel* label = new QLabel(this);
    QString columnName = "Column " + QString::number(i + 1) + ":";
    label->setText(QApplication::translate("EditHeadersDialog", columnName.toStdString().c_str(), 0));
    gridLayout->addWidget(label, i, 0, 1, 1);

    QLineEdit* lineEdit = new QLineEdit(this);
    gridLayout->addWidget(lineEdit, i, 1, 1, 1);
  }

  m_ButtonBox = new QDialogButtonBox(this);
  m_ButtonBox->setOrientation(Qt::Horizontal);
  m_ButtonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
  gridLayout->addWidget(m_ButtonBox, model->columnCount(), 0, 1, 2);

  connect(m_ButtonBox, SIGNAL(accepted()), this, SLOT(m_ButtonBox_accepted()));
  connect(m_ButtonBox, SIGNAL(rejected()), this, SLOT(m_ButtonBox_rejected()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EditHeadersDialog::m_ButtonBox_accepted()
{
  ASCIIDataModel* model = ASCIIDataModel::Instance();

  for (int i = 0; i < model->columnCount(); i++)
  {
    QLayoutItem* item = gridLayout->itemAtPosition(i, 1);
    if (nullptr != item)
    {
      QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(item->widget());
      if (nullptr != lineEdit)
      {
        m_Headers[i] = lineEdit->text();
      }
    }
  }

  accept();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EditHeadersDialog::m_ButtonBox_rejected()
{
  reject();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QString> EditHeadersDialog::getHeaders()
{
  return m_Headers;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EditHeadersDialog::setHeaders(QVector<QString> headers)
{
  m_Headers = headers;

  for (int i = 0; i < headers.size(); i++)
  {
    QLayoutItem* item = gridLayout->itemAtPosition(i, 1);
    if (nullptr != item)
    {
      QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(item->widget());
      if (nullptr != lineEdit)
      {
        lineEdit->setText(headers[i]);
      }
    }
  }
}