/*
 * Your License or Copyright Information can go here
 */

#pragma once

#include <QtCore/QObject>
#include <QtCore/QSettings>

#include "SIMPLib/Plugin/ISIMPLibPlugin.h"

#include "EMMPM/EMMPMDLLExport.h"

/**
 * @class EMMPMPlugin EMMPMPlugin.hSurfaceMeshing/EMMPMPlugin.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class EMMPM_EXPORT EMMPMPlugin : public QObject, public ISIMPLibPlugin

{
  Q_OBJECT
  Q_INTERFACES(ISIMPLibPlugin)
  Q_PLUGIN_METADATA(IID "net.bluequartz.dream3d.EMMPMPlugin")

public:
  EMMPMPlugin();
  ~EMMPMPlugin() override;
  /**
   * @brief Returns the name of the plugin that appears on the file system.
   *
   * Note that if the build is a debug build there will be a _Plugin postfix
   * to the filename.
   */
  QString getPluginFileName() override;

  /**
   * @brief getPluginDisplayName The name that should be used for human facing
   * labels and display strings
   * @return
   */
  QString getPluginDisplayName() override;

  /**
   * @brief getPluginBaseName The Name of the plugin.
   *
   * This typically will NOT have the Plugin suffix.
   * @return
   */
  QString getPluginBaseName() override;

  /**
   * @brief Returns the version
   */
  QString getVersion() override;

  /**
   * @brief Returns the compatibility version
   */
  QString getCompatibilityVersion() override;

  /**
   * @brief Returns the name of the vendor
   */
  QString getVendor() override;

  /**
   * @brief Returns the URL of the plugin
   */
  QString getURL() override;

  /**
   * @brief Returns the location of the plugin
   */
  QString getLocation() override;

  /**
   * @brief Returns the description of the plugin
   */
  QString getDescription() override;

  /**
   * @brief Returns the copyright of the plugin
   */
  QString getCopyright() override;

  /**
   * @brief Returns the license of the plugin
   */
  QString getLicense() override;

  /**
   * @brief Returns the filters of the plugin
   */
  QList<QString> getFilters() override;

  /**
   * @brief Returns the third party licenses of the plugin
   */
  QMap<QString, QString> getThirdPartyLicenses() override;

  /**
   * @brief Returns the load status of the plugin
   */
  bool getDidLoad() override;

  /**
   * @brief Sets the load status of the plugin
   */
  void setDidLoad(bool didLoad) override;

  /**
   * @brief Sets the location of the plugin on the file system.
   * This is required so that we can cache the file path information
   * as the plugin is loaded.
   */
  void setLocation(QString filePath) override;

  /**
   * @brief Register all the filters with the FilterWidgetFactory
   */
  void registerFilterWidgets(FilterWidgetManager* fwm) override;

  /**
   * @brief This registers the filters that this plugin implements with the Filter Manager that is passed in
   * @param fm The FilterManager to register the filters into.
   */
  void registerFilters(FilterManager* fm) override;

  /**
   * @brief Writes the settings in the input gui to the Application's preference file
   * @param prefs A valid QSettings pointer.
   */
  void writeSettings(QSettings& prefs) override;

  /**
   * @brief Reads the settings from the Application's preference file and sets
   * the input GUI widgets accordingly.
   * @param prefs
   */
  void readSettings(QSettings& prefs) override;

private:
  QString m_Version;
  QString m_CompatibilityVersion;
  QString m_Vendor;
  QString m_URL;
  QString m_Location;
  QString m_Copyright;
  QList<QString> m_Filters;
  bool m_DidLoad;

public:
  EMMPMPlugin(const EMMPMPlugin&) = delete;            // Copy Constructor Not Implemented
  EMMPMPlugin(EMMPMPlugin&&) = delete;                 // Move Constructor Not Implemented
  EMMPMPlugin& operator=(const EMMPMPlugin&) = delete; // Copy Assignment Not Implemented
  EMMPMPlugin& operator=(EMMPMPlugin&&) = delete;      // Move Assignment Not Implemented
};
