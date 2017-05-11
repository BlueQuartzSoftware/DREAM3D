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

#include "ImportVectorImageStackWidget.h"

// Needed for AxisAngle_t
#include "EbsdLib/EbsdConstants.h"

//-- Qt Includes
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QFileInfoList>
#include <QtCore/QString>
#include <QtCore/QThread>
#include <QtCore/QUrl>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QListWidgetItem>
#include <QtGui/QPainter>
#include <QtGui/QKeyEvent>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMenu>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Utilities/FilePathGenerator.h"

#include "SVWidgetsLib/QtSupport/QtSFileCompleter.h"
#include "SVWidgetsLib/QtSupport/QtSFileUtils.h"

#include "ImageIO/ImageIOFilters/ImportVectorImageStack.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportVectorImageStackWidget::ImportVectorImageStackWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
, m_StackingGroup(nullptr)
, m_DidCausePreflight(false)
{
  m_Filter = qobject_cast<ImportVectorImageStack*>(filter);
  Q_ASSERT_X(nullptr != m_Filter, "ImportVectorImageStackWidget can ONLY be used with ImportVectorImageStack filter", __FILE__);

  setupUi(this);
  setupGui();

  if(m_LineEdit->text().isEmpty())
  {
    setInputDirectory(QDir::homePath());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportVectorImageStackWidget::~ImportVectorImageStackWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStackWidget::setFilter(AbstractFilter* value)
{
  m_Filter = dynamic_cast<ImportVectorImageStack*>(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter* ImportVectorImageStackWidget::getFilter() const
{
  return m_Filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStackWidget::setWidgetListEnabled(bool b)
{
  foreach(QWidget* w, m_WidgetList)
  {
    w->setEnabled(b);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStackWidget::setupGui()
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
  
  
  {
    QDoubleValidator* validator = new QDoubleValidator(xRes);
    validator->setDecimals(4);
    xRes->setValidator(validator);
  }
  {
    QDoubleValidator* validator = new QDoubleValidator(yRes);
    validator->setDecimals(4);
    yRes->setValidator(validator);
  }
  {
    QDoubleValidator* validator = new QDoubleValidator(zRes);
    validator->setDecimals(4);
    zRes->setValidator(validator);
  }
  {
    QDoubleValidator* validator = new QDoubleValidator(xOrigin);
    validator->setDecimals(4);
    xOrigin->setValidator(validator);
  }
  {
    QDoubleValidator* validator = new QDoubleValidator(yOrigin);
    validator->setDecimals(4);
    yOrigin->setValidator(validator);
  }
  {
    QDoubleValidator* validator = new QDoubleValidator(zOrigin);
    validator->setDecimals(4);
    zOrigin->setValidator(validator);
  }

  m_WidgetList << m_LineEdit << m_InputDirBtn;
  m_WidgetList << m_FileExt << m_ErrorMessage << m_TotalDigits << m_Separator << m_FileSuffix;
  m_WidgetList << m_FilePrefix << m_TotalSlices << m_StartIndex << m_EndIndex << m_StartComp << m_EndComp;
  m_WidgetList << xRes << yRes << zRes;
  m_WidgetList << xOrigin << yOrigin << zOrigin;

  m_ErrorMessage->setVisible(false);

  m_StackingGroup = new QButtonGroup(this);
  m_StackingGroup->addButton(m_StackLowToHigh);
  m_StackingGroup->addButton(m_StackHighToLow);

  connect(m_StackLowToHigh, SIGNAL(toggled(bool)), this, SLOT(stackingOrderChanged(bool)));

  // Manually hook up these signals/slots
  connect(xRes, SIGNAL(textChanged(const QString&)), this, SLOT(resolutionChanged(const QString&)));
  connect(yRes, SIGNAL(textChanged(const QString&)), this, SLOT(resolutionChanged(const QString&)));
  connect(zRes, SIGNAL(textChanged(const QString&)), this, SLOT(resolutionChanged(const QString&)));

  connect(xOrigin, SIGNAL(textChanged(const QString&)), this, SLOT(originChanged(const QString&)));
  connect(yOrigin, SIGNAL(textChanged(const QString&)), this, SLOT(originChanged(const QString&)));
  connect(zOrigin, SIGNAL(textChanged(const QString&)), this, SLOT(originChanged(const QString&)));

  validateInputFile();
  getGuiParametersFromFilter();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStackWidget::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Escape)
  {
    m_LineEdit->setText(m_CurrentText);
    m_LineEdit->setStyleSheet("");
    m_LineEdit->setToolTip("");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStackWidget::setupMenuField()
{
  QFileInfo fi(m_LineEdit->text());

  QMenu* lineEditMenu = new QMenu(m_LineEdit);
  m_LineEdit->setButtonMenu(QtSLineEdit::Left, lineEditMenu);
  QLatin1String iconPath = QLatin1String(":/caret-bottom.png");

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


  if (m_LineEdit->text().isEmpty() == false && fi.exists())
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
void ImportVectorImageStackWidget::getGuiParametersFromFilter()
{
  blockSignals(true);
  m_LineEdit->setText(m_Filter->getInputPath());

  m_StartIndex->setValue(m_Filter->getStartIndex());
  m_EndIndex->setValue(m_Filter->getEndIndex());
  m_StartComp->setValue(m_Filter->getStartComp());
  m_EndComp->setValue(m_Filter->getEndComp());

  setResolutionValues();
  setOriginValues();

  m_FilePrefix->setText(m_Filter->getFilePrefix());
  m_Separator->setText(m_Filter->getSeparator());
  m_FileSuffix->setText(m_Filter->getFileSuffix());
  QString ext = m_Filter->getFileExtension();
  if(ext.isEmpty()) // Default to placing tif as the file extension instead of nothing.
  {
    ext = "tif";
  }
  m_FileExt->setText(ext);
  m_TotalDigits->setValue(m_Filter->getPaddingDigits());

  setRefFrameZDir(m_Filter->getRefFrameZDir());
  blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStackWidget::validateInputFile()
{
  QString currentPath = m_Filter->getInputPath();
  QFileInfo fi(currentPath);
  if(currentPath.isEmpty() == false && fi.exists() == false)
  {
    //    QString Ftype = getFilterParameter()->getFileType();
    //    QString ext = getFilterParameter()->getFileExtension();
    //    QString s = Ftype + QString(" Files (") + ext + QString(");;All Files(*.*)");
    QString defaultName = getInputDirectory();
    QString title = QObject::tr("Select a replacement input file in filter '%2'").arg(m_Filter->getHumanLabel());

    QString file = QFileDialog::getExistingDirectory(this, title, defaultName, QFileDialog::ShowDirsOnly);
    if(true == file.isEmpty())
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
void ImportVectorImageStackWidget::setResolutionValues()
{
  FloatVec3_t data = m_Filter->getResolution();
  xRes->setText(QString::number(data.x));
  yRes->setText(QString::number(data.y));
  zRes->setText(QString::number(data.z));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStackWidget::setOriginValues()
{
  FloatVec3_t data = m_Filter->getOrigin();
  xOrigin->setText(QString::number(data.x));
  yOrigin->setText(QString::number(data.y));
  zOrigin->setText(QString::number(data.z));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStackWidget::resolutionChanged(const QString& string)
{
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStackWidget::originChanged(const QString& string)
{
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStackWidget::checkIOFiles()
{
  if(true == this->verifyPathExists(m_LineEdit->text(), this->m_LineEdit))
  {
    findMaxSliceAndPrefix();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStackWidget::on_m_InputDirBtn_clicked()
{
  // std::cout << "on_angDirBtn_clicked" << std::endl;
  QString outputFile = this->getInputDirectory();

  outputFile = QFileDialog::getExistingDirectory(this, tr("Select Image Directory"), outputFile);
  if(!outputFile.isNull())
  {
    m_LineEdit->blockSignals(true);
    m_LineEdit->setText(QDir::toNativeSeparators(outputFile));
    on_m_LineEdit_textChanged(m_LineEdit->text());
    setInputDirectory(outputFile);
    m_LineEdit->blockSignals(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStackWidget::on_m_LineEdit_textChanged(const QString& text)
{
  if(verifyPathExists(m_LineEdit->text(), m_LineEdit))
  {
    m_ShowFileAction->setEnabled(true);
    findMaxSliceAndPrefix();
    QDir dir(m_LineEdit->text());
    QString dirname = dir.dirName();
    dir.cdUp();

    generateExampleInputFile();
    m_LineEdit->blockSignals(true);
    m_LineEdit->setText(QDir::toNativeSeparators(m_LineEdit->text()));
    m_LineEdit->blockSignals(false);
  }
  else
  {
    m_ShowFileAction->setDisabled(true);
    m_FileListView->clear();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint32_t ImportVectorImageStackWidget::getRefFrameZDir()
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
void ImportVectorImageStackWidget::setRefFrameZDir(uint32_t ref)
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
void ImportVectorImageStackWidget::stackingOrderChanged(bool checked)
{
  generateExampleInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStackWidget::on_m_EndIndex_valueChanged(int value)
{
  generateExampleInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStackWidget::on_m_StartIndex_valueChanged(int value)
{
  generateExampleInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStackWidget::on_m_EndComp_valueChanged(int value)
{
  generateExampleInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStackWidget::on_m_StartComp_valueChanged(int value)
{
  generateExampleInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStackWidget::on_m_TotalDigits_valueChanged(int value)
{
  generateExampleInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStackWidget::on_m_FileExt_textChanged(const QString& string)
{
  generateExampleInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStackWidget::on_m_FileSuffix_textChanged(const QString& string)
{
  generateExampleInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStackWidget::on_m_FilePrefix_textChanged(const QString& string)
{
  generateExampleInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStackWidget::on_m_Separator_textChanged(const QString& string)
{
  generateExampleInputFile();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStackWidget::generateExampleInputFile()
{

  QString filename = QString("%1%2%3%4%5.%6")
                         .arg(m_FilePrefix->text())
                         .arg(m_StartIndex->text(), m_TotalDigits->value(), '0')
                         .arg(m_Separator->text())
                         .arg(m_StartComp->text(), m_TotalDigits->value(), '0')
                         .arg(m_FileSuffix->text())
                         .arg(m_FileExt->text());
  m_GeneratedFileNameExample->setText(filename);

  int start = m_StartIndex->value();
  int end = m_EndIndex->value();
  int cstart = m_StartComp->value();
  int cend = m_EndComp->value();
  bool hasMissingFiles = false;

  // Now generate all the file names the user is asking for and populate the table
  QVector<QString> fileList = FilePathGenerator::GenerateVectorFileList(start, end, cstart, cend, hasMissingFiles, m_StackLowToHigh->isChecked(), m_LineEdit->text(), m_FilePrefix->text(),
                                                                        m_Separator->text(), m_FileSuffix->text(), m_FileExt->text(), m_TotalDigits->value());
  m_FileListView->clear();
  QIcon greenDot = QIcon(QString(":/bullet_ball_green.png"));
  QIcon redDot = QIcon(QString(":/bullet_ball_red.png"));
  for(QVector<QString>::size_type i = 0; i < fileList.size(); ++i)
  {
    QString filePath(fileList.at(i));
    QFileInfo fi(filePath);
    QListWidgetItem* item = new QListWidgetItem(filePath, m_FileListView);
    if(fi.exists() == true)
    {
      item->setIcon(greenDot);
    }
    else
    {
      hasMissingFiles = true;
      item->setIcon(redDot);
    }
  }

  if(hasMissingFiles == true)
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
void ImportVectorImageStackWidget::findMaxSliceAndPrefix()
{
  if(m_LineEdit->text().length() == 0)
  {
    return;
  }
  QDir dir(m_LineEdit->text());

  // Final check to make sure we have a valid file extension
  if(m_FileExt->text().isEmpty() == true)
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
    if(fi.suffix().compare(ext) && fi.isFile() == true)
    {
      pos = 0;
      list.clear();
      QString fn = fi.baseName();
      QString fns = fn;
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
      if(list.size() > 0)
      {
        currValue = list.front().toInt(&ok);
        if(false == flag)
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
  this->m_StartIndex->setValue(minSlice);
  this->m_EndIndex->setValue(maxSlice);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStackWidget::widgetChanged(const QString& text)
{
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStackWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  if(nullptr == filter)
  {
    QString ss = QObject::tr("Error Setting ImportImageStack Gui values to Filter instance. Filter instance was nullptr.").arg(getFilterParameter()->getPropertyName());
    emit errorSettingFilterParameter(ss);
  }

  ImportVectorImageStack* f = qobject_cast<ImportVectorImageStack*>(filter);
  Q_ASSERT_X(nullptr != m_Filter, "ImportVectorImageStackWidget can ONLY be used with ImportVectorImageStack filter", __FILE__);

  bool ok = false;
  f->setInputPath(m_LineEdit->text());
  f->setResolution(getResolutionValues());
  f->setOrigin(getOriginValues());

  f->setFilePrefix(m_FilePrefix->text());
  f->setSeparator(m_Separator->text());
  f->setFileSuffix(m_FileSuffix->text());
  f->setFileExtension(m_FileExt->text());
  f->setStartIndex(m_StartIndex->text().toLongLong(&ok));
  f->setEndIndex(m_EndIndex->text().toLongLong(&ok));
  f->setStartComp(m_StartComp->text().toLongLong(&ok));
  f->setEndComp(m_EndComp->text().toLongLong(&ok));
  f->setPaddingDigits(m_TotalDigits->value());

  f->setRefFrameZDir(getRefFrameZDir());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatVec3_t ImportVectorImageStackWidget::getResolutionValues()
{
  bool ok = false;
  FloatVec3_t data;
  data.x = xRes->text().toDouble(&ok);
  data.y = yRes->text().toDouble(&ok);
  data.z = zRes->text().toDouble(&ok);
  return data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatVec3_t ImportVectorImageStackWidget::getOriginValues()
{
  bool ok = false;
  FloatVec3_t data;
  data.x = xOrigin->text().toDouble(&ok);
  data.y = yOrigin->text().toDouble(&ok);
  data.z = zOrigin->text().toDouble(&ok);
  return data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStackWidget::beforePreflight()
{
  if(m_DidCausePreflight == false)
  {
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStackWidget::afterPreflight()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStackWidget::setInputDirectory(QString val) 
{
  m_LineEdit->setText(val);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportVectorImageStackWidget::getInputDirectory() 
{
  if(m_LineEdit->text().isEmpty())
  {
    return QDir::homePath();
  }
  return m_LineEdit->text();
}
