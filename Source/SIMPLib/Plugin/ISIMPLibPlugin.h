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


#ifndef _ISIMPLibPlugin_H_
#define _ISIMPLibPlugin_H_


#include <QtCore/QSettings>
#include <QtCore/QtPlugin>
#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"


#ifdef NDEBUG

#define SIMPLib_PLUGIN_2(name)\
  #name

#define SIMPLib_PLUGIN_3(name)\
  #name

#else


#define SIMPLib_PLUGIN_1(name)\
  #name

#define SIMPLib_PLUGIN_2(name)\
  SIMPLib_PLUGIN_1(name##_debug)

#define SIMPLib_PLUGIN_3(name)\
  SIMPLib_PLUGIN_1(name Debug)

#endif



#define SIMPLib_PLUGIN_CONSTANTS(libName, human)\
  namespace SIMPLib\
  {\
    namespace UIPlugins\
    {\
      const QString libName##File(SIMPLib_PLUGIN_2(libName##Plugin));\
      const QString libName##DisplayName(SIMPLib_PLUGIN_3(human));\
      const QString libName##BaseName(#libName);\
    }\
  }

class FilterManager;
class FilterWidgetManager;

/**
 * @class ISIMPLibPlugin ISIMPLibPlugin.h IPHelper/plugins/ISIMPLibPlugin.h
 * @brief This class defines the interface used by plugins that would like to process
 * images and display their output.
 *
 * @date Jul 10, 2010
 * @version 1.0
 *
 * @section intro Introduction
 *  In order to develop a plugin for the IPHelper there are several classes that need to be
 *  implemented by the developer.

 *
 *  The developer should implement all three classes in their own plugin in order to
 *  have a fully functioning plugin that is able to display an input GUI and process
 *  the input images.
 *
 * @subsection plugin_interface PluginInterface
 *    The developer should create a C++ class that inherits publicly from ISIMPLibPlugin and
 *    QObject.
 *    @code
 *    class MyPlugin : public QObject, public ISIMPLibPlugin {
 *    @endcode
 *
 *    The developer will also need to add some macro declarations to their class declaration (usually
 *     in the header file.
 *
 *    @code
 *      Q_OBJECT
 *      Q_INTERFACES(ISIMPLibPlugin )
 *    @endcode
 *    The programmer will also need to add the following macros to their implementation file.
 *    @code
 *    ;
 *    @endcode
 *
 *    At this point the developer is ready to implement each of the virtual functions in the
 *    ISIMPLibPlugin in order to make their plugin valid
 *
 * @subsection plugin_inputui PluginInputUI
 *   In order for the user to be able to set the proper inputs for thier image
 *   processing code the developer will need to design a QWidget based input UI
 *   using QtDesigner or by hand if they wish. An existing class QImageProcessingInputFrame
 *   already has some existing functionality that the developer may wish to readily inherit
 *   from. If that is the case then the developer can simply inherit from QImageProcessingInputFrame
 *   instead of another Qt class as is the norm for QtDesigner based widgets. See the
 *   Qt documentation if you are unfamiliar with how to use Qt Designer to create a GUI.
 *
 * @subsection plugin_task PluginTask
 *  If the developer would like to take advantage of the ProcessQueueController and the
 *  ProcessQueueDialog facilities
 *  that are available then they can create a MyPluginTask class that inherits from
 *  the ProcessQueueTask class. They simply need to implement the <tt>run()</tt> method.
 *
 * @section details Details
 */
class SIMPLib_EXPORT ISIMPLibPlugin
{
  public:
    SIMPL_SHARED_POINTERS(ISIMPLibPlugin)

    virtual ~ISIMPLibPlugin() {}

    /**
     * @brief Returns the name of the plugin
     */
    virtual QString getPluginName() = 0;

    /**
     * @brief Returns the version of the plugin
     */
    virtual QString getVersion() = 0;

    /**
     * @brief Returns the compatibility version of the plugin
     */
    virtual QString getCompatibilityVersion() = 0;

    /**
     * @brief Returns the name of the vendor of the plugin
     */
    virtual QString getVendor() = 0;

    /**
     * @brief Returns the URL of the plugin
     */
    virtual QString getURL() = 0;

    /**
     * @brief Returns the location of the plugin
     */
    virtual QString getLocation() = 0;

    /**
     * @brief Returns the description of the plugin
     */
    virtual QString getDescription() = 0;

    /**
     * @brief Returns the copyright string of the plugin
     */
    virtual QString getCopyright() = 0;

    /**
     * @brief Returns the license string of the plugin
     */
    virtual QString getLicense() = 0;

    /**
     * @brief Returns the dependencies of the plugin
     */
    virtual QList<QString> getFilters() = 0;

    /**
     * @brief Returns the third party licenses of the plugin
     */
    virtual QMap<QString, QString> getThirdPartyLicenses() = 0;

    /**
     * @brief Returns the load status of the plugin
     */
    virtual bool getDidLoad() = 0;

    /**
     * @brief Sets the load status of the plugin
     */
    virtual void setDidLoad(bool didLoad) = 0;

    /**
     * @brief Sets the location of the plugin on the file system.
     * This is required so that we can cache the file path information
     * as the plugin is loaded.
     */
    virtual void setLocation(QString filePath) = 0;

    /**
     * @brief Register all the filters with the FilterWidgetFactory
     */
    virtual void registerFilterWidgets(FilterWidgetManager* fwm) = 0;

    /**
     * @brief registerFilters Registers all the filters for this plugin
     */
    virtual void registerFilters(FilterManager* fm) = 0;

    /**
     * @brief Writes the settings in the input gui to the Application's preference file
     * @param prefs A valid QSettings pointer.
     */
    virtual void writeSettings(QSettings& prefs) = 0;

    /**
     * @brief Reads the settings from the Application's preference file and sets
     * the input GUI widgets accordingly.
     * @param prefs
     */
    virtual void readSettings(QSettings& prefs) = 0;

};


Q_DECLARE_INTERFACE(ISIMPLibPlugin,
                    "net.BlueQuartz.ISIMPLibPlugin/1.0")

#endif /* ISIMPLibPlugin_H_ */

