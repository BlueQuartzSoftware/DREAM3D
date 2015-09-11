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
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtGui/QImage>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/SIMPLibVersion.h"

#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"

#include "ImageIOTestFileLocations.h"


#define WRITE_IMAGES_FILTER_NAME "WriteImages"

QList<QString> fileNames;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  for (int i = 0; i < fileNames.size(); i++)
  {
    QFileInfo fi(fileNames.at(i));
    if (fi.exists())
    {
      QFile::remove(fileNames.at(i));
    }
  }
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestFilterAvailability()
{
  // Now instantiate the SaveImages Filter from the FilterManager
  QString filtName = WRITE_IMAGES_FILTER_NAME;
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
  if (NULL == filterFactory.get() )
  {
    std::stringstream ss;
    ss << "The WriteImagesTest Requires the use of the " << filtName.toStdString() << " filter which is found in the ImageIO Plugin";
    DREAM3D_TEST_THROW_EXCEPTION(ss.str())
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArray::Pointer initializeDataContainerArray(const QString& dcName, size_t comp )
{
  DataContainerArray::Pointer dca = DataContainerArray::New();
  DataContainer::Pointer vdc = DataContainer::New(dcName);
  //Set up geometry for tuples, a cuboid with dimensions 20, 10, 1
  ImageGeom::Pointer image = ImageGeom::CreateGeometry(DREAM3D::Geometry::ImageGeometry);
  vdc->setGeometry(image);
  size_t dims[3] = { 20, 10, 1 };
  image->setDimensions(dims);

  QVector<size_t> tDims(3, 0);
  tDims[0] = 20;
  tDims[1] = 10;
  tDims[2] = 1;
  QVector<size_t> cDims(1);
  cDims[0] = comp;

  AttributeMatrix::Pointer am = AttributeMatrix::New(tDims, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::AttributeMatrixType::Cell);
  DataArray<uint8_t>::Pointer data = DataArray<uint8_t>::CreateArray(tDims, cDims, "Uint8 Array");
  am->addAttributeArray(data->getName(), data);
  vdc->addAttributeMatrix(am->getName(), am);
  dca->addDataContainer(vdc);

  return dca;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SaveImagesTest(AbstractFilter::Pointer filter, DataContainerArray::Pointer dca, const QString& dcName)
{
  QVariant var;
  int err = 0, plane, fileExtension, numSlices = 1, w = 0, h = 0;
  bool propWasSet, filePrefix, b;
  QString fp, extension;
  QImage testImage;
  extension = QString();

  if (dcName == "dc0") // XY slices along Z, no file prefix, 1 slice, .tif, 20x10
  {
    plane = 0;
    fileExtension = 0;
    extension = ".tif";
    filePrefix = false;
    fp = "";
    numSlices = 1;
    w = 20;
    h = 10;
  }
  else if (dcName == "dc1") // XZ slices along Y, XZ_ file prefix, 10 slices, .bmp, 20x1
  {
    plane = 1;
    fileExtension = 1;
    extension = ".bmp";
    filePrefix = true;
    fp = "XZ_";
    numSlices = 10;
    w = 20;
    h = 1;
  }
  else if (dcName == "dc2") // YZ slices along X, YZ_ file prefix, 20 slices, .png, 10x1
  {
    plane = 2;
    fileExtension = 2;
    extension = ".png";
    filePrefix = true;
    fp = "YZ_";
    numSlices = 20;
    w = 10;
    h = 1;
  }

  filter->setDataContainerArray(dca);
  DataArrayPath path = DataArrayPath(dcName, DREAM3D::Defaults::CellAttributeMatrixName, "Uint8 Array");
  var.setValue(path);
  propWasSet = filter->setProperty("ColorsArrayPath", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

  var.setValue(plane); // plane
  propWasSet = filter->setProperty("Plane", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

  var.setValue(fileExtension); // file extension
  propWasSet = filter->setProperty("ImageFormat", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

  var.setValue(filePrefix); // file prefix checkbox
  propWasSet = filter->setProperty("FilePrefix", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

  QVariant qv(fp); // file prefix string
  propWasSet = filter->setProperty("ImagePrefix", qv);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

  var.setValue(UnitTest::TestTempDir); // output directory
  propWasSet = filter->setProperty("OutputPath", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

  filter->execute();
  if (dcName == "dc3") // 2 components, should have error -1006, number of components must be 1, 3, or 4
  {
    err = filter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, -1006);
    return;
  }
  else
  {
    DREAM3D_REQUIRED(filter->getErrorCondition(), >= , 0);
  }

  for (size_t i = 0; i < numSlices; i++)
  {
    b = testImage.load(UnitTest::TestTempDir + QDir::separator() + fp + QString::number(i) + extension);
    fileNames << UnitTest::TestTempDir + QDir::separator() + fp + QString::number(i) + extension;
    DREAM3D_REQUIRE_EQUAL(b, true);
  }

  int wRead = testImage.width();
  DREAM3D_REQUIRE_EQUAL(wRead, w);

  int hRead = testImage.height();
  DREAM3D_REQUIRE_EQUAL(hRead, h);

  int format = testImage.format();
  DREAM3D_REQUIRE_EQUAL(format, QImage::Format_RGB32);

  if (dcName == "dc2") // metadata only for .png
  {
    QString ver = testImage.text("Description");
    DREAM3D_REQUIRE_EQUAL(ver, SIMPLib::Version::PackageComplete());
  }

}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int WriteImages()
{
  DataContainerArray::Pointer dca;

  // Now instantiate the SaveImages Filter from the FilterManager
  QString filtName = WRITE_IMAGES_FILTER_NAME;
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
  if (NULL != filterFactory.get())
  {
    // If we get this far, the Factory is good so creating the filter should not fail unless something has
    // horribly gone wrong in which case the system is going to come down quickly after this.
    AbstractFilter::Pointer filter = filterFactory->create();

    dca = initializeDataContainerArray("dc0", 3);
    SaveImagesTest(filter, dca, "dc0"); // XY slices along Z, no file prefix, 1 slice, .tif, 20x10, 3 components

    dca = initializeDataContainerArray("dc1", 1);
    SaveImagesTest(filter, dca, "dc1"); // XZ slices along Y, XZ_ file prefix, 10 slices, .bmp, 20x1, 1 component

    dca = initializeDataContainerArray("dc2", 4);
    SaveImagesTest(filter, dca, "dc2"); // YZ slices along X, YZ_ file prefix, 20 slices, .png, 10x1, 4 components

    dca = initializeDataContainerArray("dc3", 2);
    SaveImagesTest(filter, dca, "dc3"); // 2 components, should have error -1006, number of components must be 1, 3, or 4
  }
  else
  {
    QString ss = QObject::tr("WriteImagesTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
    DREAM3D_REQUIRE_EQUAL(0, 1)
  }
  return 1;
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
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("WriteImagesTest");

  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( loadFilterPlugins() );
  DREAM3D_REGISTER_TEST( TestFilterAvailability() );

  DREAM3D_REGISTER_TEST(WriteImages())

  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
  PRINT_TEST_SUMMARY();
  return err;
}
