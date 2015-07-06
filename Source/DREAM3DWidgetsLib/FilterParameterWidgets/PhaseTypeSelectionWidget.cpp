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

#include "PhaseTypeSelectionWidget.h"

#include <QtCore/QMetaProperty>
#include <QtCore/QList>

#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>

#include "DREAM3DLib/Common/PhaseType.h"
#include "DREAM3DLib/DataContainers/DataArrayPath.h"
#include "DREAM3DLib/FilterParameters/PhaseTypeSelectionFilterParameter.h"
#include "DREAM3DLib/Utilities/QMetaObjectUtilities.h"
#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhaseTypeSelectionWidget::PhaseTypeSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent),
  m_DidCausePreflight(false)
{
  m_FilterParameter = dynamic_cast<PhaseTypeSelectionFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != NULL, "NULL Pointer", "PhaseTypeSelectionWidget can ONLY be used with a PhaseTypeSelectionFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhaseTypeSelectionWidget::PhaseTypeSelectionWidget(QWidget* parent) :
  FilterParameterWidget(NULL, NULL, parent),
  m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhaseTypeSelectionWidget::~PhaseTypeSelectionWidget()
{}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::setupGui()
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

  label->setText(m_FilterParameter->getHumanLabel() );

  dataContainerCombo->blockSignals(true);
  attributeMatrixCombo->blockSignals(true);
  dataContainerCombo->clear();
  attributeMatrixCombo->clear();
  // Now let the gui send signals like normal
  dataContainerCombo->blockSignals(false);
  attributeMatrixCombo->blockSignals(false);

  populateComboBoxes();

  updatePhaseComboBoxes();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::populateComboBoxes()
{
  //  std::cout << "void PhaseTypeSelectionWidget::populateComboBoxesWithSelection()" << std::endl;


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

  // Populate the DataContainerArray Combo Box with all the DataContainers
  QList<DataContainerProxy> dcList = m_DcaProxy.dataContainers.values();
  QListIterator<DataContainerProxy> iter(dcList);
  dataContainerCombo->clear();
  while(iter.hasNext() )
  {
    DataContainerProxy dc = iter.next();
    dataContainerCombo->addItem(dc.name);
  }

  // Get what is in the filter
  PhaseTypeSelectionFilterParameter* p = dynamic_cast<PhaseTypeSelectionFilterParameter*>(getFilterParameter());
  QVariant qvSelectedPath = getFilter()->property(p->getAttributeMatrixPathProperty().toLatin1().constData());
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
    if( !dca->doesAttributeMatrixExist(DataArrayPath(dcName, amName, "") ) ) { amName = ""; }
  }

  bool didBlock = false;

  if (!dataContainerCombo->signalsBlocked()) { didBlock = true; }
  dataContainerCombo->blockSignals(true);
  int dcIndex = dataContainerCombo->findText(dcName);

  dataContainerCombo->setCurrentIndex(dcIndex);
  populateAttributeMatrixList();

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
QString PhaseTypeSelectionWidget::checkStringValues(QString curDcName, QString filtDcName)
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
void PhaseTypeSelectionWidget::selectDefaultPath()
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
void PhaseTypeSelectionWidget::setSelectedPath(QString dcName, QString attrMatName, QString attrArrName)
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
void PhaseTypeSelectionWidget::on_dataContainerCombo_currentIndexChanged(int index)
{
  populateAttributeMatrixList();

  // Do not select an attribute matrix from the list
  if(attributeMatrixCombo->count() > 0)
  {
    attributeMatrixCombo->setCurrentIndex(-1);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::populateAttributeMatrixList()
{
  QString dcName = dataContainerCombo->currentText();

  // Clear the AttributeMatrix List
  attributeMatrixCombo->blockSignals(true);
  attributeMatrixCombo->clear();

  // Loop over the data containers until we find the proper data container
  QList<DataContainerProxy> containers = m_DcaProxy.dataContainers.values();
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
        attributeMatrixCombo->addItem(amName);
      }
    }
  }

  attributeMatrixCombo->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::on_attributeMatrixCombo_currentIndexChanged(int index)
{
  resetPhaseComboBoxes();
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::updatePhaseComboBoxes()
{
  bool ok = false;
  // setup the list of choices for the widget
  PhaseTypeSelectionFilterParameter* phaseTypes = dynamic_cast<PhaseTypeSelectionFilterParameter*>(getFilterParameter());
  QString countProp = phaseTypes->getPhaseTypeCountProperty();
  int phaseCount = getFilter()->property(countProp.toLatin1().constData()).toInt(&ok);
  QString phaseDataProp = phaseTypes->getPhaseTypeDataProperty();

  UInt32Vector_t vectorWrapper = getFilter()->property(phaseDataProp.toLatin1().constData()).value<UInt32Vector_t>();
  QVector<quint32> dataFromFilter = vectorWrapper.d;
  if(phaseCount < 0 && dataFromFilter.size() < 10)   // there was an issue getting the phase Count from the Filter.
  {
    phaseCount = dataFromFilter.size(); // So lets just use the count from the actual phase data
  }

  // Get our list of predefined enumeration values
  QVector<unsigned int> phaseTypeEnums;
  PhaseType::getPhaseTypeEnums(phaseTypeEnums);

  phaseListWidget->clear();
  // Get our list of Phase Type Strings
  QStringList phaseListChoices = m_FilterParameter->getPhaseListChoices();

  for (int i = 1; i < phaseCount; i++)
  {
    QComboBox* cb = new QComboBox(NULL);
    for (int s = 0; s < phaseListChoices.size(); ++s)
    {
      cb->addItem((phaseListChoices[s]), phaseTypeEnums[s]);
      cb->setItemData(static_cast<int>(s), phaseTypeEnums[s], Qt::UserRole);
    }

    QListWidgetItem* item = new QListWidgetItem(phaseListWidget);
    phaseListWidget->addItem(item);
    phaseListWidget->setItemWidget(item, cb);

    if (i < dataFromFilter.size())
    {
      cb->setCurrentIndex(dataFromFilter[i]);
    }
    connect(cb, SIGNAL(currentIndexChanged(int)),
            this, SLOT(phaseTypeComboBoxChanged(int)) );
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::phaseTypeComboBoxChanged(int index)
{
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::resetPhaseComboBoxes()
{
  int count = phaseListWidget->count();

  for (int i = 0; i < count; ++i)
  {
    QComboBox* cb = qobject_cast<QComboBox*>(phaseListWidget->itemWidget(phaseListWidget->item(i)));
    if (cb) { cb->setCurrentIndex(-1); }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::beforePreflight()
{
  if (NULL == getFilter()) { return; }
  if(m_DidCausePreflight == true)
  {
    return;
  }
  dataContainerCombo->blockSignals(true);
  attributeMatrixCombo->blockSignals(true);

  populateComboBoxes();

  dataContainerCombo->blockSignals(false);
  attributeMatrixCombo->blockSignals(false);


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::afterPreflight()
{
  updatePhaseComboBoxes();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  int count = phaseListWidget->count();
//  {
//    std::cout << "PhaseTypeSelectionWidget::filterNeedsInputParameters Count = " << count << std::endl;
//  }

  PhaseTypeSelectionFilterParameter* p = dynamic_cast<PhaseTypeSelectionFilterParameter*>(getFilterParameter());
  QVariant var;

  // QVector<uint32_t> phaseTypes(count, DREAM3D::PhaseType::UnknownPhaseType);
  QVector<uint32_t> phaseTypes(count + 1, DREAM3D::PhaseType::UnknownPhaseType);
  bool ok = false;
  phaseTypes[0] = DREAM3D::PhaseType::UnknownPhaseType;
  for (int i = 0; i < count; ++i)
  {
    QComboBox* cb = qobject_cast<QComboBox*>(phaseListWidget->itemWidget(phaseListWidget->item(i)));
    unsigned int sType = static_cast<unsigned int>(cb->itemData(cb->currentIndex(), Qt::UserRole).toUInt(&ok));
    //phaseTypes[i+1] = sType;
    phaseTypes[i + 1] = sType;
  }

  UInt32Vector_t data;
  data.d = phaseTypes;
  var.setValue(data);
  ok = false;

  // Set the value into the Filter
  ok = filter->setProperty(p->getPhaseTypeDataProperty().toLatin1().constData(), var);
  if(false == ok)
  {
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(getFilter(), getFilterParameter());
  }

  DataArrayPath path(dataContainerCombo->currentText(), attributeMatrixCombo->currentText(), "");

  var.setValue(path);
  ok = false;
  // Set the value into the Filter
  ok = filter->setProperty(p->getAttributeMatrixPathProperty().toLatin1().constData(), var);
  if(false == ok)
  {
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(getFilter(), getFilterParameter());
  }


}
