/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ReadH5EbsdWidget.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QComboBox>

#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/H5EbsdVolumeInfo.h"
#include "EbsdLib/H5EbsdVolumeReader.h"
#include "EbsdLib/TSL/H5AngVolumeReader.h"
#include "EbsdLib/TSL/AngFields.h"
#include "EbsdLib/HKL/H5CtfVolumeReader.h"
#include "EbsdLib/HKL/CtfFields.h"

#include "SIMPLib/Common/Constants.h"


#include "SVWidgetsLib/QtSupport/QtSFileCompleter.h"
#include "SVWidgetsLib/QtSupport/QtSMacros.h"
#include "SVWidgetsLib/QtSupport/QtSHelpUrlGenerator.h"


#include "OrientationAnalysis/OrientationAnalysisFilters/ReadH5Ebsd.h"
#include "OrientationAnalysis/FilterParameters/ReadH5EbsdFilterParameter.h"

// Initialize private static member variable
QString ReadH5EbsdWidget::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadH5EbsdWidget::ReadH5EbsdWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent),
  m_Version4Warning(false),
  m_DidCausePreflight(false),
  m_NewFileLoaded(false)
{
  m_FilterParameter = dynamic_cast<ReadH5EbsdFilterParameter*>(parameter);

  m_SampleTransformation.angle = 0.0f;
  m_SampleTransformation.h = 0.0f;
  m_SampleTransformation.k = 0.0f;
  m_SampleTransformation.l = 1.0f;

  m_EulerTransformation.angle = 0.0f;
  m_EulerTransformation.h = 0.0f;
  m_EulerTransformation.k = 0.0f;
  m_EulerTransformation.l = 1.0f;
  m_Filter = qobject_cast<ReadH5Ebsd*>(filter);
  Q_ASSERT_X(NULL != m_Filter, "ReadH5EbsdWidget can ONLY be used with ReadH5Ebsd filter", __FILE__);

  qRegisterMetaType<QSet<QString> >("QSet<QString>");

  if ( getOpenDialogLastDirectory().isEmpty() )
  {
    setOpenDialogLastDirectory( QDir::homePath() );
  }
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadH5EbsdWidget::~ReadH5EbsdWidget()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5EbsdWidget::setFilter(AbstractFilter* value)
{
  m_Filter = dynamic_cast<ReadH5Ebsd*>(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter* ReadH5EbsdWidget::getFilter() const
{
  return m_Filter;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5EbsdWidget::setFilterParameter(FilterParameter* value)
{
  m_FilterParameter = dynamic_cast<ReadH5EbsdFilterParameter*>(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter* ReadH5EbsdWidget::getFilterParameter() const
{
  return m_FilterParameter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5EbsdWidget::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList)
  {
    w->setEnabled(b);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5EbsdWidget::setupGui()
{

  // Catch when the filter is about to execute the preflight
  connect(m_Filter, SIGNAL(preflightAboutToExecute()),
          this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(m_Filter, SIGNAL(preflightExecuted()),
          this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(m_Filter, SIGNAL(updateFilterParameters(AbstractFilter*)),
          this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  QtSFileCompleter* com = new QtSFileCompleter(this, false);
  m_InputFile->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString&)),
                    this, SLOT(on_m_InputFile_textChanged(const QString&)));

  validateInputFile();

  // Setup the GUI widgets from what ever is in the Filter instance


  m_InputFile->setText( m_Filter->getInputFile() );
  // Set the initial range based on the z min & z max. This will get adjusted later
  m_ZStartIndex->setRange(0, m_Filter->getZEndIndex());
  m_ZStartIndex->setValue( m_Filter->getZStartIndex() );
  // Set the initial range based on the z min & z max. This will get adjusted later
  m_ZEndIndex->setRange(0, m_Filter->getZEndIndex());
  m_ZEndIndex->setValue( m_Filter->getZEndIndex() );

  m_UseTransformations->setChecked( m_Filter->getUseTransformations() );
  m_AngleRepresentationCB->setCurrentIndex(m_Filter->getAngleRepresentation());
  m_RefFrameZDir->setText( Ebsd::StackingOrder::Utils::getStringForEnum( m_Filter->getRefFrameZDir() )  );
  updateFileInfoWidgets();
  QSet<QString> selectedArrays = m_Filter->getSelectedArrayNames();
  updateModelFromFilter(selectedArrays, true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5EbsdWidget::validateInputFile()
{
  QString currentPath = m_Filter->getInputFile();
  QFileInfo fi(currentPath);
  if (currentPath.isEmpty() == false && fi.exists() == false)
  {
    QString Ftype = m_FilterParameter->getFileType();
    QString ext = m_FilterParameter->getFileExtension();
    QString s = Ftype + QString(" Files (") + ext + QString(");;All Files(*.*)");
    QString defaultName = m_OpenDialogLastDirectory + QDir::separator() + "Untitled";


    QString title = QObject::tr("Select a replacement input file in filter '%2'").arg(m_Filter->getHumanLabel());

    QString file = QFileDialog::getOpenFileName(this, title, defaultName, s);
    if(true == file.isEmpty())
    {
      file = currentPath;
    }
    file = QDir::toNativeSeparators(file);
    // Store the last used directory into the private instance variable
    QFileInfo fi(file);
    m_OpenDialogLastDirectory = fi.path();
    m_Filter->setInputFile(file);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5EbsdWidget::on_m_InputFileBtn_clicked()
{
  QObject* whoSent = sender();
  // for QButtons we prepended "btn_" to the end of the property name so strip that off
  QString propName = whoSent->objectName();
  propName = propName.remove(0, 4);

  QString Ftype = m_FilterParameter->getFileType();
  QString ext = m_FilterParameter->getFileExtension();
  QString s = Ftype + QString("HDF5 EBSD Files (*.h5 *.hdf5 *.h5ang *.h5ebsd)");
  QString defaultName = getOpenDialogLastDirectory();
  QString inputFile = QFileDialog::getOpenFileName(this, tr("Select H5Ebsd Input File"), defaultName, s);
  if(true == inputFile.isEmpty())
  {
    return;
  }
  // Store the last used directory into the private instance variable
  inputFile = QDir::toNativeSeparators(inputFile);
  if (!inputFile.isNull())
  {
    m_InputFile->setText(inputFile); // Should cause a signal to be emitted
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5EbsdWidget::on_m_InputFile_textChanged(const QString& text)
{
  setOpenDialogLastDirectory(m_InputFile->text());
  verifyPathExists(m_InputFile->text(), m_InputFile);

  m_NewFileLoaded = true;
  // Clear the QListWidget
  m_CellList->blockSignals(true);
  m_CellList->clear();
  m_CellList->blockSignals(false);

  m_DidCausePreflight = true;

  // We need to send the file down to the filter BEFORE any of the preflight starts because it needs this updated file
  m_Filter->setInputFile(m_InputFile->text());
  // Once the input file is changed then kick off the prefligth by emitting the parametersChanged() signal
  emit parametersChanged();
  m_DidCausePreflight = false;
  m_NewFileLoaded = false;

}

// -----------------------------------------------------------------------------
// Any time something gets clicked, emit the preflight
// -----------------------------------------------------------------------------
void ReadH5EbsdWidget::on_m_CellList_itemClicked(QListWidgetItem* item)
{
  m_DidCausePreflight = true;
  int checkCount = 0;
  int count = m_CellList->count();
  for(int i = 0; i < count; i++)
  {
    if (m_CellList->item(i)->checkState() == Qt::Checked)
    {
      checkCount++;
    }
  }
  m_DataArraysCheckBox->blockSignals(true);
  if (count == checkCount)
  {
    m_DataArraysCheckBox->setCheckState(Qt::Checked );
  }
  else if (checkCount > 0 && checkCount < count)
  {
    m_DataArraysCheckBox->setCheckState(Qt::PartiallyChecked );
  }
  else
  {
    m_DataArraysCheckBox->setCheckState(Qt::Unchecked );
  }
  m_DataArraysCheckBox->blockSignals(false);
  emit parametersChanged();

  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5EbsdWidget::on_m_DataArraysCheckBox_stateChanged(int state)
{
  m_DidCausePreflight = true;

  // Qt::CheckState checked = Qt::Unchecked;
  // If the user made this checkbox partially selected then make it fully selected
  if(Qt::PartiallyChecked == state)
  {
    state = (int)(Qt::Checked);
    m_DataArraysCheckBox->blockSignals(true);
    m_DataArraysCheckBox->setCheckState(Qt::CheckState(state) );
    m_DataArraysCheckBox->blockSignals(false);
  }


  int count = m_CellList->count();
  for(int i = 0; i < count; i++)
  {
    m_CellList->item(i)->setCheckState(Qt::CheckState(state));
  }
  emit parametersChanged();

  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5EbsdWidget::on_m_ZStartIndex_valueChanged(int value)
{
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5EbsdWidget::on_m_ZEndIndex_valueChanged(int value)
{
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5EbsdWidget::on_m_UseTransformations_stateChanged(int state)
{
  m_DidCausePreflight = true;
  m_AngleRepresentationCB->setEnabled(m_UseTransformations->isChecked());
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5EbsdWidget::on_m_AngleRepresentationCB_currentIndexChanged(int index)
{
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ReadH5EbsdWidget::verifyPathExists(QString outFilePath, QLineEdit* lineEdit)
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
void ReadH5EbsdWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  if (NULL == filter)
  {
    QString ss = QObject::tr("Error Setting ReadH5Ebsd Gui values to Filter instance. Filter instance was NULL.").arg(m_FilterParameter->getPropertyName());
    emit errorSettingFilterParameter(ss);
  }

  ReadH5Ebsd* readEbsd = qobject_cast<ReadH5Ebsd*>(filter);
  Q_ASSERT_X(NULL != readEbsd, "ReadH5EbsdWidget can ONLY be used with ReadH5Ebsd filter", __FILE__);

  bool ok = false;
  readEbsd->setInputFile(m_InputFile->text() );
  readEbsd->setZStartIndex(m_ZStartIndex->text().toLongLong(&ok));
  readEbsd->setZEndIndex(m_ZEndIndex->text().toLongLong(&ok));
  readEbsd->setUseTransformations(m_UseTransformations->isChecked() );
  int index = m_AngleRepresentationCB->currentIndex();
  readEbsd->setAngleRepresentation(index);
  readEbsd->setSelectedArrayNames(getSelectedArrayNames());

  //  m_Filter->setSelectedEnsembleNames(getSelectedEnsembleNames());

}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSet<QString> ReadH5EbsdWidget::getSelectedArrayNames()
{
  QSet<QString> selections;

  int count = m_CellList->count();
  for(int i = 0; i < count; i++)
  {
    if(m_CellList->item(i)->checkState() == Qt::Checked)
    {
      selections.insert(m_CellList->item(i)->text());
    }
  }

  return selections;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSet<QString> ReadH5EbsdWidget::getSelectedEnsembleNames()
{
  QSet<QString> selections;


  return selections;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5EbsdWidget::beforePreflight()
{

  if (m_NewFileLoaded == true)
  {
    // Get all the arrays from the file
    QSet<QString> arrayNames = m_Filter->getDataArrayNames();
    // Push those arrays into the QListWidget and set them all to true
    updateModelFromFilter(arrayNames, true);
    // Set our check box to be synced up with all the arrays being selected
    m_DataArraysCheckBox->blockSignals(true);
    m_DataArraysCheckBox->setCheckState(Qt::Checked);
    m_DataArraysCheckBox->blockSignals(false);
    m_NewFileLoaded = false; // We are all done with our update based a new file being loaded
  }

  if (m_DidCausePreflight == false )
  {
    QSet<QString> arrayNames = m_Filter->getDataArrayNames();
    updateModelFromFilter(arrayNames);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5EbsdWidget::afterPreflight()
{
  updateFileInfoWidgets();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5EbsdWidget::updateModelFromFilter(QSet<QString>& arrayNames, bool setChecked)
{
  m_CellList->blockSignals(true);
  // Get the items that are checked, which is different that being selected.
  QSet<QString> selections = getSelectedArrayNames();

  // Clear the list and put in those from the incoming list and also check any items that were already selected by the user
  m_CellList->clear();

  foreach(QString item, arrayNames)
  {
    QListWidgetItem* listItem = new QListWidgetItem(item, m_CellList);
    if( (selections.find(item) != selections.end() ) || (setChecked == true))
    {
      listItem->setCheckState(Qt::Checked);
    }
    else
    {
      listItem->setCheckState(Qt::Unchecked);
    }

  }
  m_CellList->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5EbsdWidget::updateFileInfoWidgets()
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

        int currentZ = m_ZStartIndex->value();
        m_ZStartIndex->setRange(zStart, zEnd);

        if(currentZ < zStart || currentZ > zEnd) {
          m_ZStartIndex->setValue(zStart);
        }

        currentZ = m_ZEndIndex->value();
        m_ZEndIndex->setRange(zStart, zEnd);
        if(currentZ < zStart || currentZ > zEnd) {
          m_ZEndIndex->setValue(zEnd);
        }

      }
      else
      {
        resetGuiFileInfoWidgets();
      }

      // Cache the Manufacturer from the File
      m_EbsdManufacturer->setText(h5Reader->getManufacturer());

      m_RefFrameZDir->setText(Ebsd::StackingOrder::Utils::getStringForEnum(h5Reader->getStackingOrder()));

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
        m_SampleTransformation.angle = h5Reader->getSampleTransformationAngle();
        QVector<float> sampleTransAxis = h5Reader->getSampleTransformationAxis();
        m_SampleTransformation.h = sampleTransAxis[0];
        m_SampleTransformation.k = sampleTransAxis[1];
        m_SampleTransformation.l = sampleTransAxis[2];
        m_EulerTransformation.angle = h5Reader->getEulerTransformationAngle();
        QVector<float> eulerTransAxis = h5Reader->getEulerTransformationAxis();
        m_EulerTransformation.h = eulerTransAxis[0];
        m_EulerTransformation.k = eulerTransAxis[1];
        m_EulerTransformation.l = eulerTransAxis[2];

        QString sampleTrans = QString("%1 @ <%2%3%4>").arg(m_SampleTransformation.angle).arg(m_SampleTransformation.h).arg(m_SampleTransformation.k).arg(m_SampleTransformation.l);
        QString eulerTrans = QString("%1 @ <%2%3%4>").arg(m_EulerTransformation.angle).arg(m_EulerTransformation.h).arg(m_EulerTransformation.k).arg(m_EulerTransformation.l);

        //        sampleTransAxis = h5Reader->getSampleTransformationAxis();
        //        eulerTransAxis = h5Reader->getEulerTransformationAxis();
        //        sampleTrans = QString::number(h5Reader->getSampleTransformationAngle()) + " @ <" + QString::number(sampleTransAxis[0]) + QString::number(sampleTransAxis[1]) + QString::number(sampleTransAxis[2]) + ">";
        //        eulerTrans = QString::number(h5Reader->getEulerTransformationAngle()) + " @ <" + QString::number(eulerTransAxis[0]) + QString::number(eulerTransAxis[1]) + QString::number(eulerTransAxis[2]) + ">";

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
void ReadH5EbsdWidget::resetGuiFileInfoWidgets()
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


