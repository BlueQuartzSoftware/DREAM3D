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

#include "DREAM3DWidgetsLib/FilterParameterWidgets/FilterParameterWidgetsDialogs.h"

#include "SIMPLib/Common/AbstractFilter.h"

#include "QtSupportLib/QFSDropLabel.h"

#include "FilterParameters/ImportASCIIDataFilterParameter.h"

#include "Widgets/ImportASCIIDataWizard/ImportASCIIDataWizard.h"

// Initialize private static member variable
QString ImportASCIIDataWidget::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportASCIIDataWidget::ImportASCIIDataWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent)
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
  filterList.push_back("Text Files (*.txt)");
  filterList.push_back("All Files(*.*)");
  QFileDialog* dialog = new QFileDialog(this);
  dialog->setFileMode(QFileDialog::ExistingFile);
  dialog->setNameFilters(filterList);
  dialog->setWindowTitle("Select File To Import");
  dialog->setLabelText(QFileDialog::Accept, "Import");

  if (dialog->exec())
  {
    QString filePath = dialog->selectedFiles()[0];

    if (true == filePath.isEmpty())
    {
      return;
    }
    filePath = QDir::toNativeSeparators(filePath);
    // Store the last used directory into the private instance variable
    QFileInfo fi(filePath);
    m_OpenDialogLastDirectory = fi.path();

    ImportASCIIDataWizard* wizard = new ImportASCIIDataWizard(this);
    int result = wizard->exec();

    if (result == QDialog::Accepted)
    {


      fileImportedLabel->setText(filePath);
      fileImportedLabel->show();
      //  filterNeedsInputParameters(getFilter());
      emit parametersChanged(); // This should force the preflight to run because we are emitting a signal
    }
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
