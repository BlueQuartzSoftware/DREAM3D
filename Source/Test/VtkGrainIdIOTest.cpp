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
#include "DREAM3DLib/Common/DataArray.hpp"
#include "DREAM3DLib/IO/VtkGrainIdWriter.h"
#include "DREAM3DLib/IO/VtkGrainIdReader.h"

#include "UnitTestSupport.hpp"
#include "TestFileLocations.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  MXADir::remove(UnitTest::VtkGrainIdIOTest::TestFile);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestVtkGrainIdWriter()
{
  int size = UnitTest::VtkGrainIdIOTest::XSize * UnitTest::VtkGrainIdIOTest::YSize * UnitTest::VtkGrainIdIOTest::ZSize;
  DataArray<int>::Pointer grainIds = DataArray<int>::CreateArray(size);
  for (int i = 0; i < size; ++i)
  {
    grainIds->SetValue(i, i + UnitTest::VtkGrainIdIOTest::Offset);
  }
  size_t nx = UnitTest::VtkGrainIdIOTest::XSize;
  size_t ny = UnitTest::VtkGrainIdIOTest::YSize;
  size_t nz = UnitTest::VtkGrainIdIOTest::ZSize;

  DataContainer::Pointer m = DataContainer::New();
  m->addVoxelData(DREAM3D::VoxelData::GrainIds, grainIds);
  m->setDimensions(nx, ny, nz);
  m->setResolution(2.0f, 3.0f, 4.0f);
  m->setOrigin(1.0f, 2.0f, 3.0f);

  VtkGrainIdWriter::Pointer writer = VtkGrainIdWriter::New();
  writer->setDataContainer(m.get());
  writer->setFileName(UnitTest::VtkGrainIdIOTest::TestFile);
  writer->execute();
  int err = writer->getErrorCondition();
  DREAM3D_REQUIRE_EQUAL(err, 0);
  return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestVtkGrainIdReader()
{

  VtkGrainIdReader::Pointer reader = VtkGrainIdReader::New();
  reader->setFileName(UnitTest::VtkGrainIdIOTest::TestFile);
  size_t nx = 0;
  size_t ny = 0;
  size_t nz = 0;

  DataContainer::Pointer m = DataContainer::New();
  reader->setDataContainer(m.get());
  reader->execute( );
  int err = reader->getErrorCondition();
  if (err < 0) {
    std::cout << reader->getErrorMessage() << std::endl;
  }
  DREAM3D_REQUIRE_EQUAL(err, 0);
  m->getDimensions(nx, ny, nz);

  IDataArray::Pointer mdata = reader->getDataContainer()->getVoxelData(DREAM3D::VoxelData::GrainIds);


  DREAM3D_REQUIRE_EQUAL(nx, UnitTest::VtkGrainIdIOTest::XSize);
  DREAM3D_REQUIRE_EQUAL(ny, UnitTest::VtkGrainIdIOTest::YSize);
  DREAM3D_REQUIRE_EQUAL(nz, UnitTest::VtkGrainIdIOTest::ZSize);
  int size = UnitTest::VtkGrainIdIOTest::XSize * UnitTest::VtkGrainIdIOTest::YSize * UnitTest::VtkGrainIdIOTest::ZSize;
  int32_t* data = Int32ArrayType::SafeReinterpretCast<IDataArray*, Int32ArrayType*, int32_t*>(mdata.get());

  for (int i = 0; i < size; ++i)
  {
    int32_t file_value = data[i];
    int32_t memory_value = i+UnitTest::VtkGrainIdIOTest::Offset;
    DREAM3D_REQUIRE_EQUAL( memory_value, file_value );
  }


  return EXIT_SUCCESS;
}


// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char **argv) {
  int err = EXIT_SUCCESS;

  DREAM3D_REGISTER_TEST( TestVtkGrainIdWriter() );
  DREAM3D_REGISTER_TEST( TestVtkGrainIdReader() );

  DREAM3D_REGISTER_TEST( RemoveTestFiles() );
  PRINT_TEST_SUMMARY();
  return err;
}

