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

#include "PipelineViewWidget.h"

#include <iostream>


#include <QtCore/QFileInfo>
#include <QtCore/QUrl>
#include <QtCore/QDir>
#include <QtCore/QTemporaryFile>
#include <QtCore/QMimeData>

#include <QtGui/QMouseEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragLeaveEvent>
#include <QtGui/QDragMoveEvent>
#include <QtWidgets/QLabel>
#include <QtGui/QPixmap>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QProgressDialog>
#include <QtWidgets/QMessageBox>

#include "Applications/DREAM3D/DREAM3DApplication.h"

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/PipelineMessage.h"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/IFilterFactory.hpp"
#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/FilterParameters/QFilterParametersReader.h"
#include "SIMPLib/FilterParameters/QFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersWriter.h"

#include "QtSupportLib/QDroppableScrollArea.h"

#include "DREAM3DWidgetsLib/FilterWidgetManager.h"



#include "DREAM3DWidgetsLib/FilterParameterWidgets/FilterParameterWidgetsDialogs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineViewWidget::PipelineViewWidget(QWidget* parent) :
  QFrame(parent),
  m_SelectedFilterWidget(NULL),
  m_FilterWidgetLayout(NULL),
  m_CurrentFilterBeingDragged(NULL),
  m_PreviousFilterBeingDragged(NULL),
  m_FilterOrigPos(-1),
  m_DropBox(NULL),
  m_DropIndex(-1),
  m_EmptyPipelineLabel(NULL),
  m_ScrollArea(NULL),
  m_AutoScroll(true),
  m_AutoScrollMargin(10),
  m_autoScrollCount(0),
  m_InputParametersWidget(NULL),
  m_PipelineMessageObserver(NULL),
  m_StatusBar(NULL)
{
  setupGui();
  m_LastDragPoint = QPoint(-1, -1);
  m_autoScrollTimer.setParent(this);

  setContextMenuPolicy(Qt::CustomContextMenu);
  setFocusPolicy(Qt::StrongFocus);

  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), dream3dApp, SLOT(on_pipelineViewContextMenuRequested(const QPoint&)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineViewWidget::~PipelineViewWidget()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::setPipelineMessageObserver(QObject* pipelineMessageObserver)
{
  m_PipelineMessageObserver = pipelineMessageObserver;
  // setup our connection
  connect(this, SIGNAL(pipelineIssuesCleared()),
          m_PipelineMessageObserver, SLOT(clearIssues()) );
  connect(this, SIGNAL(preflightPipelineComplete()),
          m_PipelineMessageObserver, SLOT(displayCachedMessages()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::setupGui()
{
  newEmptyPipelineViewLayout();
  connect(&m_autoScrollTimer, SIGNAL(timeout()), this, SLOT(doAutoScroll()));

  m_DropBox = new DropBoxWidget();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::setScrollArea(QScrollArea* sa)
{
  m_ScrollArea = sa;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::newEmptyPipelineViewLayout()
{
  if(m_EmptyPipelineLabel == NULL)
  {
    QGridLayout* gridLayout = new QGridLayout(this);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    QSpacerItem* verticalSpacer = new QSpacerItem(20, 341, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(verticalSpacer, 0, 1, 1, 1);

    QSpacerItem* horizontalSpacer_3 = new QSpacerItem(102, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addItem(horizontalSpacer_3, 1, 0, 1, 1);

    m_EmptyPipelineLabel = new QLabel(this);
    m_EmptyPipelineLabel->setObjectName(QString::fromUtf8("label"));
    m_EmptyPipelineLabel->setMinimumSize(QSize(325, 250));
    m_EmptyPipelineLabel->setStyleSheet(QString::fromUtf8("QLabel {\n"
                                                          "border-radius: 20px;\n"
                                                          "/*border: 1px solid rgb(120, 120, 120);*/\n"
                                                          "/* background-color: rgb(160, 160, 160); */\n"
                                                          "font-weight: bold;\n"
                                                          "color : rgb(150, 150, 150);\n"
                                                          "text-align: center;\n"
                                                          "margin: 5px;\n"
                                                          "padding: 10px;\n"
                                                          "}"));
    m_EmptyPipelineLabel->setAlignment(Qt::AlignCenter);
    QString text;
    QTextStream ss (&text);
    ss << "<h2>Creating a Pipeline</h2>";
    ss << "<hr>";
    ss << "File > Open <br />";
    ss << "File > New <br />";
    ss << "Drag and drop filters<br />";
    ss << "Double click a Bookmark<br />";
    ss << "Double click a Prebuilt Pipeline<br />";
    m_EmptyPipelineLabel->setText(text);


    gridLayout->addWidget(m_EmptyPipelineLabel, 1, 1, 1, 1);

    QSpacerItem* horizontalSpacer_4 = new QSpacerItem(102, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addItem(horizontalSpacer_4, 1, 2, 1, 1);

    QSpacerItem* verticalSpacer_2 = new QSpacerItem(20, 341, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(verticalSpacer_2, 2, 1, 1, 1);
  }
  emit pipelineChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PipelineViewWidget::filterCount()
{
  int count = 0;
  if (NULL != m_FilterWidgetLayout)
  {
    count = m_FilterWidgetLayout->count();
  }
  return count;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineFilterWidget* PipelineViewWidget::filterWidgetAt(int index)
{
  PipelineFilterWidget* fw = NULL;
  if (m_FilterWidgetLayout != NULL)
  {
    QWidget* w = m_FilterWidgetLayout->itemAt(index)->widget();
    fw = qobject_cast<PipelineFilterWidget*>(w);
  }
  return fw;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::resetLayout()
{
  // Check to see if we have removed all the filters
  if (filterCount() <= 0)
  {
    // Emit a signal to tell DREAM3D_UI to erase the Filter Input Widget.
    emit noFilterWidgetsInPipeline();

    // Remove the current Layout
    QLayout* l = layout();
    if (NULL != l && l == m_FilterWidgetLayout)
    {
      qDeleteAll(l->children());
      delete l;
      m_FilterWidgetLayout = NULL;
    }

    // and add the empty pipeline layout instead
    newEmptyPipelineViewLayout();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::clearWidgets()
{
  emit pipelineIssuesCleared();
  emit pipelineChanged();

  qint32 count = filterCount();
  for(qint32 i = count - 1; i >= 0; --i)
  {
    QWidget* w = m_FilterWidgetLayout->itemAt(i)->widget();
    QSpacerItem* spacer = m_FilterWidgetLayout->itemAt(i)->spacerItem();
    if (NULL != w)
    {
      m_FilterWidgetLayout->removeWidget(w);
      PipelineFilterWidget* fw = qobject_cast<PipelineFilterWidget*>(w);
      if(fw)
      {
        fw->getFilter()->setPreviousFilter(AbstractFilter::NullPointer());
        fw->getFilter()->setNextFilter(AbstractFilter::NullPointer());
      }
      w->deleteLater();
    }
    else if (NULL != spacer)
    {
      m_FilterWidgetLayout->removeItem(spacer);
    }
  }
  m_SelectedFilterWidget = NULL;
  resetLayout();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::reindexWidgetTitles()
{
  qint32 count = filterCount();
  for(qint32 i = 0; i < count; ++i)
  {
    PipelineFilterWidget* fw = filterWidgetAt(i);
    if (fw)
    {
      QString hl = fw->getFilter()->getHumanLabel();
      hl = QString("[") + QString::number(i + 1) + QString("] ") + hl;
      fw->setFilterTitle(hl);
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::Pointer PipelineViewWidget::getFilterPipeline()
{
  // Create a Pipeline Object and fill it with the filters from this View
  FilterPipeline::Pointer pipeline = FilterPipeline::New();

  qint32 count = filterCount();
  for(qint32 i = 0; i < count; ++i)
  {
    PipelineFilterWidget* fw = filterWidgetAt(i);
    if (fw)
    {
      AbstractFilter::Pointer filter = fw->getFilter();
      pipeline->pushBack(filter);
    }

  }
  pipeline->addMessageReceiver(m_PipelineMessageObserver);
  return pipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::Pointer PipelineViewWidget::getCopyOfFilterPipeline()
{
  // Create a Pipeline Object and fill it with the filters from this View
  FilterPipeline::Pointer pipeline = FilterPipeline::New();

  qint32 count = filterCount();
  for(qint32 i = 0; i < count; ++i)
  {
    PipelineFilterWidget* fw = filterWidgetAt(i);
    if (fw)
    {
      AbstractFilter::Pointer filter = fw->getFilter();
      AbstractFilter::Pointer copy = filter->newFilterInstance(true);
      pipeline->pushBack(copy);
    }

  }
  pipeline->addMessageReceiver(m_PipelineMessageObserver);
  return pipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PipelineViewWidget::writePipeline(QString filePath)
{
  QFileInfo fi(filePath);
  QString ext = fi.completeSuffix();

  //If the filePath already exists - delete it so that we get a clean write to the file
  if (fi.exists() == true && (ext == "dream3d" || ext == "json"))
  {
    QFile f(filePath);
    if (f.remove() == false)
    {
      QMessageBox::warning(this, QString::fromLatin1("Pipeline Write Error"),
                           QString::fromLatin1("There was an error removing the existing pipeline file. The pipeline was NOT saved."));
      return -1;
    }
  }

  // Create a Pipeline Object and fill it with the filters from this View
  FilterPipeline::Pointer pipeline = getFilterPipeline();

  int err = 0;
  if (ext == "dream3d")
  {
    err = H5FilterParametersWriter::WritePipelineToFile(pipeline, fi.absoluteFilePath(), fi.fileName(), reinterpret_cast<IObserver*>(m_PipelineMessageObserver));
  }
  else if (ext == "json")
  {
    err = JsonFilterParametersWriter::WritePipelineToFile(pipeline, fi.absoluteFilePath(), fi.fileName(), reinterpret_cast<IObserver*>(m_PipelineMessageObserver));
  }
  else
  {
    m_StatusBar->showMessage(tr("The pipeline was not written to file '%1'. '%2' is an unsupported file extension.").arg(fi.fileName()).arg(ext));
    return -1;
  }

  if (err < 0)
  {
    m_StatusBar->showMessage(tr("There was an error while saving the pipeline to file '%1'.").arg(fi.fileName()));
    return -1;
  }
  else
  {
    m_StatusBar->showMessage(tr("The pipeline has been saved successfully to '%1'.").arg(fi.fileName()));
  }

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PipelineViewWidget::openPipeline(const QString& filePath, int index, const bool& setOpenedFilePath, const bool& changeTitle)
{
  QFileInfo fi(filePath);
  if (fi.exists() == false)
  {
    QMessageBox::warning(this, QString::fromLatin1("Pipeline Read Error"),
                         QString::fromLatin1("There was an error opening the specified pipeline file. The pipeline file does not exist."));
    return -1;
  }

  // Clear the pipeline Issues table first so we can collect all the error messages
  emit pipelineIssuesCleared();

  QString ext = fi.suffix();
  QString name = fi.fileName();

  // Read the pipeline from the file
  FilterPipeline::Pointer pipeline = readPipelineFromFile(filePath);

  // Check that a valid extension was read...
  if (pipeline == FilterPipeline::NullPointer())
  {
    m_StatusBar->showMessage(tr("The pipeline was not read correctly from file '%1'. '%2' is an unsupported file extension.").arg(name).arg(ext));
    return -1;
  }

  // Populate the pipeline view
  populatePipelineView(pipeline, index);

  // Notify user of successful read
  m_StatusBar->showMessage(tr("The pipeline has been read successfully from '%1'.").arg(name));

  QString file = filePath;
  emit pipelineOpened(file, setOpenedFilePath, changeTitle);

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::Pointer PipelineViewWidget::readPipelineFromFile(const QString& filePath)
{
  QFileInfo fi(filePath);
  QString ext = fi.suffix();
  QString name = fi.fileName();

  FilterPipeline::Pointer pipeline;
  if (ext == "ini" || ext == "txt")
  {
    pipeline = QFilterParametersReader::ReadPipelineFromFile(filePath, QSettings::IniFormat, dynamic_cast<IObserver*>(m_PipelineMessageObserver));
  }
  else if (ext == "dream3d")
  {
    pipeline = H5FilterParametersReader::ReadPipelineFromFile(filePath);
  }
  else if (ext == "json")
  {
    pipeline = JsonFilterParametersReader::ReadPipelineFromFile(filePath);
  }
  else
  {
    pipeline = FilterPipeline::NullPointer();
  }

  return pipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::addFilter(const QString& filterClassName, int index)
{
  if (this->isEnabled() == false) { return; }
  FilterManager* fm = FilterManager::Instance();
  if(NULL == fm) { return; }
  IFilterFactory::Pointer wf = fm->getFactoryForFilter(filterClassName);
  if (NULL == wf.get()) { return; }

  // Create an instance of the filter. Since we are dealing with the AbstractFilter interface we can not
  // actually use the concrete filter class. We are going to have to rely on QProperties or Signals/Slots
  // to communicate changes back to the filter.
  AbstractFilter::Pointer filter = wf->create();

  if (index < 0) // If the programmer wants to add it to the end of the list
  {
    index = filterCount() - 1; // filterCount will come back with the vertical spacer, and if index is still
    // -1 then the spacer is not there and it will get added so the next time through. this should work
  }

  // Create a FilterWidget object
  PipelineFilterWidget* w = new PipelineFilterWidget(filter, NULL, this);

  // Add the filter widget to this view widget
  addFilterWidget(w, index);

  // Clear the pipeline Issues table first so we can collect all the error messages
  emit pipelineIssuesCleared();
  // Now preflight the pipeline for this filter.
  preflightPipeline();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::addFilterWidget(PipelineFilterWidget* pipelineFilterWidget, int index)
{
  bool addSpacer = false;
  if (filterCount() <= 0)
  {
    if (NULL != m_EmptyPipelineLabel)
    {
      m_EmptyPipelineLabel->hide();
      delete m_EmptyPipelineLabel;
      m_EmptyPipelineLabel = NULL;
    }
    QLayout* l = layout();
    if (NULL != l)
    {
      qDeleteAll(l->children());
      delete l;
    }

    m_FilterWidgetLayout = new QVBoxLayout(this);
    m_FilterWidgetLayout->setObjectName(QString::fromUtf8("m_FilterWidgetLayout"));
    m_FilterWidgetLayout->setContentsMargins(2, 2, 2, 2);
    m_FilterWidgetLayout->setSpacing(3);
    addSpacer = true;

    if(index < 0)
    {
      index = 0;
    }
  }

  // The layout will take control of the PipelineFilterWidget 'w' instance
  m_FilterWidgetLayout->insertWidget(index, pipelineFilterWidget);
  // Set the Parent
  pipelineFilterWidget->setParent(this);

  if(index == -1)
  {
    index = filterCount() - 1;
  }


  /// Now setup all the connections between the various widgets

  // When the filter is removed from this view
  connect(pipelineFilterWidget, SIGNAL(filterWidgetRemoved(PipelineFilterWidget*)),
          this, SLOT(removeFilterWidget(PipelineFilterWidget*)) );

  // When the FilterWidget is selected
  connect(pipelineFilterWidget, SIGNAL(widgetSelected(PipelineFilterWidget*)),
          this, SLOT(setSelectedFilterWidget(PipelineFilterWidget*)) );

  // When the filter widget is dragged
  connect(pipelineFilterWidget, SIGNAL(dragStarted(PipelineFilterWidget*)),
          this, SLOT(setFilterBeingDragged(PipelineFilterWidget*)) );

  connect(pipelineFilterWidget, SIGNAL(parametersChanged()),
          this, SLOT(preflightPipeline()));

  connect(pipelineFilterWidget, SIGNAL(parametersChanged()),
          this, SLOT(handleFilterParameterChanged()));


  // Check to make sure at least the vertical spacer is in the Layout
  if (addSpacer)
  {
    QSpacerItem* verticalSpacer = new QSpacerItem(20, 361, QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_FilterWidgetLayout->insertSpacerItem(-1, verticalSpacer);
  }

  // Make sure the widget titles are all correct
  reindexWidgetTitles();

  // Finally, set this new filter widget as selected in order to show the input parameters right away
  pipelineFilterWidget->setIsSelected(true);
  // Get the filter to ignore Scroll Wheel Events
  pipelineFilterWidget->installEventFilter( this);

  // Emit that the pipeline changed
  emit pipelineChanged();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineViewWidget::eventFilter(QObject* o, QEvent* e)
{
  if ( e->type() == QEvent::Wheel && qobject_cast<PipelineFilterWidget*>(o) )
  {
    return false;
  }
  return QFrame::eventFilter( o, e );
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::preflightPipeline()
{
  emit pipelineIssuesCleared();
  // Create a Pipeline Object and fill it with the filters from this View
  FilterPipeline::Pointer pipeline = getFilterPipeline();

  FilterPipeline::FilterContainerType filters = pipeline->getFilterContainer();
  for(int i = 0; i < filters.size(); i++)
  {
    PipelineFilterWidget* fw = filterWidgetAt(i);
    if (fw)
    {
      fw->setHasPreflightErrors(false);
    }
    filters.at(i)->setErrorCondition(0);
  }


  QProgressDialog progress("Preflight Pipeline", "", 0, 1, this);
  progress.setWindowModality(Qt::WindowModal);

  // Preflight the pipeline
  int err = pipeline->preflightPipeline();
  if (err < 0)
  {
    //FIXME: Implement this
  }
  progress.setValue(1);

  int count = pipeline->getFilterContainer().size();
  //Now that the preflight has been executed loop through the filters and check their error condition and set the
  // outline on the filter widget if there were errors or warnings
  for(qint32 i = 0; i < count; ++i)
  {
    PipelineFilterWidget* fw = filterWidgetAt(i);
    if (fw)
    {
      AbstractFilter::Pointer filter = fw->getFilter();
      if(filter->getErrorCondition() < 0) {fw->setHasPreflightErrors(true);}
    }
  }
  emit preflightPipelineComplete();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::removeFilterWidget(PipelineFilterWidget* whoSent)
{
  if (whoSent)
  {
    QWidget* w = qobject_cast<QWidget*>(whoSent);

    if (m_SelectedFilterWidget == w)
    {
      int index = m_FilterWidgetLayout->indexOf(w);
      if (NULL != m_FilterWidgetLayout->itemAt(index - 1))
      {
        PipelineFilterWidget* widget = qobject_cast<PipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(index - 1)->widget());
        setSelectedFilterWidget(widget);
        widget->setIsSelected(true);
      }
      else if (NULL != m_FilterWidgetLayout->itemAt(index + 1))
      {
        PipelineFilterWidget* widget = qobject_cast<PipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(index + 1)->widget());
        if (NULL != widget)
        {
          setSelectedFilterWidget(widget);
          widget->setIsSelected(true);
        }
        else
        {
          m_SelectedFilterWidget = NULL;
        }
      }
      else
      {
        m_SelectedFilterWidget = NULL;
      }
    }

    m_FilterWidgetLayout->removeWidget(w);

    if (w)
    {
      whoSent->getFilter()->setPreviousFilter(AbstractFilter::NullPointer());
      whoSent->getFilter()->setNextFilter(AbstractFilter::NullPointer());
      w->deleteLater();
    }
  }

  QSpacerItem* spacer = m_FilterWidgetLayout->itemAt(0)->spacerItem();
  if (NULL != spacer)
  {
    m_FilterWidgetLayout->removeItem(spacer);
  }

  reindexWidgetTitles();
  preflightPipeline();

  resetLayout();
  emit pipelineChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::setFilterBeingDragged(PipelineFilterWidget* w)
{
  m_CurrentFilterBeingDragged = w;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::setSelectedFilterWidget(PipelineFilterWidget* w)
{
  if(NULL != m_SelectedFilterWidget && w != m_SelectedFilterWidget)
  {
    m_SelectedFilterWidget->setIsSelected(false);
  }

  m_SelectedFilterWidget = w;

  emit filterInputWidgetChanged(w->getFilterInputWidget());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::setStatusBar(QStatusBar* statusBar)
{
  m_StatusBar = statusBar;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::addDREAM3DReaderFilter(const QString& filePath, int index)
{
  DataContainerReader::Pointer reader = DataContainerReader::New();
  reader->setInputFile(filePath);

  // Create a PipelineFilterWidget using the current AbstractFilter instance to initialize it
  PipelineFilterWidget* w = new PipelineFilterWidget(reader, NULL, this);
  addFilterWidget(w, index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::populatePipelineView(FilterPipeline::Pointer pipeline, int index)
{
  if (NULL == pipeline.get()) { clearWidgets(); return; }

  // get a reference to the filters which are in some type of container object.
  FilterPipeline::FilterContainerType& filters = pipeline->getFilterContainer();
  int fCount = filters.size();

  // QProgressDialog progress("Opening Pipeline File....", "Cancel", 0, fCount, this);
  // progress.setWindowModality(Qt::WindowModal);
  // progress.setMinimumDuration(2000);
  PipelineFilterWidget* firstWidget = NULL;
  // Start looping on each filter

  for (int i = 0; i < fCount; i++)
  {
    //   progress.setValue(i);
    // Create a PipelineFilterWidget using the current AbstractFilter instance to initialize it
    PipelineFilterWidget* w = new PipelineFilterWidget(filters.at(i), NULL, this);

    addFilterWidget(w, index);
    if (index == 0)
    {
      firstWidget = w;
    }

    index++;
  }

  if (firstWidget)
  {
    firstWidget->setIsSelected(true);
  }

  // Now preflight the pipeline for this filter.
  preflightPipeline();

  emit pipelineChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::dragMoveEvent(QDragMoveEvent* event)
{
  m_LastDragPoint = event->pos();

  // Remove the filter widget
  if (NULL != m_FilterWidgetLayout && NULL != m_CurrentFilterBeingDragged && m_FilterWidgetLayout->indexOf(m_CurrentFilterBeingDragged) != -1)
  {
    m_FilterOrigPos = m_FilterWidgetLayout->indexOf(m_CurrentFilterBeingDragged);
    m_FilterWidgetLayout->removeWidget(m_CurrentFilterBeingDragged);
    m_CurrentFilterBeingDragged->setParent(NULL);
  }

  // If cursor is within margin boundaries, start scrolling
  if (shouldAutoScroll(event->pos()))
  {
    startAutoScroll();
  }
  // Otherwise, stop scrolling
  else
  {
    stopAutoScroll();
  }

  const QMimeData* mimedata = event->mimeData();
  if (event->dropAction() == Qt::MoveAction) // WE ONLY deal with this if the user is moving an existing pipeline filter
  {
    // Remove the drop box
    if (NULL != m_FilterWidgetLayout && m_FilterWidgetLayout->indexOf(m_DropBox) != -1)
    {
      m_FilterWidgetLayout->removeWidget(m_DropBox);
      m_DropBox->setParent(NULL);
    }

    bool didInsert = false;
    int count = filterCount();
    for (int i = 0; i < count; ++i)
    {
      PipelineFilterWidget* w = qobject_cast<PipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(i)->widget());
      if (w != NULL && m_CurrentFilterBeingDragged != NULL && w != m_CurrentFilterBeingDragged)
      {
        if (event->pos().y() <= w->geometry().y() + w->geometry().height() / 2)
        {
          m_DropBox->setLabel("    [" + QString::number(i + 1) + "] " + m_CurrentFilterBeingDragged->getHumanLabel());
          m_FilterWidgetLayout->insertWidget(i, m_DropBox);
          reindexWidgetTitles();
          didInsert = true;
          break;
        }
      }
    }
    // Check to see if we are trying to move it to the end
    if (false == didInsert && count > 0)
    {
      PipelineFilterWidget* w = qobject_cast<PipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(count - 2)->widget());
      if (w != NULL && m_CurrentFilterBeingDragged != NULL && w != m_CurrentFilterBeingDragged)
      {
        if (event->pos().y() >= w->geometry().y() + w->geometry().height() / 2)
        {
          m_DropBox->setLabel("    [" + QString::number(count) + "] " + m_CurrentFilterBeingDragged->getHumanLabel());
          m_FilterWidgetLayout->insertWidget(count - 1, m_DropBox);
          reindexWidgetTitles();
        }
      }
    }
  }
  else if (mimedata->hasText())
  {
    QByteArray dropData = mimedata->data("text/plain");
    QString data(dropData);
    QFileInfo fi(data);
    QString ext = fi.completeSuffix();
    FilterManager* fm = FilterManager::Instance();
    if (NULL == fm) { return; }
    IFilterFactory::Pointer wf = fm->getFactoryForFilter(data);

    // If the dragged item is a filter item...
    if (NULL != wf)
    {
      QString humanName = wf->getFilterHumanLabel();

      bool didInsert = false;
      // This path is taken if a filter is dragged from the list of filters
      if (NULL != m_FilterWidgetLayout && m_FilterWidgetLayout->indexOf(m_DropBox) != -1)
      {
        m_FilterWidgetLayout->removeWidget(m_DropBox);
        m_DropBox->setParent(NULL);
      }

      int count = filterCount();
      for (int i = 0; i < count; ++i)
      {
        PipelineFilterWidget* w = qobject_cast<PipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(i)->widget());
        if (NULL != w && event->pos().y() <= w->geometry().y() + w->geometry().height() / 2)
        {
          m_DropBox->setLabel("    [" + QString::number(i + 1) + "] " + humanName);
          m_FilterWidgetLayout->insertWidget(i, m_DropBox);
          reindexWidgetTitles();
          didInsert = true;
          break;
        }
      }
      // Check to see if we are trying to move it to the end
      if (false == didInsert && count > 0)
      {
        PipelineFilterWidget* w = qobject_cast<PipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(count - 2)->widget());
        if (NULL != w && event->pos().y() >= w->geometry().y() + w->geometry().height() / 2)
        {
          m_DropBox->setLabel("    [" + QString::number(count) + "] " + humanName);
          m_FilterWidgetLayout->insertWidget(count - 1, m_DropBox);
          reindexWidgetTitles();
        }
      }

      event->accept();
    }
    // If the dragged item is a pipeline file...
    else if (ext == "dream3d" || ext == "json" || ext == "ini" || ext == "txt")
    {
      QString pipelineName = fi.baseName();

      bool didInsert = false;
      // This path is taken if a filter is dragged from the list of filters
      if (NULL != m_FilterWidgetLayout && m_FilterWidgetLayout->indexOf(m_DropBox) != -1)
      {
        m_FilterWidgetLayout->removeWidget(m_DropBox);
        m_DropBox->setParent(NULL);
      }

      int count = filterCount();
      for (int i = 0; i < count; ++i)
      {
        PipelineFilterWidget* w = qobject_cast<PipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(i)->widget());
        if (NULL != w && event->pos().y() <= w->geometry().y() + w->geometry().height() / 2)
        {
          m_DropBox->setLabel("Place '" + pipelineName + "' Here");
          m_FilterWidgetLayout->insertWidget(i, m_DropBox);
          reindexWidgetTitles();
          didInsert = true;
          break;
        }
      }
      // Check to see if we are trying to move it to the end
      if (false == didInsert && count > 0)
      {
        PipelineFilterWidget* w = qobject_cast<PipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(count - 2)->widget());
        if (NULL != w && event->pos().y() >= w->geometry().y() + w->geometry().height() / 2)
        {
          m_DropBox->setLabel("Place '" + pipelineName + "' Here");
          m_FilterWidgetLayout->insertWidget(count - 1, m_DropBox);
          reindexWidgetTitles();
        }
      }

      event->accept();
    }
    else
    {
      event->ignore();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::dropEvent(QDropEvent* event)
{
  const QMimeData* mimedata = event->mimeData();
  if (m_CurrentFilterBeingDragged != NULL && event->dropAction() == Qt::MoveAction)
  {
    // This path is take if a filter is being dragged around in the pipeline and dropped.
    if (NULL != m_FilterWidgetLayout && m_FilterWidgetLayout->indexOf(m_DropBox) != -1)
    {
      m_FilterWidgetLayout->removeWidget(m_DropBox);
      m_DropBox->setParent(NULL);
    }

    int count = filterCount();
    bool didInsert = false;
    for (int i = 0; i < count; ++i)
    {
      PipelineFilterWidget* w = qobject_cast<PipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(i)->widget());
      if (w != NULL && m_CurrentFilterBeingDragged != NULL && w != m_CurrentFilterBeingDragged)
      {
        if (event->pos().y() < w->geometry().y() + w->geometry().height() / 3)
        {
          m_FilterWidgetLayout->insertWidget(i, m_CurrentFilterBeingDragged);
          setSelectedFilterWidget(m_CurrentFilterBeingDragged);
          reindexWidgetTitles();
          didInsert = true;
          break;
        }
      }
    }
    // Check to see if we are trying to move it to the end
    if (false == didInsert && count > 0)
    {
      PipelineFilterWidget* w = qobject_cast<PipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(count - 2)->widget());
      if (w != NULL && m_CurrentFilterBeingDragged != NULL && w != m_CurrentFilterBeingDragged)
      {
        if (event->pos().y() > w->geometry().y() + w->geometry().height())
        {
          m_FilterWidgetLayout->insertWidget(count - 1, m_CurrentFilterBeingDragged);
          setSelectedFilterWidget(m_CurrentFilterBeingDragged);
          reindexWidgetTitles();
        }
      }
    }

    setSelectedFilterWidget(m_CurrentFilterBeingDragged);
    m_CurrentFilterBeingDragged = NULL;
    m_PreviousFilterBeingDragged = NULL;

    // Make sure the widget titles are all correct
    reindexWidgetTitles();
    preflightPipeline();

    emit pipelineChanged();
    event->acceptProposedAction();
  }
  else if (mimedata->hasUrls() || mimedata->hasText())
  {
    QByteArray dropData = mimedata->data("text/plain");
    QString data(dropData);
    QString filePath;

    if (mimedata->hasUrls())
    {
      QUrl url(data);
      filePath = url.toLocalFile();
    }
    else
    {
      filePath = data;
    }

    QFileInfo fi(filePath);
    QString ext = fi.completeSuffix();
    FilterManager* fm = FilterManager::Instance();
    if (NULL == fm) { return; }
    IFilterFactory::Pointer wf = fm->getFactoryForFilter(data);

    // If the dragged item is a filter item...
    if (NULL != wf)
    {
      // Remove the drop line
      if (NULL != m_FilterWidgetLayout && m_FilterWidgetLayout->indexOf(m_DropBox) != -1)
      {
        m_FilterWidgetLayout->removeWidget(m_DropBox);
        m_DropBox->setParent(NULL);
      }

      // We need to figure out where it was dropped relative to other filters
      int count = filterCount() - 1;
      for (int i = 0; i < count; ++i)
      {
        PipelineFilterWidget* w = qobject_cast<PipelineFilterWidget*>(m_FilterWidgetLayout->itemAt(i)->widget());
        if (w != NULL)
        {
          if (event->pos().y() < w->geometry().y())
          {
            count = i;
            break;
          }
        }
      }

      // Now that we have an index, insert the filter.
      addFilter(data, count);

      emit pipelineChanged();
      event->acceptProposedAction();
    }
    // If the dragged item is a pipeline file...
    else if (ext == "dream3d" || ext == "json" || ext == "ini" || ext == "txt")
    {
      int index = 0;
      if (NULL != m_FilterWidgetLayout)
      {
        index = m_FilterWidgetLayout->indexOf(m_DropBox);
      }

      if (ext == "json" || ext == "ini" || ext == "txt")
      {
        // Remove the drop line
        if (NULL != m_FilterWidgetLayout && index != -1)
        {
          m_FilterWidgetLayout->removeWidget(m_DropBox);
          m_DropBox->setParent(NULL);
        }

        openPipeline(filePath, index, false, false);

        emit pipelineChanged();
      }
      else if (ext == "dream3d")
      {
        FileDragMessageBox* msgBox = new FileDragMessageBox(this);
        msgBox->exec();
        msgBox->deleteLater();

        // Remove the drop line
        if (NULL != m_FilterWidgetLayout && index != -1)
        {
          m_FilterWidgetLayout->removeWidget(m_DropBox);
          m_DropBox->setParent(NULL);
        }

        if (msgBox->didPressOkBtn() == true)
        {
          if (msgBox->isExtractPipelineBtnChecked() == true)
          {
            openPipeline(filePath, index, false, false);
          }
          else
          {
            addDREAM3DReaderFilter(filePath, index);
            emit pipelineChanged();
          }
        }
      }
      event->acceptProposedAction();
    }
    else
    {
      event->ignore();
    }
  }

  // Stop auto scrolling if widget is dropped
  stopAutoScroll();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::dragLeaveEvent(QDragLeaveEvent* event)
{
  int index;
  if (NULL != m_FilterWidgetLayout)
  {
    index = m_FilterWidgetLayout->indexOf(m_DropBox);
  }

  // Remove the drop line
  if (NULL != m_FilterWidgetLayout && index != -1)
  {
    m_FilterWidgetLayout->removeWidget(m_DropBox);
    m_DropBox->setParent(NULL);
  }

  // Put filter widget back to original position
  if (NULL != m_CurrentFilterBeingDragged)
  {
    m_FilterWidgetLayout->insertWidget(m_FilterOrigPos, m_CurrentFilterBeingDragged);
    setSelectedFilterWidget(m_CurrentFilterBeingDragged);
  }

  reindexWidgetTitles();

  // Set the current filter as previous, and nullify the current
  m_PreviousFilterBeingDragged = m_CurrentFilterBeingDragged;
  m_CurrentFilterBeingDragged = NULL;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::dragEnterEvent(QDragEnterEvent* event)
{
  event->acceptProposedAction();

  // If there is a previous filter, set it as current
  if (NULL != m_PreviousFilterBeingDragged && event->dropAction() == Qt::MoveAction)
  {
    m_CurrentFilterBeingDragged = m_PreviousFilterBeingDragged;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::stopAutoScroll()
{
  m_autoScrollTimer.stop();
  m_autoScrollCount = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::startAutoScroll()
{
  int scrollInterval = 50;
  m_autoScrollTimer.start(scrollInterval);
  m_autoScrollCount = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::doAutoScroll()
{
  // find how much we should scroll with
  int verticalStep = m_ScrollArea->verticalScrollBar()->pageStep();
  int horizontalStep = m_ScrollArea->horizontalScrollBar()->pageStep();
  if (m_autoScrollCount < qMax(verticalStep, horizontalStep))
  { m_autoScrollCount = m_autoScrollCount + 15; }

  int margin = m_AutoScrollMargin;
  int verticalValue = m_ScrollArea->verticalScrollBar()->value();
  int horizontalValue = m_ScrollArea->horizontalScrollBar()->value();

  QPoint pos = m_ScrollArea->viewport()->mapFromGlobal(QCursor::pos());
  QRect area = m_ScrollArea->geometry();

  // do the scrolling if we are in the scroll margins
  if (pos.y() - area.top() < margin)
  { m_ScrollArea->verticalScrollBar()->setValue(verticalValue - m_autoScrollCount); }
  else if (area.bottom() - pos.y() < margin)
  { m_ScrollArea-> verticalScrollBar()->setValue(verticalValue + m_autoScrollCount); }
  //  if (pos.x() - area.left() < margin)
  //    m_ScrollArea->horizontalScrollBar()->setValue(horizontalValue - d->m_autoScrollCount);
  //  else if (area.right() - pos.x() < margin)
  //    m_ScrollArea->horizontalScrollBar()->setValue(horizontalValue + d->m_autoScrollCount);
  // if nothing changed, stop scrolling
  bool verticalUnchanged = (verticalValue == m_ScrollArea->verticalScrollBar()->value());
  bool horizontalUnchanged = (horizontalValue == m_ScrollArea->horizontalScrollBar()->value());
  if (verticalUnchanged && horizontalUnchanged)
  {
    stopAutoScroll();
  }
  else
  {
    m_ScrollArea->viewport()->update();
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineViewWidget::shouldAutoScroll(const QPoint& pos)
{
  if (NULL == m_ScrollArea) { return false; }
  QPoint scpos = m_ScrollArea->viewport()->mapFromGlobal(QCursor::pos());
  QRect rect = m_ScrollArea->geometry();

  if (scpos.y() <= getAutoScrollMargin())
  {
    return true;
  }
  else if (pos.y() >= rect.height() - getAutoScrollMargin())
  {
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::setContextMenuActions(QList<QAction*> list)
{
  m_MenuActions = list;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStatusBar* PipelineViewWidget::getStatusBar()
{
  return m_StatusBar;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::handleFilterParameterChanged()
{
  emit filterInputWidgetEdited();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::toRunningState()
{
  setAcceptDrops(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::toIdleState()
{
  setAcceptDrops(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::showFilterHelp(const QString& className)
{

}

