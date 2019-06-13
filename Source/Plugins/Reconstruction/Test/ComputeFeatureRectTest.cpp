// -----------------------------------------------------------------------------
// Insert your license & copyright information here
// -----------------------------------------------------------------------------

#include <QtCore/QFile>

#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "UnitTestSupport.hpp"

#include "ReconstructionTestFileLocations.h"

class ComputeFeatureRectTest
{

public:
  ComputeFeatureRectTest()
  {
  }
  virtual ~ComputeFeatureRectTest()
  {
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::ComputeFeatureRectTest::TestFile1);
    QFile::remove(UnitTest::ComputeFeatureRectTest::TestFile2);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the ComputeFeatureRectTest Filter from the FilterManager
    QString filtName = "ComputeFeatureRect";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The Reconstruction Requires the use of the " << filtName.toStdString() << " filter which is found in the Reconstruction Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  DataContainerArray::Pointer CreateTestData()
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();
    DataContainer::Pointer dc = DataContainer::New("Test");
    dca->addOrReplaceDataContainer(dc);

    ImageGeom::Pointer igeom = ImageGeom::New();
    size_t dims_in[3] = {5, 5, 1};
    igeom->setDimensions(dims_in);
    dc->setGeometry(igeom);
    std::vector<size_t> dims(3, 0);
    dims[0] = 5;
    dims[1] = 5;
    dims[2] = 1;
    AttributeMatrix::Pointer cellAM = AttributeMatrix::New(dims, "CellData", AttributeMatrix::Type::Cell);
    dc->addOrReplaceAttributeMatrix(cellAM);

    Int32ArrayType::Pointer featureIds = Int32ArrayType::CreateArray(25, "FeatureIds", true);
    featureIds->initializeWithZeros();
    featureIds->setValue(6, 1);
    featureIds->setValue(7, 1);
    featureIds->setValue(8, 1);

    featureIds->setValue(11, 1);
    featureIds->setValue(12, 1);
    featureIds->setValue(13, 1);

    featureIds->setValue(16, 1);
    featureIds->setValue(17, 1);
    featureIds->setValue(18, 1);

    cellAM->insertOrAssign(featureIds);

    dims.resize(1);
    dims[0] = 2;
    AttributeMatrix::Pointer featureAM = AttributeMatrix::New(dims, "FeatureData", AttributeMatrix::Type::CellFeature);
    dc->addOrReplaceAttributeMatrix(featureAM);

#if 0
      0, 0, 0, 0, 0,
      0, 1, 1, 1, 0,
      0, 1, 1, 1, 0,
      0, 1, 1, 1, 0,
      0, 0, 0, 0, 0;
#endif
    std::vector<size_t> compDims(1, 6);
    UInt32ArrayType::Pointer rect = UInt32ArrayType::CreateArray(2, compDims, "Rect Coords", true);
    rect->initializeWithZeros();
    rect->setValue(6, 1);
    rect->setValue(7, 1);
    rect->setValue(8, 0);
    rect->setValue(9, 3);
    rect->setValue(10, 3);
    rect->setValue(11, 0);
    // featureAM->insertOrAssign(rect);

    return dca;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestComputeFeatureRectTest()
  {
    DataContainerArray::Pointer dca = CreateTestData();

    QString filtName = "ComputeFeatureRect";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);

    AbstractFilter::Pointer filter = filterFactory->create();
    filter->setDataContainerArray(dca);

    Observer obs;
    obs.connect(filter.get(), SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), &obs, SLOT(processPipelineMessage(const AbstractMessage::Pointer&)));

    DataArrayPath path("Test", "CellData", "FeatureIds");
    QVariant variant;
    variant.setValue(path);

    bool ok = filter->setProperty("FeatureIdsArrayPath", variant);
    DREAM3D_REQUIRE_EQUAL(ok, true)

    path = DataArrayPath("Test", "FeatureData", "Rect Coords");
    variant.setValue(path);
    ok = filter->setProperty("FeatureRectArrayPath", variant);
    DREAM3D_REQUIRE_EQUAL(ok, true)

    filter->preflight();
    int err = filter->getErrorCode();
    DREAM3D_REQUIRE(err >= 0)

    dca = CreateTestData();
    filter->setDataContainerArray(dca);

    filter->execute();
    err = filter->getErrorCode();
    DREAM3D_REQUIRE(err >= 0)

    AttributeMatrix::Pointer featureAM = dca->getAttributeMatrix(DataArrayPath("Test", "FeatureData", ""));

    UInt32ArrayType::Pointer coordPtr = featureAM->getAttributeArrayAs<UInt32ArrayType>("Rect Coords");

    uint32_t* coords = coordPtr->getTuplePointer(1);

    DREAM3D_REQUIRE(coords[0] == 1)
    DREAM3D_REQUIRE(coords[1] == 1)
    DREAM3D_REQUIRE(coords[2] == 0)

    DREAM3D_REQUIRE(coords[3] == 3)
    DREAM3D_REQUIRE(coords[4] == 3)
    DREAM3D_REQUIRE(coords[5] == 0)

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(TestComputeFeatureRectTest())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

private:
  ComputeFeatureRectTest(const ComputeFeatureRectTest&); // Copy Constructor Not Implemented
  void operator=(const ComputeFeatureRectTest&);         // Move assignment Not Implemented
};
