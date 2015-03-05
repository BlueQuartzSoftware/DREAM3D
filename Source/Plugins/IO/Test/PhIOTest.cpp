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

//#include <stdlib.h>
//#include <iostream>
//#include <vector>
//#include <limits>

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
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

#include "IOTestFileLocations.h"

#include "GenerateFeatureIds.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  QFile::remove(UnitTest::PhIOTest::TestFile);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestFilterAvailability()
{
  // Now instantiate the PhWriter Filter from the FilterManager
  QString filtName = "PhWriter";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
  if (NULL == filterFactory.get() )
  {
    std::stringstream ss;
    ss << "The PhIOTest Requires the use of the " << filtName.toStdString() << " filter which is found in the IO Plugin";
    DREAM3D_TEST_THROW_EXCEPTION(ss.str())
  }

  filtName = "PhReader";
  filterFactory = fm->getFactoryForFilter(filtName);
  if (NULL == filterFactory.get() )
  {
    std::stringstream ss;
    ss << "The PhIOTest Requires the use of the " << filtName.toStdString() << " filter which is found in the IO Plugin";
    DREAM3D_TEST_THROW_EXCEPTION(ss.str())
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestPhWriter()
{
  FilterPipeline::Pointer pipeline = FilterPipeline::New();

  int err = 0;

  // Use the helper class CreateDataContainer to generate a valid DataContainer
  CreateDataContainer::Pointer createVolumeDC = CreateDataContainer::New();
  createVolumeDC->setXDim(UnitTest::FeatureIdsTest::XSize);
  createVolumeDC->setYDim(UnitTest::FeatureIdsTest::YSize);
  createVolumeDC->setZDim(UnitTest::FeatureIdsTest::ZSize);
  pipeline->pushBack(createVolumeDC);
  // Generate some "Feature Ids" inside that DataContainer
  GenerateFeatureIds::Pointer generateFeatureIds = GenerateFeatureIds::New();
  pipeline->pushBack(generateFeatureIds);

  // Now instantiate the PhWriter Filter from the FilterManager
  QString filtName = "PhWriter";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
  if (NULL != filterFactory.get() )
  {
    // If we get this far, the Factory is good so creating the filter should not fail unless something has
    // horribly gone wrong in which case the system is going to come down quickly after this.
    AbstractFilter::Pointer phWriter = filterFactory->create();

    DataArrayPath path = DataArrayPath(DREAM3D::Defaults::DataContainerName,
                                       DREAM3D::Defaults::CellAttributeMatrixName,
                                       DREAM3D::CellData::FeatureIds);
    QVariant var;
    var.setValue(path);
    bool propWasSet = phWriter->setProperty("FeatureIdsArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    propWasSet = phWriter->setProperty("OutputFile", UnitTest::PhIOTest::TestFile);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)
    pipeline->pushBack(phWriter);
  }
  else
  {
    QString ss = QObject::tr("PhIOTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
    DREAM3D_REQUIRE_EQUAL(0, 1)
  }

  err = pipeline->preflightPipeline();
  DREAM3D_REQUIRE_EQUAL(err, 0);
  pipeline->execute();
  err = pipeline->getErrorCondition();
  DREAM3D_REQUIRE_EQUAL(err, 0);
  return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestPhReader()
{
// FilterPipeline::Pointer pipeline = FilterPipeline::New();
  DataContainerArray::Pointer dca = DataContainerArray::New();
// dca->pushBack(m);

  AbstractFilter::Pointer phReader = AbstractFilter::NullPointer();
  QString filtName = "PhReader";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
  if (NULL != filterFactory.get() )
  {
    // If we get this far, the Factory is good so creating the filter should not fail unless something has gone horribly wrong in which case the system is going to come down quickly after this.
    phReader = filterFactory->create();

    bool propWasSet = phReader->setProperty("InputFile", UnitTest::PhIOTest::TestFile);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)
    phReader->setDataContainerArray(dca);
    phReader->execute();
    int err = phReader->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, 0);
    // pipeline->pushBack(phReader);
  }
  else
  {
    QString ss = QObject::tr("PhIOTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
    qDebug() << ss;
    DREAM3D_REQUIRE_EQUAL(0, 1)
  }

  size_t nx = 0;
  size_t ny = 0;
  size_t nz = 0;


  DataContainer::Pointer m = phReader->getDataContainerArray()->getDataContainer(DREAM3D::Defaults::DataContainerName);
  DREAM3D_REQUIRED_PTR(m.get(), != , NULL)

  /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->getDimensions(nx, ny, nz);
  DREAM3D_REQUIRE_EQUAL(nx, UnitTest::FeatureIdsTest::XSize);
  DREAM3D_REQUIRE_EQUAL(ny, UnitTest::FeatureIdsTest::YSize);
  DREAM3D_REQUIRE_EQUAL(nz, UnitTest::FeatureIdsTest::ZSize);

  IDataArray::Pointer mdata = phReader->getDataContainerArray()->getDataContainer(DREAM3D::Defaults::DataContainerName)->getAttributeMatrix("CellData")->getAttributeArray(DREAM3D::CellData::FeatureIds);

  int size = UnitTest::FeatureIdsTest::XSize * UnitTest::FeatureIdsTest::YSize * UnitTest::FeatureIdsTest::ZSize;
  int32_t* data = Int32ArrayType::SafeReinterpretCast<IDataArray*, Int32ArrayType*, int32_t*>(mdata.get());

  for (int i = 0; i < size; ++i)
  {
    int32_t file_value = data[i];
    int32_t memory_value = i + UnitTest::FeatureIdsTest::Offset;
    DREAM3D_REQUIRE_EQUAL( memory_value, file_value );
  }

  return 1;
}


template<typename T>
void test(T x, T y, T z, const QString& type)
{
  T totalPoints = x * y * z;
  qDebug() << "sizeof(" << type << "): " << sizeof(T) << " totalPoints: " << totalPoints;

  if (totalPoints > std::numeric_limits<int32_t>::max() )
  {
    qDebug() << "  " << type << " would over flow 32 bit signed int";
  }
  if (totalPoints > std::numeric_limits<uint32_t>::max() )
  {
    qDebug() << "  " << type << " would over flow 32 bit unsigned int";
  }
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
  QCoreApplication::setApplicationName("PhIOTest");

  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( loadFilterPlugins() );
  DREAM3D_REGISTER_TEST( TestFilterAvailability() );

  DREAM3D_REGISTER_TEST( TestPhWriter() )
  DREAM3D_REGISTER_TEST( TestPhReader() )

  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
  PRINT_TEST_SUMMARY();
  return err;
}





