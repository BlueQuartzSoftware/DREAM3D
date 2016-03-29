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




#include <stdlib.h>

#include <string>

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/Utilities/MXADir.h"
#include "EbsdLib/H5EbsdVolumeReader.h"
#include "EbsdLib/TSL/H5AngVolumeReader.h"
#include "EbsdLib/HKL/H5CtfVolumeReader.h"
#include "EbsdLib/HKL/H5CtfReader.h"


#include "EbsdImport/EbsdImport.h"

#include "UnitTestSupport.hpp"
#include "EbsdTestFileLocation.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  EbsdDir::remove(EbsdImportTest::H5EbsdOutputFile);
  EbsdDir::remove(UnitTest::CtfReaderTest::H5EbsdOutputFile);
#endif
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RunTslImporter(const QString file, bool rotateSlice, bool reorderArray, bool alignEulers)
{
  EbsdImport::Pointer m_EbsdImport = EbsdImport::New();
  m_EbsdImport->setOutputFile(file);
  m_EbsdImport->setZStartIndex(1);
  m_EbsdImport->setZEndIndex(3);
  m_EbsdImport->setZResolution(0.25f);

  m_EbsdImport->setRefFrameZDir( Ebsd::LowtoHigh );
  m_EbsdImport->setRotateSlice(rotateSlice);
  m_EbsdImport->setReorderArray(reorderArray);
  m_EbsdImport->setAlignEulers(alignEulers);

  // Now generate all the file names in the "Low to High" order because that is what the importer is expecting
  std::vector<QString> fileList;
  fileList.push_back(EbsdImportTest::FileDir + EbsdImportTest::TestFile1);
  fileList.push_back(EbsdImportTest::FileDir + EbsdImportTest::TestFile2);
  fileList.push_back(EbsdImportTest::FileDir + EbsdImportTest::TestFile3);

  m_EbsdImport->setEbsdFileList(fileList);
  m_EbsdImport->execute();
  DREAM3D_REQUIRE(m_EbsdImport->getErrorCondition() >= 0)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestTslImporter(bool rotateSlice, bool reorderArray, bool alignEulers)
{
  RunTslImporter(EbsdImportTest::H5EbsdOutputFile, rotateSlice, reorderArray, alignEulers);
  H5EbsdVolumeReader::Pointer reader = H5AngVolumeReader::New();
  reader->setFileName(EbsdImportTest::H5EbsdOutputFile);
  DREAM3D_REQUIRE(reader->getRotateSlice() == rotateSlice)
  DREAM3D_REQUIRE(reader->getReorderArray() == reorderArray)
  DREAM3D_REQUIRE(reader->getAlignEulers() == alignEulers)
  RemoveTestFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TSLImport()
{
  TestTslImporter(false, true, false);
  TestTslImporter(true, false, false);
  TestTslImporter(true, true, false);
  TestTslImporter(false, false, false);
  TestTslImporter(false, true, true);
  TestTslImporter(true, false, true);
  TestTslImporter(true, true, true);
  TestTslImporter(false, false, true);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RunHklImporter(const QString outfile, bool rotateSlice, bool reorderArray, bool alignEulers)
{
  EbsdImport::Pointer m_EbsdImport = EbsdImport::New();
  m_EbsdImport->setOutputFile(outfile);
  m_EbsdImport->setZStartIndex(1);
  m_EbsdImport->setZEndIndex(2);
  m_EbsdImport->setZResolution(0.25f);

  m_EbsdImport->setRefFrameZDir( Ebsd::LowtoHigh );
  m_EbsdImport->setRotateSlice(rotateSlice);
  m_EbsdImport->setReorderArray(reorderArray);
  m_EbsdImport->setAlignEulers(alignEulers);

  // Now generate all the file names in the "Low to High" order because that is what the importer is expecting
  std::vector<QString> fileList;
  fileList.push_back(UnitTest::CtfReaderTest::FileDir + UnitTest::CtfReaderTest::USInputFile1);
  fileList.push_back(UnitTest::CtfReaderTest::FileDir + UnitTest::CtfReaderTest::USInputFile2);

  m_EbsdImport->setEbsdFileList(fileList);
  m_EbsdImport->execute();
  DREAM3D_REQUIRE(m_EbsdImport->getErrorCondition() >= 0)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RunHklEuropeanImporter(const QString outfile, bool rotateSlice, bool reorderArray, bool alignEulers)
{
  EbsdImport::Pointer m_EbsdImport = EbsdImport::New();
  m_EbsdImport->setOutputFile(outfile);
  m_EbsdImport->setZStartIndex(1);
  m_EbsdImport->setZEndIndex(2);
  m_EbsdImport->setZResolution(0.25f);

  m_EbsdImport->setRefFrameZDir( Ebsd::LowtoHigh );
  m_EbsdImport->setRotateSlice(rotateSlice);
  m_EbsdImport->setReorderArray(reorderArray);
  m_EbsdImport->setAlignEulers(alignEulers);

  // Now generate all the file names in the "Low to High" order because that is what the importer is expecting
  std::vector<QString> fileList;
  fileList.push_back(UnitTest::CtfReaderTest::FileDir + UnitTest::CtfReaderTest::EuropeanInputFile1);
  fileList.push_back(UnitTest::CtfReaderTest::FileDir + UnitTest::CtfReaderTest::EuropeanInputFile2);

  m_EbsdImport->setEbsdFileList(fileList);
  m_EbsdImport->execute();
  DREAM3D_REQUIRE(m_EbsdImport->getErrorCondition() >= 0)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestHklImporter(bool rotateSlice, bool reorderArray, bool alignEulers)
{
  int err = 0;
  RunTslImporter(UnitTest::CtfReaderTest::H5EbsdOutputFile, rotateSlice, reorderArray, alignEulers);
  H5EbsdVolumeReader::Pointer reader = H5CtfVolumeReader::New();
  reader->setFileName(UnitTest::CtfReaderTest::H5EbsdOutputFile);
  DREAM3D_REQUIRE(reader->getRotateSlice() == rotateSlice)
  DREAM3D_REQUIRE(reader->getReorderArray() == reorderArray)
  DREAM3D_REQUIRE(reader->getAlignEulers() == alignEulers)
  RemoveTestFiles();

  RunHklEuropeanImporter(UnitTest::CtfReaderTest::H5EbsdOutputFile, rotateSlice, reorderArray, alignEulers);
  reader = H5CtfVolumeReader::New();
  reader->setFileName(UnitTest::CtfReaderTest::H5EbsdOutputFile);
  DREAM3D_REQUIRE(reader->getRotateSlice() == rotateSlice)
  DREAM3D_REQUIRE(reader->getReorderArray() == reorderArray)
  DREAM3D_REQUIRE(reader->getAlignEulers() == alignEulers)

  H5CtfReader::Pointer sliceReader = H5CtfReader::New();
  sliceReader->setFileName(UnitTest::CtfReaderTest::H5EbsdOutputFile);
  sliceReader->setHDF5Path("/1");
  err = sliceReader->readHeaderOnly();
  DREAM3D_REQUIRE(err >= 0);
  float xstep = sliceReader->getXStep();
  DREAM3D_REQUIRE(xstep == 0.5f);
  float ystep = sliceReader->getYStep();
  DREAM3D_REQUIRE(ystep == 0.5f);

  RemoveTestFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HklImport()
{
  TestHklImporter(false, true, false);
}

// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( TSLImport() )
  DREAM3D_REGISTER_TEST( HklImport() )
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )

  PRINT_TEST_SUMMARY();
  return err;
}
