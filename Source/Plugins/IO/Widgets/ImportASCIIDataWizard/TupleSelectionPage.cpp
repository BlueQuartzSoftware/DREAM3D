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

#include "TupleSelectionPage.h"

#include <QtCore/QFile>

#include "SIMPLib/Common/Constants.h"

#include "ImportASCIIDataWizard.h"
#include "ASCIIDataModel.h"
#include "AddHeadersDialog.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TupleSelectionPage::TupleSelectionPage(const QString &inputFilePath, QWidget* parent) :
  AbstractWizardPage(inputFilePath, parent)
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TupleSelectionPage::~TupleSelectionPage()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TupleSelectionPage::setupGui()
{
  ASCIIDataModel* model = ASCIIDataModel::Instance();

  dataView->setModel(model);
  dataView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TupleSelectionPage::refreshModel()
{
  ASCIIDataModel* model = ASCIIDataModel::Instance();
  model->clear();

  bool isFixedWidth = field("isFixedWidth").toBool();
  bool tabAsDelimiter = field("tabAsDelimiter").toBool();
  bool semicolonAsDelimiter = field("semicolonAsDelimiter").toBool();
  bool commaAsDelimiter = field("commaAsDelimiter").toBool();
  bool spaceAsDelimiter = field("spaceAsDelimiter").toBool();
  bool consecutiveDelimiters = field("consecutiveDelimiters").toBool();
  int beginIndex = field("startRow").toInt();

  QStringList lines = ImportASCIIDataWizard::ReadLines(m_InputFilePath, beginIndex, ImportASCIIDataWizard::TotalPreviewLines);

  ImportASCIIDataWizard::LoadOriginalLines(lines);

  QList<char> delimiters = ImportASCIIDataWizard::ConvertToDelimiters(tabAsDelimiter, semicolonAsDelimiter, commaAsDelimiter, spaceAsDelimiter);

  QList<QStringList> tokenizedLines = ImportASCIIDataWizard::TokenizeLines(lines, delimiters, isFixedWidth, consecutiveDelimiters);
  ImportASCIIDataWizard::InsertTokenizedLines(tokenizedLines, beginIndex);

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
int TupleSelectionPage::nextId() const
{
  // There is no "Next" page
  return -1;
}
