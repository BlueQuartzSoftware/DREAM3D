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

#include "ReadEdaxH5DataWidget.h"

#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>

#include "SIMPLib/Common/Constants.h"

#include "SVWidgetsLib/QtSupport/QtSFileCompleter.h"
#include "SVWidgetsLib/QtSupport/QtSHelpUrlGenerator.h"
#include "SVWidgetsLib/QtSupport/QtSMacros.h"

#include "OrientationAnalysis/FilterParameters/ReadEdaxH5DataFilterParameter.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/ReadEdaxH5Data.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadEdaxH5DataWidget::ReadEdaxH5DataWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
{
  m_FilterParameter = dynamic_cast<ReadEdaxH5DataFilterParameter*>(parameter);

  m_Filter = qobject_cast<ReadEdaxH5Data*>(filter);
  Q_ASSERT_X(nullptr != m_Filter, "ReadEdaxH5DataWidget can ONLY be used with ReadH5Ebsd filter", __FILE__);

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadEdaxH5DataWidget::~ReadEdaxH5DataWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadEdaxH5DataWidget::setFilter(AbstractFilter* value)
{
  m_Filter = dynamic_cast<ReadEdaxH5Data*>(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter* ReadEdaxH5DataWidget::getFilter() const
{
  return m_Filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadEdaxH5DataWidget::setFilterParameter(FilterParameter* value)
{
  m_FilterParameter = dynamic_cast<ReadEdaxH5DataFilterParameter*>(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter* ReadEdaxH5DataWidget::getFilterParameter() const
{
  return m_FilterParameter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadEdaxH5DataWidget::setupGui()
{
  // Catch when the filter is about to execute the preflight
  connect(m_Filter, SIGNAL(preflightAboutToExecute()), this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(m_Filter, SIGNAL(preflightExecuted()), this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(m_Filter, SIGNAL(updateFilterParameters(AbstractFilter*)), this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  QStringList selectedList = m_Filter->getSelectedScanNames();
  selectedScanNames->addItems(selectedList);

  connect(selectedScanNames, SIGNAL(dropOccurred()), this, SLOT(on_addScanName_clicked()));
  connect(totalScanNames, SIGNAL(dropOccurred()), this, SLOT(on_removeScanName_clicked()));

  updateList();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadEdaxH5DataWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  if(nullptr == filter)
  {
    QString ss = QObject::tr("Error Setting ReadH5Ebsd Gui values to Filter instance. Filter instance was nullptr.").arg(m_FilterParameter->getPropertyName());
    emit errorSettingFilterParameter(ss);
  }

  ReadEdaxH5Data* readEdax = qobject_cast<ReadEdaxH5Data*>(filter);
  Q_ASSERT_X(nullptr != readEdax, "ReadEdaxH5DataWidget can ONLY be used with ReadH5Ebsd filter", __FILE__);

  QStringList scanNames;
  for(int i = 0; i < selectedScanNames->count(); i++)
  {
    scanNames.push_back(selectedScanNames->item(i)->text());
  }

  readEdax->setSelectedScanNames(scanNames);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadEdaxH5DataWidget::beforePreflight()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadEdaxH5DataWidget::afterPreflight()
{
  updateList();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadEdaxH5DataWidget::updateList()
{
  if(getFilter())
  {
    // Get the list of choices from the filter
    QString listProp = m_FilterParameter->getListProperty();

    QVariant var = getFilter()->property(listProp.toLatin1().constData());
    if(var.isValid() == false)
    {
      qDebug() << "Error getting Property " << m_FilterParameter->getListProperty() << " from Filter";
    }
    QStringList choices = var.toStringList();

    totalScanNames->blockSignals(true);
    totalScanNames->clear();
    totalScanNames->addItems(choices);

    sortList(totalScanNames, Qt::AscendingOrder);

    for(int i = 0; i < selectedScanNames->count(); i++)
    {
      QString name = selectedScanNames->item(i)->text();
      QList<QListWidgetItem*> list = totalScanNames->findItems(name, Qt::MatchExactly);
      for(int j = 0; j < list.size(); j++)
      {
        list[j]->setFlags(list[j]->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
      }
    }

    totalScanNames->blockSignals(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadEdaxH5DataWidget::on_addScanName_clicked()
{
  if(nullptr != totalScanNames->currentItem())
  {
    QModelIndexList indexList = totalScanNames->selectionModel()->selectedRows();

    for(int i = 0; i < indexList.size(); i++)
    {
      QString currentName = totalScanNames->item(indexList[i].row())->text();
      selectedScanNames->addItem(currentName);
    }

    on_stackLowToHighBtn_toggled(stackLowToHighBtn->isChecked());

    emit parametersChanged();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadEdaxH5DataWidget::on_removeScanName_clicked()
{
  if(nullptr != selectedScanNames->currentItem())
  {
    QModelIndexList indexList = selectedScanNames->selectionModel()->selectedRows();
    QList<QPersistentModelIndex> persistentIndexList;
    for(int i = 0; i < indexList.size(); i++)
    {
      persistentIndexList.push_back(QPersistentModelIndex(indexList[i]));
    }

    for(int i = 0; i < persistentIndexList.size(); i++)
    {
      QListWidgetItem* item = selectedScanNames->takeItem(persistentIndexList[i].row());
      delete item;
    }

    on_stackLowToHighBtn_toggled(stackLowToHighBtn->isChecked());

    emit parametersChanged();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadEdaxH5DataWidget::on_stackLowToHighBtn_toggled(bool checked)
{
  if(checked == true)
  {
    sortList(selectedScanNames, Qt::AscendingOrder);
  }
  else
  {
    sortList(selectedScanNames, Qt::DescendingOrder);
  }

  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadEdaxH5DataWidget::sortList(DREAM3DListWidget* listWidget, Qt::SortOrder order)
{
  QMap<int, QString> sortingMap;
  for(int i = 0; i < listWidget->count(); i++)
  {
    QRegularExpression regExp("([A-Z]|[a-z]|_)+|\\d+");
    QString selectedScanName = listWidget->item(i)->text();
    QRegularExpressionMatchIterator iter = regExp.globalMatch(selectedScanName);

    while(iter.hasNext())
    {
      QRegularExpressionMatch match = iter.next();
      bool ok;
      int num = match.captured().toInt(&ok);
      if(ok == true)
      {
        sortingMap.insert(num, selectedScanName);
      }
    }
  }

  QStringList scanNames;
  if(order == Qt::AscendingOrder)
  {
    QMapIterator<int, QString> iter(sortingMap);
    while(iter.hasNext())
    {
      iter.next();
      scanNames.push_back(iter.value());
    }
  }
  else
  {
    QMapIterator<int, QString> iter(sortingMap);
    iter.toBack();
    while(iter.hasPrevious())
    {
      iter.previous();
      scanNames.push_back(iter.value());
    }
  }

  listWidget->clear();
  listWidget->addItems(scanNames);
}
