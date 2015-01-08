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

class NumPackage
{
  public:
    NumPackage(int min, int max) {m_Min = min; m_Max = max; m_Diff = m_Max-m_Min;}

    void setMin(int min) {m_Min = min; m_Diff = m_Max-m_Min;}
    int getMin() {return m_Min;}
    Q_PROPERTY(int Min READ getMin WRITE setMin)

    void setMax(int max) {m_Max = max; m_Diff = m_Max-m_Min;}
    int getMax() {return m_Max;}
    Q_PROPERTY(int Max READ getMax WRITE setMax)

    int getDiff() {return m_Diff;}

    private:
      int m_Diff;
      int m_Max;
      int m_Min;
};

static NumPackage originalX = NumPackage(0, 20);
static NumPackage originalY = NumPackage(0, 10);
static NumPackage originalZ = NumPackage(0, 5);

static NumPackage croppedX = NumPackage(0, 5);
static NumPackage croppedY = NumPackage(0, 6);
static NumPackage croppedZ = NumPackage(0, 3);

static float originalRes[3] = {0.25, 0.25, 0.25};
static float originalOrigin[3] = {0, 0, 0};


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

  DataArray<int32_t>::Pointer ConfidenceIndexArray = DataArray<int32_t>::CreateArray(tDims, cDims, "Confidence Index", true);
  DataArray<int32_t>::Pointer FeatureIdsArray = DataArray<int32_t>::CreateArray(tDims, cDims, "FeatureIds", true);
  DataArray<bool>::Pointer ActiveArray = DataArray<bool>::CreateArray(tDims, cDims, "Active", true);

  int64_t index = 0;
  int64_t id = 1;
  for (int64_t z = 0; z < tDims[2]; z++)
  {
    for (int64_t y = 0; y < tDims[1]; y++)
    {
      for (int64_t x = 0; x < tDims[0]; x++)
      {
        index = (z*tDims[0]*tDims[1]) + (y*tDims[0]) + x;

        int64_t temp = x;
        for(int i = 0; i < cDims[0]; i++)
        {
          if (i == 1)
          {
            temp = y;
          }
          else if (i == 2)
          {
            temp = z;
          }
          ConfidenceIndexArray->setComponent(index, i, temp);
          FeatureIdsArray->setComponent(index, i, id);
          ActiveArray->setComponent(index, i, true);
        }
        id++;

//        ConfidenceIndexArray->setComponent(index, 0, x);
//        ConfidenceIndexArray->setComponent(index, 1, y);
//        ConfidenceIndexArray->setComponent(index, 2, z);
//        FeatureIdsArray->setComponent(index, 0, x);
//        FeatureIdsArray->setComponent(index, 1, y);
//        FeatureIdsArray->setComponent(index, 2, z);
//        ActiveArray->setComponent(index, 0, true);
//        ActiveArray->setComponent(index, 1, true);
//        ActiveArray->setComponent(index, 2, true);
      }
    }
  }

  err = am1->addAttributeArray("Confidence Index", ConfidenceIndexArray);
  DREAM3D_REQUIRE(err >= 0);
  err = am1->addAttributeArray("FeatureIds", FeatureIdsArray);
  DREAM3D_REQUIRE(err >= 0);
  err = am2->addAttributeArray("Active", ActiveArray);
  DREAM3D_REQUIRE(err >= 0);

  foreach (QString da, am1->getAttributeArrayNames())
  {
    DREAM3D_REQUIRE_EQUAL(am1->getAttributeArray(da)->getSize(), tDims[0]*tDims[1]*tDims[2]*cDims[0]);
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
template<typename T>
void checkCrop(typename DataArray<T>::Pointer ptr, NumPackage X, NumPackage Y, NumPackage Z, int numComponents)
{
  DataArray<T> &p = *(ptr.get());
  int XP = (X.getMax() - X.getMin()) + 1;
  int YP = (Y.getMax() - Y.getMin()) + 1;
  int ZP = (Z.getMax() - Z.getMin()) + 1;

  DREAM3D_REQUIRE_EQUAL(p.getSize(), XP*YP*ZP*numComponents)

  for (int64_t z = Z.getMin(); z < Z.getMax(); z++)
  {
    for (int64_t y = Y.getMin(); y < Y.getMax(); y++)
    {
      for (int64_t x = X.getMin(); x < X.getMax(); x++)
      {
        int64_t index = ((z-Z.getMin())*XP*YP) + ((y-Y.getMin()) * XP) + (x-X.getMin());

        int32_t X_value = p.getComponent(index, 0);
        int32_t Y_value = p.getComponent(index, 1);
        int32_t Z_value = p.getComponent(index, 2);

        DREAM3D_REQUIRE_EQUAL(X_value, x)
        DREAM3D_REQUIRE_EQUAL(Y_value, y)
        DREAM3D_REQUIRE_EQUAL(Z_value, z)
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
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

  // Test Source Data Container Does Not Exist
  DataArrayPath path("This Should Not Exist", "This Should Not Exist", "");
  var.setValue(path);
  propWasSet = cropVolume->setProperty("CellAttributeMatrixPath", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5548)
      resetTest(cropVolume, originalX, originalY, originalZ, 3);

  // Test Source Attribute Matrix Does Not Exist
  path.setDataContainerName(DREAM3D::Defaults::VolumeDataContainerName);
  var.setValue(path);
  propWasSet = cropVolume->setProperty("CellAttributeMatrixPath", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5549)
      resetTest(cropVolume, originalX, originalY, originalZ, 3);

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
      resetTest(cropVolume, originalX, originalY, originalZ, 3);
  var.setValue(croppedX.getMin());
  propWasSet = cropVolume->setProperty("XMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(croppedY.getMax()+1);
  propWasSet = cropVolume->setProperty("YMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5551)
      resetTest(cropVolume, originalX, originalY, originalZ, 3);
  var.setValue(croppedY.getMin());
  propWasSet = cropVolume->setProperty("YMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(croppedZ.getMax()+1);
  propWasSet = cropVolume->setProperty("ZMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5552)
      resetTest(cropVolume, originalX, originalY, originalZ, 3);
  var.setValue(croppedZ.getMin());
  propWasSet = cropVolume->setProperty("ZMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(-1);
  propWasSet = cropVolume->setProperty("XMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5553)
      resetTest(cropVolume, originalX, originalY, originalZ, 3);
  var.setValue(croppedX.getMin());
  propWasSet = cropVolume->setProperty("XMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(-1);
  propWasSet = cropVolume->setProperty("YMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5554)
      resetTest(cropVolume, originalX, originalY, originalZ, 3);
  var.setValue(croppedY.getMin());
  propWasSet = cropVolume->setProperty("YMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(-1);
  propWasSet = cropVolume->setProperty("ZMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5555)
      resetTest(cropVolume, originalX, originalY, originalZ, 3);
  var.setValue(croppedZ.getMin());
  propWasSet = cropVolume->setProperty("ZMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(originalX.getMax()+1);
  propWasSet = cropVolume->setProperty("XMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5556)
      resetTest(cropVolume, originalX, originalY, originalZ, 3);
  var.setValue(croppedX.getMax());
  propWasSet = cropVolume->setProperty("XMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(originalY.getMax()+1);
  propWasSet = cropVolume->setProperty("YMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5557)
      resetTest(cropVolume, originalX, originalY, originalZ, 3);
  var.setValue(croppedY.getMax());
  propWasSet = cropVolume->setProperty("YMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(originalZ.getMax()+1);
  propWasSet = cropVolume->setProperty("ZMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      cropVolume->preflight();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5558)
      resetTest(cropVolume, originalX, originalY, originalZ, 3);
  var.setValue(croppedZ.getMax());
  propWasSet = cropVolume->setProperty("ZMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

//      var.setValue(true);
//  propWasSet = cropVolume->setProperty("RenumberFeatures", var);
//  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
//      cropVolume->preflight();
//  DREAM3D_REQUIRED(cropVolume->getErrorCondition(), >=, 0)
//      resetTest(cropVolume, originalX, originalY, originalZ, 3);
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
      resetTest(cropVolume, originalX, originalY, originalZ, 3);
  var.setValue(croppedX.getMax());
  propWasSet = cropVolume->setProperty("XMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(originalY.getMax()+1);
  propWasSet = cropVolume->setProperty("YMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      cropVolume->execute();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -951)
      resetTest(cropVolume, originalX, originalY, originalZ, 3);
  var.setValue(croppedY.getMax());
  propWasSet = cropVolume->setProperty("YMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(originalZ.getMax()+1);
  propWasSet = cropVolume->setProperty("ZMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      cropVolume->execute();
  DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -952)
      resetTest(cropVolume, originalX, originalY, originalZ, 3);
  var.setValue(croppedZ.getMax());
  propWasSet = cropVolume->setProperty("ZMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      resetTest(cropVolume, originalX, originalY, originalZ, 3);

  var.setValue(false);
  propWasSet = cropVolume->setProperty("SaveAsNewDataContainer", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

  #if 1
  // Test that the crop actually works across the entire original set
      NumPackage testX(croppedX), testY(croppedY), testZ(croppedZ);
  while ( testX.getMin() < (originalX.getMax()-testX.getDiff()) )
  {
    var.setValue(testX.getMin());
    propWasSet = cropVolume->setProperty("XMin", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

        var.setValue(testX.getMax());
    propWasSet = cropVolume->setProperty("XMax", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

        testY.setMin(croppedY.getMin());
        testY.setMax(croppedY.getMax());
        while ( testY.getMin() < (originalY.getMax()-testY.getDiff()) )
    {
              var.setValue(testY.getMin());
          propWasSet = cropVolume->setProperty("YMin", var);
          DREAM3D_REQUIRE_EQUAL(propWasSet, true)

              var.setValue(testY.getMax());
          propWasSet = cropVolume->setProperty("YMax", var);
          DREAM3D_REQUIRE_EQUAL(propWasSet, true)

              testZ.setMin(croppedZ.getMin());
              testZ.setMax(croppedZ.getMax());
              while ( testZ.getMin() < (originalZ.getMax()-testZ.getDiff()) )
          {
                    var.setValue(testZ.getMin());
                propWasSet = cropVolume->setProperty("ZMin", var);
                DREAM3D_REQUIRE_EQUAL(propWasSet, true)

                    var.setValue(testZ.getMax());
                propWasSet = cropVolume->setProperty("ZMax", var);
                DREAM3D_REQUIRE_EQUAL(propWasSet, true)

                    cropVolume->execute();
                DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), 0)

                    DataArrayPath attrMatPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, "");
                    IDataArray::Pointer iDataArray = cropVolume->getDataContainerArray()->getAttributeMatrix(attrMatPath)->getAttributeArray("Confidence Index");
                    DataArray<int32_t>::Pointer array = boost::dynamic_pointer_cast< DataArray<int32_t> >(iDataArray);

                    checkCrop<int32_t>(array, testX, testY, testZ, array->getNumberOfComponents());

                    resetTest(cropVolume, originalX, originalY, originalZ, 3);

                    testZ.setMin(testZ.getMin()+1);
                    testZ.setMax(testZ.getMax()+1);
          }
          testY.setMin(testY.getMin()+1);
          testY.setMax(testY.getMax()+1);
    }

    testX.setMin(testX.getMin()+1);
    testX.setMax(testX.getMax()+1);
  }
#endif


#if 1
  // Test that the crop actually works across the entire original set, when we are saving it in a new data container
  testX = croppedX;
  testY = croppedY;
  testZ = croppedZ;

  var.setValue(true);
  propWasSet = cropVolume->setProperty("SaveAsNewDataContainer", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      while ( testX.getMin() < (originalX.getMax()-testX.getDiff()) )
  {
    var.setValue(testX.getMin());
    propWasSet = cropVolume->setProperty("XMin", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

        var.setValue(testX.getMax());
    propWasSet = cropVolume->setProperty("XMax", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

        testY.setMin(croppedY.getMin());
    testY.setMax(croppedY.getMax());
    while ( testY.getMin() < (originalY.getMax()-testY.getDiff()) )
    {
      var.setValue(testY.getMin());
      propWasSet = cropVolume->setProperty("YMin", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

          var.setValue(testY.getMax());
      propWasSet = cropVolume->setProperty("YMax", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

          testZ.setMin(croppedZ.getMin());
      testZ.setMax(croppedZ.getMax());
      while ( testZ.getMin() < (originalZ.getMax()-testZ.getDiff()) )
      {
        var.setValue(testZ.getMin());
        propWasSet = cropVolume->setProperty("ZMin", var);
        DREAM3D_REQUIRE_EQUAL(propWasSet, true)

            var.setValue(testZ.getMax());
        propWasSet = cropVolume->setProperty("ZMax", var);
        DREAM3D_REQUIRE_EQUAL(propWasSet, true)

            cropVolume->execute();
        DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), 0)

            DataArrayPath newAttrMatPath(DREAM3D::Defaults::NewVolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, "");
        IDataArray::Pointer iDataArray = cropVolume->getDataContainerArray()->getAttributeMatrix(newAttrMatPath)->getAttributeArray("Confidence Index");
        DataArray<int32_t>::Pointer array = boost::dynamic_pointer_cast< DataArray<int32_t> >(iDataArray);

        checkCrop<int32_t>(array, testX, testY, testZ, array->getNumberOfComponents());

        resetTest(cropVolume, originalX, originalY, originalZ, 3);

        testZ.setMin(testZ.getMin()+1);
        testZ.setMax(testZ.getMax()+1);
      }
      testY.setMin(testY.getMin()+1);
      testY.setMax(testY.getMax()+1);
    }

    testX.setMin(testX.getMin()+1);
    testX.setMax(testX.getMax()+1);
  }
#endif

  resetTest(cropVolume, originalX, originalY, originalZ, 1);

  // Reset min and max values back to chosen cropped values
  var.setValue(croppedX.getMin());
  propWasSet = cropVolume->setProperty("XMin", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(croppedX.getMax());
  propWasSet = cropVolume->setProperty("XMax", var);
  DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(croppedY.getMin());
      propWasSet = cropVolume->setProperty("YMin", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

          var.setValue(croppedY.getMax());
      propWasSet = cropVolume->setProperty("YMax", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

          var.setValue(croppedZ.getMin());
          propWasSet = cropVolume->setProperty("ZMin", var);
          DREAM3D_REQUIRE_EQUAL(propWasSet, true)

              var.setValue(croppedZ.getMax());
          propWasSet = cropVolume->setProperty("ZMax", var);
          DREAM3D_REQUIRE_EQUAL(propWasSet, true)

              resetTest(cropVolume, originalX, originalY, originalZ, 1);



    var.setValue(false);
    propWasSet = cropVolume->setProperty("SaveAsNewDataContainer", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

        // Test Renumber Features
        var.setValue(true);
        propWasSet = cropVolume->setProperty("RenumberFeatures", var);
        DREAM3D_REQUIRE_EQUAL(propWasSet, true)

            cropVolume->execute();
        DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), 0)

        {
            DataArrayPath newAttrMatPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, "");
            IDataArray::Pointer iDataArray = cropVolume->getDataContainerArray()->getAttributeMatrix(newAttrMatPath)->getAttributeArray("FeatureIds");
            DataArray<int32_t>::Pointer array = boost::dynamic_pointer_cast< DataArray<int32_t> >(iDataArray);

            int64_t xMin = croppedX.getMin(), yMin = croppedY.getMin(), zMin = croppedZ.getMin();
            int64_t index = 0, id = 1;
            int XP = croppedX.getDiff()+1, YP = croppedY.getDiff()+1, ZP = croppedZ.getDiff()+1;

            DREAM3D_REQUIRE_EQUAL(array->getSize(), XP*YP*ZP)

            for (int64_t z = zMin; z < ZP; z++)
            {
              for (int64_t y = yMin; y < YP; y++)
              {
                for (int64_t x = xMin; x < XP; x++)
                {
                  int64_t index = ((z-zMin)*XP*YP) + ((y-yMin) * XP) + (x-xMin);

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
          IDataArray::Pointer iDataArray = cropVolume->getDataContainerArray()->getAttributeMatrix(newAttrMatPath)->getAttributeArray("FeatureIds");
          DataArray<int32_t>::Pointer array = boost::dynamic_pointer_cast< DataArray<int32_t> >(iDataArray);

          DREAM3D_REQUIRE_NE(cropVolume->getErrorCondition(), 0)
        }

        // Test if FeatureIds array contains an id that is larger than the total number of features
        {
          resetTest(cropVolume, originalX, originalY, originalZ, 1);
          DataArrayPath newAttrMatPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, "");
          IDataArray::Pointer iDataArray = cropVolume->getDataContainerArray()->getAttributeMatrix(newAttrMatPath)->getAttributeArray("FeatureIds");
          DataArray<int32_t>::Pointer array = boost::dynamic_pointer_cast< DataArray<int32_t> >(iDataArray);
          int64_t numTuples = originalX.getMax()*originalY.getMax()*originalZ.getMax();
          array->setValue(0, numTuples+1);
          cropVolume->execute();

          iDataArray = cropVolume->getDataContainerArray()->getAttributeMatrix(newAttrMatPath)->getAttributeArray("FeatureIds");
          array = boost::dynamic_pointer_cast< DataArray<int32_t> >(iDataArray);

          DREAM3D_REQUIRE_NE(cropVolume->getErrorCondition(), 0)
        }

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
    DataContainerArray::Pointer dca = CreateDataContainerArrayTestStructure(originalX, originalY, originalZ, 3);

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





