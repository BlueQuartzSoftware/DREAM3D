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
#include "QtSupport/DREAM3DHelpUrlGenerator.h"

#include "DREAM3DLib/IOFiltersWidgets/moc_QReadH5EbsdWidget.cxx"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QReadH5EbsdWidget::QReadH5EbsdWidget(QWidget* parent) :
  sampleTransAngle(0.0),
  eulerTransAngle(0.0),
  m_Version4Warning(false)
{
  sampleTransAxis.resize(3);
  sampleTransAxis[0] = 0.0;
  sampleTransAxis[1] = 0.0;
  sampleTransAxis[2] = 1.0;

  eulerTransAxis.resize(3);
  eulerTransAxis[0] = 0.0;
  eulerTransAxis[1] = 0.0;
  eulerTransAxis[2] = 1.0;
  if ( getOpenDialogLastDirectory().isEmpty() )
  {
    setOpenDialogLastDirectory( QDir::homePath() );
  }
  setupUi(this);
  ReadH5Ebsd::Pointer filter = ReadH5Ebsd::New();
  setInputFile( (filter->getInputFile() ) );
  m_FilterGroup = (filter->getGroupName());
  setupGui();
  getGuiParametersFromFilter( filter.get() );
  setTitle((filter->getHumanLabel()));
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
  return m_FilterGroup;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QReadH5EbsdWidget::getGuiParametersFromFilter(AbstractFilter* filt)
{
  ReadH5Ebsd* filter = ReadH5Ebsd::SafeObjectDownCast<AbstractFilter*, ReadH5Ebsd*>(filt);

  m_InputFile->setText( ( filter->getInputFile() ) );
  m_ZStartIndex->setValue( filter->getZStartIndex() );
  m_ZEndIndex->setValue( filter->getZEndIndex() );
  m_UseTransformations->setChecked( filter->getUseTransformations() );
  m_RefFrameZDir->setText( ( Ebsd::StackingOrder::Utils::getStringForEnum( filter->getRefFrameZDir() ) ) );

  arraySelectionWidget->setArraySelections(filter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer QReadH5EbsdWidget::getFilter(bool defaultValues)
{
  ReadH5Ebsd::Pointer filter =  ReadH5Ebsd::New();
  if (defaultValues == true) { return filter; }

  filter->setInputFile(m_InputFile->text());
  filter->setZStartIndex(m_ZStartIndex->value());
  filter->setZEndIndex(m_ZEndIndex->value());
  filter->setUseTransformations(m_UseTransformations->isChecked());

  filter->setRefFrameZDir(Ebsd::StackingOrder::Utils::getEnumForString(m_RefFrameZDir->text()));

  arraySelectionWidget->getArraySelections(filter.get());

  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterWidget* QReadH5EbsdWidget::createDeepCopy()
{
  QReadH5EbsdWidget* w = new QReadH5EbsdWidget();
  // Update the Filter with all of these values;
  w->setInputFile(m_InputFile->text());
  w->setZStartIndex(m_ZStartIndex->value());
  w->setZEndIndex(m_ZEndIndex->value());
  w->setRefFrameZDir(m_RefFrameZDir->text());
  return w;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QReadH5EbsdWidget::setupGui()
{
  setCheckable(true);
  setIsSelected(false);


  QR3DFileCompleter* com = new QR3DFileCompleter(this, false);
  m_InputFile->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString &)),
                    this, SLOT(on_m_InputFile_textChanged(const QString &)));

  connect(arraySelectionWidget, SIGNAL(arrayListsChanged()),
          this, SLOT(arraySelectionWidgetChanged()));

  arraySelectionWidget->setSurfaceEnabled(false);
  arraySelectionWidget->setVertexEnabled(false);
  arraySelectionWidget->removeSurfaceData();
  arraySelectionWidget->removeVertexData();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QReadH5EbsdWidget::arraySelectionWidgetChanged()
{
  emit parametersChanged();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QReadH5EbsdWidget::setInputFile(const QString &v)
{
  QString natPath = QDir::toNativeSeparators(v);
  m_InputFile->setText(natPath);
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString  QReadH5EbsdWidget::getInputFile()
{
  return m_InputFile->text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QReadH5EbsdWidget::writeOptions(QSettings &prefs)
{
  prefs.setValue("Filter_Name", "ReadH5Ebsd" );
  prefs.setValue("InputFile", QDir::toNativeSeparators(getInputFile()) );
  prefs.setValue("ZStartIndex", m_ZStartIndex->value());
  prefs.setValue("ZEndIndex", m_ZEndIndex->value() );
  prefs.setValue("UseTransformations",m_UseTransformations->isChecked());
  arraySelectionWidget->writeOptions(prefs, "ArraySelections");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QReadH5EbsdWidget::readOptions(QSettings &prefs)
{
  QString val;
  bool ok;
  qint32 i;

  {
    QVariant p_InputFile = prefs.value("InputFile");
    QString path = QDir::toNativeSeparators(p_InputFile.toString());
    QLineEdit* lb = qFindChild<QLineEdit*>(this, "InputFile");
    if (lb) { lb->setText(path); }
    setInputFile(path);
  }

  READ_SETTING(prefs, m_, ZStartIndex, ok, i, 0, Int)
  READ_SETTING(prefs, m_, ZEndIndex, ok, i, 0, Int)

  QVariant UseTrans = prefs.value("UseTransformations");
  m_UseTransformations->setChecked(UseTrans.toBool());

  arraySelectionWidget->readOptions(prefs, "ArraySelections");

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QReadH5EbsdWidget::on_m_InputFileBtn_clicked()
{
  QObject* whoSent = sender();
  // for QButtons we prepended "btn_" to the end of the property name so strip that off
  QString propName = whoSent->objectName();
  propName = propName.remove(0, 4);

  QString Ftype = getFileType(propName);
  QString ext = getFileExtension(propName);
  QString s = Ftype + QString("HDF5 EBSD Files (*.h5 *.hdf5 *.h5ang *.h5ebsd)");
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
void QReadH5EbsdWidget::on_m_InputFile_textChanged(const QString &text)
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
bool QReadH5EbsdWidget::verifyPathExists(QString outFilePath, QLineEdit* lineEdit)
{
  //  qDebug() << "outFilePath: " << outFilePath() << "\n";
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
void QReadH5EbsdWidget::preflightAboutToExecute(DataContainerArray::Pointer dca)
{
  // This would only really make sense if the Reader were in the middle of a pipeline then the list
  // would show what is currently in the pipeline
  //  arraySelectionWidget->populateArrayNames(vdc, smdc, sdc);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QReadH5EbsdWidget::preflightDoneExecuting(DataContainerArray::Pointer dca)
{
  arraySelectionWidget->populateArrayNames(vldc, sdc, edc, vdc);
  arraySelectionWidget->removeNonSelectionsFromDataContainers(vldc, sdc, edc, vdc);

  updateFileInfoWidgets();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QReadH5EbsdWidget::updateFileInfoWidgets()
{
  if(verifyPathExists(m_InputFile->text(), m_InputFile))
  {
    QFileInfo fi(m_InputFile->text());
    if(fi.exists() && fi.isFile())
    {
      // Read the Phase information from the .h5ang file
      H5EbsdVolumeReader::Pointer h5Reader = H5EbsdVolumeReader::New();
      h5Reader->setFileName(m_InputFile->text());

      float xres = 0.0f;
      float yres = 0.0f;
      float zres = 0.0f;

      int64_t xpoints = 0;
      int64_t ypoints = 0;
      int64_t zpoints = 1;

      int zStart = 0;
      int zEnd = 0;

      if (h5Reader->readVolumeInfo() >= 0)
      {

        h5Reader->getResolution(xres, yres, zres);
        m_XRes->setText(QString::number(xres));
        m_YRes->setText(QString::number(yres));
        m_ZRes->setText(QString::number(zres));

        h5Reader->getDims(xpoints, ypoints, zpoints);
        m_XDim->setText(QString::number(xpoints));
        m_YDim->setText(QString::number(ypoints));
        m_ZDim->setText(QString::number(zpoints));

        zStart = h5Reader->getZStart();
        zEnd = h5Reader->getZEnd();
        m_ZMin->setText(QString::number(zStart));
        m_ZMax->setText(QString::number(zEnd));
        m_ZStartIndex->setRange(zStart, zEnd);
        m_ZStartIndex->setValue(zStart);
        m_ZEndIndex->setRange(zStart, zEnd);
        m_ZEndIndex->setValue(zEnd);

      }
      else
      {
        resetGuiFileInfoWidgets();
      }

      // Compare the Manufactureres of the current file versus the one we have cached
      // If they are different then we need to remove all the quality filters
      QString fileManufact = (h5Reader->getManufacturer());

      // Cache the Manufacturer from the File
      m_EbsdManufacturer->setText(fileManufact);

      m_RefFrameZDir->setText((Ebsd::StackingOrder::Utils::getStringForEnum(h5Reader->getStackingOrder())));

      if (h5Reader->getFileVersion() == 4 && m_Version4Warning == false)
      {
        QMessageBox msgBox;
        msgBox.setText("H5Ebsd File Needs Updating");
        QString iText("Due to a bug in earlier versions of DREAM3D the default transformation values for TSL EBSD data were incorrect.\n");
        iText.append(QString("This may cause results obtained through DREAM3D or another analysis program to be incorrect. We suggest that you reimport your .ang files into an .h5ebsd file to correct this error."));
        msgBox.setInformativeText(iText);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        m_Version4Warning = true;
//        if (QMessageBox::Ok == ret)
//        {
//          h5Reader->updateToLatestVersion();
//        }
      }
      else if (h5Reader->getFileVersion() < 4)
      {
        QMessageBox msgBox;
        msgBox.setText("H5Ebsd File Needs Updating");
        msgBox.setInformativeText("The transformation information stored in the file does not meet the latest specification.\nShould DREAM3D update the file for you?\n  If you select NOT to do this operation DREAM3D can not use this file.");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        if (QMessageBox::Ok == ret)
        {
          h5Reader->updateToLatestVersion();
        }
      }

      // Now test again to see if we are at the correct file version
      if (h5Reader->getFileVersion() >= 4)
      {
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
      }
    }
  }
  else
  {
    resetGuiFileInfoWidgets();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QReadH5EbsdWidget::resetGuiFileInfoWidgets()
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
  m_RefFrameZDir->setText("xxx");
}

// -----------------------------------------------------------------------------
void QReadH5EbsdWidget::openHtmlHelpFile()
{
  QString lowerFilter = QString("ReadH5Ebsd").toLower();
  DREAM3DHelpUrlGenerator::generateAndOpenHTMLUrl(lowerFilter, this);
}

