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
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _FilterListDockWidget_H_
#define _FilterListDockWidget_H_

#include <QtGui/QWidget>
#include <QtGui/QMenu>

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLib.h"
#include "DREAM3DLib/Common/FilterManager.h"

#include "ui_FilterListDockWidget.h"



class DREAM3DWidgetsLib_EXPORT FilterListDockWidget : public QDockWidget, private Ui::FilterListDockWidget
{

    Q_OBJECT

  public:
    FilterListDockWidget(QWidget* parent = NULL);
    virtual ~FilterListDockWidget();


  public slots:

    /**
     * @brief on_filterList_itemDoubleClicked
     * @param item
     */
    void on_filterList_itemDoubleClicked( QListWidgetItem* item );

    /**
     * @brief updateFilterList
     * @param list
     * @param sortItems
     */
   // void updateFilterList(const QStringList& list, bool sortItems);

    /**
     * @brief searchFieldsChanged
     */
    void searchFieldsChanged();


    /**
     * @brief updateFilterList This method extracts all the names of the filters that have been
     * loaded into the application
     */
    void updateFilterList(bool sortItems = true);

  signals:

    /**
     * @brief filterItemDoubleClicked
     * @param filterName
     */
    void filterItemDoubleClicked(const QString& filterName);


  protected:

    /**
     * @brief setupGui Called to get everything in the GUI setup correctly
     */
    void setupGui();

    /**
     * @brief setupSearchFieldActions
     */
    void setupSearchField();


    /**
     * @brief searchFilters triggered when the user types something in the Search Field
     */
    void searchFilters();

    /**
     * @brief updateSearchIcons Updates the icon being shown in the search field.
     */
    void updateSearchIcons();

    /**
     * @brief addItemToList
     * @param filter
     */
    void addItemToList(AbstractFilter::Pointer filter);

  private:

    bool m_SearchFilterClassName;
    bool m_SearchFilterHumanName;
    bool m_SearchParameterPropertyName;
    bool m_SearchParameterHumanName;
    bool m_SearchSearchGroupName;
    bool m_SearchSubGroupName;
    bool m_SearchPluginName;

    QAction* m_ActionSearchFilterClassName;
    QAction* m_ActionSearchFilterHumanName;
    QAction* m_ActionSearchParameterName;
    QAction* m_ActionSearchParameterPropertyName;
    QAction* m_ActionSearchGroupName;
    QAction* m_ActionSearchSubGroupName;
    QAction* m_ActionSearchPluginName;


    FilterManager::Collection  m_LoadedFilters;

    FilterListDockWidget(const FilterListDockWidget&); // Copy Constructor Not Implemented
    void operator=(const FilterListDockWidget&); // Operator '=' Not Implemented
};


#endif /* _FilterListDockWidget_H_ */

