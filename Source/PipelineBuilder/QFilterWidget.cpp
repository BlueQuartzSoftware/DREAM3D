/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "QFilterWidget.h"



#include <QtCore/QTimer>
#include <QtCore/QResource>

#include <QtGui/QFrame>
#include <QtGui/QSpinBox>
#include <QtGui/QLabel>
#include <QtGui/QCheckBox>
#include <QtGui/QLineEdit>
#include <QtGui/QIntValidator>
#include <QtGui/QDoubleValidator>
#include <QtGui/QComboBox>
#include <QtGui/QApplication>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QFormLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QPainter>
#include <QtGui/QPushButton>
#include <QtGui/QFileDialog>
#include <QtGui/QMouseEvent>


#include "QtSupport/QR3DFileCompleter.h"
#include "QtSupport/QFSDropLineEdit.h"
#include "DREAM3DLib/Common/FilterParameter.h"

#include "PipelineArraySelectionWidget.h"

#define PADDING 5
#define BORDER 2
#define IMAGE_WIDTH 17
#define IMAGE_HEIGHT 17


// Initialize private static member variable
QString QFilterWidget::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterWidget::QFilterWidget(QWidget* parent) :
      QGroupBox(parent),
      m_CurrentBorderColorFactor(0),
      m_BorderIncrement(16),
      m_IsSelected(false),
      m_HasPreflightErrors(false),
      m_HasPreflightWarnings(false)
{

  if ( m_OpenDialogLastDirectory.isEmpty() )
  {
    m_OpenDialogLastDirectory = QDir::homePath();
  }

  m_DeleteRect.setX(PADDING + BORDER);
  m_DeleteRect.setY(PADDING + BORDER);
  m_DeleteRect.setWidth(IMAGE_WIDTH);
  m_DeleteRect.setHeight(IMAGE_HEIGHT);
  m_timer = new QTimer(this);
  connect(m_timer, SIGNAL(timeout()), this, SLOT(changeStyle()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterWidget::~QFilterWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QFilterWidget::getFilterGroup()
{
    return QString::fromStdString(DREAM3D::FilterGroups::GenericFilters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterWidget* QFilterWidget::createDeepCopy()
{
  QFilterWidget* widget = new QFilterWidget();
  return widget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::setBorderColorStyle(QString s)
{
  m_BorderColorStyle = s;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QFilterWidget::getBorderColorStyle()
{
  return m_BorderColorStyle;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::setHasPreflightErrors(bool hasErrors)
{
  m_HasPreflightErrors = hasErrors;
  if (m_HasPreflightErrors == true)
  {
//    m_timer->start(100);
//    m_CurrentBorderColorFactor = 64;
//    m_BorderIncrement = 16;
  }
  else
  {
    m_timer->stop();
  }
  changeStyle();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::setHasPreflightWarnings(bool hasWarnings)
{
  m_HasPreflightWarnings = hasWarnings;
  if (m_HasPreflightWarnings == true)
  {
//    m_timer->start(100);
//    m_CurrentBorderColorFactor = 64;
//    m_BorderIncrement = 16;
  }
  else
  {
    m_timer->stop();
  }
  changeStyle();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::setIsSelected(bool b)
{
  m_IsSelected = b;
  changeStyle();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QFilterWidget::isSelected()
{
  return m_IsSelected;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::changeStyle()
{
  QString style;
  if (m_HasPreflightErrors == true)
  {
    m_CurrentBorderColorFactor += m_BorderIncrement;
    if (m_CurrentBorderColorFactor > 127)
    {
      m_BorderIncrement = -16;
    }
    if (m_CurrentBorderColorFactor < 1)
    {
      m_BorderIncrement = 16;
    }

    style.append("border: 2px solid rgb(");
    style.append(QString::number(255 - m_CurrentBorderColorFactor, 10));
    style.append(", ");
    style.append(QString::number(m_CurrentBorderColorFactor, 10));
    style.append(", ");
    style.append(QString::number(m_CurrentBorderColorFactor, 10));
    style.append(");");
  }
  else if(m_HasPreflightWarnings)
  {
    style.append("border: 2px solid rgb(172, 168, 0);");
  }
  else if(m_IsSelected == true )
  {
    style.append("border: 2px solid purple;");
    m_CurrentBorderColorFactor = 0;
  }
  else
  {
    style.append("border: 1px solid #515151;");
    style.append("margin: 1px;");
    m_CurrentBorderColorFactor = 0;
  }
  setBorderColorStyle(style);
  updateWidgetStyle();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateWidgetStyle()
{
  QString style;

  style.append("QGroupBox{\n");

  style.append("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #FFFFFF, stop: 1 #CCCCCC);\n");

#if 0
  QString headerFile(":/QFilterWidgetHeaders/");

  headerFile.append(getFilterGroup());
  headerFile.append("_Header.png");
  QResource headerResource(headerFile);
  if (true == headerResource.isValid())
  {
      style.append("background-image: url(");
      style.append( headerFile );
      style.append(");\n");
  }
  else
#endif


  QString headerImage("background-image: url(:/filterWidgetBorder.png);");
  if(m_HasPreflightWarnings == true)
  {
      headerImage = "background-image: url(:/filterWidgetBorder_Warning.png);";
  }

  if(m_HasPreflightErrors == true)
  {
      headerImage = "background-image: url(:/filterWidgetBorder_Error.png);";
  }
  style.append(headerImage);
  style.append("background-position: top ;\n background-repeat: repeat-x;");

  style.append(getBorderColorStyle());

  style.append("border-radius: 10px;");
  style.append("padding: 30 0 0 0px;");
#if defined(Q_WS_WIN)
  style.append("font: 85 italic 10pt \"Arial\";");
#elif defined(Q_WS_MAC)
  style.append("font: 85 italic 12pt \"Arial\";");
#else
  style.append("font: 85 italic 9pt \"Arial\";");
#endif
  style.append("font-weight: bold;");
  style.append("}\n");
  style.append(" QGroupBox::title {");
  style.append("    subcontrol-origin: padding;");
  style.append("    subcontrol-position: top left;");
  style.append("    padding: 5 5px;");
  style.append("    background-color: rgba(255, 255, 255, 0);");
  style.append(" }\n");
  style.append("QGroupBox::indicator {");
  style.append("    width: 17px;");
  style.append("    height: 17px;");
  style.append("}\n");
  style.append("\nQGroupBox::indicator:unchecked { image: url(:/delete-corner.png);}");
  style.append("\nQGroupBox::indicator:unchecked:pressed { image: url(:/delete-corner-pressed.png);}");
  style.append("\nQGroupBox::indicator:checked { image: url(:/delete-corner.png);}");
  style.append("\nQGroupBox::indicator:checked:pressed { image: url(:/delete-corner-pressed.png);}");

  setStyleSheet(style);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::setupGui()
{
  setCheckable(true);

  delete layout();


  QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Maximum);
  sizePolicy2.setHorizontalStretch(0);
  sizePolicy2.setVerticalStretch(0);
  setSizePolicy(sizePolicy2);


  setTitle(QString::fromStdString(getFilter()->getHumanLabel()));
#if 0
  QVBoxLayout* vertLayout_0 = new QVBoxLayout(this);

  QTabWidget* tabWidget = new QTabWidget(this);
  tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
  // Create a QWidget to hold the Parameters
  QWidget* parameterTab = new QWidget();
  parameterTab->setObjectName(QString::fromUtf8("parameterTab"));
  tabWidget->addTab(parameterTab, "Parameters");

  // Create a QWidget to hold the Array Selection Widget
  m_ArraySelectionTab = new PipelineArraySelectionWidget();
  m_ArraySelectionTab->setObjectName(QString::fromUtf8("arrayTab"));
  tabWidget->addTab(m_ArraySelectionTab, "Array Selection");
  tabWidget->setCurrentIndex(0);


  // Add the TabWidget to the top level Vertical Layout
  vertLayout_0->addWidget(tabWidget);
QVBoxLayout* vertLayout = new QVBoxLayout(parameterTab);
#else

  QVBoxLayout* vertLayout = new QVBoxLayout(this);
#endif

  QFormLayout* frmLayout = new QFormLayout();
  vertLayout->addLayout(frmLayout);
  frmLayout->setObjectName("QFilterWidget QFormLayout Layout");

  setIsSelected(false);
  bool ok = false;

  std::vector<FilterParameter::Pointer> options = getFilter()->getFilterParameters();
  int optIndex = 0;
  for (std::vector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
  {
    FilterParameter* option = (*iter).get();
    FilterParameter::WidgetType wType = option->getWidgetType();

    QString labelName = QString::fromStdString(option->getHumanLabel());
    if (option->getUnits().empty() == false)
    {
        labelName.append(" (").append(QString::fromStdString(option->getUnits())).append(")");
    }
    QLabel* label = new QLabel(labelName, this);

    if (wType == FilterParameter::StringWidget)
    {
      QLineEdit* le = new QLineEdit(this);
      le->setObjectName(QString::fromStdString(option->getPropertyName()));

      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, label);
      frmLayout->setWidget(optIndex, QFormLayout::FieldRole, le);
      connect(le, SIGNAL(textChanged(QString)), this, SLOT(updateQLineEditStringValue(const QString &)));
      QVariant v = property(option->getPropertyName().c_str());
      le->setText(v.toString());
    }
    else if (wType == FilterParameter::IntWidget)
    {
      QLineEdit* le = new QLineEdit(this);
      le->setObjectName(QString::fromStdString(option->getPropertyName()));
      QIntValidator* ival = new QIntValidator(this);
      le->setValidator(ival);
      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, label);
      frmLayout->setWidget(optIndex, QFormLayout::FieldRole, le);
      connect(le, SIGNAL(textChanged(QString)), this, SLOT(updateQLineEditIntValue()));
      QVariant v = property(option->getPropertyName().c_str());
      le->setText(v.toString());
    }
    else if (wType == FilterParameter::DoubleWidget)
    {
      QLineEdit* le = new QLineEdit(this);
      le->setObjectName(QString::fromStdString(option->getPropertyName()));
      QDoubleValidator* ival = new QDoubleValidator(this);
      ival->setDecimals(8);
      le->setValidator(ival);
      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, label);
      frmLayout->setWidget(optIndex, QFormLayout::FieldRole, le);
      connect(le, SIGNAL(textChanged(QString)), this, SLOT(updateQLineEditDoubleValue()));
      QVariant v = property(option->getPropertyName().c_str());
      le->setText(v.toString());
    }
    else if (wType == FilterParameter::InputFileWidget)
    {
      QGridLayout* gridLayout = new QGridLayout();
      gridLayout->setContentsMargins(0,0,0,0);
      gridLayout->addWidget(label, 0, 0, 1, 1);
      QFSDropLineEdit* fp = new QFSDropLineEdit(this);
      fp->setObjectName(QString::fromStdString(option->getPropertyName()));
      QR3DFileCompleter* com = new QR3DFileCompleter(this, false);
      fp->setCompleter(com);
      QString theSlot("1");
      theSlot.append("set");
      theSlot.append(QString::fromStdString(option->getPropertyName()));
      theSlot.append("(const QString &)");
     // std::cout << getFilter()->getNameOfClass() << " - Slot Generated: " << theSlot.toStdString() << std::endl;
      QObject::connect( com, SIGNAL(activated(const QString &)),
                      this, theSlot.toAscii());
      QObject::connect( fp, SIGNAL(textChanged(const QString &)),
                        this, theSlot.toAscii());
//      connect(fp, SIGNAL(textChanged(const QString &)),
//              this, SLOT(updateLineEdit(const QString &)));

      gridLayout->addWidget(fp, 0, 1, 1, 1);

      QPushButton* btn = new QPushButton("Select...");
      btn->setObjectName(QString::fromStdString("btn_" + option->getPropertyName() ));
      gridLayout->addWidget(btn, 0, 2, 1, 1);

      vertLayout->addLayout(gridLayout);
      connect(btn, SIGNAL(clicked()), this, SLOT(selectInputFile()));
      QVariant v = property(option->getPropertyName().c_str());
      fp->setText(v.toString());
    }
    else if (wType == FilterParameter::InputPathWidget)
    {
      QGridLayout* gridLayout = new QGridLayout();
      gridLayout->setContentsMargins(0,0,0,0);
      gridLayout->addWidget(label, 0, 0, 1, 1);
      QFSDropLineEdit* fp = new QFSDropLineEdit(this);
      fp->setObjectName(QString::fromStdString(option->getPropertyName()));
      QR3DFileCompleter* com = new QR3DFileCompleter(this, false);
      fp->setCompleter(com);
      QString theSlot("1");
      theSlot.append("set");
      theSlot.append(QString::fromStdString(option->getPropertyName()));
      theSlot.append("(const QString &)");
     // std::cout << getFilter()->getNameOfClass() << " - Slot Generated: " << theSlot.toStdString() << std::endl;
      QObject::connect( com, SIGNAL(activated(const QString &)),
                      this, theSlot.toAscii());
      QObject::connect( fp, SIGNAL(textChanged(const QString &)),
                        this, theSlot.toAscii());
//      connect(fp, SIGNAL(textChanged(const QString &)),
//              this, SLOT(updateLineEdit(const QString &)));

      gridLayout->addWidget(fp, 0, 1, 1, 1);

      QPushButton* btn = new QPushButton("Select Folder");
      btn->setObjectName(QString::fromStdString("btn_" + option->getPropertyName() ));
      gridLayout->addWidget(btn, 0, 2, 1, 1);

      vertLayout->addLayout(gridLayout);
      connect(btn, SIGNAL(clicked()), this, SLOT(selectInputPath()));
      QVariant v = property(option->getPropertyName().c_str());
      fp->setText(v.toString());
    }
    else if (wType == FilterParameter::OutputFileWidget)
    {
      QGridLayout* gridLayout = new QGridLayout();
      gridLayout->setContentsMargins(0,0,0,0);
      gridLayout->addWidget(label, 0, 0, 1, 1);

      QLineEdit* fp = new QLineEdit(this);
      fp->setObjectName(QString::fromStdString(option->getPropertyName()));
      QR3DFileCompleter* com = new QR3DFileCompleter(this, false);
      fp->setCompleter(com);
      QString theSlot("1");
      theSlot.append("set");
      theSlot.append(QString::fromStdString(option->getPropertyName()));
      theSlot.append("(const QString &)");
      QObject::connect( fp, SIGNAL(textChanged(const QString &)),
                        this, theSlot.toAscii());
      gridLayout->addWidget(fp, 0, 1, 1, 1);
      QPushButton* btn = new QPushButton("Save As...");
      btn->setObjectName(QString::fromStdString("btn_" + option->getPropertyName()));
      gridLayout->addWidget(btn, 0, 2, 1, 1);

      vertLayout->addLayout(gridLayout);
      connect(btn, SIGNAL(clicked()), this, SLOT(selectOutputFile()));
      QVariant v = property(option->getPropertyName().c_str());
      fp->setText(v.toString());
    }
    else if (wType == FilterParameter::OutputPathWidget)
    {
      QGridLayout* gridLayout = new QGridLayout();
      gridLayout->setContentsMargins(0,0,0,0);
      gridLayout->addWidget(label, 0, 0, 1, 1);
      QLineEdit* fp = new QLineEdit(this);
      fp->setObjectName(QString::fromStdString(option->getPropertyName()));
      QR3DFileCompleter* com = new QR3DFileCompleter(this, false);
      fp->setCompleter(com);
      QString theSlot("1");
      theSlot.append("set");
      theSlot.append(QString::fromStdString(option->getPropertyName()));
      theSlot.append("(const QString &)");

      QObject::connect( fp, SIGNAL(textChanged(const QString &)),
                        this, theSlot.toAscii());
      gridLayout->addWidget(fp, 0, 1, 1, 1);

      QPushButton* btn = new QPushButton("Select Folder...");
      btn->setObjectName(QString::fromStdString("btn_" + option->getPropertyName()));
      gridLayout->addWidget(btn, 0, 2, 1, 1);

      vertLayout->addLayout(gridLayout);
      connect(btn, SIGNAL(clicked()), this, SLOT(selectOutputPath()));
      QVariant v = property(option->getPropertyName().c_str());
      fp->setText(v.toString());
    }
    else if (wType == FilterParameter::BooleanWidget)
    {
      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, label);
      QCheckBox* le = new QCheckBox(this);
      le->setObjectName(QString::fromStdString(option->getPropertyName()));
      frmLayout->setWidget(optIndex, QFormLayout::FieldRole, le);
      connect(le, SIGNAL(stateChanged(int)), this, SLOT(updateQCheckBoxValue(int)));
      QVariant v = property(option->getPropertyName().c_str());
      le->setChecked(v.toBool());
    }
    #if 0
    else if (wType == FilterParameter::IntConstrainedWidget)
    {
      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, label);
      QSpinBox* le = new QSpinBox(this);
      le->setObjectName(QString::fromStdString(option->getPropertyName()));
      ConstrainedFilterParameter<int>* filtOpt = dynamic_cast<ConstrainedFilterParameter<int>* >(option);
      if (filtOpt)
      {
        le->setRange(filtOpt->getMinimum(), filtOpt->getMaximum());
        le->setValue(0);
      }
      frmLayout->setWidget(optIndex, QFormLayout::FieldRole, le);
      connect(le, SIGNAL(valueChanged(int)), this, SLOT(updateQSpinBoxValue(int)));
      QVariant v = property(option->getPropertyName().c_str());
      le->setValue(v.toInt());
    }
    else if (wType == FilterParameter::DoubleConstrainedWidget)
    {
      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, label);
      QDoubleSpinBox* le = new QDoubleSpinBox(this);
      le->setObjectName(QString::fromStdString(option->getPropertyName()));
      ConstrainedFilterParameter<float>* filtOpt = dynamic_cast<ConstrainedFilterParameter<float>* >(option);
      if (filtOpt)
      {
        le->setRange(filtOpt->getMinimum(), filtOpt->getMaximum());
        le->setValue(0);
      }
      frmLayout->setWidget(optIndex, QFormLayout::FieldRole, le);
      connect(le, SIGNAL(valueChanged(double)), this, SLOT(updateQDoubleSpinBoxValue(double)));
      QVariant v = property(option->getPropertyName().c_str());
      le->setValue(v.toDouble());
    }
    #endif
    else if (wType == FilterParameter::ChoiceWidget)
    {
      ChoiceFilterParameter* choiceFilterParameter = ChoiceFilterParameter::SafeObjectDownCast<FilterParameter*, ChoiceFilterParameter*>(option);
      if (NULL == choiceFilterParameter) { return; }
      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, label);
      QComboBox* cb = new QComboBox(this);
      cb->setObjectName(QString::fromStdString(option->getPropertyName()));
      std::vector<std::string> choices = choiceFilterParameter->getChoices();
      for(unsigned int i = 0; i < choices.size(); ++i)
      {
        cb->addItem(QString::fromStdString(choices[i]));
      }
      frmLayout->setWidget(optIndex, QFormLayout::FieldRole, cb);
      connect(cb, SIGNAL( currentIndexChanged(int)), this, SLOT(updateComboBoxValue(int)));
      QVariant v = property(option->getPropertyName().c_str());
      quint32 uintValue = v.toUInt(&ok);
      if (uintValue >= static_cast<quint32>(cb->count())  )
      {
        // What ever the default from the class was it does not work with the combo box
        // so set the combo box to the zeroth value and set the same value back
        // to the filter
        uintValue = 0;
      }
      cb->setCurrentIndex(uintValue);
      setProperty(option->getPropertyName().c_str(), uintValue);
    }
    ++optIndex;
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateFilterValues()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer QFilterWidget::getFilter()
{
  return AbstractFilter::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateQLineEditIntValue()
{
  QObject* whoSent = sender();
//  std::cout << "Filter: " << title().toStdString() << " Getting updated from whoSent Name: "
//      << whoSent->objectName().toStdString() << std::endl;
  QLineEdit* le = qobject_cast<QLineEdit*>(whoSent);
  if(le) {
    bool ok = false;
    int value = le->text().toInt(&ok);
    if (false == ok) {
      value = 0;
      le->setText("0");
    }
    setProperty(whoSent->objectName().toStdString().c_str(), value);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateQLineEditDoubleValue()
{
  QObject* whoSent = sender();
//  std::cout << "Filter: " << title().toStdString() << " Getting updated from whoSent Name: "
 //     << whoSent->objectName().toStdString() << std::endl;
  QLineEdit* le = qobject_cast<QLineEdit*>(whoSent);
  if(le) {
    bool ok = false;
    double value = le->text().toDouble(&ok);
    if (false == ok) {
      value = 0;
      le->setText("0");
    }
    setProperty(whoSent->objectName().toStdString().c_str(), value);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::selectInputFile()
{
  QObject* whoSent = sender();

  QString file = QFileDialog::getOpenFileName(this,
                                              tr("Select Input File"),
                                              m_OpenDialogLastDirectory,
                                              tr("ALL Files (*.*)"));
  if(true == file.isEmpty())
  {
    return;
  }
  bool ok = false;
  file = QDir::toNativeSeparators(file);
  // Store the last used directory into the private instance variable
  QFileInfo fi(file);
  m_OpenDialogLastDirectory = fi.path();

  // for QButtons we prepended "btn_" to the end of the property name so strip that off
  QString propName = whoSent->objectName();
  propName = propName.remove(0, 4);

  ok = setProperty(propName.toStdString().c_str(), file);
  if (true == ok) { }
  else
  {
    //  std::cout << "QPushButton '" << title().toStdString() <<  "'Property: '" << whoSent->objectName().toStdString() << "' was NOT set."<< std::endl;
  }
  // Now we need to find the specific filter that we are trying to set the value into
  AbstractFilter::Pointer f = getFilter();
  std::vector<FilterParameter::Pointer> opts = f->getFilterParameters();
  for (std::vector<FilterParameter::Pointer>::iterator iter = opts.begin(); iter != opts.end(); ++iter)
  {
    if((*iter)->getPropertyName().compare(propName.toStdString()) == 0)
    {
      QLineEdit* lb = qFindChild<QLineEdit*>(this, QString::fromStdString((*iter)->getPropertyName()));
      if(lb)
      {
        lb->setText(file);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::selectInputPath()
{
  QObject* whoSent = sender();

  QString file = QFileDialog::getExistingDirectory(this,
                                              tr("Select Input Folder"),
                                              m_OpenDialogLastDirectory,
                                              QFileDialog::ShowDirsOnly);
  if(true == file.isEmpty())
  {
    return;
  }
  bool ok = false;
  file = QDir::toNativeSeparators(file);

  // Store the last used directory into the private instance variable
  QFileInfo fi(file);
  m_OpenDialogLastDirectory = fi.path();

  // for QButtons we prepended "btn_" to the end of the property name so strip that off
  QString propName = whoSent->objectName();
  propName = propName.remove(0, 4);

  ok = setProperty(propName.toStdString().c_str(), file);
  if (true == ok) { }
  else
  {
    //  std::cout << "QPushButton '" << title().toStdString() <<  "'Property: '" << whoSent->objectName().toStdString() << "' was NOT set."<< std::endl;
  }
  // Now we need to find the specific filter that we are trying to set the value into
  AbstractFilter::Pointer f = getFilter();
  std::vector<FilterParameter::Pointer> opts = f->getFilterParameters();
  for (std::vector<FilterParameter::Pointer>::iterator iter = opts.begin(); iter != opts.end(); ++iter)
  {
    if((*iter)->getPropertyName().compare(propName.toStdString()) == 0)
    {
      QLineEdit* lb = qFindChild<QLineEdit*>(this, QString::fromStdString((*iter)->getPropertyName()));
      if(lb)
      {
        lb->setText(file);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::selectOutputFile()
{
  QObject* whoSent = sender();
  // for QButtons we prepended "btn_" to the end of the property name so strip that off
  QString propName = whoSent->objectName();
  propName = propName.remove(0, 4);

  QString Ftype = getFileType(propName.toStdString());
  QString ext = getFileExtension(propName.toStdString());
  QString s = Ftype + QString(" Files (*.") + ext + QString(");;All Files(*.*)");
  QString defaultName = m_OpenDialogLastDirectory + QDir::separator() + "Untitled";
  QString file = QFileDialog::getSaveFileName(this, tr("Save File As"), defaultName, s);
  if(true == file.isEmpty())
  {
    return;
  }


  // Store the last used directory into the private instance variable
  file = QDir::toNativeSeparators(file);
  QFileInfo fi(file);
  m_OpenDialogLastDirectory = fi.path();

  bool ok = false;

  ok = setProperty(propName.toStdString().c_str(), file);
  if (true == ok) {}
  else
  {
    //  std::cout << "QPushButton '" << title().toStdString() <<  "'Property: '" << whoSent->objectName().toStdString() << "' was NOT set."<< std::endl;
  }

  // Now we need to find the specific filter that we are trying to set the value into
  AbstractFilter::Pointer f = getFilter();
  // Get the options for that filter
  std::vector<FilterParameter::Pointer> opts = f->getFilterParameters();
  // Loop on all the filter options to find the filter option we want to set
  for (std::vector<FilterParameter::Pointer>::iterator iter = opts.begin(); iter != opts.end(); ++iter)
  {
    if((*iter)->getPropertyName().compare(propName.toStdString()) == 0)
    {
      QLineEdit* lb = qFindChild<QLineEdit*>(this, QString::fromStdString((*iter)->getPropertyName()));
      if(lb)
      {
        lb->setText(file);
        // Setting the text into this QLineEdit will trigger the 'textChanged()' signal
        // to be emitted by the QLineEdit which will cause the value to sent to
        // the underlying filter instance which will trigger the SIGNAL parametersChanged()
        // to be broadcast
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::selectOutputPath()
{
  QObject* whoSent = sender();
  QString file = QFileDialog::getExistingDirectory(this, tr("Select Output Folder"),
                                              m_OpenDialogLastDirectory, QFileDialog::ShowDirsOnly);
  if(true == file.isEmpty())
  {
    return;
  }
  file = QDir::toNativeSeparators(file);

  // Store the last used directory into the private instance variable
  QFileInfo fi(file);
  m_OpenDialogLastDirectory = fi.path();

  bool ok = false;
  // for QButtons we prepended "btn_" to the end of the property name so strip that off
  QString propName = whoSent->objectName();
  propName = propName.remove(0, 4);

  ok = setProperty(propName.toStdString().c_str(), file);
  if (true == ok) {}
  else
  {
    //  std::cout << "QPushButton '" << title().toStdString() <<  "'Property: '" << whoSent->objectName().toStdString() << "' was NOT set."<< std::endl;
  }

  // Now we need to find the specific filter that we are trying to set the value into
  AbstractFilter::Pointer f = getFilter();
  // Get the options for that filter
  std::vector<FilterParameter::Pointer> opts = f->getFilterParameters();
  // Loop on all the filter options to find the filter option we want to set
  for (std::vector<FilterParameter::Pointer>::iterator iter = opts.begin(); iter != opts.end(); ++iter)
  {
    if((*iter)->getPropertyName().compare(propName.toStdString()) == 0)
    {
      QLineEdit* lb = qFindChild<QLineEdit*>(this, QString::fromStdString((*iter)->getPropertyName()));
      if(lb)
      {
        lb->setText(file);
        // Setting the text into this QLineEdit will trigger the 'textChanged()' signal
        // to be emitted by the QLineEdit which will cause the value to sent to
        // the underlying filter instance which will trigger the SIGNAL parametersChanged()
        // to be broadcast
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateComboBoxValue(int v)
{
  QObject* whoSent = sender();
//  std::cout << "Filter: " << title().toStdString() << " Getting updated from whoSent Name: " << whoSent->objectName().toStdString() << std::endl;
  QComboBox* cb = qobject_cast<QComboBox*>(whoSent);
  if(cb)
  {
    bool ok = false;
    ok = setProperty(whoSent->objectName().toStdString().c_str(), v);
    if (true == ok) { }
    else
    {
      std::cout << "QComboBox '" << title().toStdString() << "'Property: '" << whoSent->objectName().toStdString() << "' was NOT set." << std::endl;
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateQLineEditStringValue(const QString &v)
{
  QObject* whoSent = sender();
//  std::cout << "Filter: " << title().toStdString() << " Getting updated from whoSent Name: "
//      << whoSent->objectName().toStdString() << std::endl;
  QLineEdit* le = qobject_cast<QLineEdit*>(whoSent);
  if(le)
  {
    setProperty(whoSent->objectName().toStdString().c_str(), le->text());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateQSpinBoxValue(int v)
{
  assert(false);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateQDoubleSpinBoxValue(double v)
{
  assert(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateQCheckBoxValue(int v)
{
  QObject* whoSent = sender();
//  std::cout << "Filter: " << title().toStdString() << "->Property: " << whoSent->objectName().toStdString()
//      << " via QCheckBox." <<  std::endl;
  QCheckBox* le = qobject_cast<QCheckBox*>(whoSent);
  if(le)
  {
    setProperty(whoSent->objectName().toStdString().c_str(), le->isChecked());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateLineEdit(const QString &v)
{
  QObject* whoSent = sender();
  std::cout << "Filter: " << title().toStdString() << "->Property: " << whoSent->objectName().toStdString()
      << " via QLineEdit." <<  std::endl;
  assert(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::emitParametersChanged()
{
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::writeOptions(QSettings &prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::readOptions(QSettings &prefs)
{

}
#if 0

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::setCellDataArrayNames(QStringList arrayNames)
{
//  std::cout << getFilter()->getNameOfClass() << " Possible Cell Data" << std::endl;
//  QStringList::const_iterator constIterator;
//  for (constIterator = arrayNames.constBegin(); constIterator != arrayNames.constEnd(); ++constIterator)
//  {
//    std::cout << (*constIterator).toLocal8Bit().constData() << std::endl;
//  }
  getPipelineArraySelectionWidget()->setPossibleCellArrayNames(arrayNames);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::setFieldDataArrayNames(QStringList arrayNames)
{
//  std::cout << getFilter()->getNameOfClass() << " Possible Field Data" << std::endl;
//  QStringList::const_iterator constIterator;
//  for (constIterator = arrayNames.constBegin(); constIterator != arrayNames.constEnd(); ++constIterator)
//  {
//    std::cout << (*constIterator).toLocal8Bit().constData() << std::endl;
//  }
  getPipelineArraySelectionWidget()->setPossibleFieldArrayNames(arrayNames);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::setEnsembleDataArrayNames(QStringList arrayNames)
{
//  std::cout << getFilter()->getNameOfClass() << " Possible Ensemble Data" << std::endl;
//  QStringList::const_iterator constIterator;
//  for (constIterator = arrayNames.constBegin(); constIterator != arrayNames.constEnd(); ++constIterator)
//  {
//    std::cout << (*constIterator).toLocal8Bit().constData() << std::endl;
//  }
  getPipelineArraySelectionWidget()->setPossibleEnsembleArrayNames(arrayNames);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updatePipelineArrayNames(AbstractFilter* filter)
{
  typedef std::set<std::string> StringSet_t;
  StringSet_t cellNameSet = filter->getCreatedCellData();
  StringSet_t fieldNameset = filter->getCreatedFieldData();
  StringSet_t ensembleNameSet = filter->getCreatedEnsembleData();

  StringSet_t cellReqNameSet = filter->getRequiredCellData();
  StringSet_t fieldReqNameSet = filter->getRequiredFieldData();
  StringSet_t ensembleReqNameSet = filter->getRequiredEnsembleData();



}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void  QFilterWidget::mousePressEvent ( QMouseEvent* event )
{
  if(event->button() != Qt::LeftButton)
  {
    event->ignore();
    return;
  }
  // Only if we are inside the delete checkbox/image then pass up to the superclass
  if(m_DeleteRect.contains(event->pos()))
  {
    QGroupBox::mousePressEvent(event);
  }
  else
  {
    dragStartPosition = event->pos();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::mouseReleaseEvent(QMouseEvent *event)
{
  if(event->button() != Qt::LeftButton)
  {
    event->ignore();
    return;
  }
  // Only if we are inside the delete checkbox/image then pass up to the superclass
  if(m_DeleteRect.contains(event->pos()))
  {
    QGroupBox::mouseReleaseEvent(event);
  }
  else
  {
    emit widgetSelected(this);
    setIsSelected(true);
    event->setAccepted(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::mouseMoveEvent(QMouseEvent *event)
{
  if(!(event->buttons() & Qt::LeftButton))
  {
    return;
  }
  if((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
  {
    return;
  }

  QPixmap pixmap = QPixmap::grabWidget(this);

  // Create new picture for transparent
  QPixmap transparent(pixmap.size());
  // Do transparency
  transparent.fill(Qt::transparent);
#if 1
  QPainter p;

  p.begin(&transparent);
  p.setOpacity(0.70);
 // p.setCompositionMode(QPainter::CompositionMode_Plus);
  p.drawPixmap(0, 0, pixmap);
 // p.setCompositionMode(QPainter::CompositionMode_Plus);

  // Set transparency level to 150 (possible values are 0-255)
  // The alpha channel of a color specifies the transparency effect,
  // 0 represents a fully transparent color, while 255 represents
  // a fully opaque color.
//  p.fillRect(transparent.rect(), QColor(0, 0, 0, 150));
  p.end();
#endif


  QByteArray itemData;
  QDataStream dataStream(&itemData, QIODevice::WriteOnly);
  dataStream << transparent << QPoint(event->pos());

  QMimeData *mimeData = new QMimeData;
  mimeData->setData("application/x-dnditemdata", itemData);

  QDrag *drag = new QDrag(this);
  drag->setMimeData(mimeData);
  drag->setPixmap(transparent);
  drag->setHotSpot(event->pos());

  emit dragStarted(this);

//  if(drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction)
//  {
//    std::cout << "Drag should close the widget because it was MOVE" << std::endl;
//  }
//  else
//  {
//    std::cout << "Drag should leave Widget alone because it was COPY" << std::endl;
//  }
  drag->exec(Qt::MoveAction);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QFilterWidget::getFileExtension(std::string propName)
{
  std::vector<FilterParameter::Pointer> options = getFilter()->getFilterParameters();
//  int optIndex = 0;
  for (std::vector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
  {
    FilterParameter* option = (*iter).get();
    if(option->getPropertyName().compare(propName) == 0)
    {
        return QString::fromStdString(option->getFileExtension());
    }
  }
  return QString("");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QFilterWidget::getFileType(std::string propName)
{
  std::vector<FilterParameter::Pointer> options = getFilter()->getFilterParameters();
  //int optIndex = 0;
  for (std::vector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
  {
    FilterParameter* option = (*iter).get();
    if(option->getPropertyName().compare(propName) == 0)
    {
        return QString::fromStdString(option->getFileType());
    }
  }
  return QString("");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateVoxelArrayNames(VoxelDataContainer::Pointer vdc)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateSurfaceMeshArrayNames(SurfaceMeshDataContainer::Pointer smdc)
{

}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateSolidMeshArrayNames(SolidMeshDataContainer::Pointer sdc)
{

}
