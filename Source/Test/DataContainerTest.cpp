

#include <string>
#include <map>


#include "DREAM3DLib/DREAM3DLib.h"

#include "DREAM3DLib/Common/IDataArray.h"
#include "DREAM3DLib/Common/DataArray.hpp"
#include "DREAM3DLib/Common/DataContainer.h"
#include "DREAM3DLib/Common/NeighborList.hpp"
#include "DREAM3DLib/Common/AbstractFilter.h"


#include "UnitTestSupport.hpp"

#include "TestFileLocations.h"

#define helper(a, b)\
  a##b


#define MAKE_ARRAY(type, name)\
IDataArray::Pointer type##Ptr = DataArray<type>::CreateArray(5);\
type##Ptr->SetName( name );\
dataContainer->addVoxelData(name, type##Ptr);


#define TEST_DATA_CONTAINER(Type, DCType)\
{  IDataArray::Pointer t_##Type = Type::CreateArray(5);\
  t_##Type->SetName( #Type );\
  m->add##DCType(#Type, t_##Type);\
  IDataArray::Pointer t = m->get##DCType(#Type);\
  DREAM3D_REQUIRE_NE(t.get(), NULL);\
  t = m->removeFieldData(#Type);\
  DREAM3D_REQUIRE_NE(t.get(), NULL);\
  t = m->get##DCType(#Type);\
  DREAM3D_REQUIRE_EQUAL(t.get(), NULL);\
  }




template<typename T>
void insertDeleteArray(DataContainer::Pointer m)
{

  IDataArray::Pointer p = typename T::CreateArray(5);
  p->SetName( "Test" );
  m->addVoxelData("Test", p);
  IDataArray::Pointer t = m->getVoxelData("Test");
  DREAM3D_REQUIRE_NE(t.get(), NULL);
  t = m->removeVoxelData( "Test" );
  DREAM3D_REQUIRE_NE(t.get(), NULL);
  t = m->getVoxelData( "Test" );
  DREAM3D_REQUIRE_EQUAL(t.get(), NULL);



  m->addFieldData("Test", p);
  t = m->getFieldData("Test");
  DREAM3D_REQUIRE_NE(t.get(), NULL);
  t = m->removeFieldData( "Test" );
  DREAM3D_REQUIRE_NE(t.get(), NULL);
  t = m->getFieldData( "Test" );
  DREAM3D_REQUIRE_EQUAL(t.get(), NULL);



  m->addEnsembleData("Test", p);
  t = m->getEnsembleData("Test");
  DREAM3D_REQUIRE_NE(t.get(), NULL);
  t = m->removeEnsembleData( "Test" );
  DREAM3D_REQUIRE_NE(t.get(), NULL);
  t = m->getEnsembleData( "Test" );
  DREAM3D_REQUIRE_EQUAL(t.get(), NULL);



}




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestInsertDelete()
{
  DataContainer::Pointer m = DataContainer::New();
  std::list<std::string> nameList;

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

  nameList = m->getVoxelArrayNameList();
  DREAM3D_REQUIRE_EQUAL(0, nameList.size() );

  nameList = m->getFieldArrayNameList();
  DREAM3D_REQUIRE_EQUAL(0, nameList.size() );
  
  nameList = m->getEnsembleArrayNameList();
  DREAM3D_REQUIRE_EQUAL(0, nameList.size() );
}


template<typename T, typename K>
void _arrayCreation(DataContainer::Pointer m)
{
  AbstractFilter::Pointer absFilt = AbstractFilter::New();

  T* ptr = m->createVoxelData<T, K, AbstractFilter>("Test", 10, 2, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(absFilt->getErrorCondition(), 0);
  DREAM3D_REQUIRE_NE(ptr, NULL);
  absFilt->setErrorCondition(0);

  // First try getting the array, but pass in a bad array name which should produce a null pointer
  // and negative error condition
  ptr =  m->getVoxelDataSizeCheck<T, K, AbstractFilter>("BAD_ARRAY_NAME", 10*2, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(ptr , NULL)
  DREAM3D_REQUIRE_NE(0, absFilt->getErrorCondition());
  absFilt->setErrorCondition(0);

  // Next try getting the array, but pass in a bad size name which should produce a null pointer
  // and negative error condition
  ptr =  m->getVoxelDataSizeCheck<T, K, AbstractFilter>("Test", 10, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(ptr , NULL)
  DREAM3D_REQUIRE_NE(0, absFilt->getErrorCondition());
  absFilt->setErrorCondition(0);

  // Next try getting the array, but pass in a bad cast type which should produce a null pointer
  // and negative error condition
  bool* bool_ptr =  m->getVoxelDataSizeCheck<bool, BoolArrayType, AbstractFilter>("Test", 10*2, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(bool_ptr , NULL)
  DREAM3D_REQUIRE_NE(0, absFilt->getErrorCondition());
  absFilt->setErrorCondition(0);

  // Next, pass in all the correct values which should produce a Non NULL pointer value and 
  // Zero Error Condition
  ptr = m->getVoxelDataSizeCheck<T, K, AbstractFilter>("Test", 10*2, absFilt.get());
  DREAM3D_REQUIRE_NE(ptr, NULL);
  DREAM3D_REQUIRE_EQUAL(0, absFilt->getErrorCondition());

  IDataArray::Pointer t = m->removeVoxelData( "Test" );
  DREAM3D_REQUIRE_NE(t.get(), NULL);

/********************************* Field Data Tests *********************************************/
  ptr = m->createFieldData<T, K, AbstractFilter>("Test", 10, 2, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(absFilt->getErrorCondition(), 0);
  DREAM3D_REQUIRE_NE(ptr, NULL);
  absFilt->setErrorCondition(0);

  // First try getting the array, but pass in a bad array name which should produce a null pointer
  // and negative error condition
  ptr =  m->getFieldDataSizeCheck<T, K, AbstractFilter>("BAD_ARRAY_NAME", 10*2, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(ptr , NULL)
  DREAM3D_REQUIRE_NE(0, absFilt->getErrorCondition());
  absFilt->setErrorCondition(0);

  // Next try getting the array, but pass in a bad size name which should produce a null pointer
  // and negative error condition
  ptr =  m->getFieldDataSizeCheck<T, K, AbstractFilter>("Test", 10, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(ptr , NULL)
  DREAM3D_REQUIRE_NE(0, absFilt->getErrorCondition());
  absFilt->setErrorCondition(0);

  // Next try getting the array, but pass in a bad cast type which should produce a null pointer
  // and negative error condition
  bool_ptr =  m->getFieldDataSizeCheck<bool, BoolArrayType, AbstractFilter>("Test", 10*2, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(bool_ptr , NULL)
  DREAM3D_REQUIRE_NE(0, absFilt->getErrorCondition());
  absFilt->setErrorCondition(0);

  // Next, pass in all the correct values which should produce a Non NULL pointer value and 
  // Zero Error Condition
  ptr = m->getFieldDataSizeCheck<T, K, AbstractFilter>("Test", 10*2, absFilt.get());
  DREAM3D_REQUIRE_NE(ptr, NULL);
  DREAM3D_REQUIRE_EQUAL(0, absFilt->getErrorCondition());

  t = m->removeFieldData( "Test" );
  DREAM3D_REQUIRE_NE(t.get(), NULL);


  /********************************* Ensemble Data Tests *********************************************/
  ptr = m->createEnsembleData<T, K, AbstractFilter>("Test", 10, 2, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(absFilt->getErrorCondition(), 0);
  DREAM3D_REQUIRE_NE(ptr, NULL);
  absFilt->setErrorCondition(0);

  // First try getting the array, but pass in a bad array name which should produce a null pointer
  // and negative error condition
  ptr =  m->getEnsembleDataSizeCheck<T, K, AbstractFilter>("BAD_ARRAY_NAME", 10*2, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(ptr , NULL)
  DREAM3D_REQUIRE_NE(0, absFilt->getErrorCondition());
  absFilt->setErrorCondition(0);

  // Next try getting the array, but pass in a bad size name which should produce a null pointer
  // and negative error condition
  ptr =  m->getEnsembleDataSizeCheck<T, K, AbstractFilter>("Test", 10, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(ptr , NULL)
  DREAM3D_REQUIRE_NE(0, absFilt->getErrorCondition());
  absFilt->setErrorCondition(0);

  // Next try getting the array, but pass in a bad cast type which should produce a null pointer
  // and negative error condition
  bool_ptr =  m->getEnsembleDataSizeCheck<bool, BoolArrayType, AbstractFilter>("Test", 10*2, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(bool_ptr , NULL)
  DREAM3D_REQUIRE_NE(0, absFilt->getErrorCondition());
  absFilt->setErrorCondition(0);

  // Next, pass in all the correct values which should produce a Non NULL pointer value and 
  // Zero Error Condition
  ptr = m->getEnsembleDataSizeCheck<T, K, AbstractFilter>("Test", 10*2, absFilt.get());
  DREAM3D_REQUIRE_NE(ptr, NULL);
  DREAM3D_REQUIRE_EQUAL(0, absFilt->getErrorCondition());


  t = m->removeEnsembleData( "Test" );
  DREAM3D_REQUIRE_NE(t.get(), NULL);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestArrayCreation()
{
  DataContainer::Pointer m = DataContainer::New();
  std::list<std::string> nameList;

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


  nameList = m->getVoxelArrayNameList();
  DREAM3D_REQUIRE_EQUAL(0, nameList.size() );

  nameList = m->getFieldArrayNameList();
  DREAM3D_REQUIRE_EQUAL(0, nameList.size() );

  nameList = m->getEnsembleArrayNameList();
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
    for (int neighborId = 0; neighborId < grainId+5; ++neighborId)
    {
      neighborList->addEntry(grainId, neighborId*23);
    }
  }
  bool ok = true;

  int value = neighborList->getValue(5, 4, ok);
  assert(ok);

  value = neighborList->getValue(12, 4, ok);
  assert(!ok);

  std::cout << "Number of Lists: " << neighborList->getNumberOfLists() << std::endl;
  std::cout << "Number of Entries for Grain Id[5]: " << neighborList->getListSize(5) << std::endl;
  std::cout << "Value for [5][3]: " << neighborList->getValue(5, 3, ok) << std::endl;

  DataContainer::Pointer dataContainer = DataContainer::New();
  dataContainer->addVoxelData("NeighborList", iDataArray);
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
    IDataArray::Pointer ptr = dataContainer->getVoxelData("int32_t_Array");


    double* dPtr = IDataArray::SafeReinterpretCast<IDataArray*, DoubleArrayType*, double*>(ptr.get());
    assert(NULL == dPtr);

    int32_t* iPtr = IDataArray::SafeReinterpretCast<IDataArray*, Int32ArrayType*, int32_t*>(ptr.get());
    assert(NULL != iPtr);

    // Or we can downcast to the type we know it is (in line)
    Int32ArrayType* intPtr = Int32ArrayType::SafeObjectDownCast<IDataArray*, Int32ArrayType* >(dataContainer->getVoxelData("int32_t_Array").get());
    if (NULL != intPtr)
    {
      std::cout << "Downcast to intPtr pointer was successful" << std::endl;
      std::cout << "Number of Tuples:" << intPtr->GetNumberOfTuples() << std::endl;
      int32_t* p = intPtr->GetPointer(0);
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

  // Test Getting

}



// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( TestInsertDelete() );
  DREAM3D_REGISTER_TEST( TestArrayCreation() );

  PRINT_TEST_SUMMARY();
  return err;
}

