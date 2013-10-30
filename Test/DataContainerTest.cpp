/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QVector>

#include "DREAM3DLib/DREAM3DLib.h"

#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"
#include "DREAM3DLib/DataArrays/NeighborList.hpp"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/IOFilters/DataContainerReader.h"
#include "DREAM3DLib/IOFilters/DataContainerWriter.h"

#include "UnitTestSupport.hpp"

#include "TestFileLocations.h"

#define helper(a, b)\
  a##b


#define MAKE_ARRAY(m_msgType, name)\
  IDataArray::Pointer m_msgType##Ptr = DataArray<m_msgType>::CreateArray(5, name);\
  dataContainer->addCellData(name, m_msgType##Ptr);


#define TEST_DATA_CONTAINER(Type, DCType)\
  {  IDataArray::Pointer t_##Type = Type::CreateArray(5);\
    t_##Type->SetName( #Type );\
    m->add##DCType(#Type, t_##Type);\
    IDataArray::Pointer t = m->get##DCType(#Type);\
    DREAM3D_TEST_POINTER(ptr, !=, NULL);\
    t = m->removeCellFieldData(#Type);\
    DREAM3D_TEST_POINTER(ptr, !=, NULL);\
    t = m->get##DCType(#Type);\
    DREAM3D_REQUIRE_EQUAL(t.get(), NULL);\
  }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  QFile::remove(UnitTest::DataContainerIOTest::TestFile);
#endif
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestDataContainerWriter()
{

  VolumeDataContainer::Pointer m = VolumeDataContainer::New();
  m->setName(DREAM3D::HDF5::VolumeDataContainerName);
  DataContainerArray::Pointer dca = DataContainerArray::New();
  dca->pushBack(m);
  size_t nx = UnitTest::DataContainerIOTest::XSize;
  size_t ny = UnitTest::DataContainerIOTest::YSize;
  size_t nz = UnitTest::DataContainerIOTest::ZSize;
  m->setDimensions(nx, ny, nz);
  int size = nx * ny * nz;

  QDir dir(UnitTest::DataContainerIOTest::TestDir);
  dir.mkpath(".");

  Int32ArrayType::Pointer grainIds = Int32ArrayType::CreateArray(size, DREAM3D::CellData::GrainIds);
  for (int i = 0; i < size; ++i)
  {
    grainIds->SetValue(i, i + UnitTest::DataContainerIOTest::Offset);
  }
  m->addCellData(DREAM3D::CellData::GrainIds, grainIds);

  BoolArrayType::Pointer boolArray = BoolArrayType::CreateArray(size, DREAM3D::CellData::SurfaceVoxels);
  for (int i = 0; i < size; ++i)
  {
    boolArray->SetValue(i, i + UnitTest::DataContainerIOTest::Offset);
  }
  m->addCellData(DREAM3D::CellData::SurfaceVoxels, boolArray);

  QVector<int> dims(1, 3);
  FloatArrayType::Pointer avgEuler = FloatArrayType::CreateArray(4, dims, DREAM3D::FieldData::AxisEulerAngles);
  for(size_t i = 0; i < 4; ++i)
  {
    avgEuler->SetComponent(i, 0, i * 0.665f);
    avgEuler->SetComponent(i, 1, i * 0.665f);
    avgEuler->SetComponent(i, 2, i * 0.665f);
  }
  m->addCellFieldData(DREAM3D::FieldData::AxisEulerAngles, avgEuler);


  FloatArrayType::Pointer surfArea = FloatArrayType::CreateArray(4, DREAM3D::EnsembleData::TotalSurfaceAreas);
  for (int i = 0; i < 4; ++i)
  {
    surfArea->SetValue(i, i + 41.2f);
  }
  m->addCellEnsembleData(DREAM3D::EnsembleData::TotalSurfaceAreas, surfArea);


  NeighborList<int>::Pointer neighborlistPtr = NeighborList<int>::New();
  neighborlistPtr->SetName(DREAM3D::FieldData::NeighborList);
  neighborlistPtr->setNumNeighborsArrayName(DREAM3D::FieldData::NumNeighbors);
  m->addCellFieldData(DREAM3D::FieldData::NeighborList, neighborlistPtr);

  for(int i = 0; i < 4; ++i)
  {
    for(int j = 0; j < i + 4; ++j)
    {
      neighborlistPtr->addEntry(i, j * i + 3);
    }
  }

  DataContainerWriter::Pointer writer = DataContainerWriter::New();
  writer->setDataContainerArray(dca);
  writer->setOutputFile(UnitTest::DataContainerIOTest::TestFile);
  writer->execute();
  int err = writer->getErrorCondition();
  writer = DataContainerWriter::NullPointer();
  DREAM3D_REQUIRE_EQUAL(err, 0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestDataContainerReader()
{
  size_t nx = 0;
  size_t ny = 0;
  size_t nz = 0;


  VolumeDataContainer::Pointer m = VolumeDataContainer::New();
  m->setName(DREAM3D::HDF5::VolumeDataContainerName);
  DataContainerArray::Pointer dca = DataContainerArray::New();
  dca->pushBack(m);

  DataContainerReader::Pointer reader = DataContainerReader::New();
  reader->setInputFile(UnitTest::DataContainerIOTest::TestFile);
  reader->setDataContainerArray(dca);
  reader->setReadVolumeData(true);
  reader->setReadSurfaceData(false);
  reader->setReadVertexData(false);
  reader->setReadEdgeData(false);
  reader->setReadAllArrays(true);
  reader->execute();
  int err = reader->getErrorCondition();
  DREAM3D_REQUIRE(err < 0)
  m = VolumeDataContainer::NullPointer();
  dca->clear(); // Remove all the data containers
  reader->execute();
  err = reader->getErrorCondition();
  DREAM3D_REQUIRE(err >= 0)


  VolumeDataContainer* mPtr = dca->getDataContainerAs<VolumeDataContainer>(DREAM3D::HDF5::VolumeDataContainerName);
  mPtr->getDimensions(nx, ny, nz);

  DREAM3D_REQUIRE_EQUAL(mPtr->getNumCellArrays(), 2);
  DREAM3D_REQUIRE_EQUAL(mPtr->getNumCellFieldArrays(), 3);
  DREAM3D_REQUIRE_EQUAL(mPtr->getNumCellEnsembleArrays(), 1);


  // Validate the NeighborList Data
  NeighborList<int32_t>* neighborlistPtr
    = NeighborList<int32_t>::SafeObjectDownCast<IDataArray*, NeighborList<int32_t>* >(mPtr->getCellFieldData(DREAM3D::FieldData::NeighborList).get());
  DREAM3D_REQUIRE_NE(NULL, neighborlistPtr);
  NeighborList<int32_t>::SharedVectorType vec;
  size_t nLists = neighborlistPtr->getNumberOfTuples();
  DREAM3D_REQUIRE_EQUAL(nLists, 4);
  for(size_t l = 0; l < nLists; ++l)
  {
    vec = neighborlistPtr->getList(l);
    for(int j = 0; j < (int)(l + 4); ++j)
    {
      DREAM3D_REQUIRE_EQUAL(vec->at(j), (int32_t)((j * l + 3)) );
    }

  }

  DREAM3D_REQUIRE_EQUAL(err, 0);
  DREAM3D_REQUIRE_EQUAL(nx, UnitTest::DataContainerIOTest::XSize);
  DREAM3D_REQUIRE_EQUAL(ny, UnitTest::DataContainerIOTest::YSize);
  DREAM3D_REQUIRE_EQUAL(nz, UnitTest::DataContainerIOTest::ZSize);

//  DataContainerWriter::Pointer writer = DataContainerWriter::New();
//  writer->setOutputFile(UnitTest::DataContainerIOTest::TestFile2);
//  writer->setDataContainerArray(dca);
//  writer->execute();
//  err = writer->getErrorCondition();
//  DREAM3D_REQUIRE(err >= 0)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void insertDeleteArray(VolumeDataContainer::Pointer m)
{

  IDataArray::Pointer p = T::CreateArray(5, "Test");
  m->addCellData("Test", p);
  IDataArray::Pointer t = m->getCellData("Test");
  DREAM3D_TEST_POINTER(t.get(), !=, NULL);
  t = m->removeCellData( "Test" );
  DREAM3D_TEST_POINTER(t.get(), !=, NULL);
  t = m->getCellData( "Test" );
  DREAM3D_REQUIRE_EQUAL(t.get(), 0);

  m->addCellFieldData("Test", p);
  t = m->getCellFieldData("Test");
  DREAM3D_TEST_POINTER(t.get(), !=, NULL);
  t = m->removeCellFieldData( "Test" );
  DREAM3D_TEST_POINTER(t.get(), !=, NULL);
  t = m->getCellFieldData( "Test" );
  DREAM3D_REQUIRE_EQUAL(t.get(), 0);

  m->addCellEnsembleData("Test", p);
  t = m->getCellEnsembleData("Test");
  DREAM3D_TEST_POINTER(t.get(), !=, NULL);
  t = m->removeCellEnsembleData( "Test" );
  DREAM3D_TEST_POINTER(t.get(), !=, NULL);
  t = m->getCellEnsembleData( "Test" );
  DREAM3D_TEST_POINTER(t.get(), ==, NULL);


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestInsertDelete()
{
  VolumeDataContainer::Pointer m = VolumeDataContainer::New();
  m->setName(DREAM3D::HDF5::VolumeDataContainerName);
  QList<QString> nameList;

  insertDeleteArray<Int8ArrayType> (m);
  insertDeleteArray<UInt16ArrayType> (m);
  insertDeleteArray<Int16ArrayType> (m);
  insertDeleteArray<UInt16ArrayType>(m);
  insertDeleteArray<Int32ArrayType> (m);
  insertDeleteArray<UInt32ArrayType> (m);
  insertDeleteArray<Int64ArrayType> (m);
  insertDeleteArray<UInt64ArrayType> (m);
  insertDeleteArray<FloatArrayType> (m);
  insertDeleteArray<DoubleArrayType> (m);

  nameList = m->getCellArrayNameList();
  DREAM3D_REQUIRE_EQUAL(0, nameList.size() );

  nameList = m->getCellFieldArrayNameList();
  DREAM3D_REQUIRE_EQUAL(0, nameList.size() );

  nameList = m->getCellEnsembleArrayNameList();
  DREAM3D_REQUIRE_EQUAL(0, nameList.size() );
}


template<typename T, typename K>
void _arrayCreation(VolumeDataContainer::Pointer m)
{
  AbstractFilter::Pointer absFilt = AbstractFilter::New();

  QVector<int> dims(1, 2);
  T* ptr = m->createCellData<T, K, AbstractFilter>("Test", 10, dims, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(absFilt->getErrorCondition(), 0);
  DREAM3D_TEST_POINTER(ptr, !=,NULL);
  absFilt->setErrorCondition(0);

  // First try getting the array, but pass in a bad array name which should produce a null pointer
  // and negative error condition
  ptr =  m->getCellDataSizeCheck<T, K, AbstractFilter>("BAD_ARRAY_NAME", 10, 2, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(ptr , NULL)
  DREAM3D_REQUIRE_EQUAL(0, absFilt->getErrorCondition());
  absFilt->setErrorCondition(0);

  // Next try getting the array, but pass in a bad size name which should produce a null pointer
  // and negative error condition
  ptr =  m->getCellDataSizeCheck<T, K, AbstractFilter>("Test", 10, 1, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(ptr , NULL)
  DREAM3D_REQUIRE_NE(0, absFilt->getErrorCondition());
  absFilt->setErrorCondition(0);

  // Next try getting the array, but pass in a bad cast type which should produce a null pointer
  // and negative error condition
  bool* bool_ptr =  m->getCellDataSizeCheck<bool, BoolArrayType, AbstractFilter>("Test", 10, 2, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(bool_ptr , NULL)
  DREAM3D_REQUIRE_NE(0, absFilt->getErrorCondition());
  absFilt->setErrorCondition(0);

  // Next, pass in all the correct values which should produce a Non NULL pointer value and
  // Zero Error Condition
  ptr = m->getCellDataSizeCheck<T, K, AbstractFilter>("Test", 10, 2, absFilt.get());
  DREAM3D_TEST_POINTER(ptr, !=, NULL);
  DREAM3D_REQUIRE_EQUAL(0, absFilt->getErrorCondition());

  IDataArray::Pointer t = m->removeCellData( "Test" );
  DREAM3D_TEST_POINTER(ptr, !=, NULL);

  /********************************* Field Data Tests *********************************************/
  ptr = m->createCellFieldData<T, K, AbstractFilter>("Test", 10, dims, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(absFilt->getErrorCondition(), 0);
  DREAM3D_TEST_POINTER(ptr, !=, NULL);
  absFilt->setErrorCondition(0);

  // First try getting the array, but pass in a bad array name which should produce a null pointer
  // and negative error condition
  ptr =  m->getCellFieldDataSizeCheck<T, K, AbstractFilter>("BAD_ARRAY_NAME", 10, 2, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(ptr , NULL)
  DREAM3D_REQUIRE_EQUAL(0, absFilt->getErrorCondition());
  absFilt->setErrorCondition(0);

  // Next try getting the array, but pass in a bad size name which should produce a null pointer
  // and negative error condition
  ptr =  m->getCellFieldDataSizeCheck<T, K, AbstractFilter>("Test", 10, 1, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(ptr , NULL)
  DREAM3D_REQUIRE_NE(0, absFilt->getErrorCondition());
  absFilt->setErrorCondition(0);

  // Next try getting the array, but pass in a bad cast type which should produce a null pointer
  // and negative error condition
  bool_ptr =  m->getCellFieldDataSizeCheck<bool, BoolArrayType, AbstractFilter>("Test", 10, 2, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(bool_ptr , NULL)
  DREAM3D_REQUIRE_NE(0, absFilt->getErrorCondition());
  absFilt->setErrorCondition(0);

  // Next, pass in all the correct values which should produce a Non NULL pointer value and
  // Zero Error Condition
  ptr = m->getCellFieldDataSizeCheck<T, K, AbstractFilter>("Test", 10, 2, absFilt.get());
  DREAM3D_TEST_POINTER(ptr, !=, NULL);
  DREAM3D_REQUIRE_EQUAL(0, absFilt->getErrorCondition());

  t = m->removeCellFieldData( "Test" );
  DREAM3D_TEST_POINTER(ptr, !=, NULL);


  /********************************* Ensemble Data Tests *********************************************/
  ptr = m->createCellEnsembleData<T, K, AbstractFilter>("Test", 10, dims, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(absFilt->getErrorCondition(), 0);
  DREAM3D_TEST_POINTER(ptr, !=, NULL);
  absFilt->setErrorCondition(0);

  // First try getting the array, but pass in a bad array name which should produce a null pointer
  // and negative error condition
  ptr =  m->getCellEnsembleDataSizeCheck<T, K, AbstractFilter>("BAD_ARRAY_NAME", 10, 2, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(ptr , NULL)
  DREAM3D_REQUIRE_EQUAL(0, absFilt->getErrorCondition());
  absFilt->setErrorCondition(0);

  // Next try getting the array, but pass in a bad size name which should produce a null pointer
  // and negative error condition
  ptr =  m->getCellEnsembleDataSizeCheck<T, K, AbstractFilter>("Test", 10, 1, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(ptr , NULL)
  DREAM3D_REQUIRE_NE(0, absFilt->getErrorCondition());
  absFilt->setErrorCondition(0);

  // Next try getting the array, but pass in a bad cast type which should produce a null pointer
  // and negative error condition
  bool_ptr =  m->getCellEnsembleDataSizeCheck<bool, BoolArrayType, AbstractFilter>("Test", 10, 2, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(bool_ptr , NULL)
  DREAM3D_REQUIRE_NE(0, absFilt->getErrorCondition());
  absFilt->setErrorCondition(0);

  // Next, pass in all the correct values which should produce a Non NULL pointer value and
  // Zero Error Condition
  ptr = m->getCellEnsembleDataSizeCheck<T, K, AbstractFilter>("Test", 10, 2, absFilt.get());
  DREAM3D_TEST_POINTER(ptr, !=, NULL);
  DREAM3D_REQUIRE_EQUAL(0, absFilt->getErrorCondition());


  t = m->removeCellEnsembleData( "Test" );
  DREAM3D_TEST_POINTER(ptr, !=, NULL);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestArrayCreation()
{
  VolumeDataContainer::Pointer m = VolumeDataContainer::New();
  m->setName(DREAM3D::HDF5::VolumeDataContainerName);
  QList<QString> nameList;

  _arrayCreation<int8_t, Int8ArrayType>(m);
  _arrayCreation<uint8_t, UInt8ArrayType>(m);
  _arrayCreation<int16_t, Int16ArrayType>(m);
  _arrayCreation<uint16_t, UInt16ArrayType>(m);
  _arrayCreation<int32_t, Int32ArrayType>(m);
  _arrayCreation<uint32_t, UInt32ArrayType>(m);
  _arrayCreation<int64_t, Int64ArrayType>(m);
  _arrayCreation<uint64_t, UInt64ArrayType>(m);
  _arrayCreation<float, FloatArrayType>(m);
  _arrayCreation<double, DoubleArrayType>(m);


  nameList = m->getCellArrayNameList();
  DREAM3D_REQUIRE_EQUAL(0, nameList.size() );

  nameList = m->getCellFieldArrayNameList();
  DREAM3D_REQUIRE_EQUAL(0, nameList.size() );

  nameList = m->getCellEnsembleArrayNameList();
  DREAM3D_REQUIRE_EQUAL(0, nameList.size() );

  // DO NOT TEST THE BoolArrayType - we are using that as a negative test in the _arrayCreation test

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestDataContainer()
{

  IDataArray::Pointer iDataArray = NeighborList<int>::New();

  NeighborList<int>* neighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>* >(iDataArray.get());
  assert (neighborList != NULL);


  for (int grainId = 0; grainId < 10; ++grainId)
  {
    for (int neighborId = 0; neighborId < grainId + 5; ++neighborId)
    {
      neighborList->addEntry(grainId, neighborId * 23);
    }
  }
  bool ok = true;

  int value = neighborList->getValue(5, 4, ok);
  BOOST_ASSERT(ok);

  value = neighborList->getValue(12, 4, ok);
  BOOST_ASSERT(!ok);

  std::cout << "Number of Lists: " << neighborList->getNumberOfLists() << std::endl;
  std::cout << "Number of Entries for Grain Id[5]: " << neighborList->getListSize(5) << std::endl;
  std::cout << "Value for [5][3]: " << neighborList->getValue(5, 3, ok) << std::endl;

  VolumeDataContainer::Pointer dataContainer = VolumeDataContainer::New();
  dataContainer->setName(DREAM3D::HDF5::VolumeDataContainerName);
  dataContainer->addCellData("NeighborList", iDataArray);
  {
    MAKE_ARRAY(int8_t, "int8_t_Array" );
    MAKE_ARRAY(int16_t, "int16_t_Array" );
    MAKE_ARRAY(int32_t, "int32_t_Array" );
    MAKE_ARRAY(int64_t, "int64_t_Array" );
    MAKE_ARRAY(uint8_t, "uint8_t_Array" );
    MAKE_ARRAY(uint16_t, "uint16_t_Array" );
    MAKE_ARRAY(uint32_t, "uint32_t_Array" );
    MAKE_ARRAY(uint64_t, "uint64_t_Array" );
    MAKE_ARRAY(float, "float_Array" );
    MAKE_ARRAY(double, "double_Array" );
  }

  {
    // We can get a smaart pointer to the IDataArray Class
    IDataArray::Pointer ptr = dataContainer->getCellData("int32_t_Array");


    double* dPtr = IDataArray::SafeReinterpretCast<IDataArray*, DoubleArrayType*, double*>(ptr.get());
    BOOST_ASSERT(NULL == dPtr);

    int32_t* iPtr = IDataArray::SafeReinterpretCast<IDataArray*, Int32ArrayType*, int32_t*>(ptr.get());
    BOOST_ASSERT(NULL != iPtr);

    // Or we can downcast to the type we know it is (in line)
    Int32ArrayType* intPtr = Int32ArrayType::SafeObjectDownCast<IDataArray*, Int32ArrayType* >(dataContainer->getCellData("int32_t_Array").get());
    if (NULL != intPtr)
    {
      std::cout << "Downcast to intPtr pointer was successful" << std::endl;
      std::cout << "Number of Tuples:" << intPtr->getNumberOfTuples() << std::endl;
      int32_t* p = intPtr->getPointer(0);
      for (int i = 0; i < 5; ++i)
      {
        std::cout << (int)p[i] << std::endl;
      }
      for (int i = 0; i < 5; ++i)
      {
        std::cout << (int)(intPtr->GetValue(i)) << std::endl;
      }
    }


    DoubleArrayType* doublePtr = DoubleArrayType::SafeObjectDownCast<IDataArray*, DataArray<double>*>(ptr.get());
    if (NULL != doublePtr)
    {
      std::cout << "Downcast to double pointer was successful" << std::endl;
    }
    else
    {
      std::cout << "Downcast to double pointer FAILED." << std::endl;
    }

  }

  // Test Getting an Array

  // Test removing an Array

  // Test Size of all types of maps

}



// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  int err = EXIT_SUCCESS;

#if !REMOVE_TEST_FILES
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
#endif


  DREAM3D_REGISTER_TEST( TestInsertDelete() )
  DREAM3D_REGISTER_TEST( TestArrayCreation() )

  DREAM3D_REGISTER_TEST( TestDataContainerWriter() )
  DREAM3D_REGISTER_TEST( TestDataContainerReader() )

#if REMOVE_TEST_FILES
//  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
#endif

  PRINT_TEST_SUMMARY();
  return err;
}

