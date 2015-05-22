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


#include "BookmarksTreeView.h"

#include <QtGui/QMouseEvent>
#include <QtWidgets/QApplication>
#include <QtCore/QMimeData>
#include <QtGui/QDrag>

#include <iostream>



BookmarksTreeView::BookmarksTreeView(QWidget* parent) :
QTreeView(parent),
m_IndexBeingDragged(QModelIndex()),
m_TopLevelItemPlaceholder(QModelIndex())
{
  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this,
    SIGNAL(customContextMenuRequested(const QPoint&)),
    SLOT(onCustomContextMenuRequested(const QPoint&)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksTreeView::addActionList(QList<QAction*> actionList)
{
  for (int i = 0; i < actionList.size(); i++)
  {
    m_Menu.addAction(actionList[i]);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksTreeView::setNodeActionList(QList<QAction*> list)
{
  m_NodeActions = list;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksTreeView::setLeafActionList(QList<QAction*> list)
{
  m_LeafActions = list;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksTreeView::setDefaultActionList(QList<QAction*> list)
{
  m_DefaultActions = list;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksTreeView::onCustomContextMenuRequested(const QPoint& pos)
{
  //QTreeWidgetItem* item = itemAt(pos);

  //if (item)
  //{
  //  // Note: We must map the point to global from the viewport to
  //  // account for the header.
  //  showContextMenu(item, viewport()->mapToGlobal(pos));
  //}
  //else
  //{
  //  showContextMenu(NULL, mapToGlobal(pos));
  //}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksTreeView::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton)
  {
    m_StartPos = event->pos();

    QModelIndex index = indexAt(event->pos());

    if (index.isValid() == false)
    {
      // Deselect the current item
      clearSelection();
      clearFocus();
      setCurrentIndex(QModelIndex());
    }
  }
  QTreeView::mousePressEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksTreeView::showContextMenu(QTreeWidgetItem* item, const QPoint& globalPos)
{
  // Clear menu's previous actions
  m_Menu.clear();

  if (item == NULL)
  {
    if (!m_LeafActions.isEmpty())
    {
      addActionList(m_DefaultActions);
    }
  }
  else
  {
    int itemType = item->type();
    switch (itemType)
    {
    case Leaf_Item_Type:
      if (!m_LeafActions.isEmpty())
      {
        addActionList(m_LeafActions);
      }
      break;


    case Node_Item_Type:
      if (!m_NodeActions.isEmpty())
      {
        addActionList(m_NodeActions);
      }
      break;
    }
  }

  m_Menu.exec(globalPos);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksTreeView::mouseMoveEvent(QMouseEvent* event)
{
  if (event->buttons() & Qt::LeftButton)
  {
    int distance = (event->pos() - m_StartPos).manhattanLength();
    if (distance >= QApplication::startDragDistance())
    {
      performDrag();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksTreeView::performDrag()
{
  BookmarksModel* model = BookmarksModel::Instance();

  m_IndexBeingDragged = currentIndex();
  if (m_IndexBeingDragged.isValid())
  {
    QMimeData* mimeData = new QMimeData;
    QString path = model->index(m_IndexBeingDragged.row(), 1, QModelIndex()).data().toString();
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
void BookmarksTreeView::dragEnterEvent(QDragEnterEvent* event)
{
  BookmarksTreeView* source =
    qobject_cast<BookmarksTreeView*>(event->source());
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
void BookmarksTreeView::dragLeaveEvent(QDragLeaveEvent* event)
{
  BookmarksModel* model = BookmarksModel::Instance();

  if (m_TopLevelItemPlaceholder.isValid())
  {
    model->removeRow(model->rowCount() - 1, rootIndex());
    m_TopLevelItemPlaceholder = QModelIndex();
  }

  clearSelection();

  setCurrentIndex(m_IndexBeingDragged);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksTreeView::dragMoveEvent(QDragMoveEvent* event)
{
  BookmarksModel* model = BookmarksModel::Instance();
  int topLevelPHPos = model->rowCount();

  QModelIndex index = indexAt(event->pos());
  if (index.isValid() == false || index.row() == m_TopLevelItemPlaceholder.row())
  {
    if (m_TopLevelItemPlaceholder.isValid() == false)
    {
      clearSelection();
      blockSignals(true);
      model->insertRow(topLevelPHPos, rootIndex());
      m_TopLevelItemPlaceholder = model->index(topLevelPHPos, 0, rootIndex());
      model->setData(m_TopLevelItemPlaceholder, topLevelString);
      setCurrentIndex(m_TopLevelItemPlaceholder);
      blockSignals(false);
    }
  }
  else if (index.isValid() && index.row() != m_TopLevelItemPlaceholder.row())
  {
    if (m_TopLevelItemPlaceholder.isValid())
    {
      model->removeRow(topLevelPHPos-1, rootIndex());
      m_TopLevelItemPlaceholder = QModelIndex();
    }
    clearSelection();

    if (model->flags(index).testFlag(Qt::ItemIsDropEnabled) == true)
    {
      //setExpanded(index, true);
      setCurrentIndex(index);
    }
  }

  BookmarksTreeView* source =
    qobject_cast<BookmarksTreeView*>(event->source());
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
void BookmarksTreeView::dropEvent(QDropEvent* event)
{
  BookmarksModel* model = BookmarksModel::Instance();

  QModelIndexList selectList = selectedIndexes();

  if (selectList.size() > 0)
  {
    QModelIndex newParent = selectList[0];

    if (model->flags(newParent).testFlag(Qt::ItemIsDropEnabled) == true && newParent != m_IndexBeingDragged)
    {
      QModelIndex oldParent = m_IndexBeingDragged.parent();

      if (m_TopLevelItemPlaceholder.isValid())
      {
        model->removeRow(model->rowCount() - 1, rootIndex());
        m_TopLevelItemPlaceholder = QModelIndex();
      }

      // Get information needed
      QString name = model->index(m_IndexBeingDragged.row(), Name, oldParent).data().toString();
      QString path = model->index(m_IndexBeingDragged.row(), Path, oldParent).data().toString();
      bool isExpanding = model->isExpanded(m_IndexBeingDragged);

      if (model->removeRow(m_IndexBeingDragged.row(), oldParent))
      {
        model->insertRow(0, newParent);
        QModelIndex newNameIndex = model->index(0, Name, newParent);
        model->setData(newNameIndex, name);
        QModelIndex newPathIndex = model->index(0, Path, newParent);
        model->setData(newPathIndex, path);
      }

      model->sort(Name, Qt::AscendingOrder);
      
      model->setExpanded(newParent, true);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksTreeView::setModel(QAbstractItemModel* model)
{
  // Set the model
  QTreeView::setModel(model);

  BookmarksModel* bModel = qobject_cast<BookmarksModel*>(model);

  // Expand indexes that have their expand member set to true
  expandChildren(QModelIndex(), bModel);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksTreeView::expandChildren(const QModelIndex &parent, BookmarksModel* model)
{
  for (int row = 0; row < model->rowCount(parent); row++)
  {
    QModelIndex index = model->index(row, 0, parent);
    if (model->isExpanded(index))
    {
      expand(index);
    }

    // Recursive call, to expand all items in the tree
    expandChildren(index, model);
  }
}





