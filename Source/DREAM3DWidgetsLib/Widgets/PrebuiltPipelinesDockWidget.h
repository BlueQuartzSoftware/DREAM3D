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
#ifndef _PrebuiltPipelinesDockWidget_H_
#define _PrebuiltPipelinesDockWidget_H_

#include <QtCore/QDir>
#include <QtCore/QSettings>

#include <QtGui/QDockWidget>

#include "DREAM3DLib/Common/Constants.h"

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


  public slots:
    void actionAppendPipeline_triggered();
    void actionShowInFileSystem_triggered();


  protected:
    virtual QDir findPipelinesDirectory();
    virtual void readPipelines();
    void addPipelinesRecursively(QDir currentDir, QTreeWidgetItem* currentDirItem, QString iconFileName,
                                 bool allowEditing, QStringList filters, FilterLibraryTreeWidget::ItemType itemType);
    virtual QStringList generateFilterListFromPipelineFile(QString path);


  protected slots:
    void on_filterLibraryTree_itemClicked( QTreeWidgetItem* item, int column );
    void on_filterLibraryTree_itemDoubleClicked( QTreeWidgetItem* item, int column );

  signals:

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

    PrebuiltPipelinesDockWidget(const PrebuiltPipelinesDockWidget&); // Copy Constructor Not Implemented
    void operator=(const PrebuiltPipelinesDockWidget&); // Operator '=' Not Implemented


};

#endif
