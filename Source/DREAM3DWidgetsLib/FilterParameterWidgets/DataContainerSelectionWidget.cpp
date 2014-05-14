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
#include "DataContainerSelectionWidget.h"

#include <QtCore/QMetaProperty>
#include <QtCore/QList>
#include <QtGui/QListWidgetItem>

#include "DREAM3DLib/DataContainers/DataArrayPath.h"

#include "FilterParameterWidgetsDialogs.h"

#define DATA_CONTAINER_LEVEL 0
#define ATTRIBUTE_MATRIX_LEVEL 1
#define ATTRIBUTE_ARRAY_LEVEL 2
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerSelectionWidget::DataContainerSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  QWidget(parent),
  m_Filter(filter),
  m_FilterParameter(parameter),
  m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerSelectionWidget::DataContainerSelectionWidget(QWidget* parent) :
  QWidget(parent),
  m_Filter(NULL),
  m_FilterParameter(NULL),
  m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerSelectionWidget::~DataContainerSelectionWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerSelectionWidget::initializeWidget(FilterParameter* parameter, AbstractFilter* filter)
{
  m_Filter = filter;
  m_FilterParameter = parameter;
  setupGui();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerSelectionWidget::setupGui()
{
  if(m_Filter == NULL)
  {
    return;
  }
  // Catch when the filter is about to execute the preflight
  connect(m_Filter, SIGNAL(preflightAboutToExecute()),
          this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(m_Filter, SIGNAL(preflightExecuted()),
          this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(m_Filter, SIGNAL(updateFilterParameters(AbstractFilter*)),
          this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  if (m_FilterParameter == NULL)
  {
    return;
  }
  QString units = m_FilterParameter->getUnits();
  if(units.isEmpty() == false)
  {
    label->setText(m_FilterParameter->getHumanLabel() + " (" + units + ")");
  }
  else
  {
    label->setText(m_FilterParameter->getHumanLabel() );
  }

  dataContainerList->blockSignals(true);

  dataContainerList->clear();

  // Now let the gui send signals like normal
  dataContainerList->blockSignals(false);


  populateComboBoxes();

  // is the filter parameter tied to a boolean property of the Filter Instance, if it is then we need to make the check box visible
  if(m_FilterParameter->isConditional() == true)
  {
    bool boolProp = m_Filter->property(m_FilterParameter->getConditionalProperty().toLatin1().constData() ).toBool();
    conditionalCB->setChecked(boolProp);
    conditionalCB->setText(m_FilterParameter->getConditionalLabel());
    dataContainerList->setEnabled(boolProp);
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
void DataContainerSelectionWidget::on_conditionalCB_stateChanged(int state)
{
  bool boolProp = conditionalCB->isChecked();
  dataContainerList->setEnabled(boolProp);
  dataContainerList->setVisible(boolProp);

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
void DataContainerSelectionWidget::populateComboBoxes()
{
  //  std::cout << "void DataContainerSelectionWidget::populateComboBoxesWithSelection()" << std::endl;


  // Now get the DataContainerArray from the Filter instance
  // We are going to use this to get all the current DataContainers
  DataContainerArray::Pointer dca = m_Filter->getDataContainerArray();
  if(NULL == dca.get()) { return; }

  // Check to see if we have any DataContainers to actually populate drop downs with.
  if(dca->getDataContainerArray().size() == 0)
  {
    return;
  }
  // Cache the DataContainerArray Structure for our use during all the selections
  m_DcaProxy = DataContainerArrayProxy(dca.get());

  // Populate the DataContainerArray Combo Box with all the DataContainers
  QList<DataContainerProxy> dcList = m_DcaProxy.list;
  QListIterator<DataContainerProxy> iter(dcList);

  while(iter.hasNext() )
  {
    DataContainerProxy dc = iter.next();
    if(dataContainerList->findText(dc.name) == -1 ) {
      dataContainerList->addItem(dc.name);
      qDebug() << "[1] Adding " << dc.name;
    }
  }

  //remove items in the combo that are NOT in the Data Container Array
  int count = dataContainerList->count();
  for(int i = count - 1; i >= 0; i--)
  {
    QString str0 = dataContainerList->itemText(i);
    iter.toFront();
    bool boo = false;
    while(iter.hasNext() )
    {
      DataContainerProxy dc = iter.next();
      if(dc.name.compare(str0) == 0)
      {
        boo = true; // found in the list
      }
    }
    if(boo == false)
    {
      dataContainerList->removeItem(i);
      qDebug() << "[2] Removing " << str0;
    }
  }

  // Grab what is currently selected
  QString curDcName = dataContainerList->currentText();


  // Get what is in the filter
  QString filtDcName;
  QString filtAmName;

  QVariant qvSelectedPath = m_Filter->property(PROPERTY_NAME_AS_CHAR);
  if( QString("QString").compare(qvSelectedPath.typeName()) == 0 )
  {
    filtDcName = qvSelectedPath.toString();
  }
  else if( QString("DataArrayPath").compare(qvSelectedPath.typeName()) == 0 )
  {
    DataArrayPath selectedPath = qvSelectedPath.value<DataArrayPath>();
    filtDcName = selectedPath.getDataContainerName();
    filtAmName = selectedPath.getAttributeMatrixName();
  }

  // Now to figure out which one of these to use. If this is the first time through then what we picked up from the
  // gui will be empty strings because nothing is there. If there is something in the filter then we should use that.
  // If there is something in both of them and they are NOT equal then we have a problem. Use the flag m_DidCausePreflight
  // to determine if the change from the GUI should over ride the filter or vice versa. there is a potential that in future
  // versions that something else is driving DREAM3D and pushing the changes to the filter and we need to reflect those
  // changes in the GUI, like a testing script?

  QString dcName = checkStringValues(curDcName, filtDcName);


  bool didBlock = false;

  if (!dataContainerList->signalsBlocked()) { didBlock = true; }
  dataContainerList->blockSignals(true);
  int dcIndex = dataContainerList->findText(dcName);
  if(dcIndex < 0 && dcName.isEmpty() == false) {
    dataContainerList->addItem(dcName);
    qDebug() << "[2] Adding " << dcName;
  } // the string was not found so just set it to the first index
  else {
    if(dcIndex < 0) { dcIndex = 0; } // Just set it to the first DataContainer in the list
    dataContainerList->setCurrentIndex(dcIndex);
  }
  if(didBlock) { dataContainerList->blockSignals(false); didBlock = false; }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataContainerSelectionWidget::checkStringValues(QString curDcName, QString filtDcName)
{
  if(curDcName.isEmpty() == true && filtDcName.isEmpty() == false)
  {return filtDcName;}
  else if(curDcName.isEmpty() == false && filtDcName.isEmpty() == true)
  {return curDcName;}
  else if(curDcName.isEmpty() == false && filtDcName.isEmpty() == false && m_DidCausePreflight == true)
  { return curDcName;}

  return filtDcName;

}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerSelectionWidget::selectDefaultPath()
{

  // set the default DataContainer
  if(dataContainerList->count() > 0)
  {
    dataContainerList->setCurrentIndex(0);
  }
  m_Filter->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerSelectionWidget::setSelectedPath(QString dcName, QString attrMatName, QString attrArrName)
{
  //dataContainerList->blockSignals(true);
  // Set the correct DataContainer
  int count = dataContainerList->count();
  for(int i = 0; i < count; i++)
  {
    if (dataContainerList->itemText(i).compare(dcName) == 0 )
    {
      dataContainerList->setCurrentIndex(i); // This will fire the currentItemChanged(...) signal
      break;
    }
  }
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerSelectionWidget::on_dataContainerList_currentIndexChanged(int index)
{
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerSelectionWidget::beforePreflight()
{
  if (NULL == m_Filter) { return; }
  if(m_DidCausePreflight == true)
  {
    std::cout << "***  DataContainerSelectionWidget already caused a preflight, just returning" << std::endl;
    return;
  }

  dataContainerList->blockSignals(true);
  // Reset all the combo box widgets to have the default selection of the first index in the list
  populateComboBoxes();
  dataContainerList->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerSelectionWidget::afterPreflight()
{
  // std::cout << "After Preflight" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerSelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  // Generate the path to the AttributeArray
  //DataArrayPath path(dataContainerList->currentText(), attributeMatrixList->currentText(), attributeArrayList->currentText());
  QVariant var(dataContainerList->currentText() );
  // var.setValue(path);
  bool ok = false;
  // Set the value into the Filter
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, var);
  if(false == ok)
  {
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(m_Filter, m_FilterParameter);
  }

  if(m_FilterParameter->isConditional() )
  {
    var.setValue(conditionalCB->isChecked());
    ok = filter->setProperty(m_FilterParameter->getConditionalProperty().toLatin1().constData(), var);
    if(false == ok)
    {
      FilterParameterWidgetsDialogs::ShowCouldNotSetConditionalFilterParameter(m_Filter, m_FilterParameter);
    }
  }

}
