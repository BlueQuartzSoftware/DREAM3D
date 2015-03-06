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
#include "DynamicTableWidget.h"

#include <QtCore/QMetaProperty>

#include "DREAM3DLib/FilterParameters/FilterParameter.h"

#include "FilterParameterWidgetsDialogs.h"

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableWidget::DynamicTableWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
FilterParameterWidget(parameter, filter, parent)
{
	m_FilterParameter = dynamic_cast<DynamicTableFilterParameter*>(parameter);
	Q_ASSERT_X(NULL != m_FilterParameter, "DynamicTableWidget can ONLY be used with Dynamic Table Filter Parameters", __FILE__);

	setupUi(this);
	setupGui();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableWidget::~DynamicTableWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableWidget::setFilterParameter(FilterParameter* value)
{
	m_FilterParameter = dynamic_cast<DynamicTableFilterParameter*>(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter* DynamicTableWidget::getFilterParameter() const
{
	return m_FilterParameter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableWidget::setupGui()
{

	// Catch when the filter is about to execute the preflight
	connect(getFilter(), SIGNAL(preflightAboutToExecute()),
		this, SLOT(beforePreflight()));

	// Catch when the filter is finished running the preflight
	connect(getFilter(), SIGNAL(preflightExecuted()),
		this, SLOT(afterPreflight()));

	// Catch when the filter wants its values updated
	connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)),
		this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

	// Catch when a value in the table changes
	connect(dynamicTable, SIGNAL(itemChanged(QTableWidgetItem*)),
		this, SLOT(widgetChanged(QTableWidgetItem*)));

	if (m_FilterParameter != NULL)
	{
		QStringList rHeaders = m_FilterParameter->getRowHeaders();
		QStringList cHeaders = m_FilterParameter->getColumnHeaders();

		// Default row and column numbers???  How do we reconcile this with the data already coming in???

		for (int rHeader = 0; rHeader < rHeaders.size(); rHeader++)
		{
			// Populate row headers
		}

		for (int cHeader = 0; cHeader < cHeaders.size(); cHeader++)
		{
			// Populate column headers
		}

		// Populate the table with the default values
		std::vector<std::vector<double> > table = m_FilterParameter->getDefaultTable();
		for (int row = 0; row < table.size(); row++)
		{
			for (int col = 0; col < table[row].size(); col++)
			{
				QTableWidgetItem* item = new QTableWidgetItem(QString::number(table[row][col]));
				dynamicTable->setItem(row, col, item);
			}
		}

		// Hide add/remove row buttons if row count is not dynamic
		if (m_FilterParameter->getAreRowsDynamic() == false)
		{
			addRowBtn->setHidden(true);
			deleteRowBtn->setHidden(true);
		}

		// Hide add/remove column buttons if column count is not dynamic
		if (m_FilterParameter->getAreColsDynamic() == false)
		{
			addColBtn->setHidden(true);
			deleteColBtn->setHidden(true);
		}
	}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableWidget::widgetChanged(QTableWidgetItem* item)
{
	m_DidCausePreflight = true;
	emit parametersChanged();
	m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
	/*QString index = value->currentText();
	QVariant v(index);
	bool ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, v);

	if (false == ok)
	{
		FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(getFilter(), m_FilterParameter);
	}*/
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableWidget::beforePreflight()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableWidget::afterPreflight()
{
	
}
