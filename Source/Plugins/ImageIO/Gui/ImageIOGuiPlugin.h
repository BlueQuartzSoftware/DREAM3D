#pragma once

#include "ImageIO/ImageIOPlugin.h"

class ImageIOGuiPlugin : public ImageIOPlugin
{
  Q_OBJECT
  Q_INTERFACES(ISIMPLibPlugin)
  Q_PLUGIN_METADATA(IID "net.bluequartz.dream3d.ImageIOGuiPlugin")

public:
  ImageIOGuiPlugin();
   ~ImageIOGuiPlugin() override;
  
  /**
   * @brief Register all the filters with the FilterWidgetFactory
   */
  void registerFilterWidgets(FilterWidgetManager* fwm) override;
  

public:
  ImageIOGuiPlugin(const ImageIOGuiPlugin&) = delete;            // Copy Constructor Not Implemented
  ImageIOGuiPlugin(ImageIOGuiPlugin&&) = delete;                 // Move Constructor
  ImageIOGuiPlugin& operator=(const ImageIOGuiPlugin&) = delete; // Copy Assignment Not Implemented
  ImageIOGuiPlugin& operator=(ImageIOGuiPlugin&&) = delete;      // Move Assignment Not Implemented
};
