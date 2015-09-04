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

#include "AttributeMatrixCreationWidget.h"

#include <QtCore/QMetaProperty>
#include <QtCore/QList>

#include <QtGui/QStandardItemModel>

#include <QtWidgets/QListWidgetItem>

#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"
#include "FilterParameterWidgetUtils.hpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixCreationWidget::AttributeMatrixCreationWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
FilterParameterWidget(parameter, filter, parent),
m_DidCausePreflight(false)
{
  m_FilterParameter = dynamic_cast<AttributeMatrixCreationFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != NULL, "NULL Pointer", "AttributeMatrixCreationWidget can ONLY be used with a AttributeMatrixCreationFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixCreationWidget::AttributeMatrixCreationWidget(QWidget* parent) :
FilterParameterWidget(NULL, NULL, parent),
m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixCreationWidget::~AttributeMatrixCreationWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixCreationWidget::initializeWidget(FilterParameter* parameter, AbstractFilter* filter)
{
  setFilter(filter);
  setFilterParameter(parameter);
  setupGui();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixCreationWidget::setupGui()
{
  blockSignals(true);
  if (getFilterParameter() != NULL)
  {
    label->setText(getFilterParameter()->getHumanLabel());

    QString str = getFilter()->property(PROPERTY_NAME_AS_CHAR).toString();
    attributeMatrixName->setText(str);
  }
  blockSignals(false);

  applyChangesBtn->setVisible(false);


  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()),
    this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()),
    this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)),
    this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  connect(attributeMatrixName, SIGNAL(textChanged(const QString&)),
    this, SLOT(widgetChanged(const QString&)));
  connect(dataContainerCombo, SIGNAL(currentIndexChanged(const QString&)),
    this, SLOT(widgetChanged(const QString&)));

  dataContainerCombo->blockSignals(true);
  attributeMatrixName->blockSignals(true);
  dataContainerCombo->clear();
  attributeMatrixName->clear();
  // Now let the gui send signals like normal
  dataContainerCombo->blockSignals(false);
  attributeMatrixName->blockSignals(false);

  populateComboBoxes();


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixCreationWidget::populateComboBoxes()
{
  //  std::cout << "void AttributeMatrixCreationWidget::populateComboBoxesWithSelection()" << std::endl;


  // Now get the DataContainerArray from the Filter instance
  // We are going to use this to get all the current DataContainers
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if (NULL == dca.get()) { return; }

  // Check to see if we have any DataContainers to actually populate drop downs with.
  if (dca->getDataContainers().size() == 0)
  {
    return;
  }
  // Cache the DataContainerArray Structure for our use during all the selections
  m_DcaProxy = DataContainerArrayProxy(dca.get());

  // Populate the DataContainer ComboBox
  FilterPararameterWidgetUtils::PopulateDataContainerComboBox<AttributeMatrixCreationFilterParameter>(getFilter(), getFilterParameter(), dataContainerCombo, m_DcaProxy);

  // Grab what is currently selected
  QString curDcName = dataContainerCombo->currentText();
  QString curAmName = attributeMatrixName->text();

  // Get what is in the filter
  DataArrayPath selectedPath = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<DataArrayPath>();

  // Split the path up to make sure we have a valid path separated by the "|" character

  QString filtDcName = selectedPath.getDataContainerName();
  QString filtAmName = selectedPath.getAttributeMatrixName();

  // Now to figure out which one of these to use. If this is the first time through then what we picked up from the
  // gui will be empty strings because nothing is there. If there is something in the filter then we should use that.
  // If there is something in both of them and they are NOT equal then we have a problem. Use the flag m_DidCausePreflight
  // to determine if the change from the GUI should over ride the filter or vice versa. there is a potential that in future
  // versions that something else is driving DREAM3D and pushing the changes to the filter and we need to reflect those
  // changes in the GUI, like a testing script?

  QString dcName = checkStringValues(curDcName, filtDcName);
  if (!dca->doesDataContainerExist(dcName)) { dcName = ""; }

  bool didBlock = false;

  if (!dataContainerCombo->signalsBlocked()) { didBlock = true; }
  dataContainerCombo->blockSignals(true);

  int dcIndex = dataContainerCombo->findText(dcName);
  dataContainerCombo->setCurrentIndex(dcIndex);

  if (didBlock) { dataContainerCombo->blockSignals(false); didBlock = false; }

  if (!attributeMatrixName->signalsBlocked()) { didBlock = true; }
  attributeMatrixName->blockSignals(true);

  QString amName = checkStringValues(curAmName, filtAmName);

  attributeMatrixName->setText(amName);

  if (didBlock) { attributeMatrixName->blockSignals(false); didBlock = false; }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AttributeMatrixCreationWidget::checkStringValues(QString curDcName, QString filtDcName)
{
  if (curDcName.isEmpty() == true && filtDcName.isEmpty() == false)
  {
    return filtDcName;
  }
  else if (curDcName.isEmpty() == false && filtDcName.isEmpty() == true)
  {
    return curDcName;
  }
  else if (curDcName.isEmpty() == false && filtDcName.isEmpty() == false && m_DidCausePreflight == true)
  {
    return curDcName;
  }

  return filtDcName;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixCreationWidget::on_dataContainerCombo_currentIndexChanged(int index)
{
  m_DidCausePreflight = true;
  on_applyChangesBtn_clicked();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixCreationWidget::on_attributeMatrixName_returnPressed()
{
  m_DidCausePreflight = true;
  on_applyChangesBtn_clicked();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixCreationWidget::hideButton()
{
  attributeMatrixName->setToolTip("");
  applyChangesBtn->setVisible(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixCreationWidget::widgetChanged(const QString& text)
{
  attributeMatrixName->setStyleSheet(QString::fromLatin1("color: rgb(255, 0, 0);"));
  attributeMatrixName->setToolTip("Press the 'Return' key to apply your changes");
  if (applyChangesBtn->isVisible() == false)
  {
    applyChangesBtn->setVisible(true);
    fadeInWidget(applyChangesBtn);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixCreationWidget::beforePreflight()
{
  if (NULL == getFilter()) { return; }
  if (m_DidCausePreflight == true)
  {
   // std::cout << "***  AttributeMatrixCreationWidget already caused a preflight, just returning" << std::endl;
    return;
  }

  dataContainerCombo->blockSignals(true);
  attributeMatrixName->blockSignals(true);

  // Reset all the combo box widgets to have the default selection of the first index in the list
  populateComboBoxes();

  dataContainerCombo->blockSignals(false);
  attributeMatrixName->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixCreationWidget::afterPreflight()
{
  // std::cout << "After Preflight" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixCreationWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  // Generate the path to the AttributeArray
  DataArrayPath path(dataContainerCombo->currentText(), attributeMatrixName->text(), "");
  QVariant var;
  var.setValue(path);
  bool ok = false;
  // Set the value into the Filter
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, var);
  if (false == ok)
  {
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(getFilter(), getFilterParameter());
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixCreationWidget::on_applyChangesBtn_clicked()
{
  attributeMatrixName->setStyleSheet(QString(""));
  emit parametersChanged();

  QPointer<FaderWidget> faderWidget = new FaderWidget(applyChangesBtn);
  setFaderWidget(faderWidget);

  if (getFaderWidget())
  {
    faderWidget->close();
  }
  faderWidget = new FaderWidget(applyChangesBtn);
  faderWidget->setFadeOut();
  connect(faderWidget, SIGNAL(animationComplete()),
    this, SLOT(hideButton()));
  faderWidget->start();
}
