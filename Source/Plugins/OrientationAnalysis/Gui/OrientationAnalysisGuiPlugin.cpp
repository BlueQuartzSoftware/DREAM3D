

#include "OrientationAnalysisGuiPlugin.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationAnalysisGuiPlugin::OrientationAnalysisGuiPlugin()
: OrientationAnalysisPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationAnalysisGuiPlugin::~OrientationAnalysisGuiPlugin() = default;

#include "OrientationAnalysis/Gui/FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"
