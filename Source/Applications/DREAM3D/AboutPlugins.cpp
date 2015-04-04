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

#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>

#include <QtCore/QPluginLoader>

#include "DREAM3DLib/Common/FilterManager.h"

#include "DREAM3DWidgetsLib/FilterWidgetManager.h"

#include "PluginDetails.h"
#include "AboutPlugins.h"

enum ColumnIndex
{
  NAME_INDEX = 0,
  CHECKBOX_INDEX = 1,
  STATUS_INDEX = 2,
  VERSION_INDEX = 3,
};

enum SettingIndex
{
  PATH_INDEX = 0,
  ENABLED_INDEX = 1
};

static const QString NOT_FOUND_STRING = "Not Found";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AboutPlugins::AboutPlugins(QWidget* parent) :
  m_loadPreferencesDidChange(false)
{
  setupUi(this);

  setupGui();

  // Hide the add and remove plugin buttons until we are ready to use them
  addPluginBtn->setVisible(false);
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
  pluginsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  pluginsTable->horizontalHeader()->setSectionResizeMode(NAME_INDEX, QHeaderView::ResizeToContents);
  pluginsTable->horizontalHeader()->setSectionResizeMode(CHECKBOX_INDEX, QHeaderView::ResizeToContents);
  pluginsTable->horizontalHeader()->setSectionResizeMode(STATUS_INDEX, QHeaderView::ResizeToContents);

  // Set selection behavior so that only full rows can be selected
  pluginsTable->setSelectionBehavior(QAbstractItemView::SelectRows);

  QList<PluginProxy::Pointer> currentCache = readPluginCache();
  loadPlugins(currentCache);

  // Set default cell to the first item in the list
  pluginsTable->setCurrentCell(0, 0);
  QTableWidgetItem* statusItem = pluginsTable->item(pluginsTable->currentRow(), STATUS_INDEX);

  if (statusItem->text() == NOT_FOUND_STRING)
  {
	  removePluginBtn->setVisible(true);
  }
  else
  {
	  removePluginBtn->setVisible(false);
  }

  connect(pluginsTable, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(displayDetailsWindow(QTableWidgetItem*)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::loadPlugins(QList<PluginProxy::Pointer> proxies)
{
  PluginManager* manager = PluginManager::Instance();
  QVector<IDREAM3DPlugin*> vector = manager->getPluginsVector();
  pluginsTable->setRowCount(vector.size());

  // Iterate over PluginManager and add each entry to the plugin table
  for (int row=0; row<vector.size(); row++)
  {
    IDREAM3DPlugin* plugin = vector.at(row);
    addPluginToTable(plugin, row);
  }

  // Iterate over proxy and find all entries that are not in PluginManager
  QList<QString> managerNames = manager->getPluginNames();
  QList<QString> pluginNames;

  for (QList<PluginProxy::Pointer>::iterator proxyIter = proxies.begin(); proxyIter != proxies.end(); proxyIter++)
  {
    pluginNames.push_back((*proxyIter)->getPluginName());
  }

  for (QList<QString>::iterator nameIter = pluginNames.begin(); nameIter != pluginNames.end(); nameIter++)
  {
    QString proxyName = *nameIter;
    if ( managerNames.contains(proxyName) == false )
    {
      qDebug() << "The plugin " << proxyName << " was not found in the PluginManager.";
    DREAM3DPlugin* plugin = new DREAM3DPlugin();
    plugin->setPluginName(proxyName);
    plugin->setStatus(NOT_FOUND_STRING);
      addPlaceHolderToTable(plugin, 0);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::addPluginToTable(IDREAM3DPlugin* plugin, int row)
{
  QColor defaultColor(Qt::white);

  // Add name of plugin to the row
  QTableWidgetItem* nameItem = new QTableWidgetItem(plugin->getPluginName());
  nameItem->setBackgroundColor(defaultColor);
  pluginsTable->setItem(row, NAME_INDEX, nameItem);

  // Set checkbox cell to default settings
  QTableWidgetItem* enabledItem = new QTableWidgetItem();
  enabledItem->setBackgroundColor(defaultColor);
  pluginsTable->setItem(row, ENABLED_INDEX, enabledItem);

  // Add check box that is centered in the cell
  QCheckBox* checkBox = new QCheckBox(NULL);
  readCheckState(checkBox, plugin->getPluginName());

  connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(togglePluginState(int)));
  connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(setLoadPreferencesFlag(int)));

  QHBoxLayout* layout = new QHBoxLayout(NULL);
  layout->addWidget(checkBox);
  layout->setAlignment(Qt::AlignCenter);
  layout->setContentsMargins(0, 0, 0, 0);
  QWidget* widget = new QWidget(NULL);
  widget->setLayout(layout);
  pluginsTable->setCellWidget(row, CHECKBOX_INDEX, widget);


  QTableWidgetItem* statusItem;
  // Add load status information
  if (plugin->getDidLoad() == true)
  {
    statusItem = new QTableWidgetItem("Enabled");
  }
  else
  {
    statusItem = new QTableWidgetItem("Disabled");
  }
  statusItem->setBackgroundColor(Qt::white);
  pluginsTable->setItem(row, STATUS_INDEX, statusItem);

  // Add version information
  QTableWidgetItem* versionItem = new QTableWidgetItem(plugin->getVersion());
  versionItem->setBackgroundColor(Qt::white);
  pluginsTable->setItem(row, VERSION_INDEX, versionItem);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::addPlaceHolderToTable(DREAM3DPlugin* plugin, int row)
{
  // Insert the empty row
  pluginsTable->insertRow(row);

  // Create bold font
  QFont errorFont;
  errorFont.setBold(true);
  QColor errorColor(240, 165, 179);

  // Add name of plugin to the row
  QTableWidgetItem* nameItem = new QTableWidgetItem(plugin->getPluginName());
  nameItem->setFont(errorFont);
  nameItem->setBackgroundColor(errorColor);
  pluginsTable->setItem(row, NAME_INDEX, nameItem);

  // Add empty checkbox item to the row
  QTableWidgetItem* enabledItem = new QTableWidgetItem();
  enabledItem->setBackgroundColor(errorColor);
  pluginsTable->setItem(row, ENABLED_INDEX, enabledItem);

  // Add status to the row
  QTableWidgetItem* statusItem = new QTableWidgetItem(NOT_FOUND_STRING);
  statusItem->setBackgroundColor(errorColor);
  statusItem->setFont(errorFont);
  pluginsTable->setItem(row, STATUS_INDEX, statusItem);

  // Add version information
  QTableWidgetItem* versionItem = new QTableWidgetItem(plugin->getVersion());
  versionItem->setBackgroundColor(errorColor);
  pluginsTable->setItem(row, VERSION_INDEX, versionItem);
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
  QTableWidgetItem* item = pluginsTable->item(pluginsTable->currentRow(), NAME_INDEX);

  // Launch Details dialog box
  PluginDetails dialog(item->text());
  dialog.exec();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::on_pluginsTable_cellClicked(int row, int column)
{
  if (pluginsTable->item(row, STATUS_INDEX)->text() == NOT_FOUND_STRING)
  {
    detailsBtn->setDisabled(true);
	removePluginBtn->setVisible(true);
  }
  else
  {
    detailsBtn->setEnabled(true);
	removePluginBtn->setVisible(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::setLoadPreferencesFlag()
{
  setLoadPreferencesFlag(0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::setLoadPreferencesFlag(int state)
{
  m_loadPreferencesDidChange = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::on_addPluginBtn_pressed()
{
  QString pluginPath = QFileDialog::getOpenFileName(this, tr("Open Plugin File Path"), "", tr("Plugin Files (*.plugin)"));
  QFileInfo fileInfo(pluginPath);
  if (fileInfo.suffix() == "plugin")
  {
    addPlugin(pluginPath);
  }
  else
  {
    qDebug() << "Could not add plugin because " << pluginPath << " does not have the 'plugin' file extension!";
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::addPlugin(QString pluginPath)
{
  PluginManager* pluginManager = PluginManager::Instance();
  FilterManager* fm = FilterManager::Instance();
  FilterWidgetManager*  fwm = FilterWidgetManager::Instance();

  QApplication::instance()->processEvents();
  QPluginLoader* loader = new QPluginLoader(pluginPath);
  QFileInfo fi(pluginPath);
  QString fileName = fi.fileName();
  QObject* plugin = loader->instance();
  if (plugin)
  {
    IDREAM3DPlugin* ipPlugin = qobject_cast<IDREAM3DPlugin*>(plugin);
    if (ipPlugin)
    {
      QString pluginName = ipPlugin->getPluginName();

      ipPlugin->registerFilterWidgets(fwm);
      ipPlugin->registerFilters(fm);
      ipPlugin->setDidLoad(true);

      ipPlugin->setLocation(pluginPath);
      pluginManager->addPlugin(ipPlugin);

      addPluginToTable(ipPlugin, 0);
      setLoadPreferencesFlag();
    }
  }
  else
  {
    QString message("The plugin did not load with the following error\n");
    message.append(loader->errorString());
    QMessageBox::critical(this, "DREAM3D Plugin Load Error",
      message,
      QMessageBox::Ok | QMessageBox::Default);
    delete loader;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::on_removePluginBtn_pressed()
{
  QTableWidgetItem* nameItem = pluginsTable->item(pluginsTable->currentRow(), NAME_INDEX);

  if (NULL != nameItem)
  {
    QString pluginName = nameItem->text();

    QMessageBox confirm;
    confirm.setText("Are you sure that you want to remove '" + pluginName + "'?");
    confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirm.setDefaultButton(QMessageBox::Yes);
    confirm.setWindowTitle("Remove Plugin");
    int answer = confirm.exec();

    if (answer == QMessageBox::Yes)
    {
      deletePlugin(nameItem);
      m_loadPreferencesDidChange = true;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::deletePlugin(QTableWidgetItem* nameItem)
{
  QString pluginName = nameItem->text();

#if defined (Q_OS_MAC)
  QSettings::Format format = QSettings::NativeFormat;
#else
  QSettings::Format format = QSettings::IniFormat;
#endif
  QString filePath;

  QSettings prefs(format, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());

  filePath = prefs.fileName();

  // Remove plugin from cache
  prefs.beginGroup("PluginPreferences");
  prefs.remove(pluginName);
  prefs.endGroup();

  // Remove entry from plugin table
  pluginsTable->removeRow( nameItem->row() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::writePluginCache()
{
#if defined (Q_OS_MAC)
  QSettings::Format format = QSettings::NativeFormat;
#else
  QSettings::Format format = QSettings::IniFormat;
#endif
  QString filePath;

  QSettings prefs(format, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());

  filePath = prefs.fileName();

  prefs.beginGroup("PluginPreferences");

  QList<PluginProxy::Pointer> proxies = getPluginCheckBoxSettingsFromGUI();

  for (QList<PluginProxy::Pointer>::iterator proxyIter = proxies.begin(); proxyIter != proxies.end(); proxyIter++)
  {
    QString pluginName = (*proxyIter)->getPluginName();
    QString filePath = (*proxyIter)->getFilePath();
    bool enabled = (*proxyIter)->getEnabled();

  prefs.beginGroup(pluginName);
  prefs.setValue(QString::number(PATH_INDEX), filePath);
  prefs.setValue(QString::number(ENABLED_INDEX), enabled);
  prefs.endGroup();
  }

  prefs.endGroup();
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

  prefs.beginGroup("PluginPreferences");
  prefs.beginGroup(pluginName);
  checkBox->setChecked(prefs.value(QString::number(ENABLED_INDEX), true).toBool());
  prefs.endGroup();
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::displayDetailsWindow(QTableWidgetItem* item)
{
  QTableWidgetItem* statusItem = pluginsTable->item(item->row(), STATUS_INDEX);

  if (NULL != statusItem && statusItem->text() != NOT_FOUND_STRING)
  {
    on_detailsBtn_clicked();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<PluginProxy::Pointer> AboutPlugins::getPluginCheckBoxSettingsFromGUI()
{
  PluginManager* manager = PluginManager::Instance();
  QList<PluginProxy::Pointer> list;

  for (int row=0; row<pluginsTable->rowCount(); row++)
  {
    QString pluginName = pluginsTable->item(row, NAME_INDEX)->text();
    IDREAM3DPlugin* plugin = manager->findPlugin(pluginName);

    PluginProxy::Pointer proxy = PluginProxy::New();
    proxy->setPluginName(pluginName);

    if (pluginsTable->cellWidget(row, CHECKBOX_INDEX) != NULL)
    {
      QWidget* widget = pluginsTable->cellWidget(row, CHECKBOX_INDEX);

      if (NULL == widget)
      {
        return QList<PluginProxy::Pointer>();
      }
      QHBoxLayout* layout = qobject_cast<QHBoxLayout*>(widget->layout());

      if (NULL == layout)
      {
        return QList<PluginProxy::Pointer>();
      }
      QCheckBox* checkBox = qobject_cast<QCheckBox*>(layout->itemAt(0)->widget());

      if (NULL == checkBox)
      {
        return QList<PluginProxy::Pointer>();
      }

      if (checkBox->checkState() == Qt::Checked)
      {
        proxy->setEnabled(true);
      }
      else
      {
        proxy->setEnabled(false);
      }
    }

    if ( plugin )
    {
      QString filePath = plugin->getLocation();
      proxy->setFilePath(filePath);
    }

    list.push_back(proxy);
  }

  return list;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<PluginProxy::Pointer> AboutPlugins::readPluginCache()
{
  QList<PluginProxy::Pointer> proxyList;

#if defined (Q_OS_MAC)
  QSettings::Format format = QSettings::NativeFormat;
#else
  QSettings::Format format = QSettings::IniFormat;
#endif
  QString filePath;

  QSettings prefs(format, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());

  filePath = prefs.fileName();

  prefs.beginGroup("PluginPreferences");
  QStringList pluginNameList = prefs.childGroups();

  for (QStringList::iterator iter = pluginNameList.begin(); iter != pluginNameList.end(); iter++)
  {
    PluginProxy::Pointer proxy = PluginProxy::New();
    QString pluginName = *iter;

    prefs.beginGroup(pluginName);
    proxy->setPluginName(pluginName);
    proxy->setFilePath(prefs.value(QString::number(PATH_INDEX)).toString());
    proxy->setEnabled(prefs.value(QString::number(ENABLED_INDEX)).toBool());
    prefs.endGroup();

    proxyList.push_back(proxy);
  }
  prefs.endGroup();

  return proxyList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AboutPlugins::getLoadPreferencesDidChange()
{
  return m_loadPreferencesDidChange;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::togglePluginState(int state)
{

}



