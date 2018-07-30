#pragma once

#include "Sampling/SamplingPlugin.h"

class SamplingGuiPlugin : public SamplingPlugin
{
  Q_OBJECT
  Q_INTERFACES(ISIMPLibPlugin)
  Q_PLUGIN_METADATA(IID "net.bluequartz.dream3d.SamplingGuiPlugin")

public:
  SamplingGuiPlugin();
   ~SamplingGuiPlugin() override;
  
  /**
   * @brief Register all the filters with the FilterWidgetFactory
   */
  void registerFilterWidgets(FilterWidgetManager* fwm) override;
  

public:
  SamplingGuiPlugin(const SamplingGuiPlugin&) = delete;            // Copy Constructor Not Implemented
  SamplingGuiPlugin(SamplingGuiPlugin&&) = delete;                 // Move Constructor Not Implemented
  SamplingGuiPlugin& operator=(const SamplingGuiPlugin&) = delete; // Copy Assignment Not Implemented
  SamplingGuiPlugin& operator=(SamplingGuiPlugin&&) = delete;      // Move Assignment Not Implemented
};
