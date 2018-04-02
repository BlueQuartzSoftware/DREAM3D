/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#ifndef _imageioplugin_h_
#define _imageioplugin_h_

#include <QtCore/QObject>
#include <QtCore/QSettings>
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"


/**
 * @class ImageIOPlugin ImageIOPlugin.h SurfaceMeshing/ImageIOPlugin.h
 * @brief
 *
 * @date May 10, 2012
 * @version 1.0
 */
class ImageIOPlugin : public QObject, public ISIMPLibPlugin
{
    Q_OBJECT
    Q_INTERFACES(ISIMPLibPlugin)
    Q_PLUGIN_METADATA(IID "net.bluequartz.dream3d.ImageIOPlugin")

  public:
    ImageIOPlugin();
    virtual ~ImageIOPlugin();
    /**
     * @brief Returns the name of the plugin that appears on the file system. 
     * 
     * Note that if the build is a debug build there will be a _Plugin postfix
     * to the filename.
     */
    virtual QString getPluginFileName();
    
    /**
     * @brief getPluginDisplayName The name that should be used for human facing
     * labels and display strings
     * @return 
     */
    virtual QString getPluginDisplayName();
    
    /**
     * @brief getPluginBaseName The Name of the plugin.
     * 
     * This typically will NOT have the Plugin suffix.
     * @return 
     */
    virtual QString getPluginBaseName();

    /**
     * @brief Returns the version
     */
    virtual QString getVersion();

    /**
    * @brief Returns the compatibility version
    */
    virtual QString getCompatibilityVersion();

    /**
    * @brief Returns the name of the vendor
    */
    virtual QString getVendor();

    /**
     * @brief Returns the URL of the plugin
     */
    virtual QString getURL();

    /**
     * @brief Returns the location of the plugin
     */
    virtual QString getLocation();

    /**
     * @brief Returns the description of the plugin
     */
    virtual QString getDescription();

    /**
     * @brief Returns the copyright of the plugin
     */
    virtual QString getCopyright();

    /**
     * @brief Returns the license of the plugin
     */
    virtual QString getLicense();

    /**
     * @brief Returns the filters of the plugin
     */
    virtual QList<QString> getFilters();

    /**
     * @brief Returns the third party licenses of the plugin
     */
    virtual QMap<QString, QString> getThirdPartyLicenses();

    /**
     * @brief Returns the load status of the plugin
     */
    virtual bool getDidLoad();

    /**
     * @brief Sets the load status of the plugin
     */
    virtual void setDidLoad(bool didLoad);

    /**
     * @brief Sets the location of the plugin on the file system.
     * This is required so that we can cache the file path information
     * as the plugin is loaded.
     */
    virtual void setLocation(QString filePath);

    /**
     * @brief Register all the filters with the FilterWidgetFactory
     */
    virtual void registerFilterWidgets(FilterWidgetManager* fwm);

    /**
     * @brief registerFilters
     */
    virtual void registerFilters(FilterManager* fm);

    /**
     * @brief Writes the settings in the input gui to the Application's preference file
     * @param prefs A valid QSettings pointer.
     */
    virtual void writeSettings(QSettings& prefs);

    /**
     * @brief Reads the settings from the Application's preference file and sets
     * the input GUI widgets accordingly.
     * @param prefs
     */
    virtual void readSettings(QSettings& prefs);

  private:
    QString             m_Version;
    QString             m_CompatibilityVersion;
    QString             m_Vendor;
    QString             m_URL;
    QString             m_Location;
    QString             m_Copyright;
    QList<QString>      m_Filters;
    bool                m_DidLoad;

    ImageIOPlugin(const ImageIOPlugin&) = delete;  // Copy Constructor Not Implemented
    void operator=(const ImageIOPlugin&) = delete; // Move assignment Not Implemented
};

#endif /* _ImageIOPlugin_H_ */

