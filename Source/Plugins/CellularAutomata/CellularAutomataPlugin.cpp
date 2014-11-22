/*
 * Your License or Copyright Information can go here
 */


#include "CellularAutomataPlugin.h"

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"


#include "CellularAutomata/moc_CellularAutomataPlugin.cpp"

Q_EXPORT_PLUGIN2(CellularAutomataPlugin, CellularAutomataPlugin)

namespace Detail
{
  const QString CellularAutomataPluginFile("CellularAutomataPlugin");
  const QString CellularAutomataPluginDisplayName("CellularAutomataPlugin");
  const QString CellularAutomataPluginBaseName("CellularAutomataPlugin");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CellularAutomataPlugin::CellularAutomataPlugin()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CellularAutomataPlugin::~CellularAutomataPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CellularAutomataPlugin::getPluginName()
{
  return (Detail::CellularAutomataPluginDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CellularAutomataPlugin::writeSettings(QSettings& prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CellularAutomataPlugin::readSettings(QSettings& prefs)
{

}

#include "CellularAutomataFilters/RegisterKnownFilters.cpp"

#include "FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"

