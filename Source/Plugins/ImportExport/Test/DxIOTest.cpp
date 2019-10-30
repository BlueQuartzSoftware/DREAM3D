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
#include <QtCore/QFile>
#include <QtCore/QTime>

#include <QtCore/QTextStream>

#include <QtCore/QDebug>

#include "SIMPLib/DataArrays/DataArray.hpp"

#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"

#include "UnitTestSupport.hpp"

#include "ImportExportTestFileLocations.h"

#include "GenerateFeatureIds.h"

class DxIOTest
{
public:
  DxIOTest() = default;
  virtual ~DxIOTest() = default;

  QString getNameOfClass()
  {
    return QString("DxIOTest");
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void delay(int secs)
  {
    QTime dieTime = QTime::currentTime().addSecs(secs);
    while(QTime::currentTime() < dieTime)
    {
      QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::DxIOTest::TestFile);
    QFile::remove(UnitTest::DxIOTest::TestFile2);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the DxWriter Filter from the FilterManager
    QString filtName = "DxWriter";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The DxIOTest Requires the use of the " << filtName.toStdString() << " filter which is found in the IO Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }

    filtName = "DxReader";
    filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The DxIOTest Requires the use of the " << filtName.toStdString() << " filter which is found in the IO Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestDxWriter()
  {
    // Write first file
    {
      FilterPipeline::Pointer pipeline = FilterPipeline::New();

      int err = 0;

      // Use the helper class CreateImageGeomDataContainer to generate a valid DataContainer
      CreateImageGeomDataContainer::Pointer createVolumeDC = CreateImageGeomDataContainer::New();
      createVolumeDC->setXDim(UnitTest::FeatureIdsTest::XSize);
      createVolumeDC->setYDim(UnitTest::FeatureIdsTest::YSize);
      createVolumeDC->setZDim(UnitTest::FeatureIdsTest::ZSize);
      pipeline->pushBack(createVolumeDC);

      // Generate some "Feature Ids" inside that DataContainer
      GenerateFeatureIds::Pointer generateFeatureIds = GenerateFeatureIds::New();
      pipeline->pushBack(generateFeatureIds);

      // Now instantiate the DxWriter Filter from the FilterManager
      QString filtName = "DxWriter";
      FilterManager* fm = FilterManager::Instance();
      IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
      if(nullptr != filterFactory.get())
      {
        // If we get this far, the Factory is good so creating the filter should not fail unless something has
        // horribly gone wrong in which case the system is going to come down quickly after this.
        AbstractFilter::Pointer dxWriter = filterFactory->create();

        DataArrayPath path = DataArrayPath(SIMPL::Defaults::DataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds);
        QVariant var;
        var.setValue(path);
        bool propWasSet = dxWriter->setProperty("FeatureIdsArrayPath", var);
        DREAM3D_REQUIRE_EQUAL(propWasSet, true)

        propWasSet = dxWriter->setProperty("OutputFile", UnitTest::DxIOTest::TestFile);
        DREAM3D_REQUIRE_EQUAL(propWasSet, true)

        pipeline->pushBack(dxWriter);
      }
      else
      {
        QString ss = QObject::tr("DxIOTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
        DREAM3D_REQUIRE_EQUAL(0, 1)
      }

      err = pipeline->preflightPipeline();
      DREAM3D_REQUIRE_EQUAL(err, 0);
      pipeline->execute();
      err = pipeline->getErrorCode();
      DREAM3D_REQUIRE_EQUAL(err, 0);
    }

    // Write second file
    {
      FilterPipeline::Pointer pipeline = FilterPipeline::New();

      int err = 0;

      // Use the helper class CreateImageGeomDataContainer to generate a valid DataContainer
      CreateImageGeomDataContainer::Pointer createVolumeDC = CreateImageGeomDataContainer::New();
      createVolumeDC->setXDim(UnitTest::FeatureIdsTest::XSize);
      createVolumeDC->setYDim(UnitTest::FeatureIdsTest::YSize);
      createVolumeDC->setZDim(UnitTest::FeatureIdsTest::ZSize);
      pipeline->pushBack(createVolumeDC);

      // Generate some "Feature Ids" inside that DataContainer
      GenerateFeatureIds::Pointer generateFeatureIds = GenerateFeatureIds::New();
      pipeline->pushBack(generateFeatureIds);

      // Now instantiate the DxWriter Filter from the FilterManager
      QString filtName = "DxWriter";
      FilterManager* fm = FilterManager::Instance();
      IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
      if(nullptr != filterFactory.get())
      {
        // If we get this far, the Factory is good so creating the filter should not fail unless something has
        // horribly gone wrong in which case the system is going to come down quickly after this.
        AbstractFilter::Pointer dxWriter = filterFactory->create();

        DataArrayPath path = DataArrayPath(SIMPL::Defaults::DataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds);
        QVariant var;
        var.setValue(path);
        bool propWasSet = dxWriter->setProperty("FeatureIdsArrayPath", var);
        DREAM3D_REQUIRE_EQUAL(propWasSet, true)
        propWasSet = dxWriter->setProperty("AddSurfaceLayer", QVariant(true));

        propWasSet = dxWriter->setProperty("OutputFile", UnitTest::DxIOTest::TestFile2);
        DREAM3D_REQUIRE_EQUAL(propWasSet, true)
        pipeline->pushBack(dxWriter);
      }
      else
      {
        QString ss = QObject::tr("DxIOTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
        DREAM3D_REQUIRE_EQUAL(0, 1)
      }

      err = pipeline->preflightPipeline();
      DREAM3D_REQUIRE_EQUAL(err, 0);
      pipeline->execute();
      err = pipeline->getErrorCode();
      DREAM3D_REQUIRE_EQUAL(err, 0);
    }
    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestDxReader()
  {
    // FilterPipeline::Pointer pipeline = FilterPipeline::New();
    DataContainerArray::Pointer dca = DataContainerArray::New();
    // dca->pushBack(m);

    AbstractFilter::Pointer dxReader = AbstractFilter::NullPointer();
    QString filtName = "DxReader";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr != filterFactory.get())
    {
      // If we get this far, the Factory is good so creating the filter should not fail unless something has gone horribly wrong in which case the system is going to come down quickly after this.
      dxReader = filterFactory->create();

      bool propWasSet = dxReader->setProperty("InputFile", UnitTest::DxIOTest::TestFile);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      dxReader->setDataContainerArray(dca);
      dxReader->execute();
      int err = dxReader->getErrorCode();
      DREAM3D_REQUIRE_EQUAL(err, 0);
      // pipeline->pushBack(DxReader);
    }
    else
    {
      QString ss = QObject::tr("DxIOTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
      qDebug() << ss;
      DREAM3D_REQUIRE_EQUAL(0, 1)
    }

    DataContainer::Pointer m = dxReader->getDataContainerArray()->getDataContainer(SIMPL::Defaults::ImageDataContainerName);
    DREAM3D_REQUIRED_PTR(m.get(), !=, nullptr)

    SizeVec3Type dims = m->getGeometryAs<ImageGeom>()->getDimensions();
    DREAM3D_REQUIRE_EQUAL(dims[0], UnitTest::FeatureIdsTest::XSize);
    DREAM3D_REQUIRE_EQUAL(dims[1], UnitTest::FeatureIdsTest::YSize);
    DREAM3D_REQUIRE_EQUAL(dims[2], UnitTest::FeatureIdsTest::ZSize);

    IDataArray::Pointer mdata =
        dxReader->getDataContainerArray()->getDataContainer(SIMPL::Defaults::ImageDataContainerName)->getAttributeMatrix("CellData")->getAttributeArray(SIMPL::CellData::FeatureIds);

    int size = UnitTest::FeatureIdsTest::XSize * UnitTest::FeatureIdsTest::YSize * UnitTest::FeatureIdsTest::ZSize;
    Int32ArrayType::Pointer dataPtr = std::dynamic_pointer_cast<Int32ArrayType>(mdata);
    int32_t* data = dataPtr->getTuplePointer(0);
    for(int i = 0; i < size; ++i)
    {
      int32_t file_value = data[i];
      int32_t memory_value = i + UnitTest::FeatureIdsTest::Offset;
      DREAM3D_REQUIRE_EQUAL(memory_value, file_value);
    }

    return 1;
  }

  template <typename T> void test(T x, T y, T z, const QString& type)
  {
    T totalPoints = x * y * z;
    qDebug() << "sizeof(" << type << "): " << sizeof(T) << " totalPoints: " << totalPoints;

    if(totalPoints > std::numeric_limits<int32_t>::max())
    {
      qDebug() << "  " << type << " would over flow 32 bit signed int";
    }
    if(totalPoints > std::numeric_limits<uint32_t>::max())
    {
      qDebug() << "  " << type << " would over flow 32 bit unsigned int";
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestDxReaderCache()
  {
    AbstractFilter::Pointer dxReader = AbstractFilter::NullPointer();
    QString filtName = "DxReader";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);

    // Reading first file
    {
      DataContainerArray::Pointer dca = DataContainerArray::New();

      if(nullptr != filterFactory.get())
      {
        // If we get this far, the Factory is good so creating the filter should not fail unless something has gone horribly wrong in which case the system is going to come down quickly after this.
        dxReader = filterFactory->create(); // Create the reader for the first time
        bool propWasSet = dxReader->setProperty("InputFile", UnitTest::DxIOTest::TestFile);
        DREAM3D_REQUIRE_EQUAL(propWasSet, true)
        dxReader->setDataContainerArray(dca);
        dxReader->preflight();
        int err = dxReader->getErrorCode();
        DREAM3D_REQUIRE_EQUAL(err, 0);
      }
      else
      {
        QString ss = QObject::tr("DxIOTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
        qDebug() << ss;
        DREAM3D_REQUIRE_EQUAL(0, 1)
      }

      DataContainer::Pointer m = dxReader->getDataContainerArray()->getDataContainer(SIMPL::Defaults::ImageDataContainerName);
      DREAM3D_REQUIRED_PTR(m.get(), !=, nullptr)

      SizeVec3Type dims = m->getGeometryAs<ImageGeom>()->getDimensions();
      DREAM3D_REQUIRE_EQUAL(dims[0], UnitTest::FeatureIdsTest::XSize);
      DREAM3D_REQUIRE_EQUAL(dims[1], UnitTest::FeatureIdsTest::YSize);
      DREAM3D_REQUIRE_EQUAL(dims[2], UnitTest::FeatureIdsTest::ZSize);

      // Check that the filter read the file
      bool prop = dxReader->property("FileWasRead").toBool();
      DREAM3D_REQUIRE_EQUAL(prop, true)
    }

    // Reading the file again
    {
      DataContainerArray::Pointer dca = DataContainerArray::New();

      if(nullptr != filterFactory.get())
      {
        bool propWasSet = dxReader->setProperty("InputFile", UnitTest::DxIOTest::TestFile);
        DREAM3D_REQUIRE_EQUAL(propWasSet, true)
        dxReader->setDataContainerArray(dca);
        dxReader->preflight();
        int err = dxReader->getErrorCode();
        DREAM3D_REQUIRE_EQUAL(err, 0);
      }
      else
      {
        QString ss = QObject::tr("DxIOTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
        qDebug() << ss;
        DREAM3D_REQUIRE_EQUAL(0, 1)
      }

      DataContainer::Pointer m = dxReader->getDataContainerArray()->getDataContainer(SIMPL::Defaults::ImageDataContainerName);
      DREAM3D_REQUIRED_PTR(m.get(), !=, nullptr)

      SizeVec3Type dims = m->getGeometryAs<ImageGeom>()->getDimensions();
      DREAM3D_REQUIRE_EQUAL(dims[0], UnitTest::FeatureIdsTest::XSize);
      DREAM3D_REQUIRE_EQUAL(dims[1], UnitTest::FeatureIdsTest::YSize);
      DREAM3D_REQUIRE_EQUAL(dims[2], UnitTest::FeatureIdsTest::ZSize);

      // Check that the filter read from the cache this time, since we're reading from the same file
      bool prop = dxReader->property("FileWasRead").toBool();
      DREAM3D_REQUIRE_EQUAL(prop, false)
    }

    // Reading a different file
    {
      DataContainerArray::Pointer dca = DataContainerArray::New();

      if(nullptr != filterFactory.get())
      {
        bool propWasSet = dxReader->setProperty("InputFile", UnitTest::DxIOTest::TestFile2);
        DREAM3D_REQUIRE_EQUAL(propWasSet, true)
        dxReader->setDataContainerArray(dca);
        dxReader->preflight();
        int err = dxReader->getErrorCode();
        DREAM3D_REQUIRE_EQUAL(err, 0);
      }
      else
      {
        QString ss = QObject::tr("DxIOTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
        qDebug() << ss;
        DREAM3D_REQUIRE_EQUAL(0, 1)
      }

      DataContainer::Pointer m = dxReader->getDataContainerArray()->getDataContainer(SIMPL::Defaults::ImageDataContainerName);
      DREAM3D_REQUIRED_PTR(m.get(), !=, nullptr)

      SizeVec3Type dims = m->getGeometryAs<ImageGeom>()->getDimensions();
      DREAM3D_REQUIRE_EQUAL(dims[0], UnitTest::FeatureIdsTest::XSize + 2); // Add 2 because we added a suface layer during the writing of the file
      DREAM3D_REQUIRE_EQUAL(dims[1], UnitTest::FeatureIdsTest::YSize + 2);
      DREAM3D_REQUIRE_EQUAL(dims[2], UnitTest::FeatureIdsTest::ZSize + 2);

      // Check that the filter read from the file again, since we changed file names
      bool prop = dxReader->property("FileWasRead").toBool();
      DREAM3D_REQUIRE_EQUAL(prop, true)
    }

    // Reading the same file, but the contents changed outside the program
    {
      delay(1); // This delay needs to be here to make the lastModified and lastRead logic in the reader to work for all OSes
      // Change the contents of the file to be read
      {
        QFile file(UnitTest::DxIOTest::TestFile2);
        if(!file.open(QFile::ReadWrite | QFile::Append))
          DREAM3D_REQUIRE_EQUAL(0, 1)
        QTextStream out(&file);
        out << "This test string should force the filter to read from the file instead of the cache.";
        file.close();
      }

      DataContainerArray::Pointer dca = DataContainerArray::New();

      if(nullptr != filterFactory.get())
      {
        bool propWasSet = dxReader->setProperty("InputFile", UnitTest::DxIOTest::TestFile2);
        DREAM3D_REQUIRE_EQUAL(propWasSet, true)
        dxReader->setDataContainerArray(dca);
        dxReader->preflight();
        int err = dxReader->getErrorCode();
        DREAM3D_REQUIRE_EQUAL(err, 0);
      }
      else
      {
        QString ss = QObject::tr("DxIOTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
        qDebug() << ss;
        DREAM3D_REQUIRE_EQUAL(0, 1)
      }

      DataContainer::Pointer m = dxReader->getDataContainerArray()->getDataContainer(SIMPL::Defaults::ImageDataContainerName);
      DREAM3D_REQUIRED_PTR(m.get(), !=, nullptr)

      SizeVec3Type dims = m->getGeometryAs<ImageGeom>()->getDimensions();

      DREAM3D_REQUIRE_EQUAL(dims[0], UnitTest::FeatureIdsTest::XSize + 2); // Add 2 because we added a suface layer during the writing of the file
      DREAM3D_REQUIRE_EQUAL(dims[1], UnitTest::FeatureIdsTest::YSize + 2);
      DREAM3D_REQUIRE_EQUAL(dims[2], UnitTest::FeatureIdsTest::ZSize + 2);

      // Check that the filter read from the file again, since we changed the contents of the file outside the program
      bool prop = dxReader->property("FileWasRead").toBool();
      DREAM3D_REQUIRE_EQUAL(prop, true)
    }

    // Reading the same file, but we are testing the cache flush function
    {
      DataContainerArray::Pointer dca = DataContainerArray::New();

      // Flush the cache by invoking the flushCache method dynamically, using Qt's meta system
      if(QMetaObject::invokeMethod(dxReader.get(), "flushCache", Qt::DirectConnection) == false)
        DREAM3D_REQUIRE_EQUAL(0, 1)

      if(nullptr != filterFactory.get())
      {
        bool propWasSet = dxReader->setProperty("InputFile", UnitTest::DxIOTest::TestFile2);
        DREAM3D_REQUIRE_EQUAL(propWasSet, true)
        dxReader->setDataContainerArray(dca);
        dxReader->preflight();
        int err = dxReader->getErrorCode();
        DREAM3D_REQUIRE_EQUAL(err, 0);
      }
      else
      {
        QString ss = QObject::tr("DxIOTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
        qDebug() << ss;
        DREAM3D_REQUIRE_EQUAL(0, 1)
      }

      DataContainer::Pointer m = dxReader->getDataContainerArray()->getDataContainer(SIMPL::Defaults::ImageDataContainerName);
      DREAM3D_REQUIRED_PTR(m.get(), !=, nullptr)

      SizeVec3Type dims = m->getGeometryAs<ImageGeom>()->getDimensions();
      DREAM3D_REQUIRE_EQUAL(dims[0], UnitTest::FeatureIdsTest::XSize + 2); // Add 2 because we added a suface layer during the writing of the file
      DREAM3D_REQUIRE_EQUAL(dims[1], UnitTest::FeatureIdsTest::YSize + 2);
      DREAM3D_REQUIRE_EQUAL(dims[2], UnitTest::FeatureIdsTest::ZSize + 2);

      // Check that the filter read from the file again, since we flushed the cache
      bool prop = dxReader->property("FileWasRead").toBool();
      DREAM3D_REQUIRE_EQUAL(prop, true)
    }

    return EXIT_SUCCESS;
  }

  /**
   * @brief
   */
  void operator()()
  {
    int err = EXIT_SUCCESS;
    std::cout << "<===== Start " << getNameOfClass().toStdString() << std::endl;

    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(TestDxWriter())
    DREAM3D_REGISTER_TEST(TestDxReader())
    DREAM3D_REGISTER_TEST(TestDxReaderCache())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

private:
  DxIOTest(const DxIOTest&);       // Copy Constructor Not Implemented
  void operator=(const DxIOTest&); // Move assignment Not Implemented
};
