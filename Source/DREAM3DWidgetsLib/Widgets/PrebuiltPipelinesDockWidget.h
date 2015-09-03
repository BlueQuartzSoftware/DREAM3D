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

#ifndef _PrebuiltPipelinesDockWidget_H_
#define _PrebuiltPipelinesDockWidget_H_

#include <QtCore/QDir>
#include <QtCore/QSettings>

#include <QtWidgets/QDockWidget>
#include <QtWidgets/QMainWindow>

#include "QtSupportLib/FileDragMessageBox.h"
#include "QtSupportLib/DREAM3DSettings.h"

#include "SIMPLib/Common/Constants.h"

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLib.h"

#include "ui_PrebuiltPipelinesDockWidget.h"


class QListWidget;
class QTreeWidgetItem;
class FilterListDockWidget;
class FilterLibraryTreeWidget;
class QSettings;


/**
 * @brief The PrebuiltPipelinesDockWidget class
 */
class DREAM3DWidgetsLib_EXPORT  PrebuiltPipelinesDockWidget : public QDockWidget, private Ui::PrebuiltPipelinesDockWidget
{

    Q_OBJECT

  public:
    /**
     * @brief PrebuiltPipelinesDockWidget
     * @param parent
     */
    PrebuiltPipelinesDockWidget(QWidget* parent = NULL);
    virtual ~PrebuiltPipelinesDockWidget();

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

    /**
    * @brief Reads the preferences from the users pref file
    */
    void readSettings(QMainWindow* main, DREAM3DSettings& prefs);

    /**
    * @brief Writes the preferences to the users pref file
    */
    void writeSettings(DREAM3DSettings& prefs);

  protected:
    virtual QDir findPipelinesDirectory();
    virtual void readPipelines();
    void addPipelinesRecursively(QDir currentDir, QTreeWidgetItem* currentDirItem, QString iconFileName,
                                 bool allowEditing, QStringList filters, FilterLibraryTreeWidget::ItemType itemType);
    virtual QStringList generateFilterListFromPipelineFile(QString path);
    virtual QString generateHtmlFilterListFromPipelineFile(QString path);


  protected slots:
    void on_prebuiltsLibraryTree_itemClicked( QTreeWidgetItem* item, int column );
    void on_prebuiltsLibraryTree_itemDoubleClicked( QTreeWidgetItem* item, int column );
    void on_prebuiltsLibraryTree_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous );


  signals:

    /**
     * @brief The signal is emitted when a pipeline file needs to be loaded into the view at an index
     * @param filePath The absolute path to the pipeline file
     * @param setOpenedFilePath A boolean that decides whether to cache the opened file path to use when re-saving.
     * @param addToRecentFiles A boolean that decides whether to add filePath to the recents file list.
     * @param newWindow A boolean that decides whether to open a new window.
     */
    void pipelineFileActivated(const QString& filePath, const bool& setOpenedFilePath, const bool& addToRecentFiles);

    /**
     * @brief filterListGenerated
     * @param filterList
     */
    void filterListGenerated(const QStringList& filterList, bool sort);

  private:

    PrebuiltPipelinesDockWidget(const PrebuiltPipelinesDockWidget&); // Copy Constructor Not Implemented
    void operator=(const PrebuiltPipelinesDockWidget&); // Operator '=' Not Implemented


};

#endif
