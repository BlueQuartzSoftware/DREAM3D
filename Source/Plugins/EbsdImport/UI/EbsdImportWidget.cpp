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

#include "EbsdImportWidget.h"

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
#include <QtGui/QButtonGroup>

#include "DREAM3DLib/Common/Constants.h"
#include "EbsdImportPlugin.h"
#include "QtSupport/QCheckboxDialog.h"
#include "QtSupport/QR3DFileCompleter.h"
#include "QtSupport/DREAM3DQtMacros.h"
#include "EbsdReferenceFrameDialog.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EbsdImportWidget::EbsdImportWidget(QWidget *parent) :
DREAM3DPluginFrame(parent),
m_EbsdImport(NULL),
m_WorkerThread(NULL),
m_RotateSlice(true),
m_ReorderArray(true),
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
EbsdImportWidget::~EbsdImportWidget()
{

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdImportWidget::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList) {
    w->setEnabled(b);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdImportWidget::setupGui()
{
  QR3DFileCompleter* com = new QR3DFileCompleter(this, true);
  m_InputDir->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString &)),
           this, SLOT(on_m_InputDir_textChanged(const QString &)));

  QR3DFileCompleter* com1 = new QR3DFileCompleter(this, false);
  m_OutputFile->setCompleter(com1);
  QObject::connect( com1, SIGNAL(activated(const QString &)),
           this, SLOT(on_m_OutputFile_textChanged(const QString &)));

  m_WidgetList << m_InputDir << m_InputDirBtn << m_OutputFile << m_OutputFileBtn;
  m_WidgetList << m_FileExt << m_ErrorMessage << m_TotalDigits;
  m_WidgetList << m_FilePrefix << m_TotalSlices << m_ZStartIndex << m_ZEndIndex << m_zSpacing;
  m_ErrorMessage->setVisible(false);

  m_StackingGroup = new QButtonGroup(this);
  m_StackingGroup->addButton(m_StackLowToHigh);
  m_StackingGroup->addButton(m_StackHighToLow);

  m_OriginGroup = new QButtonGroup;
  m_OriginGroup->addButton(m_RefFrameOptionsBtn);

  connect(m_StackLowToHigh, SIGNAL(toggled(bool)),
          this, SLOT(stackingOrderChanged(bool)));

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdImportWidget::readSettings(QSettings &prefs)
{
  QString val;
  bool ok;
  qint32 i;
  //double d;

  prefs.beginGroup("EbsdImport");
  READ_FILEPATH_SETTING(prefs, m_, InputDir, "");
  READ_STRING_SETTING(prefs, m_, FilePrefix, "");
  READ_STRING_SETTING(prefs, m_, FileSuffix, "");
  READ_STRING_SETTING(prefs, m_, FileExt, "ang");
//  READ_STRING_SETTING(prefs, m_, TotalSlices, "");
  READ_SETTING(prefs, m_, ZStartIndex, ok, i, 1 , Int);
  READ_SETTING(prefs, m_, ZEndIndex, ok, i, 10 , Int);
  READ_STRING_SETTING(prefs, m_, zSpacing, "0.25");
  READ_STRING_SETTING(prefs, m_, OutputFile, "Untitled.h5ebsd");

  on_m_InputDir_textChanged(m_InputDir->text());
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdImportWidget::writeSettings(QSettings &prefs)
{
  prefs.beginGroup("EbsdImport");
  WRITE_STRING_SETTING(prefs, m_, InputDir)
  WRITE_STRING_SETTING(prefs, m_, FilePrefix)
  WRITE_STRING_SETTING(prefs, m_, FileSuffix)
  WRITE_STRING_SETTING(prefs, m_, FileExt)
//  WRITE_STRING_SETTING(prefs, m_, TotalSlices)
  WRITE_STRING_SETTING(prefs, m_, ZStartIndex)
  WRITE_STRING_SETTING(prefs, m_, ZEndIndex)
  WRITE_STRING_SETTING(prefs, m_, zSpacing)
  WRITE_STRING_SETTING(prefs, m_, OutputFile)

  prefs.endGroup();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdImportWidget::on_m_OutputFile_textChanged(const QString & text)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdImportWidget::checkIOFiles()
{
  if (true == this->verifyPathExists(m_InputDir->text(), this->m_InputDir))
   {
     m_findEbsdMaxSliceAndPrefix();
   }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdImportWidget::on_m_OutputFileBtn_clicked()
{
  QString file = QFileDialog::getSaveFileName(this, tr("Save HDF5 EBSD File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("HDF5 EBSD Files (*.h5ebsd)") );
  if ( true == file.isEmpty() ){ return;  }
  QFileInfo fi (file);
  QString ext = fi.suffix();
  m_OutputFile->setText(fi.absoluteFilePath());
  m_OpenDialogLastDirectory = fi.path();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdImportWidget::on_m_InputDirBtn_clicked()
{
  // std::cout << "on_angDirBtn_clicked" << std::endl;
  QString outputFile = this->m_OpenDialogLastDirectory + QDir::separator();
  outputFile = QFileDialog::getExistingDirectory(this, tr("Select EBSD Directory"), outputFile);
  if (!outputFile.isNull())
  {
    m_InputDir->blockSignals(true);
    m_InputDir->setText(QDir::toNativeSeparators(outputFile));
    on_m_InputDir_textChanged(m_InputDir->text());
    m_OpenDialogLastDirectory = outputFile;
    m_InputDir->blockSignals(false);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdImportWidget::on_m_InputDir_textChanged(const QString & text)
{
  if (verifyPathExists(m_InputDir->text(), m_InputDir) )
  {
    m_findEbsdMaxSliceAndPrefix();
    QDir dir(m_InputDir->text());
    QString dirname = dir.dirName();
    dir.cdUp();

    QString outPath = dir.absolutePath() + QDir::separator() + dirname + "_Output" + QDir::separator() + dirname + ".h5ebsd";
    outPath = QDir::toNativeSeparators(outPath);
    m_OutputFile->setText(outPath);
    verifyPathExists(m_OutputFile->text(), m_OutputFile);
    m_generateExampleEbsdInputFile();
    m_InputDir->blockSignals(true);
    m_InputDir->setText(QDir::toNativeSeparators(m_InputDir->text()));
    m_InputDir->blockSignals(false);
  }
  else
  {
    m_FileListView->clear();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Ebsd::RefFrameZDir EbsdImportWidget::getRefFrameZDir()
{
	if (m_StackLowToHigh->isChecked()) return Ebsd::LowtoHigh;
	if (m_StackHighToLow->isChecked()) return Ebsd::HightoLow;
  return Ebsd::UnknownRefFrameZDirection;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdImportWidget::on_m_GoBtn_clicked()
{

  bool ok = false;
  if (m_GoBtn->text().compare("Cancel") == 0)
  {
    if (m_EbsdImport != NULL)
    {
      emit cancelPipeline();
    }
    return;
  }

  // Check the output path for existence.
  QString outPath = m_OutputFile->text();
  QFileInfo fi(m_OutputFile->text());
  m_OutputFile->setText(fi.absolutePath());
  if (false == sanityCheckOutputDirectory(m_OutputFile, QString("EBSD Import")) )
  {
    m_OutputFile->setText(outPath);
    return;
  }
  verifyPathExists(m_OutputFile->text(), m_OutputFile);
  m_OutputFile->setText(outPath);


  // Check the inputs
  SANITY_CHECK_INPUT(m_ , InputDir)

  if (m_WorkerThread != NULL)
  {
    m_WorkerThread->wait(); // Wait until the thread is complete
    delete m_WorkerThread; // Kill the thread
    m_WorkerThread = NULL;
  }
  m_WorkerThread = new QThread(); // Create a new Thread Resource

  m_EbsdImport = new QEbsdImport(NULL);

  // Move the GrainGenerator object into the thread that we just created.
  m_EbsdImport->moveToThread(m_WorkerThread);

  m_EbsdImport->setOutputFile(QDir::toNativeSeparators(m_OutputFile->text()).toStdString());
  m_EbsdImport->setZStartIndex(m_ZStartIndex->value());
  m_EbsdImport->setZEndIndex(m_ZEndIndex->value());
  m_EbsdImport->setZResolution(m_zSpacing->text().toFloat(&ok));

  m_EbsdImport->setRefFrameZDir( getRefFrameZDir() );
  m_EbsdImport->setRotateSlice( m_RotateSlice );
  m_EbsdImport->setReorderArray( m_ReorderArray );
  m_EbsdImport->setAlignEulers( m_AlignEulers );


  QString filename = QString("%1%2%3.%4").arg(m_FilePrefix->text())
      .arg(m_ZStartIndex->text(), m_TotalDigits->value(), '0')
      .arg(m_FileSuffix->text()).arg(m_FileExt->text());
  m_GeneratedFileNameExample->setText(filename);

  int start = m_ZStartIndex->value();
  int end = m_ZEndIndex->value();
  bool hasMissingFiles = false;

  // Now generate all the file names in the "Low to High" order because that is what the importer is expecting
  std::vector<std::string> fileList = generateFileList(start, end, hasMissingFiles, true, filename);

  m_EbsdImport->setEbsdFileList(fileList);

  /* Connect the signal 'started()' from the QThread to the 'run' slot of the
   * Reconstruction object. Since the Reconstruction object has been moved to another
   * thread of execution and the actual QThread lives in *this* thread then the
   * type of connection will be a Queued connection.
   */
  // When the thread starts its event loop, start the Reconstruction going
  connect(m_WorkerThread, SIGNAL(started()), m_EbsdImport, SLOT(run()));

  // When the Reconstruction ends then tell the QThread to stop its event loop
  connect(m_EbsdImport, SIGNAL(finished() ), m_WorkerThread, SLOT(quit()));

  // When the QThread finishes, tell this object that it has finished.
  connect(m_WorkerThread, SIGNAL(finished()), this, SLOT( pipelineComplete() ));

  // Send Progress from the Reconstruction to this object for display
  connect(m_EbsdImport, SIGNAL (updateProgress(int)), this, SLOT(pipelineProgress(int) ));

  // If the use clicks on the "Cancel" button send a message to the Reconstruction object
  // We need a Direct Connection so the
  connect(this, SIGNAL(cancelPipeline() ), m_EbsdImport, SLOT (on_CancelWorker() ), Qt::DirectConnection);

  // Send progress messages from Pipeline to this object for display
  connect(m_EbsdImport, SIGNAL (progressMessage(QString)), this, SLOT(addProgressMessage(QString) ));

  // Send progress messages from Pipeline to this object for display
  connect(m_EbsdImport, SIGNAL (warningMessage(QString)), this, SLOT(addWarningMessage(QString) ));

  // Send progress messages from Pipeline to this object for display
  connect(m_EbsdImport, SIGNAL (errorMessage(QString)), this, SLOT(addErrorMessage(QString) ));

  // If the use clicks on the "Cancel" button send a message to the Reconstruction object
  // We need a Direct Connection so the
  connect(this, SIGNAL(cancelPipeline() ), m_EbsdImport, SLOT (on_CancelWorker() ), Qt::DirectConnection);

  setWidgetListEnabled(false);
  emit pipelineStarted();
  m_WorkerThread->start();
  m_GoBtn->setText("Cancel");

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdImportWidget::stackingOrderChanged(bool checked)
{
  m_generateExampleEbsdInputFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdImportWidget::on_m_ZEndIndex_valueChanged(int value)
{
  m_generateExampleEbsdInputFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdImportWidget::on_m_ZStartIndex_valueChanged(int value)
{
  m_generateExampleEbsdInputFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdImportWidget::on_m_TotalDigits_valueChanged(int value)
{
    m_generateExampleEbsdInputFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdImportWidget::on_m_FileExt_textChanged(const QString &string)
{
  m_generateExampleEbsdInputFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdImportWidget::on_m_FileSuffix_textChanged(const QString &string)
{
  m_generateExampleEbsdInputFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdImportWidget::on_m_FilePrefix_textChanged(const QString &string)
{
  m_generateExampleEbsdInputFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<std::string> EbsdImportWidget::generateFileList(int start, int end, bool &hasMissingFiles,
                                               bool stackLowToHigh, QString filename)
{
  int index = 0;
  std::vector<std::string> fileList;

  for (int i = 0; i < (end-start)+1; ++i)
    {
      if (stackLowToHigh)
      {
         index = start + i;
      }
      else
      {
        index = end - i;
      }
      filename = QString("%1%2%3.%4").arg(m_FilePrefix->text())
          .arg(QString::number(index), m_TotalDigits->value(), '0')
          .arg(m_FileSuffix->text()).arg(m_FileExt->text());
      QString filePath = m_InputDir->text() + QDir::separator() + filename;
      filePath = QDir::toNativeSeparators(filePath);
      fileList.push_back(filePath.toStdString());
    }
  return fileList;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdImportWidget::m_generateExampleEbsdInputFile()
{

  QString filename = QString("%1%2%3.%4").arg(m_FilePrefix->text())
      .arg(m_ZStartIndex->text(), m_TotalDigits->value(), '0')
      .arg(m_FileSuffix->text()).arg(m_FileExt->text());
  m_GeneratedFileNameExample->setText(filename);

  int start = m_ZStartIndex->value();
  int end = m_ZEndIndex->value();
  bool hasMissingFiles = false;

  // Now generate all the file names the user is asking for and populate the table
  std::vector<std::string> fileList = generateFileList(start, end, hasMissingFiles, m_StackLowToHigh->isChecked(), filename);

  m_FileListView->clear();
  QIcon greenDot = QIcon(QString(":/green-dot.png"));
  QIcon redDot = QIcon(QString(":/red-dot.png"));
  for(std::vector<std::string>::size_type i = 0; i < fileList.size(); ++i)
  {
    QString filePath(fileList.at(i).c_str());
    QFileInfo fi(filePath);
    QListWidgetItem* item = new QListWidgetItem( filePath, m_FileListView);
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
    m_ErrorMessage->setVisible(true);
    m_ErrorMessage->setText("Alert: Red Dot File(s) on the list do NOT exist on the filesystem. Please make sure all files exist");
  }
  else
  {
    m_ErrorMessage->setVisible(true);
    m_ErrorMessage->setText("All files exist.");
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdImportWidget::on_m_RefFrameOptionsBtn_clicked()
{
  QString filename = QString("%1%2%3.%4").arg(m_FilePrefix->text())
      .arg(m_ZStartIndex->text(), m_TotalDigits->value(), '0')
      .arg(m_FileSuffix->text()).arg(m_FileExt->text());
  //m_GeneratedFileNameExample->setText(filename);

  int start = m_ZStartIndex->value();
  int end = m_ZEndIndex->value();
  bool hasMissingFiles = false;

  // Now generate all the file names in the "Low to High" order because that is what the importer is expecting
  std::vector<std::string> fileList = generateFileList(start, end, hasMissingFiles, true, filename);
  if (fileList.size() == 0)
  {
    return;
  }
  QString ebsdFileName = QString::fromStdString(fileList[0]);

  EbsdReferenceFrameDialog d(ebsdFileName, this);
  d.setEbsdFileName(ebsdFileName);
  int ret = d.exec();
  if (ret == QDialog::Accepted)
  {
//    m_RotateSlice = d.getValue("Rotate Slice");
//    m_ReorderArray = d.getValue("Reorder Array");
//    m_AlignEulers = d.getValue("Align Eulers");
  }

#if 0
  QVector<QString> options;
  options.push_back("Rotate 2D Scan(s) So Origin of Sample is in Lower Left \n (This Accounts for Samples Being Upside-down with Long WD at the Top of the Scan)");
  options.push_back("Set 0,0 position of Sample RF at Lower Left \n (This Reorders the Data Array to Remove Computer Graphics Convention of Placing the 0 position of an Array in the Upper Left)");
  options.push_back("Align Euler RF to Sample RF \n (This Accounts for Disjoined RFs in Commercial EBSD Data Collection Codes)");

  QCheckboxDialog d(options, this);
  d.setWindowTitle(QString("Advanced Reference Frame Options"));

  d.setValue("Rotate Slice", m_RotateSlice);
  d.setValue("Reorder Array", m_ReorderArray);
  d.setValue("Align Eulers", m_AlignEulers);

  int ret = d.exec();
  if (ret == QDialog::Accepted)
  {
    m_RotateSlice = d.getValue("Rotate Slice");
    m_ReorderArray = d.getValue("Reorder Array");
    m_AlignEulers = d.getValue("Align Eulers");
  }
#endif

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdImportWidget::m_findEbsdMaxSliceAndPrefix()
{
  if (m_InputDir->text().length() == 0) { return; }
  QDir dir(m_InputDir->text());
  m_FileExt->setText("");
  {
    QString ext = ".ang";
    QStringList filters;
    filters << "*" + ext;
    dir.setNameFilters(filters);
    QFileInfoList angList = dir.entryInfoList();
    if (angList.size() != 0)
    {
      m_FileExt->setText("ang");
    }
  }
  if (m_FileExt->text().isEmpty() == true)
  {
    QString ext = ".ctf";
    QStringList filters;
    filters << "*" + ext;
    dir.setNameFilters(filters);
    QFileInfoList angList = dir.entryInfoList();
    if (angList.size() != 0)
    {
      m_FileExt->setText("ctf");
    }
  }
  // Add in more file formats to look for here


  // Final check to make sure we have a valid file extension
  if (m_FileExt->text().isEmpty() == true)
   {
    return;
   }

  QString ext = "." + m_FileExt->text();
  QStringList filters;
  filters << "*" + ext;
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
  int minTotalDigits = 1000;
  foreach(QFileInfo fi, angList)
  {
    if (fi.suffix().compare(ext) && fi.isFile() == true)
    {
      pos = 0;
      list.clear();
      QString fn = fi.baseName();
      std::string fns = fn.toStdString();
      int length =  fn.length();
      digitEnd = length-1;
      while(digitEnd >= 0 && fn[digitEnd] >= '0' && fn[digitEnd]<='9')
      {
        --digitEnd;
      }
      pos = digitEnd;

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

      if ( digitEnd - digitStart < minTotalDigits) { minTotalDigits = digitEnd - digitStart; }
      m_TotalDigits->setValue(minTotalDigits);
      if (list.size() > 0) {
        currValue = list.front().toInt(&ok);
        if (false == flag) { minSlice = currValue; flag = true;}
        if (currValue > maxSlice) { maxSlice = currValue; }
        if (currValue < minSlice) { minSlice = currValue; }
      }
      ++totalOimFilesFound;
    }
  }
  this->m_TotalSlices->setText(QString::number(totalOimFilesFound));
  this->m_FilePrefix->setText(fPrefix);
  this->m_ZStartIndex->setValue(minSlice);
  this->m_ZEndIndex->setValue(maxSlice);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdImportWidget::pipelineComplete()
{
 // std::cout << "EbsdImportWidget::threadFinished()" << std::endl;
  m_GoBtn->setText("Go");
  setWidgetListEnabled(true);
  this->m_progressBar->setValue(0);
  emit pipelineEnded();
  m_EbsdImport->deleteLater();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdImportWidget::pipelineProgress(int val)
{
  this->m_progressBar->setValue( val );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdImportWidget::addErrorMessage(QString message)
{
  QString title = QString::fromStdString(DREAM3D::UIPlugins::EbsdImportDisplayName).append(" Error");
  displayDialogBox(title, message, QMessageBox::Critical);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdImportWidget::addWarningMessage(QString message)
{
  QString title = QString::fromStdString(DREAM3D::UIPlugins::EbsdImportDisplayName).append(" Warning");
  displayDialogBox(title, message, QMessageBox::Warning);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdImportWidget::addProgressMessage(QString message)
{
  if (NULL != this->statusBar()) {
    this->statusBar()->showMessage(message);
  }
}
