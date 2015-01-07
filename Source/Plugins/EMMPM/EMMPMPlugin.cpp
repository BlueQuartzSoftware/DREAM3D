/*
 * Your License or Copyright Information can go here
 */


#include "EMMPMPlugin.h"

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"


#include "EMMPM/moc_EMMPMPlugin.cpp"

Q_EXPORT_PLUGIN2(EMMPMPlugin, EMMPMPlugin)

namespace Detail
{
  const QString EMMPMPluginFile("EMMPMPlugin");
  const QString EMMPMPluginDisplayName("EMMPMPlugin");
  const QString EMMPMPluginBaseName("EMMPMPlugin");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPMPlugin::EMMPMPlugin()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPMPlugin::~EMMPMPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMMPMPlugin::getPluginName()
{
  return (Detail::EMMPMPluginDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMPlugin::writeSettings(QSettings& prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMPlugin::readSettings(QSettings& prefs)
{

}

#include "EMMPMFilters/RegisterKnownFilters.cpp"

#include "FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"

