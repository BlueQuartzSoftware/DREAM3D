
#include "PipelineBuilderWidget.h"


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
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DREAM3DFilters.h"

//#include "FilterWidgets/QFilterWidgetManager.h"
//#include "FilterWidgetHeaders.h"
//#include "FilterWidgets/RegisterKnownFilterWidgets.h"


#include "PipelineBuilder/PipelineBuilder.h"
#include "PipelineBuilderPlugin.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineBuilderWidget::PipelineBuilderWidget(QWidget *parent) :
DREAM3DPluginFrame(parent),
m_PipelineBuilder(NULL),
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
PipelineBuilderWidget::~PipelineBuilderWidget()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::readSettings(QSettings &prefs)
{
  prefs.beginGroup("PipelineBuilder");

  prefs.endGroup();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::writeSettings(QSettings &prefs)
{
  prefs.beginGroup("PipelineBuilder");

  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList) {
    w->setEnabled(b);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::setupGui()
{

  QVBoxLayout* verticalLayout = new QVBoxLayout(scrollAreaWidgetContents);
  verticalLayout->setContentsMargins(0, 0, 0, 0);
  verticalLayout->setSpacing(20);
  verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::on_m_LoadSettingsBtn_clicked()
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
void PipelineBuilderWidget::on_m_SaveSettingsBtn_clicked()
{
  QString proposedFile = m_OpenDialogLastDirectory + QDir::separator() + "PipelineBuilderSettings.txt";
  QString file = QFileDialog::getSaveFileName(this, tr("Save PipelineBuilder Settings"),
                                              proposedFile,
                                              tr("*.txt") );
  if ( true == file.isEmpty() ){ return;  }

  QSettings prefs(file, QSettings::IniFormat, this);
  writeSettings(prefs);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::checkIOFiles()
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
void PipelineBuilderWidget::on_m_GoBtn_clicked()
{

  if (m_GoBtn->text().compare("Cancel") == 0)
  {
    if(m_PipelineBuilder!= NULL)
    {
      //std::cout << "canceling from GUI...." << std::endl;
      emit cancelPipeline();
    }
    return;
  }

#if 0
  if (false == sanityCheckOutputDirectory(m_OutputDir, QString("PipelineBuilder")) )
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

  m_PipelineBuilder = new QPipelineBuilder(NULL);

  // Move the PipelineBuilder object into the thread that we just created.
  m_PipelineBuilder->moveToThread(m_WorkerThread);

  // Pull the values from the GUI and push them into the m_PipelineBuilder variable


  /* Connect the signal 'started()' from the QThread to the 'run' slot of the
   * PipelineBuilder object. Since the PipelineBuilder object has been moved to another
   * thread of execution and the actual QThread lives in *this* thread then the
   * type of connection will be a Queued connection.
   */
  // When the thread starts its event loop, start the PipelineBuilder going
  connect(m_WorkerThread, SIGNAL(started()),
          m_PipelineBuilder, SLOT(run()));

  // When the PipelineBuilder ends then tell the QThread to stop its event loop
  connect(m_PipelineBuilder, SIGNAL(finished() ),
          m_WorkerThread, SLOT(quit()) );

  // When the QThread finishes, tell this object that it has finished.
  connect(m_WorkerThread, SIGNAL(finished()),
          this, SLOT( pipelineComplete() ) );

  // If the use clicks on the "Cancel" button send a message to the PipelineBuilder object
  // We need a Direct Connection so the
  connect(this, SIGNAL(cancelPipeline() ),
          m_PipelineBuilder, SLOT (on_CancelWorker() ) , Qt::DirectConnection);

  // Send Progress from the PipelineBuilder to this object for display
  connect(m_PipelineBuilder, SIGNAL (updateProgress(int)),
          this, SLOT(pipelineProgress(int) ) );

  // Send progress messages from PipelineBuilder to this object for display
  connect(m_PipelineBuilder, SIGNAL (progressMessage(QString)),
          this, SLOT(addProgressMessage(QString) ));

  // Send progress messages from PipelineBuilder to this object for display
  connect(m_PipelineBuilder, SIGNAL (warningMessage(QString)),
          this, SLOT(addWarningMessage(QString) ));

  // Send progress messages from PipelineBuilder to this object for display
  connect(m_PipelineBuilder, SIGNAL (errorMessage(QString)),
          this, SLOT(addErrorMessage(QString) ));



  setWidgetListEnabled(false);
  emit pipelineStarted();
  m_WorkerThread->start();
  m_GoBtn->setText("Cancel");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::pipelineComplete()
{
 // std::cout << "PipelineBuilderWidget::PipelineBuilder_Finished()" << std::endl;
  m_GoBtn->setText("Go");
  setWidgetListEnabled(true);
  this->m_progressBar->setValue(0);
  emit pipelineEnded();
  checkIOFiles();
  m_PipelineBuilder->deleteLater();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::pipelineProgress(int val)
{
  this->m_progressBar->setValue( val );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::addErrorMessage(QString message)
{
  QString title = QString::fromStdString(DREAM3D::UIPlugins::PipelineBuilderDisplayName).append(" Error");
  displayDialogBox(title, message, QMessageBox::Critical);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::addWarningMessage(QString message)
{
  QString title = QString::fromStdString(DREAM3D::UIPlugins::PipelineBuilderDisplayName).append(" Warning");
  displayDialogBox(title, message, QMessageBox::Warning);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::addProgressMessage(QString message)
{
  if (NULL != this->statusBar()) {
    this->statusBar()->showMessage(message);
  }
}
