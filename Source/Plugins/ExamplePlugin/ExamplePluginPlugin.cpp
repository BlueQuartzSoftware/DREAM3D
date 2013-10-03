/*
 * Your License or Copyright Information can go here
 */

#include "ExamplePluginPlugin.h"

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"

#include "PipelineBuilder/FilterWidgetManager.h"
#include "FilterWidgets/QFilterWidgetFactory.hpp"

#include "moc_ExamplePluginPlugin.cxx"

Q_EXPORT_PLUGIN2(ExamplePluginPlugin, ExamplePluginPlugin)

namespace Detail
{
  const std::string ExamplePluginPluginFile("ExamplePluginPlugin");
  const std::string ExamplePluginPluginDisplayName("ExamplePluginPlugin");
  const std::string ExamplePluginPluginBaseName("ExamplePluginPlugin");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExamplePluginPlugin::ExamplePluginPlugin()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExamplePluginPlugin::~ExamplePluginPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExamplePluginPlugin::getPluginName()
{
  return QString::fromStdString(Detail::ExamplePluginPluginDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExamplePluginPlugin::writeSettings(QSettings& prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExamplePluginPlugin::readSettings(QSettings& prefs)
{

}

#include "ExamplePluginFilters/RegisterKnownFilterWidgets.cpp"
#include "ExamplePluginFilters/RegisterKnownFilters.cpp"

