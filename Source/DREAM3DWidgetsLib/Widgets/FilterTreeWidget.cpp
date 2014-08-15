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

#include "FilterTreeWidget.h"

#include <QtGui/QMouseEvent>
#include <QtGui/QApplication>

// #include "DREAM3DWidgetsLib/moc_FilterTreeWidget.cpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterTreeWidget::FilterTreeWidget(QWidget* parent)
  : QTreeWidget(parent)
{
  setAcceptDrops(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterTreeWidget::~FilterTreeWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterTreeWidget::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton)
  { startPos = event->pos(); }
  QTreeWidget::mousePressEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterTreeWidget::mouseMoveEvent(QMouseEvent* event)
{
  if (event->buttons() & Qt::LeftButton)
  {
    int distance = (event->pos() - startPos).manhattanLength();
    if (distance >= QApplication::startDragDistance())
    { performDrag(); }
  }
  QTreeWidget::mouseMoveEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterTreeWidget::performDrag()
{
  QTreeWidgetItem* item = currentItem();
  if(item)
  {
    QMimeData* mimeData = new QMimeData;
    QString filterClassName = item->data(0, Qt::UserRole+1).toString();
    if(filterClassName.isEmpty() == false)
    {
      mimeData->setText(filterClassName);

      QDrag* drag = new QDrag(this);
      drag->setMimeData(mimeData);
      drag->exec(Qt::CopyAction);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterTreeWidget::dragEnterEvent(QDragEnterEvent* event)
{
  FilterTreeWidget* source =
      qobject_cast<FilterTreeWidget*>(event->source());
  if (source && source != this)
  {
    event->setDropAction(Qt::MoveAction);
    event->accept();
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterTreeWidget::dragMoveEvent(QDragMoveEvent* event)
{
  FilterTreeWidget* source =
      qobject_cast<FilterTreeWidget*>(event->source());
  if (source && source != this)
  {
    event->setDropAction(Qt::MoveAction);
    event->accept();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterTreeWidget::dropEvent(QDropEvent* event)
{
#if 0
  FilterTreeWidget* source = qobject_cast<FilterTreeWidget*>(event->source());
  if (source && source != this)
  {
    addItem(event->mimeData()->text());
    event->setDropAction(Qt::MoveAction);
    event->accept();
  }
#endif
}

