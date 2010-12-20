///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, 2010 Michael A. Jackson for BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number
//                           FA8650-04-C-5229
//
///////////////////////////////////////////////////////////////////////////////
#ifndef _HDF5_LITE_H_
#define _HDF5_LITE_H_

#include <MXA/Common/DLLExport.h>
#include <MXA/MXATypes.h>
#include <MXA/Common/LogTime.h>

//--C++ Headers
#include <typeinfo>

//-- STL Headers
#include <string>
#include <iostream>
#include <vector>
#include <map>

//-- HDF Headers
#include <hdf5.h>
#include <H5Tpublic.h>

/* H5LITE_USE_MXA_CONSTRUCTS is used to include MXADataModel Specific classes in
 * this class. If this is being compiled as part of MXADataModel this should
 * _always_ be defined. If this code is being used as part of another project
 * then this should probably NOT be defined.
 */
#ifdef H5LITE_USE_MXA_CONSTRUCTS
class IMXAArray;
#endif

//TODO: Add tests for the find* methods

#define CloseH5A(aid, err, retError)\
   err = H5Aclose( attr_id );\
   if (err<0) {std::cout << "File: " << __FILE__ << "(" << __LINE__ << "): " << "Error Closing Attribute." << std::endl;retErr = err;}

#define CloseH5D(did, err, retError)\
  err = H5Dclose(did);\
  if (err < 0) { std::cout << "File: " << __FILE__ << "(" << __LINE__ << "): "<< "Error Closing Dataset." << std::endl; retError = err;}

#define CloseH5S(sid, err, retError)\
  err = H5Sclose(sid); \
  if ( err < 0) {std::cout << "File: " << __FILE__ << "(" << __LINE__ << "): "<< "Error closing Dataspace." << std::endl;retErr = err;}

#define CloseH5T(tid, err, retError)\
  err = H5Tclose(tid);\
  if (err < 0 ) {std::cout << "File: " << __FILE__ << "(" << __LINE__ << "): "<< "Error closing DataType" << std::endl; retErr = err;}

#define HDF_ERROR_HANDLER_OFF\
  herr_t (*_oldHDF_error_func)(void *);\
  void *_oldHDF_error_client_data;\
  H5Eget_auto(&_oldHDF_error_func, &_oldHDF_error_client_data);\
  H5Eset_auto(NULL, NULL);

#define HDF_ERROR_HANDLER_ON  H5Eset_auto(_oldHDF_error_func, _oldHDF_error_client_data);


#define UNUSED(x) ((void)(x));

#ifdef __cplusplus
extern "C" {
#endif
  herr_t MXA_EXPORT find_attr( hid_t loc_id, const char *name, void *op_data);

  herr_t MXA_EXPORT find_dataset( hid_t loc_id, const char *name, void *op_data);

#ifdef __cplusplus
}
#endif

/**
 * @brief Class to bring together some high level methods to read/write data to HDF5 files.
 * @class H5Lite
 * @author Mike Jackson
 * @date April 2007
 * @version $Revision: 1.3 $
 */
class H5Lite
{
public:
  /**
   * @brief Turns off the global error handler/reporting objects. Note that once
   * they are turned off using this method they CAN NOT be turned back on. If you
   * would like to turn them off for a piece of code then surround your code with
   * the HDF_ERROR_HANDLER_OFF and HDF_ERROR_HANDLER_ON macros defined in
   * H5Lite.h
   */
  static MXA_EXPORT void disableErrorHandlers();

/**
 * @brief Opens an object for HDF5 operations
 * @param loc_id The parent object that holds the true object we want to open
 * @param objName The string name of the object
 * @param obj_type The HDF5_TYPE of object
 * @return Standard HDF5 Error Conditions
 */
static MXA_EXPORT herr_t openId( hid_t loc_id, const std::string& objName, int32_t obj_type);

/**
 * @brief Opens an HDF5 Object
 * @param obj_id The Object id
 * @param obj_type Basic Object Type
 * @return Standard HDF5 Error Conditions
 */
static MXA_EXPORT herr_t closeId( hid_t obj_id, int32_t obj_type );

/**
 * @brief Given one of the HDF Types as a string, this will return the HDF Type
 * as an hid_t value.
 * @param value The HDF_Type as a string
 * @return the hid_t value for the given type. -1 if the string does not match a type.
 */
static hid_t HDFTypeFromString(const std::string &value)
{
  if (value.compare("H5T_STRING") == 0) return H5T_STRING;

  if (value.compare("H5T_NATIVE_INT8") == 0) return H5T_NATIVE_INT8;
  if (value.compare("H5T_NATIVE_UINT8") == 0) return H5T_NATIVE_UINT8;

  if (value.compare("H5T_NATIVE_INT16") == 0) return H5T_NATIVE_INT16;
  if (value.compare("H5T_NATIVE_UINT16") == 0) return H5T_NATIVE_UINT16;

  if (value.compare("H5T_NATIVE_INT32") == 0) return H5T_NATIVE_INT32;
  if (value.compare("H5T_NATIVE_UINT32") == 0) return H5T_NATIVE_UINT32;

  if (value.compare("H5T_NATIVE_INT64") == 0) return H5T_NATIVE_INT64;
  if (value.compare("H5T_NATIVE_UINT64") == 0) return H5T_NATIVE_UINT64;

  if (value.compare("H5T_NATIVE_FLOAT") == 0) return H5T_NATIVE_FLOAT;
  if (value.compare("H5T_NATIVE_DOUBLE") == 0) return H5T_NATIVE_DOUBLE;

  std::cout << DEBUG_OUT(logTime) << "Error: HDFTypeFromString - Unknown Type: " << value << std::endl;
  return -1;
}


/**
 * @brief Returns a string version of the HDF Type
 * @param type The HDF5 Type to query
 * @return
 */
static std::string StringForHDFType(hid_t type)
{
  if ( type == H5T_STRING) return "H5T_STRING";

  if (H5Tequal(type ,H5T_NATIVE_INT8) ) return "H5T_NATIVE_INT8";
  if (H5Tequal(type ,H5T_NATIVE_UINT8) ) return "H5T_NATIVE_UINT8";

  if (H5Tequal(type ,H5T_NATIVE_INT16) ) return "H5T_NATIVE_INT16";
  if (H5Tequal(type ,H5T_NATIVE_UINT16) ) return "H5T_NATIVE_UINT16";

  if (H5Tequal(type ,H5T_NATIVE_INT32) ) return "H5T_NATIVE_INT32";
  if (H5Tequal(type ,H5T_NATIVE_UINT32) ) return "H5T_NATIVE_UINT32";

  if (H5Tequal(type ,H5T_NATIVE_INT64) ) return "H5T_NATIVE_INT64";
  if (H5Tequal(type ,H5T_NATIVE_UINT64) ) return "H5T_NATIVE_UINT64";

  if (H5Tequal(type ,H5T_NATIVE_FLOAT) ) return "H5T_NATIVE_FLOAT";
  if (H5Tequal(type ,H5T_NATIVE_DOUBLE) ) return "H5T_NATIVE_DOUBLE";

  std::cout << DEBUG_OUT(logTime) << "Error: HDFTypeForPrimitiveAsStr - Unknown Type: " << std::endl;
  return "Unknown";
}

/**
* @brief Returns the HDF Type for a given primitive value.
 * @param value A value to use. Can be anything. Just used to get the type info
 * from
 * @return A std::string representing the HDF5 Type
 */
template<typename T>
static std::string HDFTypeForPrimitiveAsStr(T value)
{
  if (typeid(value) == typeid(int8_t)) return "H5T_NATIVE_INT8";
  if (typeid(value) == typeid(uint8_t)) return "H5T_NATIVE_UINT8";

  if (typeid(value) == typeid(int16_t)) return "H5T_NATIVE_INT16";
  if (typeid(value) == typeid(uint16_t)) return "H5T_NATIVE_UINT16";

  if (typeid(value) == typeid(int32_t)) return "H5T_NATIVE_INT32";
  if (typeid(value) == typeid(uint32_t)) return "H5T_NATIVE_UINT32";

  if (typeid(value) == typeid(int64_t)) return "H5T_NATIVE_INT64";
  if (typeid(value) == typeid(uint64_t)) return "H5T_NATIVE_UINT64";

  if (typeid(value) == typeid(float32)) return "H5T_NATIVE_FLOAT";
  if (typeid(value) == typeid(float64)) return "H5T_NATIVE_DOUBLE";

  //if (typeid(value) == typeid(bool)) return "H5T_NATIVE_UINT8";

  std::cout << DEBUG_OUT(logTime) << "Error: HDFTypeForPrimitiveAsStr - Unknown Type: " << typeid(value).name() << std::endl;
  return "";
}

/**
* @brief Returns the HDF Type for a given primitive value.
 * @param value A value to use. Can be anything. Just used to get the type info
 * from
 * @return The HDF5 native type for the value
 */
template<typename T>
static hid_t HDFTypeForPrimitive(T value)
{

  if (typeid(value) == typeid(float32)) return H5T_NATIVE_FLOAT;
  if (typeid(value) == typeid(float64)) return H5T_NATIVE_DOUBLE;

  if (typeid(value) == typeid(int8_t)) return H5T_NATIVE_INT8;
  if (typeid(value) == typeid(uint8_t)) return H5T_NATIVE_UINT8;
# if CMP_TYPE_CHAR_IS_SIGNED
  if (typeid(value) == typeid(char)) return H5T_NATIVE_INT8;
#else
  if (typeid(value) == typeid(char)) return H5T_NATIVE_UINT8;
#endif
  if (typeid(value) == typeid(signed char)) return H5T_NATIVE_INT8;
  if (typeid(value) == typeid(unsigned char)) return H5T_NATIVE_UINT8;


  if (typeid(value) == typeid(int16_t)) return H5T_NATIVE_INT16;
  if (typeid(value) == typeid(short)) return H5T_NATIVE_INT16;
  if (typeid(value) == typeid(signed short)) return H5T_NATIVE_INT16;
  if (typeid(value) == typeid(uint16_t)) return H5T_NATIVE_UINT16;
  if (typeid(value) == typeid(unsigned short)) return H5T_NATIVE_UINT16;


  if (typeid(value) == typeid(int32_t)) return H5T_NATIVE_INT32;
  if (typeid(value) == typeid(uint32_t)) return H5T_NATIVE_UINT32;
#if (CMP_SIZEOF_INT == 4)
  if (typeid(value) == typeid(int)) return H5T_NATIVE_INT32;
  if (typeid(value) == typeid(signed int)) return H5T_NATIVE_INT32;
  if (typeid(value) == typeid(unsigned int)) return H5T_NATIVE_UINT32;
#endif


#if (CMP_SIZEOF_LONG == 4)
  if (typeid(value) == typeid(long int)) return H5T_NATIVE_INT32;
  if (typeid(value) == typeid(signed long int)) return H5T_NATIVE_INT32;
  if (typeid(value) == typeid(unsigned long int)) return H5T_NATIVE_UINT32;
#elif (CMP_SIZEOF_LONG == 8)
  if (typeid(value) == typeid(long int)) return H5T_NATIVE_INT64;
  if (typeid(value) == typeid(signed long int)) return H5T_NATIVE_INT64;
  if (typeid(value) == typeid(unsigned long int)) return H5T_NATIVE_UINT64;
#endif


#if (CMP_SIZEOF_LONG_LONG == 8)
  if (typeid(value) == typeid(long long int)) return H5T_NATIVE_INT64;
  if (typeid(value) == typeid(signed long long int)) return H5T_NATIVE_INT64;
  if (typeid(value) == typeid(unsigned long long int)) return H5T_NATIVE_UINT64;
#endif
  if (typeid(value) == typeid(int64_t)) return H5T_NATIVE_INT64;
  if (typeid(value) == typeid(uint64_t)) return H5T_NATIVE_UINT64;

  if (typeid(value) == typeid(bool)) return H5T_NATIVE_UINT8;

  std::cout << DEBUG_OUT(logTime) << "Error: HDFTypeForPrimitive - Unknown Type: " << (typeid(value).name()) << std::endl;
  const char* name = typeid(value).name();
  if (NULL != name && name[0] == 'l' ) {
    std::cout << "You are using 'long int' as a type which is not 32/64 bit safe. Suggest you use one of the MXATypes defined in <Common/MXATypes.h> such as int32_t or uint32_t." << std::endl;
  }
  return -1;
}



/**
 * @brief Inquires if an attribute named attr_name exists attached to the object loc_id.
 * @param loc_id The location to search
 * @param attrName The attribute to search for
 * @return Standard HDF5 Error condition
 */
static MXA_EXPORT herr_t findAttribute( hid_t loc_id, const std::string& attrName );

/**
 * @brief Finds a Data set given a data set name
 * @param loc_id The location to search
 * @param name The attribute to search for
 * @return Standard HDF5 Error condition
 */
static MXA_EXPORT herr_t findDataset( hid_t loc_id, const std::string& name );

/**
 * @brief Creates a Dataset with the given name at the location defined by loc_id
 *
 *
 * @param loc_id The Parent location to store the data
 * @param dsetName The name of the dataset
 * @param dims The dimensions of the dataset
 * @param data The data to write to the file
 * @return Standard HDF5 error conditions
 *
 * The dimensions of the data sets are usually passed as both a "rank" and
 * dimensions array. By using a std::vector<hsize_t> that stores the values of
 * each of the dimensions we can reduce the number of arguments to this method as
 * the value of the "rank" simply becomes dims.length(). So to create a Dims variable
 * for a 3D data space of size(x,y,z) = {10,20,30} I would use the following code:
 * <code>
 * std::vector<hsize_t> dims;
 * dims.push_back(10);
 * dims.push_back(20);
 * dims.push_back(30);
 * </code>
 *
 * Also when passing data BE SURE that the type of data and the data type match.
 * For example if I create some data in a std::vector<UInt8Type> I would need to
 * pass H5T_NATIVE_UINT8 as the dataType.
 */
template <typename T>
static herr_t writeVectorDataset (hid_t loc_id,
                            const std::string& dsetName,
                            std::vector<hsize_t> &dims,
                            std::vector<T> &data)
{
  herr_t err = -1;
  hid_t did = -1;
  hid_t sid = -1;
  herr_t retErr = 0;
  hid_t dataType = H5Lite::HDFTypeForPrimitive(data.front());
  if(dataType == -1)
  {
    return -1;
  }
  //Create the DataSpace
  std::vector<uint64_t>::size_type size = dims.size();
  //std::vector<hsize_t> _dims(size, 0);
  std::vector<hsize_t> _dims(size, 0);
  for (std::vector<uint64_t>::size_type i = 0; i < size; ++i)
  {
    _dims[i] = static_cast<hsize_t>(dims[i]);
  }
  sid = H5Screate_simple( static_cast<int>(size), &(_dims.front()), NULL );
  if (sid < 0)
  {
    return sid;
  }
  // Create the Dataset
  did = H5Dcreate (loc_id, dsetName.c_str(), dataType, sid, H5P_DEFAULT);
  if ( did >= 0 )
  {
    err = H5Dwrite( did, dataType, H5S_ALL, H5S_ALL, H5P_DEFAULT, &(data.front()) );
    if (err < 0 ) {
      std::cout << "Error Writing Data" << std::endl;
      retErr = err;
    }
    err = H5Dclose( did );
    if (err < 0) {
      std::cout << "Error Closing Dataset." << std::endl;
      retErr = err;
    }
  } else {
    retErr = did;
  }
  /* Terminate access to the data space. */
  err= H5Sclose( sid );
  if (err< 0) {
    std::cout << "Error Closing Dataspace" << std::endl;
    retErr = err;
  }
  return retErr;
}

/**
 * @brief Writes the data of a pointer to an HDF5 file
 * @param loc_id The hdf5 object id of the parent
 * @param dsetName The name of the dataset to write to. This can be a name of Path
 * @param rank The number of dimensions
 * @param dims The sizes of each dimension
 * @param data The data to be written.
 * @return Standard hdf5 error condition.
 */
template <typename T>
static herr_t writePointerDataset (hid_t loc_id,
                            const std::string& dsetName,
                            int32_t   rank,
                            hsize_t* dims,
                            T* data)
{

  herr_t err    = -1;
  hid_t did     = -1;
  hid_t sid     = -1;
  herr_t retErr = 0;

  hid_t dataType = H5Lite::HDFTypeForPrimitive(data[0]);
  if(dataType == -1)
  {
    return -1;
  }
  //Create the DataSpace
  std::vector<uint64_t>::size_type size = static_cast<std::vector<uint64_t>::size_type>(rank);

  std::vector<hsize_t> _dims(size, 0);
  for (int32_t i = 0; i < rank; ++i)
  {
    _dims[i] = static_cast<hsize_t>(dims[i]);
  }
//  sid = H5Screate_simple( size, &(_dims.front()), NULL );
  sid = H5Screate_simple( rank, dims, NULL);
  if (sid < 0)
  {
    return sid;
  }
  // Create the Dataset
  did = H5Dcreate (loc_id, dsetName.c_str(), dataType, sid, H5P_DEFAULT);
  if ( did >= 0 )
  {
    err = H5Dwrite( did, dataType, H5S_ALL, H5S_ALL, H5P_DEFAULT, data );
    if (err < 0 ) {
      std::cout << "Error Writing Data" << std::endl;
      retErr = err;
    }
    err = H5Dclose( did );
    if (err < 0) {
      std::cout << "Error Closing Dataset." << std::endl;
      retErr = err;
    }
  } else {
    retErr = did;
  }
  /* Terminate access to the data space. */
  err= H5Sclose( sid );
  if (err< 0) {
    std::cout << "Error Closing Dataspace" << std::endl;
    retErr = err;
  }
  return retErr;
}



/**
 * @brief Creates a Dataset with the given name at the location defined by loc_id
 *
 * @param loc_id The Parent location to store the data
 * @param dsetName The name of the dataset
 * @param dims The dimensions of the dataset
 * @param rank The number of dimensions to the dataset
 * @param data The data to write to the file
 * @return Standard HDF5 error conditions
 *
 * The dimensions of the data sets are usually passed as both a "rank" and
 * dimensions array. By using a std::vector<hsize_t> that stores the values of
 * each of the dimensions we can reduce the number of arguments to this method as
 * the value of the "rank" simply becomes dims.length(). So to create a Dims variable
 * for a 3D data space of size(x,y,z) = {10,20,30} I would use the following code:
 * <code>
 * std::vector<hsize_t> dims;
 * dims.push_back(10);
 * dims.push_back(20);
 * dims.push_back(30);
 * </code>
 *
 */
template<typename T>
static herr_t writeDataset(hid_t loc_id,
                           const std::string& dsetName,
                           int32_t &rank,
                           hsize_t* dims,
                           T* data)
{
  herr_t err = -1;
  hid_t did = -1;
  hid_t sid = -1;
  herr_t retErr = 0;
  hid_t dataType = H5Lite::HDFTypeForPrimitive(data[0]);
  if (dataType == -1)
  {
    return -1;
  }
  //Create the DataSpace
  sid = H5Screate_simple( rank, dims, NULL );
  if (sid < 0)
  {
    return sid;
  }
  // Create the Dataset
  did = H5Dcreate (loc_id, dsetName.c_str(), dataType, sid, H5P_DEFAULT);
  if ( did >= 0 )
  {
    err = H5Dwrite( did, dataType, H5S_ALL, H5S_ALL, H5P_DEFAULT, data );
    if (err < 0 ) {
      std::cout << "Error Writing Data" << std::endl;
      retErr = err;
    }
    err = H5Dclose( did );
    if (err < 0) {
      std::cout << "Error Closing Dataset." << std::endl;
      retErr = err;
    }
  } else {
    retErr = did;
  }
  /* Terminate access to the data space. */
  err= H5Sclose( sid );
  if (err< 0) {
    std::cout << "Error Closing Dataspace" << std::endl;
    retErr = err;
  }
  return retErr;
}


/**
 * @brief Creates a Dataset with the given name at the location defined by loc_id.
 * This version of writeDataset should be used with a single scalar value. If you
 * need to write an array of values, use the form that takes an std::vector<>
 *
 * @param loc_id The Parent location to store the data
 * @param dsetName The name of the dataset
 * @param value The value to write to the HDF5 dataset
 * @return Standard HDF5 error conditions
 */
template <typename T>
static herr_t writeScalarDataset (hid_t loc_id,
                            const std::string& dsetName,
                            T &value)
{
  herr_t err = -1;
  hid_t did = -1;
  hid_t sid = -1;
  herr_t retErr = 0;
  hsize_t dims = 1;
  hid_t rank = 1;
  hid_t dataType = H5Lite::HDFTypeForPrimitive(value);
  if (dataType == -1)
  {
    return -1;
  }
  //Create the DataSpace
  sid = H5Screate_simple( rank, &(dims), NULL );
  if (sid < 0)
  {
    return sid;
  }
  // Create the Dataset
  did = H5Dcreate (loc_id, dsetName.c_str(), dataType, sid, H5P_DEFAULT);
  if ( did >= 0 )
  {
    err = H5Dwrite( did, dataType, H5S_ALL, H5S_ALL, H5P_DEFAULT, &value );
    if (err < 0 ) {
      std::cout << "Error Writing Data" << std::endl;
      retErr = err;
    }
    err = H5Dclose( did );
    if (err < 0) {
      std::cout << "Error Closing Dataset." << std::endl;
      retErr = err;
    }
  } else {
    retErr = did;
  }
  /* Terminate access to the data space. */
  err = H5Sclose( sid );
  if (err< 0) {
    std::cout << "Error Closing Dataspace" << std::endl;
    retErr = err;
  }
  return retErr;
}

/**
 * @brief Writes a std::string as a HDF Dataset.
 * @param loc_id The Parent location to write the dataset
 * @param dsetName The Name to use for the dataset
 * @param data The actual data to write as a null terminated string
 * @return Standard HDF5 error conditions
 */
static MXA_EXPORT herr_t  writeStringDataset (hid_t loc_id,
                                        const std::string& dsetName,
                                        const std::string& data);

/**
 * @brief Writes a null terminated 'C String' to an HDF Dataset.
 * @param loc_id The Parent location to write the dataset
 * @param dsetName The Name to use for the dataset
 * @param data const char pointer to write as a null terminated string
 * @param size The number of characters in the string
 * @return Standard HDF5 error conditions
 */
static MXA_EXPORT herr_t  writeStringDataset (hid_t loc_id,
                                        const std::string& dsetName,
                                        size_t size,
                                        const char* data);


/**
 * @brief Writes an Attribute to an HDF5 Object
 * @param loc_id The Parent Location of the HDFobject that is getting the attribute
 * @param objName The Name of Object to write the attribute into.
 * @param attrName The Name of the Attribute
 * @param rank The number of dimensions in the attribute data
 * @param dims The Dimensions of the attribute data
 * @param data The Attribute Data to write as a pointer
 * @return Standard HDF Error Condition
 */
template <typename T>
static herr_t writePointerAttribute(hid_t loc_id,
                             const std::string& objName,
                             const std::string& attrName,
                             int32_t   rank,
                             uint64_t* dims,
                             T* data)
{
  hid_t      obj_id, sid, attr_id;
  int32_t        has_attr;
  H5G_stat_t statbuf;
  herr_t err = 0;
  herr_t retErr = 0;
  T test = 0x00;
  hid_t dataType = H5Lite::HDFTypeForPrimitive(test);
  if (dataType == -1)
  {
    std::cout << DEBUG_OUT(logTime) << "dataType was unknown" << std::endl;
    return -1;
  }
  /* Get the type of object */
  if (H5Gget_objinfo(loc_id, objName.c_str(), 1, &statbuf) < 0) {
    std::cout << "Error getting object info." << std::endl;
    return -1;
  }
  /* Open the object */
  obj_id = H5Lite::openId( loc_id, objName, statbuf.type );
  if ( obj_id < 0) {
    std::cout << "Error opening Object for Attribute operations." << std::endl;
    return -1;
  }

  /* Create the data space for the attribute. */
  hsize_t* dimsPtr = 0x0;
 // size mismatch between hsize_t and size_t
  std::vector<hsize_t> _dims(rank, 0);
  for (int32_t i = 0; i < rank; ++i)
  {
    _dims[i] = static_cast<hsize_t>(dims[i]);
  }
  dimsPtr = &(_dims.front() );

  sid = H5Screate_simple( rank, dimsPtr, NULL );
  if ( sid >= 0 ) {
    /* Verify if the attribute already exists */
    has_attr = H5Lite::findAttribute( obj_id, attrName );

    /* The attribute already exists, delete it */
    if ( has_attr == 1 ) {
      err = H5Adelete( obj_id, attrName.c_str() );
      if (err < 0) {
        std::cout << "Error Deleting Existing Attribute" << std::endl;
        retErr = err;
      }
    }

    if ( err >= 0 ) {
      /* Create the attribute. */
      attr_id = H5Acreate( obj_id, attrName.c_str() , dataType, sid, H5P_DEFAULT );
      if ( attr_id >= 0 ) {
        /* Write the attribute data. */
        err = H5Awrite( attr_id, dataType, data );
        if ( err < 0 ) {
          std::cout << "Error Writing Attribute" << std::endl;
          retErr = err;
        }
      }
      /* Close the attribute. */
      err = H5Aclose( attr_id );
      if (err < 0) {
        std::cout << "Error Closing Attribute" << std::endl;
        retErr = err;
      }
    }
    /* Close the dataspace. */
    err = H5Sclose( sid );
    if ( err < 0 ) {
      std::cout << "Error Closing Dataspace" << std::endl;
      retErr = err;
    }
  }
  else
  {
    retErr = sid;
  }
  /* Close the object */
  err = H5Lite::closeId( obj_id, statbuf.type );
  if ( err < 0 ) {
    std::cout << "Error Closing HDF5 Object ID" << std::endl;
    retErr = err;
  }
  return retErr;
}


/**
 * @brief Writes an Attribute to an HDF5 Object
 * @param loc_id The Parent Location of the HDFobject that is getting the attribute
 * @param objName The Name of Object to write the attribute into.
 * @param attrName The Name of the Attribute
 * @param dims The Dimensions of the data set
 * @param data The Attribute Data to write
 * @return Standard HDF Error Condition
 *
 */
template <typename T>
static herr_t writeVectorAttribute(hid_t loc_id,
                             const std::string& objName,
                             const std::string& attrName,
                             std::vector<uint64_t> &dims,
                             std::vector<T> &data )
{
  hid_t      obj_id, sid, attr_id;
  //hsize_t    dim_size = data.size();
  int32_t        has_attr;
  H5G_stat_t statbuf;
  herr_t err = 0;
  herr_t retErr = 0;
  hid_t dataType = H5Lite::HDFTypeForPrimitive(data.front());
  if (dataType == -1)
  {
    return -1;
  }
  /* Get the type of object */
  if (H5Gget_objinfo(loc_id, objName.c_str(), 1, &statbuf) < 0) {
    std::cout << "Error getting object info." << std::endl;
    return -1;
  }
  /* Open the object */
  obj_id = H5Lite::openId( loc_id, objName, statbuf.type );
  if ( obj_id < 0) {
    std::cout << "Error opening Object for Attribute operations." << std::endl;
    return -1;
  }

  /* Create the data space for the attribute. */
  hsize_t* dimsPtr = 0x0;
  //size mismatch between hsize_t and size_t
  std::vector<uint64_t>::size_type _size = dims.size();
  //hsize_t _dims[ _size ];
  std::vector<hsize_t> _dims(_size, 0);
  for (std::vector<uint64_t>::size_type i = 0; i < _size; ++i)
  {
    _dims[i] = static_cast<hsize_t>(dims[i]);
  }
  dimsPtr = &(_dims.front() );

  sid = H5Screate_simple( static_cast<int>(dims.size()), dimsPtr, NULL );
  if ( sid >= 0 ) {
    /* Verify if the attribute already exists */
    has_attr = H5Lite::findAttribute( obj_id, attrName );

    /* The attribute already exists, delete it */
    if ( has_attr == 1 ) {
      err = H5Adelete( obj_id, attrName.c_str() );
      if (err < 0) {
        std::cout << "Error Deleting Existing Attribute" << std::endl;
        retErr = err;
      }
    }

    if ( err >= 0 ) {
      /* Create the attribute. */
      attr_id = H5Acreate( obj_id, attrName.c_str() , dataType, sid, H5P_DEFAULT );
      if ( attr_id >= 0 ) {
        /* Write the attribute data. */
        err = H5Awrite( attr_id, dataType, &(data.front()) );
        if ( err < 0 ) {
          std::cout << "Error Writing Attribute" << std::endl;
          retErr = err;
        }
      }
      /* Close the attribute. */
      err = H5Aclose( attr_id );
      if (err < 0) {
        std::cout << "Error Closing Attribute" << std::endl;
        retErr = err;
      }
    }
    /* Close the dataspace. */
    err = H5Sclose( sid );
    if ( err < 0 ) {
      std::cout << "Error Closing Dataspace" << std::endl;
      retErr = err;
    }
  }
  else
  {
    retErr = sid;
  }
  /* Close the object */
  err = H5Lite::closeId( obj_id, statbuf.type );
  if ( err < 0 ) {
    std::cout << "Error Closing HDF5 Object ID" << std::endl;
    retErr = err;
  }
  return retErr;
}

/**
 * @brief Writes a string as a null terminated attribute.
 * @param loc_id The location to look for objName
 * @param objName The Object to write the attribute to
 * @param attrName The name of the Attribute
 * @param data The string to write as the attribute
 * @return Standard HDF error conditions
 */
static MXA_EXPORT herr_t  writeStringAttribute(hid_t loc_id,
                              const std::string& objName,
                              const std::string& attrName,
                              const std::string& data);
/**
 * @brief Writes a null terminated string as an attribute
 * @param loc_id The location to look for objName
 * @param objName The Object to write the attribute to
 * @param attrName The name of the Attribute
 * @param size The number of characters  in the string
 * @param data pointer to a const char array
 * @return Standard HDF error conditions
 */
static MXA_EXPORT herr_t  writeStringAttribute(hid_t loc_id,
                              const std::string& objName,
                              const std::string& attrName,
                              hsize_t size,
                              const char* data);


/**
 * @brief Writes attributes that all have a data type of STRING. The first value
 * in each set is the key, the second is the actual value of the attribute.
 * @param loc_id The location to look for objName
 * @param objName The Object to write the attribute to
 * @param attributes The attributes to be written where the first value is the name
 * of the attribute, and the second is the actual value of the attribute.
 * @return Standard HDF error condition
 */
static MXA_EXPORT herr_t writeStringAttributes(hid_t loc_id,
                                     const std::string &objName,
                                     const std::map<std::string, std::string> &attributes);
/**
 * @brief Writes an attribute to the given object. This method is designed with
 * a Template parameter that represents a primitive value. If you need to write
 * an array, please use the other over loaded method that takes a vector.
 * @param loc_id The location to look for objName
 * @param objName The Object to write the attribute to
 * @param attrName The  name of the attribute
 * @param data The data to be written as the attribute
 * @return Standard HDF error condition
 */
template <typename T>
static herr_t  writeScalarAttribute(hid_t loc_id,
                             const std::string& objName,
                             const std::string& attrName,
                             T data )
{

  hid_t      obj_id, sid, attr_id;
  int32_t        has_attr;
  H5G_stat_t statbuf;
  herr_t err = 0;
  herr_t retErr = 0;
  hsize_t dims = 1;
  hid_t rank = 1;
  hid_t dataType = H5Lite::HDFTypeForPrimitive(data);
  if (dataType == -1)
  {
    return -1;
  }
  /* Get the type of object */
  if (H5Gget_objinfo(loc_id, objName.c_str(), 1, &statbuf) < 0) {
    std::cout << "Error getting object info." << std::endl;
    return -1;
  }
  /* Open the object */
  obj_id = H5Lite::openId( loc_id, objName, statbuf.type );
  if ( obj_id < 0) {
    std::cout << "Error opening Object for Attribute operations." << std::endl;
    return -1;
  }

  /* Create the data space for the attribute. */
  sid = H5Screate_simple( rank, &dims, NULL );
  if ( sid >= 0 ) {
    /* Verify if the attribute already exists */
    has_attr = H5Lite::findAttribute( obj_id, attrName );

    /* The attribute already exists, delete it */
    if ( has_attr == 1 ) {
      err = H5Adelete( obj_id, attrName.c_str() );
      if (err < 0) {
        std::cout << "Error Deleting Existing Attribute" << std::endl;
        retErr = err;
      }
    }

    if ( err >= 0 ) {
      /* Create the attribute. */
      attr_id = H5Acreate( obj_id, attrName.c_str() , dataType, sid, H5P_DEFAULT );
      if ( attr_id >= 0 ) {
        /* Write the attribute data. */
        err = H5Awrite( attr_id, dataType, &data );
        if ( err < 0 ) {
          std::cout << "Error Writing Attribute" << std::endl;
          retErr = err;
        }
      }
      /* Close the attribute. */
      err = H5Aclose( attr_id );
      if (err < 0) {
        std::cout << "Error Closing Attribute" << std::endl;
        retErr = err;
      }
    }
    /* Close the dataspace. */
    err = H5Sclose( sid );
    if ( err < 0 ) {
      std::cout << "Error Closing Dataspace" << std::endl;
      retErr = err;
    }
  }
  else
  {
    retErr = sid;
  }

  /* Close the object */
  err = H5Lite::closeId( obj_id, statbuf.type );
  if ( err < 0 ) {
    std::cout << "Error Closing HDF5 Object ID" << std::endl;
    retErr = err;
  }
  return retErr;
}

/**
 * @brief Reads data from the HDF5 File into a preallocated array.
 * @param loc_id The parent location that contains the dataset to read
 * @param dsetName The name of the dataset to read
 * @param data A Pointer to the PreAllocated Array of Data
 * @return Standard HDF error condition
 */
template <typename T>
static herr_t readPointerDataset(hid_t loc_id,
                                const std::string& dsetName,
                                T* data)
{
  hid_t did;
  herr_t err = 0;
  herr_t retErr = 0;
  hid_t dataType = 0;
  T test = 0x00;
  dataType = H5Lite::HDFTypeForPrimitive(test);
  if (dataType == -1)
  {
    std::cout << DEBUG_OUT(logTime) << "dataType was not supported." << std::endl;
    return -10;
  }
  if (loc_id < 0)
  {
    std::cout << DEBUG_OUT(logTime) << "loc_id was Negative: This is not allowed." << std::endl;
    return -2;
  }
  if (NULL == data)
  {
    std::cout << DEBUG_OUT(logTime) << "The Pointer to hold the data is NULL. This is NOT allowed." << std::endl;
    return -3;
  }
  did = H5Dopen( loc_id, dsetName.c_str() );
  if ( did < 0 )
  {
    std::cout << DEBUG_OUT(logTime) << " Error opening Dataset: " << did << std::endl;
    return -1;
  }
  if ( did >= 0 )
  {
    err = H5Dread(did, dataType, H5S_ALL, H5S_ALL, H5P_DEFAULT, data );
    if (err < 0)
    {
      std::cout << DEBUG_OUT(logTime) << "Error Reading Data." << std::endl;
      retErr = err;
    }
    err = H5Dclose( did );
    if (err < 0 )
    {
      std::cout << DEBUG_OUT(logTime) << "Error Closing Dataset id" << std::endl;
      retErr = err;
    }
  }
  return retErr;
}


/**
 * @brief Reads data from the HDF5 File into an std::vector<T> object. If the dataset
 * is very large this can be an expensive method to use. It is here for convenience
 * using STL with hdf5.
 * @param loc_id The parent location that contains the dataset to read
 * @param dsetName The name of the dataset to read
 * @param data A std::vector<T>. Note the vector WILL be resized to fit the data.
 * The best idea is to just allocate the vector but not to size it. The method
 * will size it for you.
 * @return Standard HDF error condition
 */
template <typename T>
static herr_t readVectorDataset(hid_t loc_id,
                          const std::string& dsetName,
                          std::vector<T> &data)
{
  hid_t   did;
  herr_t  err = 0;
  herr_t retErr = 0;
  hid_t spaceId;
  hid_t dataType;
  T test = static_cast<T>(0x00);
  dataType = H5Lite::HDFTypeForPrimitive(test);
  if (dataType == -1)
  {
    return -1;
  }
  //std::cout << "HDF5 Data Type: " << H5Lite::HDFTypeForPrimitiveAsStr(test) << std::endl;
 /* Open the dataset. */
// std::cout << "  Opening " << dsetName << " for data Retrieval.  " << std::endl;
  did = H5Dopen( loc_id, dsetName.c_str() );
  if ( did < 0 ) {
    std::cout << " Error opening Dataset: " << did << std::endl;
    return -1;
  }
  if ( did >= 0 ) {
    spaceId = H5Dget_space(did);
    if ( spaceId > 0 ) {
      int32_t rank = H5Sget_simple_extent_ndims(spaceId);
      if (rank > 0) {
        std::vector<hsize_t> dims;
        dims.resize(rank);// Allocate enough room for the dims
        rank = H5Sget_simple_extent_dims(spaceId, &(dims.front()), NULL);
        hsize_t numElements = 1;
        for (std::vector<hsize_t>::iterator iter = dims.begin(); iter < dims.end(); ++iter ) {
          numElements = numElements * (*iter);
        }
       // std::cout << "NumElements: " << numElements << std::endl;
        //Resize the vector
        data.resize( static_cast<int>(numElements) );
       // for (uint32_t i = 0; i<numElements; ++i) { data[i] = 55555555;  }
        err = H5Dread(did, dataType, H5S_ALL, H5S_ALL, H5P_DEFAULT, &( data.front() ) );
        if (err < 0) {
          std::cout << "Error Reading Data." << std::endl;
          retErr = err;
        }
      }
      err = H5Sclose(spaceId);
      if (err < 0 ) {
        std::cout << "Error Closing Data Space" << std::endl;
        retErr = err;
      }
    }
    else
    {
      std::cout << "Error Opening SpaceID" << std::endl;
      retErr = spaceId;
    }
    err = H5Dclose( did );
    if (err < 0 ) {
      std::cout << "Error Closing Dataset" << std::endl;
      retErr = err;
    }
  }
  return retErr;
}

/**
 * @brief Reads a dataset that consists of a single scalar value
 * @param loc_id The HDF5 file or group id
 * @param dsetName The name or path to the dataset to read
 * @param data The variable to store the data into
 * @return HDF error condition.
 */
template <typename T>
static herr_t readScalarDataset(hid_t loc_id,
                          const std::string& dsetName,
                          T &data)
{
  hid_t   did;
  herr_t  err = 0;
  herr_t retErr = 0;
  hid_t spaceId;

  hid_t dataType = H5Lite::HDFTypeForPrimitive(data);
  if (dataType == -1)
  {
    return -1;
  }
 /* Open the dataset. */
  did = H5Dopen( loc_id, dsetName.c_str() );
  if ( did < 0 ) {
    std::cout << "Error opening Dataset." << std::endl;
    return -1;
  }
  if ( did >= 0 ) {
    spaceId = H5Dget_space(did);
    if ( spaceId > 0 ) {
      int32_t rank = H5Sget_simple_extent_ndims(spaceId);
      if (rank > 0) {
        std::vector<hsize_t> dims;
        dims.resize(rank);// Allocate enough room for the dims
        rank = H5Sget_simple_extent_dims(spaceId, &(dims.front()), NULL);
        hsize_t numElements = 1;
        for (std::vector<hsize_t>::iterator iter = dims.begin(); iter < dims.end(); ++iter ) {
          numElements = numElements * (*iter);
        }
        err = H5Dread(did, dataType, H5S_ALL, H5S_ALL, H5P_DEFAULT, &data );
        if (err < 0) {
          std::cout << "Error Reading Data." << std::endl;
          retErr = err;
        }
      }
      err = H5Sclose(spaceId);
      if (err < 0 ) {
        std::cout << "Error Closing Data Space" << std::endl;
        retErr = err;
      }
    }
    else
    {
      retErr = spaceId;
    }
    err = H5Dclose( did );
    if (err < 0 ) {
      std::cout << "Error Closing Dataset" << std::endl;
      retErr = err;
    }
  }
  return retErr;
}


/**
 * @brief Reads a string dataset into the supplied string.
 * @param loc_id The parent group that holds the data object to read
 * @param dsetName The name of the dataset.
 * @param data The std::string to hold the data
 * @return Standard HDF error condition
 */
static MXA_EXPORT herr_t readStringDataset(hid_t loc_id,
                                     const std::string& dsetName,
                                     std::string &data);



/**
 * @brief reads a nullterminated string dataset into the supplied buffer. The buffer
 * should be already preallocated.
 * @param loc_id The parent group that holds the data object to read
 * @param dsetName The name of the dataset.
 * @param data pointer to the buffer
 * @return Standard HDF error condition
 */
static MXA_EXPORT herr_t readStringDataset(hid_t loc_id,
                                           const std::string &dsetName,
                                           uint8_t* data);

/**
 * @brief Reads an Attribute from an HDF5 Object.
 *
 * Use this method if you already know the datatype of the attribute. If you do
 * not know this already then use another form of this method.
 *
 * @param loc_id The Parent object that holds the object to which you want to read an attribute
 * @param objName The name of the object to which the attribute is to be read
 * @param attrName The name of the Attribute to read
 * @param data The memory to store the data
 * @return Standard HDF Error condition
 */
template <typename T>
static herr_t readVectorAttribute(hid_t loc_id,
                            const std::string& objName,
                            const std::string& attrName,
                            std::vector<T> &data)
{
  /* identifiers */
  hid_t      obj_id;
  H5G_stat_t statbuf;
  herr_t err = 0;
  herr_t retErr = 0;
  hid_t attr_id;
  hid_t tid;
  T test = 0x00;
  hid_t dataType = H5Lite::HDFTypeForPrimitive(test);
  if (dataType == -1)
  {
    return -1;
  }
  //std::cout << "   Reading Vector Attribute at Path '" << objName << "' with Key: '" << attrName << "'" << std::endl;
  /* Get the type of object */
  err = H5Gget_objinfo(loc_id, objName.c_str(), 1, &statbuf);
  if (err<0)
    return err;
  /* Open the object */
  obj_id = H5Lite::openId( loc_id, objName, statbuf.type);
  if ( obj_id >= 0)
  {
    attr_id = H5Aopen_name( obj_id, attrName.c_str() );
    if ( attr_id >= 0 )
    {
      //Need to allocate the array size
      H5T_class_t type_class;
      size_t type_size;
      std::vector<uint64_t> dims;
      err = H5Lite::getAttributeInfo(loc_id, objName, attrName, dims, type_class, type_size, tid);
      hsize_t numElements = 1;
      for (std::vector<uint64_t>::iterator iter = dims.begin(); iter < dims.end(); ++iter )
      {
        numElements *= *(iter);
      }
      //std::cout << "    Vector Attribute has " << numElements << " elements." << std::endl;
      data.resize( static_cast<signed int>(numElements) );
      err = H5Aread( attr_id, dataType, &(data.front()) );
      if ( err < 0 ) {
        std::cout << "Error Reading Attribute." << err << std::endl;
        retErr = err;
      }
      err = H5Aclose( attr_id );
      if ( err < 0 ) {
        std::cout << "Error Closing Attribute" << std::endl;
        retErr = err;
      }
    }
    else
    {
      retErr = attr_id;
    }
    err = H5Lite::closeId( obj_id, statbuf.type );
    if ( err < 0 ) {
     std::cout << "Error Closing Object" << std::endl;
     retErr = err;
    }
  }
  return retErr;
}

/**
 * @brief Reads a scalar attribute value from a dataset
 * @param loc_id
 * @param objName The name of the dataset
 * @param attrName The name of the Attribute
 * @param data The preallocated memory for the variable to be stored into
 * @return Standard HDF5 error condition
 */
template <typename T>
static herr_t  readScalarAttribute(hid_t loc_id,
                            const std::string& objName,
                            const std::string& attrName,
                            T &data)
{

  /* identifiers */
  hid_t      obj_id;
  H5G_stat_t statbuf;
  herr_t err = 0;
  herr_t retErr = 0;
  hid_t attr_id;
  T test = 0x00;
  hid_t dataType = H5Lite::HDFTypeForPrimitive(test);
  if (dataType == -1)
  {
    return -1;
  }
  //std::cout << "Reading Scalar style Attribute at Path '" << objName << "' with Key: '" << attrName << "'" << std::endl;
  /* Get the type of object */
  err = H5Gget_objinfo(loc_id, objName.c_str(), 1, &statbuf);
  if (err<0)
    return err;
  /* Open the object */
  obj_id = H5Lite::openId( loc_id, objName, statbuf.type);
  if ( obj_id >= 0)
  {
    attr_id = H5Aopen_name( obj_id, attrName.c_str() );
    if ( attr_id >= 0 )
    {
      err = H5Aread( attr_id, dataType, &data );
      if ( err < 0 ) {
        std::cout << "Error Reading Attribute." << std::endl;
        retErr = err;
      }
      err = H5Aclose( attr_id );
      if ( err < 0 ) {
        std::cout << "Error Closing Attribute" << std::endl;
        retErr = err;
      }
    }
    else
    {
      retErr = attr_id;
    }
    err = H5Lite::closeId( obj_id, statbuf.type );
    if ( err < 0 ) {
     std::cout << "Error Closing Object" << std::endl;
     retErr = err;
    }
  }
  return retErr;
}

/**
 * @brief Reads the Attribute into a pre-allocated pointer
 * @param loc_id
 * @param objName The name of the dataset
 * @param attrName The name of the Attribute
 * @param data The preallocated memory for the variable to be stored into
 * @return Standard HDF5 error condition
 */
template <typename T>
static herr_t readPointerAttribute(hid_t loc_id,
                            const std::string& objName,
                            const std::string& attrName,
                            T* data)
{
  /* identifiers */
  hid_t      obj_id;
  H5G_stat_t statbuf;
  herr_t err = 0;
  herr_t retErr = 0;
  hid_t attr_id;
  T test = 0x00;
  hid_t dataType = H5Lite::HDFTypeForPrimitive(test);
  if (dataType == -1)
  {
    return -1;
  }
  //std::cout << "   Reading Vector Attribute at Path '" << objName << "' with Key: '" << attrName << "'" << std::endl;
  /* Get the type of object */
  err = H5Gget_objinfo(loc_id, objName.c_str(), 1, &statbuf);
  if (err<0)
    return err;
  /* Open the object */
  obj_id = H5Lite::openId( loc_id, objName, statbuf.type);
  if ( obj_id >= 0)
  {
    attr_id = H5Aopen_name( obj_id, attrName.c_str() );
    if ( attr_id >= 0 )
    {
      err = H5Aread( attr_id, dataType, data);
      if ( err < 0 ) {
        std::cout << "Error Reading Attribute." << err << std::endl;
        retErr = err;
      }
      err = H5Aclose( attr_id );
      if ( err < 0 ) {
        std::cout << "Error Closing Attribute" << std::endl;
        retErr = err;
      }
    }
    else
    {
      retErr = attr_id;
    }
    err = H5Lite::closeId( obj_id, statbuf.type );
    if ( err < 0 ) {
     std::cout << "Error Closing Object" << std::endl;
     retErr = err;
    }
  }
  return retErr;
}

/**
 * @brief Reads a string attribute from an HDF object
 * @param loc_id The Parent object that holds the object to which you want to read an attribute
 * @param objName The name of the object to which the attribute is to be read
 * @param attrName The name of the Attribute to read
 * @param data The memory to store the data
 * @return Standard HDF Error condition
 */
static MXA_EXPORT herr_t readStringAttribute(hid_t loc_id,
                                             const std::string& objName,
                                             const std::string& attrName,
                                             std::string &data);

/**
 * @brief Reads a string attribute from an HDF object into a precallocated buffer
 * @param loc_id The Parent object that holds the object to which you want to read an attribute
 * @param objName The name of the object to which the attribute is to be read
 * @param attrName The name of the Attribute to read
 * @param data The memory to store the data into
 * @return Standard HDF Error condition
 */
static MXA_EXPORT herr_t readStringAttribute(hid_t loc_id,
                                   const std::string& objName,
                                   const std::string& attrName,
                                   uint8_t* data);
/**
 * @brief Returns the number of dimensions for a given attribute
 * @param loc_id The HDF5 id of the parent group/file for the objName
 * @param objName The name of the dataset
 * @param attrName The name of the attribute
 * @param rank (out) Number of dimensions is store into this variable
 */
static MXA_EXPORT hid_t getAttributeNDims(hid_t loc_id, const std::string& objName, const std::string& attrName, hid_t &rank);

/**
 * @brief Returns the number of dimensions for a given dataset
 * @param loc_id The HDF5 id of the parent group/file for the objName
 * @param objName The name of the dataset
 * @param rank (out) Number of dimensions is store into this variable
 */
static MXA_EXPORT hid_t getDatasetNDims(hid_t loc_id, const std::string& objName, hid_t &rank);

/**
 * @brief Returns the H5T value for a given dataset.
 *
 * Returns the type of data stored in the dataset. You MUST use H5Tclose(tid)
 * on the returned value or resource leaks will occur.
 * @param loc_id A Valid HDF5 file or group id.
 * @param dsetName Path to the dataset
 * @return
 */
static MXA_EXPORT hid_t getDatasetType(hid_t loc_id, const std::string &dsetName);

/**
 * @brief Get the information about a dataset.
 *
 * @param loc_id The parent location of the Dataset
 * @param dsetName The name of the dataset
 * @param dims A std::vector that will hold the sizes of the dimensions
 * @param type_class The HDF5 class type
 * @param type_size THe HDF5 size of the data
 * @return Negative value is Failure. Zero or Positive is success;
 */
static MXA_EXPORT herr_t getDatasetInfo( hid_t loc_id,
                              const std::string& dsetName,
                              std::vector<hsize_t> &dims,
                              H5T_class_t &type_class,
                              size_t &type_size );

/**
 * @brief Returns the information about an attribute.
 * You must close the attributeType argument or resource leaks will occur. Use
 *  H5Tclose(attr_type); after your call to this method if you do not need the id for
 *   anything.
 * @param loc_id The parent location of the Dataset
 * @param objName The name of the dataset
 * @param attr_name The name of the attribute
 * @param dims A std::vector that will hold the sizes of the dimensions
 * @param type_class The HDF5 class type
 * @param type_size THe HDF5 size of the data
 * @param attr_type The Attribute ID - which needs to be closed after you are finished with the data
 * @return
 */
static MXA_EXPORT herr_t getAttributeInfo(hid_t loc_id,
                               const std::string& objName,
                               const std::string& attr_name,
                               std::vector<uint64_t> &dims,
                               H5T_class_t &type_class,
                               size_t &type_size,
                               hid_t &attr_type);




#ifdef H5LITE_USE_MXA_CONSTRUCTS
/**
 * @brief Writes the data contained within an IMXAArray object into an hdf5 file
 * @param loc_id The parent ID of the dataset
 * @param dsetName The name of the dataset, Can be a name or path within the hdf5 file. If
 * it represents a path YOU need to make sure all intermediate groups are already
 * crreated.
 * @param array The IMXAArray class
 * @return Standard hdf5 error condition
 */
static MXA_EXPORT herr_t writeMXAArray(hid_t loc_id,
                            const std::string &dsetName,
                            IMXAArray* array);
/**
 * @brief Writes the data contained within an IMXAArray object into an hdf5 file
 * @param loc_id The parent ID of the dataset
 * @param dsetName The name of the dataset, Can be a name or path within the hdf5 file. If
 * it represents a path YOU need to make sure all intermediate groups are already
 * crreated.
 * @param attributeKey The name of the attribute
 * @param array The IMXAArray class
 * @return Standard hdf5 error condition
 */
static MXA_EXPORT herr_t writeMXAAttribute(hid_t loc_id,
                            const std::string &dsetName,
                            const std::string &attributeKey,
                            IMXAArray* array);


/**
 * @brief Reads the data from an HDF5 file into a newly allocated MXAArrayTemplate
 * object - PLEASE NOTE: You MUST call delete on the object or otherwise make
 * arrangements to clean up the pointer otherwise memory leaks will occur.
 * @param loc_id The HDF5 file or group Id into which to store the data
 * @param dsetName The name/path of the data set
 * @return IMXAArray Pointer. NULL value is possible on error
 */
static MXA_EXPORT IMXAArray* readMXAArray(hid_t loc_id,
                           const std::string &dsetName );


/**
 * @brief Reads the attribute data from an HDF5 file into a newly allocated MXAArrayTemplate
 * object - PLEASE NOTE: You MUST call delete on the object or otherwise make
 * arrangements to clean up the pointer otherwise memory leaks will occur.
 * @param loc_id The HDF5 file or group Id into which to store the data
 * @param dsetName The name/path of the data set
 * @param attributeKey The name of the attribute
 * @return IMXAArray Pointer. NULL value is possible on error
 */
static MXA_EXPORT IMXAArray* readMXAAttribute(hid_t loc_id,
                                   const std::string &dsetName,
                                   const std::string &attributeKey);


#endif

// -----------------------------------------------------------------------------
protected:
  H5Lite();
  ~H5Lite();


private:
   void NEVER_USED() {
     herr_t ret = H5Aiterate( 0, NULL, find_attr, (void *)(NULL) );
     ret = H5Giterate( 0, NULL, 0, find_dataset, (void *)(NULL) );
     ret =0;
   }
};

#endif
