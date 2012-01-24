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
#include "DREAM3DLib/Common/NeighborList.hpp"
#include "DREAM3DLib/GenericFilters/DataContainerReader.h"
#include "DREAM3DLib/GenericFilters/DataContainerWriter.h"

#include "UnitTestSupport.hpp"
#include "TestFileLocations.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  MXADir::remove(UnitTest::DataContainerIOTest::TestFile);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestDataContainerWriter()
{

  DataContainer::Pointer m = DataContainer::New();
  size_t nx = UnitTest::DataContainerIOTest::XSize;
  size_t ny = UnitTest::DataContainerIOTest::YSize;
  size_t nz = UnitTest::DataContainerIOTest::ZSize;
  m->setDimensions(nx, ny, nz);
  int size = nx * ny * nz;


  Int32ArrayType::Pointer grainIds = Int32ArrayType::CreateArray(size);
  for (int i = 0; i < size; ++i)
  {
    grainIds->SetValue(i, i + UnitTest::DataContainerIOTest::Offset);
  }
  grainIds->SetName(DREAM3D::VoxelData::GrainIds);
  m->addVoxelData(DREAM3D::VoxelData::GrainIds, grainIds);



  FloatArrayType::Pointer avgEuler = FloatArrayType::CreateArray(12);
  avgEuler->SetNumberOfComponents(3);
  for(size_t i = 0; i < 4; ++i)
  {
    avgEuler->SetComponent(i, 0, i*0.665f);
    avgEuler->SetComponent(i, 1, i*0.665f);
    avgEuler->SetComponent(i, 2, i*0.665f);
  }
  avgEuler->SetName(DREAM3D::FieldData::AxisEulerAngles);
  m->addFieldData(DREAM3D::FieldData::AxisEulerAngles, avgEuler);


  FloatArrayType::Pointer surfArea = FloatArrayType::CreateArray(4);
  for (int i = 0; i < 4; ++i)
  {
    surfArea->SetValue(i, i + 41.2f);
  }
  surfArea->SetName(DREAM3D::EnsembleData::TotalSurfaceArea);
  m->addEnsembleData(DREAM3D::EnsembleData::TotalSurfaceArea, surfArea);


  NeighborList<int>::Pointer neighborlistPtr = NeighborList<int>::New();
  neighborlistPtr->SetName(DREAM3D::FieldData::NeighborList);
  m->addFieldData(DREAM3D::FieldData::NeighborList, neighborlistPtr);

  for(int i = 0; i < 4; ++i)
  {
    for(int j = 0; j < i+4; ++j)
    {
      neighborlistPtr->addEntry(i, j*i+3);
    }
  }

  DataContainerWriter::Pointer writer = DataContainerWriter::New();
  writer->setDataContainer(m.get());
  writer->setOutputFile(UnitTest::DataContainerIOTest::TestFile);

  writer->execute();
  int err = writer->getErrorCondition();
  DREAM3D_REQUIRE_EQUAL(err, 0);
  return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestDataContainerReader()
{
  size_t nx = 0;
  size_t ny = 0;
  size_t nz = 0;

  DataContainer::Pointer m = DataContainer::New();

  DataContainerReader::Pointer reader = DataContainerReader::New();
  reader->setInputFile(UnitTest::DataContainerIOTest::TestFile);
  reader->setDataContainer(m.get());
  reader->execute( );
  int err = reader->getErrorCondition();
  m->getDimensions(nx, ny, nz);


  DREAM3D_REQUIRE_EQUAL(err, 0);
  DREAM3D_REQUIRE_EQUAL(nx, UnitTest::DataContainerIOTest::XSize);
  DREAM3D_REQUIRE_EQUAL(ny, UnitTest::DataContainerIOTest::YSize);
  DREAM3D_REQUIRE_EQUAL(nz, UnitTest::DataContainerIOTest::ZSize);
//  int size = UnitTest::DataContainerIOTest::XSize * UnitTest::DataContainerIOTest::YSize * UnitTest::DataContainerIOTest::ZSize;
//  int32_t* data = Int32ArrayType::SafeReinterpretCast<IDataArray*, Int32ArrayType*, int32_t*>(mdata.get());
//
//  for (int i = 0; i < size; ++i)
//  {
//    int32_t file_value = data[i];
//    int32_t memory_value = i+UnitTest::DataContainerIOTest::Offset;
//    DREAM3D_REQUIRE_EQUAL( memory_value, file_value );
//  }


  return EXIT_SUCCESS;
}


// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char **argv) {
  int err = EXIT_SUCCESS;

  DREAM3D_REGISTER_TEST( TestDataContainerWriter() );
  DREAM3D_REGISTER_TEST( TestDataContainerReader() );
//
//  DREAM3D_REGISTER_TEST( RemoveTestFiles() );
  PRINT_TEST_SUMMARY();
  return err;
}

