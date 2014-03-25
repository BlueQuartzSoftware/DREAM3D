/*
 * Your License or Copyright Information can go here
 */

#include "ImageProcessingPlugin.h"

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"

#include "PipelineBuilder/FilterWidgetManager.h"
#include "FilterWidgets/QFilterWidgetFactory.hpp"

#include "moc_ImageProcessingPlugin.cxx"

Q_EXPORT_PLUGIN2(ImageProcessingPlugin, ImageProcessingPlugin)

namespace Detail
{
   const std::string ImageProcessingPluginFile("ImageProcessingPlugin");
   const std::string ImageProcessingPluginDisplayName("ImageProcessingPlugin");
   const std::string ImageProcessingPluginBaseName("ImageProcessingPlugin");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageProcessingPlugin::ImageProcessingPlugin()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageProcessingPlugin::~ImageProcessingPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImageProcessingPlugin::getPluginName()
{
  return QString::fromStdString(Detail::ImageProcessingPluginDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageProcessingPlugin::writeSettings(QSettings &prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageProcessingPlugin::readSettings(QSettings &prefs)
{

}

#include "ImageProcessingFilters/RegisterKnownFilterWidgets.cpp"
#include "ImageProcessingFilters/RegisterKnownFilters.cpp"

