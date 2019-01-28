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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS”
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

#pragma once

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

#include "SIMPLib/Filtering/QMetaObjectUtilities.h"

#include "UnitTestSupport.hpp"
//#include "EbsdMontageTestFileLocations.h"

class ImportEbsdMontageTest
{
  AbstractFilter::Pointer m_EBSDMontageFilter{nullptr};
  const QString m_filtName{"ImportEbsdMontage"};

  int ValidateData()
  {
    return 0;
  }

  int RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
//    QFile::remove(UnitTest::ImportEbsdMontageTest::TestFile1);
//    QFile::remove(UnitTest::ImportEbsdMontageTest::TestFile2);
#endif
    return 0;
  }

public:
  ImportEbsdMontageTest()
  {
    IFilterFactory::Pointer ebsdMontageFactory{FilterManager::Instance()->getFactoryFromClassName(m_filtName)};
    DREAM3D_REQUIRE(ebsdMontageFactory.get() != nullptr);

    m_EBSDMontageFilter = ebsdMontageFactory->create();
    DREAM3D_REQUIRE(m_EBSDMontageFilter.get() != nullptr);
  }
  ImportEbsdMontageTest(const ImportEbsdMontageTest&) = delete;            // Copy Constructor
  ImportEbsdMontageTest(ImportEbsdMontageTest&&) = delete;                 // Move Constructor
  ImportEbsdMontageTest& operator=(const ImportEbsdMontageTest&) = delete; // Copy Assignment
  ImportEbsdMontageTest& operator=(ImportEbsdMontageTest&&) = delete;      // Move Assignment

  ~ImportEbsdMontageTest() = default;

  int SetUp()
  {
    return 0;
  }

  int RunTest()
  {
    m_EBSDMontageFilter->execute();
    int erred{m_EBSDMontageFilter->getErrorCondition()};
    DREAM3D_REQUIRE_EQUAL(erred, 0)
    int dataInvalidated{ValidateData()};
    DREAM3D_REQUIRE_EQUAL(dataInvalidated, 0)

    return erred + dataInvalidated;
  }

  int TearDown()
  {
    DREAM3D_REQUIRE_EQUAL(RemoveTestFiles(), 0)
    return 0;
  }

  void operator()()
  {
    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(SetUp());
    DREAM3D_REGISTER_TEST(RunTest())
    DREAM3D_REGISTER_TEST(TearDown())
  }
};
