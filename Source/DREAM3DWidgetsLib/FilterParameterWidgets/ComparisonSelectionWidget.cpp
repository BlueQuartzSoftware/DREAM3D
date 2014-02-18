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


#include "DREAM3DWidgetsLib/FilterParameterWidgets/moc_ComparisonSelectionWidget.cxx"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonSelectionWidget::ComparisonSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  QWidget(parent),
  m_ShowOperators(false),
  m_Filter(filter),
  m_FilterParameter(parameter),
  m_DidCausePreflight(false),
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
  QVector<QString> featureNames;
  QVector<float> featureValues;
  QVector<int> featureOperators;
  m_ComparisonSelectionTableModel->getTableData(featureNames, featureValues, featureOperators);

  for(int i = 0; i < filterCount; ++i)
  {
    ComparisonInput_t comp;
    comp.arrayName = featureNames[i];
    comp.compOperator = featureOperators[i];
    comp.compValue = featureValues[i];
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
void ComparisonSelectionWidget::on_addComparison_clicked()
{
  if (!m_ComparisonSelectionTableModel->insertRow(m_ComparisonSelectionTableModel->rowCount())) { return; }

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
  QItemSelectionModel* selectionModel = m_ComparisonSelectionTableView->selectionModel();
  if (!selectionModel->hasSelection()) { return; }
  QModelIndex index = selectionModel->currentIndex();
  if (!index.isValid()) { return; }
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
void ComparisonSelectionWidget::setComparisons(QVector<ComparisonInput_t> comparisons)
{
  qint32 count = comparisons.size();

  QVector<QString> arrayNames(count);
  QVector<int>   compOperators(count);
  QVector<float> compValues(count);
  //bool ok = false;
  for(int i = 0; i < count; ++i)
  {
    arrayNames[i] = (comparisons[i].arrayName);
    compOperators[i] = comparisons[i].compOperator;
    compValues[i] = comparisons[i].compValue;
  }
  m_ComparisonSelectionTableModel->setTableData(arrayNames, compValues, compOperators);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
 // qDebug() << "DataContainerArrayProxyWidget::filterNeedsInputParameters(AbstractFilter* filter)";

  QVariant var;
  //var.setValue(m_DcaProxy);
  bool ok = false;
  // Set the value into the Filter
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, var);
  if(false == ok)
  {
    QString ss = QObject::tr("Filter '%1': Error occurred setting Filter Parameter '%2'").arg(m_Filter->getNameOfClass()).arg(m_FilterParameter->getPropertyName() );
    emit errorSettingFilterParameter(ss);
    qDebug() << ss;
  }

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::beforePreflight()
{
  if (m_DidCausePreflight == false)
  {

  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::afterPreflight()
{
 // qDebug() << m_Filter->getNameOfClass() << " DataContainerArrayProxyWidget::afterPreflight()";
}


#if 0

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSelectionWidget::populateArrayNames(DataContainerArray::Pointer dca)
{
#if __APPLE__
#warning This needs to be fixed
#endif
  if (m_ArrayListType >= CellListType && m_ArrayListType <= FaceListType )
  {
//    populateVolumeArrayNames(vldc);
  }
  else if (m_ArrayListType >= FeatureListType && m_ArrayListType <= FaceListType)
  {
    //populateSurfaceArrayNames(sdc);
  }
  else if (m_ArrayListType >= FeatureListType && m_ArrayListType <= EdgeListType)
  {
    //  populateEdgeArrayNames(edc);
  }
  else if (m_ArrayListType >= FeatureListType && m_ArrayListType <= VertexListType)
  {
    //  populateVertexArrayNames(vdc);
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
  else if (m_ArrayListType == FeatureListType)
  {
    cellNames = vldc->getCellFeatureArrayNameList();
  }
  else if (m_ArrayListType == EnsembleListType)
  {
    cellNames = vldc->getCellEnsembleArrayNameList();
  }
  m_ComparisonSelectionTableModel->setPossibleFeatures(cellNames);
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
  else if (m_ArrayListType == FeatureListType)
  {
    cellNames = sdc->getFaceFeatureArrayNameList();
  }
  else if (m_ArrayListType == EnsembleListType)
  {
    cellNames = sdc->getFaceEnsembleArrayNameList();
  }
  m_ComparisonSelectionTableModel->setPossibleFeatures(cellNames);
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
  else if (m_ArrayListType == FeatureListType)
  {
    cellNames = edc->getEdgeFeatureArrayNameList();
  }
  else if (m_ArrayListType == EnsembleListType)
  {
    cellNames = edc->getEdgeEnsembleArrayNameList();
  }
  m_ComparisonSelectionTableModel->setPossibleFeatures(cellNames);
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
  else if (m_ArrayListType == FeatureListType)
  {
    cellNames = vdc->getVertexFeatureArrayNameList();
  }
  else if (m_ArrayListType == EnsembleListType)
  {
    cellNames = vdc->getVertexEnsembleArrayNameList();
  }
  m_ComparisonSelectionTableModel->setPossibleFeatures(cellNames);
}
#endif
