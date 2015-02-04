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

#ifndef ABOUTPLUGINS_H
#define ABOUTPLUGINS_H

#include <QtGui/QCheckBox>

#include <QtCore/QObject>

#include <DREAM3DLib/Plugin/PluginProxy.h>
#include <DREAM3DLib/Plugin/PluginManager.h>
#include <DREAM3DLib/Plugin/IDREAM3DPlugin.h>
#include <DREAM3DLib/Plugin/DREAM3DPlugin.h>


//-- UIC generated Header
#include "ui_AboutPlugins.h"

class AboutPlugins : public QDialog, private Ui::AboutPlugins
{
    Q_OBJECT

  public:
    AboutPlugins(QWidget* parent = 0);
    virtual ~AboutPlugins();

    void setupGui();

    void addPluginToTable(IDREAM3DPlugin* plugin, int row);
	void addPlaceHolderToTable(DREAM3DPlugin* plugin, int row);

    void loadPlugins(QList<PluginProxy::Pointer> proxies);

    void readCheckState(QCheckBox* checkBox, QString pluginName);

    QList<PluginProxy::Pointer> getPluginCheckBoxSettingsFromGUI();
    static QList<PluginProxy::Pointer> readPluginCache();

	void writePluginCache();

    bool getLoadPreferencesDidChange();

	void deletePlugin(QTableWidgetItem* nameItem);

	void addPlugin(QString pluginPath);

	void setLoadPreferencesFlag();

  public slots:
    void on_closeBtn_clicked();
    void on_detailsBtn_clicked();
	void on_addPluginBtn_pressed();
	void on_removePluginBtn_pressed();
	void on_pluginsTable_cellClicked(int row, int column);
    void displayDetailsWindow(QTableWidgetItem* item);
	void setLoadPreferencesFlag(int state);

  private:
    bool m_loadPreferencesDidChange;

    AboutPlugins(const AboutPlugins&); // Copy Constructor Not Implemented
    void operator=(const AboutPlugins&); // Operator '=' Not Implemented
};

#endif // ABOUTPLUGINS_H
