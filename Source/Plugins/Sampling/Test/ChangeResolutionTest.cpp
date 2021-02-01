// -----------------------------------------------------------------------------
// Insert your license & copyright information here
// -----------------------------------------------------------------------------
#pragma once

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "Sampling/SamplingFilters/ChangeResolution.h"

#include "UnitTestSupport.hpp"

#include "SamplingTestFileLocations.h"

class ChangeResolutionTest
{

public:
  ChangeResolutionTest() = default;
  ~ChangeResolutionTest() = default;
  
  ChangeResolutionTest(const ChangeResolutionTest&) = delete;            // Copy Constructor
  ChangeResolutionTest(ChangeResolutionTest&&) = delete;                 // Move Constructor
  ChangeResolutionTest& operator=(const ChangeResolutionTest&) = delete; // Copy Assignment
  ChangeResolutionTest& operator=(ChangeResolutionTest&&) = delete;      // Move Assignment

  /**
   * @brief Returns the name of the class for SampleSurfaceMeshSpecifiedPointsTest
   */
  /**
   * @brief Returns the name of the class for SampleSurfaceMeshSpecifiedPointsTest
   */
  QString getNameOfClass() const
  {
    return QString("SampleSurfaceMeshSpecifiedPointsTest");
  }

  /**
   * @brief Returns the name of the class for SampleSurfaceMeshSpecifiedPointsTest
   */
  QString ClassName()
  {
    return QString("SampleSurfaceMeshSpecifiedPointsTest");
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
//    QFile::remove(UnitTest::SampleSurfaceMeshSpecifiedPointsTest::TestFile1);
//    QFile::remove(UnitTest::SampleSurfaceMeshSpecifiedPointsTest::TestFile2);
#endif
  }

  // -----------------------------------------------------------------------------
  DataContainerArray::Pointer createDataStructure(bool addGeometry)
  {

    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer dc = DataContainer::New("DataContainer");
    dca->addOrReplaceDataContainer(dc);

    std::vector<size_t> dims = {10, 10, 10};
    AttributeMatrix::Pointer cellAM = AttributeMatrix::New(dims, "CellData", AttributeMatrix::Type::Cell);
    dc->addOrReplaceAttributeMatrix(cellAM);

    UInt8ArrayType::Pointer data = UInt8ArrayType::CreateArray(dims, {1ULL}, "Data", true);
    cellAM->addOrReplaceAttributeArray(data);
    if(addGeometry)
    {
      ImageGeom::Pointer imageGeom = ImageGeom::New();
      imageGeom->setDimensions(dims);
      imageGeom->setOrigin({5.0F, 5.0F, 5.0F});
      imageGeom->setSpacing({1.0F, 1.0F, 1.0F});
      dc->setGeometry(imageGeom);
    }
    return dca;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int RunTest()
  {
    Observer obs;

    DataContainerArray::Pointer dca = createDataStructure(false);

    ChangeResolution::Pointer changeSpacing = ChangeResolution::New();
    changeSpacing->connect(changeSpacing.get(), SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), &obs, SLOT(processPipelineMessage(const AbstractMessage::Pointer&)));

    changeSpacing->setDataContainerArray(dca);

    // Take the default values for the filter
    changeSpacing->preflight();
    int32_t error = changeSpacing->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(error, -999);

    // Set a valid Data Container / Attribute Matrix Path
    // This will fail because the default spacing values are {0.0, 0.0, 0.0} which is invalid
    changeSpacing->setCellAttributeMatrixPath({"DataContainer", "CellData", ""});
    changeSpacing->preflight();
    error = changeSpacing->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(error, -5557);

    // Set a valid Spacing vector
    changeSpacing->setSpacing({1.0F, 1.0F, 1.0F});
    changeSpacing->preflight();
    error = changeSpacing->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(error, -385);

    // Data Container now has a geometry, but the input spacing and the spacing of the geometry are the same.
    // No error is thrown, the filter just will not do anything
    dca = createDataStructure(true);
    changeSpacing->setDataContainerArray(dca);
    changeSpacing->preflight();
    error = changeSpacing->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(error, 0);

    // Set an updated Spacing vector
    changeSpacing->setSpacing({0.5F, 0.5F, 0.5F});
    changeSpacing->preflight();
    error = changeSpacing->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(error, 0);

    // Check the Data Container to see if the Geometry was updated correctly.
    ImageGeom::Pointer imageGeom = dca->getDataContainer("DataContainer")->getGeometryAs<ImageGeom>();
    SizeVec3Type dims = imageGeom->getDimensions();
    DREAM3D_REQUIRE_EQUAL(dims[0], 20);
    DREAM3D_REQUIRE_EQUAL(dims[1], 20);
    DREAM3D_REQUIRE_EQUAL(dims[2], 20);

    FloatVec3Type spacing = imageGeom->getSpacing();
    DREAM3D_REQUIRE_EQUAL(spacing[0], 0.5F);
    DREAM3D_REQUIRE_EQUAL(spacing[1], 0.5F);
    DREAM3D_REQUIRE_EQUAL(spacing[2], 0.5F);

    // Save as a new Data Container
    dca = createDataStructure(true);
    changeSpacing->setDataContainerArray(dca);
    changeSpacing->setSaveAsNewDataContainer(true);
    changeSpacing->setNewDataContainerPath({"Resampled", "", ""});
    changeSpacing->preflight();
    error = changeSpacing->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(error, 0);

    DataContainer::Pointer copyDc = dca->getDataContainer("Resampled");
    DREAM3D_REQUIRE_VALID_POINTER(copyDc);

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(RunTest())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

private:
};
