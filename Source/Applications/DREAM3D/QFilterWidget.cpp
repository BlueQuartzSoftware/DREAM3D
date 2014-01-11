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
#include <QtCore/QDir>

//#include <QtGui/QFrame>
//#include <QtGui/QSpinBox>
#include <QtGui/QLabel>
//#include <QtGui/QCheckBox>
//#include <QtGui/QLineEdit>
//#include <QtGui/QIntValidator>
//#include <QtGui/QDoubleValidator>
//#include <QtGui/QComboBox>
//#include <QtGui/QApplication>
//#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QFormLayout>
//#include <QtGui/QGridLayout>
//#include <QtGui/QPainter>
//#include <QtGui/QPushButton>
//#include <QtGui/QFileDialog>
#include <QtGui/QMouseEvent>
//#include <QtGui/QDesktopServices>
//#include <QtGui/QMessageBox>
#include <QtGui/QPainter>


#include "QtSupport/QR3DFileCompleter.h"
#include "QtSupport/QFSDropLineEdit.h"
#include "QtSupport/DREAM3DHelpUrlGenerator.h"

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"


#if 0
#include "ArraySelectionWidget.h"
#include "ComparisonSelectionWidget.h"
#include "AxisAngleWidget.h"
#endif

#define PADDING 5
#define BORDER 2
#define IMAGE_WIDTH 17
#define IMAGE_HEIGHT 17

// These Streams need to be implemented so that our 3 Element Vectors can be read/write to disk/prefs files
QDataStream& operator<<( QDataStream& out, const IntVec3Widget_t& v)
{
  out << v.x << v.y << v.z;
  return out;
}
QDataStream& operator>>( QDataStream& in, IntVec3Widget_t& v) { in >> v.x >> v.y >> v.z; return in; }

QDataStream& operator<<( QDataStream& out, const FloatVec3Widget_t& v) { out << v.x << v.y << v.z; return out; }
QDataStream& operator>>( QDataStream& in, FloatVec3Widget_t& v) { in >> v.x >> v.y >> v.z; return in; }

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

  m_Filter = AbstractFilter::NullPointer();
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
  style.append("padding: 0 0 0 0px;");
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
void QFilterWidget::initializeWithFilter(QString filterClassName)
{

  FilterManager::Pointer wm = FilterManager::Instance();
  if(NULL == wm.get() ) { return; }
  IFilterFactory::Pointer wf = wm->getFactoryForFilter(filterClassName);
  if (NULL == wf.get()) { return; }

  // Create an instance of the filter. Since we are dealing with the AbstractFilter interface we can not
  // actually use the actual filter class. We are going to have to rely on QProperties or Signals/Slots
  // to communicate changes back to the filter.
  m_Filter = wf->create();
  m_FilterGroup = m_Filter->getGroupName();
  m_FilterSubGroup = m_Filter->getSubGroupName();
  setTitle(m_Filter->getHumanLabel());

  // We need to initialize each of the parameter widgets with the default values from the filter instance

  setCheckable(true);

  delete layout();


  QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Maximum);
  sizePolicy2.setHorizontalStretch(0);
  sizePolicy2.setVerticalStretch(0);
  setSizePolicy(sizePolicy2);

  QVBoxLayout* vertLayout = new QVBoxLayout(this);

  QFormLayout* frmLayout = new QFormLayout();
  vertLayout->addLayout(frmLayout);
  frmLayout->setObjectName("QFilterWidget QFormLayout Layout");

  setIsSelected(false);

  QVector<FilterParameter::Pointer> options = m_Filter->getFilterParameters();
  int optIndex = 0;
  for (QVector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
  {
  #if 0
    FilterParameter* option = (*iter).get();
    FilterParameterWidgetType::WidgetType wType = option->getWidgetType();

    QString labelName = (option->getHumanLabel());
    if (option->getUnits().isEmpty() == false)
    {
      labelName.append(" (").append((option->getUnits())).append(")");
    }
    QLabel* label = new QLabel(labelName, this);

    // if (wType == FilterParameterWidgetType::StringWidget)
    {
      QLineEdit* le = new QLineEdit(this);
      le->setObjectName(option->getPropertyName());

      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, label);
      frmLayout->setWidget(optIndex, QFormLayout::FieldRole, le);
      //connect(le, SIGNAL(textChanged(QString)), this, SLOT(updateQLineEditStringValue(const QString &)));
      //QVariant v = property(option->getPropertyName());
      le->setText(option->getPropertyName());
    }
#endif
    ++optIndex;
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer QFilterWidget::getFilter()
{
  return m_Filter;
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
void QFilterWidget::mouseReleaseEvent(QMouseEvent* event)
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
    emit widgetSelected(getFilter().get());
    setIsSelected(true);
    event->setAccepted(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::mouseMoveEvent(QMouseEvent* event)
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

  QMimeData* mimeData = new QMimeData;
  mimeData->setData("application/x-dnditemdata", itemData);

  QDrag* drag = new QDrag(this);
  drag->setMimeData(mimeData);
  drag->setPixmap(transparent);
  drag->setHotSpot(event->pos());

  emit dragStarted(this);

  //  if(drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction)
  //  {
  //    qDebug() << "Drag should close the widget because it was MOVE" << "\n";
  //  }
  //  else
  //  {
  //    qDebug() << "Drag should leave Widget alone because it was COPY" << "\n";
  //  }
  drag->exec(Qt::MoveAction);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::getGuiParametersFromFilter(AbstractFilter* filt)
{
  BOOST_ASSERT("QFilterWidget::getGuiParametersFromFilter executed");    // Code should never enter this function
}
