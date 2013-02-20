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

#include "QReadH5EbsdWidget.h"

#include <QtGui/QLabel>
#include <QtGui/QMessageBox>
#include <QtGui/QComboBox>

#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/H5EbsdVolumeInfo.h"
#include "EbsdLib/H5EbsdVolumeReader.h"
#include "EbsdLib/TSL/H5AngVolumeReader.h"
#include "EbsdLib/TSL/AngFields.h"
#include "EbsdLib/HKL/H5CtfVolumeReader.h"
#include "EbsdLib/HKL/CtfFields.h"

#include "DREAM3DLib/Common/Constants.h"

#include "QtSupport/QR3DFileCompleter.h"
#include "QtSupport/DREAM3DQtMacros.h"

#include "IOFiltersWidgets/moc_QReadH5EbsdWidget.cxx"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QReadH5EbsdWidget::QReadH5EbsdWidget(QWidget* parent) :
    sampleTransAngle(0.0),
    sampleTransAxis(0.0),
    eulerTransAngle(0.0),
    eulerTransAxis(0.0)
{
  if ( getOpenDialogLastDirectory().isEmpty() )
  {
    setOpenDialogLastDirectory( QDir::homePath() );
  }
  setupUi(this);
  ReadH5Ebsd::Pointer filter = ReadH5Ebsd::New();
  setupGui();
  setTitle(QString::fromStdString(filter->getHumanLabel()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QReadH5EbsdWidget::~QReadH5EbsdWidget()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QReadH5EbsdWidget::getFilterGroup()
{
    return QString::fromStdString(DREAM3D::FilterGroups::GenericFilters);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer QReadH5EbsdWidget::getFilter()
{

  ReadH5Ebsd::Pointer filter =  ReadH5Ebsd::New();

  // Update the Filter with all of these values;
  filter->setH5EbsdFile(m_H5EbsdFile->text().toStdString());
  filter->setZStartIndex(m_ZStartIndex->value());
  filter->setZEndIndex(m_ZEndIndex->value());
  filter->setUseTransformations(m_UseTransformations->isChecked());

  filter->setRefFrameZDir(Ebsd::StackingOrder::Utils::getEnumForString(m_StackingOrder->text().toStdString()));
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterWidget* QReadH5EbsdWidget::createDeepCopy()
{
  #if 0
  QReadH5EbsdWidget* w = new QReadH5EbsdWidget();
  ReadH5Ebsd::Pointer f = ReadH5Ebsd::New();

  // Update the Filter with all of these values;
  f->setH5EbsdFile(m_H5EbsdFile->text().toStdString());
  f->setZStartIndex(m_ZStartIndex->value());
  f->setZEndIndex(m_ZEndIndex->value());

  f->setRefFrameZDir(Ebsd::StackingOrder::Utils::getEnumForString(m_StackingOrder->text().toStdString()));
  return w;
  #endif
  return NULL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QReadH5EbsdWidget::verifyPathExists(QString outFilePath, QLineEdit* lineEdit)
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
void QReadH5EbsdWidget::setupGui()
{
  setCheckable(true);
  setIsSelected(false);

  QR3DFileCompleter* com = new QR3DFileCompleter(this, false);
  m_H5EbsdFile->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString &)),
           this, SLOT(on_m_H5EbsdFile_textChanged(const QString &)));

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QReadH5EbsdWidget::on_m_H5EbsdBtn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Input File"),
                                                 getOpenDialogLastDirectory(),
                                                 tr("HDF5 EBSD Files (*.h5 *.hdf5 *.h5ang *.h5ebsd)") );
  if ( true == file.isEmpty() ){ return; }
  QFileInfo fi (file);
  m_H5EbsdFile->blockSignals(true);
  QString p = QDir::toNativeSeparators(fi.absoluteFilePath());
  m_H5EbsdFile->setText(p);
  on_m_H5EbsdFile_textChanged(m_H5EbsdFile->text() );
  m_H5EbsdFile->blockSignals(false);
  setOpenDialogLastDirectory( fi.path() );
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QReadH5EbsdWidget::on_m_H5EbsdFile_textChanged(const QString &text)
{

  if(verifyPathExists(m_H5EbsdFile->text(), m_H5EbsdFile))
  {
    QFileInfo fi(m_H5EbsdFile->text());
    if(fi.exists() && fi.isFile())
    {
      m_SetSliceInfo();

      // Read the Phase information from the .h5ang file
      H5EbsdVolumeReader::Pointer h5Reader = H5EbsdVolumeReader::New();
      h5Reader->setFileName(m_H5EbsdFile->text().toStdString());
      h5Reader->setSliceStart(m_ZStartIndex->value());

      int err = 0;
      int64_t xpoints = 0;
      int64_t ypoints = 0;
      int64_t zpoints = 1;
      float xres = 0.0f;
      float yres = 0.0f;
      float zres = 0.0f;
      int zStart = 0;
      int zEnd = 0;

      // Read these values from the h5ebsd file in order to put something in the fields.
      zStart = h5Reader->getZStart();
      zEnd = h5Reader->getZEnd();
      err = h5Reader->getResolution(xres, yres, zres);

      h5Reader->setSliceStart(zStart);
      h5Reader->setSliceEnd(zEnd);
      zpoints = 1; // Just read the first dataset and hope the rest are the same
      err = h5Reader->getDimsAndResolution(xpoints, ypoints, zpoints, xres, yres, zres);
      if(err >= 0)
      {
        m_XDim->setText(QString::number(xpoints));
        m_YDim->setText(QString::number(ypoints));
        m_ZDim->setText(QString::number(zpoints));
        m_XRes->setText(QString::number(xres));
        m_YRes->setText(QString::number(yres));
        m_ZRes->setText(QString::number(zres));
      }

      // Compare the Manufactureres of the current file versus the one we have cached
      // If they are different then we need to remove all the quality filters
      QString fileManufact = QString::fromStdString(h5Reader->getManufacturer());

      // Cache the Manufacturer from the File
      m_EbsdManufacturer->setText(fileManufact);

      m_StackingOrder->setText(QString::fromStdString(Ebsd::StackingOrder::Utils::getStringForEnum(h5Reader->getStackingOrder())));

	  sampleTransAngle = h5Reader->getSampleTransformationAngle();
	  sampleTransAxis = h5Reader->getSampleTransformationAxis();
	  eulerTransAngle = h5Reader->getEulerTransformationAngle();
	  eulerTransAxis = h5Reader->getEulerTransformationAxis();

	  QString sampleTrans;
	  QString eulerTrans;
	  sampleTransAxis = h5Reader->getSampleTransformationAxis();
	  eulerTransAxis = h5Reader->getEulerTransformationAxis();
	  sampleTrans = QString::number(h5Reader->getSampleTransformationAngle()) + " @ <" + QString::number(sampleTransAxis[0]) + QString::number(sampleTransAxis[1]) + QString::number(sampleTransAxis[2]) + ">";
	  eulerTrans = QString::number(h5Reader->getEulerTransformationAngle()) + " @ <" + QString::number(eulerTransAxis[0]) + QString::number(eulerTransAxis[1]) + QString::number(eulerTransAxis[2]) + ">";
	  m_SampleTransformationLabel->setText(sampleTrans);
      m_EulerTransformationLabel->setText(eulerTrans);

      setOpenDialogLastDirectory( fi.path() );

      populateCreatedRequiredLists( m_H5EbsdFile->text() );
    }
  }
  else
  {
    m_XDim->setText("xxx");
    m_YDim->setText("xxx");
    m_ZDim->setText("xxx");
    m_XRes->setText("xxx");
    m_YRes->setText("xxx");
    m_ZRes->setText("xxx");
    m_EbsdManufacturer->setText("xxx");
    m_SampleTransformationLabel->setText("xxx");
    m_EulerTransformationLabel->setText("xxx");
    m_ZMin->setText("xxx");
    m_ZMax->setText("xxx");
    m_StackingOrder->setText("xxx");
  }
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QReadH5EbsdWidget::populateCreatedRequiredLists(QString filePath)
{
#if 0
  ReadH5Ebsd::Pointer readH5Ebsd = ReadH5Ebsd::New();
  readH5Ebsd->setH5EbsdFile(filePath.toStdString());
  VoxelDataContainer::Pointer voxelDataContainer = VoxelDataContainer::New();
  readH5Ebsd->setVoxelDataContainer(voxelDataContainer.get());
  readH5Ebsd->preflight(); // This gets all the create/required array names
  m_CreatedArrays->clear();
  std::set<std::string> createdCellData = readH5Ebsd->getCreatedCellData();
  for(std::set<std::string>::iterator i = createdCellData.begin(); i != createdCellData.end(); ++i)
  {
    m_CreatedArrays->addItem(QString::fromStdString(*i));
  }
  #endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QReadH5EbsdWidget::m_SetSliceInfo()
{
  H5EbsdVolumeInfo::Pointer reader = H5EbsdVolumeInfo::New();

  QFileInfo fi(m_H5EbsdFile->text());
  if (fi.isFile() == false)
  {
    return;
  }

  reader->setFileName(m_H5EbsdFile->text().toStdString());
  if (reader->readVolumeInfo() >= 0)
  {
    float x, y, z;
    reader->getResolution(x, y, z);
    m_ZStartIndex->setRange(reader->getZStart(), reader->getZEnd());
    m_ZStartIndex->setValue(reader->getZStart());
    m_ZEndIndex->setRange(reader->getZStart(), reader->getZEnd());
    m_ZEndIndex->setValue(reader->getZEnd());
    m_ZRes->setText(QString::number(z));
    m_ZMin->setText(QString::number(reader->getZStart()));
    m_ZMax->setText(QString::number(reader->getZEnd()));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QReadH5EbsdWidget::readOptions(QSettings &prefs)
{
  QString val;
  bool ok;
  qint32 i;
  bool b;

  READ_FILEPATH_SETTING(prefs, m_, H5EbsdFile, "");
  on_m_H5EbsdFile_textChanged(QString(""));
  READ_SETTING(prefs, m_, ZStartIndex, ok, i, 0, Int)
  READ_SETTING(prefs, m_, ZEndIndex, ok, i, 0, Int)

  
  QVariant UseTrans = prefs.value("UseTransformations");
  m_UseTransformations->setChecked(UseTrans.toBool());


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QReadH5EbsdWidget::writeOptions(QSettings &prefs)
{
  prefs.setValue("Filter_Name", "ReadH5Ebsd" );
  prefs.setValue("H5EbsdFile", m_H5EbsdFile->text());
  prefs.setValue("ZStartIndex", m_ZStartIndex->value());
  prefs.setValue("ZEndIndex", m_ZEndIndex->value() );
  prefs.setValue("UseTransformations",m_UseTransformations->isChecked());
//  prefs.setValue("MisorientationTolerance", m_MisorientationTolerance->value() );


}


