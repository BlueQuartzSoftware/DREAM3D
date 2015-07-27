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
#include "MultiDataArraySelectionWidget.h"

#include <QtCore/QMetaProperty>
#include <QtCore/QList>

#include <QtGui/QStandardItemModel>

#include <QtWidgets/QListWidgetItem>


#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/FilterParameters/MultiDataArraySelectionFilterParameter.h"
#include "DREAM3DLib/DataContainers/DataArrayPath.h"
#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"

#define DATA_CONTAINER_LEVEL 0
#define ATTRIBUTE_MATRIX_LEVEL 1
#define ATTRIBUTE_ARRAY_LEVEL 2
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
  dataContainerList->blockSignals(true);
  attributeMatrixList->blockSignals(true);
  attributeArraysWidget->blockSignals(true);

  dataContainerList->clear();
  attributeMatrixList->clear();
  attributeArraysWidget->clear();

  dataContainerList->blockSignals(false);
  attributeMatrixList->blockSignals(false);
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
    dataContainerList->clear();
    attributeMatrixList->clear();
    return;
  }

  // Grab what is currently selected
  QString curDcName = dataContainerList->currentText();
  QString curAmName = attributeMatrixList->currentText();

  // Cache the DataContainerArray Structure for our use during all the selections
  m_DcaProxy = DataContainerArrayProxy(dca.get());

  // Populate the DataContainerArray Combo Box with all the DataContainers
  QList<DataContainerProxy> dcList = m_DcaProxy.dataContainers.values();
  QListIterator<DataContainerProxy> iter(dcList);
  QVector<unsigned int> defVec = m_FilterParameter->getDefaultGeometryTypes();
  while (iter.hasNext())
  {
    DataContainerProxy dcProxy = iter.next();
    if (dataContainerList->findText(dcProxy.name) == -1)
    {
      int index = dataContainerList->currentIndex();
      dataContainerList->addItem(dcProxy.name);
      dataContainerList->setCurrentIndex(index);
    }
    DataContainer::Pointer dc = dca->getDataContainer(dcProxy.name);

    if (NULL != dc.get() && defVec.isEmpty() == false && defVec.contains(dc->getGeometry()->getGeometryType()) == false)
    {
      QStandardItemModel* model = qobject_cast<QStandardItemModel*>(dataContainerList->model());
      if (NULL != model)
      {
        QStandardItem* item = model->item(dataContainerList->findText(dcProxy.name));
        if (NULL != item)
        {
          item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
        }
      }
    }
  }

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

  if (!dataContainerList->signalsBlocked()) { didBlock = true; }
  dataContainerList->blockSignals(true);

  int dcIndex = dataContainerList->findText(dcName);
  dataContainerList->setCurrentIndex(dcIndex);

  populateAttributeMatrixList();

  if (didBlock) { dataContainerList->blockSignals(false); didBlock = false; }

  if (!attributeMatrixList->signalsBlocked()) { didBlock = true; }
  attributeMatrixList->blockSignals(true);

  if (dcIndex < 0)
  {
    attributeMatrixList->setCurrentIndex(-1);
    attributeArraysWidget->clear();
  }
  else
  {
    int amIndex = attributeMatrixList->findText(amName);
    attributeMatrixList->setCurrentIndex(amIndex);
    populateAttributeArrayList(selectedPaths);
  }

  if (didBlock) { attributeMatrixList->blockSignals(false); didBlock = false; }

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
void MultiDataArraySelectionWidget::populateAttributeMatrixList()
{
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if (NULL == dca.get()) { return; }

  QString dcName = dataContainerList->currentText();

  // Clear the AttributeMatrix List
  attributeMatrixList->blockSignals(true);
  attributeMatrixList->clear();

  // Loop over the data containers until we find the proper data container
  QList<DataContainerProxy> containers = m_DcaProxy.dataContainers.values();
  QListIterator<DataContainerProxy> containerIter(containers);
  QVector<unsigned int> defVec = m_FilterParameter->getDefaultAttributeMatrixTypes();
  while (containerIter.hasNext())
  {
    DataContainerProxy dc = containerIter.next();

    if (dc.name.compare(dcName) == 0)
    {
      // We found the proper Data Container, now populate the AttributeMatrix List
      QMap<QString, AttributeMatrixProxy> attrMats = dc.attributeMatricies;
      QMapIterator<QString, AttributeMatrixProxy> attrMatsIter(attrMats);
      while (attrMatsIter.hasNext())
      {
        attrMatsIter.next();
        QString amName = attrMatsIter.key();
        AttributeMatrix::Pointer am = dca->getAttributeMatrix(DataArrayPath(dc.name, amName, ""));
        attributeMatrixList->addItem(amName);

        if (NULL != am.get() && defVec.isEmpty() == false && defVec.contains(am->getType()) == false)
        {
          QStandardItemModel* model = qobject_cast<QStandardItemModel*>(attributeMatrixList->model());
          if (NULL != model)
          {
            QStandardItem* item = model->item(attributeMatrixList->findText(amName));
            if (NULL != item)
            {
              item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
            }
          }
        }
      }
    }
  }

  attributeMatrixList->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::on_dataContainerList_currentIndexChanged(int index)
{
  populateAttributeMatrixList();

  // Do not select an attribute matrix from the list
  if (attributeMatrixList->count() > 0)
  {
    attributeMatrixList->setCurrentIndex(-1);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::on_attributeMatrixList_currentIndexChanged(int index)
{
  QVector<DataArrayPath> selectedPaths = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<QVector<DataArrayPath> >();

  populateAttributeArrayList(selectedPaths);
  updateSelectAllCheckbox();

  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::populateAttributeArrayList(QVector<DataArrayPath> selectedPaths)
{
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if (NULL == dca.get()) { return; }

  attributeArraysWidget->blockSignals(true);
  attributeArraysWidget->clear();

  // Get the selected Data Container Name from the DataContainerList Widget
  QString currentDCName = dataContainerList->currentText();
  QString currentAttrMatName = attributeMatrixList->currentText();

  // Loop over the data containers until we find the proper data container
  QList<DataContainerProxy> containers = m_DcaProxy.dataContainers.values();
  QListIterator<DataContainerProxy> containerIter(containers);
  QVector<QString> daTypes = m_FilterParameter->getDefaultAttributeArrayTypes();
  QVector< QVector<size_t> > cDims = m_FilterParameter->getDefaultComponentDimensions();
  while (containerIter.hasNext())
  {
    DataContainerProxy dc = containerIter.next();
    if (dc.name.compare(currentDCName) == 0)
    {
      // We found the proper Data Container, now populate the AttributeMatrix List
      QMap<QString, AttributeMatrixProxy> attrMats = dc.attributeMatricies;
      QMapIterator<QString, AttributeMatrixProxy> attrMatsIter(attrMats);
      while (attrMatsIter.hasNext())
      {
        attrMatsIter.next();
        QString amName = attrMatsIter.key();
        if (amName.compare(currentAttrMatName) == 0)
        {
          // Clear the list of arrays from the QListWidget
          attributeArraysWidget->clear();
          // We found the selected AttributeMatrix, so loop over this attribute matrix arrays and populate the list widget
          AttributeMatrixProxy amProxy = attrMatsIter.value();
          QMap<QString, DataArrayProxy> dataArrays = amProxy.dataArrays;
          QMapIterator<QString, DataArrayProxy> dataArraysIter(dataArrays);
          while (dataArraysIter.hasNext())
          {
            dataArraysIter.next();
            QString daName = dataArraysIter.key();
            QListWidgetItem* daItem = new QListWidgetItem(daName);
            daItem->setCheckState(Qt::Unchecked);

            for (int i = 0; i < selectedPaths.size(); i++)
            {
              if (selectedPaths.at(i).getDataArrayName() == daName)
              {
                daItem->setCheckState(Qt::Checked);
              }
            }

            IDataArray::Pointer da = dca->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(NULL, DataArrayPath(dc.name, amProxy.name, daName));
            attributeArraysWidget->addItem(daItem);

            if (NULL != da.get() && ((daTypes.isEmpty() == false && daTypes.contains(da->getTypeAsString()) == false) || (cDims.isEmpty() == false && cDims.contains(da->getComponentDimensions()) == false)))
            {
              QList<QListWidgetItem*> rejectList = attributeArraysWidget->findItems(daName, Qt::MatchRecursive);
              for (int i = 0; i < rejectList.size(); i++)
              {
                QListWidgetItem* item = rejectList[i];
                item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
              }
            }
          }
        }
      }
    }
  }

  attributeArraysWidget->blockSignals(false);
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
    std::cout << "***  MultiDataArraySelectionWidget already caused a preflight, just returning" << std::endl;
    return;
  }

  dataContainerList->blockSignals(true);
  attributeMatrixList->blockSignals(true);
  attributeArraysWidget->blockSignals(true);
  // Reset all the combo box widgets to have the default selection of the first index in the list
  populateComboBoxes();
  dataContainerList->blockSignals(false);
  attributeMatrixList->blockSignals(false);
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
  QString dcaName = dataContainerList->currentText();
  DataContainerProxy dcProxy(dcaName, true);

  QString amName = attributeMatrixList->currentText();
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

  return DataContainerArrayProxy();

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
      DataArrayPath path(dataContainerList->currentText(), attributeMatrixList->currentText(), attributeArraysWidget->item(i)->text());
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
    dataContainerList->setDisabled(true);
    attributeMatrixList->setDisabled(true);
    selectCheckBox->blockSignals(false);
  }
  else if (checkedStateCount == attributeArraysWidget->count())
  {
    selectCheckBox->blockSignals(true);
    selectCheckBox->setCheckState(Qt::Checked);
    dataContainerList->setDisabled(true);
    attributeMatrixList->setDisabled(true);
    selectCheckBox->blockSignals(false);
  }
  else
  {
    selectCheckBox->blockSignals(true);
    selectCheckBox->setCheckState(Qt::Unchecked);
    dataContainerList->setEnabled(true);
    attributeMatrixList->setEnabled(true);
    selectCheckBox->blockSignals(false);
  }
}






