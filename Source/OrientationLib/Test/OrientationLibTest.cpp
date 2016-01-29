
#include "TestPrintFunctions.h"
#include "GenerateFunctionList.h"
#include "TextureTest.cpp"
#include "ODFTest.cpp"
#include "OrientationArrayTest.cpp"
#include "OrientationConverterTest.cpp"
#include "IPFLegendTest.cpp"
#include "SO3SamplerTest.cpp"
#include "OrientationTransformsTest.cpp"



// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  int err = EXIT_SUCCESS;

  // Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("CombineAttributeArraysTest");
  // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();
  SIMPLibPluginLoader::LoadPluginFilters(fm);

  // Send progress messages from PipelineBuilder to this object for display
  QMetaObjectUtilities::RegisterMetaTypes();




  TextureTest()();
  ODFTest()();
  OrientationArrayTest()();
  OrientationConverterTest()();
  IPFLegendTest()();
  SO3SamplerTest()();
  OrientationTransformsTest()();

  return err;
}
