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

#include "SIMPLib/Common/Constants.h"

#include "ImportASCIIDataWizard.h"
#include "ASCIIDataModel.h"
#include "AddHeadersDialog.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataFormatPage::DataFormatPage(const QString &inputFilePath, QWidget* parent) :
  AbstractWizardPage(inputFilePath, parent)
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
  ASCIIDataModel* model = ASCIIDataModel::Instance();

  dataView->setModel(model);
  dataView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  connect(dataView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(updateSelection(const QItemSelection&, const QItemSelection&)));

  headersIndexLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[1-9]|1[0-9]|20"), headersIndexLineEdit));

  registerField("startRow", startRowSpin);

  QStringList dataTypes;
  dataTypes.push_back(DREAM3D::TypeNames::Double);
  dataTypes.push_back(DREAM3D::TypeNames::Float);
  dataTypes.push_back(DREAM3D::TypeNames::Int8);
  dataTypes.push_back(DREAM3D::TypeNames::Int16);
  dataTypes.push_back(DREAM3D::TypeNames::Int32);
  dataTypes.push_back(DREAM3D::TypeNames::Int64);
  dataTypes.push_back(DREAM3D::TypeNames::UInt8);
  dataTypes.push_back(DREAM3D::TypeNames::UInt16);
  dataTypes.push_back(DREAM3D::TypeNames::UInt32);
  dataTypes.push_back(DREAM3D::TypeNames::UInt64);
  dataTypeCB->addItems(dataTypes);
  dataTypeCB->setCurrentIndex(0);

  addHeadersBtn->setDisabled(true);
  columnDataGroupBox->setDisabled(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::showEvent(QShowEvent* event)
{
  bool isFixedWidth = field("isFixedWidth").toBool();
  bool tabAsDelimiter = field("tabAsDelimiter").toBool();
  bool semicolonAsDelimiter = field("semicolonAsDelimiter").toBool();
  bool commaAsDelimiter = field("commaAsDelimiter").toBool();
  bool spaceAsDelimiter = field("spaceAsDelimiter").toBool();
  bool consecutiveDelimiters = field("consecutiveDelimiters").toBool();

  ASCIIDataModel* model = ASCIIDataModel::Instance();
  QStringList lines = model->originalStrings();

  QList<char> delimiters = ImportASCIIDataWizard::ConvertToDelimiters(tabAsDelimiter, semicolonAsDelimiter, commaAsDelimiter, spaceAsDelimiter);

  QList<QStringList> tokenizedLines = ImportASCIIDataWizard::TokenizeLines(lines, delimiters, isFixedWidth, consecutiveDelimiters);
  ImportASCIIDataWizard::InsertTokenizedLines(tokenizedLines, startRowSpin->value());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_startRowSpin_valueChanged(int value)
{
  bool isFixedWidth = field("isFixedWidth").toBool();
  bool tabAsDelimiter = field("tabAsDelimiter").toBool();
  bool semicolonAsDelimiter = field("semicolonAsDelimiter").toBool();
  bool commaAsDelimiter = field("commaAsDelimiter").toBool();
  bool spaceAsDelimiter = field("spaceAsDelimiter").toBool();
  bool consecutiveDelimiters = field("consecutiveDelimiters").toBool();

  QStringList lines = ImportASCIIDataWizard::ReadLines(m_InputFilePath, value, ImportASCIIDataWizard::TotalPreviewLines);
  ImportASCIIDataWizard::LoadOriginalLines(lines);

  QList<char> delimiters = ImportASCIIDataWizard::ConvertToDelimiters(tabAsDelimiter, semicolonAsDelimiter, commaAsDelimiter, spaceAsDelimiter);

  QList<QStringList> tokenizedLines = ImportASCIIDataWizard::TokenizeLines(lines, delimiters, isFixedWidth, consecutiveDelimiters);
  ImportASCIIDataWizard::InsertTokenizedLines(tokenizedLines, startRowSpin->value());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_hasHeadersRadio_toggled(bool checked)
{
  if (checked == true)
  {
    addHeadersBtn->setDisabled(true);
    lineNumberLabel->setEnabled(true);
    headersIndexLineEdit->setEnabled(true);

    // Reload the headers
    on_headersIndexLineEdit_textChanged(headersIndexLineEdit->text());
  }
  else
  {
    addHeadersBtn->setEnabled(true);
    lineNumberLabel->setDisabled(true);
    headersIndexLineEdit->setDisabled(true);

    ASCIIDataModel* model = ASCIIDataModel::Instance();
    for (int i = 0; i < model->columnCount(); i++)
    {
      model->setHeaderData(i, Qt::Horizontal, "", Qt::DisplayRole);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_headersIndexLineEdit_textChanged(const QString &text)
{
  ASCIIDataModel* model = ASCIIDataModel::Instance();
  int lineNum = text.toInt();

  QString line = ImportASCIIDataWizard::ReadLine(m_InputFilePath, lineNum);
  
  QStringList list;
  list.push_back(line);

  bool isFixedWidth = field("isFixedWidth").toBool();
  bool tabAsDelimiter = field("tabAsDelimiter").toBool();
  bool semicolonAsDelimiter = field("semicolonAsDelimiter").toBool();
  bool commaAsDelimiter = field("commaAsDelimiter").toBool();
  bool spaceAsDelimiter = field("spaceAsDelimiter").toBool();
  bool consecutiveDelimiters = field("consecutiveDelimiters").toBool();

  QList<char> delimiters = ImportASCIIDataWizard::ConvertToDelimiters(tabAsDelimiter, semicolonAsDelimiter, commaAsDelimiter, spaceAsDelimiter);

  QList<QStringList> result = ImportASCIIDataWizard::TokenizeLines(list, delimiters, isFixedWidth, consecutiveDelimiters);

  QStringList tokenizedLine = result[0];

  for (int i = 0; i < model->columnCount(); i++)
  {
    QString header = "";

    if (i < tokenizedLine.size())
    {
      header = tokenizedLine[i];
    }

    model->setHeaderData(i, Qt::Horizontal, header, Qt::DisplayRole);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_addHeadersBtn_clicked()
{
  AddHeadersDialog* addHeadersDialog = new AddHeadersDialog();
  int result = addHeadersDialog->exec();

  if (result == QDialog::Accepted)
  {
    QStringList headers = addHeadersDialog->getHeaders();
    ASCIIDataModel* model = ASCIIDataModel::Instance();

    for (int i = 0; i < headers.size(); i++)
    {
      QString header = headers[i];

      model->setHeaderData(i, Qt::Horizontal, header, Qt::DisplayRole);
    }
  }

  delete addHeadersDialog;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::updateSelection(const QItemSelection &selected, const QItemSelection &deselected)
{
  ASCIIDataModel* model = ASCIIDataModel::Instance();
  QModelIndexList selectedIndexList = selected.indexes();

  if (selectedIndexList.size() <= 0)
  {
    columnDataGroupBox->setDisabled(true);
  }
  else
  {
    columnDataGroupBox->setEnabled(true);

    int selectedColumn = selectedIndexList[0].column();
    QString selectedType = model->columnDataType(selectedColumn);

    if (selectedType.isEmpty())
    {
      dataTypeRadio->setChecked(true);
      dataTypeCB->setEnabled(true);
      dataTypeCB->setCurrentIndex(0);
    }
    else if (selectedType == "Skip")
    {
      skipRadio->setChecked(true);
      dataTypeCB->setDisabled(true);
    }
    else
    {
      dataTypeRadio->setChecked(true);
      dataTypeCB->setEnabled(true);
      dataTypeCB->setCurrentText(selectedType);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_dataTypeRadio_clicked()
{
  ASCIIDataModel* model = ASCIIDataModel::Instance();
  QModelIndexList indexList = dataView->selectionModel()->selectedColumns();

  if (indexList.size() > 0)
  {
    QModelIndex index = indexList[0];
    int column = index.column();

    model->setColumnDataType(column, dataTypeCB->currentText());
  }

  dataTypeCB->setEnabled(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_dataTypeCB_currentTextChanged(const QString &text)
{
  on_dataTypeRadio_clicked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_skipRadio_clicked()
{
  ASCIIDataModel* model = ASCIIDataModel::Instance();
  QModelIndexList indexList = dataView->selectionModel()->selectedColumns();

  if (indexList.size() > 0)
  {
    QModelIndex index = indexList[0];
    int column = index.column();

    model->setColumnDataType(column, "Skip");
  }

  dataTypeCB->setDisabled(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::cleanupPage()
{
  startRowSpin->setValue(1);
  headersIndexLineEdit->clear();
  hasHeadersRadio->setChecked(true);
  doesNotHaveHeadersRadio->setChecked(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::refreshModel()
{
  ASCIIDataModel* model = ASCIIDataModel::Instance();
  model->clear();

  bool isFixedWidth = field("isFixedWidth").toBool();
  bool tabAsDelimiter = field("tabAsDelimiter").toBool();
  bool semicolonAsDelimiter = field("semicolonAsDelimiter").toBool();
  bool commaAsDelimiter = field("commaAsDelimiter").toBool();
  bool spaceAsDelimiter = field("spaceAsDelimiter").toBool();
  bool consecutiveDelimiters = field("consecutiveDelimiters").toBool();

  QStringList lines = ImportASCIIDataWizard::ReadLines(m_InputFilePath, startRowSpin->value(), ImportASCIIDataWizard::TotalPreviewLines);

  ImportASCIIDataWizard::LoadOriginalLines(lines);

  QList<char> delimiters = ImportASCIIDataWizard::ConvertToDelimiters(tabAsDelimiter, semicolonAsDelimiter, commaAsDelimiter, spaceAsDelimiter);

  QList<QStringList> tokenizedLines = ImportASCIIDataWizard::TokenizeLines(lines, delimiters, isFixedWidth, consecutiveDelimiters);
  ImportASCIIDataWizard::InsertTokenizedLines(tokenizedLines, startRowSpin->value());

  // Refresh the headers
  if (hasHeadersRadio->isChecked())
  {
    on_headersIndexLineEdit_textChanged(headersIndexLineEdit->text());
  }
  else
  {

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
