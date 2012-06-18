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

#include "DREAM3DLib/Common/FilterOption.h"

#define PADDING 5
#define BORDER 2
#define IMAGE_WIDTH 17
#define IMAGE_HEIGHT 17




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterWidget::QFilterWidget(QWidget* parent) :
      QGroupBox(parent),
      m_CurrentBorderColorFactor(0),
      m_BorderIncrement(16),
      m_IsSelected(false),
      m_HasPreflightErrors(false)
{
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
    m_timer->start(100);
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
  else if(m_IsSelected == true )
  {
    style.append("border: 2px solid purple;");
    m_CurrentBorderColorFactor = 0;
  }
  else
  {
    style.append("border: 1px solid gray;");
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

  style.append("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #FFFFFF, stop: 1 #DCDCDC);");
  style.append("background-image: url(:/filterWidgetBorder.png);");
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


  QVBoxLayout* vertLayout = new QVBoxLayout(this);

  QFormLayout* frmLayout = new QFormLayout();
  vertLayout->addLayout(frmLayout);
  frmLayout->setObjectName("QFilterWidget QFormLayout Layout");

  setIsSelected(false);
  bool ok = false;

  std::vector<FilterOption::Pointer> options = getFilter()->getFilterOptions();
  int optIndex = 0;
  for (std::vector<FilterOption::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
  {
    FilterOption* option = (*iter).get();
    FilterOption::WidgetType wType = option->getWidgetType();

    if (wType == FilterOption::StringWidget)
    {
      QLineEdit* le = new QLineEdit(this);
      le->setObjectName(QString::fromStdString(option->getPropertyName()));
      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, new QLabel(QString::fromStdString(option->getHumanLabel()), this));
      frmLayout->setWidget(optIndex, QFormLayout::FieldRole, le);
      connect(le, SIGNAL(textChanged(QString)), this, SLOT(updateQLineEditStringValue(const QString &)));
      QVariant v = property(option->getPropertyName().c_str());
      le->setText(v.toString());
    }
    else if (wType == FilterOption::IntWidget)
    {
      QLineEdit* le = new QLineEdit(this);
      le->setObjectName(QString::fromStdString(option->getPropertyName()));
      QIntValidator* ival = new QIntValidator(this);
      le->setValidator(ival);
      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, new QLabel(QString::fromStdString(option->getHumanLabel()), this));
      frmLayout->setWidget(optIndex, QFormLayout::FieldRole, le);
      connect(le, SIGNAL(textChanged(QString)), this, SLOT(updateQLineEditIntValue()));
      QVariant v = property(option->getPropertyName().c_str());
      le->setText(v.toString());
    }
    else if (wType == FilterOption::DoubleWidget)
    {
      QLineEdit* le = new QLineEdit(this);
      le->setObjectName(QString::fromStdString(option->getPropertyName()));
      QDoubleValidator* ival = new QDoubleValidator(this);
      le->setValidator(ival);
      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, new QLabel(QString::fromStdString(option->getHumanLabel()), this));
      frmLayout->setWidget(optIndex, QFormLayout::FieldRole, le);
      connect(le, SIGNAL(textChanged(QString)), this, SLOT(updateQLineEditDoubleValue()));
      QVariant v = property(option->getPropertyName().c_str());
      le->setText(v.toString());
    }
    else if (wType == FilterOption::InputFileWidget)
    {
      QGridLayout* gridLayout = new QGridLayout();
      gridLayout->setContentsMargins(0,0,0,0);

      QLabel* label = new QLabel(QString::fromStdString(option->getHumanLabel()));
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
    else if (wType == FilterOption::OutputFileWidget)
    {
      QGridLayout* gridLayout = new QGridLayout();
      gridLayout->setContentsMargins(0,0,0,0);

      QLabel* label = new QLabel(QString::fromStdString(option->getHumanLabel()));
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
    else if (wType == FilterOption::BooleanWidget)
    {
      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, new QLabel(QString::fromStdString(option->getHumanLabel()), this));
      QCheckBox* le = new QCheckBox(this);
      le->setObjectName(QString::fromStdString(option->getPropertyName()));
      frmLayout->setWidget(optIndex, QFormLayout::FieldRole, le);
      connect(le, SIGNAL(stateChanged(int)), this, SLOT(updateQCheckBoxValue(int)));
      QVariant v = property(option->getPropertyName().c_str());
      le->setChecked(v.toBool());
    }
    else if (wType == FilterOption::IntConstrainedWidget)
    {
      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, new QLabel(QString::fromStdString(option->getHumanLabel()), this));
      QSpinBox* le = new QSpinBox(this);
      le->setObjectName(QString::fromStdString(option->getPropertyName()));
      ConstrainedFilterOption<int>* filtOpt = dynamic_cast<ConstrainedFilterOption<int>* >(option);
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
    else if (wType == FilterOption::DoubleConstrainedWidget)
    {
      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, new QLabel(QString::fromStdString(option->getHumanLabel()), this));
      QDoubleSpinBox* le = new QDoubleSpinBox(this);
      le->setObjectName(QString::fromStdString(option->getPropertyName()));
      ConstrainedFilterOption<float>* filtOpt = dynamic_cast<ConstrainedFilterOption<float>* >(option);
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
    else if (wType == FilterOption::ChoiceWidget)
    {
      ChoiceFilterOption* choiceFilterOption = ChoiceFilterOption::SafeObjectDownCast<FilterOption*, ChoiceFilterOption*>(option);
      if (NULL == choiceFilterOption) { return; }
      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, new QLabel(QString::fromStdString(option->getHumanLabel()), this));
      QComboBox* cb = new QComboBox(this);
      cb->setObjectName(QString::fromStdString(option->getPropertyName()));
      std::vector<std::string> choices = choiceFilterOption->getChoices();
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
    setProperty(whoSent->objectName().toStdString().c_str(), le->text().toInt(&ok));
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
    ok = setProperty(whoSent->objectName().toStdString().c_str(), le->text().toDouble(&ok));
    if (true == ok) { }
    else
    {
  //    std::cout << "QLineEdit '" << title().toStdString() <<  "'Property: '" << whoSent->objectName().toStdString() << "' was NOT set."<< std::endl;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::selectInputFile()
{
  QObject* whoSent = sender();

  QString file = QFileDialog::getOpenFileName(this, tr("Select Input File"), "", tr("ALL Files (*.*)"));
  if(true == file.isEmpty())
  {
    return;
  }
  bool ok = false;
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
  std::vector<FilterOption::Pointer> opts = f->getFilterOptions();
  for (std::vector<FilterOption::Pointer>::iterator iter = opts.begin(); iter != opts.end(); ++iter)
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
  QString file = QFileDialog::getSaveFileName(this, tr("Save File As"), "", tr("ALL Files (*.*)"));
  if(true == file.isEmpty())
  {
    return;
  }
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
  std::vector<FilterOption::Pointer> opts = f->getFilterOptions();
  // Loop on all the filter options to find the filter option we want to set
  for (std::vector<FilterOption::Pointer>::iterator iter = opts.begin(); iter != opts.end(); ++iter)
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
#if 0
void QFilterWidget::selectOutputFile(QString file)
{
  if (true == file.isEmpty()) { return;}
  QObject* whoSent = sender();
  bool ok = false;
    ok = setProperty(whoSent->objectName().toStdString().c_str(), file);
}
#endif

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



