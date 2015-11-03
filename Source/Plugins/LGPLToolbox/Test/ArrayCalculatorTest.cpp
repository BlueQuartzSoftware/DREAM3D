/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                               *
 * Copyright (c) 2015 The Regents of the University of California, Author: William Lenthe        *
 *                                                                                               *
 * This program is free software: you can redistribute it and/or modify it under the terms of    *
 * the GNU Lesser General Public License as published by the Free Software Foundation, either    *
 * version 2 of the License, or (at your option) any later version.                              *
 *                                                                                               *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;     *
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.     *
 * See the GNU Lesser General Public License for more details.                                   *
 *                                                                                               *
 * You should have received a copy of the GNU Lesser General Public License                      *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.                         *
 *                                                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"

#include "LGPLToolboxTestFileLocations.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  QFile::remove(UnitTest::ArrayCalculatorTest::TestFile1);
  QFile::remove(UnitTest::ArrayCalculatorTest::TestFile2);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestFilterAvailability()
{
	// Now instantiate the ArrayCalculator Filter from the FilterManager
	QString filtName = "ArrayCalculator";
	FilterManager* fm = FilterManager::Instance();
	IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
	if (NULL == filterFactory.get())
	{
		std::stringstream ss;
		ss << "The ArrayCalculatorTest Requires the use of the " << filtName.toStdString() << " filter which is found in the LGPLToolbox Plugin";
		DREAM3D_TEST_THROW_EXCEPTION(ss.str())
	}
	return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ArrayCalculatorTest()
{
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 /* Please write ArrayCalculator test code here.
  *
  * Your IO test files are:
  * UnitTest::ArrayCalculatorTest::TestFile1
  * UnitTest::ArrayCalculatorTest::TestFile2
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

  int foo = -1;
  DREAM3D_REQUIRE_EQUAL(foo, 0)

  return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void loadFilterPlugins()
{
  // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();
  SIMPLibPluginLoader::LoadPluginFilters(fm);

  // Send progress messages from PipelineBuilder to this object for display
  QMetaObjectUtilities::RegisterMetaTypes();
}


// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  // Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("ArrayCalculatorTest");

  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( loadFilterPlugins() );
  DREAM3D_REGISTER_TEST( TestFilterAvailability() );

  DREAM3D_REGISTER_TEST( ArrayCalculatorTest() )

  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
  PRINT_TEST_SUMMARY();
  return err;
}
