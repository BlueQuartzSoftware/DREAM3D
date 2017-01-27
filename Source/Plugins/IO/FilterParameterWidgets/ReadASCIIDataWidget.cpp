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

#include "ReadASCIIDataWidget.h"

#include <QtCore/QFileInfo>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidgetsDialogs.h"

#include "SIMPLib/Common/AbstractFilter.h"

#include "FilterParameters/ReadASCIIDataFilterParameter.h"

#include "IO/IOFilters/util/ASCIIWizardData.hpp"
#include "IO/IOFilters/ReadASCIIData.h"

#include "Widgets/ImportASCIIDataWizard/AbstractDataParser.hpp"
#include "Widgets/ImportASCIIDataWizard/DataFormatPage.h"
#include "Widgets/ImportASCIIDataWizard/ImportASCIIDataWizard.h"
#include "Widgets/ImportASCIIDataWizard/LineCounterObject.h"



// Initialize private static member variable
QString ReadASCIIDataWidget::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadASCIIDataWidget::ReadASCIIDataWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
, m_ImportWizard(nullptr)
, m_WorkerThread(nullptr)
, m_LineCounter(nullptr)
{
  m_FilterParameter = dynamic_cast<ReadASCIIDataFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "nullptr Pointer", "ReadASCIIDataWidget can ONLY be used with an ReadASCIIDataFilterParameter object");

  m_Filter = dynamic_cast<ReadASCIIData*>(filter);
  Q_ASSERT_X(m_Filter != nullptr, "nullptr Pointer", "ReadASCIIDataWidget can ONLY be used with an ReadASCIIData filter");

  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadASCIIDataWidget::~ReadASCIIDataWidget()
{
    if(nullptr != m_ImportWizard)
      m_ImportWizard->deleteLater();

    if(nullptr != m_LineCounter)
       m_LineCounter->deleteLater();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadASCIIDataWidget::setupGui()
{
  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()), this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()), this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)), this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  // If the filter was loaded from a pipeline file, fill in the information in the widget
  if(m_Filter->getWizardData().isEmpty() == false)
  {

    ASCIIWizardData wizardData = m_Filter->getWizardData();
    m_ImportWizard = new ImportASCIIDataWizard(&wizardData, m_Filter->getDataContainerArray(), this);

    int beginIndex = m_Filter->getWizardData().beginIndex;
    int numOfDataLines = m_Filter->getWizardData().numberOfLines - beginIndex + 1;

    m_LineCounter = new LineCounterObject(m_FilePath, m_Filter->getWizardData().numberOfLines);

    QVector<size_t> tupleDimsArray = m_Filter->getWizardData().tupleDims;

    if(tupleDimsArray.size() > 0)
    {
      QString tupleDimsStr = "";
      for(int i = 0; i < tupleDimsArray.size(); i++)
      {
        tupleDimsStr.append("[" + QString::number(tupleDimsArray[i]) + "]");
      }
      tupleDims->setText(tupleDimsStr);
    }

    tupleCount->setText(QString::number(numOfDataLines));
    fileImportedLabel->setText(m_Filter->getWizardData().inputFilePath);
  }
  else
  {
    fileImportedLabel->hide();
    warningLabel->hide();
    resetWizardBtn->hide();
    tupleCountLabel->hide();
    tupleCount->hide();
    tupleDimsLabel->hide();
    tupleDims->hide();
  }

  loadingProgress->hide();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadASCIIDataWidget::on_editImportSettings_clicked()
{
  if(m_ImportWizard != nullptr)
  {
    int numOfLines = -1;
    if(m_LineCounter)
    {
      numOfLines = m_LineCounter->getNumberOfLines();
    }
    m_ImportWizard->setEditSettings(true);

    int result = m_ImportWizard->exec();

    if(result == QDialog::Accepted)
    {
      int beginIndex = m_ImportWizard->getBeginningLineNum();
      int numOfDataLines = numOfLines - beginIndex + 1;
      tupleCount->setText(QString::number(numOfDataLines));
      QVector<size_t> dims = m_ImportWizard->getTupleDims();

      QString tupleDimsStr = "";
      for(int i = 0; i < dims.size(); i++)
      {
        tupleDimsStr.append("[" + QString::number(dims[i]) + "]");
      }

      tupleDims->setText(tupleDimsStr);
      m_FilePath = m_ImportWizard->getInputFilePath();
      fileImportedLabel->setText(m_FilePath);
      fileImportedLabel->show();
      warningLabel->show();
      resetWizardBtn->show();
      tupleCountLabel->show();
      tupleCount->show();
      tupleDimsLabel->show();
      tupleDims->show();

      emit parametersChanged(); // This should force the preflight to run because we are emitting a signal
    }
    m_ImportWizard->setEditSettings(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadASCIIDataWidget::on_importFileBtn_pressed()
{
  QStringList filterList;
  filterList.push_back("All Files(*.*)");
  QFileDialog* dialog = new QFileDialog(this);
  dialog->setFileMode(QFileDialog::ExistingFile);
  dialog->setNameFilters(filterList);
  dialog->setWindowTitle("Select File To Import");
  dialog->setLabelText(QFileDialog::Accept, "Import");

  if(dialog->exec())
  {
    // Clean up previous wizard and settings
    if(nullptr != m_ImportWizard)
    {
      delete m_ImportWizard;
      m_ImportWizard = nullptr;
    }

    m_FilePath = dialog->selectedFiles()[0];
    QFileInfo fi(m_FilePath);

    if(m_FilePath.isEmpty() == true)
    {
      return;
    }

    m_FilePath = QDir::toNativeSeparators(m_FilePath);
    // Store the last used directory into the private instance variable
    m_OpenDialogLastDirectory = fi.path();

    // Validate that the file is an ASCII file
    {
      int64_t bufferSize = 2048;
      char* buffer;
      size_t result;

      // Obtain the file size
      QFile qFile(m_FilePath);
      int64_t fileSize = qFile.size();

      // Open the file
      if(qFile.open(QIODevice::ReadOnly) == false)
      {
        QString errorStr = "Error: Unable to open file \"" + m_FilePath + "\"";
        fputs(errorStr.toStdString().c_str(), stderr);
        return;
      }

      int64_t actualSize;
      if(fileSize <= bufferSize)
      {
        actualSize = fileSize;
      }
      else
      {
        actualSize = bufferSize;
      }

      // Allocate the buffer
      std::vector<char> bufferVec(actualSize);
      buffer = bufferVec.data();
      if(buffer == nullptr)
      {
        QString errorStr = "Error: Unable to allocate memory to read in data from \"" + m_FilePath + "\"";
        fputs(errorStr.toStdString().c_str(), stderr);
        return;
      }

      // Copy the file contents into the buffer
      result = qFile.read(buffer, actualSize);
      if(result != actualSize)
      {
        QString errorStr = "Error: There was an error reading the data from \"" + m_FilePath + "\"";
        fputs(errorStr.toStdString().c_str(), stderr);
        return;
      }

      // Check the buffer for invalid characters, tab characters, new-line characters, and carriage return characters
      bool hasNewLines = false;
      bool hasCarriageReturns = false;
      bool hasTabs = false;
      for(int i = 0; i < actualSize; i++)
      {
        char currentChar = buffer[i];

        if(currentChar < 32 && currentChar != 9 && currentChar != 10 && currentChar != 13)
        {
          // This is an unprintable character, so throw up an error dialog and STOP
          QString errorStr = "Unprintable characters have been detected in the file \"" + m_FilePath + "\"\n\nPlease import a different file.";
          QMessageBox::critical(this, tr("ASCII Data Import Error"), tr(errorStr.toStdString().c_str()), QMessageBox::Ok, QMessageBox::Ok);
          return;
        }
        else if(currentChar == 9)
        {
          hasTabs = true;
        }
        else if(currentChar == 10)
        {
          hasNewLines = true;
        }
        else if(currentChar == 13)
        {
          hasCarriageReturns = true;
        }
      }

      if(hasNewLines == false && hasCarriageReturns == false && hasTabs == false)
      {
        // This might be a binary file, so throw up a warning dialog
        QString warningStr = "The file \"" + m_FilePath + "\" might be a binary file, because line-feed, tab, or carriage return characters have not been detected.\nWarning: Using this file may "
                                                          "crash the program or cause unexpected results.\n\nWould you still like to continue importing the file?";
        int ret = QMessageBox::warning(this, tr("ASCII Data Import Warning"), tr(warningStr.toStdString().c_str()), QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);

        if(ret == QMessageBox::No)
        {
          return;
        }
      }

      // Close the file and free the memory from the buffer
      qFile.close();
    }

    if(m_WorkerThread != nullptr)
    {
      m_WorkerThread->wait(); // Wait until the thread is complete
      if(m_WorkerThread->isFinished() == true)
      {
        delete m_WorkerThread;
        m_WorkerThread = nullptr;
      }
    }
    m_WorkerThread = new QThread(); // Create a new Thread Resource

    if(nullptr != m_LineCounter)
    {
      delete m_LineCounter;
      m_LineCounter = nullptr;
    }
    m_LineCounter = new LineCounterObject(m_FilePath);

    // When the thread starts its event loop, start the LineCounter going
    connect(m_WorkerThread, SIGNAL(started()), m_LineCounter, SLOT(run()));

    // When the LineCounter ends then tell the QThread to stop its event loop
    connect(m_LineCounter, SIGNAL(finished()), m_WorkerThread, SLOT(quit()));

    // When the QThread finishes, tell this object that it has finished.
    connect(m_WorkerThread, SIGNAL(finished()), this, SLOT(lineCountDidFinish()));

    // Connection to send progress updates
    connect(m_LineCounter, SIGNAL(progressUpdateGenerated(double)), this, SLOT(updateProgress(double)));

    // Move the QFile object into the thread that we just created.
    m_LineCounter->moveToThread(m_WorkerThread);
    loadingProgress->show();
    m_WorkerThread->start();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadASCIIDataWidget::updateProgress(double percentage)
{
  int64_t progressInt = static_cast<int64_t>(percentage);
  loadingProgress->setText(QString::number(progressInt) + "%");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadASCIIDataWidget::lineCountDidFinish()
{
  loadingProgress->setText("0%");
  loadingProgress->hide();

  int numOfLines = m_LineCounter->getNumberOfLines();

  m_ImportWizard = new ImportASCIIDataWizard(m_FilePath, numOfLines, m_Filter->getDataContainerArray(), this);
  int result = m_ImportWizard->exec();

  if(result == QDialog::Accepted)
  {
    int beginIndex = m_ImportWizard->getBeginningLineNum();
    int numOfDataLines = numOfLines - beginIndex + 1;
    tupleCount->setText(QString::number(numOfDataLines));
    QVector<size_t> dims = m_ImportWizard->getTupleDims();

    QString tupleDimsStr = "";
    for(int i = 0; i < dims.size(); i++)
    {
      tupleDimsStr.append("[" + QString::number(dims[i]) + "]");
    }

    tupleDims->setText(tupleDimsStr);

    fileImportedLabel->setText(m_FilePath);
    fileImportedLabel->show();
    warningLabel->show();
    resetWizardBtn->show();
    tupleCountLabel->show();
    tupleCount->show();
    tupleDimsLabel->show();
    tupleDims->show();
    editImportSettings->show();

    emit parametersChanged(); // This should force the preflight to run because we are emitting a signal
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadASCIIDataWidget::on_resetWizardBtn_pressed()
{
  delete m_ImportWizard;
  m_ImportWizard = nullptr;

  fileImportedLabel->hide();
  warningLabel->hide();
  resetWizardBtn->hide();
  tupleCount->hide();
  tupleCountLabel->hide();
  tupleDims->hide();
  tupleDimsLabel->hide();
  editImportSettings->hide();

  emit parametersChanged(); // This should force the preflight to run because we are emitting a signal
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadASCIIDataWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  ASCIIWizardData data;
  int numOfLines = -1;

  if(nullptr != m_ImportWizard)
  {
    numOfLines = m_LineCounter->getNumberOfLines();

    data.consecutiveDelimiters = m_ImportWizard->getConsecutiveDelimiters();
    data.delimiters = m_ImportWizard->getDelimiters();
    data.inputFilePath = m_ImportWizard->getInputFilePath();
    data.dataHeaders = m_ImportWizard->getHeaders();
    data.headerIsCustom = data.dataHeaders.size() > 0 ? true : false;
    data.headerLine = data.headerIsCustom ? -1 : m_ImportWizard->getBeginningLineNum() - 1;
//        data.headerUsesDefaults =
    data.numberOfLines = numOfLines;
    data.beginIndex = m_ImportWizard->getBeginningLineNum();
    data.dataTypes = m_ImportWizard->getDataTypes();
    data.tupleDims = m_ImportWizard->getTupleDims();
    data.automaticAM = m_ImportWizard->getAutomaticAM();
    data.selectedPath = m_ImportWizard->getSelectedPath();
    data.attrMatType = m_ImportWizard->getAttributeMatrixType();
  }

  // Now set the value into the filter.
  QVariant v;
  v.setValue(data);
  bool ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, v);
  if(false == ok)
  {
    // FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(getFilter(), getFilterParameter());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadASCIIDataWidget::beforePreflight()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadASCIIDataWidget::afterPreflight()
{
}
