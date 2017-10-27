/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
*
* Redistribution and use in source and binary forms, with or without
* modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of
* its
* contributors may be used to endorse or promote products derived from this
* software
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

#include "EbsdLib/HKL/CtfReader.h"

#include "UnitTestSupport.hpp"

#include "EbsdLib/Test/EbsdLibTestFileLocations.h"

class CtfReaderTest
{
public:
  CtfReaderTest()
  {
  }
  virtual ~CtfReaderTest()
  {
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestCtfReader()
  {
    CtfReader reader;
    reader.setFileName(UnitTest::CtfReaderTest::EuropeanInputFile1);
    int err = reader.readFile();
    DREAM3D_REQUIRE(err >= 0);

    float xstep = reader.getXStep();
    DREAM3D_REQUIRE(xstep == 0.5f);
    float ystep = reader.getYStep();
    DREAM3D_REQUIRE(ystep == 0.5f);
    DREAM3D_REQUIRE(reader.getNumPhases() == 1);
    CtfPhase::Pointer phase = reader.getPhaseVector().at(0);
    QVector<float> latticeConstants = phase->getLatticeConstants();
    DREAM3D_REQUIRE(latticeConstants[0] >= 3.230f && latticeConstants[0] <= 3.232f)
    DREAM3D_REQUIRE(latticeConstants[1] >= 3.230f && latticeConstants[1] <= 3.232f)
    DREAM3D_REQUIRE(latticeConstants[2] >= 5.147f && latticeConstants[2] <= 5.149f)
    DREAM3D_REQUIRE(latticeConstants[3] >= 90.00f && latticeConstants[3] <= 90.00f)
    DREAM3D_REQUIRE(latticeConstants[3] >= 90.00f && latticeConstants[3] <= 90.00f)
    DREAM3D_REQUIRE(latticeConstants[5] >= 120.00f && latticeConstants[5] <= 120.00f)
    DREAM3D_REQUIRE(latticeConstants.size() == 6)

    float* euler1 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ctf::Euler1));
    DREAM3D_REQUIRE(euler1 != nullptr)
    DREAM3D_REQUIRE(euler1[1] == 103.85f)

    float* euler2 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ctf::Euler2));
    DREAM3D_REQUIRE(euler2 != nullptr)
    DREAM3D_REQUIRE(euler2[1] == 40.207f)

    float* euler3 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ctf::Euler3));
    DREAM3D_REQUIRE(euler3 != nullptr)
    DREAM3D_REQUIRE(euler3[1] == 29.394f)
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestMultiplePhases_European()
  {
    CtfReader reader;
    reader.setFileName(UnitTest::CtfReaderTest::EuropeanInputFile2);
    int err = reader.readFile();
    DREAM3D_REQUIRE(err >= 0);

    float xstep = reader.getXStep();
    DREAM3D_REQUIRE(xstep == 0.5f);
    float ystep = reader.getYStep();
    DREAM3D_REQUIRE(ystep == 0.5f);
    DREAM3D_REQUIRE(reader.getNumPhases() == 5);
    CtfPhase::Pointer phase = reader.getPhaseVector().at(0);
    QVector<float> latticeConstants = phase->getLatticeConstants();
    DREAM3D_REQUIRE(latticeConstants[0] >= 3.61f && latticeConstants[0] <= 3.61f)
    DREAM3D_REQUIRE(latticeConstants[1] >= 3.61f && latticeConstants[1] <= 3.61f)
    DREAM3D_REQUIRE(latticeConstants[2] >= 3.61f && latticeConstants[2] <= 3.61f)
    DREAM3D_REQUIRE(latticeConstants[3] >= 90.00f && latticeConstants[3] <= 90.00f)
    DREAM3D_REQUIRE(latticeConstants[4] >= 90.00f && latticeConstants[4] <= 90.00f)
    DREAM3D_REQUIRE(latticeConstants[5] >= 90.00f && latticeConstants[5] <= 90.00f)
    DREAM3D_REQUIRE(latticeConstants.size() == 6)

    float* euler1 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ctf::Euler1));
    DREAM3D_REQUIRE(euler1 != nullptr)
    DREAM3D_REQUIRE(euler1[1] == 5.673f)

    float* euler2 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ctf::Euler2));
    DREAM3D_REQUIRE(euler2 != nullptr)
    DREAM3D_REQUIRE(euler2[1] == 0.2549f)

    float* euler3 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ctf::Euler3));
    DREAM3D_REQUIRE(euler3 != nullptr)
    DREAM3D_REQUIRE(euler3[1] == 0.2423f)
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestMultiplePhases_US()
  {
    CtfReader reader;
    reader.setFileName(UnitTest::CtfReaderTest::USInputFile2);
    int err = reader.readFile();
    DREAM3D_REQUIRE(err >= 0);

    float xstep = reader.getXStep();
    DREAM3D_REQUIRE(xstep == 0.5f);
    float ystep = reader.getYStep();
    DREAM3D_REQUIRE(ystep == 0.5f);
    DREAM3D_REQUIRE(reader.getNumPhases() == 5);
    CtfPhase::Pointer phase = reader.getPhaseVector().at(0);
    QVector<float> latticeConstants = phase->getLatticeConstants();
    DREAM3D_REQUIRE(latticeConstants[0] >= 3.61f && latticeConstants[0] <= 3.61f)
    DREAM3D_REQUIRE(latticeConstants[1] >= 3.61f && latticeConstants[1] <= 3.61f)
    DREAM3D_REQUIRE(latticeConstants[2] >= 3.61f && latticeConstants[2] <= 3.61f)
    DREAM3D_REQUIRE(latticeConstants[3] >= 90.00f && latticeConstants[3] <= 90.00f)
    DREAM3D_REQUIRE(latticeConstants[4] >= 90.00f && latticeConstants[4] <= 90.00f)
    DREAM3D_REQUIRE(latticeConstants[5] >= 90.00f && latticeConstants[5] <= 90.00f)
    DREAM3D_REQUIRE(latticeConstants.size() == 6)

    float* euler1 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ctf::Euler1));
    DREAM3D_REQUIRE(euler1 != nullptr)
    DREAM3D_REQUIRE(euler1[1] == 103.85f)

    float* euler2 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ctf::Euler2));
    DREAM3D_REQUIRE(euler2 != nullptr)
    DREAM3D_REQUIRE(euler2[1] == 40.207f)

    float* euler3 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ctf::Euler3));
    DREAM3D_REQUIRE(euler3 != nullptr)
    DREAM3D_REQUIRE(euler3[1] == 29.394f)
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestCellCountToLarge()
  {
    CtfReader reader;
    reader.setFileName(UnitTest::CtfReaderTest::Corrupted_XCells);
    int err = reader.readFile();
    qDebug() << reader.getErrorMessage();
    DREAM3D_REQUIRE(err == -106);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestShortFile()
  {
    CtfReader reader;
    reader.setFileName(UnitTest::CtfReaderTest::ShortFile);
    int err = reader.readFile();
    DREAM3D_REQUIRE(err == -105);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestZeroXYCells()
  {
    CtfReader reader;
    reader.setFileName(UnitTest::CtfReaderTest::ZeroXYCells);
    int err = reader.readFile();
    DREAM3D_REQUIRE(err == -103);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestWriteCtfFile()
  {
    CtfReader reader;
    reader.setFileName(UnitTest::CtfReaderTest::USInputFile1);
    int err = reader.readFile();
    DREAM3D_REQUIRE(err == 0);

    QString header = reader.getOriginalHeader();

    float* phi2Ptr = reinterpret_cast<float*>(reader.getPointerByName("Euler3"));
    if(nullptr != phi2Ptr)
    {
      int total = reader.getXCells() * reader.getYCells() * reader.getZCells();
      for(int i = 0; i < total; i++)
      {
        phi2Ptr[i] = phi2Ptr[i] + 30.0f;
      }
    }

    QString filePath = QString("%1/%2").arg(UnitTest::TestTempDir).arg("CTF_WriteFile_test.ctf");
    reader.writeFile(filePath);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;
    DREAM3D_REGISTER_TEST(TestCtfReader())
    DREAM3D_REGISTER_TEST(TestMultiplePhases_European())
    DREAM3D_REGISTER_TEST(TestMultiplePhases_US())
    DREAM3D_REGISTER_TEST(TestCellCountToLarge())
    DREAM3D_REGISTER_TEST(TestShortFile())
    DREAM3D_REGISTER_TEST(TestZeroXYCells())
    DREAM3D_REGISTER_TEST(TestWriteCtfFile());
  }
};
