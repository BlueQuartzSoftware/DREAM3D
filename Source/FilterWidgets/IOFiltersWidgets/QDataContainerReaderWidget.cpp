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


  QR3DFileCompleter* com = new QR3DFileCompleter(this, true);
  m_InputFile->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString &)),
           this, SLOT(on_m_InputFile_textChanged(const QString &)));

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
  // std::cout << "on_angDirBtn_clicked" << std::endl;
  QString outputFile = this->getOpenDialogLastDirectory() + QDir::separator();
  outputFile = QFileDialog::getExistingDirectory(this, tr("Select EBSD Directory"), outputFile);
  if (!outputFile.isNull())
  {
    m_InputFile->blockSignals(true);
    m_InputFile->setText(QDir::toNativeSeparators(outputFile));
    on_m_InputFile_textChanged(m_InputFile->text());
    getOpenDialogLastDirectory() = outputFile;
    m_InputFile->blockSignals(false);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDataContainerReaderWidget::on_m_InputFile_textChanged(const QString & text)
{
  if (verifyPathExists(m_InputFile->text(), m_InputFile) )
  {

    QDir dir(m_InputFile->text());
    QString dirname = dir.dirName();
    dir.cdUp();

    QString outPath = dir.absolutePath() + QDir::separator() + dirname + "_Output" + QDir::separator() + dirname + ".h5ebsd";
    outPath = QDir::toNativeSeparators(outPath);

    m_InputFile->blockSignals(true);
    m_InputFile->setText(QDir::toNativeSeparators(m_InputFile->text()));
    m_InputFile->blockSignals(false);
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
void QDataContainerReaderWidget::updateVoxelArrayNames(VoxelDataContainer::Pointer vdc)
{
  std::list<std::string> cellNames = vdc->getCellArrayNameList();
  for(std::list<std::string>::iterator iter = cellNames.begin(); iter != cellNames.end(); ++iter)
  {
    voxelCellDataList->addItem(QString::fromStdString(*iter));
    voxelCellDataList->item(voxelCellDataList->count() - 1)->setCheckState(Qt::Unchecked);
  }

    std::list<std::string> fieldNames = vdc->getFieldArrayNameList();
  for(std::list<std::string>::iterator iter = fieldNames.begin(); iter != fieldNames.end(); ++iter)
  {
    voxelFieldDataList->addItem(QString::fromStdString(*iter));
    voxelFieldDataList->item(voxelFieldDataList->count() - 1)->setCheckState(Qt::Unchecked);
  }

      std::list<std::string> ensembleNames = vdc->getEnsembleArrayNameList();
  for(std::list<std::string>::iterator iter = ensembleNames.begin(); iter != ensembleNames.end(); ++iter)
  {
    voxelEnsembleDataList->addItem(QString::fromStdString(*iter));
    voxelEnsembleDataList->item(voxelEnsembleDataList->count() - 1)->setCheckState(Qt::Unchecked);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDataContainerReaderWidget::updateSurfaceMeshArrayNames(SurfaceMeshDataContainer::Pointer smdc)
{

}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDataContainerReaderWidget::updateSolidMeshArrayNames(SolidMeshDataContainer::Pointer sdc)
{

}


