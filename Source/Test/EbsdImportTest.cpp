/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "EbsdTestFileLocation.h"

#include <stdlib.h>

#include <string>

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/Utilities/MXADir.h"
#include "EbsdLib/H5EbsdVolumeReader.h"
#include "EbsdLib/TSL/H5AngVolumeReader.h"
#include "UnitTestSupport.hpp"

#include "EbsdImport/EbsdImport.h"

#define REMOVE_TEST_FILES 1

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  EbsdDir::remove(EbsdImportTest::H5EbsdOutputFile);
#endif
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RunImporter(const std::string file, bool rotateSlice, bool reorderArray, bool alignEulers)
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
  std::vector<std::string> fileList;
  fileList.push_back(EbsdImportTest::FileDir + EbsdImportTest::TestFile1);
  fileList.push_back(EbsdImportTest::FileDir + EbsdImportTest::TestFile2);
  fileList.push_back(EbsdImportTest::FileDir + EbsdImportTest::TestFile3);

  m_EbsdImport->setEbsdFileList(fileList);
  m_EbsdImport->execute();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestImporter(bool rotateSlice, bool reorderArray, bool alignEulers)
{
  RunImporter(EbsdImportTest::H5EbsdOutputFile, rotateSlice, reorderArray, alignEulers);
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
void TestAll()
{
  TestImporter(false, true, false);
  TestImporter(true, false, false);
  TestImporter(true, true, false);
  TestImporter(false, false, false);
  TestImporter(false, true, true);
  TestImporter(true, false, true);
  TestImporter(true, true, true);
  TestImporter(false, false, true);
}

// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( TestAll() );
  DREAM3D_REGISTER_TEST( RemoveTestFiles() );

  PRINT_TEST_SUMMARY();
  return err;
}
