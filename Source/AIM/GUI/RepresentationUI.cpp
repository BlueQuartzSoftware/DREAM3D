///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  Copyright (c) 2009, Michael Groeber, US Air Force Research Laboratory
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
// This code was partly written under US Air Force Contract FA8650-07-D-5800
//
///////////////////////////////////////////////////////////////////////////////
#include "RepresentationUI.h"

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


//-- AIMRep Includes
#include "AIM/Common/Constants.h"
#include "AIM/Common/AIMRepresentationVersion.h"
#include "AIM/Common/Qt/ApplicationAboutBoxDialog.h"
#include "AIM/Common/Qt/QRecentFileList.h"
#include "AIM/Common/Qt/QR3DFileCompleter.h"
#include "AIM/ANG/H5AngDataLoader.h"


#define READ_STRING_SETTING(prefs, var, emptyValue)\
  var->setText( prefs.value(#var).toString() );\
  if (var->text().isEmpty() == true) { var->setText(emptyValue); }

#define READ_FILEPATH_SETTING(prefs, var, emptyValue)\
    var->blockSignals(true);\
  var->setText( prefs.value(#var).toString() );\
  _verifyPathExists(prefs.value(#var).toString(), var);\
  if (var->text().isEmpty() == true) { var->setText(emptyValue); }\
  var->blockSignals(false);

#define READ_SETTING(prefs, var, ok, temp, default, type)\
  ok = false;\
  temp = prefs.value(#var).to##type(&ok);\
  if (false == ok) {temp = default;}\
  var->setValue(temp);

#define READ_BOOL_SETTING(prefs, prefix, tag, emptyValue)\
  { QString s = prefs.value(#tag).toString();\
  if (s.isEmpty() == false) {\
    bool bb = prefs.value(#tag).toBool();\
  prefix##tag->setChecked(bb); } else { prefix##tag->setChecked(emptyValue); } }

#define WRITE_BOOL_SETTING(prefs, tag,  b)\
    prefs.setValue(#tag, (b) );

#define WRITE_STRING_SETTING(prefs, var)\
  prefs.setValue(#var , this->var->text());

#define WRITE_SETTING(prefs, var)\
  prefs.setValue(#var, this->var->value());


#define READ_COMBO_BOX(prefs, combobox)\
    { bool ok = false;\
    int i = prefs.value(#combobox).toInt(&ok);\
    if (false == ok) {i=0;}\
    combobox->setCurrentIndex(i); }

#define WRITE_INT_SETTING(prefs, var)\
    prefs.setValue(#var, var);

#define READ_INT_SETTING(prefs, var)\
    { bool ok = false;\
    int i = prefs.value(#var).toInt(&ok);\
    if (false == ok) {i=0;} }

#define WRITE_COMBO_BOX(prefs, combobox)\
  prefs.setValue(#combobox, this->combobox->currentIndex());


#define CHECK_QLABEL_OUTPUT_FILE_EXISTS_BODY1(ns, prefixname, name)\
  prefixname->setText(ns::name.c_str());\
  prefixname##Icon->setPixmap(QPixmap(iconFile));\



#define CHECK_QLINEEDIT_FILE_EXISTS(name) \
  { \
  QString absPath = QDir::toNativeSeparators(name->text());\
  QFileInfo fi ( absPath );\
  QString iconFile;\
  if ( fi.exists() && fi.isFile() )  {\
    iconFile = QString(":/") + QString("Check") + QString("-16x16.png");\
  } else {\
    iconFile = QString(":/") + QString("Delete") + QString("-16x16.png");\
  }\
  name##Icon->setPixmap(QPixmap(iconFile));\
 }


#define CHECK_QLABEL_OUTPUT_FILE_EXISTS(ns, prefix, name) \
{ \
  QString absPath = prefix##OutputDir->text() + QDir::separator() + ns::name.c_str();\
  absPath = QDir::toNativeSeparators(absPath);\
  QFileInfo fi ( absPath );\
  QString iconFile;\
  if ( fi.exists() )  {\
    iconFile = QString(":/") + QString("Check") + QString("-16x16.png");\
  } else {\
    iconFile = QString(":/") + QString("Delete") + QString("-16x16.png");\
  }\
  CHECK_QLABEL_OUTPUT_FILE_EXISTS_BODY1(ns, prefix##name, name)\
}


#define CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(ns, prefix, name) \
{ \
  QString absPath = prefix##OutputDir->text() + QDir::separator() + ns::name.c_str();\
  absPath = QDir::toNativeSeparators(absPath);\
  QFileInfo fi ( absPath );\
  QString iconFile;\
  if ( fi.exists() )  {\
    iconFile = QString(":/") + QString("Check") + QString("-16x16.png");\
  } else {\
    iconFile = QString(":/") + QString("Delete") + QString("-16x16.png");\
  }\
  CHECK_QLABEL_OUTPUT_FILE_EXISTS_BODY1(ns, prefix##name, name)\
}

#define SANITY_CHECK_INPUT(prefix, input)\
  if (_verifyPathExists(prefix##input->text(), prefix##input) == false) {\
  QMessageBox::critical(this, tr("AIM Representation"),\
  tr("The input " #input " does not exist. Please ensure the file or folder exists before starting the operation"),\
  QMessageBox::Ok,\
  QMessageBox::Ok);\
  return;\
  }


#define SANITY_CHECK_QLABEL_FILE(ns, prefix, input) \
  { \
  QString absPath = prefix##InputDir->text() + QDir::separator() + ns::input.c_str();\
  absPath = QDir::toNativeSeparators(absPath);\
  QFileInfo fi ( absPath );\
  QString theMessage = QString("The input ") + QString(ns::input.c_str()) + \
  QString(" does not exist. Please ensure the file or folder exists before starting the operation");\
  if ( fi.exists() == false)  {\
  QMessageBox::critical(this, tr("AIM Representation"),\
  theMessage,\
  QMessageBox::Ok,\
  QMessageBox::Ok);\
  return;\
  }\
 }

#define CHECK_QLABEL_INPUT_FILE_EXISTS(ns, prefix, name) \
{ \
  QString absPath = prefix##InputDir->text() + QDir::separator() + ns::name.c_str();\
  absPath = QDir::toNativeSeparators(absPath);\
  QFileInfo fi ( absPath );\
  QString iconFile;\
  if ( fi.exists() )  {\
    iconFile = QString(":/") + QString("Check") + QString("-16x16.png");\
  } else {\
    iconFile = QString(":/") + QString("Delete") + QString("-16x16.png");\
  }\
  CHECK_QLABEL_OUTPUT_FILE_EXISTS_BODY1(ns, prefix##name, name)\
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RepresentationUI::RepresentationUI(QWidget *parent) :
  QMainWindow(parent),
  m_WorkerThread(NULL),
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
  if (m_WorkerThread != NULL) {
    delete m_WorkerThread;
  }
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
  qint32 err = checkDirtyDocument();
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
#if defined (Q_OS_MAC)
  QSettings prefs(QSettings::NativeFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#else
  QSettings prefs(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#endif
  QString val;
  bool ok;
  qint32 i;
  double d;

  int ActiveTab = 0;
  READ_INT_SETTING(prefs, ActiveTab);
  this->tabWidget->setCurrentIndex(ActiveTab);

  /* ******** This Section is for the OIM Import Data Tab ************ */
  oim_LoadSettings(prefs);

  /* ******** This Section is for the Reconstruction Tab ************ */
  rec_LoadSettings(prefs);

  /* ******** This Section is for the Grain Generator Tab ************ */
  gg_LoadSettings(prefs);

  /* ******** This Section is for the Surface Meshing Tab ************ */
  sm_LoadSettings(prefs);

  /* ******** This Section is for the Volume Meshing Tab ************ */
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

}

// -----------------------------------------------------------------------------
//  Write our Prefs to file
// -----------------------------------------------------------------------------
void RepresentationUI::writeSettings()
{
  // std::cout << "writeSettings" << std::endl;
#if defined (Q_OS_MAC)
  QSettings prefs(QSettings::NativeFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#else
  QSettings prefs(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#endif
//  bool ok = false;
//  qint32 i = 0;
  int ActiveTab = this->tabWidget->currentIndex();
  WRITE_INT_SETTING(prefs, ActiveTab);

  /* ******** This Section is for the OIM Import Data Tab ************ */
  oim_SaveSettings(prefs);

  /* ******** This Section is for the Reconstruction Tab ************ */
  rec_SaveSettings(prefs);

  /* ******** This Section is for the Grain Generator Tab ************ */
  gg_SaveSettings(prefs);

  /* ******** This Section is for the Surface Meshing Tab ************ */
  sm_SaveSettings(prefs);

  /* ******** This Section is for the Volume Meshing Tab ************ */
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

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::setupGui()
{
  // Setup the OIM Data Import Tab
  oim_SetupGui();
  oim_CheckIOFiles();

  // Setup the Reconstruction Tab GUI
  rec_SetupGui();
  rec_CheckIOFiles();

  // Setup the Grain Generator Tab Gui
  gg_SetupGui();
  gg_CheckIOFiles();

  // Setup the SurfaceMeshing Tab Gui
  sm_SetupGui();
  sm_CheckIOFiles();

  // Setup the Volume Meshing Tab Gui
//  vm_SetupGui();
//  vm_CheckIOFiles();
  this->tabWidget->removeTab(4);
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
//  if (false == parent.exists() )
//  {
//    lineEdit->setStyleSheet("border: 1px solid red;");
//  }
//  else
//  {
//    lineEdit->setStyleSheet("");
//  }
  return parent.exists();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool RepresentationUI::_verifyPathExists(QString outFilePath, QLineEdit* lineEdit)
{
//  std::cout << "outFilePath: " << outFilePath.toStdString() << std::endl;
  QFileInfo fileinfo(outFilePath);
  if (false == fileinfo.exists() )
  {
    lineEdit->setStyleSheet("border: 1px solid red;");
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
qint32 RepresentationUI::checkDirtyDocument()
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
  err = checkDirtyDocument();
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

// Reconstruction Methods
/* *****************************************************************************
 *
 * Reconstruction Methods
 *
 ***************************************************************************** */


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::rec_SetupGui()
{

  QR3DFileCompleter* com2 = new QR3DFileCompleter(this, true);
  rec_OutputDir->setCompleter(com2);
  QObject::connect( com2, SIGNAL(activated(const QString &)),
           this, SLOT(on_rec_OutputDir_textChanged(const QString &)));

  QR3DFileCompleter* com1 = new QR3DFileCompleter(this, false);
  rec_H5InputFile->setCompleter(com1);
  QObject::connect( com1, SIGNAL(activated(const QString &)),
           this, SLOT(on_rec_H5InputFile_textChanged(const QString &)));

  QString msg ("All files will be over written that appear in the output directory.");

  QFileInfo fi (rec_OutputDir->text() + QDir::separator() +  AIM::Reconstruction::VisualizationVizFile.c_str() );
  if (rec_alreadyFormed->isChecked() == true && fi.exists() == false)
  {
    rec_alreadyFormed->setChecked(false);
  }

  if (rec_alreadyFormed->isChecked())
  {
    msg += QString("\nThe 'reconstructed_data.txt' file will be used as an import and NOT over written with new data");
  }
  messageLabel->setText(msg);

  m_WidgetList << rec_OutputDir << rec_OutputDirBtn;
  m_WidgetList << rec_ZStartIndex << rec_ZEndIndex;
  m_WidgetList << rec_mergeTwins << rec_mergeColonies << rec_fillinSample << rec_alreadyFormed << rec_AlignMeth << rec_MinAllowedGrainSize << rec_MinConfidence << rec_DownSampleFactor << rec_MisOrientationTolerance;
  m_WidgetList << rec_CrystalStructure << rec_BinStepSize;
  m_WidgetList << rec_DisorientationVizFile << rec_ImageQualityVizFile << rec_IPFVizFile << rec_SchmidFactorVizFile << rec_VisualizationVizFile << rec_DownSampledVizFile;
  m_WidgetList << rec_MinImageQuality;
  m_WidgetList << rec_HDF5GrainFile << rec_H5StatisticsFile << rec_AlignmentFile << rec_GrainDataFile;
  m_WidgetList << rec_LoadSettingsBtn << rec_SaveSettingsBtn;

}


// -----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------_
void RepresentationUI::rec_CheckIOFiles()
{

  this->_verifyPathExists(rec_OutputDir->text(), this->rec_OutputDir);

  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::Reconstruction,rec_, AlignmentFile)
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::Reconstruction,rec_, GrainDataFile)

  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::Reconstruction, rec_ , DisorientationVizFile)
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::Reconstruction, rec_ , ImageQualityVizFile)
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::Reconstruction, rec_ , IPFVizFile)
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::Reconstruction, rec_ , SchmidFactorVizFile)
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::Reconstruction, rec_ , VisualizationVizFile)
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::Reconstruction, rec_ , DownSampledVizFile)
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::Reconstruction, rec_ , HDF5GrainFile)
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::Reconstruction, rec_ , H5StatisticsFile)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_rec_SaveSettingsBtn_clicked()
{
  QString proposedFile = m_OpenDialogLastDirectory + QDir::separator() + "ReconstructionSettings.txt";
  QString file = QFileDialog::getSaveFileName(this, tr("Save Reconstruction Settings"),
                                              proposedFile,
                                              tr("*.txt") );
  if ( true == file.isEmpty() ){ return;  }

  QSettings prefs(file, QSettings::IniFormat, this);
  rec_SaveSettings(prefs);
}


#define WRITE_CHECKBOX_SETTING(prefs, o)\
    prefs.setValue( o->objectName(), o->isChecked() );


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::rec_SaveSettings(QSettings &prefs)
{
  prefs.beginGroup("Reconstruction");
  WRITE_STRING_SETTING(prefs, rec_OutputDir)
  WRITE_STRING_SETTING(prefs, rec_H5InputFile)
  WRITE_SETTING(prefs, rec_ZStartIndex)
  WRITE_SETTING(prefs, rec_ZEndIndex)

  rec_mergeTwins->objectName();
  prefs.setValue(rec_mergeTwins->objectName(), rec_mergeTwins->isChecked() );


  WRITE_CHECKBOX_SETTING(prefs, rec_mergeTwins)
  WRITE_CHECKBOX_SETTING(prefs, rec_mergeColonies)
  WRITE_CHECKBOX_SETTING(prefs, rec_fillinSample)
  WRITE_CHECKBOX_SETTING(prefs, rec_alreadyFormed)

  WRITE_SETTING(prefs, rec_MinAllowedGrainSize)
  WRITE_SETTING(prefs, rec_MinConfidence)
  WRITE_SETTING(prefs, rec_BinStepSize)
  WRITE_SETTING(prefs, rec_DownSampleFactor)
  WRITE_SETTING(prefs, rec_MinImageQuality)
  WRITE_SETTING(prefs, rec_MisOrientationTolerance)
  WRITE_COMBO_BOX(prefs, rec_CrystalStructure)
  WRITE_COMBO_BOX(prefs, rec_AlignMeth)

  WRITE_CHECKBOX_SETTING(prefs, rec_DisorientationVizFile)
  WRITE_CHECKBOX_SETTING(prefs, rec_ImageQualityVizFile)
  WRITE_CHECKBOX_SETTING(prefs, rec_IPFVizFile)
  WRITE_CHECKBOX_SETTING(prefs, rec_SchmidFactorVizFile)
  WRITE_CHECKBOX_SETTING(prefs, rec_VisualizationVizFile)
  WRITE_CHECKBOX_SETTING(prefs, rec_DownSampledVizFile)
  WRITE_CHECKBOX_SETTING(prefs, rec_HDF5GrainFile)
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_rec_LoadSettingsBtn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Settings File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("Settings File (*.txt)") );
  if ( true == file.isEmpty() ){return;  }
  QSettings prefs(file, QSettings::IniFormat, this);
  rec_LoadSettings(prefs);
}

#define READ_CHECKBOX_SETTING(prefs, tag, emptyValue)\
  { QString s = prefs.value(#tag).toString();\
  if (s.isEmpty() == false) {\
    bool bb = prefs.value(#tag).toBool();\
  tag->setChecked(bb); } else { tag->setChecked(emptyValue); } }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::rec_LoadSettings(QSettings &prefs)
{
  QString val;
  bool ok;
  qint32 i;
  double d;

  prefs.beginGroup("Reconstruction");
  READ_FILEPATH_SETTING(prefs, rec_OutputDir, "");
  READ_FILEPATH_SETTING(prefs, rec_H5InputFile, "");
  if (_verifyPathExists(rec_H5InputFile->text(), rec_H5InputFile) )
  {
    rec_SetSliceInfo();
  }
  READ_SETTING(prefs, rec_ZStartIndex, ok, i, 0, Int)
  READ_SETTING(prefs, rec_ZEndIndex, ok, i, 0, Int)

  READ_CHECKBOX_SETTING(prefs, rec_mergeTwins, false);
  READ_CHECKBOX_SETTING(prefs, rec_mergeColonies, false);
  READ_CHECKBOX_SETTING(prefs, rec_fillinSample, false);
  READ_CHECKBOX_SETTING(prefs, rec_alreadyFormed, false);
  READ_SETTING(prefs, rec_MinAllowedGrainSize, ok, i, 8 , Int);
  READ_SETTING(prefs, rec_MinConfidence, ok, d, 0.1 , Double);
  READ_SETTING(prefs, rec_DownSampleFactor, ok, d, 1.0 , Double);
  READ_SETTING(prefs, rec_MinImageQuality, ok, d, 50.0 , Double);
  READ_SETTING(prefs, rec_MisOrientationTolerance, ok, d, 5.0 , Double);
  READ_SETTING(prefs, rec_BinStepSize, ok, d, 1.0 , Double);
  READ_COMBO_BOX(prefs, rec_CrystalStructure)
  READ_COMBO_BOX(prefs, rec_AlignMeth)

  READ_CHECKBOX_SETTING(prefs, rec_DisorientationVizFile, true);
  READ_CHECKBOX_SETTING(prefs, rec_ImageQualityVizFile, true);
  READ_CHECKBOX_SETTING(prefs, rec_IPFVizFile, true);
  READ_CHECKBOX_SETTING(prefs, rec_SchmidFactorVizFile, true);
  READ_CHECKBOX_SETTING(prefs, rec_VisualizationVizFile, true);
  READ_CHECKBOX_SETTING(prefs, rec_DownSampledVizFile, true);
  READ_CHECKBOX_SETTING(prefs, rec_HDF5GrainFile, true);
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_rec_OIMH5Btn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Input File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("HDF5 OIM Files (*.h5 *.hdf5 *.h5ang)") );
  if ( true == file.isEmpty() ){return;  }
  QFileInfo fi (file);
 // QString ext = fi.suffix();
  _verifyPathExists(file, rec_H5InputFile);
  rec_H5InputFile->setText(fi.absoluteFilePath());
  rec_SetSliceInfo();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_rec_alreadyFormed_stateChanged(int currentState)
{
  QString absPath = rec_OutputDir->text() + QDir::separator() + AIM::Reconstruction::VisualizationVizFile.c_str();
  absPath = QDir::toNativeSeparators(absPath);
  QFileInfo fi (absPath);
  QString msg ("All files will be over written that appear in the output directory.");
  if (rec_alreadyFormed->isChecked() == true && fi.exists() == false)
  {
    QMessageBox::critical(this, tr("AIM Representation"),
      tr("You have selected the 'Already Formed' check box \nbut the correct output file does not exist.\n"
      "The checkbox will revert to an unchecked state.?"),
      QMessageBox::Ok,
      QMessageBox::Ok);
    rec_alreadyFormed->setChecked(false);
      CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::Reconstruction, rec_, VisualizationVizFile)
  }

  if (rec_alreadyFormed->isChecked())
  {
    msg += QString("\nThe 'reconstructed_data.txt' file will be used as an import and NOT over written with new data");
  }
  messageLabel->setText(msg);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_rec_OutputDirBtn_clicked()
{
  QString outputFile = this->m_OpenDialogLastDirectory + QDir::separator();
  outputFile = QFileDialog::getExistingDirectory(this, tr("Select Output Directory"), outputFile);
  if (!outputFile.isNull())
  {
    this->rec_OutputDir->setText(outputFile);
    if (_verifyPathExists(outputFile, rec_OutputDir) == true )
    {
      rec_CheckIOFiles();
      QFileInfo fi (rec_OutputDir->text() + QDir::separator() +  AIM::Reconstruction::VisualizationVizFile.c_str() );
      if (rec_alreadyFormed->isChecked() == true && fi.exists() == false)
      {
        rec_alreadyFormed->setChecked(false);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_rec_H5InputFile_textChanged(const QString &text)
{
  if (_verifyPathExists(rec_H5InputFile->text(), rec_H5InputFile) )
  {
    rec_SetSliceInfo();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::rec_SetSliceInfo()
{
  AbstractAngDataLoader::Pointer ptr = H5AngDataLoader::New();
  H5AngDataLoader* reader = dynamic_cast<H5AngDataLoader*>(ptr.get());
  QFileInfo fi(rec_H5InputFile->text());
  if (fi.isFile() == false)
  {
    return;
  }

  reader->setFilename(rec_H5InputFile->text().toStdString());
  int zStart = -1, zEnd = -1;
  float zRes = 1.0;
  if (reader->readZHeader(zStart, zEnd, zRes) >= 0)
  {
    rec_ZStartIndex->setValue(zStart);
    rec_ZStartIndex->setRange(zStart, zEnd);
    rec_ZEndIndex->setValue(zEnd);
    rec_ZEndIndex->setRange(zStart, zEnd);
    rec_ZRes->setText(QString::number(zRes));
    rec_ZMin->setText(QString::number(zStart));
    rec_ZMax->setText(QString::number(zEnd));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_rec_OutputDir_textChanged(const QString & text)
{
  _verifyPathExists(rec_OutputDir->text(), rec_OutputDir);
  rec_CheckIOFiles();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_rec_GoBtn_clicked()
{
  if (rec_GoBtn->text().compare("Cancel") == 0)
  {
    if(m_Reconstruction.get() != NULL)
    {
      //std::cout << "canceling from GUI...." << std::endl;
      emit sig_CancelWorker();
    }
    return;
  }

  SANITY_CHECK_INPUT(rec_ , OutputDir)
  if (m_WorkerThread != NULL)
  {
    m_WorkerThread->wait(); // Wait until the thread is complete
    delete m_WorkerThread; // Kill the thread
    m_WorkerThread = NULL;
  }
  m_WorkerThread = new QThread(); // Create a new Thread Resource

  m_Reconstruction = Reconstruction::New();

  // Move the Reconstruction object into the thread that we just created.
  m_Reconstruction->moveToThread(m_WorkerThread);


  m_Reconstruction->setH5AngFile(rec_H5InputFile->text().toStdString());

  m_Reconstruction->setZStartIndex(rec_ZStartIndex->value());
  m_Reconstruction->setZEndIndex(rec_ZEndIndex->value() + 1);
  m_Reconstruction->setOutputDirectory(rec_OutputDir->text().toStdString());

  m_Reconstruction->setMergeTwins(rec_mergeTwins->isChecked() );
  m_Reconstruction->setMergeColonies(rec_mergeColonies->isChecked() );
  m_Reconstruction->setFillinSample(rec_fillinSample->isChecked() );
  m_Reconstruction->setMinAllowedGrainSize(rec_MinAllowedGrainSize->value());
  m_Reconstruction->setMinSeedConfidence(rec_MinConfidence->value());
  m_Reconstruction->setSizeBinStepSize(rec_BinStepSize->value());
  m_Reconstruction->setDownSampleFactor(rec_DownSampleFactor->value());
  m_Reconstruction->setMinSeedImageQuality(rec_MinImageQuality->value());
  m_Reconstruction->setMisorientationTolerance(rec_MisOrientationTolerance->value());

  AIM::Reconstruction::CrystalStructure crystruct = static_cast<AIM::Reconstruction::CrystalStructure>(rec_CrystalStructure->currentIndex());
  AIM::Reconstruction::AlignmentMethod alignmeth = static_cast<AIM::Reconstruction::AlignmentMethod>(rec_AlignMeth->currentIndex() );

  m_Reconstruction->setCrystalStructure(crystruct);
  m_Reconstruction->setAlignmentMethod(alignmeth);
  m_Reconstruction->setAlreadyFormed(rec_alreadyFormed->isChecked());

  m_Reconstruction->setWriteVisualizationFile(rec_VisualizationVizFile->isChecked());
  m_Reconstruction->setWriteIPFFile(rec_IPFVizFile->isChecked());
  m_Reconstruction->setWriteDisorientationFile(rec_DisorientationVizFile->isChecked());
  m_Reconstruction->setWriteImageQualityFile(rec_ImageQualityVizFile->isChecked());
  m_Reconstruction->setWriteSchmidFactorFile(rec_SchmidFactorVizFile->isChecked());
  m_Reconstruction->setWriteDownSampledFile(rec_DownSampledVizFile->isChecked());
  m_Reconstruction->setWriteHDF5GrainFile(rec_HDF5GrainFile->isChecked());


  /* Connect the signal 'started()' from the QThread to the 'run' slot of the
   * Reconstruction object. Since the Reconstruction object has been moved to another
   * thread of execution and the actual QThread lives in *this* thread then the
   * type of connection will be a Queued connection.
   */
  // When the thread starts its event loop, start the Reconstruction going
  connect(m_WorkerThread, SIGNAL(started()),
          m_Reconstruction.get(), SLOT(compute()));

  // When the Reconstruction ends then tell the QThread to stop its event loop
  connect(m_Reconstruction.get(), SIGNAL(finished() ),
          m_WorkerThread, SLOT(quit()) );

  // When the QThread finishes, tell this object that it has finished.
  connect(m_WorkerThread, SIGNAL(finished()),
          this, SLOT( rec_ThreadFinished() ) );

  // Send Progress from the Reconstruction to this object for display
  connect(m_Reconstruction.get(), SIGNAL (updateProgress(int)),
    this, SLOT(rec_ThreadProgressed(int) ) );

  // Send progress messages from Reconstruction to this object for display
  connect(m_Reconstruction.get(), SIGNAL (updateMessage(QString)),
          this, SLOT(threadHasMessage(QString) ) );

  // If the use clicks on the "Cancel" button send a message to the Reconstruction object
  // We need a Direct Connection so the
  connect(this, SIGNAL(sig_CancelWorker() ),
          m_Reconstruction.get(), SLOT (on_CancelWorker() ) , Qt::DirectConnection);


  setWidgetListEnabled(false);
  grainGeneratorTab->setEnabled(false);
  surfaceMeshingTab->setEnabled(false);
  volumeMeshingTab->setEnabled(false);
  m_WorkerThread->start();
  rec_GoBtn->setText("Cancel");
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
void RepresentationUI::rec_ThreadFinished()
{
  std::cout << "RepresentationUI::reconstruction_Finished()" << std::endl;
  rec_GoBtn->setText("Go");
  setWidgetListEnabled(true);
  this->rec_progressBar->setValue(0);
  grainGeneratorTab->setEnabled(true);
  surfaceMeshingTab->setEnabled(true);
  volumeMeshingTab->setEnabled(true);
  oimImportTab->setEnabled(true);
  rec_CheckIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::rec_ThreadProgressed(int val)
{
  this->rec_progressBar->setValue( val );
}


/* *****************************************************************************
 *
 * Grain Generator Methods
 *
 ***************************************************************************** */
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::gg_SetupGui()
{
  gg_outputfile_msg->setText("");

  if (NULL == gg_H5InputStatisticsFile->completer()) {
    QR3DFileCompleter* com = new QR3DFileCompleter(this, false);
    gg_H5InputStatisticsFile->setCompleter(com);
    QObject::connect( com, SIGNAL(activated(const QString &)),
             this, SLOT(on_gg_H5InputStatisticsFile_textChanged(const QString &)));
  }

  if (NULL == gg_OutputDir->completer()) {
    QR3DFileCompleter* com2 = new QR3DFileCompleter(this, true);
    gg_OutputDir->setCompleter(com2);
    QObject::connect( com2, SIGNAL(activated(const QString &)),
             this, SLOT(on_gg_OutputDir_textChanged(const QString &)));
  }
  QString msg ("All files will be over written that appear in the output directory.");

  QFileInfo fi (gg_OutputDir->text() + QDir::separator() +  AIM::SyntheticBuilder::VisualizationFile.c_str() );
  if (gg_AlreadyFormed->isChecked() == true && fi.exists() == false)
  {
    gg_AlreadyFormed->setChecked(false);
  }

  if (gg_AlreadyFormed->isChecked())
  {
    msg += QString("\nThe 'Cube.vtk' file will be used as an import and NOT over written with new data");
  }
  messageLabel->setText(msg);

  m_WidgetList << gg_H5StatisticsFile << gg_InputH5StatisticsFileBtn << gg_OutputDir << gg_OutputDirBtn;
  m_WidgetList << gg_CrystalStructure << gg_NumGrains << gg_XResolution << gg_YResolution << gg_ZResolution << gg_FillingErrorWeight;
  m_WidgetList << gg_NeighborhoodErrorWeight << gg_SizeDistErrorWeight << gg_FractionPrecipitates;
  m_WidgetList << gg_ShapeClass << gg_Precipitates << gg_AlreadyFormed;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_gg_LoadSettingsBtn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Settings File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("Settings File (*.txt)") );
  if ( true == file.isEmpty() ){return;  }
  QSettings prefs(file, QSettings::IniFormat, this);
  gg_LoadSettings(prefs);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_gg_SaveSettingsBtn_clicked()
{
  QString proposedFile = m_OpenDialogLastDirectory + QDir::separator() + "GrainGeneratorSettings.txt";
  QString file = QFileDialog::getSaveFileName(this, tr("Save Grain Generator Settings"),
                                              proposedFile,
                                              tr("*.txt") );
  if ( true == file.isEmpty() ){ return;  }

  QSettings prefs(file, QSettings::IniFormat, this);
  gg_SaveSettings(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::gg_SaveSettings(QSettings &prefs)
{
  prefs.beginGroup("GrainGenerator");
  WRITE_STRING_SETTING(prefs, gg_H5InputStatisticsFile)
  WRITE_STRING_SETTING(prefs, gg_OutputDir)
  WRITE_SETTING(prefs, gg_XResolution )
  WRITE_SETTING(prefs, gg_YResolution )
  WRITE_SETTING(prefs, gg_ZResolution )
  WRITE_SETTING(prefs, gg_FillingErrorWeight )
  WRITE_SETTING(prefs, gg_NeighborhoodErrorWeight )
  WRITE_SETTING(prefs, gg_SizeDistErrorWeight )
  WRITE_SETTING(prefs, gg_FractionPrecipitates )
  WRITE_SETTING(prefs, gg_NumGrains )

  WRITE_BOOL_SETTING(prefs, gg_AlreadyFormed, gg_AlreadyFormed->isChecked())
  WRITE_COMBO_BOX(prefs, gg_CrystalStructure)
  WRITE_COMBO_BOX(prefs, gg_ShapeClass)
  WRITE_COMBO_BOX(prefs, gg_Precipitates)
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::gg_LoadSettings(QSettings &prefs)
{
  QString val;
  bool ok;
  qint32 i;
  double d;

  prefs.beginGroup("GrainGenerator");
  READ_FILEPATH_SETTING(prefs, gg_H5InputStatisticsFile, "");
  READ_FILEPATH_SETTING(prefs, gg_OutputDir, "");
  READ_SETTING(prefs, gg_XResolution, ok, d, 0.25 , Double);
  READ_SETTING(prefs, gg_YResolution, ok, d, 0.25 , Double);
  READ_SETTING(prefs, gg_ZResolution, ok, d, 0.25 , Double);
  READ_SETTING(prefs, gg_FillingErrorWeight, ok, d, 1.0 , Double);
  READ_SETTING(prefs, gg_NeighborhoodErrorWeight, ok, d, 1.0 , Double);
  READ_SETTING(prefs, gg_SizeDistErrorWeight, ok, d, 1.0 , Double);
  READ_SETTING(prefs, gg_FractionPrecipitates, ok, d, 25 , Double);
  READ_SETTING(prefs, gg_NumGrains, ok, i, 1000 , Int);

  READ_BOOL_SETTING(prefs, gg_, AlreadyFormed, false);
  gg_AlreadyFormed->blockSignals(false);
  READ_COMBO_BOX(prefs, gg_CrystalStructure)
  READ_COMBO_BOX(prefs, gg_ShapeClass)
  READ_COMBO_BOX(prefs, gg_Precipitates)
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::gg_CheckIOFiles()
{
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::SyntheticBuilder, gg_, CrystallographicErrorFile)
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::SyntheticBuilder, gg_, EulerFile)
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::SyntheticBuilder, gg_, GrainDataFile)
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::SyntheticBuilder, gg_, H5StatisticsFile)
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::SyntheticBuilder, gg_, VisualizationFile)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_gg_AlreadyFormed_stateChanged(int currentState)
{

  QString absPath = gg_OutputDir->text() + QDir::separator() + AIM::SyntheticBuilder::VisualizationFile.c_str();
  absPath = QDir::toNativeSeparators(absPath);
  QFileInfo fi (absPath);
  QString msg ("All files will be over written that appear in the output directory.");
  if (gg_AlreadyFormed->isChecked() == true && fi.exists() == false)
  {
    QMessageBox::critical(this, tr("AIM Representation"),
      tr("You have selected the 'Already Formed' check box \nbut the correct output file does not exist.\n"
      "The checkbox will revert to an unchecked state.?"),
      QMessageBox::Ok,
      QMessageBox::Ok);
      gg_AlreadyFormed->setChecked(false);
      CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::SyntheticBuilder, gg_, VisualizationFile)
  }

  if (gg_AlreadyFormed->isChecked())
  {
    msg += QString("\nThe 'reconstructed_data.txt' file will be used as an import and NOT over written with new data");
  }
  messageLabel->setText(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_gg_InputH5StatisticsFileBtn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Input File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("HDF5 Stats Files (*.h5 *.hdf5)") );
  if ( true == file.isEmpty() ){return;  }
  QFileInfo fi (file);
  QString ext = fi.suffix();
  gg_H5InputStatisticsFile->setText(fi.absoluteFilePath());
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
    if (_verifyPathExists(outputFile, gg_OutputDir) == true )
    {
      gg_CheckIOFiles();
      QFileInfo fi (gg_OutputDir->text() + QDir::separator() +  AIM::SyntheticBuilder::VisualizationFile.c_str() );
      if (gg_AlreadyFormed->isChecked() == true && fi.exists() == false)
      {
        gg_AlreadyFormed->setChecked(false);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_gg_OutputDir_textChanged(const QString & text)
{
  if (_verifyPathExists(gg_OutputDir->text(), gg_OutputDir) )
  {
    gg_CheckIOFiles();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_gg_H5InputStatisticsFile_textChanged(const QString & text)
{


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

  SANITY_CHECK_INPUT(gg_, OutputDir)
  if (m_WorkerThread != NULL)
  {
    m_WorkerThread->wait(); // Wait until the thread is complete
    delete m_WorkerThread; // Kill the thread
    m_WorkerThread = NULL;
  }
  m_WorkerThread = new QThread(); // Create a new Thread Resource

  m_GrainGenerator = GrainGenerator::New(NULL);

  // Move the Reconstruction object into the thread that we just created.
  m_GrainGenerator->moveToThread(m_WorkerThread);


  m_GrainGenerator->setH5StatsFile(gg_H5InputStatisticsFile->text().toStdString() );
  m_GrainGenerator->setOutputDirectory(gg_OutputDir->text().toStdString());
  m_GrainGenerator->setNumGrains(gg_NumGrains->value());

  int shapeclass = gg_ShapeClass->currentIndex() + 1;
  m_GrainGenerator->setShapeClass(shapeclass);

  int Precipitates = gg_Precipitates->currentIndex() + 1;
  m_GrainGenerator->setPrecipitates(Precipitates);

  m_GrainGenerator->setXResolution(gg_XResolution->value());
  m_GrainGenerator->setYResolution(gg_YResolution->value());
  m_GrainGenerator->setZResolution(gg_ZResolution->value());
  m_GrainGenerator->setFillingErrorWeight(gg_FillingErrorWeight->value());
  m_GrainGenerator->setNeighborhoodErrorWeight(gg_NeighborhoodErrorWeight->value());
  m_GrainGenerator->setSizeDistErrorWeight(gg_SizeDistErrorWeight->value());
  m_GrainGenerator->setFractionPrecipitates(gg_FractionPrecipitates->value());


  m_GrainGenerator->setAlreadyFormed(gg_AlreadyFormed->isChecked() );

  AIM::Reconstruction::CrystalStructure crystruct = static_cast<AIM::Reconstruction::CrystalStructure>(gg_CrystalStructure->currentIndex());

  m_GrainGenerator->setCrystalStructure(crystruct);


  /* Connect the signal 'started()' from the QThread to the 'run' slot of the
   * Reconstruction object. Since the Reconstruction object has been moved to another
   * thread of execution and the actual QThread lives in *this* thread then the
   * type of connection will be a Queued connection.
   */
  // When the thread starts its event loop, start the Reconstruction going
  connect(m_WorkerThread, SIGNAL(started()),
          m_GrainGenerator.get(), SLOT(compute()));

  // When the Reconstruction ends then tell the QThread to stop its event loop
  connect(m_GrainGenerator.get(), SIGNAL(finished() ),
          m_WorkerThread, SLOT(quit()) );

  // When the QThread finishes, tell this object that it has finished.
  connect(m_WorkerThread, SIGNAL(finished()),
          this, SLOT( gg_ThreadFinished() ) );

  // Send Progress from the Reconstruction to this object for display
  connect(m_GrainGenerator.get(), SIGNAL (updateProgress(int)),
    this, SLOT(gg_ThreadProgressed(int) ) );

  // Send progress messages from Reconstruction to this object for display
  connect(m_GrainGenerator.get(), SIGNAL (updateMessage(QString)),
          this, SLOT(threadHasMessage(QString) ) );

  // If the use clicks on the "Cancel" button send a message to the Reconstruction object
  // We need a Direct Connection so the
  connect(this, SIGNAL(sig_CancelWorker() ),
          m_GrainGenerator.get(), SLOT (on_CancelWorker() ) , Qt::DirectConnection);


  setWidgetListEnabled(false);
  reconstructionTab->setEnabled(false);
  surfaceMeshingTab->setEnabled(false);
  volumeMeshingTab->setEnabled(false);
  m_WorkerThread->start();
  gg_GoBtn->setText("Cancel");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::gg_ThreadFinished()
{
 // std::cout << "RepresentationUI::grainGenerator_Finished()" << std::endl;
  gg_GoBtn->setText("Go");
  setWidgetListEnabled(true);
  this->gg_progressBar->setValue(0);
  reconstructionTab->setEnabled(true);
  surfaceMeshingTab->setEnabled(true);
  volumeMeshingTab->setEnabled(true);
  gg_CheckIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::gg_ThreadProgressed(int val)
{
  this->gg_progressBar->setValue( val );
}

/* *****************************************************************************
 *
 * Surface Meshing Methods
 *
 ***************************************************************************** */

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::sm_SaveSettings(QSettings &prefs)
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
void RepresentationUI::sm_LoadSettings(QSettings &prefs)
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
void RepresentationUI::sm_SetupGui()
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
void RepresentationUI::sm_CheckIOFiles()
{

  if ( _verifyPathExists(sm_InputFile->text(), sm_InputFile) == true )
  {
    QFileInfo fi (sm_InputFile->text() );
    QString ext = fi.suffix();
  }
  CHECK_QLINEEDIT_FILE_EXISTS(sm_InputFile)

  _verifyPathExists(sm_OutputDir->text(), sm_OutputDir);

  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::SurfaceMeshing, sm_, NodesFile)
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::SurfaceMeshing, sm_, TrianglesFile)
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::SurfaceMeshing, sm_, NodesRawFile)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_sm_InputFileBtn_clicked()
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
void RepresentationUI::on_sm_OutputDirBtn_clicked()
{
  QString outputFile = this->m_OpenDialogLastDirectory + QDir::separator();
  outputFile = QFileDialog::getExistingDirectory(this, tr("Select Surface Meshing Output Directory"), outputFile);
  if (!outputFile.isNull())
  {
    this->sm_OutputDir->setText(outputFile);
    if (_verifyPathExists(outputFile, sm_OutputDir) == true )
    {
      sm_CheckIOFiles();
      sm_OutputDir->setText(outputFile);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_sm_InputFile_textChanged(const QString & text)
{
  sm_CheckIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_sm_OutputDir_textChanged(const QString & text)
{
  sm_CheckIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_sm_GoBtn_clicked()
{
  if (sm_GoBtn->text().compare("Cancel") == 0)
  {
    if(m_SurfaceMesh.get() != NULL)
    {
      //std::cout << "canceling from GUI...." << std::endl;
      emit sig_CancelWorker();
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
          this, SLOT( sm_ThreadFinished() ) );

  // Send Progress from the Reconstruction to this object for display
  connect(m_SurfaceMesh.get(), SIGNAL (updateProgress(int)),
    this, SLOT(sm_ThreadProgressed(int) ) );

  // Send progress messages from Reconstruction to this object for display
  connect(m_SurfaceMesh.get(), SIGNAL (updateMessage(QString)),
          this, SLOT(threadHasMessage(QString) ) );

  // If the use clicks on the "Cancel" button send a message to the Reconstruction object
  // We need a Direct Connection so the
  connect(this, SIGNAL(sig_CancelWorker() ),
          m_SurfaceMesh.get(), SLOT (on_CancelWorker() ) , Qt::DirectConnection);

  setWidgetListEnabled(false);
  reconstructionTab->setEnabled(false);
  grainGeneratorTab->setEnabled(false);
  volumeMeshingTab->setEnabled(false);
  m_WorkerThread->start();
  sm_GoBtn->setText("Cancel");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::sm_ThreadFinished()
{
 // std::cout << "RepresentationUI::surface_meshing()" << std::endl;
  sm_GoBtn->setText("Go");
  setWidgetListEnabled(true);
  this->sm_progressBar->setValue(0);
  reconstructionTab->setEnabled(true);
  surfaceMeshingTab->setEnabled(true);
  volumeMeshingTab->setEnabled(true);
  sm_CheckIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::sm_ThreadProgressed(int value)
{
  sm_progressBar->setValue(value);
}



/* *****************************************************************************
 *
 * Volume Meshing Methods
 *
 ***************************************************************************** */

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::vm_SetupGui()
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
void RepresentationUI::vm_CheckIOFiles()
{

  _verifyPathExists(vm_NodesFile->text(), vm_NodesFile);
  _verifyPathExists(vm_TrianglesFile->text(), vm_TrianglesFile);
  _verifyPathExists(vm_OutputDir->text(), vm_OutputDir );

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
void RepresentationUI::on_vm_NodesFileBtn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Nodes File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("Txt Files (*.txt)") );
  if ( true == file.isEmpty() ){ return;  }
  QFileInfo fi (file);
  vm_NodesFile->setText(fi.absoluteFilePath());
  vm_CheckIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_vm_TrianglesFileBtn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Triangles File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("Txt Files (*.txt)") );
  if ( true == file.isEmpty() ){ return;  }
  QFileInfo fi (file);
  vm_TrianglesFile->setText(fi.absoluteFilePath());
  vm_CheckIOFiles();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_vm_OutputDirBtn_clicked()
{
  QString outputFile = this->m_OpenDialogLastDirectory + QDir::separator();
  outputFile = QFileDialog::getExistingDirectory(this, tr("Select Volume Meshing Output Directory"), outputFile);
  if (!outputFile.isNull())
  {
    this->vm_OutputDir->setText(outputFile);
    if (_verifyPathExists(outputFile, vm_OutputDir) == true )
    {
      vm_CheckIOFiles();
      vm_OutputDir->setText(outputFile);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_vm_GoBtn_clicked()
{

  if (vm_GoBtn->text().compare("Cancel") == 0)
  {
    if(m_VolumeMesh.get() != NULL)
    {
      //std::cout << "canceling from GUI...." << std::endl;
      emit sig_CancelWorker();
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
          this, SLOT( vm_ThreadFinished() ) );

  // Send Progress from the Reconstruction to this object for display
  connect(m_VolumeMesh.get(), SIGNAL (updateProgress(int)),
    this, SLOT(vm_ThreadProgressed(int) ) );

  // Send progress messages from Reconstruction to this object for display
  connect(m_VolumeMesh.get(), SIGNAL (updateMessage(QString)),
          this, SLOT(threadHasMessage(QString) ) );

  // If the use clicks on the "Cancel" button send a message to the Reconstruction object
  // We need a Direct Connection so the
  connect(this, SIGNAL(sig_CancelWorker() ),
          m_VolumeMesh.get(), SLOT (on_CancelWorker() ) , Qt::DirectConnection);

  setWidgetListEnabled(false);
  reconstructionTab->setEnabled(false);
  grainGeneratorTab->setEnabled(false);
  volumeMeshingTab->setEnabled(false);
  m_WorkerThread->start();
  sm_GoBtn->setText("Cancel");

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_vm_NodesFile_textChanged(const QString & text)
{
  vm_CheckIOFiles();
  _verifyPathExists(vm_NodesFile->text(), vm_NodesFile);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_vm_TrianglesFile_textChanged(const QString & text)
{
  vm_CheckIOFiles();
  _verifyPathExists(vm_TrianglesFile->text(), vm_TrianglesFile);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_vm_OutputDir_textChanged(const QString & text)
{
  vm_CheckIOFiles();
  _verifyPathExists(vm_OutputDir->text(), vm_OutputDir);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::vm_ThreadFinished()
{
 // std::cout << "RepresentationUI::volume_meshing()" << std::endl;
  sm_GoBtn->setText("Go");
  setWidgetListEnabled(true);
  this->vm_progressBar->setValue(0);
  reconstructionTab->setEnabled(true);
  grainGeneratorTab->setEnabled(true);
  surfaceMeshingTab->setEnabled(true);
  vm_CheckIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::vm_ThreadProgressed(int value)
{
  vm_progressBar->setValue(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::oim_SetupGui()
{
  QR3DFileCompleter* com = new QR3DFileCompleter(this, true);
  oim_InputDir->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString &)),
           this, SLOT(on_oim_InputDir_textChanged(const QString &)));

  QR3DFileCompleter* com1 = new QR3DFileCompleter(this, false);
  oim_OutputFile->setCompleter(com1);
  QObject::connect( com1, SIGNAL(activated(const QString &)),
           this, SLOT(on_oim_OutputFile_textChanged(const QString &)));

  m_WidgetList << oim_InputDir << oim_InputDirBtn << oim_OutputFile << oim_OutputFileBtn;
  m_WidgetList << oim_FileExt << oim_ErrorMessage << oim_TotalDigits;
  m_WidgetList << oim_FilePrefix << oim_TotalSlices << oim_ZStartIndex << oim_ZEndIndex << oim_zSpacing;
  oim_ErrorMessage->setVisible(false);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::oim_SaveSettings(QSettings &prefs)
{
  prefs.beginGroup("OIMImport");
  WRITE_STRING_SETTING(prefs, oim_InputDir)
  WRITE_STRING_SETTING(prefs, oim_FilePrefix)
  WRITE_STRING_SETTING(prefs, oim_FileSuffix)
  WRITE_STRING_SETTING(prefs, oim_FileExt)
//  WRITE_STRING_SETTING(prefs, oim_TotalSlices)
  WRITE_STRING_SETTING(prefs, oim_ZStartIndex)
  WRITE_STRING_SETTING(prefs, oim_ZEndIndex)
  WRITE_STRING_SETTING(prefs, oim_zSpacing)
  WRITE_STRING_SETTING(prefs, oim_OutputFile)

  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::oim_LoadSettings(QSettings &prefs)
{
  QString val;
  bool ok;
  qint32 i;
  //double d;

  prefs.beginGroup("OIMImport");
  READ_FILEPATH_SETTING(prefs, oim_InputDir, "");
  READ_STRING_SETTING(prefs, oim_FilePrefix, "");
  READ_STRING_SETTING(prefs, oim_FileSuffix, "");
  READ_STRING_SETTING(prefs, oim_FileExt, "ang");
//  READ_STRING_SETTING(prefs, oim_TotalSlices, "");
  READ_SETTING(prefs, oim_ZStartIndex, ok, i, 1 , Int);
  READ_SETTING(prefs, oim_ZEndIndex, ok, i, 10 , Int);
  READ_STRING_SETTING(prefs, oim_zSpacing, "0.25");
  READ_STRING_SETTING(prefs, oim_OutputFile, "Untitled.h5ang");
  prefs.endGroup();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_oim_OutputFile_textChanged(const QString & text)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::oim_CheckIOFiles()
{
  if (true == this->_verifyPathExists(oim_InputDir->text(), this->oim_InputDir))
   {
     oim_findAngMaxSliceAndPrefix();
   }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_oim_OutputFileBtn_clicked()
{
  QString file = QFileDialog::getSaveFileName(this, tr("Save OIM HDF5 File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("HDF5 OIM Files (*.h5ang)") );
  if ( true == file.isEmpty() ){ return;  }
  QFileInfo fi (file);
  QString ext = fi.suffix();
  oim_OutputFile->setText(fi.absoluteFilePath());
  m_OpenDialogLastDirectory = fi.path();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_oim_InputDirBtn_clicked()
{
  // std::cout << "on_angDirBtn_clicked" << std::endl;
  QString outputFile = this->m_OpenDialogLastDirectory + QDir::separator();
  outputFile = QFileDialog::getExistingDirectory(this, tr("Select Ang Directory"), outputFile);
  if (!outputFile.isNull())
  {
    this->oim_InputDir->setText(outputFile);
    oim_findAngMaxSliceAndPrefix();
    _verifyPathExists(outputFile, oim_InputDir);
    m_OpenDialogLastDirectory = outputFile;
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_oim_InputDir_textChanged(const QString & text)
{
  if (_verifyPathExists(oim_InputDir->text(), oim_InputDir) )
  {
    oim_findAngMaxSliceAndPrefix();
  }
  else
  {
    oim_FileListView->clear();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_oim_GoBtn_clicked()
{

  bool ok = false;
  if (oim_GoBtn->text().compare("Cancel") == 0)
  {
    if(m_H5AngImporter.get() != NULL)
    {
      emit sig_CancelWorker();
    }
    return;
  }

  SANITY_CHECK_INPUT( , oim_InputDir)
  m_H5AngImporter = H5AngImporter::New();
  m_H5AngImporter->setOutputFile(oim_OutputFile->text().toStdString() );
  m_H5AngImporter->setZStartIndex(oim_ZStartIndex->value());
  m_H5AngImporter->setZEndIndex(oim_ZEndIndex->value());
  m_H5AngImporter->setZResolution(oim_zSpacing->text().toDouble(&ok));

//  m_H5AngImporter->setInputDirectory(oim_InputDir->text().toStdString() );
//  m_H5AngImporter->setAngFilePrefix(oim_FilePrefix->text().toStdString());
//  m_H5AngImporter->setAngSeriesMaxSlice(oim_TotalSlices->text().toInt(&ok));

  int fileCount = oim_FileListView->count();
  std::vector<std::string> fileList;
  for (int f = 0; f < fileCount; ++f)
  {
    fileList.push_back(oim_FileListView->item(f)->text().toStdString() );
  }

  m_H5AngImporter->setAngFileList(fileList);

  connect(m_H5AngImporter.get(), SIGNAL(finished()),
          this, SLOT( oim_ThreadFinished() ) );
  connect(m_H5AngImporter.get(), SIGNAL (updateProgress(int)),
    this, SLOT(oim_ThreadProgressed(int) ) );
  connect(m_H5AngImporter.get(), SIGNAL (updateMessage(QString)),
          this, SLOT(threadHasMessage(QString) ) );
  connect(this, SIGNAL(sig_CancelWorker() ),
          m_H5AngImporter.get(), SLOT (on_CancelWorker() ) );

  setWidgetListEnabled(false);
  m_H5AngImporter->start();
  oim_GoBtn->setText("Cancel");
  reconstructionTab->setEnabled(false);
  grainGeneratorTab->setEnabled(false);
  surfaceMeshingTab->setEnabled(false);
  volumeMeshingTab->setEnabled(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_oim_ZEndIndex_valueChanged(int value)
{
  oim_generateExampleOimInputFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_oim_ZStartIndex_valueChanged(int value)
{
  oim_generateExampleOimInputFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_oim_TotalDigits_valueChanged(int value)
{
    oim_generateExampleOimInputFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_oim_FileExt_textChanged(const QString &string)
{
  oim_generateExampleOimInputFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_oim_FileSuffix_textChanged(const QString &string)
{
  oim_generateExampleOimInputFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::on_oim_FilePrefix_textChanged(const QString &string)
{
  oim_generateExampleOimInputFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::oim_generateExampleOimInputFile()
{

  QString filename = QString("%1%2%3.%4").arg(oim_FilePrefix->text())
      .arg(oim_ZStartIndex->text(), oim_TotalDigits->value(), '0')
      .arg(oim_FileSuffix->text()).arg(oim_FileExt->text());
  oim_GeneratedFileNameExample->setText(filename);

  // Now generate all the file names the user is asking for and populate the table
  oim_FileListView->clear();
  int start = oim_ZStartIndex->value();
  int end = oim_ZEndIndex->value();
  QIcon greenDot = QIcon(QString(":/green-dot.png"));
  QIcon redDot = QIcon(QString(":/red-dot.png"));
  bool hasMissingFiles = false;
  for (int i = start; i <= end; ++i)
  {
    filename = QString("%1%2%3.%4").arg(oim_FilePrefix->text())
        .arg(QString::number(i), oim_TotalDigits->value(), '0')
        .arg(oim_FileSuffix->text()).arg(oim_FileExt->text());
    QString filePath = oim_InputDir->text() + QDir::separator() + filename;
    QFileInfo fi(filePath);
    QListWidgetItem* item = new QListWidgetItem(filePath, oim_FileListView);
    if (fi.exists() == true)
    {
      item->setIcon(greenDot);
    }
    else
    {
      hasMissingFiles = true;
      item->setIcon(redDot);
    }
  }
  if (hasMissingFiles == true)
  {
    oim_ErrorMessage->setVisible(true);
    oim_ErrorMessage->setText("Alert: File(s) on the list do NOT exist on the filesystem. Please make sure all files exist");
  }
  else
  {
    oim_ErrorMessage->setVisible(false);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::oim_findAngMaxSliceAndPrefix()
{
  if (oim_InputDir->text().length() == 0) { return; }
  QDir dir(oim_InputDir->text());
  QStringList filters;
  QString ext = "." + oim_FileExt->text();
  filters << "*"+ext;
  dir.setNameFilters(filters);
  QFileInfoList angList = dir.entryInfoList();
  int minSlice = 0;
  int maxSlice = 0;
  int currValue = 0;
  bool flag = false;
  bool ok;
  QString fPrefix;
  QRegExp rx("(\\d+)");
  QStringList list;
  int pos = 0;
  int digitStart = 0;
  int digitEnd = 0;
  int totalOimFilesFound = 0;
  foreach(QFileInfo fi, angList)
  {
    if (fi.suffix().compare(ext) && fi.isFile() == true)
    {
      pos = 0;
      list.clear();
      QString fn = fi.baseName();
      digitStart = pos = rx.indexIn(fn, pos);
      digitEnd = digitStart;
      while ((pos = rx.indexIn(fn, pos)) != -1)
      {
        list << rx.cap(0);
        fPrefix = fn.left(pos);
        pos += rx.matchedLength();
      }
      while(digitEnd >= 0 && fn[digitEnd] >= '0' && fn[digitEnd]<='9')
      {
        ++digitEnd;
      }
      oim_TotalDigits->setValue(digitEnd - digitStart);
      if (list.size() > 0) {
        currValue = list.front().toInt(&ok);
        if (false == flag) { minSlice = currValue; flag = true;}
        if (currValue > maxSlice) { maxSlice = currValue; }
        if (currValue < minSlice) { minSlice = currValue; }
      }
      ++totalOimFilesFound;
    }
  }
  this->oim_TotalSlices->setText(QString::number(totalOimFilesFound));
  this->oim_FilePrefix->setText(fPrefix);
  this->oim_ZStartIndex->setValue(minSlice);
  this->oim_ZEndIndex->setValue(maxSlice);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::oim_ThreadFinished()
{
  //std::cout << "RepresentationUI::reconstruction_Finished()" << std::endl;
  oim_GoBtn->setText("Go");
  setWidgetListEnabled(true);
  this->oim_progressBar->setValue(0);
  reconstructionTab->setEnabled(true);
  grainGeneratorTab->setEnabled(true);
  surfaceMeshingTab->setEnabled(true);
  volumeMeshingTab->setEnabled(true);
  oimImportTab->setEnabled(true);
  oim_CheckIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RepresentationUI::oim_ThreadProgressed(int val)
{
  this->oim_progressBar->setValue( val );
}
