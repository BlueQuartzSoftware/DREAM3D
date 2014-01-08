/*
 * Your License or Copyright Information can go here
 */

#include "TestWidgetsPlugin.h"

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"

//#include "PipelineBuilder/FilterWidgetManager.h"
//#include "FilterWidgets/QFilterWidgetFactory.hpp"

#include "moc_TestWidgetsPlugin.cxx"

Q_EXPORT_PLUGIN2(TestWidgetsPlugin, TestWidgetsPlugin)

namespace Detail
{
  const QString TestWidgetsPluginFile("TestWidgetsPlugin");
  const QString TestWidgetsPluginDisplayName("TestWidgetsPlugin");
  const QString TestWidgetsPluginBaseName("TestWidgetsPlugin");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TestWidgetsPlugin::TestWidgetsPlugin()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TestWidgetsPlugin::~TestWidgetsPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString TestWidgetsPlugin::getPluginName()
{
  return (Detail::TestWidgetsPluginDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestWidgetsPlugin::writeSettings(QSettings& prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestWidgetsPlugin::readSettings(QSettings& prefs)
{

}

//#include "TestWidgetsFilters/RegisterKnownFilterWidgets.cpp"
#include "TestWidgetsFilters/RegisterKnownFilters.cpp"

