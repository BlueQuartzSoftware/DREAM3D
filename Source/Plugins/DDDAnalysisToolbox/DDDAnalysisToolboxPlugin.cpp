/*
 * Your License or Copyright Information can go here
 */


#include "DDDAnalysisToolboxPlugin.h"

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"


#include "DDDAnalysisToolbox/moc_DDDAnalysisToolboxPlugin.cpp"

Q_EXPORT_PLUGIN2(DDDAnalysisToolboxPlugin, DDDAnalysisToolboxPlugin)

namespace Detail
{
  const QString DDDAnalysisToolboxPluginFile("DDDAnalysisToolboxPlugin");
  const QString DDDAnalysisToolboxPluginDisplayName("DDDAnalysisToolboxPlugin");
  const QString DDDAnalysisToolboxPluginBaseName("DDDAnalysisToolboxPlugin");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DDDAnalysisToolboxPlugin::DDDAnalysisToolboxPlugin() :
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
DDDAnalysisToolboxPlugin::~DDDAnalysisToolboxPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DDDAnalysisToolboxPlugin::getPluginName()
{
  return (Detail::DDDAnalysisToolboxPluginDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DDDAnalysisToolboxPlugin::getVersion()
{
  return m_Version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DDDAnalysisToolboxPlugin::getCompatibilityVersion()
{
  return m_CompatibilityVersion;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DDDAnalysisToolboxPlugin::getVendor()
{
  return m_Vendor;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DDDAnalysisToolboxPlugin::getGroup()
{
  return m_Group;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DDDAnalysisToolboxPlugin::getURL()
{
  return m_URL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DDDAnalysisToolboxPlugin::getLocation()
{
  return m_Location;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> DDDAnalysisToolboxPlugin::getPlatforms()
{
  return m_Platforms;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DDDAnalysisToolboxPlugin::getDescription()
{
  return m_Description;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DDDAnalysisToolboxPlugin::getCopyright()
{
  return m_Copyright;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DDDAnalysisToolboxPlugin::getLicense()
{
  return m_License;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> DDDAnalysisToolboxPlugin::getDependencies()
{
  return m_Dependencies;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DDDAnalysisToolboxPlugin::writeSettings(QSettings& prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DDDAnalysisToolboxPlugin::readSettings(QSettings& prefs)
{

}

#include "DDDAnalysisToolboxFilters/RegisterKnownFilters.cpp"

#include "DDDAnalysisToolbox/FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"

