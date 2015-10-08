
//
//  Copyright (c) 2007, 2010 Michael A. Jackson for BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number
//                           FA8650-04-C-5229
//

#ifndef _HDF5_UTILITIES_H_
#define _HDF5_UTILITIES_H_


// C++ Includes
#include <map>
#include <list>
#include <string>

//-- HDF Headers
#include <hdf5.h>

#include "H5Support/H5Support.h"
#include "H5Support/H5Lite.h"

/* H5LITE_USE_H5Support_CONSTRUCTS is used to include H5SupportDataModel Specific classes in
 * this class. If this is being compiled as part of H5SupportDataModel this should
 * _always_ be defined. If this code is being used as part of another project
 * then this should probably NOT be defined.
 */

#if defined (H5Support_NAMESPACE)
namespace H5Support_NAMESPACE
{
#endif

  /**
   * @brief General Utilities for working with the HDF5 data files and API
   */
  class H5Utilities
  {

    public:

      virtual ~H5Utilities();

      enum CustomHDFDataTypes
      {
        H5Support_GROUP = 1,
        H5Support_DATASET = 2,
        H5Support_TYPE = 4,
        H5Support_LINK = 8,
        H5Support_ANY = 15
      };

      // -----------HDF5 File Operations
      static H5Support_EXPORT hid_t openFile(const std::string& filename, bool readOnly = false);

      static H5Support_EXPORT hid_t createFile(const std::string& filename);

      static H5Support_EXPORT herr_t closeFile(hid_t& fileId);

      // -------------- HDF Indentifier Methods ----------------------------
      /**
      * @brief Retuirns the path to an object
      * @param objId The HDF5 id of the object
      * @param trim set to False to trim the path
      * @return  The path to the object relative to the objId
      */
      static H5Support_EXPORT std::string getObjectPath(hid_t objId, bool trim = false);

      /**
      * @brief Returns the hdf object type
      * @param objId The hdf5 object id
      * @param objName The path to the data set
      * @param objType The type of the object
      * @return  Negative value on error
      */
      static H5Support_EXPORT herr_t getObjectType(hid_t objId, const std::string& objName, int32_t* objType);

      /**
      * @brief Retrieves the object name for a given index
      * @param fileId The hdf5 object id
      * @param idx The index to retrieve the name for
      * @param name The variable to store the name
      * @return Negative value is error
      */
      static H5Support_EXPORT herr_t objectNameAtIndex(hid_t fileId, int32_t idx, std::string& name);

      /**
      * @brief Returns if a given hdf5 object is a group
      * @param objId The hdf5 object that contains an object with name objName
      * @param objName The name of the object to check
      * @return True if the given hdf5 object id is a group
      */
      static H5Support_EXPORT bool isGroup(hid_t objId, const std::string& objName);


      /**
      * @brief Opens an HDF5 object for hdf5 operations
      * @param locId the Object id of the parent
      * @param objectPath The path of the object to open
      * @return The hdf5 id of the opened object. Negative value is error.
      */
      static H5Support_EXPORT hid_t openHDF5Object(hid_t locId, const std::string& objectPath);

      /**
      * @brief Closes the object id
      * @param locId The object id to close
      * @return Negative value is error.
      */
      static H5Support_EXPORT herr_t closeHDF5Object(hid_t locId);


      static H5Support_EXPORT std::string HDFClassTypeAsStr(hid_t class_type);

      /**
      * @brief prints the class type of the given class
      * @param classT The Class Type to print
      */
      static H5Support_EXPORT void printHDFClassType(H5T_class_t classT);

      // -------------- HDF Group Methods ----------------------------
      /**
      * @brief Returns a list of child hdf5 objects for a given object id
      * @param loc_id The parent hdf5 id
      * @param typeFilter A filter to apply to the list
      * @param names Variable to store the list
      * @return
      */
      static H5Support_EXPORT herr_t getGroupObjects(hid_t loc_id, int32_t typeFilter, std::list<std::string>& names);

      /**
       * @brief Creates a HDF Group by checking if the group already exists. If the
       * group already exists then that group is returned otherwise a new group is
       * created.
       * @param loc_id The HDF unique id given to files or groups
       * @param group The name of the group to create. Note that this group name should
       * not be any sort of 'path'. It should be a single group.
       */
      static H5Support_EXPORT hid_t createGroup(hid_t loc_id, const std::string& group);

      /**
       * @brief Given a path relative to the Parent ID, this method will create all
       * the intermediate groups if necessary.
       * @param pathToCheck The path to either create or ensure exists.
       * @param parent The HDF unique id for the parent
       * @return Error Condition: Negative is error. Positive is success.
       */
      static H5Support_EXPORT herr_t  createGroupsFromPath(const std::string& pathToCheck, hid_t parent);

      /**
       * @brief Given a path relative to the Parent ID, this method will create all
       * the intermediate groups if necessary.
       * @param datasetPath The path to the dataset that you want to make all the intermediate groups for
       * @param parent The HDF unique id for the parent
       * @return Error Condition: Negative is error. Positive is success.
       */
      static H5Support_EXPORT herr_t createGroupsForDataset(const std::string& datasetPath, hid_t parent);

      /**
      * @brief Extracts the object name from a given path
      * @param path The path which to extract the object name
      * @return The name of the object
      */
      static H5Support_EXPORT std::string extractObjectName(const std::string& path);

      // -------------- HDF Attribute Methods ----------------------------
      /**
      * @brief Looks for an attribute with a given name
      * @param loc_id The objects Parent id
      * @param obj_name The name of the object
      * @param attr_name The attribute to look for (by name)
      * @return True if the attribute exists.
      */
      static H5Support_EXPORT bool probeForAttribute(hid_t loc_id,
                                                     const std::string& obj_name,
                                                     const std::string& attr_name);


      /**
      * @brief Returns a list of all the attribute names
      * @param objId The parent object
      * @param names Variable to hold the list of attribute names
      * @return Negate value is error
      */
      static H5Support_EXPORT herr_t getAllAttributeNames(hid_t objId, std::list<std::string>& names);

      /**
      * @brief Returns a list of all the attribute names
      * @param objId The parent object
      * @param obj_name The name of the object whose attribute names you want a list
      * @param names Variable to hold the list of attribute names
      * @return Negative value is error
      */
      static H5Support_EXPORT herr_t getAllAttributeNames(hid_t objId, const std::string& obj_name,
                                                          std::list<std::string>& names);


    protected:
      H5Utilities() {} //This is just a bunch of Static methods

    private:
      H5Utilities(const H5Utilities&);   //Copy Constructor Not Implemented
      void operator=(const H5Utilities&); //Copy Assignment Not Implemented
  };

#if defined (H5Support_NAMESPACE)
}
#endif

#endif /* _HDF5_UTILITIES_H_ */


