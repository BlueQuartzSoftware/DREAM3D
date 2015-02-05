/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2012 Joseph B. Kleingers (Student Research Assistant)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, Joseph B. Kleingers,
 * the US Air Force, BlueQuartz Software nor the names of its contributors may be
 * used to endorse or promote products derived from this software without specific
 * prior written permission.
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

#ifndef _PLUGINMAKER_H_
#define _PLUGINMAKER_H_

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtGui/QMainWindow>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QCloseEvent>
#include <QtCore/QSettings>

#include "ui_PluginMaker.h"
#include "FilterBundler.h"

class PluginMaker : public QMainWindow, public Ui::PluginMaker
{
    Q_OBJECT

  public:
    PluginMaker(QWidget* parent = 0);

  protected:
    void setupGui();

    QString generateFileSystemPath(QString);

  protected slots:
    void on_selectButton_clicked();
    void on_generateButton_clicked();

    void on_m_PluginName_textChanged(const QString& text);
    void on_m_OutputDir_textChanged(const QString& text);

    void on_treeWidget_itemSelectionChanged();
    void generationError(const QString& test);

    void on_actionPlugin_Maker_Help_triggered();
    void on_actionAbout_triggered();

    void on_addFilterBtn_clicked();
    void on_removeFilterBtn_clicked();

  private:
    QString m_OpenDialogLastDirectory;
    QString cleanName(QString name);
    QString cleanName_filters(QString name);
    void closeEvent(QCloseEvent* event);
    qint32 checkDirtyDocument();
    void writeSettings();
    void readSettings();
    void readWindowSettings(QSettings& prefs);
    void writeWindowSettings(QSettings& prefs);
    QString generateCmakeContents();
// QString generateQrcContents();

    QVector<FilterBundler> m_FilterBundles;
    QTreeWidgetItem* F_name;
    QTreeWidgetItem* F_doc;
    QTreeWidgetItem* F_res;
    QTreeWidgetItem* F_res_sub;
    QTreeWidgetItem* F_namefilters;

};

#endif
