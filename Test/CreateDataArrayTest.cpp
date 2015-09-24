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
#include <QtCore/QDir>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "DREAM3DTestFileLocations.h"

enum ErrorCodes
{
  NO_ERROR = 0,
  INT8_ERROR = -4050
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestFilterAvailability()
{
  // Now instantiate the CreateDataArray Filter from the FilterManager
  QString filtName = "CreateDataArray";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
  if (NULL == filterFactory.get() )
  {
    std::stringstream ss;
    ss << "The CreateDataArrayTest Requires the use of the " << filtName.toStdString() << " filter which is found in the IO Plugin";
    DREAM3D_TEST_THROW_EXCEPTION(ss.str())
  }

  // Now instantiate the CreateDataContainer Filter from the FilterManager
  filtName = "CreateDataContainer";
  filterFactory = fm->getFactoryForFilter(filtName);
  if (NULL == filterFactory.get() )
  {
    std::stringstream ss;
    ss << "The CreateDataArrayTest Requires the use of the " << filtName.toStdString() << " filter which is found in the IO Plugin";
    DREAM3D_TEST_THROW_EXCEPTION(ss.str())
  }

  // Now instantiate the CreateDataContainer Filter from the FilterManager
  filtName = "CreateAttributeMatrix";
  fm = FilterManager::Instance();
  filterFactory = fm->getFactoryForFilter(filtName);
  if (NULL == filterFactory.get() )
  {
    std::stringstream ss;
    ss << "The CreateDataArrayTest Requires the use of the " << filtName.toStdString() << " filter which is found in the IO Plugin";
    DREAM3D_TEST_THROW_EXCEPTION(ss.str())
  }

  return EXIT_SUCCESS;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestCreateDataContainerAndAttributeMatrix ()
{

  int err = 0;
  bool propWasSet = false;
  // bool ok = false;
  QVariant var;

  DataContainerArray::Pointer dca = DataContainerArray::New();

  // Now instantiate the CreateDataArray Filter from the FilterManager
  FilterManager* fm = FilterManager::Instance();

  QString filtName = "CreateDataContainer";
  IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
  if (NULL != filterFactory.get())
  {
    // If we get this far, the Factory is good so creating the filter should not fail unless something has
    // horribly gone wrong in which case the system is going to come down quickly after this.
    AbstractFilter::Pointer filter = filterFactory->create();

    filter->setDataContainerArray(dca);

    // Test 1 set int32 array with an initialization of -5 and read value for comparison
    var.setValue(QString::fromLatin1("Test Data Container"));
    propWasSet = filter->setProperty("CreatedDataContainer", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    filter->preflight();
    err = filter->getErrorCondition();
    DREAM3D_REQUIRED(err, >=, 0)
  }




  filtName = "CreateAttributeMatrix";
  filterFactory = fm->getFactoryForFilter(filtName);
  if (NULL != filterFactory.get())
  {
    // If we get this far, the Factory is good so creating the filter should not fail unless something has
    // horribly gone wrong in which case the system is going to come down quickly after this.
    AbstractFilter::Pointer filter = filterFactory->create();

    filter->setDataContainerArray(dca);

    var.setValue(QString::fromLatin1("Test Data Container"));
    propWasSet = filter->setProperty("CreatedDataContainer", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)


    var.setValue(QString::fromLatin1("Cell Attribute Matrix"));
    propWasSet = filter->setProperty("CreatedAttributeMatrix", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    filter->preflight();
    err = filter->getErrorCondition();
    DREAM3D_REQUIRED(err, >=, 0)
  }

  return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArray::Pointer CreateDataContainerArray()
{
  DataContainerArray::Pointer dca = DataContainerArray::New();
  DataContainer::Pointer m = DataContainer::New(DREAM3D::Defaults::DataContainerName);
  dca->addDataContainer(m);
  AttributeMatrix::Pointer attrMatrix = AttributeMatrix::New(QVector<size_t>(1, 1), DREAM3D::Defaults::AttributeMatrixName, DREAM3D::AttributeMatrixType::Generic);
  m->addAttributeMatrix(DREAM3D::Defaults::AttributeMatrixName, attrMatrix);
  return dca;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void TestScalarType(AbstractFilter::Pointer filter, DataArrayPath path, int scalarType)
{

  DataContainerArray::Pointer dca = CreateDataContainerArray();
  filter->setDataContainerArray(dca);

  QVariant var;
  int err = 0;
  bool propWasSet = false;


  var.setValue(scalarType);
  propWasSet = filter->setProperty("ScalarType", var); //
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

  var.setValue(1);
  propWasSet = filter->setProperty("NumberOfComponents", var); // 1 component
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

  var.setValue(path);
  propWasSet = filter->setProperty("NewArray", var); // array path
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

  /* ==== Test The Maximum Value for the primitive type ===== */
  T max = std::numeric_limits<T>::max();
  var.setValue(QString::number(max));
  propWasSet = filter->setProperty("InitializationValue", var); // initialize with
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

  filter->preflight();
  err = filter->getErrorCondition();
  DREAM3D_REQUIRE_EQUAL(err, NO_ERROR);

  dca = CreateDataContainerArray();
  filter->setDataContainerArray(dca);
  filter->execute();
  err = filter->getErrorCondition();
  DREAM3D_REQUIRE_EQUAL(err, NO_ERROR);

  {
    IDataArray::Pointer testArrayPtr = dca->getDataContainer(DREAM3D::Defaults::DataContainerName)->getAttributeMatrix(path.getAttributeMatrixName())->getAttributeArray(path.getDataArrayName());
    typedef DataArray<T> DataArrayType;
    typename DataArrayType::Pointer inputArray = boost::dynamic_pointer_cast<DataArrayType>(testArrayPtr);
    DREAM3D_REQUIRE_VALID_POINTER(inputArray.get());
    T* inputArrayPtr = inputArray->getPointer(0); // pointer to the int array created from the filter
    DREAM3D_REQUIRE_EQUAL(inputArrayPtr[0], max)
  }

  dca = CreateDataContainerArray();
  filter->setDataContainerArray(dca);

  /* ==== Test The Minmum Value for the primitive type ===== */
  T min = std::numeric_limits<T>::min();
  var.setValue(QString::number(min));
  propWasSet = filter->setProperty("InitializationValue", var); // initialize with
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

  filter->preflight();
  err = filter->getErrorCondition();
  DREAM3D_REQUIRE_EQUAL(err, NO_ERROR);

  dca = CreateDataContainerArray();
  filter->setDataContainerArray(dca);

  filter->execute();
  err = filter->getErrorCondition();
  DREAM3D_REQUIRE_EQUAL(err, NO_ERROR);

  {
    IDataArray::Pointer testArrayPtr = dca->getDataContainer(DREAM3D::Defaults::DataContainerName)->getAttributeMatrix(path.getAttributeMatrixName())->getAttributeArray(path.getDataArrayName());
    typedef DataArray<T> DataArrayType;
    typename DataArrayType::Pointer inputArray = boost::dynamic_pointer_cast<DataArrayType>(testArrayPtr);
    DREAM3D_REQUIRE_VALID_POINTER(inputArray.get());
    T* inputArrayPtr = inputArray->getPointer(0); // pointer to the int array created from the filter
    DREAM3D_REQUIRE_EQUAL(inputArrayPtr[0], min)
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestCreateDataArray()
{
//  bool propWasSet = false;
//  bool ok = false;
//  int err = 0;

  // Now instantiate the CreateDataArray Filter from the FilterManager
  QString filtName = "CreateDataArray";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
  if (NULL != filterFactory.get())
  {
    // If we get this far, the Factory is good so creating the filter should not fail unless something has
    // horribly gone wrong in which case the system is going to come down quickly after this.
    AbstractFilter::Pointer filter = filterFactory->create();
    DataArrayPath path = DataArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::AttributeMatrixName, "testArray");
    DataArrayPath path1 = DataArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::AttributeMatrixName, "testArray");

    TestScalarType<int8_t>(filter, path, 0);
    TestScalarType<uint8_t>(filter, path, 1);
    TestScalarType<int16_t>(filter, path, 2);
    TestScalarType<uint16_t>(filter, path, 3);
    TestScalarType<int32_t>(filter, path, 4);
    TestScalarType<uint32_t>(filter, path, 5);
    TestScalarType<int64_t>(filter, path, 6);
    TestScalarType<uint64_t>(filter, path, 7);
//    TestScalarType<float>(filter, path, 8);
//    TestScalarType<double>(filter, path, path1, 9);

#if 0
    // Test 2 set a bool array with a non-zero value and compare to 1
    filter->setDataContainerArray(dca);
    filter->getDataContainerArray()->getAttributeMatrix(path1)->removeAttributeArray(path1.getDataArrayName());
    QVariant var;
    var.setValue(10);
    propWasSet = filter->setProperty("ScalarType", var); // bool
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    double d = QString("0.000001").toDouble(&ok);
    var.setValue(d);
    propWasSet = filter->setProperty("InitializationValue", var); // initialize with d (9.9999999999999995e-007 Visual Studio)
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    filter->execute();
    err = filter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, NO_ERROR);

    IDataArray::Pointer boolArray = m->getAttributeMatrix(path.getAttributeMatrixName())->getAttributeArray(path.getDataArrayName());
    DataArray<bool>* inputArrayBool = DataArray<bool>::SafePointerDownCast(boolArray.get());
    bool* inputArrayPtrBool = inputArrayBool->getPointer(0); // pointer to the bool array created from the filter
    if (*inputArrayPtrBool != 1) // check the initialization value from the bool array
    {
      DREAM3D_REQUIRE_EQUAL(0, 1)
    }

    // Test 3 set int8 array with an initialization of 128 for out of range error
    filter->setDataContainerArray(dca);
    var.setValue(0);
    propWasSet = filter->setProperty("ScalarType", var); // int8
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(128);
    propWasSet = filter->setProperty("InitializationValue", var); // initialize with 128
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    filter->execute();
    err = filter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, INT8_ERROR);

#endif
  }
  else
  {
    QString ss = QObject::tr("CreateDataArrayTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
    DREAM3D_REQUIRE_EQUAL(0, 1)
  }
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
  QCoreApplication::setApplicationName("CreateDataArrayTest");

  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( loadFilterPlugins() );
  DREAM3D_REGISTER_TEST( TestFilterAvailability() );

  DREAM3D_REGISTER_TEST(TestCreateDataArray())

  PRINT_TEST_SUMMARY();

  return err;
}

