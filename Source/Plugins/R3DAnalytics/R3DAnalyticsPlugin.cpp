/*
 * Your License or Copyright Information can go here
 */
#include "R3DAnalyticsPlugin.h"

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"

#include "PipelineBuilder/FilterWidgetManager.h"
#include "FilterWidgets/QFilterWidgetFactory.hpp"

#include "moc_R3DAnalyticsPlugin.cxx"

Q_EXPORT_PLUGIN2(R3DAnalyticsPlugin, R3DAnalyticsPlugin)

namespace Detail
{
   const std::string R3DAnalyticsPluginFile("R3DAnalyticsPlugin");
   const std::string R3DAnalyticsPluginDisplayName("R3DAnalyticsPlugin");
   const std::string R3DAnalyticsPluginBaseName("R3DAnalyticsPlugin");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
R3DAnalyticsPlugin::R3DAnalyticsPlugin()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
R3DAnalyticsPlugin::~R3DAnalyticsPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString R3DAnalyticsPlugin::getPluginName()
{
  return QString::fromStdString(Detail::R3DAnalyticsPluginDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void R3DAnalyticsPlugin::writeSettings(QSettings &prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void R3DAnalyticsPlugin::readSettings(QSettings &prefs)
{

}

#include "R3DAnalyticsFilters/RegisterKnownFilterWidgets.cpp"
#include "R3DAnalyticsFilters/RegisterKnownFilters.cpp"
