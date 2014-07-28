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
#include <QtGui/QFileDialog>



#include "QtSupport/DREAM3DHelpUrlGenerator.h"

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"


#include "DREAM3DWidgetsLib/FilterWidgetManager.h"
#include "DREAM3DWidgetsLib/FilterParameterWidgets/LinkedBooleanWidget.h"

// -----------------------------------------------------------------------------
// Include the "moc" file that was generated for this file
// -----------------------------------------------------------------------------
//#include "DREAM3DWidgetsLib/moc_PipelineFilterWidget.cpp"

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
  //  m_CurrentBorderColorFactor(0),
  //  m_BorderIncrement(16),
  m_IsSelected(false),
  m_HasPreflightErrors(false),
  m_HasPreflightWarnings(false),
  m_BasicVerticalLayout(NULL),
  m_AdvVerticalLayout(NULL),
  m_BasicInputsWidget(NULL),
  m_AdvancedInputWidget(NULL),
  m_Observer(NULL)
{
  initialize(AbstractFilter::NullPointer());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineFilterWidget::PipelineFilterWidget(AbstractFilter::Pointer filter, IObserver* observer, QWidget* parent) :
  QFrame(parent),
  //  m_CurrentBorderColorFactor(0),
  //  m_BorderIncrement(16),
  m_IsSelected(false),
  m_HasPreflightErrors(false),
  m_HasPreflightWarnings(false),
  m_BasicInputsWidget(NULL),
  m_AdvancedInputWidget(NULL),
  m_Observer(observer)
{
  initialize(filter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterWidget::initialize(AbstractFilter::Pointer filter)
{

  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
          this, SLOT(showCustomContextMenu(const QPoint&)));

  setAttribute(Qt::WA_NoMousePropagation);

  setupUi(this);

  if ( m_OpenDialogLastDirectory.isEmpty() )
  {
    m_OpenDialogLastDirectory = QDir::homePath();
  }

  m_DeleteRect.setX(PADDING + BORDER);
  m_DeleteRect.setY(PADDING + BORDER);
  m_DeleteRect.setWidth(IMAGE_WIDTH);
  m_DeleteRect.setHeight(IMAGE_HEIGHT);

  // Set the AbstractFilter for this class
  m_Filter = filter;

  // Layout the widgets in the parent widget
  layoutWidgets();

#if 0
  QAction* action = new QAction(NULL);
  action->setObjectName(QString::fromUtf8("actionSomeAction"));
  action->setText(QApplication::translate("DREAM3D_UI", "Some Action", 0, QApplication::UnicodeUTF8));
  m_MenuActions << action;
  //m_Menu->addAction(action);
  //QKeySequence actionClearKeySeq(Qt::CTRL + Qt::Key_Delete);
  //action->setShortcut(actionClearKeySeq);
  //  connect(action, SIGNAL(triggered()),
  //          this, SLOT( on_actionClearPipeline_triggered() ) );
#endif

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterWidget::layoutWidgets()
{
  // Make sure the m_Filter is Non NULL
  if (NULL == m_Filter.get())
  {
    return;
  }

  // If the filter is valid then instantiate all the FilterParameterWidgets
  // Create the Widget that will be placed into the Basic Inputs Scroll Area
  m_BasicInputsWidget = new QWidget(this);
  QString basicname = QString::fromUtf8("basicInputsScrollWidget_") + m_Filter->getNameOfClass();
  m_BasicInputsWidget->setObjectName(basicname);
  m_BasicInputsWidget->setGeometry(QRect(0, 0, 250, 267));
  m_BasicVerticalLayout = new QVBoxLayout(m_BasicInputsWidget);
  basicname = QString::fromUtf8("verticalLayout") + m_Filter->getNameOfClass();
  m_BasicVerticalLayout->setObjectName(basicname);

  m_AdvancedInputWidget = new QWidget(this);
  QString advname = QString::fromUtf8("advancedInputsScrollWidget_") + m_Filter->getNameOfClass();
  m_AdvancedInputWidget->setObjectName(advname);
  m_AdvancedInputWidget->setGeometry(QRect(0, 0, 250, 267));
  m_AdvVerticalLayout = new QVBoxLayout(m_AdvancedInputWidget);
  advname = QString::fromUtf8("verticalLayout") + m_Filter->getNameOfClass();
  m_AdvVerticalLayout->setObjectName(advname);

  m_BasicParameterCount = 0;
  m_AdvParameterCount = 0;

  // Set the Name of the filter into the FilterWidget
  filterName->setText(m_Filter->getHumanLabel() );

  // Get the FilterWidgetManagere instance so we can instantiate new FilterParameterWidgets
  FilterWidgetManager*  fwm = FilterWidgetManager::Instance();
  // Get a list of all the filterParameters from the filter.
  QVector<FilterParameter::Pointer> filterParameters = m_Filter->getFilterParameters();
  // Create all the FilterParameterWidget objects that can be displayed where ever the developer needs
  for (QVector<FilterParameter::Pointer>::iterator iter = filterParameters.begin(); iter != filterParameters.end(); ++iter )
  {
    FilterParameter* option = (*iter).get();

    validateFileSystemFilterParameter(option);

    QWidget* w = fwm->createWidget(option, m_Filter.get());
    m_PropertyToWidget.insert(option->getPropertyName(), w); // Update our Map of Filter Parameter Properties to the Widget

    if (NULL == w) { continue; }
    m_FilterParameterWidgets.push_back(w);

    // Determine which layout to add the widget into
    if(option->getAdvanced() == true)
    {
      w->setParent(m_AdvancedInputWidget);// Set the parent for the widget
      m_AdvVerticalLayout->addWidget(w);
      m_AdvParameterCount++;
    }
    else
    {
      w->setParent(m_BasicInputsWidget);
      m_BasicVerticalLayout->addWidget(w);// Add the FilterWidget to the layout
      m_BasicParameterCount++;
    }

    // Connect up some signals and slots
    connect(w, SIGNAL(parametersChanged() ),
            parent(), SLOT(preflightPipeline() ) );
    connect(w, SIGNAL(errorSettingFilterParameter(const QString&)),
            this, SLOT(displayFilterParameterWidgetError(const QString&)));

  }

  // Now link any boolean widgets to any conditional Widgets that they might control.
  linkConditionalWidgets(filterParameters);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterWidget::validateFileSystemFilterParameter(FilterParameter* option)
{
  FileSystemFilterParameter* fsParam = dynamic_cast<FileSystemFilterParameter*>(option);
  if(fsParam)
  {
    QString currentPath = m_Filter->property(fsParam->getPropertyName().toLatin1().constData()).toString();
    QFileInfo fi(currentPath);

    if (currentPath.isEmpty() == false && fi.exists() == false)
    {
      QString Ftype = fsParam->getFileType();
      QString ext = fsParam->getFileExtension();
      QString s = Ftype + QString(" Files (") + ext + QString(");;All Files(*.*)");
      QString defaultName = m_OpenDialogLastDirectory + QDir::separator() + "Untitled";

      if (fsParam->getWidgetType().compare(FilterParameterWidgetType::InputFileWidget) == 0 )
      {
        QString title = QObject::tr("Select a replacement input file for parameter '%1' in filter '%2'").arg(fsParam->getHumanLabel()).arg(m_Filter->getHumanLabel());

        QString file = QFileDialog::getOpenFileName(this, title, defaultName, s);
        if(true == file.isEmpty())
        {
          file = currentPath;
        }
        file = QDir::toNativeSeparators(file);
        // Store the last used directory into the private instance variable
        QFileInfo fi(file);
        m_OpenDialogLastDirectory = fi.path();
        m_Filter->setProperty(fsParam->getPropertyName().toLatin1().constData(), file);
      }

      else if (fsParam->getWidgetType().compare(FilterParameterWidgetType::InputPathWidget) == 0 )
      {
        QString title = QObject::tr("Select a replacement input folder for parameter '%1' in filter '%2'").arg(fsParam->getHumanLabel()).arg(m_Filter->getHumanLabel());

        QString file = QFileDialog::getExistingDirectory(this, title, defaultName, QFileDialog::ShowDirsOnly);
        file = QDir::toNativeSeparators(file);
        if(true == file.isEmpty())
        {
          file = currentPath;
        }
        // Store the last used directory into the private instance variable
        QFileInfo fi(file);
        m_OpenDialogLastDirectory = fi.path();
        m_Filter->setProperty(fsParam->getPropertyName().toLatin1().constData(), file);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterWidget::linkConditionalWidgets(QVector<FilterParameter::Pointer> filterParameters)
{
  // Figure out if we have any "Linked Boolean Widgets" to hook up to other widgets
  for (QVector<FilterParameter::Pointer>::iterator iter = filterParameters.begin(); iter != filterParameters.end(); ++iter )
  {
    FilterParameter* option = (*iter).get();
    if(option->getWidgetType().compare(FilterParameterWidgetType::LinkedBooleanWidget) == 0 )
    {
      //  qDebug() << option->getHumanLabel() << " is conditional";
      QStringList linkedProps = option->getConditionalProperties();

      QStringListIterator iter = QStringListIterator(linkedProps);
      QWidget* checkboxSource = m_PropertyToWidget[option->getPropertyName()];
      LinkedBooleanWidget* lbw = qobject_cast<LinkedBooleanWidget*>(checkboxSource);
      while(iter.hasNext())
      {
        QString propName = iter.next();
        QWidget* w = m_PropertyToWidget[propName];
        if(w)
        {
          connect(checkboxSource, SIGNAL(conditionalPropertyChanged(int)),
                  w, SLOT(setLinkedConditionalState(int) ) );
          if(lbw && lbw->getLinkedState() != Qt::Checked)
          {
            w->hide();
          }
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterWidget::adjustLayout(QWidget* w, int state)
{
  if (state == Qt::Checked)
  {
    w->show();
  }
  else
  {
    w->hide();
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
  m_Filter->setPreviousFilter(AbstractFilter::NullPointer());
  m_Filter->setNextFilter(AbstractFilter::NullPointer());
  m_Filter = AbstractFilter::NullPointer();
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
  { return m_Filter->getHumanLabel(); }

  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineFilterWidget::getFilterClassName()
{
  if (NULL != m_Filter.get())
  { return m_Filter->getNameOfClass(); }

  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineFilterWidget::getCompiledLibraryName()
{
  if (NULL != m_Filter.get())
  { return m_Filter->getCompiledLibraryName(); }

  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineFilterWidget::getFilterGroup()
{
  if (NULL != m_Filter.get())
  { return m_Filter->getGroupName(); }

  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineFilterWidget::getFilterSubGroup()
{
  if (NULL != m_Filter.get())
  { return m_Filter->getSubGroupName(); }

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
QWidget* PipelineFilterWidget::getBasicInputsWidget()
{
  return m_BasicInputsWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* PipelineFilterWidget::getAdvancedInputsWidget()
{
  return m_AdvancedInputWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterWidget::setHasPreflightErrors(bool hasErrors)
{
  m_HasPreflightErrors = hasErrors;
  changeStyle();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterWidget::setHasPreflightWarnings(bool hasWarnings)
{
  m_HasPreflightWarnings = hasWarnings;
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
  //  int m_CurrentBorderColorFactor = 0;
  //  int m_BorderIncrement = 16;

  /*  if (m_HasPreflightErrors == true)
  {
    style.append("border: 2px solid rgb(255, 0, 0);");
  }
  else*/
  if(m_HasPreflightWarnings)
  {
    style.append("border: 2px solid rgb(172, 168, 0);");
  }
  else if(m_IsSelected == true )
  {
    style.append("border: 3px solid purple;");
    //  style.append("background-color: palette(highlight);");
  }
  else
  {
    style.append("border: 1px solid #515151;");
    style.append("margin: 1px;");
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


  if (m_HasPreflightErrors == true)
  {
    style.append("background-color: rgb(180, 60, 60);\ncolor: rgb(255, 255, 255);");
  }
  else
  {
    style.append("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(185, 185, 185, 255), stop:0.5 rgba(226, 226, 226, 255), stop:1 rgba(150, 150, 150, 255));\n");
    //   style.append("background-color: rgb(147, 184, 149);");
  }

  style.append("background-position: top ;\n background-repeat: repeat-x;");

  style.append(getBorderColorStyle());

  style.append("border-radius: 10px;");
  style.append("padding: 0 0 0 0px;");
  style.append("}\n");

  style.append("QLabel\n {\n");

#if defined(Q_WS_WIN)
  style.append("font: 75 10pt \"Arial\";");
#elif defined(Q_WS_MAC)
  style.append("font: 100 italic 12pt \"Arial\";");
#else
  style.append("font: 85 italic 9pt \"Arial\";");
#endif
  style.append("font-weight: bold; ");
  if (m_HasPreflightErrors == true)
  {
    style.append("color: rgb(255, 255, 255);");
  }
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
void PipelineFilterWidget::mousePressEvent ( QMouseEvent* event )
{
  if(event->button() == Qt::RightButton)
  {
    QFrame::mousePressEvent(event);
  }
  else if(event->button() == Qt::LeftButton)
  {
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
    event->accept();
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


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterWidget::setContextMenuActions(QList<QAction*> list)
{
  m_MenuActions = list;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterWidget::showCustomContextMenu(const QPoint& pos)
{
  // Note: We must map the point to global from the viewport to
  // account for the header.
  showContextMenu(mapToGlobal(pos) );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterWidget::showContextMenu(const QPoint& globalPos)
{
  m_Menu.clear();
  for (int i = 0; i < m_MenuActions.size(); i++)
  {
    m_Menu.addAction(m_MenuActions[i]);
  }
  m_Menu.exec(globalPos);
}
