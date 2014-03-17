/*
 * Your License or Copyright Information can go here
 */


#include "DDDAnalysisToolboxPlugin.h"

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"


#include "DDDAnalysisToolbox/moc_DDDAnalysisToolboxPlugin.cpp"

Q_EXPORT_PLUGIN2(DDDAnalysisToolboxPlugin, DDDAnalysisToolboxPlugin)

namespace Detail
{
  const QString DDDAnalysisToolboxPluginFile("DDDAnalysisToolboxPlugin");
  const QString DDDAnalysisToolboxPluginDisplayName("DDDAnalysisToolboxPlugin");
  const QString DDDAnalysisToolboxPluginBaseName("DDDAnalysisToolboxPlugin");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DDDAnalysisToolboxPlugin::DDDAnalysisToolboxPlugin()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DDDAnalysisToolboxPlugin::~DDDAnalysisToolboxPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DDDAnalysisToolboxPlugin::getPluginName()
{
  return (Detail::DDDAnalysisToolboxPluginDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DDDAnalysisToolboxPlugin::writeSettings(QSettings& prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DDDAnalysisToolboxPlugin::readSettings(QSettings& prefs)
{

}

#include "DDDAnalysisToolboxFilters/RegisterKnownFilters.cpp"

#include "FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"

