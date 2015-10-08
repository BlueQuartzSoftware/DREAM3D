/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include "FileListInfoWidget.h"

//-- Qt Includes
#include <QtCore/QDir>
#include <QtWidgets/QFileDialog>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Utilities/FilePathGenerator.h"
#include "SIMPLib/FilterParameters/FileListInfoFilterParameter.h"

#include "QtSupportLib/QFileCompleter.h"

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"

// Initialize private static member variable
QString FileListInfoWidget::m_OpenDialogLastDirectory = "";

// Include the MOC generated file for this class
#include "moc_FileListInfoWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileListInfoWidget::FileListInfoWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent),
  m_OrderingGroup(NULL),
  m_DidCausePreflight(false)
{
  FileListInfoFilterParameter* fli = dynamic_cast<FileListInfoFilterParameter*>(parameter);
  if(NULL == fli)
  {
    QString msg;
    QTextStream ss(&msg);
    ss << "FileListInfoWidget can ONLY be used with FileListInfoFilterParameter objects. The programmer of the filter has a bug.";
    ss << " The name of the filter was " << filter->getHumanLabel() << " and the name of the Filter Parameter was " << parameter->getHumanLabel();
    ss << " and is trying to get the propery " << parameter->getPropertyName() << " in the filter";
    Q_ASSERT_X(NULL != fli, msg.toLatin1().constData(), __FILE__);
  }
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileListInfoWidget::~FileListInfoWidget()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList)
  {
    w->setEnabled(b);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::setupGui()
{


  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()),
          this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()),
          this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)),
          this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  QFileCompleter* com = new QFileCompleter(this, true);
  m_InputDir->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString&)),
                    this, SLOT(on_m_InputDir_textChanged(const QString&)));

  m_WidgetList << m_InputDir << m_InputDirBtn;
  m_WidgetList << m_FileExt << m_ErrorMessage << m_TotalDigits << m_FileSuffix;
  m_WidgetList << m_FilePrefix << m_TotalSlices << m_StartIndex << m_EndIndex;

  m_ErrorMessage->setVisible(false);

  m_OrderingGroup = new QButtonGroup(this);
  m_OrderingGroup->addButton(m_OrderAscending);
  m_OrderingGroup->addButton(m_OrderDescending);

  connect(m_OrderAscending, SIGNAL(toggled(bool)),
          this, SLOT(orderingChanged(bool)));

  validateInputFile();
  getGuiParametersFromFilter();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::getGuiParametersFromFilter()
{
  blockSignals(true);

  FileListInfo_t data = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<FileListInfo_t>();

  m_InputDir->setText(data.InputPath);

  m_StartIndex->setValue(data.StartIndex);
  m_EndIndex->setValue(data.EndIndex);

  m_FilePrefix->setText(data.FilePrefix);
  m_FileSuffix->setText(data.FileSuffix);

  m_FileExt->setText(data.FileExtension);
  m_TotalDigits->setValue(data.PaddingDigits);

  setOrdering(data.Ordering);
  blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::validateInputFile()
{
  FileListInfo_t data = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<FileListInfo_t>();

  QString currentPath = data.InputPath;
  QFileInfo fi(currentPath);
  if (currentPath.isEmpty() == false && fi.exists() == false)
  {
//    QString Ftype = getFilterParameter()->getFileType();
//    QString ext = getFilterParameter()->getFileExtension();
//    QString s = Ftype + QString(" Files (") + ext + QString(");;All Files(*.*)");
    QString defaultName = m_OpenDialogLastDirectory;


    QString title = QObject::tr("Select a replacement input file in filter '%2'").arg(getFilter()->getHumanLabel());

    QString file = QFileDialog::getExistingDirectory(this, title, defaultName, QFileDialog::ShowDirsOnly);
    if(true == file.isEmpty())
    {
      file = currentPath;
    }
    file = QDir::toNativeSeparators(file);
    // Store the last used directory into the private instance variable
    QFileInfo fi(file);
    m_OpenDialogLastDirectory = fi.path();
    data.InputPath = file;

    QVariant v;
    v.setValue(data);
    bool ok = getFilter()->setProperty(PROPERTY_NAME_AS_CHAR, v);
    if (!ok)
    {

    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FileListInfoWidget::verifyPathExists(QString outFilePath, QLineEdit* lineEdit)
{
  //  std::cout << "outFilePath: " << outFilePath << std::endl;
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
void FileListInfoWidget::checkIOFiles()
{
  if (true == this->verifyPathExists(m_InputDir->text(), this->m_InputDir))
  {
    findMaxSliceAndPrefix();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::on_m_InputDirBtn_clicked()
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
void FileListInfoWidget::on_m_InputDir_textChanged(const QString& text)
{
  if (verifyPathExists(m_InputDir->text(), m_InputDir) )
  {

    findMaxSliceAndPrefix();
    QDir dir(m_InputDir->text());
    QString dirname = dir.dirName();
    dir.cdUp();

    generateExampleInputFile();
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
uint32_t FileListInfoWidget::getOrdering()
{
  if (m_OrderAscending->isChecked()) { return 0; }
  if (m_OrderDescending->isChecked()) { return 1; }
  return 999;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::setOrdering(uint32_t ref)
{
  if (ref == 0)
  {
    m_OrderAscending->setChecked(true);
  }
  if (ref == 1)
  {
    m_OrderDescending->setChecked(true);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::orderingChanged(bool checked)
{
  generateExampleInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::on_m_EndIndex_valueChanged(int value)
{
  generateExampleInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::on_m_StartIndex_valueChanged(int value)
{
  generateExampleInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::on_m_TotalDigits_valueChanged(int value)
{
  generateExampleInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::on_m_FileExt_textChanged(const QString& string)
{
  generateExampleInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::on_m_FileSuffix_textChanged(const QString& string)
{
  generateExampleInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::on_m_FilePrefix_textChanged(const QString& string)
{
  generateExampleInputFile();
  emit parametersChanged();
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::generateExampleInputFile()
{

  QString filename = QString("%1%2%3.%4").arg(m_FilePrefix->text())
                     .arg(m_StartIndex->text(), m_TotalDigits->value(), '0')
                     .arg(m_FileSuffix->text()).arg(m_FileExt->text());
  m_GeneratedFileNameExample->setText(filename);

  int start = m_StartIndex->value();
  int end = m_EndIndex->value();
  bool hasMissingFiles = false;

  // Now generate all the file names the user is asking for and populate the table
  QVector<QString> fileList = FilePathGenerator::GenerateFileList(start, end, hasMissingFiles, m_OrderAscending->isChecked(),
                              m_InputDir->text(),
                              m_FilePrefix->text(),
                              m_FileSuffix->text(),
                              m_FileExt->text(),
                              m_TotalDigits->value());
  m_FileListView->clear();
  QIcon greenDot = QIcon(QString(":/bullet_ball_green.png"));
  QIcon redDot = QIcon(QString(":/bullet_ball_red.png"));
  for(QVector<QString>::size_type i = 0; i < fileList.size(); ++i)
  {
    QString filePath(fileList.at(i));
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
void FileListInfoWidget::findMaxSliceAndPrefix()
{
  if (m_InputDir->text().length() == 0) { return; }
  QDir dir(m_InputDir->text());
#if 0
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
#endif

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
      QString fns = fn;
      int length =  fn.length();
      digitEnd = length - 1;
      while(digitEnd >= 0 && fn[digitEnd] >= '0' && fn[digitEnd] <= '9')
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
      while(digitEnd >= 0 && fn[digitEnd] >= '0' && fn[digitEnd] <= '9')
      {
        ++digitEnd;
      }

      if ( digitEnd - digitStart < minTotalDigits) { minTotalDigits = digitEnd - digitStart; }
      m_TotalDigits->setValue(minTotalDigits);
      if (list.size() > 0)
      {
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
  this->m_StartIndex->setValue(minSlice);
  this->m_EndIndex->setValue(maxSlice);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::widgetChanged(const QString& text)
{
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  if (NULL == filter)
  {
    QString ss = QObject::tr("Error Setting FileListStack Gui values to Filter instance. Filter instance was NULL.").arg(getFilterParameter()->getPropertyName());
    emit errorSettingFilterParameter(ss);
  }
  bool ok = false;

  FileListInfo_t data;
  data.EndIndex = m_EndIndex->text().toLongLong(&ok);
  data.FileExtension = m_FileExt->text();
  data.FilePrefix = m_FilePrefix->text();
  data.FileSuffix = m_FileSuffix->text();
  data.InputPath = m_InputDir->text();
  data.Ordering = getOrdering();
  data.PaddingDigits = m_TotalDigits->value();
  data.StartIndex = m_StartIndex->text().toLongLong(&ok);

  QVariant v;
  v.setValue(data);
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, v);
  if(false == ok)
  {
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(getFilter(), getFilterParameter());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::beforePreflight()
{
  if (m_DidCausePreflight == false)
  {

  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileListInfoWidget::afterPreflight()
{

}

