/*
 * Your License or Copyright Information can go here
 */

#include "UCSBPlugin.h"

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"

#include "PipelineBuilder/FilterWidgetManager.h"
#include "FilterWidgets/QFilterWidgetFactory.hpp"

#include "moc_UCSBPlugin.cxx"

Q_EXPORT_PLUGIN2(UCSBPlugin, UCSBPlugin)

namespace Detail
{
   const std::string UCSBPluginFile("UCSBPlugin");
   const std::string UCSBPluginDisplayName("UCSBPlugin");
   const std::string UCSBPluginBaseName("UCSBPlugin");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UCSBPlugin::UCSBPlugin()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UCSBPlugin::~UCSBPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString UCSBPlugin::getPluginName()
{
  return QString::fromStdString(Detail::UCSBPluginDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UCSBPlugin::writeSettings(QSettings &prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UCSBPlugin::readSettings(QSettings &prefs)
{

}

#include "UCSBFilters/RegisterKnownFilterWidgets.cpp"
#include "UCSBFilters/RegisterKnownFilters.cpp"

