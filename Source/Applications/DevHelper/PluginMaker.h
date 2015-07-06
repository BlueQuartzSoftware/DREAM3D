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


#ifndef _PLUGINMAKER_H_
#define _PLUGINMAKER_H_

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTreeWidgetItem>
#include <QtGui/QCloseEvent>
#include <QtCore/QSettings>
#include <QtCore/QSignalMapper>

#include "ui_PluginMaker.h"
#include "FilterBundler.h"

class PluginMaker : public QWidget, public Ui::PluginMaker
{
    Q_OBJECT

  public:
    PluginMaker(QWidget* parent = 0);

    bool validityCheck();

  protected:
    void setupGui();

  protected slots:
    void on_selectButton_clicked();
    void on_generateButton_clicked();

    void on_m_PluginName_textChanged(const QString& text);
    void on_m_OutputDir_textChanged(const QString& text);

    void on_treeWidget_itemSelectionChanged();
    void generationError(const QString& test);

    void on_addFilterBtn_clicked();
    void on_removeFilterBtn_clicked();

    void testFileLocationsHandler();

  signals:
    void clicked(QSet<QString> names);
    void updateStatusBar(QString message);

  private:
    QString m_OpenDialogLastDirectory;
    QString cleanName(QString name);
    QString cleanName_filters(QString name);
    qint32 checkDirtyDocument();
    QString generateCmakeContents();
// QString generateQrcContents();

    QVector<FilterBundler> m_FilterBundles;
    QSet<QString> m_TestFileLocationNames;
    QTreeWidgetItem* F_name;
    QTreeWidgetItem* F_doc;
    QTreeWidgetItem* F_res;
    QTreeWidgetItem* F_res_sub;
    QTreeWidgetItem* F_test;
    QTreeWidgetItem* F_namefilters;
};

#endif
