/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "QDataContainerReaderWidget.h"
#include <QtCore/QDir>

#include <QtGui/QFileDialog>
#include <QtGui/QCheckbox>


#include "QtSupport/QCheckboxDialog.h"
#include "QtSupport/QR3DFileCompleter.h"
#include "QtSupport/DREAM3DQtMacros.h"


#include "IOFiltersWidgets/moc_QDataContainerReaderWidget.cxx"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QDataContainerReaderWidget::QDataContainerReaderWidget(QWidget* parent) :
  QFilterWidget(parent)
{
  if ( getOpenDialogLastDirectory().isEmpty() )
  {
    setOpenDialogLastDirectory( QDir::homePath() );
  }
  setupUi(this);
  DataContainerReader::Pointer filter = DataContainerReader::New();
  setInputFile( QString::fromStdString(filter->getInputFile() ) );
  setReadVoxelData( filter->getReadVoxelData() );
  setReadSurfaceMeshData( filter->getReadSurfaceMeshData() );
  m_FilterGroup = QString::fromStdString(filter->getGroupName());
  setupGui();
  setTitle(QString::fromStdString(filter->getHumanLabel()));
}

// -----------------------------------------------------------------------------
QDataContainerReaderWidget::~QDataContainerReaderWidget()
{
}

// -----------------------------------------------------------------------------
QString QDataContainerReaderWidget::getFilterGroup()
{
  return m_FilterGroup;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer QDataContainerReaderWidget::getFilter()
{
  DataContainerReader::Pointer filter = DataContainerReader::New();
  filter->setInputFile( getInputFile().toStdString() );
  filter->setReadVoxelData( getReadVoxelData() );
  filter->setReadSurfaceMeshData( getReadSurfaceMeshData() );

  filter->setVoxelSelectedArrayNames( getSelectedArrays(voxelCellArrayList), getSelectedArrays(voxelFieldArrayList), getSelectedArrays(voxelEnsembleArrayList));
  filter->setSurfaceMeshSelectedArrayNames( getSelectedArrays(surfaceMeshVertexArrayList), getSelectedArrays(surfaceMeshFaceArrayList), getSelectedArrays(surfaceMeshEdgeArrayList));
  filter->setSolidMeshSelectedArrayNames( getSelectedArrays(solidMeshVertexArrayList), getSelectedArrays(solidMeshFaceArrayList), getSelectedArrays(solidMeshEdgeArrayList));
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterWidget* QDataContainerReaderWidget::createDeepCopy()
{
  QDataContainerReaderWidget* w = new QDataContainerReaderWidget(NULL);
  w->setInputFile( getInputFile() );
  w->setReadVoxelData( getReadVoxelData() );
  w->setReadSurfaceMeshData( getReadSurfaceMeshData() );
  return w;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDataContainerReaderWidget::setupGui()
{
  setCheckable(true);
  setIsSelected(false);


  QR3DFileCompleter* com = new QR3DFileCompleter(this, false);
  m_InputFile->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString &)),
                    this, SLOT(on_m_InputFile_textChanged(const QString &)));



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

}

// -----------------------------------------------------------------------------
void QDataContainerReaderWidget::setInputFile(const QString &v)
{
  QString natPath = QDir::toNativeSeparators(v);
  m_InputFile->setText(natPath);
  emit parametersChanged();
}
// -----------------------------------------------------------------------------
QString  QDataContainerReaderWidget::getInputFile()
{
  return m_InputFile->text();
}
// -----------------------------------------------------------------------------
void QDataContainerReaderWidget::writeOptions(QSettings &prefs)
{
  prefs.setValue("Filter_Name", "DataContainerReader" );
  prefs.setValue("InputFile", QDir::toNativeSeparators(getInputFile()) );
  prefs.setValue("ReadVoxelData", getReadVoxelData() );
  prefs.setValue("ReadSurfaceMeshData", getReadSurfaceMeshData() );
}
// -----------------------------------------------------------------------------
void QDataContainerReaderWidget::readOptions(QSettings &prefs)
{
  {
    QVariant p_InputFile = prefs.value("InputFile");
    QString path = QDir::toNativeSeparators(p_InputFile.toString());
    QLineEdit* lb = qFindChild<QLineEdit*>(this, "InputFile");
    if (lb) { lb->setText(path); }
    setInputFile(path);
  }
  {
    QVariant p_ReadVoxelData = prefs.value("ReadVoxelData");
    QCheckBox* le = findChild<QCheckBox*>("ReadVoxelData");
    if (le) { le->setChecked(p_ReadVoxelData.toBool()); }
  }
  {
    QVariant p_ReadSurfaceMeshData = prefs.value("ReadSurfaceMeshData");
    QCheckBox* le = findChild<QCheckBox*>("ReadSurfaceMeshData");
    if (le) { le->setChecked(p_ReadSurfaceMeshData.toBool()); }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDataContainerReaderWidget::on_m_InputFileBtn_clicked()
{
  QObject* whoSent = sender();
  // for QButtons we prepended "btn_" to the end of the property name so strip that off
  QString propName = whoSent->objectName();
  propName = propName.remove(0, 4);

  QString Ftype = getFileType(propName.toStdString());
  QString ext = getFileExtension(propName.toStdString());
  QString s = Ftype + QString("DREAM3D Files (*.dream3d *.h5 *.hdf5);;All Files(*.*)");
  QString defaultName = getOpenDialogLastDirectory();
  QString inputFile = QFileDialog::getOpenFileName(this, tr("Select Input File"), defaultName, s);
  if(true == inputFile.isEmpty())
  {
    return;
  }
  // Store the last used directory into the private instance variable
  inputFile = QDir::toNativeSeparators(inputFile);
  if (!inputFile.isNull())
  {
    setInputFile(inputFile);
    setOpenDialogLastDirectory(inputFile);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDataContainerReaderWidget::on_m_InputFile_textChanged(const QString & text)
{
  if (verifyPathExists(m_InputFile->text(), m_InputFile) )
  {
    setInputFile(m_InputFile->text());
    setOpenDialogLastDirectory(m_InputFile->text());
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QDataContainerReaderWidget::verifyPathExists(QString outFilePath, QLineEdit* lineEdit)
{
  //  std::cout << "outFilePath: " << outFilePath.toStdString() << std::endl;
  QFileInfo fileinfo(outFilePath);
  if (false == fileinfo.exists() )
  {
    lineEdit->setStyleSheet("border: 1px solid red;");
  }
  else
  {
    lineEdit->setStyleSheet("");
  }
  return fileinfo.exists();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDataContainerReaderWidget::preflightAboutToExecute(VoxelDataContainer::Pointer vdc, SurfaceMeshDataContainer::Pointer smdc, SolidMeshDataContainer::Pointer sdc)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDataContainerReaderWidget::preflightDoneExecuting(VoxelDataContainer::Pointer vdc, SurfaceMeshDataContainer::Pointer smdc, SolidMeshDataContainer::Pointer sdc)
{
  updateVoxelArrayNames(vdc);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDataContainerReaderWidget::arrayListUpdated(QListWidgetItem *item)
{
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDataContainerReaderWidget::updateVoxelArrayNames(VoxelDataContainer::Pointer vdc)
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
void QDataContainerReaderWidget::updateSurfaceMeshArrayNames(SurfaceMeshDataContainer::Pointer vdc)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDataContainerReaderWidget::updateSolidMeshArrayNames(SolidMeshDataContainer::Pointer vdc)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDataContainerReaderWidget::updateArrayList(QListWidget* listWidget, std::list<std::string> &arrayNames, VoxelDataContainer::Pointer vdc )
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
std::set<std::string> QDataContainerReaderWidget::getSelectedArrays(QListWidget*listWidget)
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
std::set<std::string> QDataContainerReaderWidget::getNonSelectedArrays(QListWidget*listWidget)
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
void QDataContainerReaderWidget::clearArraySelectionLists()
{
  voxelCellArrayList->clear();
  voxelFieldArrayList->clear();
  voxelEnsembleArrayList->clear();

  surfaceMeshVertexArrayList->clear();
  surfaceMeshFaceArrayList->clear();
  surfaceMeshEdgeArrayList->clear();
}
