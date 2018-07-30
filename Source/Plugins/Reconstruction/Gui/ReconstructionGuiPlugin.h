#pragma once

#include "Reconstruction/ReconstructionPlugin.h"

class ReconstructionGuiPlugin : public ReconstructionPlugin
{
  Q_OBJECT
  Q_INTERFACES(ISIMPLibPlugin)
  Q_PLUGIN_METADATA(IID "net.bluequartz.dream3d.ReconstructionGuiPlugin")

public:
  ReconstructionGuiPlugin();
   ~ReconstructionGuiPlugin() override;
  
  /**
   * @brief Register all the filters with the FilterWidgetFactory
   */
  void registerFilterWidgets(FilterWidgetManager* fwm) override;
  

public:
  ReconstructionGuiPlugin(const ReconstructionGuiPlugin&) = delete;            // Copy Constructor Not Implemented
  ReconstructionGuiPlugin(ReconstructionGuiPlugin&&) = delete;                 // Move Constructor Not Implemented
  ReconstructionGuiPlugin& operator=(const ReconstructionGuiPlugin&) = delete; // Copy Assignment Not Implemented
  ReconstructionGuiPlugin& operator=(ReconstructionGuiPlugin&&) = delete;      // Move Assignment Not Implemented
};
