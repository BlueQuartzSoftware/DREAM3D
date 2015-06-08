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


#include "PipelineFilterWidget.h"



#include <QtCore/QTimer>
#include <QtCore/QResource>
#include <QtCore/QDir>
#include <QtCore/QMimeData>

#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QDrag>

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QGroupBox>


#include "QtSupportLib/DREAM3DStyles.h"
#include "QtSupportLib/DREAM3DHelpUrlGenerator.h"

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"
#include "DREAM3DLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "DREAM3DLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "DREAM3DLib/FilterParameters/DataContainerReaderFilterParameter.h"

#include "DREAM3DWidgetsLib/FilterWidgetManager.h"
#include "DREAM3DWidgetsLib/FilterParameterWidgets/LinkedBooleanWidget.h"
#include "DREAM3DWidgetsLib/FilterParameterWidgets/ChoiceWidget.h"
#include "DREAM3DWidgetsLib/Widgets/PipelineViewWidget.h"
#include "DREAM3DWidgetsLib/Widgets/DataContainerArrayWidget.h"
#include "DREAM3DWidgetsLib/Widgets/DREAM3DUserManualDialog.h"



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
  m_CurrStrucVerticalLayout(NULL),
  m_BasicInputsWidget(NULL),
  m_AdvancedInputWidget(NULL),
  m_CurrentStructureWidget(NULL),
  m_Observer(NULL),
  m_ContextMenu(NULL),
  m_FilterInputWidget(NULL)
{
  initialize(AbstractFilter::NullPointer());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineFilterWidget::PipelineFilterWidget(AbstractFilter::Pointer filter, IObserver* observer, QWidget* parent) :
  QFrame(parent),
  m_IsSelected(false),
  m_HasPreflightErrors(false),
  m_HasPreflightWarnings(false),
  m_BasicInputsWidget(NULL),
  m_AdvancedInputWidget(NULL),
  m_Observer(observer),
  m_ContextMenu(new QMenu(this)),
  m_FilterInputWidget(NULL)
{
  initialize(filter);

  setupFilterInputWidget();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterWidget::initialize(AbstractFilter::Pointer filter)
{

  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenuForWidget(const QPoint&)));

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
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterWidget::setupFilterInputWidget()
{
  // Instantiate the filter input widget object
  m_FilterInputWidget = new FilterInputWidget(this);

  // Initialize the filter input widget with values
  m_FilterInputWidget->displayFilterParameters(this);
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

  QString basicname = QString::fromUtf8("basicInputsScrollWidget1_") + m_Filter->getNameOfClass();
  m_BasicInputsWidget->setObjectName(basicname);
  m_BasicInputsWidget->setGeometry(QRect(0, 0, 250, 267));
  m_BasicVerticalLayout = new QVBoxLayout(m_BasicInputsWidget);
  basicname = QString::fromUtf8("verticalLayout1") + m_Filter->getNameOfClass();
  m_BasicVerticalLayout->setObjectName(basicname);

  QString groupBoxStyle;
  QTextStream ss(&groupBoxStyle);
  ss << "QGroupBox\n";
  ss << "{\n";
  ss << "font-weight: bold\n";
  ss << "}";
  
  QGroupBox* parametersGroupBox = new QGroupBox("Parameters", this);
  QVBoxLayout* pLayout = new QVBoxLayout(parametersGroupBox);
  parametersGroupBox->setStyleSheet(groupBoxStyle);

  QGroupBox* requiredGroupBox = new QGroupBox("Required Arrays", this);
  QVBoxLayout* rLayout = new QVBoxLayout(requiredGroupBox);
  requiredGroupBox->setStyleSheet(groupBoxStyle);

  QGroupBox* createdGroupBox = new QGroupBox("Created Arrays", this);
  QVBoxLayout* cLayout = new QVBoxLayout(createdGroupBox);
  createdGroupBox->setStyleSheet(groupBoxStyle);

  m_BasicVerticalLayout->addWidget(parametersGroupBox);
  m_BasicVerticalLayout->addWidget(requiredGroupBox);
  m_BasicVerticalLayout->addWidget(createdGroupBox);

  m_AdvancedInputWidget = new QWidget(this);
  QString advname = QString::fromUtf8("advancedInputsScrollWidget2_") + m_Filter->getNameOfClass();
  m_AdvancedInputWidget->setObjectName(advname);
  m_AdvancedInputWidget->setGeometry(QRect(0, 0, 250, 267));
  m_AdvVerticalLayout = new QVBoxLayout(m_AdvancedInputWidget);
  advname = QString::fromUtf8("verticalLayout2") + m_Filter->getNameOfClass();
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
      //m_AdvVerticalLayout->addWidget(w);
      cLayout->addWidget(w);
      m_AdvParameterCount++;
    }
    else
    {
      w->setParent(m_BasicInputsWidget);
      //m_BasicVerticalLayout->addWidget(w);// Add the FilterWidget to the layout
      rLayout->addWidget(w);
      m_BasicParameterCount++;
    }

    // Connect up some signals and slots
    connect(w, SIGNAL(parametersChanged() ),
            parent(), SLOT(preflightPipeline() ) );
    connect(w, SIGNAL(parametersChanged()),
      this, SLOT(handleFilterParameterChanged()));
    connect(w, SIGNAL(errorSettingFilterParameter(const QString&)),
            this, SLOT(displayFilterParameterWidgetError(const QString&)));

  }

  // Now link any boolean widgets to any conditional Widgets that they might control.
  linkConditionalWidgets(filterParameters);


  // Now layout the CurrenStructureWidget
  m_CurrentStructureWidget = new DataContainerArrayWidget(m_Filter.get(), this);
  QString curStructName = QString::fromUtf8("advancedInputsScrollWidget_CurrStructWidget");
  m_CurrentStructureWidget->setObjectName(curStructName);
  m_CurrentStructureWidget->setGeometry(QRect(0, 0, 250, 267));
  //  m_CurrStrucVerticalLayout = new QVBoxLayout(m_CurrentStructureWidget);
  //  curStructName = QString::fromUtf8("verticalLayout3");
  //  m_CurrStrucVerticalLayout->setObjectName(curStructName);

}

// -----------------------------------------------------------------------------
//  CONNECT - PipelineFilterWidget::layoutWidgets()
// -----------------------------------------------------------------------------
void PipelineFilterWidget::handleFilterParameterChanged()
{
  /* SLOT - PipelineViewWidget::handleFilterParameterChanged()
     CONNECT - PipelineViewWidget::addFilter(...) */
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterWidget::validateFileSystemFilterParameter(FilterParameter* option)
{
  FileSystemFilterParameter* fsParam = dynamic_cast<FileSystemFilterParameter*>(option);
  if(fsParam)
  {
    DataContainerReaderFilterParameter* rParam = NULL;
    QString currentPath = "";
    QString Ftype = "";
    QString ext = "";
    if (fsParam->getWidgetType().compare(FilterParameterWidgetType::DataContainerReaderWidget) == 0)
    {
      rParam = dynamic_cast<DataContainerReaderFilterParameter*>(fsParam);
      currentPath = m_Filter->property(rParam->getInputFileProperty().toLatin1().constData()).toString();
    }
    else
    {
      currentPath = m_Filter->property(fsParam->getPropertyName().toLatin1().constData()).toString();
    }
    QFileInfo fi(currentPath);

    if (currentPath.isEmpty() == false && fi.exists() == false)
    {
    if (NULL != rParam)
    {
      Ftype = rParam->getFileType();
      ext = rParam->getFileExtension();
    }
    else
    {
      Ftype = fsParam->getFileType();
      ext = fsParam->getFileExtension();
    }
      QString s = Ftype + QString(" Files (*") + ext + QString(");;All Files (*.*)");
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

    else if (fsParam->getWidgetType().compare(FilterParameterWidgetType::DataContainerReaderWidget) == 0)
    {
      QString title = QObject::tr("Select a replacement input file for parameter '%1' in filter '%2'").arg(fsParam->getHumanLabel()).arg(m_Filter->getHumanLabel());

      QString file = QFileDialog::getOpenFileName(this, title, defaultName, s);
      if (true == file.isEmpty())
      {
        file = currentPath;
      }
      file = QDir::toNativeSeparators(file);
      // Store the last used directory into the private instance variable
      QFileInfo fi(file);
      m_OpenDialogLastDirectory = fi.path();
      m_Filter->setProperty(rParam->getInputFileProperty().toLatin1().constData(), file);
    }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterWidget::linkConditionalWidgets(QVector<FilterParameter::Pointer>& filterParameters)
{
  // Figure out if we have any "Linked Boolean Widgets" to hook up to other widgets
  for (QVector<FilterParameter::Pointer>::iterator iter = filterParameters.begin(); iter != filterParameters.end(); ++iter )
  {
    FilterParameter::Pointer option = (*iter);
    LinkedBooleanFilterParameter::Pointer optionPtr = boost::dynamic_pointer_cast<LinkedBooleanFilterParameter>(option);
    if(NULL != optionPtr.get() && option->getWidgetType().compare(FilterParameterWidgetType::LinkedBooleanWidget) == 0 )
    {
      QStringList linkedProps = optionPtr->getConditionalProperties();

      QStringListIterator iter = QStringListIterator(linkedProps);
      QWidget* checkboxSource = m_PropertyToWidget[optionPtr->getPropertyName()];
      while(iter.hasNext())
      {
        QString propName = iter.next();
        QWidget* w = m_PropertyToWidget[propName];
        if(w)
        {
          connect(checkboxSource, SIGNAL(conditionalPropertyChanged(int)),
                  w, SLOT(setLinkedConditionalState(int) ) );
          LinkedBooleanWidget* lbw = qobject_cast<LinkedBooleanWidget*>(checkboxSource);
          if(lbw && lbw->getLinkedState() != Qt::Checked)
          {
            w->hide();
          }
        }
      }
    }


    // Figure out if we have any Linked ComboBox Widgets to hook up to other widgets
    LinkedChoicesFilterParameter::Pointer optionPtr2 = boost::dynamic_pointer_cast<LinkedChoicesFilterParameter>(option);
    if(NULL != optionPtr2.get() && option->getWidgetType().compare(FilterParameterWidgetType::ChoiceWidget) == 0 )
    {
      QStringList linkedProps = optionPtr2->getLinkedProperties();

      QStringListIterator iter = QStringListIterator(linkedProps);
      QWidget* checkboxSource = m_PropertyToWidget[optionPtr2->getPropertyName()];
      while(iter.hasNext())
      {
        QString propName = iter.next();
        QWidget* w = m_PropertyToWidget[propName];
        if(w)
        {
          //qDebug() << "Connecting: " << optionPtr2->getPropertyName() << " to " << propName;
          connect(checkboxSource, SIGNAL(conditionalPropertyChanged(int)),
                  w, SLOT(setLinkedComboBoxState(int) ) );

          ChoiceWidget* choiceWidget = qobject_cast<ChoiceWidget*>(checkboxSource);
          if(choiceWidget)
          {
            choiceWidget->widgetChanged(choiceWidget->getCurrentIndex(), false);
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

  if (NULL != m_FilterInputWidget)
  {
    delete m_FilterInputWidget;
    m_FilterInputWidget = NULL;
  }
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
void PipelineFilterWidget::setFilterTitle(const QString title)
{
  filterName->setText(title);
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
QWidget* PipelineFilterWidget::getCurrentStructureWidget()
{
  return m_CurrentStructureWidget;
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
  QTextStream ss(&style);

  if(m_HasPreflightWarnings)
  {
    ss << "border: 3px solid rgb(172, 168, 0);";
  }
  else if(m_IsSelected == true )
  {
    ss << "border: 3px solid MediumBlue;";
  }
  else
  {
    ss << "border: 1px solid #515151;";
    ss << "margin: 1px;";
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
  QTextStream ss(&style);

  ss << "PipelineFilterWidget {\n";


  if (m_HasPreflightErrors == true)
  {
    //ss << "background-color: rgb(200, 75, 75);\ncolor: rgb(255, 255, 255);";
    ss << "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(180, 55, 55, 255), stop:0.5 rgba(235, 110, 110, 255), stop:1 rgba(180, 55, 55, 255));\n";
  }
  else
  {
    ss << "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(185, 185, 185, 255), stop:0.5 rgba(226, 226, 226, 255), stop:1 rgba(150, 150, 150, 255));\n";
  }

  ss << "background-position: top ;\n background-repeat: repeat-x;";

  ss << getBorderColorStyle();

  ss << "border-radius: 8px;";
  ss << "padding: 0 0 0 0px;";
  ss << "}\n";

  ss << "QLabel\n {\n";

#if defined(Q_OS_WIN)
  ss << "font: 9pt \"" << DREAM3DStyles::GetUIFont() << "\";";
#elif defined(Q_OS_MAC)
  ss << "font: 100 12pt \"" << DREAM3DStyles::GetUIFont() << "\";";
#else
  ss << "font: 100 10pt \"" << DREAM3DStyles::GetUIFont() << "\";";
#endif
  ss << "/* font-weight: bold; */";
  if (m_HasPreflightErrors == true)
  {
    ss << "color: rgb(240, 240, 240);";
  }
  ss << "}\n";


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
  QPixmap pixmap = grab();

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
void PipelineFilterWidget::showContextMenuForWidget(const QPoint &pos)
{
  if (NULL != getFilter())
  {
    // Clear the existing context menu
    m_ContextMenu->clear();

    QAction* actionLaunchHelp = new QAction(m_ContextMenu);
    actionLaunchHelp->setObjectName(QString::fromUtf8("actionLaunchHelp"));
    actionLaunchHelp->setText(QApplication::translate("DREAM3D_UI", "Filter Help", 0));
    connect(actionLaunchHelp, SIGNAL(triggered()),
      this, SLOT(launchHelpForItem()));

    //QAction* actionLaunchHelp = new QAction(m_ContextMenu);
    //actionLaunchHelp->setObjectName(QString::fromUtf8("actionLaunchHelp"));
    //actionLaunchHelp->setText(QApplication::translate("DREAM3D_UI", "Filter Help", 0));
    //connect(actionLaunchHelp, SIGNAL(triggered()),
    //  this, SLOT(launchHelpForItem()));

    m_ContextMenu->addAction(actionLaunchHelp);
    m_ContextMenu->exec(QCursor::pos());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineFilterWidget::launchHelpForItem()
{
  QString name = getFilter()->getHumanLabel();

  // Launch the dialog
  DREAM3DUserManualDialog::LaunchHelpDialog(name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterInputWidget* PipelineFilterWidget::getFilterInputWidget()
{
  return m_FilterInputWidget;
}


