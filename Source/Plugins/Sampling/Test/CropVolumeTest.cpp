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

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/Common/FilterPipeline.h"
#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/FilterFactory.hpp"
#include "DREAM3DLib/Plugin/DREAM3DPluginInterface.h"
#include "DREAM3DLib/Plugin/DREAM3DPluginLoader.h"
#include "DREAM3DLib/Utilities/UnitTestSupport.hpp"
#include "DREAM3DLib/Utilities/QMetaObjectUtilities.h"

#include "GenerateFeatureIds.h"

class numPackage
{
  public:
    numPackage(int min, int max) {m_Min = min; m_Max = max;}

    DREAM3D_FILTER_PARAMETER(int, Min)
    Q_PROPERTY(int Min READ getMin WRITE setMin)

    DREAM3D_FILTER_PARAMETER(int, Max)
    Q_PROPERTY(int Max READ getMax WRITE setMax)

    numPackage() {}
};

static const numPackage originalX = numPackage(0, 40);
static const numPackage originalY = numPackage(0, 30);
static const numPackage originalZ = numPackage(0, 20);

static const numPackage croppedX = numPackage(9, 25);
static const numPackage croppedY = numPackage(5, 18);
static const numPackage croppedZ = numPackage(2, 7);

static float originalRes[3] = {0.25, 0.25, 0.25};
static float originalOrigin[3] = {0, 0, 0};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArray::Pointer CreateDataContainerArrayTestStructure()
{
  int err = 0;
  DataContainerArray::Pointer dca = DataContainerArray::New();

  //VolumeDataContainer::Pointer dc1 = VolumeDataContainer::New("Data Container");
  VolumeDataContainer::Pointer dc1 = VolumeDataContainer::New(DREAM3D::Defaults::VolumeDataContainerName);
  dc1->setDimensions(originalX.getMax(), originalY.getMax(), originalZ.getMax());
  dc1->setOrigin(originalOrigin);
  dc1->setResolution(originalRes);

  QVector<size_t> amDims;
  amDims.push_back(originalX.getMax());
  amDims.push_back(originalY.getMax());
  amDims.push_back(originalZ.getMax());
  AttributeMatrix::Pointer am1 = AttributeMatrix::New(amDims, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::AttributeMatrixType::Cell);

  QVector<size_t> tDims;
  tDims.push_back(originalX.getMax());
  tDims.push_back(originalY.getMax());
  tDims.push_back(originalZ.getMax());

  QVector<size_t> cDims(1, 1);

  DataArray<int32_t>::Pointer ConfidenceIndexArray = DataArray<int32_t>::CreateArray(tDims, cDims, "Confidence Index", true);
  err = am1->addAttributeArray("Confidence Index", ConfidenceIndexArray);
  DREAM3D_REQUIRE(err >= 0);

  DataArray<int32_t>::Pointer FeatureIdsArray = DataArray<int32_t>::CreateArray(tDims, cDims, "FeatureIds", true);
  err = am1->addAttributeArray("FeatureIds", FeatureIdsArray);
  DREAM3D_REQUIRE(err >= 0);

  int64_t XP = tDims[0];
  int64_t YP = tDims[1];
  int64_t ZP = tDims[2];

  int64_t col, row, plane;
  int64_t index;

  QList<QString> voxelArrayNames = am1->getAttributeArrayNames();
  int64_t val = 1;

  for (int64_t i = 0; i < ZP; i++)
  {
    plane = i * XP * YP;
    for (int64_t j = 0; j < YP; j++)
    {
      row = j * XP;
      for (int64_t k = 0; k < XP; k++)
      {
        col = k;
        index = plane + row + col;
        for (QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
        {
          IDataArray::Pointer p = am1->getAttributeArray(*iter);
          p->initializeTuple(index, val);
          val++;
        }
      }
    }
  }

  foreach (QString da, am1->getAttributeArrayNames())
  {
    DREAM3D_REQUIRE_EQUAL(am1->getAttributeArray(da)->getSize(), XP*YP*ZP);
  }

  dc1->addAttributeMatrix(am1->getName(), am1);

  dca->addDataContainer(dc1);

  return dca;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void resetTest(AbstractFilter::Pointer cropVolume)
{
  cropVolume->setErrorCondition(0);
  DataContainerArray::Pointer dca = CreateDataContainerArrayTestStructure();
  cropVolume->setDataContainerArray(dca);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void checkCrop(typename DataArray<T>::Pointer ptr, numPackage X, numPackage Y, numPackage Z, int numComponents)
{
  int originalVal = X.getMin();

  DataArray<T> &p = *(ptr.get());
  int Xloop = (X.getMax() - X.getMin()) + 1;
  int Yloop = (Y.getMax() - Y.getMin()) + 1;
  int Zloop = (Z.getMax() - Z.getMin()) + 1;

  DREAM3D_REQUIRE_EQUAL(ptr->getSize(), Xloop*Yloop*Zloop)

  for (int64_t i = 0; i < Zloop; i++)
  {
    int64_t plane = (i * Xloop * Yloop);
    for (int64_t j = 0; j < Yloop; j++)
    {
      int64_t row = (j * Xloop);
      for (int64_t k = 0; k < Xloop; k++)
      {
        int64_t col = k;
        int64_t index = plane + row + col;
        for (int64_t m = 0; m < numComponents; m++)
        {
          int64_t value = p[index];
          DREAM3D_REQUIRE_EQUAL( p[index], originalVal )
          originalVal++;
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void preflightTests(AbstractFilter::Pointer cropVolume)
{
  // Test Source Data Container Does Not Exist
  DataArrayPath path("This Should Not Exist", "This Should Not Exist", "");
  QVariant var;
  var.setValue(path);
  bool propWasSet = cropVolume->setProperty("CellAttributeMatrixPath", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5548)
      resetTest(cropVolume);

  // Test Source Attribute Matrix Does Not Exist
  path.setDataContainerName(DREAM3D::Defaults::VolumeDataContainerName);
  var.setValue(path);
  propWasSet = cropVolume->setProperty("CellAttributeMatrixPath", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5549)
      resetTest(cropVolume);

  path.setAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName);
  var.setValue(path);
  propWasSet = cropVolume->setProperty("CellAttributeMatrixPath", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      // Testing New Data Container
      var.setValue(true);
  propWasSet = cropVolume->setProperty("SaveAsNewDataContainer", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      // Test Cropping Bounds
      var.setValue(croppedX.getMax()+1);
  propWasSet = cropVolume->setProperty("XMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5550)
      resetTest(cropVolume);
  var.setValue(croppedX.getMin());
  propWasSet = cropVolume->setProperty("XMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(croppedY.getMax()+1);
  propWasSet = cropVolume->setProperty("YMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5551)
      resetTest(cropVolume);
  var.setValue(croppedY.getMin());
  propWasSet = cropVolume->setProperty("YMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(croppedZ.getMax()+1);
  propWasSet = cropVolume->setProperty("ZMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5552)
      resetTest(cropVolume);
  var.setValue(croppedZ.getMin());
  propWasSet = cropVolume->setProperty("ZMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(-1);
  propWasSet = cropVolume->setProperty("XMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5553)
      resetTest(cropVolume);
  var.setValue(croppedX.getMin());
  propWasSet = cropVolume->setProperty("XMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(-1);
  propWasSet = cropVolume->setProperty("YMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5554)
      resetTest(cropVolume);
  var.setValue(croppedY.getMin());
  propWasSet = cropVolume->setProperty("YMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(-1);
  propWasSet = cropVolume->setProperty("ZMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5555)
      resetTest(cropVolume);
  var.setValue(croppedZ.getMin());
  propWasSet = cropVolume->setProperty("ZMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(originalX.getMax()+1);
  propWasSet = cropVolume->setProperty("XMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5556)
      resetTest(cropVolume);
  var.setValue(croppedX.getMax());
  propWasSet = cropVolume->setProperty("XMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(originalY.getMax()+1);
  propWasSet = cropVolume->setProperty("YMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5557)
      resetTest(cropVolume);
  var.setValue(croppedY.getMax());
  propWasSet = cropVolume->setProperty("YMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(originalZ.getMax()+1);
  propWasSet = cropVolume->setProperty("ZMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5558)
      resetTest(cropVolume);
  var.setValue(croppedZ.getMax());
  propWasSet = cropVolume->setProperty("ZMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

//      var.setValue(true);
//  propWasSet = cropVolume->setProperty("RenumberFeatures", var);
//  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
//      cropVolume->preflight();
//  DREAM3D_REQUIRED(cropVolume->getErrorCondition(), >=, 0)
//      resetTest(cropVolume);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void executeTests(AbstractFilter::Pointer cropVolume)
{
  QVariant var(originalX.getMax()+1);
  bool propWasSet = cropVolume->setProperty("XMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      cropVolume->execute();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -950)
      resetTest(cropVolume);
  var.setValue(croppedX.getMax());
  propWasSet = cropVolume->setProperty("XMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(originalY.getMax()+1);
  propWasSet = cropVolume->setProperty("YMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      cropVolume->execute();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -951)
      resetTest(cropVolume);
  var.setValue(croppedY.getMax());
  propWasSet = cropVolume->setProperty("YMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(originalZ.getMax()+1);
  propWasSet = cropVolume->setProperty("ZMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      cropVolume->execute();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -952)
      resetTest(cropVolume);
  var.setValue(croppedZ.getMax());
  propWasSet = cropVolume->setProperty("ZMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      resetTest(cropVolume);

  var.setValue(false);
  propWasSet = cropVolume->setProperty("SaveAsNewDataContainer", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

  // Test that the crop actually works
      cropVolume->execute();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), 0)

      DataArrayPath attrMatPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, "");
      IDataArray::Pointer iDataArray = cropVolume->getDataContainerArray()->getAttributeMatrix(attrMatPath)->getAttributeArray("Confidence Index");
      DataArray<int32_t>::Pointer array = boost::dynamic_pointer_cast< DataArray<int32_t> >(iDataArray);

      checkCrop<int32_t>(array, croppedX, croppedY, croppedZ, 1);


//  float origin[3] = cropVolume->getDataContainerArray()->getPrereqDataContainer(cropVolume, DREAM3D::Defaults::VolumeDataContainerName

//  DREAM3D_REQUIRE_EQUAL()

  resetTest(cropVolume);

  // Test if the crop works when we are saving it in a new data container
  var.setValue(true);
  propWasSet = cropVolume->setProperty("SaveAsNewDataContainer", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      cropVolume->execute();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), 0)

  DataArrayPath newAttrMatPath(DREAM3D::Defaults::NewVolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, "");
  iDataArray = cropVolume->getDataContainerArray()->getAttributeMatrix(newAttrMatPath)->getAttributeArray("Confidence Index");
  array = boost::dynamic_pointer_cast< DataArray<int32_t> >(iDataArray);

  checkCrop<int32_t>(array, croppedX, croppedY, croppedZ, 1);

  resetTest(cropVolume);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestCropVolume()
{
  int err = 0;

  // Now instantiate the CropVolume Filter from the FilterManager
  QString filtName = "CropVolume";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
  if (NULL != filterFactory.get() )
  {
    // If we get this far, the Factory is good so creating the filter should not fail unless something has
    // horribly gone wrong in which case the system is going to come down quickly after this.
    DataContainerArray::Pointer dca = CreateDataContainerArrayTestStructure();

    AbstractFilter::Pointer cropVolume = filterFactory->create();
    cropVolume->setDataContainerArray(dca);

    QVariant var;
    var.setValue(croppedX.getMax());
    bool propWasSet = cropVolume->setProperty("XMax", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

        var.setValue(croppedY.getMax());
    propWasSet = cropVolume->setProperty("YMax", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

        var.setValue(croppedZ.getMax());
    propWasSet = cropVolume->setProperty("ZMax", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

        var.setValue(croppedX.getMin());
    propWasSet = cropVolume->setProperty("XMin", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

        var.setValue(croppedY.getMin());
    propWasSet = cropVolume->setProperty("YMin", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

        var.setValue(croppedZ.getMin());
    propWasSet = cropVolume->setProperty("ZMin", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

        //    var.setValue(true);
        //    propWasSet = cropVolume->setProperty("SaveAsNewDataContainer", var);
        //    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    preflightTests(cropVolume);

    executeTests(cropVolume);

    return EXIT_SUCCESS;

  }
  else
  {
    QString ss = QObject::tr("CropVolumeTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
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
  QCoreApplication::setApplicationName("CropVolumeTest");

  int err = EXIT_SUCCESS;

  DREAM3D_REGISTER_TEST( loadFilterPlugins() );

  DREAM3D_REGISTER_TEST( TestCropVolume() )

      PRINT_TEST_SUMMARY();
  return err;
}





