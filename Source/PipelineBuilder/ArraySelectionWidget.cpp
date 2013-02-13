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

  connect(surfaceMeshVertexArrayList, SIGNAL(	itemChanged ( QListWidgetItem*)),
          this, SLOT(arrayListUpdated(QListWidgetItem*)));
  connect(surfaceMeshFaceArrayList, SIGNAL(	itemChanged ( QListWidgetItem*)),
          this, SLOT(arrayListUpdated(QListWidgetItem*)));
  connect(surfaceMeshEdgeArrayList, SIGNAL(	itemChanged ( QListWidgetItem*)),
          this, SLOT(arrayListUpdated(QListWidgetItem*)));

  connect(solidMeshVertexArrayList, SIGNAL(	itemChanged ( QListWidgetItem*)),
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
void ArraySelectionWidget::updateArrays(VoxelDataContainer::Pointer vdc,
                                               SurfaceMeshDataContainer::Pointer smdc,
                                               SolidMeshDataContainer::Pointer sdc)
{
  updateVoxelArrayNames(vdc);
  updateSurfaceMeshArrayNames(smdc);
  updateSolidMeshArrayNames(sdc);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::updateVoxelArrayNames(VoxelDataContainer::Pointer vdc)
{
  std::list<std::string> cellNames = vdc->getCellArrayNameList();
  updateArrayList(voxelCellArrayList, cellNames, vdc);
  std::set<std::string> nonSelectedArrays = getNonSelectedArrays(voxelCellArrayList);
  for(std::set<std::string>::iterator iter = nonSelectedArrays.begin(); iter != nonSelectedArrays.end(); ++iter)
  {
    vdc->removeCellData(*iter);
  }


  std::list<std::string> fieldNames = vdc->getFieldArrayNameList();
  updateArrayList(voxelFieldArrayList, fieldNames, vdc);
  nonSelectedArrays = getNonSelectedArrays(voxelFieldArrayList);
  for(std::set<std::string>::iterator iter = nonSelectedArrays.begin(); iter != nonSelectedArrays.end(); ++iter)
  {
    vdc->removeFieldData(*iter);
  }

  std::list<std::string> ensembleNames = vdc->getEnsembleArrayNameList();
  updateArrayList(voxelEnsembleArrayList, ensembleNames, vdc);
  nonSelectedArrays = getNonSelectedArrays(voxelEnsembleArrayList);
  for(std::set<std::string>::iterator iter = nonSelectedArrays.begin(); iter != nonSelectedArrays.end(); ++iter)
  {
    vdc->removeEnsembleData(*iter);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::updateSurfaceMeshArrayNames(SurfaceMeshDataContainer::Pointer vdc)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::updateSolidMeshArrayNames(SolidMeshDataContainer::Pointer vdc)
{

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionWidget::updateArrayList(QListWidget* listWidget, std::list<std::string> &arrayNames, VoxelDataContainer::Pointer vdc )
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
  for(std::list<std::string>::iterator iter = arrayNames.begin(); iter != arrayNames.end(); ++iter)
  {
    QString name = QString::fromStdString(*iter);
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

  surfaceMeshVertexArrayList->clear();
  surfaceMeshFaceArrayList->clear();
  surfaceMeshEdgeArrayList->clear();
}

