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

#include "SIMPLib/Common/ComparisonInputsAdvanced.h"
#include "SIMPLib/Common/ComparisonValue.h"
#include "SIMPLib/Common/ComparisonSet.h"
#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"

class MultiThresholdObjects2Test
{
public:
  MultiThresholdObjects2Test()
  {
  }
  virtual ~MultiThresholdObjects2Test()
  {
  }
  SIMPL_TYPE_MACRO(MultiThresholdObjects2Test)

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the MultiThresholdObjects2Test Filter from the FilterManager
    QString filtName = "MultiThresholdObjects2";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The MultiThresholdObjects2Test Requires the use of the " << filtName.toStdString() << " filter which is found in the IO Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  AbstractFilter::Pointer CreateFilter()
  {
    // Create two test arrays, a float array and a int array
    DataContainerArray::Pointer dca = DataContainerArray::New();
    DataContainer::Pointer vdc = DataContainer::New("dc");
    // Set up geometry for tuples, a cuboid with dimensions 20, 10, 1
    ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
    vdc->setGeometry(image);
    size_t dims[1] = { 20 };
    image->setDimensions(dims);

    QVector<size_t> tDims(1, 0);
    tDims[0] = 20;
    QVector<size_t> cDims(1);
    cDims[0] = 1;
    float fnum = 0.0f;
    int inum = 0;
    AttributeMatrix::Pointer am = AttributeMatrix::New(tDims, SIMPL::Defaults::CellAttributeMatrixName, AttributeMatrix::Type::Cell);
    DataArray<float>::Pointer dataf = DataArray<float>::CreateArray(tDims, cDims, "TestArrayFloat");
    DataArray<int32_t>::Pointer datai = DataArray<int32_t>::CreateArray(tDims, cDims, "TestArrayInt");

    // Fill the float array with {.01,.02,.03,.04,.05,.06,.07,.08,.09,.10,.11,.12,.13,.14,.15.,16,.17,.18,.19,.20}
    // Fill the int array with { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19 }
    for (size_t i = 0; i < 20; i++)
    {
      fnum += 0.01f;
      dataf->setValue(i, fnum);  // float array
      datai->setValue(i, inum); // int array
      inum += 1;
    }
    am->addAttributeArray(dataf->getName(), dataf);   // float array
    am->addAttributeArray(datai->getName(), datai); // int array
    vdc->addAttributeMatrix(am->getName(), am);
    dca->addDataContainer(vdc);

    // Now instantiate the MultiThresholdObjects2Test Filter from the FilterManager
    QString filtName = "MultiThresholdObjects2";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
    if (nullptr != filterFactory.get())
    {
      // If we get this far, the Factory is good so creating the filter should not fail unless something has
      // horribly gone wrong in which case the system is going to come down quickly after this.
      AbstractFilter::Pointer filter = filterFactory->create();
      filter->setDataContainerArray(dca);

      return filter;
    }
    else
    {
      QString ss = QObject::tr("MultiThresholdObjects2Test Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
      DREAM3D_REQUIRE_EQUAL(0, 1)
    }
    return nullptr;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template<typename T>
  void ComparisonValueTest(AbstractFilter::Pointer filter, DataArray<T>* dataArray, SIMPL::Comparison::Enumeration comparisonOperator, T value, QString outputName)
  {
    QString dataContainerName = filter->getDataContainerArray()->getDataContainerNames().at(0);

    QVariant var;
    bool propWasSet;

    ComparisonInputsAdvanced comp;
    comp.setDataContainerName(dataContainerName);
    comp.setAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName);

    ComparisonValue::Pointer compVal = ComparisonValue::New();
    compVal->setAttributeArrayName(dataArray->getName());
    compVal->setCompOperator(comparisonOperator);
    compVal->setCompValue(value);
    comp.addInput(compVal);

    var.setValue(comp);
    propWasSet = filter->setProperty("SelectedThresholds", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    //var.setValue(SIMPL::GeneralData::ThresholdArray + QString::number(thresholdOffset));
    var.setValue(outputName);
    propWasSet = filter->setProperty("DestinationArrayName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    filter->execute();
    DREAM3D_REQUIRED(filter->getErrorCondition(), >= , 0);

    DataArrayPath path = DataArrayPath(dataContainerName, SIMPL::Defaults::CellAttributeMatrixName, outputName);
    IDataArray::Pointer thresholdArray = filter->getDataContainerArray()->getDataContainer(path.getDataContainerName())->
      getAttributeMatrix(path.getAttributeMatrixName())->getAttributeArray(path.getDataArrayName());
    DataArray<bool>* inputArray = DataArray<bool>::SafePointerDownCast(thresholdArray.get());
    bool* inputArrayPtr = inputArray->getPointer(0); // pointer for threshold array created from the filter for the float array

                                                     // For the comparison value of 0.1, the threshold array elements 0 to 9 should be false and 10 through 19 should be true
    
    T* dataArrayPtr = dataArray->getPointer(0);
    int numElements = dataArray->getSize();
    int compOperator = compVal->getCompOperator();
    for (size_t i = 0; i < numElements; i++)
    {
      switch (compOperator)
      {
      case SIMPL::Comparison::Operator_GreaterThan:
        DREAM3D_REQUIRE_EQUAL((dataArrayPtr[i] > value), inputArrayPtr[i])
        break;
      case SIMPL::Comparison::Operator_LessThan:
        DREAM3D_REQUIRE_EQUAL((dataArrayPtr[i] < value), inputArrayPtr[i])
        break;
      case SIMPL::Comparison::Operator_Equal:
        DREAM3D_REQUIRE_EQUAL((dataArrayPtr[i] == value), inputArrayPtr[i])
        break;
      case SIMPL::Comparison::Operator_NotEqual:
        DREAM3D_REQUIRE_EQUAL((dataArrayPtr[i] != value), inputArrayPtr[i])
        break;
      default:
        DREAM3D_REQUIRE_EQUAL(0, 1)
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template<typename T>
  void ComparisonValueTestAll(AbstractFilter::Pointer filter, DataArray<T>* dataArray, T comparisonValue, QString outputName)
  {
    ComparisonValueTest(filter, dataArray, SIMPL::Comparison::Operator_GreaterThan, comparisonValue, outputName + QString::number(0));

    ComparisonValueTest(filter, dataArray, SIMPL::Comparison::Operator_LessThan, comparisonValue, outputName + QString::number(1));

    ComparisonValueTest(filter, dataArray, SIMPL::Comparison::Operator_Equal, comparisonValue, outputName + QString::number(2));

    ComparisonValueTest(filter, dataArray, SIMPL::Comparison::Operator_NotEqual, comparisonValue, outputName + QString::number(3));
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void ComparisonSetTest(AbstractFilter::Pointer filter, ComparisonSet::Pointer compSet, QString outputName, bool* expectedResults)
  {
    QString dataContainerName = filter->getDataContainerArray()->getDataContainerNames().at(0);

    QVariant var;
    bool propWasSet;

    ComparisonInputsAdvanced comp;
    comp.setDataContainerName(dataContainerName);
    comp.setAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName);

    comp.addInput(compSet);

    var.setValue(comp);
    propWasSet = filter->setProperty("SelectedThresholds", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(outputName);
    propWasSet = filter->setProperty("DestinationArrayName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      filter->execute();
    DREAM3D_REQUIRED(filter->getErrorCondition(), >= , 0);

    DataArrayPath path = DataArrayPath(dataContainerName, SIMPL::Defaults::CellAttributeMatrixName, outputName);
    IDataArray::Pointer thresholdArray = filter->getDataContainerArray()->getDataContainer(path.getDataContainerName())->
      getAttributeMatrix(path.getAttributeMatrixName())->getAttributeArray(path.getDataArrayName());
    DataArray<bool>* inputArray = DataArray<bool>::SafePointerDownCast(thresholdArray.get());
    bool* inputArrayPtr = inputArray->getPointer(0); // pointer for threshold array created from the filter for the float array

                                                     // For the comparison value of 0.1, the threshold array elements 0 to 9 should be false and 10 through 19 should be true
    size_t numElements = inputArray->getSize();
    for (size_t i = 0; i < numElements; i++)
    {
      DREAM3D_REQUIRE_EQUAL(inputArrayPtr[i], expectedResults[i])
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int RunComparisonValueTests()
  {
    AbstractFilter::Pointer filter = CreateFilter();

    DataArrayPath path = DataArrayPath("dc", "CellData", "TestArrayFloat");
    IDataArray::Pointer idataf = filter->getDataContainerArray()->getDataContainer(path)->getAttributeMatrix(path)->getAttributeArray(path.getDataArrayName());
    DataArray<float>* dataf = DataArray<float>::SafePointerDownCast(idataf.get());
    ComparisonValueTestAll(filter, dataf, 0.1f, "FloatComparison");

    path.setDataArrayName("TestArrayInt");
    IDataArray::Pointer idatai = filter->getDataContainerArray()->getDataContainer(path)->getAttributeMatrix(path)->getAttributeArray(path.getDataArrayName());
    DataArray<int>* datai = DataArray<int>::SafePointerDownCast(idatai.get());
    ComparisonValueTestAll(filter, datai, 10, "IntComparison");

    return 1;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int RunComparisonSetTests()
  {
    AbstractFilter::Pointer filter = CreateFilter();

    DataArrayPath path = DataArrayPath("dc", "CellData", "TestArrayInt");
    IDataArray::Pointer idatai = filter->getDataContainerArray()->getDataContainer(path)->getAttributeMatrix(path)->getAttributeArray(path.getDataArrayName());
    //DataArray<int>* datai = DataArray<int>::SafePointerDownCast(idatai.get());
    
    // Check AND / OR operators
    ComparisonSet::Pointer compSet = ComparisonSet::New();
    
    ComparisonValue::Pointer comp0 = ComparisonValue::New();
    comp0->setCompOperator(SIMPL::Comparison::Operator_GreaterThan);
    comp0->setCompValue(5);
    comp0->setAttributeArrayName(path.getDataArrayName());
    compSet->addComparison(comp0);

    ComparisonValue::Pointer comp1 = ComparisonValue::New();
    comp1->setUnionOperator(SIMPL::Union::Operator_And);
    comp1->setAttributeArrayName(path.getDataArrayName());
    comp1->setCompOperator(SIMPL::Comparison::Operator_LessThan);
    comp1->setCompValue(10);
    compSet->addComparison(comp1);

    ComparisonValue::Pointer comp2 = ComparisonValue::New();
    comp2->setUnionOperator(SIMPL::Union::Operator_Or);
    comp2->setAttributeArrayName(path.getDataArrayName());
    comp2->setCompOperator(SIMPL::Comparison::Operator_Equal);
    comp2->setCompValue(2);
    compSet->addComparison(comp2);
    
    bool expectedOutput[] = { false, false, true, false, false,         // 0, 1, 2, 3, 4
                            false, true, true, true, true,             // 5, 6, 7, 8, 9
                            false, false, false, false, false,         // 10, 11, 12, 13, 14
                            false, false, false, false, false };       // 15, 16, 17, 18, 19

    ComparisonSetTest(filter, compSet, SIMPL::GeneralData::ThresholdArray + QString::number(1), expectedOutput);

    // Ensure that setting the first Comparison to OR does not default output to true
    ComparisonSet::Pointer compSet2 = ComparisonSet::New();
    comp0->setUnionOperator(SIMPL::Union::Operator_Or);
    compSet2->addComparison(comp0);

    bool expectedOutput2[] = { false, false, false, false, false,
                               false, true, true, true, true,
                               true, true, true, true, true,
                               true, true, true, true, true };

    ComparisonSetTest(filter, compSet2, SIMPL::GeneralData::ThresholdArray + QString::number(2), expectedOutput2);

    // Invert ComparisonSet
    for (int i = 0; i < 20; i++)
    {
      expectedOutput[i] = !expectedOutput[i];
    }
    compSet->setInvertComparison(true);
    ComparisonSetTest(filter, compSet, SIMPL::GeneralData::ThresholdArray + QString::number(3), expectedOutput);

    return 1;
  }

  /**
* @brief
*/
  void operator()()
  {
    int err = EXIT_SUCCESS;
    DREAM3D_REGISTER_TEST(TestFilterAvailability());
    DREAM3D_REGISTER_TEST(RunComparisonValueTests())
    DREAM3D_REGISTER_TEST(RunComparisonSetTests())
  }

private:
  MultiThresholdObjects2Test(const MultiThresholdObjects2Test&); // Copy Constructor Not Implemented
  void operator=(const MultiThresholdObjects2Test&);            // Operator '=' Not Implemented
};
