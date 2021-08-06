#pragma once

#include "StatsToolbox/StatsToolboxPlugin.h"

class StatsToolboxGuiPlugin : public StatsToolboxPlugin
{
  Q_OBJECT
  Q_INTERFACES(ISIMPLibPlugin)
  Q_PLUGIN_METADATA(IID "net.bluequartz.dream3d.StatsToolboxGuiPlugin")

public:
  StatsToolboxGuiPlugin();
  ~StatsToolboxGuiPlugin() override;

  /**
   * @brief Register all the filters with the FilterWidgetFactory
   */
  void registerFilterWidgets(FilterWidgetManager* fwm) override;

public:
  StatsToolboxGuiPlugin(const StatsToolboxGuiPlugin&) = delete;            // Copy Constructor Not Implemented
  StatsToolboxGuiPlugin(StatsToolboxGuiPlugin&&) = delete;                 // Move Constructor Not Implemented
  StatsToolboxGuiPlugin& operator=(const StatsToolboxGuiPlugin&) = delete; // Copy Assignment Not Implemented
  StatsToolboxGuiPlugin& operator=(StatsToolboxGuiPlugin&&) = delete;      // Move Assignment Not Implemented
};
