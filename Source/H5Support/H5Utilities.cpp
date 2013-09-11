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
#include <H5Support/H5Utilities.h>

// C Includes
#include <string.h>

// C++ Includes
#include <iostream>

#include "H5Support/H5Lite.h"

#ifdef H5LITE_USE_MXA_CONSTRUCTS
#include <MXA/DataWrappers/MXAAsciiStringData.h>
#endif

#define CheckValidLocId(locId)\
  if (locId < 0 ) {std::cout << "Invalid HDF Location ID: " << locId << std::endl;return -1;}

#if defined (H5Support_NAMESPACE)
using namespace H5Support_NAMESPACE;
#endif


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
hid_t H5Utilities::createFile(const QString &filename)
{
// HDF_ERROR_HANDLER_OFF
  //Create the HDF File
  hid_t fileId = H5Fcreate(filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
// HDF_ERROR_HANDLER_ON
  return fileId;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
hid_t H5Utilities::openFile(const QString &filename, bool readOnly)
{
  HDF_ERROR_HANDLER_OFF
  hid_t fileId = -1;
  if ( readOnly ) {
    fileId = H5Fopen(filename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
  } else {
    fileId = H5Fopen(filename.c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
  }

  HDF_ERROR_HANDLER_ON
  return fileId;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t H5Utilities::closeFile(hid_t &fileId)
{
  herr_t err = 1;
  if (fileId < 0) {  // fileId isn't open
    return 1;
  }


  // Get the number of open identifiers of all types
  //  except files
  ssize_t num_open = H5Fget_obj_count(fileId, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
            H5F_OBJ_DATATYPE | H5F_OBJ_ATTR);
  if (num_open > 0) {
    std::cout << "WARNING: Some IDs weren't closed. Closing them."  << std::endl;
    QVector<hid_t> attr_ids(num_open, 0);
    H5Fget_obj_ids(fileId, H5F_OBJ_DATASET | H5F_OBJ_GROUP |
       H5F_OBJ_DATATYPE | H5F_OBJ_ATTR,
       num_open, &(attr_ids.front()) );
    for (int i=0; i<num_open; i++)
    {
      char name[1024];
      ::memset(name, 0, 1024);
      //hid_t obj_type = H5Iget_type(attr_ids[i]);
      ssize_t charsRead = H5Iget_name( attr_ids[i], name, 1024);
      if (charsRead < 0)
      {
        std::cout << "Error Trying to get the name of an hdf object that was not closed. This is probably pretty bad. " << __FILE__ << "(" << __LINE__ << ")" << std::endl;
        return -1;
      }
      std::cout << "H5 Object left open. Id=" << attr_ids[i]  << " Name='" << name << "'" << std::endl;
      H5Utilities::closeHDF5Object(attr_ids[i]);
    }
  }

  err = H5Fclose(fileId);
  if (err < 0) {
    std::cout << "Error Closing HDF5 File. " << err << std::endl;
  }
  fileId= -1;
  return err;
}

// -----------------------------------------------------------------------------
//  Returns the full path to the object referred to by the
// -----------------------------------------------------------------------------
QString H5Utilities::getObjectPath(hid_t loc_id, bool trim)
{
  //char *obj_name;
  size_t name_size;
  name_size = 1 + H5Iget_name(loc_id, NULL, 0);
  QVector<char> obj_name(name_size, 0);
  H5Iget_name(loc_id, &(obj_name.front()), name_size);
  QString objPath(&(obj_name.front()));

  if ((objPath != "/") &&
      (objPath.at(0) == '/')) {
    objPath.erase(0, 1);
  }

  return objPath;
}


// -----------------------------------------------------------------------------
// @brief Retrieves the HDF object type for obj_name at loc_id and stores
//    it in the parameter obj_type passed in.
// -----------------------------------------------------------------------------
herr_t H5Utilities::getObjectType(hid_t objId, const QString &objName, int32_t *objType)
{
  herr_t err=1;
  H5O_info_t obj_info;

  err = H5Oget_info_by_name(objId, objName.c_str(),  &obj_info, H5P_DEFAULT);
  if (err < 0) {
    return err;
  }

  (* objType) = obj_info.type;

  return err;
}



// Opens and returns the HDF object (since the HDF api requires
//  different open and close methods for different types of objects
hid_t H5Utilities::openHDF5Object(hid_t loc_id, const QString &objName)
{
  int32_t obj_type;
  hid_t obj_id;
  herr_t err=0;
  HDF_ERROR_HANDLER_OFF;
  err = getObjectType(loc_id, objName, &obj_type);
  if (err < 0) {
   // std::cout << "Error: Unable to get object type for object: " << objName << std::endl;
    HDF_ERROR_HANDLER_ON;
    return -1;
  }

  switch (obj_type) {
    case H5O_TYPE_GROUP:
      obj_id = H5Gopen(loc_id, objName.c_str(), H5P_DEFAULT);
      break;
    case H5O_TYPE_DATASET:
      obj_id = H5Dopen(loc_id, objName.c_str(), H5P_DEFAULT);
      break;
    default:
      std::cout << "Unknonwn HDF Type: " << obj_type << std::endl;
      obj_id = -1;
  }
  HDF_ERROR_HANDLER_ON;
  return obj_id;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t H5Utilities::closeHDF5Object(hid_t obj_id)
{
  if (obj_id < 0) // Object was not valid.
  {
    return 0;
  }
  H5I_type_t obj_type;
  herr_t err=-1;  // default to an error
  char name[1024];
  ::memset(name, 0, 1024);
  obj_type = H5Iget_type(obj_id);
  ssize_t charsRead = H5Iget_name( obj_id, name, 1024);
  if (charsRead < 0)
  {
    std::cout << "Error Trying to get the name of an hdf object that was not closed. This is probably pretty bad. " << __FILE__ << "(" << __LINE__ << ")" << std::endl;
    return -1;
  }

  switch(obj_type) {
  case H5I_FILE:
    err = H5Fclose(obj_id);
    break;
  case H5I_GROUP:
    //std::cout << "H5 Group Object left open. Id=" << obj_id  << " Name='" << name << "'" << std::endl;
    err = H5Gclose(obj_id);
    break;
  case H5I_DATASET:
   // std::cout << "H5 Dataset Object left open. Id=" << obj_id << " Name='" << name << "'" << std::endl;
    err = H5Dclose(obj_id);
    break;
  case H5I_ATTR:
   // std::cout << "H5 Attribute Object left open. Id=" << obj_id << " Name='" << name << "'" << std::endl;
    err = H5Aclose(obj_id);
    break;
  case H5I_DATATYPE:
   // std::cout << "H5 DataType Object left open. Id=" << obj_id << " Name='" << name << "'" << std::endl;
    err = H5Tclose(obj_id);
    break;
  case H5I_DATASPACE:
   // std::cout << "H5 Data Space Object left open. Id=" << obj_id << " Name='" << name << "'" << std::endl;
    err = H5Sclose(obj_id);
    break;
  default:
   // std::cout << "Error unknown HDF object for closing: " << " Name='" << name << "'" << " Object Type=" << obj_type << std::endl;
    err = -1;
  }

  return err;
}




//--------------------------------------------------------------------//
// HDF Group Methods
//--------------------------------------------------------------------//

/*! @brief Returns a std::list of std::strings containing the names
 *   of all objects attached to the group referred to by loc_id
 *
 * @parameter typeFilter is one of MXA_GROUP, HDF5_DATASET, HDF5_TYPE,
 *  or HDF5_LINK or any combination of these using the bitwise or |
 *  command.  Or you can pass in HDF5_ANY (equivalent to: HDF5_GROUP |
 *  HDF5_DATASET | HDF5_TYPE | HDF5_LINK) to not filter at all
 */
herr_t H5Utilities::getGroupObjects(hid_t loc_id, int32_t typeFilter, std::list<std::string>& names)
{
  herr_t err=0;
  hsize_t numObjs = 0;
  H5G_info_t group_info;
  err = H5Gget_info(loc_id, &group_info);
  if (err < 0) {
    //std::cout << "Error getting number of objects for group: " << loc_id << std::endl;
    return err;
  }
  numObjs = group_info.nlinks;

  if (numObjs <= 0) {
    return 0; // no objects in group
  }

  size_t size=0;
  H5O_type_t type = H5O_TYPE_NTYPES;

  for (hsize_t i=0; i<numObjs; i++)
  {
    size = 1 + H5Lget_name_by_idx(loc_id, ".", H5_INDEX_NAME, H5_ITER_INC, i, NULL, 0, H5P_DEFAULT);

    QVector<char> name(size * sizeof(char), 0);

    H5Lget_name_by_idx(loc_id, ".", H5_INDEX_NAME, H5_ITER_INC, i, &(name.front()), size, H5P_DEFAULT);
    if (typeFilter == H5Support_ANY)
    {
      QString objName( &(name.front()) );
      names.push_back(objName);
    }
    else
    {
      H5O_info_t object_info;
      err = H5Oget_info_by_name( loc_id, &(name.front()), &object_info, H5P_DEFAULT);
      if (err >= 0)
      {
        type = object_info.type;
        if ( ((type == H5O_TYPE_GROUP) && (H5Support_GROUP & typeFilter)) ||
            ((type == H5O_TYPE_DATASET) && (H5Support_DATASET & typeFilter)) )
        {
          QString objName( &(name.front()) );
          names.push_back(objName);
        }
      }
    }
  }

  return err;
}

// -----------------------------------------------------------------------------
// HDF Creation/Modification Methods
// -----------------------------------------------------------------------------
hid_t H5Utilities::createGroup(hid_t loc_id, const QString &group)
{
  hid_t grp_id = -1;
  herr_t err = -1;
  H5O_info_t obj_info;
  HDF_ERROR_HANDLER_OFF

  err = H5Oget_info_by_name(loc_id, group.c_str(),  &obj_info, H5P_DEFAULT);
//  std::cout << "H5Gget_objinfo = " << err << " for " << group << std::endl;
  if (err == 0)
  {
    grp_id = H5Gopen(loc_id, group.c_str(), H5P_DEFAULT);
  }
  else
  {
    grp_id = H5Gcreate(loc_id, group.c_str(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  }
  // Turn the HDF Error handlers back on
  HDF_ERROR_HANDLER_ON

  return grp_id;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t H5Utilities::createGroupsForDataset(const QString &datasetPath, hid_t parent)
{
  // Generate the internal HDF dataset path and create all the groups necessary to write the dataset
  std::string::size_type pos = 0;
  pos = datasetPath.find_last_of("/");
  //QString parentPath;
  if (pos != 0 && pos != std::string::npos)
  {
    QString parentPath ( datasetPath.substr(0, pos)  );
    return H5Utilities::createGroupsFromPath(parentPath, parent);
  }
  //Make sure all the intermediary groups are in place in the HDF5 File
  return 1;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t H5Utilities::createGroupsFromPath(const QString &pathToCheck, hid_t parent)
{

  hid_t gid = 1;
  herr_t err = -1;
  QString first;
  QString second;
  QString path (pathToCheck); //make a copy of the input


  if (parent <= 0) {
    std::cout << "Bad parent Id. Returning from createGroupsFromPath" << std::endl;
    return -1;
  }
  // remove any front slash
  std::string::size_type pos = path.find_first_of("/", 0);
  if ( 0 == pos )
  {
    path = path.substr(1, path.size());
  }
  else if (pos == std::string::npos) // Path contains only one element
  {
    gid = H5Utilities::createGroup(parent, path);
    if (gid < 0)
    {
      std::cout << "Error creating group: " << path << " err:" << gid << std::endl;
      return gid;
    }
    err = H5Gclose(gid);
    if (err < 0) { std::cout  << "Error closing group during group creation." << std::endl; return err; }
    return err; //Now return here as this was a special case.
  }

  //Remove any trailing slash
  pos = path.find_last_of("/");
  if ( pos == (path.size() - 1) ) // slash was in the last position
  {
    path = path.substr(0, pos);
  }

  if (path.size() == 0)
  {
    return -1; // The path that was passed in was only a slash..
  }


  pos = path.find_first_of("/", 0);
  if (pos == std::string::npos) // Only one element in the path
  {
    gid = H5Utilities::createGroup(parent, path);
    if (gid < 0)
    {
      std::cout << "Error creating group '" << path << "' for group id " << gid << std::endl;
      return gid;
    }
    err = H5Gclose(gid);
    return err;
  }

  while (pos != std::string::npos )
  {
    first = path.substr(0, pos);
    second = path.substr(pos+1, path.length() );
    gid = H5Utilities::createGroup(parent,first);
    if (gid < 0)
    {
      std::cout << "Error creating group:" << gid << std::endl;
      return gid;
    }
    err = H5Gclose(gid);
    pos = path.find_first_of("/", pos+1);
    if (pos == std::string::npos)
    {
      first += "/" + second;
      gid = createGroup(parent,first);
      if (gid < 0)
      {
        std::cout << "Error creating group:" << gid << std::endl;
        return gid;
      }
      err = H5Gclose(gid);
    }
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString H5Utilities::extractObjectName(const QString &path)
{
  std::string::size_type pos;
  pos = path.find_last_of('/');
  if (pos == std::string::npos)
  {
    return path;
  }
  return path.substr(pos);
}


//--------------------------------------------------------------------//
// HDF Attribute Methods
//--------------------------------------------------------------------//
bool H5Utilities::probeForAttribute(hid_t loc_id,
                                    const QString &obj_name,
                                    const QString &attr_name)
{
  herr_t err=0;
  int32_t rank;
  HDF_ERROR_HANDLER_OFF
  err = H5Lite::getAttributeNDims(loc_id, obj_name, attr_name, rank);
  HDF_ERROR_HANDLER_ON
  if (err < 0)
  {
    return false;
  }
  return true;
}

//--------------------------------------------------------------------//
// Returns a std::list of all attribute names attached to the object
//  referred to by obj_id
//--------------------------------------------------------------------//
herr_t H5Utilities::getAllAttributeNames(hid_t obj_id,
                                         std::list<std::string> &results)
{
  if (obj_id < 0) { return -1; }
  herr_t err = -1;
  hsize_t num_attrs;
  hid_t attr_id;
  size_t name_size;
  H5O_info_t object_info;
  err = H5Oget_info(obj_id, &object_info);
  num_attrs = object_info.num_attrs;

  for (hsize_t i=0; i<num_attrs; i++)
  {
    attr_id = H5Aopen_by_idx(obj_id, ".", H5_INDEX_NAME, H5_ITER_INC, i, H5P_DEFAULT, H5P_DEFAULT);
    name_size = 1 + H5Aget_name(attr_id, 0, NULL);
    QVector<char> attr_name(name_size * sizeof(char), 0);
    H5Aget_name(attr_id, name_size, &(attr_name.front() ) );
    results.push_back(std::string( &(attr_name.front() ) ) );
    err = H5Aclose(attr_id);
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
 herr_t H5Utilities::getAllAttributeNames(hid_t loc_id,
                                           const QString &obj_name,
                                           std::list<std::string> &names)
{
  hid_t obj_id = -1;
  herr_t err = -1;
  names.clear();

  obj_id = openHDF5Object(loc_id, obj_name);
  if (obj_id < 0) {
    return obj_id;
  }
  err = getAllAttributeNames(obj_id, names);
  err = closeHDF5Object(obj_id);

  return err;
}

#if 0
 // Returns a QMap with all of the attributes for obj_name
 //  and their attribute values in a QString QMap
 herr_t H5Utilities::getAttributesMap(hid_t loc_id,
                                      const QString &obj_name,
                                      QMap<std::string, std::string> &attributes)
 {
   //QMap<std::string, std::string> attributes;
   herr_t err=0;
   H5T_class_t attr_type;
   size_t attr_size;
   QString res;
   hid_t tid;
   MXATypes::Int32Vector ires;
   MXATypes::Float32Vector fres;
   QVector<uint64_t> dims;  //Reusable for the loop
   std::list<std::string> names;
   err = getAllAttributeNames(loc_id, obj_name, names);
   if (err < 0) { return err; }
   std::list<std::string>::iterator iter;
   for (iter=names.begin(); iter != names.end(); iter++)
   {
     err = H5Lite::getAttributeInfo(loc_id, obj_name, (*iter), dims, attr_type, attr_size, tid);
     if (err < 0) {
       std::cout << "Error getting all attributes" << std::endl;
       return err;
     } else {
       switch(attr_type)
       {
       case H5T_STRING:
         err = H5Lite::readStringAttribute(loc_id, obj_name, (*iter), res );
         if (err >= 0) {
           attributes[(*iter)] = res;
         } else { return err; }
         break;
       case H5T_INTEGER:
         err = H5Lite::readVectorAttribute(loc_id, obj_name, (*iter), ires);
         if (err >= 0) {
           attributes[(*iter)] = StringUtils::numToString(ires[0]);
         } else { return err; }
         break;
       case H5T_FLOAT:
         err = H5Lite::readVectorAttribute(loc_id, obj_name, (*iter), fres);
         if (err >= 0) {
           attributes[(*iter)] = StringUtils::numToString(fres[0]);
         } else { return err; }
         break;
       default:
         std::cout << "Unknown attribute type: " << attr_type << ": ";
         printHDFClassType(attr_type);
       }
     }
   }

   return err;
 }
#endif

#if H5LITE_USE_MXA_CONSTRUCTS
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t H5Utilities::readAllAttributes(hid_t fileId,
                                      const QString &datasetPath,
                                      MXAAbstractAttributes &attributes)
{
  CheckValidLocId(fileId);
  herr_t err = -1;
  herr_t retErr = 1;
  hid_t typeId = -1;
  H5T_class_t attr_type;
  size_t attr_size;
  QString res;

  QVector<hsize_t> dims;  //Reusable for the loop
  std::list<std::string> names;
  err = H5Utilities::getAllAttributeNames(fileId, datasetPath, names );
  for (std::list<std::string>::iterator iter=names.begin(); iter != names.end(); iter++)
  {
    //std::cout << "Reading Attribute " << *iter << std::endl;
    err = H5Lite::getAttributeInfo(fileId, datasetPath, (*iter), dims, attr_type, attr_size, typeId);
    if (err < 0) {
      std::cout << "Error in getAttributeInfo method in readUserMetaData." << std::endl;
    } else {
      switch(attr_type)
      {
      case H5T_STRING:
        res.clear(); //Clear the string out first
        err = H5Lite::readStringAttribute(fileId, datasetPath, (*iter), res );
        if (err >= 0) {
          IMXAArray::Pointer attr = MXAAsciiStringData::Create( res );
          attr->setName(*iter);
          attributes[*iter] = attr;
        }
        break;
      case H5T_INTEGER:
        //std::cout << "User Meta Data Type is Integer" << std::endl;
        if ( H5Tequal(typeId, H5T_STD_U8BE) || H5Tequal(typeId,H5T_STD_U8LE) ) {
          IMXAArray::Pointer ptr = H5Utilities::readH5Attribute<uint8_t>(fileId, const_cast<std::string&>(datasetPath), (*iter), dims);
          attributes[*iter] = ptr;
          ptr->setName(*iter);
        } else if ( H5Tequal(typeId, H5T_STD_U16BE) || H5Tequal(typeId,H5T_STD_U16LE) ) {
          IMXAArray::Pointer ptr = H5Utilities::readH5Attribute<uint16_t>(fileId, const_cast<std::string&>(datasetPath), (*iter), dims);
          attributes[*iter] = ptr;
          ptr->setName(*iter);
        } else if ( H5Tequal(typeId, H5T_STD_U32BE) || H5Tequal(typeId,H5T_STD_U32LE) ) {
          IMXAArray::Pointer ptr = H5Utilities::readH5Attribute<uint32_t>(fileId, const_cast<std::string&>(datasetPath), (*iter), dims);
          attributes[*iter] = ptr;
          ptr->setName(*iter);
        } else if ( H5Tequal(typeId, H5T_STD_U64BE) || H5Tequal(typeId,H5T_STD_U64LE) ) {
          IMXAArray::Pointer ptr = H5Utilities::readH5Attribute<uint64_t>(fileId, const_cast<std::string&>(datasetPath), (*iter), dims);
          attributes[*iter] = ptr;
          ptr->setName(*iter);
        } else if ( H5Tequal(typeId, H5T_STD_I8BE) || H5Tequal(typeId,H5T_STD_I8LE) ) {
          IMXAArray::Pointer ptr = H5Utilities::readH5Attribute<int8_t>(fileId, const_cast<std::string&>(datasetPath), (*iter), dims);
          attributes[*iter] = ptr;
          ptr->setName(*iter);
        } else if ( H5Tequal(typeId, H5T_STD_I16BE) || H5Tequal(typeId,H5T_STD_I16LE) ) {
          IMXAArray::Pointer ptr = H5Utilities::readH5Attribute<int16_t>(fileId, const_cast<std::string&>(datasetPath), (*iter), dims);
          attributes[*iter] = ptr;
          ptr->setName(*iter);
        } else if ( H5Tequal(typeId, H5T_STD_I32BE) || H5Tequal(typeId,H5T_STD_I32LE) ) {
          IMXAArray::Pointer ptr = H5Utilities::readH5Attribute<int32_t>(fileId, const_cast<std::string&>(datasetPath), (*iter), dims);
          attributes[*iter] = ptr;
          ptr->setName(*iter);
        } else if ( H5Tequal(typeId, H5T_STD_I64BE) || H5Tequal(typeId,H5T_STD_I64LE) ) {
          IMXAArray::Pointer ptr = H5Utilities::readH5Attribute<int64_t>(fileId, const_cast<std::string&>(datasetPath), (*iter), dims);
          attributes[*iter] = ptr;
          ptr->setName(*iter);
        } else {
          std::cout << "Unknown Type: " << typeId << " at " <<  datasetPath << std::endl;
          err = -1;
          retErr = -1;
        }
        break;
      case H5T_FLOAT:
        if (attr_size == 4) {
          IMXAArray::Pointer ptr = H5Utilities::readH5Attribute<float32>(fileId, const_cast<std::string&>(datasetPath), (*iter), dims);
          attributes[*iter] = ptr;
          ptr->setName(*iter);
        } else if (attr_size == 8 ) {
          IMXAArray::Pointer ptr = H5Utilities::readH5Attribute<float64>(fileId, const_cast<std::string&>(datasetPath), (*iter), dims);
          attributes[*iter] = ptr;
          ptr->setName(*iter);
        } else {
          std::cout << "Unknown Floating point type" << std::endl;
          err = -1;
          retErr = -1;
        }
        break;
      default:
        std::cout << "Error: readUserMetaData() Unknown attribute type: " << attr_type << std::endl;
        H5Utilities::printHDFClassType(attr_type);
      }
      CloseH5T(typeId, err, retErr); //Close the H5A type Id that was retrieved during the loop
    }
   if (retErr < 0)
   {
     break;
   }
  }

  return retErr;
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
 QString H5Utilities::HDFClassTypeAsStr(hid_t class_type)
 {
   switch(class_type) {
   case H5T_INTEGER:
     return "H5T_INTEGER";
     break;
   case H5T_FLOAT:
     return "H5T_FLOAT";
     break;
   case H5T_STRING:
     return "H5T_STRING";
     break;
   case H5T_TIME:
     return "H5T_TIME";
     break;
   case H5T_BITFIELD:
     return "H5T_BITFIELD";
     break;
   case H5T_OPAQUE:
     return "H5T_OPAQUE";
     break;
   case H5T_COMPOUND:
     return "H5T_COMPOUND";
     break;
   case H5T_REFERENCE:
     return "H5T_REFERENCE";
     break;
   case H5T_ENUM:
     return "H5T_ENUM";
     break;
   case H5T_VLEN:
     return "H5T_VLEN";
     break;
   case H5T_ARRAY:
     return "H5T_ARRAY";
     break;
   default:
     return "OTHER";
   }
 }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5Utilities::printHDFClassType(H5T_class_t class_type)
{
  switch(class_type) {
  case H5T_INTEGER:
    std::cout << "H5T_INTEGER" << std::endl;
    break;
  case H5T_FLOAT:
    std::cout << "H5T_FLOAT" << std::endl;
    break;
  case H5T_STRING:
    std::cout << "H5T_STRING" << std::endl;
    break;
  case H5T_TIME:
    std::cout << "H5T_TIME" << std::endl;
    break;
  case H5T_BITFIELD:
    std::cout << "H5T_BITFIELD" << std::endl;
    break;
  case H5T_OPAQUE:
    std::cout << "H5T_OPAQUE" << std::endl;
    break;
  case H5T_COMPOUND:
    std::cout << "H5T_COMPOUND" << std::endl;
    break;
  case H5T_REFERENCE:
    std::cout << "H5T_REFERENCE" << std::endl;
    break;
  case H5T_ENUM:
    std::cout << "H5T_ENUM" << std::endl;
    break;
  case H5T_VLEN:
    std::cout << "H5T_VLEN" << std::endl;
    break;
  case H5T_ARRAY:
    std::cout << "H5T_ARRAY" << std::endl;
    break;
  default:
    std::cout << "OTHER" << std::endl;
  }

}


// -----------------------------------------------------------------------------
//  Returns a QString that is the name of the object at the given index
// -----------------------------------------------------------------------------
herr_t H5Utilities::objectNameAtIndex(hid_t fileId, int32_t idx, QString &name)
{
  ssize_t err = -1;
  // call H5Gget_objname_by_idx with name as NULL to get its length
  ssize_t name_len = H5Lget_name_by_idx(fileId, ".", H5_INDEX_NAME, H5_ITER_NATIVE, (hsize_t)idx, NULL, 0, H5P_DEFAULT);
  if(name_len < 0) {
    name.clear();
    return -1;
  }

  QVector<char> buf(name_len + 1, 0);
  err = H5Lget_name_by_idx ( fileId, ".", H5_INDEX_NAME, H5_ITER_NATIVE, (hsize_t)idx, &(buf.front()), name_len + 1, H5P_DEFAULT );
  if (err < 0) {
    std::cout << "Error Trying to get the dataset name for index " << idx << std::endl;
    name.clear(); //Make an empty string if this fails
  } else {
    name.append( &(buf.front()) ); //Append the string to the given string
  }
  return static_cast<herr_t>(err);
}

// -----------------------------------------------------------------------------
// Checks the given name object to see what type of HDF5 object it is.
// -----------------------------------------------------------------------------
bool H5Utilities::isGroup(hid_t nodeId, const QString &objName)   {
  bool isGroup = true;
  herr_t err = -1;
  H5O_info_t statbuf;
  err = H5Oget_info_by_name(nodeId, objName.c_str(),  &statbuf, H5P_DEFAULT);
  if (err < 0)
  {
    std::cout  << "Error in methd H5Gget_objinfo" << std::endl;
    return false;
  }
  switch (statbuf.type) {
    case H5O_TYPE_GROUP:
      isGroup = true;
      break;
    case H5O_TYPE_DATASET:
      isGroup = false;
      break;
    case H5O_TYPE_NAMED_DATATYPE:
      isGroup = false;
      break;
    default:
      isGroup = false;
  }
  return isGroup;
}



