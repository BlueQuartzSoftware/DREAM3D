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

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/Common/TemplateHelpers.hpp"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"

#include "DREAM3DTestFileLocations.h"

#define CREATE_DATA_ARRAY(type, attrMat, tDims, cDims, initVal, name, err)\
  DataArray<type>::Pointer name##Array = DataArray<type>::CreateArray(tDims, cDims, name, true);\
  err = attrMat->addAttributeArray(name, name##Array);\
  name##Array->initializeWithValue(initVal);\
  DREAM3D_REQUIRE(err >= 0);

#define CHECK_FOR_FAIL(filter, featureIds1, featureIds2, attrMat1, attrMat2, newIndex, newAttrMat, errVal)\
  var.setValue(featureIds1);\
  propWasSet = filter->setProperty("FirstIndexArrayPath", var);\
  if(false == propWasSet)\
  {\
    qDebug() << "Unable to set property FirstIndexArrayPath";\
  }\
  var.setValue(featureIds2);\
  propWasSet = filter->setProperty("SecondIndexArrayPath", var);\
  if(false == propWasSet)\
  {\
    qDebug() << "Unable to set property SecondIndexArrayPath";\
  }\
  var.setValue(attrMat1);\
  propWasSet = filter->setProperty("FirstAttributeMatrixPath", var);\
  if(false == propWasSet)\
  {\
    qDebug() << "Unable to set property FirstAttributeMatrixPath";\
  }\
  var.setValue(attrMat2);\
  propWasSet = filter->setProperty("SecondAttributeMatrixPath", var);\
  if(false == propWasSet)\
  {\
    qDebug() << "Unable to set property SecondAttributeMatrixPath";\
  }\
  var.setValue(newIndex);\
  propWasSet = filter->setProperty("NewIndexArrayName", var);\
  if(false == propWasSet)\
  {\
    qDebug() << "Unable to set property NewIndexArrayName";\
  }\
  var.setValue(newAttrMat);\
  propWasSet = filter->setProperty("CombinedAttributeMatrixName", var);\
  if(false == propWasSet)\
  {\
    qDebug() << "Unable to set property CombinedAttributeMatrixName";\
  }\
  filter->execute();\
  err = filter->getErrorCondition();\
  DREAM3D_REQUIRE_EQUAL(err, errVal);

#define CHECK_FOR_PASS1(filter, featureIds1, featureIds2, attrMat1, attrMat2, newIndex, newAttrMat)\
  var.setValue(featureIds1);\
  propWasSet = filter->setProperty("FirstIndexArrayPath", var);\
  if(false == propWasSet)\
  {\
    qDebug() << "Unable to set property FirstIndexArrayPath";\
  }\
  var.setValue(featureIds2);\
  propWasSet = filter->setProperty("SecondIndexArrayPath", var);\
  if(false == propWasSet)\
  {\
    qDebug() << "Unable to set property SecondIndexArrayPath";\
  }\
  var.setValue(attrMat1);\
  propWasSet = filter->setProperty("FirstAttributeMatrixPath", var);\
  if(false == propWasSet)\
  {\
    qDebug() << "Unable to set property FirstAttributeMatrixPath";\
  }\
  var.setValue(attrMat2);\
  propWasSet = filter->setProperty("SecondAttributeMatrixPath", var);\
  if(false == propWasSet)\
  {\
    qDebug() << "Unable to set property SecondAttributeMatrixPath";\
  }\
  var.setValue(newIndex);\
  propWasSet = filter->setProperty("NewIndexArrayName", var);\
  if(false == propWasSet)\
  {\
    qDebug() << "Unable to set property NewIndexArrayName";\
  }\
  var.setValue(newAttrMat);\
  propWasSet = filter->setProperty("CombinedAttributeMatrixName", var);\
  if(false == propWasSet)\
  {\
    qDebug() << "Unable to set property CombinedAttributeMatrixName";\
  }\
  filter->execute();\
  err = filter->getErrorCondition();\
  DREAM3D_REQUIRE_EQUAL(err, 0);\
  validateCombinedEnsembleAMs(filter, featureIds1, featureIds2, attrMat1, attrMat2, newIndex, newAttrMat);

#define CHECK_FOR_PASS2(filter, featureIds1, featureIds2, attrMat1, attrMat2, newIndex, newAttrMat)\
  var.setValue(featureIds1);\
  propWasSet = filter->setProperty("FirstIndexArrayPath", var);\
  if(false == propWasSet)\
    {\
    qDebug() << "Unable to set property FirstIndexArrayPath";\
    }\
  var.setValue(featureIds2);\
  propWasSet = filter->setProperty("SecondIndexArrayPath", var);\
  if(false == propWasSet)\
    {\
    qDebug() << "Unable to set property SecondIndexArrayPath";\
    }\
  var.setValue(attrMat1);\
  propWasSet = filter->setProperty("FirstAttributeMatrixPath", var);\
  if(false == propWasSet)\
    {\
    qDebug() << "Unable to set property FirstAttributeMatrixPath";\
    }\
  var.setValue(attrMat2);\
  propWasSet = filter->setProperty("SecondAttributeMatrixPath", var);\
  if(false == propWasSet)\
    {\
    qDebug() << "Unable to set property SecondAttributeMatrixPath";\
    }\
  var.setValue(newIndex);\
  propWasSet = filter->setProperty("NewIndexArrayName", var);\
  if(false == propWasSet)\
    {\
    qDebug() << "Unable to set property NewIndexArrayName";\
    }\
  var.setValue(newAttrMat);\
  propWasSet = filter->setProperty("CombinedAttributeMatrixName", var);\
  if(false == propWasSet)\
    {\
    qDebug() << "Unable to set property CombinedAttributeMatrixName";\
    }\
  filter->execute();\
  err = filter->getErrorCondition();\
  DREAM3D_REQUIRE_EQUAL(err, 0);\
  validateCombinedFeatureAMs(filter, featureIds1, featureIds2, attrMat1, attrMat2, newIndex, newAttrMat);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestFilterAvailability()
{
  // Now instantiate the FindDifferenceMapTest Filter from the FilterManager
  QString filtName = "CombineAttributeMatrices";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
  if (NULL == filterFactory.get())
  {
    std::stringstream ss;
    ss << "The CombineAttribtueMatricesTest Requires the use of the " << filtName.toStdString() << " filter which is found in Core Filters";
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

  //make two data containers to check AMs are from same DC
  DataContainer::Pointer m = DataContainer::New();
  m->setName("CombineAttributeMatricesTest");
  DataContainer::Pointer m2 = DataContainer::New();
  m2->setName("CombineAttributeMatricesTest2");

  // Create Attribute Matrices with different tDims to test validation of tuple compatibility
  QVector<size_t> tDims(3, 0);
  tDims[0] = 2;
  tDims[1] = 2;
  tDims[2] = 1;
  QVector<size_t> cDims(1, 1);
  int32_t initVal = 10;

  //Make Cell AM
  AttributeMatrix::Pointer cellAttrMat = AttributeMatrix::New(tDims, "cellAttrMat", DREAM3D::AttributeMatrixType::Cell);
  m->addAttributeMatrix("cellAttrMat", cellAttrMat);

  //Create 2 featureIds arrays in the Cell AM
  QString featureIds1 = "featureIds1";
  QString featureIds2 = "featureIds2";
  CREATE_DATA_ARRAY(int32_t, cellAttrMat, tDims, cDims, initVal, featureIds1, err);
  CREATE_DATA_ARRAY(int32_t, cellAttrMat, tDims, cDims, initVal, featureIds2, err);

  //pull down featureIds arrays and initialize them as:
  //1: | 1 | 2 |   2: | 0 | 0 |
  //   | 0 | 0 |      | 1 | 2 |
  Int32ArrayType::Pointer featureIds1Ptr = cellAttrMat->getAttributeArrayAs<Int32ArrayType>(featureIds1);
  int32_t* fIds1 = featureIds1Ptr->getPointer(0);
  fIds1[0] = 1;
  fIds1[1] = 2;
  fIds1[2] = 0;
  fIds1[3] = 0;
  Int32ArrayType::Pointer featureIds2Ptr = cellAttrMat->getAttributeArrayAs<Int32ArrayType>(featureIds2);
  int32_t* fIds2 = featureIds2Ptr->getPointer(0);
  fIds2[0] = 0;
  fIds2[1] = 0;
  fIds2[2] = 1;
  fIds2[3] = 2;

  //Make a feature and ensemble AM for the first featureIds array and add some arrays
  tDims.resize(1);
  tDims[0] = 3;
  AttributeMatrix::Pointer featureAttrMat1 = AttributeMatrix::New(tDims, "featureAttrMat1", DREAM3D::AttributeMatrixType::CellFeature);
  m->addAttributeMatrix("featureAttrMat1", featureAttrMat1);
  QString fAM1AA1 = "ensembleIds";
  QString fAM1AA2 = "sizes";
  CREATE_DATA_ARRAY(int32_t, featureAttrMat1, tDims, cDims, initVal, fAM1AA1, err);
  CREATE_DATA_ARRAY(float, featureAttrMat1, tDims, cDims, initVal, fAM1AA2, err);
  tDims[0] = 2;
  AttributeMatrix::Pointer ensembleAttrMat1 = AttributeMatrix::New(tDims, "ensembleAttrMat1", DREAM3D::AttributeMatrixType::CellEnsemble);
  m->addAttributeMatrix("ensembleAttrMat1", ensembleAttrMat1);
  QString eAM1AA1 = "crystalStructures";
  CREATE_DATA_ARRAY(int32_t, ensembleAttrMat1, tDims, cDims, initVal, eAM1AA1, err);

  //pull down and initialize arrays just created for feature and ensemble AM 1
  Int32ArrayType::Pointer ensembleIds1Ptr = featureAttrMat1->getAttributeArrayAs<Int32ArrayType>(fAM1AA1);
  int32_t* eIds1 = ensembleIds1Ptr->getPointer(0);
  FloatArrayType::Pointer sizesPtr = featureAttrMat1->getAttributeArrayAs<FloatArrayType>(fAM1AA2);
  float* sizes = sizesPtr->getPointer(0);
  Int32ArrayType::Pointer crystStructs1Ptr = ensembleAttrMat1->getAttributeArrayAs<Int32ArrayType>(eAM1AA1);
  int32_t* crystStructs1 = crystStructs1Ptr->getPointer(0);
  eIds1[0] = 0;
  eIds1[1] = 1;
  eIds1[2] = 2;
  sizes[0] = 0;
  sizes[1] = 10.5;
  sizes[2] = 4.7;
  crystStructs1[0] = 0;
  crystStructs1[1] = 1;


  //Make a feature and ensemble AM for the second featureIds array and add some arrays
  tDims[0] = 3;
  AttributeMatrix::Pointer featureAttrMat2 = AttributeMatrix::New(tDims, "featureAttrMat2", DREAM3D::AttributeMatrixType::CellFeature);
  m->addAttributeMatrix("featureAttrMat2", featureAttrMat2);
  QString fAM2AA1 = "ensembleIds";
  QString fAM2AA2 = "surfaceFeatures";
  CREATE_DATA_ARRAY(int32_t, featureAttrMat2, tDims, cDims, initVal, fAM2AA1, err);
  CREATE_DATA_ARRAY(bool, featureAttrMat2, tDims, cDims, initVal, fAM2AA2, err);
  tDims[0] = 2;
  AttributeMatrix::Pointer ensembleAttrMat2 = AttributeMatrix::New(tDims, "ensembleAttrMat2", DREAM3D::AttributeMatrixType::CellEnsemble);
  m->addAttributeMatrix("ensembleAttrMat2", ensembleAttrMat2);
  QString eAM2AA1 = "crystalStructures";
  CREATE_DATA_ARRAY(int32_t, ensembleAttrMat2, tDims, cDims, initVal, eAM2AA1, err);

  //pull down and initialize arrays just created for feature and ensemble AM 1
  Int32ArrayType::Pointer ensembleIds2Ptr = featureAttrMat2->getAttributeArrayAs<Int32ArrayType>(fAM2AA1);
  int32_t* eIds2 = ensembleIds2Ptr->getPointer(0);
  BoolArrayType::Pointer surfFeatPtr = featureAttrMat2->getAttributeArrayAs<BoolArrayType>(fAM2AA2);
  bool* surfFeat= surfFeatPtr->getPointer(0);
  Int32ArrayType::Pointer crystStructs2Ptr = ensembleAttrMat1->getAttributeArrayAs<Int32ArrayType>(eAM2AA1);
  int32_t* crystStructs2 = crystStructs2Ptr->getPointer(0);
  eIds2[0] = 0;
  eIds2[1] = 1;
  eIds2[2] = 2;
  surfFeat[0] = false;
  surfFeat[1] = false;
  surfFeat[2] = true;
  crystStructs2[0] = 0;
  crystStructs2[1] = 1;

  dca->addDataContainer(m);

  //add a feature AM to the second DC to allow for check that AMs are in same DC
  m2->addAttributeMatrix("featureAttrMat2", featureAttrMat2);
  dca->addDataContainer(m2);

  return dca;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void removeDataArraysAfterFirstCombination(DataContainer::Pointer dc, DataArrayPath path1, DataArrayPath path2)
{
  AttributeMatrix::Pointer am1 = dc->getAttributeMatrix(path1.getAttributeMatrixName());
  am1->removeAttributeArray(path1.getDataArrayName());
  AttributeMatrix::Pointer am2 = dc->getAttributeMatrix(path2.getAttributeMatrixName());
  am2->removeAttributeArray(path2.getDataArrayName());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void validateCombinedFeatureAMs(AbstractFilter::Pointer filter, DataArrayPath featureIds1, DataArrayPath featureIds2, DataArrayPath attrMat1, DataArrayPath attrMat2, QString newIndex, QString newAttrMat)
{
  DataContainerArray::Pointer dca = filter->getDataContainerArray();

  //make path to get new AM for checking its size and contents
  AttributeMatrix::Pointer attrMat1Ptr = dca->getAttributeMatrix(attrMat1);
  DataArrayPath temp;
  temp.update(attrMat1.getDataContainerName(), newAttrMat, "");
  AttributeMatrix::Pointer newAttrMatPtr = dca->getAttributeMatrix(temp);
  //new combined feature AM should have 5 tuples (3 + 3 - 1)
  DREAM3D_REQUIRE_EQUAL(newAttrMatPtr->getNumTuples(), 5)
  // combined feature AM should have ensembleIds, sizes, shapes arrays
  DREAM3D_REQUIRE_EQUAL(newAttrMatPtr->getNumAttributeArrays(), 3)
  //use the featureIds1 path from cAM to get the cAM and ask for the newFeatureIds out of that matrix
  AttributeMatrix::Pointer cellAttrMatPtr = dca->getAttributeMatrix(featureIds1);
  Int32ArrayType::Pointer newFeatureIdsPtr = cellAttrMatPtr->getAttributeArrayAs<Int32ArrayType>(newIndex);
  int32_t* newFIds = newFeatureIdsPtr->getPointer(0);
  //ensemble ids should be updated so that featureIds1 still equal 1 and 2, but featureIds2 should be changed from 1 and 2 to 3 and 4
  DREAM3D_REQUIRE_EQUAL(newFIds[0], 1)
  DREAM3D_REQUIRE_EQUAL(newFIds[1], 2)
  DREAM3D_REQUIRE_EQUAL(newFIds[2], 3)
  DREAM3D_REQUIRE_EQUAL(newFIds[3], 4)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void validateCombinedEnsembleAMs(AbstractFilter::Pointer filter, DataArrayPath ensembleIds1, DataArrayPath ensembleIds2, DataArrayPath attrMat1, DataArrayPath attrMat2, QString newIndex, QString newAttrMat)
{
  DataContainerArray::Pointer dca = filter->getDataContainerArray();

  //make path to get new AM for checking its size and contents
  DataArrayPath temp;
  temp.update(attrMat1.getDataContainerName(), newAttrMat, "");
  AttributeMatrix::Pointer newAttrMatPtr = dca->getAttributeMatrix(temp);
  //new combined ensemble AM should have 3 tuples (2 + 2 - 1)
  DREAM3D_REQUIRE_EQUAL(newAttrMatPtr->getNumTuples(), 3)
  // combined ensemble AM should only have crystal structures array
  DREAM3D_REQUIRE_EQUAL(newAttrMatPtr->getNumAttributeArrays(), 1)
  //use the ensembleIds path from fAM1 to get the fAM1 and ask for the newEnsembleIds out of that matrix
  AttributeMatrix::Pointer indexAttrMat1Ptr = dca->getAttributeMatrix(ensembleIds1);
  Int32ArrayType::Pointer newEnsembleIdsPtr = indexAttrMat1Ptr->getAttributeArrayAs<Int32ArrayType>(newIndex);
  int32_t* newEIds = newEnsembleIdsPtr->getPointer(0);
  //ensemble ids should be updated so that ensembleIds1 still equal 1, but ensembleIds2 should be changed from 1 to 2
  DREAM3D_REQUIRE_EQUAL(newEIds[0], 0)
  DREAM3D_REQUIRE_EQUAL(newEIds[1], 1)
  DREAM3D_REQUIRE_EQUAL(newEIds[2], 2)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void validateCombineAM(AbstractFilter::Pointer filter, DataContainerArray::Pointer dca)
{
  QVariant var;
  bool propWasSet;
  int err = 0;

  DataContainer::Pointer dc = dca->getDataContainer("CombineAttributeMatricesTest");

  DataArrayPath fiap("CombineAttributeMatricesTest", "featureAttrMat1", "ensembleIds");
  DataArrayPath fiap2("CombineAttributeMatricesTest", "cellAttrMat", "featureIds1");
  DataArrayPath siap("CombineAttributeMatricesTest", "featureAttrMat2", "ensembleIds");
  DataArrayPath siap2("CombineAttributeMatricesTest", "cellAttrMat", "featureIds2");
  QString nian("newEnsembleIds");
  QString nian2("newFeatureIds");

  DataArrayPath famp("CombineAttributeMatricesTest", "ensembleAttrMat1", "");
  DataArrayPath samp("CombineAttributeMatricesTest", "ensembleAttrMat2", "");
  DataArrayPath famp2("CombineAttributeMatricesTest", "featureAttrMat1", "");
  DataArrayPath samp2("CombineAttributeMatricesTest", "featureAttrMat2", "");
  DataArrayPath samp3("CombineAttributeMatricesTest2", "featureAttrMat2", "");
  QString namn("CombinedEnsembleAttrMat");
  QString namn2("CombinedFeatureAttrMat");


  // Fail if an AMs are not in the same DC
  CHECK_FOR_FAIL(filter, fiap2, siap2, famp2, samp3, nian2, namn2, -5557)
  // Fail if the AMs are the same AM
  CHECK_FOR_FAIL(filter, fiap2, siap2, famp2, famp2, nian2, namn2, -5558)
  // Fail if the AMs are not the same type (ie feature or ensemble)
  CHECK_FOR_FAIL(filter, fiap2, siap2, famp2, samp, nian2, namn2, -5559)

  // combine ensemble AMs sucessfully
  CHECK_FOR_PASS1(filter, fiap, siap, famp, samp, nian, namn)

  removeDataArraysAfterFirstCombination(dc, fiap, siap);

  // combine feature AMs sucessfully
  CHECK_FOR_PASS2(filter, fiap2, siap2, famp2, samp2, nian2, namn2)

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CombineAMTest()
{
  DataContainerArray::Pointer dca = initializeDataContainerArray();

  QString filtName = "CombineAttributeMatrices";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer factory = fm->getFactoryForFilter(filtName);
  DREAM3D_REQUIRE(factory.get() != NULL)

  AbstractFilter::Pointer combineAttributeMatricesFilter = factory->create();
  DREAM3D_REQUIRE(combineAttributeMatricesFilter.get() != NULL)

  combineAttributeMatricesFilter->setDataContainerArray(dca);

  validateCombineAM(combineAttributeMatricesFilter, dca);

  return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void loadFilterPlugins()
{
  // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();
  SIMPLibPluginLoader::LoadPluginFilters(fm);

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
  QCoreApplication::setApplicationName("CombineAttributeMatricesTest");

  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( loadFilterPlugins() );
  DREAM3D_REGISTER_TEST( TestFilterAvailability() );

  DREAM3D_REGISTER_TEST( CombineAMTest() )

  PRINT_TEST_SUMMARY();
  return err;
}
