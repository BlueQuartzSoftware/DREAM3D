/*
 * Your License or Copyright Information can go here
 */

#include "ITKPlugin.h"

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"

#include "PipelineBuilder/FilterWidgetManager.h"
#include "FilterWidgets/QFilterWidgetFactory.hpp"

#include "moc_ITKPlugin.cxx"

Q_EXPORT_PLUGIN2(ITKPlugin, ITKPlugin)

namespace Detail
{
   const std::string ITKPluginFile("ITKPlugin");
   const std::string ITKPluginDisplayName("ITKPlugin");
   const std::string ITKPluginBaseName("ITKPlugin");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ITKPlugin::ITKPlugin()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ITKPlugin::~ITKPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ITKPlugin::getPluginName()
{
  return QString::fromStdString(Detail::ITKPluginDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKPlugin::writeSettings(QSettings &prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKPlugin::readSettings(QSettings &prefs)
{

}

#include "ITKFilters/RegisterKnownFilterWidgets.cpp"
#include "ITKFilters/RegisterKnownFilters.cpp"

