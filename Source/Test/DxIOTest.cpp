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

#include <stdlib.h>
#include <iostream>
#include <vector>

#include "MXA/Utilities/MXADir.h"

#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AIMArray.hpp"
#include "DREAM3DLib/IO/DxWriter.hpp"
#include "DREAM3DLib/IO/DxReader.h"

#include "UnitTestSupport.hpp"
#include "TestFileLocations.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  MXADir::remove(UnitTest::DxIOTest::TestFile);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestDxWriter()
{
  int size = UnitTest::DxIOTest::XSize * UnitTest::DxIOTest::YSize * UnitTest::DxIOTest::ZSize;
  AIMArray<int>::Pointer grainIds = AIMArray<int>::CreateArray(size);
  for (int i = 0; i < size; ++i)
  {
    grainIds->SetValue(i, i + UnitTest::DxIOTest::Offset);
  }
  int nx = UnitTest::DxIOTest::XSize;
  int ny = UnitTest::DxIOTest::YSize;
  int nz = UnitTest::DxIOTest::ZSize;


  DxWriter::Pointer writer = DxWriter::New();
  writer->setFileName(UnitTest::DxIOTest::TestFile);
  writer->setData(grainIds);
  writer->setDimensions(nx, ny, nz);
  int err = writer->writeFile();
  DREAM3D_REQUIRE_EQUAL(err, 0);
  return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestDxReader()
{

  DxReader::Pointer reader = DxReader::New();
  reader->setFileName(UnitTest::DxIOTest::TestFile);
  int nx = 0;
  int ny = 0;
  int nz = 0;

  int err = reader->readFile( );
  reader->getDimensions(nx, ny, nz);
  AIMArray<int>::Pointer data = reader->getData();
  DREAM3D_REQUIRE_EQUAL(err, 0);
  DREAM3D_REQUIRE_EQUAL(nx, UnitTest::DxIOTest::XSize);
  DREAM3D_REQUIRE_EQUAL(ny, UnitTest::DxIOTest::YSize);
  DREAM3D_REQUIRE_EQUAL(nz, UnitTest::DxIOTest::ZSize);
  int size = UnitTest::DxIOTest::XSize * UnitTest::DxIOTest::YSize * UnitTest::DxIOTest::ZSize;

  for (int i = 0; i < size; ++i)
  {
    DREAM3D_REQUIRE_EQUAL( (i+UnitTest::DxIOTest::Offset), data->GetValue(i) );
  }


  return EXIT_SUCCESS;
}


// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char **argv) {
  int err = EXIT_SUCCESS;

  DREAM3D_REGISTER_TEST( TestDxWriter() );
  DREAM3D_REGISTER_TEST( TestDxReader() );

  DREAM3D_REGISTER_TEST( RemoveTestFiles() );
  PRINT_TEST_SUMMARY();
  return err;
}


#if 0
#include <iostream>


#include "DREAM3DLib/Common/DataContainer.h"
#include "GrainGenerator/StructureReaders/DXStructureReader.h"
#include "DREAM3DLib/VTKUtils/VTKFileWriters.hpp"



int main(int argc, char **argv)
{

  GrainGeneratorFunc::Pointer r = GrainGeneratorFunc::New();

  DXStructureReader::Pointer reader = DXStructureReader::New();
  reader->setInputFileName(argv[1]);

  std::cout << "Reading DX File...." << std::endl;

  int err = reader->readStructure(r.get());
  if (err < 0)
  {
    std::cout << "Error Reading Structure." << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "Writing VTK File..." << std::endl;


  VtkScalarWriter* w0 =
      static_cast<VtkScalarWriter*>(new VoxelGrainIdScalarWriter<GrainGeneratorFunc>(r.get()));
  std::vector<VtkScalarWriter*> scalarsToWrite;
  scalarsToWrite.push_back(w0);


  VTKRectilinearGridFileWriter writer;
  writer.write<GrainGeneratorFunc>("/tmp/vtkfilewriterstest.vtk", r.get(), scalarsToWrite);

  std::cout << "Done." << std::endl;
  return EXIT_SUCCESS;
}

#endif

