

#include <string>
#include <map>


#include "DREAM3DLib/DREAM3DLib.h"

#include "DREAM3DLib/Common/IDataArray.h"
#include "DREAM3DLib/Common/DataArray.hpp"
#include "DREAM3DLib/Common/DataContainer.h"
#include "DREAM3DLib/Common/NeighborList.hpp"



#define helper(a, b)\
  a##b




#define MAKE_ARRAY(type, name)\
IDataArray::Pointer type##Ptr = DataArray<type>::CreateArray(5);\
type##Ptr->SetName( name );\
dataContainer->addVoxelData(name, type##Ptr);


typedef DataArray<int8_t>  Int8ArrayType;
typedef DataArray<uint8_t>  UInt8ArrayType;
typedef DataArray<int32_t>  Int32ArrayType;
typedef DataArray<uint32_t>  UInt32ArrayType;

typedef DataArray<float>  FloatArrayType;
typedef DataArray<double>  DoubleArrayType;






// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
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
  return 0;
}
