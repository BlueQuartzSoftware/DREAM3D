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
#include "SingleArraySelectionWidget.h"

#include <QtCore/QMetaProperty>


#include "DREAM3DWidgetsLib/FilterParameterWidgets/moc_SingleArraySelectionWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SingleArraySelectionWidget::SingleArraySelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  QWidget(parent),
  m_Filter(filter),
  m_FilterParameter(parameter)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SingleArraySelectionWidget::~SingleArraySelectionWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SingleArraySelectionWidget::setupGui()
{

  connect(m_Filter, SIGNAL(preflightAboutToExecute()),
          this, SLOT(beforePreflight() ) );

  connect(m_Filter, SIGNAL(preflightExecuted()),
          this, SLOT(afterPreflight()) );


  if (m_FilterParameter != NULL)
  {
    label->setText(m_FilterParameter->getHumanLabel() );
    beforePreflight();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SingleArraySelectionWidget::on_dataContainerList_currentIndexChanged(int index)
{
  if(index < 0)
  {
    return;
  }
  std::cout << "void SingleArraySelectionWidget::on_dataContainerList_currentIndexChanged(int index)" << std::endl;

  // Clear the AttributeMatrix List
  attributeMatrixList->clear();

  // Loop over the data containers until we find the proper data container
  DataContainerArray::Pointer dca = m_Filter->getDataContainerArray();
  QList<DataContainer::Pointer> containers = dca->getDataContainerArray();
  QListIterator<DataContainer::Pointer> containerIter(containers);

  QString dcName = dataContainerList->itemText(index);
  while(containerIter.hasNext())
  {
    DataContainer::Pointer dc = containerIter.next();
    if(dc->getName().compare(dcName) == 0 )
    {
      // We found the proper Data Container, now populate the AttributeMatrix List
      QMap<QString, AttributeMatrix::Pointer>& attrMats = dc->getAttributeMatrices();
      QMapIterator<QString, AttributeMatrix::Pointer> attrMatsIter(attrMats);
      while(attrMatsIter.hasNext() )
      {
        attrMatsIter.next();
        QString amName = attrMatsIter.key();
        attributeMatrixList->addItem(amName);
      }
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SingleArraySelectionWidget::on_attributeMatrixList_currentIndexChanged(int index)
{
  if(index < 0)
  {
    return;
  }
  std::cout << "void SingleArraySelectionWidget::on_attributeMatrixList_currentIndexChanged(int index)" << std::endl;

  // Clear the AttributeMatrix List
  attributeArrayList->clear();

  // Loop over the data containers until we find the proper data container
  DataContainerArray::Pointer dca = m_Filter->getDataContainerArray();

  DataContainer::Pointer dc = dca->getDataContainer(dataContainerList->currentText() );
  AttributeMatrix::Pointer attrMat = dc->getAttributeMatrix(attributeMatrixList->itemText(index) );

  // Get the list of AttributeArrays from the current AttributeMatrix
  // and add them to the combobox
  QList<QString> daNames = attrMat->getAttributeArrayNameList();
  QListIterator<QString> daIter(daNames);
  while(daIter.hasNext())
  {
    QString daName = daIter.next();
    attributeArrayList->addItem(daName);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SingleArraySelectionWidget::on_attributeArrayList_currentIndexChanged(int index)
{
  std::cout << "void SingleArraySelectionWidget::on_attributeArrayList_currentIndexChanged(int index)" << std::endl;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SingleArraySelectionWidget::initializeHeirarchy()
{
  std::cout << "SingleArraySelectionWidget::initializeHeirarchy()" << std::endl;
  dataContainerList->clear();
  attributeMatrixList->clear();
  attributeArrayList->clear();

  if (NULL == m_Filter) { return; }

  dataContainerList->blockSignals(true);// We don't want any signals being emitted during the rebuilding
  DataContainerArray::Pointer dca = m_Filter->getDataContainerArray();
  QList<DataContainer::Pointer> containers = dca->getDataContainerArray();
  QListIterator<DataContainer::Pointer> containerIter(containers);
  while(containerIter.hasNext() )
  {
    DataContainer::Pointer dc = containerIter.next();
    dataContainerList->addItem(dc->getName() );
  }
  dataContainerList->blockSignals(false); // Turn the signals back on

  // Populate the attribute matrix list if only a single Data Container
  if(dataContainerList->count() == 1)
  {
    attributeMatrixList->blockSignals(true);
    on_dataContainerList_currentIndexChanged(0);
    attributeMatrixList->blockSignals(false);
  }

  // Populate the attributeArray list if only a single AttributeMatrix is available
  if(attributeMatrixList->count() == 1)
  {
    attributeArrayList->blockSignals(true);
    on_attributeMatrixList_currentIndexChanged(0);
    attributeArrayList->blockSignals(false);
  }

  // Select the AttributeArray if there is only a single one available
  if(attributeArrayList->count() == 1)
  {
    attributeArrayList->blockSignals(true);
    on_attributeArrayList_currentIndexChanged(0);
    attributeArrayList->blockSignals(false);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SingleArraySelectionWidget::beforePreflight()
{
  if (NULL == m_Filter) { return; }
  std::cout << "SingleArraySelectionWidget::beforePreflight()" << std::endl;

  // Now get the DataContainerArray from the Filter instance
  // We are going to use this to get all the current DataContainers
  DataContainerArray::Pointer dca = m_Filter->getDataContainerArray();

  // Set the initial list of DataContainers
  initializeHeirarchy();

  // now get the selection from the filter
  QString path = m_Filter->property(PROPERTY_NAME_AS_CHAR).toString();
  QString dcName;
  QString amName;
  QString daName;

  QStringList tokens = path.split(DREAM3D::PathSep);
  // Save what was selected from the filter
  if( tokens.size() != 3)
  {
    return;
  }


  dcName = tokens.at(0);
  amName = tokens.at(1);
  daName = tokens.at(2);

  // Set the correct DataContainer
  int count = dataContainerList->count();
  dataContainerList->setCurrentIndex(-1);
  for(int i = 0; i < count; i++)
  {
    if (dataContainerList->itemText(i).compare(dcName) == 0 )
    {
      dataContainerList->setCurrentIndex(i); // This will fire the currentItemChanged(...) signal
      break;
    }
  }

  // Set the correct AttributeMatrix
  count = attributeMatrixList->count();
  attributeMatrixList->setCurrentIndex(-1);
  for(int i = 0; i < count; i++)
  {
    if (attributeMatrixList->itemText(i).compare(amName) == 0 )
    {
      attributeMatrixList->setCurrentIndex(i); // This will fire the currentItemChanged(...) signal
      break;
    }
  }

  attributeArrayList->blockSignals(true);
  // Set the correct Data Array (if Possible). It may have changed since we started
  count = attributeArrayList->count();
  attributeArrayList->setCurrentIndex(-1);
  for(int i = 0; i < count; i++)
  {
    if (attributeArrayList->itemText(i).compare(daName) == 0 )
    {
      attributeArrayList->setCurrentIndex(i); // This will fire the currentItemChanged(...) signal
      break;
    }
  }
  attributeArrayList->blockSignals(false);


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SingleArraySelectionWidget::afterPreflight()
{

}
