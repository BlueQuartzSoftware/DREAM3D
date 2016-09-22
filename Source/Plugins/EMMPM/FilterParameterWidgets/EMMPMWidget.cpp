/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include "EMMPMWidget.h"

#include <assert.h>

#include <QtCore/QMetaProperty>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QDateTime>

#include <QtWidgets/QLabel>
#include <QtWidgets/QFileDialog>
#include <QtGui/QStandardItemModel>
#include <QtCore/QItemSelectionModel>
#include <QtWidgets/QListWidget>

#include "EMMPM/EMMPMFilters/EMMPMFilter.h"
#include "EMMPM/FilterParameters/EMMPMFilterParameter.h"
#include "EMMPMLib/Core/EMMPM_Data.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"

// Include the MOC generated file for this class
#include "moc_EMMPMWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPMWidget::EMMPMWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent),
  m_DidCausePreflight(false)
{
  m_FilterParameter = dynamic_cast<EMMPMFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "EMMPMWidget can ONLY be used with a EMMPMFilterParameter object");
  m_Filter = dynamic_cast<EMMPMFilter*>(filter);
  Q_ASSERT_X(getFilter() != nullptr, "NULL Pointer", "EMMPMWidget can ONLY be used with a EMMPMFilter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPMWidget::EMMPMWidget(QWidget* parent) :
  FilterParameterWidget(nullptr, nullptr, parent),
  m_Filter(nullptr),
  m_FilterParameter(nullptr),
  m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPMWidget::~EMMPMWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMWidget::setFilter(AbstractFilter* value)
{
  m_Filter = dynamic_cast<EMMPMFilter*>(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter* EMMPMWidget::getFilter() const
{
  return m_Filter;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMWidget::setFilterParameter(FilterParameter* value)
{
  m_FilterParameter = dynamic_cast<EMMPMFilterParameter*>(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter* EMMPMWidget::getFilterParameter() const
{
  return m_FilterParameter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMWidget::setupGui()
{

  connect(getFilter(), SIGNAL(preflightAboutToExecute()),
          this, SLOT(beforePreflight()));

  connect(getFilter(), SIGNAL(preflightExecuted()),
          this, SLOT(afterPreflight()));

  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)),
          this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  if(m_Filter != nullptr)
  {
    m_NumClasses->blockSignals(true);
    m_NumClasses->setValue(m_Filter->getNumClasses());
    m_NumClasses->blockSignals(false);
    m_Beta->setText(QString::number(m_Filter->getExchangeEnergy()));
    m_EmIterations->setValue(m_Filter->getHistogramLoops());
    m_MpmIterations->setValue(m_Filter->getSegmentationLoops());

    EMMPM_InitializationType initType = m_Filter->getEmmpmInitType();
    if (initType == EMMPM_InitializationType::EMMPM_ManualInit)
    {
      enableManualInit->setChecked(true);
    }
    else
    {
      enableManualInit->setChecked(false);
    }

    useSimulatedAnnealing->setChecked(m_Filter->getUseSimulatedAnnealing());

    tableWidget->blockSignals(true);
    {
      DynamicTableData emmpmTableData = m_Filter->getEMMPMTableData();

      std::vector<std::vector<double> > data = emmpmTableData.getTableData();
      for (int row = 0; row < data.size(); row++)
      {
        tableWidget->insertRow(row);
        for (int col = 0; col < data[row].size(); col++)
        {
          if (tableWidget->columnCount() == col)
          {
            tableWidget->insertColumn(col);
          }

          QTableWidgetItem* item = new QTableWidgetItem(QString::number(data[row][col]));
          if (initType != EMMPM_InitializationType::EMMPM_ManualInit && (col == 2 || col == 3))
          {
            item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
          }
          tableWidget->setItem(row, col, item);
        }
      }

      tableWidget->setVerticalHeaderLabels(emmpmTableData.getRowHeaders());
      tableWidget->setHorizontalHeaderLabels(emmpmTableData.getColHeaders());
    }
    tableWidget->blockSignals(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableData EMMPMWidget::getDynamicTableData(QTableWidget* tableWidget)
{
  QStringList rHeaders, cHeaders;
  for (int i = 0; i < tableWidget->rowCount(); i++)
  {
    QTableWidgetItem* vItem = tableWidget->verticalHeaderItem(i);
    if (nullptr != vItem)
    {
      QString vName = vItem->data(Qt::DisplayRole).toString();
      rHeaders << vName;
    }
  }
  for (int i = 0; i < tableWidget->columnCount(); i++)
  {
    QTableWidgetItem* cItem = tableWidget->horizontalHeaderItem(i);
    if (nullptr != cItem)
    {
      QString cName = cItem->data(Qt::DisplayRole).toString();
      cHeaders << cName;
    }
  }

  int rCount = tableWidget->rowCount(), cCount = tableWidget->columnCount();
  std::vector<std::vector<double> > data(rCount, std::vector<double>(cCount));

  for (int row = 0; row < rCount; row++)
  {
    for (int col = 0; col < cCount; col++)
    {
      bool ok = false;
      QTableWidgetItem* item = tableWidget->item(row, col);
      if (nullptr == item)
      {
        return std::vector < std::vector<double> >();
      }
      data[row][col] = item->data(Qt::DisplayRole).toDouble(&ok);

      if (ok == false)
      {
        qDebug() << "Could not set the model data into the DynamicTableData object.";
        data.clear();
      }
    }
  }

  DynamicTableData dataObj;
  dataObj.setTableData(data);
  dataObj.setRowHeaders(rHeaders);
  dataObj.setColHeaders(cHeaders);

  return dataObj;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMWidget::on_tableWidget_cellChanged(int row, int column)
{
  Q_UNUSED(row)
  Q_UNUSED(column)

  DynamicTableData newData = getDynamicTableData(tableWidget);
  m_Filter->setEMMPMTableData(newData);

  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMWidget::on_m_NumClasses_valueChanged(int i)
{
  if (tableWidget->rowCount() < i)
  {
    EMMPM_InitializationType initType = m_Filter->getEmmpmInitType();
    for (int row = tableWidget->rowCount(); row < i; row++)
    {
      tableWidget->insertRow(row);

      QTableWidgetItem* item = new QTableWidgetItem("0");
      tableWidget->setItem(row, 0, item);

      item = new QTableWidgetItem("4.5");
      tableWidget->setItem(row, 1, item);

      item = new QTableWidgetItem("128");
      if (initType != EMMPM_InitializationType::EMMPM_ManualInit)
      {
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
      }
      tableWidget->setItem(row, 2, item);

      item = new QTableWidgetItem("20");
      if (initType != EMMPM_InitializationType::EMMPM_ManualInit)
      {
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
      }
      tableWidget->setItem(row, 3, item);
    }
  }
  else
  {
    for (int row = tableWidget->rowCount() - 1; row >= i; row--)
    {
      tableWidget->removeRow(row);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMWidget::on_enableManualInit_toggled(bool checked)
{
  if (checked == true)
  {
    for (int row=0; row<tableWidget->rowCount(); row++)
    {
      QTableWidgetItem* item = tableWidget->item(row, 2);
      item->setFlags(item->flags() | Qt::ItemIsEnabled);

      item = tableWidget->item(row, 3);
      item->setFlags(item->flags() | Qt::ItemIsEnabled);
    }
  }
  else
  {
    for (int row=0; row<tableWidget->rowCount(); row++)
    {
      QTableWidgetItem* item = tableWidget->item(row, 2);
      item->setFlags(item->flags() & ~Qt::ItemIsEnabled);

      item = tableWidget->item(row, 3);
      item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMWidget::beforePreflight()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMWidget::afterPreflight()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  Q_UNUSED(filter)

  m_Filter->setNumClasses(m_NumClasses->value());

  bool ok = false;
  float val = m_Beta->text().toFloat(&ok);
  if (ok)
  {
    m_Filter->setExchangeEnergy(val);
  }

  m_Filter->setHistogramLoops(m_EmIterations->value());
  m_Filter->setSegmentationLoops(m_MpmIterations->value());
  m_Filter->setUseSimulatedAnnealing(useSimulatedAnnealing->isChecked());

  EMMPM_InitializationType initType = EMMPM_InitializationType::EMMPM_Basic;
  if (enableManualInit->isChecked())
  {
    initType = EMMPM_InitializationType::EMMPM_ManualInit;
  }

  DynamicTableData tableData = getDynamicTableData(tableWidget);
  m_Filter->setEMMPMTableData(tableData);

  m_Filter->setEmmpmInitType(initType);
}
