#pragma once

#include "IO/IOPlugin.h"

class IOGuiPlugin : public IOPlugin
{
  Q_OBJECT
  Q_INTERFACES(ISIMPLibPlugin)
  Q_PLUGIN_METADATA(IID "net.bluequartz.dream3d.IOGuiPlugin")

public:
  IOGuiPlugin();
   ~IOGuiPlugin() override;
  
  /**
   * @brief Register all the filters with the FilterWidgetFactory
   */
  void registerFilterWidgets(FilterWidgetManager* fwm) override;
  

public:
  IOGuiPlugin(const IOGuiPlugin&) = delete;            // Copy Constructor Not Implemented
  IOGuiPlugin(IOGuiPlugin&&) = delete;                 // Move Constructor Not Implemented
  IOGuiPlugin& operator=(const IOGuiPlugin&) = delete; // Copy Assignment Not Implemented
  IOGuiPlugin& operator=(IOGuiPlugin&&) = delete;      // Move Assignment Not Implemented
};
