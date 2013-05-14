#ifndef _TREEWIDGETBUILDER_H_
#define _TREEWIDGETBUILDER_H_

#include <QApplication>


#include <QtGui/QMenu>
#include <QtGui/QMouseEvent>
#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>

class PipelineBuilderWidget;


class PipelineTreeWidget : public QTreeWidget
{
    Q_OBJECT;
 
public:
	enum ItemType
	{
		Default_Item_Type = 0,
		Favorite_Item_Type = 1,
		Prebuilt_Item_Type = 2
	};

    PipelineTreeWidget(QFrame* parent = 0);


	/**
     * @brief Adds the actions in the actionList parameter to the right-click menu
     */
	void addActionList(QList<QAction*> actionList);

	/**
     * @brief Setter for the different action lists
     */
	void setActionList(ItemType type, QList<QAction*> list);
 
private slots:
    void onCustomContextMenuRequested(const QPoint& pos);
    void showContextMenu(QTreeWidgetItem* item, const QPoint& globalPos);

private:
	QMenu								m_Menu;
	QList<QAction*>						m_FavoritesActions;
	QList<QAction*>						m_PrebuiltActions;
	QList<QAction*>						m_DefaultActions;
};

#endif /* _TREEWIDGETBUILDER_H_ */
