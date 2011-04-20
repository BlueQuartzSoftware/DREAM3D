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
  READ_FILEPATH_SETTING(prefs, vm_NodesFile, "");
  READ_FILEPATH_SETTING(prefs, vm_TrianglesFile, "");
  READ_FILEPATH_SETTING(prefs, vm_OutputDir, "");
  READ_SETTING(prefs, vm_NumGrains, ok, i, 1000 , Int);
  READ_SETTING(prefs, vm_XDim, ok, d, 100 , Double);
  READ_SETTING(prefs, vm_YDim, ok, d, 100 , Double);
  READ_SETTING(prefs, vm_ZDim, ok, d, 100 , Double);

  READ_SETTING(prefs, vm_XRes, ok, d, 0.25 , Double);
  READ_SETTING(prefs, vm_YRes, ok, d, 0.25 , Double);
  READ_SETTING(prefs, vm_ZRes, ok, d, 0.25 , Double);
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMeshingWidget::writeSettings(QSettings &prefs)
{
  prefs.beginGroup("VolumeMeshing");
  WRITE_STRING_SETTING(prefs, vm_NodesFile);
  WRITE_STRING_SETTING(prefs, vm_TrianglesFile);
  WRITE_STRING_SETTING(prefs, vm_OutputDir);
  WRITE_SETTING(prefs, vm_NumGrains);
  WRITE_SETTING(prefs, vm_XDim);
  WRITE_SETTING(prefs, vm_YDim);
  WRITE_SETTING(prefs, vm_ZDim);

  WRITE_SETTING(prefs, vm_XRes);
  WRITE_SETTING(prefs, vm_YRes);
  WRITE_SETTING(prefs, vm_ZRes);
  prefs.endGroup();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMeshingWidget::setupGui()
{
  if (NULL == vm_NodesFile->completer()) {
    QR3DFileCompleter* com4 = new QR3DFileCompleter(this, true);
    vm_NodesFile->setCompleter(com4);
    QObject::connect( com4, SIGNAL(activated(const QString &)),
      this, SLOT(on_vm_NodesFile_textChanged(const QString &)));
  }

  if (NULL == vm_TrianglesFile->completer()) {
    QR3DFileCompleter* com4 = new QR3DFileCompleter(this, true);
    vm_TrianglesFile->setCompleter(com4);
    QObject::connect( com4, SIGNAL(activated(const QString &)),
      this, SLOT(on_vm_TrianglesFile_textChanged(const QString &)));
  }

  if (NULL == vm_OutputDir->completer()) {
    QR3DFileCompleter* com4 = new QR3DFileCompleter(this, true);
    vm_OutputDir->setCompleter(com4);
    QObject::connect( com4, SIGNAL(activated(const QString &)),
      this, SLOT(on_vm_OutputDir_textChanged(const QString &)));
  }


  m_WidgetList << vm_NodesFile << vm_NodesFileBtn << vm_TrianglesFile << vm_TrianglesFileBtn;
  m_WidgetList << vm_XDim << vm_XRes << vm_YDim << vm_YRes << vm_ZDim << vm_ZRes;
  m_WidgetList << vm_NumGrains << vm_OutputDir << vm_OutputDirBtn;

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

  verifyPathExists(vm_NodesFile->text(), vm_NodesFile);
  verifyPathExists(vm_TrianglesFile->text(), vm_TrianglesFile);
  verifyPathExists(vm_OutputDir->text(), vm_OutputDir );

  CHECK_QLINEEDIT_FILE_EXISTS(vm_NodesFile);
  CHECK_QLINEEDIT_FILE_EXISTS(vm_TrianglesFile);

  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::VolumeMeshing, vm_, MeshFile);
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::VolumeMeshing, vm_, MeshFile2);
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::VolumeMeshing, vm_, ElementQualityFile);
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::VolumeMeshing, vm_, VoxelsFile);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMeshingWidget::on_vm_NodesFileBtn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Nodes File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("Txt Files (*.txt)") );
  if ( true == file.isEmpty() ){ return;  }
  QFileInfo fi (file);
  vm_NodesFile->setText(fi.absoluteFilePath());
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMeshingWidget::on_vm_TrianglesFileBtn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Triangles File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("Txt Files (*.txt)") );
  if ( true == file.isEmpty() ){ return;  }
  QFileInfo fi (file);
  vm_TrianglesFile->setText(fi.absoluteFilePath());
  checkIOFiles();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMeshingWidget::on_vm_OutputDirBtn_clicked()
{
  QString outputFile = this->m_OpenDialogLastDirectory + QDir::separator();
  outputFile = QFileDialog::getExistingDirectory(this, tr("Select Volume Meshing Output Directory"), outputFile);
  if (!outputFile.isNull())
  {
    this->vm_OutputDir->setText(outputFile);
    if (verifyPathExists(outputFile, vm_OutputDir) == true )
    {
      checkIOFiles();
      vm_OutputDir->setText(outputFile);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMeshingWidget::on_vm_GoBtn_clicked()
{

  if (vm_GoBtn->text().compare("Cancel") == 0)
  {
    if(m_VolumeMesh.get() != NULL)
    {
      //std::cout << "canceling from GUI...." << std::endl;
      emit cancelProcess();
    }
    return;
  }


  SANITY_CHECK_INPUT(vm_, NodesFile);
  SANITY_CHECK_INPUT(vm_, TrianglesFile);
  SANITY_CHECK_INPUT(vm_, OutputDir);

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
  m_VolumeMesh->setNodesFile(vm_NodesFile->text().toStdString() );
  m_VolumeMesh->setTrianglesFile(vm_TrianglesFile->text().toStdString() );
  m_VolumeMesh->setOutputDirectory(vm_OutputDir->text().toStdString());
  m_VolumeMesh->setXDim(vm_XDim->value());
  m_VolumeMesh->setYDim(vm_YDim->value());
  m_VolumeMesh->setZDim(vm_ZDim->value());

  m_VolumeMesh->setXRes(vm_XRes->value());
  m_VolumeMesh->setYRes(vm_YRes->value());
  m_VolumeMesh->setZRes(vm_ZRes->value());
  m_VolumeMesh->setNumGrains(vm_NumGrains->value());

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
  vm_GoBtn->setText("Cancel");

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMeshingWidget::on_vm_NodesFile_textChanged(const QString & text)
{
  checkIOFiles();
  verifyPathExists(vm_NodesFile->text(), vm_NodesFile);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMeshingWidget::on_vm_TrianglesFile_textChanged(const QString & text)
{
  checkIOFiles();
  verifyPathExists(vm_TrianglesFile->text(), vm_TrianglesFile);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMeshingWidget::on_vm_OutputDir_textChanged(const QString & text)
{
  checkIOFiles();
  verifyPathExists(vm_OutputDir->text(), vm_OutputDir);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMeshingWidget::threadFinished()
{
 // std::cout << "VolumeMeshingWidget::volume_meshing()" << std::endl;
  vm_GoBtn->setText("Go");
  setWidgetListEnabled(true);
  this->vm_progressBar->setValue(0);
  emit processEnded();
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMeshingWidget::threadProgressed(int value)
{
  vm_progressBar->setValue(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMeshingWidget::threadHasMessage(QString message)
{
 // std::cout << "RepresentationUI::threadHasMessage()" << message.toStdString() << std::endl;
  this->statusBar()->showMessage(message);
}
