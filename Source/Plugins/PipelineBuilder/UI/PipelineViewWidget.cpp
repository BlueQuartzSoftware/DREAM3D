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

#include <QtGui/QMouseEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragLeaveEvent>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QLabel>
#include <QtGui/QPixmap>
#include <QtGui/QVBoxLayout>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DREAM3DFilters.h"


#include "PipelineBuilder/FilterWidgets/QFilterWidgetManager.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineViewWidget::PipelineViewWidget(QWidget* parent) :
QFrame(parent),
m_SelectedFilterWidget(NULL),
m_FilterWidgetLayout(NULL),
m_FilterBeingDragged(NULL)
{
  setupGui();
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
  setAcceptDrops(true);

//  setFrameShape(QFrame::Box);
//  setFrameShadow(QFrame::Plain);
//  setLineWidth(2);


  m_InsertedLabel.setText("Temp Label");

  m_FilterWidgetLayout = new QVBoxLayout(this);
  m_FilterWidgetLayout->setContentsMargins(5, 15, 5, 15);
  m_FilterWidgetLayout->setSpacing(15);
  m_FilterWidgetLayout->setObjectName(QString::fromUtf8("verticalLayout"));
  setLayout(m_FilterWidgetLayout);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PipelineViewWidget::filterCount()
{
  return m_FilterWidgetLayout->count();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterWidget* PipelineViewWidget::filterWidgetAt(int index)
{
  QWidget* w = m_FilterWidgetLayout->itemAt(index)->widget();
  QFilterWidget* fw = qobject_cast<QFilterWidget*>(w);
  return fw;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::clearWidgets()
{
  qint32 count = m_FilterWidgetLayout->count();
  for(qint32 i = count - 1; i >= 0; --i)
  {
    QWidget* w = m_FilterWidgetLayout->itemAt(i)->widget();
    m_FilterWidgetLayout->removeWidget(w);
    w->deleteLater();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterWidget* PipelineViewWidget::addFilter(QString filterName)
{
  QFilterWidgetManager::Pointer wm = QFilterWidgetManager::Instance();
  IFilterWidgetFactory::Pointer wf = wm->getFactoryForFilter(filterName.toStdString());
  QFilterWidget* w = wf->createWidget();

  m_FilterWidgetLayout->addWidget(w);
  w->setParent(this);
  connect(w, SIGNAL(clicked(bool)),
          this, SLOT(removeFilterWidget()) );
  connect(w, SIGNAL(widgetSelected(QFilterWidget*)),
          this, SLOT(setSelectedFilterWidget(QFilterWidget*)) );
  connect(w, SIGNAL(dragStarted(QFilterWidget*)),
          this, SLOT(filterBeingDragged(QFilterWidget*)) );

  setSelectedFilterWidget(w);
  return w;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::addDroppedFilter(QString name)
{
  addFilter(name);
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
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::filterBeingDragged(QFilterWidget* w)
{
 // std::cout << "PipelineViewWidget::filterBeingDragged: " << w->getFilter()->getNameOfClass() << std::endl;
 // m_FilterWidgetLayout->removeWidget(w);
  m_FilterBeingDragged = w;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::setSelectedFilterWidget(QFilterWidget* w)
{
  if(NULL != m_SelectedFilterWidget && w != m_SelectedFilterWidget)
  {
    m_SelectedFilterWidget->changeStyle(false);
  }
  m_SelectedFilterWidget = w;

  if(NULL != m_SelectedFilterWidget)
  {
    m_SelectedFilterWidget->changeStyle(true);
#if 0
    qint32 selectedIndex = m_FilterWidgetLayout->indexOf(m_SelectedFilterWidget);
    qint32 count = m_FilterWidgetLayout->count();
    if(count > 1)
    {
      filterUp->setEnabled(true);
      filterDown->setEnabled(true);
    }
    else
    {
      filterUp->setEnabled(false);
      filterDown->setEnabled(false);
    }

    if(selectedIndex == 0)
    {
      filterUp->setEnabled(false);
    }
    else if(selectedIndex == count - 1)
    {
      filterDown->setEnabled(false);
    }
#endif
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::dragEnterEvent( QDragEnterEvent* event)
{
  event->acceptProposedAction();
#if 0
  std::cout << "PipelineViewWidget::dragEnterEvent: " << event->pos().x() << ", " << event->pos().y() << std::endl;
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
//  std::cout << "PipelineViewWidget::dragMoveEvent: " << event->pos().x() << ", " << event->pos().y() << std::endl;
//  QFilterWidget* w = qobject_cast<QFilterWidget*>(childAt(event->pos()));
//  if(w != NULL)
//  {
//    std::cout << "PipelineViewWidget::dragMoveEvent: QFilterWidget Found: " << w->getFilter()->getNameOfClass() << std::endl;
//  }
//  QVBoxLayout* l = qobject_cast<QVBoxLayout*>(childAt(event->pos()));
//  if(l != NULL)
//  {
//    std::cout << "PipelineViewWidget::dragMoveEvent: Found the QVBoxLayout" << std::endl;
//  }
  QObject* o = qobject_cast<QObject*>(childAt(event->pos()));
  if(o == NULL)
  {
    int count = m_FilterWidgetLayout->count();
    for (int i = 0; i < count; ++i)
    {
      QFilterWidget* w = qobject_cast<QFilterWidget*>(m_FilterWidgetLayout->itemAt(i)->widget());
      if(w != NULL && w != m_FilterBeingDragged)
      {
        if(event->pos().y() < w->geometry().y())
        {
          m_FilterWidgetLayout->removeWidget(m_FilterBeingDragged);
          break;
        }
      }
    }
    bool didInsert = false;
    count = m_FilterWidgetLayout->count();
    for (int i = 0; i < count; ++i)
    {
      QFilterWidget* w = qobject_cast<QFilterWidget*>(m_FilterWidgetLayout->itemAt(i)->widget());
      if(w != NULL && w != m_FilterBeingDragged)
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
    if (false == didInsert)
    {
      QFilterWidget* w = qobject_cast<QFilterWidget*>(m_FilterWidgetLayout->itemAt(count - 1)->widget());
      if(w != NULL && w != m_FilterBeingDragged)
      {
        if(event->pos().y() > w->geometry().y() + w->geometry().height())
        {
          m_FilterWidgetLayout->insertWidget(count - 1, m_FilterBeingDragged);
          setSelectedFilterWidget(m_FilterBeingDragged);
          didInsert = true;
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
//  std::cout << "PipelineViewWidget::dropEvent: " << event->pos().x() << ", " << event->pos().y() << std::endl;
  if (m_FilterBeingDragged != NULL) {
    setSelectedFilterWidget(m_FilterBeingDragged);
    m_FilterBeingDragged = NULL;
  }
  event->acceptProposedAction();
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewWidget::mousePressEvent(QMouseEvent *event)
{
  std::cout << "PipelineViewWidget::mousePressEvent: " << event->pos().x() << ", " << event->pos().y() << std::endl;
    QFilterWidget* w = static_cast<QFilterWidget*>(childAt(event->pos()));
    if (!w)
        return;

    QPixmap pixmap = QPixmap::grabWidget(w);
    if (pixmap.isNull() == false)
    {
      std::cout << "We got an image of the widget." << std::endl;
    }

}
#endif
