/*
 * Your License or Copyright Information can go here
 */

#ifndef _EMMPM_H_
#define _EMMPM_H_

#include <QtCore/QObject>
#include <QtCore/QSettings>

#include "DREAM3DLib/Plugin/DREAM3DPluginInterface.h"


/**
 * @class EMMPMPlugin EMMPMPlugin.h SurfaceMeshing/EMMPMPlugin.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class EMMPMPlugin : public QObject, public DREAM3DPluginInterface
{
    Q_OBJECT

    Q_INTERFACES(DREAM3DPluginInterface)

  public:
    EMMPMPlugin();
    virtual ~EMMPMPlugin();
    /**
     * @brief Returns the name of the plugin
     */
    virtual QString getPluginName();

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
     * @brief Returns the group of the plugin
     */
    virtual QString getGroup();

    /**
     * @brief Returns the URL of the plugin
     */
    virtual QString getURL();

    /**
     * @brief Returns the location of the plugin
     */
    virtual QString getLocation();

    /**
     * @brief Returns the compatible platforms of the plugin
     */
    virtual QList<QString> getPlatforms();

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
     * @brief Returns the dependencies of the plugin
     */
    virtual QList<QString> getDependencies();


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
    QString             m_Group;
    QString             m_URL;
    QString             m_Location;
    QList<QString>      m_Platforms;
    QString             m_Description;
    QString             m_Copyright;
    QString             m_License;
    QList<QString>      m_Dependencies;

    EMMPMPlugin(const EMMPMPlugin&); // Copy Constructor Not Implemented
    void operator=(const EMMPMPlugin&); // Operator '=' Not Implemented
};

#endif /* _EMMPM_H_ */
