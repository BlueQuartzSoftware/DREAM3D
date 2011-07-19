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

#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/Common/PhaseType.h"
#include "DREAM3D/ANGSupport/H5AngDataLoader.h"
#include "QtSupport/QR3DFileCompleter.h"
#include "QtSupport/AIM_QtMacros.h"
#include "QtSupport/QCheckboxDialog.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReconstructionWidget::ReconstructionWidget(QWidget *parent) :
AIMPluginFrame(parent),
m_Reconstruction(NULL),
m_WorkerThread(NULL),
m_phaseTypeEdited(false),
m_WriteEuclideanScalars(false),
m_WritePhaseIdScalars(true),
m_WriteImageQualityScalars(true),
m_WriteIPFColorScalars(true),
m_WriteBinaryVTKFile(true),
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
  on_m_H5InputFile_textChanged(QString(""));

  READ_FILEPATH_SETTING(prefs, m_, OutputDir, "");
  READ_STRING_SETTING(prefs, m_, OutputFilePrefix, "Reconstruction_")

  READ_SETTING(prefs, m_, ZStartIndex, ok, i, 0, Int)
  READ_SETTING(prefs, m_, ZEndIndex, ok, i, 0, Int)

  READ_CHECKBOX_SETTING(prefs, m_, MergeColonies, false);
  READ_CHECKBOX_SETTING(prefs, m_, MergeTwins, false);
  READ_CHECKBOX_SETTING(prefs, m_, FillinSample, false);
  READ_COMBO_BOX(prefs, m_, AlignMeth)
  READ_COMBO_BOX(prefs, m_, Orientation)

  READ_SETTING(prefs, m_, MinImageQuality, ok, d, 50.0 , Double);
  READ_SETTING(prefs, m_, MisOrientationTolerance, ok, d, 5.0 , Double);
  READ_SETTING(prefs, m_, MinAllowedGrainSize, ok, i, 8 , Int);
  READ_SETTING(prefs, m_, MinConfidence, ok, d, 0.1 , Double);
  READ_SETTING(prefs, m_, DownSampleFactor, ok, d, 1.0 , Double);

  READ_BOOL_SETTING(prefs, m_, WriteEuclideanScalars, true);
  READ_BOOL_SETTING(prefs, m_, WritePhaseIdScalars, true);
  READ_BOOL_SETTING(prefs, m_, WriteImageQualityScalars, true);
  READ_BOOL_SETTING(prefs, m_, WriteIPFColorScalars, true);
  READ_BOOL_SETTING(prefs, m_, WriteBinaryVTKFile, true);

  READ_CHECKBOX_SETTING(prefs, m_, VisualizationVizFile, true);
  READ_CHECKBOX_SETTING(prefs, m_, DownSampledVizFile, true);
  m_HDF5GrainFile->blockSignals(true);
  READ_CHECKBOX_SETTING(prefs, m_, HDF5GrainFile, true);
  m_HDF5GrainFile->blockSignals(false);
  READ_CHECKBOX_SETTING(prefs, m_, DxFile, true);
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

  WRITE_SETTING(prefs, m_, MinAllowedGrainSize)
  WRITE_SETTING(prefs, m_, MinConfidence)
  WRITE_SETTING(prefs, m_, DownSampleFactor)
  WRITE_SETTING(prefs, m_, MinImageQuality)
  WRITE_SETTING(prefs, m_, MisOrientationTolerance)
  WRITE_COMBO_BOX(prefs, m_, AlignMeth)
  WRITE_COMBO_BOX(prefs, m_, Orientation)


  WRITE_BOOL_SETTING(prefs, m_, WriteEuclideanScalars, true);
  WRITE_BOOL_SETTING(prefs, m_, WritePhaseIdScalars, true);
  WRITE_BOOL_SETTING(prefs, m_, WriteImageQualityScalars, true);
  WRITE_BOOL_SETTING(prefs, m_, WriteIPFColorScalars, true);
  WRITE_BOOL_SETTING(prefs, m_, WriteBinaryVTKFile, true);


  WRITE_CHECKBOX_SETTING(prefs, m_, VisualizationVizFile)
  WRITE_CHECKBOX_SETTING(prefs, m_, DownSampledVizFile)
  WRITE_CHECKBOX_SETTING(prefs, m_, HDF5GrainFile)
  WRITE_CHECKBOX_SETTING(prefs, m_, DxFile)
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


  m_WidgetList << m_H5InputFile << m_OutputDir << m_OutputDirBtn << m_OutputFilePrefix;
  m_WidgetList << m_ZStartIndex << m_ZEndIndex;
  m_WidgetList << m_MergeTwins << m_MergeColonies << m_FillinSample << m_AlignMeth << m_Orientation;
  m_WidgetList << m_MinAllowedGrainSize << m_MinConfidence << m_DownSampleFactor << m_MisOrientationTolerance;
  m_WidgetList << m_VisualizationVizFile << m_DownSampledVizFile;
  m_WidgetList << m_MinImageQuality;
  m_WidgetList << m_HDF5GrainFile;
  m_WidgetList << m_LoadSettingsBtn << m_SaveSettingsBtn << phaseTypeList;

}


// -----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------_
void ReconstructionWidget::checkIOFiles()
{
  this->verifyPathExists(m_OutputDir->text(), this->m_OutputDir);

  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::Reconstruction,m_, H5VoxelFile)

  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::Reconstruction, m_ , VisualizationVizFile)
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::Reconstruction, m_ , DownSampledVizFile)
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::Reconstruction, m_ , HDF5GrainFile)
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::Reconstruction, m_ , DxFile)
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
  if ( true == file.isEmpty() ){ return; }
  QFileInfo fi (file);
  m_H5InputFile->blockSignals(true);
  QString p = QDir::toNativeSeparators(fi.absoluteFilePath());
  m_H5InputFile->setText(p);
  on_m_H5InputFile_textChanged(m_H5InputFile->text() );
  m_H5InputFile->blockSignals(false);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_m_H5InputFile_textChanged(const QString &text)
{

  if (verifyPathExists(m_H5InputFile->text(), m_H5InputFile) )
  {
    QFileInfo fi(m_H5InputFile->text());
    QString outPath = fi.absolutePath() + QDir::separator() + fi.baseName() + "_Reconstruction";
    outPath = QDir::toNativeSeparators(outPath);
    m_OutputDir->setText(outPath);
    m_SetSliceInfo();
  }

  QFileInfo fi(m_H5InputFile->text());
  if (fi.exists() && fi.isFile())
  {
    // Set the output file Prefix based on the name of the input file
    m_OutputFilePrefix->setText(fi.baseName() + QString("_") );

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

      phaseTypeList->addItem(AIM::PhaseType::PrimaryStr().c_str());
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

    int err = 0;
    int xpoints = 0;
    int ypoints = 0;
    int zpoints = 1;
    float xres = 0.0f;
    float yres = 0.0f;
    float zres = 0.0f;
    int zStart = 0;
    int zEnd = 0;
    err = h5io->readZHeader(zStart, zEnd, zres);
    h5io->setZStartIndex(zStart);
    h5io->setZEndIndex(zEnd);
    zpoints = 1; // Just read the first dataset and hope the rest are the same
    err = h5io->getSizeAndResolution(xpoints, ypoints, zpoints, xres, yres, zres);
    if (err >= 0)
    {
      xDim->setText(QString::number(xpoints));
      yDim->setText(QString::number(ypoints));
      zDim->setText(QString::number(zpoints));
      m_XRes->setText(QString::number(xres));
      m_YRes->setText(QString::number(yres));
      m_ZRes->setText(QString::number(zres));
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//void ReconstructionWidget::on_m_AlreadyFormed_stateChanged(int currentState)
//{
//  QString absPath = m_OutputDir->text() + QDir::separator() + AIM::Reconstruction::VisualizationVizFile.c_str();
//  absPath = QDir::toNativeSeparators(absPath);
//  QFileInfo fi (absPath);
//  QString msg ("All files will be over written that appear in the output directory.");
//}



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
    if(m_Reconstruction != NULL)
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
//      msg.append(QString::number(i)).append(" is not set correctly. Please set the phase to Primary, Precipitate or Transformation.");
      msg.append(QString::number(i)).append(" is not set correctly. Please set the phase to Primary or Precipitate.");
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

  m_Reconstruction = new QReconstruction(NULL);

  // Move the Reconstruction object into the thread that we just created.
  m_Reconstruction->moveToThread(m_WorkerThread);

  m_Reconstruction->setH5AngFile( QDir::toNativeSeparators(m_H5InputFile->text()).toStdString());

  m_Reconstruction->setZStartIndex(m_ZStartIndex->value());
  m_Reconstruction->setZEndIndex(m_ZEndIndex->value() + 1);
  m_Reconstruction->setPhaseTypes(phaseTypes);

  m_Reconstruction->setMergeColonies(m_MergeColonies->isChecked() );
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

  m_Reconstruction->setOutputDirectory(QDir::toNativeSeparators(m_OutputDir->text()).toStdString());
  m_Reconstruction->setOutputFilePrefix(m_OutputFilePrefix->text().toStdString());


  m_Reconstruction->setWriteVtkFile(m_VisualizationVizFile->isChecked());
  m_Reconstruction->setWriteEuclidean(m_WriteEuclideanScalars);
  m_Reconstruction->setWritePhaseId(m_WritePhaseIdScalars);
  m_Reconstruction->setWriteImageQuality(m_WriteImageQualityScalars);
  m_Reconstruction->setWriteIPFColor(m_WriteIPFColorScalars);
  m_Reconstruction->setWriteBinaryVTKFiles(m_WriteBinaryVTKFile);

  m_Reconstruction->setWriteDownSampledFile(m_DownSampledVizFile->isChecked());
  m_Reconstruction->setWriteHDF5GrainFile(m_HDF5GrainFile->isChecked());
  m_Reconstruction->setWriteDxFile(m_DxFile->isChecked());

  // m_Reconstruction->printSettings(std::cout);

  /* Connect the signal 'started()' from the QThread to the 'run' slot of the
   * Reconstruction object. Since the Reconstruction object has been moved to another
   * thread of execution and the actual QThread lives in *this* thread then the
   * type of connection will be a Queued connection.
   */
  // When the thread starts its event loop, start the Reconstruction going
  connect(m_WorkerThread, SIGNAL(started()),
          m_Reconstruction, SLOT(run()));

  // When the Reconstruction ends then tell the QThread to stop its event loop
  connect(m_Reconstruction, SIGNAL(finished() ),
          m_WorkerThread, SLOT(quit()) );

  // When the QThread finishes, tell this object that it has finished.
  connect(m_WorkerThread, SIGNAL(finished()),
          this, SLOT( threadFinished() ) );

  // Send Progress from the Reconstruction to this object for display
  connect(m_Reconstruction, SIGNAL (updateProgress(int)),
    this, SLOT(threadProgressed(int) ) );

  // Send progress messages from Reconstruction to this object for display
  connect(m_Reconstruction, SIGNAL (updateMessage(QString)),
          this, SLOT(threadHasMessage(QString) ) );

  // If the use clicks on the "Cancel" button send a message to the Reconstruction object
  // We need a Direct Connection so the
  connect(this, SIGNAL(cancelProcess() ),
          m_Reconstruction, SLOT (on_CancelWorker() ) , Qt::DirectConnection);


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
 // std::cout << "ReconstructionWidget::threadFinished()" << std::endl;
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_m_VtkOptionsBtn_clicked()
{
  QVector<QString> options;
  options.push_back("Write Euclidean Scalars");
  options.push_back("Write Phase Ids Scalars");
  options.push_back("Write Image Quality Scalars");
  options.push_back("Write IPF Color Scalars");
  options.push_back("Write Binary VTK File");
  QCheckboxDialog d(options, this);
  d.setWindowTitle(QString("VTK Output Options"));

  d.setValue("Write Euclidean Scalars", m_WriteEuclideanScalars);
  d.setValue("Write Phase Ids Scalars", m_WritePhaseIdScalars);
  d.setValue("Write Image Quality Scalars", m_WriteImageQualityScalars);
  d.setValue("Write IPF Color Scalars", m_WriteIPFColorScalars);
  d.setValue("Write Binary VTK File", m_WriteBinaryVTKFile);

  int ret = d.exec();
  if (ret == QDialog::Accepted)
  {
    m_WriteEuclideanScalars = d.getValue("Write Euclidean Scalars");
    m_WritePhaseIdScalars = d.getValue("Write Phase Ids Scalars");
    m_WriteImageQualityScalars = d.getValue("Write Image Quality Scalars");
    m_WriteIPFColorScalars = d.getValue("Write IPF Color Scalars");
    m_WriteBinaryVTKFile = d.getValue("Write Binary VTK File");
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_m_HDF5GrainFile_stateChanged(int state)
{
  if (m_HDF5GrainFile->isChecked())
  {
    if (verifyPathExists(m_OutputDir->text(), m_OutputDir) == false) {
      QMessageBox::warning(this, tr("DREAM.3D"),
      tr("In order to render the data contained in the H5Grain file you will need a plugin for ParaView which is available separately."
          "\n  The raw data in the file can be viewed with any HDF5 data viewer. See http://www.hdfgroup.org for more details."),
      QMessageBox::Ok,
      QMessageBox::Ok);
      return;
      }
  }
}

