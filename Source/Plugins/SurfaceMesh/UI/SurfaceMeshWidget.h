/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#ifndef _SURFACEMESHWIDGET_H_
#define _SURFACEMESHWIDGET_H_

#include <QtCore/QString>
#include <QtCore/QSettings>

#include "ui_SurfaceMeshWidget.h"

#include "QtSupport/AIMPluginFrame.h"
#include "QSurfaceMesh.h"

/**
 * @class SurfaceMeshWidget SurfaceMeshWidget.h Plugins/SurfaceMesh/UI/SurfaceMeshWidget.h
 * @brief This class represents the User Interface for the Surface Meshing module
 * of the AIMRepresentation program. The user interface subclasses QFrame which
 * should make it able to be embedded in most Qt applications.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jan 30, 2011
 * @version 1.0
 */
class SurfaceMeshWidget : public AIMPluginFrame, private Ui::SurfaceMeshWidget
{
  Q_OBJECT

  public:
    SurfaceMeshWidget(QWidget *parent = 0);
    virtual ~SurfaceMeshWidget();

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

    /**
     * @brief Concrete Implementation of checkIOFiles
     */
    virtual void checkIOFiles();


  protected slots:
    /* Surface Meshing Slots */
    void on_m_InputFileBtn_clicked();
    void on_m_OutputDirBtn_clicked();

    void on_m_GoBtn_clicked();

  private slots:
    // slots for our worker thread to communicate
    virtual void threadHasMessage(QString message);

    /* Surface Mesh Thread communicates throught these methods */
    virtual void threadFinished();
    virtual void threadProgressed(int value);

    void on_m_InputFile_textChanged(const QString & text);
    void on_m_OutputDir_textChanged(const QString & text);
    void on_m_OutputFilePrefix_textChanged(const QString &text);

  private:
    QList<QWidget*>             m_WidgetList;
    QSurfaceMesh*               m_SurfaceMesh;
    QThread*                    m_WorkerThread;

    QString m_OpenDialogLastDirectory;

    SurfaceMeshWidget(const SurfaceMeshWidget&); // Copy Constructor Not Implemented
    void operator=(const SurfaceMeshWidget&); // Operator '=' Not Implemented


};

#endif /* _SURFACEMESHWIDGET_H_ */
