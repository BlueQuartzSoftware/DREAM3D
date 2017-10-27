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
#include <QtCore/QDir>

#include <QtGui/QImage>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "UnitTestSupport.hpp"

#include "EMMPMTestFileLocations.h"

class EMMPMSegmentationTest
{
public:
  EMMPMSegmentationTest()
  {
  }
  virtual ~EMMPMSegmentationTest()
  {
  }
  SIMPL_TYPE_MACRO(EMMPMSegmentationTest)

  enum ErrorCodes
  {
    NO_ERROR = 0,
    COMPONENTS_DONT_MATCH = -503
  };

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::EMMPMSegmentationTest::TestFile);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    {
      // Now instantiate the EMMPM Filter from the FilterManager
      QString filtName = "EMMPMFilter";
      FilterManager* fm = FilterManager::Instance();
      IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
      if(nullptr == filterFactory.get())
      {
        std::stringstream ss;
        ss << "Unable to initialize the EMMPMFilter while executing the EMMPMSegmentationTest.";
        DREAM3D_TEST_THROW_EXCEPTION(ss.str())
      }
    }

    {
      // Now instantiate the MultiEmmpm Filter from the FilterManager
      QString filtName = "MultiEmmpmFilter";
      FilterManager* fm = FilterManager::Instance();
      IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
      if(nullptr == filterFactory.get())
      {
        std::stringstream ss;
        ss << "Unable to initialize the MultiEmmpmFilter while executing the EMMPMSegmentationTest.";
        DREAM3D_TEST_THROW_EXCEPTION(ss.str())
      }
    }

    {
      // Now instantiate the ItkReadImage Filter from the FilterManager
      m_ImageProcessingPluginLoaded = 1; // Assume it loaded.
      QString filtName = m_ReadImageFilterName;
      FilterManager* fm = FilterManager::Instance();
      IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
      if(nullptr == filterFactory.get())
      {
        std::stringstream ss;
        ss << "Unable to initialize the " << m_ReadImageFilterName.toStdString() << " while executing the EMMPMSegmentationTest.";
        std::cout << ss.str() << std::endl;
        m_ImageProcessingPluginLoaded = 0; // Plugin or filter not available.
      }
    }

    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void createAndAddReadImageFilter(FilterPipeline::Pointer pipeline, QString inputFile)
  {

    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(m_ReadImageFilterName);

    if(nullptr != filterFactory.get())
    {
      // If we get this far, the Factory is good so creating the filter should not fail unless something has
      // horribly gone wrong in which case the system is going to come down quickly after this.
      AbstractFilter::Pointer filter = filterFactory->create();

      QVariant var;
      bool propWasSet;

      var.setValue(inputFile);
      propWasSet = filter->setProperty("InputFileName", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      pipeline->pushBack(filter);
    }
    else
    {
      QString ss = QObject::tr("EMMPMSegmentationTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(m_ReadImageFilterName);
      DREAM3D_REQUIRE_EQUAL(0, 1)
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void createAndAddConvertRGBToGrayscaleFilter(FilterPipeline::Pointer pipeline, DataArrayPath path, const QString& name, const QString& outputAttributeMatrixName)
  {
    QString filtName = "ItkRGBToGray";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);

    if(nullptr != filterFactory.get())
    {
      // If we get this far, the Factory is good so creating the filter should not fail unless something has
      // horribly gone wrong in which case the system is going to come down quickly after this.
      AbstractFilter::Pointer filter = filterFactory->create();

      QVariant var;
      bool propWasSet;

      QVector<DataArrayPath> paths(1, path);

      var.setValue(paths);
      propWasSet = filter->setProperty("InputDataArrayVector", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(name);
      propWasSet = filter->setProperty("OutputArrayPrefix", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(outputAttributeMatrixName);
      propWasSet = filter->setProperty("OutputAttributeMatrixName", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      pipeline->pushBack(filter);
    }
    else
    {
      QString ss = QObject::tr("EMMPMSegmentationTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
      DREAM3D_REQUIRE_EQUAL(0, 1)
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void createAndAddEMMPMFilter(FilterPipeline::Pointer pipeline, DataArrayPath input, DataArrayPath output)
  {
    QString filtName = "EMMPMFilter";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);

    if(nullptr != filterFactory.get())
    {
      // If we get this far, the Factory is good so creating the filter should not fail unless something has
      // horribly gone wrong in which case the system is going to come down quickly after this.
      AbstractFilter::Pointer filter = filterFactory->create();

      QVariant var;
      bool propWasSet;

      var.setValue(input);
      propWasSet = filter->setProperty("InputDataArrayPath", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(output);
      propWasSet = filter->setProperty("OutputDataArrayPath", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      pipeline->pushBack(filter);
    }
    else
    {
      QString ss = QObject::tr("EMMPMSegmentationTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
      DREAM3D_REQUIRE_EQUAL(0, 1)
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void createAndAddMultiEMMPMFilter(FilterPipeline::Pointer pipeline, QVector<DataArrayPath> input)
  {
    QString filtName = "MultiEmmpmFilter";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);

    if(nullptr != filterFactory.get())
    {
      // If we get this far, the Factory is good so creating the filter should not fail unless something has
      // horribly gone wrong in which case the system is going to come down quickly after this.
      AbstractFilter::Pointer filter = filterFactory->create();

      QVariant var;
      bool propWasSet;

      var.setValue(input);
      propWasSet = filter->setProperty("InputDataArrayVector", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      pipeline->pushBack(filter);
    }
    else
    {
      QString ss = QObject::tr("EMMPMSegmentationTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
      DREAM3D_REQUIRE_EQUAL(0, 1)
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestEMMPMSegmentation()
  {
    int h = 100;
    int w = 100;
    QImage inputImage(h, w, QImage::Format_ARGB32);
    for(int i = 0; i < h; i++)
    {
      for(int j = 0; j < w; j++)
      {
        inputImage.setPixel(i, j, 0xAA);
      }
    }
    bool saveResult = inputImage.save(UnitTest::EMMPMSegmentationTest::TestFile);
    DREAM3D_REQUIRE_EQUAL(saveResult, true)

    FilterPipeline::Pointer pipeline = FilterPipeline::New();
    //    Observer observer;
    //    pipeline->addMessageReceiver(&observer);

    {

      createAndAddReadImageFilter(pipeline, UnitTest::EMMPMSegmentationTest::TestFile);
      createAndAddConvertRGBToGrayscaleFilter(pipeline, DataArrayPath("ImageDataContainer", "CellData", "ImageData"), "Gray", "GrayScaleCellData");
      createAndAddEMMPMFilter(pipeline, DataArrayPath("ImageDataContainer", "GrayScaleCellData", "GrayImageData"), DataArrayPath("ImageDataContainer", "GrayScaleCellData", "Test"));

      pipeline->execute();
      DREAM3D_REQUIRE_EQUAL(pipeline->getErrorCondition(), NO_ERROR)
    }

    pipeline->clear();

    {
      createAndAddReadImageFilter(pipeline, UnitTest::EMMPMSegmentationTest::TestFile);
      // createAndAddConvertRGBToGrayscaleFilter(pipeline, DataArrayPath("DataContainer", "CellData", "ImageData"), "Gray", "GrayScaleCellData");
      createAndAddEMMPMFilter(pipeline, DataArrayPath("ImageDataContainer", "CellData", "ImageData"), DataArrayPath("ImageDataContainer", "CellData", "Test"));

      pipeline->execute();
      DREAM3D_REQUIRE_EQUAL(pipeline->getErrorCondition(), COMPONENTS_DONT_MATCH)
    }

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestMultiEMMPMSegmentation()
  {
    QImage inputImage(100, 100, QImage::Format_ARGB32);
    bool saveResult = inputImage.save(UnitTest::EMMPMSegmentationTest::TestFile);
    DREAM3D_REQUIRE_EQUAL(saveResult, true)

    FilterPipeline::Pointer pipeline = FilterPipeline::New();

    {
      createAndAddReadImageFilter(pipeline, UnitTest::EMMPMSegmentationTest::TestFile);
      createAndAddConvertRGBToGrayscaleFilter(pipeline, DataArrayPath("ImageDataContainer", "CellData", "ImageData"), "Gray", "GrayScaleCellData");

      QVector<DataArrayPath> inputPaths;
      inputPaths.push_back(DataArrayPath("ImageDataContainer", "GrayScaleCellData", "GrayImageData"));
      createAndAddMultiEMMPMFilter(pipeline, inputPaths);
      pipeline->execute();
      DREAM3D_REQUIRE_EQUAL(pipeline->getErrorCondition(), NO_ERROR)
    }

    pipeline->clear();

    {
      createAndAddReadImageFilter(pipeline, UnitTest::EMMPMSegmentationTest::TestFile);
      createAndAddConvertRGBToGrayscaleFilter(pipeline, DataArrayPath("ImageDataContainer", "CellData", "ImageData"), "Gray", "GrayScaleCellData");

      QVector<DataArrayPath> inputPaths;
      inputPaths.push_back(DataArrayPath("ImageDataContainer", "CellData", "ImageData")); // This is a 3 component RGB or 4 Comp RGBA array
      inputPaths.push_back(DataArrayPath("ImageDataContainer", "CellData", "ImageData"));
      createAndAddMultiEMMPMFilter(pipeline, inputPaths);
      pipeline->execute();
      DREAM3D_REQUIRE_EQUAL(pipeline->getErrorCondition(), COMPONENTS_DONT_MATCH)
    }

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {

    int err = EXIT_SUCCESS;
    DREAM3D_REGISTER_TEST(TestFilterAvailability());
    if(m_ImageProcessingPluginLoaded)
    {
      DREAM3D_REGISTER_TEST(TestEMMPMSegmentation())
      DREAM3D_REGISTER_TEST(TestMultiEMMPMSegmentation())
    }
    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

private:
  int m_ImageProcessingPluginLoaded = 0;
  QString m_ReadImageFilterName = QString("ItkReadImage");

  EMMPMSegmentationTest(const EMMPMSegmentationTest&); // Copy Constructor Not Implemented
  void operator=(const EMMPMSegmentationTest&);        // Operator '=' Not Implemented
};
