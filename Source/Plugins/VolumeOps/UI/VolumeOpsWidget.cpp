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

  READ_FILEPATH_SETTING(prefs, m_, H5OutputFile, "");
 // READ_STRING_SETTING(prefs, m_, OutputFilePrefix, "VolumeOps_")

  READ_SETTING(prefs, m_, XMin, ok, i, 0, Int)
  READ_SETTING(prefs, m_, YMin, ok, i, 0, Int)
  READ_SETTING(prefs, m_, ZMin, ok, i, 0, Int)
  READ_SETTING(prefs, m_, XMax, ok, i, 0, Int)
  READ_SETTING(prefs, m_, YMax, ok, i, 0, Int)
  READ_SETTING(prefs, m_, ZMax, ok, i, 0, Int)
  READ_SETTING(prefs, m_, XResolution, ok, i, 0.0, Float)
  READ_SETTING(prefs, m_, YResolution, ok, i, 0.0, Float)
  READ_SETTING(prefs, m_, ZResolution, ok, i, 0.0, Float)


  prefs.endGroup();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeOpsWidget::writeSettings(QSettings &prefs)
{
  prefs.beginGroup("VolumeOps");
  WRITE_STRING_SETTING(prefs, m_, H5InputFile)
  WRITE_STRING_SETTING(prefs, m_, H5OutputFile)

  WRITE_SETTING(prefs, m_, XMin)
  WRITE_SETTING(prefs, m_, YMin)
  WRITE_SETTING(prefs, m_, ZMin)
  WRITE_SETTING(prefs, m_, XMax)
  WRITE_SETTING(prefs, m_, YMax)
  WRITE_SETTING(prefs, m_, ZMax)
  WRITE_SETTING(prefs, m_, XResolution)
  WRITE_SETTING(prefs, m_, YResolution)
  WRITE_SETTING(prefs, m_, ZResolution)

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

  QR3DFileCompleter* com2 = new QR3DFileCompleter(this, false);
  m_H5OutputFile->setCompleter(com2);
  QObject::connect( com2, SIGNAL(activated(const QString &)),
           this, SLOT(on_m_H5OutputFile_textChanged(const QString &)));


  m_WidgetList << m_H5InputFile << m_H5OutputFile << m_H5OutputFileBtn;
  m_WidgetList << m_LoadSettingsBtn << m_SaveSettingsBtn << m_XMin;
  m_WidgetList << m_YMin << m_ZMin << m_XMax << m_YMax << m_ZMax;
  m_WidgetList << m_XResolution << m_YResolution << m_ZResolution;
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

  verifyPathExists(m_H5InputFile->text(), m_H5InputFile);

  QFileInfo fi(m_H5InputFile->text());
  if (fi.exists() && fi.isFile())
  {
    // Set the output file Prefix based on the name of the input file
    //m_OutputFilePrefix->setText(fi.baseName() + QString("_") );

    H5VoxelReader::Pointer h5Reader = H5VoxelReader::New();
    h5Reader->setFileName(m_H5InputFile->text().toStdString());
    int dims[3];
    float spacing[3];
    float origin[3];
    int err = h5Reader->getSizeResolutionOrigin(dims, spacing, origin);
    if (err >= 0)
    {
      // These are the values from the data file and are displayed to the user
      m_xDim->setText(QString::number(dims[0]));
      m_yDim->setText(QString::number(dims[1]));
      m_zDim->setText(QString::number(dims[2]));
      m_XRes->setText(QString::number(spacing[0]));
      m_YRes->setText(QString::number(spacing[1]));
      m_ZRes->setText(QString::number(spacing[2]));

      // These are the new values that the user wants to use.
      m_XMin->setValue(0);
      m_XMin->setRange(0,dims[0]-1);
      m_YMin->setValue(0);
      m_YMin->setRange(0,dims[1]-1);
      m_ZMin->setValue(0);
      m_ZMin->setRange(0,dims[2]-1);
      m_XMax->setValue(dims[0]);
      m_XMax->setRange(0,dims[0]-1);
      m_YMax->setValue(dims[1]);
      m_YMax->setRange(0,dims[1]-1);
      m_ZMax->setValue(dims[2]);
      m_ZMax->setRange(0,dims[2]-1);

      m_XResolution->setValue(spacing[0]);
      m_YResolution->setValue(spacing[1]);
      m_ZResolution->setValue(spacing[2]);
    }

  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeOpsWidget::on_m_H5OutputFileBtn_clicked()
{
  QString file = QFileDialog::getSaveFileName(this, tr("Save HDF5 Voxel File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("HDF5 Voxel Files (*.h5voxel)") );
  if ( true == file.isEmpty() ){ return;  }
  QFileInfo fi (file);
  QString ext = fi.suffix();
  m_H5OutputFile->setText(fi.absoluteFilePath());
  m_OpenDialogLastDirectory = fi.path();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeOpsWidget::on_m_H5OutputFile_textChanged(const QString & text)
{
  verifyPathExists(m_H5OutputFile->text(), m_H5OutputFile);
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

  m_VolumeOps->setH5InputFile( QDir::toNativeSeparators(m_H5InputFile->text()).toStdString());
  m_VolumeOps->setH5OutputFile( QDir::toNativeSeparators(m_H5OutputFile->text()).toStdString());

  m_VolumeOps->setXRes(m_XResolution->value());
  m_VolumeOps->setYRes(m_YResolution->value());
  m_VolumeOps->setZRes(m_ZResolution->value());
  m_VolumeOps->setXMin(m_XMin->value());
  m_VolumeOps->setXMax(m_XMax->value());
  m_VolumeOps->setYMin(m_YMin->value());
  m_VolumeOps->setYMax(m_YMax->value());
  m_VolumeOps->setZMin(m_ZMin->value());
  m_VolumeOps->setZMax(m_ZMax->value());

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
  // If the operation was canceled, delete the output file
  if (m_VolumeOps->getCancel() == true)
  {
    QDir fi;
    fi.remove(m_H5OutputFile->text());
  }
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
