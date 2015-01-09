/*
 * Your License or Copyright Information can go here
 */

#include "SamplingPlugin.h"

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"

//#include "PipelineBuilder/FilterWidgetManager.h"
//#include "FilterWidgets/PipelineFilterWidgetFactory.hpp"

#include "moc_SamplingPlugin.cpp"

Q_EXPORT_PLUGIN2(SamplingPlugin, SamplingPlugin)

namespace Detail
{
  const QString SamplingPluginFile("SamplingPlugin");
  const QString SamplingPluginDisplayName("SamplingPlugin");
  const QString SamplingPluginBaseName("SamplingPlugin");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SamplingPlugin::SamplingPlugin() :
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
SamplingPlugin::~SamplingPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SamplingPlugin::getPluginName()
{
  return (Detail::SamplingPluginDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SamplingPlugin::getVersion()
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SamplingPlugin::getCompatibilityVersion()
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SamplingPlugin::getVendor()
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SamplingPlugin::getGroup()
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SamplingPlugin::getURL()
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SamplingPlugin::getLocation()
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> SamplingPlugin::getPlatforms()
{
  QList<QString> empty;
  return empty;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SamplingPlugin::getDescription()
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SamplingPlugin::getCopyright()
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SamplingPlugin::getLicense()
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> SamplingPlugin::getDependencies()
{
  QList<QString> empty;
  return empty;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SamplingPlugin::writeSettings(QSettings& prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SamplingPlugin::readSettings(QSettings& prefs)
{

}

#include "SamplingFilters/RegisterKnownFilters.cpp"
#include "Sampling/FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"

