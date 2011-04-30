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

#include "VolumeMeshingWidget.h"


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
VolumeMeshingWidget::VolumeMeshingWidget(QWidget *parent) :
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
VolumeMeshingWidget::~VolumeMeshingWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMeshingWidget::readSettings(QSettings &prefs)
{
  QString val;
  bool ok;
  qint32 i;
  double d;

  prefs.beginGroup("VolumeMeshing");
  READ_FILEPATH_SETTING(prefs, m_, NodesFile, "");
  READ_FILEPATH_SETTING(prefs, m_, TrianglesFile, "");
  READ_FILEPATH_SETTING(prefs, m_, OutputDir, "");
  READ_SETTING(prefs, m_, NumGrains, ok, i, 1000 , Int);
  READ_SETTING(prefs, m_, XDim, ok, d, 100 , Double);
  READ_SETTING(prefs, m_, YDim, ok, d, 100 , Double);
  READ_SETTING(prefs, m_, ZDim, ok, d, 100 , Double);

  READ_SETTING(prefs, m_, XRes, ok, d, 0.25 , Double);
  READ_SETTING(prefs, m_, YRes, ok, d, 0.25 , Double);
  READ_SETTING(prefs, m_, ZRes, ok, d, 0.25 , Double);
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMeshingWidget::writeSettings(QSettings &prefs)
{
  prefs.beginGroup("VolumeMeshing");
  WRITE_STRING_SETTING(prefs, m_, NodesFile);
  WRITE_STRING_SETTING(prefs, m_, TrianglesFile);
  WRITE_STRING_SETTING(prefs, m_, OutputDir);
  WRITE_SETTING(prefs, m_, NumGrains);
  WRITE_SETTING(prefs, m_, XDim);
  WRITE_SETTING(prefs, m_, YDim);
  WRITE_SETTING(prefs, m_, ZDim);

  WRITE_SETTING(prefs, m_, XRes);
  WRITE_SETTING(prefs, m_, YRes);
  WRITE_SETTING(prefs, m_, ZRes);
  prefs.endGroup();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMeshingWidget::setupGui()
{
  if (NULL == m_NodesFile->completer()) {
    QR3DFileCompleter* com4 = new QR3DFileCompleter(this, true);
    m_NodesFile->setCompleter(com4);
    QObject::connect( com4, SIGNAL(activated(const QString &)),
      this, SLOT(on_m_NodesFile_textChanged(const QString &)));
  }

  if (NULL == m_TrianglesFile->completer()) {
    QR3DFileCompleter* com4 = new QR3DFileCompleter(this, true);
    m_TrianglesFile->setCompleter(com4);
    QObject::connect( com4, SIGNAL(activated(const QString &)),
      this, SLOT(on_m_TrianglesFile_textChanged(const QString &)));
  }

  if (NULL == m_OutputDir->completer()) {
    QR3DFileCompleter* com4 = new QR3DFileCompleter(this, true);
    m_OutputDir->setCompleter(com4);
    QObject::connect( com4, SIGNAL(activated(const QString &)),
      this, SLOT(on_m_OutputDir_textChanged(const QString &)));
  }


  m_WidgetList << m_NodesFile << m_NodesFileBtn << m_TrianglesFile << m_TrianglesFileBtn;
  m_WidgetList << m_XDim << m_XRes << m_YDim << m_YRes << m_ZDim << m_ZRes;
  m_WidgetList << m_NumGrains << m_OutputDir << m_OutputDirBtn;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMeshingWidget::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList) {
    w->setEnabled(b);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMeshingWidget::checkIOFiles()
{

  verifyPathExists(m_NodesFile->text(), m_NodesFile);
  verifyPathExists(m_TrianglesFile->text(), m_TrianglesFile);
  verifyPathExists(m_OutputDir->text(), m_OutputDir );

  CHECK_QLINEEDIT_FILE_EXISTS(m_NodesFile);
  CHECK_QLINEEDIT_FILE_EXISTS(m_TrianglesFile);

  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::VolumeMeshing, m_, MeshFile);
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::VolumeMeshing, m_, MeshFile2);
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::VolumeMeshing, m_, ElementQualityFile);
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::VolumeMeshing, m_, VoxelsFile);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMeshingWidget::on_m_OutputFilePrefix_textChanged(const QString &text)
{
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMeshingWidget::on_m_NodesFileBtn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Nodes File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("Txt Files (*.txt)") );
  if ( true == file.isEmpty() ){ return;  }
  QFileInfo fi (file);
  m_NodesFile->setText(fi.absoluteFilePath());
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMeshingWidget::on_m_TrianglesFileBtn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Triangles File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("Txt Files (*.txt)") );
  if ( true == file.isEmpty() ){ return;  }
  QFileInfo fi (file);
  m_TrianglesFile->setText(fi.absoluteFilePath());
  checkIOFiles();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMeshingWidget::on_m_OutputDirBtn_clicked()
{
  QString outputFile = this->m_OpenDialogLastDirectory + QDir::separator();
  outputFile = QFileDialog::getExistingDirectory(this, tr("Select Volume Meshing Output Directory"), outputFile);
  if (!outputFile.isNull())
  {
    this->m_OutputDir->setText(outputFile);
    if (verifyPathExists(outputFile, m_OutputDir) == true )
    {
      checkIOFiles();
      m_OutputDir->setText(outputFile);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMeshingWidget::on_m_GoBtn_clicked()
{

  if (m_GoBtn->text().compare("Cancel") == 0)
  {
    if(m_VolumeMesh.get() != NULL)
    {
      //std::cout << "canceling from GUI...." << std::endl;
      emit cancelProcess();
    }
    return;
  }


  SANITY_CHECK_INPUT(m_, NodesFile);
  SANITY_CHECK_INPUT(m_, TrianglesFile);
  SANITY_CHECK_INPUT(m_, OutputDir);

  if (m_WorkerThread != NULL)
  {
    m_WorkerThread->wait(); // Wait until the thread is complete
    delete m_WorkerThread; // Kill the thread
    m_WorkerThread = NULL;
  }
  m_WorkerThread = new QThread(); // Create a new Thread Resource
  // Move the Reconstruction object into the thread that we just created.
  m_VolumeMesh->moveToThread(m_WorkerThread);

  m_VolumeMesh = VolumeMesh::New(NULL);
  m_VolumeMesh->setNodesFile(m_NodesFile->text().toStdString() );
  m_VolumeMesh->setTrianglesFile(m_TrianglesFile->text().toStdString() );
  m_VolumeMesh->setOutputDirectory(m_OutputDir->text().toStdString());
  m_VolumeMesh->setOutputFilePrefix(m_OutputFilePrefix->text().toStdString());
  m_VolumeMesh->setXDim(m_XDim->value());
  m_VolumeMesh->setYDim(m_YDim->value());
  m_VolumeMesh->setZDim(m_ZDim->value());

  m_VolumeMesh->setXRes(m_XRes->value());
  m_VolumeMesh->setYRes(m_YRes->value());
  m_VolumeMesh->setZRes(m_ZRes->value());
  m_VolumeMesh->setNumGrains(m_NumGrains->value());

  /* Connect the signal 'started()' from the QThread to the 'run' slot of the
   * Reconstruction object. Since the Reconstruction object has been moved to another
   * thread of execution and the actual QThread lives in *this* thread then the
   * type of connection will be a Queued connection.
   */
  // When the thread starts its event loop, start the Reconstruction going
  connect(m_WorkerThread, SIGNAL(started()),
          m_VolumeMesh.get(), SLOT(compute()));

  // When the Reconstruction ends then tell the QThread to stop its event loop
  connect(m_VolumeMesh.get(), SIGNAL(finished() ),
          m_WorkerThread, SLOT(quit()) );

  // When the QThread finishes, tell this object that it has finished.
  connect(m_WorkerThread, SIGNAL(finished()),
          this, SLOT( threadFinished() ) );

  // Send Progress from the Reconstruction to this object for display
  connect(m_VolumeMesh.get(), SIGNAL (updateProgress(int)),
    this, SLOT(threadProgressed(int) ) );

  // Send progress messages from Reconstruction to this object for display
  connect(m_VolumeMesh.get(), SIGNAL (updateMessage(QString)),
          this, SLOT(threadHasMessage(QString) ) );

  // If the use clicks on the "Cancel" button send a message to the Reconstruction object
  // We need a Direct Connection so the
  connect(this, SIGNAL(cancelProcess() ),
          m_VolumeMesh.get(), SLOT (on_CancelWorker() ) , Qt::DirectConnection);

  setWidgetListEnabled(false);
  emit processStarted();
  m_WorkerThread->start();
  m_GoBtn->setText("Cancel");

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMeshingWidget::on_m_NodesFile_textChanged(const QString & text)
{
  checkIOFiles();
  verifyPathExists(m_NodesFile->text(), m_NodesFile);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMeshingWidget::on_m_TrianglesFile_textChanged(const QString & text)
{
  checkIOFiles();
  verifyPathExists(m_TrianglesFile->text(), m_TrianglesFile);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMeshingWidget::on_m_OutputDir_textChanged(const QString & text)
{
  checkIOFiles();
  verifyPathExists(m_OutputDir->text(), m_OutputDir);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMeshingWidget::threadFinished()
{
 // std::cout << "VolumeMeshingWidget::volume_meshing()" << std::endl;
  m_GoBtn->setText("Go");
  setWidgetListEnabled(true);
  this->m_progressBar->setValue(0);
  emit processEnded();
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMeshingWidget::threadProgressed(int value)
{
  m_progressBar->setValue(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMeshingWidget::threadHasMessage(QString message)
{
 // std::cout << "RepresentationUI::threadHasMessage()" << message.toStdString() << std::endl;
  this->statusBar()->showMessage(message);
}
