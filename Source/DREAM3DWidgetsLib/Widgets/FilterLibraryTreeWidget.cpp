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


#include "FilterLibraryTreeWidget.h"

#include <QtGui/QMouseEvent>
#include <QtWidgets/QApplication>
#include <QtCore/QMimeData>
#include <QtGui/QDrag>

#include <iostream>

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_FilterLibraryTreeWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterLibraryTreeWidget::FilterLibraryTreeWidget(QWidget* parent) :
  QTreeWidget(parent),
  m_ItemBeingDragged(NULL),
  m_TopLevelItemPlaceholder(NULL)
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
void FilterLibraryTreeWidget::dragLeaveEvent(QDragLeaveEvent* event)
{
  if (NULL != m_TopLevelItemPlaceholder)
  {
    delete m_TopLevelItemPlaceholder;
    m_TopLevelItemPlaceholder = NULL;
  }

  clearSelection();

  m_ItemBeingDragged->setSelected(true);
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
  else if (NULL == item || item == m_TopLevelItemPlaceholder)
  {
    if (NULL == m_TopLevelItemPlaceholder)
    {
      clearSelection();
      blockSignals(true);
      m_TopLevelItemPlaceholder = new QTreeWidgetItem(this, m_ItemBeingDragged->type());
      m_TopLevelItemPlaceholder->setText(0, "[Top Level]");
      m_TopLevelItemPlaceholder->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
      m_TopLevelItemPlaceholder->setSelected(true);
      blockSignals(false);
    }
  }
  else
  {
    if (NULL != m_TopLevelItemPlaceholder)
    {
      delete m_TopLevelItemPlaceholder;
      m_TopLevelItemPlaceholder = NULL;
    }
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
  QList<QTreeWidgetItem*> parents = selectedItems();

  if (parents.size() == 1)
  {
    QTreeWidgetItem* parent = parents[0];
    if (parent->flags().testFlag(Qt::ItemIsDropEnabled) == true && parent != m_ItemBeingDragged)
    {
      if (NULL != m_TopLevelItemPlaceholder)
      {
        parent = invisibleRootItem();
        delete m_TopLevelItemPlaceholder;
        m_TopLevelItemPlaceholder = NULL;
      }

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
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterLibraryTreeWidget* FilterLibraryTreeWidget::FromJsonObject(QJsonObject treeObject)
{
  QTreeWidgetItem* root = FilterLibraryTreeWidget::UnwrapTreeItem(treeObject);

  QList<QTreeWidgetItem*> list = root->takeChildren();

  FilterLibraryTreeWidget* tree = new FilterLibraryTreeWidget();
  tree->addTopLevelItems(list);

  QTreeWidgetItemIterator iter(tree);

  while (*iter)
  {
    QTreeWidgetItem* item = *iter;
    bool expanded = item->data(2, Qt::UserRole).toBool();
    item->setExpanded(expanded);

    ++iter;
  }

  tree->sortItems(0, Qt::AscendingOrder);
  return tree;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject FilterLibraryTreeWidget::toJsonObject()
{
  QJsonObject treeObj;
  treeObj["Type"] = FilterLibraryTreeWidget::Node_Item_Type;
  for (int i = 0; i < topLevelItemCount(); i++)
  {
    QJsonObject topLevelObj = wrapTreeItem(topLevelItem(i));
    treeObj["Child " + QString::number(i + 1)] = topLevelObj;
  }
  return treeObj;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject FilterLibraryTreeWidget::wrapTreeItem(QTreeWidgetItem* item)
{
  QJsonObject obj;
  QList<QTreeWidgetItem*> childList = item->takeChildren();

  for (int i = 0; i < childList.size(); i++)
  {
    QTreeWidgetItem* child = childList[i];
    obj.insert("Child " + QString::number(i + 1), wrapTreeItem(child));
  }

  obj.insert("Name", item->text(0));
  obj.insert("Type", item->type());

  if (item->type() == FilterLibraryTreeWidget::Node_Item_Type)
  {
    obj.insert("Expanded", item->isExpanded());
  }
  else
  {
    obj.insert("Path", item->text(1));
  }

  return obj;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QTreeWidgetItem* FilterLibraryTreeWidget::UnwrapTreeItem(QJsonObject object)
{
  FilterLibraryTreeWidget::ItemType type = FilterLibraryTreeWidget::ItemType(object["Type"].toInt());
  QTreeWidgetItem* item = new QTreeWidgetItem(type);

  QString name = object["Name"].toString();
  item->setText(0, name);
  item->setData(0, Qt::UserRole, name);

  if (type == FilterLibraryTreeWidget::Node_Item_Type)
  {
    item->setIcon(0, QIcon(":/folder_blue.png"));
    item->setFlags(item->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
    item->setData(2, Qt::UserRole, object["Expanded"].toBool());
    for (QJsonObject::iterator iter = object.begin(); iter != object.end(); ++iter)
    {
      if (iter.value().isObject())
      {
        QJsonObject childObj = iter.value().toObject();
        QTreeWidgetItem* child = FilterLibraryTreeWidget::UnwrapTreeItem(childObj);
        item->insertChild(0, child);
      }
    }
  }
  else
  {
    item->setIcon(0, QIcon(":/text.png"));
    item->setFlags(item->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled);
    item->setText(1, object["Path"].toString());
    item->setData(1, Qt::UserRole, object["Path"].toString());
  }

  return item;
}
