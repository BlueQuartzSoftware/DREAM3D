#include "TreeWidgetBuilder.h"

#include "PipelineBuilderWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TreeWidgetBuilder::onCustomContextMenuRequested(const QPoint& pos)
{
  printf("hi\n");
  QTreeWidgetItem* item = itemAt(pos);

  if (item)
  {
    // Note: We must map the point to global from the viewport to
    // account for the header.
    showContextMenu( item, viewport()->mapToGlobal(pos) );
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TreeWidgetBuilder::showContextMenu(QTreeWidgetItem* item, const QPoint& globalPos)
{
  // Clear menu's previous actions
  m_Menu.clear();

  int itemType = item->type();
  switch (itemType)
  {
    case Favorite_Item_Type:
      addActionListFavorites(m_FavoritesActions);
      break;

    case Default_Item_Type:
      break;
  }

  m_Menu.exec(globalPos);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TreeWidgetBuilder::addActionListFavorites(QList<QAction*> actionList)
{
  for (int i=0; i<actionList.size(); i++)
  {
    m_Menu.addAction(actionList[i]);
  }
}


void TreeWidgetBuilder::setFavoritesActionList(QList<QAction*> list)
{
  m_FavoritesActions = list;
}
