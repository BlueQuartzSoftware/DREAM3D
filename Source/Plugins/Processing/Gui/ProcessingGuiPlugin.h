#pragma once

#include "Processing/ProcessingPlugin.h"

class ProcessingGuiPlugin : public ProcessingPlugin
{
  Q_OBJECT
  Q_INTERFACES(ISIMPLibPlugin)
  Q_PLUGIN_METADATA(IID "net.bluequartz.dream3d.ProcessingGuiPlugin")

public:
  ProcessingGuiPlugin();
   ~ProcessingGuiPlugin() override;
  
  /**
   * @brief Register all the filters with the FilterWidgetFactory
   */
  void registerFilterWidgets(FilterWidgetManager* fwm) override;
  

public:
  ProcessingGuiPlugin(const ProcessingGuiPlugin&) = delete;            // Copy Constructor Not Implemented
  ProcessingGuiPlugin(ProcessingGuiPlugin&&) = delete;                 // Move Constructor Not Implemented
  ProcessingGuiPlugin& operator=(const ProcessingGuiPlugin&) = delete; // Copy Assignment Not Implemented
  ProcessingGuiPlugin& operator=(ProcessingGuiPlugin&&) = delete;      // Move Assignment Not Implemented
};
