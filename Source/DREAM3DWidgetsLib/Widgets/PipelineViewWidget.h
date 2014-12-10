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

#ifndef _PipelineViewWidget_H
#define _PipelineViewWidget_H

#include <vector>

#include <QtCore/QTimer>
#include <QtGui/QFrame>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QTextEdit>
#include <QtGui/QTableWidget>
#include <QtGui/QStatusBar>


#include "DREAM3DLib/Common/PipelineMessage.h"
#include "DREAM3DLib/Common/FilterPipeline.h"
#include "DREAM3DLib/FilterParameters/H5FilterParametersReader.h"
#include "DREAM3DLib/CoreFilters/DataContainerReader.h"

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLib.h"
#include "DREAM3DWidgetsLib/Widgets/PipelineFilterWidget.h"

#include "QtSupport/DREAM3DFileDragMessageBox.h"


class QScrollArea;
class QContextMenuEvent;
class QLabel;
class QEvent;

/*
 *
 */
class DREAM3DWidgetsLib_EXPORT PipelineViewWidget : public QFrame
{
    Q_OBJECT

  public:
    PipelineViewWidget(QWidget* parent = 0);
    virtual ~PipelineViewWidget();

    /**
     * @brief filterCount
     * @return
     */
    int filterCount();

    /**
     * @brief filterWidgetAt
     * @param index
     * @return
     */
    PipelineFilterWidget* filterWidgetAt(int index);

    /**
     * @brief clearWidgets
     */
    void clearWidgets();

    /**
     * @brief getFilterPipeline
     * @return
     */
    FilterPipeline::Pointer getFilterPipeline();

    /**
     * @brief Returns a FilterPipeline Object with a new filter instance that has the input parameters copied
     * from the filter instance that is embedded in the PipelineFilterWidget instance. This function does NOT perform
     * a DEEP copy of the filter.
     * @return
     */
    FilterPipeline::Pointer getCopyOfFilterPipeline();

    /**
     * @brief setScrollArea
     * @param sa
     */
    void setScrollArea(QScrollArea* sa);
    /**
     * @brief setAutoScroll
     * @param scroll
     */
    void setAutoScroll(bool scroll) {m_AutoScroll = scroll;}
    /**
     * @brief hasAutoScroll
     * @return
     */
    bool hasAutoScroll() {return m_AutoScroll;}

    /**
     * @brief setAutoScrollMargin
     * @param margin
     */
    void setAutoScrollMargin(int margin) {m_AutoScrollMargin = margin;}

    /**
     * @brief getAutoScrollMargin
     * @return
     */
    int getAutoScrollMargin() {return m_AutoScrollMargin;}

    /**
     * @brief stopAutoScroll Stops the time so the Auto Scrolling will cease.
     */
    void stopAutoScroll();

    /**
     * @brief startAutoScroll Starts the QTimer in charge of auto scrolling the view
     */
    void startAutoScroll();

    /**
     * @brief shouldAutoScroll Figures out if the mouse position is in the margin area that would trigger an
     * autoscroll
     * @param pos
     * @return
     */
    bool shouldAutoScroll(const QPoint& pos);

    /**
     * @brief setInputParametersWidget
     * @param w
     */
    void setInputParametersWidget(QWidget* w);

    /**
     * @brief newEmptyPipelineViewLayout
     */
    void newEmptyPipelineViewLayout();

    /**
     * @brief resetLayout
     */
    void resetLayout();

    /**
     * @brief setPipelineMessageObserver
     * @param pipelineMessageObserver
     */
    void setPipelineMessageObserver(QObject* pipelineMessageObserver);

    /**
     * @brief Sets the actions that will be displayed as a context menu
     * @param list
     */
    void setContextMenuActions(QList<QAction*> list);

    /**
     * @brief populatePipelineView
     * @param pipeline
     */
    void populatePipelineView(FilterPipeline::Pointer pipeline, ExtractionType type);

    bool eventFilter(QObject *, QEvent *);

  public slots:

    /**
     * @brief addFilter
     * @param filterClassName
     * @param index
     */
    void addFilter(const QString& filterClassName, int index = -1);

    /**
     * @brief addFilterWidget
     * @param w
     * @param filter
     * @param index
     */
    void addFilterWidget(PipelineFilterWidget* w, int index = -1);

    /**
     * @brief removeFilterWidget
     * @param whoSent
     */
    void removeFilterWidget(PipelineFilterWidget* whoSent);

    /**
     * @brief setSelectedFilterWidget
     * @param w
     */
    void setSelectedFilterWidget(PipelineFilterWidget* w);

    /**
     * @brief setFilterBeingDragged
     * @param w
     */
    void setFilterBeingDragged(PipelineFilterWidget* w);

    /**
     * @brief setFilterBeingDragged
     * @param w
     */
    void setStatusBar(QStatusBar* statusBar);

    /**
     * @brief preflightPipeline
     */
    void preflightPipeline();

    /**
     * @brief doAutoScroll This does the actual scrolling of the Widget
     */
    void doAutoScroll();

    /**
     * @brief loadPipelineFile
     * @param filePath
     * @param format
     * @param append
     */
    void loadPipelineFile(const QString& filePath, QSettings::Format format = QSettings::IniFormat, bool append = false);

    /**
     * @brief savePipeline Saves the pipeline to the file. If the file exists this function will <b>DELETE</b> the pipeline
     * before saving this pipeline. MAKE SURE YOU UNDERSTAND THIS before using this function
     * @param filePath The absolute path to the pipeline file
     * @param name The name that will be used for display purposes
     */
    void savePipeline(const QString& filePath, const QString& name, QSettings::Format = QSettings::IniFormat);

    /**
     * @brief onCustomContextMenuRequested
     * @param pos
     */
    void on_customContextMenuRequested(const QPoint& pos);

    /**
     * @brief reindexWidgetTitles
     */
    void reindexWidgetTitles();

    /**
     * @brief extractPipelineFromFile
     * @param filePath
     */
    void extractPipelineFromFile(const QString &filePath, ExtractionType type);

    /**
     * @brief addDREAM3DReaderFilter
     * @param filePath
     */
    void addDREAM3DReaderFilter(const QString &filePath, ExtractionType type);

    /**
     * @brief readPipelineFromFile
     * @param fileId
     * @return
     */
    int readPipelineFromFile(hid_t fileId);


  signals:
    void addPlaceHolderFilter(QPoint p);
    void removePlaceHolderFilter();
    void preflightHasMessage(PipelineMessage msg);

    void pipelineFileDropped(QString& file);
    void pipelineHasErrorsSignal();
    void pipelineHasNoErrors();
    void pipelineIssuesCleared();
    void pipelineTitleUpdated(QString name);
    void pipelineChanged();

  protected:
    void setupGui();
    void dragEnterEvent(QDragEnterEvent* event);
    //  void dragLeaveEvent(QDragLeaveEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void dropEvent(QDropEvent* event);

    void showContextMenu(const QPoint& globalPos);

  private:
    PipelineFilterWidget*     m_SelectedFilterWidget;
    QVBoxLayout*              m_FilterWidgetLayout;
    PipelineFilterWidget*     m_FilterBeingDragged;
    int                       m_DropIndex;
    QLabel*                   m_EmptyPipelineLabel;
    QPoint                    m_LastDragPoint;
    QScrollArea*              m_ScrollArea;
    QTimer                    m_autoScrollTimer;
    bool                      m_AutoScroll;
    int                       m_AutoScrollMargin;
    int                       m_autoScrollCount;
    QWidget*                  m_InputParametersWidget;
    QObject*                  m_PipelineMessageObserver;
    QMenu                     m_Menu;
    QStatusBar*               m_StatusBar;
    QList<QAction*>           m_MenuActions;

    PipelineViewWidget(const PipelineViewWidget&); // Copy Constructor Not Implemented
    void operator=(const PipelineViewWidget&); // Operator '=' Not Implemented
};

#endif /* _PipelineViewWidget_H */

