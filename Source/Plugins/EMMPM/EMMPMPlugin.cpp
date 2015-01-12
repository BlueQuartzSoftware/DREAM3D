/*
 * Your License or Copyright Information can go here
 */


#include "EMMPMPlugin.h"

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"


#include "EMMPM/moc_EMMPMPlugin.cpp"

Q_EXPORT_PLUGIN2(EMMPMPlugin, EMMPMPlugin)

namespace Detail
{
  const QString EMMPMPluginFile("EMMPMPlugin");
  const QString EMMPMPluginDisplayName("EMMPMPlugin");
  const QString EMMPMPluginBaseName("EMMPMPlugin");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPMPlugin::EMMPMPlugin() :
m_Version(""),
m_CompatibilityVersion(""),
m_Vendor(""),
m_Group(""),
m_URL(""),
m_Location(""),
m_Platforms(QList<QString>()),
m_Description(""),
m_Copyright(""),
m_License(""),
m_Dependencies(QList<QString>())
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPMPlugin::~EMMPMPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMMPMPlugin::getPluginName()
{
  return (Detail::EMMPMPluginDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMMPMPlugin::getVersion()
{
  return m_Version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMMPMPlugin::getCompatibilityVersion()
{
  return m_CompatibilityVersion;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMMPMPlugin::getVendor()
{
  return m_Vendor;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMMPMPlugin::getGroup()
{
  return m_Group;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMMPMPlugin::getURL()
{
  return m_URL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMMPMPlugin::getLocation()
{
  return m_Location;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> EMMPMPlugin::getPlatforms()
{
  return m_Platforms;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMMPMPlugin::getDescription()
{
  return m_Description;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMMPMPlugin::getCopyright()
{
  return m_Copyright;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMMPMPlugin::getLicense()
{
  return m_License;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> EMMPMPlugin::getDependencies()
{
  return m_Dependencies;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMPlugin::writeSettings(QSettings& prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMPlugin::readSettings(QSettings& prefs)
{

}

#include "EMMPMFilters/RegisterKnownFilters.cpp"

#include "FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"

