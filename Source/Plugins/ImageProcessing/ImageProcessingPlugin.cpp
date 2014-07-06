/*
 * Your License or Copyright Information can go here
 */


#include "ImageProcessingPlugin.h"

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"


#include "ImageProcessing/moc_ImageProcessingPlugin.cpp"

Q_EXPORT_PLUGIN2(ImageProcessingPlugin, ImageProcessingPlugin)

namespace Detail
{
  const QString ImageProcessingPluginFile("ImageProcessingPlugin");
  const QString ImageProcessingPluginDisplayName("ImageProcessingPlugin");
  const QString ImageProcessingPluginBaseName("ImageProcessingPlugin");
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
  return (Detail::ImageProcessingPluginDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageProcessingPlugin::writeSettings(QSettings& prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageProcessingPlugin::readSettings(QSettings& prefs)
{

}

#include "ImageProcessingFilters/RegisterKnownFilters.cpp"

#include "ImageProcessing/FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"

