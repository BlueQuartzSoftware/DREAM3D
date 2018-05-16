

#include "GenericGuiPlugin.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenericGuiPlugin::GenericGuiPlugin()
: GenericPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenericGuiPlugin::~GenericGuiPlugin() = default;

#include "Generic/Gui/FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"
