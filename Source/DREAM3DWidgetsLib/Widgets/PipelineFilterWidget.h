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

#ifndef _PipelineFilterWidget_H_
#define _PipelineFilterWidget_H_

#include <QtCore/QStringList>
#include <QtCore/QSettings>
#include <QtCore/QUrl>

#include <QtWidgets/QFrame>
#include <QtWidgets/QMenu>


#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/FilterParameters/FilterParameter.h"

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLib.h"
#include "DREAM3DWidgetsLib/Widgets/FilterInputWidget.h"

#include "DREAM3DWidgetsLib/ui_PipelineFilterWidget.h"

// This needs to be defined
class QMouseEvent;
class AbstractFilter;
class QFormLayout;
class QLabel;
class QComboBox;
class IObserver;

/**
 * @class PipelineFilterWidget PipelineFilterWidget.h FilterWidgets/PipelineFilterWidget.h
 * @brief  This class is a subclass of the QGroupBox class and is used to display
 * Filter Options that the user can set. This class is capable of constructing a
 * default GUI widget set for each type of Filter Option that is available. If
 * the programmer needs more specialized widgets then they can simply subclass
 * this class and over ride or implement their custom code.
 *
 * @date Jan 6, 2012
 * @version 1.0
 */
class DREAM3DWidgetsLib_EXPORT PipelineFilterWidget : public QFrame, private Ui::PipelineFilterWidget
{
    Q_OBJECT
  public:
    PipelineFilterWidget(QWidget* parent = NULL);
    PipelineFilterWidget(AbstractFilter::Pointer filter, IObserver* observer = NULL, QWidget* parent = NULL);
    virtual ~PipelineFilterWidget();


    virtual AbstractFilter::Pointer getFilter();

    virtual bool isSelected();

    // These are convenience functions that just pass through to the filter instance
    virtual QString getHumanLabel();
    virtual QString getFilterGroup();
    virtual QString getFilterSubGroup();
    virtual QString getFilterClassName();
    virtual QString getCompiledLibraryName();

    virtual void setFilterTitle(const QString title);

    static QString getOpenDialogLastDirectory() { return m_OpenDialogLastDirectory; }
    static void setOpenDialogLastDirectory(QString val) { m_OpenDialogLastDirectory = val; }

    virtual void getGuiParametersFromFilter(AbstractFilter* filt);

    FilterInputWidget* getFilterInputWidget();

    QVector<QWidget*>& getFilterParameterWidgets();

    QWidget* getBasicInputsWidget();
    QWidget* getCurrentStructureWidget();


    /**
     * @brief Sets the actions that will be displayed as a context menu
     * @param list
     */
    void setContextMenuActions(QList<QAction*> list);

    SIMPL_INSTANCE_PROPERTY(QString, BorderColorStyle)

  public slots:


    virtual void setIsSelected(bool b);


    /**
     * @brief changeStyle
     */
    void changeStyle();

    /**
      *@brief
      */
    void updateWidgetStyle();

    /**
      * @brief Slot for a QTime to call to update the border of the widget in a
      * pulsing fashion
      */
    void setHasPreflightErrors(bool hasErrors);

    /**
     * @brief setHasPreflightWarnings
     * @param hasWarnings
     */
    void setHasPreflightWarnings(bool hasWarnings);

    /**
     * @brief displayFilterParameterWidgetError
     * @param msg
     */
    void displayFilterParameterWidgetError(const QString& msg);

    /**
     * @brief adjustLayout
     * @param w
     * @param state
     */
    void adjustLayout(QWidget* w, int state);

    /**
    * @brief showContextMenuForWidget
    * @param pos
    */
    void showContextMenuForWidget(const QPoint& pos);

    /**
    * @brief launchHelpForItem
    */
    void launchHelpForItem();

    /**
    * @brief toRunningState
    */
    void toRunningState();

    /**
    * @brief toIdleState
    */
    void toIdleState();

  protected slots:
    /**
     * @brief on_deleteBtn_clicked
     */
    void on_deleteBtn_clicked();

  signals:

    /**
     * @brief widgetSelected
     * @param w
     */
    void widgetSelected(PipelineFilterWidget* w);

    /**
     * @brief filterWidgetRemoved
     * @param widget
     */
    void filterWidgetRemoved(PipelineFilterWidget* widget);

    /**
     * @brief dragStarted
     * @param widget
     */
    void dragStarted(PipelineFilterWidget* widget);

    /**
     * @brief parametersChanged
     */
    void parametersChanged();

  protected:
    /**
     * @brief mousePressEvent
     * @param event
     */
    virtual void mousePressEvent( QMouseEvent* event );

    /**
     * @brief mouseReleaseEvent
     * @param event
     */
    virtual void mouseReleaseEvent( QMouseEvent* event );

    /**
     * @brief mouseMoveEvent
     * @param event
     */
    virtual void mouseMoveEvent( QMouseEvent* event );

    /**
     * @brief layoutWidgets
     */
    void layoutWidgets();

    /**
     * @brief validateFileSystemFilterParameter
     * @param option
     */
    void validateFileSystemFilterParameter(FilterParameter* parameter);

    /**
     * @brief linkConditionalWidgets
     * @param filterParameters
     */
    void linkConditionalWidgets(QVector<FilterParameter::Pointer>& filterParameters);

  private:
    QRect                     m_DeleteRect;
    QPoint                    dragStartPosition;
    bool                      m_IsSelected;
    bool                      m_HasPreflightErrors;
    bool                      m_HasPreflightWarnings;
    static QString            m_OpenDialogLastDirectory;
    AbstractFilter::Pointer   m_Filter;
    QVector<QWidget*>         m_FilterParameterWidgets;
    QVBoxLayout*              m_VariablesVerticalLayout;
    QVBoxLayout*              m_CurrStrucVerticalLayout;
    QWidget*                  m_VariablesWidget;
    QWidget*                  m_CurrentStructureWidget;
    IObserver*                m_Observer;
    QMap<QString, QWidget*>   m_PropertyToWidget;
    QMenu*                    m_ContextMenu;
    FilterInputWidget*        m_FilterInputWidget;


    /**
     * @brief initialize Calls all the necessary initialization code for the widget
     * @param filter
     */
    void initialize(AbstractFilter::Pointer filter);

    /**
    * @brief setupFilterInputWidget Creates and initializes the filter input widget.
    */
    void setupFilterInputWidget();

    PipelineFilterWidget(const PipelineFilterWidget&); // Copy Constructor Not Implemented
    void operator=(const PipelineFilterWidget&); // Operator '=' Not Implemented
};

#endif /* _PipelineFilterWidget_H_ */

