

#include "ReconstructionGuiPlugin.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReconstructionGuiPlugin::ReconstructionGuiPlugin()
: ReconstructionPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReconstructionGuiPlugin::~ReconstructionGuiPlugin() = default;

#include "Reconstruction/Gui/FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"
