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
#include <QtCore/QSettings>
#include <QtGui/QDockWidget>
#include <QtGui/QTreeWidgetItem>


#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLib.h"

#include "ui_FavoritesDockWidget.h"

class QListWidget;
class QTreeWidgetItem;
class FilterListDockWidget;
class FilterLibraryTreeWidget;
class QSettings;

/**
 * @brief The FavoritesDockWidget class
 */
class DREAM3DWidgetsLib_EXPORT FavoritesDockWidget : public QDockWidget, private Ui::FavoritesDockWidget
{

    Q_OBJECT
  public:
    /**
     * @brief FavoritesDockWidget
     * @param parent
     */
    FavoritesDockWidget(QWidget* parent = NULL);
    virtual ~FavoritesDockWidget();

    /**
     * @brief setupGui
     */
    virtual void setupGui();

    /**
     * @brief connectFilterList
     * @param filterListWidget
     */
    void connectFilterList(FilterListDockWidget* filterListWidget);

    /**
     * @brief getFilterLibraryTreeWidget
     * @return
     */
    FilterLibraryTreeWidget* getFilterLibraryTreeWidget();

  public slots:
    //// Slots to catch signals from main menu or context menu
    void actionAddFavoriteFolder_triggered();
    void actionAddFavorite_triggered();
    void actionUpdateFavorite_triggered();
    void removeFavorite(QTreeWidgetItem* item);
    void actionRemoveFavorite_triggered();
    void actionRenameFavorite_triggered();
    void actionAppendFavorite_triggered();
    void actionShowInFileSystem_triggered();

  protected:

//   void addFavorite(QString path, QString favoriteTitle);
    virtual QDir findPipelinesDirectory();
    virtual void readPipelines();
    QStringList generateFilterListFromPipelineFile(QString path);
    void populateFilterList(QStringList filterNames);
    bool checkFavoriteTitle(QString favoritePath, QString newFavoriteTitle, QTreeWidgetItem* item);
    QString writeNewFavoriteFilePath(QString newFavoriteTitle, QString favoritePath, QTreeWidgetItem* item);
    bool hasIllegalFavoriteName(QString favoritePath, QString newFavoriteTitle, QTreeWidgetItem* item);
    bool hasDuplicateFavorites(QList<QTreeWidgetItem*> favoritesList, QString favoritePath, QString newFavoriteTitle, QTreeWidgetItem* item);

    void addPipelinesRecursively(QDir currentDir, QTreeWidgetItem* currentDirItem, QString iconFileName,
                                 bool allowEditing, QStringList filters, FilterLibraryTreeWidget::ItemType itemType);
    /**
     * @brief FavoritesDockWidget::addFavoriteTreeItem
     * @param selection
     * @param favoriteTitle
     * @param icon
     * @param itemType
     * @param favoritePath
     * @param allowEditing
     */
    void addFavoriteTreeItem(QTreeWidgetItem* selection,
                             QString& favoriteTitle,
                             QIcon icon,
                             FilterLibraryTreeWidget::ItemType itemType,
                             QString favoritePath,
                             bool allowEditing);

    /**
     * @brief addFavorite
     * @param folder Is the new favorite to be a folder or an actual pipeline file
     */
    void addFavorite(bool folder);

    /**
     * @brief displayNewFavoriteDialog
     * @return
     */
    QString displayNewFavoriteDialog();

  protected slots:
    //// Slots to catch signals from the QTreeWidget
    void on_filterLibraryTree_itemClicked( QTreeWidgetItem* item, int column );
    void on_filterLibraryTree_itemChanged( QTreeWidgetItem* item, int column );
    void on_filterLibraryTree_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous );
    void on_filterLibraryTree_itemDoubleClicked( QTreeWidgetItem* item, int column );



  signals:

    void fireWriteSettings();

    /**
     * @brief pipelineNeedsToBeSaved
     * @param path The absolute path to the pipeline file
     * @param name The name that the favorite will show up as in the GUI
     */
    void pipelineNeedsToBeSaved(const QString& path, const QString& name);

    /**
     * @brief The signal is emitted when the user double clicks on a pipeline file
     * @param filePath The absolute path to the pipeline file
     * @param append Should the pipeline be appended to the current pipeline
     */
    void pipelineFileActivated(const QString& filePath, QSettings::Format, bool append);

    /**
     * @brief filterListGenerated
     * @param filterList
     */
    void filterListGenerated(const QStringList& filterList, bool sort);

  private:

    FavoritesDockWidget(const FavoritesDockWidget&); // Copy Constructor Not Implemented
    void operator=(const FavoritesDockWidget&); // Operator '=' Not Implemented


};

#endif
