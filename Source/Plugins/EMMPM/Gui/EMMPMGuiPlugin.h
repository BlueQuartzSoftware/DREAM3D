#pragma once

#include "EMMPM/EMMPMPlugin.h"

class EMMPMGuiPlugin : public EMMPMPlugin
{
  Q_OBJECT
  Q_INTERFACES(ISIMPLibPlugin)
  Q_PLUGIN_METADATA(IID "net.bluequartz.dream3d.EMMPMGuiPlugin")

public:
  EMMPMGuiPlugin();
   ~EMMPMGuiPlugin() override;
  
  /**
   * @brief Register all the filters with the FilterWidgetFactory
   */
  void registerFilterWidgets(FilterWidgetManager* fwm) override;
  

public:
  EMMPMGuiPlugin(const EMMPMGuiPlugin&) = delete;            // Copy Constructor Not Implemented
  EMMPMGuiPlugin(EMMPMGuiPlugin&&) = delete;                 // Move Constructor Not Implemented
  EMMPMGuiPlugin& operator=(const EMMPMGuiPlugin&) = delete; // Copy Assignment Not Implemented
  EMMPMGuiPlugin& operator=(EMMPMGuiPlugin&&) = delete;      // Move Assignment Not Implemented
};
