#ifndef _TREEWIDGETBUILDER_H_
#define _TREEWIDGETBUILDER_H_

#include <QApplication>


#include <QtGui/QMenu>
#include <QtGui/QMouseEvent>
#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>

class PipelineBuilderWidget;


class TreeWidgetBuilder : public QTreeWidget
{
    Q_OBJECT;
 
public:
	enum ItemType
	{
		Default_Item_Type = 0,
		Favorite_Item_Type = 1
	};

    TreeWidgetBuilder(QFrame* parent = 0) :
		QTreeWidget(parent)
    {
        setContextMenuPolicy(Qt::CustomContextMenu);
 
        connect(this,
                SIGNAL(customContextMenuRequested(const QPoint&)),
                SLOT(onCustomContextMenuRequested(const QPoint&)));
    }

	void addActionList_Favorites(QList<QAction*> actionList);

	void setFavoritesActionList(QList<QAction*> list);
 
private slots:
    void onCustomContextMenuRequested(const QPoint& pos);
    void showContextMenu(QTreeWidgetItem* item, const QPoint& globalPos);

private:
	QMenu								m_Menu;
	QList<QAction*>						m_FavoritesActions;
};

#endif /* _TREEWIDGETBUILDER_H_ */
