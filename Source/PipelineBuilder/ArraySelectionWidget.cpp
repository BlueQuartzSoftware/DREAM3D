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
  connect(volumeCellArrayList, SIGNAL(	itemChanged ( QListWidgetItem*)),
          this, SLOT(arrayListUpdated(QListWidgetItem*)));
  connect(volumeFieldArrayList, SIGNAL(	itemChanged ( QListWidgetItem*)),
          this, SLOT(arrayListUpdated(QListWidgetItem*)));
  connect(volumeEnsembleArrayList, SIGNAL(	itemChanged ( QListWidgetItem*)),
          this, SLOT(arrayListUpdated(QListWidgetItem*)));

  connect(surfaceVertexArrayList, SIGNAL(	itemChanged ( QListWidgetItem*)),
          this, SLOT(arrayListUpdated(QListWidgetItem*)));
  connect(surfaceFaceArrayList, SIGNAL(	itemChanged ( QListWidgetItem*)),
          this, SLOT(arrayListUpdated(QListWidgetItem*)));
  connect(surfaceEdgeArrayList, SIGNAL(	itemChanged ( QListWidgetItem*)),
          this, SLOT(arrayListUpdated(QListWidgetItem*)));

  connect(vertexVertexArrayList, SIGNAL(	itemChanged ( QListWidgetItem*)),
          this, SLOT(arrayListUpdated(QListWidgetItem*)));
  connect(vertexFieldArrayList, SIGNAL(	itemChanged ( QListWidgetItem*)),
          this, SLOT(arrayListUpdated(QListWidgetItem*)));
  connect(vertexEnsembleArrayList, SIGNAL(	itemChanged ( QListWidgetItem*)),
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
void ArraySelectionWidget::populateArrayNames(VolumeDataContainer::Pointer vdc,
                                              SurfaceDataContainer::Pointer smdc,
                                              VertexDataContainer::Pointer sdc)
{
  populateVolumeArrayNames(vdc);
  populateSurfaceArrayNames(smdc);
  populateVertexArrayNames(sdc);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::populateVolumeArrayNames(VolumeDataContainer::Pointer vdc)
{
  std::list<std::string> cellNames = vdc->getCellArrayNameList();
  populateArrayList(volumeCellArrayList, cellNames, volumeCellCB);

  std::list<std::string> fieldNames = vdc->getFieldArrayNameList();
  populateArrayList(volumeFieldArrayList, fieldNames, volumeFieldCB);

  std::list<std::string> ensembleNames = vdc->getEnsembleArrayNameList();
  populateArrayList(volumeEnsembleArrayList, ensembleNames, volumeEnsembleCB);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::populateSurfaceArrayNames(SurfaceDataContainer::Pointer vdc)
{
  std::list<std::string> cellNames = vdc->getVertexArrayNameList();
  populateArrayList(surfaceVertexArrayList, cellNames, surfaceVertexArraysCB);

  std::list<std::string> faceNames = vdc->getFaceArrayNameList();
  populateArrayList(surfaceFaceArrayList, faceNames, surfaceFaceArraysCB);

  std::list<std::string> edgeNames = vdc->getEdgeArrayNameList();
  populateArrayList(surfaceEdgeArrayList, edgeNames, surfaceEdgeArraysCB);

  std::list<std::string> fieldNames = vdc->getFieldArrayNameList();
  populateArrayList(surfaceFieldArrayList, fieldNames, surfaceFieldArraysCB);

  std::list<std::string> ensembleNames = vdc->getEnsembleArrayNameList();
  populateArrayList(surfaceEnsembleArrayList, ensembleNames, surfaceEnsembleArraysCB);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::populateVertexArrayNames(VertexDataContainer::Pointer vdc)
{
  std::list<std::string> cellNames = vdc->getVertexArrayNameList();
  populateArrayList(vertexVertexArrayList, cellNames, NULL);

  std::list<std::string> fieldNames = vdc->getFieldArrayNameList();
  populateArrayList(vertexFieldArrayList, fieldNames, NULL);

  std::list<std::string> ensembleNames = vdc->getEnsembleArrayNameList();
  populateArrayList(vertexEnesembleArrayList, ensembleNames, NULL);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::populateArrayList(QListWidget* listWidget, std::list<std::string> &arrayNames, QCheckBox* cb)
{
  // Convert from STL container to Qt Container then pass through to the next method.
  QStringList qArrayNames;
  for(std::list<std::string>::iterator iter = arrayNames.begin(); iter != arrayNames.end(); ++iter)
  {
    qArrayNames << QString::fromStdString(*iter);
  }
  populateArrayList(listWidget, qArrayNames, cb);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::populateArrayList(QListWidget* listWidget, QStringList &arrayNames, QCheckBox* cb)
{
  QStringList selectedArrays;
  int listWidgetCount = listWidget->count();
  // First loop through and get the list of arrays that were selected
  for(qint32 i = 0; i < listWidgetCount; ++i)
  {
    if (listWidget->item(i)->checkState() == Qt::Checked)
    {
      selectedArrays << listWidget->item(i)->text();
    }
  }

  // Now clear the list and start dropping in the array names checking for any previous selections
  int selectedCount = 0;
  listWidget->blockSignals(true);
  listWidget->clear();
  foreach(QString name, arrayNames)
  {
    listWidget->addItem(name);
    Qt::CheckState checked = Qt::Unchecked;
    if (selectedArrays.contains(name) == true) {
      checked = Qt::Checked;
      selectedCount++;
    }
    listWidget->item(listWidget->count() - 1)->setCheckState(checked);
  }

// Now make sure to check the "Select All" checkbox for this type of data
  listWidgetCount = listWidget->count();
  if (cb != NULL && listWidgetCount > 0)
  {
    cb->blockSignals(true);
    cb->setChecked(listWidgetCount == selectedCount);
    cb->blockSignals(false);
  }


//  std::cout << "List Widget: " << listWidget->objectName().toStdString() << " Count: " << listWidget->count() << std::endl;
  listWidget->blockSignals(false);

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::on_surfaceVertexArraysCB_stateChanged(int state)
{
  if (state == Qt::PartiallyChecked) {
    surfaceVertexArraysCB->setCheckState(Qt::Checked);
    state = Qt::Checked;
  }
  toggleListSelections(surfaceVertexArrayList, state);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::on_surfaceFaceArraysCB_stateChanged(int state)
{
  if (state == Qt::PartiallyChecked) {
    surfaceFaceArraysCB->setCheckState(Qt::Checked);
    state = Qt::Checked;
  }
  toggleListSelections(surfaceFaceArrayList, state);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::on_surfaceEdgeArraysCB_stateChanged(int state)
{
  if (state == Qt::PartiallyChecked) {
    surfaceEdgeArraysCB->setCheckState(Qt::Checked);
    state = Qt::Checked;
  }
  toggleListSelections(surfaceEdgeArrayList, state);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::on_surfaceFieldArraysCB_stateChanged(int state)
{
  if (state == Qt::PartiallyChecked) {
    surfaceFieldArraysCB->setCheckState(Qt::Checked);
    state = Qt::Checked;
  }
  toggleListSelections(surfaceFieldArrayList, state);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::on_surfaceEnsembleArraysCB_stateChanged(int state)
{
  if (state == Qt::PartiallyChecked) {
    surfaceEnsembleArraysCB->setCheckState(Qt::Checked);
    state = Qt::Checked;
  }
  toggleListSelections(surfaceEnsembleArrayList, state);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::on_volumeCellCB_stateChanged(int state)
{
  if (state == Qt::PartiallyChecked) {
    volumeCellCB->setCheckState(Qt::Checked);
    state = Qt::Checked;
  }
  toggleListSelections(volumeCellArrayList, state);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::on_volumeFieldCB_stateChanged(int state)
{
  if (state == Qt::PartiallyChecked) {
    volumeFieldCB->setCheckState(Qt::Checked);
    state = Qt::Checked;
  }
  toggleListSelections(volumeFieldArrayList, state);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::on_volumeEnsembleCB_stateChanged(int state)
{
  if (state == Qt::PartiallyChecked) {
    volumeEnsembleCB->setCheckState(Qt::Checked);
    state = Qt::Checked;
  }
  toggleListSelections(volumeEnsembleArrayList, state);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::toggleListSelections(QListWidget* w, int state)
{

  w->blockSignals(true);
  int count = w->count();
  for(int i = 0; i < count; ++i)
  {
      w->item(i)->setCheckState(static_cast<Qt::CheckState>(state));
  }
  w->blockSignals(false);
  // Emit this signal so the preflight runs which will update all the guis after this one
  emit arrayListsChanged();
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
void ArraySelectionWidget::removeSelectionsFromDataContainers(VolumeDataContainer::Pointer vdc,
                                                              SurfaceDataContainer::Pointer smdc,
                                                              VertexDataContainer::Pointer sdc)
{
  REMOVE_ARRAYS_HELPER(volume, vdc, Cell, Selected)
  REMOVE_ARRAYS_HELPER(volume, vdc, Field, Selected)
  REMOVE_ARRAYS_HELPER(volume, vdc, Ensemble, Selected)

  REMOVE_ARRAYS_HELPER(surface, smdc, Vertex, Selected)
  REMOVE_ARRAYS_HELPER(surface, smdc, Face, Selected)
  REMOVE_ARRAYS_HELPER(surface, smdc, Edge, Selected)
  REMOVE_ARRAYS_HELPER(surface, smdc, Field, Selected)
  REMOVE_ARRAYS_HELPER(surface, smdc, Ensemble, Selected)

  REMOVE_ARRAYS_HELPER(vertex, sdc, Vertex, Selected)
  REMOVE_ARRAYS_HELPER(vertex, sdc, Field, Selected)
  REMOVE_ARRAYS_HELPER(vertex, sdc, Ensemble, Selected)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::removeNonSelectionsFromDataContainers(VolumeDataContainer::Pointer vdc,
                                                                 SurfaceDataContainer::Pointer smdc,
                                                                 VertexDataContainer::Pointer sdc)
{
  REMOVE_ARRAYS_HELPER(volume, vdc, Cell, NonSelected)
  REMOVE_ARRAYS_HELPER(volume, vdc, Field, NonSelected)
  REMOVE_ARRAYS_HELPER(volume, vdc, Ensemble, NonSelected)

  REMOVE_ARRAYS_HELPER(surface, smdc, Vertex, NonSelected)
  REMOVE_ARRAYS_HELPER(surface, smdc, Face, NonSelected)
  REMOVE_ARRAYS_HELPER(surface, smdc, Edge, NonSelected)
  REMOVE_ARRAYS_HELPER(surface, smdc, Field, NonSelected)
  REMOVE_ARRAYS_HELPER(surface, smdc, Ensemble, NonSelected)

  REMOVE_ARRAYS_HELPER(vertex, sdc, Vertex, NonSelected)
  REMOVE_ARRAYS_HELPER(vertex, sdc, Field, NonSelected)
  REMOVE_ARRAYS_HELPER(vertex, sdc, Ensemble, NonSelected)
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
void ArraySelectionWidget::setSelectedArrays(std::set<std::string> names, QListWidget*listWidget)
{
  if (names.empty() == true)
  {
    return;
  }
  std::set<std::string>::iterator iter = names.begin();
  for (; iter != names.end(); iter++)
  {
    for(qint32 i = 0; i < listWidget->count(); ++i)
    {
      if (listWidget->item(i)->text().toStdString() == *iter)
      {
        listWidget->item(i)->setCheckState(Qt::Checked);
      }
    }
  }
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
  volumeCellArrayList->clear();
  volumeFieldArrayList->clear();
  volumeEnsembleArrayList->clear();

  surfaceVertexArrayList->clear();
  surfaceFaceArrayList->clear();
  surfaceEdgeArrayList->clear();
  surfaceFieldArrayList->clear();
  surfaceEnsembleArrayList->clear();

  vertexVertexArrayList->clear();
  vertexFieldArrayList->clear();
  vertexEnsembleArrayList->clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::readOptions(QSettings &prefs, QString name)
{
  readSelections(prefs, name, "VolumeCell", volumeCellArrayList);
  readSelections(prefs, name, "VolumeField", volumeFieldArrayList);
  readSelections(prefs, name, "VolumeEnsemble", volumeEnsembleArrayList);

  readSelections(prefs, name, "SurfacePoint", surfaceVertexArrayList);
  readSelections(prefs, name, "SurfaceFace", surfaceFaceArrayList);
  readSelections(prefs, name, "SurfaceEdge", surfaceEdgeArrayList);
  readSelections(prefs, name, "SurfaceField", surfaceFieldArrayList);
  readSelections(prefs, name, "SurfaceEnsemble", surfaceEnsembleArrayList);

  readSelections(prefs, name, "VertexPoint", vertexVertexArrayList);
  readSelections(prefs, name, "VertexField", vertexFieldArrayList);
  readSelections(prefs, name, "VertexEnsemble", vertexEnsembleArrayList);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::writeOptions(QSettings &prefs, QString name)
{
  writeSelections(prefs, name, "VolumeCell", volumeCellArrayList);
  writeSelections(prefs, name, "VolumeField", volumeFieldArrayList);
  writeSelections(prefs, name, "VolumeEnsemble", volumeEnsembleArrayList);

  writeSelections(prefs, name, "SurfacePoint", surfaceVertexArrayList);
  writeSelections(prefs, name, "SurfaceFace", surfaceFaceArrayList);
  writeSelections(prefs, name, "SurfaceEdge", surfaceEdgeArrayList);
  writeSelections(prefs, name, "SurfaceField", surfaceFieldArrayList);
  writeSelections(prefs, name, "SurfaceEnsemble", surfaceEnsembleArrayList);

  writeSelections(prefs, name, "VertexPoint", vertexVertexArrayList);
  writeSelections(prefs, name, "VertexField", vertexFieldArrayList);
  writeSelections(prefs, name, "VertexEnsemble", vertexEnsembleArrayList);
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


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::setVolumeDataEnabled(bool b)
{
  volume_data->setEnabled(b);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::setSurfaceEnabled(bool b)
{
  surface_data->setEnabled(b);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::setVertexEnabled(bool b)
{
  vertex_data->setEnabled(b);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::removeVolumeData()
{
  int index = indexOf(volume_data);
  removeTab(index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::removeSurfaceData()
{
  int index = indexOf(surface_data);
  removeTab(index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::removeVertexData()
{
  int index = indexOf(vertex_data);
  removeTab(index);
}
