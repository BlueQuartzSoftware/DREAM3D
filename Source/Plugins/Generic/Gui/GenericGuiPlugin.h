#pragma once

#include "Generic/GenericPlugin.h"

class GenericGuiPlugin : public GenericPlugin
{
  Q_OBJECT
  Q_INTERFACES(ISIMPLibPlugin)
  Q_PLUGIN_METADATA(IID "net.bluequartz.dream3d.GenericGuiPlugin")

public:
  GenericGuiPlugin();
   ~GenericGuiPlugin() override;
  
  /**
   * @brief Register all the filters with the FilterWidgetFactory
   */
  void registerFilterWidgets(FilterWidgetManager* fwm) override;
  

public:
  GenericGuiPlugin(const GenericGuiPlugin&) = delete;            // Copy Constructor Not Implemented
  GenericGuiPlugin(GenericGuiPlugin&&) = delete;                 // Move Constructor Not Implemented
  GenericGuiPlugin& operator=(const GenericGuiPlugin&) = delete; // Copy Assignment Not Implemented
  GenericGuiPlugin& operator=(GenericGuiPlugin&&) = delete;      // Move Assignment Not Implemented
};
