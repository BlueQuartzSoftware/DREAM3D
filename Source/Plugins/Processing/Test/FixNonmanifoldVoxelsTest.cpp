// -----------------------------------------------------------------------------
// Insert your license & copyright information here
// -----------------------------------------------------------------------------
#pragma once

#include <QtCore/QFile>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"

#include "SIMPLib/Filtering/QMetaObjectUtilities.h"

#include "UnitTestSupport.hpp"

#include "ProcessingTestFileLocations.h"

class FixNonmanifoldVoxelsTest
{

public:
  FixNonmanifoldVoxelsTest()
  {
  }
  virtual ~FixNonmanifoldVoxelsTest()
  {
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::FixNonmanifoldVoxelsTest::TestFile1);
    QFile::remove(UnitTest::FixNonmanifoldVoxelsTest::TestFile2);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the FixNonmanifoldVoxelsTest Filter from the FilterManager
    QString filtName = "FixNonmanifoldVoxels";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The Processing Requires the use of the " << filtName.toStdString() << " filter which is found in the Processing Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFixNonmanifoldVoxelsTest()
  {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /* Please write FixNonmanifoldVoxelsTest test code here.
     *
     * Your IO test files are:
     * UnitTest::FixNonmanifoldVoxelsTest::TestFile1
     * UnitTest::FixNonmanifoldVoxelsTest::TestFile2
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

    DREAM3D_REGISTER_TEST(TestFixNonmanifoldVoxelsTest())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

private:
  FixNonmanifoldVoxelsTest(const FixNonmanifoldVoxelsTest&); // Copy Constructor Not Implemented
  void operator=(const FixNonmanifoldVoxelsTest&);           // Operator '=' Not Implemented
};
