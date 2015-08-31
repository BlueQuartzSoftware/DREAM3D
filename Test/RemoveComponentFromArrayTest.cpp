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
#include "DREAM3DLib/DataContainers/DataContainer.h"

#include "DREAM3DTestFileLocations.h"

enum ErrorCodes
{
  NO_ERROR = 0,
  INT8_ERROR = -4050
};

size_t numTuples = 5;
size_t numComps = 4;

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
  AttributeMatrix::Pointer attrMatrix = AttributeMatrix::New(QVector<size_t>(1, numTuples), DREAM3D::Defaults::AttributeMatrixName, DREAM3D::AttributeMatrixType::Generic);
  m->addAttributeMatrix(DREAM3D::Defaults::AttributeMatrixName, attrMatrix);
  return dca;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestNewArray(AbstractFilter::Pointer filter, DataArrayPath path, DataContainerArray::Pointer dca)
{
    QVariant var;
    int err;
    bool propWasSet = false;
    int compNumber = 2;
    int badCompNumber = numComps + 5;

    filter->setDataContainerArray(dca);


    QString NewArrayArrayName = "RemovedComp";
    QString ReducedArrayArrayName = "ReducedArray";

    //Test to make sure reduced array returns correct values
    var.setValue(compNumber);
    propWasSet = filter->setProperty("CompNumber", var); //
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(path);
    propWasSet = filter->setProperty("SelectedArrayPath", var); //
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(true);
    propWasSet = filter->setProperty("SaveRemovedComponent", var); //
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)


    var.setValue(NewArrayArrayName);
    propWasSet = filter->setProperty("NewArrayArrayName", var); //
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(ReducedArrayArrayName);
    propWasSet = filter->setProperty("ReducedArrayArrayName", var); //
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    filter->execute();
    err = filter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, NO_ERROR);

    IDataArray::Pointer ReducedArrayPtr = dca->getDataContainer(DREAM3D::Defaults::DataContainerName)->getAttributeMatrix(path.getAttributeMatrixName())->getAttributeArray(ReducedArrayArrayName);
    Int8ArrayType::Pointer ReducedArray = boost::dynamic_pointer_cast<Int8ArrayType>(ReducedArrayPtr);
    DREAM3D_REQUIRE_VALID_POINTER(ReducedArray.get());



    //We had originally created an array whose (flattened) index was it's value (mostly for simplicity - it could be any array really).
    //Now we test to make sure when we remove a component that the new array has all the values except for the ones corresponding to that component/index.

    size_t count = 0;
    size_t value = 0;
    for(size_t i = 0; i < numTuples; i++)
    {
        for (size_t j = 0; j < numComps-1; j++)
        {

            if (j == compNumber)
            {
                value++;
            }
            DREAM3D_REQUIRE_EQUAL(ReducedArray->getValue(count), value);
            count ++;
            value ++;

        }
    }

    //Test to make sure saving removed component array works

    IDataArray::Pointer NewArrayPtr = dca->getDataContainer(DREAM3D::Defaults::DataContainerName)->getAttributeMatrix(path.getAttributeMatrixName())->getAttributeArray(NewArrayArrayName);
    Int8ArrayType::Pointer NewArray = boost::dynamic_pointer_cast<Int8ArrayType>(NewArrayPtr);
    DREAM3D_REQUIRE_VALID_POINTER(NewArray.get());


    for(size_t i = 0; i < numTuples; i++)
         {
            DREAM3D_REQUIRE_EQUAL(NewArray->getValue(i), i*numComps + compNumber);

         }

    // test to make sure component filter fails if component number is larger than total number of components
    var.setValue(badCompNumber);
    propWasSet = filter->setProperty("CompNumber", var); //
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    filter->execute();
    err = filter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, -11004);

    //Test to make sure sending in a single component array fails.
    //We use the single component array created from the extraction of the previous array

    var.setValue(0);
    propWasSet = filter->setProperty("CompNumber", var); //
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    path = DataArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::AttributeMatrixName, NewArrayArrayName);
    var.setValue(path);
    propWasSet = filter->setProperty("SelectedArrayPath", var); //
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(false);
    propWasSet = filter->setProperty("SaveRemovedComponent", var); //
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    ReducedArrayArrayName = "SecondReducedArray";
    var.setValue(ReducedArrayArrayName);
    propWasSet = filter->setProperty("ReducedArrayArrayName", var); //
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    filter->execute();
    err = filter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, -11003);


}



int TestRemoveComponent(DataContainerArray::Pointer dca)
{


   // Now instantiate the CreateDataArray Filter from the FilterManager
  QString filtName = "RemoveComponentFromArray";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
  if (NULL != filterFactory.get())
    {
      // If we get this far, the Factory is good so creating the filter should not fail unless something has
      // horribly gone wrong in which case the system is going to come down quickly after this.
      AbstractFilter::Pointer filter = filterFactory->create();
      DataArrayPath path = DataArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::AttributeMatrixName, "testArray");

      TestNewArray(filter, path, dca);


    }
  else
    {
        QString ss = QObject::tr("RemoveComponentFromArray Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
        DREAM3D_REQUIRE_EQUAL(0, 1)
    }





      return EXIT_SUCCESS;

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

  var.setValue(numComps);
  propWasSet = filter->setProperty("NumberOfComponents", var); // multi-component
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

  var.setValue(0);
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


    IDataArray::Pointer testArrayPtr = dca->getDataContainer(DREAM3D::Defaults::DataContainerName)->getAttributeMatrix(path.getAttributeMatrixName())->getAttributeArray(path.getDataArrayName());
    typedef DataArray<T> DataArrayType;
    typename DataArrayType::Pointer inputArray = boost::dynamic_pointer_cast<DataArrayType>(testArrayPtr);
    DREAM3D_REQUIRE_VALID_POINTER(inputArray.get());

    size_t count = 0;

    for(size_t i = 0; i < numTuples; i++)
    {
        for (size_t j = 0; j < numComps; j++)
        {
            inputArray->setValue(count, count);
            count++;
        }
    }

    std::cout << "pause" << std::endl;

    err = TestRemoveComponent(dca);


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
  QCoreApplication::setApplicationName("CreateDataArrayTest");

  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( loadFilterPlugins() );
  DREAM3D_REGISTER_TEST( TestFilterAvailability() );

  DREAM3D_REGISTER_TEST(TestCreateDataArray())


  PRINT_TEST_SUMMARY();

  return err;
}


