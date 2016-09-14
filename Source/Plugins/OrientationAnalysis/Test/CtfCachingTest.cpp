/* ============================================================================
 * Copyright (c) 2015 BlueQuartz Softwae, LLC
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QThread>

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

#include "OrientationAnalysis/OrientationAnalysisFilters/ReadCtfData.h"

#include "OrientationAnalysisTestFileLocations.h"



class CtfCachingTest
{
  private:

     QString File1;
     QString File2;
     QString CopiedFile1;
     QString CopiedFile2;

     int numPhases1;
     int numPhases2;

  public:
    CtfCachingTest()
    {
      File1 = UnitTest::CtfCachingTest::TestInputFile1;
      File2 = UnitTest::CtfCachingTest::TestInputFile2;
      CopiedFile1 = UnitTest::CtfCachingTest::Test1;
      CopiedFile2 = UnitTest::CtfCachingTest::Test2;
      numPhases1 = 1;
      numPhases2 = 5;
    }
    virtual ~CtfCachingTest(){}
    SIMPL_TYPE_MACRO(CtfCachingTest)

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void CopyTestFiles()
    {
      // Copy test files to Build directory to manipulate
      QFile::copy(File1, CopiedFile1);
      QFile::copy(File2, CopiedFile2);
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void RemoveTestFiles()
    {
#if REMOVE_TEST_FILES
      QFile::remove(CopiedFile1);
      QFile::remove(CopiedFile2);
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
      if (nullptr == filterFactory.get())
      {
        std::stringstream ss;
        ss << "The CtfCachingTest requires the use of the " << filtName.toStdString() << " filter which is found in the OrientationAnalysis Plugin";
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

        if (nullptr != filterFactory.get())
        {
          // If we get this far, the Factory is good so creating the filter should not fail unless something has gone horribly wrong in which case the system is going to come down quickly after this.
          ctfReader = filterFactory->create();    // Create the reader for the first time
          bool propWasSet = ctfReader->setProperty("InputFile", CopiedFile1);
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

        Ctf_Private_Data data = ctfReader->property("Data").value<Ctf_Private_Data>();
        DREAM3D_REQUIRE_EQUAL(data.phases.size(), numPhases1)

            // Check that the filter read the file
            bool prop = ctfReader->property("FileWasRead").toBool();
        DREAM3D_REQUIRE_EQUAL(prop, true)
      }

      // Reading the file again
      {
        DataContainerArray::Pointer dca = DataContainerArray::New();

        if (nullptr != filterFactory.get())
        {
          bool propWasSet = ctfReader->setProperty("InputFile", CopiedFile1);
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

        Ctf_Private_Data data = ctfReader->property("Data").value<Ctf_Private_Data>();
        DREAM3D_REQUIRE_EQUAL(data.phases.size(), numPhases1)

            // Check that the filter read from the cache this time, since we're reading from the same file
            bool prop = ctfReader->property("FileWasRead").toBool();
        DREAM3D_REQUIRE_EQUAL(prop, false)
      }

      // Reading a different file
      {
        DataContainerArray::Pointer dca = DataContainerArray::New();

        if (nullptr != filterFactory.get())
        {
          bool propWasSet = ctfReader->setProperty("InputFile", CopiedFile2);
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

        Ctf_Private_Data data = ctfReader->property("Data").value<Ctf_Private_Data>();
        DREAM3D_REQUIRE_EQUAL(data.phases.size(), numPhases2)

            // Check that the filter read from the file again, since we changed file names
            bool prop = ctfReader->property("FileWasRead").toBool();
        DREAM3D_REQUIRE_EQUAL(prop, true)
      }

      // Force the thread to sleep for 1 second for the next test
      QThread::sleep(1);

      // Reading the same file, but the contents changed outside the program
      {
        // Change the contents of the file to be read
        {
          QFile file(CopiedFile2);
          if (!file.open(QFile::ReadWrite | QFile::Text))
            DREAM3D_REQUIRE_EQUAL(0, 1)
                QTextStream out(&file);
          out << "This test string should force the filter to read from the file instead of the cache.";
          file.close();
        }

        DataContainerArray::Pointer dca = DataContainerArray::New();

        if (nullptr != filterFactory.get())
        {
          bool propWasSet = ctfReader->setProperty("InputFile", CopiedFile2);
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

        Ctf_Private_Data data = ctfReader->property("Data").value<Ctf_Private_Data>();
        DREAM3D_REQUIRE_EQUAL(data.phases.size(), numPhases2)

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

              if (nullptr != filterFactory.get())
          {
            bool propWasSet = ctfReader->setProperty("InputFile", CopiedFile2);
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

        Ctf_Private_Data data = ctfReader->property("Data").value<Ctf_Private_Data>();
        DREAM3D_REQUIRE_EQUAL(data.phases.size(), numPhases2)

            // Check that the filter read from the file again, since we flushed the cache
            bool prop = ctfReader->property("FileWasRead").toBool();
        DREAM3D_REQUIRE_EQUAL(prop, true)
      }

      return EXIT_SUCCESS;
    }
    /**
  * @brief
*/
    void operator()()
    {
      int err = EXIT_SUCCESS;
      DREAM3D_REGISTER_TEST(TestFilterAvailability());

      DREAM3D_REGISTER_TEST(RemoveTestFiles())
          DREAM3D_REGISTER_TEST(CopyTestFiles())

          DREAM3D_REGISTER_TEST(TestCtfReader())

          DREAM3D_REGISTER_TEST(RemoveTestFiles())
    }
  private:
    CtfCachingTest(const CtfCachingTest&); // Copy Constructor Not Implemented
    void operator=(const CtfCachingTest&); // Operator '=' Not Implemented
};
