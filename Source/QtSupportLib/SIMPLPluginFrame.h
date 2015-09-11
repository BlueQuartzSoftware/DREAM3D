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


#ifndef _AIM_PLUGIN_FRAME_H_
#define _AIM_PLUGIN_FRAME_H_

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtWidgets/QFrame>
#include <QtCore/QSettings>
#include <QtWidgets/QStatusBar>
#include <QtGui/QIcon>
#include <QtWidgets/QMessageBox>

#include "SIMPLib/Common/PipelineMessage.h"

#include "QtSupportLib/QtSupportLib.h"

class QLineEdit;


/**
 * @class SIMPLPluginFrame SIMPLPluginFrame.h QtSupportLib/SIMPLPluginFrame.h
 * @brief This class is a super class for any High Level User Interface element
 * that would get loaded from a plugin type architecture. The class implements
 * some common methods that are needed by all subclasses. Subclasses should implement
 * the pure virtual methods and so this class can not be directly instantiated.
 *
 * @date Jan 30, 2011
 * @version 1.0
 */
class QtSupportLib_EXPORT SIMPLPluginFrame : public QFrame
{
    Q_OBJECT

  public:
    SIMPLPluginFrame(QWidget* parent = NULL);
    virtual ~SIMPLPluginFrame();


    /**
      * @brief Reads the Preferences from the users pref file
      */
    virtual void readSettings(QSettings& prefs, bool shouldClear);

    /**
     * @brief Writes the preferences to the users pref file
     */
    virtual void writeSettings(QSettings& prefs);

    /**
     * @brief Enables or Disables all the widgets in a list
     * @param b Enable or Disable the widgets.
     */
    virtual void setWidgetListEnabled(bool b) = 0;

    /**
     * @brief Verifies that a path exists on the file system.
     * @param outFilePath The file path to check
     * @param lineEdit The QLineEdit object to modify visuals of (Usually by placing a red line around the QLineEdit widget)
     */
    bool verifyPathExists(QString outFilePath, QLineEdit* lineEdit);

    /**
     * @brief Verifies that a parent path exists on the file system.
     * @param outFilePath The parent file path to check
     * @param lineEdit The QLineEdit object to modify visuals of (Usually by placing a red line around the QLineEdit widget)
     */
    bool verifyOutputPathParentExists(QString outFilePath, QLineEdit* lineEdit);

    /**
     * @brief Initializes some of the GUI elements with selections or other GUI related items
     */
    virtual void setupGui() = 0;

    /**
     * @brief Checks for the existance of output files
     */
    virtual void checkIOFiles() = 0;

    /**
     * @brief Sets the status bar object for the frame
     * @param statusBar
     */
    virtual void setStatusBar(QStatusBar* statusBar);

    /**
     * @brief Returns a pointer to the QStatusBar being used
     * @return
     */
    virtual QStatusBar* statusBar();

    /**
     * @brief Runs checks on the output files to let the user know which output
     * files already exist in the output directory location potentially warning
     * them of an over writing situation
     * @param le The QLineEdit where the output directory is defined
     * @param msgTitle A message to display.
     * @return
     */
    virtual bool sanityCheckOutputDirectory(QLineEdit* le, QString msgTitle);

    /**
     * @brief Displays a Type Dialog box used for errors that occur
     * during the execution of the program.
     * @param title
     * @param text
     */
    virtual void displayDialogBox(QString title, QString text, QMessageBox::Icon icon);

  signals:

    /**
     * @brief Signal emitted when a process is started
     */
    void pipelineStarted();

    /**
     * @brief Signal Emitted when a process has ended.
     */
    void pipelineEnded();

    /**
     * @brief Signal Emitted when the process has been canceled.
     */
    void cancelPipeline();

    /**
     * @brief Signal emitted when a message is available for display to the user
     * @param
     */
    void pipelineWarningMessage(const QString&);
    void pipelineErrorMessage(const QString&);

  private slots:
    /**
     * @brief Slot to add a message to display to the user
     * @param message
     */
    virtual void addMessage(PipelineMessage msg) = 0;

    /**
     * @brief Slot to add a Progress message to display to the user
     * @param message
     */
    virtual void addProgressMessage(QString message) = 0;

    /**
     * @brief Slot to catch signals when the processing thread is finished.
     */
    virtual void pipelineComplete() = 0;

    /**
     * @brief Slot to catch signals when a processing thread has made progress
     * @param value The value of the progress, Typically between 0 and 100.
     */
    virtual void pipelineProgress(int value) = 0;

  private:
    QStatusBar*    m_StatusBar;

    SIMPLPluginFrame(const SIMPLPluginFrame&); // Copy Constructor Not Implemented
    void operator=(const SIMPLPluginFrame&); // Operator '=' Not Implemented
};

#endif /* _AIM_PLUGIN_FRAME_H_ */

