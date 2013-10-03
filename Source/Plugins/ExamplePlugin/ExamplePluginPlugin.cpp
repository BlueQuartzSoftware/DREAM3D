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
  const QString ExamplePluginPluginFile("ExamplePluginPlugin");
  const QString ExamplePluginPluginDisplayName("ExamplePluginPlugin");
  const QString ExamplePluginPluginBaseName("ExamplePluginPlugin");
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
  return (Detail::ExamplePluginPluginDisplayName);
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

