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



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonSelectionWidget::ComparisonSelectionWidget(bool showOperators, QWidget *parent) :
  QWidget(parent),
  m_ArrayListType(CellListType),
  m_ShowOperators(showOperators),
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
QVector<ComparisonInput_t> ComparisonSelectionWidget::getComparisonInputs()
{
  QVector<ComparisonInput_t> comps;
  if (m_ComparisonSelectionTableModel == NULL) { return comps; }

  int filterCount = m_ComparisonSelectionTableModel->rowCount();
  QVector<QString> fieldNames;
  QVector<float> fieldValues;
  QVector<int> fieldOperators;
  m_ComparisonSelectionTableModel->getTableData(fieldNames, fieldValues, fieldOperators);

  for(int i = 0; i < filterCount; ++i)
  {
    ComparisonInput_t comp;
    comp.arrayName = fieldNames[i].toStdString();
    comp.compOperator = fieldOperators[i];
    comp.compValue = fieldValues[i];
    comps.push_back(comp);
  }
  return comps;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::setupGui()
{

  m_ComparisonSelectionTableModel = new ComparisonSelectionTableModel(m_ShowOperators);
  QAbstractItemModel* model = m_ComparisonSelectionTableView->model();
  m_ComparisonSelectionTableView->setModel(m_ComparisonSelectionTableModel);

  m_ComparisonSelectionTableModel->setNumberOfPhases(1);
  if(model != m_ComparisonSelectionTableModel && model == NULL)
  {
    delete model; // Clean up this memory
  }
  // Set the ItemDelegate for the table.
  QAbstractItemDelegate* aid = m_ComparisonSelectionTableModel->getItemDelegate();
  m_ComparisonSelectionTableView->setItemDelegate(aid);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::setHumanLabel(QString title)
{
  humanLabel->setText(title);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::on_addComparison_clicked()
{
  if (!m_ComparisonSelectionTableModel->insertRow(m_ComparisonSelectionTableModel->rowCount())) return;

  QModelIndex index = m_ComparisonSelectionTableModel->index(m_ComparisonSelectionTableModel->rowCount() - 1, 0);
  m_ComparisonSelectionTableView->setCurrentIndex(index);
  m_ComparisonSelectionTableView->resizeColumnsToContents();
  m_ComparisonSelectionTableView->scrollToBottom();
  m_ComparisonSelectionTableView->setFocus();
  emit parametersChanged();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::on_removeComparison_clicked()
{
  //qDebug() << "on_removeComparisonSelection_clicked" << "\n";
  QItemSelectionModel *selectionModel = m_ComparisonSelectionTableView->selectionModel();
  if (!selectionModel->hasSelection()) return;
  QModelIndex index = selectionModel->currentIndex();
  if (!index.isValid()) return;
  m_ComparisonSelectionTableModel->removeRow(index.row(), index.parent());
  if (m_ComparisonSelectionTableModel->rowCount() > 0)
  {
    m_ComparisonSelectionTableView->resizeColumnsToContents();
  }
  emit parametersChanged();
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::populateArrayNames(VolumeDataContainer::Pointer vldc,
                                                   SurfaceDataContainer::Pointer sdc,
                                                   EdgeDataContainer::Pointer edc,
                                                   VertexDataContainer::Pointer vdc)
{

  if (m_ArrayListType >= CellListType && m_ArrayListType <= FaceListType )
  {
    populateVolumeArrayNames(vldc);
  }
  else if (m_ArrayListType >= FieldListType && m_ArrayListType <= FaceListType)
  { 
    populateSurfaceArrayNames(sdc);
  }
  else if (m_ArrayListType >= FieldListType && m_ArrayListType <= EdgeListType)
  { 
    populateEdgeArrayNames(edc);
  }
  else if (m_ArrayListType >= FieldListType && m_ArrayListType <= VertexListType)
  { 
    populateVertexArrayNames(vdc);
  }


  // We need to do this each time the possible arrays names are changed upstream in the
  // pipeline so that we get a new/updated array list.
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
    arrayNames[i] = QString::fromStdString(comparisons[i].arrayName);
    compOperators[i] = comparisons[i].compOperator;
    compValues[i] = comparisons[i].compValue;
  }
  m_ComparisonSelectionTableModel->setTableData(arrayNames, compValues, compOperators);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::populateVolumeArrayNames(VolumeDataContainer::Pointer vldc)
{

  QList<QString> cellNames;
  if (m_ArrayListType == VertexListType)
  {
    cellNames = vldc->getVertexArrayNameList();
  }
  else if (m_ArrayListType == EdgeListType)
  {
    cellNames = vldc->getEdgeArrayNameList();
  }
  else if (m_ArrayListType == FaceListType)
  {
    cellNames = vldc->getFaceArrayNameList();
  }
  if (m_ArrayListType == CellListType)
  {
    cellNames = vldc->getCellArrayNameList();
  }
  else if (m_ArrayListType == FieldListType)
  {
    cellNames = vldc->getFieldArrayNameList();
  }
  else if (m_ArrayListType == EnsembleListType)
  {
    cellNames = vldc->getEnsembleArrayNameList();
  }
  m_ComparisonSelectionTableModel->setPossibleFields(cellNames);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::populateSurfaceArrayNames(SurfaceDataContainer::Pointer sdc)
{
  QList<QString> cellNames;
  if (m_ArrayListType == VertexListType)
  {
    cellNames = sdc->getVertexArrayNameList();
  }
  else if (m_ArrayListType == EdgeListType)
  {
    cellNames = sdc->getEdgeArrayNameList();
  }
  else if (m_ArrayListType == FaceListType)
  {
    cellNames = sdc->getFaceArrayNameList();
  }
  else if (m_ArrayListType == FieldListType)
  {
    cellNames = sdc->getFieldArrayNameList();
  }
  else if (m_ArrayListType == EnsembleListType)
  {
    cellNames = sdc->getEnsembleArrayNameList();
  }
  m_ComparisonSelectionTableModel->setPossibleFields(cellNames);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::populateEdgeArrayNames(EdgeDataContainer::Pointer edc)
{
  QList<QString> cellNames;
  if (m_ArrayListType == VertexListType)
  {
    cellNames = edc->getVertexArrayNameList();
  }
  else if (m_ArrayListType == EdgeListType)
  {
    cellNames = edc->getEdgeArrayNameList();
  }
  else if (m_ArrayListType == FieldListType)
  {
    cellNames = edc->getFieldArrayNameList();
  }
  else if (m_ArrayListType == EnsembleListType)
  {
    cellNames = edc->getEnsembleArrayNameList();
  }
  m_ComparisonSelectionTableModel->setPossibleFields(cellNames);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::populateVertexArrayNames(VertexDataContainer::Pointer vdc)
{
  QList<QString> cellNames;
  if (m_ArrayListType == VertexListType)
  {
    cellNames = vdc->getVertexArrayNameList();
  }
  else if (m_ArrayListType == FieldListType)
  {
    cellNames = vdc->getVertexFieldArrayNameList();
  }
  else if (m_ArrayListType == EnsembleListType)
  {
    cellNames = vdc->getVertexEnsembleArrayNameList();
  }
  m_ComparisonSelectionTableModel->setPossibleFields(cellNames);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::readOptions(QSettings &prefs, QString name)
{
  int count = prefs.beginReadArray(name);
  QVector<QString> arrayNames(count);
  QVector<int>   compOperators(count);
  QVector<float> compValues(count);
  bool ok = false;
  for(int i = 0; i < count; ++i)
  {
    prefs.setArrayIndex(i);
    arrayNames[i] = prefs.value("ArrayName").toString();
    compOperators[i] = prefs.value("CompOperator").toInt(&ok);
    compValues[i] = prefs.value("CompValue").toFloat(&ok);
  }
  prefs.endArray();

  m_ComparisonSelectionTableModel->setTableData(arrayNames, compValues, compOperators);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::writeOptions(QSettings &prefs, QString name)
{
  QVector<QString> arrayNames;
  QVector<int>   compOperators;
  QVector<float> compValues;
  m_ComparisonSelectionTableModel->getTableData(arrayNames, compValues, compOperators);

  int count = arrayNames.count();
  prefs.beginWriteArray(name, count);
  for(int i = 0; i < count; ++i)
  {
    prefs.setArrayIndex(i);
    prefs.setValue("ArrayName", arrayNames[i]);
    prefs.setValue("CompOperator", compOperators[i]);
    prefs.setValue("CompValue", (double)(compValues[i]));
  }
  prefs.endArray();
}

