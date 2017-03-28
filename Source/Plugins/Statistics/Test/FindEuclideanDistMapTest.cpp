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
#include <QtCore/QFile>

#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/TemplateHelpers.hpp"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "StatisticsTestFileLocations.h"

#define CREATE_DATA_ARRAY(type, attrMat, tDims, cDims, initVal, err)                                                                                                                                   \
  DataArray<type>::Pointer _##type##_##attrMat##Array = DataArray<type>::CreateArray(tDims, cDims, #type, true);                                                                                       \
  err = attrMat->addAttributeArray(#type, _##type##_##attrMat##Array);                                                                                                                                 \
  _##type##_##attrMat##Array->initializeWithValue(initVal);                                                                                                                                            \
  DREAM3D_REQUIRE(err >= 0);

#define SET_PROPERTIES_AND_CHECK_NE(filter, firstPath, secondPath, diffMapPath, errVal)                                                                                                                \
  var.setValue(firstPath);                                                                                                                                                                             \
  propWasSet = filter->setProperty("FirstInputArrayPath", var);                                                                                                                                        \
  if(false == propWasSet)                                                                                                                                                                              \
  {                                                                                                                                                                                                    \
    qDebug() << "Unable to set property FirstInputArrayPath";                                                                                                                                          \
  }                                                                                                                                                                                                    \
  var.setValue(secondPath);                                                                                                                                                                            \
  propWasSet = filter->setProperty("SecondInputArrayPath", var);                                                                                                                                       \
  if(false == propWasSet)                                                                                                                                                                              \
  {                                                                                                                                                                                                    \
    qDebug() << "Unable to set property SecondInputArrayPath";                                                                                                                                         \
  }                                                                                                                                                                                                    \
  var.setValue(diffMapPath);                                                                                                                                                                           \
  propWasSet = filter->setProperty("DifferenceMapArrayPath", var);                                                                                                                                     \
  if(false == propWasSet)                                                                                                                                                                              \
  {                                                                                                                                                                                                    \
    qDebug() << "Unable to set property DifferenceMapArrayPath";                                                                                                                                       \
  }                                                                                                                                                                                                    \
  filter->execute();                                                                                                                                                                                   \
  err = filter->getErrorCondition();                                                                                                                                                                   \
  DREAM3D_REQUIRE_EQUAL(err, errVal);

#define SET_PROPERTIES_AND_CHECK_EQ(filter, firstPath, secondPath, diffMapPath, type)                                                                                                                  \
  var.setValue(firstPath);                                                                                                                                                                             \
  propWasSet = filter->setProperty("FirstInputArrayPath", var);                                                                                                                                        \
  if(false == propWasSet)                                                                                                                                                                              \
  {                                                                                                                                                                                                    \
    qDebug() << "Unable to set property FirstInputArrayPath";                                                                                                                                          \
  }                                                                                                                                                                                                    \
  var.setValue(secondPath);                                                                                                                                                                            \
  propWasSet = filter->setProperty("SecondInputArrayPath", var);                                                                                                                                       \
  if(false == propWasSet)                                                                                                                                                                              \
  {                                                                                                                                                                                                    \
    qDebug() << "Unable to set property SecondInputArrayPath";                                                                                                                                         \
  }                                                                                                                                                                                                    \
  var.setValue(diffMapPath);                                                                                                                                                                           \
  propWasSet = filter->setProperty("DifferenceMapArrayPath", var);                                                                                                                                     \
  if(false == propWasSet)                                                                                                                                                                              \
  {                                                                                                                                                                                                    \
    qDebug() << "Unable to set property DifferenceMapArrayPath";                                                                                                                                       \
  }                                                                                                                                                                                                    \
  filter->execute();                                                                                                                                                                                   \
  err = filter->getErrorCondition();                                                                                                                                                                   \
  DREAM3D_REQUIRE_EQUAL(err, 0);                                                                                                                                                                       \
  diffMap = dc->getAttributeMatrix(diffMapPath.getAttributeMatrixName())->getAttributeArray(diffMapPath.getDataArrayName());                                                                           \
  firstArray = dc->getAttributeMatrix(firstPath.getAttributeMatrixName())->getAttributeArray(firstPath.getDataArrayName());                                                                            \
  if(!TemplateHelpers::CanDynamicCast<DataArray<type>>()(diffMap))                                                                                                                                     \
  {                                                                                                                                                                                                    \
    err = -1;                                                                                                                                                                                          \
  }                                                                                                                                                                                                    \
  DREAM3D_REQUIRE_EQUAL(err, 0);                                                                                                                                                                       \
  checkDims1 = firstArray->getComponentDimensions();                                                                                                                                                   \
  checkDims2 = diffMap->getComponentDimensions();                                                                                                                                                      \
  if(checkDims1 != checkDims2)                                                                                                                                                                         \
  {                                                                                                                                                                                                    \
    err = -1;                                                                                                                                                                                          \
  }                                                                                                                                                                                                    \
  DREAM3D_REQUIRE_EQUAL(err, 0);                                                                                                                                                                       \
  validateDiffMapValues<type>(diffMap);

const DataArrayPath k_FeatureIdsArrayPath = DataArrayPath("TestDataContainer1", "TestAttributeMatrix1", "FeatureIds");

class FindEuclideanDistMapTest
{
public:
  FindEuclideanDistMapTest()
  {
  }
  virtual ~FindEuclideanDistMapTest()
  {
  }
  SIMPL_TYPE_MACRO(FindEuclideanDistMapTest)

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
//    QFile::remove(UnitTest::FindEuclideanDistMapTest::TestFile1);
//    QFile::remove(UnitTest::FindEuclideanDistMapTest::TestFile2);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the FindEuclideanDistMap Filter from the FilterManager
    QString filtName = "FindEuclideanDistMap";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The FindEuclideanDistMapTest requires the use of the " << filtName.toStdString() << " filter which is found in Core Filters";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }

    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  DataContainerArray::Pointer initializeDataContainerArray()
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer m = DataContainer::New();
    m->setName(k_FeatureIdsArrayPath.getDataContainerName());
    ImageGeom::Pointer geom = ImageGeom::CreateGeometry("ImageGeometry");
    m->setGeometry(geom);

    // Create Attribute Matrices with different tDims to test validation of tuple compatibility
    QVector<size_t> tDims;
    tDims.push_back(13);
    tDims.push_back(13);
    AttributeMatrix::Pointer attrMat1 = AttributeMatrix::New(tDims, k_FeatureIdsArrayPath.getAttributeMatrixName(), AttributeMatrix::Type::Cell);

    m->addAttributeMatrix(k_FeatureIdsArrayPath.getAttributeMatrixName(), attrMat1);
    dca->addDataContainer(m);

    QVector<size_t> cDims(1, 1);

    Int32ArrayType::Pointer featureIds = Int32ArrayType::CreateArray(tDims, cDims, k_FeatureIdsArrayPath.getDataArrayName());
    int err = attrMat1->addAttributeArray(k_FeatureIdsArrayPath.getDataArrayName(), featureIds);
    DREAM3D_REQUIRE(err >= 0);
    featureIds->initializeWithValue(1);

    for (size_t i = 1; i < tDims[0] - 1; i++)
    {
      for (size_t j = 1; j < tDims[1] - 1; j++)
      {
        size_t index = tDims[0] * j + i;
        featureIds->setValue(index, 2);
      }
    }

    return dca;
  }

//  // -----------------------------------------------------------------------------
//  //
//  // -----------------------------------------------------------------------------
//  template <typename T> void validateDiffMapValues(IDataArray::Pointer iArray)
//  {
//    typename DataArray<T>::Pointer diffMapPtr = std::dynamic_pointer_cast<DataArray<T>>(iArray);
//    T* diffMap = diffMapPtr->getPointer(0);
//    size_t numTuples = diffMapPtr->getNumberOfTuples();
//    int32_t numComps = diffMapPtr->getNumberOfComponents();

//    for(size_t i = 0; i < numTuples; i++)
//    {
//      for(size_t j = 0; j < numComps; j++)
//      {
//        DREAM3D_REQUIRE_EQUAL(diffMap[numComps * i + j], 5)
//      }
//    }
//  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int RunTest()
  {
    DataContainerArray::Pointer dca = initializeDataContainerArray();

    QString filtName = "FindEuclideanDistMap";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer factory = fm->getFactoryForFilter(filtName);
    DREAM3D_REQUIRE(factory.get() != nullptr)

    AbstractFilter::Pointer filter = factory->create();
    DREAM3D_REQUIRE(filter.get() != nullptr)

    filter->setDataContainerArray(dca);

    QVariant var;
    var.setValue(k_FeatureIdsArrayPath);
    int err = filter->setProperty("FeatureIdsArrayPath", var);
    DREAM3D_REQUIRE(err >= 0);

    QString boundaryArrayName = "DistanceToBoundariesArray";
    var.setValue(boundaryArrayName);
    err = filter->setProperty("GBEuclideanDistancesArrayName", var);
    DREAM3D_REQUIRE(err >= 0);

//    filter->execute();
//    DREAM3D_REQUIRE(filter->getErrorCondition() >= 0);

//    // Failures
//    SET_PROPERTIES_AND_CHECK_NE(filter, attrMat1_bool, attrMat2_bool, diffMap1, -90000)

//    // Successes
//    SET_PROPERTIES_AND_CHECK_EQ(filter, attrMat1_uint8, attrMat2_uint8, diffMap1, uint8_t)

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "#### FindEuclideanDistMapTest Starting ####" << std::endl;

    int err = EXIT_SUCCESS;
    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(RunTest())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

private:
  FindEuclideanDistMapTest(const FindEuclideanDistMapTest&); // Copy Constructor Not Implemented
  void operator=(const FindEuclideanDistMapTest&);        // Operator '=' Not Implemented
};
