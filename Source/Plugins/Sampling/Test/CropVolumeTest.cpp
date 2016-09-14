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


#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/CoreFilters/DataContainerWriter.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "SamplingTestFileLocations.h"

static const QString k_DataArrayName("Data");
static const QString k_4CompDataArrayName("Data 4 Comp");
static const QString k_FeatureIdsName("FeatureIds");
static const QString k_ActivesName("Actives");
static const QString k_DataContainerName = SIMPL::Defaults::ImageDataContainerName;
static const QString k_NewDataContainerName = SIMPL::Defaults::NewImageDataContainerName;
static const QString k_CellAttributeMatrixName = SIMPL::Defaults::CellAttributeMatrixName;
static const QString k_FeatureAttributeMatrixName = SIMPL::Defaults::CellFeatureAttributeMatrixName;


static Observer obs;

class NumPackage
{
  public:
    NumPackage(const NumPackage& rhs) { m_Min = rhs.getMin(); m_Max = rhs.getMax(); m_Diff = m_Max - m_Min; }
    NumPackage(int min, int max) { m_Min = min; m_Max = max; m_Diff = m_Max - m_Min; }
    void operator=(const NumPackage& rhs)
    {
      m_Min = rhs.getMin();
      m_Max = rhs.getMax();
      m_Diff = m_Max - m_Min;
    }

    void setMin(int min) { m_Min = min; m_Diff = m_Max - m_Min; }
    int getMin() const { return m_Min; }
    Q_PROPERTY(int Min READ getMin WRITE setMin)

    void setMax(int max) { m_Max = max; m_Diff = m_Max - m_Min; }
    int getMax() const { return m_Max; }
    Q_PROPERTY(int Max READ getMax WRITE setMax)

    int getDiff() const { return m_Diff; }

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

class CropVolumeTest
{
  public:
    CropVolumeTest(){}
    virtual ~CropVolumeTest(){}
    SIMPL_TYPE_MACRO(CropVolumeTest)

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    AbstractFilter::Pointer CreateDataContainerWriter(const QString& outputFile)
    {
      AbstractFilter::Pointer filter = AbstractFilter::NullPointer();

      QString filtName = "DataContainerWriter";
      FilterManager* fm = FilterManager::Instance();
      IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
      DREAM3D_REQUIRE_VALID_POINTER(filterFactory.get());
      if (nullptr != filterFactory.get())
      {

        filter = filterFactory->create();
        filter->connect(filter.get(), SIGNAL(filterGeneratedMessage(const PipelineMessage&)),
                        &obs, SLOT(processPipelineMessage(const PipelineMessage&)));

        QVariant var;
        var.setValue(outputFile);
        bool propWasSet = filter->setProperty("OutputFile", var);
        require_equal<bool, bool>(propWasSet, "OutputFile", true, "true");


        var.setValue(false);
        propWasSet = filter->setProperty("WriteXdmfFile", var);
        require_equal<bool, bool>(propWasSet, "WriteXdmfFile", true, "true");


      }

      return filter;
    }


    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    DataContainerArray::Pointer CreateDataContainerArrayTestStructure(NumPackage X, NumPackage Y, NumPackage Z, int numComponents)
    {
      int err = 0;
      DataContainerArray::Pointer dca = DataContainerArray::New();

      DataContainer::Pointer dc1 = DataContainer::New(SIMPL::Defaults::ImageDataContainerName);

      ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
      dc1->setGeometry(image);

      dc1->getGeometryAs<ImageGeom>()->setDimensions(X.getMax(), Y.getMax(), Z.getMax());
      dc1->getGeometryAs<ImageGeom>()->setOrigin(originalOrigin);
      dc1->getGeometryAs<ImageGeom>()->setResolution(originalRes);

      QVector<size_t> amDims;
      amDims.push_back(X.getMax());
      amDims.push_back(Y.getMax());
      amDims.push_back(Z.getMax());
      AttributeMatrix::Pointer am1 = AttributeMatrix::New(amDims, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::AttributeMatrixType::Cell);
      AttributeMatrix::Pointer am2 = AttributeMatrix::New(amDims, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::AttributeMatrixType::CellFeature);

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
      featureIds->initializeWithValue(0xABABABAB);
      DataArray<bool>::Pointer actives = DataArray<bool>::CreateArray(tDims, cDims, k_ActivesName, true);

      size_t index = 0;

      struct { uint8_t r; uint8_t g; uint8_t b; uint8_t a;} rgba;
      int32_t* ptr = reinterpret_cast<int32_t*>(&rgba);
      uint8_t* u8ptr = reinterpret_cast<uint8_t*>(&rgba);


      u8ptr[3] = 0;
      for (size_t z = 0; z < tDims[2]; z++)
      {
        u8ptr[2] = (uint8_t)(z);
        for (size_t y = 0; y < tDims[1]; y++)
        {
          u8ptr[1] = (uint8_t)(y);
          for (size_t x = 0; x < tDims[0]; x++)
          {
            index = (z * tDims[0] * tDims[1]) + (y * tDims[0]) + x;

            u8ptr[0] = (uint8_t)(x);
            genericData->setComponent(index, 0, *ptr);

            fourCompData->setComponent(index, 0, x);
            fourCompData->setComponent(index, 1, y);
            fourCompData->setComponent(index, 2, z);
            fourCompData->setComponent(index, 3, 0);
            // These are 1 component arrays
            featureIds->setValue(index, index);
            actives->setValue(index, true);
          }
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
      DataArray<T>& p = *(data.get());
      // We need to add the +1 here because the Crop Volume filter is "inclusive" of the ranges
      // that the user passes to it. This means that for an x range of 0->4 there are actually
      // 5 elements. If the range is 1->5 then there are 5 elements.
      int XP = (X.getMax() - X.getMin()) + 1;
      int YP = (Y.getMax() - Y.getMin()) + 1;
      int ZP = (Z.getMax() - Z.getMin()) + 1;
      int numComponents = data->getNumberOfComponents();

      require_equal<size_t, size_t>(p.getSize(), "p.getSize()", XP * YP * ZP * numComponents, "XP*YP*ZP*numComponents", __FILE__, __LINE__);

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

            int64_t index = ((z - Z.getMin()) * XP * YP) + ((y - Y.getMin()) * XP) + (x - X.getMin());
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
    template<typename T, typename K>
    void checkFeatureIdsCrop(typename DataArray<T>::Pointer data, NumPackage& X, NumPackage& Y, NumPackage& Z)
    {
      DataArray<T>& p = *(data.get());
      // We need to add the +1 here because the Crop Volume filter is "inclusive" of the ranges
      // that the user passes to it. This means that for an x range of 0->4 there are actually
      // 5 elements. If the range is 1->5 then there are 5 elements.
      int XP = (X.getMax() - X.getMin()) + 1;
      int YP = (Y.getMax() - Y.getMin()) + 1;
      int ZP = (Z.getMax() - Z.getMin()) + 1;
      int numComponents = data->getNumberOfComponents();

      require_equal<size_t, size_t>(p.getSize(), "p.getSize()", XP * YP * ZP * numComponents, "XP*YP*ZP*numComponents", __FILE__, __LINE__);

      for (int64_t z = Z.getMin(); z < Z.getMax(); z++)
      {
        for (int64_t y = Y.getMin(); y < Y.getMax(); y++)
        {
          for (int64_t x = X.getMin(); x < X.getMax(); x++)
          {
            int64_t index = ((z - Z.getMin()) * XP * YP) + ((y - Y.getMin()) * XP) + (x - X.getMin());
            int32_t calc = ((z ) * XP * YP) + ((y) * XP) + (x ) + 1; // The +1 is because DREAM3D stars from 1, NOT Zero
            T value = p.getValue(index);
            require_equal<T, T>(value, "Stored", calc, "Calculated", __FILE__, __LINE__);
          }
        }
      }
    }


    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    template<typename T, typename K>
    void checkRenumber(typename DataArray<T>::Pointer data, NumPackage& X, NumPackage& Y, NumPackage& Z)
    {
      DataArray<T>& p = *(data.get());
      // We need to add the +1 here because the Crop Volume filter is "inclusive" of the ranges
      // that the user passes to it. This means that for an x range of 0->4 there are actually
      // 5 elements. If the range is 1->5 then there are 5 elements.
      int XP = X.getDiff() + 1; //X.getMax() - X.getMin());
      int YP = Y.getDiff() + 1; //(Y.getMax() - Y.getMin());
      int ZP = Z.getDiff() + 1; //(Z.getMax() - Z.getMin());
      int numComponents = data->getNumberOfComponents();

      require_equal<size_t, size_t>(p.getSize(), "p.getSize()", XP * YP * ZP * numComponents, "XP*YP*ZP*numComponents", __FILE__, __LINE__);
      int32_t featureId = 1;
      for (int64_t z = Z.getMin(); z < Z.getMax(); z++)
      {
        for (int64_t y = Y.getMin(); y < Y.getMax(); y++)
        {
          for (int64_t x = X.getMin(); x < X.getMax(); x++)
          {
            int64_t index = ((z - Z.getMin()) * XP * YP) + ((y - Y.getMin()) * XP) + (x - X.getMin());

            T value = p.getComponent(index, 0);

            //   int32_t calculated = (z * s_OriginalX.getDiff() * s_OriginalY.getDiff()) + (s_OriginalX.getDiff() * y) + x;
            require_equal<T, T>(value, "Stored", featureId, "featureId", __FILE__, __LINE__);
            featureId++;
          }
        }
      }
    }


    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    template<typename T>
    void printArraySlice(typename DataArray<T>::Pointer ptr, NumPackage X, NumPackage Y, NumPackage Z)
    {
      DataArray<T>& p = *(ptr.get());
      int XP = (X.getMax() - X.getMin());
      int YP = (Y.getMax() - Y.getMin());
      //int ZP = (Z.getMax() - Z.getMin());

      int numComponents = p.getNumberOfComponents();

      QVector<size_t> cDims(1, numComponents);
      qDebug() << "---------------- " << ptr->getName() << " -------------------";
      for (int64_t z = Z.getMin(); z < Z.getMax(); z++)
      {
        qDebug() << "<< SLICE " << z << " >>";
        for (int64_t y = Y.getMin(); y < Y.getMax(); y++)
        {
          QString ss;
          QTextStream out(&ss);
          for (int64_t x = X.getMin(); x < X.getMax(); x++)
          {
            int64_t index = ((z - Z.getMin()) * XP * YP) + ((y - Y.getMin()) * XP) + (x - X.getMin());
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
      // Fails because getPrereqGeometryFromDataContainer catches the nullptr DataContainer
      // Error code should be -999
      DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -999)
          resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 3);

      // Test Source Attribute Matrix Does Not Exist
      path.setDataContainerName(SIMPL::Defaults::ImageDataContainerName);
      var.setValue(path);
      propWasSet = cropVolume->setProperty("CellAttributeMatrixPath", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      cropVolume->preflight();
      // Fails because getPrereqAttributeMatrixFromPath catches the nullptr AttributeMatrix
      // Error code should be -301 * 1020 = -307020
      DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -307020)
          resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 3);

      path.setAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName);
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
      DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5550)
          resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 3);
      var.setValue(s_CroppedY.getMin());
      propWasSet = cropVolume->setProperty("YMin", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      var.setValue(s_CroppedZ.getMax() + 1);
      propWasSet = cropVolume->setProperty("ZMin", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      cropVolume->preflight();
      DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5550)
          resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 3);
      var.setValue(s_CroppedZ.getMin());
      propWasSet = cropVolume->setProperty("ZMin", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      var.setValue(-1);
      propWasSet = cropVolume->setProperty("XMin", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      cropVolume->preflight();
      DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5550)
          resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 3);
      var.setValue(s_CroppedX.getMin());
      propWasSet = cropVolume->setProperty("XMin", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      var.setValue(-1);
      propWasSet = cropVolume->setProperty("YMin", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      cropVolume->preflight();
      DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5550)
          resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 3);
      var.setValue(s_CroppedY.getMin());
      propWasSet = cropVolume->setProperty("YMin", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      var.setValue(-1);
      propWasSet = cropVolume->setProperty("ZMin", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      cropVolume->preflight();
      DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5550)
          resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 3);
      var.setValue(s_CroppedZ.getMin());
      propWasSet = cropVolume->setProperty("ZMin", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      var.setValue(s_OriginalX.getMax() + 1);
      propWasSet = cropVolume->setProperty("XMax", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      cropVolume->preflight();
      DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5550)
          resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 3);
      var.setValue(s_CroppedX.getMax());
      propWasSet = cropVolume->setProperty("XMax", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      var.setValue(s_OriginalY.getMax() + 1);
      propWasSet = cropVolume->setProperty("YMax", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      cropVolume->preflight();
      DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5550)
          resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 3);
      var.setValue(s_CroppedY.getMax());
      propWasSet = cropVolume->setProperty("YMax", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      var.setValue(s_OriginalZ.getMax() + 1);
      propWasSet = cropVolume->setProperty("ZMax", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);
      cropVolume->preflight();
      DREAM3D_REQUIRE_EQUAL(cropVolume->getErrorCondition(), -5550)
          resetTest(cropVolume, s_OriginalX, s_OriginalY, s_OriginalZ, 3);
      var.setValue(s_CroppedZ.getMax());
      propWasSet = cropVolume->setProperty("ZMax", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

    }


    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    AbstractFilter::Pointer CreateCropVolumeFilter(NumPackage x, NumPackage y, NumPackage z,
                                                   bool renumberGrains, bool createNewDC)
    {
      AbstractFilter::Pointer cropVolume = AbstractFilter::NullPointer();

      QString filtName = "CropImageGeometry";
      FilterManager* fm = FilterManager::Instance();
      IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
      if (nullptr != filterFactory.get())
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
    // Test the basic Min/Max settings the user/programmer could pass in and make sure
    // we are trapping on out-of-bounds values
    // -----------------------------------------------------------------------------
    void TestCropVolume_0()
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
    void TestCropVolume_1()
    {

      // Setup Data Structure
      bool renumberGrains = false;
      bool createNewDataContainer = false;
      AbstractFilter::Pointer cropVolume = CreateCropVolumeFilter(s_CroppedX, s_CroppedY, s_CroppedZ, renumberGrains, createNewDataContainer);
      DataArrayPath dap(k_DataContainerName, k_CellAttributeMatrixName, k_DataArrayName);

      // Get the data array so we can print it to the console - for debugging only
      Int32ArrayType::Pointer data = cropVolume->getDataContainerArray()->getPrereqIDataArrayFromPath<Int32ArrayType>(cropVolume.get(), dap);
      if(0)
      {
        printArraySlice<int>(data, s_OriginalX, s_OriginalY, s_OriginalZ);
      }
      // Now get the 4 comp (UInt8 array)
      dap.setDataArrayName(k_4CompDataArrayName);
      UInt8ArrayType::Pointer four = cropVolume->getDataContainerArray()->getPrereqIDataArrayFromPath<UInt8ArrayType>(cropVolume.get(), dap);
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
      data = cropVolume->getDataContainerArray()->getPrereqIDataArrayFromPath<Int32ArrayType>(cropVolume.get(), dap);
      checkCrop<int, int>(data, s_CroppedX, s_CroppedY, s_CroppedZ);
      // Now get the 4 comp (UInt8 array)
      dap.setDataArrayName(k_4CompDataArrayName);
      four = cropVolume->getDataContainerArray()->getPrereqIDataArrayFromPath<UInt8ArrayType>(cropVolume.get(), dap);
      checkCrop<uint8_t, uint8_t>(four, s_CroppedX, s_CroppedY, s_CroppedZ);

      if(0)
      {
        s_CroppedX.setMax(s_CroppedX.getMax() + 1);
        s_CroppedY.setMax(s_CroppedY.getMax() + 1);
        s_CroppedZ.setMax(s_CroppedZ.getMax() + 1);
        printArraySlice<uint8_t>(four, s_CroppedX, s_CroppedY, s_CroppedZ);
      }
      // Now we need to check that the FeatureIds got cropped correctly
      dap = DataArrayPath(k_DataContainerName, k_CellAttributeMatrixName, k_FeatureIdsName);
      data = cropVolume->getDataContainerArray()->getPrereqIDataArrayFromPath<Int32ArrayType>(cropVolume.get(), dap);
      DREAM3D_REQUIRE_VALID_POINTER(data.get());
      if(0)
      {
        NumPackage X(s_CroppedX.getMin(), s_CroppedX.getMax() + 1);
        NumPackage Y(s_CroppedY.getMin(), s_CroppedY.getMax() + 1);
        NumPackage Z(s_CroppedZ.getMin(), s_CroppedZ.getMax() + 1);
        printArraySlice<int>(data, X, Y, Z);
      }
      checkFeatureIdsCrop<int32_t, int32_t>(data, s_CroppedX, s_CroppedY, s_CroppedZ);
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
      Int32ArrayType::Pointer data = cropVolume->getDataContainerArray()->getPrereqIDataArrayFromPath<Int32ArrayType>(cropVolume.get(), dap);
      if(0)
      {
        printArraySlice<int>(data, s_OriginalX, s_OriginalY, s_OriginalZ);
      }
      // Now get the 4 comp (UInt8 array)
      dap.setDataArrayName(k_4CompDataArrayName);
      UInt8ArrayType::Pointer four = cropVolume->getDataContainerArray()->getPrereqIDataArrayFromPath<UInt8ArrayType>(cropVolume.get(), dap);
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
      data = cropVolume->getDataContainerArray()->getPrereqIDataArrayFromPath<Int32ArrayType>(cropVolume.get(), dap);
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
      four = cropVolume->getDataContainerArray()->getPrereqIDataArrayFromPath<UInt8ArrayType>(cropVolume.get(), dap);
      checkCrop<uint8_t, uint8_t>(four, s_CroppedX, s_CroppedY, s_CroppedZ);
      if(0)
      {
        s_CroppedX.setMax(s_CroppedX.getMax() + 1);
        s_CroppedY.setMax(s_CroppedY.getMax() + 1);
        s_CroppedZ.setMax(s_CroppedZ.getMax() + 1);
        printArraySlice<uint8_t>(four, s_CroppedX, s_CroppedY, s_CroppedZ);
      }

      // Now we need to check that the FeatureIds got cropped correctly
      dap.setDataArrayName(k_FeatureIdsName);
      data = cropVolume->getDataContainerArray()->getPrereqIDataArrayFromPath<Int32ArrayType>(cropVolume.get(), dap);
      DREAM3D_REQUIRE_VALID_POINTER(data.get());
      if(0)
      {
        NumPackage X(s_CroppedX.getMin(), s_CroppedX.getMax() + 1);
        NumPackage Y(s_CroppedY.getMin(), s_CroppedY.getMax() + 1);
        NumPackage Z(s_CroppedZ.getMin(), s_CroppedZ.getMax() + 1);
        printArraySlice<int>(data, X, Y, Z);
      }
      checkFeatureIdsCrop<int32_t, int32_t>(data, s_CroppedX, s_CroppedY, s_CroppedZ);
    }


    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void TestCropVolume_3()
    {
      // Setup Data Structure
      bool renumberGrains = true;
      bool createNewDataContainer = false;
      AbstractFilter::Pointer cropVolume = CreateCropVolumeFilter(s_CroppedX, s_CroppedY, s_CroppedZ, renumberGrains, createNewDataContainer);
      DataArrayPath dap(k_DataContainerName, k_CellAttributeMatrixName, k_DataArrayName);

      // Get the data array so we can print it to the console - for debugging only
      Int32ArrayType::Pointer data = cropVolume->getDataContainerArray()->getPrereqIDataArrayFromPath<Int32ArrayType>(cropVolume.get(), dap);
      if(0)
      {
        printArraySlice<int>(data, s_OriginalX, s_OriginalY, s_OriginalZ);
      }
      // Now get the 4 comp (UInt8 array)
      dap.setDataArrayName(k_4CompDataArrayName);
      UInt8ArrayType::Pointer four = cropVolume->getDataContainerArray()->getPrereqIDataArrayFromPath<UInt8ArrayType>(cropVolume.get(), dap);
      if(0) // And possibly print it out
      {
        printArraySlice<unsigned char>(four, s_OriginalX, s_OriginalY, s_OriginalZ);
      }

      dap = DataArrayPath(k_DataContainerName, k_CellAttributeMatrixName, k_FeatureIdsName);
      data = cropVolume->getDataContainerArray()->getPrereqIDataArrayFromPath<Int32ArrayType>(cropVolume.get(), dap);
      if(0)
      {
        printArraySlice<int32_t>(data, s_OriginalX, s_OriginalY, s_OriginalZ);
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


      AbstractFilter::Pointer writer = CreateDataContainerWriter(UnitTest::CropVolumeTest::CropVolumeTest_3);
      writer->setDataContainerArray(cropVolume->getDataContainerArray());
      writer->execute();
      DREAM3D_REQUIRE(writer->getErrorCondition() > -1);


      // Get the data array and check the crop on it.
      dap = DataArrayPath(k_DataContainerName, k_CellAttributeMatrixName, k_DataArrayName);
      data = cropVolume->getDataContainerArray()->getPrereqIDataArrayFromPath<Int32ArrayType>(cropVolume.get(), dap);
      DREAM3D_REQUIRE_VALID_POINTER(data.get())

          checkCrop<int, int>(data, s_CroppedX, s_CroppedY, s_CroppedZ);

      if(0)
      {
        NumPackage X(s_CroppedX.getMin(), s_CroppedX.getMax() + 1);
        NumPackage Y(s_CroppedY.getMin(), s_CroppedY.getMax() + 1);
        NumPackage Z(s_CroppedZ.getMin(), s_CroppedZ.getMax() + 1);
        printArraySlice<int>(data, X, Y, Z);
      }
      // Now get the 4 comp (UInt8 array)
      dap.setDataArrayName(k_4CompDataArrayName);
      four = cropVolume->getDataContainerArray()->getPrereqIDataArrayFromPath<UInt8ArrayType>(cropVolume.get(), dap);
      checkCrop<uint8_t, uint8_t>(four, s_CroppedX, s_CroppedY, s_CroppedZ);
      if(0)
      {
        NumPackage X(s_CroppedX.getMin(), s_CroppedX.getMax() + 1);
        NumPackage Y(s_CroppedY.getMin(), s_CroppedY.getMax() + 1);
        NumPackage Z(s_CroppedZ.getMin(), s_CroppedZ.getMax() + 1);
        printArraySlice<uint8_t>(four, X, Y, Z);
      }

      // Now we need to check that the FeatureIds got cropped correctly
      dap.setDataArrayName(k_FeatureIdsName);
      data = cropVolume->getDataContainerArray()->getPrereqIDataArrayFromPath<Int32ArrayType>(cropVolume.get(), dap);
      DREAM3D_REQUIRE_VALID_POINTER(data.get());
      if(0)
      {
        NumPackage X(s_CroppedX.getMin(), s_CroppedX.getMax() + 1);
        NumPackage Y(s_CroppedY.getMin(), s_CroppedY.getMax() + 1);
        NumPackage Z(s_CroppedZ.getMin(), s_CroppedZ.getMax() + 1);
        printArraySlice<int>(data, X, Y, Z);
      }
      checkRenumber<int32_t, int32_t>(data, s_CroppedX, s_CroppedY, s_CroppedZ);

    }


    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void TestCropVolume_4()
    {
      // Setup Data Structure
      bool renumberGrains = true;
      bool createNewDataContainer = true;
      AbstractFilter::Pointer cropVolume = CreateCropVolumeFilter(s_CroppedX, s_CroppedY, s_CroppedZ, renumberGrains, createNewDataContainer);
      DataArrayPath dap(k_DataContainerName, k_CellAttributeMatrixName, k_DataArrayName);

      // Get the data array so we can print it to the console - for debugging only
      Int32ArrayType::Pointer data = cropVolume->getDataContainerArray()->getPrereqIDataArrayFromPath<Int32ArrayType>(cropVolume.get(), dap);
      if(0)
      {
        printArraySlice<int>(data, s_OriginalX, s_OriginalY, s_OriginalZ);
      }
      // Now get the 4 comp (UInt8 array)
      dap.setDataArrayName(k_4CompDataArrayName);
      UInt8ArrayType::Pointer four = cropVolume->getDataContainerArray()->getPrereqIDataArrayFromPath<UInt8ArrayType>(cropVolume.get(), dap);
      if(0) // And possibly print it out
      {
        printArraySlice<unsigned char>(four, s_OriginalX, s_OriginalY, s_OriginalZ);
      }

      dap = DataArrayPath(k_DataContainerName, k_CellAttributeMatrixName, k_FeatureIdsName);
      data = cropVolume->getDataContainerArray()->getPrereqIDataArrayFromPath<Int32ArrayType>(cropVolume.get(), dap);
      if(0)
      {
        printArraySlice<int32_t>(data, s_OriginalX, s_OriginalY, s_OriginalZ);
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
      data = cropVolume->getDataContainerArray()->getPrereqIDataArrayFromPath<Int32ArrayType>(cropVolume.get(), dap);
      DREAM3D_REQUIRE_VALID_POINTER(data.get());

      checkCrop<int, int>(data, s_CroppedX, s_CroppedY, s_CroppedZ);

      if(0)
      {
        NumPackage X(s_CroppedX.getMin(), s_CroppedX.getMax() + 1);
        NumPackage Y(s_CroppedY.getMin(), s_CroppedY.getMax() + 1);
        NumPackage Z(s_CroppedZ.getMin(), s_CroppedZ.getMax() + 1);
        printArraySlice<int>(data, X, Y, Z);
      }
      // Now get the 4 comp (UInt8 array)
      dap.setDataArrayName(k_4CompDataArrayName);
      four = cropVolume->getDataContainerArray()->getPrereqIDataArrayFromPath<UInt8ArrayType>(cropVolume.get(), dap);
      checkCrop<uint8_t, uint8_t>(four, s_CroppedX, s_CroppedY, s_CroppedZ);
      if(0)
      {
        NumPackage X(s_CroppedX.getMin(), s_CroppedX.getMax() + 1);
        NumPackage Y(s_CroppedY.getMin(), s_CroppedY.getMax() + 1);
        NumPackage Z(s_CroppedZ.getMin(), s_CroppedZ.getMax() + 1);
        printArraySlice<uint8_t>(four, X, Y, Z);
      }

      // Now we need to check that the FeatureIds got renumbered correctly
      dap.setDataArrayName(k_FeatureIdsName);
      data = cropVolume->getDataContainerArray()->getPrereqIDataArrayFromPath<Int32ArrayType>(cropVolume.get(), dap);
      DREAM3D_REQUIRE_VALID_POINTER(data.get());

      AbstractFilter::Pointer writer = CreateDataContainerWriter(UnitTest::CropVolumeTest::CropVolumeTest_4);
      writer->setDataContainerArray(cropVolume->getDataContainerArray());
      writer->execute();
      DREAM3D_REQUIRE(writer->getErrorCondition() > -1);

      //  AttributeMatrix::Pointer am = cropVolume->getDataContainerArray()->getAttributeMatrix(dap);
      //  QVector<size_t> tDims = am->getTupleDimensions();
      //  for(size_t i = 0; i < tDims.size(); i++) { qDebug() << tDims[i]; }

      // Now we need to check that the FeatureIds got cropped correctly
      dap.setDataArrayName(k_FeatureIdsName);
      data = cropVolume->getDataContainerArray()->getPrereqIDataArrayFromPath<Int32ArrayType>(cropVolume.get(), dap);
      DREAM3D_REQUIRE_VALID_POINTER(data.get());
      if(0)
      {
        NumPackage X(s_CroppedX.getMin(), s_CroppedX.getMax() + 1);
        NumPackage Y(s_CroppedY.getMin(), s_CroppedY.getMax() + 1);
        NumPackage Z(s_CroppedZ.getMin(), s_CroppedZ.getMax() + 1);
        printArraySlice<int>(data, X, Y, Z);
      }
      checkRenumber<int32_t, int32_t>(data, s_CroppedX, s_CroppedY, s_CroppedZ);
    }

    /**
  * @brief
*/
    void operator()()
    {
      int err = EXIT_SUCCESS;
      DREAM3D_REGISTER_TEST(TestCropVolume_0());
      DREAM3D_REGISTER_TEST(TestCropVolume_1());
      DREAM3D_REGISTER_TEST(TestCropVolume_2());
      DREAM3D_REGISTER_TEST(TestCropVolume_3());
      DREAM3D_REGISTER_TEST(TestCropVolume_4());
    }
  private:
    CropVolumeTest(const CropVolumeTest&); // Copy Constructor Not Implemented
    void operator=(const CropVolumeTest&); // Operator '=' Not Implemented
};





