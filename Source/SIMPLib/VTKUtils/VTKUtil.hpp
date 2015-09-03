#ifndef _VTKUtil_H_
#define _VTKUtil_H_

#include <typeinfo>

#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"

class SIMPLib_EXPORT VTKUtil
{
  public:

    /**
    * @brief Returns the HDF Type for a given primitive value.
     * @param value A value to use. Can be anything. Just used to get the type info
     * from
     * @return The HDF5 native type for the value
     */
    template<typename T>
    static QString TypeForPrimitive(T value)
    {

      if (typeid(value) == typeid(float)) { return QString("float"); }
      if (typeid(value) == typeid(double)) { return QString("double"); }

      if (typeid(value) == typeid(int8_t)) { return QString("char"); }
      if (typeid(value) == typeid(uint8_t)) { return QString("unsigned_char"); }
# if CMP_TYPE_CHAR_IS_SIGNED
      if (typeid(value) == typeid(char)) { return QString("char"); }
#else
      if (typeid(value) == typeid(char)) { return H5T_NATIVE_UINT8; }
#endif
      if (typeid(value) == typeid(signed char)) { return QString("char"); }
      if (typeid(value) == typeid(unsigned char)) { return QString("char"); }


      if (typeid(value) == typeid(int16_t)) { return QString("short"); }
      if (typeid(value) == typeid(short)) { return QString("short"); }
      if (typeid(value) == typeid(signed short)) { return QString("short"); }
      if (typeid(value) == typeid(uint16_t)) { return QString("unsigned_short"); }
      if (typeid(value) == typeid(unsigned short)) { return QString("unsigned_short"); }


      if (typeid(value) == typeid(int32_t)) { return QString("int"); }
      if (typeid(value) == typeid(uint32_t)) { return QString("unsigned_int"); }
#if (CMP_SIZEOF_INT == 4)
      if (typeid(value) == typeid(int)) { return QString("int"); }
      if (typeid(value) == typeid(signed int)) { return QString("int"); }
      if (typeid(value) == typeid(unsigned int)) { return QString("unsigned_int"); }
#endif


#if (CMP_SIZEOF_LONG == 4)
      if (typeid(value) == typeid(long int)) { return QString("int"); }
      if (typeid(value) == typeid(signed long int)) { return QString("int"); }
      if (typeid(value) == typeid(unsigned long int)) { return QString("unsigned_int"); }
#elif (CMP_SIZEOF_LONG == 8)
      if (typeid(value) == typeid(long int)) { return QString("long"); }
      if (typeid(value) == typeid(signed long int)) { return QString("long"); }
      if (typeid(value) == typeid(unsigned long int)) { return QString("unsigned_long"); }
#endif


#if (CMP_SIZEOF_LONG_LONG == 8)
      if (typeid(value) == typeid(long long int)) { return QString("long"); }
      if (typeid(value) == typeid(signed long long int)) { return QString("long"); }
      if (typeid(value) == typeid(unsigned long long int)) { return QString("unsigned_long"); }
#endif
      if (typeid(value) == typeid(int64_t)) { return QString("long"); }
      if (typeid(value) == typeid(uint64_t)) { return QString("unsigned_long"); }

      if (typeid(value) == typeid(bool)) { return QString("char"); }

      std::cout  << "Error: VtkTypeForPrimitive - Unknown Type: " << (typeid(value).name()) << std::endl;
      const char* name = typeid(value).name();
      if (NULL != name && name[0] == 'l' )
      {
        std::cout << "You are using 'long int' as a type which is not 32/64 bit safe. Suggest you use one of the H5SupportTypes defined in <Common/H5SupportTypes.h> such as int32_t or uint32_t." << std::endl;
      }
      return QString("");
    }


  private:
    VTKUtil(const VTKUtil&); // Copy Constructor Not Implemented
    void operator=(const VTKUtil&); // Operator '=' Not Implemented
};


#endif /* _VTKUtil_H_ */
