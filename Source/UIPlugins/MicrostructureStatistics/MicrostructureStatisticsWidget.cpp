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
#include "MicrostructureStatisticsWidget.h"

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
#include <QtGui/QListWidgetItem>

#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/MicrostructureStatistics/MicrostructureStatistics.h"
#include "DREAM3D/HDF5/H5ReconStatsReader.h"
#include "DREAM3D/HDF5/H5ReconStatsWriter.h"

#include "QtSupport/AIM_QtMacros.h"
#include "QtSupport/QR3DFileCompleter.h"
#include "QtSupport/QCheckboxDialog.h"
#include "DREAM3D/HDF5/H5VoxelReader.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MicrostructureStatisticsWidget::MicrostructureStatisticsWidget(QWidget *parent) :
  AIMPluginFrame(parent), m_MicrostructureStatistics(NULL), m_WorkerThread(NULL),
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
MicrostructureStatisticsWidget::~MicrostructureStatisticsWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::readSettings(QSettings &prefs)
{
  QString val;
  prefs.beginGroup("MicrostructureStatistics");
  bool ok = false;
  double d;

  READ_FILEPATH_SETTING(prefs, m_, OutputDir, "");
  READ_STRING_SETTING(prefs, m_, OutputFilePrefix, "MicrostructureStatistics_")
  READ_FILEPATH_SETTING(prefs, m_, InputFile, "");

  READ_BOOL_SETTING(prefs, m_, ComputeGrainSize, true);
  READ_BOOL_SETTING(prefs, m_, ComputeGrainShapes, true);
  READ_BOOL_SETTING(prefs, m_, ComputeNumNeighbors, true);
  READ_BOOL_SETTING(prefs, m_, ComputeMDF, true);
  READ_BOOL_SETTING(prefs, m_, ComputeODF, true);

  READ_SETTING(prefs, m_, BinStepSize, ok, d, 1.0 , Double);

  READ_CHECKBOX_SETTING(prefs, m_, H5StatisticsFile, true)
  READ_CHECKBOX_SETTING(prefs, m_, GrainDataFile, true)
  READ_BOOL_SETTING(prefs, m_, WriteGrainSize, true);
  READ_BOOL_SETTING(prefs, m_, WriteGrainShapes, true);
  READ_BOOL_SETTING(prefs, m_, WriteNumNeighbors, true);
  READ_BOOL_SETTING(prefs, m_, WriteAverageOrientations, true);

  READ_CHECKBOX_SETTING(prefs, m_, VisualizationVizFile, true);
  READ_BOOL_SETTING(prefs, m_, WriteBinaryVTKFile, true);
  READ_BOOL_SETTING(prefs, m_, WriteSurfaceVoxelScalars, true)
  READ_BOOL_SETTING(prefs, m_, WritePhaseIdScalars, true)
  READ_BOOL_SETTING(prefs, m_, WriteKernelMisorientationsScalars, true)
  READ_BOOL_SETTING(prefs, m_, WriteIPFColorScalars, true)
  READ_BOOL_SETTING(prefs, m_, WriteBinaryVTKFile, true)

  prefs.endGroup();
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::writeSettings(QSettings &prefs)
{
  prefs.beginGroup("MicrostructureStatistics");
  WRITE_STRING_SETTING(prefs, m_, OutputDir)
  WRITE_STRING_SETTING(prefs, m_, OutputFilePrefix)
  WRITE_STRING_SETTING(prefs, m_, InputFile)

  WRITE_BOOL_SETTING(prefs, m_, ComputeGrainSize, true);
  WRITE_BOOL_SETTING(prefs, m_, ComputeGrainShapes, true);
  WRITE_BOOL_SETTING(prefs, m_, ComputeNumNeighbors, true);
  WRITE_BOOL_SETTING(prefs, m_, ComputeMDF, true);
  WRITE_BOOL_SETTING(prefs, m_, ComputeODF, true);

  WRITE_SETTING(prefs, m_, BinStepSize);

  WRITE_CHECKBOX_SETTING(prefs, m_, H5StatisticsFile)
  WRITE_CHECKBOX_SETTING(prefs, m_, GrainDataFile)
  WRITE_BOOL_SETTING(prefs, m_, WriteGrainSize, true);
  WRITE_BOOL_SETTING(prefs, m_, WriteGrainShapes, true);
  WRITE_BOOL_SETTING(prefs, m_, WriteNumNeighbors, true);
  WRITE_BOOL_SETTING(prefs, m_, WriteAverageOrientations, true);

  WRITE_CHECKBOX_SETTING(prefs, m_, VisualizationVizFile)
  WRITE_BOOL_SETTING(prefs, m_, WriteBinaryVTKFile, true);
  WRITE_BOOL_SETTING(prefs, m_, WriteSurfaceVoxelScalars, true)
  WRITE_BOOL_SETTING(prefs, m_, WritePhaseIdScalars, true)
  WRITE_BOOL_SETTING(prefs, m_, WriteKernelMisorientationsScalars, true)
  WRITE_BOOL_SETTING(prefs, m_, WriteIPFColorScalars, true)
  WRITE_BOOL_SETTING(prefs, m_, WriteBinaryVTKFile, true)

  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList)
    {
      w->setEnabled(b);
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::on_m_InputFileBtn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Structure File"), m_OpenDialogLastDirectory, tr("Any File (*.*)"));
  if (true == file.isEmpty())
  {
    return;
  }
  QFileInfo fi(file);
  QString ext = fi.suffix();
  m_InputFile->setText(fi.absoluteFilePath());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::setupGui()
{

  QR3DFileCompleter* com2 = new QR3DFileCompleter(this, true);
  m_OutputDir->setCompleter(com2);
  QObject::connect(com2, SIGNAL(activated(const QString &)), this, SLOT(on_m_OutputDir_textChanged(const QString &)));

  QR3DFileCompleter* com3 = new QR3DFileCompleter(this, false);
  m_InputFile->setCompleter(com3);
  QObject::connect(com3, SIGNAL(activated(const QString &)), this, SLOT(on_m_InputFile_textChanged(const QString &)));

  QString msg("All files will be over written that appear in the output directory.");

  QFileInfo fi(m_OutputDir->text() + QDir::separator() + AIM::SyntheticBuilder::VisualizationVizFile.c_str());

  m_WidgetList << m_OutputDir << m_OutputDirBtn;
  m_WidgetList << m_OutputFilePrefix;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::on_m_LoadSettingsBtn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Settings File"), m_OpenDialogLastDirectory, tr("Settings File (*.txt)"));
  if (true == file.isEmpty())
  {
    return;
  }
  QSettings prefs(file, QSettings::IniFormat, this);
  readSettings(prefs);
  if (verifyPathExists(m_OutputDir->text(), m_OutputDir))
  {
    checkIOFiles();
  }
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::on_m_SaveSettingsBtn_clicked()
{
  QString proposedFile = m_OpenDialogLastDirectory + QDir::separator() + "MicrostructureStatisticsSettings.txt";
  QString file = QFileDialog::getSaveFileName(this, tr("Save Grain Generator Settings"), proposedFile, tr("*.txt"));
  if (true == file.isEmpty())
  {
    return;
  }

  QSettings prefs(file, QSettings::IniFormat, this);
  writeSettings(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::checkIOFiles()
{
  //CHECK_QLINEEDIT_FILE_EXISTS(m_InputFile)
  if (verifyPathExists(m_InputFile->text(), m_InputFile) == true)
  {
    // Load up the voxel data
    H5VoxelReader::Pointer h5Reader = H5VoxelReader::New();
    h5Reader->setFilename(m_InputFile->text().toStdString());
    int dims[3];
    float spacing[3];
    int err = h5Reader->getSizeAndResolution(dims, spacing);
    if (err >= 0)
    {
      xDim->setText(QString::number(dims[0]));
      yDim->setText(QString::number(dims[1]));
      zDim->setText(QString::number(dims[2]));
    }
  }

  verifyPathExists(m_OutputDir->text(), m_OutputDir);

  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::Reconstruction, m_ , VisualizationVizFile)
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::MicroStats, m_, GrainDataFile)
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::MicroStats, m_, H5StatisticsFile)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::on_m_InputFile_textChanged(const QString &text)
{
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::on_m_OutputDirBtn_clicked()
{
  QString outputFile = this->m_OpenDialogLastDirectory + QDir::separator();
  outputFile = QFileDialog::getExistingDirectory(this, tr("Select Grain Generator Output Directory"), outputFile);
  if (!outputFile.isNull())
  {
    this->m_OutputDir->setText(outputFile);
    if (verifyPathExists(outputFile, m_OutputDir) == true)
    {
      checkIOFiles();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::on_m_OutputDir_textChanged(const QString &text)
{
  if (verifyPathExists(m_OutputDir->text(), m_OutputDir))
  {
    checkIOFiles();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::on_m_GoBtn_clicked()
{

  if (m_GoBtn->text().compare("Cancel") == 0)
  {
    if (m_MicrostructureStatistics != NULL)
    {
      //std::cout << "canceling from GUI...." << std::endl;
      emit cancelProcess();
    }
    return;
  }

  if (false == sanityCheckOutputDirectory(m_OutputDir, QString("Grain Generator")))
  {
    return;
  }
  SANITY_CHECK_INPUT(m_, OutputDir)

  if (m_WorkerThread != NULL)
  {
    m_WorkerThread->wait(); // Wait until the thread is complete
    delete m_WorkerThread; // Kill the thread
    m_WorkerThread = NULL;
  }
  m_WorkerThread = new QThread(); // Create a new Thread Resource

  m_MicrostructureStatistics = new QMicrostructureStatistics(NULL);

  // Move the MicrostructureStatistics object into the thread that we just created.
  m_MicrostructureStatistics->moveToThread(m_WorkerThread);
  m_MicrostructureStatistics->setOutputDirectory(QDir::toNativeSeparators(m_OutputDir->text()).toStdString());
  m_MicrostructureStatistics->setOutputFilePrefix(m_OutputFilePrefix->text().toStdString());
  m_MicrostructureStatistics->setInputFile(QDir::toNativeSeparators(m_InputFile->text()).toStdString());

  /********************************
   * Gather any other values from the User Interface and send those to the lower level code
   */
  m_MicrostructureStatistics->setComputeGrainSize(m_ComputeGrainSize);
  m_MicrostructureStatistics->setComputeGrainShapes(m_ComputeGrainShapes);
  m_MicrostructureStatistics->setComputeNumNeighbors(m_ComputeNumNeighbors);
  m_MicrostructureStatistics->setComputeODF(m_ComputeODF);
  m_MicrostructureStatistics->setComputeMDF(m_ComputeMDF);

  m_MicrostructureStatistics->setBinStepSize(m_BinStepSize->value());
  m_MicrostructureStatistics->setWriteH5StatsFile(m_H5StatisticsFile->isChecked());

  m_MicrostructureStatistics->setWriteGrainFile(m_GrainDataFile->isChecked());
  m_MicrostructureStatistics->setWriteGrainSize(m_WriteGrainSize);
  m_MicrostructureStatistics->setWriteGrainShapes(m_WriteGrainShapes);
  m_MicrostructureStatistics->setWriteNumNeighbors(m_WriteNumNeighbors);
  m_MicrostructureStatistics->setWriteAverageOrientations(m_WriteAverageOrientations);

  m_MicrostructureStatistics->setWriteVtkFile(m_VisualizationVizFile->isChecked());
  m_MicrostructureStatistics->setWriteSurfaceVoxel(m_WriteSurfaceVoxelScalars);
  m_MicrostructureStatistics->setWritePhaseId(m_WritePhaseIdScalars);
  m_MicrostructureStatistics->setWriteKernelMisorientations(m_WriteKernelMisorientationsScalars);
  m_MicrostructureStatistics->setWriteIPFColor(m_WriteIPFColorScalars);
  m_MicrostructureStatistics->setWriteBinaryVTKFiles(m_WriteBinaryVTKFile);

  /* Connect the signal 'started()' from the QThread to the 'run' slot of the
   * Reconstruction object. Since the Reconstruction object has been moved to another
   * thread of execution and the actual QThread lives in *this* thread then the
   * type of connection will be a Queued connection.
   */
  // When the thread starts its event loop, start the MicrostructureStatistics going
  connect(m_WorkerThread, SIGNAL(started()), m_MicrostructureStatistics, SLOT(run()));

  // When the Reconstruction ends then tell the QThread to stop its event loop
  connect(m_MicrostructureStatistics, SIGNAL(finished() ), m_WorkerThread, SLOT(quit()));

  // When the QThread finishes, tell this object that it has finished.
  connect(m_WorkerThread, SIGNAL(finished()), this, SLOT( threadFinished() ));

  // Send Progress from the Reconstruction to this object for display
  connect(m_MicrostructureStatistics, SIGNAL (updateProgress(int)), this, SLOT(threadProgressed(int) ));

  // Send progress messages from Reconstruction to this object for display
  connect(m_MicrostructureStatistics, SIGNAL (updateMessage(QString)), this, SLOT(threadHasMessage(QString) ));

  // If the use clicks on the "Cancel" button send a message to the Reconstruction object
  // We need a Direct Connection so the
  connect(this, SIGNAL(cancelProcess() ), m_MicrostructureStatistics, SLOT (on_CancelWorker() ), Qt::DirectConnection);

  setWidgetListEnabled(false);
  emit
  processStarted();
  m_WorkerThread->start();
  m_GoBtn->setText("Cancel");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::threadFinished()
{
  // std::cout << "MicrostructureStatisticsWidget::grainGenerator_Finished()" << std::endl;
  m_GoBtn->setText("Go");
  setWidgetListEnabled(true);
  this->m_progressBar->setValue(0);
  emit
  processEnded();
  checkIOFiles();
  m_MicrostructureStatistics->deleteLater();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::threadProgressed(int val)
{
  this->m_progressBar->setValue(val);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::threadHasMessage(QString message)
{
  if (NULL != this->statusBar())
  {
    this->statusBar()->showMessage(message);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::on_m_OutputFilePrefix_textChanged(const QString &text)
{
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::on_m_VtkOptionsBtn_clicked()
{
  QVector<QString> options;
  options.push_back("Write Surface Voxel Scalars");
  options.push_back("Write Phase Ids Scalars");
  options.push_back("Write Kernel Misorientation Scalars");
  options.push_back("Write IPF Color Scalars");
  options.push_back("Write Binary VTK File");
  QCheckboxDialog d(options, this);
  d.setWindowTitle(QString("VTK Output Options"));

  d.setValue("Write Surface Voxel Scalars", m_WriteSurfaceVoxelScalars);
  d.setValue("Write Phase Ids Scalars", m_WritePhaseIdScalars);
  d.setValue("Write Kernel Misorientation Scalars", m_WriteKernelMisorientationsScalars);
  d.setValue("Write IPF Color Scalars", m_WriteIPFColorScalars);
  d.setValue("Write Binary VTK File", m_WriteBinaryVTKFile);

  int ret = d.exec();
  if (ret == QDialog::Accepted)
  {
    m_WriteSurfaceVoxelScalars = d.getValue("Write Surface Voxel Scalars");
    m_WritePhaseIdScalars = d.getValue("Write Phase Ids Scalars");
    m_WriteKernelMisorientationsScalars = d.getValue("Write Kernel Misorientation Scalars");
    m_WriteIPFColorScalars = d.getValue("Write IPF Color Scalars");
    m_WriteBinaryVTKFile = d.getValue("Write Binary VTK File");
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::on_m_GrainFileOptionsBtn_clicked()
{
  QVector<QString> options;
  options.push_back("Grain Size");
  options.push_back("Grain Shape");
  options.push_back("Number Neighbors");
  options.push_back("Average Orientations");

  QCheckboxDialog d(options, this);
  d.setWindowTitle(QString("Grain File Output Options"));

  d.setValue("Grain Size", m_WriteGrainSize);
  d.setValue("Grain Shape", m_WriteGrainShapes);
  d.setValue("Number Neighbors", m_WriteNumNeighbors);
  d.setValue("Average Orientations", m_WriteAverageOrientations);

  int ret = d.exec();
  if (ret == QDialog::Accepted)
  {
    m_WriteGrainSize = d.getValue("Grain Size");
    m_WriteGrainShapes = d.getValue("Grain Shape");
    m_WriteNumNeighbors = d.getValue("Number Neighbors");
    m_WriteAverageOrientations = d.getValue("Average Orientations");
    if(m_WriteGrainSize == true) m_ComputeGrainSize = true;
    if(m_WriteGrainShapes == true) m_ComputeGrainShapes = true;
    if(m_WriteNumNeighbors == true) m_ComputeNumNeighbors = true;
    if(m_WriteAverageOrientations == true) m_ComputeAverageOrientations = true;
    if (m_H5StatisticsFile->isChecked() == false)
	{
	    if(m_WriteGrainSize == false) m_ComputeGrainSize = false;
	    if(m_WriteGrainShapes == false) m_ComputeGrainShapes = false;
	    if(m_WriteNumNeighbors == false) m_ComputeNumNeighbors = false;
		if(m_WriteAverageOrientations == false) m_ComputeAverageOrientations = false;
	}
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::on_m_H5StatisticsFile_stateChanged()
{
  if (m_H5StatisticsFile->isChecked() == true)
  {
    m_ComputeGrainSize = true;
    m_ComputeGrainShapes = true;
    m_ComputeNumNeighbors = true;
    m_ComputeODF = true;
    m_ComputeMDF = true;
  }
  else
  {
    if(m_WriteGrainSize == false) m_ComputeGrainSize = false;
    if(m_WriteGrainShapes == false) m_ComputeGrainShapes = false;
    if(m_WriteNumNeighbors == false) m_ComputeNumNeighbors = false;
    m_ComputeODF = false;
    m_ComputeMDF = false;
  }
}
