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

#include "QImportR3DStackWidget.h"

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
#include "R3DAnalytics/R3DAnalyticsFilters/ImportR3DStack.h"

#include "R3DAnalytics/R3DAnalyticsFiltersWidgets/moc_QImportR3DStackWidget.cxx"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImportR3DStackWidget::QImportR3DStackWidget(QWidget *parent) :
QFilterWidget(parent)
{

  if ( getOpenDialogLastDirectory().isEmpty() )
  {
    setOpenDialogLastDirectory( QDir::homePath() );
  }
  setupUi(this);

  setupGui();
  ImportR3DStack::Pointer filter = ImportR3DStack::New();
  FloatVec3Widget_t origin = filter->getOrigin();
  FloatVec3Widget_t resolution = filter->getResolution();
  // Get the default values from the filter
  xOrigin->setText(QString::number(origin.x));
  yOrigin->setText(QString::number(origin.y));
  zOrigin->setText(QString::number(origin.z));

  xRes->setText(QString::number(resolution.x));
  yRes->setText(QString::number(resolution.y));
  zRes->setText(QString::number(resolution.z));

  setTitle(QString::fromStdString(filter->getHumanLabel()));
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImportR3DStackWidget::~QImportR3DStackWidget()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QImportR3DStackWidget::getFilterGroup()
{
    return QString::fromStdString(DREAM3D::FilterGroups::GenericFilters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer QImportR3DStackWidget::getFilter()
{
  //bool ok = false;
  ImportR3DStack::Pointer filter =  ImportR3DStack::New();

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
  std::vector<std::string> fileList = generateFileList(start, end, hasMissingFiles, m_StackLowToHigh->isChecked(), filename);
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

  bool ok = false;

  FloatVec3Widget_t vec3f;
  vec3f.x = xOrigin->text().toFloat(&ok);
  vec3f.y = yOrigin->text().toFloat(&ok);
  vec3f.z = zOrigin->text().toFloat(&ok);
  filter->setOrigin(vec3f);

  vec3f.x = xRes->text().toFloat(&ok);
  vec3f.y = yRes->text().toFloat(&ok);
  vec3f.z = zRes->text().toFloat(&ok);
  filter->setResolution(vec3f);

  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterWidget* QImportR3DStackWidget::createDeepCopy()
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
  std::vector<std::string> fileList = generateFileList(start, end, hasMissingFiles, m_StackLowToHigh->isChecked(), filename);

  w->setEbsdFileList(fileList);

  return w;
  #endif
  return NULL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QImportR3DStackWidget::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList) {
    w->setEnabled(b);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QImportR3DStackWidget::setupGui()
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
  m_WidgetList << xOrigin << yOrigin << zOrigin;
  m_WidgetList << xRes << yRes << zRes;
  m_ErrorMessage->setVisible(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QImportR3DStackWidget::writeOptions(QSettings &prefs)
{
  prefs.setValue("Filter_Name", "ImportR3DStack");
  WRITE_STRING_SETTING(prefs, m_, InputDir)
  WRITE_STRING_SETTING(prefs, m_, FilePrefix)
  WRITE_STRING_SETTING(prefs, m_, FileSuffix)
  WRITE_STRING_SETTING(prefs, m_, FileExt)
  WRITE_STRING_SETTING(prefs, m_, ZStartIndex)
  WRITE_STRING_SETTING(prefs, m_, ZEndIndex)
  WRITE_STRING_SETTING(prefs, m_, TotalDigits)

  bool ok = false;
  // ------------- Origin ----------------------------------
  FloatVec3Widget_t v_Origin;
  v_Origin.x = xOrigin->text().toFloat(&ok);
  v_Origin.y = yOrigin->text().toFloat(&ok);
  v_Origin.z = zOrigin->text().toFloat(&ok);
  prefs.beginWriteArray("Origin", 3);
  prefs.setArrayIndex(0);
  prefs.setValue("x", (double)(v_Origin.x));
  prefs.setArrayIndex(1);
  prefs.setValue("y", (double)(v_Origin.y));
  prefs.setArrayIndex(2);
  prefs.setValue("z", (double)(v_Origin.z));
  prefs.endArray();
  // ------------- Resolution ----------------------------------
  FloatVec3Widget_t v_Resolution;
  v_Resolution.x = xRes->text().toFloat(&ok);
  v_Resolution.y = yRes->text().toFloat(&ok);
  v_Resolution.z = zRes->text().toFloat(&ok);
  prefs.beginWriteArray("Resolution", 3);
  prefs.setArrayIndex(0);
  prefs.setValue("x", (double)(v_Resolution.x));
  prefs.setArrayIndex(1);
  prefs.setValue("y", (double)(v_Resolution.y));
  prefs.setArrayIndex(2);
  prefs.setValue("z", (double)(v_Resolution.z));
  prefs.endArray();

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QImportR3DStackWidget::readOptions(QSettings &prefs)
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
  READ_STRING_SETTING(prefs, m_, FileExt, "tif");

    // ------------- Origin ----------------------------------
  {
   QVariant p_Origin = prefs.value("Origin");

   FloatVec3Widget_t v3 = p_Origin.value<FloatVec3Widget_t>();
   prefs.beginReadArray("Origin");
   prefs.setArrayIndex(0);
   v3.x = prefs.value("x", v3.x).toFloat(&ok);
   xOrigin->setText(QString::number(v3.x));
   prefs.setArrayIndex(1);
   v3.y = prefs.value("y", v3.y).toFloat(&ok);
   yOrigin->setText(QString::number(v3.y));
   prefs.setArrayIndex(2);
   v3.z = prefs.value("z", v3.z).toFloat(&ok);
   zOrigin->setText(QString::number(v3.z));
   prefs.endArray();
  }
  // ------------- Resolution ----------------------------------
  {
   QVariant p_Resolution = prefs.value("Resolution");

   FloatVec3Widget_t v3 = p_Resolution.value<FloatVec3Widget_t>();
   prefs.beginReadArray("Resolution");
   prefs.setArrayIndex(0);
   v3.x = prefs.value("x", v3.x).toFloat(&ok);
   xRes->setText(QString::number(v3.x));
   prefs.setArrayIndex(1);
   v3.y = prefs.value("y", v3.y).toFloat(&ok);
   yRes->setText(QString::number(v3.y));
   prefs.setArrayIndex(2);
   v3.z = prefs.value("z", v3.z).toFloat(&ok);
   zRes->setText(QString::number(v3.z));
   prefs.endArray();
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QImportR3DStackWidget::verifyPathExists(QString outFilePath, QLineEdit* lineEdit)
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
void QImportR3DStackWidget::checkIOFiles()
{
  if (true == this->verifyPathExists(m_InputDir->text(), this->m_InputDir))
   {
     m_findEbsdMaxSliceAndPrefix();
   }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QImportR3DStackWidget::on_m_InputDirBtn_clicked()
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
void QImportR3DStackWidget::on_m_InputDir_textChanged(const QString & text)
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
void QImportR3DStackWidget::on_m_ZEndIndex_valueChanged(int value)
{
  m_generateExampleEbsdInputFile();
    emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QImportR3DStackWidget::on_m_ZStartIndex_valueChanged(int value)
{
  m_generateExampleEbsdInputFile();
    emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QImportR3DStackWidget::on_m_TotalDigits_valueChanged(int value)
{
    m_generateExampleEbsdInputFile();
      emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QImportR3DStackWidget::on_m_FileExt_textChanged(const QString &string)
{
  m_generateExampleEbsdInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QImportR3DStackWidget::on_m_FileSuffix_textChanged(const QString &string)
{
  m_generateExampleEbsdInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QImportR3DStackWidget::on_m_FilePrefix_textChanged(const QString &string)
{
  m_generateExampleEbsdInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<std::string> QImportR3DStackWidget::generateFileList(int start, int end, bool &hasMissingFiles,
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
void QImportR3DStackWidget::m_generateExampleEbsdInputFile()
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
void QImportR3DStackWidget::m_findEbsdMaxSliceAndPrefix()
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


// -----------------------------------------------------------------------------
QUrl QImportR3DStackWidget::htmlHelpIndexFile()
{
  QString lowerFilter = QString("ImportR3DStack").toLower();
  QString appPath = qApp->applicationDirPath();
  QDir helpDir = QDir(appPath);
  QString s("file://");
#if defined(Q_OS_WIN)
  s = s + "/"; /* Need the third slash on windows because file paths start with a drive letter */
#elif defined(Q_OS_MAC)
  if (helpDir.dirName() == "MacOS")
  {
    helpDir.cdUp();
    helpDir.cdUp();
    helpDir.cdUp();
  }
#else
  /* We are on Linux - I think */
  helpDir.cdUp();
#endif
#if defined(Q_OS_WIN)
  QFileInfo fi( helpDir.absolutePath() + "/Help/DREAM3D/" + lowerFilter + ".html");
  if (fi.exists() == false)
  {
    /* The help file does not exist at the default location because we are probably running from visual studio.*/
    /* Try up one more directory */
    helpDir.cdUp();
  }
#endif
  s = s + helpDir.absolutePath() + "/Help/DREAM3D/" + lowerFilter + ".html";
  return QUrl(s);
}

