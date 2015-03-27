/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
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

#include "TestFileLocations.h"

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
  return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestCreateDataArray()
{
  DataContainerArray::Pointer dca = DataContainerArray::New();
  DataContainer::Pointer m = DataContainer::New(DREAM3D::Defaults::DataContainerName);
  dca->addDataContainer(m);
  AttributeMatrix::Pointer attrMatrix = AttributeMatrix::New(QVector<size_t>(1,1), DREAM3D::Defaults::AttributeMatrixName, DREAM3D::AttributeMatrixType::Generic);
  m->addAttributeMatrix(DREAM3D::Defaults::AttributeMatrixName, attrMatrix);

  // Now instantiate the CreateDataArray Filter from the FilterManager
  QString filtName = "CreateDataArray";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
  if (NULL != filterFactory.get())
  {
    // If we get this far, the Factory is good so creating the filter should not fail unless something has
    // horribly gone wrong in which case the system is going to come down quickly after this.
    AbstractFilter::Pointer filter = filterFactory->create();

    QVariant var;
    int err = 0;
    bool propWasSet, ok = false;
    filter->setDataContainerArray(dca);

    // Test 1 set int32 array with an initialization of -5 and read value for comparison
    var.setValue(4);
    propWasSet = filter->setProperty("ScalarType", var); // int32
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(1);
    propWasSet = filter->setProperty("NumberOfComponents", var); // 1 component
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    DataArrayPath path1 = DataArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::AttributeMatrixName, "");
    var.setValue(path1);
    propWasSet = filter->setProperty("AttributeMatrixPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    QVariant qv("testArray");
    propWasSet = filter->setProperty("OutputArrayName", qv); // array name
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(-5);
    propWasSet = filter->setProperty("InitializationValue", var); // initialize with -5
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    filter->execute();
    err = filter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, NO_ERROR);

    DataArrayPath path = DataArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::AttributeMatrixName, "testArray");
    IDataArray::Pointer testArrayPtr = m->getAttributeMatrix(path.getAttributeMatrixName())->getAttributeArray(path.getDataArrayName());
    DataArray<int32_t>* inputArray = DataArray<int32_t>::SafePointerDownCast(testArrayPtr.get());
    int32_t* inputArrayPtr = inputArray->getPointer(0); // pointer to the int array created from the filter
    if (*inputArrayPtr != -5) // check the initialization value from the int32 array
    {
      DREAM3D_REQUIRE_EQUAL(0, 1)
    }

    // Test 2 set a bool array with a non-zero value and compare to 1 
    filter->setDataContainerArray(dca);
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

