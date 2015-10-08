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

#include "PluginManager.h"

PluginManager* PluginManager::self = NULL;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PluginManager::PluginManager()
{
//  qDebug() << "PluginManager()" << this;
  Q_ASSERT_X(!self, "PluginManager", "There should be only one PluginManager object");
  PluginManager::self = this;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PluginManager::~PluginManager()
{
//  qDebug() << "~PluginManager()" << this;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PluginManager* PluginManager::Instance()
{

  if (self == NULL)
  {
    //  qDebug() << "PluginManager::Instance self was NULL" << "\n";
    self = new PluginManager();
  }
  return self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginManager::printPluginNames()
{
  for(QVector<ISIMPLibPlugin*>::iterator iter = plugins.begin(); iter != plugins.end(); ++iter)
  {
    qDebug() << "Name: " << *iter << "\n";
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginManager::addPlugin(ISIMPLibPlugin* plugin)
{
  plugins.push_back(plugin);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> PluginManager::getPluginNames()
{
  QList<QString> pluginNames;
  for (QVector<ISIMPLibPlugin*>::iterator iter = plugins.begin(); iter != plugins.end(); ++iter)
  {
    ISIMPLibPlugin* plugin = *iter;
    if(NULL != plugin)
    {
      pluginNames.push_back(plugin->getPluginName());
    }
  }
  return pluginNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PluginManager::getPluginName(QString filtName)
{
  QVector<ISIMPLibPlugin*> plugins = getPluginsVector();
  for (int i = 0; i < plugins.size(); i++)
  {
    ISIMPLibPlugin* plugin = plugins[i];

    if (plugin->getFilters().contains(filtName))
    {
      return plugin->getPluginName();
    }
  }

  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<ISIMPLibPlugin*> PluginManager::getPluginsVector()
{
  return plugins;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ISIMPLibPlugin* PluginManager::findPlugin(QString pluginName)
{
  for (QVector<ISIMPLibPlugin*>::iterator iter = plugins.begin(); iter != plugins.end(); iter++)
  {
    ISIMPLibPlugin* plugin = *iter;
    if (plugin->getPluginName() == pluginName)
    {
      return plugin;
    }
  }

  // If the plugin isn't found in the vector...
  return NULL;
}
