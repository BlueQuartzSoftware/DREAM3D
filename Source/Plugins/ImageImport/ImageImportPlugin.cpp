/*
 * Your License or Copyright Information can go here
 */
#include "ImageImportPlugin.h"

#include "PipelineBuilder/FilterWidgetManager.h"
#include "FilterWidgets/QFilterWidgetFactory.hpp"

#include "moc_ImageImportPlugin.cxx"

Q_EXPORT_PLUGIN2(ImageImportPlugin, ImageImportPlugin)

namespace Detail
{
   const std::string ImageImportPluginFile("ImageImportPlugin");
   const std::string ImageImportPluginDisplayName("ImageImportPlugin");
   const std::string ImageImportPluginBaseName("ImageImportPlugin");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageImportPlugin::ImageImportPlugin()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageImportPlugin::~ImageImportPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImageImportPlugin::getPluginName()
{
  return QString::fromStdString(Detail::ImageImportPluginDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageImportPlugin::writeSettings(QSettings &prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageImportPlugin::readSettings(QSettings &prefs)
{

}

#include "ImageImportFilters/RegisterKnownFilterWidgets.cpp"

