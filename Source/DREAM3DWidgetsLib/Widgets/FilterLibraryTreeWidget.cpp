/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2012 Joseph B. Kleingers (Intern, University of Dayton)
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

#include "FilterLibraryTreeWidget.h"

#include <QtGui/QMouseEvent>
#include <QtWidgets/QApplication>
#include <QtCore/QMimeData>
#include <QtGui/QDrag>



FilterLibraryTreeWidget::FilterLibraryTreeWidget(QWidget* parent) :
  QTreeWidget(parent)
{
  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this,
          SIGNAL(customContextMenuRequested(const QPoint&)),
          SLOT(onCustomContextMenuRequested(const QPoint&)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryTreeWidget::addActionList(QList<QAction*> actionList)
{
  for (int i = 0; i < actionList.size(); i++)
  {
    m_Menu.addAction(actionList[i]);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryTreeWidget::setNodeActionList(QList<QAction*> list)
{
  m_NodeActions = list;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryTreeWidget::setLeafActionList(QList<QAction*> list)
{
  m_LeafActions = list;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryTreeWidget::setDefaultActionList(QList<QAction*> list)
{
  m_DefaultActions = list;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryTreeWidget::onCustomContextMenuRequested(const QPoint& pos)
{
  QTreeWidgetItem* item = itemAt(pos);

  if (item)
  {
    // Note: We must map the point to global from the viewport to
    // account for the header.
    showContextMenu( item, viewport()->mapToGlobal(pos) );
  }
  else
  {
    showContextMenu( NULL, mapToGlobal(pos) );
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryTreeWidget::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton)
  {
    m_StartPos = event->pos();

    QTreeWidgetItem* item = itemAt(event->pos());

    if (NULL == item)
    {
      // Deselect the current item
      clearSelection();
      clearFocus();
      setCurrentItem(NULL);
    }
  }
  QTreeWidget::mousePressEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryTreeWidget::showContextMenu(QTreeWidgetItem* item, const QPoint& globalPos)
{
  // Clear menu's previous actions
  m_Menu.clear();

  if (item == NULL)
  {
    if ( !m_LeafActions.isEmpty() )
    { addActionList(m_DefaultActions); }
  }
  else
  {
    int itemType = item->type();
    switch (itemType)
    {
      case Leaf_Item_Type:
        if ( !m_LeafActions.isEmpty() )
        { addActionList(m_LeafActions); }
        break;


      case Node_Item_Type:
        if ( !m_NodeActions.isEmpty() )
        { addActionList(m_NodeActions); }
        break;
    }
  }

  m_Menu.exec(globalPos);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryTreeWidget::mouseMoveEvent(QMouseEvent* event)
{
  if (event->buttons() & Qt::LeftButton)
  {
    int distance = (event->pos() - m_StartPos).manhattanLength();
    if (distance >= QApplication::startDragDistance())
    {
      performDrag();
    }
  }
  QTreeWidget::mouseMoveEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryTreeWidget::performDrag()
{
  QTreeWidgetItem* item = currentItem();
  if (item)
  {
    QMimeData* mimeData = new QMimeData;
    QString path = item->data(0, Qt::UserRole).toString();
    QUrl url(path);
    QList<QUrl> list;
    list.append(url);
    if (path.isEmpty() == false)
    {
      mimeData->setUrls(list);

      QDrag* drag = new QDrag(this);
      drag->setMimeData(mimeData);
      drag->exec(Qt::CopyAction);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryTreeWidget::dragEnterEvent(QDragEnterEvent* event)
{
  FilterLibraryTreeWidget* source =
    qobject_cast<FilterLibraryTreeWidget*>(event->source());
  if (source && source != this)
  {
    event->setDropAction(Qt::MoveAction);
    event->accept();
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryTreeWidget::dragMoveEvent(QDragMoveEvent* event)
{
  FilterLibraryTreeWidget* source =
    qobject_cast<FilterLibraryTreeWidget*>(event->source());
  if (source && source != this)
  {
    event->setDropAction(Qt::MoveAction);
    event->accept();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryTreeWidget::dropEvent(QDropEvent* event)
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
