

#include "ImportExportGuiPlugin.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportExportGuiPlugin::ImportExportGuiPlugin()
: ImportExportPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportExportGuiPlugin::~ImportExportGuiPlugin() = default;

#include "ImportExport/Gui/FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"
