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
#include "MultiDataArraySelectionWidget.h"

#include <QtCore/QMetaProperty>
#include <QtCore/QList>

#include <QtGui/QStandardItemModel>

#include <QtWidgets/QListWidgetItem>

#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/FilterParameters/FilterParameter.h"
#include "SIMPLib/FilterParameters/MultiDataArraySelectionFilterParameter.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"
#include "FilterParameterWidgetUtils.hpp"

// Include the MOC generated file for this class
#include "moc_MultiDataArraySelectionWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiDataArraySelectionWidget::MultiDataArraySelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent),
  m_DidCausePreflight(false)
{
  m_FilterParameter = dynamic_cast<MultiDataArraySelectionFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != NULL, "NULL Pointer", "MultiDataArraySelectionWidget can ONLY be used with a MultiDataArraySelectionFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiDataArraySelectionWidget::MultiDataArraySelectionWidget(QWidget* parent) :
  FilterParameterWidget(NULL, NULL, parent),
  m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiDataArraySelectionWidget::~MultiDataArraySelectionWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::initializeWidget(FilterParameter* parameter, AbstractFilter* filter)
{
  //qDebug() << getFilter()->getHumanLabel() << "  " << getFilterParameter()->getHumanLabel() << " MultiDataArraySelectionWidget::initializeWidget";
  setFilter(filter);
  setFilterParameter(parameter);
  setupGui();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::setupGui()
{
  // Sanity Check the filter and the filter parameter
  if (getFilter() == NULL)
  {
    return;
  }
  if (getFilterParameter() == NULL)
  {
    return;
  }

  // Generate the text for the QLabel
  label->setText(getFilterParameter()->getHumanLabel());

  // Block Signals from the ComboBoxes while we clear them
  dataContainerCombo->blockSignals(true);
  attributeMatrixCombo->blockSignals(true);
  attributeArraysWidget->blockSignals(true);

  dataContainerCombo->clear();
  attributeMatrixCombo->clear();
  attributeArraysWidget->clear();

  dataContainerCombo->blockSignals(false);
  attributeMatrixCombo->blockSignals(false);
  attributeArraysWidget->blockSignals(false);

  populateComboBoxes();

  // Lastly, hook up the filter's signals and slots to our own signals and slots
  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()),
          this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()),
          this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)),
          this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::populateComboBoxes()
{
  // Now get the DataContainerArray from the Filter instance
  // We are going to use this to get all the current DataContainers
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if (NULL == dca.get()) { return; }

  // Check to see if we have any DataContainers to actually populate drop downs with.
  if (dca->getDataContainers().size() == 0)
  {
    dataContainerCombo->clear();
    attributeMatrixCombo->clear();
    return;
  }

  // Grab what is currently selected
  QString curDcName = dataContainerCombo->currentText();
  QString curAmName = attributeMatrixCombo->currentText();

  // Cache the DataContainerArray Structure for our use during all the selections
  m_DcaProxy = DataContainerArrayProxy(dca.get());

  // Populate the DataContainer ComboBox
  FilterPararameterWidgetUtils::PopulateDataContainerComboBox<MultiDataArraySelectionFilterParameter>(getFilter(), getFilterParameter(), dataContainerCombo, m_DcaProxy);


  // Get what is in the filter
  QVector<DataArrayPath> selectedPaths = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<QVector<DataArrayPath> >();
  QString filtDcName = "";
  QString filtAmName = "";

  if (DataArrayPath::ValidateVector(selectedPaths) == false)
  {
    // Throw an error?  The Data Containers and Attribute Matrices are not the same
  }

  if (selectedPaths.isEmpty() == false)
  {
    filtDcName = selectedPaths.first().getDataContainerName();
    filtAmName = selectedPaths.first().getAttributeMatrixName();
  }

  QString dcName = "";
  QString amName = "";

  // If EVERYTHING is empty, then try the default value
  if (filtDcName.isEmpty() && filtAmName.isEmpty() && curDcName.isEmpty() && curAmName.isEmpty())
  {
    QVector<DataArrayPath> daPaths = dynamic_cast<MultiDataArraySelectionFilterParameter*>(getFilterParameter())->getDefaultPaths();
    selectedPaths = daPaths;

    if (daPaths.isEmpty() == false)
    {
      dcName = daPaths.first().getDataContainerName();
      amName = daPaths.first().getAttributeMatrixName();
    }
  }
  else
  {
    // Now to figure out which one of these to use. If this is the first time through then what we picked up from the
    // gui will be empty strings because nothing is there. If there is something in the filter then we should use that.
    // If there is something in both of them and they are NOT equal then we have a problem. Use the flag m_DidCausePreflight
    // to determine if the change from the GUI should over ride the filter or vice versa. there is a potential that in future
    // versions that something else is driving DREAM3D and pushing the changes to the filter and we need to reflect those
    // changes in the GUI, like a testing script?

    dcName = checkStringValues(curDcName, filtDcName);
    amName = checkStringValues(curAmName, filtAmName);
  }

  bool didBlock = false;

  if (!dataContainerCombo->signalsBlocked()) { didBlock = true; }
  dataContainerCombo->blockSignals(true);

  int dcIndex = dataContainerCombo->findText(dcName);
  dataContainerCombo->setCurrentIndex(dcIndex);

  FilterPararameterWidgetUtils::PopulateAttributeMatrixComboBox<MultiDataArraySelectionFilterParameter>(getFilter(), getFilterParameter(), dataContainerCombo, attributeMatrixCombo, m_DcaProxy);

  if (didBlock) { dataContainerCombo->blockSignals(false); didBlock = false; }

  if (!attributeMatrixCombo->signalsBlocked()) { didBlock = true; }
  attributeMatrixCombo->blockSignals(true);

  if (dcIndex < 0)
  {
    attributeMatrixCombo->setCurrentIndex(-1);
    attributeArraysWidget->clear();
  }
  else
  {
    int amIndex = attributeMatrixCombo->findText(amName);
    attributeMatrixCombo->setCurrentIndex(amIndex);
    FilterPararameterWidgetUtils::PopulateAttributeArrayList<MultiDataArraySelectionFilterParameter, QListWidget>
            (getFilter(), getFilterParameter(), dataContainerCombo, attributeMatrixCombo, attributeArraysWidget, m_DcaProxy, selectedPaths);

  }

  if(didBlock) { attributeMatrixCombo->blockSignals(false); didBlock = false; }

  updateSelectAllCheckbox();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiDataArraySelectionWidget::checkStringValues(QString curDcName, QString filtDcName)
{
  ////qDebug() << "    checkStringValues(...)" << curDcName << "  " << filtDcName;
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
void MultiDataArraySelectionWidget::on_dataContainerCombo_currentIndexChanged(int index)
{
  FilterPararameterWidgetUtils::PopulateAttributeMatrixComboBox<MultiDataArraySelectionFilterParameter>(getFilter(), getFilterParameter(), dataContainerCombo, attributeMatrixCombo, m_DcaProxy);

  // Do not select an attribute matrix from the list
  if (attributeMatrixCombo->count() > 0)
  {
    attributeMatrixCombo->setCurrentIndex(-1);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::on_attributeMatrixCombo_currentIndexChanged(int index)
{
  QVector<DataArrayPath> selectedPaths = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<QVector<DataArrayPath> >();

  FilterPararameterWidgetUtils::PopulateAttributeArrayList<MultiDataArraySelectionFilterParameter, QListWidget>
          (getFilter(), getFilterParameter(), dataContainerCombo, attributeMatrixCombo, attributeArraysWidget, m_DcaProxy, selectedPaths);

  updateSelectAllCheckbox();

  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::on_attributeArraysWidget_itemChanged(QListWidgetItem* item)
{
  updateSelectAllCheckbox();

  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::beforePreflight()
{
  if (NULL == getFilter()) { return; }
  if (m_DidCausePreflight == true)
  {
   // std::cout << "***  MultiDataArraySelectionWidget already caused a preflight, just returning" << std::endl;
    return;
  }

  dataContainerCombo->blockSignals(true);
  attributeMatrixCombo->blockSignals(true);
  attributeArraysWidget->blockSignals(true);
  // Reset all the combo box widgets to have the default selection of the first index in the list
  populateComboBoxes();
  dataContainerCombo->blockSignals(false);
  attributeMatrixCombo->blockSignals(false);
  attributeArraysWidget->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::afterPreflight()
{
  // std::cout << "After Preflight" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy MultiDataArraySelectionWidget::generateDCAProxy()
{
  DataContainerArrayProxy dcaProxy;
  QString dcaName = dataContainerCombo->currentText();
  DataContainerProxy dcProxy(dcaName, true);

  QString amName = attributeMatrixCombo->currentText();
  AttributeMatrixProxy amProxy(amName, true);

  for (int i = 0; i < attributeArraysWidget->count(); i++)
  {
    QListWidgetItem* item = attributeArraysWidget->item(i);
    QString daName = item->text();

    bool checkState;
    if (item->checkState() == Qt::Checked)
    {
      checkState = true;
    }
    else
    {
      checkState = false;
    }
    DataArrayProxy daProxy(dcaName + "|" + amName, daName, checkState);
    amProxy.dataArrays.insert(daName, daProxy);
    dcProxy.attributeMatricies.insert(amName, amProxy);
    dcaProxy.dataContainers.insert(dcProxy.name, dcProxy);
  }

  return dcaProxy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  QVector<DataArrayPath> selectedPaths;
  for (int i = 0; i < attributeArraysWidget->count(); i++)
  {
    if (attributeArraysWidget->item(i)->checkState() == Qt::Checked)
    {
      DataArrayPath path(dataContainerCombo->currentText(), attributeMatrixCombo->currentText(), attributeArraysWidget->item(i)->text());
      selectedPaths.push_back(path);
    }
  }

  // Generate the path to the AttributeArray
  QVariant var;
  var.setValue(selectedPaths);
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
void MultiDataArraySelectionWidget::on_selectCheckBox_stateChanged(int state)
{
  attributeArraysWidget->blockSignals(true);

  for (int i = 0; i < attributeArraysWidget->count(); i++)
  {
    if (attributeArraysWidget->item(i)->flags().testFlag(Qt::ItemIsEnabled) == true)
    {
      if (state == Qt::Checked)
      {
        attributeArraysWidget->item(i)->setCheckState(Qt::Checked);
      }
      else if (state == Qt::Unchecked)
      {
        attributeArraysWidget->item(i)->setCheckState(Qt::Unchecked);
      }
      else
      {
        // Skip the "Partially Checked" check state when merely toggling the "Select All/Deselect All" checkbox
        selectCheckBox->setCheckState(Qt::Checked);
      }
    }
  }

  attributeArraysWidget->blockSignals(false);

  // This is needed to enable/disable the Data Container and Attribute Matrix combo boxes
  updateSelectAllCheckbox();

  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::updateSelectAllCheckbox()
{
  bool checkedStateExists = false;
  int checkedStateCount = 0;
  bool uncheckedStateExists = false;
  bool oneItemEnabled = false;

  for (int i = 0; i < attributeArraysWidget->count(); i++)
  {
    if (attributeArraysWidget->item(i)->flags().testFlag(Qt::ItemIsEnabled) == true)
    {
      oneItemEnabled = true;
      if (attributeArraysWidget->item(i)->checkState() == Qt::Checked)
      {
        checkedStateExists = true;
        checkedStateCount++;
      }
      else
      {
        uncheckedStateExists = true;
      }
    }
  }

  if (oneItemEnabled == false)
  {
    selectCheckBox->blockSignals(true);
    selectCheckBox->setDisabled(true);
    selectCheckBox->blockSignals(false);
  }
  else if (oneItemEnabled == true)
  {
    selectCheckBox->blockSignals(true);
    selectCheckBox->setDisabled(false);
    selectCheckBox->blockSignals(false);
  }
  else if (checkedStateExists == true && uncheckedStateExists == true)
  {
    selectCheckBox->blockSignals(true);
    selectCheckBox->setCheckState(Qt::PartiallyChecked);
    dataContainerCombo->setDisabled(true);
    attributeMatrixCombo->setDisabled(true);
    selectCheckBox->blockSignals(false);
  }
  else if (checkedStateCount == attributeArraysWidget->count())
  {
    selectCheckBox->blockSignals(true);
    selectCheckBox->setCheckState(Qt::Checked);
    dataContainerCombo->setDisabled(true);
    attributeMatrixCombo->setDisabled(true);
    selectCheckBox->blockSignals(false);
  }
  else
  {
    selectCheckBox->blockSignals(true);
    selectCheckBox->setCheckState(Qt::Unchecked);
    dataContainerCombo->setEnabled(true);
    attributeMatrixCombo->setEnabled(true);
    selectCheckBox->blockSignals(false);
  }
}






