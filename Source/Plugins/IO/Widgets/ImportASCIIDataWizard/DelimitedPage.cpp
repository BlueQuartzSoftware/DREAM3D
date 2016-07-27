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

#include "DelimitedPage.h"

#include "ImportASCIIDataWizard.h"
#include "ASCIIDataModel.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DelimitedPage::DelimitedPage(const QString &inputFilePath, int numLines, QWidget* parent) :
  AbstractWizardPage(inputFilePath, parent),
  m_NumLines(numLines)
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DelimitedPage::~DelimitedPage()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DelimitedPage::setupGui()
{
  ASCIIDataModel* model = ASCIIDataModel::Instance();

  refreshModel();

  dataView->setModel(model);
  //dataView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  registerField("tabAsDelimiter", tabCheckBox);
  registerField("semicolonAsDelimiter", semicolonCheckBox);
  registerField("commaAsDelimiter", commaCheckBox);
  registerField("spaceAsDelimiter", spaceCheckBox);
  registerField("consecutiveDelimiters", consecutiveDCheckBox);

  connect(tabCheckBox, SIGNAL(stateChanged(int)), this, SLOT(checkBox_Toggled(int)));
  connect(semicolonCheckBox, SIGNAL(stateChanged(int)), this, SLOT(checkBox_Toggled(int)));
  connect(commaCheckBox, SIGNAL(stateChanged(int)), this, SLOT(checkBox_Toggled(int)));
  connect(spaceCheckBox, SIGNAL(stateChanged(int)), this, SLOT(checkBox_Toggled(int)));
  connect(consecutiveDCheckBox, SIGNAL(stateChanged(int)), this, SLOT(checkBox_Toggled(int)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DelimitedPage::checkBox_Toggled(int state)
{
  bool tabAsDelimiter = tabCheckBox->isChecked();
  bool semicolonAsDelimiter = semicolonCheckBox->isChecked();
  bool commaAsDelimiter = commaCheckBox->isChecked();
  bool spaceAsDelimiter = spaceCheckBox->isChecked();
  bool consecutiveDelimiters = consecutiveDCheckBox->isChecked();

  ASCIIDataModel* model = ASCIIDataModel::Instance();
  QStringList lines = model->originalStrings();

  model->clear();

  ImportASCIIDataWizard::LoadOriginalLines(lines);

  QList<char> delimiters = ImportASCIIDataWizard::ConvertToDelimiters(tabAsDelimiter, semicolonAsDelimiter, commaAsDelimiter, spaceAsDelimiter);

  QList<QStringList> tokenizedLines = ImportASCIIDataWizard::TokenizeLines(lines, delimiters, consecutiveDelimiters);
  ImportASCIIDataWizard::InsertTokenizedLines(tokenizedLines, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DelimitedPage::refreshModel()
{
  ASCIIDataModel* model = ASCIIDataModel::Instance();
  model->clear();

  bool tabAsDelimiter = tabCheckBox->isChecked();
  bool semicolonAsDelimiter = semicolonCheckBox->isChecked();
  bool commaAsDelimiter = commaCheckBox->isChecked();
  bool spaceAsDelimiter = spaceCheckBox->isChecked();
  bool consecutiveDelimiters = consecutiveDCheckBox->isChecked();

  QStringList lines = ImportASCIIDataWizard::ReadLines(m_InputFilePath, 1, ImportASCIIDataWizard::TotalPreviewLines);

  ImportASCIIDataWizard::LoadOriginalLines(lines);

  QList<char> delimiters = ImportASCIIDataWizard::ConvertToDelimiters(tabAsDelimiter, semicolonAsDelimiter, commaAsDelimiter, spaceAsDelimiter);

  QList<QStringList> tokenizedLines = ImportASCIIDataWizard::TokenizeLines(lines, delimiters, consecutiveDelimiters);
  ImportASCIIDataWizard::InsertTokenizedLines(tokenizedLines, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DelimitedPage::showEvent(QShowEvent* event)
{
  ASCIIDataModel* model = ASCIIDataModel::Instance();
  model->clearContents();

  if (model->columnCount() > 0)
  {
    model->removeColumns(0, model->columnCount());
  }

  // This is the first screen, so everything automatically goes into one column for now
  model->insertColumn(0);

  for (int row = 0; row < model->rowCount(); row++)
  {
    QString line = model->originalString(row);

    QModelIndex index = model->index(row, 0);

    model->setData(index, line, Qt::DisplayRole);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DelimitedPage::nextId() const
{
  return ImportASCIIDataWizard::DataFormat;
}
