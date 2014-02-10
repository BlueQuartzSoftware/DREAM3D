/*
 * Your License or Copyright Information can go here
 */
#include "R3DAnalytics.h"

#include "PipelineBuilder/FilterWidgetManager.h"
#include "FilterWidgets/QFilterWidgetFactory.hpp"

#include "moc_R3DAnalytics.cxx"

Q_EXPORT_PLUGIN2(R3DAnalytics, R3DAnalytics)

namespace Detail
{
   const std::string R3DAnalyticsFile("R3DAnalytics");
   const std::string R3DAnalyticsDisplayName("R3DAnalytics");
   const std::string R3DAnalyticsBaseName("R3DAnalytics");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
R3DAnalytics::R3DAnalytics()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
R3DAnalytics::~R3DAnalytics()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString R3DAnalytics::getPluginName()
{
  return QString::fromStdString(Detail::R3DAnalyticsDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void R3DAnalytics::writeSettings(QSettings &prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void R3DAnalytics::readSettings(QSettings &prefs)
{

}

#include "R3DAnalyticsFilters/RegisterKnownFilterWidgets.cpp"

