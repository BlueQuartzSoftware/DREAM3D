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
#include <QtWidgets/QListWidgetItem>


#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/FilterParameters/FilterParameter.h"
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
	QString units = getFilterParameter()->getUnits();
	if (units.isEmpty() == false)
	{
		label->setText(getFilterParameter()->getHumanLabel() + " (" + units + ")");
	}
	else
	{
		label->setText(getFilterParameter()->getHumanLabel());
	}

	// Get the default path from the Filter instance to cache
	m_DefaultPath = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<DataArrayPath>();
	//  dataContainerList->addItem(m_DefaultPath.getDataContainerName());
	//  attributeMatrixList->addItem(m_DefaultPath.getAttributeMatrixName() );
	//  attributeArrayList->addItem(m_DefaultPath.getDataArrayName() );

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
	//qDebug() << "-----------------------------------------------";
	//qDebug() << getFilter()->getHumanLabel() << "  " << getFilterParameter()->getHumanLabel() << " MultiDataArraySelectionWidget::populateComboBoxes()";
	// Now get the DataContainerArray from the Filter instance
	// We are going to use this to get all the current DataContainers
	DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
	if (NULL == dca.get()) { return; }


	//qDebug() << getFilter()->getHumanLabel() << "  " << getFilterParameter()->getHumanLabel();
	// Grab what is currently selected
	QString curDcName = dataContainerList->currentText();
	QString curAmName = attributeMatrixList->currentText();
	//qDebug() << "Current ComboBox Value: " << curDcName << "::" << curAmName << "::" << curDaName;

	// Check to see if we have any DataContainers to actually populate drop downs with.
	if (dca->getDataContainers().size() == 0)
	{
		dataContainerList->clear();
		attributeMatrixList->clear();
		return;
	}
	// Cache the DataContainerArray Structure for our use during all the selections
	m_DcaProxy = DataContainerArrayProxy(dca.get());

	// Populate the DataContainerArray Combo Box with all the DataContainers
	QList<DataContainerProxy> dcList = m_DcaProxy.list;
	QListIterator<DataContainerProxy> iter(dcList);

	while (iter.hasNext())
	{
		DataContainerProxy dc = iter.next();
		if (dataContainerList->findText(dc.name) == -1)
		{
			dataContainerList->addItem(dc.name);
		}
	}

	// Get what is in the filter  
	DataArrayPathBundle selectedPaths = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<DataArrayPathBundle>();

	// Split the path up to make sure we have a valid path separated by the "|" character
	QString filtDcName = selectedPaths.getDataContainerName();
	QString filtAmName = selectedPaths.getAttributeMatrixName();
	//QString filtDaName = selectedPaths.getDataArrayName();
	QString filtDaName = "";

	QString dcName;
	QString amName;
	QString daName;

	// If EVERYTHING is empty, then try the default value
	if (filtDcName.isEmpty() && filtAmName.isEmpty() && filtDaName.isEmpty()
		&& curDcName.isEmpty() && curAmName.isEmpty())
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
		amName = checkStringValues(curAmName, filtAmName);
	}

	bool didBlock = false;

	if (!dataContainerList->signalsBlocked()) { didBlock = true; }
	dataContainerList->blockSignals(true);
	int dcIndex = dataContainerList->findText(dcName);
	dataContainerList->setCurrentIndex(dcIndex);
	if (dcIndex < 0)
	{
		attributeMatrixList->setCurrentIndex(dcIndex);
	}
	else
	{
		populateAttributeMatrixList();
	}
	if (didBlock) { dataContainerList->blockSignals(false); didBlock = false; }
	if (dcIndex < 0) { return; }


	if (!attributeMatrixList->signalsBlocked()) { didBlock = true; }
	attributeMatrixList->blockSignals(true);
	int amIndex = attributeMatrixList->findText(amName);
	if (amIndex < 0 && amName.isEmpty() == false) { attributeMatrixList->addItem(amName); }
	else
	{
		attributeMatrixList->setCurrentIndex(amIndex);
		populateAttributeArrayList();
	}
	if (didBlock) { attributeMatrixList->blockSignals(false); didBlock = false; }
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
	//qDebug() << getFilter()->getHumanLabel() << "  " << getFilterParameter()->getHumanLabel() << " MultiDataArraySelectionWidget::populateAttributeMatrixList()";

	QString dcName = dataContainerList->currentText();

	// Clear the AttributeMatrix List
	attributeMatrixList->blockSignals(true);
	attributeMatrixList->clear();

	// Loop over the data containers until we find the proper data container
	QList<DataContainerProxy> containers = m_DcaProxy.list;
	QListIterator<DataContainerProxy> containerIter(containers);
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
				attributeMatrixList->addItem(amName);
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

	//qDebug() << getFilter()->getHumanLabel() << "  " << getFilterParameter()->getHumanLabel() << " MultiDataArraySelectionWidget::on_dataContainerList_currentIndexChanged(int index)";
	populateAttributeMatrixList();

	// Select the first AttributeMatrix in the list
	if (attributeMatrixList->count() > 0)
	{
		on_attributeMatrixList_currentIndexChanged(0);
	}

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::on_attributeMatrixList_currentIndexChanged(int index)
{
	populateAttributeArrayList();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::populateAttributeArrayList()
{
	attributeArraysWidget->blockSignals(true);
	attributeArraysWidget->clear();

	// Get the selected Data Container Name from the DataContainerList Widget
	QString currentDCName = dataContainerList->currentText();
	QString currentAttrMatName = attributeMatrixList->currentText();

	// Loop over the data containers until we find the proper data container
	QList<DataContainerProxy> containers = m_DcaProxy.list;
	QListIterator<DataContainerProxy> containerIter(containers);
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
						//DataArrayProxy daProxy = dataArraysIter.value();
						QString daName = dataArraysIter.key();
						QListWidgetItem* daItem = new QListWidgetItem(daName);
						daItem->setCheckState(Qt::Unchecked);
						attributeArraysWidget->addItem(daItem);
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
	DataContainerArrayProxy dcaProxy(true);
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
		dcaProxy.list.push_back(dcProxy);
	}

	return dcaProxy;

	return DataContainerArrayProxy();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiDataArraySelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
#if 0
	// Generate the path to the AttributeArray
	DataArrayPath path(dataContainerList->currentText(), attributeMatrixList->currentText(), attributeArrayList->currentText());
	QVariant var;
	var.setValue(path);
	bool ok = false;
	// Set the value into the Filter
	ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, var);
	if (false == ok)
	{
		FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(getFilter(), getFilterParameter());
	}
#endif // 0


}

