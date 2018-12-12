/*
 * Your License or Copyright can go here
 */

#pragma once

#include <QtCore/QObject>
#include <QtCore/QSettings>

#include "SIMPLib/Plugin/ISIMPLibPlugin.h"

#include "ImportExport/ImportExportDLLExport.h"

/**
 * @brief The ImportExportPlugin class
 */
class ImportExport_EXPORT ImportExportPlugin : public QObject, public ISIMPLibPlugin
{
  Q_OBJECT
  Q_INTERFACES(ISIMPLibPlugin)
  Q_PLUGIN_METADATA(IID "net.bluequartz.dream3d.ImportExportPlugin")

public:
  ImportExportPlugin();
  virtual ~ImportExportPlugin();

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
   * @brief getVersion Returns the version
   */
  virtual QString getVersion();

  /**
   * @brief getCompatibilityVersion Returns the compatibility version
   */
  virtual QString getCompatibilityVersion();

  /**
   * @brief getVendor Returns the name of the vendor
   */
  virtual QString getVendor();

  /**
   * @brief getURL Returns the URL of the plugin
   */
  virtual QString getURL();

  /**
   * @brief getLocationReturns the location of the plugin
   */
  virtual QString getLocation();

  /**
   * @brief getDescription Returns the description of the plugin
   * PLEASE UPDATE YOUR PLUGIN'S DESCRIPTION FILE.
   * It is located at ImportExport/Resources/ImportExport/ImportExportDescription.txt
   */
  virtual QString getDescription();

  /**
   * @brief getCopyright Returns the copyright of the plugin
   */
  virtual QString getCopyright();

  /**
   * @brief getLicense Returns the license of the plugin
   * PLEASE UPDATE YOUR PLUGIN'S LICENSE FILE.
   * It is located at ImportExport/Resources/ImportExport/ImportExportLicense.txt
   */
  virtual QString getLicense();

  /**
   * @brief getFilters Returns the filters of the plugin
   */
  virtual QList<QString> getFilters();

  /**
   * @brief getThirdPartyLicenses Returns the third party licenses of the plugin
   */
  virtual QMap<QString, QString> getThirdPartyLicenses();

  /**
   * @brief getDidLoad Returns the load status of the plugin
   */
  virtual bool getDidLoad();

  /**
   * @brief setDidLoad Sets the load status of the plugin
   * @param didLoad Boolean value to set status
   */
  virtual void setDidLoad(bool didLoad);

  /**
   * @brief setLocation Sets the location of the plugin on the file system.
   * This is required so that we can cache the file path information
   * as the plugin is loaded.
   * @param filePath File path
   */
  virtual void setLocation(QString filePath);

  /**
   * @brief registerFilterWidgets Register all the filters with the FilterWidgetFactory
   * @param fwm FilterWidgetManager instance pointer
   */
  virtual void registerFilterWidgets(FilterWidgetManager* fwm);

  /**
   * @brief registerFilters Registers the filters that this plugin implements with the Filter Manager that is passed in
   * @param fm FilterManager instance pointer
   */
  virtual void registerFilters(FilterManager* fm);

  /**
   * @brief writeSettings Writes the settings in the input gui to the Application's preference file
   * @param prefs QSettings reference variable
   */
  virtual void writeSettings(QSettings& prefs);

  /**
   * @brief readSettings Reads the settings from the Application's preference file and sets
   * the input GUI widgets accordingly.
   * @param prefs QSettings reference variable
   */
  virtual void readSettings(QSettings& prefs);

private:
  QString m_Version;
  QString m_CompatibilityVersion;
  QString m_Vendor;
  QString m_URL;
  QString m_Location;
  QString m_Description;
  QString m_Copyright;
  QList<QString> m_Filters;
  bool m_DidLoad;

  ImportExportPlugin(const ImportExportPlugin&); // Copy Constructor Not Implemented
  void operator=(const ImportExportPlugin&);     // Operator '=' Not Implemented
};
