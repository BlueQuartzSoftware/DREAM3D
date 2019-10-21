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

#include "EnsembleInfoCreationWidget.h"



#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"

#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidgetUtils.hpp"
#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidgetsDialogs.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnsembleInfoCreationWidget::EnsembleInfoCreationWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
, m_ShowOperators(true)
, m_DidCausePreflight(false)
, m_EnsembleInfoTableModel(nullptr)
{
  m_FilterParameter = dynamic_cast<EnsembleInfoFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "EnsembleInfoCreationWidget can ONLY be used with a EnsembleInfoFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnsembleInfoCreationWidget::~EnsembleInfoCreationWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnsembleInfo EnsembleInfoCreationWidget::getEnsembleInfo()
{
  EnsembleInfo ensembleInfo;
  if(m_EnsembleInfoTableModel == nullptr)
  {
    return ensembleInfo;
  }

  int rowCount = m_EnsembleInfoTableModel->rowCount();
  QVector<int> crystalStructures;
  QVector<int> phaseTypes;
  QVector<QString> phaseNames;
  m_EnsembleInfoTableModel->getTableData(crystalStructures, phaseTypes, phaseNames);

  for(int i = 0; i < rowCount; ++i)
  {
    EnsembleInfo::CrystalStructure structure = static_cast<EnsembleInfo::CrystalStructure>(crystalStructures[i]);
    PhaseType::Type phaseType = static_cast<PhaseType::Type>(phaseTypes[i]);
    QString phaseName = phaseNames[i];

    ensembleInfo.addValues(structure, phaseType, phaseName);
  }
  return ensembleInfo;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoCreationWidget::setupGui()
{
  if(getFilter() == nullptr)
  {
    return;
  }
  if(getFilterParameter() == nullptr)
  {
    return;
  }

  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()), this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()), this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)), this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  // Create the table model
  m_EnsembleInfoTableModel = createEnsembleInfoModel();

  // Set the data into the TableModel
  EnsembleInfo info = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<EnsembleInfo>();
  m_EnsembleInfoTableModel->setTableData(info);


#if 0
  // is the filter parameter tied to a boolean property of the Filter Instance, if it is then we need to make the check box visible
  if(getFilterParameter()->isConditional())
  {
    bool boolProp = getFilter()->property(getFilterParameter()->getConditionalProperty().toLatin1().constData() ).toBool();
    conditionalCB->setChecked(boolProp);
    conditionalCB->setText(getFilterParameter()->getConditionalLabel());
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
void EnsembleInfoCreationWidget::on_conditionalCB_stateChanged(int state)
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
QStringList EnsembleInfoCreationWidget::generateAttributeArrayList(const QString& currentDCName, const QString& currentAttrMatName)
{
  QStringList attributeArrayList;

  // Loop over the data containers until we find the proper data container
  DataContainerArrayProxy::StorageType& dcMap = m_DcaProxy.getDataContainers();
  for(auto& dc : dcMap)
  {
    if(dc.getName() == currentDCName)
    {
      // We found the proper Data Container, now populate the AttributeMatrix List
      QMap<QString, AttributeMatrixProxy>& attrMats = dc.getAttributeMatricies();
      for(auto& amProxy : attrMats)
      {
        if(amProxy.getName() == currentAttrMatName)
        {
          // We found the selected AttributeMatrix, so loop over this attribute matrix arrays and populate the list widget
          QMap<QString, DataArrayProxy>& dataArrays = amProxy.getDataArrays();
          for(auto& daProxy : dataArrays)
          {
            attributeArrayList << daProxy.getName();
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
QString EnsembleInfoCreationWidget::checkStringValues(QString curDcName, QString filtDcName)
{
  if(curDcName.isEmpty() && !filtDcName.isEmpty())
  {
    return filtDcName;
  }
  if(!curDcName.isEmpty() && filtDcName.isEmpty())
  {
    return curDcName;
  }
  if(!curDcName.isEmpty() && !filtDcName.isEmpty() && m_DidCausePreflight)
  {
    return curDcName;
  }

  return filtDcName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoCreationWidget::tableDataWasChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
  Q_UNUSED(topLeft)
  Q_UNUSED(bottomRight)
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoCreationWidget::widgetChanged(const QString& text)
{
  Q_UNUSED(text)
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoCreationWidget::on_addBtn_clicked()
{
  if(!m_EnsembleInfoTableModel->insertRow(m_EnsembleInfoTableModel->rowCount()))
  {
    return;
  }

  QModelIndex index = m_EnsembleInfoTableModel->index(m_EnsembleInfoTableModel->rowCount() - 1, 0);
  ensemblePhasesTableView->setCurrentIndex(index);
  ensemblePhasesTableView->resizeColumnsToContents();
  ensemblePhasesTableView->scrollToBottom();
  ensemblePhasesTableView->setFocus();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoCreationWidget::on_deleteBtn_clicked()
{
  QItemSelectionModel* selectionModel = ensemblePhasesTableView->selectionModel();

  if(!selectionModel->hasSelection())
  {
    return;
  }
  QModelIndex index = selectionModel->currentIndex();
  if(!index.isValid())
  {
    return;
  }
  m_EnsembleInfoTableModel->removeRow(index.row(), index.parent());
  if(m_EnsembleInfoTableModel->rowCount() > 0)
  {
    ensemblePhasesTableView->resizeColumnsToContents();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoCreationWidget::setEnsembleInput(EnsembleInfo info)
{
  qint32 count = info.size();

  QVector<int> crystalStructures(count);
  QVector<int> phaseTypes(count);
  QVector<QString> phaseNames(count);
  // bool ok = false;

  for(int i = 0; i < count; ++i)
  {
    EnsembleInfo::CrystalStructure structure;
    PhaseType::Type type;
    QString name;

    info.getValues(i, structure, type, name);

    crystalStructures[i] = static_cast<int>(structure);
    phaseTypes[i] = static_cast<int>(type);
    phaseNames[i] = name;
  }
  m_EnsembleInfoTableModel->setTableData(crystalStructures, phaseTypes, phaseNames);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoCreationWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  QVariant var;
  EnsembleInfo comps = getEnsembleInfo();
  var.setValue(comps);
  bool ok = false;
  // Set the value into the Filter
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, var);
  if(!ok)
  {
    getFilter()->notifyMissingProperty(getFilterParameter());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoCreationWidget::beforePreflight()
{
  if(nullptr == getFilter())
  {
    return;
  }
  if(m_DidCausePreflight)
  {
    // std::cout << "***  EnsembleInfoCreationWidget already caused a preflight, just returning" << std::endl;
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoCreationWidget::afterPreflight()
{
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(nullptr == dca.get())
  {
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnsembleInfoTableModel* EnsembleInfoCreationWidget::createEnsembleInfoModel()
{
  EnsembleInfoTableModel* newModel = new EnsembleInfoTableModel(m_ShowOperators);
  QAbstractItemModel* oldModel = ensemblePhasesTableView->model();

    delete oldModel;

  ensemblePhasesTableView->setModel(newModel);
  newModel->setNumberOfPhases(1);
  ensemblePhasesTableView->resizeColumnsToContents();

  // Now connect all the signals and slots
  connect(newModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex)), this, SLOT(tableDataWasChanged(const QModelIndex&, const QModelIndex&)));

  // Set the ItemDelegate for the table.
  QAbstractItemDelegate* aid = newModel->getItemDelegate();
  ensemblePhasesTableView->setItemDelegate(aid);

  return newModel;
}

// -----------------------------------------------------------------------------
void EnsembleInfoCreationWidget::setArrayListType(const EnsembleInfoCreationWidget::ArrayListType& value)
{
  m_ArrayListType = value;
}

// -----------------------------------------------------------------------------
EnsembleInfoCreationWidget::ArrayListType EnsembleInfoCreationWidget::getArrayListType() const
{
  return m_ArrayListType;
}

// -----------------------------------------------------------------------------
void EnsembleInfoCreationWidget::setShowOperators(bool value)
{
  m_ShowOperators = value;
}

// -----------------------------------------------------------------------------
bool EnsembleInfoCreationWidget::getShowOperators() const
{
  return m_ShowOperators;
}
