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
#include <QtGui/QCheckBox>

#include "QtSupport/QCheckboxDialog.h"
#include "QtSupport/QR3DFileCompleter.h"
#include "QtSupport/DREAM3DQtMacros.h"
#include "QtSupport/DREAM3DHelpUrlGenerator.h"

#include "H5Support/H5Utilities.h"
#include "H5Support/H5Lite.h"

#include "H5Support/HDF5ScopedFileSentinel.h"
#include "DREAM3DLib/IOFilters/VoxelDataContainerReader.h"


#include "DREAM3DLib/IOFiltersWidgets/moc_QDataContainerReaderWidget.cxx"


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
  m_FilterGroup = QString::fromStdString(filter->getGroupName());
  setupGui();
  getGuiParametersFromFilter( filter.get() );
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
void QDataContainerReaderWidget::getGuiParametersFromFilter(AbstractFilter* filt)
{
  DataContainerReader* filter = DataContainerReader::SafeObjectDownCast<AbstractFilter*, DataContainerReader*>(filt);
  blockSignals(true);
  setInputFile( QString::fromStdString( filter->getInputFile() ) );
  setReadVoxelData( filter->getReadVoxelData() );
  setReadSurfaceMeshData( filter->getReadSurfaceMeshData() );
  setReadSolidMeshData( filter->getReadSolidMeshData() );
  blockSignals(false);
  arraySelectionWidget->blockSignals(true);
  arraySelectionWidget->setArraySelections(filter);
  arraySelectionWidget->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer QDataContainerReaderWidget::getFilter(bool defaultValues)
{
  DataContainerReader::Pointer filter = DataContainerReader::New();
  if (defaultValues == true) { return filter; }
  filter->setInputFile( getInputFile().toStdString() );
  filter->setReadVoxelData( getReadVoxelData() );
  filter->setReadSurfaceMeshData( getReadSurfaceMeshData() );
  filter->setReadSolidMeshData( getReadSolidMeshData() );

  arraySelectionWidget->getArraySelections(filter.get());

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
  InputFile->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString &)),
                    this, SLOT(on_InputFile_textChanged(const QString &)));

  connect(arraySelectionWidget, SIGNAL(arrayListsChanged()),
          this, SLOT(arraySelectionWidgetChanged()));

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDataContainerReaderWidget::arraySelectionWidgetChanged()
{
  emit parametersChanged();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDataContainerReaderWidget::on_ReadVoxelData_stateChanged(int state)
{
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDataContainerReaderWidget::on_ReadSurfaceMeshData_stateChanged(int state)
{
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDataContainerReaderWidget::on_ReadSolidMeshData_stateChanged(int state)
{
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDataContainerReaderWidget::setInputFile(const QString &v)
{
  QString natPath = QDir::toNativeSeparators(v);
  InputFile->setText(natPath);
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString  QDataContainerReaderWidget::getInputFile()
{
  return InputFile->text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDataContainerReaderWidget::writeOptions(QSettings &prefs)
{
  prefs.setValue("Filter_Name", "DataContainerReader" );
  prefs.setValue("InputFile", QDir::toNativeSeparators(getInputFile()) );
  prefs.setValue("ReadVoxelData", getReadVoxelData() );
  prefs.setValue("ReadSurfaceMeshData", getReadSurfaceMeshData() );
  prefs.setValue("ReadSolidMeshData", getReadSolidMeshData() );
  arraySelectionWidget->writeOptions(prefs, "ArraySelections");
}

// -----------------------------------------------------------------------------
//
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
    if (le) {
     le->setChecked(p_ReadSurfaceMeshData.toBool());
    }
  }
  {
    QVariant p_ReadSolidMeshData = prefs.value("ReadSolidMeshData");
    QCheckBox* le = findChild<QCheckBox*>("ReadSolidMeshData");
    if (le) { le->setChecked(p_ReadSolidMeshData.toBool()); }
  }

  arraySelectionWidget->readOptions(prefs, "ArraySelections");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDataContainerReaderWidget::on_InputFileBtn_clicked()
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
void QDataContainerReaderWidget::on_InputFile_textChanged(const QString & text)
{
  if (verifyPathExists(InputFile->text(), InputFile) )
  {
    setInputFile(InputFile->text());
    setOpenDialogLastDirectory(InputFile->text());
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
  // This would only really make sense if the Reader were in the middle of a pipeline then the list
  // would show what is currently in the pipeline
  //  arraySelectionWidget->populateArrayNames(vdc, smdc, sdc);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDataContainerReaderWidget::preflightDoneExecuting(VoxelDataContainer::Pointer vdc, SurfaceMeshDataContainer::Pointer smdc, SolidMeshDataContainer::Pointer sdc)
{
  arraySelectionWidget->populateArrayNames(vdc, smdc, sdc);
  arraySelectionWidget->removeNonSelectionsFromDataContainers(vdc, smdc, sdc);

  // -- This section fills in the GUI elements for the Dims, Res and Origin
  int64_t dims[3] = {0, 0, 0};
  float res[3] = {0.0f, 0.0f, 0.0f};
  float origin[3] = {0.0f, 0.0f, 0.0f};
  m_XDim->setText(QString::number(dims[0]));
  m_YDim->setText(QString::number(dims[1]));
  m_ZDim->setText(QString::number(dims[2]));

  m_XRes->setText(QString::number(res[0]));
  m_YRes->setText(QString::number(res[1]));
  m_ZRes->setText(QString::number(res[2]));

  m_XOrigin->setText(QString::number(origin[0]));
  m_YOrigin->setText(QString::number(origin[1]));
  m_ZOrigin->setText(QString::number(origin[2]));

  if (ReadVoxelData->isChecked() == true)
  {

    hid_t fileId = -1;
    fileId = H5Utilities::openFile(InputFile->text().toStdString(), true);
    if (fileId < 0) {
      return;
    }

    // This should automatically close the file when we leave this function
    //HDF5ScopedFileSentinel sentinel(&fileId, true);

    VoxelDataContainerReader::Pointer reader = VoxelDataContainerReader::New();
    reader->setHdfFileId(fileId);
    int err = reader->getSizeResolutionOrigin(fileId, dims, res, origin);

    if (err < 0) {
      err = H5Utilities::closeFile(fileId);
      return;
    }
    err = H5Utilities::closeFile(fileId);

    m_XDim->setText(QString::number(dims[0]));
    m_YDim->setText(QString::number(dims[1]));
    m_ZDim->setText(QString::number(dims[2]));

    m_XRes->setText(QString::number(res[0]));
    m_YRes->setText(QString::number(res[1]));
    m_ZRes->setText(QString::number(res[2]));

    m_XOrigin->setText(QString::number(origin[0]));
    m_YOrigin->setText(QString::number(origin[1]));
    m_ZOrigin->setText(QString::number(origin[2]));
  }


}


// -----------------------------------------------------------------------------
void QDataContainerReaderWidget::openHtmlHelpFile()
{
  QString lowerFilter = QString("DataContainerReader").toLower();
  DREAM3DHelpUrlGenerator::generateAndOpenHTMLUrl(lowerFilter, this);
}
