/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
  for(QList<DREAM3DPluginInterface*>::iterator iter = list.begin(); iter != list.end(); ++iter)
  {
    qDebug() << "Name: " << *iter << "\n";
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginManager::addPlugin(DREAM3DPluginInterface* plugin)
{
  list.push_back(plugin);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSet<QString> PluginManager::getPluginNames()
{
  QSet<QString> pluginNames;
  for (QList<DREAM3DPluginInterface*>::iterator iter = list.begin(); iter != list.end(); ++iter)
  {
    DREAM3DPluginInterface* plugin = *iter;
    if(NULL != plugin)
    {
      pluginNames.insert(plugin->getPluginName());
    }
  }
  return pluginNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<DREAM3DPluginInterface*> PluginManager::getPluginList()
{
  return list;
}
