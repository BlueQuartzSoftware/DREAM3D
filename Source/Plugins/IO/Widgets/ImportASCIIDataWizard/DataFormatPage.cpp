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

#include "DataFormatPage.h"

#include <QtCore/QFile>

#include "SIMPLib/Common/Constants.h"

#include "ImportASCIIDataWizard.h"
#include "ASCIIDataModel.h"
#include "EditHeadersDialog.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataFormatPage::DataFormatPage(const QString &inputFilePath, int numLines, QWidget* parent) :
  AbstractWizardPage(inputFilePath, parent),
  m_NumLines(numLines),
  m_EditHeadersDialog(NULL)
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
  connect(tupleDimsTable, SIGNAL(tupleDimsChanged(QVector<size_t>)), this, SLOT(checkTupleDimensions(QVector<size_t>)));

  headersIndexLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[1-9][0-9]*"), headersIndexLineEdit));

  registerField("startRow", startRowSpin);

  QStringList dataTypes;
  dataTypes.push_back(SIMPL::TypeNames::Double);
  dataTypes.push_back(SIMPL::TypeNames::Float);
  dataTypes.push_back(SIMPL::TypeNames::Int8);
  dataTypes.push_back(SIMPL::TypeNames::Int16);
  dataTypes.push_back(SIMPL::TypeNames::Int32);
  dataTypes.push_back(SIMPL::TypeNames::Int64);
  dataTypes.push_back(SIMPL::TypeNames::UInt8);
  dataTypes.push_back(SIMPL::TypeNames::UInt16);
  dataTypes.push_back(SIMPL::TypeNames::UInt32);
  dataTypes.push_back(SIMPL::TypeNames::UInt64);
  dataTypeCB->addItems(dataTypes);
  dataTypeCB->setCurrentIndex(0);

  int beginIndex = startRowSpin->value();
  int numOfDataLines = m_NumLines - beginIndex + 1;

  tupleDimsTable->blockSignals(true);
  tupleDimsTable->addTupleDimensions(QVector<size_t>(1, numOfDataLines));
  tupleDimsTable->blockSignals(false);
  tupleCountLabel->setText(QString::number(numOfDataLines));

  editHeadersBtn->setDisabled(true);
  columnDataGroupBox->setDisabled(true);
  
  lineNumErrLabel->hide();
  tupleTableErrLabel->hide();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::showEvent(QShowEvent* event)
{
  bool tabAsDelimiter = field("tabAsDelimiter").toBool();
  bool semicolonAsDelimiter = field("semicolonAsDelimiter").toBool();
  bool commaAsDelimiter = field("commaAsDelimiter").toBool();
  bool spaceAsDelimiter = field("spaceAsDelimiter").toBool();
  bool consecutiveDelimiters = field("consecutiveDelimiters").toBool();

  ASCIIDataModel* model = ASCIIDataModel::Instance();
  QStringList lines = model->originalStrings();

  QList<char> delimiters = ImportASCIIDataWizard::ConvertToDelimiters(tabAsDelimiter, semicolonAsDelimiter, commaAsDelimiter, spaceAsDelimiter);

  QList<QStringList> tokenizedLines = ImportASCIIDataWizard::TokenizeLines(lines, delimiters, consecutiveDelimiters);
  ImportASCIIDataWizard::InsertTokenizedLines(tokenizedLines, startRowSpin->value());

  for (int i = 0; i < model->columnCount(); i++)
  {
    if (model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString().isEmpty() == true)
    {
      model->setColumnHasErrors(i, true);
      arrayErrLabel->setText("Column headers cannot be empty.");
      arrayErrLabel->show();
      wizard()->button(QWizard::FinishButton)->setDisabled(true);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_startRowSpin_valueChanged(int value)
{
  if (value > m_NumLines)
  {
    wizard()->button(QWizard::FinishButton)->setDisabled(true);
    tupleCountLabel->setText("ERR");
    return;
  }

  wizard()->button(QWizard::FinishButton)->setEnabled(true);

  tupleCountLabel->setText(QString::number(m_NumLines - value + 1));

  bool tabAsDelimiter = field("tabAsDelimiter").toBool();
  bool semicolonAsDelimiter = field("semicolonAsDelimiter").toBool();
  bool commaAsDelimiter = field("commaAsDelimiter").toBool();
  bool spaceAsDelimiter = field("spaceAsDelimiter").toBool();
  bool consecutiveDelimiters = field("consecutiveDelimiters").toBool();

  QStringList lines = ImportASCIIDataWizard::ReadLines(m_InputFilePath, value, ImportASCIIDataWizard::TotalPreviewLines);
  ImportASCIIDataWizard::LoadOriginalLines(lines);

  QList<char> delimiters = ImportASCIIDataWizard::ConvertToDelimiters(tabAsDelimiter, semicolonAsDelimiter, commaAsDelimiter, spaceAsDelimiter);

  QList<QStringList> tokenizedLines = ImportASCIIDataWizard::TokenizeLines(lines, delimiters, consecutiveDelimiters);
  ImportASCIIDataWizard::InsertTokenizedLines(tokenizedLines, startRowSpin->value());

  // Re-check the tuple dimensions
  checkTupleDimensions(tupleDimsTable->getData());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_hasHeadersRadio_toggled(bool checked)
{
  if (checked == true)
  {
    editHeadersBtn->setDisabled(true);
    lineNumberLabel->setEnabled(true);
    headersIndexLineEdit->setEnabled(true);

    // Reload the headers
    on_headersIndexLineEdit_textChanged(headersIndexLineEdit->text());
  }
  else
  {
    editHeadersBtn->setEnabled(true);
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
  bool ok = false;
  int lineNum = text.toInt(&ok);

  if ( text.isEmpty() == false && (lineNum > m_NumLines || ok == false) )
  {
    model->clearHeaders(Qt::Horizontal);
    validateHeaders(QVector<QString>());
    return;
  }

  headersIndexLineEdit->setStyleSheet("");
  lineNumErrLabel->hide();
  wizard()->button(QWizard::FinishButton)->setEnabled(true);

  if (text.isEmpty() == true)
  {
    model->clearHeaders(Qt::Horizontal);
    validateHeaders(QVector<QString>());
    return;
  }

  QString line = ImportASCIIDataWizard::ReadLine(m_InputFilePath, lineNum);
  
  QStringList list;
  list.push_back(line);

  bool tabAsDelimiter = field("tabAsDelimiter").toBool();
  bool semicolonAsDelimiter = field("semicolonAsDelimiter").toBool();
  bool commaAsDelimiter = field("commaAsDelimiter").toBool();
  bool spaceAsDelimiter = field("spaceAsDelimiter").toBool();
  bool consecutiveDelimiters = field("consecutiveDelimiters").toBool();

  QList<char> delimiters = ImportASCIIDataWizard::ConvertToDelimiters(tabAsDelimiter, semicolonAsDelimiter, commaAsDelimiter, spaceAsDelimiter);

  QList<QStringList> result = ImportASCIIDataWizard::TokenizeLines(list, delimiters, consecutiveDelimiters);

  QStringList tokenizedLine = result[0];

  QVector<QString> headers;
  for (int i = 0; i < model->columnCount(); i++)
  {
    QString header = "";

    if (i < tokenizedLine.size())
    {
      header = tokenizedLine[i];
    }

    model->setHeaderData(i, Qt::Horizontal, header, Qt::DisplayRole);

    headers.push_back(header);
  }

  validateHeaders(headers);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_editHeadersBtn_clicked()
{
  if (NULL == m_EditHeadersDialog)
  {
    m_EditHeadersDialog = new EditHeadersDialog(this);
  }

  launchEditHeadersDialog();
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
void DataFormatPage::checkTupleDimensions(QVector<size_t> tupleDims)
{
  int tupleTotal = 1;

  for (int i = 0; i < tupleDims.size(); i++)
  {
    tupleTotal = tupleTotal * tupleDims[i];
  }

  int beginIndex = startRowSpin->value();
  int numOfDataLines = m_NumLines - beginIndex + 1;
  if (tupleTotal != numOfDataLines)
  {
    tupleTableErrLabel->setText("The current tuple dimensions do not match the total number of tuples.");
    tupleTableErrLabel->show();
    wizard()->button(QWizard::FinishButton)->setDisabled(true);
  }
  else
  {
    tupleTableErrLabel->setText("");
    tupleTableErrLabel->hide();
    wizard()->button(QWizard::FinishButton)->setEnabled(true);
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

  ASCIIDataModel* model = ASCIIDataModel::Instance();
  for (int i = 0; i < model->columnCount(); i++)
  {
    model->setColumnHasErrors(i, false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::refreshModel()
{
  ASCIIDataModel* model = ASCIIDataModel::Instance();
  model->clear();

  bool tabAsDelimiter = field("tabAsDelimiter").toBool();
  bool semicolonAsDelimiter = field("semicolonAsDelimiter").toBool();
  bool commaAsDelimiter = field("commaAsDelimiter").toBool();
  bool spaceAsDelimiter = field("spaceAsDelimiter").toBool();
  bool consecutiveDelimiters = field("consecutiveDelimiters").toBool();

  QStringList lines = ImportASCIIDataWizard::ReadLines(m_InputFilePath, startRowSpin->value(), ImportASCIIDataWizard::TotalPreviewLines);

  ImportASCIIDataWizard::LoadOriginalLines(lines);

  QList<char> delimiters = ImportASCIIDataWizard::ConvertToDelimiters(tabAsDelimiter, semicolonAsDelimiter, commaAsDelimiter, spaceAsDelimiter);

  QList<QStringList> tokenizedLines = ImportASCIIDataWizard::TokenizeLines(lines, delimiters, consecutiveDelimiters);
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
void DataFormatPage::launchEditHeadersDialog()
{
  if (NULL != m_EditHeadersDialog)
  {
    ASCIIDataModel* model = ASCIIDataModel::Instance();
    QVector<QString> currentHeaders;
    for (int i = 0; i < model->columnCount(); i++)
    {
      currentHeaders.push_back(model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString());
      model->setHeaderData(i, Qt::Horizontal, QString::number(i + 1), Qt::DisplayRole);
    }

    int result = m_EditHeadersDialog->exec();
    if (result == QDialog::Accepted)
    {
      QVector<QString> headers = m_EditHeadersDialog->getHeaders();

      validateHeaders(headers);
    }
    else
    {
      for (int i = 0; i < model->columnCount(); i++)
      {
        model->setHeaderData(i, Qt::Horizontal, currentHeaders[i], Qt::DisplayRole);
      }

      m_EditHeadersDialog->setHeaders(currentHeaders);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::validateHeaders(QVector<QString> headers)
{
  ASCIIDataModel* model = ASCIIDataModel::Instance();

  bool hasSlashes = false;
  bool hasDuplicates = false;

  bool isEmpty = false;
  if (headers.size() <= 0)
  {
    isEmpty = true;
    for (int i = 0; i < model->columnCount(); i++)
    {
      model->setColumnHasErrors(i, true);
    }
  }
  for (int i = 0; i < headers.size(); i++)
  {
    QString header = headers[i];

    if (header.isEmpty() == true)
    {
      isEmpty = true;
      model->setColumnHasErrors(i, true);
    }
    else if (header.contains('/') || header.contains('\\'))
    {
      hasSlashes = true;
      model->setColumnHasErrors(i, true);
    }
    else
    {
      model->setColumnHasErrors(i, false);
    }

    for (int j = 0; j < headers.size(); j++)
    {
      QString otherHeader = headers[j];

      if (i != j && header.isEmpty() == false && otherHeader.isEmpty() == false && header == otherHeader)
      {
        hasDuplicates = true;
        model->setColumnHasErrors(i, true);
        model->setColumnHasErrors(j, true);
      }
      else if (hasDuplicates == false && isEmpty == false && hasSlashes == false)
      {
        model->setColumnHasErrors(i, false);
        model->setColumnHasErrors(j, false);
      }
    }

    model->setHeaderData(i, Qt::Horizontal, header, Qt::DisplayRole);
  }

  if (isEmpty == true)
  {
    arrayErrLabel->setText("Column headers cannot be empty.");
    arrayErrLabel->show();
    wizard()->button(QWizard::FinishButton)->setDisabled(true);
  }
  else if (hasSlashes == true)
  {
    arrayErrLabel->setText("Column headers cannot contain slashes.");
    arrayErrLabel->show();
    wizard()->button(QWizard::FinishButton)->setDisabled(true);
  }
  else if (hasDuplicates == true)
  {
    arrayErrLabel->setText("Column headers cannot have the same name.");
    arrayErrLabel->show();
    wizard()->button(QWizard::FinishButton)->setDisabled(true);
  }
  else
  {
    arrayErrLabel->setText("");
    arrayErrLabel->hide();
    wizard()->button(QWizard::FinishButton)->setEnabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TupleTableWidget* DataFormatPage::getTupleTable()
{
  return tupleDimsTable;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataFormatPage::nextId() const
{
  // There is no "Next" page
  return -1;
}
