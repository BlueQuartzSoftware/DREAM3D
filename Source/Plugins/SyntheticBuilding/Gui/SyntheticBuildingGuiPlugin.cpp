

#include "SyntheticBuildingGuiPlugin.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SyntheticBuildingGuiPlugin::SyntheticBuildingGuiPlugin()
: SyntheticBuildingPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SyntheticBuildingGuiPlugin::~SyntheticBuildingGuiPlugin() = default;

#include "SyntheticBuilding/Gui/FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"
