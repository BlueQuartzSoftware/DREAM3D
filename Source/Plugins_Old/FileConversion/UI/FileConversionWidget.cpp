
#include "FileConversionWidget.h"


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

#include "FileConversion/FileConversion.h"
#include "FileConversionPlugin.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileConversionWidget::FileConversionWidget(QWidget *parent) :
DREAM3DPluginFrame(parent),
m_FileConversion(NULL),
m_WorkerThread(NULL)
{
#if defined(Q_WS_WIN)
m_OpenDialogLastDirectory = ("C:\\");
#else
m_OpenDialogLastDirectory = ("~/");
#endif
  setupUi(this);
  setupGui();
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileConversionWidget::~FileConversionWidget()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileConversionWidget::readSettings(QSettings &prefs)
{
  prefs.beginGroup("FileConversion");
  READ_FILEPATH_SETTING(prefs, m_, InputFilePath, "");
  on_m_InputFilePath_textChanged(m_InputFilePath->text());
  READ_FILEPATH_SETTING(prefs, m_, OutputFilePath, "");
  on_m_OutputFilePath_textChanged(m_OutputFilePath->text());
  prefs.endGroup();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileConversionWidget::writeSettings(QSettings &prefs)
{
  prefs.beginGroup("FileConversion");
  WRITE_STRING_SETTING(prefs, m_, InputFilePath)
  WRITE_STRING_SETTING(prefs, m_, OutputFilePath)
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileConversionWidget::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList) {
    w->setEnabled(b);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileConversionWidget::setupGui()
{


  QR3DFileCompleter* com = new QR3DFileCompleter(this, false);
  m_InputFilePath->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString &)),
           this, SLOT(on_m_InputFilePath_textChanged(const QString &)));

  QR3DFileCompleter* com2 = new QR3DFileCompleter(this, false);
  m_OutputFilePath->setCompleter(com2);
  QObject::connect( com2, SIGNAL(activated(const QString &)),
           this, SLOT(on_m_OutputFilePath_textChanged(const QString &)));


  // Add the other widgets from the GUI here so that when the pipeline is running
  // the user can not change the values of any of the inputs
  m_WidgetList << m_LoadSettingsBtn << m_SaveSettingsBtn;
  m_WidgetList << m_InputFilePath << m_OutputFilePath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileConversionWidget::on_m_LoadSettingsBtn_clicked()
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
void FileConversionWidget::on_m_SaveSettingsBtn_clicked()
{
  QString proposedFile = m_OpenDialogLastDirectory + QDir::separator() + "FileConversionSettings.txt";
  QString file = QFileDialog::getSaveFileName(this, tr("Save FileConversion Settings"),
                                              proposedFile,
                                              tr("*.txt") );
  if ( true == file.isEmpty() ){ return;  }

  QSettings prefs(file, QSettings::IniFormat, this);
  writeSettings(prefs);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileConversionWidget::checkIOFiles()
{
#if 0
  // Use this code as an example of using some macros to make the validation of
  // input/output files easier
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::SyntheticBuilder, m_, CrystallographicErrorFile)

  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::SyntheticBuilder, m_ , IPFVizFile)
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileConversionWidget::on_m_GoBtn_clicked()
{

  if (m_GoBtn->text().compare("Cancel") == 0)
  {
    if(m_FileConversion!= NULL)
    {
      //std::cout << "canceling from GUI...." << std::endl;
      emit cancelPipeline();
    }
    return;
  }


  QFileInfo fi(m_InputFilePath->text());
  if (fi.exists() == false)
  {
    QMessageBox::critical(this, tr("Input File Error"), tr("Input File does not exist. Please check the path."), QMessageBox::Ok);
    return;
  }

  if (m_OutputFilePath->text().isEmpty() == true)
  {
    QMessageBox::critical(this, tr("Output File Error"), tr("Please select a file name for the converted voxel file to be saved as."), QMessageBox::Ok);
    return;
  }
  QFile file(m_OutputFilePath->text());
  if (file.exists() == true)
  {
    int ret = QMessageBox::warning(this, tr("File Conversion"),
                                   tr("The Output File Already Exists\nDo you want to over write the existing file?"),
                                   QMessageBox::No | QMessageBox::Default,
                                   QMessageBox::Yes,
                                   QMessageBox::Cancel);
    if (ret == QMessageBox::Cancel)
    {
      return;
    }
    else if (ret == QMessageBox::No)
    {
      QString outputFile = getOpenDialogLastDirectory() + QDir::separator() + "VoxelFile";
      outputFile = QFileDialog::getSaveFileName(this, tr("Save Output File As ..."), outputFile, tr("Voxel Files (*.dx *.ph *.vtk *.h5voxel)"));
      if (outputFile.isNull())
      {
        return;
      }
      m_OutputFilePath->setText(outputFile);
    }
  }

  fi = QFileInfo(m_OutputFilePath->text());
  QDir outputDir(fi.absolutePath());
  if (outputDir.exists() == false)
  {
    bool ok = outputDir.mkpath(".");
    if (ok == false)
    {
      QMessageBox::critical(this, tr("Output Directory Creation"), tr("The output directory could not be created."), QMessageBox::Ok);
      return;
    }
  }



  if (m_WorkerThread != NULL)
  {
    m_WorkerThread->wait(); // Wait until the thread is complete
    delete m_WorkerThread; // Kill the thread
    m_WorkerThread = NULL;
  }
  m_WorkerThread = new QThread(); // Create a new Thread Resource

  m_FileConversion = new QFileConversion(NULL);

  // Move the FileConversion object into the thread that we just created.
  m_FileConversion->moveToThread(m_WorkerThread);

  // Pull the values from the GUI and push them into the m_FileConversion variable
  m_FileConversion->setInputFilePath(m_InputFilePath->text().toStdString() );
  m_FileConversion->setOutputFilePath(m_OutputFilePath->text().toStdString());

  /* Connect the signal 'started()' from the QThread to the 'run' slot of the
   * FileConversion object. Since the FileConversion object has been moved to another
   * thread of execution and the actual QThread lives in *this* thread then the
   * type of connection will be a Queued connection.
   */
  // When the thread starts its event loop, start the FileConversion going
  connect(m_WorkerThread, SIGNAL(started()),
          m_FileConversion, SLOT(run()));

  // When the FileConversion ends then tell the QThread to stop its event loop
  connect(m_FileConversion, SIGNAL(finished() ),
          m_WorkerThread, SLOT(quit()) );

  // When the QThread finishes, tell this object that it has finished.
  connect(m_WorkerThread, SIGNAL(finished()),
          this, SLOT( pipelineComplete() ) );

  // If the use clicks on the "Cancel" button send a message to the FileConversion object
  // We need a Direct Connection so the
  connect(this, SIGNAL(cancelPipeline() ),
          m_FileConversion, SLOT (on_CancelWorker() ) , Qt::DirectConnection);

  // Send Progress from the FileConversion to this object for display
  connect(m_FileConversion, SIGNAL (updateProgress(int)),
          this, SLOT(pipelineProgress(int) ) );

  // Send progress messages from FileConversion to this object for display
  connect(m_FileConversion, SIGNAL (progressMessage(QString)),
          this, SLOT(addProgressMessage(QString) ));

  // Send progress messages from FileConversion to this object for display
  connect(m_FileConversion, SIGNAL (warningMessage(QString)),
          this, SLOT(addWarningMessage(QString) ));

  // Send progress messages from FileConversion to this object for display
  connect(m_FileConversion, SIGNAL (errorMessage(QString)),
          this, SLOT(addErrorMessage(QString) ));



  setWidgetListEnabled(false);
  emit pipelineStarted();
  m_WorkerThread->start();
  m_GoBtn->setText("Cancel");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileConversionWidget::pipelineComplete()
{
 // std::cout << "FileConversionWidget::FileConversion_Finished()" << std::endl;
  m_GoBtn->setText("Go");
  setWidgetListEnabled(true);
  this->m_progressBar->setValue(0);
  emit pipelineEnded();
  checkIOFiles();
  m_FileConversion->deleteLater();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileConversionWidget::pipelineProgress(int val)
{
  this->m_progressBar->setValue( val );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileConversionWidget::addErrorMessage(QString message)
{
  QString title = QString::fromStdString(DREAM3D::UIPlugins::FileConversionDisplayName).append(" Error");
  displayDialogBox(title, message, QMessageBox::Critical);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileConversionWidget::addWarningMessage(QString message)
{
  QString title = QString::fromStdString(DREAM3D::UIPlugins::FileConversionDisplayName).append(" Warning");
  displayDialogBox(title, message, QMessageBox::Warning);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileConversionWidget::addProgressMessage(QString message)
{
  if (NULL != this->statusBar()) {
    this->statusBar()->showMessage(message);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileConversionWidget::on_m_InputFilePath_textChanged(const QString text)
{
  if (verifyPathExists(m_InputFilePath->text(), m_InputFilePath) )
    {
  checkFile(m_InputFilePath, m_InputFileType);
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileConversionWidget::on_m_OutputFilePath_textChanged(const QString text)
{
  checkFile(m_OutputFilePath, m_OutputFileType);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileConversionWidget::checkFile(QLineEdit* lineEdit, QLabel* label)
{

     QFileInfo fi(lineEdit->text());
 //    if (fi.exists() && fi.isFile())
     {
       QString extension = fi.suffix();
       if (extension.compare("vtk") == 0)
       {
         label->setText("VTK File");
       }
       else if (extension.compare("ph") == 0)
       {
         label->setText("Ph File");
       }
       else if (extension.compare("h5voxel") == 0)
       {
         label->setText("HDF5 Voxel File");
       }
       else if (extension.compare("dx") == 0)
       {
         label->setText("DX File");
       }
       else
       {
         label->setText("Unknown File Extension");
       }
     }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileConversionWidget::on_m_SelectInputBtn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Input File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("Voxel Files (*.vtk *.ph *.dx *.h5voxel)") );
  if ( true == file.isEmpty() ){ return; }
  QFileInfo fi (file);
  m_InputFilePath->blockSignals(true);
  QString p = QDir::toNativeSeparators(fi.absoluteFilePath());
  m_InputFilePath->setText(p);
  on_m_InputFilePath_textChanged(m_InputFilePath->text() );
  m_InputFilePath->blockSignals(false);}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileConversionWidget::on_m_SaveOutputBtn_clicked()
{
  QString file = QFileDialog::getSaveFileName(this, tr("Save Voxel File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("Voxel Files (*.vtk *.ph *.dx *.h5voxel)") );
  if ( true == file.isEmpty() ){ return;  }
  QFileInfo fi (file);
  QString ext = fi.suffix();
  m_OutputFilePath->setText(fi.absoluteFilePath());
  m_OpenDialogLastDirectory = fi.path();
}


