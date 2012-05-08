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

#include <QtCore/QString>
#include <QtCore/QSettings>
#include <QtCore/QEvent>
#include <QtGui/QDropEvent>


#include "ui_PipelineBuilderWidget.h"

#include "QtSupport/DREAM3DPluginFrame.h"
#include "PipelineBuilder/QFilterPipeline.h"

class QListWidgetItem;
class QTreeWidgetItem;
class QFilterWidget;

/**
 * @class PipelineBuilderWidget PipelineBuilderWidget.h PipelineBuilder/UI/PipelineBuilderWidget.h
 * @brief This class represents the User Interface for the Synthetic Grain Generator module
 * of the DREAM3D program. The user interface subclasses QFrame which
 * should make it able to be embedded in most Qt applications.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jan 30, 2011
 * @version 1.0
 */
class PipelineBuilderWidget : public DREAM3DPluginFrame, private Ui::PipelineBuilderWidget
{
  Q_OBJECT

  public:
    PipelineBuilderWidget(QWidget *parent = 0);
    virtual ~PipelineBuilderWidget();

    /**
      * @brief Reads the Preferences from the users pref file
      */
    virtual void readSettings(QSettings &prefs);

     /**
      * @brief Writes the preferences to the users pref file
      */
    virtual void writeSettings(QSettings &prefs);

    /**
     * @brief Enables or Disables all the widgets in a list
     * @param b
     */
    virtual void setWidgetListEnabled(bool b);

    /**
     * @brief Initializes some of the GUI elements with selections or other GUI related items
     */
    virtual void setupGui();


    virtual void checkIOFiles();

  protected slots:
    void on_m_GoBtn_clicked();
    void on_m_SaveSettingsBtn_clicked();
    void on_m_LoadSettingsBtn_clicked();

    void on_filterLibraryTree_itemClicked( QTreeWidgetItem* item, int column );

    void on_filterList_itemDoubleClicked( QListWidgetItem* item );
    void on_filterList_currentItemChanged( QListWidgetItem * item, QListWidgetItem * previous );


    void on_toggleDocs_clicked();
    void on_showErrors_clicked();

#if 0
    void on_filterUp_clicked();
    void on_filterDown_clicked();
#endif

  private slots:
    // slots for our worker thread to communicate
    virtual void addErrorMessage(QString message);
    virtual void addWarningMessage(QString message);
    virtual void addProgressMessage(QString message);

    /* Surface Mesh Thread communicates throught these methods */
    virtual void pipelineComplete();
    virtual void pipelineProgress(int value);


  private:
    QList<QWidget*>           m_WidgetList;
    QFilterPipeline*          m_FilterPipeline;
    QThread*                  m_WorkerThread;
    bool                      m_HelpIsClosed;
    bool                      m_ErrorsIsClosed;
    QString                   m_OpenDialogLastDirectory;

    PipelineBuilderWidget(const PipelineBuilderWidget&); // Copy Constructor Not Implemented
    void operator=(const PipelineBuilderWidget&); // Operator '=' Not Implemented
};

#endif /* PipelineBuilderWIDGET_H_ */
