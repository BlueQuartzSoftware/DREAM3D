/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "VolumeOpsWidget.h"


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

#include "DREAM3DLib/Common/Constants.h"

#include "QtSupport/DREAM3DQtMacros.h"
#include "QtSupport/QR3DFileCompleter.h"
#include "QtSupport/QCheckboxDialog.h"
#include "DREAM3DLib/HDF5/H5VoxelReader.h"

#include "VolumeOps/VolumeOps.h"
#include "VolumeOpsPlugin.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeOpsWidget::VolumeOpsWidget(QWidget *parent) :
DREAM3DPluginFrame(parent),
m_VolumeOps(NULL),
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
VolumeOpsWidget::~VolumeOpsWidget()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeOpsWidget::readSettings(QSettings &prefs)
{
  qint32 i;
  bool ok;

  prefs.beginGroup("VolumeOps");
  READ_FILEPATH_SETTING(prefs, m_, H5InputFile, "");
  on_m_H5InputFile_textChanged(QString(""));

  READ_FILEPATH_SETTING(prefs, m_, OutputDir, "");
  READ_STRING_SETTING(prefs, m_, OutputFilePrefix, "VolumeOps_")

  READ_SETTING(prefs, m_, xMin, ok, i, 0, Int)
  READ_SETTING(prefs, m_, yMin, ok, i, 0, Int)
  READ_SETTING(prefs, m_, zMin, ok, i, 0, Int)
  READ_SETTING(prefs, m_, xMax, ok, i, 0, Int)
  READ_SETTING(prefs, m_, yMax, ok, i, 0, Int)
  READ_SETTING(prefs, m_, zMax, ok, i, 0, Int)
  READ_SETTING(prefs, m_, XRes_New, ok, i, 0.0, Float)
  READ_SETTING(prefs, m_, YRes_New, ok, i, 0.0, Float)
  READ_SETTING(prefs, m_, ZRes_New, ok, i, 0.0, Float)


  prefs.endGroup();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeOpsWidget::writeSettings(QSettings &prefs)
{
  prefs.beginGroup("VolumeOps");
  WRITE_STRING_SETTING(prefs, m_, H5InputFile)
  WRITE_STRING_SETTING(prefs, m_, OutputDir)
  WRITE_STRING_SETTING(prefs, m_, OutputFilePrefix)

  WRITE_SETTING(prefs, m_, xMin)
  WRITE_SETTING(prefs, m_, yMin)
  WRITE_SETTING(prefs, m_, zMin)
  WRITE_SETTING(prefs, m_, xMax)
  WRITE_SETTING(prefs, m_, yMax)
  WRITE_SETTING(prefs, m_, zMax)
  WRITE_SETTING(prefs, m_, XRes_New)
  WRITE_SETTING(prefs, m_, YRes_New)
  WRITE_SETTING(prefs, m_, ZRes_New)

  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeOpsWidget::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList) {
    w->setEnabled(b);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeOpsWidget::setupGui()
{

  QR3DFileCompleter* com = new QR3DFileCompleter(this, false);
  m_H5InputFile->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString &)),
           this, SLOT(on_m_H5InputFile_textChanged(const QString &)));

  QR3DFileCompleter* com2 = new QR3DFileCompleter(this, true);
  m_OutputDir->setCompleter(com2);
  QObject::connect( com2, SIGNAL(activated(const QString &)),
           this, SLOT(on_m_OutputDir_textChanged(const QString &)));


  m_WidgetList << m_H5InputFile << m_OutputDir << m_OutputDirBtn << m_OutputFilePrefix;
  m_WidgetList << m_LoadSettingsBtn << m_SaveSettingsBtn << m_xMin;
  m_WidgetList << m_yMin << m_zMin << m_xMax << m_yMax << m_zMax;
  m_WidgetList << m_XRes_New << m_YRes_New << m_ZRes_New;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeOpsWidget::on_m_LoadSettingsBtn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Settings File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("Settings File (*.txt)") );
  if ( true == file.isEmpty() ){  return;  }
  QSettings prefs(file, QSettings::IniFormat, this);
  readSettings(prefs);
  // Do any additional validation necessary

}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeOpsWidget::on_m_SaveSettingsBtn_clicked()
{
  QString proposedFile = m_OpenDialogLastDirectory + QDir::separator() + "VolumeOpsSettings.txt";
  QString file = QFileDialog::getSaveFileName(this, tr("Save VolumeOps Settings"),
                                              proposedFile,
                                              tr("*.txt") );
  if ( true == file.isEmpty() ){ return;  }

  QSettings prefs(file, QSettings::IniFormat, this);
  writeSettings(prefs);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeOpsWidget::checkIOFiles()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeOpsWidget::on_m_H5Btn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Input File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("HDF5 EBSD Files (*.h5voxel)") );
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
void VolumeOpsWidget::on_m_H5InputFile_textChanged(const QString &text)
{

  if (verifyPathExists(m_H5InputFile->text(), m_H5InputFile) )
  {
    QFileInfo fi(m_H5InputFile->text());
    QString outPath = fi.absolutePath() + QDir::separator() + fi.baseName() + "_VolumeOps";
    outPath = QDir::toNativeSeparators(outPath);
    m_OutputDir->setText(outPath);
  }

  QFileInfo fi(m_H5InputFile->text());
  if (fi.exists() && fi.isFile())
  {
    // Set the output file Prefix based on the name of the input file
    m_OutputFilePrefix->setText(fi.baseName() + QString("_") );

    H5VoxelReader::Pointer h5Reader = H5VoxelReader::New();
    h5Reader->setFileName(m_H5InputFile->text().toStdString());
    int dims[3];
    float spacing[3];
    float origin[3];
    int err = h5Reader->getSizeResolutionOrigin(dims, spacing, origin);
    if (err >= 0)
    {
      m_xDim->setText(QString::number(dims[0]));
      m_yDim->setText(QString::number(dims[1]));
      m_zDim->setText(QString::number(dims[2]));
      m_XRes->setText(QString::number(spacing[0]));
      m_YRes->setText(QString::number(spacing[1]));
      m_ZRes->setText(QString::number(spacing[2]));
      m_xMin->setValue(0);
      m_xMin->setRange(0,dims[0]-1);
      m_yMin->setValue(0);
      m_yMin->setRange(0,dims[1]-1);
      m_zMin->setValue(0);
      m_zMin->setRange(0,dims[2]-1);
      m_xMax->setValue(dims[0]);
      m_xMax->setRange(0,dims[0]-1);
      m_yMax->setValue(dims[1]);
      m_yMax->setRange(0,dims[1]-1);
      m_zMax->setValue(dims[2]);
      m_zMax->setRange(0,dims[2]-1);
    }

  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeOpsWidget::on_m_OutputDirBtn_clicked()
{
  QString outputFile = this->m_OpenDialogLastDirectory + QDir::separator();
  outputFile = QFileDialog::getExistingDirectory(this, tr("Select Output Directory"), outputFile);
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
void VolumeOpsWidget::on_m_OutputDir_textChanged(const QString & text)
{
  verifyPathExists(m_OutputDir->text(), m_OutputDir);
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeOpsWidget::on_m_OutputFilePrefix_textChanged(const QString &text)
{
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeOpsWidget::on_m_GoBtn_clicked()
{

  if (m_GoBtn->text().compare("Cancel") == 0)
  {
    if(m_VolumeOps!= NULL)
    {
      //std::cout << "canceling from GUI...." << std::endl;
      emit cancelPipeline();
    }
    return;
  }

#if 0
  if (false == sanityCheckOutputDirectory(m_OutputDir, QString("VolumeOps")) )
  {
    return;
  }
  SANITY_CHECK_INPUT(m_, OutputDir)
#endif


  if (m_WorkerThread != NULL)
  {
    m_WorkerThread->wait(); // Wait until the thread is complete
    delete m_WorkerThread; // Kill the thread
    m_WorkerThread = NULL;
  }
  m_WorkerThread = new QThread(); // Create a new Thread Resource

  m_VolumeOps = new QVolumeOps(NULL);

  // Move the VolumeOps object into the thread that we just created.
  m_VolumeOps->moveToThread(m_WorkerThread);

  // Pull the values from the GUI and push them into the m_VolumeOps variable


  /* Connect the signal 'started()' from the QThread to the 'run' slot of the
   * VolumeOps object. Since the VolumeOps object has been moved to another
   * thread of execution and the actual QThread lives in *this* thread then the
   * type of connection will be a Queued connection.
   */
  // When the thread starts its event loop, start the VolumeOps going
  connect(m_WorkerThread, SIGNAL(started()),
          m_VolumeOps, SLOT(run()));

  // When the VolumeOps ends then tell the QThread to stop its event loop
  connect(m_VolumeOps, SIGNAL(finished() ),
          m_WorkerThread, SLOT(quit()) );

  // When the QThread finishes, tell this object that it has finished.
  connect(m_WorkerThread, SIGNAL(finished()),
          this, SLOT( pipelineComplete() ) );

  // If the use clicks on the "Cancel" button send a message to the VolumeOps object
  // We need a Direct Connection so the
  connect(this, SIGNAL(cancelPipeline() ),
          m_VolumeOps, SLOT (on_CancelWorker() ) , Qt::DirectConnection);

  // Send Progress from the VolumeOps to this object for display
  connect(m_VolumeOps, SIGNAL (updateProgress(int)),
          this, SLOT(pipelineProgress(int) ) );

  // Send progress messages from VolumeOps to this object for display
  connect(m_VolumeOps, SIGNAL (progressMessage(QString)),
          this, SLOT(addProgressMessage(QString) ));

  // Send progress messages from VolumeOps to this object for display
  connect(m_VolumeOps, SIGNAL (warningMessage(QString)),
          this, SLOT(addWarningMessage(QString) ));

  // Send progress messages from VolumeOps to this object for display
  connect(m_VolumeOps, SIGNAL (errorMessage(QString)),
          this, SLOT(addErrorMessage(QString) ));

  m_VolumeOps->setH5InputFile( QDir::toNativeSeparators(m_H5InputFile->text()).toStdString());
  m_VolumeOps->setOutputDirectory(QDir::toNativeSeparators(m_OutputDir->text()).toStdString());
  m_VolumeOps->setOutputFilePrefix(m_OutputFilePrefix->text().toStdString());

  m_VolumeOps->setXRes_New(m_XRes_New->value());
  m_VolumeOps->setYRes_New(m_YRes_New->value());
  m_VolumeOps->setZRes_New(m_ZRes_New->value());
  m_VolumeOps->setxMin(m_xMin->value());
  m_VolumeOps->setxMax(m_xMax->value());
  m_VolumeOps->setyMin(m_yMin->value());
  m_VolumeOps->setyMax(m_yMax->value());
  m_VolumeOps->setzMin(m_zMin->value());
  m_VolumeOps->setzMax(m_zMax->value());

  setWidgetListEnabled(false);
  emit pipelineStarted();
  m_WorkerThread->start();
  m_GoBtn->setText("Cancel");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeOpsWidget::pipelineComplete()
{
 // std::cout << "VolumeOpsWidget::VolumeOps_Finished()" << std::endl;
  m_GoBtn->setText("Go");
  setWidgetListEnabled(true);
  this->m_progressBar->setValue(0);
  emit pipelineEnded();
  checkIOFiles();
  m_VolumeOps->deleteLater();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeOpsWidget::pipelineProgress(int val)
{
  this->m_progressBar->setValue( val );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeOpsWidget::addErrorMessage(QString message)
{
  QString title = QString::fromStdString(DREAM3D::UIPlugins::VolumeOpsDisplayName).append(" Error");
  displayDialogBox(title, message, QMessageBox::Critical);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeOpsWidget::addWarningMessage(QString message)
{
  QString title = QString::fromStdString(DREAM3D::UIPlugins::VolumeOpsDisplayName).append(" Warning");
  displayDialogBox(title, message, QMessageBox::Warning);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeOpsWidget::addProgressMessage(QString message)
{
  if (NULL != this->statusBar()) {
    this->statusBar()->showMessage(message);
  }
}
