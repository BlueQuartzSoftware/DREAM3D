

#include "SamplingGuiPlugin.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SamplingGuiPlugin::SamplingGuiPlugin()
: SamplingPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SamplingGuiPlugin::~SamplingGuiPlugin() = default;

#include "Sampling/Gui/FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"
