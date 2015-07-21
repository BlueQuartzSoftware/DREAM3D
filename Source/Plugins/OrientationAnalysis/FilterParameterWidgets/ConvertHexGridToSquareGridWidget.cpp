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


#include "ConvertHexGridToSquareGridWidget.h"

//-- Qt Includes
#include <QtCore/QDir>
#include <QtWidgets/QFileDialog>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Utilities/FilePathGenerator.h"
#include "DREAM3DLib/Geometry/ImageGeom.h"


#include "QtSupportLib/QFileCompleter.h"


#include "OrientationAnalysis/OrientationAnalysisFilters/ConvertHexGridToSquareGrid.h"
#include "OrientationAnalysis/FilterParameters/ConvertHexGridToSquareGridFilterParameter.h"

// Initialize private static member variable
QString ConvertHexGridToSquareGridWidget::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertHexGridToSquareGridWidget::ConvertHexGridToSquareGridWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent),
  m_StackingGroup(NULL),
  m_DidCausePreflight(false)
{
  m_Filter = qobject_cast<ConvertHexGridToSquareGrid*>(filter);
  Q_ASSERT_X(NULL != m_Filter, "ConvertHexGridToSquareGridWidget can ONLY be used with ConvertHexGridToSquareGridWidget filter", __FILE__);

  m_FilterParameter = dynamic_cast<ConvertHexGridToSquareGridFilterParameter*>(parameter);

  if ( getOpenDialogLastDirectory().isEmpty() )
  {
    setOpenDialogLastDirectory( QDir::homePath() );
  }
  setupUi(this);
  setupGui();
//  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertHexGridToSquareGridWidget::~ConvertHexGridToSquareGridWidget()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGridWidget::setFilter(AbstractFilter* value)
{
  m_Filter = dynamic_cast<ConvertHexGridToSquareGrid*>(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter* ConvertHexGridToSquareGridWidget::getFilter() const
{
  return m_Filter;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGridWidget::setFilterParameter(FilterParameter* value)
{
  m_FilterParameter = dynamic_cast<ConvertHexGridToSquareGridFilterParameter*>(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter* ConvertHexGridToSquareGridWidget::getFilterParameter() const
{
  return m_FilterParameter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGridWidget::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList)
  {
    w->setEnabled(b);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGridWidget::setupGui()
{


  // Catch when the filter is about to execute the preflight
  connect(m_Filter, SIGNAL(preflightAboutToExecute()),
          this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(m_Filter, SIGNAL(preflightExecuted()),
          this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(m_Filter, SIGNAL(updateFilterParameters(AbstractFilter*)),
          this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  QFileCompleter* com = new QFileCompleter(this, true);
  m_InputDir->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString&)),
                    this, SLOT(on_m_InputDir_textChanged(const QString&)));
  m_OutputDir->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString&)),
                    this, SLOT(on_m_OutputDir_textChanged(const QString&)));

  {
    QDoubleValidator* validator = new QDoubleValidator(m_xSpacing);
    validator->setDecimals(4);
    m_xSpacing->setValidator(validator);
  }
  {
    QDoubleValidator* validator = new QDoubleValidator(m_ySpacing);
    validator->setDecimals(4);
    m_ySpacing->setValidator(validator);
  }

  m_WidgetList << m_InputDir << m_InputDirBtn;
  m_WidgetList << m_FileExt << m_ErrorMessage << m_TotalDigits;
  m_WidgetList << m_FilePrefix << m_TotalSlices << m_ZStartIndex << m_ZEndIndex;
  m_WidgetList << m_xSpacing << m_ySpacing;

  m_ErrorMessage->setVisible(false);

  // Manually hook up these signals/slots
  connect(m_xSpacing, SIGNAL(textChanged(const QString&)),
          this, SLOT(resolutionChanged(const QString&)));
  connect(m_ySpacing, SIGNAL(textChanged(const QString&)),
          this, SLOT(resolutionChanged(const QString&)));


  getGuiParametersFromFilter();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGridWidget::getGuiParametersFromFilter()
{

  m_InputDir->setText(m_Filter->getInputPath());

  QObjectList obs = children();
  foreach(QObject* ob, obs)
  {
    ob->blockSignals(true);
  }

  m_ZStartIndex->setValue( m_Filter->getZStartIndex() );
  m_ZEndIndex->setValue( m_Filter->getZEndIndex() );

  m_xSpacing->setText(QString::number(m_Filter->getXResolution()) );
  m_ySpacing->setText(QString::number(m_Filter->getYResolution()) );

  m_FilePrefix->setText(m_Filter->getFilePrefix());
  m_FileSuffix->setText(m_Filter->getFileSuffix());
  QString ext = m_Filter->getFileExtension();
  if(ext.isEmpty()) // Default to placing tif as the file extension instead of nothing.
  {
    ext = "ang";
  }
  m_FileExt->setText(ext);
  m_TotalDigits->setValue(m_Filter->getPaddingDigits());
  m_OutputDir->setText(m_Filter->getOutputPath());
  m_OutputPrefix->setText(m_Filter->getOutputPrefix());

  foreach(QObject* ob, obs)
  {
    ob->blockSignals(false);
  }

  validateInputFile();
  generateExampleInputFile();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGridWidget::validateInputFile()
{
  QString currentPath = m_Filter->getInputPath();
  QFileInfo fi(currentPath);
  if (currentPath.isEmpty() == false && fi.exists() == false)
  {
//    QString Ftype = m_FilterParameter->getFileType();
//    QString ext = m_FilterParameter->getFileExtension();
//    QString s = Ftype + QString(" Files (") + ext + QString(");;All Files(*.*)");
    QString defaultName = m_OpenDialogLastDirectory;


    QString title = QObject::tr("Select a replacement input file in filter '%2'").arg(m_Filter->getHumanLabel());

    QString file = QFileDialog::getExistingDirectory(this, title, defaultName, QFileDialog::ShowDirsOnly);
    if(true == file.isEmpty())
    {
      file = currentPath;
    }
    file = QDir::toNativeSeparators(file);
    // Store the last used directory into the private instance variable
    QFileInfo fi(file);
    m_OpenDialogLastDirectory = fi.path();
    m_Filter->setInputPath(file);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGridWidget::resolutionChanged(const QString& string)
{
  emit parametersChanged();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ConvertHexGridToSquareGridWidget::verifyPathExists(QString outFilePath, QLineEdit* lineEdit)
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
void ConvertHexGridToSquareGridWidget::checkIOFiles()
{
  if (true == this->verifyPathExists(m_InputDir->text(), this->m_InputDir))
  {
    findMaxSliceAndPrefix();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGridWidget::on_m_InputDirBtn_clicked()
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
void ConvertHexGridToSquareGridWidget::on_m_InputDir_textChanged(const QString& text)
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
void ConvertHexGridToSquareGridWidget::on_m_OutputDirBtn_clicked()
{
  QString currentPath = m_Filter->getOutputPath();
  if(currentPath.isEmpty() == true)
  {
    currentPath = m_OpenDialogLastDirectory;
  }
  QString Ftype = m_FilterParameter->getFileType();
  QString ext = m_FilterParameter->getFileExtension();
  QString s = Ftype + QString(" Files (") + ext + QString(");;All Files(*.*)");
  QString defaultName = currentPath + QDir::separator() + "Untitled";
  QString file = QFileDialog::getExistingDirectory(this,
                                                   tr("Select Output Folder"),
                                                   defaultName,
                                                   QFileDialog::ShowDirsOnly);

  if(true == file.isEmpty())
  {
    return;
  }
  //  bool ok = false;
  file = QDir::toNativeSeparators(file);
  // Store the last used directory into the private instance variable
  QFileInfo fi(file);
  m_OpenDialogLastDirectory = fi.path();
  m_OutputDir->setText(file);

 // on_m_OutputDir_textChanged(file);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGridWidget::on_m_OutputDir_textChanged(const QString& text)
{
  //if (verifyPathExists(text, m_OutputFile) == true )
  {
    QFileInfo fi(text);
    setOpenDialogLastDirectory( fi.path() );
  }
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGridWidget::on_m_OutputPrefix_textChanged(const QString& text)
{
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGridWidget::on_m_ZEndIndex_valueChanged(int value)
{
  generateExampleInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGridWidget::on_m_ZStartIndex_valueChanged(int value)
{
  generateExampleInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGridWidget::on_m_TotalDigits_valueChanged(int value)
{
  generateExampleInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGridWidget::on_m_FileExt_textChanged(const QString& string)
{
  generateExampleInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGridWidget::on_m_FileSuffix_textChanged(const QString& string)
{
  generateExampleInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGridWidget::on_m_FilePrefix_textChanged(const QString& string)
{
  generateExampleInputFile();
  emit parametersChanged();
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGridWidget::generateExampleInputFile()
{

  QString filename = QString("%1%2%3.%4").arg(m_FilePrefix->text())
                     .arg(m_ZStartIndex->text(), m_TotalDigits->value(), '0')
                     .arg(m_FileSuffix->text()).arg(m_FileExt->text());
  m_GeneratedFileNameExample->setText(filename);

  int start = m_ZStartIndex->value();
  int end = m_ZEndIndex->value();
  bool hasMissingFiles = false;

  // Now generate all the file names the user is asking for and populate the table
  QVector<QString> fileList = FilePathGenerator::GenerateFileList(start, end, hasMissingFiles, true,
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
void ConvertHexGridToSquareGridWidget::findMaxSliceAndPrefix()
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
// this->m_FilePrefix->setText(fPrefix);
  this->m_ZStartIndex->setValue(minSlice);
  this->m_ZEndIndex->setValue(maxSlice);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGridWidget::widgetChanged(const QString& text)
{
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGridWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  if (NULL == filter)
  {
    QString ss = QObject::tr("Error Setting ConvertHexGridToSquareGrid Gui values to Filter instance. Filter instance was NULL.").arg(m_FilterParameter->getPropertyName());
    emit errorSettingFilterParameter(ss);
  }

  ConvertHexGridToSquareGrid* f = qobject_cast<ConvertHexGridToSquareGrid*>(filter);
  Q_ASSERT_X(NULL != m_Filter, "ConvertHexGridToSquareGridWidget can ONLY be used with ConvertHexGridToSquareGrid filter", __FILE__);

  bool ok = false;
  f->setInputPath(m_InputDir->text());
  f->setOutputPath(m_OutputDir->text());
  f->setOutputPrefix(m_OutputPrefix->text());
  f->setXResolution(m_xSpacing->text().toDouble(&ok));
  f->setYResolution(m_ySpacing->text().toDouble(&ok));

  f->setFilePrefix(m_FilePrefix->text());
  f->setFileSuffix(m_FileSuffix->text());
  f->setFileExtension(m_FileExt->text());
  f->setZStartIndex(m_ZStartIndex->text().toLongLong(&ok));
  f->setZEndIndex(m_ZEndIndex->text().toLongLong(&ok));
  f->setPaddingDigits(m_TotalDigits->value());

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGridWidget::beforePreflight()
{
  if (m_DidCausePreflight == false)
  {

  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGridWidget::afterPreflight()
{

}

