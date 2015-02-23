/* ============================================================================
* Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
* Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
* All rights reserved.
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
* Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
* BlueQuartz Software nor the names of its contributors may be used to endorse
* or promote products derived from this software without specific prior written
* permission.
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
*  This code was written under United States Air Force Contract number
*                           FA8650-07-D-5800
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/Common/FilterPipeline.h"
#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/FilterFactory.hpp"
#include "DREAM3DLib/Plugin/IDREAM3DPlugin.h"
#include "DREAM3DLib/Plugin/DREAM3DPluginLoader.h"
#include "DREAM3DLib/Utilities/UnitTestSupport.hpp"
#include "DREAM3DLib/Utilities/QMetaObjectUtilities.h"

#include "Plugins/OrientationAnalysis/OrientationAnalysisFilters/ReadCtfData.h"

#include "TestFileLocations.h"

const QString InputFile1(UnitTest::CtfReaderTest::EuropeanInputFile1);
const QString InputFile2(UnitTest::CtfReaderTest::EuropeanInputFile2);
const QString OutputFile1(UnitTest::CtfCachingTest::TestFile1);
const QString OutputFile2(UnitTest::CtfCachingTest::TestFile2);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
	QFile::remove(OutputFile1);
	QFile::remove(OutputFile2);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestFilterAvailability()
{
	// Now instantiate the ReadCtfData Filter from the FilterManager
	QString filtName = "ReadCtfData";
	FilterManager* fm = FilterManager::Instance();
	IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
	if (NULL == filterFactory.get())
	{
		std::stringstream ss;
		ss << "The AngCachingTest Requires the use of the " << filtName.toStdString() << " filter which is found in the OrientationAnalysis Plugin";
		DREAM3D_TEST_THROW_EXCEPTION(ss.str())
	}
	return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestCtfReader()
{
	AbstractFilter::Pointer ctfReader = AbstractFilter::NullPointer();
	QString filtName = "ReadCtfData";
	FilterManager* fm = FilterManager::Instance();
	IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);

	// Reading first file
	{
		DataContainerArray::Pointer dca = DataContainerArray::New();

		if (NULL != filterFactory.get())
		{
			// If we get this far, the Factory is good so creating the filter should not fail unless something has gone horribly wrong in which case the system is going to come down quickly after this.
			ctfReader = filterFactory->create();		// Create the reader for the first time
			bool propWasSet = ctfReader->setProperty("InputFile", InputFile1);
			DREAM3D_REQUIRE_EQUAL(propWasSet, true)
				ctfReader->setDataContainerArray(dca);
			ctfReader->preflight();
			int err = ctfReader->getErrorCondition();
			DREAM3D_REQUIRE_EQUAL(err, 0);
		}
		else
		{
			QString ss = QObject::tr("CtfCachingTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
			qDebug() << ss;
			DREAM3D_REQUIRE_EQUAL(0, 1)
		}

		// Check that the filter read the file
		bool prop = ctfReader->property("FileWasRead").toBool();
		DREAM3D_REQUIRE_EQUAL(prop, true)
	}

	// Reading the file again
	{
		DataContainerArray::Pointer dca = DataContainerArray::New();

		if (NULL != filterFactory.get())
		{
			bool propWasSet = ctfReader->setProperty("InputFile", InputFile1);
			DREAM3D_REQUIRE_EQUAL(propWasSet, true)
				ctfReader->setDataContainerArray(dca);
			ctfReader->preflight();
			int err = ctfReader->getErrorCondition();
			DREAM3D_REQUIRE_EQUAL(err, 0);
		}
		else
		{
			QString ss = QObject::tr("CtfCachingTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
			qDebug() << ss;
			DREAM3D_REQUIRE_EQUAL(0, 1)
		}

		// Check that the filter read from the cache this time, since we're reading from the same file
		bool prop = ctfReader->property("FileWasRead").toBool();
		DREAM3D_REQUIRE_EQUAL(prop, false)
	}

	// Reading a different file
	{
		DataContainerArray::Pointer dca = DataContainerArray::New();

		if (NULL != filterFactory.get())
		{
			bool propWasSet = ctfReader->setProperty("InputFile", InputFile2);
			DREAM3D_REQUIRE_EQUAL(propWasSet, true)
				ctfReader->setDataContainerArray(dca);
			ctfReader->preflight();
			int err = ctfReader->getErrorCondition();
			DREAM3D_REQUIRE_EQUAL(err, 0);
		}
		else
		{
			QString ss = QObject::tr("CtfCachingTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
			qDebug() << ss;
			DREAM3D_REQUIRE_EQUAL(0, 1)
		}

		// Check that the filter read from the file again, since we changed file names
		bool prop = ctfReader->property("FileWasRead").toBool();
		DREAM3D_REQUIRE_EQUAL(prop, true)
	}

	// Reading the same file, but the contents changed outside the program
	{
		// Change the contents of the file to be read
		{
			QFile file(InputFile2);
			if (!file.open(QFile::ReadWrite | QFile::Text))
				DREAM3D_REQUIRE_EQUAL(0, 1)
				QTextStream out(&file);
			out << "This test string should force the filter to read from the file instead of the cache.";
			file.close();
		}

		DataContainerArray::Pointer dca = DataContainerArray::New();

		if (NULL != filterFactory.get())
		{
			bool propWasSet = ctfReader->setProperty("InputFile", InputFile2);
			DREAM3D_REQUIRE_EQUAL(propWasSet, true)
				ctfReader->setDataContainerArray(dca);
			ctfReader->preflight();
			int err = ctfReader->getErrorCondition();
			DREAM3D_REQUIRE_EQUAL(err, 0);
		}
		else
		{
			QString ss = QObject::tr("CtfCachingTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
			qDebug() << ss;
			DREAM3D_REQUIRE_EQUAL(0, 1)
		}

		// Check that the filter read from the file again, since we changed the contents of the file outside the program
		bool prop = ctfReader->property("FileWasRead").toBool();
		DREAM3D_REQUIRE_EQUAL(prop, true)
	}

	// Reading the same file, but we are testing the cache flush function
	{
		DataContainerArray::Pointer dca = DataContainerArray::New();

		// Flush the cache by invoking the flushCache method dynamically, using Qt's meta system
		if (QMetaObject::invokeMethod(ctfReader.get(), "flushCache", Qt::DirectConnection) == false)
			DREAM3D_REQUIRE_EQUAL(0, 1)

			if (NULL != filterFactory.get())
			{
				bool propWasSet = ctfReader->setProperty("InputFile", InputFile2);
				DREAM3D_REQUIRE_EQUAL(propWasSet, true)
					ctfReader->setDataContainerArray(dca);
				ctfReader->preflight();
				int err = ctfReader->getErrorCondition();
				DREAM3D_REQUIRE_EQUAL(err, 0);
			}
			else
			{
				QString ss = QObject::tr("CtfCacheTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
				qDebug() << ss;
				DREAM3D_REQUIRE_EQUAL(0, 1)
			}

		// Check that the filter read from the file again, since we flushed the cache
		bool prop = ctfReader->property("FileWasRead").toBool();
		DREAM3D_REQUIRE_EQUAL(prop, true)
	}

	return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void loadFilterPlugins()
{
	// Register all the filters including trying to load those from Plugins
	FilterManager* fm = FilterManager::Instance();
	DREAM3DPluginLoader::LoadPluginFilters(fm);

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
	QCoreApplication::setApplicationName("CtfCachingTest");

	int err = EXIT_SUCCESS;
	DREAM3D_REGISTER_TEST(loadFilterPlugins());
	DREAM3D_REGISTER_TEST(TestFilterAvailability());

	DREAM3D_REGISTER_TEST(TestCtfReader())

		DREAM3D_REGISTER_TEST(RemoveTestFiles())
		PRINT_TEST_SUMMARY();
	return err;
}

