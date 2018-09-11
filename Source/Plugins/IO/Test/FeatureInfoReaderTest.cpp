/* ============================================================================
 * Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFile>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"

#include "SIMPLib/CoreFilters/CreateDataArray.h"
#include "SIMPLib/CoreFilters/CreateDataContainer.h"
#include "SIMPLib/CoreFilters/CreateGeometry.h"
#include "SIMPLib/CoreFilters/DataContainerWriter.h"

#include "UnitTestSupport.hpp"

#include "IOTestFileLocations.h"

class FeatureInfoReaderTest
{
public:
  FeatureInfoReaderTest()
  {
  }
  virtual ~FeatureInfoReaderTest()
  {
  }
  SIMPL_TYPE_MACRO(FeatureInfoReaderTest)

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::FeatureInfoReaderTest::InputFile);
    QFile::remove(UnitTest::FeatureInfoReaderTest::OutputFile);
    QFile::remove(UnitTest::FeatureInfoReaderTest::OutputFileXdmf);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the DxWriter Filter from the FilterManager
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(m_FilterName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The FeatureInfoReaderTest Requires the use of the " << m_FilterName.toStdString() << " filter which is found in the IO Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }

    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void WriteInputFile(bool comments, char delimeter)
  {

    static const char nl = '\n';
    QFile file(UnitTest::FeatureInfoReaderTest::InputFile);
    bool didOpen = file.open(QIODevice::WriteOnly | QIODevice::Text);
    DREAM3D_REQUIRE(didOpen == true)

    QTextStream ss(&file);
    
    if(comments) { ss << "# Comment" << nl; }
    ss << m_NumFeatures << nl;

    if(comments) { ss << "# Header Row" << nl; }

    int32_t phase = 1;
    float phi1 = 0.0f, phi = 1.0f, phi2 = 2.0f;
    for(int32_t featureId = 1; featureId <= m_NumFeatures; featureId++)
    {
      ss << featureId << delimeter << phase << delimeter << phi1 << delimeter << phi << delimeter << phi2 << nl;
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestFilter()
  {
    static const int32_t k_ImageGeomType = 0;
    static const QString k_DataContainerName("DataContainer");
    static const QString k_CellAMName("CelLData");
    static const QString k_FeatureIdsName("FeatureIds");


    FilterPipeline::Pointer pipeline = FilterPipeline::New();

    CreateDataContainer::Pointer createDataContainer = CreateDataContainer::New();
    createDataContainer->setDataContainerName(k_DataContainerName);
    pipeline->pushBack(createDataContainer);

    CreateGeometry::Pointer createGeom = CreateGeometry::New();
    createGeom->setGeometryType(k_ImageGeomType);
    IntVec3_t dims;
    dims.IntVec3(64, 64, 64);
    createGeom->setDimensions(dims);
    createGeom->setDataContainerName(k_DataContainerName);
    createGeom->setImageCellAttributeMatrixName(k_CellAMName);
    pipeline->pushBack(createGeom);

    CreateDataArray::Pointer createDataArray = CreateDataArray::New();
    createDataArray->setScalarType(SIMPL::ScalarTypes::Type::Int32);
    createDataArray->setNumberOfComponents(1);
    createDataArray->setInitializationType(0);
    createDataArray->setInitializationValue(QString::number(m_NumFeatures));
    createDataArray->setNewArray(DataArrayPath(k_DataContainerName, k_CellAMName, k_FeatureIdsName));
    pipeline->pushBack(createDataArray);

    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(m_FilterName);
    AbstractFilter::Pointer featureInfoReader = filterFactory->create();

    QVariant var;

    DataArrayPath dap(k_DataContainerName, k_CellAMName, "");
    var.setValue(dap);
    bool propWasSet = featureInfoReader->setProperty("CellAttributeMatrixName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(UnitTest::FeatureInfoReaderTest::InputFile);
    propWasSet = featureInfoReader->setProperty("InputFile", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(true);
    propWasSet = featureInfoReader->setProperty("CreateCellLevelArrays", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(true);
    propWasSet = featureInfoReader->setProperty("RenumberFeatures", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap = DataArrayPath(k_DataContainerName, k_CellAMName, k_FeatureIdsName);
    var.setValue(dap);
    propWasSet = featureInfoReader->setProperty("FeatureIdsArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(QString("Phases"));
    propWasSet = featureInfoReader->setProperty("CellPhasesArrayName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(QString("EulerAngles"));
    propWasSet = featureInfoReader->setProperty("CellEulerAnglesArrayName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(QString("Phases"));
    propWasSet = featureInfoReader->setProperty("FeaturePhasesArrayName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(QString("EulerAngles"));
    propWasSet = featureInfoReader->setProperty("FeatureEulerAnglesArrayName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(QString("FeatureAttributeMatrix"));
    propWasSet = featureInfoReader->setProperty("CellFeatureAttributeMatrixName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(0);
    propWasSet = featureInfoReader->setProperty("Delimiter", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    pipeline->pushBack(featureInfoReader);
    
    DataContainerWriter::Pointer writer = DataContainerWriter::New();
    writer->setOutputFile(UnitTest::FeatureInfoReaderTest::OutputFile);
    pipeline->pushBack(writer);

    // Try an input file with Comments and comma delimited while in comma mode
    WriteInputFile(true, ',');
    var.setValue(0);
    featureInfoReader->setProperty("Delimiter", var);
    int err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    DataContainerArray::Pointer dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >= , 0)

    // Try an input file with Comments and semicolon delimited while in semicolon mode
    WriteInputFile(true, ';');
    var.setValue(1);
    featureInfoReader->setProperty("Delimiter", var);
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >= , 0)

    // Try an input file with Comments and colon delimited while in colon mode
    WriteInputFile(true, ':');
    var.setValue(2);
    featureInfoReader->setProperty("Delimiter", var);
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >= , 0)

    // Try an input file with Comments and tab delimited while in tab mode
    WriteInputFile(true, '\t');
    var.setValue(3);
    featureInfoReader->setProperty("Delimiter", var);
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >=, 0)

    // Try an input file with Comments and space delimited while in space mode
    WriteInputFile(true, ' ');
    var.setValue(4);
    featureInfoReader->setProperty("Delimiter", var);
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >=, 0)

    // Try an input file without Comments and comma delimited while in comma mode
    WriteInputFile(false, ',');
    var.setValue(0);
    featureInfoReader->setProperty("Delimiter", var);
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >= , 0)

    // Try an input file without Comments and semicolon delimited while in semicolon mode
    WriteInputFile(false, ';');
    var.setValue(1);
    featureInfoReader->setProperty("Delimiter", var);
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >= , 0)

    // Try an input file without Comments and colon delimited while in colon mode
    WriteInputFile(false, ':');
    var.setValue(2);
    featureInfoReader->setProperty("Delimiter", var);
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >= , 0)

    // Try an input file without Comments and tab delimited while in tab mode
    WriteInputFile(false, '\t');
    var.setValue(3);
    featureInfoReader->setProperty("Delimiter", var);
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >=, 0)

    // Try an input file without Comments and space delimited while in space mode
    WriteInputFile(false, ' ');
    var.setValue(4);
    featureInfoReader->setProperty("Delimiter", var);
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >=, 0)

    // Try an input file without Comments and comma delimited while in semicolon mode which will induce a runtime error
    WriteInputFile(false, ',');
    var.setValue(1);
    featureInfoReader->setProperty("Delimiter", var);
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >= , -68001)

    // Try an input file without Comments and comma delimited while in colon mode which will induce a runtime error
    WriteInputFile(false, ',');
    var.setValue(2);
    featureInfoReader->setProperty("Delimiter", var);
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >= , -68001)

    // Try an input file without Comments and comma delimited while in tab mode which will induce a runtime error
    WriteInputFile(false, ',');
    var.setValue(3);
    featureInfoReader->setProperty("Delimiter", var);
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >= , -68001)

    // Try an input file without Comments and comma delimited while in space mode which will induce a runtime error
    WriteInputFile(false, ',');
    var.setValue(4);
    featureInfoReader->setProperty("Delimiter", var);
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >= , -68001)

    // Try an input file without Comments and semicolon delimited while in comma mode which will induce a runtime error
    WriteInputFile(false, ';');
    var.setValue(0);
    featureInfoReader->setProperty("Delimiter", var);
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >= , -68001)

    // Try an input file without Comments and semicolon delimited while in colon mode which will induce a runtime error
    WriteInputFile(false, ';');
    var.setValue(2);
    featureInfoReader->setProperty("Delimiter", var);
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >= , -68001)

    // Try an input file without Comments and semicolon delimited while in tab mode which will induce a runtime error
    WriteInputFile(false, ';');
    var.setValue(3);
    featureInfoReader->setProperty("Delimiter", var);
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >= , -68001)

    // Try an input file without Comments and semicolon delimited while in space mode which will induce a runtime error
    WriteInputFile(false, ';');
    var.setValue(4);
    featureInfoReader->setProperty("Delimiter", var);
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >= , -68001)

    // Try an input file without Comments and colon delimited while in comma mode which will induce a runtime error
    WriteInputFile(false, ':');
    var.setValue(0);
    featureInfoReader->setProperty("Delimiter", var);
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >= , -68001)

    // Try an input file without Comments and colon delimited while in semicolon mode which will induce a runtime error
    WriteInputFile(false, ':');
    var.setValue(1);
    featureInfoReader->setProperty("Delimiter", var);
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >= , -68001)

    // Try an input file without Comments and colon delimited while in tab mode which will induce a runtime error
    WriteInputFile(false, ':');
    var.setValue(3);
    featureInfoReader->setProperty("Delimiter", var);
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >= , -68001)

    // Try an input file without Comments and colon delimited while in space mode which will induce a runtime error
    WriteInputFile(false, ':');
    var.setValue(4);
    featureInfoReader->setProperty("Delimiter", var);
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >= , -68001)

    // Try an input file without Comments and tab delimited while in comma mode which will induce a runtime error
    WriteInputFile(false, '\t');
    var.setValue(0);
    featureInfoReader->setProperty("Delimiter", var);
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >= , -68001)

    // Try an input file without Comments and tab delimited while in semicolon mode which will induce a runtime error
    WriteInputFile(false, '\t');
    var.setValue(1);
    featureInfoReader->setProperty("Delimiter", var);
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >= , -68001)

    // Try an input file without Comments and tab delimited while in colon mode which will induce a runtime error
    WriteInputFile(false, '\t');
    var.setValue(2);
    featureInfoReader->setProperty("Delimiter", var);
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >= , -68001)


    // Try an input file without Comments and tab delimited while in space mode which will induce a runtime error
    WriteInputFile(false, '\t');
    var.setValue(4);
    featureInfoReader->setProperty("Delimiter", var);
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >= , -68001)

    // Try an input file without Comments and space delimited while in comma mode which will induce a runtime error
    WriteInputFile(false, ' ');
    var.setValue(0);
    featureInfoReader->setProperty("Delimiter", var);
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >= , -68001)

    // Try an input file without Comments and space delimited while in semicolon mode which will induce a runtime error
    WriteInputFile(false, ' ');
    var.setValue(1);
    featureInfoReader->setProperty("Delimiter", var);
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >= , -68001)

    // Try an input file without Comments and space delimited while in colon mode which will induce a runtime error
    WriteInputFile(false, ' ');
    var.setValue(2);
    featureInfoReader->setProperty("Delimiter", var);
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >= , -68001)


    // Try an input file without Comments and space delimited while in tab mode which will induce a runtime error
    WriteInputFile(false, ' ');
    var.setValue(3);
    featureInfoReader->setProperty("Delimiter", var);
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRE(err >= 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, >= , -68001)

    // Change the init value to 10 so that we induce a runtime error
    WriteInputFile(false, ' ');
    var.setValue(4);
    featureInfoReader->setProperty("Delimiter", var);
    createDataArray->setInitializationValue(QString::number(10));
    err = pipeline->preflightPipeline();
    DREAM3D_REQUIRED(err, >=, 0)

    dca = pipeline->execute();
    err = pipeline->getErrorCondition();
    DREAM3D_REQUIRED(err, ==, -68000)
  }

  /**
   * @brief
   */
  void operator()()
  {
    int err = EXIT_SUCCESS;
    std::cout << "<===== Start " << getNameOfClass().toStdString() << std::endl;
    
    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(TestFilter())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

private:
  int32_t m_NumFeatures = 8;
  QString m_FilterName = QString("FeatureInfoReader");

public:
  FeatureInfoReaderTest(const FeatureInfoReaderTest&); // Copy Constructor Not Implemented
  void operator=(const FeatureInfoReaderTest&);        // Move assignment Not Implemented
};
