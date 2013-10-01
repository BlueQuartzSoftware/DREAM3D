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
#include <QtGui/QDesktopServices>
#include <QtGui/QMessageBox>


#include "QtSupport/QR3DFileCompleter.h"
#include "QtSupport/QFSDropLineEdit.h"
#include "QtSupport/DREAM3DHelpUrlGenerator.h"


#include "ArraySelectionWidget.h"
#include "ComparisonSelectionWidget.h"
#include "AxisAngleWidget.h"

#define PADDING 5
#define BORDER 2
#define IMAGE_WIDTH 17
#define IMAGE_HEIGHT 17

// These Streams need to be implemented so that our 3 Element Vectors can be read/write to disk/prefs files
QDataStream &operator<<( QDataStream &out, const IntVec3Widget_t& v) {
  out << v.x << v.y << v.z; return out;
}
QDataStream &operator>>( QDataStream &in, IntVec3Widget_t& v) { in >> v.x >> v.y >> v.z; return in; }

QDataStream &operator<<( QDataStream &out, const FloatVec3Widget_t& v) { out << v.x << v.y << v.z; return out; }
QDataStream &operator>>( QDataStream &in, FloatVec3Widget_t& v) { in >> v.x >> v.y >> v.z; return in; }

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
  m_HasPreflightWarnings(false),
  m_FilterMenu(this)
{
  qRegisterMetaType<IntVec3Widget_t>("IntVec3Widget_t");
  qRegisterMetaTypeStreamOperators<IntVec3Widget_t>("IntVec3Widget_t");

  qRegisterMetaType<FloatVec3Widget_t>("FloatVec3Widget_t");
  qRegisterMetaTypeStreamOperators<FloatVec3Widget_t>("FloatVec3Widget_t");

  // qRegisterMetaType<AxisAngleWidget_t>("AxisAngleWidget_t");
  // qRegisterMetaTypeStreamOperators<AxisAngleWidget_t>("AxisAngleWidget_t");

  if ( m_OpenDialogLastDirectory.isEmpty() )
  {
    m_OpenDialogLastDirectory = QDir::homePath();
  }

  // Initialize right-click menu
  initFilterMenu();

  m_DeleteRect.setX(PADDING + BORDER);
  m_DeleteRect.setY(PADDING + BORDER);
  m_DeleteRect.setWidth(IMAGE_WIDTH);
  m_DeleteRect.setHeight(IMAGE_HEIGHT);
  m_timer = new QTimer(this);
  connect(m_timer, SIGNAL(timeout()), this, SLOT(changeStyle()));

  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this,
          SIGNAL(customContextMenuRequested(const QPoint&)),
          SLOT(onCustomContextMenuRequested(const QPoint&)));
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
void QFilterWidget::openHtmlHelpFile()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::onCustomContextMenuRequested(const QPoint& pos)
{
  m_FilterMenu.exec( mapToGlobal(pos) );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::initFilterMenu()
{
  m_actionFilterHelp = new QAction(this);
  m_actionFilterHelp->setObjectName(QString::fromUtf8("actionWidgetHelp"));
  m_actionFilterHelp->setText(QApplication::translate("QFilterWidget", "Show Filter Help", 0, QApplication::UnicodeUTF8));
  connect(m_actionFilterHelp, SIGNAL(triggered()),
          this, SLOT( actionWidgetHelp_triggered() ) );
  m_FilterMenu.addAction(m_actionFilterHelp);

  m_FilterMenu.addSeparator();

  m_actionRemoveFilter = new QAction(this);
  m_actionRemoveFilter->setObjectName(QString::fromUtf8("actionRemoveFilter"));
  m_actionRemoveFilter->setText(QApplication::translate("QFilterWidget", "Remove Filter", 0, QApplication::UnicodeUTF8));
  connect(m_actionRemoveFilter, SIGNAL(triggered()),
          this, SLOT( actionRemoveFilter_triggered() ) );
  m_FilterMenu.addAction(m_actionRemoveFilter);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::actionWidgetHelp_triggered()
{
  openHtmlHelpFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::actionRemoveFilter_triggered()
{
  emit clicked(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QFilterWidget::getFilterGroup()
{
  return (DREAM3D::FilterGroups::GenericFilters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QFilterWidget::getFilterSubGroup()
{
  return (DREAM3D::FilterSubGroups::MiscFilters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//QFilterWidget* QFilterWidget::createDeepCopy()
//{
//  QFilterWidget* widget = new QFilterWidget();
//  return widget;
//}

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


  setTitle((getFilter(true)->getHumanLabel()));
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

  QVector<FilterParameter::Pointer> options = getFilter(true)->getFilterParameters();
  int optIndex = 0;
  for (QVector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
  {
    FilterParameter* option = (*iter).get();
    FilterParameter::WidgetType wType = option->getWidgetType();

    QString labelName = (option->getHumanLabel());
    if (option->getUnits().isEmpty() == false)
    {
      labelName.append(" (").append((option->getUnits())).append(")");
    }
    QLabel* label = new QLabel(labelName, this);

    if (wType == FilterParameter::StringWidget)
    {
      QLineEdit* le = new QLineEdit(this);
      le->setObjectName((option->getPropertyName()));

      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, label);
      frmLayout->setWidget(optIndex, QFormLayout::FieldRole, le);
      connect(le, SIGNAL(textChanged(QString)), this, SLOT(updateQLineEditStringValue(const QString &)));
      QVariant v = property(option->getPropertyName().toLatin1().data());
      le->setText(v.toString());
    }
    else if (wType == FilterParameter::IntWidget)
    {
      QLineEdit* le = new QLineEdit(this);
      le->setObjectName((option->getPropertyName()));
      QIntValidator* ival = new QIntValidator(this);
      le->setValidator(ival);
      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, label);
      frmLayout->setWidget(optIndex, QFormLayout::FieldRole, le);
      connect(le, SIGNAL(textChanged(QString)), this, SLOT(updateQLineEditIntValue()));
      QVariant v = property(option->getPropertyName().toLatin1().data());
      le->setText(v.toString());
    }
    else if (wType == FilterParameter::DoubleWidget)
    {
      QLineEdit* le = new QLineEdit(this);
      le->setObjectName((option->getPropertyName()));
      QDoubleValidator* ival = new QDoubleValidator(this);
      ival->setDecimals(8);
      le->setValidator(ival);
      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, label);
      frmLayout->setWidget(optIndex, QFormLayout::FieldRole, le);
      connect(le, SIGNAL(textChanged(QString)), this, SLOT(updateQLineEditDoubleValue()));
      QVariant v = property(option->getPropertyName().toLatin1().data());
      le->setText(v.toString());
    }
    else if (wType == FilterParameter::InputFileWidget)
    {
      QGridLayout* gridLayout = new QGridLayout();
      gridLayout->setContentsMargins(0,0,0,0);
      gridLayout->addWidget(label, 0, 0, 1, 1);
      QFSDropLineEdit* fp = new QFSDropLineEdit(this);
      fp->setObjectName((option->getPropertyName()));
      QR3DFileCompleter* com = new QR3DFileCompleter(this, false);
      fp->setCompleter(com);
      QString theSlot("1");
      theSlot.append("set");
      theSlot.append((option->getPropertyName()));
      theSlot.append("(const QString &)");
      // qDebug() << getFilter()->getNameOfClass() << " - Slot Generated: " << theSlot<< "\n";
      QObject::connect( com, SIGNAL(activated(const QString &)),
                        this, theSlot.toAscii());
      QObject::connect( fp, SIGNAL(textChanged(const QString &)),
                        this, theSlot.toAscii());
      gridLayout->addWidget(fp, 0, 1, 1, 1);

      QPushButton* btn = new QPushButton("Select...");
      btn->setObjectName(("btn_" + option->getPropertyName() ));
      gridLayout->addWidget(btn, 0, 2, 1, 1);

      vertLayout->addLayout(gridLayout);
      connect(btn, SIGNAL(clicked()), this, SLOT(selectInputFile()));
      QVariant v = property(option->getPropertyName().toLatin1().data());
      fp->setText(v.toString());
    }
    else if (wType == FilterParameter::InputPathWidget)
    {
      QGridLayout* gridLayout = new QGridLayout();
      gridLayout->setContentsMargins(0,0,0,0);
      gridLayout->addWidget(label, 0, 0, 1, 1);
      QFSDropLineEdit* fp = new QFSDropLineEdit(this);
      fp->setObjectName((option->getPropertyName()));
      QR3DFileCompleter* com = new QR3DFileCompleter(this, false);
      fp->setCompleter(com);
      QString theSlot("1");
      theSlot.append("set");
      theSlot.append((option->getPropertyName()));
      theSlot.append("(const QString &)");
      // qDebug() << getFilter()->getNameOfClass() << " - Slot Generated: " << theSlot<< "\n";
      QObject::connect( com, SIGNAL(activated(const QString &)),
                        this, theSlot.toAscii());
      QObject::connect( fp, SIGNAL(textChanged(const QString &)),
                        this, theSlot.toAscii());
      //      connect(fp, SIGNAL(textChanged(const QString &)),
      //              this, SLOT(updateLineEdit(const QString &)));

      gridLayout->addWidget(fp, 0, 1, 1, 1);

      QPushButton* btn = new QPushButton("Select Folder");
      btn->setObjectName(("btn_" + option->getPropertyName() ));
      gridLayout->addWidget(btn, 0, 2, 1, 1);

      vertLayout->addLayout(gridLayout);
      connect(btn, SIGNAL(clicked()), this, SLOT(selectInputPath()));
      QVariant v = property(option->getPropertyName().toLatin1().data());
      fp->setText(v.toString());
    }
    else if (wType == FilterParameter::OutputFileWidget)
    {
      QGridLayout* gridLayout = new QGridLayout();
      gridLayout->setContentsMargins(0,0,0,0);
      gridLayout->addWidget(label, 0, 0, 1, 1);

      QFSDropLineEdit* fp = new QFSDropLineEdit(this);
      fp->setObjectName((option->getPropertyName()));
      QR3DFileCompleter* com = new QR3DFileCompleter(this, false);
      fp->setCompleter(com);
      QString theSlot("1");
      theSlot.append("set");
      theSlot.append((option->getPropertyName()));
      theSlot.append("(const QString &)");
      QObject::connect( fp, SIGNAL(textChanged(const QString &)),
                        this, theSlot.toAscii());
      gridLayout->addWidget(fp, 0, 1, 1, 1);
      QPushButton* btn = new QPushButton("Save As...");
      btn->setObjectName(("btn_" + option->getPropertyName()));
      gridLayout->addWidget(btn, 0, 2, 1, 1);

      vertLayout->addLayout(gridLayout);
      connect(btn, SIGNAL(clicked()), this, SLOT(selectOutputFile()));
      QVariant v = property(option->getPropertyName().toLatin1().data());
      fp->setText(v.toString());
    }
    else if (wType == FilterParameter::OutputPathWidget)
    {
      QGridLayout* gridLayout = new QGridLayout();
      gridLayout->setContentsMargins(0,0,0,0);
      gridLayout->addWidget(label, 0, 0, 1, 1);
      QFSDropLineEdit* fp = new QFSDropLineEdit(this);
      fp->setObjectName((option->getPropertyName()));
      QR3DFileCompleter* com = new QR3DFileCompleter(this, false);
      fp->setCompleter(com);
      QString theSlot("1");
      theSlot.append("set");
      theSlot.append((option->getPropertyName()));
      theSlot.append("(const QString &)");

      QObject::connect( fp, SIGNAL(textChanged(const QString &)),
                        this, theSlot.toAscii());
      gridLayout->addWidget(fp, 0, 1, 1, 1);

      QPushButton* btn = new QPushButton("Select Folder...");
      btn->setObjectName(("btn_" + option->getPropertyName()));
      gridLayout->addWidget(btn, 0, 2, 1, 1);

      vertLayout->addLayout(gridLayout);
      connect(btn, SIGNAL(clicked()), this, SLOT(selectOutputPath()));
      QVariant v = property(option->getPropertyName().toLatin1().data());
      fp->setText(v.toString());
    }
    else if (wType == FilterParameter::BooleanWidget)
    {
      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, label);
      QCheckBox* le = new QCheckBox(this);
      le->setObjectName((option->getPropertyName()));
      frmLayout->setWidget(optIndex, QFormLayout::FieldRole, le);
      connect(le, SIGNAL(stateChanged(int)), this, SLOT(updateQCheckBoxValue(int)));
      QVariant v = property(option->getPropertyName().toLatin1().data());
      le->setChecked(v.toBool());
    }
#if 0
    else if (wType == FilterParameter::IntConstrainedWidget)
    {
      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, label);
      QSpinBox* le = new QSpinBox(this);
      le->setObjectName((option->getPropertyName()));
      ConstrainedFilterParameter<int>* filtOpt = dynamic_cast<ConstrainedFilterParameter<int>* >(option);
      if (filtOpt)
      {
        le->setRange(filtOpt->getMinimum(), filtOpt->getMaximum());
        le->setValue(0);
      }
      frmLayout->setWidget(optIndex, QFormLayout::FieldRole, le);
      connect(le, SIGNAL(valueChanged(int)), this, SLOT(updateQSpinBoxValue(int)));
      QVariant v = property(option->getPropertyName().toLatin1().data());
      le->setValue(v.toInt());
    }
    else if (wType == FilterParameter::DoubleConstrainedWidget)
    {
      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, label);
      QDoubleSpinBox* le = new QDoubleSpinBox(this);
      le->setObjectName((option->getPropertyName()));
      ConstrainedFilterParameter<float>* filtOpt = dynamic_cast<ConstrainedFilterParameter<float>* >(option);
      if (filtOpt)
      {
        le->setRange(filtOpt->getMinimum(), filtOpt->getMaximum());
        le->setValue(0);
      }
      frmLayout->setWidget(optIndex, QFormLayout::FieldRole, le);
      connect(le, SIGNAL(valueChanged(double)), this, SLOT(updateQDoubleSpinBoxValue(double)));
      QVariant v = property(option->getPropertyName().toLatin1().data());
      le->setValue(v.toDouble());
    }
#endif
    else if (wType == FilterParameter::ChoiceWidget)
    {
      ChoiceFilterParameter* choiceFilterParameter = ChoiceFilterParameter::SafeObjectDownCast<FilterParameter*, ChoiceFilterParameter*>(option);
      if (NULL == choiceFilterParameter) { return; }
      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, label);
      QComboBox* cb = new QComboBox(this);
      cb->setEditable(choiceFilterParameter->getEditable());
      cb->setObjectName((option->getPropertyName()));
      QVector<QString> choices = choiceFilterParameter->getChoices();
      QVariant v = property(option->getPropertyName().toLatin1().data());
      int selectedIndex = -1;
      for(unsigned int i = 0; i < choices.size(); ++i)
      {
        QString choice = (choices[i]);
        cb->addItem(choice);
        QString strChoice = v.toString();

        if ( strChoice.compare(choice) == 0)
        {
          selectedIndex = i;
        }
      }
      if (selectedIndex < 0) { selectedIndex = 0; }
      frmLayout->setWidget(optIndex, QFormLayout::FieldRole, cb);

      if (choiceFilterParameter->getValueType().compare("string") == 0
          && choiceFilterParameter->getEditable() == true)
      {
        connect(cb, SIGNAL( editTextChanged ( const QString&  )), this, SLOT(updateArrayNameComboBoxValue(const QString&)));
        connect(cb, SIGNAL( currentIndexChanged(int)), this, SLOT(updateArrayNameComboBoxValue(int)));

        cb->setCurrentIndex(selectedIndex);
        bool success = setProperty(option->getPropertyName().toLatin1().data(), cb->currentText());
        if (success == false)
        {
          BOOST_ASSERT(false);
        }
      }
      else
      {
        connect(cb, SIGNAL( currentIndexChanged(int)), this, SLOT(updateComboBoxValue(int)));
        quint32 uintValue = v.toUInt(&ok);
        if (uintValue >= static_cast<quint32>(cb->count())  )
        {
          // What ever the default from the class was it does not work with the combo box
          // so set the combo box to the zeroth value and set the same value back
          // to the filter
          uintValue = 0;
        }
        cb->setCurrentIndex(uintValue);
        setProperty(option->getPropertyName().toLatin1().data(), uintValue);
      }
    }
    else if (wType == FilterParameter::VolumeVertexArrayNameSelectionWidget)
    {
      setupVertexArrayNameChoiceWidget(frmLayout, optIndex, option, label);
    }
    else if (wType == FilterParameter::VolumeEdgeArrayNameSelectionWidget)
    {
      setupEdgeArrayNameChoiceWidget(frmLayout, optIndex, option, label);
    }
    else if (wType == FilterParameter::VolumeFaceArrayNameSelectionWidget)
    {
      setupFaceArrayNameChoiceWidget(frmLayout, optIndex, option, label);
    }
    else if (wType == FilterParameter::VolumeCellArrayNameSelectionWidget)
    {
      setupCellArrayNameChoiceWidget(frmLayout, optIndex, option, label);
    }
    else if (wType == FilterParameter::VolumeFieldArrayNameSelectionWidget)
    {
      setupFieldArrayNameChoiceWidget(frmLayout, optIndex, option, label);
    }
    else if (wType == FilterParameter::VolumeEnsembleArrayNameSelectionWidget)
    {
      setupEnsembleArrayNameChoiceWidget(frmLayout, optIndex, option, label);
    }
    else if (wType == FilterParameter::SurfaceVertexArrayNameSelectionWidget)
    {
      setupVertexArrayNameChoiceWidget(frmLayout, optIndex, option, label);
    }
    else if (wType == FilterParameter::SurfaceEdgeArrayNameSelectionWidget)
    {
      setupEdgeArrayNameChoiceWidget(frmLayout, optIndex, option, label);
    }
    else if (wType == FilterParameter::SurfaceFaceArrayNameSelectionWidget)
    {
      setupFaceArrayNameChoiceWidget(frmLayout, optIndex, option, label);
    }
    else if (wType == FilterParameter::SurfaceFieldArrayNameSelectionWidget)
    {
      setupFieldArrayNameChoiceWidget(frmLayout, optIndex, option, label);
    }
    else if (wType == FilterParameter::SurfaceEnsembleArrayNameSelectionWidget)
    {
      setupEnsembleArrayNameChoiceWidget(frmLayout, optIndex, option, label);
    }
    else if (wType == FilterParameter::EdgeVertexArrayNameSelectionWidget)
    {
      setupVertexArrayNameChoiceWidget(frmLayout, optIndex, option, label);
    }
    else if (wType == FilterParameter::EdgeEdgeArrayNameSelectionWidget)
    {
      setupEdgeArrayNameChoiceWidget(frmLayout, optIndex, option, label);
    }
    else if (wType == FilterParameter::EdgeFieldArrayNameSelectionWidget)
    {
      setupFieldArrayNameChoiceWidget(frmLayout, optIndex, option, label);
    }
    else if (wType == FilterParameter::EdgeEnsembleArrayNameSelectionWidget)
    {
      setupEnsembleArrayNameChoiceWidget(frmLayout, optIndex, option, label);
    }
    else if (wType == FilterParameter::VertexVertexArrayNameSelectionWidget)
    {
      setupVertexArrayNameChoiceWidget(frmLayout, optIndex, option, label);
    }
    else if (wType == FilterParameter::VertexFieldArrayNameSelectionWidget)
    {
      setupFieldArrayNameChoiceWidget(frmLayout, optIndex, option, label);
    }
    else if (wType == FilterParameter::VertexEnsembleArrayNameSelectionWidget)
    {
      setupEnsembleArrayNameChoiceWidget(frmLayout, optIndex, option, label);
    }

    else if (wType == FilterParameter::ArraySelectionWidget)
    {
      setupArraySelectionWidget(frmLayout, optIndex, option, label);
    }
    else if (wType == FilterParameter::IntVec3Widget)
    {
      setupIntVec3Widget(frmLayout, optIndex, option, label);
    }
    else if (wType == FilterParameter::FloatVec3Widget)
    {
      setupFloatVec3Widget(frmLayout, optIndex, option, label);
    }
    else if (wType == FilterParameter::AxisAngleWidget)
    {
      setupAxisAngleWidget(frmLayout, optIndex, option, label);
    }
    else if (wType >= FilterParameter::CellArrayComparisonSelectionWidget && wType <= FilterParameter::EdgeArrayComparisonSelectionWidget)
    {
      setupComparisonArraysWidget(frmLayout, optIndex, option, label, wType);
    }
    ++optIndex;
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::setupIntVec3Widget(QFormLayout* frmLayout, int optIndex,
                                       FilterParameter* option, QLabel* label )
{
  frmLayout->setWidget(optIndex, QFormLayout::LabelRole, label);
  QString prop = (option->getPropertyName());
  QVariant v = property(option->getPropertyName().toLatin1().data());
  IntVec3Widget_t v3 = v.value<IntVec3Widget_t>();

  QHBoxLayout* horizontalLayout = new QHBoxLayout();
  horizontalLayout->setSpacing(8);
  horizontalLayout->setContentsMargins(-1, 0, -1, -1);
  QLineEdit* Dimensions_0 = new QLineEdit(this);
  Dimensions_0->setObjectName(QString::fromUtf8("0_") + prop);
  QIntValidator* val_0 = new QIntValidator(this);
  Dimensions_0->setValidator(val_0);
  Dimensions_0->setText(QString::number(v3.x));
  horizontalLayout->addWidget(Dimensions_0);

  QLineEdit* Dimensions_1 = new QLineEdit(this);
  Dimensions_1->setObjectName(QString::fromUtf8("1_") + prop);
  QIntValidator* val_1 = new QIntValidator(this);
  Dimensions_1->setValidator(val_1);
  Dimensions_1->setText(QString::number(v3.y));
  horizontalLayout->addWidget(Dimensions_1);

  QLineEdit* Dimensions_2 = new QLineEdit(this);
  Dimensions_2->setObjectName(QString::fromUtf8("2_") + prop);
  QIntValidator* val_2 = new QIntValidator(this);
  Dimensions_2->setValidator(val_2);
  Dimensions_2->setText(QString::number(v3.z));
  horizontalLayout->addWidget(Dimensions_2);

  frmLayout->setLayout(optIndex, QFormLayout::FieldRole, horizontalLayout);
  connect(Dimensions_0, SIGNAL( textChanged(const QString &)), this, SLOT(updateIntVec3Widget(const QString &)));
  connect(Dimensions_1, SIGNAL( textChanged(const QString &)), this, SLOT(updateIntVec3Widget(const QString &)));
  connect(Dimensions_2, SIGNAL( textChanged(const QString &)), this, SLOT(updateIntVec3Widget(const QString &)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::setupFloatVec3Widget(QFormLayout* frmLayout, int optIndex,
                                         FilterParameter* option, QLabel* label )
{
  frmLayout->setWidget(optIndex, QFormLayout::LabelRole, label);
  QString prop = (option->getPropertyName());
  QVariant v = property(option->getPropertyName().toLatin1().data());
  FloatVec3Widget_t v3 = v.value<FloatVec3Widget_t>();

  QHBoxLayout* horizontalLayout = new QHBoxLayout();
  horizontalLayout->setSpacing(8);
  horizontalLayout->setContentsMargins(-1, 0, -1, -1);

  QLineEdit* Dimensions_0 = new QLineEdit(this);
  Dimensions_0->setObjectName(QString::fromUtf8("0_") + prop);
  QDoubleValidator* val_0 = new QDoubleValidator(this);
  val_0->setDecimals(8);
  Dimensions_0->setValidator(val_0);
  Dimensions_0->setText(QString::number(v3.x));
  horizontalLayout->addWidget(Dimensions_0);

  QLineEdit* Dimensions_1 = new QLineEdit(this);
  Dimensions_1->setObjectName(QString::fromUtf8("1_") + prop);
  QDoubleValidator* val_1 = new QDoubleValidator(this);
  val_1->setDecimals(8);
  Dimensions_1->setValidator(val_1);
  Dimensions_1->setText(QString::number(v3.y));
  horizontalLayout->addWidget(Dimensions_1);

  QLineEdit* Dimensions_2 = new QLineEdit(this);
  Dimensions_2->setObjectName(QString::fromUtf8("2_") + prop);
  QDoubleValidator* val_2 = new QDoubleValidator(this);
  val_2->setDecimals(8);
  Dimensions_2->setValidator(val_2);
  Dimensions_2->setText(QString::number(v3.z));
  horizontalLayout->addWidget(Dimensions_2);

  frmLayout->setLayout(optIndex, QFormLayout::FieldRole, horizontalLayout);
  connect(Dimensions_0, SIGNAL( textChanged(const QString &)), this, SLOT(updateFloatVec3Widget(const QString &)));
  connect(Dimensions_1, SIGNAL( textChanged(const QString &)), this, SLOT(updateFloatVec3Widget(const QString &)));
  connect(Dimensions_2, SIGNAL( textChanged(const QString &)), this, SLOT(updateFloatVec3Widget(const QString &)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::setupAxisAngleWidget(QFormLayout* frmLayout, int optIndex,
                                         FilterParameter* option, QLabel* label )
{
  label->deleteLater();
  AxisAngleWidget* w = new AxisAngleWidget(this);
  w->setObjectName(((option->getPropertyName())));
  frmLayout->setWidget(optIndex, QFormLayout::SpanningRole, w);
  connect(w, SIGNAL(AxisAnglesChanged()), this, SLOT(updateAxisAngleWidget()));
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::setupVertexArrayNameChoiceWidget(QFormLayout* frmLayout, int optIndex,
                                                   FilterParameter* option, QLabel* label )
{
  frmLayout->setWidget(optIndex, QFormLayout::LabelRole, label);
  QComboBox* cb = new QComboBox(this);
  cb->setObjectName((option->getPropertyName()));
  frmLayout->setWidget(optIndex, QFormLayout::FieldRole, cb);
  connect(cb, SIGNAL( currentIndexChanged(int)), this, SLOT(updateArrayNameComboBoxValue(int)));

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::setupEdgeArrayNameChoiceWidget(QFormLayout* frmLayout, int optIndex,
                                                   FilterParameter* option, QLabel* label )
{
  frmLayout->setWidget(optIndex, QFormLayout::LabelRole, label);
  QComboBox* cb = new QComboBox(this);
  cb->setObjectName((option->getPropertyName()));
  frmLayout->setWidget(optIndex, QFormLayout::FieldRole, cb);
  connect(cb, SIGNAL( currentIndexChanged(int)), this, SLOT(updateArrayNameComboBoxValue(int)));

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::setupFaceArrayNameChoiceWidget(QFormLayout* frmLayout, int optIndex,
                                                   FilterParameter* option, QLabel* label )
{
  frmLayout->setWidget(optIndex, QFormLayout::LabelRole, label);
  QComboBox* cb = new QComboBox(this);
  cb->setObjectName((option->getPropertyName()));
  frmLayout->setWidget(optIndex, QFormLayout::FieldRole, cb);
  connect(cb, SIGNAL( currentIndexChanged(int)), this, SLOT(updateArrayNameComboBoxValue(int)));

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::setupCellArrayNameChoiceWidget(QFormLayout* frmLayout, int optIndex,
                                                   FilterParameter* option, QLabel* label )
{
  frmLayout->setWidget(optIndex, QFormLayout::LabelRole, label);
  QComboBox* cb = new QComboBox(this);
  cb->setObjectName((option->getPropertyName()));
  frmLayout->setWidget(optIndex, QFormLayout::FieldRole, cb);
  connect(cb, SIGNAL( currentIndexChanged(int)), this, SLOT(updateArrayNameComboBoxValue(int)));

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::setupFieldArrayNameChoiceWidget(QFormLayout* frmLayout, int optIndex,
                                                    FilterParameter* option, QLabel* label )
{
  frmLayout->setWidget(optIndex, QFormLayout::LabelRole, label);
  QComboBox* cb = new QComboBox(this);
  cb->setObjectName((option->getPropertyName()));
  frmLayout->setWidget(optIndex, QFormLayout::FieldRole, cb);
  connect(cb, SIGNAL( currentIndexChanged(int)), this, SLOT(updateArrayNameComboBoxValue(int)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::setupEnsembleArrayNameChoiceWidget(QFormLayout* frmLayout, int optIndex,
                                                       FilterParameter* option, QLabel* label )
{
  frmLayout->setWidget(optIndex, QFormLayout::LabelRole, label);
  QComboBox* cb = new QComboBox(this);
  cb->setObjectName((option->getPropertyName()));
  frmLayout->setWidget(optIndex, QFormLayout::FieldRole, cb);
  connect(cb, SIGNAL( currentIndexChanged(int)), this, SLOT(updateArrayNameComboBoxValue(int)));
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::setupArraySelectionWidget(QFormLayout* frmLayout, int optIndex,
                                              FilterParameter* option, QLabel* label )
{
  label->deleteLater();
  ArraySelectionWidget* w = new ArraySelectionWidget(this);
  w->setObjectName(((option->getPropertyName())));
  frmLayout->setWidget(optIndex, QFormLayout::SpanningRole, w);
  connect(w, SIGNAL(arrayListsChanged()), this, SLOT(updateArraySelectionWidget()));
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::setupComparisonArraysWidget(QFormLayout* frmLayout, int optIndex, FilterParameter* option, QLabel* label,
                                                FilterParameter::WidgetType arrayListType)
{
  ComparisonFilterParameter* param = ComparisonFilterParameter::SafePointerDownCast(option);
  if (NULL == param) { return; }

  ComparisonSelectionWidget* w = new ComparisonSelectionWidget(param->getShowOperators(), this);
  switch(arrayListType)
  {
    case FilterParameter::CellArrayComparisonSelectionWidget:
      w->setArrayListType(ComparisonSelectionWidget::CellListType);
      break;
    case FilterParameter::FieldArrayComparisonSelectionWidget:
      w->setArrayListType(ComparisonSelectionWidget::FieldListType);
      break;
    case FilterParameter::EnsembleArrayComparisonSelectionWidget:
      w->setArrayListType(ComparisonSelectionWidget::EnsembleListType);
      break;
    case FilterParameter::VertexArrayComparisonSelectionWidget:
      w->setArrayListType(ComparisonSelectionWidget::VertexListType);
      break;
    case FilterParameter::EdgeArrayComparisonSelectionWidget:
      w->setArrayListType(ComparisonSelectionWidget::EdgeListType);
      break;
    case FilterParameter::FaceArrayComparisonSelectionWidget:
      w->setArrayListType(ComparisonSelectionWidget::FaceListType);
      break;
    default:
      break;
  }


  w->setHumanLabel(label->text());
  w->setObjectName(((option->getPropertyName())));
  frmLayout->setWidget(optIndex, QFormLayout::SpanningRole, w);
  connect(w, SIGNAL(parametersChanged()), this, SLOT(updateComparisonSelectionWidget()));
  label->deleteLater();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateIntVec3Widget(const QString &v)
{
  QObject* whoSent = sender();
  QString baseName = whoSent->objectName();
  baseName = baseName.mid(2); // Grab everything to the RIGHT of the index=2 element

  bool ok = false;

  IntVec3Widget_t value;
  value.x = 0;
  value.y = 0;
  value.z = 0;

  QString name = QString::fromUtf8("0_") + baseName;
  QLineEdit* le = qFindChild<QLineEdit*>(this, name);
  if(le) {
    value.x = le->text().toInt(&ok);
    if (false == ok) {
      value.x = 0;
      le->setText("0");
    }
  }

  name = QString::fromUtf8("1_") + baseName;
  le = qFindChild<QLineEdit*>(this, name);
  if(le) {
    value.y = le->text().toInt(&ok);
    if (false == ok) {
      value.y = 0;
      le->setText("0");
    }
  }

  name = QString::fromUtf8("2_") + baseName;
  le = qFindChild<QLineEdit*>(this, name);
  if(le) {
    value.z = le->text().toInt(&ok);
    if (false == ok) {
      value.z = 0;
      le->setText("0");
    }
  }

  QVariant variant;
  variant.setValue(value);
  setProperty(baseName.toAscii(), variant);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateFloatVec3Widget(const QString &v)
{
  QObject* whoSent = sender();
  QString baseName = whoSent->objectName();
  baseName = baseName.mid(2); // Grab everything to the RIGHT of the index=2 element

  bool ok = false;

  FloatVec3Widget_t value;
  value.x = 0.0;
  value.y = 0.0;
  value.z = 0.0;

  QString name = QString::fromUtf8("0_") + baseName;
  QLineEdit* le = qFindChild<QLineEdit*>(this, name);
  if(le) {
    value.x = le->text().toFloat(&ok);
    if (false == ok) {
      value.x = 0;
      le->setText("0");
    }
  }

  name = QString::fromUtf8("1_") + baseName;
  le = qFindChild<QLineEdit*>(this, name);
  if(le) {
    value.y = le->text().toFloat(&ok);
    if (false == ok) {
      value.y = 0;
      le->setText("0");
    }
  }

  name = QString::fromUtf8("2_") + baseName;
  le = qFindChild<QLineEdit*>(this, name);
  if(le) {
    value.z = le->text().toFloat(&ok);
    if (false == ok) {
      value.z = 0;
      le->setText("0");
    }
  }

  QVariant variant;
  variant.setValue(value);
  setProperty(baseName.toAscii(), variant);
}

// -----------------------------------------------------------------------------
// This slot is just here to
// -----------------------------------------------------------------------------
void QFilterWidget::updateArraySelectionWidget()
{
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateComparisonSelectionWidget()
{
  emit parametersChanged();
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
AbstractFilter::Pointer QFilterWidget::getFilter(bool defaultValues)
{
  return AbstractFilter::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateQLineEditIntValue()
{
  QObject* whoSent = sender();
  //  qDebug() << "Filter: " << title() << " Getting updated from whoSent Name: "
  //      << whoSent->objectName()<< "\n";
  QLineEdit* le = qobject_cast<QLineEdit*>(whoSent);
  if(le) {
    bool ok = false;
    int value = le->text().toInt(&ok);
    if (false == ok) {
      value = 0;
      le->setText("0");
    }
    setProperty(whoSent->objectName().toLatin1().data(), value);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateQLineEditDoubleValue()
{
  QObject* whoSent = sender();
  //  qDebug() << "Filter: " << title() << " Getting updated from whoSent Name: "
  //     << whoSent->objectName()<< "\n";
  QLineEdit* le = qobject_cast<QLineEdit*>(whoSent);
  if(le) {
    bool ok = false;
    double value = le->text().toDouble(&ok);
    if (false == ok) {
      value = 0;
      le->setText("0");
    }
    setProperty(whoSent->objectName().toLatin1().data(), value);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::selectInputFile()
{
  QObject* whoSent = sender();
  // for QButtons we prepended "btn_" to the end of the property name so strip that off
  QString propName = whoSent->objectName();
  propName = propName.remove(0, 4);

  QString Ftype = getFileType(propName);
  QString ext = getFileExtension(propName);
  QString s = Ftype + QString(" Files (") + ext + QString(");;All Files(*.*)");
  QString defaultName = m_OpenDialogLastDirectory + QDir::separator() + "Untitled";
  QString file = QFileDialog::getOpenFileName(this, tr("Select Input File"), defaultName, s);

  if(true == file.isEmpty())
  {
    return;
  }
  bool ok = false;
  file = QDir::toNativeSeparators(file);
  // Store the last used directory into the private instance variable
  QFileInfo fi(file);
  m_OpenDialogLastDirectory = fi.path();

  ok = setProperty(propName.toLatin1().data(), file);
  if (true == ok) { }
  else
  {
    //  qDebug() << "QPushButton '" << title() <<  "'Property: '" << whoSent->objectName() << "' was NOT set."<< "\n";
  }
  // Now we need to find the specific filter that we are trying to set the value into
  AbstractFilter::Pointer f = getFilter(false);
  QVector<FilterParameter::Pointer> opts = f->getFilterParameters();
  for (QVector<FilterParameter::Pointer>::iterator iter = opts.begin(); iter != opts.end(); ++iter)
  {
    if((*iter)->getPropertyName().compare(propName) == 0)
    {
      QLineEdit* lb = qFindChild<QLineEdit*>(this, ((*iter)->getPropertyName()));
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

  ok = setProperty(propName.toLatin1().data(), file);
  if (true == ok) { }
  else
  {
    //  qDebug() << "QPushButton '" << title() <<  "'Property: '" << whoSent->objectName() << "' was NOT set."<< "\n";
  }
  // Now we need to find the specific filter that we are trying to set the value into
  AbstractFilter::Pointer f = getFilter(false);
  QVector<FilterParameter::Pointer> opts = f->getFilterParameters();
  for (QVector<FilterParameter::Pointer>::iterator iter = opts.begin(); iter != opts.end(); ++iter)
  {
    if((*iter)->getPropertyName().compare(propName) == 0)
    {
      QLineEdit* lb = qFindChild<QLineEdit*>(this, ((*iter)->getPropertyName()));
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

  QString Ftype = getFileType(propName);
  QString ext = getFileExtension(propName);
  QString s = Ftype + QString(" Files (") + ext + QString(");;All Files(*.*)");
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

  ok = setProperty(propName.toLatin1().data(), file);
  if (true == ok) {}
  else
  {
    //  qDebug() << "QPushButton '" << title() <<  "'Property: '" << whoSent->objectName() << "' was NOT set."<< "\n";
  }

  // Now we need to find the specific filter that we are trying to set the value into
  AbstractFilter::Pointer f = getFilter(false);
  // Get the options for that filter
  QVector<FilterParameter::Pointer> opts = f->getFilterParameters();
  // Loop on all the filter options to find the filter option we want to set
  for (QVector<FilterParameter::Pointer>::iterator iter = opts.begin(); iter != opts.end(); ++iter)
  {
    if((*iter)->getPropertyName().compare(propName) == 0)
    {
      QLineEdit* lb = qFindChild<QLineEdit*>(this, ((*iter)->getPropertyName()));
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

  ok = setProperty(propName.toLatin1().data(), file);
  if (true == ok) {}
  else
  {
    //  qDebug() << "QPushButton '" << title() <<  "'Property: '" << whoSent->objectName() << "' was NOT set."<< "\n";
  }

  // Now we need to find the specific filter that we are trying to set the value into
  AbstractFilter::Pointer f = getFilter(false);
  // Get the options for that filter
  QVector<FilterParameter::Pointer> opts = f->getFilterParameters();
  // Loop on all the filter options to find the filter option we want to set
  for (QVector<FilterParameter::Pointer>::iterator iter = opts.begin(); iter != opts.end(); ++iter)
  {
    if((*iter)->getPropertyName().compare(propName) == 0)
    {
      QLineEdit* lb = qFindChild<QLineEdit*>(this, ((*iter)->getPropertyName()));
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
  //  qDebug() << "Filter: " << title() << " Getting updated from whoSent Name: " << whoSent->objectName()<< "\n";
  QComboBox* cb = qobject_cast<QComboBox*>(whoSent);
  if(cb)
  {
    bool ok = false;
    ok = setProperty(whoSent->objectName().toLatin1().data(), v);
    if (true == ok) { }
    else
    {
      qDebug() << "QComboBox '" << title() << "'Property: '" << whoSent->objectName() << "' was NOT set." << "\n";
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateArrayNameComboBoxValue(int v)
{
  QObject* whoSent = sender();
  // qDebug() << "Filter: " << title() << " Getting updated from whoSent Name: " << whoSent->objectName()<< "\n";
  QComboBox* cb = qobject_cast<QComboBox*>(whoSent);
  if(cb)
  {
    bool ok = false;
    QString text = cb->itemText(v);
    ok = setProperty(whoSent->objectName().toLatin1().data(), text);
    if (true == ok) { }
    else
    {
      qDebug() << "QComboBox '" << title() << "'Property: '" << whoSent->objectName() << "' was NOT set."<< "\n";
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateArrayNameComboBoxValue(const QString &text)
{
  QObject* whoSent = sender();
  //qDebug() << "Filter: " << title() << " Getting updated from whoSent Name: " << whoSent->objectName()<< "\n";
  QComboBox* cb = qobject_cast<QComboBox*>(whoSent);
  if(cb)
  {
    bool ok = false;
    ok = setProperty(whoSent->objectName().toLatin1().data(), text);
    if (true == ok) { }
    else
    {
      qDebug() << "QComboBox '" << title() << "'Property: '" << whoSent->objectName() << "' was NOT set."<< "\n";
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateQLineEditStringValue(const QString &v)
{
  QObject* whoSent = sender();
  //  qDebug() << "Filter: " << title() << " Getting updated from whoSent Name: "
  //      << whoSent->objectName()<< "\n";
  QLineEdit* le = qobject_cast<QLineEdit*>(whoSent);
  if(le)
  {
    setProperty(whoSent->objectName().toLatin1().data(), le->text());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateQSpinBoxValue(int v)
{
  BOOST_ASSERT(false);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateQDoubleSpinBoxValue(double v)
{
  BOOST_ASSERT(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateQCheckBoxValue(int v)
{
  QObject* whoSent = sender();
  //  qDebug() << "Filter: " << title() << "->Property: " << whoSent->objectName()
  //      << " via QCheckBox." << "\n";
  QCheckBox* le = qobject_cast<QCheckBox*>(whoSent);
  if(le)
  {
    setProperty(whoSent->objectName().toLatin1().data(), le->isChecked());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateLineEdit(const QString &v)
{
  QObject* whoSent = sender();
  qDebug() << "Filter: " << title() << "->Property: " << whoSent->objectName()
            << " via QLineEdit." << "\n";
  BOOST_ASSERT(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateAxisAngleWidget()
{
  emit parametersChanged();
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
  //    qDebug() << "Drag should close the widget because it was MOVE"<< "\n";
  //  }
  //  else
  //  {
  //    qDebug() << "Drag should leave Widget alone because it was COPY"<< "\n";
  //  }
  drag->exec(Qt::MoveAction);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QFilterWidget::getFileExtension(QString propName)
{
  QVector<FilterParameter::Pointer> options = getFilter(false)->getFilterParameters();
  //  int optIndex = 0;
  for (QVector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
  {
    FilterParameter* option = (*iter).get();
    if(option->getPropertyName().compare(propName) == 0)
    {
      return (option->getFileExtension());
    }
  }
  return QString("");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QFilterWidget::getFileType(QString propName)
{
  QVector<FilterParameter::Pointer> options = getFilter(false)->getFilterParameters();
  //int optIndex = 0;
  for (QVector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
  {
    FilterParameter* option = (*iter).get();
    if(option->getPropertyName().compare(propName) == 0)
    {
      return (option->getFileType());
    }
  }
  return QString("");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::preflightAboutToExecute(VolumeDataContainer::Pointer vldc, SurfaceDataContainer::Pointer sdc, EdgeDataContainer::Pointer edc, VertexDataContainer::Pointer vdc)
{
  // This section will update any combo boxes that are "Array Name" lists for the Voxel arrays.
  QVector<FilterParameter::Pointer> options = getFilter(false)->getFilterParameters();
  for (QVector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
  {
    FilterParameter* option = (*iter).get();
    FilterParameter::WidgetType wType = option->getWidgetType();
    QString propertyName = (option->getPropertyName());

    switch(wType)
    {
      case FilterParameter::VolumeVertexArrayNameSelectionWidget:
        updateArrayNameComboBox(vldc->getVertexArrayNameList(), propertyName);
        break;
      case FilterParameter::VolumeEdgeArrayNameSelectionWidget:
        updateArrayNameComboBox(vldc->getEdgeArrayNameList(), propertyName);
        break;
      case FilterParameter::VolumeFaceArrayNameSelectionWidget:
        updateArrayNameComboBox(vldc->getFaceArrayNameList(), propertyName);
        break;
      case FilterParameter::VolumeCellArrayNameSelectionWidget:
        updateArrayNameComboBox(vldc->getCellArrayNameList(), propertyName);
        break;
      case FilterParameter::VolumeFieldArrayNameSelectionWidget:
        updateArrayNameComboBox(vldc->getFieldArrayNameList(), propertyName);
        break;
      case FilterParameter::VolumeEnsembleArrayNameSelectionWidget:
        updateArrayNameComboBox(vldc->getEnsembleArrayNameList(), propertyName);
        break;
      case FilterParameter::SurfaceVertexArrayNameSelectionWidget:
        updateArrayNameComboBox(sdc->getVertexArrayNameList(), propertyName);
        break;
      case FilterParameter::SurfaceEdgeArrayNameSelectionWidget:
        updateArrayNameComboBox(sdc->getEdgeArrayNameList(), propertyName);
        break;
      case FilterParameter::SurfaceFaceArrayNameSelectionWidget:
        updateArrayNameComboBox(sdc->getFaceArrayNameList(), propertyName);
        break;
      case FilterParameter::SurfaceFieldArrayNameSelectionWidget:
        updateArrayNameComboBox(sdc->getFieldArrayNameList(), propertyName);
        break;
      case FilterParameter::SurfaceEnsembleArrayNameSelectionWidget:
        updateArrayNameComboBox(sdc->getEnsembleArrayNameList(), propertyName);
        break;
      case FilterParameter::EdgeVertexArrayNameSelectionWidget:
        updateArrayNameComboBox(edc->getVertexArrayNameList(), propertyName);
        break;
      case FilterParameter::EdgeEdgeArrayNameSelectionWidget:
        updateArrayNameComboBox(edc->getEdgeArrayNameList(), propertyName);
        break;
      case FilterParameter::EdgeFieldArrayNameSelectionWidget:
        updateArrayNameComboBox(edc->getFieldArrayNameList(), propertyName);
        break;
      case FilterParameter::EdgeEnsembleArrayNameSelectionWidget:
        updateArrayNameComboBox(edc->getEnsembleArrayNameList(), propertyName);
        break;
      case FilterParameter::VertexVertexArrayNameSelectionWidget:
        updateArrayNameComboBox(vdc->getVertexArrayNameList(), propertyName);
        break;
      case FilterParameter::VertexFieldArrayNameSelectionWidget:
        updateArrayNameComboBox(vdc->getFieldArrayNameList(), propertyName);
        break;
      case FilterParameter::VertexEnsembleArrayNameSelectionWidget:
        updateArrayNameComboBox(vdc->getEnsembleArrayNameList(), propertyName);
        break;
      case FilterParameter::ArraySelectionWidget:
        //     updateArraySelectionWidget(vdc, smdc, sdc, propertyName);
        break;
      default:
        break;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::preflightDoneExecuting(VolumeDataContainer::Pointer vldc, SurfaceDataContainer::Pointer sdc, EdgeDataContainer::Pointer edc, VertexDataContainer::Pointer vdc)
{
  //  qDebug() << "void QFilterWidget::preflightDoneExecuting(...)"<< "\n";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateArrayNameComboBox(const QList<QString> &arrayNames, QString propertyName)
{
  //if (arrayNames.size() == 0) { return; }
  QComboBox* cb = qFindChild<QComboBox*>(this, propertyName);
  if (NULL == cb) { return; }
  QString selectedName = cb->currentText();
  cb->blockSignals(true);
  cb->clear();
  int index = 0;
  for(QList<QString>::const_iterator iter = arrayNames.begin(); iter != arrayNames.end(); ++iter)
  {
    QString name = (*iter);
    cb->addItem(name);
    if (name.compare(selectedName) == 0)
    {
      index = cb->count()-1;
    }
  }
  cb->setCurrentIndex(index);
  cb->blockSignals(false);

  // This is implemented by subclasses of QFilter Widget if they need this implemented.
  arrayNameComboBoxUpdated(cb);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateArraySelectionWidget(VolumeDataContainer::Pointer vdc,
                                               SurfaceDataContainer::Pointer smdc,
                                               VertexDataContainer::Pointer sdc,
                                               QString propertyName)
{
  //  ArraySelectionWidget* w = qFindChild<ArraySelectionWidget*>(this, propertyName);
  //  if (NULL == w) { return; }
  //  w->updateArrays(vdc, smdc, sdc);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::arrayNameComboBoxUpdated(QComboBox* cb)
{
  AbstractFilter::Pointer filt = AbstractFilter::New();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::getGuiParametersFromFilter(AbstractFilter* filt)
{
  BOOST_ASSERT("QFilterWidget::getGuiParametersFromFilter executed");    // Code should never enter this function
}
