/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "UnitTestSupport.hpp"

#include "SamplingTestFileLocations.h"

class SampleSurfaceMeshSpecifiedPointsTest
{
public:
  SampleSurfaceMeshSpecifiedPointsTest()
  {
  }
  virtual ~SampleSurfaceMeshSpecifiedPointsTest()
  {
  }
  SIMPL_TYPE_MACRO(SampleSurfaceMeshSpecifiedPointsTest)

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::SampleSurfaceMeshSpecifiedPointsTest::TestFile1);
    QFile::remove(UnitTest::SampleSurfaceMeshSpecifiedPointsTest::TestFile2);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the SampleSurfaceMeshSpecifiedPointsTest Filter from the FilterManager
    QString filtName = "SampleSurfaceMeshSpecifiedPoints";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The SampleSurfaceMeshSpecifiedPointsTest Requires the use of the " << filtName.toStdString() << " filter which is found in the Sampling Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int RunTest()
  {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /* Please write SampleSurfaceMeshSpecifiedPointsTest test code here.
 *
 * Your IO test files are:
 * UnitTest::SampleSurfaceMeshSpecifiedPointsTest::TestFile1
 * UnitTest::SampleSurfaceMeshSpecifiedPointsTest::TestFile2
 *
 * DREAM3D provides some macros that will throw exceptions when a test fails
 * and thus report that during testing. These macros are located in the
 * DREAM3DLib/Utilities/UnitTestSupport.hpp file. Some examples are:
 *
 * DREAM3D_REQUIRE_EQUAL(foo, 0)
 * This means that if the variable foo is NOT equal to Zero then test will fail
 * and the current test will exit immediately. If there are more tests registered
 * with the DREAM3D_REGISTER_TEST() macro, the next test will execute. There are
 * lots of examples in the DREAM3D/Test folder to look at.
 */
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    return EXIT_SUCCESS;
  }

  /**
* @brief
*/
  void operator()()
  {
    int err = EXIT_SUCCESS;
    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(RunTest())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

private:
  SampleSurfaceMeshSpecifiedPointsTest(const SampleSurfaceMeshSpecifiedPointsTest&); // Copy Constructor Not Implemented
  void operator=(const SampleSurfaceMeshSpecifiedPointsTest&);                       // Move assignment Not Implemented
};
