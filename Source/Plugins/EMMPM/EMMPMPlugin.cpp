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
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMMPMPlugin::getCompatibilityVersion()
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMMPMPlugin::getVendor()
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMMPMPlugin::getGroup()
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMMPMPlugin::getURL()
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMMPMPlugin::getLocation()
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> EMMPMPlugin::getPlatforms()
{
  QList<QString> empty;
  return empty;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMMPMPlugin::getDescription()
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMMPMPlugin::getCopyright()
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMMPMPlugin::getLicense()
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> EMMPMPlugin::getDependencies()
{
  QList<QString> empty;
  return empty;
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

