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

#include "InputFileWidget.h"

#include <QtCore/QMetaProperty>
#include <QtCore/QDir>

#include <QtWidgets/QLabel>
#include <QtWidgets/QFileDialog>

#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"

#include "QtSupportLib/QFileCompleter.h"

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"

// Initialize private static member variable
QString InputFileWidget::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InputFileWidget::InputFileWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent)
{
  m_FilterParameter = dynamic_cast<InputFileFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != NULL, "NULL Pointer", "InputFileWidget can ONLY be used with a InputFileFilterParameter object");

  setupUi(this);
  setupGui();
  if(filter)
  {
    QString currentPath = filter->property(PROPERTY_NAME_AS_CHAR).toString();
    if(currentPath.isEmpty() == false)
    {
      currentPath = QDir::toNativeSeparators(currentPath);
      // Store the last used directory into the private instance variable
      QFileInfo fi(currentPath);
      m_OpenDialogLastDirectory = fi.path();
    }
    else
    {
      m_OpenDialogLastDirectory = QDir::homePath();
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InputFileWidget::~InputFileWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InputFileWidget::setFilterParameter(FilterParameter* value)
{
  m_FilterParameter = dynamic_cast<InputFileFilterParameter*>(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter* InputFileWidget::getFilterParameter() const
{
  return m_FilterParameter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InputFileWidget::setupGui()
{

  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()),
          this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()),
          this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)),
          this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  // Put a file path completer to help out the user to select a valid file
  //  QFileCompleter* com = new QFileCompleter(this, false);
  //  value->setCompleter(com);
  //  QObject::connect( com, SIGNAL(activated(const QString&)),
  //                    this, SLOT(on_value_textChanged(const QString&)));

  QFont inputFileFont;
  inputFileFont.setBold(true);
  inputFileFont.setItalic(true);
  inputFileFont.setWeight(75);
  inputFileFont.setStyleStrategy(QFont::PreferAntialias);
  // inputFileFont.setFamily(QString::fromUtf8("Times New Roman"));
#if defined (Q_OS_MAC)
  inputFileFont.setPointSize(12);
#elif defined (Q_OS_WIN)
  inputFileFont.setPointSize(9);
#else
  inputFileFont.setPointSize(10);
#endif

  value->setFont(inputFileFont);

  // See if we can get the default value from the filter instance
  if (getFilterParameter() != NULL)
  {
    selectBtn->setText(getFilterParameter()->getHumanLabel()  + " ...");

    QString currentPath = getFilter()->property(PROPERTY_NAME_AS_CHAR).toString();
    value->setText(currentPath);
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool InputFileWidget::verifyPathExists(QString filePath, QFSDropLabel* lineEdit)
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

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InputFileWidget::on_value_editingFinished()
{
  filterNeedsInputParameters(getFilter());
  emit parametersChanged(); // This should force the preflight to run because we are emitting a signal
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InputFileWidget::on_value_textChanged(const QString& text)
{
  setOpenDialogLastDirectory(text);
  // Set/Remove the red outline if the file does exist
  if (verifyPathExists(text, value) == true)
  {
    emit parametersChanged(); // This should force the preflight to run because we are emitting a signal
  }
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InputFileWidget::on_value_fileDropped(const QString& text)
{
  setOpenDialogLastDirectory(text);
  // Set/Remove the red outline if the file does exist
  if (verifyPathExists(text, value) == true)
  {
    emit parametersChanged(); // This should force the preflight to run because we are emitting a signal
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InputFileWidget::on_selectBtn_clicked()
{

  //QString currentPath = getFilter()->property(PROPERTY_NAME_AS_CHAR).toString();
  QString Ftype = m_FilterParameter->getFileType();
  QString ext = m_FilterParameter->getFileExtension();
  QString s = Ftype + QString(" Files (") + ext + QString(");;All Files(*.*)");
  //QString defaultName = m_OpenDialogLastDirectory + QDir::separator() + "Untitled";
  QString file = QFileDialog::getOpenFileName(this, tr("Select Input File"), m_OpenDialogLastDirectory, s);

  if(true == file.isEmpty())
  {
    return;
  }
  file = QDir::toNativeSeparators(file);
  // Store the last used directory into the private instance variable
  QFileInfo fi(file);
  m_OpenDialogLastDirectory = fi.path();
  value->setText(file);
  //  filterNeedsInputParameters(getFilter());
  emit parametersChanged(); // This should force the preflight to run because we are emitting a signal
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InputFileWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  QString text = value->text();
  if (verifyPathExists(text, value) == true ) {}

  bool ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, text);
  if(false == ok)
  {
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(getFilter(), getFilterParameter());
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InputFileWidget::beforePreflight()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InputFileWidget::afterPreflight()
{

}
