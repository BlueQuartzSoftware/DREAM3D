

/* ============================================================================
 * Copyright (c) 2021-2021 BlueQuartz Software, LLC
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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <array>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

#include <QtCore/QFile>
#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Observer.h"
#include "SIMPLib/Common/QtBackwardCompatibilityMacro.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

#include "Generic/GenericFilters/GenerateVectorColors.h"
#include "GenericTestFileLocations.h"
#include "UnitTestSupport.hpp"

#define S2Q(str) QString::fromStdString((str))

class GenerateVectorColorsTest
{

  const std::string k_DataContainerName = {"DataContainer"};
  const std::string k_AttributeMatrixName = {"CellData"};
  const std::string k_VectorsName = {"VectorData"};
  const std::string k_ColorsName = {"Colors"};
  const std::string k_MaskName = {"Mask"};

  const size_t k_ImageDim = 1024;
  const std::vector<size_t> k_TupleDims = {k_ImageDim, k_ImageDim};

  const std::string k_TiffFilepath = UnitTest::TestTempDir.toStdString() + "/VectorColors.tif";

public:
  GenerateVectorColorsTest() = default;
  virtual ~GenerateVectorColorsTest() = default;

  QString getNameOfClass()
  {
    return QString("GenerateVectorColorsTest");
  }

  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(S2Q(k_TiffFilepath));
#endif
  }

  // -----------------------------------------------------------------------------
  DataContainerArray::Pointer createDataStructure()
  {
    // SIMPL::Rgb color;

    DataContainerArray::Pointer dca = DataContainerArray::New();
    DataContainer::Pointer dc = DataContainer::New(QString::fromStdString(k_DataContainerName));
    dca->addOrReplaceDataContainer(dc);
    AttributeMatrix::Pointer am = AttributeMatrix::New(k_TupleDims, QString::fromStdString(k_AttributeMatrixName), AttributeMatrix::Type::Cell);
    dc->addOrReplaceAttributeMatrix(am);
    FloatArrayType::comp_dims_type compDims = {3};

    FloatArrayType::Pointer vecData = FloatArrayType::CreateArray(k_ImageDim * k_ImageDim, compDims, QString::fromStdString(k_VectorsName), true);
    am->addOrReplaceAttributeArray(vecData);

    BoolArrayType::Pointer mask = BoolArrayType::CreateArray(k_ImageDim * k_ImageDim, QString::fromStdString(k_MaskName), true);
    mask->initializeWithValue(true);
    am->addOrReplaceAttributeArray(mask);

    int32_t imageDim = static_cast<int32_t>(k_ImageDim);

    double xInc = 1.0f / static_cast<double>(imageDim);
    double yInc = 1.0f / static_cast<double>(imageDim);
    double rad = 1.0f;

    double x = 0.0f;
    double y = 0.0f;
    double a = 0.0f;
    double b = 0.0f;
    double c = 0.0f;

    double val = 0.0f;
    double denom = 0.0f;
    size_t idx = 0;
    size_t yScanLineIndex = 0; // We use this to control where the data is drawn. Otherwise the image will come out flipped vertically
    // Loop over every pixel in the image and project up to the sphere to get the angle and then figure out the RGB from there.

    for(int32_t yIndex = 0; yIndex < imageDim; ++yIndex)
    {

      for(int32_t xIndex = 0; xIndex < imageDim; ++xIndex)
      {
        std::array<float, 3> normVec = {0.0F, 0.0F, 0.0F};
        idx = (imageDim * yScanLineIndex) + xIndex;

        x = -1.0f + 2.0f * xIndex * xInc;
        y = -1.0f + 2.0f * yIndex * yInc;

        double sumSquares = (x * x) + (y * y);
        if(sumSquares > 1.0) // Outside unit circle
        {
          normVec = {0.0F, 0.0F, 0.0F};
          mask->setValue(idx, false);
        }
        else if(sumSquares > (rad - 2 * xInc) && sumSquares < (rad + 2 * xInc)) // Black Border line
        {
          normVec = {0.0F, 0.0F, 0.0F};
          mask->setValue(idx, false);
        }
        else
        {
          a = (x * x + y * y + 1);
          b = (2 * x * x + 2 * y * y);
          c = (x * x + y * y - 1);

          val = (-b + std::sqrt(b * b - 4.0 * a * c)) / (2.0 * a);
          normVec[0] = (1 + val) * x;
          normVec[1] = (1 + val) * y;
          normVec[2] = val;
          denom = (normVec[0] * normVec[0]) + (normVec[1] * normVec[1]) + (normVec[2] * normVec[2]);
          denom = std::sqrt(denom);
          normVec[0] = normVec[0] / denom;
          normVec[1] = normVec[1] / denom;
          normVec[2] = normVec[2] / denom;
        }
        vecData->setTuple(idx, normVec.data());
      }
      yScanLineIndex++;
    }

    return dca;
  }

  // -----------------------------------------------------------------------------
  void Test()
  {

    DataContainerArray::Pointer dca = createDataStructure();

    Observer obs;

    GenerateVectorColors::Pointer vecColorFilter = GenerateVectorColors::New();
    vecColorFilter->connect(vecColorFilter.get(), SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), &obs, SLOT(processPipelineMessage(const AbstractMessage::Pointer&)));
    vecColorFilter->setDataContainerArray(dca);
    vecColorFilter->setVectorsArrayPath({S2Q(k_DataContainerName), S2Q(k_AttributeMatrixName), S2Q(k_VectorsName)});
    vecColorFilter->setGoodVoxelsArrayPath({S2Q(k_DataContainerName), S2Q(k_AttributeMatrixName), S2Q(k_MaskName)});

    vecColorFilter->setUseGoodVoxels(true);
    vecColorFilter->setCellVectorColorsArrayName(S2Q(k_ColorsName));
    vecColorFilter->preflight();
    int32_t error = vecColorFilter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(error, 0);

    dca = createDataStructure();
    vecColorFilter->setDataContainerArray(dca);
    vecColorFilter->execute();
    error = vecColorFilter->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(error, 0);

    UInt8ArrayType::Pointer rgbPtr = dca->getDataContainer(S2Q(k_DataContainerName))->getAttributeMatrix(S2Q(k_AttributeMatrixName))->getAttributeArrayAs<UInt8ArrayType>(S2Q(k_ColorsName));

    uint16_t smpPerPix = 3;
    std::pair<int32_t, std::string> retValue = WriteColorImage(k_TiffFilepath, static_cast<int32_t>(k_ImageDim), static_cast<int32_t>(k_ImageDim), smpPerPix, rgbPtr->data());
    DREAM3D_REQUIRE_EQUAL(retValue.first, 0)
  }

  /**
   * @brief
   */
  void operator()()
  {
    int err = EXIT_SUCCESS;
    std::cout << "<===== Start " << getNameOfClass().toStdString() << std::endl;

    DREAM3D_REGISTER_TEST(Test())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

private:
  GenerateVectorColorsTest(const GenerateVectorColorsTest&); // Copy Constructor Not Implemented
  void operator=(const GenerateVectorColorsTest&);           // Move assignment Not Implemented

  // -----------------------------------------------------------------------------
  const uint16_t PHOTOMETRIC_MINISBLACK = 0x0001;
  const uint16_t PHOTOMETRIC_RGB = 0x0002;

  struct TIFTAG
  {
    int16_t tagId = 0;      // The tag identifier
    int16_t dataType = 0;   // The scalar type of the data items
    int32_t dataCount = 0;  // The number of items in the tag data
    int32_t dataOffset = 0; // The byte offset to the data items

    friend std::ostream& operator<<(std::ostream& out, const TIFTAG& tag)
    {
      out.write(reinterpret_cast<const char*>(&tag.tagId), sizeof(tag.tagId));
      out.write(reinterpret_cast<const char*>(&tag.dataType), sizeof(tag.dataType));
      out.write(reinterpret_cast<const char*>(&tag.dataCount), sizeof(tag.dataCount));
      out.write(reinterpret_cast<const char*>(&tag.dataOffset), sizeof(tag.dataOffset));

      return out;
    }
  };

  enum class Endianess
  {
    Little = 0,
    Big
  };

  Endianess checkEndianess()
  {
    constexpr uint32_t i = 0x01020304;
    const std::byte* u8 = reinterpret_cast<const std::byte*>(&i);

    return u8[0] == std::byte{0x01} ? Endianess::Big : Endianess::Little;
  }

  // -----------------------------------------------------------------------------
  std::pair<int32_t, std::string> WriteColorImage(const std::string& filepath, int32_t width, int32_t height, uint16_t samplesPerPixel, const uint8_t* data)
  {
    // Check for Endianess of the system and write the appropriate magic number according to the tiff spec
    std::array<char, 4> magicNumber = {0x49, 0x49, 0x2A, 0x00};

    if(checkEndianess() == Endianess::Big)
    {
      magicNumber = {0x4D, 0x4D, 0x00, 0x2A};
    }

    // open file and write header
    std::ofstream outputFile(filepath, std::ios::binary);
    if(!outputFile.is_open())
    {
      return {-1, "Could not open output file for writing"};
    }

    outputFile.write(magicNumber.data(), magicNumber.size());
    // Generate the offset into the Image File Directory (ifd) which we are going to write first
    constexpr uint32_t ifd_Offset = 8;
    outputFile.write(reinterpret_cast<const char*>(&ifd_Offset), sizeof(ifd_Offset));

    std::vector<TIFTAG> tags;
    tags.push_back(TIFTAG{0x00FE, 0x0004, 1, 0x00000000});                       // NewSubfileType
    tags.push_back(TIFTAG{0x0100, 0x0004, 1, width});                            // ImageWidth
    tags.push_back(TIFTAG{0x0101, 0x0004, 1, height});                           // ImageLength
    tags.push_back(TIFTAG{0x0102, 0x0003, 1, 8 * sizeof(char)});                 // BitsPerSample
    tags.push_back(TIFTAG{0x0103, 0x0003, 1, 0x0001});                           // Compression
    tags.push_back(TIFTAG{0x0106, 0x0003, 1, PHOTOMETRIC_RGB});                  // PhotometricInterpretation  // For SamplesPerPixel = 3 or 4 (RGB or RGBA)
    tags.push_back(TIFTAG{0x0112, 0x0003, 1, 1});                                // Orientation
    tags.push_back(TIFTAG{0x0115, 0x0003, 1, samplesPerPixel});                  // SamplesPerPixel
    tags.push_back(TIFTAG{0x0116, 0x0004, 1, height});                           // RowsPerStrip
    tags.push_back(TIFTAG{0x0117, 0x0004, 1, width * height * samplesPerPixel}); // StripByteCounts
    // TIFTAG XResolution;
    // TIFTAG YResolution;
    // TIFTAG ResolutionUnit;
    tags.push_back(TIFTAG{0x011c, 0x0003, 1, 0x0001}); // PlanarConfiguration

    // Now compute the offset to the image data so that we can put that into the tag.
    // THESE NEXT 2 LINES MUST BE THE LAST TAG TO BE PUSHED BACK INTO THE VECTOR OR THE MATH WILL BE WRONG
    int32_t imageDataOffset = static_cast<int32_t>(8 + ((tags.size() + 1) * 12) + 6); // Header + tags + IDF Tag entry count and Next IFD Offset
    tags.push_back(TIFTAG{0x0111, 0x0004, 1, imageDataOffset});                       // StripOffsets

    // Write the number of tags to the IFD section
    uint16_t numEntries = static_cast<uint16_t>(tags.size());
    outputFile.write(reinterpret_cast<const char*>(&numEntries), sizeof(numEntries));
    // write the tags to the file.
    for(const auto& tag : tags)
    {
      outputFile << tag;
    }
    // Write the "Next Tag Offset"
    constexpr uint32_t nextOffset = 0;
    outputFile.write(reinterpret_cast<const char*>(&nextOffset), sizeof(nextOffset));

    // Now write the actual image data
    int32_t imageByteCount = width * height * samplesPerPixel;
    outputFile.write(reinterpret_cast<const char*>(data), imageByteCount);

    // and we are done.
    return {0, "No Error"};
  }
};
