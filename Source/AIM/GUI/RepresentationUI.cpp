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
  name->setText(AIM::Representation::name.c_str());\
  name##Icon->setPixmap(QPixmap(iconFile));\
  }


#define CHECK_OUTPUT_FILE_EXISTS(name) \
  { \
  QString absPath = outputDir->text() + QDir::separator() + AIM::Representation::name.c_str();\
  absPath = QDir::toNativeSeparators(absPath);\
  QFileInfo fi ( absPath );\
  if ( fi.exists() )  {\
  SET_TEXT_ICON(name, Check)\
  } else {\
  SET_TEXT_ICON(name, Delete) }  }


#define READ_COMBO_BOX(prefs, combobox)\
    { bool ok = false;\
    int i = prefs.value(#combobox).toInt(&ok);\
    if (false == ok) {i=0;}\
    combobox->setCurrentIndex(i); }\


#define GG_CHECK_OUTPUT_FILE_EXISTS(name) \
  { \
  QString absPath = gg_OutputDir->text() + QDir::separator() + AIM::Representation::name.c_str();\
  absPath = QDir::toNativeSeparators(absPath);\
  QFileInfo fi ( absPath );\
  if ( fi.exists() )  {\
    QString iconFile = QString(":/") + QString("Check") + QString("-16x16.png");\
      name##_3->setText(AIM::Representation::name.c_str());\
      name##Icon##_3->setPixmap(QPixmap(iconFile));\
  } else {\
    QString iconFile = QString(":/") + QString("Delete") + QString("-16x16.png");\
    name##_3->setText(AIM::Representation::name.c_str());\
    name##Icon##_3->setPixmap(QPixmap(iconFile));\
  }\
  }


#define GG_CHECK_INPUT_FILE_EXISTS(name) \
  { \
  QString absPath = gg_InputDir->text() + QDir::separator() + AIM::Representation::name.c_str();\
  absPath = QDir::toNativeSeparators(absPath);\
  QFileInfo fi ( absPath );\
  if ( fi.exists() )  {\
    QString iconFile = QString(":/") + QString("Check") + QString("-16x16.png");\
      name##_2->setText(AIM::Representation::name.c_str());\
      name##Icon##_2->setPixmap(QPixmap(iconFile));\
  } else {\
    QString iconFile = QString(":/") + QString("Delete") + QString("-16x16.png");\
    name##_2->setText(AIM::Representation::name.c_str());\
    name##Icon##_2->setPixmap(QPixmap(iconFile));\
  }\
  }



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
  READ_SETTING(prefs, angMaxSlice, ok, i, 300 , Int);
  READ_STRING_SETTING(prefs, angFilePrefix, "");
  READ_SETTING(prefs, zStartIndex, ok, i, 1 , Int);
  READ_SETTING(prefs, zEndIndex, ok, i, 10 , Int);
  READ_STRING_SETTING(prefs, zSpacing, "0.25");
  READ_BOOL_SETTING(prefs, mergeTwins, true);
  alreadyFormed->blockSignals(true);
  READ_BOOL_SETTING(prefs, alreadyFormed, false);
  alreadyFormed->blockSignals(false);
  READ_SETTING(prefs, minAllowedGrainSize, ok, i, 8 , Int);
  READ_SETTING(prefs, minConfidence, ok, d, 0.1 , Double);
  READ_SETTING(prefs, misOrientationTolerance, ok, d, 5.0 , Double);
  READ_COMBO_BOX(prefs, crystalStructure)


  /* ******** This Section is for the Grain Generator Tab ************ */
  READ_FILEPATH_SETTING(prefs, gg_InputDir, "");
  READ_FILEPATH_SETTING(prefs, gg_OutputDir, "");
  READ_SETTING(prefs, gg_XResolution, ok, i, 0.25 , Double);
  READ_SETTING(prefs, gg_YResolution, ok, i, 0.25 , Double);
  READ_SETTING(prefs, gg_ZResolution, ok, i, 0.25 , Double);
  READ_SETTING(prefs, gg_NumGrains, ok, i, 1000 , Int);

  READ_SETTING(prefs, gg_OverlapAllowed, ok, i, 0.00 , Double);
  READ_COMBO_BOX(prefs, gg_CrystalStructure)
  READ_COMBO_BOX(prefs, gg_ShapeClass)
  READ_COMBO_BOX(prefs, gg_OverlapAssignment)

}

// -----------------------------------------------------------------------------
//  Write our Prefs to file
// -----------------------------------------------------------------------------
void RepresentationUI::writeSettings()
{
  // std::cout << "writeSettings" << std::endl;
  QSettings prefs;
  bool ok = false;
  qint32 i = 0;
  /* ******** This Section is for the Reconstruction Tab ************ */
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

  /* ******** This Section is for the Grain Generator Tab ************ */
  WRITE_STRING_SETTING(prefs, gg_InputDir);
  WRITE_STRING_SETTING(prefs, gg_OutputDir);
  READ_SETTING(prefs, gg_XResolution, ok, i, 0.25 , Double);
  READ_SETTING(prefs, gg_YResolution, ok, i, 0.25 , Double);
  READ_SETTING(prefs, gg_ZResolution, ok, i, 0.25 , Double);
  READ_SETTING(prefs, gg_NumGrains, ok, i, 300 , Int);

  READ_SETTING(prefs, gg_OverlapAllowed, ok, i, 0.00 , Double);
  READ_COMBO_BOX(prefs, gg_CrystalStructure)
  READ_COMBO_BOX(prefs, gg_ShapeClass)
  READ_COMBO_BOX(prefs, gg_OverlapAssignment)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::setupGui()
{
  // Setup the Reconstruction Tab GUI
  setupGui_Reconstruction();

  // Setup the Grain Generator Tab Gui
  setupGui_GrainGenerator();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::setupGui_Reconstruction()
{
  QString msg ("All files will be over written that appear in the output directory.");

  if (true == this->_verifyPathExists(angDir->text(), this->angDir) )
  {
    findAngMaxSliceAndPrefix();
  }

  if (this->_verifyPathExists(outputDir->text(), this->outputDir) )
  {
    findReconstructionOutputFiles();
  }
  else
  {
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
  }

  QFileInfo fi (outputDir->text() + QDir::separator() +  AIM::Representation::ReconstructedDataFile.c_str() );
  if (alreadyFormed->isChecked() == true && fi.exists() == false)
  {
    alreadyFormed->setChecked(false);
  }

  if (alreadyFormed->isChecked())
  {
    msg += QString("\nThe 'reconstructed_data.txt' file will be used as an import and NOT over written with new data");
  }
  messageLabel->setText(msg);


  m_WidgetList << angDir << angDirBtn << outputDir << outputDirBtn;
  m_WidgetList << angFilePrefix << angMaxSlice << zStartIndex << zEndIndex << zSpacing;
  m_WidgetList << mergeTwins << alreadyFormed << minAllowedGrainSize << minConfidence << misOrientationTolerance;
  m_WidgetList << crystalStructure;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::setupGui_GrainGenerator()
{
  GG_CHECK_INPUT_FILE_EXISTS(VolBinFile)
  GG_CHECK_INPUT_FILE_EXISTS(BOverABinsFile)
  GG_CHECK_INPUT_FILE_EXISTS(COverABinsFile)
  GG_CHECK_INPUT_FILE_EXISTS(COverBBinsFile)
  GG_CHECK_INPUT_FILE_EXISTS(SeNBinsFile)
  GG_CHECK_INPUT_FILE_EXISTS(AxisOrientationsFile)
  GG_CHECK_INPUT_FILE_EXISTS(EulerAnglesFile)
  GG_CHECK_INPUT_FILE_EXISTS(SVNFile)
  GG_CHECK_INPUT_FILE_EXISTS(SVSFile)
  GG_CHECK_INPUT_FILE_EXISTS(MisorientationBinsFile)
  GG_CHECK_INPUT_FILE_EXISTS(MicroBinsFile)

  GG_CHECK_OUTPUT_FILE_EXISTS(CubeFile)
  GG_CHECK_OUTPUT_FILE_EXISTS(AnalysisFile)
  GG_CHECK_OUTPUT_FILE_EXISTS(BoundaryCentersFile)
  GG_CHECK_OUTPUT_FILE_EXISTS(GrainsFile)
  GG_CHECK_OUTPUT_FILE_EXISTS(VolumeFile)

  m_WidgetList << gg_InputDir << gg_InputDirBtn << gg_OutputDir << gg_OutputDirBtn;
  m_WidgetList << gg_CrystalStructure << gg_NumGrains << gg_XResolution << gg_YResolution << gg_ZResolution;
  m_WidgetList << gg_OverlapAllowed << gg_OverlapAssignment << gg_ShapeClass;
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
  QString absPath = outputDir->text() + QDir::separator() + AIM::Representation::ReconstructedDataFile.c_str();
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
  this->angMaxSlice->setValue(maxSlice);
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
      QFileInfo fi (outputDir->text() + QDir::separator() +  AIM::Representation::ReconstructedDataFile.c_str() );
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
  bool ok = false;
  if (reconstructBtn->text().compare("Cancel") == 0)
  {
    if(m_Reconstruction.get() != NULL)
    {
      //std::cout << "canceling from GUI...." << std::endl;
      emit sig_CancelWorker();
    }
    return;
  }


  m_Reconstruction = Reconstruction::New();
  m_Reconstruction->setInputDirectory(angDir->text().toStdString() );
  m_Reconstruction->setOutputDirectory(outputDir->text().toStdString());
  m_Reconstruction->setAngFilePrefix(angFilePrefix->text().toStdString());
  m_Reconstruction->setAngSeriesMaxSlice(angMaxSlice->value());
  m_Reconstruction->setZStartIndex(zStartIndex->value());
  m_Reconstruction->setZEndIndex(zEndIndex->value() + 1);
  m_Reconstruction->setZResolution(zSpacing->text().toDouble(&ok));
  m_Reconstruction->setMergeTwins(mergeTwins->isChecked() );
  m_Reconstruction->setMinAllowedGrainSize(minAllowedGrainSize->value());
  m_Reconstruction->setMinSeedConfidence(minConfidence->value());
  m_Reconstruction->setMisorientationTolerance(misOrientationTolerance->value());

  AIM::Representation::CrystalStructure crystruct = static_cast<AIM::Representation::CrystalStructure>(crystalStructure->currentIndex());
  if (crystruct == 0) { crystruct = AIM::Representation::Cubic; }
  else if (crystruct == 1) { crystruct = AIM::Representation::Hexagonal; }

  m_Reconstruction->setCrystalStructure(crystruct);
  m_Reconstruction->setAlreadyFormed(alreadyFormed->isChecked());

  connect(m_Reconstruction.get(), SIGNAL(finished()),
          this, SLOT( reconstruction_ThreadFinished() ) );
  connect(m_Reconstruction.get(), SIGNAL (updateProgress(int)),
    this, SLOT(reconstruction_ThreadProgressed(int) ) , Qt::DirectConnection);
  connect(m_Reconstruction.get(), SIGNAL (updateMessage(QString)),
          this, SLOT(threadHasMessage(QString) ) );

  setWidgetListEnabled(false);
  m_Reconstruction->start();
  reconstructBtn->setText("Cancel");
  grainGeneratorTab->setEnabled(false);
  surfaceMeshingTab->setEnabled(false);
  volumeMeshingTab->setEnabled(false);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::threadHasMessage(QString message)
{
 // std::cout << "RepresentationUI::threadHasMessage()" << message.toStdString() << std::endl;
  this->statusBar()->showMessage(message);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::reconstruction_ThreadFinished()
{
 // std::cout << "RepresentationUI::reconstruction_Finished()" << std::endl;
  reconstructBtn->setText("Go");
  setWidgetListEnabled(true);
  this->progressBar->setValue(0);
  grainGeneratorTab->setEnabled(true);
  surfaceMeshingTab->setEnabled(true);
  volumeMeshingTab->setEnabled(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::reconstruction_ThreadProgressed(int val)
{
  this->progressBar->setValue( val );
}



/* Grain Generator Slots*/
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::gg_ThreadFinished()
{
  std::cout << "RepresentationUI::grainGenerator_Finished()" << std::endl;
  gg_GoBtn->setText("Go");
  setWidgetListEnabled(true);
  this->gg_progressBar->setValue(0);
  reconstructionTab->setEnabled(true);
  surfaceMeshingTab->setEnabled(true);
  volumeMeshingTab->setEnabled(true);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::gg_ThreadProgressed(int val)
{
  this->gg_progressBar->setValue( val );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_gg_InputDirBtn_clicked()
{
  std::cout << "on_gg_InputDirBtn_clicked" << std::endl;
  QString outputFile = this->m_OpenDialogLastDirectory + QDir::separator();
  outputFile = QFileDialog::getExistingDirectory(this, tr("Select Input Directory"), outputFile);
  if (!outputFile.isNull())
  {
    this->gg_InputDir->setText(outputFile);
    if (_verifyPathExists(outputFile, gg_InputDir) == true)
    {
      setupGui_GrainGenerator(); // Rescan for files in the input and output directory
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_gg_OutputDirBtn_clicked()
{
  QString outputFile = this->m_OpenDialogLastDirectory + QDir::separator();
  outputFile = QFileDialog::getExistingDirectory(this, tr("Select Grain Generator Output Directory"), outputFile);
  if (!outputFile.isNull())
  {
    this->gg_OutputDir->setText(outputFile);
    if (_verifyPathExists(outputFile, outputDir) == true )
    {
      setupGui_GrainGenerator();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_gg_GoBtn_clicked()
{

  if (gg_GoBtn->text().compare("Cancel") == 0)
  {
    if(m_GrainGenerator.get() != NULL)
    {
      //std::cout << "canceling from GUI...." << std::endl;
      emit sig_CancelWorker();
    }
    return;
  }

  m_GrainGenerator = GrainGenerator::New(NULL);
  m_GrainGenerator->setInputDirectory(gg_InputDir->text().toStdString() );
  m_GrainGenerator->setOutputDirectory(gg_OutputDir->text().toStdString());
  m_GrainGenerator->setNumGrains(gg_NumGrains->value());

  int shapeclass = gg_ShapeClass->currentIndex() + 1;
  m_GrainGenerator->setShapeClass(shapeclass);

  m_GrainGenerator->setXResolution(gg_XResolution->value());
  m_GrainGenerator->setYResolution(gg_YResolution->value());
  m_GrainGenerator->setZResolution(gg_ZResolution->value());


  m_GrainGenerator->setOverlapAllowed(gg_OverlapAllowed->value());
  int overlapassignment = gg_OverlapAssignment->currentIndex() + 1;
  m_GrainGenerator->setOverlapAssignment(overlapassignment);

  int crystruct =  gg_CrystalStructure->currentIndex() + 1;
  m_GrainGenerator->setCrystalStructure(crystruct);


  connect(m_GrainGenerator.get(), SIGNAL(finished()),
          this, SLOT( gg_ThreadFinished() ) );
  connect(m_GrainGenerator.get(), SIGNAL (updateProgress(int)),
    this, SLOT(gg_ThreadProgressed(int) ) , Qt::DirectConnection);
  connect(m_GrainGenerator.get(), SIGNAL (updateMessage(QString)),
          this, SLOT(threadHasMessage(QString) ) );

  setWidgetListEnabled(false);
  reconstructionTab->setEnabled(false);
  surfaceMeshingTab->setEnabled(false);
  volumeMeshingTab->setEnabled(false);
  m_GrainGenerator->start();
  gg_GoBtn->setText("Cancel");
}





void RepresentationUI::surfaceMesh_ThreadFinished()
{

}
void RepresentationUI::surfaceMesh_ThreadProgressed(int value)
{

}

void RepresentationUI::volumeMesh_ThreadFinished()
{

}
void RepresentationUI::volumeMesh_ThreadProgressed(int value)
{

}


