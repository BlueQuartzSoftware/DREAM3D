

#include "ImageIOGuiPlugin.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageIOGuiPlugin::ImageIOGuiPlugin()
: ImageIOPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageIOGuiPlugin::~ImageIOGuiPlugin() = default;

#include "ImageIO/Gui/FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"
