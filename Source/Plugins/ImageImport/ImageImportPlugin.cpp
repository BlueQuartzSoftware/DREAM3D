/*
 * Your License or Copyright Information can go here
 */

#include "ImageImportPlugin.h"

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"

//#include "PipelineBuilder/FilterWidgetManager.h"
//#include "FilterWidgets/PipelineFilterWidgetFactory.hpp"

#include "moc_ImageImportPlugin.cxx"

Q_EXPORT_PLUGIN2(ImageImportPlugin, ImageImportPlugin)

namespace Detail
{
  const QString ImageImportPluginFile("ImageImportPlugin");
  const QString ImageImportPluginDisplayName("ImageImportPlugin");
  const QString ImageImportPluginBaseName("ImageImportPlugin");
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
  return (Detail::ImageImportPluginDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageImportPlugin::writeSettings(QSettings& prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageImportPlugin::readSettings(QSettings& prefs)
{

}

//#include "ImageImportPluginFilters/RegisterKnownFilterWidgets.cpp"
#include "ImageImportFilters/RegisterKnownFilters.cpp"
