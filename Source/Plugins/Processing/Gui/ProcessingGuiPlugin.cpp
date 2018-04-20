

#include "ProcessingGuiPlugin.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ProcessingGuiPlugin::ProcessingGuiPlugin()
: ProcessingPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ProcessingGuiPlugin::~ProcessingGuiPlugin() = default;

#include "Processing/Gui/FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"
