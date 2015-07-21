/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/Common/FilterPipeline.h"
#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/FilterFactory.hpp"
#include "DREAM3DLib/Plugin/IDREAM3DPlugin.h"
#include "DREAM3DLib/Plugin/DREAM3DPluginLoader.h"
#include "DREAM3DLib/Utilities/UnitTestSupport.hpp"
#include "DREAM3DLib/Utilities/QMetaObjectUtilities.h"

#include "StatisticsTestFileLocations.h"

#define REMOVE_ARRAY(attrMatPath, array)\
  filter->getDataContainerArray()->getAttributeMatrix(attrMatPath)->removeAttributeArray(#array);\

#define CREATE_DATA_ARRAY(type, attrMat, tDims, cDims, initVal, err)\
  DataArray<type>::Pointer _##type##_##attrMat##Array = DataArray<type>::CreateArray(tDims, cDims, #type, true);\
  err = attrMat->addAttributeArray(#type, _##type##_##attrMat##Array);\
  _##type##_##attrMat##Array->initializeWithValue(initVal);\
  DREAM3D_REQUIRE(err >= 0);

#define SET_PROPERTIES_AND_CHECK_NE(filter, firstPath, secondPath, diffMapPath)\
  var.setValue(firstPath);\
  propWasSet = filter->setProperty("FirstInputArrayPath", var);\
  if(false == propWasSet)\
  {\
    qDebug() << "Unable to set property FirstInputArrayPath";\
  }\
  var.setValue(secondPath);\
  propWasSet = filter->setProperty("SecondInputArrayPath", var);\
  if(false == propWasSet)\
  {\
    qDebug() << "Unable to set property SecondInputArrayPath";\
  }\
  var.setValue(secondPath);\
  propWasSet = filter->setProperty("DifferenceMapArrayPath", var);\
  if(false == propWasSet)\
  {\
    qDebug() << "Unable to set property DifferenceMapArrayPath";\
  }\
  filter->execute();\
  err = filter->getErrorCondition();\
  DREAM3D_REQUIRE_NE(err, 0);

//#define SET_PROPERTIES_AND_CHECK_EQ(filter, firstPath, secondPath, diffMapPath)\
//  var.setValue(path);\
//  propWasSet = filter->setProperty("SelectedArrayPath", var);\
//  if(false == propWasSet)\
//  {\
//    qDebug() << "Unable to set property SelectedArrayPath for" << #dc;\
//  }\
//  var.setValue(derivsName);\
//  propWasSet = filter->setProperty("DerivativesArrayPath", var);\
//  if(false == propWasSet)\
//  {\
//    qDebug() << "Unable to set property DerivativesArrayPath for" << #dc;\
//  }\
//  filter->execute();\
//  err = filter->getErrorCondition();\
//  DREAM3D_REQUIRE_EQUAL(err, 0);\
//  data = dc->getAttributeMatrix(derivsName.getAttributeMatrixName())->getAttributeArray(derivsName.getDataArrayName());\
//  if (NULL == boost::dynamic_pointer_cast<DoubleArrayType>(data))\
//  {\
//    err = 1;\
//  }\
//  DREAM3D_REQUIRE_EQUAL(err, 0);\
//  checkDims = data->getComponentDimensions();\
//  if ((3 * cDims[0]) != checkDims[0])\
//  {\
//    err = 1;\
//  }\
//  DREAM3D_REQUIRE_EQUAL(err, 0);

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
  QString filtName = "FindDifferenceMapTest";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
  if (NULL == filterFactory.get())
  {
    std::stringstream ss;
    ss << "The FindDifferenceMapTestTest Requires the use of the " << filtName.toStdString() << " filter which is found in the Statistics Plugin";
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

  tDims[0] = 10;
  AttributeMatrix::Pointer attrMat3 = AttributeMatrix::New(tDims, "DiffMapTestAttrMat3", 3);

  m->addAttributeMatrix("DiffMapTestAttrMat1", attrMat1);
  m->addAttributeMatrix("DiffMapTestAttrMat2", attrMat2);
  m->addAttributeMatrix("DiffMapTestAttrMat3", attrMat3);
  dca->addDataContainer(m);

  QVector<size_t> cDims(1, 3);
  int32_t initVal = 5;

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

  initVal = 10;

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

  CREATE_DATA_ARRAY(float, attrMat3, tDims, cDims, initVal, err)

  return dca;
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

  DataArrayPath attrMat1_uint8("FindDifferenceMapTest", "DiffMapTestAttrMat1", "_uint8_t_attrMat1Array");
  DataArrayPath attrMat1_int8("FindDifferenceMapTest", "DiffMapTestAttrMat1", "_int8_t_attrMat1Array");
  DataArrayPath attrMat1_uint16("FindDifferenceMapTest", "DiffMapTestAttrMat1", "_uint16_t_attrMat1Array");
  DataArrayPath attrMat1_int16("FindDifferenceMapTest", "DiffMapTestAttrMat1", "_int16_t_attrMat1Array");
  DataArrayPath attrMat1_uint32("FindDifferenceMapTest", "DiffMapTestAttrMat1", "_uint32_t_attrMat1Array");
  DataArrayPath attrMat1_int32("FindDifferenceMapTest", "DiffMapTestAttrMat1", "_int32_t_attrMat1Array");
  DataArrayPath attrMat1_uint64("FindDifferenceMapTest", "DiffMapTestAttrMat1", "_uint64_t_attrMat1Array");
  DataArrayPath attrMat1_int64("FindDifferenceMapTest", "DiffMapTestAttrMat1", "_int64_t_attrMat1Array");
  DataArrayPath attrMat1_float("FindDifferenceMapTest", "DiffMapTestAttrMat1", "_float_attrMat1Array");
  DataArrayPath attrMat1_double("FindDifferenceMapTest", "DiffMapTestAttrMat1", "_double_attrMat1Array");
  DataArrayPath attrMat1_bool("FindDifferenceMapTest", "DiffMapTestAttrMat1", "_bool_attrMat1Array");

  DataArrayPath attrMat2_uint8("FindDifferenceMapTest", "DiffMapTestAttrMat2", "_uint8_t_attrMat2Array");
  DataArrayPath attrMat2_int8("FindDifferenceMapTest", "DiffMapTestAttrMat2", "_int8_t_attrMat2Array");
  DataArrayPath attrMat2_uint16("FindDifferenceMapTest", "DiffMapTestAttrMat2", "_uint16_t_attrMat2Array");
  DataArrayPath attrMat2_int16("FindDifferenceMapTest", "DiffMapTestAttrMat2", "_int16_t_attrMat2Array");
  DataArrayPath attrMat2_uint32("FindDifferenceMapTest", "DiffMapTestAttrMat2", "_uint32_t_attrMat2Array");
  DataArrayPath attrMat2_int32("FindDifferenceMapTest", "DiffMapTestAttrMat2", "_int32_t_attrMat2Array");
  DataArrayPath attrMat2_uint64("FindDifferenceMapTest", "DiffMapTestAttrMat2", "_uint64_t_attrMat2Array");
  DataArrayPath attrMat2_int64("FindDifferenceMapTest", "DiffMapTestAttrMat2", "_int64_t_attrMat2Array");
  DataArrayPath attrMat2_float("FindDifferenceMapTest", "DiffMapTestAttrMat2", "_float_attrMat2Array");
  DataArrayPath attrMat2_double("FindDifferenceMapTest", "DiffMapTestAttrMat2", "_double_attrMat2Array");
  DataArrayPath attrMat2_bool("FindDifferenceMapTest", "DiffMapTestAttrMat2", "_bool_attrMat2Array");

  DataArrayPath attrMat3_float("FindDifferenceMapTest", "DiffMapTestAttrMat3", "_float_attrMat3Array");

  DataArrayPath diffMap1("FindDifferenceMapTest", "DiffMapTestAttrMat1", diffMapName);
  DataArrayPath diffMap2("FindDifferenceMapTest", "DiffMapTestAttrMat3", diffMapName);

  // Fail if an input array is bool
  SET_PROPERTIES_AND_CHECK_NE(filter, attrMat1_bool, attrMat2_bool, diffMap1)
  REMOVE_ARRAY(attrMat1_bool, diffMapName)

  // Fail if input arrays are of different type
  SET_PROPERTIES_AND_CHECK_NE(filter, attrMat1_uint8, attrMat2_float, diffMap1)
  REMOVE_ARRAY(attrMat1_bool, diffMapName)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindDifferenceMapTestTest()
{
  int err = 0;

  DataContainerArray::Pointer dca = initializeDataContainerArray();

  QString filtName = "FindDifferenceMap";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer factory = fm->getFactoryForFilter(filtName);
  DREAM3D_REQUIRE(factory.get() != NULL)

  AbstractFilter::Pointer diffMapFilter = factory->create();
  DREAM3D_REQUIRE(diffMapFilter.get() != NULL)

  diffMapFilter->setDataContainerArray(dca);

  validateFindDifferenceMap(diffMapFilter, dca);

  return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void loadFilterPlugins()
{
  // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();
  DREAM3DPluginLoader::LoadPluginFilters(fm);

  // Send progress messages from PipelineBuilder to this object for display
  QMetaObjectUtilities::RegisterMetaTypes();
}


// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  // Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("FindDifferenceMapTestTest");

  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( loadFilterPlugins() );
  DREAM3D_REGISTER_TEST( TestFilterAvailability() );

  DREAM3D_REGISTER_TEST( FindDifferenceMapTestTest() )

  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
  PRINT_TEST_SUMMARY();
  return err;
}
