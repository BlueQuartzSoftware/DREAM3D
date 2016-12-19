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

#include <QtCore/QSignalMapper>

#include <QtWidgets/QMenu>
#include <QtWidgets/QDesktopWidget>

#include "SIMPLib/Common/Constants.h"
#include "SVWidgetsLib/QtSupport/QtSStyles.h"
#include "SVWidgetsLib/QtSupport/QtSFaderWidget.h"

#include "ASCIIDataModel.h"
#include "EditHeadersDialog.h"
#include "ImportASCIIDataWizard.h"

namespace Detail
{
  const QString Delimiter(" / ");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataFormatPage::DataFormatPage(const QString &inputFilePath, int numLines, DataContainerArray::Pointer dca, QWidget* parent) :
  AbstractWizardPage(inputFilePath, parent),
  m_NumLines(numLines),
  m_EditHeadersDialog(nullptr),
  m_Dca(dca)
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

  connect(dataView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(updateSelection(const QItemSelection&, const QItemSelection&)));
  connect(tupleDimsTable, SIGNAL(tupleDimsChanged(QVector<size_t>)), this, SLOT(checkTupleDimensions(QVector<size_t>)));
  connect(amName, SIGNAL(textEdited(const QString&)), this, SLOT(widgetChanged(const QString&)));

  headersIndexLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[1-9][0-9]*"), headersIndexLineEdit));

  registerField("startRow", startRowSpin);

  // Do not allow the user to put a forward slash into the attributeMatrixName line edit
  amName->setValidator(new QRegularExpressionValidator(QRegularExpression("[^/]*"), this));

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

  selectedDCBtn->setStyleSheet(QtSStyles::DAPSelectionButtonStyle(false));
  selectedAMBtn->setStyleSheet(QtSStyles::DAPSelectionButtonStyle(false));

  m_AMMenuMapper = new QSignalMapper(this);
  connect(m_AMMenuMapper, SIGNAL(mapped(QString)),
            this, SLOT(amItemSelected(QString)));

  m_DCMenuMapper = new QSignalMapper(this);
  connect(m_DCMenuMapper, SIGNAL(mapped(QString)),
            this, SLOT(dcItemSelected(QString)));

  editHeadersBtn->setDisabled(true);
  columnDataGroupBox->setDisabled(true);

  lineNumErrLabel->hide();
  tupleTableErrLabel->hide();
  dcSelectionFrame->hide();
  amSelectionError->hide();
  amCreationError->hide();
  applyChangesBtn->hide();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::setEditSettings(bool value)
{
  m_EditSettings = value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::setUseDefaultHeaders(bool ok)
{
  useDefaultHeaders->setChecked(ok);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::setUseCustomHeaders(bool value)
{
  doesNotHaveHeadersRadio->setChecked(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::setHeaderLine(int line)
{
  if(line >= 0) {
    hasHeadersRadio->setChecked(true);
    headersIndexLineEdit->setText(QString::number(line));
  }
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

  if(!m_EditSettings)
  {
    QStringList lines = model->originalStrings();

    QList<char> delimiters = ImportASCIIDataWizard::ConvertToDelimiters(tabAsDelimiter, semicolonAsDelimiter, commaAsDelimiter, spaceAsDelimiter);

    QList<QStringList> tokenizedLines = ImportASCIIDataWizard::TokenizeLines(lines, delimiters, consecutiveDelimiters);
    ImportASCIIDataWizard::InsertTokenizedLines(tokenizedLines, startRowSpin->value());
  }
  for(int i = 0; i < model->columnCount(); i++)
  {
    if(model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString().isEmpty() == true)
    {
      model->setColumnHasErrors(i, true);
      arrayErrLabel->setText("Column headers cannot be empty.");
      arrayErrLabel->show();
    }
    else
    {
      model->setColumnHasErrors(i, false);
      arrayErrLabel->hide();
    }
  }

  if (amAutomatically->isChecked())
  {
    tupleDimsGB->hide();
  }
  else
  {
    tupleDimsGB->show();
  }

  if (isComplete() == true)
  {
    wizard()->button(QWizard::FinishButton)->setEnabled(true);
  }
  else
  {
    wizard()->button(QWizard::FinishButton)->setDisabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::amItemSelected(QString path)
{
  selectedAMBtn->setText(path);

  DataArrayPath dap = DataArrayPath::Deserialize(path, Detail::Delimiter);

  AttributeMatrix::Pointer am = m_Dca->getAttributeMatrix(dap);   // This will always be valid, because we create the selection options
  if (am->getTupleDimensions() != getTupleTable()->getData())
  {
    QString ss = "The attribute matrix '" + dap.getAttributeMatrixName() + "' does not have the same tuple dimensions as the data in the file.";
    amSelectionError->setText(ss);
    amSelectionError->show();
    selectedAMBtn->setStyleSheet(QtSStyles::DAPSelectionButtonStyle(false));
  }
  else
  {
    amSelectionError->hide();
    selectedAMBtn->setStyleSheet(QtSStyles::DAPSelectionButtonStyle(true));
  }

  ImportASCIIDataWizard* importWizard = dynamic_cast<ImportASCIIDataWizard*>(wizard());
  if (importWizard == nullptr) { return; }

  QStringList headers = importWizard->getHeaders();

  QStringList amArrays = am->getAttributeArrayNames();
  for (int i = 0; i < amArrays.size(); i++)
  {
    QString amArrayName = amArrays[i];
    for (int j = 0; j < headers.size(); j++)
    {
      QString headerName = headers[j];
      if (amArrayName == headerName)
      {
        QString ss = "The header name \"" + headerName + "\" matches an array name that already exists in the selected attribute matrix.";
        amSelectionError->setText(ss);
        amSelectionError->show();
        selectedAMBtn->setStyleSheet(QtSStyles::DAPSelectionButtonStyle(false));
      }
      else
      {
        amSelectionError->hide();
        selectedAMBtn->setStyleSheet(QtSStyles::DAPSelectionButtonStyle(true));
      }
    }
  }

  if (isComplete() == true)
  {
    wizard()->button(QWizard::FinishButton)->setEnabled(true);
  }
  else
  {
    wizard()->button(QWizard::FinishButton)->setDisabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::dcItemSelected(QString path)
{
  selectedDCBtn->setText(path);

  DataArrayPath dap = DataArrayPath::Deserialize(path, Detail::Delimiter);
  dap.setAttributeMatrixName(amName->text());

  if (m_Dca->doesAttributeMatrixExist(dap) == true)
  {
    QString ss = "An attribute matrix with that name already exists in the selected data container.  Choose a different attribute matrix name.";
    amCreationError->setText(ss);
    amCreationError->show();
    QtSStyles::LineEditErrorStyle(amName);
    return;
  }

  amCreationError->hide();
  selectedDCBtn->setStyleSheet(QtSStyles::DAPSelectionButtonStyle(true));
  QtSStyles::LineEditClearStyle(amName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::widgetChanged(const QString& text)
{
  Q_UNUSED(text)

  amName->setStyleSheet(QString::fromLatin1("color: rgb(255, 0, 0);"));
  amName->setToolTip("Press the 'Return' key to apply your changes");
  if (applyChangesBtn->isVisible() == false)
  {
    applyChangesBtn->setVisible(true);
  }
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_amName_returnPressed()
{
  on_applyChangesBtn_clicked();
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_applyChangesBtn_clicked()
{
  amName->setStyleSheet(QString(""));

  QPointer<QtSFaderWidget> faderWidget = new QtSFaderWidget(applyChangesBtn);
  m_FaderWidget = faderWidget;

  if (m_FaderWidget != nullptr)
  {
    m_FaderWidget->close();
  }
  m_FaderWidget = new QtSFaderWidget(applyChangesBtn);
  m_FaderWidget->setFadeOut();
  connect(m_FaderWidget, SIGNAL(animationComplete()),
    this, SLOT(hideButton()));
  m_FaderWidget->start();

  dcItemSelected(selectedDCBtn->text());

  if (isComplete() == true)
  {
    wizard()->button(QWizard::FinishButton)->setEnabled(true);
  }
  else
  {
    wizard()->button(QWizard::FinishButton)->setDisabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::hideButton()
{
  amName->setToolTip("");
  applyChangesBtn->setVisible(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_selectedAMBtn_pressed()
{
  // Get the menu and clear it out
  QMenu* menu = selectedAMBtn->menu();
  if(!menu)
  {
    menu = new QMenu();
    selectedAMBtn->setMenu(menu);
    menu->installEventFilter(this);
  }
  if(menu) {
    menu->clear();
  }

  // Get the DataContainerArray object
  // Loop over the data containers until we find the proper data container
  QList<DataContainer::Pointer> containers = m_Dca->getDataContainers();

  QListIterator<DataContainer::Pointer> containerIter(containers);
  while(containerIter.hasNext())
  {
    DataContainer::Pointer dc = containerIter.next();

    QMenu* dcMenu = new QMenu(dc->getName());
    menu->addMenu(dcMenu);

    // We found the proper Data Container, now populate the AttributeMatrix List
    DataContainer::AttributeMatrixMap_t attrMats = dc->getAttributeMatrices();
    QMapIterator<QString, AttributeMatrix::Pointer> attrMatsIter(attrMats);
    while(attrMatsIter.hasNext() )
    {
      attrMatsIter.next();
      QString amName = attrMatsIter.key();

      QAction* action = new QAction(amName, dcMenu);
      DataArrayPath daPath(dc->getName(), amName, "");
      QString path = daPath.serialize(Detail::Delimiter);
      action->setData(path);

      connect(action, SIGNAL(triggered(bool)), m_AMMenuMapper, SLOT(map()));
      m_AMMenuMapper->setMapping(action, path);
      dcMenu->addAction(action);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_selectedDCBtn_pressed()
{
  // Get the menu and clear it out
  QMenu* menu = selectedDCBtn->menu();
  if(!menu)
  {
    menu = new QMenu();
    selectedDCBtn->setMenu(menu);
    menu->installEventFilter(this);
  }
  if(menu) {
    menu->clear();
  }

  // Get the DataContainerArray object
  // Loop over the data containers until we find the proper data container
  QList<DataContainer::Pointer> containers = m_Dca->getDataContainers();

  QListIterator<DataContainer::Pointer> containerIter(containers);
  while(containerIter.hasNext())
  {
    DataContainer::Pointer dc = containerIter.next();

    QString dcName = dc->getName();
    QAction* action = new QAction(dcName, menu);
    DataArrayPath dcPath(dcName, "", "");
    QString path = dcPath.serialize(Detail::Delimiter);
    action->setData(path);

    connect(action, SIGNAL(triggered(bool)), m_DCMenuMapper, SLOT(map()));
    m_DCMenuMapper->setMapping(action, path);
    menu->addAction(action);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataFormatPage::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == QEvent::Show)
  {
    if (obj == selectedAMBtn->menu())
    {
      QPoint pos = adjustedMenuPosition(selectedAMBtn);
      selectedAMBtn->menu()->move(pos);
      return true;
    }
    else if (obj == selectedDCBtn->menu())
    {
      QPoint pos = adjustedMenuPosition(selectedDCBtn);
      selectedDCBtn->menu()->move(pos);
      return true;
    }
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QPoint DataFormatPage::adjustedMenuPosition(QPushButton* pushButton)
{
  QSize menuSize = pushButton->menu()->sizeHint();
  QPoint point = QCursor::pos();
  point.setX(QCursor::pos().x() - menuSize.width() / 2);

  int desktopWidth = QApplication::desktop()->availableGeometry(pushButton).width();
  int desktopHeight = QApplication::desktop()->availableGeometry(pushButton).height();

  // If the menu is going to go off the screen in the X-axis, reposition it until it's completely on the screen
  while(point.x() + menuSize.width() > desktopWidth)
  {
    point.setX(point.x() - 1);
  }

  QPoint localButtonCoords = pushButton->geometry().bottomLeft();
  QPoint globalButtonCoords = pushButton->mapToGlobal(localButtonCoords);
  //  qDebug() << "QApplication::desktop()->geometry().width(): " << desktopWidth << "," << desktopHeight;
  //  qDebug() << "localButtonCoords: " << localButtonCoords << "\tglobalButtonCoords: " << globalButtonCoords;

  point.setY(globalButtonCoords.y());

  // If the menu is going to go off the screen in the Y-axis, reposition it until it's completely on the screen
  while(point.y() + menuSize.height() > desktopHeight)
  {
    point.setY(point.y() - 1);
  }

  return point;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_startRowSpin_valueChanged(int value)
{
  if(value > m_NumLines)
  {
    wizard()->button(QWizard::FinishButton)->setDisabled(true);
    tupleCountLabel->setText("ERR");
    return;
  }

  wizard()->button(QWizard::FinishButton)->setEnabled(true);

  ASCIIDataModel* model = ASCIIDataModel::Instance();
  model->clear();

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

  // Update headers
  on_hasHeadersRadio_toggled(hasHeadersRadio->isChecked());
  on_doesNotHaveHeadersRadio_toggled(doesNotHaveHeadersRadio->isChecked());
  on_useDefaultHeaders_toggled(useDefaultHeaders->isChecked());

  // Update Tuple Dimensions
  tupleCountLabel->setText(QString::number(m_NumLines - value + 1));
  checkTupleDimensions(getTupleTable()->getData());

  if (isComplete() == true)
  {
    wizard()->button(QWizard::FinishButton)->setEnabled(true);
  }
  else
  {
    wizard()->button(QWizard::FinishButton)->setDisabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataFormatPage::isComplete() const
{
  ASCIIDataModel* model = ASCIIDataModel::Instance();

  if (amSelectionError->isVisible() || amCreationError->isVisible() || tupleTableErrLabel->isVisible())
  {
    return false;
  }

  if (amAutomatically->isChecked() == false)
  {
    if (selectedAMBtn->text().isEmpty())
    {
      return false;
    }
  }
  else if (selectedDCBtn->text().isEmpty() || amName->text().isEmpty())
  {
    return false;
  }

  for (int i=0; i<model->columnCount(); i++)
  {
    if (model->columnHasErrors(i) == true)
    {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_amAutomatically_stateChanged(int state)
{
  if (state == Qt::Checked)
  {
    tupleDimsGB->hide();
    amSelectionFrame->hide();
    dcSelectionFrame->show();
  }
  else
  {
    tupleDimsGB->show();
    amSelectionFrame->show();
    dcSelectionFrame->hide();
  }

  if (isComplete() == true)
  {
    wizard()->button(QWizard::FinishButton)->setEnabled(true);
  }
  else
  {
    wizard()->button(QWizard::FinishButton)->setDisabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_hasHeadersRadio_toggled(bool checked)
{
  if(checked == true)
  {    
    editHeadersBtn->setDisabled(true);
    lineNumberLabel->setEnabled(true);
    headersIndexLineEdit->setEnabled(true);

    // Reload the headers
    on_headersIndexLineEdit_textChanged(headersIndexLineEdit->text());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_doesNotHaveHeadersRadio_toggled(bool checked)
{
  if (checked == true)
  {
    editHeadersBtn->setEnabled(true);
    lineNumberLabel->setDisabled(true);
    headersIndexLineEdit->setDisabled(true);

    ASCIIDataModel* model = ASCIIDataModel::Instance();
    QVector<QString> headers;
    if (m_EditHeadersDialog != nullptr)
    {
      headers = m_EditHeadersDialog->getHeaders();
    }

    for(int i = 0; i < model->columnCount(); i++)
    {
      if (i < headers.size())
      {
        model->setHeaderData(i, Qt::Horizontal, headers[i], Qt::DisplayRole);
      }
      else
      {
        model->setHeaderData(i, Qt::Horizontal, "", Qt::DisplayRole);
      }
    }

    checkHeaders();
    emit completeChanged();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_useDefaultHeaders_toggled(bool checked)
{
  if (checked == true)
  {
    editHeadersBtn->setDisabled(true);
    lineNumberLabel->setDisabled(true);
    headersIndexLineEdit->setDisabled(true);

    ASCIIDataModel* model = ASCIIDataModel::Instance();
    for(int i = 0; i < model->columnCount(); i++)
    {
      QString arrayName = "Array " + QString::number(i+1);
      model->setHeaderData(i, Qt::Horizontal, arrayName, Qt::DisplayRole);
    }

    checkHeaders();
    emit completeChanged();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_headersIndexLineEdit_textChanged(const QString& text)
{


  if(text.isEmpty()) // No test then bail out now.
  {
    return;
  }
  ASCIIDataModel* model = ASCIIDataModel::Instance();
  bool ok = false;
  int lineNum = text.toInt(&ok);

  if(text.isEmpty() == false && (lineNum > m_NumLines || ok == false))
  {
    model->clearHeaders(Qt::Horizontal);
    checkHeaders(QVector<QString>());
    return;
  }

  headersIndexLineEdit->setStyleSheet("");
  lineNumErrLabel->hide();
  wizard()->button(QWizard::FinishButton)->setEnabled(true);

  if(text.isEmpty() == true)
  {
    model->clearHeaders(Qt::Horizontal);
    checkHeaders(QVector<QString>());
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
  for(int i = 0; i < model->columnCount(); i++)
  {
    QString header = "";

    if(i < tokenizedLine.size())
    {
      header = tokenizedLine[i];
    }

    model->setHeaderData(i, Qt::Horizontal, header, Qt::DisplayRole);

    headers.push_back(header);
  }

  checkHeaders(headers);

  if (isComplete() == true)
  {
    wizard()->button(QWizard::FinishButton)->setEnabled(true);
  }
  else
  {
    wizard()->button(QWizard::FinishButton)->setDisabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_editHeadersBtn_clicked()
{
  if(nullptr == m_EditHeadersDialog)
  {
    m_EditHeadersDialog = new EditHeadersDialog(this);
  }

  launchEditHeadersDialog();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::updateSelection(const QItemSelection& selected, const QItemSelection& deselected)
{
  ASCIIDataModel* model = ASCIIDataModel::Instance();
  QModelIndexList selectedIndexList = selected.indexes();

  if(selectedIndexList.size() <= 0)
  {
    columnDataGroupBox->setDisabled(true);
  }
  else
  {
    columnDataGroupBox->setEnabled(true);

    int selectedColumn = selectedIndexList[0].column();
    QString selectedType = model->columnDataType(selectedColumn);

    if(selectedType.isEmpty())
    {
      dataTypeRadio->setChecked(true);
      dataTypeCB->setEnabled(true);
      dataTypeCB->setCurrentIndex(0);
    }
    else if(selectedType == "Skip")
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
bool DataFormatPage::checkTupleDimensions(QVector<size_t> tupleDims)
{
  if (validateTupleDimensions(tupleDims) == false)
  {
    tupleTableErrLabel->setText("The current tuple dimensions do not match the total number of tuples.");
    tupleTableErrLabel->show();
    return false;
  }
  else
  {
    tupleTableErrLabel->setText("");
    tupleTableErrLabel->hide();
    return true;
  }

  emit completeChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataFormatPage::validateTupleDimensions(QVector<size_t> tupleDims)
{
  int tupleTotal = 1;

  for(int i = 0; i < tupleDims.size(); i++)
  {
    tupleTotal = tupleTotal * tupleDims[i];
  }

  int beginIndex = startRowSpin->value();
  int numOfDataLines = m_NumLines - beginIndex + 1;
  if(tupleTotal != numOfDataLines)
  {
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_dataTypeRadio_clicked()
{
  ASCIIDataModel* model = ASCIIDataModel::Instance();
  QModelIndexList indexList = dataView->selectionModel()->selectedColumns();

  if(indexList.size() > 0)
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
void DataFormatPage::on_dataTypeCB_currentTextChanged(const QString& text)
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

  if(indexList.size() > 0)
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
  for(int i = 0; i < model->columnCount(); i++)
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
  if(hasHeadersRadio->isChecked())
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
  if(nullptr != m_EditHeadersDialog)
  {
    ASCIIDataModel* model = ASCIIDataModel::Instance();
    QVector<QString> currentHeaders;
    for(int i = 0; i < model->columnCount(); i++)
    {
      currentHeaders.push_back(model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString());
      model->setHeaderData(i, Qt::Horizontal, QString::number(i + 1), Qt::DisplayRole);
    }
    m_EditHeadersDialog->setHeaders(currentHeaders);

    int result = m_EditHeadersDialog->exec();
    if(result == QDialog::Accepted)
    {
      QVector<QString> headers = m_EditHeadersDialog->getHeaders();
      checkHeaders(headers);
    }
    else
    {
      for(int i = 0; i < model->columnCount(); i++)
      {
        model->setHeaderData(i, Qt::Horizontal, currentHeaders[i], Qt::DisplayRole);
      }

      m_EditHeadersDialog->setHeaders(currentHeaders);
    }

    if (isComplete() == true)
    {
      wizard()->button(QWizard::FinishButton)->setEnabled(true);
    }
    else
    {
      wizard()->button(QWizard::FinishButton)->setDisabled(true);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::checkHeaders()
{
  ASCIIDataModel* model = ASCIIDataModel::Instance();
  QVector<QString> headers;
  for(int i = 0; i < model->columnCount(); i++)
  {
    headers.push_back(model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString());
  }

  checkHeaders(headers);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::checkHeaders(QVector<QString> headers)
{
  if (validateHeaders(headers) == true)
  {
    wizard()->button(QWizard::FinishButton)->setEnabled(true);
  }
  else
  {
    wizard()->button(QWizard::FinishButton)->setDisabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataFormatPage::validateHeaders(QVector<QString> headers)
{
  ASCIIDataModel* model = ASCIIDataModel::Instance();

  bool hasSlashes = false;
  bool hasDuplicates = false;

  bool isEmpty = false;
  if(headers.size() <= 0)
  {
    isEmpty = true;
    for(int i = 0; i < model->columnCount(); i++)
    {
      model->setColumnHasErrors(i, true);
    }
  }
  for(int i = 0; i < headers.size(); i++)
  {
    QString header = headers[i];

    if(header.isEmpty() == true)
    {
      isEmpty = true;
      model->setColumnHasErrors(i, true);
    }
    else if(header.contains('/') || header.contains('\\'))
    {
      hasSlashes = true;
      model->setColumnHasErrors(i, true);
    }
    else
    {
      model->setColumnHasErrors(i, false);
    }

    for(int j = 0; j < headers.size(); j++)
    {
      QString otherHeader = headers[j];

      if(i != j && header.isEmpty() == false && otherHeader.isEmpty() == false && header == otherHeader)
      {
        hasDuplicates = true;
        model->setColumnHasErrors(i, true);
        model->setColumnHasErrors(j, true);
      }
      else if(hasDuplicates == false && isEmpty == false && hasSlashes == false)
      {
        model->setColumnHasErrors(i, false);
        model->setColumnHasErrors(j, false);
      }
    }

    model->setHeaderData(i, Qt::Horizontal, header, Qt::DisplayRole);
  }

  if(isEmpty == true)
  {
    arrayErrLabel->setText("Column headers cannot be empty.");
    arrayErrLabel->show();
    return false;
  }
  else if(hasSlashes == true)
  {
    arrayErrLabel->setText("Column headers cannot contain slashes.");
    arrayErrLabel->show();
    return false;
  }
  else if(hasDuplicates == true)
  {
    arrayErrLabel->setText("Column headers cannot have the same name.");
    arrayErrLabel->show();
    return false;
  }
  else
  {
    arrayErrLabel->setText("");
    arrayErrLabel->hide();
    return true;
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
void DataFormatPage::setAutomaticAM(bool automatic)
{
  amAutomatically->setChecked(automatic);
  //on_amAutomatically_stateChanged(automatic);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataFormatPage::getAutomaticAM()
{
  return amAutomatically->isChecked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath DataFormatPage::getSelectedPath()
{
  if (amAutomatically->isChecked())
  {
    return DataArrayPath::Deserialize(selectedDCBtn->text(), Detail::Delimiter);
  }
  else
  {
    return DataArrayPath::Deserialize(selectedAMBtn->text(), Detail::Delimiter);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::setAutomaticAttrMatrixName(const QString &name)
{
  amName->setText(name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::setAutomaticAttrMatrixName(const DataArrayPath &path)
{
  amName->setText(path.getAttributeMatrixName());
  selectedDCBtn->setText(path.getDataContainerName());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataFormatPage::getAutomaticAttrMatrixName()
{
  return amName->text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataFormatPage::nextId() const
{
  // There is no "Next" page
  return -1;
}
