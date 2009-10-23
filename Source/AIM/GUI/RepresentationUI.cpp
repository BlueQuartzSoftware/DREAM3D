/*
 * RepresentationUI.cpp
 *
 *  Created on: Oct 19, 2009
 *      Author: mjackson
 */

#include "RepresentationUI.h"

#include <AIM/Common/Qt/AIMAboutBox.h>
#include <AIM/Common/Qt/QRecentFileList.h>
#include <AIM/Common/Constants.h>
#include <AIM/Threads/AIMThread.h>
#include <AIM/Reconstruction/Reconstruction.h>

//-- Qt Includes
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include <QtCore/QThread>
#include <QtCore/QFileInfoList>
#include <QtGui/QFileDialog>
#include <QtGui/QCloseEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QListWidget>



#define READ_STRING_SETTING(prefs, var, emptyValue)\
  var->setText( prefs.value(#var).toString() );\
  if (var->text().isEmpty() == true) { var->setText(emptyValue); }

#define READ_FILEPATH_SETTING(prefs, var, emptyValue)\
  var->setText( prefs.value(#var).toString() );\
  if (var->text().isEmpty() == true) { var->setText(emptyValue); }

#define READ_SETTING(prefs, var, ok, temp, default, type)\
  ok = false;\
  temp = prefs.value(#var).to##type(&ok);\
  if (false == ok) {temp = default;}\
  var->setValue(temp);

#define READ_BOOL_SETTING(prefs, var, emptyValue)\
  { QString s = prefs.value(#var).toString();\
  if (s.isEmpty() == false) {\
    bool bb = prefs.value(#var).toBool();\
  var->setChecked(bb); } else { var->setChecked(emptyValue); } }

#define WRITE_BOOL_SETTING(prefs, var, b)\
    prefs.setValue(#var, (b) );

#define WRITE_STRING_SETTING(prefs, var)\
  prefs.setValue(#var , this->var->text());

#define WRITE_SETTING(prefs, var)\
  prefs.setValue(#var, this->var->value());



#define SET_TEXT_ICON(name, icon)\
  { \
  QString iconFile = QString(":/") + QString(#icon) + QString("-16x16.png");\
  name->setText(AIM::Reconstruction::name.c_str());\
  name##Icon->setPixmap(QPixmap(iconFile));\
  }


#define CHECK_OUTPUT_FILE_EXISTS(name) \
  { \
  QString absPath = outputDir->text() + QDir::separator() + AIM::Reconstruction::name.c_str();\
  absPath = QDir::toNativeSeparators(absPath);\
  QFileInfo fi ( absPath );\
  if ( fi.exists() )  {\
  SET_TEXT_ICON(name, Check)\
  } else {\
  SET_TEXT_ICON(name, Delete) }  } \


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RepresentationUI::RepresentationUI(QWidget *parent) :
  QMainWindow(parent),
#if defined(Q_WS_WIN)
m_OpenDialogLastDirectory("C:\\")
#else
m_OpenDialogLastDirectory("~/")
#endif
{
  setupUi(this);
  readSettings();
  setupGui();

   QRecentFileList* recentFileList = QRecentFileList::instance();
   connect(recentFileList, SIGNAL (fileListChanged(const QString &)),
           this, SLOT(updateRecentFileList(const QString &)) );
   // Get out initial Recent File List
   this->updateRecentFileList(QString::null);


   this->setAcceptDrops(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RepresentationUI::~RepresentationUI()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::resizeEvent ( QResizeEvent * event )
{
 // std::cout << "RepresentationUI::resizeEvent" << std::endl;
 // std::cout << "   oldSize: " << event->oldSize().width() << " x " << event->oldSize().height() << std::endl;
 // std::cout << "   newSize: " << event->size().width() << " x " << event->size().height() << std::endl;
  emit parentResized();
 // std::cout << "RepresentationUI::resizeEvent --- Done" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_actionExit_triggered()
{
  this->close();
}

// -----------------------------------------------------------------------------
//  Called when the main window is closed.
// -----------------------------------------------------------------------------
void RepresentationUI::closeEvent(QCloseEvent *event)
{
  qint32 err = _checkDirtyDocument();
  if (err < 0)
  {
    event->ignore();
  }
  else
  {
    writeSettings();
    event->accept();
  }
}

// -----------------------------------------------------------------------------
//  Read the prefs from the local storage file
// -----------------------------------------------------------------------------
void RepresentationUI::readSettings()
{
  // std::cout << "Read Settings" << std::endl;
  QSettings prefs;
  QString val;
  bool ok;
  qint32 i;
  double d;

  /* ******** This Section is for the Reconstruction Tab ************ */
  READ_FILEPATH_SETTING(prefs, angDir, "");
  READ_FILEPATH_SETTING(prefs, outputDir, "");
  READ_STRING_SETTING(prefs, angFilePrefix, "");
  READ_STRING_SETTING(prefs, angMaxSlice, "");
  READ_STRING_SETTING(prefs, zStartIndex, "");
  READ_STRING_SETTING(prefs, zEndIndex, "");
  READ_STRING_SETTING(prefs, zSpacing, "");

  READ_BOOL_SETTING(prefs, mergeTwins, true);
  alreadyFormed->blockSignals(true);
  READ_BOOL_SETTING(prefs, alreadyFormed, false);
  alreadyFormed->blockSignals(false);
 
  READ_SETTING(prefs, minAllowedGrainSize, ok, i, 8 , Int);
  READ_SETTING(prefs, minConfidence, ok, d, 0.0 , Double);
  READ_SETTING(prefs, misOrientationTolerance, ok, d, 5.0 , Double);

  ok = false;
  i = prefs.value("crystalStructure").toInt(&ok);
  if (false == ok) {i=0;}
  crystalStructure->setCurrentIndex(i);



}

// -----------------------------------------------------------------------------
//  Write our Prefs to file
// -----------------------------------------------------------------------------
void RepresentationUI::writeSettings()
{
  // std::cout << "writeSettings" << std::endl;
  QSettings prefs;
  WRITE_STRING_SETTING(prefs, angDir);
  WRITE_STRING_SETTING(prefs, outputDir);
  WRITE_STRING_SETTING(prefs, angFilePrefix);
  WRITE_STRING_SETTING(prefs, angMaxSlice);
  WRITE_STRING_SETTING(prefs, zStartIndex);
  WRITE_STRING_SETTING(prefs, zEndIndex);
  WRITE_STRING_SETTING(prefs, zSpacing);

  WRITE_BOOL_SETTING(prefs, mergeTwins, mergeTwins->isChecked());
  WRITE_BOOL_SETTING(prefs, alreadyFormed, alreadyFormed->isChecked());

  WRITE_SETTING(prefs, minAllowedGrainSize);
  WRITE_SETTING(prefs, minConfidence);
  WRITE_SETTING(prefs, misOrientationTolerance);
  prefs.setValue("crystalStructure", this->crystalStructure->currentIndex());

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::setupGui()
{
#if 0
  SET_TEXT_ICON(StatsFile, Delete)
  SET_TEXT_ICON(VolBinFile, Delete)
  SET_TEXT_ICON(BOverABinsFile, Delete)
  SET_TEXT_ICON(COverABinsFile, Delete)
  SET_TEXT_ICON(COverBBinsFile, Delete)
  SET_TEXT_ICON(SVNFile, Delete)
  SET_TEXT_ICON(SVSFile, Delete)
  SET_TEXT_ICON(MisorientationBinsFile, Delete)
  SET_TEXT_ICON(MicroBinsFile, Delete)
  SET_TEXT_ICON(ReconstructedDataFile, Delete)
  SET_TEXT_ICON(ReconstructedVisualizationFile, Delete)
  SET_TEXT_ICON(GrainsFile, Delete)
  SET_TEXT_ICON(BoundaryCentersFile, Delete)
  SET_TEXT_ICON(AxisOrientationsFile, Delete)
  SET_TEXT_ICON(EulerAnglesFile, Delete)
#endif
  QString msg ("All files will be over written that appear in the output directory.");

  if (true == this->_verifyPathExists(angDir->text(), this->angDir) )
  {
    findAngMaxSliceAndPrefix();
  }
  if (this->_verifyPathExists(outputDir->text(), this->outputDir) )
  {
    findReconstructionOutputFiles();
  }

  QFileInfo fi (outputDir->text() + QDir::separator() +  AIM::Reconstruction::ReconstructedDataFile.c_str() );
  if (alreadyFormed->isChecked() == true && fi.exists() == false)
  {
    alreadyFormed->setChecked(false);
  } 

  if (alreadyFormed->isChecked())
  {
    msg += QString("\nThe 'reconstructed_data.txt' file will be used as an import and NOT over written with new data");
  }
  messageLabel->setText(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList) {
    w->setEnabled(b);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::dragEnterEvent(QDragEnterEvent* e)
{
  const QMimeData* dat = e->mimeData();
  QList<QUrl> urls = dat->urls();
  QString file = urls.count() ? urls[0].toLocalFile() : QString();
  QDir parent(file);
  this->m_OpenDialogLastDirectory = parent.dirName();
  QFileInfo fi(file );
  QString ext = fi.suffix();
  if (fi.exists() && fi.isFile() && ( ext.compare("mxa") || ext.compare("h5") || ext.compare("hdf5") ) )
  {
    e->accept();
  }
  else
  {
    e->ignore();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::dropEvent(QDropEvent* e)
{
  const QMimeData* dat = e->mimeData();
  QList<QUrl> urls = dat->urls();
  QString file = urls.count() ? urls[0].toLocalFile() : QString();
  QDir parent(file);
  this->m_OpenDialogLastDirectory = parent.dirName();
  QFileInfo fi(file );
  QString ext = fi.suffix();
  file = QDir::toNativeSeparators(file);
  if (fi.exists() && fi.isFile() && ( ext.compare("tif") || ext.compare("tiff")  ) )
  {
    //TODO: INSERT Drop Event CODE HERE
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool RepresentationUI::_verifyOutputPathParentExists(QString outFilePath, QLineEdit* lineEdit)
{
  QFileInfo fileinfo(outFilePath);
  QDir parent (fileinfo.dir() );
  if (false == parent.exists() )
  {
    lineEdit->setStyleSheet("border: 2px solid red;");
  }
  else
  {
    lineEdit->setStyleSheet("");
  }
  return parent.exists();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool RepresentationUI::_verifyPathExists(QString outFilePath, QLineEdit* lineEdit)
{
  QFileInfo fileinfo(outFilePath);
  if (false == fileinfo.exists() )
  {
    lineEdit->setStyleSheet("border: 2px solid red;");
  }
  else
  {
    lineEdit->setStyleSheet("");
  }
  return fileinfo.exists();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
qint32 RepresentationUI::_checkDirtyDocument()
{
  qint32 err = -1;

  if (this->isWindowModified() == true)
  {
    int r = QMessageBox::warning(this, tr("AIM Mount Maker"),
                            tr("The Image has been modified.\nDo you want to save your changes?"),
                            QMessageBox::Save | QMessageBox::Default,
                            QMessageBox::Discard,
                            QMessageBox::Cancel | QMessageBox::Escape);
    if (r == QMessageBox::Save)
    {
      //TODO: Save the current document or otherwise save the state.
    }
    else if (r == QMessageBox::Discard)
    {
      err = 1;
    }
    else if (r == QMessageBox::Cancel)
    {
      err = -1;
    }
  }
  else
  {
    err = 1;
  }

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_actionClose_triggered() {
 // std::cout << "RepresentationUI::on_actionClose_triggered" << std::endl;
  qint32 err = -1;
  err = _checkDirtyDocument();
  if (err >= 0)
  {
    // Close the window. Files have been saved if needed
    if (QApplication::activeWindow() == this)
    {
      this->close();
    }
    else
    {
      QApplication::activeWindow()->close();
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::updateRecentFileList(const QString &file)
{
 // std::cout << "RepresentationUI::updateRecentFileList" << std::endl;

  // Clear the Recent Items Menu
  this->menu_RecentFiles->clear();

  // Get the list from the static object
  QStringList files = QRecentFileList::instance()->fileList();
  foreach (QString file, files)
    {
      QAction* action = new QAction(this->menu_RecentFiles);
      action->setText(QRecentFileList::instance()->parentAndFileName(file));
      action->setData(file);
      action->setVisible(true);
      this->menu_RecentFiles->addAction(action);
      connect(action, SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::openRecentFile()
{
  //std::cout << "QRecentFileList::openRecentFile()" << std::endl;

  QAction *action = qobject_cast<QAction *>(sender());
  if (action)
  {
    //std::cout << "Opening Recent file: " << action->data().toString().toStdString() << std::endl;
    QString file = action->data().toString();
    //TODO: use the 'file' object to figure out what to open
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::threadHasMessage(QString message)
{
  this->statusBar()->showMessage(message);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::threadFinished()
{
  std::cout << "thread Finished." << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::threadProgressed(float percent)
{
  std::cout << "Thread Progressed" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_angDirBtn_clicked()
{
  std::cout << "on_angDirBtn_clicked" << std::endl;
  QString outputFile = this->m_OpenDialogLastDirectory + QDir::separator();
  outputFile = QFileDialog::getExistingDirectory(this, tr("Select Ang Directory"), outputFile);
  if (!outputFile.isNull())
  {
    this->angDir->setText(outputFile);
    findAngMaxSliceAndPrefix();
    _verifyPathExists(outputFile, angDir);
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::findReconstructionOutputFiles()
{
  CHECK_OUTPUT_FILE_EXISTS(StatsFile)
  CHECK_OUTPUT_FILE_EXISTS(VolBinFile)
  CHECK_OUTPUT_FILE_EXISTS(BOverABinsFile)
  CHECK_OUTPUT_FILE_EXISTS(COverABinsFile)
  CHECK_OUTPUT_FILE_EXISTS(COverBBinsFile)
  CHECK_OUTPUT_FILE_EXISTS(SVNFile)
  CHECK_OUTPUT_FILE_EXISTS(SVSFile)
  CHECK_OUTPUT_FILE_EXISTS(MisorientationBinsFile)
  CHECK_OUTPUT_FILE_EXISTS(MicroBinsFile)
  CHECK_OUTPUT_FILE_EXISTS(ReconstructedDataFile)
  CHECK_OUTPUT_FILE_EXISTS(ReconstructedVisualizationFile)
  CHECK_OUTPUT_FILE_EXISTS(GrainsFile)
  CHECK_OUTPUT_FILE_EXISTS(BoundaryCentersFile)
  CHECK_OUTPUT_FILE_EXISTS(AxisOrientationsFile)
  CHECK_OUTPUT_FILE_EXISTS(EulerAnglesFile)

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_alreadyFormed_stateChanged(int currentState)
{
  QString absPath = outputDir->text() + QDir::separator() + AIM::Reconstruction::ReconstructedDataFile.c_str();
  absPath = QDir::toNativeSeparators(absPath);
  QFileInfo fi (absPath);
  QString msg ("All files will be over written that appear in the output directory.");
  if (alreadyFormed->isChecked() == true && fi.exists() == false)
  {
    int ret = QMessageBox::critical(this, tr("AIM Representation"),
      tr("You have selected the 'Already Formed' check box \nbut the correct output file does not exist.\n"
      "The checkbox will revert to an unchecked state.?"),
      QMessageBox::Ok,
      QMessageBox::Ok);
      alreadyFormed->setChecked(false);
      CHECK_OUTPUT_FILE_EXISTS(ReconstructedDataFile)
  }

  if (alreadyFormed->isChecked())
  {
    msg += QString("\nThe 'reconstructed_data.txt' file will be used as an import and NOT over written with new data");
  }
  messageLabel->setText(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::findAngMaxSliceAndPrefix()
{
  if (angDir->text().length() == 0) { return; }
  QDir dir(angDir->text());
  QStringList filters;
  filters << "*.ang";
  dir.setNameFilters(filters);
  QFileInfoList angList = dir.entryInfoList();
  int maxSlice = 0;
  int currValue =0;
  bool ok;
  QString fPrefix;
  QRegExp rx("(\\d+)");
  QStringList list;
  int pos = 0;
  foreach(QFileInfo fi, angList)
  {
    if (fi.suffix().compare(".ang") && fi.isFile() == true)
    {
      pos = 0;
      list.clear();
      QString fn = fi.baseName();
      while ((pos = rx.indexIn(fn, pos)) != -1)
      {
        list << rx.cap(0);
        fPrefix = fn.left(pos);
        pos += rx.matchedLength();
      }
      if (list.size() > 0) {
        currValue = list.front().toInt(&ok);
        if (currValue > maxSlice) { maxSlice = currValue; }
      }
    }
  }
  this->angMaxSlice->setText(QString::number(maxSlice, 10));
  this->angFilePrefix->setText(fPrefix);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_outputDirBtn_clicked()
{
  QString outputFile = this->m_OpenDialogLastDirectory + QDir::separator();
  outputFile = QFileDialog::getExistingDirectory(this, tr("Select Output Directory"), outputFile);
  if (!outputFile.isNull())
  {
    this->outputDir->setText(outputFile);
    if (_verifyPathExists(outputFile, outputDir) == true )
    {
      findReconstructionOutputFiles();
      QFileInfo fi (outputDir->text() + QDir::separator() +  AIM::Reconstruction::ReconstructedDataFile.c_str() );
      if (alreadyFormed->isChecked() == true && fi.exists() == false)
      {
        alreadyFormed->setChecked(false);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_reconstructBtn_clicked()
{


  Reconstruction::Pointer worker = Reconstruction::New();
  //TODO: Set all the values into the worker
  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

  QString tName("Reconstruction Worker-");
  tName += QString::number(0);

  AIMThread::Pointer t = AIMThread::New(-1, tName, NULL );

  worker->moveToThread(t.get());
  // Have the worker start when the thread is started
  worker->connect(t.get(), SIGNAL(started()),
                           SLOT(compute()));

  // Have the worker quit the thread when it is done doing work
  t->connect(worker.get(), SIGNAL(workerComplete()),
                           SLOT(quit() ) , Qt::DirectConnection);

  this->connect(t.get(), SIGNAL(threadFinished()),
                         SLOT(threadFinished() ), Qt::DirectConnection );

  //Send messages from the worker object up to the GUI
  this->connect(worker.get(), SIGNAL(workerHasMessage(QString)),
                              SLOT(on_threadHasMessage(QString)) );

  // Send Cancel Messages from GUI to Worker
  worker->connect(this, SIGNAL(sig_CancelWorker()),
                        SLOT(on_CancelWorker() ) );

}

