/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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

#include "ReconstructionWidget.h"

//-- Qt Includes
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QThread>
#include <QtCore/QFileInfoList>
#include <QtGui/QFileDialog>
#include <QtGui/QCloseEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QListWidget>

#include "AIM/Common/Constants.h"
#include "AIM/ANG/H5AngDataLoader.h"
#include "AIM/Common/Qt/QR3DFileCompleter.h"
#include "AIM/Common/Qt/AIM_QtMacros.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReconstructionWidget::ReconstructionWidget(QWidget *parent) :
AIMPluginFrame(parent),
m_WorkerThread(NULL),
#if defined(Q_WS_WIN)
m_OpenDialogLastDirectory("C:\\")
#else
m_OpenDialogLastDirectory("~/")
#endif
{
  setupUi(this);
  setupGui();
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReconstructionWidget::~ReconstructionWidget()
{
}


// -----------------------------------------------------------------------------
//  Read the prefs from the local storage file
// -----------------------------------------------------------------------------
void ReconstructionWidget::readSettings(QSettings &prefs)
{

  QString val;
  bool ok;
  qint32 i;
  double d;

  prefs.beginGroup("Reconstruction");
  READ_FILEPATH_SETTING(prefs, rec_OutputDir, "");
  READ_FILEPATH_SETTING(prefs, rec_H5InputFile, "");
  if (verifyPathExists(rec_H5InputFile->text(), rec_H5InputFile) )
  {
    rec_SetSliceInfo();
  }
  READ_SETTING(prefs, rec_ZStartIndex, ok, i, 0, Int)
  READ_SETTING(prefs, rec_ZEndIndex, ok, i, 0, Int)

  READ_CHECKBOX_SETTING(prefs, rec_mergeColonies, false);
  READ_CHECKBOX_SETTING(prefs, rec_alreadyFormed, false);
  READ_CHECKBOX_SETTING(prefs, rec_mergeTwins, false);
  READ_CHECKBOX_SETTING(prefs, rec_fillinSample, false);
  READ_COMBO_BOX(prefs, rec_CrystalStructure)
  READ_COMBO_BOX(prefs, rec_AlignMeth)
  READ_COMBO_BOX(prefs, rec_Orientation)

  READ_SETTING(prefs, rec_MinImageQuality, ok, d, 50.0 , Double);
  READ_SETTING(prefs, rec_MisOrientationTolerance, ok, d, 5.0 , Double);
  READ_SETTING(prefs, rec_MinAllowedGrainSize, ok, i, 8 , Int);
  READ_SETTING(prefs, rec_MinConfidence, ok, d, 0.1 , Double);
  READ_SETTING(prefs, rec_DownSampleFactor, ok, d, 1.0 , Double);
  READ_SETTING(prefs, rec_BinStepSize, ok, d, 1.0 , Double);


  READ_CHECKBOX_SETTING(prefs, rec_DisorientationVizFile, true);
  READ_CHECKBOX_SETTING(prefs, rec_ImageQualityVizFile, true);
  READ_CHECKBOX_SETTING(prefs, rec_IPFVizFile, true);
  READ_CHECKBOX_SETTING(prefs, rec_SchmidFactorVizFile, true);
  READ_CHECKBOX_SETTING(prefs, rec_VisualizationVizFile, true);
  READ_CHECKBOX_SETTING(prefs, rec_DownSampledVizFile, true);
  READ_CHECKBOX_SETTING(prefs, rec_HDF5GrainFile, true);
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//  Write our Prefs to file
// -----------------------------------------------------------------------------
void ReconstructionWidget::writeSettings(QSettings &prefs)
{
  prefs.beginGroup("Reconstruction");
  WRITE_STRING_SETTING(prefs, rec_OutputDir)
  WRITE_STRING_SETTING(prefs, rec_H5InputFile)
  WRITE_SETTING(prefs, rec_ZStartIndex)
  WRITE_SETTING(prefs, rec_ZEndIndex)

  rec_mergeTwins->objectName();
  prefs.setValue(rec_mergeTwins->objectName(), rec_mergeTwins->isChecked() );


  WRITE_CHECKBOX_SETTING(prefs, rec_mergeTwins)
  WRITE_CHECKBOX_SETTING(prefs, rec_mergeColonies)
  WRITE_CHECKBOX_SETTING(prefs, rec_fillinSample)
  WRITE_CHECKBOX_SETTING(prefs, rec_alreadyFormed)

  WRITE_SETTING(prefs, rec_MinAllowedGrainSize)
  WRITE_SETTING(prefs, rec_MinConfidence)
  WRITE_SETTING(prefs, rec_BinStepSize)
  WRITE_SETTING(prefs, rec_DownSampleFactor)
  WRITE_SETTING(prefs, rec_MinImageQuality)
  WRITE_SETTING(prefs, rec_MisOrientationTolerance)
  WRITE_COMBO_BOX(prefs, rec_CrystalStructure)
  WRITE_COMBO_BOX(prefs, rec_AlignMeth)
  WRITE_COMBO_BOX(prefs, rec_Orientation)

  WRITE_CHECKBOX_SETTING(prefs, rec_DisorientationVizFile)
  WRITE_CHECKBOX_SETTING(prefs, rec_ImageQualityVizFile)
  WRITE_CHECKBOX_SETTING(prefs, rec_IPFVizFile)
  WRITE_CHECKBOX_SETTING(prefs, rec_SchmidFactorVizFile)
  WRITE_CHECKBOX_SETTING(prefs, rec_VisualizationVizFile)
  WRITE_CHECKBOX_SETTING(prefs, rec_DownSampledVizFile)
  WRITE_CHECKBOX_SETTING(prefs, rec_HDF5GrainFile)
  prefs.endGroup();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList) {
    w->setEnabled(b);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::setupGui()
{

  QR3DFileCompleter* com = new QR3DFileCompleter(this, false);
  rec_H5InputFile->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString &)),
           this, SLOT(on_rec_H5InputFile_textChanged(const QString &)));

  QR3DFileCompleter* com2 = new QR3DFileCompleter(this, true);
  rec_OutputDir->setCompleter(com2);
  QObject::connect( com2, SIGNAL(activated(const QString &)),
           this, SLOT(on_rec_OutputDir_textChanged(const QString &)));


  QString msg ("All files will be over written that appear in the output directory.");

  QFileInfo fi (rec_OutputDir->text() + QDir::separator() +  AIM::Reconstruction::VisualizationVizFile.c_str() );
  if (rec_alreadyFormed->isChecked() == true && fi.exists() == false)
  {
    rec_alreadyFormed->setChecked(false);
  }

  if (rec_alreadyFormed->isChecked())
  {
    msg += QString("\nThe 'reconstructed_data.txt' file will be used as an import and NOT over written with new data");
  }
  messageLabel->setText(msg);

  m_WidgetList << rec_OutputDir << rec_OutputDirBtn;
  m_WidgetList << rec_ZStartIndex << rec_ZEndIndex;
  m_WidgetList << rec_mergeTwins << rec_mergeColonies << rec_fillinSample << rec_alreadyFormed << rec_AlignMeth << rec_MinAllowedGrainSize << rec_MinConfidence << rec_DownSampleFactor << rec_MisOrientationTolerance;
  m_WidgetList << rec_CrystalStructure << rec_BinStepSize;
  m_WidgetList << rec_DisorientationVizFile << rec_ImageQualityVizFile << rec_IPFVizFile << rec_SchmidFactorVizFile << rec_VisualizationVizFile << rec_DownSampledVizFile;
  m_WidgetList << rec_MinImageQuality;
  m_WidgetList << rec_HDF5GrainFile << rec_H5StatisticsFile << rec_AlignmentFile << rec_GrainDataFile;
  m_WidgetList << rec_LoadSettingsBtn << rec_SaveSettingsBtn;

}


// -----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------_
void ReconstructionWidget::checkIOFiles()
{

  this->verifyPathExists(rec_OutputDir->text(), this->rec_OutputDir);

  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::Reconstruction,rec_, AlignmentFile)
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::Reconstruction,rec_, GrainDataFile)

  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::Reconstruction, rec_ , DisorientationVizFile)
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::Reconstruction, rec_ , ImageQualityVizFile)
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::Reconstruction, rec_ , IPFVizFile)
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::Reconstruction, rec_ , SchmidFactorVizFile)
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::Reconstruction, rec_ , VisualizationVizFile)
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::Reconstruction, rec_ , DownSampledVizFile)
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::Reconstruction, rec_ , HDF5GrainFile)
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::Reconstruction, rec_ , H5StatisticsFile)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_rec_SaveSettingsBtn_clicked()
{
  QString proposedFile = m_OpenDialogLastDirectory + QDir::separator() + "ReconstructionSettings.txt";
  QString file = QFileDialog::getSaveFileName(this, tr("Save Reconstruction Settings"),
                                              proposedFile,
                                              tr("*.txt") );
  if ( true == file.isEmpty() ){ return;  }

  QSettings prefs(file, QSettings::IniFormat, this);
  writeSettings(prefs);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_rec_LoadSettingsBtn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Settings File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("Settings File (*.txt)") );
  if ( true == file.isEmpty() ){return;  }
  QSettings prefs(file, QSettings::IniFormat, this);
  readSettings(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_rec_OIMH5Btn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Input File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("HDF5 OIM Files (*.h5 *.hdf5 *.h5ang)") );
  if ( true == file.isEmpty() ){return;  }
  QFileInfo fi (file);
 // QString ext = fi.suffix();
  verifyPathExists(file, rec_H5InputFile);
  rec_H5InputFile->setText(fi.absoluteFilePath());
  rec_SetSliceInfo();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_rec_alreadyFormed_stateChanged(int currentState)
{
  QString absPath = rec_OutputDir->text() + QDir::separator() + AIM::Reconstruction::VisualizationVizFile.c_str();
  absPath = QDir::toNativeSeparators(absPath);
  QFileInfo fi (absPath);
  QString msg ("All files will be over written that appear in the output directory.");
  if (rec_alreadyFormed->isChecked() == true && fi.exists() == false)
  {
    QMessageBox::critical(this, tr("AIM Representation"),
      tr("You have selected the 'Already Formed' check box \nbut the correct output file does not exist.\n"
      "The checkbox will revert to an unchecked state.?"),
      QMessageBox::Ok,
      QMessageBox::Ok);
    rec_alreadyFormed->setChecked(false);
      CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::Reconstruction, rec_, VisualizationVizFile)
  }

  if (rec_alreadyFormed->isChecked())
  {
    msg += QString("\nThe 'reconstructed_data.txt' file will be used as an import and NOT over written with new data");
  }
  messageLabel->setText(msg);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_rec_OutputDirBtn_clicked()
{
  QString outputFile = this->m_OpenDialogLastDirectory + QDir::separator();
  outputFile = QFileDialog::getExistingDirectory(this, tr("Select Output Directory"), outputFile);
  if (!outputFile.isNull())
  {
    this->rec_OutputDir->setText(outputFile);
    if (verifyPathExists(outputFile, rec_OutputDir) == true )
    {
      checkIOFiles();
      QFileInfo fi (rec_OutputDir->text() + QDir::separator() +  AIM::Reconstruction::VisualizationVizFile.c_str() );
      if (rec_alreadyFormed->isChecked() == true && fi.exists() == false)
      {
        rec_alreadyFormed->setChecked(false);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_rec_H5InputFile_textChanged(const QString &text)
{
  if (verifyPathExists(rec_H5InputFile->text(), rec_H5InputFile) )
  {
    rec_SetSliceInfo();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::rec_SetSliceInfo()
{
  AbstractAngDataLoader::Pointer ptr = H5AngDataLoader::New();
  H5AngDataLoader* reader = dynamic_cast<H5AngDataLoader*>(ptr.get());
  QFileInfo fi(rec_H5InputFile->text());
  if (fi.isFile() == false)
  {
    return;
  }

  reader->setFilename(rec_H5InputFile->text().toStdString());
  int zStart = -1, zEnd = -1;
  float zRes = 1.0;
  if (reader->readZHeader(zStart, zEnd, zRes) >= 0)
  {
    rec_ZStartIndex->setValue(zStart);
    rec_ZStartIndex->setRange(zStart, zEnd);
    rec_ZEndIndex->setValue(zEnd);
    rec_ZEndIndex->setRange(zStart, zEnd);
    rec_ZRes->setText(QString::number(zRes));
    rec_ZMin->setText(QString::number(zStart));
    rec_ZMax->setText(QString::number(zEnd));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_rec_OutputDir_textChanged(const QString & text)
{
  verifyPathExists(rec_OutputDir->text(), rec_OutputDir);
  checkIOFiles();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_rec_GoBtn_clicked()
{
  if (rec_GoBtn->text().compare("Cancel") == 0)
  {
    if(m_Reconstruction.get() != NULL)
    {
      //std::cout << "canceling from GUI...." << std::endl;
      emit cancelProcess();
    }
    return;
  }



  if (false == sanityCheckOutputDirectory(rec_OutputDir, QString("Reconstruction")) )
  {
    return;
  }
  SANITY_CHECK_INPUT(rec_ , OutputDir)
  if (m_WorkerThread != NULL)
  {
    m_WorkerThread->wait(); // Wait until the thread is complete
    delete m_WorkerThread; // Kill the thread
    m_WorkerThread = NULL;
  }
  m_WorkerThread = new QThread(); // Create a new Thread Resource

  m_Reconstruction = Reconstruction::New();

  // Move the Reconstruction object into the thread that we just created.
  m_Reconstruction->moveToThread(m_WorkerThread);


  m_Reconstruction->setH5AngFile(rec_H5InputFile->text().toStdString());

  m_Reconstruction->setZStartIndex(rec_ZStartIndex->value());
  m_Reconstruction->setZEndIndex(rec_ZEndIndex->value() + 1);

  m_Reconstruction->setMergeColonies(rec_mergeColonies->isChecked() );
  m_Reconstruction->setAlreadyFormed(rec_alreadyFormed->isChecked());
  m_Reconstruction->setMergeTwins(rec_mergeTwins->isChecked() );
  m_Reconstruction->setFillinSample(rec_fillinSample->isChecked() );

  AIM::Reconstruction::CrystalStructure crystruct = static_cast<AIM::Reconstruction::CrystalStructure>(rec_CrystalStructure->currentIndex());
  AIM::Reconstruction::AlignmentMethod alignmeth = static_cast<AIM::Reconstruction::AlignmentMethod>(rec_AlignMeth->currentIndex() );
  m_Reconstruction->setCrystalStructure(crystruct);
  m_Reconstruction->setAlignmentMethod(alignmeth);

  Ang::Orientation orientation = static_cast<Ang::Orientation>(rec_Orientation->currentIndex());
  m_Reconstruction->setOrientation(orientation);

  m_Reconstruction->setMinAllowedGrainSize(rec_MinAllowedGrainSize->value());
  m_Reconstruction->setMisorientationTolerance(rec_MisOrientationTolerance->value());
  m_Reconstruction->setMinSeedImageQuality(rec_MinImageQuality->value());
  m_Reconstruction->setMinSeedConfidence(rec_MinConfidence->value());
  m_Reconstruction->setDownSampleFactor(rec_DownSampleFactor->value());
  m_Reconstruction->setSizeBinStepSize(rec_BinStepSize->value());

  m_Reconstruction->setOutputDirectory(rec_OutputDir->text().toStdString());
  m_Reconstruction->setWriteVisualizationFile(rec_VisualizationVizFile->isChecked());
  m_Reconstruction->setWriteIPFFile(rec_IPFVizFile->isChecked());
  m_Reconstruction->setWriteDisorientationFile(rec_DisorientationVizFile->isChecked());
  m_Reconstruction->setWriteImageQualityFile(rec_ImageQualityVizFile->isChecked());
  m_Reconstruction->setWriteSchmidFactorFile(rec_SchmidFactorVizFile->isChecked());
  m_Reconstruction->setWriteDownSampledFile(rec_DownSampledVizFile->isChecked());
  m_Reconstruction->setWriteHDF5GrainFile(rec_HDF5GrainFile->isChecked());


  /* Connect the signal 'started()' from the QThread to the 'run' slot of the
   * Reconstruction object. Since the Reconstruction object has been moved to another
   * thread of execution and the actual QThread lives in *this* thread then the
   * type of connection will be a Queued connection.
   */
  // When the thread starts its event loop, start the Reconstruction going
  connect(m_WorkerThread, SIGNAL(started()),
          m_Reconstruction.get(), SLOT(compute()));

  // When the Reconstruction ends then tell the QThread to stop its event loop
  connect(m_Reconstruction.get(), SIGNAL(finished() ),
          m_WorkerThread, SLOT(quit()) );

  // When the QThread finishes, tell this object that it has finished.
  connect(m_WorkerThread, SIGNAL(finished()),
          this, SLOT( threadFinished() ) );

  // Send Progress from the Reconstruction to this object for display
  connect(m_Reconstruction.get(), SIGNAL (updateProgress(int)),
    this, SLOT(threadProgressed(int) ) );

  // Send progress messages from Reconstruction to this object for display
  connect(m_Reconstruction.get(), SIGNAL (updateMessage(QString)),
          this, SLOT(threadHasMessage(QString) ) );

  // If the use clicks on the "Cancel" button send a message to the Reconstruction object
  // We need a Direct Connection so the
  connect(this, SIGNAL(cancelProcess() ),
          m_Reconstruction.get(), SLOT (on_CancelWorker() ) , Qt::DirectConnection);


  setWidgetListEnabled(false);
  emit processStarted();
  m_WorkerThread->start();
  rec_GoBtn->setText("Cancel");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::threadFinished()
{
  //std::cout << "ReconstructionWidget::threadFinished()" << std::endl;
  rec_GoBtn->setText("Go");
  setWidgetListEnabled(true);
  this->progressBar->setValue(0);
  emit processEnded();
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::threadProgressed(int val)
{
  this->progressBar->setValue( val );
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::threadHasMessage(QString message)
{
  if (NULL != this->statusBar()) {
    this->statusBar()->showMessage(message);
  }
}
