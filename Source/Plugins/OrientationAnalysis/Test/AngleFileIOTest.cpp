/* ============================================================================
 * Copyright (c) 2015 BlueQuartz Softwae, LLC
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>

#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"
#include "SIMPLib/Utilities/TestObserver.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"

#include "OrientationLib/IO/AngleFileLoader.h"

#include "OrientationAnalysis/OrientationAnalysisFilters/WriteStatsGenOdfAngleFile.h"

#include "OrientationAnalysisTestFileLocations.h"

namespace
{
static const int k_AngleCount = 100;
}

class AngleFileIOTest
{
public:
  AngleFileIOTest()
  {
  }
  virtual ~AngleFileIOTest()
  {
  }
  SIMPL_TYPE_MACRO(AngleFileIOTest)

  DataContainerArray::Pointer InitializeDataContainerArray()
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer m = DataContainer::New("AngleFileIO");

    dca->addDataContainer(m);

    // Create Attribute Matrices with different tDims to test validation of tuple compatibility
    QVector<size_t> tDims(1, k_AngleCount);
    AttributeMatrix::Pointer attrMat1 = AttributeMatrix::New(tDims, SIMPL::Defaults::CellAttributeMatrixName, AttributeMatrix::Type::Cell);
    m->addAttributeMatrix(attrMat1->getName(), attrMat1);

    QVector<size_t> cDims(1, 3);
    DataArray<float>::Pointer angles = DataArray<float>::CreateArray(k_AngleCount, cDims, SIMPL::CellData::EulerAngles, true);
    attrMat1->addAttributeArray(angles->getName(), angles);
    cDims[0] = 1;
    DataArray<int32_t>::Pointer phases = DataArray<int32_t>::CreateArray(k_AngleCount, cDims, SIMPL::CellData::Phases, true);
    attrMat1->addAttributeArray(phases->getName(), phases);
    for(int i = 0; i < k_AngleCount; i++)
    {

      float angle = 360.0 / k_AngleCount * i;
      angles->setComponent(i, 0, angle);

      angles->setComponent(i, 2, angle);
      angle = 180.0 / k_AngleCount * i;
      angles->setComponent(i, 1, angle);

      phases->setValue(i, 1);
    }

    return dca;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestWriter()
  {

    TestObserver obs;

    DataContainerArray::Pointer dca = InitializeDataContainerArray();

    AbstractFilter::Pointer filter = AbstractFilter::NullPointer();
    QString filtName = "WriteStatsGenOdfAngleFile";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);

    DREAM3D_REQUIRE_VALID_POINTER(filterFactory.get())

    filter = filterFactory->create();
    DREAM3D_REQUIRE_VALID_POINTER(filter.get())
    filter->setDataContainerArray(dca);
    filter->connect(filter.get(), SIGNAL(filterGeneratedMessage(const PipelineMessage&)), &obs, SLOT(processPipelineMessage(const PipelineMessage&)));

    QVariant var;
    bool propWasSet = filter->setProperty("OutputFile", UnitTest::AngleFileIOTest::OutputFile);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    DataArrayPath dap("AngleFileIO", SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::EulerAngles);
    var.setValue(dap);
    propWasSet = filter->setProperty("CellEulerAnglesArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    dap.setDataArrayName(SIMPL::CellData::Phases);
    var.setValue(dap);
    propWasSet = filter->setProperty("CellPhasesArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    bool setToDegrees = false;
    propWasSet = filter->setProperty("ConvertToDegrees", QVariant(setToDegrees));
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    setToDegrees = false;
    propWasSet = filter->setProperty("UseGoodVoxels", QVariant(setToDegrees));
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    filter->preflight();
    int32_t err = filter->getErrorCondition();

    DREAM3D_REQUIRED(err, >=, 0)

    filter->execute();
    err = filter->getErrorCondition();

    DREAM3D_REQUIRE(err >= 0)
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestReader()
  {
    AngleFileLoader::Pointer reader = AngleFileLoader::New();

    QFileInfo fi(UnitTest::AngleFileIOTest::OutputFile);
    QString absPath = fi.absolutePath();
    QString fname = fi.completeBaseName();
    QString suffix = fi.suffix();

    QString absFilePath = absPath + "/" + fname + "_Phase_" + QString::number(1) + "." + suffix;

    reader->setInputFile(absFilePath);
    reader->setAngleRepresentation(AngleFileLoader::EulerAngles);
    reader->setDelimiter(QString(" "));
    reader->setFileAnglesInDegrees(true);
    reader->setOutputAnglesInDegrees(true);

    DataArray<float>::Pointer angles = reader->loadData();
    if(reader->getErrorCode() < 0)
    {
      qDebug() << reader->getErrorMessage();
    }
    DREAM3D_REQUIRED(reader->getErrorCode(), >=, 0)
    DREAM3D_REQUIRE_VALID_POINTER(angles.get())

    DREAM3D_REQUIRED(angles->getNumberOfTuples(), ==, k_AngleCount)
    DREAM3D_REQUIRED(angles->getNumberOfComponents(), ==, 5)

    for(int i = 0; i < 100; i++)
    {
      float angle = 360.0 / 100.0 * i;
      DREAM3D_REQUIRED(angle, ==, angles->getComponent(i, 0));
      DREAM3D_REQUIRED(angle, ==, angles->getComponent(i, 2));

      angle = 180.0 / 100.0 * i;
      DREAM3D_REQUIRED(angle, ==, angles->getComponent(i, 1));
    }
  }

  /**
  * @brief
  */
  void operator()()
  {
    int err = EXIT_SUCCESS;
    DREAM3D_REGISTER_TEST(TestWriter());
    DREAM3D_REGISTER_TEST(TestReader());
  }

private:
  AngleFileIOTest(const AngleFileIOTest&); // Copy Constructor Not Implemented
  void operator=(const AngleFileIOTest&);  // Operator '=' Not Implemented
};
