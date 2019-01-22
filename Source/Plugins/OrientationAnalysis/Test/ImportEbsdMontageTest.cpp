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

  ImportEbsdMontageTest(const ImportEbsdMontageTest&) = delete;            // Copy Constructor
  ImportEbsdMontageTest(ImportEbsdMontageTest&&) = delete;                 // Move Constructor
  ImportEbsdMontageTest& operator=(const ImportEbsdMontageTest&) = delete; // Copy Assignment
  ImportEbsdMontageTest& operator=(ImportEbsdMontageTest&&) = delete;      // Move Assignment

  int RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::ImportEbsdMontageTest::TestFile1);
    QFile::remove(UnitTest::ImportEbsdMontageTest::TestFile2);
#endif
    return 0;
  }

public:
  ImportEbsdMontageTest()
  {
    IFilterFactory::Pointer ebsdMontageFactory{
      FilterManager::Instance()->getFactoryFromClassName(m_filtName)
    };
    DREAM3D_REQUIRE(ebsdMontageFactory.get() != nullptr);

    m_EBSDMontageFilter =  ebsdMontageFactory->create();
    DREAM3D_REQUIRE(m_EBSDMontageFilter.get() != nullptr);
  }
  ~ImportEbsdMontageTest() = default;

  int SetUp()
  {
    return 0;
  }

  int RunTest()
  {
    int foo = -1;
    DREAM3D_REQUIRE_EQUAL(foo, 0)

    return EXIT_SUCCESS;
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
