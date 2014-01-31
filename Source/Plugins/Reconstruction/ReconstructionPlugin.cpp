/*
 * Your License or Copyright Information can go here
 */

#include "ReconstructionPlugin.h"

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"

//#include "PipelineBuilder/FilterWidgetManager.h"
//#include "FilterWidgets/PipelineFilterWidgetFactory.hpp"

#include "moc_ReconstructionPlugin.cxx"

Q_EXPORT_PLUGIN2(ReconstructionPlugin, ReconstructionPlugin)

namespace Detail
{
  const QString ReconstructionPluginFile("ReconstructionPlugin");
  const QString ReconstructionPluginDisplayName("ReconstructionPlugin");
  const QString ReconstructionPluginBaseName("ReconstructionPlugin");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReconstructionPlugin::ReconstructionPlugin()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReconstructionPlugin::~ReconstructionPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReconstructionPlugin::getPluginName()
{
  return (Detail::ReconstructionPluginDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionPlugin::writeSettings(QSettings& prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionPlugin::readSettings(QSettings& prefs)
{

}

//#include "ReconstructionFilters/RegisterKnownFilterWidgets.cpp"
#include "ReconstructionFilters/RegisterKnownFilters.cpp"

