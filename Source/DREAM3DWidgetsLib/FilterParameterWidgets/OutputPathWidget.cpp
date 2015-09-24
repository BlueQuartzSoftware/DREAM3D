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

#include "OutputPathWidget.h"

#include <QtCore/QMetaProperty>
#include <QtCore/QDir>

#include <QtWidgets/QFileDialog>

#include "SIMPLib/FilterParameters/OutputPathFilterParameter.h"

#include "QtSupportLib/QFileCompleter.h"
#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"

// Initialize private static member variable
QString OutputPathWidget::m_OpenDialogLastDirectory = "";
// Include the MOC generated file for this class
#include "moc_OutputPathWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OutputPathWidget::OutputPathWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent)
{
  m_FilterParameter = dynamic_cast<OutputPathFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != NULL, "NULL Pointer", "OutputPathWidget can ONLY be used with a OutputPathFilterParameter object");

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
OutputPathWidget::~OutputPathWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OutputPathWidget::setFilterParameter(FilterParameter* value)
{
  m_FilterParameter = dynamic_cast<OutputPathFilterParameter*>(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter* OutputPathWidget::getFilterParameter() const
{
  return m_FilterParameter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OutputPathWidget::setupGui()
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

  //  connect(value, SIGNAL(textChanged(const QString&)),
  //          this, SLOT(widgetChanged(const QString&)));

  QFileCompleter* com = new QFileCompleter(this, false);
  value->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString&)),
                    this, SLOT(on_value_textChanged(const QString&)));

  if (getFilterParameter() != NULL)
  {
    label->setText(getFilterParameter()->getHumanLabel() );

    QString currentPath = getFilter()->property(PROPERTY_NAME_AS_CHAR).toString();
    value->setText(currentPath);
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OutputPathWidget::on_selectBtn_clicked()
{
  QString currentPath = getFilter()->property(PROPERTY_NAME_AS_CHAR).toString();
  if(currentPath.isEmpty() == true)
  {
    currentPath = m_OpenDialogLastDirectory;
  }
  QString Ftype = m_FilterParameter->getFileType();
  QString ext = m_FilterParameter->getFileExtension();
  QString s = Ftype + QString(" Files (") + ext + QString(");;All Files(*.*)");
  QString defaultName = currentPath + QDir::separator() + "Untitled";
  QString file = QFileDialog::getExistingDirectory(this,
                                                   tr("Select Output Folder"),
                                                   defaultName,
                                                   QFileDialog::ShowDirsOnly);

  if(true == file.isEmpty())
  {
    return;
  }
  //  bool ok = false;
  file = QDir::toNativeSeparators(file);
  // Store the last used directory into the private instance variable
  QFileInfo fi(file);
  m_OpenDialogLastDirectory = fi.path();

  value->setText(file);
  on_value_editingFinished();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//void OutputPathWidget::widgetChanged(const QString &text)
//{
//  emit parametersChanged();
//}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OutputPathWidget::on_value_editingFinished()
{
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OutputPathWidget::on_value_textChanged(const QString& text)
{
  // We dont want to run a preflight for every character that is typed
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OutputPathWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  QString text = value->text();
  bool ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, text);
  if(false == ok)
  {
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(getFilter(), getFilterParameter());
  }

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OutputPathWidget::beforePreflight()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OutputPathWidget::afterPreflight()
{

}
