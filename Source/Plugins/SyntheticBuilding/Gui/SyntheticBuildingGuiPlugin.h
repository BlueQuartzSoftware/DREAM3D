#pragma once

#include "SyntheticBuilding/SyntheticBuildingPlugin.h"

class SyntheticBuildingGuiPlugin : public SyntheticBuildingPlugin
{
  Q_OBJECT
  Q_INTERFACES(ISIMPLibPlugin)
  Q_PLUGIN_METADATA(IID "net.bluequartz.dream3d.SyntheticBuildingGuiPlugin")

public:
  SyntheticBuildingGuiPlugin();
   ~SyntheticBuildingGuiPlugin() override;
  
  /**
   * @brief Register all the filters with the FilterWidgetFactory
   */
  void registerFilterWidgets(FilterWidgetManager* fwm) override;
  

public:
  SyntheticBuildingGuiPlugin(const SyntheticBuildingGuiPlugin&) = delete;            // Copy Constructor Not Implemented
  SyntheticBuildingGuiPlugin(SyntheticBuildingGuiPlugin&&) = delete;                 // Move Constructor Not Implemented
  SyntheticBuildingGuiPlugin& operator=(const SyntheticBuildingGuiPlugin&) = delete; // Copy Assignment Not Implemented
  SyntheticBuildingGuiPlugin& operator=(SyntheticBuildingGuiPlugin&&) = delete;      // Move Assignment Not Implemented
};
