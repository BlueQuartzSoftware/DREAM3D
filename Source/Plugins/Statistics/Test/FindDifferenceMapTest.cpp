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

class FindDifferenceMapTest
{
public:
  FindDifferenceMapTest()
  {
  }
  virtual ~FindDifferenceMapTest()
  {
  }
  SIMPL_TYPE_MACRO(FindDifferenceMapTest)

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::FindDifferenceMapTest::TestFile1);
    QFile::remove(UnitTest::FindDifferenceMapTest::TestFile2);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the FindDifferenceMapTest Filter from the FilterManager
    QString filtName = "FindDifferenceMap";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The FindDifferenceMapTest Requires the use of the " << filtName.toStdString() << " filter which is found in the Statistics Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  DataContainerArray::Pointer initializeDataContainerArray()
  {
    int err = 0;

    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer m = DataContainer::New();
    m->setName("FindDifferenceMapTest");

    // Create Attribute Matrices with different tDims to test validation of tuple compatibility
    QVector<size_t> tDims(1, 100);
    AttributeMatrix::Pointer attrMat1 = AttributeMatrix::New(tDims, "DiffMapTestAttrMat1", 3);
    AttributeMatrix::Pointer attrMat2 = AttributeMatrix::New(tDims, "DiffMapTestAttrMat2", 3);
    AttributeMatrix::Pointer attrMat11 = AttributeMatrix::New(tDims, "DiffMapTestAttrMat11", 3);
    AttributeMatrix::Pointer attrMat22 = AttributeMatrix::New(tDims, "DiffMapTestAttrMat22", 3);

    tDims[0] = 10;
    AttributeMatrix::Pointer attrMat3 = AttributeMatrix::New(tDims, "DiffMapTestAttrMat3", 3);

    m->addAttributeMatrix("DiffMapTestAttrMat1", attrMat1);
    m->addAttributeMatrix("DiffMapTestAttrMat2", attrMat2);
    m->addAttributeMatrix("DiffMapTestAttrMat11", attrMat11);
    m->addAttributeMatrix("DiffMapTestAttrMat22", attrMat22);
    m->addAttributeMatrix("DiffMapTestAttrMat3", attrMat3);
    dca->addDataContainer(m);

    QVector<size_t> cDims(1, 3);
    int32_t initVal = 10;
    tDims[0] = 100;

    CREATE_DATA_ARRAY(uint8_t, attrMat1, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(int8_t, attrMat1, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(uint16_t, attrMat1, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(int16_t, attrMat1, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(uint32_t, attrMat1, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(int32_t, attrMat1, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(uint64_t, attrMat1, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(int64_t, attrMat1, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(double, attrMat1, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(float, attrMat1, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(bool, attrMat1, tDims, cDims, true, err);

    cDims[0] = 1;

    CREATE_DATA_ARRAY(uint8_t, attrMat11, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(int8_t, attrMat11, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(uint16_t, attrMat11, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(int16_t, attrMat11, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(uint32_t, attrMat11, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(int32_t, attrMat11, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(uint64_t, attrMat11, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(int64_t, attrMat11, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(double, attrMat11, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(float, attrMat11, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(bool, attrMat11, tDims, cDims, true, err);

    initVal = 5;
    cDims[0] = 3;

    CREATE_DATA_ARRAY(uint8_t, attrMat2, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(int8_t, attrMat2, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(uint16_t, attrMat2, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(int16_t, attrMat2, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(uint32_t, attrMat2, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(int32_t, attrMat2, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(uint64_t, attrMat2, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(int64_t, attrMat2, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(double, attrMat2, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(float, attrMat2, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(bool, attrMat2, tDims, cDims, false, err);

    cDims[0] = 1;

    CREATE_DATA_ARRAY(uint8_t, attrMat22, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(int8_t, attrMat22, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(uint16_t, attrMat22, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(int16_t, attrMat22, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(uint32_t, attrMat22, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(int32_t, attrMat22, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(uint64_t, attrMat22, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(int64_t, attrMat22, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(double, attrMat22, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(float, attrMat22, tDims, cDims, initVal, err);
    CREATE_DATA_ARRAY(bool, attrMat22, tDims, cDims, true, err);

    tDims[0] = 10;

    CREATE_DATA_ARRAY(float, attrMat3, tDims, cDims, initVal, err)

    return dca;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T> void validateDiffMapValues(IDataArray::Pointer iArray)
  {
    typename DataArray<T>::Pointer diffMapPtr = std::dynamic_pointer_cast<DataArray<T>>(iArray);
    T* diffMap = diffMapPtr->getPointer(0);
    size_t numTuples = diffMapPtr->getNumberOfTuples();
    int32_t numComps = diffMapPtr->getNumberOfComponents();

    for(size_t i = 0; i < numTuples; i++)
    {
      for(size_t j = 0; j < numComps; j++)
      {
        DREAM3D_REQUIRE_EQUAL(diffMap[numComps * i + j], 5)
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void removeArrayFromDataContainerArray(DataContainerArray::Pointer dca, DataArrayPath path)
  {
    dca->getDataContainer(path.getDataContainerName())->getAttributeMatrix(path.getAttributeMatrixName())->removeAttributeArray(path.getDataArrayName());
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void validateFindDifferenceMap(AbstractFilter::Pointer filter, DataContainerArray::Pointer dca)
  {
    QVariant var;
    bool propWasSet;
    int err = 0;
    QString diffMapName = "DifferenceMap";

    DataContainer::Pointer dc = dca->getDataContainer("FindDifferenceMapTest");
    QVector<size_t> checkDims1;
    QVector<size_t> checkDims2;
    IDataArray::Pointer diffMap;
    IDataArray::Pointer firstArray;

    DataArrayPath attrMat1_uint8("FindDifferenceMapTest", "DiffMapTestAttrMat1", "uint8_t");
    DataArrayPath attrMat1_int8("FindDifferenceMapTest", "DiffMapTestAttrMat1", "int8_t");
    DataArrayPath attrMat1_uint16("FindDifferenceMapTest", "DiffMapTestAttrMat1", "uint16_t");
    DataArrayPath attrMat1_int16("FindDifferenceMapTest", "DiffMapTestAttrMat1", "int16_t");
    DataArrayPath attrMat1_uint32("FindDifferenceMapTest", "DiffMapTestAttrMat1", "uint32_t");
    DataArrayPath attrMat1_int32("FindDifferenceMapTest", "DiffMapTestAttrMat1", "int32_t");
    DataArrayPath attrMat1_uint64("FindDifferenceMapTest", "DiffMapTestAttrMat1", "uint64_t");
    DataArrayPath attrMat1_int64("FindDifferenceMapTest", "DiffMapTestAttrMat1", "int64_t");
    DataArrayPath attrMat1_float("FindDifferenceMapTest", "DiffMapTestAttrMat1", "float");
    DataArrayPath attrMat1_double("FindDifferenceMapTest", "DiffMapTestAttrMat1", "double");
    DataArrayPath attrMat1_bool("FindDifferenceMapTest", "DiffMapTestAttrMat1", "bool");

    DataArrayPath attrMat2_uint8("FindDifferenceMapTest", "DiffMapTestAttrMat2", "uint8_t");
    DataArrayPath attrMat2_int8("FindDifferenceMapTest", "DiffMapTestAttrMat2", "int8_t");
    DataArrayPath attrMat2_uint16("FindDifferenceMapTest", "DiffMapTestAttrMat2", "uint16_t");
    DataArrayPath attrMat2_int16("FindDifferenceMapTest", "DiffMapTestAttrMat2", "int16_t");
    DataArrayPath attrMat2_uint32("FindDifferenceMapTest", "DiffMapTestAttrMat2", "uint32_t");
    DataArrayPath attrMat2_int32("FindDifferenceMapTest", "DiffMapTestAttrMat2", "int32_t");
    DataArrayPath attrMat2_uint64("FindDifferenceMapTest", "DiffMapTestAttrMat2", "uint64_t");
    DataArrayPath attrMat2_int64("FindDifferenceMapTest", "DiffMapTestAttrMat2", "int64_t");
    DataArrayPath attrMat2_float("FindDifferenceMapTest", "DiffMapTestAttrMat2", "float");
    DataArrayPath attrMat2_double("FindDifferenceMapTest", "DiffMapTestAttrMat2", "double");
    DataArrayPath attrMat2_bool("FindDifferenceMapTest", "DiffMapTestAttrMat2", "bool");

    DataArrayPath attrMat11_uint8("FindDifferenceMapTest", "DiffMapTestAttrMat11", "uint8_t");
    DataArrayPath attrMat11_int8("FindDifferenceMapTest", "DiffMapTestAttrMat11", "int8_t");
    DataArrayPath attrMat11_uint16("FindDifferenceMapTest", "DiffMapTestAttrMat11", "uint16_t");
    DataArrayPath attrMat11_int16("FindDifferenceMapTest", "DiffMapTestAttrMat11", "int16_t");
    DataArrayPath attrMat11_uint32("FindDifferenceMapTest", "DiffMapTestAttrMat11", "uint32_t");
    DataArrayPath attrMat11_int32("FindDifferenceMapTest", "DiffMapTestAttrMat11", "int32_t");
    DataArrayPath attrMat11_uint64("FindDifferenceMapTest", "DiffMapTestAttrMat11", "uint64_t");
    DataArrayPath attrMat11_int64("FindDifferenceMapTest", "DiffMapTestAttrMat11", "int64_t");
    DataArrayPath attrMat11_float("FindDifferenceMapTest", "DiffMapTestAttrMat11", "float");
    DataArrayPath attrMat11_double("FindDifferenceMapTest", "DiffMapTestAttrMat11", "double");

    DataArrayPath attrMat22_uint8("FindDifferenceMapTest", "DiffMapTestAttrMat22", "uint8_t");
    DataArrayPath attrMat22_int8("FindDifferenceMapTest", "DiffMapTestAttrMat22", "int8_t");
    DataArrayPath attrMat22_uint16("FindDifferenceMapTest", "DiffMapTestAttrMat22", "uint16_t");
    DataArrayPath attrMat22_int16("FindDifferenceMapTest", "DiffMapTestAttrMat22", "int16_t");
    DataArrayPath attrMat22_uint32("FindDifferenceMapTest", "DiffMapTestAttrMat22", "uint32_t");
    DataArrayPath attrMat22_int32("FindDifferenceMapTest", "DiffMapTestAttrMat22", "int32_t");
    DataArrayPath attrMat22_uint64("FindDifferenceMapTest", "DiffMapTestAttrMat22", "uint64_t");
    DataArrayPath attrMat22_int64("FindDifferenceMapTest", "DiffMapTestAttrMat22", "int64_t");
    DataArrayPath attrMat22_float("FindDifferenceMapTest", "DiffMapTestAttrMat22", "float");
    DataArrayPath attrMat22_double("FindDifferenceMapTest", "DiffMapTestAttrMat22", "double");

    DataArrayPath diffMap1("FindDifferenceMapTest", "DiffMapTestAttrMat1", diffMapName);
    DataArrayPath diffMap2("FindDifferenceMapTest", "DiffMapTestAttrMat3", diffMapName);

    // Fail if an input array is bool
    SET_PROPERTIES_AND_CHECK_NE(filter, attrMat1_bool, attrMat2_bool, diffMap1, -90000)
    removeArrayFromDataContainerArray(dca, diffMap1);

    // Fail if input arrays are of different type
    SET_PROPERTIES_AND_CHECK_NE(filter, attrMat1_uint8, attrMat2_float, diffMap1, -90001)
    removeArrayFromDataContainerArray(dca, diffMap1);

    // Fail if tuples do not match up
    SET_PROPERTIES_AND_CHECK_NE(filter, attrMat1_uint8, attrMat2_uint8, diffMap2, -10200)
    removeArrayFromDataContainerArray(dca, diffMap2);

    // Fail if input arrays have different numbers of components
    SET_PROPERTIES_AND_CHECK_NE(filter, attrMat1_uint8, attrMat11_uint8, diffMap1, -90003)
    removeArrayFromDataContainerArray(dca, diffMap1);

    // Succeed for all possible test combinations

    // vectors
    SET_PROPERTIES_AND_CHECK_EQ(filter, attrMat1_uint8, attrMat2_uint8, diffMap1, uint8_t)
    removeArrayFromDataContainerArray(dca, diffMap1);
    SET_PROPERTIES_AND_CHECK_EQ(filter, attrMat1_int8, attrMat2_int8, diffMap1, int8_t)
    removeArrayFromDataContainerArray(dca, diffMap1);
    SET_PROPERTIES_AND_CHECK_EQ(filter, attrMat1_uint16, attrMat2_uint16, diffMap1, uint16_t)
    removeArrayFromDataContainerArray(dca, diffMap1);
    SET_PROPERTIES_AND_CHECK_EQ(filter, attrMat1_int16, attrMat2_int16, diffMap1, int16_t)
    removeArrayFromDataContainerArray(dca, diffMap1);
    SET_PROPERTIES_AND_CHECK_EQ(filter, attrMat1_uint32, attrMat2_uint32, diffMap1, uint32_t)
    removeArrayFromDataContainerArray(dca, diffMap1);
    SET_PROPERTIES_AND_CHECK_EQ(filter, attrMat1_int32, attrMat2_int32, diffMap1, int32_t)
    removeArrayFromDataContainerArray(dca, diffMap1);
    SET_PROPERTIES_AND_CHECK_EQ(filter, attrMat1_uint64, attrMat2_uint64, diffMap1, uint64_t)
    removeArrayFromDataContainerArray(dca, diffMap1);
    SET_PROPERTIES_AND_CHECK_EQ(filter, attrMat1_int64, attrMat2_int64, diffMap1, int64_t)
    removeArrayFromDataContainerArray(dca, diffMap1);
    SET_PROPERTIES_AND_CHECK_EQ(filter, attrMat1_float, attrMat2_float, diffMap1, float)
    removeArrayFromDataContainerArray(dca, diffMap1);
    SET_PROPERTIES_AND_CHECK_EQ(filter, attrMat1_double, attrMat2_double, diffMap1, double)
    removeArrayFromDataContainerArray(dca, diffMap1);

    // scalars
    SET_PROPERTIES_AND_CHECK_EQ(filter, attrMat11_uint8, attrMat22_uint8, diffMap1, uint8_t)
    removeArrayFromDataContainerArray(dca, diffMap1);
    SET_PROPERTIES_AND_CHECK_EQ(filter, attrMat11_int8, attrMat22_int8, diffMap1, int8_t)
    removeArrayFromDataContainerArray(dca, diffMap1);
    SET_PROPERTIES_AND_CHECK_EQ(filter, attrMat11_uint16, attrMat22_uint16, diffMap1, uint16_t)
    removeArrayFromDataContainerArray(dca, diffMap1);
    SET_PROPERTIES_AND_CHECK_EQ(filter, attrMat11_int16, attrMat22_int16, diffMap1, int16_t)
    removeArrayFromDataContainerArray(dca, diffMap1);
    SET_PROPERTIES_AND_CHECK_EQ(filter, attrMat11_uint32, attrMat22_uint32, diffMap1, uint32_t)
    removeArrayFromDataContainerArray(dca, diffMap1);
    SET_PROPERTIES_AND_CHECK_EQ(filter, attrMat11_int32, attrMat22_int32, diffMap1, int32_t)
    removeArrayFromDataContainerArray(dca, diffMap1);
    SET_PROPERTIES_AND_CHECK_EQ(filter, attrMat11_uint64, attrMat22_uint64, diffMap1, uint64_t)
    removeArrayFromDataContainerArray(dca, diffMap1);
    SET_PROPERTIES_AND_CHECK_EQ(filter, attrMat11_int64, attrMat22_int64, diffMap1, int64_t)
    removeArrayFromDataContainerArray(dca, diffMap1);
    SET_PROPERTIES_AND_CHECK_EQ(filter, attrMat11_float, attrMat22_float, diffMap1, float)
    removeArrayFromDataContainerArray(dca, diffMap1);
    SET_PROPERTIES_AND_CHECK_EQ(filter, attrMat11_double, attrMat22_double, diffMap1, double)
    removeArrayFromDataContainerArray(dca, diffMap1);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int RunTest()
  {
    DataContainerArray::Pointer dca = initializeDataContainerArray();

    QString filtName = "FindDifferenceMap";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer factory = fm->getFactoryForFilter(filtName);
    DREAM3D_REQUIRE(factory.get() != nullptr)

    AbstractFilter::Pointer diffMapFilter = factory->create();
    DREAM3D_REQUIRE(diffMapFilter.get() != nullptr)

    diffMapFilter->setDataContainerArray(dca);

    validateFindDifferenceMap(diffMapFilter, dca);

    return EXIT_SUCCESS;
  }
  /**
* @brief
*/
  void operator()()
  {
    int err = EXIT_SUCCESS;
    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(RunTest())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

private:
  FindDifferenceMapTest(const FindDifferenceMapTest&); // Copy Constructor Not Implemented
  void operator=(const FindDifferenceMapTest&);        // Operator '=' Not Implemented
};
