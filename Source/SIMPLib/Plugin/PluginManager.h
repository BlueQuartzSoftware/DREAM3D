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

#ifndef _PluginManager_H_
#define _PluginManager_H_


#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

#include "ISIMPLibPlugin.h"

/**
 * @brief The PluginManager class manages instances of plugins and is mainly used to keep
 * track of all plugin instances. This class uses the Singleton design pattern.
 */
class SIMPLib_EXPORT PluginManager
{
  public:
//    SIMPL_SHARED_POINTERS(PluginManager)
    SIMPL_TYPE_MACRO(PluginManager)

    virtual ~PluginManager();

    /**
     * @brief Static instance to retrieve the global instance of this class
     * @return
     */
    static PluginManager* Instance();


    /**
     * @brief PluginManager::printPluginNames
     */
    void printPluginNames();

    /**
     * @brief Adds a Plugin to the list
     * @param plugin
     */
    void addPlugin(ISIMPLibPlugin* plugin);

    /**
     * @brief getPluginNames Returns all plugin names as a QSet
     * @return
     */
    QList<QString> getPluginNames();

    /**
    * @brief getPluginName Returns the plugin name for the given filter name
    * @return
    */
    QString getPluginName(QString filtName);

    /**
     * @brief getPluginList Returns the plugins list
     * @return
     */
    QVector<ISIMPLibPlugin*> getPluginsVector();

    /**
     * @brief findPlugin
     * @param pluginName
     * @return
     */
    ISIMPLibPlugin* findPlugin(QString pluginName);


  protected:
    PluginManager();

  private:

    QVector<ISIMPLibPlugin*> plugins;
    static PluginManager* self;

    PluginManager(const PluginManager&); // Copy Constructor Not Implemented
    void operator=(const PluginManager&); // Operator '=' Not Implemented
};

#endif /* _PluginManager_H_ */

