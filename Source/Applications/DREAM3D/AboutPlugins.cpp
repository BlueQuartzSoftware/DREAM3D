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

enum ColumnIndex
{
  NAME_INDEX = 0,
  CHECKBOX_INDEX = 1,
  STATUS_INDEX = 2,
  LOCATION_INDEX = 3,
  VERSION_INDEX = 4,
  VENDOR_INDEX = 5,
  DELETE_INDEX = 6
};

enum SettingIndex
{
  PATH_INDEX = 0,
  ENABLED_INDEX = 1
};

static const QString DOES_NOT_EXIST_STRING = "This Plugin No Longer Exists";

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
  pluginsTable->horizontalHeader()->setResizeMode(NAME_INDEX, QHeaderView::ResizeToContents);
  pluginsTable->horizontalHeader()->setResizeMode(CHECKBOX_INDEX, QHeaderView::ResizeToContents);
  pluginsTable->horizontalHeader()->setResizeMode(STATUS_INDEX, QHeaderView::ResizeToContents);
  pluginsTable->horizontalHeader()->setResizeMode(LOCATION_INDEX, QHeaderView::ResizeToContents);
  pluginsTable->horizontalHeader()->setResizeMode(VERSION_INDEX, QHeaderView::ResizeToContents);


  // Set selection behavior so that only full rows can be selected
  pluginsTable->setSelectionBehavior(QAbstractItemView::SelectRows);

  QList<PluginProxy::Pointer> currentCache = readPluginCache();
  loadPlugins(currentCache);

  // Set default cell to the first item in the list
  pluginsTable->setCurrentCell(0, 0);

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
	  plugin->setDidLoad(false);
	  plugin->setLocation(DOES_NOT_EXIST_STRING);
      addPlaceHolderToTable(plugin, 0);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::addPluginToTable(IDREAM3DPlugin* plugin, int row)
{
  // Add name of plugin to the row
  QTableWidgetItem* nameItem = new QTableWidgetItem(plugin->getPluginName());
  pluginsTable->setItem(row, NAME_INDEX, nameItem);


  // Add check box that is centered in the cell
  QCheckBox* checkBox = new QCheckBox(NULL);
  readCheckState(checkBox, plugin->getPluginName());

  connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(writePluginCache(int)));

  connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(togglePluginState(int)));

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
  pluginsTable->setItem(row, STATUS_INDEX, statusItem);

  // Add location information
  QTableWidgetItem* locationItem = new QTableWidgetItem(plugin->getLocation());
  pluginsTable->setItem(row, LOCATION_INDEX, locationItem);

  // Add version information
  QTableWidgetItem* versionItem = new QTableWidgetItem(plugin->getVersion());
  pluginsTable->setItem(row, VERSION_INDEX, versionItem);

  // Add vendor information
  QTableWidgetItem* vendorItem = new QTableWidgetItem(plugin->getVendor());
  pluginsTable->setItem(row, VENDOR_INDEX, vendorItem);

  // Add delete button that is centered in the cell
  QWidget* deleteWidget = new QWidget(NULL);
  QHBoxLayout* deleteLayout = new QHBoxLayout(NULL);
  QPushButton* deleteBtn = new QPushButton(deleteWidget);
  deleteBtn->setText("Delete");

  connect(deleteBtn, SIGNAL(pressed()), this, SLOT(deletePlugin()));

  deleteLayout->addWidget(deleteBtn);
  deleteLayout->setAlignment(Qt::AlignCenter);
  deleteLayout->setContentsMargins(0, 0, 0, 0);
  deleteWidget->setLayout(deleteLayout);
  pluginsTable->setCellWidget(row, DELETE_INDEX, deleteWidget);
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

	// Add name of plugin to the row
	QTableWidgetItem* nameItem = new QTableWidgetItem(plugin->getPluginName());
	nameItem->setFont(errorFont);
	nameItem->setBackgroundColor(Qt::red);
	pluginsTable->setItem(row, NAME_INDEX, nameItem);

	// Add empty checkbox item to the row
	QTableWidgetItem* enabledItem = new QTableWidgetItem();
	enabledItem->setBackgroundColor(Qt::red);
	pluginsTable->setItem(row, ENABLED_INDEX, enabledItem);

	// Add empty status item to the row
	QTableWidgetItem* statusItem = new QTableWidgetItem();
	statusItem->setBackgroundColor(Qt::red);
	pluginsTable->setItem(row, STATUS_INDEX, statusItem);

	// Add location information
	QTableWidgetItem* locationItem = new QTableWidgetItem(plugin->getLocation());
	locationItem->setFont(errorFont);
	locationItem->setBackgroundColor(Qt::red);
	pluginsTable->setItem(row, LOCATION_INDEX, locationItem);

	// Add version information
	QTableWidgetItem* versionItem = new QTableWidgetItem(plugin->getVersion());
	versionItem->setBackgroundColor(Qt::red);
	pluginsTable->setItem(row, VERSION_INDEX, versionItem);

	// Add vendor information
	QTableWidgetItem* vendorItem = new QTableWidgetItem(plugin->getVendor());
	vendorItem->setBackgroundColor(Qt::red);
	pluginsTable->setItem(row, VENDOR_INDEX, vendorItem);

	// Add delete button that is centered in the cell
	QWidget* deleteWidget = new QWidget(NULL);
	QHBoxLayout* deleteLayout = new QHBoxLayout(NULL);
	QPushButton* deleteBtn = new QPushButton(deleteWidget);
	deleteBtn->setText("Delete");

	connect(deleteBtn, SIGNAL(pressed()), this, SLOT(deletePlugin()));

	deleteLayout->addWidget(deleteBtn);
	deleteLayout->setAlignment(Qt::AlignCenter);
	deleteLayout->setContentsMargins(0, 0, 0, 0);
	deleteWidget->setLayout(deleteLayout);
	pluginsTable->setCellWidget(row, DELETE_INDEX, deleteWidget);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::deletePlugin()
{
	QPushButton* btn = qobject_cast<QPushButton*>( sender() );

	if (NULL == btn)
	{
		return;
	}

	QWidget* cellWidget = qobject_cast<QWidget*>( btn->parent() );

	if (NULL == cellWidget)
	{
		return;
	}

	QModelIndex index = pluginsTable->indexAt(cellWidget->pos());
	QTableWidgetItem* nameItem = pluginsTable->item(index.row(), NAME_INDEX);

	if (NULL != nameItem)
	{
		QString pluginName = nameItem->text();
	}
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AboutPlugins::deletePlugin(QString pluginName)
{
#if defined (Q_OS_MAC)
	QSettings::Format format = QSettings::NativeFormat;
#else
	QSettings::Format format = QSettings::IniFormat;
#endif
	QString filePath;

	QSettings prefs(format, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());

	filePath = prefs.fileName();

	PluginManager* manager = PluginManager::Instance();
	QVector<IDREAM3DPlugin*> pluginsVector = manager->getPluginsVector();
	for (QVector<IDREAM3DPlugin*>::iterator pluginIter = pluginsVector.begin(); pluginIter != pluginsVector.end(); pluginIter++)
	{
		IDREAM3DPlugin* plugin = *pluginIter;
		if (plugin->getPluginName() == pluginName)
		{
			QString path = plugin->getLocation();
			prefs.beginGroup("PluginPreferences");

		}
	}
}
#endif // 0


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
	if (pluginsTable->item(row, LOCATION_INDEX)->text() == DOES_NOT_EXIST_STRING)
	{
		detailsBtn->setDisabled(true);
	}
	else
	{
		detailsBtn->setEnabled(true);
	}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutPlugins::writePluginCache(int state)
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
	QTableWidgetItem* locationItem = pluginsTable->item(item->row(), LOCATION_INDEX);

	if (NULL != locationItem && locationItem->text() != DOES_NOT_EXIST_STRING)
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





