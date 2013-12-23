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
#ifndef _QDREAM3D_DOCUMENTS_DOCKWIDGET_H_
#define _QDREAM3D_DOCUMENTS_DOCKWIDGET_H_

#include <QtCore/QFileInfo>
#include <QtGui/QDockWidget>
#include <QtGui/QTreeWidgetItem>


#include "DREAM3DLib/Common/FilterManager.h"


#include "ui_QDream3DDocumentsDockWidget.h"

class QListWidget;
class QListWidgetItem;
class QTreeWidgetItem;



class QDream3DDocumentsDockWidget : public QDockWidget, private Ui::QDream3DDocumentsDockWidget
{

    enum ItemType
    {
      Default_Item_Type = 0,
      Favorite_Item_Type = 1,
      Favorite_Category_Item_Type = 3
    };

    Q_OBJECT
  public:
    QDream3DDocumentsDockWidget(QWidget* parent = NULL);
    virtual ~QDream3DDocumentsDockWidget();

    virtual void setupGui();


  protected:

    void addFavorite(QString favoriteTitle);
    void readFavoritePipelines(QTreeWidgetItem *m_favorites);
    QStringList generateFilterListFromPipelineFile(QString path);
    void populateFilterList(QStringList filterNames);
    bool checkFavoriteTitle(QString favoritePath, QString newFavoriteTitle, QTreeWidgetItem* item);
    QString writeNewFavoriteFilePath(QString newFavoriteTitle, QString favoritePath, QTreeWidgetItem* item);
    bool hasIllegalFavoriteName(QString favoritePath, QString newFavoriteTitle, QTreeWidgetItem* item);
    bool hasDuplicateFavorites(QList<QTreeWidgetItem*> favoritesList, QString favoritePath, QString newFavoriteTitle, QTreeWidgetItem* item);

  protected slots:
    //// Slots to catch signals from the QTreeWidget
    void on_filterLibraryTree_itemClicked( QTreeWidgetItem* item, int column );
    void on_filterLibraryTree_itemChanged( QTreeWidgetItem* item, int column );
    void on_filterLibraryTree_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous );
    void on_filterLibraryTree_itemDoubleClicked( QTreeWidgetItem* item, int column );

    //// Slots to catch signals from main menu or context menu
    void actionAddFavorite_triggered();
    void actionUpdateFavorite_triggered();
    void removeFavorite(QTreeWidgetItem* item);
    void actionRemoveFavorite_triggered();
    void actionRenameFavorite_triggered();
    void actionAppendFavorite_triggered();
    void actionShowInFileSystem_triggered();


  signals:

    void fireWriteSettings();
    void pipelineFileActivated(const QString &filePath);

  private:

    QDream3DDocumentsDockWidget(const QDream3DDocumentsDockWidget&); // Copy Constructor Not Implemented
    void operator=(const QDream3DDocumentsDockWidget&); // Operator '=' Not Implemented


};

#endif
