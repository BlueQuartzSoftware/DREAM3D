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


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MicrostructureStatisticsWidget::MicrostructureStatisticsWidget(QWidget *parent) :
AIMPluginFrame(parent),
m_MicrostructureStatistics(NULL),
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

  READ_FILEPATH_SETTING(prefs, m_, OutputDir, "");
  READ_STRING_SETTING(prefs, m_, OutputFilePrefix, "MicrostructureStatistics_")
  READ_FILEPATH_SETTING(prefs, m_, InputFile, "");

  prefs.endGroup();
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
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList) {
    w->setEnabled(b);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::on_m_InputFileBtn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Structure File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("Any File (*.*)") );
  if ( true == file.isEmpty() ){return;  }
  QFileInfo fi (file);
  QString ext = fi.suffix();
  m_InputFile->setText(fi.absoluteFilePath());
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::on_m_InputFile_textChanged(const QString &text)
{
  verifyPathExists(text, m_InputFile);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::setupGui()
{
  messageLabel->setText("");


  QR3DFileCompleter* com2 = new QR3DFileCompleter(this, true);
  m_OutputDir->setCompleter(com2);
  QObject::connect( com2, SIGNAL(activated(const QString &)),
           this, SLOT(on_m_OutputDir_textChanged(const QString &)));

  QR3DFileCompleter* com3 = new QR3DFileCompleter(this, false);
  m_InputFile->setCompleter(com3);
  QObject::connect( com3, SIGNAL(activated(const QString &)),
           this, SLOT(on_m_InputFile_textChanged(const QString &)));

  QString msg ("All files will be over written that appear in the output directory.");

  QFileInfo fi (m_OutputDir->text() + QDir::separator() +  AIM::SyntheticBuilder::VisualizationVizFile.c_str() );

  m_WidgetList << m_OutputDir << m_OutputDirBtn;
  m_WidgetList << m_OutputFilePrefix;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::on_m_LoadSettingsBtn_clicked()
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
void MicrostructureStatisticsWidget::on_m_SaveSettingsBtn_clicked()
{
  QString proposedFile = m_OpenDialogLastDirectory + QDir::separator() + "MicrostructureStatisticsSettings.txt";
  QString file = QFileDialog::getSaveFileName(this, tr("Save Grain Generator Settings"),
                                              proposedFile,
                                              tr("*.txt") );
  if ( true == file.isEmpty() ){ return;  }

  QSettings prefs(file, QSettings::IniFormat, this);
  writeSettings(prefs);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::checkIOFiles()
{
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::SyntheticBuilder, m_, CrystallographicErrorFile)
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::SyntheticBuilder, m_, GrainAnglesFile)
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::SyntheticBuilder, m_, GrainDataFile)
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::SyntheticBuilder, m_ , PackGrainsFile)
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::SyntheticBuilder, m_ , H5StatisticsFile)

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
    if (verifyPathExists(outputFile, m_OutputDir) == true )
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
  if (verifyPathExists(m_OutputDir->text(), m_OutputDir) )
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
    if(m_MicrostructureStatistics != NULL)
    {
      //std::cout << "canceling from GUI...." << std::endl;
      emit cancelProcess();
    }
    return;
  }


  if (false == sanityCheckOutputDirectory(m_OutputDir, QString("Grain Generator")) )
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


  /* Connect the signal 'started()' from the QThread to the 'run' slot of the
   * Reconstruction object. Since the Reconstruction object has been moved to another
   * thread of execution and the actual QThread lives in *this* thread then the
   * type of connection will be a Queued connection.
   */
  // When the thread starts its event loop, start the MicrostructureStatistics going
  connect(m_WorkerThread, SIGNAL(started()),
          m_MicrostructureStatistics, SLOT(run()));

  // When the Reconstruction ends then tell the QThread to stop its event loop
  connect(m_MicrostructureStatistics, SIGNAL(finished() ),
          m_WorkerThread, SLOT(quit()) );

  // When the QThread finishes, tell this object that it has finished.
  connect(m_WorkerThread, SIGNAL(finished()),
          this, SLOT( threadFinished() ) );

  // Send Progress from the Reconstruction to this object for display
  connect(m_MicrostructureStatistics, SIGNAL (updateProgress(int)),
    this, SLOT(threadProgressed(int) ) );

  // Send progress messages from Reconstruction to this object for display
  connect(m_MicrostructureStatistics, SIGNAL (updateMessage(QString)),
          this, SLOT(threadHasMessage(QString) ) );

  // If the use clicks on the "Cancel" button send a message to the Reconstruction object
  // We need a Direct Connection so the
  connect(this, SIGNAL(cancelProcess() ),
          m_MicrostructureStatistics, SLOT (on_CancelWorker() ) , Qt::DirectConnection);


  setWidgetListEnabled(false);
  emit processStarted();
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
  emit processEnded();
  checkIOFiles();
  m_MicrostructureStatistics->deleteLater();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::threadProgressed(int val)
{
  this->m_progressBar->setValue( val );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::threadHasMessage(QString message)
{
  if (NULL != this->statusBar()) {
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
