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

#include "SIMPLib/Common/ComparisonInputs.h"
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

    // Now instantiate the MultiThresholdObjectsTest Filter from the FilterManager
    QString filtName = "MultiThresholdObjects";
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
      QString ss = QObject::tr("MultiThresholdObjectsTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
      DREAM3D_REQUIRE_EQUAL(0, 1)
    }
    return nullptr;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template<typename T>
  void ComparisonTest(AbstractFilter::Pointer filter, DataArray<T>* dataArray, ComparisonInput_t input, QString outputName)
  {
    QString dataContainerName = filter->getDataContainerArray()->getDataContainerNames().at(0);

    QVariant var;
    bool propWasSet;

    ComparisonInputs comp;
    comp.addInput(input);

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
    int compOperator = input.compOperator;
    for (size_t i = 0; i < numElements; i++)
    {
      switch (compOperator)
      {
      case SIMPL::Comparison::Operator_GreaterThan:
        DREAM3D_REQUIRE_EQUAL((dataArrayPtr[i] > input.compValue), inputArrayPtr[i])
        break;
      case SIMPL::Comparison::Operator_LessThan:
        DREAM3D_REQUIRE_EQUAL((dataArrayPtr[i] < input.compValue), inputArrayPtr[i])
        break;
      case SIMPL::Comparison::Operator_Equal:
        DREAM3D_REQUIRE_EQUAL((dataArrayPtr[i] == input.compValue), inputArrayPtr[i])
        break;
      case SIMPL::Comparison::Operator_NotEqual:
        DREAM3D_REQUIRE_EQUAL((dataArrayPtr[i] != input.compValue), inputArrayPtr[i])
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
  void ComparisonTestAll(AbstractFilter::Pointer filter, DataArrayPath path, DataArray<T>* dataArray, double comparisonValue, QString outputName)
  {
    ComparisonInput_t comparison;
    comparison.dataContainerName = path.getDataContainerName();
    comparison.attributeMatrixName = path.getAttributeMatrixName();
    comparison.attributeArrayName = dataArray->getName();                  // input array
    comparison.compOperator = SIMPL::Comparison::Operator_GreaterThan;     // greater than
    comparison.compValue = comparisonValue;                                // comparison value

    ComparisonTest(filter, dataArray, comparison, outputName + QString::number(0));

    comparison.compOperator = SIMPL::Comparison::Operator_LessThan;
    ComparisonTest(filter, dataArray, comparison, outputName + QString::number(1));

    comparison.compOperator = SIMPL::Comparison::Operator_Equal;
    ComparisonTest(filter, dataArray, comparison, outputName + QString::number(2));

    comparison.compOperator = SIMPL::Comparison::Operator_NotEqual;
    ComparisonTest(filter, dataArray, comparison, outputName + QString::number(3));
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void ChainComparisonTest(AbstractFilter::Pointer filter, ComparisonInputs comp, QString outputName, bool* expectedResults)
  {
    QString dataContainerName = filter->getDataContainerArray()->getDataContainerNames().at(0);

    QVariant var;
    bool propWasSet;

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
    int numElements = inputArray->getSize();
    for (size_t i = 0; i < numElements; i++)
    {
      DREAM3D_REQUIRE_EQUAL(inputArrayPtr[i], expectedResults[i])
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int RunSingleComparisonTests()
  {
    AbstractFilter::Pointer filter = CreateFilter();

    DataArrayPath path = DataArrayPath("dc", "CellData", "TestArrayFloat");
    IDataArray::Pointer idataf = filter->getDataContainerArray()->getDataContainer(path)->getAttributeMatrix(path)->getAttributeArray(path.getDataArrayName());
    DataArray<float>* dataf = DataArray<float>::SafePointerDownCast(idataf.get());
    ComparisonTestAll(filter, path, dataf, 0.1, "FloatComparison");

    path.setDataArrayName("TestArrayInt");
    IDataArray::Pointer idatai = filter->getDataContainerArray()->getDataContainer(path)->getAttributeMatrix(path)->getAttributeArray(path.getDataArrayName());
    DataArray<int>* datai = DataArray<int>::SafePointerDownCast(idatai.get());
    ComparisonTestAll(filter, path, datai, 10, "IntComparison");

    return 1;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int RunGroupComparisonTests()
  {
    AbstractFilter::Pointer filter = CreateFilter();

    DataArrayPath path = DataArrayPath("dc", "CellData", "TestArrayInt");
    IDataArray::Pointer idatai = filter->getDataContainerArray()->getDataContainer(path)->getAttributeMatrix(path)->getAttributeArray(path.getDataArrayName());
    DataArray<int>* datai = DataArray<int>::SafePointerDownCast(idatai.get());
    
    ComparisonInputs inputs;
    
    ComparisonInput_t comp0;
    comp0.unionOperator = SIMPL::Union::Operator_And;
    comp0.dataContainerName = path.getDataContainerName();
    comp0.attributeMatrixName = path.getAttributeMatrixName();
    comp0.attributeArrayName = path.getDataArrayName();                // input array
    comp0.compOperator = SIMPL::Comparison::Operator_GreaterThan;     // greater than
    comp0.compValue = 5;                                              // comparison value
    inputs.addInput(comp0);

    ComparisonInput_t comp1;
    comp1.unionOperator = SIMPL::Union::Operator_And;
    comp1.dataContainerName = path.getDataContainerName();
    comp1.attributeMatrixName = path.getAttributeMatrixName();
    comp1.attributeArrayName = path.getDataArrayName();               // input array
    comp1.compOperator = SIMPL::Comparison::Operator_LessThan;        // greater than
    comp1.compValue = 10;                                             // comparison value
    inputs.addInput(comp1);

    ComparisonInput_t comp2;
    comp2.unionOperator = SIMPL::Union::Operator_Or;
    comp2.dataContainerName = path.getDataContainerName();
    comp2.attributeMatrixName = path.getAttributeMatrixName();
    comp2.attributeArrayName = path.getDataArrayName();               // input array
    comp2.compOperator = SIMPL::Comparison::Operator_Equal;           // greater than
    comp2.compValue = 2;                                              // comparison value
    inputs.addInput(comp2);
    
    bool expectedOutput[] = { false, false, true, false, false,         // 0, 1, 2, 3, 4
                            false, true, true, true, true,             // 5, 6, 7, 8, 9
                            false, false, false, false, false,         // 10, 11, 12, 13, 14
                            false, false, false, false, false };       // 15, 16, 17, 18, 19

    ChainComparisonTest(filter, inputs, SIMPL::GeneralData::ThresholdArray, expectedOutput);

    ComparisonInputs inputs2;
    comp0.unionOperator = SIMPL::Union::Operator_Or;
    inputs2.addInput(comp0);

    bool expectedOutput2[] = { false, false, false, false, false,
                               false, true, true, true, true,
                               true, true, true, true, true,
                               true, true, true, true, true };

    ChainComparisonTest(filter, inputs2, SIMPL::GeneralData::ThresholdArray + QString::number(2), expectedOutput2);

    return 1;
  }

  /**
* @brief
*/
  void operator()()
  {
    int err = EXIT_SUCCESS;
    DREAM3D_REGISTER_TEST(TestFilterAvailability());
    DREAM3D_REGISTER_TEST(RunSingleComparisonTests())
    DREAM3D_REGISTER_TEST(RunGroupComparisonTests())
  }

private:
  MultiThresholdObjectsTest(const MultiThresholdObjectsTest&); // Copy Constructor Not Implemented
  void operator=(const MultiThresholdObjectsTest&);            // Operator '=' Not Implemented
};
