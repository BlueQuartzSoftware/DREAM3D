/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "SurfaceMeshWidget.h"

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


#include "QtSupport/QR3DFileCompleter.h"
#include "QtSupport/DREAM3DQtMacros.h"
#include "QtSupport/QCheckboxDialog.h"

#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/HDF5/H5VoxelReader.h"

#include "Meshing/SMVtkFileIO.h"

#include "SurfaceMeshPlugin.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshWidget::SurfaceMeshWidget(QWidget *parent) :
DREAM3DPluginFrame(parent),
m_SurfaceMesh(NULL),
m_WorkerThread(NULL),
#if defined(Q_WS_WIN)
m_OpenDialogLastDirectory("C:\\")
#else
m_OpenDialogLastDirectory("~/")
#endif
{
  setupUi(this);
  setupGui();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshWidget::~SurfaceMeshWidget()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshWidget::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList) {
    w->setEnabled(b);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshWidget::readSettings(QSettings &prefs)
{
  QString val;
  bool ok;
  qint32 i;
  //double d;

  prefs.beginGroup("SurfaceMesh");
  READ_FILEPATH_SETTING(prefs, m_, InputFile, "");
  verifyPathExists(m_InputFile->text(), m_InputFile);

  READ_FILEPATH_SETTING(prefs, m_, OutputDir, "");
  READ_STRING_SETTING(prefs, m_, OutputFilePrefix, "SurfaceMesh_")

  READ_CHECKBOX_SETTING(prefs, m_, WriteSTLFiles, true);
  READ_BOOL_SETTING(prefs, m_, WriteConformalMesh, true);
  READ_BOOL_SETTING(prefs, m_, WriteBinaryVTKFile, true);

  READ_CHECKBOX_SETTING(prefs, m_, SmoothMesh, false);
  READ_CHECKBOX_SETTING(prefs, m_, LockQuadPoints, false);
  READ_SETTING(prefs, m_, SmoothIterations, ok, i, 1 , Int);
  READ_SETTING(prefs, m_, WriteOutputFileIncrement, ok, i, 10 , Int);
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshWidget::writeSettings(QSettings &prefs)
{
  prefs.beginGroup("SurfaceMesh");
  WRITE_STRING_SETTING(prefs, m_, InputFile);
  WRITE_STRING_SETTING(prefs, m_, OutputDir);
  WRITE_STRING_SETTING(prefs, m_, OutputFilePrefix)

  WRITE_CHECKBOX_SETTING(prefs, m_, WriteSTLFiles)
  WRITE_BOOL_SETTING(prefs, m_, WriteConformalMesh, false);
  WRITE_BOOL_SETTING(prefs, m_, WriteBinaryVTKFile, true);


  WRITE_BOOL_SETTING(prefs, m_, SmoothMesh, m_SmoothMesh->isChecked() );
  WRITE_BOOL_SETTING(prefs, m_, LockQuadPoints, m_LockQuadPoints->isChecked() );
  WRITE_SETTING(prefs, m_, SmoothIterations );
  WRITE_SETTING(prefs, m_, WriteOutputFileIncrement );
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshWidget::setupGui()
{

  if (NULL == m_InputFile->completer()){
    QR3DFileCompleter* com = new QR3DFileCompleter(this, false);
    m_InputFile->setCompleter(com);
    QObject::connect( com, SIGNAL(activated(const QString &)),
             this, SLOT(on_m_InputFile_textChanged(const QString &)));
  }

  if (NULL == m_OutputDir->completer()) {
    QR3DFileCompleter* com4 = new QR3DFileCompleter(this, true);
    m_OutputDir->setCompleter(com4);
    QObject::connect( com4, SIGNAL(activated(const QString &)),
             this, SLOT(on_m_OutputDir_textChanged(const QString &)));
  }

  m_ScalarsName->setText(QString::fromStdString(DREAM3D::VTK::GrainIdScalarName));
  messageLabel->setText("Any existing output files will be over written with new versions during the operation.");
  m_WidgetList << m_InputFile << messageLabel;
  m_WidgetList << m_ScalarsName << xDim << yDim << zDim;
  m_WidgetList << m_InputFileBtn << m_OutputDir << m_OutputDirBtn << m_OutputFilePrefix;
  m_WidgetList << m_VisualizationVizFile << m_DeleteTempFiles;
//  m_WidgetList  << m_LockQuadPoints << m_SmoothIterations << m_SmoothMesh << m_WriteOutputFileIncrement;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshWidget::checkIOFiles()
{
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(DREAM3D::SurfaceMesh, m_, VisualizationVizFile)
    {
//  QString absPath = m_OutputDir->text() + QDir::separator() + "STL_Files/";
//  absPath = QDir::toNativeSeparators(absPath);
//  QFileInfo fi(absPath);
    QString fileName = QString("STL_Files/") + m_OutputFilePrefix->text() + "*.stl";
    m_WriteSTLFiles->setText(fileName);
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshWidget::on_m_InputFileBtn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select HDF5 Voxel File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("Voxel Data (*.h5voxel *.h5)") );
  if ( true == file.isEmpty() ){ return; }
  QFileInfo fi (file);
  m_InputFile->blockSignals(true);
  QString p = QDir::toNativeSeparators(fi.absoluteFilePath());
  m_InputFile->setText(p);
  on_m_InputFile_textChanged(m_InputFile->text() );
  m_InputFile->blockSignals(false);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshWidget::on_m_InputFile_textChanged(const QString & text)
{

  if (verifyPathExists(m_InputFile->text(), m_InputFile) )
  {
    QFileInfo fi(m_InputFile->text());
    QString outPath = fi.absolutePath() + QDir::separator() + fi.baseName() + "_SurfaceMesh";
    outPath = QDir::toNativeSeparators(outPath);
    m_OutputDir->setText(outPath);
  }

  QFileInfo fi(m_InputFile->text());
  if (fi.exists() && fi.isFile())
  {
    // Set the output file Prefix based on the name of the input file
    m_OutputFilePrefix->setText(fi.baseName() + QString("_") );

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
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshWidget::on_m_OutputDirBtn_clicked()
{
  QString outputFile = this->m_OpenDialogLastDirectory + QDir::separator();
  outputFile = QFileDialog::getExistingDirectory(this, tr("Select Surface Meshing Output Directory"), outputFile);
  if (!outputFile.isNull())
  {
    this->m_OutputDir->setText(outputFile);
    verifyPathExists(m_OutputDir->text(), m_OutputDir);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshWidget::on_m_OutputDir_textChanged(const QString & text)
{
  verifyPathExists(m_OutputDir->text(), m_OutputDir);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshWidget::on_m_GoBtn_clicked()
{
  if (m_GoBtn->text().compare("Cancel") == 0)
  {
    if(m_SurfaceMesh != NULL)
    {
      //std::cout << "canceling from GUI...." << std::endl;
      emit cancelPipeline();
    }
    return;
  }

  SANITY_CHECK_INPUT(m_, InputFile)

  if (false == sanityCheckOutputDirectory(m_OutputDir, QString("Reconstruction")) )
  {
    return;
  }

  if (m_WorkerThread != NULL)
  {
    m_WorkerThread->wait(); // Wait until the thread is complete
    delete m_WorkerThread; // Kill the thread
    m_WorkerThread = NULL;
  }
  m_WorkerThread = new QThread(); // Create a new Thread Resource


  m_SurfaceMesh = new QSurfaceMesh(NULL);
  // Move the Reconstruction object into the thread that we just created.
  m_SurfaceMesh->moveToThread(m_WorkerThread);

  m_SurfaceMesh->setInputFile(QDir::toNativeSeparators(m_InputFile->text()).toStdString() );
  m_SurfaceMesh->setOutputDirectory(QDir::toNativeSeparators(m_OutputDir->text()).toStdString());
  m_SurfaceMesh->setOutputFilePrefix(m_OutputFilePrefix->text().toStdString());
  m_SurfaceMesh->setDeleteTempFiles(m_DeleteTempFiles->isChecked());
  m_SurfaceMesh->setWriteBinaryVTKFiles(m_WriteBinaryVTKFile);
  m_SurfaceMesh->setWriteConformalMesh(m_WriteConformalMesh);
  m_SurfaceMesh->setWriteSTLFile(m_WriteSTLFiles->isChecked());

  m_SurfaceMesh->setSmoothMesh(m_SmoothMesh->isChecked());
  m_SurfaceMesh->setSmoothIterations(m_SmoothIterations->value());
  m_SurfaceMesh->setSmoothFileOutputIncrement(m_WriteOutputFileIncrement->value());
  m_SurfaceMesh->setSmoothLockQuadPoints(m_LockQuadPoints->isChecked());
  m_SurfaceMesh->setScalarName(m_ScalarsName->text().toStdString());


  /* Connect the signal 'started()' from the QThread to the 'run' slot of the
   * Reconstruction object. Since the Reconstruction object has been moved to another
   * thread of execution and the actual QThread lives in *this* thread then the
   * type of connection will be a Queued connection.
   */
  // When the thread starts its event loop, start the Reconstruction going
  connect(m_WorkerThread, SIGNAL(started()),
          m_SurfaceMesh, SLOT(run()));

  // When the Reconstruction ends then tell the QThread to stop its event loop
  connect(m_SurfaceMesh, SIGNAL(finished() ),
          m_WorkerThread, SLOT(quit()) );

  // When the QThread finishes, tell this object that it has finished.
  connect(m_WorkerThread, SIGNAL(finished()),
          this, SLOT( pipelineComplete() ) );

  // Send Progress from the Reconstruction to this object for display
  connect(m_SurfaceMesh, SIGNAL (updateProgress(int)),
    this, SLOT(pipelineProgress(int) ) );

  // Send progress messages from Reconstruction to this object for display
  connect(m_SurfaceMesh, SIGNAL (progressMessage(QString)), this, SLOT(addProgressMessage(QString) ));

  // Send progress messages from Reconstruction to this object for display
  connect(m_SurfaceMesh, SIGNAL (warningMessage(QString)), this, SLOT(addWarningMessage(QString) ));

  // Send progress messages from Reconstruction to this object for display
  connect(m_SurfaceMesh, SIGNAL (errorMessage(QString)), this, SLOT(addErrorMessage(QString) ));

  // If the use clicks on the "Cancel" button send a message to the Reconstruction object
  // We need a Direct Connection so the
  connect(this, SIGNAL(cancelPipeline() ),
          m_SurfaceMesh, SLOT (on_CancelWorker() ) , Qt::DirectConnection);

  setWidgetListEnabled(false);
  emit pipelineStarted();
  m_WorkerThread->start();
  m_GoBtn->setText("Cancel");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshWidget::on_m_VtkOptionsBtn_clicked()
{
  QVector<QString> options;
  options.push_back("Write Conformal Mesh");
  options.push_back("Write Binary VTK File");
  QCheckboxDialog d(options, this);
  d.setWindowTitle(QString("VTK Output Options"));

  d.setValue("Write Conformal Mesh", m_WriteConformalMesh);
  d.setValue("Write Binary VTK File", m_WriteBinaryVTKFile);

  int ret = d.exec();
  if (ret == QDialog::Accepted)
  {
    m_WriteConformalMesh = d.getValue("Write Conformal Mesh");
    m_WriteBinaryVTKFile = d.getValue("Write Binary VTK File");
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshWidget::pipelineComplete()
{
 // std::cout << "SurfaceMeshWidget::surface_meshing()" << std::endl;
  m_GoBtn->setText("Go");
  setWidgetListEnabled(true);
  this->m_progressBar->setValue(0);
  emit pipelineEnded();
  checkIOFiles();
  m_SurfaceMesh->deleteLater();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshWidget::pipelineProgress(int value)
{
  m_progressBar->setValue(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshWidget::on_m_OutputFilePrefix_textChanged(const QString &text)
{
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshWidget::addErrorMessage(QString message)
{
  QString title = QString::fromStdString(DREAM3D::UIPlugins::SurfaceMeshDisplayName).append(" Error");
  displayDialogBox(title, message, QMessageBox::Critical);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshWidget::addWarningMessage(QString message)
{
  QString title = QString::fromStdString(DREAM3D::UIPlugins::SurfaceMeshDisplayName).append(" Warning");
  displayDialogBox(title, message, QMessageBox::Warning);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshWidget::addProgressMessage(QString message)
{
  if (NULL != this->statusBar()) {
    this->statusBar()->showMessage(message);
  }
}
