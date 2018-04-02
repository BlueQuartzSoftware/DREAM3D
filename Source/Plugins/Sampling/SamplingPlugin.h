/*
 * Your License or Copyright Information can go here
 */

#ifndef _samplingplugin_h_
#define _samplingplugin_h_

#include <QtCore/QObject>
#include <QtCore/QSettings>

#include "SIMPLib/Plugin/ISIMPLibPlugin.h"


/**
 * @class SamplingPlugin SamplingPlugin.h Sampling/SamplingPlugin.h
 * @brief
 *
 * @date May 10, 2012
 * @version 1.0
 */
class SamplingPlugin : public QObject, public ISIMPLibPlugin
{
    Q_OBJECT
    Q_INTERFACES(ISIMPLibPlugin)
    Q_PLUGIN_METADATA(IID "net.bluequartz.dream3d.SamplingPlugin")


  public:
    SamplingPlugin();
    virtual ~SamplingPlugin();
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
    * @brief This registers the filters that this plugin implements with the Filter Manager that is passed in
    * @param fm The FilterManager to register the filters into.
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
    QString             m_Description;
    QString             m_Copyright;
    QList<QString>      m_Filters;
    bool                m_DidLoad;

    SamplingPlugin(const SamplingPlugin&) = delete; // Copy Constructor Not Implemented
    void operator=(const SamplingPlugin&) = delete; // Move assignment Not Implemented
};

#endif /* _Sampling_H_ */

