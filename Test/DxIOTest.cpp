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

#include <QtCore/QDir>
#include <QtCore/QFile>

#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/DataArray.hpp"
#include "DREAM3DLib/Common/FilterPipeline.h"
#include "DREAM3DLib/IOFilters/DxWriter.h"
#include "DREAM3DLib/IOFilters/DxReader.h"

#include "UnitTestSupport.hpp"
#include "TestFileLocations.h"


class GenerateGrainIds : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(GenerateGrainIds)
    DREAM3D_STATIC_NEW_MACRO(GenerateGrainIds)
    DREAM3D_TYPE_MACRO_SUPER(GenerateGrainIds, AbstractFilter)

    //------ Created Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(GrainIdsArrayName)

    virtual ~GenerateGrainIds(){}

    virtual const std::string getGroupName()
    {
      return "UnitTest";
    }
    virtual const std::string getHumanLabel()
    {
      return "Generate Grain Ids";
    }
    virtual void execute()
    {
      setErrorCondition(0);
      VoxelDataContainer* m = getVoxelDataContainer();
      if(NULL == m)
      {
        setErrorCondition(-1);
        std::stringstream ss;
        ss << " DataContainer was NULL";
        addErrorMessage(getHumanLabel(), ss.str(), -1);
        return;
      }
      int size = UnitTest::DxIOTest::XSize * UnitTest::DxIOTest::YSize * UnitTest::DxIOTest::ZSize;

      int64_t nx = UnitTest::DxIOTest::XSize;
      int64_t ny = UnitTest::DxIOTest::YSize;
      int64_t nz = UnitTest::DxIOTest::ZSize;
      m->setDimensions(nx, ny, nz);

      int64_t totalPoints = m->getTotalPoints();
      dataCheck(false, totalPoints, m->getNumFieldTuples(), m->getNumEnsembleTuples());
      // Set the default data into the GrainIds
      for (int i = 0; i < size; ++i)
      {
        m_GrainIds[i] = i + UnitTest::DxIOTest::Offset;
      }

    }
    virtual void preflight()
    {
      dataCheck(true, 1, 1, 1);
    }

  protected:
    GenerateGrainIds() :
        AbstractFilter(),
        m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
        m_GrainIds(NULL)
    {
    }

  private:
    int32_t* m_GrainIds;

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
    {
      setErrorCondition(0);
      std::stringstream ss;
      VoxelDataContainer* m = getVoxelDataContainer();
      CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, int32_t, Int32ArrayType, 0, voxels, 1)
    }

    GenerateGrainIds(const GenerateGrainIds&); // Copy Constructor Not Implemented
    void operator=(const GenerateGrainIds&); // Operator '=' Not Implemented
};



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  QFile::remove(UnitTest::DxIOTest::TestFile);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestDxWriter()
{
  FilterPipeline::Pointer pipeline = FilterPipeline::New();

  // This should FAIL because there are no GrainIds anywhere to write. It should
  // fail in the preflight
  DxWriter::Pointer writer = DxWriter::New();
  writer->setOutputFile(UnitTest::DxIOTest::TestFile);
  pipeline->pushBack(writer);
  int err = pipeline->preflightPipeline();
  DREAM3D_REQUIRE(err < 0);
  pipeline->execute();
  err = pipeline->getErrorCondition();
  DREAM3D_REQUIRE(err < 0);

  // Now create some GrainIds and lets setup a real pipeline that should work
  pipeline->clear(); // Remove any filters from the pipeline first


  GenerateGrainIds::Pointer generateGrainIds = GenerateGrainIds::New();
  pipeline->pushBack(generateGrainIds);

  writer = DxWriter::New();
  writer->setOutputFile(UnitTest::DxIOTest::TestFile);
  pipeline->pushBack(writer);
  err = pipeline->preflightPipeline();
  DREAM3D_REQUIRE_EQUAL(err, 0);
  pipeline->execute();
  err = pipeline->getErrorCondition();
  DREAM3D_REQUIRE_EQUAL(err, 0);

  return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestDxReader()
{

  DxReader::Pointer reader = DxReader::New();
  reader->setInputFile(UnitTest::DxIOTest::TestFile);
  size_t nx = 0;
  size_t ny = 0;
  size_t nz = 0;

  VoxelDataContainer::Pointer m = VoxelDataContainer::New();
  reader->setVoxelDataContainer(m.get());
  reader->preflight();
  int err = reader->getErrorCondition();
  DREAM3D_REQUIRE(err >= 0);

  m->clearCellData(); // We MUST clear out the dummy data that was put in the Cell Data of the Voxel Data container
  // Now execute the filter
  reader->execute( );
  err = reader->getErrorCondition();
  m->getDimensions(nx, ny, nz);

  IDataArray::Pointer mdata = reader->getVoxelDataContainer()->getCellData(DREAM3D::CellData::GrainIds);

  DREAM3D_REQUIRE_EQUAL(err, 0);
  DREAM3D_REQUIRE_EQUAL(nx, UnitTest::DxIOTest::XSize);
  DREAM3D_REQUIRE_EQUAL(ny, UnitTest::DxIOTest::YSize);
  DREAM3D_REQUIRE_EQUAL(nz, UnitTest::DxIOTest::ZSize);
  int size = UnitTest::DxIOTest::XSize * UnitTest::DxIOTest::YSize * UnitTest::DxIOTest::ZSize;
  int32_t* data = Int32ArrayType::SafeReinterpretCast<IDataArray*, Int32ArrayType*, int32_t*>(mdata.get());

  for (int i = 0; i < size; ++i)
  {
    int32_t file_value = data[i];
    int32_t memory_value = i+UnitTest::DxIOTest::Offset;
    DREAM3D_REQUIRE_EQUAL( memory_value, file_value );
  }


  return EXIT_SUCCESS;
}


// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char **argv) {
  int err = EXIT_SUCCESS;

  DREAM3D_REGISTER_TEST( TestDxWriter() )
  DREAM3D_REGISTER_TEST( TestDxReader() )

  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
  PRINT_TEST_SUMMARY();
  return err;
}

