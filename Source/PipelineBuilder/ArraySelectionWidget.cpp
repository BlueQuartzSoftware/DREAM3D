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

#include "ArraySelectionWidget.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ArraySelectionWidget::ArraySelectionWidget(QWidget *parent) :
  QTabWidget(parent)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ArraySelectionWidget::~ArraySelectionWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::setupGui()
{
  connect(voxelCellArrayList, SIGNAL(	itemChanged ( QListWidgetItem*)),
          this, SLOT(arrayListUpdated(QListWidgetItem*)));
  connect(voxelFieldArrayList, SIGNAL(	itemChanged ( QListWidgetItem*)),
          this, SLOT(arrayListUpdated(QListWidgetItem*)));
  connect(voxelEnsembleArrayList, SIGNAL(	itemChanged ( QListWidgetItem*)),
          this, SLOT(arrayListUpdated(QListWidgetItem*)));

  connect(surfaceMeshPointArrayList, SIGNAL(	itemChanged ( QListWidgetItem*)),
          this, SLOT(arrayListUpdated(QListWidgetItem*)));
  connect(surfaceMeshFaceArrayList, SIGNAL(	itemChanged ( QListWidgetItem*)),
          this, SLOT(arrayListUpdated(QListWidgetItem*)));
  connect(surfaceMeshEdgeArrayList, SIGNAL(	itemChanged ( QListWidgetItem*)),
          this, SLOT(arrayListUpdated(QListWidgetItem*)));

  connect(solidMeshPointArrayList, SIGNAL(	itemChanged ( QListWidgetItem*)),
          this, SLOT(arrayListUpdated(QListWidgetItem*)));
  connect(solidMeshFaceArrayList, SIGNAL(	itemChanged ( QListWidgetItem*)),
          this, SLOT(arrayListUpdated(QListWidgetItem*)));
  connect(solidMeshEdgeArrayList, SIGNAL(	itemChanged ( QListWidgetItem*)),
          this, SLOT(arrayListUpdated(QListWidgetItem*)));

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::arrayListUpdated(QListWidgetItem *item)
{
  emit arrayListsChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::populateArrayNames(VoxelDataContainer::Pointer vdc,
                                              SurfaceMeshDataContainer::Pointer smdc,
                                              SolidMeshDataContainer::Pointer sdc)
{
  populateVoxelArrayNames(vdc);
  populateSurfaceMeshArrayNames(smdc);
  populateSolidMeshArrayNames(sdc);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::populateVoxelArrayNames(VoxelDataContainer::Pointer vdc)
{
  std::list<std::string> cellNames = vdc->getCellArrayNameList();
  populateArrayList(voxelCellArrayList, cellNames);

  std::list<std::string> fieldNames = vdc->getFieldArrayNameList();
  populateArrayList(voxelFieldArrayList, fieldNames);

  std::list<std::string> ensembleNames = vdc->getEnsembleArrayNameList();
  populateArrayList(voxelEnsembleArrayList, ensembleNames);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::populateSurfaceMeshArrayNames(SurfaceMeshDataContainer::Pointer vdc)
{
  std::list<std::string> cellNames = vdc->getPointArrayNameList();
  populateArrayList(surfaceMeshPointArrayList, cellNames);

  std::list<std::string> fieldNames = vdc->getFaceArrayNameList();
  populateArrayList(surfaceMeshFaceArrayList, fieldNames);

  std::list<std::string> ensembleNames = vdc->getEdgeArrayNameList();
  populateArrayList(surfaceMeshEdgeArrayList, ensembleNames);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::populateSolidMeshArrayNames(SolidMeshDataContainer::Pointer vdc)
{
  std::list<std::string> cellNames = vdc->getPointArrayNameList();
  populateArrayList(solidMeshPointArrayList, cellNames);

  std::list<std::string> fieldNames = vdc->getFaceArrayNameList();
  populateArrayList(solidMeshFaceArrayList, fieldNames);

  std::list<std::string> ensembleNames = vdc->getEdgeArrayNameList();
  populateArrayList(solidMeshEdgeArrayList, ensembleNames);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::populateArrayList(QListWidget* listWidget, std::list<std::string> &arrayNames)
{
  // Convert from STL container to Qt Container then pass through to the next method.
  QStringList qArrayNames;
  for(std::list<std::string>::iterator iter = arrayNames.begin(); iter != arrayNames.end(); ++iter)
  {
    qArrayNames << QString::fromStdString(*iter);
  }
  populateArrayList(listWidget, qArrayNames);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::populateArrayList(QListWidget* listWidget, QStringList &arrayNames)
{
  QStringList selectedArrays;
  for(qint32 i = 0; i < listWidget->count(); ++i)
  {
    if (listWidget->item(i)->checkState() == Qt::Checked)
    {
      selectedArrays << listWidget->item(i)->text();
    }
  }
  listWidget->blockSignals(true);
  listWidget->clear();
  foreach(QString name, arrayNames)
  {
    listWidget->addItem(name);
    Qt::CheckState checked = Qt::Unchecked;
    if (selectedArrays.contains(name) == true) { checked = Qt::Checked; }
    listWidget->item(listWidget->count() - 1)->setCheckState(checked);
  }
  listWidget->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::setSelections(QListWidget* listWidget, QStringList &selections)
{
  listWidget->blockSignals(true);
  int count = listWidget->count();
  for(int i = 0; i < count; ++i)
  {
    QString name = listWidget->item(i)->text();
    if (selections.contains(name) == true) {
      listWidget->item(i)->setCheckState(Qt::Checked);
    }
  }
  listWidget->blockSignals(false);
}


#define REMOVE_ARRAYS_HELPER(dtype, dataContainer, grouping, op)\
{std::set<std::string> arrays = get##op##Arrays(dtype##grouping##ArrayList);\
  for(std::set<std::string>::iterator iter = arrays.begin(); iter != arrays.end(); ++iter) {\
  dataContainer->remove##grouping##Data(*iter);\
  }}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::removeSelectionsFromDataContainers(VoxelDataContainer::Pointer vdc,
                                                              SurfaceMeshDataContainer::Pointer smdc,
                                                              SolidMeshDataContainer::Pointer sdc)
{
  REMOVE_ARRAYS_HELPER(voxel, vdc, Cell, Selected)
  REMOVE_ARRAYS_HELPER(voxel, vdc, Field, Selected)
  REMOVE_ARRAYS_HELPER(voxel, vdc, Ensemble, Selected)

  REMOVE_ARRAYS_HELPER(surfaceMesh, smdc, Point, Selected)
  REMOVE_ARRAYS_HELPER(surfaceMesh, smdc, Face, Selected)
  REMOVE_ARRAYS_HELPER(surfaceMesh, smdc, Edge, Selected)

  REMOVE_ARRAYS_HELPER(solidMesh, sdc, Point, Selected)
  REMOVE_ARRAYS_HELPER(solidMesh, sdc, Face, Selected)
  REMOVE_ARRAYS_HELPER(solidMesh, sdc, Edge, Selected)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::removeNonSelectionsFromDataContainers(VoxelDataContainer::Pointer vdc,
                                                                 SurfaceMeshDataContainer::Pointer smdc,
                                                                 SolidMeshDataContainer::Pointer sdc)
{
  REMOVE_ARRAYS_HELPER(voxel, vdc, Cell, NonSelected)
  REMOVE_ARRAYS_HELPER(voxel, vdc, Field, NonSelected)
  REMOVE_ARRAYS_HELPER(voxel, vdc, Ensemble, NonSelected)

  REMOVE_ARRAYS_HELPER(surfaceMesh, smdc, Point, NonSelected)
  REMOVE_ARRAYS_HELPER(surfaceMesh, smdc, Face, NonSelected)
  REMOVE_ARRAYS_HELPER(surfaceMesh, smdc, Edge, NonSelected)

  REMOVE_ARRAYS_HELPER(solidMesh, sdc, Point, NonSelected)
  REMOVE_ARRAYS_HELPER(solidMesh, sdc, Face, NonSelected)
  REMOVE_ARRAYS_HELPER(solidMesh, sdc, Edge, NonSelected)
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::set<std::string> ArraySelectionWidget::getSelectedArrays(QListWidget*listWidget)
{
  std::set<std::string> selectedArrays;
  for(qint32 i = 0; i < listWidget->count(); ++i)
  {
    if (listWidget->item(i)->checkState() == Qt::Checked)
    {
      selectedArrays.insert(listWidget->item(i)->text().toStdString());
    }
  }
  return selectedArrays;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::set<std::string> ArraySelectionWidget::getNonSelectedArrays(QListWidget*listWidget)
{
  std::set<std::string> non_selectedArrays;
  for(qint32 i = 0; i < listWidget->count(); ++i)
  {
    if (listWidget->item(i)->checkState() == Qt::Unchecked)
    {
      non_selectedArrays.insert(listWidget->item(i)->text().toStdString());
    }
  }
  return non_selectedArrays;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::clearArraySelectionLists()
{
  voxelCellArrayList->clear();
  voxelFieldArrayList->clear();
  voxelEnsembleArrayList->clear();

  surfaceMeshPointArrayList->clear();
  surfaceMeshFaceArrayList->clear();
  surfaceMeshEdgeArrayList->clear();

  solidMeshPointArrayList->clear();
  solidMeshFaceArrayList->clear();
  solidMeshEdgeArrayList->clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::readOptions(QSettings &prefs, QString name)
{
  readSelections(prefs, name, "VoxelCell", voxelCellArrayList);
  readSelections(prefs, name, "VoxelField", voxelFieldArrayList);
  readSelections(prefs, name, "VoxelEnsemble", voxelEnsembleArrayList);

  readSelections(prefs, name, "SurfaceMeshPoint", surfaceMeshPointArrayList);
  readSelections(prefs, name, "SurfaceMeshFace", surfaceMeshFaceArrayList);
  readSelections(prefs, name, "SurfaceMeshEdge", surfaceMeshEdgeArrayList);

  readSelections(prefs, name, "SolidMeshPoint", solidMeshPointArrayList);
  readSelections(prefs, name, "SolidMeshFace", solidMeshFaceArrayList);
  readSelections(prefs, name, "SolidMeshEnsemble", solidMeshEdgeArrayList);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::writeOptions(QSettings &prefs, QString name)
{
  writeSelections(prefs, name, "VoxelCell", voxelCellArrayList);
  writeSelections(prefs, name, "VoxelField", voxelFieldArrayList);
  writeSelections(prefs, name, "VoxelEnsemble", voxelEnsembleArrayList);

  writeSelections(prefs, name, "SurfaceMeshPoint", surfaceMeshPointArrayList);
  writeSelections(prefs, name, "SurfaceMeshFace", surfaceMeshFaceArrayList);
  writeSelections(prefs, name, "SurfaceMeshEdge", surfaceMeshEdgeArrayList);

  writeSelections(prefs, name, "SolidMeshPoint", solidMeshPointArrayList);
  writeSelections(prefs, name, "SolidMeshFace", solidMeshFaceArrayList);
  writeSelections(prefs, name, "SolidMeshEnsemble", solidMeshEdgeArrayList);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::writeSelections(QSettings &prefs, QString name, QString prefix, QListWidget* widget)
{

  std::set<std::string> selections = getSelectedArrays(widget);
  int count = selections.size();
  prefs.beginWriteArray(name + "_" + prefix, count);
  count = 0;
  for(std::set<std::string>::iterator iter = selections.begin(); iter != selections.end(); ++iter)
  {
    prefs.setArrayIndex(count++);
    prefs.setValue(prefix, QString::fromStdString(*iter));
  }
  prefs.endArray();
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::readSelections(QSettings &prefs, QString name, QString prefix, QListWidget* widget)
{
  int count = prefs.beginReadArray(name + "_" + prefix);
  QStringList selections;
  for(int i = 0; i < count; ++i)
  {
    prefs.setArrayIndex(i);
    QString str = prefs.value(prefix, QString::fromUtf8("NOT_FOUND")).toString();
    selections.push_back(str);
  }
  prefs.endArray();
  populateArrayList(widget, selections);
  setSelections(widget, selections);
}

