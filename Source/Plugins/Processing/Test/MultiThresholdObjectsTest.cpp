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
#include <QtCore/QString>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/ComparisonInputs.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "UnitTestSupport.hpp"

class MultiThresholdObjectsTest
{
public:
  MultiThresholdObjectsTest()
  {
  }
  virtual ~MultiThresholdObjectsTest()
  {
  }
  SIMPL_TYPE_MACRO(MultiThresholdObjectsTest)

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the MultiThresholdObjectsTest Filter from the FilterManager
    QString filtName = "MultiThresholdObjects";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The MultiThresholdObjectsTest Requires the use of the " << filtName.toStdString() << " filter which is found in the IO Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int RunTest()
  {
    // Create two test arrays, a float array and a int array
    DataContainerArray::Pointer dca = DataContainerArray::New();
    DataContainer::Pointer vdc = DataContainer::New("dc1");
    // Set up geometry for tuples, a cuboid with dimensions 20, 10, 1
    ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
    vdc->setGeometry(image);
    size_t dims[1] = {20};
    image->setDimensions(dims);

    QVector<size_t> tDims(1, 0);
    tDims[0] = 20;
    QVector<size_t> cDims(1);
    cDims[0] = 1;
    float fnum = 0.0f;
    int inum = 0;
    AttributeMatrix::Pointer am = AttributeMatrix::New(tDims, SIMPL::Defaults::CellAttributeMatrixName, AttributeMatrix::Type::Cell);
    DataArray<float>::Pointer data = DataArray<float>::CreateArray(tDims, cDims, "TestArrayFloat");
    DataArray<int32_t>::Pointer data1 = DataArray<int32_t>::CreateArray(tDims, cDims, "TestArrayInt");

    // Fill the float array with {.01,.02,.03,.04,.05,.06,.07,.08,.09,.10,.11,.12,.13,.14,.15.,16,.17,.18,.19,.20}
    // Fill the int array with { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19 }
    for(size_t i = 0; i < 20; i++)
    {
      fnum += 0.01f;
      data->setValue(i, fnum);  // float array
      data1->setValue(i, inum); // int array
      inum += 1;
    }
    am->addAttributeArray(data->getName(), data);   // float array
    am->addAttributeArray(data1->getName(), data1); // int array
    vdc->addAttributeMatrix(am->getName(), am);
    dca->addDataContainer(vdc);

    // Now instantiate the MultiThresholdObjectsTest Filter from the FilterManager
    QString filtName = "MultiThresholdObjects";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
    if(nullptr != filterFactory.get())
    {
      // If we get this far, the Factory is good so creating the filter should not fail unless something has
      // horribly gone wrong in which case the system is going to come down quickly after this.
      AbstractFilter::Pointer filter = filterFactory->create();
      filter->setDataContainerArray(dca);

      QVariant var;
      bool propWasSet;

      ComparisonInputs comp;
      ComparisonInput_t v;
      v.dataContainerName = "dc1";
      v.attributeMatrixName = am->getName();
      v.attributeArrayName = data->getName(); // float array
      v.compOperator = 1;                     // greater than
      v.compValue = 0.1;                      // comparison value
      comp.addInput(v);

      var.setValue(comp);
      propWasSet = filter->setProperty("SelectedThresholds", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(SIMPL::GeneralData::ThresholdArray);
      propWasSet = filter->setProperty("DestinationArrayName", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      filter->execute();
      DREAM3D_REQUIRED(filter->getErrorCondition(), >=, 0);

      DataArrayPath path = DataArrayPath("dc1", SIMPL::Defaults::CellAttributeMatrixName, SIMPL::GeneralData::ThresholdArray);
      IDataArray::Pointer thresholdArray = vdc->getAttributeMatrix(path.getAttributeMatrixName())->getAttributeArray(path.getDataArrayName());
      DataArray<bool>::Pointer inputArray = std::dynamic_pointer_cast<DataArray<bool>>(thresholdArray);
      bool* inputArrayPtr = inputArray->getPointer(0); // pointer for threshold array created from the filter for the float array

      // For the comparison value of 0.1, the threshold array elements 0 to 9 should be false and 10 through 19 should be true
      for(size_t i = 0; i < 20; i++)
      {
        if(!((i < 10 && inputArrayPtr[i] == false) || (i >= 10 && inputArrayPtr[i] == true)))
        {
          DREAM3D_REQUIRE_EQUAL(0, 1)
        }
      }

      ComparisonInputs comp1;
      ComparisonInput_t v1;
      v1.dataContainerName = "dc1";
      v1.attributeMatrixName = am->getName();
      v1.attributeArrayName = data1->getName(); // int array
      v1.compOperator = 1;                      // greater than
      v1.compValue = 15;                        // comparison value
      comp1.addInput(v1);

      var.setValue(comp1);
      propWasSet = filter->setProperty("SelectedThresholds", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      QString fp = "ThresholdArray1";
      QVariant qv(fp);
      propWasSet = filter->setProperty("DestinationArrayName", qv);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      filter->execute();
      DREAM3D_REQUIRED(filter->getErrorCondition(), >=, 0);

      DataArrayPath path1 = DataArrayPath("dc1", SIMPL::Defaults::CellAttributeMatrixName, "ThresholdArray1");
      IDataArray::Pointer thresholdArray1 = vdc->getAttributeMatrix(path1.getAttributeMatrixName())->getAttributeArray(path1.getDataArrayName());
      DataArray<bool>::Pointer inputArray1 = std::dynamic_pointer_cast<DataArray<bool>>(thresholdArray1);
      bool* inputArrayPtr1 = inputArray1->getPointer(0); // pointer for threshold array created from the filter for the int array

      // For the comparison value of 15, the threshold array elements 0 to 15 should be false and 16 through 19 should be true
      for(size_t i = 0; i < 20; i++)
      {
        if(!((i <= 15 && inputArrayPtr1[i] == false) || (i > 15 && inputArrayPtr1[i] == true)))
        {
          DREAM3D_REQUIRE_EQUAL(0, 1)
        }
      }
    }
    else
    {
      QString ss = QObject::tr("MultiThresholdObjectsTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
      DREAM3D_REQUIRE_EQUAL(0, 1)
    }
    return 1;
  }

  /**
* @brief
*/
  void operator()()
  {
    int err = EXIT_SUCCESS;
    DREAM3D_REGISTER_TEST(TestFilterAvailability());
    DREAM3D_REGISTER_TEST(RunTest())
  }

private:
  MultiThresholdObjectsTest(const MultiThresholdObjectsTest&); // Copy Constructor Not Implemented
  void operator=(const MultiThresholdObjectsTest&);            // Operator '=' Not Implemented
};
