// -----------------------------------------------------------------------------
// Insert your license & copyright information here
// -----------------------------------------------------------------------------
#pragma once

#include <QtCore/QFile>

#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"

#include "SIMPLib/Filtering/QMetaObjectUtilities.h"

#include "UnitTestSupport.hpp"

#include "OrientationAnalysisTestFileLocations.h"

class GenerateOrientationMatrixTransposeTest
{

public:
  GenerateOrientationMatrixTransposeTest() = default;
  ~GenerateOrientationMatrixTransposeTest() = default;
  GenerateOrientationMatrixTransposeTest(const GenerateOrientationMatrixTransposeTest&) = delete;            // Copy Constructor
  GenerateOrientationMatrixTransposeTest(GenerateOrientationMatrixTransposeTest&&) = delete;                 // Move Constructor
  GenerateOrientationMatrixTransposeTest& operator=(const GenerateOrientationMatrixTransposeTest&) = delete; // Copy Assignment
  GenerateOrientationMatrixTransposeTest& operator=(GenerateOrientationMatrixTransposeTest&&) = delete;      // Move Assignment

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::GenerateOrientationMatrixTransposeTest::TestFile1);
    QFile::remove(UnitTest::GenerateOrientationMatrixTransposeTest::TestFile2);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the GenerateOrientationMatrixTransposeTest Filter from the FilterManager
    QString filtName = "GenerateOrientationMatrixTranspose";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The GenerateOrientationMatrixTransposeTest Requires the use of the " << filtName.toStdString() << " filter which is found in the OrientationAnalysis Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestGenerateOrientationMatrixTransposeTest()
  {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /* Please write GenerateOrientationMatrixTransposeTest test code here.
     *
     * Your IO test files are:
     * UnitTest::GenerateOrientationMatrixTransposeTest::TestFile1
     * UnitTest::GenerateOrientationMatrixTransposeTest::TestFile2
     *
     * SIMPLib provides some macros that will throw exceptions when a test fails
     * and thus report that during testing. These macros are located in the
     * SIMPLib/Utilities/UnitTestSupport.hpp file. Some examples are:
     *
     * SIMPLib_REQUIRE_EQUAL(foo, 0)
     * This means that if the variable foo is NOT equal to Zero then test will fail
     * and the current test will exit immediately. If there are more tests registered
     * with the SIMPLib_REGISTER_TEST() macro, the next test will execute. There are
     * lots of examples in the SIMPLib/Test folder to look at.
     */
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

    DREAM3D_REGISTER_TEST(TestGenerateOrientationMatrixTransposeTest())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

private:
};
