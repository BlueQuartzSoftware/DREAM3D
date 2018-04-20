

#include "SurfaceMeshingGuiPlugin.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshingGuiPlugin::SurfaceMeshingGuiPlugin()
: SurfaceMeshingPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshingGuiPlugin::~SurfaceMeshingGuiPlugin() = default;

#include "SurfaceMeshing/Gui/FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"
