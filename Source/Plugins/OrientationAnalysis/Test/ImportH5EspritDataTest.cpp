/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
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
#include <QtCore/QFile>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/CoreFilters/DataContainerWriter.h"
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

#include "OrientationAnalysisTestFileLocations.h"

class ImportH5EspritDataTest
{

public:
  ImportH5EspritDataTest() = default;
  ~ImportH5EspritDataTest() = default;

  QString m_FiltName = QString("ImportH5EspritData");
  const QString k_HDF5Path = QString("Section_435");

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::ImportH5EspritDataTest::OutputFile);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the ImportH5EspritDataTest Filter from the FilterManager
    QString filtName = m_FiltName;
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The OrientationAnalysis Requires the use of the " << filtName.toStdString() << " filter which is found in the OrientationAnalysis Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestImportH5EspritDataTest()
  {
    Observer obs;

    FilterPipeline::Pointer pipeline = FilterPipeline::New();
    pipeline->addMessageReceiver(&obs);

    AbstractFilter::Pointer filter = AbstractFilter::NullPointer();
    QString filtName = m_FiltName;
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);

    DREAM3D_REQUIRE_VALID_POINTER(filterFactory.get());

    filter = filterFactory->create();
    DREAM3D_REQUIRE_VALID_POINTER(filter.get())

    pipeline->pushBack(filter);

    // Set an input file
    QVariant var(UnitTest::ImportH5EspritDataTest::InputFile);
    bool propWasSet = filter->setProperty("InputFile", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true);

    var = QStringList(k_HDF5Path);
    propWasSet = filter->setProperty("SelectedScanNames", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true);

    var = QVariant(1.0f);
    propWasSet = filter->setProperty("ZSpacing", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true);

    var = QVariant(false);
    propWasSet = filter->setProperty("ReadPatternData", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true);

    DataContainerWriter::Pointer writer = DataContainerWriter::New();
    writer->setOutputFile(UnitTest::ImportH5EspritDataTest::OutputFile);
    pipeline->pushBack(writer);

    pipeline->preflightPipeline();
    int error = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(error, >=, 0)

    pipeline->execute();
    error = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(error, >=, 0)

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;
    qDebug() << "---- ImportH5EspritDataTest Starting ----";
    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(TestImportH5EspritDataTest())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

public:
  ImportH5EspritDataTest(const ImportH5EspritDataTest&) = delete;            // Copy Constructor Not Implemented
  ImportH5EspritDataTest(ImportH5EspritDataTest&&) = delete;                 // Move Constructor Not Implemented
  ImportH5EspritDataTest& operator=(const ImportH5EspritDataTest&) = delete; // Copy Assignment Not Implemented
  ImportH5EspritDataTest& operator=(ImportH5EspritDataTest&&) = delete;      // Move Assignment Not Implemented
};
