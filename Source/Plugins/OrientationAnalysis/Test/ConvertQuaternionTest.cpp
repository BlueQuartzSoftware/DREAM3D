// -----------------------------------------------------------------------------
// Insert your license & copyright information here
// -----------------------------------------------------------------------------
#pragma once


#include "SIMPLib/SIMPLib.h"

#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "UnitTestSupport.hpp"

#include "OrientationAnalysis/OrientationAnalysisFilters/ConvertQuaternion.h"
#include "OrientationAnalysisTestFileLocations.h"

class ConvertQuaternionTest
{

  public:
    ConvertQuaternionTest() = default;
    ~ConvertQuaternionTest() = default;
    ConvertQuaternionTest(const ConvertQuaternionTest&) = delete;            // Copy Constructor
    ConvertQuaternionTest(ConvertQuaternionTest&&) = delete;                 // Move Constructor
    ConvertQuaternionTest& operator=(const ConvertQuaternionTest&) = delete; // Copy Assignment
    ConvertQuaternionTest& operator=(ConvertQuaternionTest&&) = delete;      // Move Assignment

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    return 0;
  }

  // -----------------------------------------------------------------------------
  DataContainerArray::Pointer createDataStructure(const ConvertQuaternion::Pointer& filter, int32_t convType)
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();
    DataContainer::Pointer dc = DataContainer::New("Data Container");
    dca->addOrReplaceDataContainer(dc);
    std::vector<size_t> tDims = {50};
    AttributeMatrix::Pointer am = AttributeMatrix::New(tDims, "Cell Data", AttributeMatrix::Type::Cell);
    dc->addOrReplaceAttributeMatrix(am);

    std::vector<size_t> cDims = {4};
    FloatArrayType::Pointer vectorScalarPtr = FloatArrayType::CreateArray(tDims[0], cDims, "VectorScalar", true);

    am->addOrReplaceAttributeArray(vectorScalarPtr);

    filter->setDataContainerArray(dca);
    DataArrayPath inputDap("Data Container", "Cell Data", "VectorScalar");
    filter->setQuaternionDataArrayPath(inputDap);
    DataArrayPath outputDap("Data Container", "Cell Data", "ScalarVector");
    filter->setOutputDataArrayPath(outputDap);
    filter->setDeleteOriginalData(false);
    filter->setConversionType(convType);

    // Create Data
    float* data = vectorScalarPtr->getTuplePointer(0);
    for(size_t i = 0; i < tDims[0]; i++)
    {
      data[i * 4] = static_cast<float>(i + 100);
      data[i * 4 + 1] = static_cast<float>(i + 100.0f);
      data[i * 4 + 2] = static_cast<float>(i + 200.0f);
      data[i * 4 + 3] = static_cast<float>(i);
    }
    return dca;
  }
  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestConvertQuaternion()
  {
    {
      ConvertQuaternion::Pointer filter = ConvertQuaternion::New();
      DataContainerArray::Pointer dca = createDataStructure(filter, 0);
      filter->setDataContainerArray(dca);
      filter->preflight();
      int32_t err = filter->getErrorCode();

      DREAM3D_REQUIRED(err, >=, 0)
    }
    {
      ConvertQuaternion::Pointer filter = ConvertQuaternion::New();
      DataContainerArray::Pointer dca = createDataStructure(filter, 0);
      filter->setDataContainerArray(dca);
      filter->execute();
      int32_t err = filter->getErrorCode();
      DREAM3D_REQUIRED(err, >=, 0)

      FloatArrayType::Pointer inputArrayPtr = dca->getDataContainer("Data Container")->getAttributeMatrix("Cell Data")->getAttributeArrayAs<FloatArrayType>("VectorScalar");
      FloatArrayType::Pointer outputArrayPtr = dca->getDataContainer("Data Container")->getAttributeMatrix("Cell Data")->getAttributeArrayAs<FloatArrayType>("ScalarVector");

      for(size_t i = 0; i < inputArrayPtr->getNumberOfTuples(); i++)
      {
        float* in = inputArrayPtr->getTuplePointer(i);
        float* out = outputArrayPtr->getTuplePointer(i);

        DREAM3D_REQUIRED(in[0], ==, out[1]);
        DREAM3D_REQUIRED(in[1], ==, out[2]);
        DREAM3D_REQUIRED(in[2], ==, out[3]);
        DREAM3D_REQUIRED(in[3], ==, out[0]);
      }
    }
    {
      ConvertQuaternion::Pointer filter = ConvertQuaternion::New();
      DataContainerArray::Pointer dca = createDataStructure(filter, 1);
      filter->setDataContainerArray(dca);
      filter->execute();
      int32_t err = filter->getErrorCode();
      DREAM3D_REQUIRED(err, >=, 0)

      FloatArrayType::Pointer inputArrayPtr = dca->getDataContainer("Data Container")->getAttributeMatrix("Cell Data")->getAttributeArrayAs<FloatArrayType>("VectorScalar");
      FloatArrayType::Pointer outputArrayPtr = dca->getDataContainer("Data Container")->getAttributeMatrix("Cell Data")->getAttributeArrayAs<FloatArrayType>("ScalarVector");

      for(size_t i = 0; i < inputArrayPtr->getNumberOfTuples(); i++)
      {
        float* in = inputArrayPtr->getTuplePointer(i);
        float* out = outputArrayPtr->getTuplePointer(i);

        DREAM3D_REQUIRED(in[0], ==, out[3]);
        DREAM3D_REQUIRED(in[1], ==, out[0]);
        DREAM3D_REQUIRED(in[2], ==, out[1]);
        DREAM3D_REQUIRED(in[3], ==, out[2]);
      }
    }

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "########### ConvertQuaternionTest ##############" << std::endl;
    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(TestConvertQuaternion())
  }
};
