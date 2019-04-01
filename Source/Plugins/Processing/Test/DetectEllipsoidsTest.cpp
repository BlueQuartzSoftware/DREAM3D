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
 * SERVICES; LOSS OF USE, Data, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
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

#include "SIMPLib/Common/Observer.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "UnitTestSupport.hpp"

#include "ProcessingTestFileLocations.h"

class DetectEllipsoidsTest
{

public:
  DetectEllipsoidsTest()
  {
  }
  virtual ~DetectEllipsoidsTest()
  {
  }

  QFile testOutFile;
  QFile exemplaryOutFile;

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::DetectEllipsoidsTest::TestOutputPath);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the DetectEllipsoidsTest Filter from the FilterManager
    QString filtName = "DetectEllipsoids";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The FiberToolbox Requires the use of the " << filtName.toStdString() << " filter which is found in the FiberToolbox Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestDetectEllipsoids()
  {
    testOutFile.setFileName(UnitTest::DetectEllipsoidsTest::TestOutputPath);
    exemplaryOutFile.setFileName(UnitTest::DetectEllipsoidsTest::EllipseOutputPath);

    JsonFilterParametersReader::Pointer reader = JsonFilterParametersReader::New();
    FilterPipeline::Pointer pipeline = reader->readPipelineFromFile(UnitTest::DetectEllipsoidsTest::TestPipelinePath);
    DREAM3D_REQUIRE_VALID_POINTER(pipeline.get());

    FilterPipeline::FilterContainerType container = pipeline->getFilterContainer();
    DREAM3D_REQUIRE_EQUAL(container.size(), 7); // There should be 7 filters... if not, bail

    AbstractFilter::Pointer readerFilter = container[0];

    QVariant var;
    QString propName = "FileName";
    var.setValue(UnitTest::DetectEllipsoidsTest::InputSegmentationPath);
    bool propWasSet = readerFilter->setProperty(propName.toStdString().c_str(), var);
    if(false == propWasSet)
    {
      qDebug() << "Unable to set property '" + propName + "' in filter " << readerFilter->getNameOfClass();
      DREAM3D_REQUIRE_EQUAL(0, 1);
    }

    AbstractFilter::Pointer writerFilter = container[container.size() - 1];

    propName = "OutputPath";
    var.setValue(UnitTest::DetectEllipsoidsTest::TestOutputDirectory);
    propWasSet = writerFilter->setProperty(propName.toStdString().c_str(), var);
    if(false == propWasSet)
    {
      qDebug() << "Unable to set property '" + propName + "' in filter " << writerFilter->getNameOfClass();
      DREAM3D_REQUIRE_EQUAL(0, 1);
    }

    AbstractFilter::Pointer detectEllipsoidsFilter = container[container.size() - 2];

    Observer obs;
    obs.connect(detectEllipsoidsFilter.get(), SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), &obs, SLOT(processPipelineMessage(const AbstractMessage::Pointer&)));

    pipeline->preflightPipeline();
    DREAM3D_REQUIRE_EQUAL(pipeline->getErrorCode(), 0);

    pipeline->execute();
    DREAM3D_REQUIRE_EQUAL(pipeline->getErrorCode(), 0);

    // Compare output with exemplary output
    if(exemplaryOutFile.open(QFile::ReadOnly) == false || testOutFile.open(QFile::ReadOnly) == false)
    {
      // Bail
      DREAM3D_REQUIRE_EQUAL(0, 1);
    }

    QTextStream testIn(&testOutFile);
    QTextStream exemplaryIn(&exemplaryOutFile);

    while(!testIn.atEnd() && !exemplaryIn.atEnd())
    {
      QString exemplaryLine = exemplaryIn.readLine();
      QString testLine = testIn.readLine();

      QTextStream::Status testStatus = testIn.status();
      QTextStream::Status exemplaryStatus = exemplaryIn.status();
      if(testStatus != QTextStream::Ok || exemplaryStatus != QTextStream::Ok)
      {
        // Bail
        DREAM3D_REQUIRE_EQUAL(0, 1);
      }

      QStringList exemplaryStrList = exemplaryLine.split('\t');
      QStringList testStrList = testLine.split('\t');

      for(int i = 0; i < exemplaryStrList.size(); i++)
      {
        QString exemplaryStr = exemplaryStrList[i];
        QString testStr = testStrList[i];

        bool ok = true;
        int exemplaryInt = exemplaryStr.toInt(&ok);
        if(ok == false)
        {
          // Bail
          DREAM3D_REQUIRE_EQUAL(0, 1);
        }

        int testInt = testStr.toInt(&ok);
        if(ok == false)
        {
          // Bail
          DREAM3D_REQUIRE_EQUAL(0, 1);
        }

        if(exemplaryInt == 0)
        {
          DREAM3D_REQUIRE_EQUAL(exemplaryInt, testInt);
        }
        else if(exemplaryInt > 0)
        {
          DREAM3D_REQUIRE(exemplaryInt > 0);
          DREAM3D_REQUIRE(testInt > 0);
        }
        else
        {
          // Bail
          DREAM3D_REQUIRE_EQUAL(0, 1);
        }
      }
    }

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(TestDetectEllipsoids());

    if(testOutFile.isOpen())
    {
      testOutFile.close();
    }
    if(exemplaryOutFile.isOpen())
    {
      exemplaryOutFile.close();
    }

    DREAM3D_REGISTER_TEST(RemoveTestFiles());
  }

private:
  DetectEllipsoidsTest(const DetectEllipsoidsTest&); // Copy Constructor Not Implemented
  void operator=(const DetectEllipsoidsTest&);       // Move assignment Not Implemented
};
