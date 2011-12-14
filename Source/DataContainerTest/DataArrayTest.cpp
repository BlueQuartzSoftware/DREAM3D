

#include <string>
#include <map>


#include "DREAM3DLib/DREAM3DLib.h"

#include "DREAM3DLib/Common/IDataArray.h"
#include "DREAM3DLib/Common/DataArray.hpp"

#define helper(a, b)\
  a##b



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class DataContainer
{
  public:
    DataContainer() {}
    virtual ~DataContainer() {}


    void addVoxelData(const std::string &name, IDataArray::Pointer data)
    {
      m_VoxelData[name] = data;
    }

    IDataArray::Pointer getVoxelData(const std::string &name)
    {
      std::map<std::string, IDataArray::Pointer>::iterator it;
      it =  m_VoxelData.find(name);
      if ( it == m_VoxelData.end() )
      {
        return IDataArray::NullPointer();
      }
      return (*it).second;
    }


  protected:

  private:
    std::map<std::string, IDataArray::Pointer> m_VoxelData;



    DataContainer(const DataContainer&); // Copy Constructor Not Implemented
    void operator=(const DataContainer&); // Operator '=' Not Implemented
};


#define MAKE_ARRAY(type, name)\
IDataArray::Pointer type##Ptr = DataArray<type>::CreateArray(5);\
type##Ptr->SetName( name );\
dataContainer.addVoxelData(name, type##Ptr);


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

  DataContainer dataContainer;

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
    IDataArray::Pointer ptr = dataContainer.getVoxelData("int32_t_Array");


    double* dPtr = IDataArray::SafeReinterpretCast<IDataArray*, DoubleArrayType*, double*>(ptr.get());
    assert(NULL == dPtr);

    int32_t* iPtr = IDataArray::SafeReinterpretCast<IDataArray*, Int32ArrayType*, int32_t*>(ptr.get());
    assert(NULL != iPtr);

    // Or we can downcast to the type we know it is (in line)
    Int32ArrayType* intPtr = Int32ArrayType::SafeObjectDownCast<IDataArray*, Int32ArrayType* >(dataContainer.getVoxelData("int32_t_Array").get());
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
