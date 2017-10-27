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

#include <string.h>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QtDebug>

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/TSL/AngReader.h"
#include "EbsdLib/TSL/H5OIMReader.h"
#include "EbsdLib/Test/EbsdLibTestFileLocations.h"

#include "UnitTestSupport.hpp"

class EdaxOIMReaderTest
{
public:
  EdaxOIMReaderTest()
  {
  }
  virtual ~EdaxOIMReaderTest()
  {
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
// QFile::remove(UnitTest::AngImportTest::H5EbsdOutputFile);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestH5OIMReader()
  {
    int err = 0;
    int err1 = 0;
    H5OIMReader::Pointer reader = H5OIMReader::New();
    reader->setFileName(UnitTest::AngImportTest::EdaxOIMH5File);
    QStringList names;
    err = reader->readScanNames(names);
    DREAM3D_REQUIRED(err, >=, 0)
    int count = names.size();
    DREAM3D_REQUIRED(count, ==, 1)

    reader->setHDF5Path("Scan 1");
    err1 = reader->readHeaderOnly();

    err = reader->getErrorCode();
    DREAM3D_REQUIRED(err, >=, 0)
    DREAM3D_REQUIRED(err1, >=, 0)

    int x = reader->getXDimension();
    DREAM3D_REQUIRED(x, ==, 186)
    int y = reader->getYDimension();
    DREAM3D_REQUIRED(y, ==, 151)

    reader->setReadPatternData(true);
    err = reader->readFile();
    float* f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::Phi1));
    DREAM3D_REQUIRE_VALID_POINTER(f1)
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::Phi));
    DREAM3D_REQUIRE_VALID_POINTER(f1)
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::Phi2));
    DREAM3D_REQUIRE_VALID_POINTER(f1)
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::ImageQuality));
    DREAM3D_REQUIRE_VALID_POINTER(f1)
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::ConfidenceIndex));
    DREAM3D_REQUIRE_VALID_POINTER(f1)
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::SEMSignal));
    DREAM3D_REQUIRE_VALID_POINTER(f1)
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::Fit));

    int* phasePtr = reinterpret_cast<int*>(reader->getPointerByName(Ebsd::Ang::PhaseData));
    DREAM3D_REQUIRE_VALID_POINTER(phasePtr)

    if(reader->getReadPatternData())
    {
      uint8_t* patterns = reader->getPatternData();
      DREAM3D_REQUIRE_VALID_POINTER(patterns)
      int patternDims[2] = {0, 0};
      reader->getPatternDims(patternDims);
      DREAM3D_REQUIRED(patternDims[0], ==, 60)
      DREAM3D_REQUIRED(patternDims[1], ==, 60)
    }
  }

  void operator()()
  {
    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(TestH5OIMReader())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }
};
