/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "OutputFileWidget.h"

#include <QtCore/QMetaProperty>
#include <QtCore/QDir>

#include <QtGui/QFileDialog>

#include "QtSupport/QFileCompleter.h"


#include "FilterParameterWidgetsDialogs.h"

// Initialize private static member variable
QString OutputFileWidget::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OutputFileWidget::OutputFileWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  QWidget(parent),
  m_Filter(filter),
  m_FilterParameter(parameter)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OutputFileWidget::~OutputFileWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OutputFileWidget::setupGui()
{

  // Catch when the filter is about to execute the preflight
  connect(m_Filter, SIGNAL(preflightAboutToExecute()),
          this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(m_Filter, SIGNAL(preflightExecuted()),
          this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(m_Filter, SIGNAL(updateFilterParameters(AbstractFilter*)),
          this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  QFileCompleter* com = new QFileCompleter(this, false);
  value->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString&)),
                    this, SLOT(on_value_textChanged(const QString&)));

  if (m_FilterParameter != NULL)
  {
    QString units = m_FilterParameter->getUnits();
    if(units.isEmpty() == false)
    {
      label->setText(m_FilterParameter->getHumanLabel() + " (" + units + ")");
    }
    else
    {
      label->setText(m_FilterParameter->getHumanLabel() );
    }

    QString currentPath = m_Filter->property(PROPERTY_NAME_AS_CHAR).toString();
    value->setText(currentPath);
  }

  // is the filter parameter tied to a boolean property of the Filter Instance, if it is then we need to make the check box visible
  if(m_FilterParameter->isConditional() == true)
  {
    bool boolProp = m_Filter->property(m_FilterParameter->getConditionalProperty().toLatin1().constData() ).toBool();
    conditionalCB->setChecked(boolProp);
    conditionalCB->setText(m_FilterParameter->getConditionalLabel());
    value->setEnabled(boolProp);
    on_conditionalCB_stateChanged(conditionalCB->checkState());
  }
  else
  {
    widgetLayout->removeWidget(conditionalCB);
    conditionalCB->deleteLater();
    widgetLayout->removeWidget(linkLeft);
    linkLeft->deleteLater();
    widgetLayout->removeWidget(linkRight);
    linkRight->deleteLater();
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OutputFileWidget::on_conditionalCB_stateChanged(int state)
{
  bool boolProp = conditionalCB->isChecked();
  value->setEnabled(boolProp);
  value->setVisible(boolProp);
  selectBtn->setEnabled(boolProp);
  selectBtn->setVisible(boolProp);

  label->setVisible(boolProp);
  linkLeft->setVisible(boolProp);
  linkRight->setVisible(boolProp);

  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool OutputFileWidget::verifyPathExists(QString filePath, QLineEdit* lineEdit)
{
  QFileInfo fileinfo(filePath);
  if (false == fileinfo.exists())
  {
    lineEdit->setStyleSheet("border: 1px solid red;");
  }
  else
  {
    lineEdit->setStyleSheet("");
  }
  return fileinfo.exists();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OutputFileWidget::on_value_editingFinished()
{
  emit parametersChanged(); // This should force the preflight to run because we are emitting a signal
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OutputFileWidget::on_value_textChanged(const QString& text)
{
  setOpenDialogLastDirectory(text);
  // Set/Remove the red outline if the file does exist
  emit parametersChanged(); // This should force the preflight to run because we are emitting a signal
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OutputFileWidget::on_value_fileDropped(const QString& text)
{
  setOpenDialogLastDirectory(text);
  // Set/Remove the red outline if the file does exist
  verifyPathExists(text, value);

  emit parametersChanged(); // This should force the preflight to run because we are emitting a signal
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OutputFileWidget::on_selectBtn_clicked()
{
  QString currentPath = m_Filter->property(PROPERTY_NAME_AS_CHAR).toString();
  if(currentPath.isEmpty() == true)
  {
    currentPath = m_OpenDialogLastDirectory;
  }
  QString Ftype = m_FilterParameter->getFileType();
  QString ext = m_FilterParameter->getFileExtension();
  QString s = Ftype + QString(" Files (") + ext + QString(");;All Files(*.*)");
  QString defaultName = m_OpenDialogLastDirectory + QDir::separator() + "Untitled";
  QString file = QFileDialog::getSaveFileName(this, tr("Save File As"), defaultName, s);


  if(true == file.isEmpty())
  {
    return;
  }
  // bool ok = false;
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
void OutputFileWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  QString text = value->text();
  bool ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, text);
  if(false == ok)
  {
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(m_Filter, m_FilterParameter);
  }

  if(m_FilterParameter->isConditional() )
  {
    QVariant var(conditionalCB->isChecked());
    ok = filter->setProperty(m_FilterParameter->getConditionalProperty().toLatin1().constData(), var);
    if(false == ok)
    {
      FilterParameterWidgetsDialogs::ShowCouldNotSetConditionalFilterParameter(m_Filter, m_FilterParameter);
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OutputFileWidget::beforePreflight()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OutputFileWidget::afterPreflight()
{

}
