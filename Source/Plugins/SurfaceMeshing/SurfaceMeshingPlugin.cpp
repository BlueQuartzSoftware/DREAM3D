/*
 * Your License or Copyright Information can go here
 */

#include "SurfaceMeshingPlugin.h"

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"

//#include "PipelineBuilder/FilterWidgetManager.h"
//#include "FilterWidgets/QFilterWidgetFactory.hpp"

#include "moc_SurfaceMeshingPlugin.cxx"

Q_EXPORT_PLUGIN2(SurfaceMeshingPlugin, SurfaceMeshingPlugin)

namespace Detail
{
  const QString SurfaceMeshingPluginFile("SurfaceMeshingPlugin");
  const QString SurfaceMeshingPluginDisplayName("SurfaceMeshingPlugin");
  const QString SurfaceMeshingPluginBaseName("SurfaceMeshingPlugin");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshingPlugin::SurfaceMeshingPlugin()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshingPlugin::~SurfaceMeshingPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SurfaceMeshingPlugin::getPluginName()
{
  return (Detail::SurfaceMeshingPluginDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshingPlugin::writeSettings(QSettings& prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshingPlugin::readSettings(QSettings& prefs)
{

}

//#include "SurfaceMeshingFilters/RegisterKnownFilterWidgets.cpp"
#include "SurfaceMeshingFilters/RegisterKnownFilters.cpp"

