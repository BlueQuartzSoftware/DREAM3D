// -----------------------------------------------------------------------------
// Insert your license & copyright information here
// -----------------------------------------------------------------------------

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"
#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"

#include "OrientationAnalysisTestFileLocations.h"

class CreateLambertSphereTest
{

public:
  CreateLambertSphereTest()
  {
  }
  virtual ~CreateLambertSphereTest()
  {
  }

  QString m_FiltName = QString("CreateLambertSphere");

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::CreateLambertSphereTest::OutputFile);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the CreateLambertSphereTest Filter from the FilterManager
    QString filtName = m_FiltName;
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
  int TestCreateLambertSphereTest()
  {
    AbstractFilter::Pointer filter = AbstractFilter::NullPointer();
    QString filtName = m_FiltName;
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);

    DREAM3D_REQUIRE_VALID_POINTER(filterFactory.get());

    filter = filterFactory->create();
    DREAM3D_REQUIRE_VALID_POINTER(filter.get())

    // Set a bad Hemisphere value
    QVariant var(-1);
    bool propWasSet = filter->setProperty("Hemisphere", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true);

    filter->preflight();
    int error = filter->getErrorCondition();
    DREAM3D_REQUIRE(error < 0)

    // Set a valid hemisphere value
    var.setValue(0);
    propWasSet = filter->setProperty("Hemisphere", var);

    // Set a bad path
    DataArrayPath path;
    var.setValue(path);
    propWasSet = filter->setProperty("ImageDataArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true);

    filter->preflight();
    error = filter->getErrorCondition();
    DREAM3D_REQUIRE(error < 0)

    QStringList props = {"QuadDataContainerName",
        "TriangleDataContainerName",
        "EdgeDataContainerName",
        "VertexDataContainerName",
        "VertexAttributeMatrixName",
        "EdgeAttributeMatrixName",
        "FaceAttributeMatrixName"
    };
    foreach(QString prop, props)
    {
      // Reset the filter by creating a new one
      filter = filterFactory->create();
      propWasSet = filter->setProperty(prop.toLatin1().constData(), QString(""));
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      filter->preflight();
      error = filter->getErrorCondition();
      DREAM3D_REQUIRE(error < 0)
    }

    /* Run a prewritten pipeline from a file. The pipeline should be the following:
    * "Filter_Name": "ITKImageReader",
    * "Filter_Human_Label": "Set Origin & Resolution (Image)",
    * "Filter_Human_Label": "Color to GrayScale",
    * "Filter_Human_Label": "Create 3D Sphere from 2D Master Pattern",
    * "Filter_Human_Label": "Write DREAM.3D Data File",
    */

    //Observer obs;

    JsonFilterParametersReader::Pointer reader = JsonFilterParametersReader::New();
    FilterPipeline::Pointer pipeline = reader->readPipelineFromFile(UnitTest::CreateLambertSphereTest::PipelineFile);
    DREAM3D_REQUIRE_VALID_POINTER(pipeline.get());
    //pipeline->addMessageReceiver(&obs);

    FilterPipeline::FilterContainerType filters = pipeline->getFilterContainer();
    filter = filters[0];
    var.setValue(UnitTest::CreateLambertSphereTest::MasterPattern);
    propWasSet = filter->setProperty("FileName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true);

    filter = filters[4];
    var.setValue(UnitTest::CreateLambertSphereTest::OutputFile);
    propWasSet = filter->setProperty("OutputFile", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true);

    error = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(error >= 0)

    pipeline->run();
    error = pipeline->getErrorCondition();
    DREAM3D_REQUIRE(error >= 0)

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(TestCreateLambertSphereTest())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

private:
  CreateLambertSphereTest(const CreateLambertSphereTest&); // Copy Constructor Not Implemented
  void operator=(const CreateLambertSphereTest&);          // Operator '=' Not Implemented
};
