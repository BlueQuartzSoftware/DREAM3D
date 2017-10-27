// -----------------------------------------------------------------------------
// Insert your license & copyright information here
// -----------------------------------------------------------------------------

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
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

#include "OrientationAnalysisTestFileLocations.h"

#include "OrientationLib/LaueOps/LaueOps.h"

class WriteIPFStandardTriangleTest
{

public:
  WriteIPFStandardTriangleTest()
  {
  }
  virtual ~WriteIPFStandardTriangleTest()
  {
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES

#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the WriteIPFStandardTriangle Filter from the FilterManager
    QString filtName = "WriteIPFStandardTriangle";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
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
  int TestWriteIPFStandardTriangle()
  {

    QString filtName = "WriteIPFStandardTriangle";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
    DREAM3D_REQUIRE_VALID_POINTER(filterFactory.get())

    AbstractFilter::Pointer filter = filterFactory->create();
    DREAM3D_REQUIRE_VALID_POINTER(filter.get())

    std::vector<QString> names = LaueOps::GetLaueNames();
    for(std::vector<QString>::size_type i = 0; i < names.size(); i++)
    {

      QString outputFile = UnitTest::TestTempDir + QDir::separator() + QString::number(i) + "_" + names[i] + ".png";
      QVariant var;

      bool propWasSet = filter->setProperty("LaueClass", static_cast<int>(i));
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      propWasSet = filter->setProperty("OutputFile", outputFile);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      int imageSize = 1024;
      propWasSet = filter->setProperty("ImageSize", imageSize);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      filter->execute();
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

    DREAM3D_REGISTER_TEST(TestWriteIPFStandardTriangle())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

private:
  WriteIPFStandardTriangleTest(const WriteIPFStandardTriangleTest&); // Copy Constructor Not Implemented
  void operator=(const WriteIPFStandardTriangleTest&);               // Operator '=' Not Implemented
};
