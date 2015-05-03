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

#include "DevHelper.h"

#include <QtCore/QtDebug>

#include "QtSupportLib/ApplicationAboutBoxDialog.h"

#include <QtWidgets/QDesktopWidget>

#include "PluginMaker/PluginMaker.h"
#include "PluginMaker/FilterMaker.h"

#include "DREAM3D/License/PluginMakerLicenseFiles.h"

#include <iostream>

#include "HelpWidget.h"

enum WidgetIndices {
  PLUGIN_MAKER,
  FILTER_MAKER
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DevHelper::DevHelper(QWidget* parent) :
QMainWindow(parent)
{
  setupUi(this);

  setupGui();

  readSettings();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DevHelper::~DevHelper()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DevHelper::setupGui()
{
  // Allow internal widgets to update the status bar
  connect(filterMaker, SIGNAL(updateStatusBar(QString)), this, SLOT(updateStatusMessage(QString)));
  connect(pluginMaker, SIGNAL(updateStatusBar(QString)), this, SLOT(updateStatusMessage(QString)));

  //Set window to open at the center of the screen
  QDesktopWidget* desktop = QApplication::desktop();

  int screenWidth, width;
  int screenHeight, height;
  int x, y;
  QSize windowSize;

  screenWidth = desktop->width(); // get width of screen
  screenHeight = desktop->height(); // get height of screen

  windowSize = size(); // size of application window
  width = windowSize.width();
  height = windowSize.height();

  x = (screenWidth - width) / 2;
  y = (screenHeight - height) / 2;
  y -= 50;

  // move window to desired coordinates
  move(x, y);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DevHelper::updateStatusMessage(QString message)
{
  statusbar->showMessage(message);
}

// -----------------------------------------------------------------------------
//  Called when the main window is closed.
// -----------------------------------------------------------------------------
void DevHelper::closeEvent(QCloseEvent* event)
{
    writeSettings();
}

// -----------------------------------------------------------------------------
//  Write our Prefs to file
// -----------------------------------------------------------------------------
void DevHelper::writeSettings()
{
  // qDebug() << "writeSettings" << "\n";
#if defined (Q_OS_MAC)
  QSettings prefs(QSettings::NativeFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#else
  QSettings prefs(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#endif

  prefs.beginGroup("DevHelper");

  // Write PluginMaker settings
  prefs.beginGroup("PluginMaker");
  //Save the Plugin Name and Output Directory features to the QSettings object
  prefs.setValue("Plugin Name", pluginMaker->m_PluginName->text());
  prefs.setValue("Output Directory", pluginMaker->m_OutputDir->text());
  prefs.endGroup();

  // Write FilterMaker settings
  prefs.beginGroup("FilterMaker");
  //Save the Plugin Name and Output Directory features to the QSettings object
  prefs.setValue("Plugin Directory", filterMaker->pluginDir->text());
  prefs.setValue("Filter Name", filterMaker->filterName->text());
  prefs.beginWriteArray("FilterParameters");
  for (int i = 0; i < filterMaker->filterParametersTable->rowCount(); i++)
  {
    prefs.setArrayIndex(i);
    prefs.setValue("Variable Name", filterMaker->filterParametersTable->item(i, VAR_NAME)->text());
    prefs.setValue("Human Label", filterMaker->filterParametersTable->item(i, HUMAN_NAME)->text());
    prefs.setValue("Type", filterMaker->filterParametersTable->item(i, TYPE)->text());
    prefs.setValue("Initial Value", filterMaker->filterParametersTable->item(i, INIT_VALUE)->text());
  }
  prefs.endArray();
  prefs.endGroup();

  writeWindowSettings(prefs);
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DevHelper::writeWindowSettings(QSettings& prefs)
{
  prefs.beginGroup("WindowSettings");
  QByteArray geo_data = saveGeometry();
  QByteArray layout_data = saveState();
  prefs.setValue(QString("Geometry"), geo_data);
  prefs.setValue(QString("Layout"), layout_data);
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DevHelper::readSettings()
{
  // qDebug() << "Read Settings" << "\n";
#if defined (Q_OS_MAC)
  QSettings prefs(QSettings::NativeFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#else
  QSettings prefs(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#endif

  prefs.beginGroup("DevHelper");

  prefs.beginGroup("PluginMaker");
  // Have the PipelineBuilder Widget read its settings
  pluginMaker->m_PluginName->setText(prefs.value("Plugin Name").toString());
  pluginMaker->m_OutputDir->setText(prefs.value("Output Directory").toString());
  readWindowSettings(prefs);
  prefs.endGroup();

  prefs.beginGroup("FilterMaker");
  // Have the PipelineBuilder Widget read its settings
  filterMaker->pluginDir->setText(prefs.value("Plugin Directory").toString());
  filterMaker->filterName->setText(prefs.value("Filter Name").toString());
  int fpSize = prefs.beginReadArray("FilterParameters");
  for (int i = 0; i < fpSize; i++)
  {
    prefs.setArrayIndex(i);

    AddFilterParameter addFilterParameter;
    addFilterParameter.setVariableName(prefs.value("Variable Name").toString());
    addFilterParameter.setHumanName(prefs.value("Human Label").toString());
    addFilterParameter.setType(prefs.value("Type").toString());
    addFilterParameter.setInitValue(prefs.value("Initial Value").toString());

    filterMaker->addFilterParameterToTable(&addFilterParameter);
  }
  prefs.endArray();
  prefs.endGroup();

  readWindowSettings(prefs);
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DevHelper::readWindowSettings(QSettings& prefs)
{
  bool ok = false;
  prefs.beginGroup("WindowSettings");
  if (prefs.contains(QString("Geometry")))
  {
    QByteArray geo_data = prefs.value(QString("Geometry")).toByteArray();
    ok = restoreGeometry(geo_data);
    if (!ok)
    {
      qDebug() << "Error Restoring the Window Geometry" << "\n";
    }
  }

  if (prefs.contains(QString("Layout")))
  {
    QByteArray layout_data = prefs.value(QString("Layout")).toByteArray();
    restoreState(layout_data);
  }
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DevHelper::on_actionShowUserManual_triggered()
{
  HelpWidget* helpDialog = new HelpWidget(this);
  helpDialog->show();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DevHelper::on_actionAbout_triggered()
{
  ApplicationAboutBoxDialog about(PluginMakerProj::LicenseList, this);
  about.setApplicationInfo("DevHelper", "1.0.0");
  about.exec();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DevHelper::on_actionExit_triggered()
{
  // Quit the app
  qApp->quit();
}


