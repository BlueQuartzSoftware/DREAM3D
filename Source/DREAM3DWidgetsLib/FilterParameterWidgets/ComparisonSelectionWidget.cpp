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
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ComparisonSelectionWidget.h"


#include "FilterParameterWidgetsDialogs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonSelectionWidget::ComparisonSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  QWidget(parent),
  m_ShowOperators(true),
  m_Filter(filter),
  m_FilterParameter(parameter),
  m_DidCausePreflight(false),
  m_ComparisonSelectionTableModel(NULL)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonSelectionWidget::~ComparisonSelectionWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonInputs ComparisonSelectionWidget::getComparisonInputs()
{
  ComparisonInputs comps;
  if (m_ComparisonSelectionTableModel == NULL) { return comps; }

  int filterCount = m_ComparisonSelectionTableModel->rowCount();
  QVector<QString> featureNames;
  QVector<float> featureValues;
  QVector<int> featureOperators;
  m_ComparisonSelectionTableModel->getTableData(featureNames, featureValues, featureOperators);

  for(int i = 0; i < filterCount; ++i)
  {
    ComparisonInput_t comp;
    comp.dataContainerName = dataContainerList->currentText();
    comp.attributeMatrixName = attributeMatrixList->currentText();
    comp.attributeArrayName = featureNames[i];
    comp.compOperator = featureOperators[i];
    comp.compValue = featureValues[i];
    comps.addInput(comp);
  }
  return comps;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::setupGui()
{


  if (m_FilterParameter == NULL)
  {
    return;
  }

  m_ComparisonSelectionTableModel = new ComparisonSelectionTableModel(m_ShowOperators);
  QAbstractItemModel* model = m_ComparisonSelectionTableView->model();
  if(NULL != model)
  {
    delete model;
  }
  m_ComparisonSelectionTableView->setModel(m_ComparisonSelectionTableModel);
  m_ComparisonSelectionTableModel->setNumberOfPhases(1);

  // Set the ItemDelegate for the table.
  QAbstractItemDelegate* aid = m_ComparisonSelectionTableModel->getItemDelegate();
  m_ComparisonSelectionTableView->setItemDelegate(aid);


//  dataContainerList->blockSignals(true);
//  attributeMatrixList->blockSignals(true);

  dataContainerList->clear();
  attributeMatrixList->clear();

  // Now let the gui send signals like normal
//  dataContainerList->blockSignals(false);
//  attributeMatrixList->blockSignals(false);

  //Block the signals as we do not
  populateComboBoxes();

  // Now connect all the signals and slots
  connect(m_ComparisonSelectionTableModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex)),
          this, SLOT(tableDataWasChanged(const QModelIndex&, const QModelIndex&)));

  // Catch when the filter is about to execute the preflight
  connect(m_Filter, SIGNAL(preflightAboutToExecute()),
          this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(m_Filter, SIGNAL(preflightExecuted()),
          this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(m_Filter, SIGNAL(updateFilterParameters(AbstractFilter*)),
          this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

#if 0
  // is the filter parameter tied to a boolean property of the Filter Instance, if it is then we need to make the check box visible
  if(m_FilterParameter->isConditional() == true)
  {
    bool boolProp = m_Filter->property(m_FilterParameter->getConditionalProperty().toLatin1().constData() ).toBool();
    conditionalCB->setChecked(boolProp);
    conditionalCB->setText(m_FilterParameter->getConditionalLabel());
    dataContainerList->setEnabled(boolProp);
    attributeMatrixList->setEnabled(boolProp);
    attributeArrayList->setEnabled(boolProp);
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
#endif

}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::on_conditionalCB_stateChanged(int state)
{
  bool boolProp = conditionalCB->isChecked();
  dataContainerList->setEnabled(boolProp);
  dataContainerList->setVisible(boolProp);
  attributeMatrixList->setEnabled(boolProp);
  attributeMatrixList->setVisible(boolProp);
  attributeArrayList->setEnabled(boolProp);
  attributeArrayList->setVisible(boolProp);

  label->setVisible(boolProp);
  linkLeft->setVisible(boolProp);
  linkRight->setVisible(boolProp);
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;

}
#endif


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::populateComboBoxes()
{
  //  std::cout << "void SingleArraySelectionWidget::populateComboBoxesWithSelection()" << std::endl;


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
    if(dataContainerList->findText(dc.name) == -1 )
    {
      dataContainerList->addItem(dc.name);
    }
  }

  // Grab what is currently selected
  QString curDcName = dataContainerList->currentText();
  QString curAmName = attributeMatrixList->currentText();
  //  QString curDaName = attributeArrayList->currentText();

  // Get what is in the filter
  ComparisonInputs comps = m_Filter->property(PROPERTY_NAME_AS_CHAR).value<ComparisonInputs>();
  // Split the path up to make sure we have a valid path separated by the "|" character
  ComparisonInput_t comp;
  if(comps.size() > 0)
  {
    comp = comps[0]; // Get the first threshold value;
  }
  QString filtDcName = comp.dataContainerName;
  QString filtAmName = comp.attributeMatrixName;
  QString filtDaName = comp.attributeArrayName;
  //QStringList tokens = selectedPath.split(DREAM3D::PathSep);

  // Now to figure out which one of these to use. If this is the first time through then what we picked up from the
  // gui will be empty strings because nothing is there. If there is something in the filter then we should use that.
  // If there is something in both of them and they are NOT equal then we have a problem. Use the flag m_DidCausePreflight
  // to determine if the change from the GUI should over ride the filter or vice versa. there is a potential that in future
  // versions that something else is driving DREAM3D and pushing the changes to the filter and we need to reflect those
  // changes in the GUI, like a testing script?

  QString dcName = checkStringValues(curDcName, filtDcName);
  QString amName = checkStringValues(curAmName, filtAmName);
  // QString daName = checkStringValues(curDaName, filtDaName);

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
    // Set the selected index in the Attribute Matrix
    attributeMatrixList->setCurrentIndex(amIndex);
    // Now based on that AttributeMatrix get a list the AttributeArrays
    QStringList possibleArrays = generateAttributeArrayList();
    // Push that list into the Table Model
    m_ComparisonSelectionTableModel->setPossibleFeatures(possibleArrays);
    // Now that we have an updated list of names we need to Set the ItemDelegate for the table.
    QAbstractItemDelegate* aid = m_ComparisonSelectionTableModel->getItemDelegate();
    m_ComparisonSelectionTableView->setItemDelegate(aid);

    // Now we need to actually get the selections from the filter instance
    // which we have from above

    int count = comps.size();
    // Create our inputs to directly set the table model
    QVector<QString> featureNames(count);
    QVector<float> featureValues(count);
    QVector<int> featureOperators(count);
    for (int i = 0; i < count; i++)
    {
      comp = comps[i];
      featureNames[i] = comp.attributeArrayName;
      featureValues[i] = comp.compValue;
      featureOperators[i] = comp.compOperator;
    }
    // Now set the table data directly.
    m_ComparisonSelectionTableModel->setTableData(featureNames, featureValues, featureOperators);
  }
  if(didBlock) { attributeMatrixList->blockSignals(false); didBlock = false; }


}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::populateAttributeMatrixList()
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
QStringList ComparisonSelectionWidget::generateAttributeArrayList()
{

  QStringList attributeArrayList;
  // Get the selected Data Container Name from the DataContainerList Widget
  QString currentDCName = dataContainerList->currentText();
  QString currentAttrMatName = attributeMatrixList->currentText();

  // Loop over the data containers until we find the proper data container
  QList<DataContainerProxy> containers = m_DcaProxy.list;
  QListIterator<DataContainerProxy> containerIter(containers);
  while(containerIter.hasNext())
  {
    DataContainerProxy dc = containerIter.next();
    if(dc.name.compare(currentDCName) == 0 )
    {
      // We found the proper Data Container, now populate the AttributeMatrix List
      QMap<QString, AttributeMatrixProxy> attrMats = dc.attributeMatricies;
      QMapIterator<QString, AttributeMatrixProxy> attrMatsIter(attrMats);
      while(attrMatsIter.hasNext() )
      {
        attrMatsIter.next();
        QString amName = attrMatsIter.key();
        if(amName.compare(currentAttrMatName) == 0 )
        {

          // We found the selected AttributeMatrix, so loop over this attribute matrix arrays and populate the list widget
          AttributeMatrixProxy amProxy = attrMatsIter.value();
          QMap<QString, DataArrayProxy> dataArrays = amProxy.dataArrays;
          QMapIterator<QString, DataArrayProxy> dataArraysIter(dataArrays);
          while(dataArraysIter.hasNext() )
          {
            dataArraysIter.next();
            //DataArrayProxy daProxy = dataArraysIter.value();
            QString daName = dataArraysIter.key();
            attributeArrayList << daName;
          }
        }
      }
    }
  }

  return attributeArrayList;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ComparisonSelectionWidget::checkStringValues(QString curDcName, QString filtDcName)
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
void ComparisonSelectionWidget::tableDataWasChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::on_addComparison_clicked()
{
  if (!m_ComparisonSelectionTableModel->insertRow(m_ComparisonSelectionTableModel->rowCount())) { return; }

  QModelIndex index = m_ComparisonSelectionTableModel->index(m_ComparisonSelectionTableModel->rowCount() - 1, 0);
  m_ComparisonSelectionTableView->setCurrentIndex(index);
  m_ComparisonSelectionTableView->resizeColumnsToContents();
  m_ComparisonSelectionTableView->scrollToBottom();
  m_ComparisonSelectionTableView->setFocus();
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::on_removeComparison_clicked()
{
  //qDebug() << "on_removeComparisonSelection_clicked" << "\n";
  QItemSelectionModel* selectionModel = m_ComparisonSelectionTableView->selectionModel();
  if (!selectionModel->hasSelection()) { return; }
  QModelIndex index = selectionModel->currentIndex();
  if (!index.isValid()) { return; }
  m_ComparisonSelectionTableModel->removeRow(index.row(), index.parent());
  if (m_ComparisonSelectionTableModel->rowCount() > 0)
  {
    m_ComparisonSelectionTableView->resizeColumnsToContents();
  }
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::on_dataContainerList_currentIndexChanged(int index)
{

  //  std::cout << "void SingleArraySelectionWidget::on_dataContainerList_currentIndexChanged(int index)" << std::endl;
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
void ComparisonSelectionWidget::on_attributeMatrixList_currentIndexChanged(int index)
{
  QStringList possibleArrays = generateAttributeArrayList();
  m_ComparisonSelectionTableModel->setPossibleFeatures(possibleArrays);
  // Set the ItemDelegate for the table.
  QAbstractItemDelegate* aid = m_ComparisonSelectionTableModel->getItemDelegate();
  m_ComparisonSelectionTableView->setItemDelegate(aid);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::setComparisons(QVector<ComparisonInput_t> comparisons)
{
  qint32 count = comparisons.size();

  QVector<QString> arrayNames(count);
  QVector<int>   compOperators(count);
  QVector<float> compValues(count);
  //bool ok = false;
  for(int i = 0; i < count; ++i)
  {
    arrayNames[i] = (comparisons[i].attributeArrayName);
    compOperators[i] = comparisons[i].compOperator;
    compValues[i] = comparisons[i].compValue;
  }
  m_ComparisonSelectionTableModel->setTableData(arrayNames, compValues, compOperators);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  // qDebug() << "DataContainerArrayProxyWidget::filterNeedsInputParameters(AbstractFilter* filter)";

  QVariant var;
  ComparisonInputs comps = getComparisonInputs();
  var.setValue(comps);
  bool ok = false;
  // Set the value into the Filter
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, var);
  if(false == ok)
  {
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(m_Filter, m_FilterParameter);
  }

//  if(m_FilterParameter->isConditional() )
//  {
//    var.setValue(conditionalCB->isChecked());
//    ok = filter->setProperty(m_FilterParameter->getConditionalProperty().toLatin1().constData(), var);
//    if(false == ok)
//    {
//      FilterParameterWidgetsDialogs::ShowCouldNotSetConditionalFilterParameter(m_Filter, m_FilterParameter);
//    }
//  }

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::beforePreflight()
{
  if (NULL == m_Filter) { return; }
  if(m_DidCausePreflight == true)
  {
    std::cout << "***  ComparisonSelectionWidget already caused a preflight, just returning" << std::endl;
    return;
  }

  dataContainerList->blockSignals(true);
  attributeMatrixList->blockSignals(true);
  m_ComparisonSelectionTableModel->blockSignals(true);
  // Reset all the combo box widgets to have the default selection of the first index in the list
  populateComboBoxes();
  m_ComparisonSelectionTableModel->blockSignals(false);
  dataContainerList->blockSignals(false);
  attributeMatrixList->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::afterPreflight()
{
  // qDebug() << m_Filter->getNameOfClass() << " DataContainerArrayProxyWidget::afterPreflight()";
}

