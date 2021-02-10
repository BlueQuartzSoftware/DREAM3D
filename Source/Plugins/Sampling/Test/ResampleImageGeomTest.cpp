// -----------------------------------------------------------------------------
// Insert your license & copyright information here
// -----------------------------------------------------------------------------
#pragma once

#include <cstring>
#include <string>

#include <QtCore/QFile>
#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/CoreFilters/DataContainerReader.h"
#include "SIMPLib/CoreFilters/DataContainerWriter.h"
#include "SIMPLib/CoreFilters/RawBinaryReader.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersWriter.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "UnitTestSupport.hpp"

#include "Sampling/SamplingFilters/ResampleImageGeom.h"
#include "SamplingTestFileLocations.h"

class ResampleImageGeomTest
{

  const QString k_PipelineFile = {"ResampleImageGeomPipeline.json"};
  const QString k_Dream3DInputFile = {"Small_IN100_Slice1.dream3d"};

  const QString k_SuperSample1 = {"SuperSample1.dream3d"};
  const QString k_SuperSample2 = {"SuperSample2.dream3d"};
  const QString k_SuperSample3 = {"SuperSample3.dream3d"};
  const QString k_SuperSample4 = {"SuperSample4.dream3d"};

  const QString k_SubSample1 = {"SubSample1.dream3d"};
  const QString k_SubSample2 = {"SubSample2.dream3d"};
  const QString k_SubSample3 = {"SubSample3.dream3d"};
  const QString k_SubSample4 = {"SubSample4.dream3d"};

public:
  ResampleImageGeomTest() = default;
  ~ResampleImageGeomTest() = default;

  ResampleImageGeomTest(const ResampleImageGeomTest&) = delete;            // Copy Constructor
  ResampleImageGeomTest(ResampleImageGeomTest&&) = delete;                 // Move Constructor
  ResampleImageGeomTest& operator=(const ResampleImageGeomTest&) = delete; // Copy Assignment
  ResampleImageGeomTest& operator=(ResampleImageGeomTest&&) = delete;      // Move Assignment

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
    QFile::remove(UnitTest::TestTempDir + "/" + k_SuperSample1);
    QFile::remove(UnitTest::TestTempDir + "/" + k_SuperSample2);
    QFile::remove(UnitTest::TestTempDir + "/" + k_SuperSample3);
    QFile::remove(UnitTest::TestTempDir + "/" + k_SuperSample4);
    QFile::remove(UnitTest::TestTempDir + "/" + k_SubSample1);
    QFile::remove(UnitTest::TestTempDir + "/" + k_SubSample2);
    QFile::remove(UnitTest::TestTempDir + "/" + k_SubSample3);
    QFile::remove(UnitTest::TestTempDir + "/" + k_SubSample4);
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

    Int32ArrayType::Pointer featureIds = Int32ArrayType::CreateArray(dims, {1ULL}, "FeatureIds", true);
    cellAM->addOrReplaceAttributeArray(featureIds);

    BoolArrayType::Pointer actives = BoolArrayType::CreateArray(1ULL, std::string("Active Objects"), true);

    AttributeMatrix::Pointer featureAM = AttributeMatrix::New({1}, "Grain Data", AttributeMatrix::Type::CellFeature);
    featureAM->addOrReplaceAttributeArray(actives);

    dc->addOrReplaceAttributeMatrix(featureAM);

    return dca;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int SanityCheckParameters()
  {
    Observer obs;

    DataContainerArray::Pointer dca = createDataStructure(false);

    ResampleImageGeom::Pointer changeSpacing = ResampleImageGeom::New();
    changeSpacing->connect(changeSpacing.get(), SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), &obs, SLOT(processPipelineMessage(const AbstractMessage::Pointer&)));

    changeSpacing->setDataContainerArray(dca);

    // Take the default values for the filter
    changeSpacing->preflight();
    int32_t error = changeSpacing->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(error, -5557);

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

    // Set to renumber the features but do NOT set a featureIds Path
    dca = createDataStructure(true);
    changeSpacing->setDataContainerArray(dca);
    changeSpacing->setRenumberFeatures(true);
    changeSpacing->preflight();
    error = changeSpacing->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(error, -80002);

    dca = createDataStructure(true);
    changeSpacing->setDataContainerArray(dca);
    changeSpacing->setFeatureIdsArrayPath({"DataContainer", "CellData", "FeatureIds"});
    changeSpacing->setCellFeatureAttributeMatrixPath({"DataContainer", "Grain Data", ""});
    changeSpacing->preflight();
    error = changeSpacing->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(error, 0);

    DataContainer::Pointer copyDc = dca->getDataContainer("Resampled");
    DREAM3D_REQUIRE_VALID_POINTER(copyDc);

    return EXIT_SUCCESS;
  }

  /**
   * @brief SuperSamplingTest
   * @return
   */
  int SuperSamplingTest()
  {
    int32_t error = 0;

    QString pipelineFile = UnitTest::SamplingSourceTestFilesDir + "/" + k_PipelineFile;
    JsonFilterParametersReader::Pointer jsonReader = JsonFilterParametersReader::New();
    FilterPipeline::Pointer pipeline = jsonReader->readPipelineFromFile(pipelineFile);
    std::cout << "Filter Count: " << pipeline->size() << std::endl;
    Observer obs; // Create an Observer to report errors/progress from the executing pipeline
                  //    pipeline->addMessageReceiver(&obs);

    DataContainerReader::Pointer reader = std::dynamic_pointer_cast<DataContainerReader>(pipeline->getFilterContainer()[0]);
    reader->setInputFile(UnitTest::SamplingSourceTestFilesDir + "/" + k_Dream3DInputFile);

    ResampleImageGeom::Pointer resample = ResampleImageGeom::New();
    resample->setNewDataContainerPath({"Resampled", "", ""});
    resample->setCellAttributeMatrixPath({"Small IN100 Slice 1", "EBSD Scan Data", ""});
    resample->setCellFeatureAttributeMatrixPath({"Small IN100 Slice 1", "Grain Data", ""});
    resample->setSpacing({0.125F, 0.125F, 0.125F});
    resample->setRenumberFeatures(true);
    resample->setSaveAsNewDataContainer(true);
    resample->setFeatureIdsArrayPath({"Small IN100 Slice 1", "EBSD Scan Data", "FeatureIds"});
    pipeline->pushBack(resample);

    // Bring in the comparison data
    RawBinaryReader::Pointer rawReader = RawBinaryReader::New();
    rawReader->setInputFile(UnitTest::SamplingSourceTestFilesDir + "/" + "Super_Confidence_Index.bin");
    rawReader->setScalarType(SIMPL::NumericTypes::Type::Float);
    rawReader->setNumberOfComponents(1);
    rawReader->setEndian(0);
    rawReader->setSkipHeaderBytes(0);
    rawReader->setCreatedAttributeArrayPath({"Resampled", "EBSD Scan Data", "Confidence Index [Exemplar]"});
    pipeline->pushBack(rawReader);

    // Write everything out to a file for debugging
    DataContainerWriter::Pointer writer = DataContainerWriter::New();
    writer->setOutputFile(UnitTest::TestTempDir + "/" + k_SuperSample1);
    pipeline->pushBack(writer);

    {
      // Preflight the pipeline
      error = pipeline->preflightPipeline();
      DREAM3D_REQUIRE_EQUAL(error, 0);

      // Execute the pipeline
      DataContainerArray::Pointer dca = pipeline->execute();
      error = pipeline->getErrorCode();
      DREAM3D_REQUIRE_EQUAL(error, 0);

      error = compareOutput(dca, {"Resampled", "EBSD Scan Data", "Confidence Index [Exemplar]"}, {"Resampled", "EBSD Scan Data", "Confidence Index"});
      DREAM3D_REQUIRE_EQUAL(error, 0);
    }

    {
      resample->setRenumberFeatures(false);
      resample->setSaveAsNewDataContainer(true);
      writer->setOutputFile(UnitTest::TestTempDir + "/" + k_SuperSample2);

      // Preflight the pipeline
      error = pipeline->preflightPipeline();
      DREAM3D_REQUIRE_EQUAL(error, 0);

      // Execute the pipeline
      DataContainerArray::Pointer dca = pipeline->execute();
      error = pipeline->getErrorCode();
      DREAM3D_REQUIRE_EQUAL(error, 0);

      error = compareOutput(dca, {"Resampled", "EBSD Scan Data", "Confidence Index [Exemplar]"}, {"Resampled", "EBSD Scan Data", "Confidence Index"});
      DREAM3D_REQUIRE_EQUAL(error, 0);
    }

    {
      resample->setRenumberFeatures(true);
      resample->setSaveAsNewDataContainer(false);
      rawReader->setCreatedAttributeArrayPath({"Small IN100 Slice 1", "EBSD Scan Data", "Confidence Index [Exemplar]"});
      writer->setOutputFile(UnitTest::TestTempDir + "/" + k_SuperSample3);

      // Preflight the pipeline
      error = pipeline->preflightPipeline();
      DREAM3D_REQUIRE_EQUAL(error, 0);

      // Execute the pipeline
      DataContainerArray::Pointer dca = pipeline->execute();
      error = pipeline->getErrorCode();
      DREAM3D_REQUIRE_EQUAL(error, 0);

      error = compareOutput(dca, {"Small IN100 Slice 1", "EBSD Scan Data", "Confidence Index [Exemplar]"}, {"Small IN100 Slice 1", "EBSD Scan Data", "Confidence Index"});
      DREAM3D_REQUIRE_EQUAL(error, 0);
    }

    {
      resample->setRenumberFeatures(false);
      resample->setSaveAsNewDataContainer(false);
      rawReader->setCreatedAttributeArrayPath({"Small IN100 Slice 1", "EBSD Scan Data", "Confidence Index [Exemplar]"});
      writer->setOutputFile(UnitTest::TestTempDir + "/" + k_SuperSample4);

      // Preflight the pipeline
      error = pipeline->preflightPipeline();
      DREAM3D_REQUIRE_EQUAL(error, 0);

      // Execute the pipeline
      DataContainerArray::Pointer dca = pipeline->execute();
      error = pipeline->getErrorCode();
      DREAM3D_REQUIRE_EQUAL(error, 0);

      error = compareOutput(dca, {"Small IN100 Slice 1", "EBSD Scan Data", "Confidence Index [Exemplar]"}, {"Small IN100 Slice 1", "EBSD Scan Data", "Confidence Index"});
      DREAM3D_REQUIRE_EQUAL(error, 0);
    }
    return error;
  }

  /**
   * @brief SuperSamplingTest
   * @return
   */
  int SubSamplingTest()
  {
    int32_t error = 0;

    QString pipelineFile = UnitTest::SamplingSourceTestFilesDir + "/" + k_PipelineFile;
    JsonFilterParametersReader::Pointer jsonReader = JsonFilterParametersReader::New();
    FilterPipeline::Pointer pipeline = jsonReader->readPipelineFromFile(pipelineFile);
    std::cout << "Filter Count: " << pipeline->size() << std::endl;
    Observer obs; // Create an Observer to report errors/progress from the executing pipeline
    // pipeline->addMessageReceiver(&obs);

    DataContainerReader::Pointer reader = std::dynamic_pointer_cast<DataContainerReader>(pipeline->getFilterContainer().at(0));
    reader->setInputFile(UnitTest::SamplingSourceTestFilesDir + "/" + k_Dream3DInputFile);

    ResampleImageGeom::Pointer resample = ResampleImageGeom::New();
    resample->setNewDataContainerPath({"Resampled", "", ""});
    resample->setCellAttributeMatrixPath({"Small IN100 Slice 1", "EBSD Scan Data", ""});
    resample->setCellFeatureAttributeMatrixPath({"Small IN100 Slice 1", "Grain Data", ""});
    resample->setSpacing({1.0F, 1.0F, 1.0F});
    resample->setRenumberFeatures(true);
    resample->setSaveAsNewDataContainer(true);
    resample->setFeatureIdsArrayPath({"Small IN100 Slice 1", "EBSD Scan Data", "FeatureIds"});
    pipeline->pushBack(resample);

    // Bring in the comparison data
    RawBinaryReader::Pointer rawReader = RawBinaryReader::New();
    rawReader->setInputFile(UnitTest::SamplingSourceTestFilesDir + "/" + "Sub_Confidence_Index.bin");
    rawReader->setScalarType(SIMPL::NumericTypes::Type::Float);
    rawReader->setNumberOfComponents(1);
    rawReader->setEndian(0);
    rawReader->setSkipHeaderBytes(0);
    rawReader->setCreatedAttributeArrayPath({"Resampled", "EBSD Scan Data", "Confidence Index [Exemplar]"});
    pipeline->pushBack(rawReader);

    // Write everything out to a file for debugging
    DataContainerWriter::Pointer writer = DataContainerWriter::New();
    writer->setOutputFile(UnitTest::TestTempDir + "/" + k_SubSample1);
    pipeline->pushBack(writer);

    {
      // Preflight the pipeline
      error = pipeline->preflightPipeline();
      DREAM3D_REQUIRE_EQUAL(error, 0);

      // Execute the pipeline
      DataContainerArray::Pointer dca = pipeline->execute();
      error = pipeline->getErrorCode();
      DREAM3D_REQUIRE_EQUAL(error, 0);

      error = compareOutput(dca, {"Resampled", "EBSD Scan Data", "Confidence Index [Exemplar]"}, {"Resampled", "EBSD Scan Data", "Confidence Index"});
      DREAM3D_REQUIRE_EQUAL(error, 0);
    }

    {
      resample->setRenumberFeatures(false);
      resample->setSaveAsNewDataContainer(true);
      writer->setOutputFile(UnitTest::TestTempDir + "/" + k_SubSample2);

      // Preflight the pipeline
      error = pipeline->preflightPipeline();
      DREAM3D_REQUIRE_EQUAL(error, 0);

      // Execute the pipeline
      DataContainerArray::Pointer dca = pipeline->execute();
      error = pipeline->getErrorCode();
      DREAM3D_REQUIRE_EQUAL(error, 0);

      error = compareOutput(dca, {"Resampled", "EBSD Scan Data", "Confidence Index [Exemplar]"}, {"Resampled", "EBSD Scan Data", "Confidence Index"});
      DREAM3D_REQUIRE_EQUAL(error, 0);
    }

    {
      resample->setRenumberFeatures(true);
      resample->setSaveAsNewDataContainer(false);
      rawReader->setCreatedAttributeArrayPath({"Small IN100 Slice 1", "EBSD Scan Data", "Confidence Index [Exemplar]"});

      writer->setOutputFile(UnitTest::TestTempDir + "/" + k_SubSample3);

      // Preflight the pipeline
      error = pipeline->preflightPipeline();
      DREAM3D_REQUIRE_EQUAL(error, 0);

      // Execute the pipeline
      DataContainerArray::Pointer dca = pipeline->execute();
      error = pipeline->getErrorCode();
      DREAM3D_REQUIRE_EQUAL(error, 0);

      error = compareOutput(dca, {"Small IN100 Slice 1", "EBSD Scan Data", "Confidence Index [Exemplar]"}, {"Small IN100 Slice 1", "EBSD Scan Data", "Confidence Index"});
      DREAM3D_REQUIRE_EQUAL(error, 0);
    }

    {
      resample->setRenumberFeatures(false);
      resample->setSaveAsNewDataContainer(false);
      rawReader->setCreatedAttributeArrayPath({"Small IN100 Slice 1", "EBSD Scan Data", "Confidence Index [Exemplar]"});
      writer->setOutputFile(UnitTest::TestTempDir + "/" + k_SubSample4);

      // Preflight the pipeline
      error = pipeline->preflightPipeline();
      DREAM3D_REQUIRE_EQUAL(error, 0);

      // Execute the pipeline
      DataContainerArray::Pointer dca = pipeline->execute();
      error = pipeline->getErrorCode();
      DREAM3D_REQUIRE_EQUAL(error, 0);

      error = compareOutput(dca, {"Small IN100 Slice 1", "EBSD Scan Data", "Confidence Index [Exemplar]"}, {"Small IN100 Slice 1", "EBSD Scan Data", "Confidence Index"});
      DREAM3D_REQUIRE_EQUAL(error, 0);
    }
    return error;
  }

  /**
   * @brief compareOutput
   * @param dca
   * @param path1
   * @param path2
   * @return
   */
  int32_t compareOutput(DataContainerArray::Pointer& dca, DataArrayPath path1, DataArrayPath path2)
  {

    FloatArrayType::Pointer data1 = dca->getPrereqArrayFromPath<FloatArrayType>(nullptr, path1, {1});
    FloatArrayType::Pointer data2 = dca->getPrereqArrayFromPath<FloatArrayType>(nullptr, path2, {1});
    if(data1->size() != data2->size())
    {
      return -2;
    }
    if(0 == ::memcmp(data1->getTuplePointer(0), data2->getTuplePointer(0), data1->size() * sizeof(float)))
    {
      return 0;
    }
    return -1;
  }

  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(SanityCheckParameters())
    DREAM3D_REGISTER_TEST(SuperSamplingTest())
    DREAM3D_REGISTER_TEST(SubSamplingTest())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

private:
};
