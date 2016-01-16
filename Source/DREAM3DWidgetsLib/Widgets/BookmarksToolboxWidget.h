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
#ifndef _BookmarksToolboxWidget_H_
#define _BookmarksToolboxWidget_H_

#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QTreeWidgetItem>

#include "QtSupportLib/FileDragMessageBox.h"
#include "QtSupportLib/DREAM3DSettings.h"

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/Constants.h"

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLib.h"

#include "ui_BookmarksToolboxWidget.h"

class QListWidget;
class QTreeWidgetItem;
class FilterListToolboxWidget;
class FilterLibraryTreeWidget;
class QSettings;
class QAction;

/**
 * @brief The BookmarksToolboxWidget class
 */
class DREAM3DWidgetsLib_EXPORT BookmarksToolboxWidget : public QWidget, private Ui::BookmarksToolboxWidget
{

    Q_OBJECT
  public:
    /**
     * @brief BookmarksToolboxWidget
     * @param parent
     */
    BookmarksToolboxWidget(QWidget* parent = NULL);
    virtual ~BookmarksToolboxWidget();

    /**
     * @brief setupGui
     */
    virtual void setupGui();

    /**
     * @brief connectFilterList
     * @param filterListWidget
     */
    void connectFilterList(FilterListToolboxWidget* filterListWidget);

    /**
       @brief Delete a directory along with all of its contents.
       @param dirName Path of directory to remove.
       @return true on success; false on error.
    */
    static bool removeDir(const QString& dirName);

    /**
     * @brief getBookmarksTreeView
     * @return
     */
    BookmarksTreeView* getBookmarksTreeView();

    /**
    * @brief getBookmarksPrefsPath
    */
    QString getBookmarksPrefsPath();

    /**
    * @brief Reads the preferences from the users pref file
    */
    void readSettings(DREAM3DSettings* prefs);

    /**
    * @brief Writes the preferences to the users pref file
    */
    void writeSettings(DREAM3DSettings* prefs);

    virtual QDir findV4FavoritesDirectory();

    void readPrebuiltPipelines();

  public slots:

    /**
    * @brief BookmarksToolboxWidget::addBookmark
    * @param filePath
    * @param parent
    */
    void addBookmark(const QString& filePath, const QModelIndex& parent);

    /**
    * @brief BookmarksToolboxWidget::addFavoriteTreeItem
    * @param parent
    * @param favoriteTitle
    * @param icon
    * @param favoritePath
    * @param allowEditing
    */
    int addTreeItem(QModelIndex parent,
                    QString& favoriteTitle,
                    QIcon icon,
                    QString favoritePath,
                    int insertIndex,
                    bool allowEditing,
                    bool editState,
                    bool isExpanded);

  protected:
    QStringList generateFilterListFromPipelineFile(QString path);
    QString generateHtmlFilterListFromPipelineFile(QString path);

    void populateFilterList(QStringList filterNames);
    QString writeNewFavoriteFilePath(QString newFavoriteTitle, QString favoritePath, QTreeWidgetItem* item);

    QDir findPipelinesDirectory();
    void addPipelinesRecursively(QDir currentDir, QModelIndex parent, QString iconFileName,
      bool allowEditing, QStringList filters, FilterLibraryTreeWidget::ItemType itemType);

  protected slots:

    //// Slots to catch signals from the QTreeWidget
    void on_bookmarksTreeView_clicked(const QModelIndex& index);
    void on_bookmarksTreeView_doubleClicked(const QModelIndex& index);

  signals:

    void fireWriteSettings();

    /**
     * @brief pipelineNeedsToBeSaved
     * @param path The absolute path to the pipeline file
     * @param name The name that the favorite will show up as in the GUI
     */
    void pipelineNeedsToBeSaved(const QString& path, const QString& name);

    /**
     * @brief filterListGenerated
     * @param filterList
     */
    void filterListGenerated(const QStringList& filterList, bool sort);

    /**
    * @brief The signal is emitted when the user double clicks on a pipeline file
    * @param filePath The absolute path to the pipeline file
    * @param setOpenedFilePath A boolean that decides whether to cache the opened file path to use when re-saving.
    * @param addToRecentFiles A boolean that decides whether to add filePath to the recents file list.
    * @param newWindow A boolean that decides whether to open a new window.
    */
    void pipelineFileActivated(const QString& filePath, const bool& setOpenedFilePath, const bool& addToRecentFiles);

    void updateStatusBar(const QString& msg);

  private:
    QString                 m_OpenDialogLastDirectory;

    /**
    * @brief serializeTreePath
    * @param treePath
    */
    QList<QString> deserializeTreePath(QString treePath);

    /**
    * @brief writeSettings
    */
    void writeSettings();

    BookmarksToolboxWidget(const BookmarksToolboxWidget&); // Copy Constructor Not Implemented
    void operator=(const BookmarksToolboxWidget&); // Operator '=' Not Implemented
};

#endif
