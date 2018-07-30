#pragma once

#include "Statistics/StatisticsPlugin.h"

class StatisticsGuiPlugin : public StatisticsPlugin
{
  Q_OBJECT
  Q_INTERFACES(ISIMPLibPlugin)
  Q_PLUGIN_METADATA(IID "net.bluequartz.dream3d.StatisticsGuiPlugin")

public:
  StatisticsGuiPlugin();
   ~StatisticsGuiPlugin() override;
  
  /**
   * @brief Register all the filters with the FilterWidgetFactory
   */
  void registerFilterWidgets(FilterWidgetManager* fwm) override;
  

public:
  StatisticsGuiPlugin(const StatisticsGuiPlugin&) = delete;            // Copy Constructor Not Implemented
  StatisticsGuiPlugin(StatisticsGuiPlugin&&) = delete;                 // Move Constructor Not Implemented
  StatisticsGuiPlugin& operator=(const StatisticsGuiPlugin&) = delete; // Copy Assignment Not Implemented
  StatisticsGuiPlugin& operator=(StatisticsGuiPlugin&&) = delete;      // Move Assignment Not Implemented
};
