

#include "EMMPMGuiPlugin.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPMGuiPlugin::EMMPMGuiPlugin()
: EMMPMPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPMGuiPlugin::~EMMPMGuiPlugin() = default;

#include "EMMPM/Gui/FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"
