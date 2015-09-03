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

#ifndef _FilterListDockWidget_H_
#define _FilterListDockWidget_H_

#include <QtWidgets/QWidget>
#include <QtWidgets/QMenu>

#include <QtCore/QSignalMapper>
#include <QtCore/QList>

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLib.h"
#include "SIMPLib/Common/FilterManager.h"

#include "QtSupportLib/DREAM3DSettings.h"

#include "ui_FilterListDockWidget.h"



class DREAM3DWidgetsLib_EXPORT FilterListDockWidget : public QDockWidget, private Ui::FilterListDockWidget
{

    Q_OBJECT

  public:
    FilterListDockWidget(QWidget* parent = NULL);
    virtual ~FilterListDockWidget();

    QList<QString> serializeString(QString string, char token);
    QString deserializeString(QList<QString> list, char token);

    void matchFilter(QMapIterator<QString, IFilterFactory::Pointer> iter, QString fullWord, int& filterCount);

    /**
    * @brief Reads the preferences from the users pref file
    */
    void readSettings(QMainWindow* main, DREAM3DSettings& prefs);

    /**
    * @brief Writes the preferences to the users pref file
    */
    void writeSettings(DREAM3DSettings& prefs);

    /**
    * @brief Returns true if a search is in progress, else returns false
    */
    bool searchInProgress();

    /**
    * @brief Returns the filter list widget
    */
    FilterListWidget* getFilterListWidget();

  public slots:

    /**
     * @brief on_filterList_itemDoubleClicked
     * @param item
     */
    void on_filterList_itemDoubleClicked( QListWidgetItem* item );

    /**
    * @brief searchFilters triggered when the user types something in the Search Field
    */
    void searchFilters(QString text);

    /**
     * @brief updateFilterList This method extracts all the names of the filters that have been
     * loaded into the application
     */
    void updateFilterList(bool sortItems = true);

    /**
    * @brief showContextMenuForWidget
    * @param pos
    */
    void showContextMenuForWidget(const QPoint& pos);

    /**
    * @brief launchHelpForItem
    * @param name
    */
    void launchHelpForItem(QString name);

    /**
    * @brief searchFieldsChanged
    */
    void searchFieldsChanged(bool isChecked);

    /**
    * @brief getActiveSearchAction
    */
    QAction* getActiveSearchAction();

    /**
    * @brief setActiveSearchAction
    */
    void setActiveSearchAction(QAction* action);

    /**
    * @brief getActiveSearchAction
    */
    QList<QAction*> getSearchActionList();

  signals:

    /**
     * @brief filterItemDoubleClicked
     * @param filterName
     */
    void filterItemDoubleClicked(const QString& filterName);


  protected:

    /**
    * @brief Detect a key press
    */
    void keyPressEvent(QKeyEvent* event);

    /**
     * @brief setupGui Called to get everything in the GUI setup correctly
     */
    void setupGui();

    /**
     * @brief setupSearchFieldActions
     */
    void setupSearchField();

    /**
     * @brief addItemToList
     * @param filter
     */
    void addItemToList(AbstractFilter::Pointer filter);

  private:
    QMenu* m_ContextMenu;
    QSignalMapper* m_Mapper;

    bool m_SearchAnyWords;
    bool m_SearchExactPhrase;
    bool m_SearchAllWords;

    QAction* m_ActionAnyWords;
    QAction* m_ActionExactPhrase;
    QAction* m_ActionAllWords;

    FilterManager::Collection  m_LoadedFilters;

    QMap<QString, AbstractFilter::Pointer> getHumanNameMap(QList<AbstractFilter::Pointer> list);

    FilterListDockWidget(const FilterListDockWidget&); // Copy Constructor Not Implemented
    void operator=(const FilterListDockWidget&); // Operator '=' Not Implemented
};


#endif /* _FilterListDockWidget_H_ */

