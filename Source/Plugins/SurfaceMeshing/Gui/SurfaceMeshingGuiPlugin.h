#pragma once

#include "SurfaceMeshing/SurfaceMeshingPlugin.h"

class SurfaceMeshingGuiPlugin : public SurfaceMeshingPlugin
{
  Q_OBJECT
  Q_INTERFACES(ISIMPLibPlugin)
  Q_PLUGIN_METADATA(IID "net.bluequartz.dream3d.SurfaceMeshingGuiPlugin")

public:
  SurfaceMeshingGuiPlugin();
  ~SurfaceMeshingGuiPlugin() override;

public:
  SurfaceMeshingGuiPlugin(const SurfaceMeshingGuiPlugin&) = delete;            // Copy Constructor Not Implemented
  SurfaceMeshingGuiPlugin(SurfaceMeshingGuiPlugin&&) = delete;                 // Move Constructor
  SurfaceMeshingGuiPlugin& operator=(const SurfaceMeshingGuiPlugin&) = delete; // Copy Assignment Not Implemented
  SurfaceMeshingGuiPlugin& operator=(SurfaceMeshingGuiPlugin&&) = delete;      // Move Assignment Not Implemented
};
