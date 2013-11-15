/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef PipelineBuilderWIDGET_H_
#define PipelineBuilderWIDGET_H_

#include <QtCore/QPoint>
#include <QtCore/QList>
#include <QtCore/QDir>
#include <QtGui/QMenu>

#include "ui_PipelineBuilderWidget.h"

#include "QtSupport/DREAM3DPluginFrame.h"
#include "PipelineBuilder/QFilterPipeline.h"
#include "PipelineBuilder/FilterWidgetManager.h"
#include "PipelineBuilder/PipelineBuilderDLLExport.h"

class QListWidgetItem;
class QTreeWidgetItem;
class QFilterWidget;
class QMenu;
class HelpDialog;
class FilterManager;

/**
 * @class PipelineBuilderWidget PipelineBuilderWidget.h PipelineBuilder/UI/PipelineBuilderWidget.h
 * @brief This class represents the User Interface for the Synthetic Grain Generator module
 * of the DREAM3D program. The user interface subclasses QFrame which
 * should make it able to be embedded in most Qt applications.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jan 30, 2011
 * @version 1.0
 */
class PipelineBuilderLib_EXPORT PipelineBuilderWidget : public DREAM3DPluginFrame, private Ui::PipelineBuilderWidget
{
    Q_OBJECT

  public:
    PipelineBuilderWidget(QMenu* pipelineMenu, FilterManager* fm, QWidget *parent = 0);
    virtual ~PipelineBuilderWidget();

    /**
      * @brief Reads the Preferences from the users pref file
      */
    virtual void readGeometrySettings(QSettings &prefs);

    /**
      * @brief Writes the preferences to the users pref file
      */
    virtual void writeGeometrySettings(QSettings &prefs);


    /**
     * @brief savePipeline
     * @param filePath
     */
   // virtual void savePipeline(const QString &filePath);

    /**
      * @brief Reads the Preferences from the users pref file
      */
    // virtual void readPipelineFile(QSettings &prefs, PipelineViewWidget* viewWidget, bool shouldClear);

    /**
     * @brief Writes the preferences to the users pref file
     */
    // virtual void writePipelineFile(QSettings &prefs, PipelineViewWidget* viewWidget);

    /**
     * @brief loadPipelineFileIntoPipelineView
     * @param path
     */
    // void loadPipelineFileIntoPipelineView(QString path);


    /**
     * @brief Enables or Disables all the widgets in a list
     * @param b
     */
    virtual void setWidgetListEnabled(bool b);

    /**
    * @brief Sets the menu bar instance variable
    */
    virtual void setPipelineMenu(QMenu* menuBar);

    /**
    * @brief Creates and sets the right click actions for the TreeWidgetBuilder items
    */
    void setupContextualMenus();

    /**
    * @brief returns the menu bar
    */
    virtual QMenu* getPipelineMenu();

    /**
     * @brief Initializes some of the GUI elements with selections or other GUI related items
     */
    virtual void setupGui();

    /**
     * @brief
     */
    virtual void checkIOFiles();

    /**
     * @brief
     * @param factories
     */
    virtual void updateFilterGroupList(FilterWidgetManager::Collection &factories);

    /**
     * @brief Reads favorite pipelines from the file system and loads them into the GUI
     */
    void readFavoritePipelines();

    /**
     * @brief Loads prebuilt pipelines into the GUI
     */
    void readPrebuiltPipelines();

    /**
     * @brief Getter for the filterLibraryTree private variable
     */
    PipelineTreeWidget* getPipelineTreeWidget() {return filterLibraryTree;}

    QStringList generateFilterListFromPipelineFile(QString path);
    void populateFilterList(QStringList filterNames);


    QString getLastDirectory() const { return m_OpenDialogLastDirectory; }
    void setLastDirectory(QString val) { m_OpenDialogLastDirectory = val; }

    /**
     * @brief Recursively adds all filters (.txt files) to the Prebuilt list
     */
    void addFiltersRecursively(QDir currentDir, QTreeWidgetItem* currentDirItem);

    /**
     * @brief Checks the QString "newFavoriteTitle" for illegal characters
   * and to see if the title chosen matches any favorite titles already in the list.
   * This function returns true if there are illegal characters or duplicates, and
   * returns false otherwise.
     */
    bool checkFavoriteTitle(QString favoritePath, QString newFavoriteTitle, QTreeWidgetItem* item);

    /**
     * @brief Helper function of the checkFavoriteTitle method that checks the new favorite
   * name specified by the user against the other existing favorite names and detects if
   * the new name already exists in the existing favorites list.
     */
    bool hasDuplicateFavorites(QList<QTreeWidgetItem*> favoritesList, QString favoritePath, QString newFavoriteTitle, QTreeWidgetItem* item);

    /**
     * @brief Helper function of the checkFavoriteTitle method that checks the new favorite
   * name specified by the user for illegal characters.
     */
    bool hasIllegalFavoriteName(QString favoritePath, QString newFavoriteTitle, QTreeWidgetItem* item);

    /**
     * @brief Creates the new favorite file path according to the new favorite name and writes
   * to that path.  This function returns the new path.
     */
    QString writeNewFavoriteFilePath(QString newFavoriteTitle, QString favoritePath, QTreeWidgetItem* item);

    /**
     * @brief Initializes the right-click menu for each filter in filterList
     */
    void initFilterListMenu();

    /**
     * @brief Creates and returns a QLabel hyperlink to the documentation of the filter that sent the PipelineMessage.
     * Returns NULL if either the filter class name or human label are empty.
     */
    QLabel* createHyperlinkLabel(PipelineMessage msg);

    /**
     * @brief getPipelineViewWidget
     * @return
     */
    PipelineViewWidget* getPipelineViewWidget();


  public slots:

    void acceptPipelineFileDrop(const QString &filePath);


    /**
     * @brief Reads the pipeline stored inside a dream3d file using the file's ID
     */
    void openPipelineFile(const QString& filePath, QSettings::Format format = QSettings::IniFormat);


  protected slots:
    void actionClearPipeline_triggered();
    void actionAddFavorite_triggered();
    void actionRemoveFavorite_triggered();
    void actionRenameFavorite_triggered();
    void actionAppendFavorite_triggered();
    void actionShowInFileSystem_triggered();
    void actionFilterListHelp_triggered();
    void actionUpdateFavorite_triggered();

    void on_m_GoBtn_clicked();
    void disableGoBtn();
    void enableGoBtn();

    void on_filterLibraryTree_itemClicked( QTreeWidgetItem* item, int column );
    void on_filterLibraryTree_itemChanged( QTreeWidgetItem* item, int column );
    void on_filterLibraryTree_currentItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous );
    void on_filterLibraryTree_itemDoubleClicked( QTreeWidgetItem* item, int column );

    void on_filterList_itemDoubleClicked( QListWidgetItem* item );
    void on_filterList_currentItemChanged( QListWidgetItem * item, QListWidgetItem * previous );

    void on_filterSearch_textChanged (const QString& text);

    void on_errorTableWidget_itemClicked(QTableWidgetItem* item);

    void on_toggleDocs_clicked();
    void on_showErrors_clicked();

    void onFilterListCustomContextMenuRequested(const QPoint& pos);

    void clearMessagesTable();

    void addDREAM3DReaderFilter(const QString &filePath);

    virtual void addMessage(PipelineMessage msg);
    virtual void addProgressMessage(QString message);

  private slots:
    // slots for our worker thread to communicate
    virtual void pipelineComplete();
    virtual void pipelineProgress(int value);

  protected:
    void removeFavorite(QTreeWidgetItem* item);
    void addFavorite(QString favoriteTitle);

  signals:
    void fireWriteSettings();
    void fireReadSettings();


  private:
    QList<QWidget*>             m_WidgetList;
    QFilterPipeline*            m_FilterPipeline;
    QMenu*                      m_MenuPipeline;
    QThread*                    m_WorkerThread;
    bool                        m_DocErrorTabsIsOpen;
    QString                     m_OpenDialogLastDirectory;

    QList<QAction*>             m_FavoriteCategoryActions;
    QList<QAction*>             m_FavoriteItemActions;
    QList<QAction*>             m_PrebuiltItemActions;

    QTreeWidgetItem*            m_favorites;
    QTreeWidgetItem*            m_prebuilts;
    bool                        m_hasErrors;
    bool                        m_hasWarnings;
    HelpDialog*                 m_HelpDialog;

    QMenu                       m_FilterMenu;
    QAction*                    m_actionFilterHelp;
    QPoint                      m_FilterListPosition;
    FilterPipeline::Pointer     m_PipelineFromFile;
    FilterManager*              m_FilterManager;

    PipelineBuilderWidget(const PipelineBuilderWidget&); // Copy Constructor Not Implemented
    void operator=(const PipelineBuilderWidget&); // Operator '=' Not Implemented
};

#endif /* PipelineBuilderWIDGET_H_ */
