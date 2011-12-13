

#include <string>
#include <map>


#include "DREAM3DLib/DREAM3DLib.h"

#include "IDataArray.h"
#include "DataArray.hpp"

#define helper(a, b)\
  a##b

#define MAKE_ARRAY(type, name)\
IDataArray::Pointer type##Ptr = DataArray<type>::CreateArray(5);\
type##Ptr->SetName( name );\
voxelData[name] = type##Ptr;


int main(int argc, char **argv)
{

  std::map<std::string, IDataArray::Pointer> voxelData;


  {
    MAKE_ARRAY(int8_t, "int8_t_Ptr" );
    MAKE_ARRAY(int16_t, "int16_t_Ptr" );
    MAKE_ARRAY(int32_t, "int32_t_Ptr" );
    MAKE_ARRAY(int64_t, "int64_t_Ptr" );
    MAKE_ARRAY(uint8_t, "uint8_t_Ptr" );
    MAKE_ARRAY(uint16_t, "uint16_t_Ptr" );
    MAKE_ARRAY(uint32_t, "uint32_t_Ptr" );
    MAKE_ARRAY(uint64_t, "uint64_t_Ptr" );
    MAKE_ARRAY(float, "float_Ptr" );
    MAKE_ARRAY(double, "double_Ptr" );
  }



  {
    IDataArray::Pointer ptr = voxelData["int8_t_Ptr"];
    DataArray<int8_t>* int8Ptr = DataArray<int8_t>::SafeDownCast<IDataArray*, DataArray<int8_t>* >(ptr.get());
    if (NULL != int8Ptr)
    {
      std::cout << "Downcast to int8 pointer was successful" << std::endl;
      std::cout << "Number of Tuples:" << int8Ptr->GetNumberOfTuples() << std::endl;
      int8_t* p = int8Ptr->GetPointer(0);
      for (int i = 0; i < 5; ++i)
      {
        std::cout << (int)p[i] << std::endl;
      }
    }


    DataArray<double>* doublePtr = DataArray<double>::SafeDownCast<IDataArray*, DataArray<double>*>(ptr.get());
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
