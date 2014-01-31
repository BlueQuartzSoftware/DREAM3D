/*
 * Your License or Copyright Information can go here
 */

#include "SamplingPlugin.h"

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"

//#include "PipelineBuilder/FilterWidgetManager.h"
//#include "FilterWidgets/PipelineFilterWidgetFactory.hpp"

#include "moc_SamplingPlugin.cxx"

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
SamplingPlugin::SamplingPlugin()
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
void SamplingPlugin::writeSettings(QSettings& prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SamplingPlugin::readSettings(QSettings& prefs)
{

}

//#include "SamplingFilters/RegisterKnownFilterWidgets.cpp"
#include "SamplingFilters/RegisterKnownFilters.cpp"

