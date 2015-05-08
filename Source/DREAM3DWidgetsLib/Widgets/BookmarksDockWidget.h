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
#include <QtCore/QString>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QTreeWidgetItem>

#include "QtSupportLib/FileDragMessageBox.h"

#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLib.h"

#include "ui_BookmarksDockWidget.h"

class QListWidget;
class QTreeWidgetItem;
class FilterListDockWidget;
class FilterLibraryTreeWidget;
class QSettings;
class QAction;

/**
 * @brief The BookmarksDockWidget class
 */
class DREAM3DWidgetsLib_EXPORT BookmarksDockWidget : public QDockWidget, private Ui::BookmarksDockWidget
{

    Q_OBJECT
  public:
    /**
     * @brief BookmarksDockWidget
     * @param parent
     */
    BookmarksDockWidget(QWidget* parent = NULL);
    virtual ~BookmarksDockWidget();

    DREAM3D_INSTANCE_PROPERTY(QAction*, RenameAction)
    DREAM3D_INSTANCE_PROPERTY(QAction*, DeleteAction)

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
       @brief Delete a directory along with all of its contents.
       @param dirName Path of directory to remove.
       @return true on success; false on error.
    */
    static bool removeDir(const QString& dirName);

    /**
     * @brief getFilterLibraryTreeWidget
     * @return
     */
    FilterLibraryTreeWidget* getFilterLibraryTreeWidget();

    QTreeWidgetItem* getSelectedParentTreeItem();

    void configureFilterLibraryTree();

    /**
    * @brief Reads the preferences from the users pref file
    */
    void readSettings(QMainWindow* main, QSettings& prefs);

    /**
    * @brief Writes the preferences to the users pref file
    */
    void writeSettings(QSettings& prefs);

  public slots:
    //// Slots to catch signals from main menu or context menu
    void m_ActionNewFolder_triggered();
    void m_ActionAddPipeline_triggered();
    void m_ActionUpdatePipeline_triggered();
    void removeBookmark(QTreeWidgetItem* item);
    void m_ActionRemovePipeline_triggered();
    void m_ActionRenamePipeline_triggered();
    void m_ActionAddToPipelineView_triggered();
    void m_ActionShowInFileSystem_triggered();
    void removeBookmark(QString bookmarkName, QString bookmarkPath, QString bookmarkTreePath);
    void renameBookmark(QString oldName, QString newName, QString filePath, QString treePath);

    /**
    * @brief Adds pipelines to the Bookmarks Dock Widget
    */
    void addPipelines(QList<QString> newPaths);

    /**
    * @brief Adds a folder to the Bookmarks Dock Widget
    * @param name
    * @param parentName
    * @param parentTreePath
    * @param editState
    */
    void addFolder(QString name, QString parentName, QString parentTreePath, bool editState);

    /**
    * @brief Updates the widget with the new information in the settings file
    */
    void updateWidget();


  protected:

    virtual QDir findPipelinesDirectory();
    virtual void readPipelines();
    QStringList generateFilterListFromPipelineFile(QString path);
    QString generateHtmlFilterListFromPipelineFile(QString path);

    void populateFilterList(QStringList filterNames);
    bool checkFavoriteTitle(QString favoritePath, QString newFavoriteTitle, QTreeWidgetItem* item);
    QString writeNewFavoriteFilePath(QString newFavoriteTitle, QString favoritePath, QTreeWidgetItem* item);
    bool hasIllegalFavoriteName(QString favoritePath, QString newFavoriteTitle, QTreeWidgetItem* item);
    bool hasDuplicateFavorites(QList<QTreeWidgetItem*> favoritesList, QString favoritePath, QString newFavoriteTitle, QTreeWidgetItem* item);

    void addPipelinesRecursively(QDir currentDir, QTreeWidgetItem* currentDirItem, QString iconFileName,
                                 bool allowEditing, QStringList filters, FilterLibraryTreeWidget::ItemType itemType);
    /**
     * @brief BookmarksDockWidget::addFavoriteTreeItem
     * @param selection
     * @param favoriteTitle
     * @param icon
     * @param itemType
     * @param favoritePath
     * @param allowEditing
     */
    int addTreeItem(QTreeWidgetItem* selection,
                             QString& favoriteTitle,
                             QIcon icon,
                             FilterLibraryTreeWidget::ItemType itemType,
                             QString favoritePath,
                             bool allowEditing,
                             bool editState,
                             bool isExpanded);

    void readTreeSettings(QSettings& prefs);

  protected slots:
    //// Slots to catch signals from the QTreeWidget
    void on_filterLibraryTree_itemClicked( QTreeWidgetItem* item, int column );
    void on_filterLibraryTree_itemDoubleClicked( QTreeWidgetItem* item, int column );

    void on_filterLibraryTree_itemChanged( QTreeWidgetItem* item, int column );
    void on_filterLibraryTree_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous );

    void readSettings();

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
     * @param append Should the pipeline be appended, replaced, or prepended
     */
    void pipelineFileActivated(const QString& filePath, ExtractionType type);

    /**
     * @brief filterListGenerated
     * @param filterList
     */
    void filterListGenerated(const QStringList& filterList, bool sort);

    void updateStatusBar(const QString &msg);

    void settingsUpdated();

  private:
    QString                 m_OpenDialogLastDirectory;

    /**
    * @brief getTreePathFromItem
    * @param item
    */
    QString getTreePathFromItem(QTreeWidgetItem* item);

    /**
    * @brief getItemFromTreePath
    * @param treePath
    */
    QTreeWidgetItem* getItemFromTreePath(QString treePath);

    /**
    * @brief serializeTreePath
    * @param treePath
    */
    QList<QString> deserializeTreePath(QString treePath);

    /**
    * @brief writeSettings
    */
    void writeSettings();

    BookmarksDockWidget(const BookmarksDockWidget&); // Copy Constructor Not Implemented
    void operator=(const BookmarksDockWidget&); // Operator '=' Not Implemented


};

#endif
