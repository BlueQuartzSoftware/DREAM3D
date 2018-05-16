

#include "IOGuiPlugin.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IOGuiPlugin::IOGuiPlugin()
: IOPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IOGuiPlugin::~IOGuiPlugin() = default;

#include "IO/Gui/FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"
