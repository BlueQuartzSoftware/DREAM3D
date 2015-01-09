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
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DDDAnalysisToolboxPlugin::getCompatibilityVersion()
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DDDAnalysisToolboxPlugin::getVendor()
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DDDAnalysisToolboxPlugin::getGroup()
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DDDAnalysisToolboxPlugin::getURL()
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DDDAnalysisToolboxPlugin::getLocation()
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> DDDAnalysisToolboxPlugin::getPlatforms()
{
  QList<QString> empty;
  return empty;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DDDAnalysisToolboxPlugin::getDescription()
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DDDAnalysisToolboxPlugin::getCopyright()
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DDDAnalysisToolboxPlugin::getLicense()
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> DDDAnalysisToolboxPlugin::getDependencies()
{
  QList<QString> empty;
  return empty;
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

