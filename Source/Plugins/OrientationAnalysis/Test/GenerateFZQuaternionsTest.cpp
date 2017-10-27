// -----------------------------------------------------------------------------
// Insert your license & copyright information here
// -----------------------------------------------------------------------------

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"
#include "UnitTestSupport.hpp"

#include "OrientationAnalysisTestFileLocations.h"

class GenerateFZQuaternionsTest
{

public:
  GenerateFZQuaternionsTest()
  {
  }
  virtual ~GenerateFZQuaternionsTest()
  {
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::GenerateFZQuaternionsTest::TestFile1);
    QFile::remove(UnitTest::GenerateFZQuaternionsTest::TestFile2);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the GenerateFZQuaternionsTest Filter from the FilterManager
    QString filtName = "GenerateFZQuaternions";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The GenerateFZQuaternionsTest Requires the use of the " << filtName.toStdString() << " filter which is found in the OrientationAnalysis Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestGenerateFZQuaternionsTest()
  {
    int foo = 0;
    DREAM3D_REQUIRE_EQUAL(foo, 0)

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(TestGenerateFZQuaternionsTest())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

private:
  GenerateFZQuaternionsTest(const GenerateFZQuaternionsTest&); // Copy Constructor Not Implemented
  void operator=(const GenerateFZQuaternionsTest&);            // Operator '=' Not Implemented
};
