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

#include "ImportASCIIDataWidget.h"

#include <QtCore/QFileInfo>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include "DREAM3DWidgetsLib/FilterParameterWidgets/FilterParameterWidgetsDialogs.h"

#include "SIMPLib/Common/AbstractFilter.h"

#include "QtSupportLib/QFSDropLabel.h"

#include "FilterParameters/ImportASCIIDataFilterParameter.h"

#include "Widgets/ImportASCIIDataWizard/ImportASCIIDataWizard.h"
#include "Widgets/ImportASCIIDataWizard/AbstractDataParser.hpp"

// Initialize private static member variable
QString ImportASCIIDataWidget::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportASCIIDataWidget::ImportASCIIDataWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent),
  m_NumLines(0)
{
  m_FilterParameter = dynamic_cast<ImportASCIIDataFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != NULL, "NULL Pointer", "ImportASCIIDataWidget can ONLY be used with an ImportASCIIDataFilterParameter object");

  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportASCIIDataWidget::~ImportASCIIDataWidget()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportASCIIDataWidget::setupGui()
{
  fileImportedLabel->hide();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportASCIIDataWidget::on_importFileBtn_pressed()
{
  QStringList filterList;
  filterList.push_back("All Files(*.*)");
  QFileDialog* dialog = new QFileDialog(this);
  dialog->setFileMode(QFileDialog::ExistingFile);
  dialog->setNameFilters(filterList);
  dialog->setWindowTitle("Select File To Import");
  dialog->setLabelText(QFileDialog::Accept, "Import");

  if (dialog->exec())
  {
    QString filePath = dialog->selectedFiles()[0];
    QFileInfo fi(filePath);

    if (filePath.isEmpty() == true)
    {
      return;
    }

    filePath = QDir::toNativeSeparators(filePath);
    // Store the last used directory into the private instance variable
    m_OpenDialogLastDirectory = fi.path();

    // Validate that the file is an ASCII file
    {
      FILE* file;
      long fileSize;
      long bufferSize = 2048;
      char* buffer;
      size_t result;

      // Open the file
      file = fopen(filePath.toStdString().c_str(), "rb");
      if (file == NULL)
      {
        QString errorStr = "Error: Unable to open file \"" + filePath + "\"";
        fputs(errorStr.toStdString().c_str(), stderr);
        return;
      }

      // Obtain the file size
      fseek(file, 0, SEEK_END);
      fileSize = ftell(file);
      rewind(file);

      long actualSize;
      if (fileSize <= bufferSize)
      {
        actualSize = fileSize;
      }
      else
      {
        actualSize = bufferSize;
      }

      // Allocate the buffer
      buffer = (char*)malloc(sizeof(char)*actualSize);
      if (buffer == NULL)
      {
        QString errorStr = "Error: Unable to allocate memory to read in data from \"" + filePath + "\"";
        fputs(errorStr.toStdString().c_str(), stderr);
        return;
      }

      // Copy the file contents into the buffer
      result = fread(buffer, 1, actualSize, file);
      if (result != actualSize)
      {
        QString errorStr = "Error: There was an error reading the data from \"" + filePath + "\"";
        fputs(errorStr.toStdString().c_str(), stderr);
        return;
      }

      // Check the buffer for invalid characters, tab characters, new-line characters, and carriage return characters
      bool hasNewLines = false;
      bool hasCarriageReturns = false;
      bool hasTabs = false;
      for (int i = 0; i < actualSize; i++)
      {
        char currentChar = buffer[i];

        if (currentChar < 32 && currentChar != 9 && currentChar != 10 && currentChar != 13)
        {
          // This is an unprintable character, so throw up an error dialog and STOP
          QString errorStr = "Unprintable characters have been detected in the file \"" + filePath + "\"\n\nPlease import a different file.";
          QMessageBox::critical(this, tr("ASCII Data Import Error"), tr(errorStr.toStdString().c_str()), QMessageBox::Ok, QMessageBox::Ok);
          return;
        }
        else if (currentChar == 9)
        {
          hasTabs = true;
        }
        else if (currentChar == 10)
        {
          hasNewLines = true;
        }
        else if (currentChar == 13)
        {
          hasCarriageReturns = true;
        }
      }

      if (hasNewLines == false && hasCarriageReturns == false && hasTabs == false)
      {
        // This might be a binary file, so throw up a warning dialog
        QString warningStr = "The file \"" + filePath + "\" might be a binary file, because line-feed, tab, or carriage return characters have not been detected.\nWarning: Using this file may crash the program or cause unexpected results.\n\nWould you still like to continue importing the file?";
        int ret = QMessageBox::warning(this, tr("ASCII Data Import Warning"), tr(warningStr.toStdString().c_str()), QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);

        if (ret == QMessageBox::No)
        {
          return;
        }
      }

      // Close the file and free the memory from the buffer
      fclose(file);
      free(buffer);
    }

    // Run through the file, to get the total number of lines
    QFile inputFile(filePath);
    if (inputFile.open(QIODevice::ReadOnly))
    {
      QTextStream in(&inputFile);

      while (in.atEnd() == false)
      {
        in.readLine();
        m_NumLines++;
      }
      inputFile.close();
    }

    ImportASCIIDataWizard* wizard = new ImportASCIIDataWizard(filePath, this);
    int result = wizard->exec();

    if (result == QDialog::Accepted)
    {
      fileImportedLabel->setText(filePath);
      fileImportedLabel->show();
      emit parametersChanged(); // This should force the preflight to run because we are emitting a signal
    }

    // Create the arrays
    QStringList headers = wizard->getHeaders();
    int beginLineNum = wizard->getBeginningLineNum();
    QStringList dataTypes = wizard->getDataTypes();

    QList<IO::AbstractDataParser::Pointer> dataParsers;
    for (int i = 0; i < dataTypes.size(); i++)
    {
      QString dataType = dataTypes[i];
      QString name = headers[i];

      if (dataType == "Double")
      {

      }
      else if (dataType == "Integer")
      {
        int index = 0;
        Int32ArrayType::Pointer data = Int32ArrayType::CreateArray(0, name, false);
        IO::Int32Parser::Pointer dparser = IO::Int32Parser::New(data, name, index);
        dataParsers.push_back(dparser);
      }
      else if (dataType == "String")
      {

      }
      else if (dataType == "Skip")
      {
        dataParsers.push_back(IO::AbstractDataParser::NullPointer());
      }
    }

    QStringList lines = ImportASCIIDataWizard::ReadLines(filePath, beginLineNum, m_NumLines - beginLineNum);
    QList<QStringList> tokenizedLines = ImportASCIIDataWizard::TokenizeLines(lines, wizard->getHasFixedWidth(), wizard->getTabAsDelimiter(), wizard->getSemicolonAsDelimiter(), wizard->getCommaAsDelimiter(), wizard->getSpaceAsDelimiter(), wizard->getConsecutiveDelimiters());

    for (int row = 0; row < tokenizedLines.size(); row++)
    {
      QStringList tokens = tokenizedLines[row];
      if (dataTypes.size() != tokens.size())
      {
        // Throw an error
      }

      for (int column = 0; column < tokens.size(); column++)
      {
        QString dataType = dataTypes[column];
        QString token = tokens[column];

        if (dataParsers[column] == IO::AbstractDataParser::NullPointer())
        {
          continue;
        }
      }
    }

    delete wizard;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ImportASCIIDataWidget::verifyPathExists(QString filePath, QFSDropLabel* lineEdit)
{
  QFileInfo fileinfo(filePath);
  if (false == fileinfo.exists())
  {
    //lineEdit->setStyleSheet("border: 1px solid red;");
    lineEdit->changeStyleSheet(QFSDropLabel::FS_DOESNOTEXIST_STYLE);
  }
  else
  {
    lineEdit->changeStyleSheet(QFSDropLabel::FS_STANDARD_STYLE);
  }
  return fileinfo.exists();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportASCIIDataWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  //QString text = value->text();
  //if (verifyPathExists(text, value) == true) {}

  //bool ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, text);
  //if (false == ok)
  //{
  //  FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(getFilter(), getFilterParameter());
  //}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportASCIIDataWidget::beforePreflight()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportASCIIDataWidget::afterPreflight()
{

}
