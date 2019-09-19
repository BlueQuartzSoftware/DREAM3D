/* ============================================================================
 * Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include "EbsdToH5EbsdWidget.h"

//-- Qt Includes
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QFileInfoList>
#include <QtCore/QString>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QMenu>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Utilities/FilePathGenerator.h"
#include "SIMPLib/Utilities/SIMPLDataPathValidator.h"

#include "SVWidgetsLib/QtSupport/QtSCheckboxDialog.h"
#include "SVWidgetsLib/QtSupport/QtSFileCompleter.h"
#include "SVWidgetsLib/QtSupport/QtSFileUtils.h"
#include "SVWidgetsLib/QtSupport/QtSHelpUrlGenerator.h"
#include "SVWidgetsLib/QtSupport/QtSMacros.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"

#include "OrientationAnalysis/OrientationAnalysisFilters/EbsdToH5Ebsd.h"

#include "OrientationAnalysis/Gui/Widgets/QEbsdReferenceFrameDialog.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EbsdToH5EbsdWidget::EbsdToH5EbsdWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
, m_TSLchecked(false)
, m_HKLchecked(false)
, m_HEDMchecked(false)
, m_NoTranschecked(true)
{
  m_SampleTransformation.angle = 0.0f;
  m_SampleTransformation.h = 0.0f;
  m_SampleTransformation.k = 0.0f;
  m_SampleTransformation.l = 0.0f;

  m_EulerTransformation.angle = 0.0f;
  m_EulerTransformation.h = 0.0f;
  m_EulerTransformation.k = 0.0f;
  m_EulerTransformation.l = 0.0f;

  m_Filter = qobject_cast<EbsdToH5Ebsd*>(filter);
  Q_ASSERT_X(nullptr != m_Filter, "EbsdToH5EbsdWidget can ONLY be used with EbsdToH5Ebsd filter", __FILE__);

  setupUi(this);
  setupGui();

  if(m_LineEdit->text().isEmpty())
  {
    setInputDirectory(QDir::homePath());
  }
  if(getOutputPath().isEmpty())
  {
    setOutputPath(QDir::homePath());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EbsdToH5EbsdWidget::~EbsdToH5EbsdWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::setFilter(AbstractFilter* value)
{
  m_Filter = dynamic_cast<EbsdToH5Ebsd*>(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter* EbsdToH5EbsdWidget::getFilter() const
{
  return m_Filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::setWidgetListEnabled(bool b)
{
  foreach(QWidget* w, m_WidgetList)
  {
    w->setEnabled(b);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::setupGui()
{

  // Catch when the filter is about to execute the preflight
  connect(m_Filter, SIGNAL(preflightAboutToExecute()), this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(m_Filter, SIGNAL(preflightExecuted()), this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(m_Filter, SIGNAL(updateFilterParameters(AbstractFilter*)), this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  QtSFileCompleter* com = new QtSFileCompleter(this, true);
  m_LineEdit->setCompleter(com);
  QObject::connect(com, SIGNAL(activated(const QString&)), this, SLOT(on_m_LineEdit_textChanged(const QString&)));

  setupMenuField();

  inputAbsPathLabel->hide();
  outputAbsPathLabel->hide();

  QtSFileCompleter* com1 = new QtSFileCompleter(this, false);
  m_OutputFile->setCompleter(com1);
  QObject::connect(com1, SIGNAL(activated(const QString&)), this, SLOT(on_m_OutputFile_textChanged(const QString&)));

  // Update the widget when the data directory changes
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  connect(validator, &SIMPLDataPathValidator::dataDirectoryChanged, [=] {
    blockSignals(true);
    on_m_LineEdit_textChanged(m_LineEdit->text());
    on_m_OutputFile_textChanged(m_OutputFile->text());
    blockSignals(false);

    emit parametersChanged();
  });

  m_WidgetList << m_LineEdit << m_InputDirBtn << m_OutputFile << m_OutputFileBtn;
  m_WidgetList << m_FileExt << m_ErrorMessage << m_TotalDigits;
  m_WidgetList << m_FilePrefix << m_TotalSlices << m_ZStartIndex << m_ZEndIndex << m_zSpacing;
  m_ErrorMessage->setVisible(false);

  m_StackingGroup = new QButtonGroup(this);
  m_StackingGroup->addButton(m_StackLowToHigh);
  m_StackingGroup->addButton(m_StackHighToLow);

  m_OriginGroup = new QButtonGroup;
  m_OriginGroup->addButton(m_RefFrameOptionsBtn);

  connect(m_StackLowToHigh, SIGNAL(toggled(bool)), this, SLOT(stackingOrderChanged(bool)));

  m_RefFrameOptionsBtn->setEnabled(false);

//  validateInputFile();
  getGuiParametersFromFilter();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::keyPressEvent(QKeyEvent* event)
{
  if(event->key() == Qt::Key_Escape)
  {
    m_LineEdit->setText(m_CurrentText);
    m_LineEdit->setToolTip("");
    setValidFilePath(m_LineEdit->text());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::setupMenuField()
{
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(m_LineEdit->text());

  QFileInfo fi(inputPath);

  QMenu* lineEditMenu = new QMenu(m_LineEdit);
  m_LineEdit->setButtonMenu(QtSLineEdit::Left, lineEditMenu);
  QLatin1String iconPath = QLatin1String(":/SIMPL/icons/images/caret-bottom.png");

  m_LineEdit->setButtonVisible(QtSLineEdit::Left, true);

  QPixmap pixmap(8, 8);
  pixmap.fill(Qt::transparent);
  QPainter painter(&pixmap);
  const QPixmap mag = QPixmap(iconPath);
  painter.drawPixmap(0, (pixmap.height() - mag.height()) / 2, mag);
  m_LineEdit->setButtonPixmap(QtSLineEdit::Left, pixmap);

  {
    m_ShowFileAction = new QAction(lineEditMenu);
    m_ShowFileAction->setObjectName(QString::fromUtf8("showFileAction"));
#if defined(Q_OS_WIN)
    m_ShowFileAction->setText("Show in Windows Explorer");
#elif defined(Q_OS_MAC)
    m_ShowFileAction->setText("Show in Finder");
#else
    m_ShowFileAction->setText("Show in File System");
#endif
    lineEditMenu->addAction(m_ShowFileAction);
    connect(m_ShowFileAction, SIGNAL(triggered()), this, SLOT(showFileInFileSystem()));
  }

  if(!m_LineEdit->text().isEmpty() && fi.exists())
  {
    m_ShowFileAction->setEnabled(true);
  }
  else
  {
    m_ShowFileAction->setDisabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::getGuiParametersFromFilter()
{

  QString inputPath = m_Filter->getInputPath();
  if(inputPath.isEmpty())
  {
    inputPath = QDir::homePath();
  }
  m_LineEdit->setText(inputPath);
  setValidFilePath(m_LineEdit->text());

  m_OutputFile->setText(m_Filter->getOutputFile());

  QObjectList obs = children();
  foreach(QObject* ob, obs)
  {
    ob->blockSignals(true);
  }

  m_ZStartIndex->setValue(m_Filter->getZStartIndex());
  m_ZEndIndex->setValue(m_Filter->getZEndIndex());
  m_zSpacing->setText(QString::number(m_Filter->getZResolution()));
  setRefFrameZDir(m_Filter->getRefFrameZDir());

  m_FilePrefix->setText(m_Filter->getFilePrefix());
  m_FileSuffix->setText(m_Filter->getFileSuffix());
  m_FileExt->setText(m_Filter->getFileExtension());
  m_TotalDigits->setValue(m_Filter->getPaddingDigits());

  m_SampleTransformation = m_Filter->getSampleTransformation();
  m_EulerTransformation = m_Filter->getEulerTransformation();

  foreach(QObject* ob, obs)
  {
    ob->blockSignals(false);
  }

  generateExampleEbsdInputFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::validateInputFile()
{
  QString currentPath = m_Filter->getInputPath();
  QFileInfo fi(currentPath);
  if(!currentPath.isEmpty() && !fi.exists())
  {
    QString defaultName = getInputDirectory();
    if(!m_LineEdit->text().isEmpty())
    {
      defaultName = m_LineEdit->text();
    }

    QString title = QObject::tr("Select a replacement input file in filter '%2'").arg(m_Filter->getHumanLabel());

    QString file = QFileDialog::getExistingDirectory(this, title, defaultName, QFileDialog::ShowDirsOnly);
    if(file.isEmpty())
    {
      file = currentPath;
    }
    file = QDir::toNativeSeparators(file);
    // Store the last used directory into the private instance variable
    QFileInfo fi(file);
    setInputDirectory(fi.path());
    m_Filter->setInputPath(file);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::on_m_OutputFile_textChanged(const QString& text)
{
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString outputPath = validator->convertToAbsolutePath(text);

  QFileInfo fi(text);
  if (fi.isRelative())
  {
    outputAbsPathLabel->setText(outputPath);
    outputAbsPathLabel->show();
  }
  else
  {
    outputAbsPathLabel->hide();
  }

  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::checkIOFiles()
{
  if(QtSFileUtils::VerifyPathExists(m_LineEdit->text(), this->m_LineEdit))
  {
    findEbsdMaxSliceAndPrefix();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::on_m_OutputFileBtn_clicked()
{
  QString defaultName = getOutputPath();
  QString file = QFileDialog::getSaveFileName(this, tr("Save HDF5 EBSD File"), defaultName, tr("HDF5 EBSD Files (*.h5ebsd)"));
  if(file.isEmpty())
  {
    return;
  }
  QFileInfo fi(file);
  m_OutputFile->setText(fi.absoluteFilePath());
  setOutputPath(file);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::on_m_InputDirBtn_clicked()
{
  // std::cout << "on_angDirBtn_clicked" << std::endl;
  QString outputFile = this->getInputDirectory() + QDir::separator();
  outputFile = QFileDialog::getExistingDirectory(this, tr("Select EBSD Directory"), outputFile);
  if(!outputFile.isNull())
  {
    m_LineEdit->blockSignals(true);
    m_LineEdit->setText(QDir::toNativeSeparators(outputFile));
    setValidFilePath(m_LineEdit->text());
    on_m_LineEdit_textChanged(m_LineEdit->text());
    setInputDirectory(outputFile);
    m_LineEdit->blockSignals(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::on_m_LineEdit_textChanged(const QString& text)
{
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(text);

  QFileInfo fi(text);
  if (fi.isRelative())
  {
    inputAbsPathLabel->setText(inputPath);
    inputAbsPathLabel->show();
  }
  else
  {
    inputAbsPathLabel->hide();
  }

  if(QtSFileUtils::VerifyPathExists(inputPath, m_LineEdit))
  {
    m_ShowFileAction->setEnabled(true);
    m_RefFrameOptionsBtn->setEnabled(true);
    findEbsdMaxSliceAndPrefix();
    QDir dir(inputPath);
    dir.cdUp();

    generateExampleEbsdInputFile();
    m_LineEdit->blockSignals(true);
    m_LineEdit->setText(QDir::toNativeSeparators(m_LineEdit->text()));
    setValidFilePath(m_LineEdit->text());
    m_LineEdit->blockSignals(false);
    referenceFrameCheck->setText("Have you set the Reference Frame?");
  }
  else
  {
    m_ShowFileAction->setDisabled(true);
    m_FileListView->clear();
  }
  m_fileExtUsedForTransformInit = false;

  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint32_t EbsdToH5EbsdWidget::getRefFrameZDir()
{
  if(m_StackLowToHigh->isChecked())
  {
    return SIMPL::RefFrameZDir::LowtoHigh;
  }
  if(m_StackHighToLow->isChecked())
  {
    return SIMPL::RefFrameZDir::HightoLow;
  }
  return SIMPL::RefFrameZDir::UnknownRefFrameZDirection;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::setRefFrameZDir(uint32_t ref)
{
  if(ref == SIMPL::RefFrameZDir::LowtoHigh)
  {
    m_StackLowToHigh->setChecked(true);
  }
  if(ref == SIMPL::RefFrameZDir::HightoLow)
  {
    m_StackHighToLow->setChecked(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::on_m_zSpacing_textChanged(const QString& string)
{
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::stackingOrderChanged(bool checked)
{
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::on_m_ZEndIndex_valueChanged(int value)
{
  generateExampleEbsdInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::on_m_ZStartIndex_valueChanged(int value)
{
  generateExampleEbsdInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::on_m_TotalDigits_valueChanged(int value)
{
  generateExampleEbsdInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::on_m_FileExt_textChanged(const QString& string)
{
  generateExampleEbsdInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::on_m_FileSuffix_textChanged(const QString& string)
{
  generateExampleEbsdInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::on_m_FilePrefix_textChanged(const QString& string)
{
  generateExampleEbsdInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::generateExampleEbsdInputFile()
{
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(m_LineEdit->text());

  QString filename = QString("%1%2%3.%4").arg(m_FilePrefix->text()).arg(m_ZStartIndex->text(), m_TotalDigits->value(), '0').arg(m_FileSuffix->text()).arg(m_FileExt->text());
  m_GeneratedFileNameExample->setText(filename);

  int start = m_ZStartIndex->value();
  int end = m_ZEndIndex->value();
  bool hasMissingFiles = false;
  int increment = 1;

  // Now generate all the file names the user is asking for and populate the table
  QVector<QString> fileList = FilePathGenerator::GenerateFileList(start, end, increment, hasMissingFiles, m_StackLowToHigh->isChecked(), inputPath, m_FilePrefix->text(), m_FileSuffix->text(),
                                                                  m_FileExt->text(), m_TotalDigits->value());
  m_FileListView->clear();
  QIcon greenDot = QIcon(QString(":/SIMPL/icons/images/bullet_ball_green.png"));
  QIcon redDot = QIcon(QString(":/SIMPL/icons/images/bullet_ball_red.png"));

  for(const auto& filePath : fileList)
  {
    QFileInfo fi(filePath);
    QListWidgetItem* item = new QListWidgetItem(filePath, m_FileListView);
    if(fi.exists())
    {
      item->setIcon(greenDot);
    }
    else
    {
      hasMissingFiles = true;
      item->setIcon(redDot);
    }
  }

  if(hasMissingFiles)
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
void EbsdToH5EbsdWidget::on_m_RefFrameOptionsBtn_clicked()
{
  //referenceFrameCheck->setStyleSheet(QString(""));
  referenceFrameCheck->setText("");

  int start = m_ZStartIndex->value();
  int end = m_ZEndIndex->value();
  bool hasMissingFiles = false;
  int increment = 1;

  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(m_LineEdit->text());

  // Now generate all the file names the user is asking for and populate the table
  QVector<QString> fileList = FilePathGenerator::GenerateFileList(start, end, increment, hasMissingFiles, m_StackLowToHigh->isChecked(), inputPath, m_FilePrefix->text(), m_FileSuffix->text(),
                                                                  m_FileExt->text(), m_TotalDigits->value());
  if(fileList.empty())
  {
    return;
  }
  QString ebsdFileName = (fileList[0]);

  QFileInfo fi(ebsdFileName);
  if(fi.suffix() == "ctf")
  {
  }

  identifyRefFrame();

  QEbsdReferenceFrameDialog d(ebsdFileName, this);
  d.setEbsdFileName(ebsdFileName);

  if(!m_fileExtUsedForTransformInit)
  {
    if(fi.suffix() == "ctf")
    {
      m_HKLchecked = true;
      m_NoTranschecked = false;
    }
    if(fi.suffix() == "ang")
    {
      m_TSLchecked = true;
      m_NoTranschecked = false;
    }
    if(fi.suffix() == "mic")
    {
      m_HEDMchecked = true;
      m_NoTranschecked = false;
    }
    m_fileExtUsedForTransformInit = true;
  }

  d.setTSLDefault(m_TSLchecked);
  d.setHKLDefault(m_HKLchecked);
  d.setHEDMDefault(m_HEDMchecked);
  d.setNoTrans(m_NoTranschecked);
  int ret = d.exec();
  if(ret == QDialog::Accepted)
  {
    m_TSLchecked = d.getTSLchecked();
    m_HKLchecked = d.getHKLchecked();
    m_HEDMchecked = d.getHEDMchecked();
    m_NoTranschecked = d.getNoTranschecked();
    d.getSampleTranformation(m_SampleTransformation);
    d.getEulerTranformation(m_EulerTransformation);
    emit parametersChanged(); // emit to let the system know to preflight
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::identifyRefFrame()
{
  m_TSLchecked = false;
  m_HKLchecked = false;
  m_NoTranschecked = true;
  m_HEDMchecked = false;

  // TSL/EDAX
  if(m_SampleTransformation.angle == 180.0f && m_SampleTransformation.h == 0.0f && m_SampleTransformation.k == 1.0f && m_SampleTransformation.l == 0.0f

     && m_EulerTransformation.angle == 90.0f && m_EulerTransformation.h == 0.0f && m_EulerTransformation.k == 0.0f && m_EulerTransformation.l == 1.0f)

  {
    m_TSLchecked = true;
    m_NoTranschecked = false;
  }
  else if(m_SampleTransformation.angle == 180.0f // HKL
          && m_SampleTransformation.h == 0.0f && m_SampleTransformation.k == 1.0f && m_SampleTransformation.l == 0.0f

          && m_EulerTransformation.angle == 0.0f && m_EulerTransformation.h == 0.0f && m_EulerTransformation.k == 0.0f && m_EulerTransformation.l == 1.0f)

  {
    m_HKLchecked = true;
    m_NoTranschecked = false;
  }
  else if(m_SampleTransformation.angle == 0.0f // HEDM
          && m_SampleTransformation.h == 0.0f && m_SampleTransformation.k == 0.0f && m_SampleTransformation.l == 1.0f

          && m_EulerTransformation.angle == 0.0f && m_EulerTransformation.h == 0.0f && m_EulerTransformation.k == 0.0f && m_EulerTransformation.l == 1.0f)

  {
    m_HEDMchecked = true;
    m_NoTranschecked = false;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::findEbsdMaxSliceAndPrefix()
{
  if(m_LineEdit->text().length() == 0)
  {
    return;
  }

  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(m_LineEdit->text());

  QDir dir(inputPath);
  m_FileExt->setText("");
  {
    QString ext = ".ang";
    QStringList filters;
    filters << "*" + ext;
    dir.setNameFilters(filters);
    QFileInfoList angList = dir.entryInfoList();
    if(!angList.empty())
    {
      m_FileExt->setText("ang");
    }
  }
  if(m_FileExt->text().isEmpty())
  {
    QString ext = ".ctf";
    QStringList filters;
    filters << "*" + ext;
    dir.setNameFilters(filters);
    QFileInfoList angList = dir.entryInfoList();
    if(!angList.empty())
    {
      m_FileExt->setText("ctf");
    }
  }
  // Add in more file formats to look for here

  // Final check to make sure we have a valid file extension
  if(m_FileExt->text().isEmpty())
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
    if((fi.suffix().compare(ext) != 0) && fi.isFile())
    {
      pos = 0;
      list.clear();
      QString fn = fi.baseName();
      int length = fn.length();
      digitEnd = length - 1;
      while(digitEnd >= 0 && fn[digitEnd] >= '0' && fn[digitEnd] <= '9')
      {
        --digitEnd;
      }
      pos = digitEnd;

      digitStart = pos = rx.indexIn(fn, pos);
      digitEnd = digitStart;
      while((pos = rx.indexIn(fn, pos)) != -1)
      {
        list << rx.cap(0);
        fPrefix = fn.left(pos);
        pos += rx.matchedLength();
      }
      while(digitEnd >= 0 && fn[digitEnd] >= '0' && fn[digitEnd] <= '9')
      {
        ++digitEnd;
      }

      if(digitEnd - digitStart < minTotalDigits)
      {
        minTotalDigits = digitEnd - digitStart;
      }
      m_TotalDigits->setValue(minTotalDigits);
      if(!list.empty())
      {
        currValue = list.front().toInt(&ok);
        if(!flag)
        {
          minSlice = currValue;
          flag = true;
        }
        if(currValue > maxSlice)
        {
          maxSlice = currValue;
        }
        if(currValue < minSlice)
        {
          minSlice = currValue;
        }
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
void EbsdToH5EbsdWidget::widgetChanged(const QString& text)
{
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  if(nullptr == filter)
  {
    QString ss = QObject::tr("Error Setting EbsdToH5Ebsd Gui values to Filter instance. Filter instance was nullptr.").arg(getFilterParameter()->getPropertyName());
    emit errorSettingFilterParameter(ss);
  }

  EbsdToH5Ebsd* ebsdConverter = qobject_cast<EbsdToH5Ebsd*>(filter);
  Q_ASSERT_X(nullptr != ebsdConverter, "EbsdToH5EbsdWidget can ONLY be used with EbsdToH5Ebsd filter", __FILE__);

  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();

  bool ok = false;

  QString outputPath = m_OutputFile->text();
  outputPath = validator->convertToAbsolutePath(outputPath);

  ebsdConverter->setOutputFile(outputPath);
  ebsdConverter->setZStartIndex(m_ZStartIndex->text().toLongLong(&ok));
  ebsdConverter->setZEndIndex(m_ZEndIndex->text().toLongLong(&ok));
  ebsdConverter->setZResolution(m_zSpacing->text().toDouble(&ok));
  ebsdConverter->setRefFrameZDir(getRefFrameZDir());


  QString inputPath = m_LineEdit->text();
  inputPath = validator->convertToAbsolutePath(inputPath);

  ebsdConverter->setInputPath(inputPath);
  ebsdConverter->setFilePrefix(m_FilePrefix->text());
  ebsdConverter->setFileSuffix(m_FileSuffix->text());
  ebsdConverter->setFileExtension(m_FileExt->text());
  ebsdConverter->setPaddingDigits(m_TotalDigits->value());

  ebsdConverter->setSampleTransformation(m_SampleTransformation);
  ebsdConverter->setEulerTransformation(m_EulerTransformation);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::beforePreflight()
{
  if(!m_DidCausePreflight)
  {
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::afterPreflight()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EbsdToH5EbsdWidget::getInputDirectory()
{
  if(m_LineEdit->text().isEmpty())
  {
    return QDir::homePath();
  }
  return m_LineEdit->text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::setOutputPath(const QString& val)
{
  m_OutputFile->setText(val);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EbsdToH5EbsdWidget::getOutputPath()
{
  if(m_OutputFile->text().isEmpty())
  {
    return QDir::homePath();
  }
  return m_OutputFile->text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::setInputDirectory(const QString &val)
{
  m_LineEdit->setText(val);
  setValidFilePath(m_LineEdit->text());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdWidget::showFileInFileSystem()
{
  QtSFileUtils::VerifyPathExists(m_LineEdit->text(), m_LineEdit); // This basically sets an internal variable of the superclass.
  FilterParameterWidget::showFileInFileSystem();
#if 0
  QFileInfo fi(m_CurrentlyValidPath);
  QString path;
  if (fi.isFile())
  {
    path = fi.absoluteFilePath();
  }
  else
  {
    path = fi.absolutePath();
  }

  QtSFileUtils::ShowPathInGui(this, fi.absoluteFilePath());
#endif
}
