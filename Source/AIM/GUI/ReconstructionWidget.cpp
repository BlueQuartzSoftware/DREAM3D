/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
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
#include "AIM/Common/PhaseType.h"
#include "AIM/ANG/H5AngDataLoader.h"
#include "QtSupport/QR3DFileCompleter.h"
#include "QtSupport/AIM_QtMacros.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReconstructionWidget::ReconstructionWidget(QWidget *parent) :
AIMPluginFrame(parent),
m_WorkerThread(NULL),
m_phaseTypeEdited(false),
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
  READ_FILEPATH_SETTING(prefs, m_, H5InputFile, "");
  if (verifyPathExists(m_H5InputFile->text(), m_H5InputFile) )
  {
    m_SetSliceInfo();
    on_m_H5InputFile_textChanged(QString(""));
  }
  READ_FILEPATH_SETTING(prefs, m_, OutputDir, "");
  READ_STRING_SETTING(prefs, m_, OutputFilePrefix, "Reconstruction_")

  READ_SETTING(prefs, m_, ZStartIndex, ok, i, 0, Int)
  READ_SETTING(prefs, m_, ZEndIndex, ok, i, 0, Int)

  READ_CHECKBOX_SETTING(prefs, m_, MergeColonies, false);
  READ_CHECKBOX_SETTING(prefs, m_, AlreadyFormed, false);
  READ_CHECKBOX_SETTING(prefs, m_, MergeTwins, false);
  READ_CHECKBOX_SETTING(prefs, m_, FillinSample, false);
  READ_COMBO_BOX(prefs, m_, AlignMeth)
  READ_COMBO_BOX(prefs, m_, Orientation)

  READ_SETTING(prefs, m_, MinImageQuality, ok, d, 50.0 , Double);
  READ_SETTING(prefs, m_, MisOrientationTolerance, ok, d, 5.0 , Double);
  READ_SETTING(prefs, m_, MinAllowedGrainSize, ok, i, 8 , Int);
  READ_SETTING(prefs, m_, MinConfidence, ok, d, 0.1 , Double);
  READ_SETTING(prefs, m_, DownSampleFactor, ok, d, 1.0 , Double);
  READ_SETTING(prefs, m_, BinStepSize, ok, d, 1.0 , Double);


  READ_CHECKBOX_SETTING(prefs, m_, DisorientationVizFile, true);
  READ_CHECKBOX_SETTING(prefs, m_, ImageQualityVizFile, true);
  READ_CHECKBOX_SETTING(prefs, m_, IPFVizFile, true);
  READ_CHECKBOX_SETTING(prefs, m_, SchmidFactorVizFile, true);
  READ_CHECKBOX_SETTING(prefs, m_, VisualizationVizFile, true);
  READ_CHECKBOX_SETTING(prefs, m_, DownSampledVizFile, true);
  READ_CHECKBOX_SETTING(prefs, m_, HDF5GrainFile, true);
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//  Write our Prefs to file
// -----------------------------------------------------------------------------
void ReconstructionWidget::writeSettings(QSettings &prefs)
{
  prefs.beginGroup("Reconstruction");
  WRITE_STRING_SETTING(prefs, m_, H5InputFile)
  WRITE_STRING_SETTING(prefs, m_, OutputDir)
  WRITE_STRING_SETTING(prefs, m_, OutputFilePrefix)
  WRITE_SETTING(prefs, m_, ZStartIndex)
  WRITE_SETTING(prefs, m_, ZEndIndex)

  m_MergeTwins->objectName();
  prefs.setValue(m_MergeTwins->objectName(), m_MergeTwins->isChecked() );


  WRITE_CHECKBOX_SETTING(prefs, m_, MergeTwins)
  WRITE_CHECKBOX_SETTING(prefs, m_, MergeColonies)
  WRITE_CHECKBOX_SETTING(prefs, m_, FillinSample)
  WRITE_CHECKBOX_SETTING(prefs, m_, AlreadyFormed)

  WRITE_SETTING(prefs, m_, MinAllowedGrainSize)
  WRITE_SETTING(prefs, m_, MinConfidence)
  WRITE_SETTING(prefs, m_, BinStepSize)
  WRITE_SETTING(prefs, m_, DownSampleFactor)
  WRITE_SETTING(prefs, m_, MinImageQuality)
  WRITE_SETTING(prefs, m_, MisOrientationTolerance)
  WRITE_COMBO_BOX(prefs, m_, AlignMeth)
  WRITE_COMBO_BOX(prefs, m_, Orientation)

  WRITE_CHECKBOX_SETTING(prefs, m_, DisorientationVizFile)
  WRITE_CHECKBOX_SETTING(prefs, m_, ImageQualityVizFile)
  WRITE_CHECKBOX_SETTING(prefs, m_, IPFVizFile)
  WRITE_CHECKBOX_SETTING(prefs, m_, SchmidFactorVizFile)
  WRITE_CHECKBOX_SETTING(prefs, m_, VisualizationVizFile)
  WRITE_CHECKBOX_SETTING(prefs, m_, DownSampledVizFile)
  WRITE_CHECKBOX_SETTING(prefs, m_, HDF5GrainFile)
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
  m_H5InputFile->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString &)),
           this, SLOT(on_m_H5InputFile_textChanged(const QString &)));

  QR3DFileCompleter* com2 = new QR3DFileCompleter(this, true);
  m_OutputDir->setCompleter(com2);
  QObject::connect( com2, SIGNAL(activated(const QString &)),
           this, SLOT(on_m_OutputDir_textChanged(const QString &)));


  QString msg ("All files will be over written that appear in the output directory.");

  QFileInfo fi (m_OutputDir->text() + QDir::separator() +  AIM::Reconstruction::VisualizationVizFile.c_str() );
  if (m_AlreadyFormed->isChecked() == true && fi.exists() == false)
  {
    m_AlreadyFormed->setChecked(false);
  }

  if (m_AlreadyFormed->isChecked())
  {
    msg += QString("\nThe 'reconstructed_data.txt' file will be used as an import and NOT over written with new data");
  }
  messageLabel->setText(msg);

  m_WidgetList << m_H5InputFile << m_OutputDir << m_OutputDirBtn << m_OutputFilePrefix;
  m_WidgetList << m_ZStartIndex << m_ZEndIndex;
  m_WidgetList << m_MergeTwins << m_MergeColonies << m_FillinSample << m_AlreadyFormed << m_AlignMeth << m_Orientation;
  m_WidgetList << m_MinAllowedGrainSize << m_MinConfidence << m_DownSampleFactor << m_MisOrientationTolerance;
  m_WidgetList << m_BinStepSize;
  m_WidgetList << m_DisorientationVizFile << m_ImageQualityVizFile << m_IPFVizFile << m_SchmidFactorVizFile << m_VisualizationVizFile << m_DownSampledVizFile;
  m_WidgetList << m_MinImageQuality;
  m_WidgetList << m_HDF5GrainFile << m_H5StatisticsFile << m_AlignmentFile << m_GrainDataFile;
  m_WidgetList << m_LoadSettingsBtn << m_SaveSettingsBtn;

}


// -----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------_
void ReconstructionWidget::checkIOFiles()
{

  this->verifyPathExists(m_OutputDir->text(), this->m_OutputDir);

  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::Reconstruction,m_, AlignmentFile)
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::Reconstruction,m_, GrainDataFile)

  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::Reconstruction, m_ , DisorientationVizFile)
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::Reconstruction, m_ , ImageQualityVizFile)
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::Reconstruction, m_ , IPFVizFile)
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::Reconstruction, m_ , SchmidFactorVizFile)
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::Reconstruction, m_ , VisualizationVizFile)
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::Reconstruction, m_ , DownSampledVizFile)
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::Reconstruction, m_ , HDF5GrainFile)
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::Reconstruction, m_ , H5StatisticsFile)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_m_SaveSettingsBtn_clicked()
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
void ReconstructionWidget::on_m_LoadSettingsBtn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Settings File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("Settings File (*.txt)") );
  if ( true == file.isEmpty() ){return;  }
  QSettings prefs(file, QSettings::IniFormat, this);
  readSettings(prefs);
  if (verifyPathExists(m_OutputDir->text(), m_OutputDir) )
  {
    checkIOFiles();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_m_OIMH5Btn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Input File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("HDF5 OIM Files (*.h5 *.hdf5 *.h5ang)") );
  if ( true == file.isEmpty() ){return;  }
  QFileInfo fi (file);
 // QString ext = fi.suffix();
  verifyPathExists(file, m_H5InputFile);
  m_H5InputFile->setText(fi.absoluteFilePath());
  m_SetSliceInfo();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_m_AlreadyFormed_stateChanged(int currentState)
{
  QString absPath = m_OutputDir->text() + QDir::separator() + AIM::Reconstruction::VisualizationVizFile.c_str();
  absPath = QDir::toNativeSeparators(absPath);
  QFileInfo fi (absPath);
  QString msg ("All files will be over written that appear in the output directory.");
  if (m_AlreadyFormed->isChecked() == true && fi.exists() == false)
  {
    QMessageBox::critical(this, tr("AIM Representation"),
      tr("You have selected the 'Already Formed' check box \nbut the correct output file does not exist.\n"
      "The checkbox will revert to an unchecked state.?"),
      QMessageBox::Ok,
      QMessageBox::Ok);
    m_AlreadyFormed->setChecked(false);
      CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::Reconstruction, m_, VisualizationVizFile)
  }

  if (m_AlreadyFormed->isChecked())
  {
    msg += QString("\nThe 'reconstructed_data.txt' file will be used as an import and NOT over written with new data");
  }
  messageLabel->setText(msg);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_m_OutputDirBtn_clicked()
{
  QString outputFile = this->m_OpenDialogLastDirectory + QDir::separator();
  outputFile = QFileDialog::getExistingDirectory(this, tr("Select Output Directory"), outputFile);
  if (!outputFile.isNull())
  {
    this->m_OutputDir->setText(outputFile);
    if (verifyPathExists(outputFile, m_OutputDir) == true )
    {
      checkIOFiles();
      QFileInfo fi (m_OutputDir->text() + QDir::separator() +  AIM::Reconstruction::VisualizationVizFile.c_str() );
      if (m_AlreadyFormed->isChecked() == true && fi.exists() == false)
      {
        m_AlreadyFormed->setChecked(false);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_m_H5InputFile_textChanged(const QString &text)
{
  if (verifyPathExists(m_H5InputFile->text(), m_H5InputFile) )
  {
    m_SetSliceInfo();
  }

  QFileInfo fi(m_H5InputFile->text());
  if (fi.exists() && fi.isFile())
  {
    // Read the Phase information from the .h5ang file
    AbstractAngDataLoader::Pointer oimDataLoader = H5AngDataLoader::New();
    H5AngDataLoader* h5io = dynamic_cast<H5AngDataLoader*>(oimDataLoader.get());
    h5io->setFilename(m_H5InputFile->text().toStdString() );
    h5io->setZStartIndex(m_ZStartIndex->value());

    std::vector<AngPhase::Pointer> phases = h5io->getPhases();
    int size = phases.size();
    std::vector<std::string> phaseTypeStrings;
    AIM::PhaseType::getPhaseTypeStrings(phaseTypeStrings);
    std::vector<AIM::Reconstruction::PhaseType> phaseTypeEnums;
    AIM::PhaseType::getPhaseTypeEnums(phaseTypeEnums);

    // Remove all the items
    phaseTypeList->clear();

    for (int i = 0; i < size; i++)
    {

      phaseTypeList->addItem(AIM::PhaseType::PrimaryStr.c_str());
      QListWidgetItem* item = phaseTypeList->item(i);

      QComboBox* cb = new QComboBox(phaseTypeList);
      for(size_t i = 0; i < phaseTypeStrings.size(); ++i)
      {
        cb->addItem(QString::fromStdString( phaseTypeStrings[i]), phaseTypeEnums[i] );
        cb->setItemData(i, phaseTypeEnums[i], Qt::UserRole);
      }
      phaseTypeList->setItemWidget(item, cb);
      connect(cb, SIGNAL(currentIndexChanged(int)),
              this, SLOT(phaseTypeEdited(int)));
    }
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::m_SetSliceInfo()
{
  AbstractAngDataLoader::Pointer ptr = H5AngDataLoader::New();
  H5AngDataLoader* reader = dynamic_cast<H5AngDataLoader*>(ptr.get());
  QFileInfo fi(m_H5InputFile->text());
  if (fi.isFile() == false)
  {
    return;
  }

  reader->setFilename(m_H5InputFile->text().toStdString());
  int zStart = -1, zEnd = -1;
  float zRes = 1.0;
  if (reader->readZHeader(zStart, zEnd, zRes) >= 0)
  {
    m_ZStartIndex->setValue(zStart);
    m_ZStartIndex->setRange(zStart, zEnd);
    m_ZEndIndex->setValue(zEnd);
    m_ZEndIndex->setRange(zStart, zEnd);
    m_ZRes->setText(QString::number(zRes));
    m_ZMin->setText(QString::number(zStart));
    m_ZMax->setText(QString::number(zEnd));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::phaseTypeEdited(int i)
{
  m_phaseTypeEdited = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ReconstructionWidget::checkPhaseTypes()
{


  if (m_phaseTypeEdited == true)
  {
    int
        ret =
            QMessageBox::warning(this, QString("Reconstruction"), "The Phase Types were Edited. We need to save this data to the input file. Do you want to do that now. Canceling will leave all files untouched but NOT execute the grain generation.", QMessageBox::Ok
                | QMessageBox::Default, QMessageBox::No);
    if (ret == QMessageBox::No)
    {
      return false;
    }
    else
    {
      std::cout << "We need to save the Phase Type to something?" << std::endl;
      return false;
    }

  }
  return true;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_m_OutputDir_textChanged(const QString & text)
{
  verifyPathExists(m_OutputDir->text(), m_OutputDir);
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_m_OutputFilePrefix_textChanged(const QString &text)
{
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_m_GoBtn_clicked()
{
  if (m_GoBtn->text().compare("Cancel") == 0)
  {
    if(m_Reconstruction.get() != NULL)
    {
      std::cout << "canceling from GUI...." << std::endl;
      emit cancelProcess();
    }
    return;
  }



  if (false == sanityCheckOutputDirectory(m_OutputDir, QString("Reconstruction")) )
  {
    return;
  }
  SANITY_CHECK_INPUT(m_ , OutputDir)

  std::vector<AIM::Reconstruction::PhaseType> phaseTypes(1, AIM::Reconstruction::UnknownPhaseType);
  int count = phaseTypeList->count();
  bool ok = false;
  for (int i = 0; i < count; ++i)
  {
    QListWidgetItem* item = phaseTypeList->item(i);
    QComboBox* cb = qobject_cast<QComboBox*> (phaseTypeList->itemWidget(item));
    AIM::Reconstruction::PhaseType enPtValue = static_cast<AIM::Reconstruction::PhaseType>(cb->itemData(cb->currentIndex(), Qt::UserRole).toUInt(&ok));
    if (enPtValue >= AIM::Reconstruction::UnknownPhaseType)
    {
      QString msg("The Phase Type for phase ");
      msg.append(QString::number(i)).append(" is not set correctly. Please set the phase to Primary, Precipitate or Transformation.");
      QMessageBox::critical(this, QString("Grain Generator"), msg, QMessageBox::Ok | QMessageBox::Default);
      return;
    }
    phaseTypes.push_back(enPtValue);
  }

  ok = checkPhaseTypes();

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


  m_Reconstruction->setH5AngFile(m_H5InputFile->text().toStdString());

  m_Reconstruction->setZStartIndex(m_ZStartIndex->value());
  m_Reconstruction->setZEndIndex(m_ZEndIndex->value() + 1);
  m_Reconstruction->setPhaseTypes(phaseTypes);

  m_Reconstruction->setMergeColonies(m_MergeColonies->isChecked() );
  m_Reconstruction->setAlreadyFormed(m_AlreadyFormed->isChecked());
  m_Reconstruction->setMergeTwins(m_MergeTwins->isChecked() );
  m_Reconstruction->setFillinSample(m_FillinSample->isChecked() );

  AIM::Reconstruction::AlignmentMethod alignmeth = static_cast<AIM::Reconstruction::AlignmentMethod>(m_AlignMeth->currentIndex() );
  m_Reconstruction->setAlignmentMethod(alignmeth);

  Ang::Orientation orientation = static_cast<Ang::Orientation>(m_Orientation->currentIndex());
  m_Reconstruction->setOrientation(orientation);

  m_Reconstruction->setMinAllowedGrainSize(m_MinAllowedGrainSize->value());
  m_Reconstruction->setMisorientationTolerance(m_MisOrientationTolerance->value());
  m_Reconstruction->setMinSeedImageQuality(m_MinImageQuality->value());
  m_Reconstruction->setMinSeedConfidence(m_MinConfidence->value());
  m_Reconstruction->setDownSampleFactor(m_DownSampleFactor->value());
  m_Reconstruction->setSizeBinStepSize(m_BinStepSize->value());

  m_Reconstruction->setOutputDirectory(m_OutputDir->text().toStdString());
  m_Reconstruction->setOutputFilePrefix(m_OutputFilePrefix->text().toStdString());
  m_Reconstruction->setWriteBinaryFiles(m_BinaryVtkFiles->isChecked());

  m_Reconstruction->setWriteVisualizationFile(m_VisualizationVizFile->isChecked());
  m_Reconstruction->setWriteIPFFile(m_IPFVizFile->isChecked());
  m_Reconstruction->setWriteDisorientationFile(m_DisorientationVizFile->isChecked());
  m_Reconstruction->setWriteImageQualityFile(m_ImageQualityVizFile->isChecked());
  m_Reconstruction->setWriteSchmidFactorFile(m_SchmidFactorVizFile->isChecked());
  m_Reconstruction->setWriteDownSampledFile(m_DownSampledVizFile->isChecked());
  m_Reconstruction->setWriteHDF5GrainFile(m_HDF5GrainFile->isChecked());

  m_Reconstruction->printSettings(std::cout);

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
  m_GoBtn->setText("Cancel");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::threadFinished()
{
  std::cout << "ReconstructionWidget::threadFinished()" << std::endl;
  m_GoBtn->setText("Go");
  setWidgetListEnabled(true);
  this->progressBar->setValue(0);
  emit processEnded();
  checkIOFiles();
  m_Reconstruction->deleteLater();
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
