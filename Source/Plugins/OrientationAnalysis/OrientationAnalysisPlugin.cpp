/*
 * Your License or Copyright Information can go here
 */

#include "OrientationAnalysisPlugin.h"

//#include "DREAM3DLib/Common/FilterManager.h"
//#include "DREAM3DLib/Common/IFilterFactory.hpp"
//#include "DREAM3DLib/Common/FilterFactory.hpp"

#include "PipelineBuilder/FilterWidgetManager.h"
#include "FilterWidgets/QFilterWidgetFactory.hpp"

#include "moc_OrientationAnalysisPlugin.cxx"

Q_EXPORT_PLUGIN2(OrientationAnalysisPlugin, OrientationAnalysisPlugin)

namespace Detail
{
  const std::string OrientationAnalysisPluginFile("OrientationAnalysisPlugin");
  const std::string OrientationAnalysisPluginDisplayName("OrientationAnalysisPlugin");
  const std::string OrientationAnalysisPluginBaseName("OrientationAnalysisPlugin");
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
  return QString::fromStdString(Detail::OrientationAnalysisPluginDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationAnalysisPlugin::writeSettings(QSettings& prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationAnalysisPlugin::readSettings(QSettings& prefs)
{

}

#include "OrientationAnalysisFilters/RegisterKnownFilterWidgets.cpp"
//#include "OrientationAnalysisFilters/RegisterKnownFilters.cpp"

