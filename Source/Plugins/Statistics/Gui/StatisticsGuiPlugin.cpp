

#include "StatisticsGuiPlugin.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatisticsGuiPlugin::StatisticsGuiPlugin()
: StatisticsPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatisticsGuiPlugin::~StatisticsGuiPlugin() = default;

#include "Statistics/Gui/FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"
