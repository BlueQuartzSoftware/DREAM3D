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

#include "DataArraySelectionWidget.h"

#include <QtCore/QMetaProperty>
#include <QtCore/QList>

#include <QtGui/QStandardItemModel>

#include <QtWidgets/QListWidgetItem>


#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/FilterParameters/FilterParameter.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"
#include "FilterParameterWidgetUtils.hpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArraySelectionWidget::DataArraySelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent),
  m_DidCausePreflight(false)
{
  m_FilterParameter = dynamic_cast<DataArraySelectionFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != NULL, "NULL Pointer", "DataArraySelectionWidget can ONLY be used with a DataArraySelectionFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArraySelectionWidget::DataArraySelectionWidget(QWidget* parent) :
  FilterParameterWidget(NULL, NULL, parent),
  m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArraySelectionWidget::~DataArraySelectionWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArraySelectionWidget::initializeWidget(FilterParameter* parameter, AbstractFilter* filter)
{
  //qDebug() << getFilter()->getHumanLabel() << "  " << getFilterParameter()->getHumanLabel() << " DataArraySelectionWidget::initializeWidget";
  setFilter(filter);
  setFilterParameter(parameter);
  setupGui();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArraySelectionWidget::setupGui()
{
  // Sanity Check the filter and the filter parameter
  if(getFilter() == NULL)
  {
    return;
  }
  if (getFilterParameter() == NULL)
  {
    return;
  }

  // Generate the text for the QLabel
  label->setText(getFilterParameter()->getHumanLabel() );

  // Get the default path from the Filter instance to cache
  m_DefaultPath = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<DataArrayPath>();
  //  dataContainerCombo->addItem(m_DefaultPath.getDataContainerName());
  //  attributeMatrixCombo->addItem(m_DefaultPath.getAttributeMatrixName() );
  //  attributeArrayCombo->addItem(m_DefaultPath.getDataArrayName() );

  // Block Signals from the ComboBoxes while we clear them
  dataContainerCombo->blockSignals(true);
  attributeMatrixCombo->blockSignals(true);
  attributeArrayCombo->blockSignals(true);

  dataContainerCombo->clear();
  attributeMatrixCombo->clear();
  attributeArrayCombo->clear();

  dataContainerCombo->blockSignals(false);
  attributeMatrixCombo->blockSignals(false);
  attributeArrayCombo->blockSignals(false);

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
void DataArraySelectionWidget::populateComboBoxes()
{
  //qDebug() << "-----------------------------------------------";
  //qDebug() << getFilter()->getHumanLabel() << "  " << getFilterParameter()->getHumanLabel() << " DataArraySelectionWidget::populateComboBoxes()";
  // Now get the DataContainerArray from the Filter instance
  // We are going to use this to get all the current DataContainers
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(NULL == dca.get()) { return; }

  //qDebug() << getFilter()->getHumanLabel() << "  " << getFilterParameter()->getHumanLabel();
  // Grab what is currently selected
  QString curDcName = dataContainerCombo->currentText();
  QString curAmName = attributeMatrixCombo->currentText();
  QString curDaName = attributeArrayCombo->currentText();
  //qDebug() << "Current ComboBox Value: " << curDcName << "::" << curAmName << "::" << curDaName;

  // Check to see if we have any DataContainers to actually populate drop downs with.
  if(dca->getDataContainers().size() == 0)
  {
    dataContainerCombo->clear();
    attributeMatrixCombo->clear();
    attributeArrayCombo->clear();
    return;
  }
  // Cache the DataContainerArray Structure for our use during all the selections
  m_DcaProxy = DataContainerArrayProxy(dca.get());

  // Populate the DataContainer ComboBox
  FilterPararameterWidgetUtils::PopulateDataContainerComboBox<DataArraySelectionFilterParameter>(getFilter(), getFilterParameter(), dataContainerCombo, m_DcaProxy);

  // Get what is in the filter
  DataArrayPath selectedPath = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<DataArrayPath>();

  // Split the path up to make sure we have a valid path separated by the "|" character
  QString filtDcName = selectedPath.getDataContainerName();
  QString filtAmName = selectedPath.getAttributeMatrixName();
  QString filtDaName = selectedPath.getDataArrayName();

  QString dcName;
  QString amName;
  QString daName;

  // If EVERYTHING is empty, then try the default value
  if(filtDcName.isEmpty() && filtAmName.isEmpty() && filtDaName.isEmpty()
      && curDcName.isEmpty() && curAmName.isEmpty() && curDaName.isEmpty() )
  {
    DataArrayPath daPath = getFilterParameter()->getDefaultValue().value<DataArrayPath>();
    dcName = daPath.getDataContainerName();
    amName = daPath.getAttributeMatrixName();
    daName = daPath.getDataArrayName();
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
    daName = checkStringValues(curDaName, filtDaName);
    if ( !dca->doesAttributeArrayExist(DataArrayPath(dcName, amName, daName) )) { daName = ""; }
  }

  bool didBlock = false;

  if (!dataContainerCombo->signalsBlocked()) { didBlock = true; }
  dataContainerCombo->blockSignals(true);
  int dcIndex = dataContainerCombo->findText(dcName);
  dataContainerCombo->setCurrentIndex(dcIndex);

  FilterPararameterWidgetUtils::PopulateAttributeMatrixComboBox<DataArraySelectionFilterParameter>(getFilter(), getFilterParameter(), dataContainerCombo, attributeMatrixCombo, m_DcaProxy);

  if(didBlock) { dataContainerCombo->blockSignals(false); didBlock = false; }
  if(!attributeMatrixCombo->signalsBlocked()) { didBlock = true; }
  attributeMatrixCombo->blockSignals(true);

  int amIndex = -1;
  if (dcIndex < 0)
  {
    attributeMatrixCombo->setCurrentIndex(-1);
    attributeArrayCombo->setCurrentIndex(-1);
  }
  else
  {
    amIndex = attributeMatrixCombo->findText(amName);
    attributeMatrixCombo->setCurrentIndex(amIndex);
    FilterPararameterWidgetUtils::PopulateAttributeArrayComboBox<DataArraySelectionFilterParameter>(getFilter(), getFilterParameter(), dataContainerCombo, attributeMatrixCombo, attributeArrayCombo, m_DcaProxy);
  }

  if(didBlock) { attributeMatrixCombo->blockSignals(false); didBlock = false; }

  if(!attributeArrayCombo->signalsBlocked()) { didBlock = true; }
  attributeArrayCombo->blockSignals(true);

  if (amIndex < 0)
  {
    attributeArrayCombo->setCurrentIndex(-1);
  }
  else
  {
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(attributeArrayCombo->model());
    if (NULL != model)
    {
      int daIndex = attributeArrayCombo->findText(daName);
      // The DataArray Name was empty, lets instantiate the filter and get the default value and try that
      if (daIndex < 0)
      {
        QVariant var = getFilterParameter()->getDefaultValue();
        DataArrayPath path = var.value<DataArrayPath>();
        daName = path.getDataArrayName(); // Pick up the DataArray Name from a Default instantiation of the filter
        daIndex = attributeArrayCombo->findText(daName);
      }

      QStandardItem* item = model->item(daIndex);
      if (NULL != item)
      {
        if (item->isEnabled())
        {
          attributeArrayCombo->setCurrentIndex(daIndex); // we set the selection but we are NOT triggering anything so we should
        }
      }
    }
  }

  if(didBlock) { attributeArrayCombo->blockSignals(false); didBlock = false; }// not be triggering an infinte recursion of preflights
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataArraySelectionWidget::checkStringValues(QString curDcName, QString filtDcName)
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
void DataArraySelectionWidget::on_dataContainerCombo_currentIndexChanged(int index)
{
  FilterPararameterWidgetUtils::PopulateAttributeMatrixComboBox<DataArraySelectionFilterParameter>(getFilter(), getFilterParameter(), dataContainerCombo, attributeMatrixCombo, m_DcaProxy);

  // Do not select an attribute matrix from the list
  if (attributeMatrixCombo->count() > 0)
  {
    attributeMatrixCombo->setCurrentIndex(-1);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArraySelectionWidget::on_attributeMatrixCombo_currentIndexChanged(int index)
{
  FilterPararameterWidgetUtils::PopulateAttributeArrayComboBox<DataArraySelectionFilterParameter>(getFilter(), getFilterParameter(), dataContainerCombo, attributeMatrixCombo, attributeArrayCombo, m_DcaProxy);

  // Do not select an attribute array from the list
  if (attributeArrayCombo->count() > 0)
  {
    attributeArrayCombo->setCurrentIndex(-1);
  }

  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArraySelectionWidget::setSelectedPath(QString dcName, QString attrMatName, QString attrArrName)
{
  //dataContainerCombo->blockSignals(true);
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
  //dataContainerCombo->blockSignals(false);

  //attributeMatrixCombo->blockSignals(true);
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
  //attributeMatrixCombo->blockSignals(false);

  attributeArrayCombo->blockSignals(true);
  // Set the correct Data Array (if Possible). It may have changed since we started
  count = attributeArrayCombo->count();
  for(int i = 0; i < count; i++)
  {
    if (attributeArrayCombo->itemText(i).compare(attrArrName) == 0 )
    {
      attributeArrayCombo->setCurrentIndex(i);
      break;
    }
  }
  attributeArrayCombo->blockSignals(false);
}


// -----------------------------------------------------------------------------
//blockSignals(false);
// -----------------------------------------------------------------------------
void DataArraySelectionWidget::on_attributeArrayCombo_currentIndexChanged(int index)
{
  //qDebug() << getFilter()->getHumanLabel() << "  " << getFilterParameter()->getHumanLabel() << " DataArraySelectionWidget::on_attributeArrayCombo_currentIndexChanged(int index)";
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArraySelectionWidget::beforePreflight()
{
  if (NULL == getFilter()) { return; }
  if(m_DidCausePreflight == true)
  {
    // std::cout << "***  DataArraySelectionWidget already caused a preflight, just returning" << std::endl;
    return;
  }

  dataContainerCombo->blockSignals(true);
  attributeMatrixCombo->blockSignals(true);
  attributeArrayCombo->blockSignals(true);

  populateComboBoxes();

  dataContainerCombo->blockSignals(false);
  attributeMatrixCombo->blockSignals(false);
  attributeArrayCombo->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArraySelectionWidget::afterPreflight()
{
  // std::cout << "After Preflight" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy DataArraySelectionWidget::generateDCAProxy()
{
  // This will only work for a single selection
  DataContainerArrayProxy dcaProxy;
  QString dcaName = dataContainerCombo->currentText();
  DataContainerProxy dcProxy(dcaName, true);

  QString amName = attributeMatrixCombo->currentText();
  AttributeMatrixProxy amProxy(amName, true);

  QString daName = attributeArrayCombo->currentText();
  DataArrayProxy daProxy(dcaName + "|" + amName, daName, true);
  amProxy.dataArrays.insert(daName, daProxy);
  dcProxy.attributeMatricies.insert(amName, amProxy);
  dcaProxy.dataContainers.insert(dcProxy.name, dcProxy);

  return dcaProxy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArraySelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  // Generate the path to the AttributeArray
  QString dc = dataContainerCombo->currentText();
  QString am = attributeMatrixCombo->currentText();
  QString da = attributeArrayCombo->currentText();
 // qDebug() << "++++++++++++++++++++++++++++++++++++++++++++";
 // qDebug() << getFilterParameter()->getHumanLabel() << ":" << dc << "/" << am << "/" << da << "   m_DidCausePreflight:" << (int)(m_DidCausePreflight) << " " << (int)(attributeArrayCombo->signalsBlocked());
 // qDebug() << "++++++++++++++++++++++++++++++++++++++++++++";
  DataArrayPath path(dc, am, da);
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

