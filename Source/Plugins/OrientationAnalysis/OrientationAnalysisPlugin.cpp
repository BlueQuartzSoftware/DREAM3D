/*
 * Your License or Copyright Information can go here
 */

#include "OrientationAnalysisPlugin.h"

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"

#include "PipelineBuilder/FilterWidgetManager.h"
#include "FilterWidgets/QFilterWidgetFactory.hpp"

#include "moc_OrientationAnalysisPlugin.cxx"

Q_EXPORT_PLUGIN2(OrientationAnalysisPlugin, OrientationAnalysisPlugin)

namespace Detail
{
   const QString OrientationAnalysisPluginFile("OrientationAnalysisPlugin");
   const QString OrientationAnalysisPluginDisplayName("OrientationAnalysisPlugin");
   const QString OrientationAnalysisPluginBaseName("OrientationAnalysisPlugin");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationAnalysisPlugin::OrientationAnalysisPlugin()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationAnalysisPlugin::~OrientationAnalysisPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString OrientationAnalysisPlugin::getPluginName()
{
  return (Detail::OrientationAnalysisPluginDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationAnalysisPlugin::writeSettings(QSettings &prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationAnalysisPlugin::readSettings(QSettings &prefs)
{

}

#include "OrientationAnalysisFilters/RegisterKnownFilterWidgets.cpp"
#include "OrientationAnalysisFilters/RegisterKnownFilters.cpp"

