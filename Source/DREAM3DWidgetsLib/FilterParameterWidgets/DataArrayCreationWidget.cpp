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
#include "DataArrayCreationWidget.h"

#include <QtCore/QMetaProperty>
#include <QtCore/QList>
#include <QtGui/QListWidgetItem>

#include "DREAM3DLib/DataContainers/DataArrayPath.h"
#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"

#define DATA_CONTAINER_LEVEL 0
#define ATTRIBUTE_MATRIX_LEVEL 1
#define ATTRIBUTE_ARRAY_LEVEL 2
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayCreationWidget::DataArrayCreationWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent),
  m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayCreationWidget::DataArrayCreationWidget(QWidget* parent) :
  FilterParameterWidget(NULL, NULL, parent),
  m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayCreationWidget::~DataArrayCreationWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayCreationWidget::initializeWidget(FilterParameter* parameter, AbstractFilter* filter)
{
  setFilter(filter);
  setFilterParameter(parameter);
  setupGui();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayCreationWidget::setupGui()
{


  blockSignals(true);
  if (getFilterParameter() != NULL)
  {
    QString units = getFilterParameter()->getUnits();
    if(units.isEmpty() == false)
    {
      label->setText(getFilterParameter()->getHumanLabel() + " (" + units + ")");
    }
    else
    {
      label->setText(getFilterParameter()->getHumanLabel() );
    }

    QString str = getFilter()->property(PROPERTY_NAME_AS_CHAR).toString();
    dataArrayName->setText(str);
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

  connect(dataArrayName, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&)));

  dataContainerList->blockSignals(true);
  attributeMatrixList->blockSignals(true);
  dataArrayName->blockSignals(true);
  dataContainerList->clear();
  attributeMatrixList->clear();
  dataArrayName->clear();
  // Now let the gui send signals like normal
  dataContainerList->blockSignals(false);
  attributeMatrixList->blockSignals(false);
  dataArrayName->blockSignals(false);

  populateComboBoxes();


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayCreationWidget::populateComboBoxes()
{
  //  std::cout << "void DataArrayCreationWidget::populateComboBoxesWithSelection()" << std::endl;


  // Now get the DataContainerArray from the Filter instance
  // We are going to use this to get all the current DataContainers
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
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
    if(dataContainerList->findText(dc.name) == -1 )
    {
      dataContainerList->addItem(dc.name);
    }
  }

  // Grab what is currently selected
  QString curDcName = dataContainerList->currentText();
  QString curAmName = attributeMatrixList->currentText();
  QString curDaName = dataArrayName->text();

  // Get what is in the filter
  DataArrayPath selectedPath = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<DataArrayPath>();

  // Split the path up to make sure we have a valid path separated by the "|" character

  QString filtDcName = selectedPath.getDataContainerName();
  QString filtAmName = selectedPath.getAttributeMatrixName();
  QString filtDaName = selectedPath.getDataArrayName();

  // Now to figure out which one of these to use. If this is the first time through then what we picked up from the
  // gui will be empty strings because nothing is there. If there is something in the filter then we should use that.
  // If there is something in both of them and they are NOT equal then we have a problem. Use the flag m_DidCausePreflight
  // to determine if the change from the GUI should over ride the filter or vice versa. there is a potential that in future
  // versions that something else is driving DREAM3D and pushing the changes to the filter and we need to reflect those
  // changes in the GUI, like a testing script?

  QString dcName = checkStringValues(curDcName, filtDcName);
  QString amName = checkStringValues(curAmName, filtAmName);
  QString daName = checkStringValues(curDaName, filtDaName);

  bool didBlock = false;

  if (!dataContainerList->signalsBlocked()) { didBlock = true; }
  dataContainerList->blockSignals(true);
  int dcIndex = dataContainerList->findText(dcName);
  if(dcIndex < 0 && dcName.isEmpty() == false)
  {
    dataContainerList->addItem(dcName);
  } // the string was not found so just set it to the first index
  else
  {
    if(dcIndex < 0) { dcIndex = 0; } // Just set it to the first DataContainer in the list
    dataContainerList->setCurrentIndex(dcIndex);
    populateAttributeMatrixList();
  }
  if(didBlock) { dataContainerList->blockSignals(false); didBlock = false; }


  if(!attributeMatrixList->signalsBlocked()) { didBlock = true; }
  attributeMatrixList->blockSignals(true);
  int amIndex = attributeMatrixList->findText(amName);
  if(amIndex < 0 && amName.isEmpty() == false) { attributeMatrixList->addItem(amName); } // The name of the attributeMatrix was not found so just set the first one
  else
  {
    if(amIndex < 0) { amIndex = 0; }
    attributeMatrixList->setCurrentIndex(amIndex);

  }
  if(didBlock) { attributeMatrixList->blockSignals(false); didBlock = false; }


  if(!dataArrayName->signalsBlocked()) { didBlock = true; }
  dataArrayName->blockSignals(true);
  dataArrayName->setText(daName);

  if(didBlock) { dataArrayName->blockSignals(false); didBlock = false; }// not be triggering an infinte recursion of preflights

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataArrayCreationWidget::checkStringValues(QString curDcName, QString filtDcName)
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
void DataArrayCreationWidget::selectDefaultPath()
{

  // set the default DataContainer
  if(dataContainerList->count() > 0)
  {
    dataContainerList->setCurrentIndex(0);
  }

  // Select the first AttributeMatrix in the list
  if(attributeMatrixList->count() > 0)
  {
    attributeMatrixList->setCurrentIndex(0);
  }

  // Set the default AttributeArray
  getFilter()->blockSignals(true);
  dataArrayName->setText("Default");
  getFilter()->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayCreationWidget::setSelectedPath(QString dcName, QString attrMatName, QString attrArrName)
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
  //dataContainerList->blockSignals(false);

  //attributeMatrixList->blockSignals(true);
  // Set the correct AttributeMatrix
  count = attributeMatrixList->count();
  for(int i = 0; i < count; i++)
  {
    if (attributeMatrixList->itemText(i).compare(attrMatName) == 0 )
    {
      attributeMatrixList->setCurrentIndex(i); // This will fire the currentItemChanged(...) signal
      break;
    }
  }
  //attributeMatrixList->blockSignals(false);

  dataArrayName->blockSignals(true);
  // Set the correct Data Array (if Possible). It may have changed since we started
  dataArrayName->setText("Foo");
  dataArrayName->blockSignals(false);
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayCreationWidget::on_dataContainerList_currentIndexChanged(int index)
{

  //  std::cout << "void DataArrayCreationWidget::on_dataContainerList_currentIndexChanged(int index)" << std::endl;
  populateAttributeMatrixList();

  // Select the first AttributeMatrix in the list
  if(attributeMatrixList->count() > 0)
  {
    on_attributeMatrixList_currentIndexChanged(0);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayCreationWidget::populateAttributeMatrixList()
{
  QString dcName = dataContainerList->currentText();

  // Clear the AttributeMatrix List
  attributeMatrixList->blockSignals(true);
  attributeMatrixList->clear();

  // Loop over the data containers until we find the proper data container
  QList<DataContainerProxy> containers = m_DcaProxy.list;
  QListIterator<DataContainerProxy> containerIter(containers);
  while(containerIter.hasNext())
  {
    DataContainerProxy dc = containerIter.next();

    if(dc.name.compare(dcName) == 0 )
    {
      // We found the proper Data Container, now populate the AttributeMatrix List
      QMap<QString, AttributeMatrixProxy> attrMats = dc.attributeMatricies;
      QMapIterator<QString, AttributeMatrixProxy> attrMatsIter(attrMats);
      while(attrMatsIter.hasNext() )
      {
        attrMatsIter.next();
        QString amName = attrMatsIter.key();
        attributeMatrixList->addItem(amName);
      }
    }
  }

  attributeMatrixList->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayCreationWidget::on_attributeMatrixList_currentIndexChanged(int index)
{
  // std::cout << "void DataArrayCreationWidget::on_attributeMatrixList_currentIndexChanged(int index)" << std::endl;
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayCreationWidget::on_dataArrayName_returnPressed()
{
  //qDebug() << "DataArrayCreationWidget::on_value_returnPressed() " << this;
  m_DidCausePreflight = true;
  on_applyChangesBtn_clicked();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayCreationWidget::hideButton()
{
  dataArrayName->setToolTip("");
  applyChangesBtn->setVisible(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayCreationWidget::widgetChanged(const QString& text)
{
  dataArrayName->setStyleSheet(QString::fromLatin1("color: rgb(255, 0, 0);"));
  dataArrayName->setToolTip("Press the 'Return' key to apply your changes");
  if(applyChangesBtn->isVisible() == false)
  {
    applyChangesBtn->setVisible(true);
    fadeInWidget(applyChangesBtn);
  }
}



#if 0
// -----------------------------------------------------------------------------
//blockSignals(false);
// -----------------------------------------------------------------------------
void DataArrayCreationWidget::on_dataArrayName_textChanged(const QString& string)
{
  //qDebug() << "void DataArrayCreationWidget::on_dataArrayName_currentIndexChanged(int index)";
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

#endif


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayCreationWidget::beforePreflight()
{
  if (NULL == getFilter()) { return; }
  if(m_DidCausePreflight == true)
  {
    std::cout << "***  DataArrayCreationWidget already caused a preflight, just returning" << std::endl;
    return;
  }

  dataContainerList->blockSignals(true);
  attributeMatrixList->blockSignals(true);
  dataArrayName->blockSignals(true);
  // Reset all the combo box widgets to have the default selection of the first index in the list
  populateComboBoxes();
  dataContainerList->blockSignals(false);
  attributeMatrixList->blockSignals(false);
  dataArrayName->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayCreationWidget::afterPreflight()
{
  // std::cout << "After Preflight" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayCreationWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  // Generate the path to the AttributeArray
  DataArrayPath path(dataContainerList->currentText(), attributeMatrixList->currentText(), dataArrayName->text());
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayCreationWidget::on_applyChangesBtn_clicked()
{
  dataArrayName->setStyleSheet(QString(""));
  emit parametersChanged();

  QPointer<FaderWidget> faderWidget = new FaderWidget(applyChangesBtn);
  setFaderWidget(faderWidget);

  if (getFaderWidget())
  {
    faderWidget->close();
  }
  faderWidget = new FaderWidget(applyChangesBtn);
  faderWidget->setFadeOut();
  connect(faderWidget, SIGNAL(animationComplete() ),
          this, SLOT(hideButton()));
  faderWidget->start();
}
