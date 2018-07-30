#pragma once

#include "OrientationAnalysis/OrientationAnalysisPlugin.h"

class OrientationAnalysisGuiPlugin : public OrientationAnalysisPlugin
{
  Q_OBJECT
  Q_INTERFACES(ISIMPLibPlugin)
  Q_PLUGIN_METADATA(IID "net.bluequartz.dream3d.OrientationAnalysisGuiPlugin")

public:
  OrientationAnalysisGuiPlugin();
   ~OrientationAnalysisGuiPlugin() override;
  
  /**
   * @brief Register all the filters with the FilterWidgetFactory
   */
  void registerFilterWidgets(FilterWidgetManager* fwm) override;
  

public:
  OrientationAnalysisGuiPlugin(const OrientationAnalysisGuiPlugin&) = delete;            // Copy Constructor Not Implemented
  OrientationAnalysisGuiPlugin(OrientationAnalysisGuiPlugin&&) = delete;                 // Move Constructor Not Implemented
  OrientationAnalysisGuiPlugin& operator=(const OrientationAnalysisGuiPlugin&) = delete; // Copy Assignment Not Implemented
  OrientationAnalysisGuiPlugin& operator=(OrientationAnalysisGuiPlugin&&) = delete;      // Move Assignment Not Implemented
};
