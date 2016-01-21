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
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"

#include "DREAM3DTestFileLocations.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestFilterAvailability()
{
  // Now instantiate the CreateFeatureArrayFromElementArray Filter from the FilterManager
  QString filtName = "CreateFeatureArrayFromElementArray";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
  if (NULL == filterFactory.get())
  {
    std::stringstream ss;
    ss << "The CreateFeatureArrayFromElementArrayTest Requires the use of the " << filtName.toStdString() << " filter which is found in the AMProcessMonitoring Plugin";
    DREAM3D_TEST_THROW_EXCEPTION(ss.str())
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CreateFeatureArrayFromElementArrayTest()
{
  // Single Component Warning Test
  {
    FilterPipeline::Pointer pipeline = FilterPipeline::New();

    QString filtName = "CreateFeatureArrayFromElementArray";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
    if (NULL != filterFactory.get())
    {
      // If we get this far, the Factory is good so creating the filter should not fail unless something has
      // horribly gone wrong in which case the system is going to come down quickly after this.
      AbstractFilter::Pointer createFeatureArrayFromElementArrayFilter = filterFactory->create();

      DataContainerArray::Pointer dca = createFeatureArrayFromElementArrayFilter->getDataContainerArray();

      DataContainer::Pointer dc = DataContainer::New("DataContainer");

      AttributeMatrix::Pointer cellAttr = AttributeMatrix::New(QVector<size_t>(1, 16), "Cell Attribute Matrix", DREAM3D::AttributeMatrixType::Cell);
      AttributeMatrix::Pointer featureAttr = AttributeMatrix::New(QVector<size_t>(1, 5), "Feature Attribute Matrix", DREAM3D::AttributeMatrixType::Cell);

      DataArray<int32_t>::Pointer featureIds = DataArray<int32_t>::CreateArray(16, "FeatureIds");
      size_t numTuples = featureIds->getNumberOfTuples();
      featureIds->initializeTuple(0, 1);
      featureIds->initializeTuple(1, 1);
      featureIds->initializeTuple(2, 2);
      featureIds->initializeTuple(3, 2);
      featureIds->initializeTuple(4, 1);
      featureIds->initializeTuple(5, 1);
      featureIds->initializeTuple(6, 2);
      featureIds->initializeTuple(7, 2);
      featureIds->initializeTuple(8, 3);
      featureIds->initializeTuple(9, 3);
      featureIds->initializeTuple(10, 4);
      featureIds->initializeTuple(11, 4);
      featureIds->initializeTuple(12, 3);
      featureIds->initializeTuple(13, 3);
      featureIds->initializeTuple(14, 4);
      featureIds->initializeTuple(15, 4);

      DataArray<float>::Pointer cellDataArray = DataArray<float>::CreateArray(16, "CellData");
      numTuples = cellDataArray->getNumberOfTuples();
      for (size_t i = 0; i < numTuples; i++)
      {
        cellDataArray->initializeTuple(i, i + 1);
      }

      cellAttr->addAttributeArray("CellData", cellDataArray);
      cellAttr->addAttributeArray("FeatureIds", featureIds);
      dc->addAttributeMatrix("Cell Attribute Matrix", cellAttr);
      dc->addAttributeMatrix("Feature Attribute Matrix", featureAttr);
      dca->addDataContainer(dc);

      DataArrayPath path = DataArrayPath(dc->getName(), cellAttr->getName(), cellDataArray->getName());
      QVariant var;
      var.setValue(path);
      bool propWasSet = createFeatureArrayFromElementArrayFilter->setProperty("SelectedCellArrayPath", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      path = DataArrayPath(dc->getName(), cellAttr->getName(), featureIds->getName());
      var.setValue(path);
      propWasSet = createFeatureArrayFromElementArrayFilter->setProperty("FeatureIdsArrayPath", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      path = DataArrayPath(dc->getName(), featureAttr->getName(), QString());
      var.setValue(path);
      propWasSet = createFeatureArrayFromElementArrayFilter->setProperty("CellFeatureAttributeMatrixName", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      QString createdName = "CreatedArray";
      propWasSet = createFeatureArrayFromElementArrayFilter->setProperty("CreatedArrayName", createdName);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      createFeatureArrayFromElementArrayFilter->execute();
    }
    else
    {
      QString ss = QObject::tr("CreateFeatureArrayFromElementArrayTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
      DREAM3D_REQUIRE_EQUAL(0, 1)
    }
  }

  // Multi-Component Warning Test
  {
    FilterPipeline::Pointer pipeline = FilterPipeline::New();

    QString filtName = "CreateFeatureArrayFromElementArray";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
    if (NULL != filterFactory.get())
    {
      // If we get this far, the Factory is good so creating the filter should not fail unless something has
      // horribly gone wrong in which case the system is going to come down quickly after this.
      AbstractFilter::Pointer createFeatureArrayFromElementArrayFilter = filterFactory->create();

      DataContainerArray::Pointer dca = createFeatureArrayFromElementArrayFilter->getDataContainerArray();

      DataContainer::Pointer dc = DataContainer::New("DataContainer");

      AttributeMatrix::Pointer cellAttr = AttributeMatrix::New(QVector<size_t>(1, 16), "Cell Attribute Matrix", DREAM3D::AttributeMatrixType::Cell);
      AttributeMatrix::Pointer featureAttr = AttributeMatrix::New(QVector<size_t>(1, 5), "Feature Attribute Matrix", DREAM3D::AttributeMatrixType::Cell);

      DataArray<int32_t>::Pointer featureIds = DataArray<int32_t>::CreateArray(16, "FeatureIds");
      size_t numTuples = featureIds->getNumberOfTuples();
      featureIds->initializeTuple(0, 1);
      featureIds->initializeTuple(1, 1);
      featureIds->initializeTuple(2, 2);
      featureIds->initializeTuple(3, 2);
      featureIds->initializeTuple(4, 1);
      featureIds->initializeTuple(5, 1);
      featureIds->initializeTuple(6, 2);
      featureIds->initializeTuple(7, 2);
      featureIds->initializeTuple(8, 3);
      featureIds->initializeTuple(9, 3);
      featureIds->initializeTuple(10, 4);
      featureIds->initializeTuple(11, 4);
      featureIds->initializeTuple(12, 3);
      featureIds->initializeTuple(13, 3);
      featureIds->initializeTuple(14, 4);
      featureIds->initializeTuple(15, 4);

      QVector<size_t> cDims = QVector<size_t>(1, 3);
      DataArray<float>::Pointer cellDataArray = DataArray<float>::CreateArray(16, cDims, "CellData");
      numTuples = cellDataArray->getNumberOfTuples();
      for (size_t i = 0; i < numTuples; i++)
      {
        cellDataArray->initializeTuple(i, i + 1);
      }

      cellAttr->addAttributeArray("CellData", cellDataArray);
      cellAttr->addAttributeArray("FeatureIds", featureIds);
      dc->addAttributeMatrix("Cell Attribute Matrix", cellAttr);
      dc->addAttributeMatrix("Feature Attribute Matrix", featureAttr);
      dca->addDataContainer(dc);

      DataArrayPath path = DataArrayPath(dc->getName(), cellAttr->getName(), cellDataArray->getName());
      QVariant var;
      var.setValue(path);
      bool propWasSet = createFeatureArrayFromElementArrayFilter->setProperty("SelectedCellArrayPath", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

        path = DataArrayPath(dc->getName(), cellAttr->getName(), featureIds->getName());
      var.setValue(path);
      propWasSet = createFeatureArrayFromElementArrayFilter->setProperty("FeatureIdsArrayPath", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

        path = DataArrayPath(dc->getName(), featureAttr->getName(), QString());
      var.setValue(path);
      propWasSet = createFeatureArrayFromElementArrayFilter->setProperty("CellFeatureAttributeMatrixName", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

        QString createdName = "CreatedArray";
      propWasSet = createFeatureArrayFromElementArrayFilter->setProperty("CreatedArrayName", createdName);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      createFeatureArrayFromElementArrayFilter->execute();
    }
    else
    {
      QString ss = QObject::tr("CreateFeatureArrayFromElementArrayTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
      DREAM3D_REQUIRE_EQUAL(0, 1)
    }
  }

  // Passing Test
  {
    FilterPipeline::Pointer pipeline = FilterPipeline::New();

    QString filtName = "CreateFeatureArrayFromElementArray";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
    if (NULL != filterFactory.get())
    {
      // If we get this far, the Factory is good so creating the filter should not fail unless something has
      // horribly gone wrong in which case the system is going to come down quickly after this.
      AbstractFilter::Pointer createFeatureArrayFromElementArrayFilter = filterFactory->create();

      DataContainerArray::Pointer dca = createFeatureArrayFromElementArrayFilter->getDataContainerArray();

      DataContainer::Pointer dc = DataContainer::New("DataContainer");

      AttributeMatrix::Pointer cellAttr = AttributeMatrix::New(QVector<size_t>(1, 16), "Cell Attribute Matrix", DREAM3D::AttributeMatrixType::Cell);
      AttributeMatrix::Pointer featureAttr = AttributeMatrix::New(QVector<size_t>(1, 5), "Feature Attribute Matrix", DREAM3D::AttributeMatrixType::Cell);

      DataArray<int32_t>::Pointer featureIds = DataArray<int32_t>::CreateArray(16, "FeatureIds");
      featureIds->initializeTuple(0, 1);
      featureIds->initializeTuple(1, 1);
      featureIds->initializeTuple(2, 2);
      featureIds->initializeTuple(3, 2);
      featureIds->initializeTuple(4, 1);
      featureIds->initializeTuple(5, 1);
      featureIds->initializeTuple(6, 2);
      featureIds->initializeTuple(7, 2);
      featureIds->initializeTuple(8, 3);
      featureIds->initializeTuple(9, 3);
      featureIds->initializeTuple(10, 4);
      featureIds->initializeTuple(11, 4);
      featureIds->initializeTuple(12, 3);
      featureIds->initializeTuple(13, 3);
      featureIds->initializeTuple(14, 4);
      featureIds->initializeTuple(15, 4);

      cellAttr->addAttributeArray("FeatureIds", featureIds);
      dc->addAttributeMatrix("Cell Attribute Matrix", cellAttr);
      dc->addAttributeMatrix("Feature Attribute Matrix", featureAttr);
      dca->addDataContainer(dc);

      DataArrayPath path = DataArrayPath(dc->getName(), cellAttr->getName(), featureIds->getName());
      QVariant var;
      var.setValue(path);
      bool propWasSet = createFeatureArrayFromElementArrayFilter->setProperty("FeatureIdsArrayPath", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      propWasSet = createFeatureArrayFromElementArrayFilter->setProperty("SelectedCellArrayPath", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      path = DataArrayPath(dc->getName(), featureAttr->getName(), QString());
      var.setValue(path);
      propWasSet = createFeatureArrayFromElementArrayFilter->setProperty("CellFeatureAttributeMatrixName", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

        QString createdName = "CreatedArray";
      propWasSet = createFeatureArrayFromElementArrayFilter->setProperty("CreatedArrayName", createdName);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      createFeatureArrayFromElementArrayFilter->execute();
      int err = createFeatureArrayFromElementArrayFilter->getErrorCondition();
      DREAM3D_REQUIRE_EQUAL(err, 0);
    }
    else
    {
      QString ss = QObject::tr("CreateFeatureArrayFromElementArrayTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
      DREAM3D_REQUIRE_EQUAL(0, 1)
    }
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
  QCoreApplication::setOrganizationName("");
  QCoreApplication::setOrganizationDomain("");
  QCoreApplication::setApplicationName("CreateFeatureArrayFromElementArrayTest");

  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( loadFilterPlugins() );
  DREAM3D_REGISTER_TEST( TestFilterAvailability() );

  DREAM3D_REGISTER_TEST( CreateFeatureArrayFromElementArrayTest() )

  PRINT_TEST_SUMMARY();
  return err;
}
