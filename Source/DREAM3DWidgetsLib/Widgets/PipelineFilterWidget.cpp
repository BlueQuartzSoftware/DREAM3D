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

#include "PipelineFilterWidget.h"



#include <QtCore/QTimer>
#include <QtCore/QResource>
#include <QtCore/QDir>

#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>



#include "QtSupport/DREAM3DHelpUrlGenerator.h"

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"



#include "DREAM3DWidgetsLib/FilterWidgetManager.h"


// -----------------------------------------------------------------------------
// Include the "moc" file that was generated for this file
// -----------------------------------------------------------------------------
#include "DREAM3DWidgetsLib/moc_PipelineFilterWidget.cpp"

#define PADDING 5
#define BORDER 2
#define IMAGE_WIDTH 17
#define IMAGE_HEIGHT 17



// Initialize private static member variable
QString PipelineFilterWidget::m_OpenDialogLastDirectory = "";



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineFilterWidget::PipelineFilterWidget(QWidget* parent) :
  QFrame(parent),
  m_CurrentBorderColorFactor(0),
  m_BorderIncrement(16),
  m_IsSelected(false),
  m_HasPreflightErrors(false),
  m_HasPreflightWarnings(false),
  m_BasicInputsScrollWidget(NULL),
  m_Observer(NULL)
{
  initialize(AbstractFilter::NullPointer() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineFilterWidget::PipelineFilterWidget(AbstractFilter::Pointer filter, IObserver* observer, QWidget* parent) :
  QFrame(parent),
  m_CurrentBorderColorFactor(0),
  m_BorderIncrement(16),
  m_IsSelected(false),
  m_HasPreflightErrors(false),
  m_HasPreflightWarnings(false),
  m_BasicInputsScrollWidget(NULL),
  m_Observer(observer)
{
  initialize(filter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterWidget::initialize(AbstractFilter::Pointer filter)
{
  setupUi(this);

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

  // Set the AbstractFilter for this class
  m_Filter = filter;


  // If the filter is valid then instantiate all the FilterParameterWidgets
  if (NULL != m_Filter.get())
  {
    // Create the Widget that will be placed into the Basic Inputs Scroll Area
    m_BasicInputsScrollWidget = new QWidget(this);
    QString name = QString::fromUtf8("basicInputsScrollWidget_") + m_Filter->getNameOfClass();
    m_BasicInputsScrollWidget->setObjectName(name);
    m_BasicInputsScrollWidget->setGeometry(QRect(0, 0, 250, 267));
    QVBoxLayout* verticalLayout = new QVBoxLayout(m_BasicInputsScrollWidget);
    name = QString::fromUtf8("verticalLayout") + m_Filter->getNameOfClass();
    verticalLayout->setObjectName(name);


    // Set the Name of the filter into the FilterWidget
    filterName->setText(m_Filter->getHumanLabel() );

    // Create all the FilterParameterWidget objects that can be displayed where ever the developer needs
    FilterWidgetManager::Pointer fwm = FilterWidgetManager::Instance();

    QVector<FilterParameter::Pointer> options = m_Filter->getFilterParameters();
    for (QVector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
    {

      FilterParameter* option = (*iter).get();

      QWidget* w = fwm->createWidget(option, m_Filter.get());

      if (NULL == w) continue;
      m_FilterParameterWidgets.push_back(w);
      // Set the parent for the widget
      w->setParent(m_BasicInputsScrollWidget);
      // Add the FilterWidget to the layout
      verticalLayout->addWidget(w);

      connect(w, SIGNAL(parametersChanged() ),
              parent(), SLOT(preflightPipeline() ) );
      connect(w, SIGNAL(errorSettingFilterParameter(const QString&)),
              this, SLOT(displayFilterParameterWidgetError(const QString&)));

    }

    QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verticalLayout->addItem(verticalSpacer);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterWidget::displayFilterParameterWidgetError(const QString& msg)
{
 if(m_Observer)
 {
   PipelineMessage pm("Filter Paramter Widget", msg, -1, PipelineMessage::Error);
    m_Observer->processPipelineMessage(pm);
 }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineFilterWidget::~PipelineFilterWidget()
{
  // std::cout << "~PipelineFilterWidget() " << m_Filter->getNameOfClass().toStdString() << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterWidget::on_helpBtn_clicked()
{
  DREAM3DHelpUrlGenerator::generateAndOpenHTMLUrl( m_Filter->getNameOfClass().toLower(), this );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineFilterWidget::getHumanLabel()
{
  if (NULL != m_Filter.get())
    return m_Filter->getHumanLabel();

  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineFilterWidget::getFilterGroup()
{
  if (NULL != m_Filter.get())
    return m_Filter->getGroupName();

  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineFilterWidget::getFilterSubGroup()
{
  if (NULL != m_Filter.get())
    return m_Filter->getSubGroupName();

  return "";
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QWidget*>& PipelineFilterWidget::getFilterParameterWidgets()
{
  return m_FilterParameterWidgets;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* PipelineFilterWidget::getScrollWidgetContents()
{
  return m_BasicInputsScrollWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterWidget::setBorderColorStyle(QString s)
{
  m_BorderColorStyle = s;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineFilterWidget::getBorderColorStyle()
{
  return m_BorderColorStyle;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterWidget::setHasPreflightErrors(bool hasErrors)
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
void PipelineFilterWidget::setHasPreflightWarnings(bool hasWarnings)
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
void PipelineFilterWidget::setIsSelected(bool b)
{
  m_IsSelected = b;
  changeStyle();
  if(true == b) { emit widgetSelected(this); }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineFilterWidget::isSelected()
{
  return m_IsSelected;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterWidget::changeStyle()
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
void PipelineFilterWidget::updateWidgetStyle()
{
  QString style;

  style.append("PipelineFilterWidget {\n");

  style.append("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(185, 185, 185, 255), stop:0.5 rgba(226, 226, 226, 255), stop:1 rgba(150, 150, 150, 255));\n");

#if 0
  QString headerFile(":/PipelineFilterWidgetHeaders/");

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


    style.append("background-position: top ;\n background-repeat: repeat-x;");

  style.append(getBorderColorStyle());

  style.append("border-radius: 10px;");
  style.append("padding: 0 0 0 0px;");
  style.append("}\n");

  style.append("QLabel\n {\n");

#if defined(Q_WS_WIN)
  style.append("font: 75 italic 10pt \"Arial\";");
#elif defined(Q_WS_MAC)
  style.append("font: 100 italic 12pt \"Arial\";");
#else
  style.append("font: 85 italic 9pt \"Arial\";");
#endif
  style.append("font-weight: bold;");
  style.append("}\n");

  setStyleSheet(style);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer PipelineFilterWidget::getFilter()
{
  return m_Filter;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void  PipelineFilterWidget::mousePressEvent ( QMouseEvent* event )
{
  if(event->button() != Qt::LeftButton)
  {
    event->ignore();
    return;
  }
  // Only if we are inside the delete checkbox/image then pass up to the superclass
  if(m_DeleteRect.contains(event->pos()))
  {
    QFrame::mousePressEvent(event);
  }
  else
  {
    dragStartPosition = event->pos();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterWidget::mouseReleaseEvent(QMouseEvent* event)
{
  if(event->button() != Qt::LeftButton)
  {
    event->ignore();
    return;
  }
  // Only if we are inside the delete checkbox/image then pass up to the superclass
  if(m_DeleteRect.contains(event->pos()))
  {
    QFrame::mouseReleaseEvent(event);
  }
  else
  {
    setIsSelected(true);
    event->setAccepted(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterWidget::mouseMoveEvent(QMouseEvent* event)
{
  if(!(event->buttons() & Qt::LeftButton))
  {
    return;
  }
  if((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
  {
    return;
  }
  // The user is dragging the filter widget so we should set it as selected.
  setIsSelected(true);
  QPixmap pixmap = QPixmap::grabWidget(this);

  // Create new picture for transparent
  QPixmap transparent(pixmap.size());
  // Do transparency
  transparent.fill(Qt::transparent);
#if 1
  QPainter p;
  p.begin(&transparent);
  p.setOpacity(0.70);
  p.drawPixmap(0, 0, pixmap);
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
void PipelineFilterWidget::getGuiParametersFromFilter(AbstractFilter* filt)
{
  BOOST_ASSERT("PipelineFilterWidget::getGuiParametersFromFilter executed");    // Code should never enter this function
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterWidget::on_deleteBtn_clicked()
{
  emit filterWidgetRemoved(this);
}


