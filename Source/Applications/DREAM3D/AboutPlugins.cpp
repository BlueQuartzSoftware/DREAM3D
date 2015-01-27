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

#include <QtGui/QMessageBox>

#include "PluginDetails.h"

#include "AboutPlugins.h"

enum ColumnNumbers
{
  NAME_NUM = 0,
  LOAD_NUM = 1,
  VERSION_NUM = 2,
  VENDOR_NUM = 3
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AboutPlugins::AboutPlugins(QWidget* parent) :
  m_loadPreferencesDidChange(false)
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AboutPlugins::~AboutPlugins()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::setupGui()
{
  // Resize the column widths so that all text is showing
  pluginsTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  pluginsTable->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
  pluginsTable->horizontalHeader()->setResizeMode(1, QHeaderView::ResizeToContents);
  pluginsTable->horizontalHeader()->setResizeMode(2, QHeaderView::ResizeToContents);

  // Set selection behavior so that only full rows can be selected
  pluginsTable->setSelectionBehavior(QAbstractItemView::SelectRows);

  loadInstalledPlugins();

  // Set Default Cell
  pluginsTable->setCurrentCell(0, 0);

  connect(pluginsTable, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(displayDetailsWindow(QTableWidgetItem*)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::loadInstalledPlugins()
{
  PluginManager* manager = PluginManager::Instance();
  QVector<DREAM3DPluginInterface*> vector = manager->getPluginsVector();
  pluginsTable->setRowCount(vector.size());
  int col = 0;

  for (int row=0; row<vector.size(); row++)
  {
    DREAM3DPluginInterface* plugin = vector.at(row);
    col = 0;

    // Add name of plugin to the row
    QTableWidgetItem* nameItem = new QTableWidgetItem(plugin->getPluginName());
    pluginsTable->setItem(row, col, nameItem);
    col++;

    // Add check box that is centered in the cell
    QCheckBox* checkBox = new QCheckBox(NULL);
    readCheckState(checkBox, plugin->getPluginName());

    connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(writePluginLoadingPreferences(int)));

    connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(togglePluginState(int)));

    QHBoxLayout* layout = new QHBoxLayout(NULL);
    layout->addWidget(checkBox);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(0, 0, 0, 0);
    QWidget* widget = new QWidget(NULL);
    widget->setLayout(layout);
    pluginsTable->setCellWidget(row, col, widget);
    col++;

    // Add version information
    QTableWidgetItem* versionItem = new QTableWidgetItem(plugin->getVersion());
    pluginsTable->setItem(row, col, versionItem);
    col++;

    // Add vendor information
    QTableWidgetItem* vendorItem = new QTableWidgetItem(plugin->getVendor());
    pluginsTable->setItem(row, col, vendorItem);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::on_closeBtn_clicked()
{
  // Close the dialog box
  close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::on_detailsBtn_clicked()
{  
  QTableWidgetItem* item = pluginsTable->item(pluginsTable->currentRow(), NAME_NUM);

  // Launch Details dialog box
  PluginDetails dialog(item->text());
  dialog.exec();

  // Clean up memory
  delete item;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::writePluginLoadingPreferences(int state)
{
#if defined (Q_OS_MAC)
  QSettings::Format format = QSettings::NativeFormat;
#else
  QSettings::Format format = QSettings::IniFormat;
#endif
  QString filePath;

  QSettings prefs(format, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());

  filePath = prefs.fileName();

  prefs.beginGroup("PluginLoad");

  QMap<QString,bool> pluginSettingsMap = getPluginCheckBoxSettingsFromGUI();

  for (QMap<QString,bool>::iterator iter = pluginSettingsMap.begin(); iter != pluginSettingsMap.end(); iter++)
  {
    prefs.setValue(iter.key(), iter.value());
  }

  prefs.endGroup();

  m_loadPreferencesDidChange = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::togglePluginState(int state)
{
  QCheckBox* checkBox = qobject_cast<QCheckBox*>(sender());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::readCheckState(QCheckBox* checkBox, QString pluginName)
{
#if defined (Q_OS_MAC)
  QSettings::Format format = QSettings::NativeFormat;
#else
  QSettings::Format format = QSettings::IniFormat;
#endif
  QString filePath;

  QSettings prefs(format, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());

  filePath = prefs.fileName();

  prefs.beginGroup("PluginLoad");
  checkBox->setChecked(prefs.value(pluginName, true).toBool());
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::displayDetailsWindow(QTableWidgetItem* item)
{
  on_detailsBtn_clicked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMap<QString,bool> AboutPlugins::getPluginCheckBoxSettingsFromGUI()
{
  QMap<QString,bool> pluginSettingsMap;

  for (int row=0; row<pluginsTable->rowCount(); row++)
  {
    QString pluginName = pluginsTable->item(row, NAME_NUM)->text();
    if (pluginsTable->cellWidget(row, LOAD_NUM) != NULL)
    {
      QWidget* widget = pluginsTable->cellWidget(row, LOAD_NUM);

      if (NULL == widget)
      {
        return QMap<QString,bool>();
      }
      QHBoxLayout* layout = qobject_cast<QHBoxLayout*>(widget->layout());

      if (NULL == layout)
      {
        return QMap<QString,bool>();
      }
      QCheckBox* checkBox = qobject_cast<QCheckBox*>(layout->itemAt(0)->widget());

      if (NULL == checkBox)
      {
        return QMap<QString,bool>();
      }

      if (checkBox->checkState() == Qt::Checked)
      {
        pluginSettingsMap.insert(pluginName, true);
      }
      else
      {
        pluginSettingsMap.insert(pluginName, false);
      }
    }
  }

  return pluginSettingsMap;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMap<QString,bool> AboutPlugins::readPluginCheckBoxSettingsFromFile()
{
  QMap<QString,bool> map;

#if defined (Q_OS_MAC)
  QSettings::Format format = QSettings::NativeFormat;
#else
  QSettings::Format format = QSettings::IniFormat;
#endif
  QString filePath;

  QSettings prefs(format, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());

  filePath = prefs.fileName();

  prefs.beginGroup("PluginLoad");
  QStringList pluginNameList = prefs.allKeys();

  for (QStringList::iterator iter = pluginNameList.begin(); iter != pluginNameList.end(); iter++)
  {
    map.insert(*iter, prefs.value(*iter, true).toBool());
  }
  prefs.endGroup();

  return map;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AboutPlugins::getLoadPreferencesDidChange()
{
  return m_loadPreferencesDidChange;
}





