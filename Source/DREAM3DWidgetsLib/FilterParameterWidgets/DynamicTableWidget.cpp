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

#include <QMessageBox>

#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/FilterParameters/FilterParameter.h"
#include "SIMPLib/FilterParameters/DynamicTableData.h"
#include "SIMPLib/FilterParameters/DynamicTableFilterParameter.h"

#include "FilterParameterWidgetsDialogs.h"

#include "DREAM3DWidgetsLib/FilterParameterWidgets/DynamicTableItemDelegate.h"
#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"

const QString addRowTT = "Adds a row to the table.";
const QString addColTT = "Adds a column to the table.";
const QString deleteRowTT = "Removes the currently selected row from the table.";
const QString deleteColTT = "Removes the currently selected column from the table.";

// Include the MOC generated file for this class
#include "moc_DynamicTableWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableWidget::DynamicTableWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent)
{
  m_FilterParameter = dynamic_cast<DynamicTableFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != NULL, "NULL Pointer", "DynamicTableWidget can ONLY be used with a DynamicTableFilterParameter object");

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
void DynamicTableWidget::initializeWidget(FilterParameter* parameter, AbstractFilter* filter)
{
  setFilter(filter);
  setFilterParameter(parameter);
  setupGui();
}

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

  tableLabel->setText(m_FilterParameter->getHumanLabel());

  // Set the item delegate so that we can only enter 'double' values into the table
  DynamicTableItemDelegate* dlg = new DynamicTableItemDelegate;
  dynamicTable->setItemDelegate(dlg);

  // Set button tooltips
  addRowBtn->setToolTip(addRowTT);
  addColBtn->setToolTip(addColTT);
  deleteRowBtn->setToolTip(deleteRowTT);
  deleteColBtn->setToolTip(deleteColTT);

  // Populate the table
  populateTable();

  // Set Icons
  QIcon addIcon = QIcon(QString(":/add2.png"));
  QIcon deleteIcon = QIcon(QString(":/delete2.png"));
  addRowBtn->setIcon(addIcon);
  addColBtn->setIcon(addIcon);
  deleteRowBtn->setIcon(deleteIcon);
  deleteColBtn->setIcon(deleteIcon);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableWidget::on_dynamicTable_cellChanged(int row, int col)
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
  QStringList rHeaders, cHeaders;
  for (int i = 0; i < dynamicTable->rowCount(); i++)
  {
    QTableWidgetItem* vItem = dynamicTable->verticalHeaderItem(i);
    if (NULL != vItem)
    {
      QString vName = vItem->data(Qt::DisplayRole).toString();
      rHeaders << vName;
    }
  }
  for (int i = 0; i < dynamicTable->columnCount(); i++)
  {
    QTableWidgetItem* cItem = dynamicTable->horizontalHeaderItem(i);
    if (NULL != cItem)
    {
      QString cName = cItem->data(Qt::DisplayRole).toString();
      cHeaders << cName;
    }
  }

  DynamicTableData data(getData(), rHeaders, cHeaders);

  QVariant v;
  v.setValue(data);
  bool ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, v);

  if (false == ok)
  {
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(getFilter(), m_FilterParameter);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<std::vector<double> > DynamicTableWidget::getData()
{
  int rCount = dynamicTable->rowCount(), cCount = dynamicTable->columnCount();
  std::vector<std::vector<double> > data(rCount, std::vector<double>(cCount));

  for (int row = 0; row < rCount; row++)
  {
    for (int col = 0; col < cCount; col++)
    {
      bool ok = false;
      QTableWidgetItem* item = dynamicTable->item(row, col);
      if (NULL == item)
      {
        return std::vector < std::vector<double> >();
      }
      data[row][col] = item->data(Qt::DisplayRole).toDouble(&ok);

      if (ok == false)
      {
        qDebug() << "Could not set the model data into the DynamicTableData object.";
        data.clear();
        return data;
      }
    }
  }

  return data;
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableWidget::on_addRowBtn_pressed()
{
  int row = dynamicTable->rowCount();

  // If we are adding the first row, add the first column too.
  if (row <= 0)
  {
    dynamicTable->insertColumn(0);
    dynamicTable->setHorizontalHeaderItem(0, new QTableWidgetItem("0"));
  }

  dynamicTable->insertRow(row);
  dynamicTable->setVerticalHeaderItem(row, new QTableWidgetItem(QString::number(dynamicTable->rowCount() - 1)));

  dynamicTable->blockSignals(true);
  for (int col = 0; col < dynamicTable->columnCount(); col++)
  {
    if (col + 1 == dynamicTable->columnCount())
    {
      // Only fire the signal after the last new item has been created
      dynamicTable->blockSignals(false);
    }

    QTableWidgetItem* item = new QTableWidgetItem("0");
    dynamicTable->setItem(row, col, item);
  }

  // Resize entire table to contents
  dynamicTable->resizeRowsToContents();
  dynamicTable->resizeColumnsToContents();

  // Update buttons
  updateDynamicButtons();

  // Renumber dynamic headers
  renumberDynamicHeaders();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableWidget::on_addColBtn_pressed()
{
  int col = dynamicTable->columnCount();

  // If we are adding the first column, add the first row too.
  if (col <= 0)
  {
    dynamicTable->insertRow(0);
    dynamicTable->setVerticalHeaderItem(0, new QTableWidgetItem("0"));
  }

  dynamicTable->insertColumn(col);
  dynamicTable->setHorizontalHeaderItem(col, new QTableWidgetItem(QString::number(dynamicTable->columnCount() - 1)));

  dynamicTable->blockSignals(true);
  for (int row = 0; row < dynamicTable->rowCount(); row++)
  {
    if (row + 1 == dynamicTable->rowCount())
    {
      // Only fire the signal after the last new item has been created
      dynamicTable->blockSignals(false);
    }

    QTableWidgetItem* item = new QTableWidgetItem("0");
    dynamicTable->setItem(row, col, item);
  }

  // Resize entire table to contents
  dynamicTable->resizeRowsToContents();
  dynamicTable->resizeColumnsToContents();

  // Update buttons
  updateDynamicButtons();

  // Renumber dynamic headers
  renumberDynamicHeaders();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableWidget::on_deleteRowBtn_pressed()
{
  dynamicTable->removeRow(dynamicTable->currentRow());

  if (dynamicTable->rowCount() <= 0)
  {
    while (dynamicTable->columnCount() > 0)
    {
      dynamicTable->removeColumn(0);
    }
  }

  // Update buttons
  updateDynamicButtons();

  // Renumber dynamic headers
  renumberDynamicHeaders();

  // Cause a preflight
  on_dynamicTable_cellChanged(0, 0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableWidget::on_deleteColBtn_pressed()
{
  dynamicTable->removeColumn(dynamicTable->currentColumn());

  if (dynamicTable->columnCount() <= 0)
  {
    while (dynamicTable->rowCount() > 0)
    {
      dynamicTable->removeRow(0);
    }
  }

  // Update buttons
  updateDynamicButtons();

  // Renumber dynamic headers
  renumberDynamicHeaders();

  // Cause a preflight
  on_dynamicTable_cellChanged(0, 0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableWidget::populateTable()
{
  // Get what is in the filter
  DynamicTableData data = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<DynamicTableData>();

  if (m_FilterParameter != NULL)
  {
    // If the filter parameter generated an error, empty the data object and display the error.
    if (m_FilterParameter->getErrorCondition() < 0)
    {
      QString errorMessage = m_FilterParameter->getErrorMessage();
      QString pluginName = getFilter()->getPluginInstance()->getPluginName();
      QString filterName = getFilter()->getHumanLabel();
      QString filterParameterName = m_FilterParameter->getPropertyName();
      QString vendorName = getFilter()->getPluginInstance()->getVendor();

      // Use HTML code in the error message, because this will be displayed in RichText format.
      errorMessage.prepend("The filter parameter '" + filterParameterName + "' in filter '" + filterName + "' has an error:<blockquote><b>");
      errorMessage.append("</b></blockquote>'" + filterName + "' is a part of the " + pluginName + " plugin.  ");
      errorMessage.append("Please contact " + vendorName + ", the developer of the " + pluginName + " plugin.");

      QMessageBox errorBox;
      errorBox.setIcon(QMessageBox::Critical);
      errorBox.setWindowTitle("'" + filterName + "' Error");
      errorBox.setText(errorMessage);
      errorBox.setStandardButtons(QMessageBox::Ok);
      errorBox.setDefaultButton(QMessageBox::Ok);
      errorBox.setTextFormat(Qt::RichText);
      errorBox.exec();
    }
    else if (data.isEmpty())  // If there was nothing in the filter, use the defaults
    {
      data.setTableData(m_FilterParameter->getDefaultTable());
      data.setRowHeaders(m_FilterParameter->getRowHeaders());
      data.setColHeaders(m_FilterParameter->getColumnHeaders());

      // Populate table with default values
      for (int row = 0; row < data.getNumRows(); row++)
      {
        dynamicTable->insertRow(row);
        for (int col = 0; col < data.getNumCols(); col++)
        {
          if (dynamicTable->columnCount() == col)
          {
            dynamicTable->insertColumn(col);
          }
          QTableWidgetItem* item = new QTableWidgetItem(QString::number(data.getTableData()[row][col]));
          dynamicTable->setItem(row, col, item);
        }
      }
    }
    else
    {
      std::vector<std::vector<double> > tableData = data.getTableData();

      // Populate table with filter values
      for (int row = 0; row < tableData.size(); row++)
      {
        dynamicTable->insertRow(row);
        for (int col = 0; col < tableData[row].size(); col++)
        {
          if (dynamicTable->columnCount() == col)
          {
            dynamicTable->insertColumn(col);
          }

          QTableWidgetItem* item = new QTableWidgetItem(QString::number(tableData[row][col]));
          dynamicTable->setItem(row, col, item);
        }
      }
    }

    // Populate row and column headers
    populateHeaders(data.getRowHeaders(), data.getColHeaders());

    // Resize rows and columns to contents
    dynamicTable->resizeColumnsToContents();
    dynamicTable->resizeRowsToContents();

    // Update the state of the Add/Remove buttons
    updateDynamicButtons();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableWidget::populateHeaders(QStringList rHeaders, QStringList cHeaders)
{
  if (m_FilterParameter->getAreRowsDynamic() == false)
  {
    dynamicTable->setVerticalHeaderLabels(rHeaders);
  }
  if (m_FilterParameter->getAreColsDynamic() == false)
  {
    dynamicTable->setHorizontalHeaderLabels(cHeaders);
  }

  renumberDynamicHeaders(); // Use this function to populate the dynamic headers for the first time
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableWidget::renumberDynamicHeaders()
{
  if (m_FilterParameter->getAreRowsDynamic() == true)
  {
    QStringList rHeaders;
    for (int i = 0; i < dynamicTable->rowCount(); i++)
    {
      rHeaders << QString::number(i);
    }
    dynamicTable->setVerticalHeaderLabels(rHeaders);
  }

  if (m_FilterParameter->getAreColsDynamic() == true)
  {
    QStringList cHeaders;
    for (int i = 0; i < dynamicTable->columnCount(); i++)
    {
      cHeaders << QString::number(i);
    }
    dynamicTable->setHorizontalHeaderLabels(cHeaders);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableWidget::updateDynamicButtons()
{
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

  // Enable/Disable delete row button
  if (dynamicTable->rowCount() <= m_FilterParameter->getMinRowCount())
  {
    deleteRowBtn->setDisabled(true);
    QString toolTip = "'" + m_FilterParameter->getHumanLabel() + "' must have at least " + QString::number(m_FilterParameter->getMinRowCount()) + " rows.";
    if (m_FilterParameter->getMinRowCount() == 1)
    {
      // Fix the grammar in the tooltip when there's only 1 row.
      toolTip.chop(2);
      toolTip.append('.');
    }
    deleteRowBtn->setToolTip(toolTip);
  }
  else
  {
    deleteRowBtn->setEnabled(true);
    deleteRowBtn->setToolTip(deleteRowTT);
  }

  // Enable/Disable delete column button
  if (dynamicTable->columnCount() <= m_FilterParameter->getMinColCount())
  {
    deleteColBtn->setDisabled(true);
    QString toolTip = "'" + m_FilterParameter->getHumanLabel() + "' must have at least " + QString::number(m_FilterParameter->getMinColCount()) + " columns.";
    if (m_FilterParameter->getMinColCount() == 1)
    {
      // Fix the grammar in the tooltip when there's only 1 column.
      toolTip.chop(2);
      toolTip.append('.');
    }
    deleteColBtn->setToolTip(toolTip);
  }
  else
  {
    deleteColBtn->setEnabled(true);
    deleteColBtn->setToolTip(deleteColTT);
  }
}



