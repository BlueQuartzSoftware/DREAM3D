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

#include "AttributeMatrixSelectionWidget.h"

#include <QtCore/QMetaProperty>
#include <QtCore/QList>

#include <QtGui/QStandardItemModel>

#include <QtWidgets/QListWidgetItem>

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"
#include "FilterParameterWidgetUtils.hpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixSelectionWidget::AttributeMatrixSelectionWidget(QWidget* parent) :
  FilterParameterWidget(NULL, NULL, parent),
  m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixSelectionWidget::AttributeMatrixSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent),
  m_DidCausePreflight(false)
{
  m_FilterParameter = dynamic_cast<AttributeMatrixSelectionFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != NULL, "NULL Pointer", "AttributeMatrixSelectionWidget can ONLY be used with an AttributeMatrixSelectionFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixSelectionWidget::~AttributeMatrixSelectionWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixSelectionWidget::setupGui()
{
  if(getFilter() == NULL)
  {
    return;
  }
  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()),
          this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()),
          this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)),
          this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  if (getFilterParameter() == NULL)
  {
    return;
  }

  label->setText(getFilterParameter()->getHumanLabel() );


  dataContainerCombo->blockSignals(true);
  attributeMatrixCombo->blockSignals(true);
  dataContainerCombo->clear();
  attributeMatrixCombo->clear();
  // Now let the gui send signals like normal
  dataContainerCombo->blockSignals(false);
  attributeMatrixCombo->blockSignals(false);

  populateComboBoxes();


}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixSelectionWidget::populateComboBoxes()
{
  //  std::cout << "void AttributeMatrixSelectionWidget::populateComboBoxesWithSelection()" << std::endl;


  // Now get the DataContainerArray from the Filter instance
  // We are going to use this to get all the current DataContainers
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(NULL == dca.get()) { return; }

  // Grab what is currently selected
  QString curDcName = dataContainerCombo->currentText();
  QString curAmName = attributeMatrixCombo->currentText();

  // Check to see if we have any DataContainers to actually populate drop downs with.
  if(dca->getDataContainers().size() == 0)
  {
    dataContainerCombo->clear();
    attributeMatrixCombo->clear();
    return;
  }

  // Cache the DataContainerArray Structure for our use during all the selections
  m_DcaProxy = DataContainerArrayProxy(dca.get());

  // Populate the DataContainer ComboBox
  FilterPararameterWidgetUtils::PopulateDataContainerComboBox<AttributeMatrixSelectionFilterParameter>(getFilter(), getFilterParameter(), dataContainerCombo, m_DcaProxy);



  // Get what is in the filter
  QVariant qvSelectedPath = getFilter()->property(PROPERTY_NAME_AS_CHAR);
  DataArrayPath selectedPath = qvSelectedPath.value<DataArrayPath>();

  QString filtDcName = selectedPath.getDataContainerName();
  QString filtAmName = selectedPath.getAttributeMatrixName();

  QString dcName;
  QString amName;

  // If EVERYTHING is empty, then try the default value
  if(filtDcName.isEmpty() && filtAmName.isEmpty()
      && curDcName.isEmpty() && curAmName.isEmpty() )
  {
    DataArrayPath daPath = getFilterParameter()->getDefaultValue().value<DataArrayPath>();
    dcName = daPath.getDataContainerName();
    amName = daPath.getAttributeMatrixName();
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
    if( !dca->doesDataContainerExist(dcName) ) { dcName = ""; }
    amName = checkStringValues(curAmName, filtAmName);
    if ( !dca->doesAttributeMatrixExist(DataArrayPath(dcName, amName, "") ) ) { amName = ""; }
  }

  bool didBlock = false;

  if (!dataContainerCombo->signalsBlocked()) { didBlock = true; }
  dataContainerCombo->blockSignals(true);
  int dcIndex = dataContainerCombo->findText(dcName);

  dataContainerCombo->setCurrentIndex(dcIndex);
  FilterPararameterWidgetUtils::PopulateAttributeMatrixComboBox<AttributeMatrixSelectionFilterParameter>(getFilter(), getFilterParameter(), dataContainerCombo, attributeMatrixCombo, m_DcaProxy);

  if(didBlock) { dataContainerCombo->blockSignals(false); didBlock = false; }

  if(!attributeMatrixCombo->signalsBlocked()) { didBlock = true; }
  attributeMatrixCombo->blockSignals(true);

  if (dcIndex < 0)
  {
    attributeMatrixCombo->setCurrentIndex(-1);
  }
  else
  {
    int amIndex = attributeMatrixCombo->findText(amName);
    attributeMatrixCombo->setCurrentIndex(amIndex);
  }

  if(didBlock) { attributeMatrixCombo->blockSignals(false); didBlock = false; }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AttributeMatrixSelectionWidget::checkStringValues(QString curDcName, QString filtDcName)
{
  if(curDcName.isEmpty() == true && filtDcName.isEmpty() == false)
  {return filtDcName;}
  else if(curDcName.isEmpty() == false && filtDcName.isEmpty() == true)
  {return curDcName;}
  else if(curDcName.isEmpty() == false && filtDcName.isEmpty() == false && m_DidCausePreflight == true)
  { return curDcName;}

  return filtDcName;

}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixSelectionWidget::selectDefaultPath()
{

  // set the default DataContainer
  if(dataContainerCombo->count() > 0)
  {
    dataContainerCombo->setCurrentIndex(0);
  }

  // Set the default AttributeArray
  getFilter()->blockSignals(true);
  // Select the first AttributeMatrix in the list
  if(attributeMatrixCombo->count() > 0)
  {
    attributeMatrixCombo->setCurrentIndex(0);
  }
  getFilter()->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixSelectionWidget::setSelectedPath(QString dcName, QString attrMatName, QString attrArrName)
{
  // Set the correct DataContainer
  int count = dataContainerCombo->count();
  for(int i = 0; i < count; i++)
  {
    if (dataContainerCombo->itemText(i).compare(dcName) == 0 )
    {
      dataContainerCombo->setCurrentIndex(i); // This will fire the currentItemChanged(...) signal
      break;
    }
  }

  // Set the correct AttributeMatrix
  count = attributeMatrixCombo->count();
  for(int i = 0; i < count; i++)
  {
    if (attributeMatrixCombo->itemText(i).compare(attrMatName) == 0 )
    {
      attributeMatrixCombo->setCurrentIndex(i); // This will fire the currentItemChanged(...) signal
      break;
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixSelectionWidget::on_dataContainerCombo_currentIndexChanged(int index)
{
  FilterPararameterWidgetUtils::PopulateAttributeMatrixComboBox<AttributeMatrixSelectionFilterParameter>(getFilter(), getFilterParameter(), dataContainerCombo, attributeMatrixCombo, m_DcaProxy);

  // Do not select an attribute matrix from the list
  if (attributeMatrixCombo->count() > 0)
  {
    attributeMatrixCombo->setCurrentIndex(-1);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixSelectionWidget::on_attributeMatrixCombo_currentIndexChanged(int index)
{
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixSelectionWidget::beforePreflight()
{
  if (NULL == getFilter()) { return; }
  if(m_DidCausePreflight == true)
  {
    //std::cout << "***  AttributeMatrixSelectionWidget already caused a preflight, just returning" << std::endl;
    return;
  }

  dataContainerCombo->blockSignals(true);
  attributeMatrixCombo->blockSignals(true);
  // Reset all the combo box widgets to have the default selection of the first index in the list
  populateComboBoxes();
  dataContainerCombo->blockSignals(false);
  attributeMatrixCombo->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixSelectionWidget::afterPreflight()
{
  // std::cout << "After Preflight" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixSelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  // Geenerate the path to the AttributeArray
  // QString path = QString("%1|%2").arg().arg();

  DataArrayPath path(dataContainerCombo->currentText(), attributeMatrixCombo->currentText(), "");
  QVariant var;
  var.setValue(path);
  bool ok = false;
  // Set the value into the Filter
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, var);
  if(false == ok)
  {
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(getFilter(), getFilterParameter());
  }

}
