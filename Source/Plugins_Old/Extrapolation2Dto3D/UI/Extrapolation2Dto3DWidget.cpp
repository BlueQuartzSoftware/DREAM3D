
#include "Extrapolation2Dto3DWidget.h"


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

#include "Extrapolation2Dto3D/Extrapolation2Dto3D.h"
#include "Extrapolation2Dto3DPlugin.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Extrapolation2Dto3DWidget::Extrapolation2Dto3DWidget(QWidget *parent) :
DREAM3DPluginFrame(parent),
m_Extrapolation2Dto3D(NULL),
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
Extrapolation2Dto3DWidget::~Extrapolation2Dto3DWidget()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Extrapolation2Dto3DWidget::readSettings(QSettings &prefs)
{
  prefs.beginGroup("Extrapolation2Dto3D");

  prefs.endGroup();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Extrapolation2Dto3DWidget::writeSettings(QSettings &prefs)
{
  prefs.beginGroup("Extrapolation2Dto3D");

  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Extrapolation2Dto3DWidget::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList) {
    w->setEnabled(b);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Extrapolation2Dto3DWidget::setupGui()
{

#if 0
  // If you want to add an inline "file completer" for QLineEdits use this code
  // as an example
  QR3DFileCompleter* com = new QR3DFileCompleter(this, false);
  m_H5InputStatisticsFile->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString &)),
           this, SLOT(on_m_H5InputStatisticsFile_textChanged(const QString &)));
#endif

  // Add the other widgets from the GUI here so that when the pipeline is running
  // the user can not change the values of any of the inputs
  m_WidgetList << m_LoadSettingsBtn << m_SaveSettingsBtn;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Extrapolation2Dto3DWidget::on_m_LoadSettingsBtn_clicked()
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
void Extrapolation2Dto3DWidget::on_m_SaveSettingsBtn_clicked()
{
  QString proposedFile = m_OpenDialogLastDirectory + QDir::separator() + "Extrapolation2Dto3DSettings.txt";
  QString file = QFileDialog::getSaveFileName(this, tr("Save Extrapolation2Dto3D Settings"),
                                              proposedFile,
                                              tr("*.txt") );
  if ( true == file.isEmpty() ){ return;  }

  QSettings prefs(file, QSettings::IniFormat, this);
  writeSettings(prefs);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Extrapolation2Dto3DWidget::checkIOFiles()
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
void Extrapolation2Dto3DWidget::on_m_GoBtn_clicked()
{

  if (m_GoBtn->text().compare("Cancel") == 0)
  {
    if(m_Extrapolation2Dto3D!= NULL)
    {
      //std::cout << "canceling from GUI...." << std::endl;
      emit cancelPipeline();
    }
    return;
  }

#if 0
  if (false == sanityCheckOutputDirectory(m_OutputDir, QString("Extrapolation2Dto3D")) )
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

  m_Extrapolation2Dto3D = new QExtrapolation2Dto3D(NULL);

  // Move the Extrapolation2Dto3D object into the thread that we just created.
  m_Extrapolation2Dto3D->moveToThread(m_WorkerThread);

  // Pull the values from the GUI and push them into the m_Extrapolation2Dto3D variable
 

  /* Connect the signal 'started()' from the QThread to the 'run' slot of the
   * Reconstruction object. Since the Reconstruction object has been moved to another
   * thread of execution and the actual QThread lives in *this* thread then the
   * type of connection will be a Queued connection.
   */
  // When the thread starts its event loop, start the Reconstruction going
  connect(m_WorkerThread, SIGNAL(started()),
          m_Extrapolation2Dto3D, SLOT(run()));

  // When the Reconstruction ends then tell the QThread to stop its event loop
  connect(m_Extrapolation2Dto3D, SIGNAL(finished() ),
          m_WorkerThread, SLOT(quit()) );

  // When the QThread finishes, tell this object that it has finished.
  connect(m_WorkerThread, SIGNAL(finished()),
          this, SLOT( pipelineComplete() ) );

  // Send Progress from the Reconstruction to this object for display
  connect(m_Extrapolation2Dto3D, SIGNAL (updateProgress(int)),
    this, SLOT(pipelineProgress(int) ) );

  // If the use clicks on the "Cancel" button send a message to the Reconstruction object
  // We need a Direct Connection so the
  connect(this, SIGNAL(cancelProcess() ),
          m_Extrapolation2Dto3D, SLOT (on_CancelWorker() ) , Qt::DirectConnection);
  
  // Send progress messages from Pipeline to this object for display
  connect(m_Extrapolation2Dto3D, SIGNAL (progressMessage(QString)), 
          this, SLOT(addProgressMessage(QString) ));

  // Send progress messages from Pipeline to this object for display
  connect(m_Extrapolation2Dto3D, SIGNAL (warningMessage(QString)), 
          this, SLOT(addWarningMessage(QString) ));

  // Send progress messages from Pipeline to this object for display
  connect(m_Extrapolation2Dto3D, SIGNAL (errorMessage(QString)), 
          this, SLOT(addErrorMessage(QString) ));



  setWidgetListEnabled(false);
  emit pipelineStarted();
  m_WorkerThread->start();
  m_GoBtn->setText("Cancel");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Extrapolation2Dto3DWidget::pipelineComplete()
{
 // std::cout << "Extrapolation2Dto3DWidget::Extrapolation2Dto3D_Finished()" << std::endl;
  m_GoBtn->setText("Go");
  setWidgetListEnabled(true);
  this->m_progressBar->setValue(0);
  emit pipelineEnded();
  checkIOFiles();
  m_Extrapolation2Dto3D->deleteLater();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Extrapolation2Dto3DWidget::pipelineProgress(int val)
{
  this->m_progressBar->setValue( val );
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Extrapolation2Dto3DWidget::addErrorMessage(QString message)
{
  QString title = QString::fromStdString(DREAM3D::UIPlugins::Extrapolation2Dto3DDisplayName).append(" Error");
  displayDialogBox(title, message, QMessageBox::Critical);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Extrapolation2Dto3DWidget::addWarningMessage(QString message)
{
  QString title = QString::fromStdString(DREAM3D::UIPlugins::Extrapolation2Dto3DDisplayName).append(" Warning");
  displayDialogBox(title, message, QMessageBox::Warning);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Extrapolation2Dto3DWidget::addProgressMessage(QString message)
{
  if (NULL != this->statusBar()) {
    this->statusBar()->showMessage(message);
  }
}
