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

#include <iostream>



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
  m_ItemBeingDragged = currentItem();
  if (m_ItemBeingDragged)
  {
    QMimeData* mimeData = new QMimeData;
    QString path = m_ItemBeingDragged->data(1, Qt::UserRole).toString();
    if (path.isEmpty() == false)
    {
      mimeData->setText(path);

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
  else
  {
    event->acceptProposedAction();
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryTreeWidget::dragMoveEvent(QDragMoveEvent* event)
{
  QTreeWidgetItem* item = itemAt(event->pos());
  if (NULL != item && item->flags().testFlag(Qt::ItemIsDropEnabled) == true)
  {
    clearSelection();
    item->setSelected(true);
  }
  else if (NULL == item)
  {
    //blockSignals(true);
    //QTreeWidgetItem* topLevelItem = new QTreeWidgetItem(this, m_ItemBeingDragged->type());
    //topLevelItem->setText(0, "[Top Level]");
    //topLevelItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
    //topLevelItem->
    //blockSignals(false);
  }
  else
  {
    clearSelection();
  }

  FilterLibraryTreeWidget* source =
    qobject_cast<FilterLibraryTreeWidget*>(event->source());
  if (source && source != this)
  {
    event->setDropAction(Qt::MoveAction);
    event->accept();
  }
  else
  {
    event->acceptProposedAction();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryTreeWidget::dropEvent(QDropEvent* event)
{
  QTreeWidgetItem* parent = itemAt(event->pos());
  if (NULL != parent && parent->flags().testFlag(Qt::ItemIsDropEnabled) == true && parent != m_ItemBeingDragged)
  {
    // Get information needed
    QString title = m_ItemBeingDragged->data(0, Qt::UserRole).toString();
    QString path = m_ItemBeingDragged->data(1, Qt::UserRole).toString();
    bool isExpanding = m_ItemBeingDragged->isExpanded();
    FilterLibraryTreeWidget::ItemType type = FilterLibraryTreeWidget::ItemType(m_ItemBeingDragged->type());

    // Remove the old tree widget item
    delete m_ItemBeingDragged;

    QIcon icon;
    // If the dragged item is a folder
    if (type == FilterLibraryTreeWidget::Node_Item_Type)
    {
      icon = QIcon(":/folder_blue.png");
    }
    // The dragged item is a pipeline
    else
    {
      icon = QIcon(":/text.png");
    }

    emit itemWasDropped(parent, title, icon, type, path, true, false, isExpanding);
    parent->setExpanded(true);
  }
}
