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

#ifndef _IssuesDockWidget_H_
#define _IssuesDockWidget_H_


#include <QtWidgets/QDockWidget>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/IObserver.h"
#include "SIMPLib/Common/PipelineMessage.h"

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLib.h"

#include "ui_IssuesDockWidget.h"

class FilterListToolboxWidget;
class QLabel;
class QTableWidgetItem;
class DREAM3DSettings;

class DREAM3DWidgetsLib_EXPORT IssuesDockWidget : public QDockWidget, public IObserver, private Ui::IssuesDockWidget
{

    Q_OBJECT
  public:
    IssuesDockWidget(QWidget* parent = NULL);
    virtual ~IssuesDockWidget();

    QLabel* createHyperlinkLabel(PipelineMessage msg);

    /**
    * @brief Reads the preferences from the users pref file
    */
    void readSettings(QMainWindow* main, DREAM3DSettings* prefs);

    /**
    * @brief Writes the preferences to the users pref file
    */
    void writeSettings(DREAM3DSettings* prefs);

  public slots:
    void processPipelineMessage(const PipelineMessage& msg);
    void clearIssues();
    void on_errorTableWidget_itemClicked( QTableWidgetItem* item );
    void displayCachedMessages();

    void showFilterHelp(const QString &url);

  protected:
    void setupGui();


  private:
    QVector<PipelineMessage> m_CachedMessages;


    IssuesDockWidget(const IssuesDockWidget&); // Copy Constructor Not Implemented
    void operator=(const IssuesDockWidget&); // Operator '=' Not Implemented


};

#endif
