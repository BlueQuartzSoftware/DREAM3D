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

#include "OEMEbsdScanSelectionWidget.h"

#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>

#include "SIMPLib/Common/Constants.h"

#include "SVWidgetsLib/QtSupport/QtSFileCompleter.h"
#include "SVWidgetsLib/QtSupport/QtSHelpUrlGenerator.h"
#include "SVWidgetsLib/QtSupport/QtSMacros.h"

#include "OrientationAnalysis/FilterParameters/OEMEbsdScanSelectionFilterParameter.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/ImportH5OimData.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OEMEbsdScanSelectionWidget::OEMEbsdScanSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
{
  m_FilterParameter = dynamic_cast<OEMEbsdScanSelectionFilterParameter*>(parameter);

  m_Filter = qobject_cast<ImportH5OimData*>(filter);
  Q_ASSERT_X(nullptr != m_Filter, "OEMEbsdScanSelectionWidget can ONLY be used with ImportH5OimData filter", __FILE__);

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OEMEbsdScanSelectionWidget::~OEMEbsdScanSelectionWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OEMEbsdScanSelectionWidget::setFilter(AbstractFilter* value)
{
  m_Filter = dynamic_cast<ImportH5OimData*>(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter* OEMEbsdScanSelectionWidget::getFilter() const
{
  return m_Filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OEMEbsdScanSelectionWidget::setFilterParameter(FilterParameter* value)
{
  m_FilterParameter = dynamic_cast<OEMEbsdScanSelectionFilterParameter*>(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter* OEMEbsdScanSelectionWidget::getFilterParameter() const
{
  return m_FilterParameter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OEMEbsdScanSelectionWidget::setupGui()
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
  connect(availableScanNames, SIGNAL(dropOccurred()), this, SLOT(on_removeScanName_clicked()));

  updateList();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OEMEbsdScanSelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  if(nullptr == filter)
  {
    QString ss = QObject::tr("Error Setting ReadH5Ebsd Gui values to Filter instance. Filter instance was nullptr.").arg(m_FilterParameter->getPropertyName());
    emit errorSettingFilterParameter(ss);
  }

  ImportH5OimData* readEdax = qobject_cast<ImportH5OimData*>(filter);
  Q_ASSERT_X(nullptr != readEdax, "OEMEbsdScanSelectionWidget can ONLY be used with ReadH5Ebsd filter", __FILE__);

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
void OEMEbsdScanSelectionWidget::beforePreflight()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OEMEbsdScanSelectionWidget::afterPreflight()
{
  updateList();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OEMEbsdScanSelectionWidget::updateList()
{
  if(getFilter() != nullptr)
  {
    // Get the list of choices from the filter
    QString listProp = m_FilterParameter->getListProperty();

    QVariant var = getFilter()->property(listProp.toLatin1().constData());
    if(!var.isValid())
    {
      qDebug() << "Error getting Property " << m_FilterParameter->getListProperty() << " from Filter";
      return;
    }
    QStringList choices = var.toStringList();

    availableScanNames->blockSignals(true);
    availableScanNames->clear();
    availableScanNames->addItems(choices);

    sortList(availableScanNames, Qt::AscendingOrder);

    for(int i = 0; i < selectedScanNames->count(); i++)
    {
      QString selectedScanName = selectedScanNames->item(i)->text();
      QList<QListWidgetItem*> selectedScanNameMatches = availableScanNames->findItems(selectedScanName, Qt::MatchExactly);
      for(QListWidgetItem* selectedScanNameMatch : selectedScanNameMatches)
      {
        selectedScanNameMatch->setFlags(selectedScanNameMatch->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
      }
    }

    availableScanNames->blockSignals(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OEMEbsdScanSelectionWidget::on_addScanName_clicked()
{
  if(nullptr != availableScanNames->currentItem())
  {
    QModelIndexList indexList = availableScanNames->selectionModel()->selectedRows();

    for(const auto& index : indexList)
    {
      QString currentName = availableScanNames->item(index.row())->text();
      selectedScanNames->addItem(currentName);
    }

    on_stackLowToHighBtn_toggled(stackLowToHighBtn->isChecked());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OEMEbsdScanSelectionWidget::on_removeScanName_clicked()
{
  if(nullptr != selectedScanNames->currentItem())
  {
    QModelIndexList indexList = selectedScanNames->selectionModel()->selectedRows();
    QList<QPersistentModelIndex> persistentIndexList;

    for(const auto& index : indexList)
    {
      persistentIndexList.push_back(QPersistentModelIndex(index));
    }

    for(const auto& persistentIndex : persistentIndexList)
    {
      QListWidgetItem* item = selectedScanNames->takeItem(persistentIndex.row());
      delete item;
    }

    on_stackLowToHighBtn_toggled(stackLowToHighBtn->isChecked());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OEMEbsdScanSelectionWidget::on_stackLowToHighBtn_toggled(bool checked)
{
  if(checked)
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
void OEMEbsdScanSelectionWidget::sortList(DREAM3DListWidget* listWidget, Qt::SortOrder order)
{
#if 0
  // This code will fail on a ScanName such as LEROY_0089_Section_435 in which case
  // 2 entries will be made in the list.
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
      if(ok)
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
#endif
}
