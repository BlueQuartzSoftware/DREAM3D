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

#include "Plugins/OrientationAnalysis/OrientationAnalysisFilters/ReadAngData.h"

#include "OrientationAnalysisTestFileLocations.h"

const QString File1(UnitTest::AngCachingTest::TestInputFile1);
const QString File2(UnitTest::AngCachingTest::TestInputFile2);
const QString CopiedFile1(UnitTest::AngCachingTest::Test1);
const QString CopiedFile2(UnitTest::AngCachingTest::Test2);

const float xStep1 = 0.5;
const float yStep1 = 0.5;
const int nCols1 = 20;
const int nRows1 = 10;

const float xStep2 = 0.25;
const float yStep2 = 0.25;
const int nCols2 = 40;
const int nRows2 = 4;

class AngCachingTest
{
  public:
    AngCachingTest(){}
    virtual ~AngCachingTest(){}
    SIMPL_TYPE_MACRO(AngCachingTest)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyTestFiles()
{
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
  // Now instantiate the ReadAngData Filter from the FilterManager
  QString filtName = "ReadAngData";
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
int TestAngReader()
{
  AbstractFilter::Pointer angReader = AbstractFilter::NullPointer();
  QString filtName = "ReadAngData";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
  Ang_Private_Data originalData;

  // Reading first file
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();

    if (NULL != filterFactory.get())
    {
      // If we get this far, the Factory is good so creating the filter should not fail unless something has gone horribly wrong in which case the system is going to come down quickly after this.
      angReader = filterFactory->create();    // Create the reader for the first time
      bool propWasSet = angReader->setProperty("InputFile", CopiedFile1);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      angReader->setDataContainerArray(dca);
      angReader->preflight();
      int err = angReader->getErrorCondition();
      DREAM3D_REQUIRE_EQUAL(err, 0);
    }
    else
    {
      QString ss = QObject::tr("AngCachingTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
      qDebug() << ss;
      DREAM3D_REQUIRE_EQUAL(0, 1)
    }

    Ang_Private_Data data = angReader->property("Data").value<Ang_Private_Data>();

    // XStep, YStep -> Resolution(x, y)
    DREAM3D_REQUIRE_EQUAL(data.resolution[0], xStep1)
    DREAM3D_REQUIRE_EQUAL(data.resolution[1], yStep1)

    // numRows, numCols -> Dimensions (x, y)
    DREAM3D_REQUIRE_EQUAL(data.dims[0], nCols1)
    DREAM3D_REQUIRE_EQUAL(data.dims[1], nRows1)

    // Check that the filter read the file
    bool prop = angReader->property("FileWasRead").toBool();
    DREAM3D_REQUIRE_EQUAL(prop, true)
  }

  // Reading the file again
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();

    if (NULL != filterFactory.get())
    {
      bool propWasSet = angReader->setProperty("InputFile", CopiedFile1);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      angReader->setDataContainerArray(dca);
      angReader->preflight();
      int err = angReader->getErrorCondition();
      DREAM3D_REQUIRE_EQUAL(err, 0);
    }
    else
    {
      QString ss = QObject::tr("AngCachingTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
      qDebug() << ss;
      DREAM3D_REQUIRE_EQUAL(0, 1)
    }

    Ang_Private_Data data = angReader->property("Data").value<Ang_Private_Data>();

    // XStep, YStep -> Resolution(x, y)
    DREAM3D_REQUIRE_EQUAL(data.resolution[0], xStep1)
    DREAM3D_REQUIRE_EQUAL(data.resolution[1], yStep1)

    // numRows, numCols -> Dimensions (x, y)
    DREAM3D_REQUIRE_EQUAL(data.dims[0], nCols1)
    DREAM3D_REQUIRE_EQUAL(data.dims[1], nRows1)

    // Check that the filter read from the cache this time, since we're reading from the same file
    bool prop = angReader->property("FileWasRead").toBool();
    DREAM3D_REQUIRE_EQUAL(prop, false)
  }

  // Reading a different file
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();

    if (NULL != filterFactory.get())
    {
      bool propWasSet = angReader->setProperty("InputFile", CopiedFile2);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      angReader->setDataContainerArray(dca);
      angReader->preflight();
      int err = angReader->getErrorCondition();
      DREAM3D_REQUIRE_EQUAL(err, 0);
    }
    else
    {
      QString ss = QObject::tr("AngCachingTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
      qDebug() << ss;
      DREAM3D_REQUIRE_EQUAL(0, 1)
    }

    Ang_Private_Data data = angReader->property("Data").value<Ang_Private_Data>();

    // XStep, YStep -> Resolution(x, y)
    DREAM3D_REQUIRE_EQUAL(data.resolution[0], xStep2)
    DREAM3D_REQUIRE_EQUAL(data.resolution[1], yStep2)

    // numRows, numCols -> Dimensions (x, y)
    DREAM3D_REQUIRE_EQUAL(data.dims[0], nCols2)
    DREAM3D_REQUIRE_EQUAL(data.dims[1], nRows2)

    // Check that the filter read from the file again, since we changed file names
    bool prop = angReader->property("FileWasRead").toBool();
    DREAM3D_REQUIRE_EQUAL(prop, true)
  }

  // Force the thread to sleep for 1 second for the next test
  QThread::sleep(1);

  // Reading the same file, but the contents changed outside the program
  {
    // Change the contents of the file to be read
    {
      QFile file(CopiedFile2);
      if (!file.open(QFile::ReadOnly | QFile::Text))
      {
        DREAM3D_REQUIRE_EQUAL(0, 1)
      }
      QString contents = file.readAll();
      file.close();
      // Append a line of data to the end of the file
      QTextStream in(&contents);
      in << "4.18148   1.72461   4.03519      0.00000      0.00000   40.8  0.229  1  -1904  1.512 ";

      if (!file.open(QFile::WriteOnly | QFile::Text))
      {
        DREAM3D_REQUIRE_EQUAL(0, 1)
      }

      QTextStream out( &file );
      out << contents;
      file.close();
    }

    DataContainerArray::Pointer dca = DataContainerArray::New();

    if (NULL != filterFactory.get())
    {
      bool propWasSet = angReader->setProperty("InputFile", CopiedFile2);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      angReader->setDataContainerArray(dca);
      angReader->preflight();
      int err = angReader->getErrorCondition();
      DREAM3D_REQUIRE_EQUAL(err, 0);
    }
    else
    {
      QString ss = QObject::tr("AngCachingTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
      qDebug() << ss;
      DREAM3D_REQUIRE_EQUAL(0, 1)
    }

    Ang_Private_Data data = angReader->property("Data").value<Ang_Private_Data>();

    // XStep, YStep -> Resolution(x, y)
    DREAM3D_REQUIRE_EQUAL(data.resolution[0], xStep2)
    DREAM3D_REQUIRE_EQUAL(data.resolution[1], yStep2)

    // numRows, numCols -> Dimensions (x, y)
    DREAM3D_REQUIRE_EQUAL(data.dims[0], nCols2)
    DREAM3D_REQUIRE_EQUAL(data.dims[1], nRows2)

    // Check that the filter read from the file again, since we changed the contents of the file outside the program
    bool prop = angReader->property("FileWasRead").toBool();
    DREAM3D_REQUIRE_EQUAL(prop, true)
  }

  // Reading the same file, but we are testing the cache flush function
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Flush the cache by invoking the flushCache method dynamically, using Qt's meta system
    if (QMetaObject::invokeMethod(angReader.get(), "flushCache", Qt::DirectConnection) == false)
      DREAM3D_REQUIRE_EQUAL(0, 1)

      if (NULL != filterFactory.get())
      {
        bool propWasSet = angReader->setProperty("InputFile", CopiedFile2);
        DREAM3D_REQUIRE_EQUAL(propWasSet, true)
        angReader->setDataContainerArray(dca);
        angReader->preflight();
        int err = angReader->getErrorCondition();
        DREAM3D_REQUIRE_EQUAL(err, 0);
      }
      else
      {
        QString ss = QObject::tr("AngCacheTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
        qDebug() << ss;
        DREAM3D_REQUIRE_EQUAL(0, 1)
      }

    Ang_Private_Data data = angReader->property("Data").value<Ang_Private_Data>();

    // XStep, YStep -> Resolution(x, y)
    DREAM3D_REQUIRE_EQUAL(data.resolution[0], xStep2)
    DREAM3D_REQUIRE_EQUAL(data.resolution[1], yStep2)

    // numRows, numCols -> Dimensions (x, y)
    DREAM3D_REQUIRE_EQUAL(data.dims[0], nCols2)
    DREAM3D_REQUIRE_EQUAL(data.dims[1], nRows2)

    // Check that the filter read from the file again, since we flushed the cache
    bool prop = angReader->property("FileWasRead").toBool();
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

  DREAM3D_REGISTER_TEST(CopyTestFiles())

  DREAM3D_REGISTER_TEST(TestAngReader())

  DREAM3D_REGISTER_TEST(RemoveTestFiles())
}
private:
AngCachingTest(const AngCachingTest&); // Copy Constructor Not Implemented
void operator=(const AngCachingTest&); // Operator '=' Not Implemented
};
