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

#include "QImportImageStackWidget.h"

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

#include "QtSupport/QCheckboxDialog.h"
#include "QtSupport/QR3DFileCompleter.h"
#include "QtSupport/DREAM3DQtMacros.h"

// Our own plugin headers
#include "ImageImportPlugin/ImageImportFilters/ImportImageStack.h"

#include "ImageImportPlugin/ImageImportFiltersWidgets/moc_QImportImageStackWidget.cxx"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImportImageStackWidget::QImportImageStackWidget(QWidget *parent) :
QFilterWidget(parent)
{

  if ( getOpenDialogLastDirectory().isEmpty() )
  {
    setOpenDialogLastDirectory( QDir::homePath() );
  }
  setupUi(this);
  ImportImageStack::Pointer filter = ImportImageStack::New();
  setupGui();
  setTitle(QString::fromStdString(filter->getHumanLabel()));
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImportImageStackWidget::~QImportImageStackWidget()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QImportImageStackWidget::getFilterGroup()
{
    return QString::fromStdString(DREAM3D::FilterGroups::GenericFilters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer QImportImageStackWidget::getFilter()
{
  //bool ok = false;
  ImportImageStack::Pointer filter =  ImportImageStack::New();

  filter->setZStartIndex(m_ZStartIndex->value());
  filter->setZEndIndex(m_ZEndIndex->value());


  QString filename = QString("%1%2%3.%4").arg(m_FilePrefix->text())
      .arg(m_ZStartIndex->text(), m_TotalDigits->value(), '0')
      .arg(m_FileSuffix->text()).arg(m_FileExt->text());
  m_GeneratedFileNameExample->setText(filename);

  int start = m_ZStartIndex->value();
  int end = m_ZEndIndex->value();
  bool hasMissingFiles = false;

  // Now generate all the file names in the "Low to High" order because that is what the importer is expecting
  std::vector<std::string> fileList = generateFileList(start, end, hasMissingFiles, true, filename);
  std::vector<std::string> realFileList;
  for(std::vector<std::string>::size_type i = 0; i < fileList.size(); ++i)
  {
    QString filePath = QString::fromStdString(fileList[i]);
    QFileInfo fi(filePath);
    if (fi.exists())
    {
      realFileList.push_back(fileList[i]);
    }
  }

  filter->setImageFileList(realFileList);

  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterWidget* QImportImageStackWidget::createDeepCopy()
{
  #if 0
  QFilterWidget* w = new QFilterWidget();

  bool ok = false;
  w->setOutputFile(QDir::toNativeSeparators(m_OutputFile->text()).toStdString());


  QString filename = QString("%1%2%3.%4").arg(m_FilePrefix->text())
    .arg(m_ZStartIndex->text(), m_TotalDigits->value(), '0')
    .arg(m_FileSuffix->text()).arg(m_FileExt->text());
  m_GeneratedFileNameExample->setText(filename);

  int start = m_ZStartIndex->value();
  int end = m_ZEndIndex->value();
  bool hasMissingFiles = false;

  // Now generate all the file names in the "Low to High" order because that is what the importer is expecting
  std::vector<std::string> fileList = generateFileList(start, end, hasMissingFiles, true, filename);

  w->setEbsdFileList(fileList);

  return w;
  #endif
  return NULL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QImportImageStackWidget::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList) {
    w->setEnabled(b);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QImportImageStackWidget::setupGui()
{
  setCheckable(true);

  setIsSelected(false);

  QR3DFileCompleter* com = new QR3DFileCompleter(this, true);
  m_InputDir->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString &)),
           this, SLOT(on_m_InputDir_textChanged(const QString &)));

  m_WidgetList << m_InputDir << m_InputDirBtn;
  m_WidgetList << m_FileExt << m_ErrorMessage << m_TotalDigits;
  m_WidgetList << m_FilePrefix << m_TotalSlices << m_ZStartIndex << m_ZEndIndex;
  m_ErrorMessage->setVisible(false);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QImportImageStackWidget::readOptions(QSettings &prefs)
{
  QString val;
  bool ok;
  qint32 i;
  //double d;

  // Run the input dir FIRST as the side effect of running this will be resetting much of the user
  // interface widgets to their default values. We then get the values from the prefs object
  // and populate the user interface widgets
  READ_FILEPATH_SETTING(prefs, m_, InputDir, "");

  READ_SETTING(prefs, m_, ZStartIndex, ok, i, 1 , Int);
  READ_SETTING(prefs, m_, ZEndIndex, ok, i, 10 , Int);
  READ_SETTING(prefs, m_, TotalDigits, ok, i, 4 , Int);
  READ_STRING_SETTING(prefs, m_, FilePrefix, "");
  READ_STRING_SETTING(prefs, m_, FileSuffix, "");
  READ_STRING_SETTING(prefs, m_, FileExt, "ang");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QImportImageStackWidget::writeOptions(QSettings &prefs)
{
  prefs.setValue("Filter_Name", "ImportImageStack");
  WRITE_STRING_SETTING(prefs, m_, InputDir)
  WRITE_STRING_SETTING(prefs, m_, FilePrefix)
  WRITE_STRING_SETTING(prefs, m_, FileSuffix)
  WRITE_STRING_SETTING(prefs, m_, FileExt)
  WRITE_STRING_SETTING(prefs, m_, ZStartIndex)
  WRITE_STRING_SETTING(prefs, m_, ZEndIndex)
  WRITE_STRING_SETTING(prefs, m_, TotalDigits)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QImportImageStackWidget::verifyPathExists(QString outFilePath, QLineEdit* lineEdit)
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
void QImportImageStackWidget::checkIOFiles()
{
  if (true == this->verifyPathExists(m_InputDir->text(), this->m_InputDir))
   {
     m_findEbsdMaxSliceAndPrefix();
   }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QImportImageStackWidget::on_m_InputDirBtn_clicked()
{
  // std::cout << "on_angDirBtn_clicked" << std::endl;
  QString outputFile = this->getOpenDialogLastDirectory() + QDir::separator();
  outputFile = QFileDialog::getExistingDirectory(this, tr("Select EBSD Directory"), outputFile);
  if (!outputFile.isNull())
  {
    m_InputDir->blockSignals(true);
    m_InputDir->setText(QDir::toNativeSeparators(outputFile));
    on_m_InputDir_textChanged(m_InputDir->text());
    getOpenDialogLastDirectory() = outputFile;
    m_InputDir->blockSignals(false);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QImportImageStackWidget::on_m_InputDir_textChanged(const QString & text)
{
  if (verifyPathExists(m_InputDir->text(), m_InputDir) )
  {
    m_findEbsdMaxSliceAndPrefix();
    QDir dir(m_InputDir->text());
    dir.cdUp();

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
void QImportImageStackWidget::on_m_ZEndIndex_valueChanged(int value)
{
  m_generateExampleEbsdInputFile();
    emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QImportImageStackWidget::on_m_ZStartIndex_valueChanged(int value)
{
  m_generateExampleEbsdInputFile();
    emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QImportImageStackWidget::on_m_TotalDigits_valueChanged(int value)
{
    m_generateExampleEbsdInputFile();
      emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QImportImageStackWidget::on_m_FileExt_textChanged(const QString &string)
{
  m_generateExampleEbsdInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QImportImageStackWidget::on_m_FileSuffix_textChanged(const QString &string)
{
  m_generateExampleEbsdInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QImportImageStackWidget::on_m_FilePrefix_textChanged(const QString &string)
{
  m_generateExampleEbsdInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<std::string> QImportImageStackWidget::generateFileList(int start, int end, bool &hasMissingFiles,
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
void QImportImageStackWidget::m_generateExampleEbsdInputFile()
{

  QString filename = QString("%1%2%3.%4").arg(m_FilePrefix->text())
      .arg(m_ZStartIndex->text(), m_TotalDigits->value(), '0')
      .arg(m_FileSuffix->text()).arg(m_FileExt->text());
  m_GeneratedFileNameExample->setText(filename);

  int start = m_ZStartIndex->value();
  int end = m_ZEndIndex->value();
  bool hasMissingFiles = false;

  // Now generate all the file names the user is asking for and populate the table
  std::vector<std::string> fileList = generateFileList(start, end, hasMissingFiles, true, filename);

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
void QImportImageStackWidget::m_findEbsdMaxSliceAndPrefix()
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
//  this->m_ZStartIndex->setRange(minSlice, maxSlice);
//  this->m_ZEndIndex->setRange(minSlice, maxSlice);
}
