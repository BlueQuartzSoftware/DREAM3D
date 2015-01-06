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
#include "DREAM3DLib/CoreFilters/DataContainerWriter.h"
#include "DREAM3DLib/Plugin/DREAM3DPluginInterface.h"
#include "DREAM3DLib/Plugin/DREAM3DPluginLoader.h"
#include "DREAM3DLib/Utilities/UnitTestSupport.hpp"
#include "DREAM3DLib/Utilities/QMetaObjectUtilities.h"

// #include "GenerateFeatureIds.h"

static const QString k_DataArrayName("Data");
static const QString k_4CompDataArrayName("Data 4 Comp");
static const QString k_FeatureIdsName("Feature Ids");
static const QString k_ActivesName("Actives");
static const QString k_DataContainerName = DREAM3D::Defaults::VolumeDataContainerName;
static const QString k_NewDataContainerName = "CroppedDataContainer";
static const QString k_CellAttributeMatrixName = DREAM3D::Defaults::CellAttributeMatrixName;
static const QString k_FeatureAttributeMatrixName = DREAM3D::Defaults::CellFeatureAttributeMatrixName;


static Observer obs;

class NumPackage
{
  public:
    NumPackage(int min, int max) { m_Min = min; m_Max = max; m_Diff = m_Max - m_Min; }

    void setMin(int min) { m_Min = min; m_Diff = m_Max - m_Min; }
    int getMin() { return m_Min; }
    Q_PROPERTY(int Min READ getMin WRITE setMin)

    void setMax(int max) { m_Max = max; m_Diff = m_Max - m_Min; }
    int getMax() { return m_Max; }
    Q_PROPERTY(int Max READ getMax WRITE setMax)

    int getDiff() { return m_Diff; }

  private:
    int m_Diff;
    int m_Max;
    int m_Min;
};

static NumPackage s_OriginalX = NumPackage(0, 5);
static NumPackage s_OriginalY = NumPackage(0, 10);
static NumPackage s_OriginalZ = NumPackage(0, 2);

static NumPackage s_CroppedX = NumPackage(0, 3);
static NumPackage s_CroppedY = NumPackage(1, 2);
static NumPackage s_CroppedZ = NumPackage(0, 1);

static float originalRes[3] = { 0.25, 0.25, 0.25 };
static float originalOrigin[3] = { 0, 0, 0 };


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArray::Pointer CreateDataContainerArrayTestStructure(NumPackage X, NumPackage Y, NumPackage Z, int numComponents)
{
  int err = 0;
  DataContainerArray::Pointer dca = DataContainerArray::New();

  VolumeDataContainer::Pointer dc1 = VolumeDataContainer::New(DREAM3D::Defaults::VolumeDataContainerName);
  dc1->setDimensions(X.getMax(), Y.getMax(), Z.getMax());
  dc1->setOrigin(originalOrigin);
  dc1->setResolution(originalRes);

  QVector<size_t> amDims;
  amDims.push_back(X.getMax());
  amDims.push_back(Y.getMax());
  amDims.push_back(Z.getMax());
  AttributeMatrix::Pointer am1 = AttributeMatrix::New(amDims, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::AttributeMatrixType::Cell);
  AttributeMatrix::Pointer am2 = AttributeMatrix::New(amDims, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::AttributeMatrixType::CellFeature);

  QVector<size_t> tDims;
  tDims.push_back(X.getMax());
  tDims.push_back(Y.getMax());
  tDims.push_back(Z.getMax());

  QVector<size_t> cDims(1, numComponents);

  Int32ArrayType::Pointer genericData = Int32ArrayType::CreateArray(tDims, cDims, k_DataArrayName, true);
  cDims[0] = 4;
  UInt8ArrayType::Pointer fourCompData = UInt8ArrayType::CreateArray(tDims, cDims, k_4CompDataArrayName, true);
  cDims[0] = 1;
  Int32ArrayType::Pointer featureIds = Int32ArrayType::CreateArray(tDims, cDims, k_FeatureIdsName, true);
  DataArray<bool>::Pointer actives = DataArray<bool>::CreateArray(tDims, cDims, k_ActivesName, true);

  int64_t index = 0;
  int64_t id = 1;

  struct { uint8_t r; uint8_t g; uint8_t b; uint8_t a;} rgba;
  int32_t* ptr = reinterpret_cast<int32_t*>(&rgba);
  uint8_t* u8ptr = reinterpret_cast<uint8_t*>(&rgba);


  u8ptr[3] = 0;
  for (int64_t z = 0; z < tDims[2]; z++)
  {
    u8ptr[2] = (uint8_t)(z);
    for (int64_t y = 0; y < tDims[1]; y++)
    {
      u8ptr[1] = (uint8_t)(y);
      for (int64_t x = 0; x < tDims[0]; x++)
      {
        index = (z*tDims[0] * tDims[1]) + (y*tDims[0]) + x;

        u8ptr[0] = (uint8_t)(x);
        genericData->setComponent(index, 0, *ptr);

        fourCompData->setComponent(index, 0, x);
        fourCompData->setComponent(index, 1, y);
        fourCompData->setComponent(index, 2, z);
        fourCompData->setComponent(index, 3, 0);
      }
      // These are 1 component arrays
      featureIds->setValue(index, index);
      actives->setValue(index, true);

      id++;
    }
  }


  err = am1->addAttributeArray(k_DataArrayName, genericData);
  DREAM3D_REQUIRE(err >= 0);
  err = am1->addAttributeArray(k_4CompDataArrayName, fourCompData);
  DREAM3D_REQUIRE(err >= 0);
  err = am1->addAttributeArray(k_FeatureIdsName, featureIds);
  DREAM3D_REQUIRE(err >= 0);
  err = am2->addAttributeArray(k_ActivesName, actives);
  DREAM3D_REQUIRE(err >= 0);

  foreach(QString da, am1->getAttributeArrayNames())
  {
    DREAM3D_REQUIRE_EQUAL(am1->getAttributeArray(da)->getNumberOfTuples(), tDims[0] * tDims[1] * tDims[2]);
  }

  dc1->addAttributeMatrix(am1->getName(), am1);
  dc1->addAttributeMatrix(am2->getName(), am2);

  dca->addDataContainer(dc1);

  return dca;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void resetTest(AbstractFilter::Pointer cropVolume, NumPackage X, NumPackage Y, NumPackage Z, int numComponents)
{
  cropVolume->setErrorCondition(0);
  DataContainerArray::Pointer dca = CreateDataContainerArrayTestStructure(X, Y, Z, numComponents);
  cropVolume->setDataContainerArray(dca);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T, typename K>
void checkCrop(typename DataArray<T>::Pointer data, NumPackage& X, NumPackage& Y, NumPackage& Z)
{
  DataArray<T> &p = *(data.get());
  // We need to add the +1 here because the Crop Volume filter is "inclusive" of the ranges
  // that the user passes to it. This means that for an x range of 0->4 there are actually
  // 5 elements. If the range is 1->5 then there are 5 elements.
  int XP = (X.getMax() - X.getMin()) + 1;
  int YP = (Y.getMax() - Y.getMin()) + 1;
  int ZP = (Z.getMax() - Z.getMin()) + 1;
  int numComponents = data->getNumberOfComponents();

  require_equal<size_t, size_t>(p.getSize(), "p.getSize()", XP*YP*ZP*numComponents, "XP*YP*ZP*numComponents", __FILE__, __LINE__);

  struct { uint8_t r; uint8_t g; uint8_t b; uint8_t a;} rgba;
  K* ptr = reinterpret_cast<K*>(&rgba);
  uint8_t* u8ptr = reinterpret_cast<uint8_t*>(&rgba);
  u8ptr[3] = 0;

  for (int64_t z = Z.getMin(); z < Z.getMax(); z++)
  {
    u8ptr[2] = (uint8_t)(z);
    for (int64_t y = Y.getMin(); y < Y.getMax(); y++)
    {
      u8ptr[1] = (uint8_t)(y);
      for (int64_t x = X.getMin(); x < X.getMax(); x++)
      {
        u8ptr[0] = x; // This completes building up our piece of data

        int64_t index = ((z - Z.getMin())*XP*YP) + ((y - Y.getMin()) * XP) + (x - X.getMin());
        for(int c = 0; c < numComponents; c++)
        {
          T value = p.getComponent(index, c);
          require_equal<T, T>(value, "Stored", static_cast<T>(ptr[c]), "Calculated", __FILE__, __LINE__);
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
  QVariant var;
  var.setValue(false);
  bool propWasSet = cropVolume->setProperty("RenumberFeatures", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);

  // Test Source Data Container Does Not Exist
  DataArrayPath path("This Should Not Exist", "This Should Not Exist", "");
  var.setValue(path);
  propWasSet = cropVolume->setProperty("CellAttributeMatrixPath", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);
  cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5548)
  resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 3);

  // Test Source Attribute Matrix Does Not Exist
  path.setDataContainerName(DREAM3D::Defaults::VolumeDataContainerName);
  var.setValue(path);
  propWasSet = cropVolume->setProperty("CellAttributeMatrixPath", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);
  cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5549)
  resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 3);

  path.setAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName);
  var.setValue(path);
  propWasSet = cropVolume->setProperty("CellAttributeMatrixPath", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);

  // Testing New Data Container
  var.setValue(true);
  propWasSet = cropVolume->setProperty("SaveAsNewDataContainer", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);

  // Test Cropping Bounds
  var.setValue(s_CroppedX.getMax() + 1);
  propWasSet = cropVolume->setProperty("XMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);
  cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5550)
  resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 3);
  var.setValue(s_CroppedX.getMin());
  propWasSet = cropVolume->setProperty("XMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);

  var.setValue(s_CroppedY.getMax() + 1);
  propWasSet = cropVolume->setProperty("YMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);
  cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5551)
  resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 3);
  var.setValue(s_CroppedY.getMin());
  propWasSet = cropVolume->setProperty("YMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);

  var.setValue(s_CroppedZ.getMax() + 1);
  propWasSet = cropVolume->setProperty("ZMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);
  cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5552)
  resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 3);
  var.setValue(s_CroppedZ.getMin());
  propWasSet = cropVolume->setProperty("ZMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);

  var.setValue(-1);
  propWasSet = cropVolume->setProperty("XMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);
  cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5553)
  resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 3);
  var.setValue(s_CroppedX.getMin());
  propWasSet = cropVolume->setProperty("XMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);

  var.setValue(-1);
  propWasSet = cropVolume->setProperty("YMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);
  cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5554)
  resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 3);
  var.setValue(s_CroppedY.getMin());
  propWasSet = cropVolume->setProperty("YMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);

  var.setValue(-1);
  propWasSet = cropVolume->setProperty("ZMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);
  cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5555)
  resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 3);
  var.setValue(s_CroppedZ.getMin());
  propWasSet = cropVolume->setProperty("ZMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);

  var.setValue(s_OriginalX.getMax() + 1);
  propWasSet = cropVolume->setProperty("XMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);
  cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5556)
  resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 3);
  var.setValue(s_CroppedX.getMax());
  propWasSet = cropVolume->setProperty("XMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);

  var.setValue(s_OriginalY.getMax() + 1);
  propWasSet = cropVolume->setProperty("YMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);
  cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5557)
  resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 3);
  var.setValue(s_CroppedY.getMax());
  propWasSet = cropVolume->setProperty("YMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);

  var.setValue(s_OriginalZ.getMax() + 1);
  propWasSet = cropVolume->setProperty("ZMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);
  cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5558)
  resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 3);
  var.setValue(s_CroppedZ.getMax());
  propWasSet = cropVolume->setProperty("ZMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void executeTests(AbstractFilter::Pointer cropVolume)
{
  DataContainerWriter::Pointer writer = DataContainerWriter::New();
  writer->setDataContainerArray(cropVolume->getDataContainerArray());
  writer->setOutputFile("CropVolumeTest.dream3d");
  writer->setWriteXdmfFile(true);

  QVariant var(s_OriginalX.getMax() + 1);
  bool propWasSet = cropVolume->setProperty("XMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);
  cropVolume->execute();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -950)
      resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 3);
  var.setValue(s_CroppedX.getMax());
  propWasSet = cropVolume->setProperty("XMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);

  var.setValue(s_OriginalY.getMax() + 1);
  propWasSet = cropVolume->setProperty("YMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);
  cropVolume->execute();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -951)
      resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 3);
  var.setValue(s_CroppedY.getMax());
  propWasSet = cropVolume->setProperty("YMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);

  var.setValue(s_OriginalZ.getMax() + 1);
  propWasSet = cropVolume->setProperty("ZMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);
  cropVolume->execute();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -952)
      resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 3);
  var.setValue(s_CroppedZ.getMax());
  propWasSet = cropVolume->setProperty("ZMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);

  resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 3);

  var.setValue(false);
  propWasSet = cropVolume->setProperty("SaveAsNewDataContainer", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);


  // Test that the crop actually works across the entire original set
  NumPackage testX(s_CroppedX), testY(s_CroppedY), testZ(s_CroppedZ);
#if 0
  while (testX.getMin() < (originalX.getMax() - testX.getDiff()))
  {
    var.setValue(testX.getMin());
    propWasSet = cropVolume->setProperty("XMin", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true);

    var.setValue(testX.getMax());
    propWasSet = cropVolume->setProperty("XMax", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true);

    testY.setMin(croppedY.getMin());
    testY.setMax(croppedY.getMax());
    while (testY.getMin() < (originalY.getMax() - testY.getDiff()))
    {
      var.setValue(testY.getMin());
      propWasSet = cropVolume->setProperty("YMin", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      var.setValue(testY.getMax());
      propWasSet = cropVolume->setProperty("YMax", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      testZ.setMin(croppedZ.getMin());
      testZ.setMax(croppedZ.getMax());
      while (testZ.getMin() < (originalZ.getMax() - testZ.getDiff()))
      {
        var.setValue(testZ.getMin());
        propWasSet = cropVolume->setProperty("ZMin", var);
        DREAM3D_REQUIRE_EQUAL(propWasSet, true);

        var.setValue(testZ.getMax());
        propWasSet = cropVolume->setProperty("ZMax", var);
        DREAM3D_REQUIRE_EQUAL(propWasSet, true);

        cropVolume->execute();
        DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), 0)

            writer->execute();

        DataArrayPath attrMatPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, "");
        IDataArray::Pointer iDataArray = cropVolume->getDataContainerArray()->getAttributeMatrix(attrMatPath)->getAttributeArray(k_DataArrayName);
        Int32ArrayType::Pointer array = boost::dynamic_pointer_cast<Int32ArrayType>(iDataArray);

        checkCrop<int32_t>(array, testX, testY, testZ, array->getNumberOfComponents());

        resetTest(cropVolume, originalX, originalY, originalZ, 3);

        testZ.setMin(testZ.getMin() + 1);
        testZ.setMax(testZ.getMax() + 1);
      }
      testY.setMin(testY.getMin() + 1);
      testY.setMax(testY.getMax() + 1);
    }

    testX.setMin(testX.getMin() + 1);
    testX.setMax(testX.getMax() + 1);
  }
#endif


#if 1
  // Test that the crop actually works across the entire original set, when we are saving it in a new data container
  testX = s_CroppedX;
  testY = s_CroppedY;
  testZ = s_CroppedZ;

  var.setValue(true);
  propWasSet = cropVolume->setProperty("SaveAsNewDataContainer", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);

  //	var.setValue(QString("CroppedVolume"));
  //	propWasSet = cropVolume->setProperty("NewDataContainerName", var);
  //	DREAM3D_REQUIRE_EQUAL(propWasSet, true);

  while (testX.getMin() < (s_OriginalX.getMax() - testX.getDiff()))
  {
    var.setValue(testX.getMin());
    propWasSet = cropVolume->setProperty("XMin", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true);

    var.setValue(testX.getMax());
    propWasSet = cropVolume->setProperty("XMax", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true);

    testY.setMin(s_CroppedY.getMin());
    testY.setMax(s_CroppedY.getMax());
    while (testY.getMin() < (s_OriginalY.getMax() - testY.getDiff()))
    {
      var.setValue(testY.getMin());
      propWasSet = cropVolume->setProperty("YMin", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      var.setValue(testY.getMax());
      propWasSet = cropVolume->setProperty("YMax", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      testZ.setMin(s_CroppedZ.getMin());
      testZ.setMax(s_CroppedZ.getMax());
      while (testZ.getMin() < (s_OriginalZ.getMax() - testZ.getDiff()))
      {
        var.setValue(testZ.getMin());
        propWasSet = cropVolume->setProperty("ZMin", var);
        DREAM3D_REQUIRE_EQUAL(propWasSet, true);

        var.setValue(testZ.getMax());
        propWasSet = cropVolume->setProperty("ZMax", var);
        DREAM3D_REQUIRE_EQUAL(propWasSet, true);

        cropVolume->preflight();
        DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), 0)

            // We need to remove the new data container as it was added by the
            // preflight and it will cause an error. Typically when the pipeline
            // is run from the GUI this is taken care of for us.
            DataContainerArray::Pointer dca = cropVolume->getDataContainerArray();
        dca->removeDataContainer(DREAM3D::Defaults::NewVolumeDataContainerName);
        int numContainers = dca->getNumDataContainers();
        DREAM3D_REQUIRE_EQUAL(1, numContainers);


        cropVolume->execute();
        DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), 0)

            writer->execute();

        DataArrayPath newAttrMatPath(DREAM3D::Defaults::NewVolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, "");
        IDataArray::Pointer iDataArray = cropVolume->getDataContainerArray()->getAttributeMatrix(newAttrMatPath)->getAttributeArray(k_DataArrayName);
        Int32ArrayType::Pointer array = boost::dynamic_pointer_cast<Int32ArrayType >(iDataArray);

        checkCrop<int32_t, int32_t>(array, testX, testY, testZ);

        resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 3);

        testZ.setMin(testZ.getMin() + 1);
        testZ.setMax(testZ.getMax() + 1);



      }
      testY.setMin(testY.getMin() + 1);
      testY.setMax(testY.getMax() + 1);
    }

    testX.setMin(testX.getMin() + 1);
    testX.setMax(testX.getMax() + 1);
  }
#endif

  resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 1);

  // Reset min and max values back to chosen cropped values
  var.setValue(s_CroppedX.getMin());
  propWasSet = cropVolume->setProperty("XMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);

  var.setValue(s_CroppedX.getMax());
  propWasSet = cropVolume->setProperty("XMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);

  var.setValue(s_CroppedY.getMin());
  propWasSet = cropVolume->setProperty("YMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);

  var.setValue(s_CroppedY.getMax());
  propWasSet = cropVolume->setProperty("YMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);

  var.setValue(s_CroppedZ.getMin());
  propWasSet = cropVolume->setProperty("ZMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);

  var.setValue(s_CroppedZ.getMax());
  propWasSet = cropVolume->setProperty("ZMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);

  resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 1);



  var.setValue(false);
  propWasSet = cropVolume->setProperty("SaveAsNewDataContainer", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);

  // Test Renumber Features
  var.setValue(true);
  propWasSet = cropVolume->setProperty("RenumberFeatures", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true);

  cropVolume->execute();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), 0)

  {
    DataArrayPath newAttrMatPath(k_DataContainerName, k_CellAttributeMatrixName, "");
    IDataArray::Pointer iDataArray = cropVolume->getDataContainerArray()->getAttributeMatrix(newAttrMatPath)->getAttributeArray(k_FeatureIdsName);
    Int32ArrayType::Pointer array = boost::dynamic_pointer_cast<Int32ArrayType>(iDataArray);

    int64_t xMin = s_CroppedX.getMin(), yMin = s_CroppedY.getMin(), zMin = s_CroppedZ.getMin();
    int64_t index = 0, id = 1;
    int XP = s_CroppedX.getDiff() + 1, YP = s_CroppedY.getDiff() + 1, ZP = s_CroppedZ.getDiff() + 1;

    DREAM3D_REQUIRE_EQUAL(array->getSize(), XP*YP*ZP)

        for (int64_t z = zMin; z < ZP; z++)
    {
      for (int64_t y = yMin; y < YP; y++)
      {
        for (int64_t x = xMin; x < XP; x++)
        {
          int64_t index = ((z - zMin)*XP*YP) + ((y - yMin) * XP) + (x - xMin);

          int32_t val = array->getComponent(index, 0);

          DREAM3D_REQUIRE_EQUAL(val, id)
              id++;
        }
      }
    }
  }

  // Test if FeatureIds array has zero elements
  {
    NumPackage zero(0, 0);
    resetTest(cropVolume, zero, zero, zero, 1);
    cropVolume->execute();

    DataArrayPath newAttrMatPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, "");
    IDataArray::Pointer iDataArray = cropVolume->getDataContainerArray()->getAttributeMatrix(newAttrMatPath)->getAttributeArray(k_FeatureIdsName);
    Int32ArrayType::Pointer array = boost::dynamic_pointer_cast<Int32ArrayType>(iDataArray);

    DREAM3D_REQUIRE_NE(cropVolume->getErrorCondition(), 0)
  }

  // Test if FeatureIds array contains an id that is larger than the total number of features
  {
    resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 1);
    DataArrayPath newAttrMatPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, "");
    IDataArray::Pointer iDataArray = cropVolume->getDataContainerArray()->getAttributeMatrix(newAttrMatPath)->getAttributeArray(k_FeatureIdsName);
    Int32ArrayType::Pointer array = boost::dynamic_pointer_cast<Int32ArrayType>(iDataArray);
    int64_t numTuples = s_OriginalX.getMax()*s_OriginalY.getMax()*s_OriginalZ.getMax();
    array->setValue(0, numTuples + 1);
    cropVolume->execute();

    iDataArray = cropVolume->getDataContainerArray()->getAttributeMatrix(newAttrMatPath)->getAttributeArray(k_FeatureIdsName);
    array = boost::dynamic_pointer_cast<Int32ArrayType>(iDataArray);

    DREAM3D_REQUIRE_NE(cropVolume->getErrorCondition(), 0)
  }

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestCropVolume()
{
  int err = 0;
  Observer obs;

  // Now instantiate the CropVolume Filter from the FilterManager
  QString filtName = "CropVolume";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
  if (NULL != filterFactory.get())
  {
    // If we get this far, the Factory is good so creating the filter should not fail unless something has
    // horribly gone wrong in which case the system is going to come down quickly after this.
    DataContainerArray::Pointer dca = CreateDataContainerArrayTestStructure(s_OriginalX, s_OriginalY, s_OriginalZ, 3);

    AbstractFilter::Pointer cropVolume = filterFactory->create();
    cropVolume->setDataContainerArray(dca);
    cropVolume->connect(cropVolume.get(), SIGNAL(filterGeneratedMessage(const PipelineMessage&)),
                        &obs, SLOT(processPipelineMessage(const PipelineMessage&)));

    QVariant var;
    var.setValue(s_CroppedX.getMax());
    bool propWasSet = cropVolume->setProperty("XMax", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true);

    var.setValue(s_CroppedY.getMax());
    propWasSet = cropVolume->setProperty("YMax", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true);

    var.setValue(s_CroppedZ.getMax());
    propWasSet = cropVolume->setProperty("ZMax", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true);

    var.setValue(s_CroppedX.getMin());
    propWasSet = cropVolume->setProperty("XMin", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true);

    var.setValue(s_CroppedY.getMin());
    propWasSet = cropVolume->setProperty("YMin", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true);

    var.setValue(s_CroppedZ.getMin());
    propWasSet = cropVolume->setProperty("ZMin", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true);

    //    var.setValue(true);
    //    propWasSet = cropVolume->setProperty("SaveAsNewDataContainer", var);
    //    DREAM3D_REQUIRE_EQUAL(propWasSet, true);

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
AbstractFilter::Pointer CreateCropVolumeFilter(NumPackage x, NumPackage y, NumPackage z,
                                               bool renumberGrains, bool createNewDC)
{
  AbstractFilter::Pointer cropVolume = AbstractFilter::NullPointer();

  QString filtName = "CropVolume";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
  if (NULL != filterFactory.get())
  {
    // If we get this far, the Factory is good so creating the filter should not fail unless something has
    // horribly gone wrong in which case the system is going to come down quickly after this.
    DataContainerArray::Pointer dca = CreateDataContainerArrayTestStructure(s_OriginalX, s_OriginalY, s_OriginalZ, 1);

    cropVolume = filterFactory->create();
    cropVolume->setDataContainerArray(dca);
    cropVolume->connect(cropVolume.get(), SIGNAL(filterGeneratedMessage(const PipelineMessage&)),
                        &obs, SLOT(processPipelineMessage(const PipelineMessage&)));

    QVariant var;
    var.setValue(x.getMax());
    bool propWasSet = cropVolume->setProperty("XMax", var);
    require_equal<bool, bool>(propWasSet, "propWasSet", true, "true");

    var.setValue(y.getMax());
    propWasSet = cropVolume->setProperty("YMax", var);
    require_equal<bool, bool>(propWasSet, "propWasSet", true, "true");

    var.setValue(z.getMax());
    propWasSet = cropVolume->setProperty("ZMax", var);
    require_equal<bool, bool>(propWasSet, "propWasSet", true, "true");

    var.setValue(x.getMin());
    propWasSet = cropVolume->setProperty("XMin", var);
    require_equal<bool, bool>(propWasSet, "propWasSet", true, "true");

    var.setValue(y.getMin());
    propWasSet = cropVolume->setProperty("YMin", var);
    require_equal<bool, bool>(propWasSet, "propWasSet", true, "true");

    var.setValue(z.getMin());
    propWasSet = cropVolume->setProperty("ZMin", var);
    require_equal<bool, bool>(propWasSet, "propWasSet", true, "true");

    var.setValue(createNewDC);
    propWasSet = cropVolume->setProperty("SaveAsNewDataContainer", var);
    require_equal<bool, bool>(propWasSet, "propWasSet", true, "true");

    if (createNewDC)
    {
      var.setValue(k_NewDataContainerName);
      propWasSet = cropVolume->setProperty("NewDataContainerName", var);
      require_equal<bool, bool>(propWasSet, "propWasSet", true, "true");
    }

    var.setValue(renumberGrains);
    propWasSet = cropVolume->setProperty("RenumberFeatures", var);
    require_equal<bool, bool>(propWasSet, "propWasSet", true, "true");

  }
  else
  {
    QString ss = QObject::tr("CropVolumeTest creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
    DREAM3D_REQUIRE_EQUAL(0, 1)
  }
  return cropVolume;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void printArraySlice(typename DataArray<T>::Pointer ptr, NumPackage X, NumPackage Y, NumPackage Z)
{
  DataArray<T> &p = *(ptr.get());
  int XP = (X.getMax() - X.getMin());
  int YP = (Y.getMax() - Y.getMin());
  int ZP = (Z.getMax() - Z.getMin());

  int numComponents = p.getNumberOfComponents();

  QVector<size_t> cDims(1, numComponents);

  for (int64_t z = Z.getMin(); z < Z.getMax(); z++)
  {
    for (int64_t y = Y.getMin(); y < Y.getMax(); y++)
    {
      QString ss;
      QTextStream out(&ss);
      for (int64_t x = X.getMin(); x < X.getMax(); x++)
      {
        int64_t index = ((z - Z.getMin())*XP*YP) + ((y - Y.getMin()) * XP) + (x - X.getMin());
        out << "[";
        for (int i = 0; i < cDims[0]; i++)
        {
          out << p.getComponent(index, i);
          if(i < cDims[0] - 1) { out << " "; }
        }
        out << "] ";
      }
      qDebug() << ss;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestCropVolume_1()
{

  // Setup Data Structure
  bool renumberGrains = false;
  bool createNewDataContainer = false;
  AbstractFilter::Pointer cropVolume = CreateCropVolumeFilter(s_CroppedX, s_CroppedY, s_CroppedZ, renumberGrains, createNewDataContainer);
  DataArrayPath dap(k_DataContainerName, k_CellAttributeMatrixName, k_DataArrayName);

  // Get the data array so we can print it to the console - for debugging only
  Int32ArrayType::Pointer data = cropVolume->getDataContainerArray()->getExistingPrereqArrayFromPath<Int32ArrayType>(cropVolume.get(), dap);
  if(0)
  {
    printArraySlice<int>(data, s_OriginalX, s_OriginalY, s_OriginalZ);
  }
  // Now get the 4 comp (UInt8 array)
  dap.setDataArrayName(k_4CompDataArrayName);
  UInt8ArrayType::Pointer four = cropVolume->getDataContainerArray()->getExistingPrereqArrayFromPath<UInt8ArrayType>(cropVolume.get(), dap);
  if(0) // And possibly print it out
  {
    printArraySlice<unsigned char>(four, s_OriginalX, s_OriginalY, s_OriginalZ);
  }
  int err = 0;
  cropVolume->preflight();
  err = cropVolume->getErrorCondition();
  require_equal<int, int>(err, "err", 0, "Value", __FILE__, __LINE__);

  // Create a new DataContainer Array for the actual execution of the filter. this mimics
  // what the PipelinRunner would do.
  resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 1);
  cropVolume->execute();
  err = cropVolume->getErrorCondition();
  // Make sure we executed without any error
  require_greater_than<int, int>(err, "err", -1, "Value");
  // Get the data array and check the crop on it.
  dap.setDataArrayName(k_DataArrayName);
  data = cropVolume->getDataContainerArray()->getExistingPrereqArrayFromPath<Int32ArrayType>(cropVolume.get(), dap);
  checkCrop<int, int>(data, s_CroppedX, s_CroppedY, s_CroppedZ);
  // Now get the 4 comp (UInt8 array)
  dap.setDataArrayName(k_4CompDataArrayName);
  four = cropVolume->getDataContainerArray()->getExistingPrereqArrayFromPath<UInt8ArrayType>(cropVolume.get(), dap);
  checkCrop<uint8_t, uint8_t>(four, s_CroppedX, s_CroppedY, s_CroppedZ);

  if(0)
  {
    s_CroppedX.setMax(s_CroppedX.getMax() + 1);
    s_CroppedY.setMax(s_CroppedY.getMax() + 1);
    s_CroppedZ.setMax(s_CroppedZ.getMax() + 1);
    printArraySlice<uint8_t>(four, s_CroppedX, s_CroppedY, s_CroppedZ);
  }

}

// -----------------------------------------------------------------------------
// Basic Test to crop a simple volume and store the cropped data into a new
// DataContainer.
// -----------------------------------------------------------------------------
void TestCropVolume_2()
{

  // Setup Data Structure
  bool renumberGrains = false;
  bool createNewDataContainer = true;
  AbstractFilter::Pointer cropVolume = CreateCropVolumeFilter(s_CroppedX, s_CroppedY, s_CroppedZ, renumberGrains, createNewDataContainer);
  DataArrayPath dap(k_DataContainerName, k_CellAttributeMatrixName, k_DataArrayName);

  // Get the data array so we can print it to the console - for debugging only
  Int32ArrayType::Pointer data = cropVolume->getDataContainerArray()->getExistingPrereqArrayFromPath<Int32ArrayType>(cropVolume.get(), dap);
  if(0)
  {
    printArraySlice<int>(data, s_OriginalX, s_OriginalY, s_OriginalZ);
  }
  // Now get the 4 comp (UInt8 array)
  dap.setDataArrayName(k_4CompDataArrayName);
  UInt8ArrayType::Pointer four = cropVolume->getDataContainerArray()->getExistingPrereqArrayFromPath<UInt8ArrayType>(cropVolume.get(), dap);
  if(0) // And possibly print it out
  {
    printArraySlice<unsigned char>(four, s_OriginalX, s_OriginalY, s_OriginalZ);
  }

  int err = 0;
  cropVolume->preflight();
  err = cropVolume->getErrorCondition();
  require_equal<int, int>(err, "err", 0, "Value", __FILE__, __LINE__);

  // Create a new DataContainer Array for the actual execution of the filter. this mimics
  // what the PipelinRunner would do.
  resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 1);
  cropVolume->execute();
  err = cropVolume->getErrorCondition();
  // Make sure we executed without any error
  require_greater_than<int, int>(err, "err", -1, "Value");
  // Get the data array and check the crop on it.
  dap = DataArrayPath(k_NewDataContainerName, k_CellAttributeMatrixName, k_DataArrayName);
  data = cropVolume->getDataContainerArray()->getExistingPrereqArrayFromPath<Int32ArrayType>(cropVolume.get(), dap);
  checkCrop<int, int>(data, s_CroppedX, s_CroppedY, s_CroppedZ);

  if(0)
  {
    s_CroppedX.setMax(s_CroppedX.getMax() + 1);
    s_CroppedY.setMax(s_CroppedY.getMax() + 1);
    s_CroppedZ.setMax(s_CroppedZ.getMax() + 1);
    printArraySlice<int>(data, s_CroppedX, s_CroppedY, s_CroppedZ);
  }
  // Now get the 4 comp (UInt8 array)
  dap.setDataArrayName(k_4CompDataArrayName);
  four = cropVolume->getDataContainerArray()->getExistingPrereqArrayFromPath<UInt8ArrayType>(cropVolume.get(), dap);
  checkCrop<uint8_t, uint8_t>(four, s_CroppedX, s_CroppedY, s_CroppedZ);
  if(0)
  {
    s_CroppedX.setMax(s_CroppedX.getMax() + 1);
    s_CroppedY.setMax(s_CroppedY.getMax() + 1);
    s_CroppedZ.setMax(s_CroppedZ.getMax() + 1);
    printArraySlice<uint8_t>(four, s_CroppedX, s_CroppedY, s_CroppedZ);
  }
}

// -----------------------------------------------------------------------------
// Test the basic Min/Max settings the user/programmer could pass in and make sure
// we are trapping on out-of-bounds values
// -----------------------------------------------------------------------------
void TestCropVolume_3()
{

  // Setup Data Structure
  bool renumberGrains = false;
  bool createNewDataContainer = true;
  AbstractFilter::Pointer cropVolume = CreateCropVolumeFilter(s_CroppedX, s_CroppedY, s_CroppedZ, renumberGrains, createNewDataContainer);
  preflightTests(cropVolume);

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

  DREAM3D_REGISTER_TEST(loadFilterPlugins());

  DREAM3D_REGISTER_TEST(TestCropVolume_1())
  DREAM3D_REGISTER_TEST(TestCropVolume_2())
  DREAM3D_REGISTER_TEST(TestCropVolume_3())

      PRINT_TEST_SUMMARY();
  return err;
}





