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


#ifndef _BookmarksTreeView_H_
#define _BookmarksTreeView_H_

#include <QApplication>

#include <QtCore/QJsonObject>

#include <QtWidgets/QMenu>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItem>

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLib.h"

#include "DREAM3DWidgetsLib/Widgets/BookmarksModel.h"


class PipelineBuilderWidget;
class QAction;
class QTreeWidgetItem;

class DREAM3DWidgetsLib_EXPORT BookmarksTreeView : public QTreeView
{
    Q_OBJECT

  public:
    enum ItemType
    {
      Node_Item_Type = 1,
      Leaf_Item_Type = 2,
      Unknown_Item_Type = 3
    };

    /**
    * @brief BookmarksTreeView
    * @param parent
    */
    BookmarksTreeView(QWidget* parent = 0);

    /**
    * @brief ~BookmarksTreeView()
    */
    ~BookmarksTreeView();

    /**
    * @brief fromJsonObject
    * @param modelObject
    */
    static BookmarksModel* FromJsonObject(QJsonObject modelObject);

    /**
    * @brief toJsonObject
    */
    QJsonObject toJsonObject();

    /**
    * @brief setModel
    * @param model
    */
    void setModel(QAbstractItemModel* model) Q_DECL_OVERRIDE;

  public slots:
    void collapseIndex(const QModelIndex& index);
    void expandIndex(const QModelIndex& index);

  protected:
    void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent* event) Q_DECL_OVERRIDE;
    void dragLeaveEvent(QDragLeaveEvent* event) Q_DECL_OVERRIDE;
    void dragMoveEvent(QDragMoveEvent* event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent* event) Q_DECL_OVERRIDE;

    void currentChanged(const QModelIndex& current, const QModelIndex& previous) Q_DECL_OVERRIDE;

    /**
    * @brief Adds the actions in the actionList parameter to the right-click menu
    */
    void addActionList(QList<QAction*> actionList);

  signals:
    void itemWasDropped(QModelIndex parent, QString& title, QIcon icon, QString path, bool allowEditing, bool editState, bool isExpanding);
    void currentIndexChanged(const QModelIndex& current, const QModelIndex& previous);
    void folderChangedState(const QModelIndex& index, bool expand);

  private slots:

    /**
    * @brief mousePressEvent
    * @param event
    */
    void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

  private:
    void performDrag();
    void expandChildren(const QModelIndex& parent, BookmarksModel* model);
    QJsonObject wrapModel(QModelIndex index);
    static void UnwrapModel(QString objectName, QJsonObject object, BookmarksModel* model, QModelIndex parentIndex);

    QPoint                    m_StartPos;
    QMenu                     m_Menu;
    QList<QAction*>           m_NodeActions;
    QList<QAction*>           m_LeafActions;
    QList<QAction*>           m_LeafErrorActions;
    QList<QAction*>           m_DefaultActions;
    QModelIndex               m_IndexBeingDragged;
    QModelIndex               m_TopLevelItemPlaceholder;
};

#endif /* _BookmarksTreeView_H_ */
