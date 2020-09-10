
#include <QtCore/QString>

#include "SIMPLib/Common/Observer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "UnitTestSupport.hpp"

#include "ImportExport/ImportExportFilters/SPParksDumpReader.h"

#include "ImportExportTestFileLocations.h"

class SPParksDumpReaderTest
{
  const QString k_VolumeDataContainerName = QString("SPParksVolume");
  const QString k_CellAttributeMatrixName = QString("CellData");
  const QString k_FeatureIdsName = QString("Atoms");

public:
  SPParksDumpReaderTest() = default;
  virtual ~SPParksDumpReaderTest() = default;
  /**
   * @brief Returns the name of the class for PhIOTest
   */
  /**
   * @brief Returns the name of the class for PhIOTest
   */
  QString getNameOfClass() const
  {
    return QString("PhIOTest");
  }

  /**
   * @brief Returns the name of the class for PhIOTest
   */
  QString ClassName()
  {
    return QString("PhIOTest");
  }

  // -----------------------------------------------------------------------------
  void RunTest()
  {
    std::cout << "RunTest::Starting\n    Input File: " << m_InputFile.toStdString() << std::endl;
    //    Observer obs;

    SPParksDumpReader::Pointer reader = SPParksDumpReader::New();
    //  reader->connect(reader.get(), SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), &obs, SLOT(processPipelineMessage(const AbstractMessage::Pointer&)));
    reader->setVolumeDataContainerName({k_VolumeDataContainerName, "", ""});
    reader->setCellAttributeMatrixName(k_CellAttributeMatrixName);
    reader->setInputFile(m_InputFile);
    reader->setOrigin({0.0F, 0.0F, 0.0F});
    reader->setSpacing({1.0F, 1.0F, 1.0F});
    reader->setOneBasedArrays(true);
    reader->setFeatureIdsArrayName(k_FeatureIdsName);
    // This next one should fail
    reader->setDataContainerArray(DataContainerArray::New());
    reader->preflight();
    int32_t err = reader->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, -26010);
    // this should now work
    reader->setOneBasedArrays(false);
    reader->setDataContainerArray(DataContainerArray::New());
    reader->preflight();
    err = reader->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, 0);

    // Now execute the filter and check the created geometry
    reader->setDataContainerArray(DataContainerArray::New());
    reader->execute();
    err = reader->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, 0);

    DataContainerArray::Pointer dca = reader->getDataContainerArray();
    DataContainer::Pointer dc = dca->getDataContainer({k_VolumeDataContainerName, "", ""});
    ImageGeom::Pointer imageGeom = dc->getGeometryAs<ImageGeom>();
    SizeVec3Type dims = imageGeom->getDimensions();
    DREAM3D_REQUIRE_EQUAL(dims[0], 2)
    DREAM3D_REQUIRE_EQUAL(dims[1], 2)
    DREAM3D_REQUIRE_EQUAL(dims[2], 2)

    AttributeMatrix::Pointer am = dc->getAttributeMatrix(k_CellAttributeMatrixName);
    DREAM3D_REQUIRE_VALID_POINTER(am.get());

    Int32ArrayType::Pointer idsPtr = am->getAttributeArrayAs<Int32ArrayType>(k_FeatureIdsName);
    DREAM3D_REQUIRE_VALID_POINTER(idsPtr.get());

    Int32ArrayType& ids = *idsPtr;
    DREAM3D_REQUIRE_EQUAL(ids[0], 119)
    DREAM3D_REQUIRE_EQUAL(ids[1], 120)
    DREAM3D_REQUIRE_EQUAL(ids[2], 559)
    DREAM3D_REQUIRE_EQUAL(ids[3], 130)
    DREAM3D_REQUIRE_EQUAL(ids[4], 140)
    DREAM3D_REQUIRE_EQUAL(ids[5], 150)
    DREAM3D_REQUIRE_EQUAL(ids[6], 160)
    DREAM3D_REQUIRE_EQUAL(ids[7], 558)
  }

  /**
   * @brief This is the main function
   */
  void operator()()
  {
    int err = EXIT_SUCCESS;
    std::cout << "<===== Start " << getNameOfClass().toStdString() << std::endl;
    m_InputFile = UnitTest::ImportExportTestFilesDir + "/SPParks_Standard.dump";
    DREAM3D_REGISTER_TEST(RunTest());
    m_InputFile = UnitTest::ImportExportTestFilesDir + "/SPParks_Pizza.dump";
    DREAM3D_REGISTER_TEST(RunTest());
  }

public:
  SPParksDumpReaderTest(const SPParksDumpReaderTest&) = delete;            // Copy Constructor Not Implemented
  SPParksDumpReaderTest(SPParksDumpReaderTest&&) = delete;                 // Move Constructor Not Implemented
  SPParksDumpReaderTest& operator=(const SPParksDumpReaderTest&) = delete; // Copy Assignment Not Implemented
  SPParksDumpReaderTest& operator=(SPParksDumpReaderTest&&) = delete;      // Move Assignment Not Implemented

private:
  QString m_InputFile;
};
