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

#include "DataBundleSelectionWidget.h"

#include <QtCore/QMetaProperty>
#include <QtCore/QList>
#include <QtWidgets/QListWidgetItem>

#include "DREAM3DLib/DataContainers/DataArrayPath.h"
#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"

#define DATA_CONTAINER_LEVEL 0
#define ATTRIBUTE_MATRIX_LEVEL 1
#define ATTRIBUTE_ARRAY_LEVEL 2
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataBundleSelectionWidget::DataBundleSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent),
  m_DidCausePreflight(false)
{
  m_FilterParameter = dynamic_cast<DataBundleSelectionFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != NULL, "NULL Pointer", "DataBundleSelectionWidget can ONLY be used with a DataBundleSelectionFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataBundleSelectionWidget::DataBundleSelectionWidget(QWidget* parent) :
  FilterParameterWidget(NULL, NULL, parent),
  m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataBundleSelectionWidget::~DataBundleSelectionWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataBundleSelectionWidget::initializeWidget(FilterParameter* parameter, AbstractFilter* filter)
{
  setFilter(filter);
  setFilterParameter(parameter);
  setupGui();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataBundleSelectionWidget::setupGui()
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

  dataBundleList->blockSignals(true);

  dataBundleList->clear();

  // Now let the gui send signals like normal
  dataBundleList->blockSignals(false);


  populateComboBoxes();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataBundleSelectionWidget::populateComboBoxes()
{
  //  std::cout << "void DataBundleSelectionWidget::populateComboBoxesWithSelection()" << std::endl;


  // Now get the DataContainerArray from the Filter instance
  // We are going to use this to get all the current DataContainers
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(NULL == dca.get()) { return; }

  // Check to see if we have any DataContainers to actually populate drop downs with.
  if(dca->getDataContainers().size() == 0)
  {
    return;
  }
  QMap<QString, IDataContainerBundle::Pointer> dcb = dca->getDataContainerBundles();


  // Check to see if we have any DataBundles to actually populate drop downs with.
  if(dcb.size() == 0)
  {
    return;
  }

  // Grab what is currently selected
  QString curDcName = dataBundleList->currentText();

  QList<QString> listOfBundles = dcb.keys();


  // Populate the  Combo Box with all the DataContainer Bundles

  QListIterator<QString> iter(listOfBundles);

  while(iter.hasNext())
  {
    QString dc = iter.next();
    if(dataBundleList->findText(dc) == -1 )
    {
      dataBundleList->addItem(dc);
    }
  }

  //remove items in the combo that are NOT in the Data Container Array
  int count = dataBundleList->count();
  for(int i = count - 1; i >= 0; i--)
  {
    QString str0 = dataBundleList->itemText(i);
    iter.toFront();
    bool boo = false;
    while(iter.hasNext() )
    {
      QString dc = iter.next();
      if(dc.compare(str0) == 0)
      {
        boo = true; // found in the list
      }
    }
    if(boo == false)
    {
      dataBundleList->removeItem(i);
    }
  }

  bool isInList = false;
  for (int i = 0; i < count; ++i)
  {
    QString str0 = dataBundleList->itemText(i);
    if(curDcName.compare(str0) == 0)
    {
      isInList = true;
      dataBundleList->setCurrentIndex(i);
    }
  }
  if(isInList == false)
  {
    dataBundleList->setCurrentIndex(-1);
  }

#if 0
  // Grab what is currently selected
  QString curDcName = dataBundleList->currentText();


  // Get what is in the filter
  QString filtDcName;
  QString filtAmName;

  QVariant qvSelectedPath = getFilter()->property(PROPERTY_NAME_AS_CHAR);
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

  if (!dataBundleList->signalsBlocked()) { didBlock = true; }
  dataBundleList->blockSignals(true);
  int dcIndex = dataBundleList->findText(dcName);
  if(dcIndex < 0 && dcName.isEmpty() == false)
  {
    dataBundleList->addItem(dcName);
    //qDebug() << "[2] Adding " << dcName;
  } // the string was not found so just set it to the first index
  else
  {
    if(dcIndex < 0) { dcIndex = 0; } // Just set it to the first DataContainer in the list
    dataBundleList->setCurrentIndex(dcIndex);
  }
  if(didBlock) { dataBundleList->blockSignals(false); didBlock = false; }
#endif

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataBundleSelectionWidget::checkStringValues(QString curDcName, QString filtDcName)
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
void DataBundleSelectionWidget::on_dataBundleList_currentIndexChanged(int index)
{
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataBundleSelectionWidget::beforePreflight()
{
  if (NULL == getFilter()) { return; }
  if(m_DidCausePreflight == true)
  {
    //std::cout << "***  DataBundleSelectionWidget already caused a preflight, just returning" << std::endl;
    return;
  }

  dataBundleList->blockSignals(true);
  // Reset all the combo box widgets to have the default selection of the first index in the list
  populateComboBoxes();
  dataBundleList->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataBundleSelectionWidget::afterPreflight()
{
  // std::cout << "After Preflight" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataBundleSelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  // Generate the path to the AttributeArray
  //DataArrayPath path(dataBundleList->currentText(), attributeMatrixList->currentText(), attributeArrayList->currentText());
  QVariant var(dataBundleList->currentText() );
  // var.setValue(path);
  bool ok = false;
  // Set the value into the Filter
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, var);
  if(false == ok)
  {
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(getFilter(), getFilterParameter());
  }

}
