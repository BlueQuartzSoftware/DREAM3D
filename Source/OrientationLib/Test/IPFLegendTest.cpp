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

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtGui/QImage>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Utilities/UnitTestSupport.hpp"

#include "OrientationLib.h"
#include "OrientationLib/SpaceGroupOps/CubicLowOps.h"
#include "OrientationLib/SpaceGroupOps/CubicOps.h"
#include "OrientationLib/SpaceGroupOps/HexagonalLowOps.h"
#include "OrientationLib/SpaceGroupOps/HexagonalOps.h"
#include "OrientationLib/SpaceGroupOps/MonoclinicOps.h"
#include "OrientationLib/SpaceGroupOps/OrthoRhombicOps.h"
#include "OrientationLib/SpaceGroupOps/TetragonalLowOps.h"
#include "OrientationLib/SpaceGroupOps/TetragonalOps.h"
#include "OrientationLib/SpaceGroupOps/TriclinicOps.h"
#include "OrientationLib/SpaceGroupOps/TrigonalLowOps.h"
#include "OrientationLib/SpaceGroupOps/TrigonalOps.h"

#include "OrientationLibTestFileLocations.h"


#define IMAGE_WIDTH 512
#define IMAGE_HEIGHT 512

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int getImageSize() { return IMAGE_HEIGHT; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  // QFile::remove();
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SaveImage(UInt8ArrayType::Pointer rgbaImage, const QString outputFile)
{
  QRgb* rgba = reinterpret_cast<QRgb*>(rgbaImage->getPointer(0));

  QImage image(getImageSize(), getImageSize(), QImage::Format_ARGB32_Premultiplied);

  int32_t xDim = getImageSize();
  int32_t yDim = getImageSize();
  size_t idx = 0;

  for (int32_t y = 0; y < yDim; ++y)
  {
    for (int32_t x = 0; x < xDim; ++x)
    {
      idx = (y * xDim) + x;
      image.setPixel(x, y, rgba[idx]);
    }
  }

  QFileInfo fi(outputFile);
  QDir parent(fi.absolutePath());
  if (parent.exists() == false)
  {
    parent.mkpath(fi.absolutePath());
  }

  bool saved = image.save(outputFile);
  DREAM3D_REQUIRE(saved == true)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<class SpaceGroupOpsType>
void TestIPFLegend(const QString outputFile)
{
  SpaceGroupOpsType ops;
  UInt8ArrayType::Pointer image = ops.generateIPFTriangleLegend(IMAGE_WIDTH);

  SaveImage(image, outputFile);
}

// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  int err = EXIT_SUCCESS;

  //DREAM3D_REGISTER_TEST( TestIPFLegend<CubicLowOps>(UnitTest::IPFLegendTest::CubicLowFile ) )
  DREAM3D_REGISTER_TEST( TestIPFLegend<CubicOps>(UnitTest::IPFLegendTest::CubicHighFile ) )
  DREAM3D_REGISTER_TEST( TestIPFLegend<HexagonalLowOps>(UnitTest::IPFLegendTest::HexagonalLowFile ) )
  DREAM3D_REGISTER_TEST( TestIPFLegend<HexagonalOps>(UnitTest::IPFLegendTest::HexagonalHighFile ) )
  DREAM3D_REGISTER_TEST( TestIPFLegend<MonoclinicOps>(UnitTest::IPFLegendTest::MonoclinicFile ) )
  DREAM3D_REGISTER_TEST( TestIPFLegend<OrthoRhombicOps>(UnitTest::IPFLegendTest::OrthorhombicFile ) )
  DREAM3D_REGISTER_TEST( TestIPFLegend<TetragonalLowOps>(UnitTest::IPFLegendTest::TetragonalLowFile ) )
  DREAM3D_REGISTER_TEST( TestIPFLegend<TetragonalOps>(UnitTest::IPFLegendTest::TetragonalHighFile ) )
  DREAM3D_REGISTER_TEST( TestIPFLegend<TriclinicOps>(UnitTest::IPFLegendTest::TriclinicFile ) )
  DREAM3D_REGISTER_TEST( TestIPFLegend<TrigonalLowOps>(UnitTest::IPFLegendTest::TrignonalLowFile ) )
  DREAM3D_REGISTER_TEST( TestIPFLegend<TrigonalOps>(UnitTest::IPFLegendTest::TrignonalHighFile ) )



  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
  PRINT_TEST_SUMMARY();

  return err;
}
