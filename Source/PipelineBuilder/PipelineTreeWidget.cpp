#include "PipelineTreeWidget.h"

#include "PipelineBuilderWidget.h"

PipelineTreeWidget::PipelineTreeWidget(QFrame* parent) :
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
void PipelineTreeWidget::addActionList(QList<QAction*> actionList)
{
	for (int i=0; i<actionList.size(); i++)
	{
		m_Menu.addAction(actionList[i]);
	}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeWidget::onCustomContextMenuRequested(const QPoint& pos) 
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
void PipelineTreeWidget::showContextMenu(QTreeWidgetItem* item, const QPoint& globalPos) 
{
	// Clear menu's previous actions
	m_Menu.clear();

	int itemType = item->type();
	switch (itemType) 
	{
	case Favorite_Item_Type:
		if ( !m_FavoritesActions.isEmpty() )
			addActionList(m_FavoritesActions);
		break;

	case Prebuilt_Item_Type:
		if ( !m_PrebuiltActions.isEmpty() )
			addActionList(m_PrebuiltActions);
		break;
 
	case Default_Item_Type:
		if ( !m_DefaultActions.isEmpty() )
			addActionList(m_DefaultActions);
		break;

	case Favorite_Category_Item_Type:
		if ( !m_FavoriteCategoryActions.isEmpty() )
			addActionList(m_FavoriteCategoryActions);
		break;

	case Prebuilt_Category_Item_Type:
		if ( !m_PrebuiltCategoryActions.isEmpty() )
			addActionList(m_PrebuiltCategoryActions);
		break;
	}
 
	m_Menu.exec(globalPos);
}

void PipelineTreeWidget::setActionList(ItemType type, QList<QAction*> list)
{
	switch (type) 
	{
	case Favorite_Item_Type:
		m_FavoritesActions = list;
		break;

	case Prebuilt_Item_Type:
		m_PrebuiltActions = list;
		break;

	case Default_Item_Type:
		m_DefaultActions = list;
		break;

	case Favorite_Category_Item_Type:
		m_FavoriteCategoryActions = list;
		break;

	case Prebuilt_Category_Item_Type:
		m_PrebuiltCategoryActions = list;
		break;
	}
}