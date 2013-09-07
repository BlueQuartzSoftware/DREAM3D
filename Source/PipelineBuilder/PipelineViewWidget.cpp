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
#include "PipelineViewWidget.h"

#include <iostream>


#include <QtCore/QFileInfo>
#include <QtCore/QUrl>
#include <QtCore/QDir>
#include <QtGui/QMouseEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragLeaveEvent>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QLabel>
#include <QtGui/QPixmap>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QScrollArea>
#include <QtGui/QScrollBar>


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/PipelineMessage.h"
#include "DREAM3DLib/DREAM3DFilters.h"


#include "FilterWidgetManager.h"
#include "QFilterPipeline.h"
#include "QDroppableScrollArea.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineViewWidget::PipelineViewWidget(QWidget* parent) :
QFrame(parent),
m_SelectedFilterWidget(NULL),
m_FilterWidgetLayout(NULL),
m_FilterBeingDragged(NULL),
m_DropIndex(-1),
m_EmptyPipelineLabel(NULL),
errorTableWidget(NULL),
m_AutoScroll(false),
m_AutoScrollMargin(10)
{
  setupGui();
  m_LastDragPoint = QPoint(-1, -1);
  m_autoScrollTimer.setParent(this);
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
void PipelineViewWidget::setupGui()
{
  newEmptyPipelineViewLayout();
  connect(&m_autoScrollTimer, SIGNAL(timeout()), this, SLOT(doAutoScroll()));
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
    m_EmptyPipelineLabel->setMinimumSize(QSize(325, 50));
    m_EmptyPipelineLabel->setStyleSheet(QString::fromUtf8("QLabel {\n"
                                                          "border-radius: 20px;\n"
                                                          "border: 1px solid rgb(120, 120, 120);\n"
                                                          "background-color: rgb(160, 160, 160);\n"
                                                          "font-weight: bold;\n"
                                                          "color : rgb(255, 255, 255);\n"
                                                          "text-align: center;\n"
                                                          "margin: 5px;\n"
                                                          "padding: 10px;\n"
                                                          "}"));
    m_EmptyPipelineLabel->setAlignment(Qt::AlignCenter);
    m_EmptyPipelineLabel->setText("Drag filters here to build up a pipeline");

    gridLayout->addWidget(m_EmptyPipelineLabel, 1, 1, 1, 1);

    QSpacerItem* horizontalSpacer_4 = new QSpacerItem(102, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addItem(horizontalSpacer_4, 1, 2, 1, 1);

    QSpacerItem* verticalSpacer_2 = new QSpacerItem(20, 341, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(verticalSpacer_2, 2, 1, 1, 1);

  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::setErrorsTextArea(QTableWidget* t)
{
  this->errorTableWidget = t;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PipelineViewWidget::filterCount()
{
  int count = 0;
  if (NULL != m_FilterWidgetLayout) {
    count = m_FilterWidgetLayout->count() - 1;
  }
  return count;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterWidget* PipelineViewWidget::filterWidgetAt(int index)
{
  QFilterWidget* fw = NULL;
  if (m_FilterWidgetLayout != NULL) {
    QWidget* w = m_FilterWidgetLayout->itemAt(index)->widget();
    fw = qobject_cast<QFilterWidget*>(w);
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
    // Remove the current Layout
    QLayout* l = layout();
    if (NULL != l && l == m_FilterWidgetLayout) {
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
  qint32 count = filterCount();
  for(qint32 i = count - 1; i >= 0; --i)
  {
    QWidget* w = m_FilterWidgetLayout->itemAt(i)->widget();
    if (NULL != w) {
      m_FilterWidgetLayout->removeWidget(w);
      w->deleteLater();
    }
  }
  m_SelectedFilterWidget = NULL;
  resetLayout();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterWidget* PipelineViewWidget::addFilter(QString filterName, int index)
{

  FilterWidgetManager::Pointer wm = FilterWidgetManager::Instance();
  IFilterWidgetFactory::Pointer wf = wm->getFactoryForFilter(filterName.toStdString());
  if (NULL == wf) { return NULL;}
  QFilterWidget* w = wf->createWidget();
  if (index < 0) // If the programmer wants to add it to the end of the list
  {
    index = filterCount();
  }

  if (filterCount() <= 0)
  {
    if (NULL != m_EmptyPipelineLabel) {
      m_EmptyPipelineLabel->hide();
      delete m_EmptyPipelineLabel;
      m_EmptyPipelineLabel = NULL;
    }
    QLayout* l = layout();
    if (NULL != l) {
      qDeleteAll(l->children());
      delete l;
    }

    m_FilterWidgetLayout = new QVBoxLayout(this);
    m_FilterWidgetLayout->setObjectName(QString::fromUtf8("m_FilterWidgetLayout"));
    m_FilterWidgetLayout->setContentsMargins(5, 15, 5, 15);
    m_FilterWidgetLayout->setSpacing(15);
    QSpacerItem* verticalSpacer = new QSpacerItem(20, 361, QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_FilterWidgetLayout->addItem(verticalSpacer);
  }


  m_FilterWidgetLayout->insertWidget(index, w);
  w->setParent(this);
  connect(w, SIGNAL(clicked(bool)),
          this, SLOT(removeFilterWidget()) );
  connect(w, SIGNAL(widgetSelected(QFilterWidget*)),
          this, SLOT(setSelectedFilterWidget(QFilterWidget*)) );
  connect(w, SIGNAL(dragStarted(QFilterWidget*)),
          this, SLOT(setFilterBeingDragged(QFilterWidget*)) );
  connect(w, SIGNAL(parametersChanged()),
          this, SLOT(preflightPipeline()));

  setSelectedFilterWidget(w);
  //preflightPipeline();

  return w;
}


#define CONVERT_STD_LIST_TO_QLIST(dataContainer, type, filterWidget)\
{\
  std::list<std::string> theList = dataContainer->get##type##ArrayNameList();\
  QList<QString> list;\
  for(std::list<std::string>::iterator iter = theList.begin(); iter != theList.end(); ++iter)  {\
    list << QString::fromStdString(*iter);\
  }\
  PipelineArraySelectionWidget* ptr = filterWidget->getPipelineArraySelectionWidget();\
  if (NULL != ptr) { ptr->setPossible##type##ArrayNames(list); }\
}\

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::preflightPipeline()
{
  // clear all the error messages
  m_PipelineErrorList.clear();

  for (int i=0; i<errorTableWidget->rowCount(); ++i) {
    errorTableWidget->removeRow(i);
  }
  errorTableWidget->setRowCount(0);


  // Create the DataContainer object
  VolumeDataContainer::Pointer vl = VolumeDataContainer::New();
  SurfaceDataContainer::Pointer s = SurfaceDataContainer::New();
  EdgeDataContainer::Pointer e = EdgeDataContainer::New();
  VertexDataContainer::Pointer v = VertexDataContainer::New();

  std::stringstream ss;


  // Build up the pipeline
  bool pipelineHasErrors = false;
  qint32 count = filterCount();
  for(qint32 i = 0; i < count; ++i)
  {
    QFilterWidget* fw = filterWidgetAt(i);
    if (fw)
    {
      fw->setHasPreflightErrors(false);
      fw->setHasPreflightWarnings(false);
      fw->preflightAboutToExecute(vl, s, e, v);

      AbstractFilter::Pointer filter = fw->getFilter(false);

      filter->setVolumeDataContainer(vl.get());
      filter->setSurfaceDataContainer(s.get());
      filter->setEdgeDataContainer(e.get());
      filter->setVertexDataContainer(v.get());

      filter->preflight();
      int err = filter->getErrorCondition();
      std::vector<PipelineMessage> msgs = filter->getPipelineMessages();
      if(msgs.size() > 0 || err < 0)
      {
        preflightErrorMessage(msgs);
        for(std::vector<PipelineMessage>::iterator iter = msgs.begin(); iter != msgs.end(); ++iter)
        {
          if ( (*iter).getMessageType() == PipelineMessage::Error)
          {
            fw->setHasPreflightErrors(true);
            pipelineHasErrors = true;

          }
          else if ((*iter).getMessageType() == PipelineMessage::Warning)
          {
            fw->setHasPreflightWarnings(true);
          }
        }
      }

      // Tell the widget that we have arrays and to possibly update its gui
      fw->preflightDoneExecuting(vl, s, e, v);
    }
  }

  if (pipelineHasErrors == true)
  {
    emit pipelineHasErrorsSignal();
  }
  else
  {
    emit pipelineHasNoErrors();
  }

  errorTableWidget->resizeRowsToContents();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::preflightErrorMessage(std::vector<PipelineMessage> errorStream)
{
  if(NULL != errorTableWidget)
  {

   // int rc = errorTableWidget->rowCount();

    for (std::vector<PipelineMessage>::size_type i = 0; i < errorStream.size(); ++i)
    {
      emit preflightHasMessage(errorStream.at(i));

    #if 0
      errorTableWidget->insertRow(rc);

      QString filterName = QString::fromStdString(errorStream.at(i).getFilterName());
      QString errorDescription = QString::fromStdString(errorStream.at(i).getMessageText());
      int errorCode = errorStream.at(i).getMessageCode();

      QTableWidgetItem* filterNameWidgetItem = new QTableWidgetItem(filterName);
      filterNameWidgetItem->setTextAlignment(Qt::AlignCenter);
      QTableWidgetItem* errorDescriptionWidgetItem = new QTableWidgetItem(errorDescription);

      QTableWidgetItem* errorCodeWidgetItem = new QTableWidgetItem(QString::number(errorCode));
      errorCodeWidgetItem->setTextAlignment(Qt::AlignCenter);
      PipelineMessage& msg = errorStream.at(i);
      QColor errColor(255, 255, 255);
      if (msg.getMessageType() == PipelineMessage::Error) {
        errColor = QColor(255, 191, 193);
      } else if (msg.getMessageType() == PipelineMessage::Warning) {
        errColor = QColor(251, 254, 137);
      }
      QBrush errBrush(errColor);

      filterNameWidgetItem->setBackground(errBrush);
      errorDescriptionWidgetItem->setBackground(errBrush);
      errorCodeWidgetItem->setBackground(errBrush);

      errorTableWidget->setItem(rc, 0, filterNameWidgetItem);
      errorTableWidget->setItem(rc, 1, errorDescriptionWidgetItem);
      errorTableWidget->setItem(rc, 2, errorCodeWidgetItem);
      #endif
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::removeFilterWidget()
{
  QObject* whoSent = sender();
  if (whoSent)
  {
    QWidget* w = qobject_cast<QWidget*>(whoSent);

    m_FilterWidgetLayout->removeWidget(w);
    if (m_SelectedFilterWidget == w)
    {
      m_SelectedFilterWidget = NULL;
    }
    if (w) {
      w->deleteLater();
    }
  }
  preflightPipeline();

  resetLayout();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::setFilterBeingDragged(QFilterWidget* w)
{
  m_FilterBeingDragged = w;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::setSelectedFilterWidget(QFilterWidget* w)
{

  if(NULL != m_SelectedFilterWidget && w != m_SelectedFilterWidget)
  {
    m_SelectedFilterWidget->setIsSelected(false);
  }
  m_SelectedFilterWidget = w;

  if(NULL != m_SelectedFilterWidget)
  {
    m_SelectedFilterWidget->setIsSelected(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::dragEnterEvent( QDragEnterEvent* event)
{
  event->acceptProposedAction();
 #if 0

  QFilterWidget* w = qobject_cast<QFilterWidget*>(childAt(event->pos()));
  if (w != NULL)
  {
    std::cout << "QFilterWidget Found: " << w->getFilter()->getNameOfClass() << std::endl;
  }
  QVBoxLayout* l = qobject_cast<QVBoxLayout*>(childAt(event->pos()));
  if (l != NULL)
  {
    std::cout << "PipelineViewWidget::dragEnterEvent: Found the QVBoxLayout" << std::endl;
  }
  PipelineViewWidget* o = qobject_cast<PipelineViewWidget*>(childAt(event->pos()));
  if (o != NULL)
  {
    std::cout << "PipelineViewWidget::dragEnterEvent: " << o->objectName().toStdString() << std::endl;
  }
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::dragMoveEvent( QDragMoveEvent* event)
{
  m_LastDragPoint = event->pos();

  // If cursor is within margin boundaries, start scrolling
  if ( shouldAutoScroll( event->pos() ) )
  {
    startAutoScroll();
  }
  // Otherwise, stop scrolling
  else
  {
    stopAutoScroll();
  }

  QObject* o = qobject_cast<QObject*>(childAt(event->pos()));
  if(o == NULL && event->dropAction() == Qt::MoveAction) // WE ONLY deal with this if the user is moving an existing pipeline filter
  {
    int count = filterCount();
    for (int i = 0; i < count; ++i)
    {
      QFilterWidget* w = qobject_cast<QFilterWidget*>(m_FilterWidgetLayout->itemAt(i)->widget());
      if(w != NULL && m_FilterBeingDragged != NULL && w != m_FilterBeingDragged)
      {
        if(event->pos().y() < w->geometry().y())
        {
          m_FilterWidgetLayout->removeWidget(m_FilterBeingDragged);
          break;
        }
      }
    }
    bool didInsert = false;
    count = filterCount();
    for (int i = 0; i < count; ++i)
    {
      QFilterWidget* w = qobject_cast<QFilterWidget*>(m_FilterWidgetLayout->itemAt(i)->widget());
      if(w != NULL && m_FilterBeingDragged != NULL && w != m_FilterBeingDragged)
      {
        if(event->pos().y() < w->geometry().y())
        {
          m_FilterWidgetLayout->insertWidget(i, m_FilterBeingDragged);
          setSelectedFilterWidget(m_FilterBeingDragged);
          didInsert = true;
          break;
        }
      }
    }
    // Check to see if we are trying to move it to the end
    if (false == didInsert && count > 0)
    {
      QFilterWidget* w = qobject_cast<QFilterWidget*>(m_FilterWidgetLayout->itemAt(count - 1)->widget());
      if(w != NULL && m_FilterBeingDragged != NULL && w != m_FilterBeingDragged)
      {
        if(event->pos().y() > w->geometry().y() + w->geometry().height())
        {
          m_FilterWidgetLayout->insertWidget(count - 1, m_FilterBeingDragged);
          setSelectedFilterWidget(m_FilterBeingDragged);
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::dropEvent(QDropEvent *event)
{
  if (event->mimeData()->hasUrls())
  {
    QList<QUrl> urlList;
    QString fName;
    urlList = event->mimeData()->urls(); // returns list of QUrls
    // if just text was dropped, urlList is empty (size == 0)

    if ( urlList.size() > 0) // if at least one QUrl is present in list
    {
      fName = urlList[0].toLocalFile(); // convert first QUrl to local path
      fName = QDir::toNativeSeparators(fName);
      emit pipelineFileDropped(fName);
    }
  }
  else if(m_FilterBeingDragged != NULL && event->dropAction() == Qt::MoveAction)
  {
    setSelectedFilterWidget(m_FilterBeingDragged);
    m_FilterBeingDragged = NULL;
    preflightPipeline();
  }
  else
  {  // This path is taken if a filter is dropped from the list of filters
    if(event->mimeData()->hasText())
    {
      QByteArray dropData = event->mimeData()->data("text/plain");
      QString name(dropData);
      // We need to figure out where it was dropped relative to other filters
      int count = filterCount();
      for (int i = 0; i < count; ++i)
      {
        QFilterWidget* w = qobject_cast<QFilterWidget*>(m_FilterWidgetLayout->itemAt(i)->widget());
        if(w != NULL)
        {
          if(event->pos().y() < w->geometry().y())
          {
            count = i;
            break;
          }
        }
      }
      // Now that we have an index, insert the filter.
      addFilter(name, count);
      preflightPipeline();
    }
  }
  event->acceptProposedAction();

  // Stop auto scrolling if widget is dropped
  stopAutoScroll();
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
    m_autoScrollCount = m_autoScrollCount + 15;

  int margin = m_AutoScrollMargin;
  int verticalValue = m_ScrollArea->verticalScrollBar()->value();
  int horizontalValue = m_ScrollArea->horizontalScrollBar()->value();

  QPoint pos = m_ScrollArea->viewport()->mapFromGlobal(QCursor::pos());
  QRect area = m_ScrollArea->geometry();

  // do the scrolling if we are in the scroll margins
  if (pos.y() - area.top() < margin)
    m_ScrollArea->verticalScrollBar()->setValue(verticalValue - m_autoScrollCount);
  else if (area.bottom() - pos.y() < margin)
    m_ScrollArea-> verticalScrollBar()->setValue(verticalValue + m_autoScrollCount);
//  if (pos.x() - area.left() < margin)
//    m_ScrollArea->horizontalScrollBar()->setValue(horizontalValue - d->m_autoScrollCount);
//  else if (area.right() - pos.x() < margin)
//    m_ScrollArea->horizontalScrollBar()->setValue(horizontalValue + d->m_autoScrollCount);
  // if nothing changed, stop scrolling
  bool verticalUnchanged = (verticalValue == m_ScrollArea->verticalScrollBar()->value());
  bool horizontalUnchanged = (horizontalValue == m_ScrollArea->horizontalScrollBar()->value());
  if (verticalUnchanged && horizontalUnchanged) {
    stopAutoScroll();
  } else {
    m_ScrollArea->viewport()->update();
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineViewWidget::shouldAutoScroll(const QPoint &pos)
{
  QRect rect = m_ScrollArea->geometry();
  QPoint scpos = m_ScrollArea->viewport()->mapFromGlobal(QCursor::pos());

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
