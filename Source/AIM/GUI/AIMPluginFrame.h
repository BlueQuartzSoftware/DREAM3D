/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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

#ifndef _AIM_PLUGIN_FRAME_H_
#define _AIM_PLUGIN_FRAME_H_

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtGui/QFrame>
#include <QtCore/QSettings>
#include <QtGui/QStatusBar>

class QLineEdit;

/*
 *
 */
class AIMPluginFrame : public QFrame
{
    Q_OBJECT;

  public:
    AIMPluginFrame(QWidget *parent = 0);
    virtual ~AIMPluginFrame();

    /**
      * @brief Reads the Preferences from the users pref file
      */
    virtual void readSettings(QSettings &prefs) = 0;

     /**
      * @brief Writes the preferences to the users pref file
      */
    virtual void writeSettings(QSettings &prefs) = 0;

    /**
     * @brief Enables or Disables all the widgets in a list
     * @param b
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


    virtual void checkIOFiles() = 0;

    virtual void setStatusBar(QStatusBar* statusBar);

    virtual QStatusBar* statusBar();

    virtual bool sanityCheckOutputDirectory(QLineEdit* le, QString msgTitle);


    signals:

      void processStarted();
      void processEnded();
      void cancelProcess();
      void processMessage(const QString &);

    private slots:
      // slots for our worker thread to communicate
      virtual void threadHasMessage(QString message) = 0;

      /* Reconstruction Thread communicates throught these methods */
      virtual void threadFinished() = 0;
      virtual void threadProgressed(int value) = 0;

    private:
      QStatusBar*    m_StatusBar;

      AIMPluginFrame(const AIMPluginFrame&); // Copy Constructor Not Implemented
      void operator=(const AIMPluginFrame&); // Operator '=' Not Implemented
};

#endif /* _AIM_PLUGIN_FRAME_H_ */
