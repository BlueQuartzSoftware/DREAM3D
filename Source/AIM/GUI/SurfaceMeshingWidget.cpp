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

#include "SurfaceMeshingWidget.h"

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

#include "QtSupport/QR3DFileCompleter.h"
#include "QtSupport/AIM_QtMacros.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshingWidget::SurfaceMeshingWidget(QWidget *parent) :
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
SurfaceMeshingWidget::~SurfaceMeshingWidget()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshingWidget::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList) {
    w->setEnabled(b);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshingWidget::writeSettings(QSettings &prefs)
{
  prefs.beginGroup("SurfaceMesh");
  WRITE_STRING_SETTING(prefs, sm_InputFile);
  WRITE_STRING_SETTING(prefs, sm_OutputDir);
  WRITE_BOOL_SETTING(prefs, sm_SmoothMesh, sm_SmoothMesh->isChecked() );
  WRITE_BOOL_SETTING(prefs, sm_LockQuadPoints, sm_LockQuadPoints->isChecked() );
  WRITE_SETTING(prefs, sm_SmoothIterations );
  WRITE_SETTING(prefs, sm_WriteOutputFileIncrement );
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshingWidget::readSettings(QSettings &prefs)
{
  QString val;
  bool ok;
  qint32 i;
  //double d;

  prefs.beginGroup("SurfaceMesh");
  READ_FILEPATH_SETTING(prefs, sm_InputFile, "");
  READ_FILEPATH_SETTING(prefs, sm_OutputDir, "");

  READ_BOOL_SETTING(prefs, sm_, SmoothMesh, false);
  READ_BOOL_SETTING(prefs, sm_, LockQuadPoints, false);
  READ_SETTING(prefs, sm_SmoothIterations, ok, i, 1 , Int);
  READ_SETTING(prefs, sm_WriteOutputFileIncrement, ok, i, 10 , Int);
  prefs.endGroup();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshingWidget::setupGui()
{

  if (NULL == sm_InputFile->completer()){
    QR3DFileCompleter* com = new QR3DFileCompleter(this, false);
    sm_InputFile->setCompleter(com);
    QObject::connect( com, SIGNAL(activated(const QString &)),
             this, SLOT(on_sm_InputFile_textChanged(const QString &)));
  }

  if (NULL == sm_OutputDir->completer()) {
    QR3DFileCompleter* com4 = new QR3DFileCompleter(this, true);
    sm_OutputDir->setCompleter(com4);
    QObject::connect( com4, SIGNAL(activated(const QString &)),
             this, SLOT(on_sm_OutputDir_textChanged(const QString &)));
  }

  sm_Message->setText("Any existing output files will be over written with new versions during the operation.");
  m_WidgetList << sm_InputFile;
  m_WidgetList << sm_InputFileBtn << sm_OutputDir << sm_OutputDirBtn;
  m_WidgetList << sm_Message << sm_LockQuadPoints << sm_SmoothIterations << sm_SmoothMesh;
  m_WidgetList << sm_WriteOutputFileIncrement;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshingWidget::checkIOFiles()
{

  if ( verifyPathExists(sm_InputFile->text(), sm_InputFile) == true )
  {
    QFileInfo fi (sm_InputFile->text() );
    QString ext = fi.suffix();
  }
  CHECK_QLINEEDIT_FILE_EXISTS(sm_InputFile)

  verifyPathExists(sm_OutputDir->text(), sm_OutputDir);

  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::SurfaceMeshing, sm_, NodesFile)
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::SurfaceMeshing, sm_, TrianglesFile)
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::SurfaceMeshing, sm_, NodesRawFile)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshingWidget::on_sm_InputFileBtn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Input File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("Viz Files (*.vtk)") );
  if ( true == file.isEmpty() ){return;  }
  QFileInfo fi (file);
  QString ext = fi.suffix();
  sm_InputFile->setText(fi.absoluteFilePath());
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshingWidget::on_sm_OutputDirBtn_clicked()
{
  QString outputFile = this->m_OpenDialogLastDirectory + QDir::separator();
  outputFile = QFileDialog::getExistingDirectory(this, tr("Select Surface Meshing Output Directory"), outputFile);
  if (!outputFile.isNull())
  {
    this->sm_OutputDir->setText(outputFile);
    if (verifyPathExists(outputFile, sm_OutputDir) == true )
    {
      checkIOFiles();
      sm_OutputDir->setText(outputFile);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshingWidget::on_sm_InputFile_textChanged(const QString & text)
{
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshingWidget::on_sm_OutputDir_textChanged(const QString & text)
{
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshingWidget::on_sm_GoBtn_clicked()
{
  if (sm_GoBtn->text().compare("Cancel") == 0)
  {
    if(m_SurfaceMesh.get() != NULL)
    {
      //std::cout << "canceling from GUI...." << std::endl;
      emit cancelProcess();
    }
    return;
  }

  SANITY_CHECK_INPUT(sm_, InputFile)
  SANITY_CHECK_INPUT(sm_, OutputDir)

  if (m_WorkerThread != NULL)
  {
    m_WorkerThread->wait(); // Wait until the thread is complete
    delete m_WorkerThread; // Kill the thread
    m_WorkerThread = NULL;
  }
  m_WorkerThread = new QThread(); // Create a new Thread Resource


  m_SurfaceMesh = SurfaceMesh::New(NULL);
  // Move the Reconstruction object into the thread that we just created.
  m_SurfaceMesh->moveToThread(m_WorkerThread);

  m_SurfaceMesh->setInputFile(sm_InputFile->text().toStdString() );

  QString od = sm_OutputDir->text();
  if (od.endsWith('/') == false && od.endsWith('\\') == false)
  {
    od += QDir::separator();
  }

  m_SurfaceMesh->setOutputDirectory(od.toStdString());
  m_SurfaceMesh->setSmoothMesh(sm_SmoothMesh->isChecked());
  m_SurfaceMesh->setSmoothIterations(sm_SmoothIterations->value());
  m_SurfaceMesh->setSmoothFileOutputIncrement(sm_WriteOutputFileIncrement->value());
  m_SurfaceMesh->setSmoothLockQuadPoints(sm_LockQuadPoints->isChecked());


  /* Connect the signal 'started()' from the QThread to the 'run' slot of the
   * Reconstruction object. Since the Reconstruction object has been moved to another
   * thread of execution and the actual QThread lives in *this* thread then the
   * type of connection will be a Queued connection.
   */
  // When the thread starts its event loop, start the Reconstruction going
  connect(m_WorkerThread, SIGNAL(started()),
          m_SurfaceMesh.get(), SLOT(compute()));

  // When the Reconstruction ends then tell the QThread to stop its event loop
  connect(m_SurfaceMesh.get(), SIGNAL(finished() ),
          m_WorkerThread, SLOT(quit()) );

  // When the QThread finishes, tell this object that it has finished.
  connect(m_WorkerThread, SIGNAL(finished()),
          this, SLOT( threadFinished() ) );

  // Send Progress from the Reconstruction to this object for display
  connect(m_SurfaceMesh.get(), SIGNAL (updateProgress(int)),
    this, SLOT(threadProgressed(int) ) );

  // Send progress messages from Reconstruction to this object for display
  connect(m_SurfaceMesh.get(), SIGNAL (updateMessage(QString)),
          this, SLOT(threadHasMessage(QString) ) );

  // If the use clicks on the "Cancel" button send a message to the Reconstruction object
  // We need a Direct Connection so the
  connect(this, SIGNAL(cancelProcess() ),
          m_SurfaceMesh.get(), SLOT (on_CancelWorker() ) , Qt::DirectConnection);

  setWidgetListEnabled(false);
  emit processStarted();
  m_WorkerThread->start();
  sm_GoBtn->setText("Cancel");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshingWidget::threadFinished()
{
 // std::cout << "SurfaceMeshWidget::surface_meshing()" << std::endl;
  sm_GoBtn->setText("Go");
  setWidgetListEnabled(true);
  this->sm_progressBar->setValue(0);
  emit processEnded();
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshingWidget::threadProgressed(int value)
{
  sm_progressBar->setValue(value);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshingWidget::threadHasMessage(QString message)
{
  if (NULL != this->statusBar()) {
    this->statusBar()->showMessage(message);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshingWidget::on_outputFilePrefix_textChanged(const QString &text)
{
  checkIOFiles();
}

