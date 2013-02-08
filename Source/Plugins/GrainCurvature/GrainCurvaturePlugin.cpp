/*
 * Your License or Copyright Information can go here
 */
#include "GrainCurvaturePlugin.h"

#include "PipelineBuilder/FilterWidgetManager.h"
#include "FilterWidgets/QFilterWidgetFactory.hpp"

#include "moc_GrainCurvaturePlugin.cxx"

Q_EXPORT_PLUGIN2(GrainCurvaturePlugin, GrainCurvaturePlugin)

namespace Detail
{
   const std::string GrainCurvaturePluginFile("GrainCurvaturePlugin");
   const std::string GrainCurvaturePluginDisplayName("GrainCurvaturePlugin");
   const std::string GrainCurvaturePluginBaseName("GrainCurvaturePlugin");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainCurvaturePlugin::GrainCurvaturePlugin()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainCurvaturePlugin::~GrainCurvaturePlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GrainCurvaturePlugin::getPluginName()
{
  return QString::fromStdString(Detail::GrainCurvaturePluginDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainCurvaturePlugin::writeSettings(QSettings &prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainCurvaturePlugin::readSettings(QSettings &prefs)
{

}

#include "GrainCurvatureFilters/RegisterKnownFilterWidgets.cpp"

