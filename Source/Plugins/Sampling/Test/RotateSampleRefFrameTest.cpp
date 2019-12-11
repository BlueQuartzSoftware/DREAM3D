/* ============================================================================
 * Copyright (c) 2019-2019 BlueQuartz Software, LLC
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
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
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

#include <QtCore/QFile>

#include "SIMPLib/CoreFilters/DataContainerReader.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "UnitTestSupport.hpp"

#include "SamplingTestFileLocations.h"

#include "SIMPLib/FilterParameters/DynamicTableData.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"

class RotateSampleRefFrameTest
{
private:
  const QString k_FilterName = "RotateSampleRefFrame";
  const QString k_RotationAngleName = "RotationAngle";
  const QString k_RotationAxisName = "RotationAxis";
  const QString k_RotationTableName = "RotationTable";
  const QString k_RotationRepresentationName = "RotationRepresentation";
  const QString k_CellAttributeMatrixPathName = "CellAttributeMatrixPath";

  template <class T>
  static bool dataArrayEqual(const DataArray<T>& a, const DataArray<T>& b)
  {
    if(a.getNumberOfComponents() != b.getNumberOfComponents())
    {
      return false;
    }

    if(a.getNumberOfTuples() != b.getNumberOfTuples())
    {
      return false;
    }

    return std::equal(a.begin(), a.end(), b.begin(), b.end());
  }

  static void resetGeometry(AttributeMatrix::Pointer matrix, ImageGeom::Pointer imageGeom, const std::vector<size_t>& tDims)
  {
    matrix->resizeAttributeArrays(tDims);
    imageGeom->setDimensions(tDims);
    imageGeom->setOrigin(0.0f, 0.0f, 0.0f);
    imageGeom->setSpacing(1.0f, 1.0f, 1.0f);
  }

  AbstractFilter::Pointer createFilter() const
  {
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(k_FilterName);
    DREAM3D_REQUIRE_VALID_POINTER(filterFactory)

    AbstractFilter::Pointer rotateFilter = filterFactory->create();
    DREAM3D_REQUIRE_VALID_POINTER(rotateFilter)

    return rotateFilter;
  }

  template <class T>
  static void setProperty(AbstractFilter::Pointer filter, const QString& property, const T& value)
  {
    QVariant variant;
    variant.setValue(value);
    bool propWasSet = filter->setProperty(property.toStdString().c_str(), variant);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)
  }

public:
  RotateSampleRefFrameTest() = default;
  ~RotateSampleRefFrameTest() = default;
  RotateSampleRefFrameTest(const RotateSampleRefFrameTest&) = delete;            // Copy Constructor Not Implemented
  RotateSampleRefFrameTest(RotateSampleRefFrameTest&&) = delete;                 // Move Constructor Not Implemented
  RotateSampleRefFrameTest& operator=(const RotateSampleRefFrameTest&) = delete; // Copy Assignment Not Implemented
  RotateSampleRefFrameTest& operator=(RotateSampleRefFrameTest&&) = delete;      // Move Assignment Not Implemented

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::RotateSampleRefFrameTest::TestFile1);
    QFile::remove(UnitTest::RotateSampleRefFrameTest::TestFile2);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the SampleSurfaceMeshSpecifiedPointsTest Filter from the FilterManager
    FilterManager* filterManager = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = filterManager->getFactoryFromClassName(k_FilterName);
    if(filterFactory == nullptr)
    {
      std::stringstream ss;
      ss << "The RotateSampleRefFrameTest Requires the use of the " << k_FilterName.toStdString() << " filter which is found in the Sampling Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestFilterParameters()
  {
    const std::vector<size_t> tDims{10, 20, 30};
    const DataArrayPath path("DataContainer", "AttributeMatrix", "DataArray");

    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer dc = DataContainer::New(path.getDataContainerName());

    ImageGeom::Pointer imageGeom = ImageGeom::New();

    imageGeom->setDimensions(tDims);

    dc->setGeometry(imageGeom);

    dca->addOrReplaceDataContainer(dc);

    AttributeMatrix::Pointer matrix = dc->createNonPrereqAttributeMatrix(nullptr, path.getAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell);

    AbstractFilter::Pointer rotateFilter = createFilter();
    rotateFilter->setDataContainerArray(dca);

    setProperty(rotateFilter, k_CellAttributeMatrixPathName, path);
    setProperty(rotateFilter, k_RotationRepresentationName, 0);

    // Correct axis angle inputs

    setProperty(rotateFilter, k_RotationAxisName, FloatVec3Type(0.0f, 1.0f, 0.0f));
    setProperty(rotateFilter, k_RotationAngleName, 90.0f);

    rotateFilter->preflight();
    int error = rotateFilter->getErrorCode();
    DREAM3D_REQUIRED(error, >=, 0)

    resetGeometry(matrix, imageGeom, tDims);

    // Non-normalized rotation axis should generate a warning

    setProperty(rotateFilter, k_RotationAxisName, FloatVec3Type(1.0f, 1.0f, 0.0f));

    rotateFilter->preflight();
    int warning = rotateFilter->getWarningCode();
    DREAM3D_REQUIRED(warning, <, 0)

    resetGeometry(matrix, imageGeom, tDims);

    // Correct rotation matrix inputs

    setProperty(rotateFilter, k_RotationRepresentationName, 1);

    //  0 0 1
    //  0 1 0
    // -1 0 0

    std::vector<std::vector<double>> table = {{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}};

    DynamicTableData tableData(table);

    setProperty(rotateFilter, k_RotationTableName, tableData);

    rotateFilter->preflight();
    error = rotateFilter->getErrorCode();
    DREAM3D_REQUIRED(error, >=, 0)

    resetGeometry(matrix, imageGeom, tDims);

    // Inverse != Transpose error

    // 1 0 1
    // 0 1 0
    // 0 0 1

    table = {{1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};

    tableData.setTableData(table);

    setProperty(rotateFilter, k_RotationTableName, tableData);

    rotateFilter->preflight();
    error = rotateFilter->getErrorCode();
    DREAM3D_REQUIRED(error, <, 0)

    resetGeometry(matrix, imageGeom, tDims);

    // Determinant != 1 error

    // 1 0 1
    // 0 1 0
    // 1 0 1

    table = {{1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 1.0f}};

    tableData.setTableData(table);

    setProperty(rotateFilter, k_RotationTableName, tableData);

    rotateFilter->preflight();
    error = rotateFilter->getErrorCode();
    DREAM3D_REQUIRED(error, <, 0)

    resetGeometry(matrix, imageGeom, tDims);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestRotateSampleRefFrameTest()
  {
    const DataArrayPath basePath("", "CellData", "Data");
    const std::vector<size_t> cDims{1};

    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainerReader::Pointer dataContainerReader = DataContainerReader::New();
    dataContainerReader->setDataContainerArray(dca);
    dataContainerReader->setInputFile(UnitTest::RotateSampleRefFrameTest::TestFile1);
    DataContainerArrayProxy proxy = dataContainerReader->readDataContainerArrayStructure(UnitTest::RotateSampleRefFrameTest::TestFile1);
    dataContainerReader->setInputFileDataContainerArrayProxy(proxy);

    dataContainerReader->execute();
    int error = dataContainerReader->getErrorCode();
    DREAM3D_REQUIRED(error, >=, 0)

    QList<QString> dcNames = dca->getDataContainerNames();

    bool hasOriginal = dcNames.contains("Original");

    DREAM3D_REQUIRE(hasOriginal)

    AbstractFilter::Pointer rotateFilter = createFilter();
    rotateFilter->setDataContainerArray(dca);

    DataArrayPath path = basePath;
    path.setDataContainerName("Original");
    setProperty(rotateFilter, k_CellAttributeMatrixPathName, path);

    // Set to axis angle representation
    setProperty(rotateFilter, k_RotationRepresentationName, 0);

    bool foundRotated = false;

    for(const auto& name : dcNames)
    {
      bool isRotated = name.startsWith("Rotate");
      foundRotated = isRotated ? true : foundRotated;
      if(!isRotated)
      {
        continue;
      }

      DataArrayPath expectedArrayPath = basePath;
      path.setDataContainerName(name);

      UInt8ArrayType::Pointer expectedArray = dca->getPrereqArrayFromPath<UInt8ArrayType, AbstractFilter>(nullptr, expectedArrayPath, cDims);
      DREAM3D_REQUIRE_VALID_POINTER(expectedArray)

      QStringList parts = name.split("_");
      int size = parts.size();
      DREAM3D_REQUIRED(size, ==, 5)

      bool conversionSuccess = false;

      float x = parts[1].toFloat(&conversionSuccess);
      DREAM3D_REQUIRE(conversionSuccess)
      float y = parts[2].toFloat(&conversionSuccess);
      DREAM3D_REQUIRE(conversionSuccess)
      float z = parts[3].toFloat(&conversionSuccess);
      DREAM3D_REQUIRE(conversionSuccess)
      float angle = parts[4].toFloat(&conversionSuccess);
      DREAM3D_REQUIRE(conversionSuccess)

      setProperty(rotateFilter, k_RotationAngleName, angle);
      setProperty(rotateFilter, k_RotationAxisName, FloatVec3Type(x, y, z));

      rotateFilter->execute();
      int error = rotateFilter->getErrorCode();
      DREAM3D_REQUIRED(error, >=, 0)

      UInt8ArrayType::Pointer testArray = dca->getPrereqArrayFromPath<UInt8ArrayType, AbstractFilter>(nullptr, path, cDims);
      DREAM3D_REQUIRE_VALID_POINTER(testArray)

      bool arraysEqual = dataArrayEqual(*testArray, *expectedArray);
      DREAM3D_REQUIRE(arraysEqual)
    }

    DREAM3D_REQUIRE(foundRotated)
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "----Start RotateSampleRefFrameTest----\n";

    int err = EXIT_SUCCESS;
    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(TestFilterParameters())
    DREAM3D_REGISTER_TEST(TestRotateSampleRefFrameTest())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())

    std::cout << "----End RotateSampleRefFrameTest----\n";
  }
};
