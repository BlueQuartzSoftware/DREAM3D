/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _QH5Utilities_H_
#define _QH5Utilities_H_


#include "H5Support/H5Support.h"
#include "H5Support/H5Utilities.h"

#if !defined (H5Support_USE_QT)
#error THIS FILE SHOULD NOT BE INCLUDED UNLESS THE H5Support_USE_QT is also defined
#endif


#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QList>


/**
 * @brief General Utilities for working with the HDF5 data files and API
 */
class QH5Utilities
{

  public:

    virtual ~QH5Utilities();


    static H5Support_EXPORT hid_t openFile(const QString& filename, bool readOnly = false);

    static H5Support_EXPORT hid_t createFile(const QString& filename);


    static H5Support_EXPORT herr_t closeFile(hid_t& fileId);

    // -------------- HDF Indentifier Methods ----------------------------
    /**
    * @brief Retuirns the path to an object
    * @param objId The HDF5 id of the object
    * @param trim set to False to trim the path
    * @return  The path to the object relative to the objId
    */
    static H5Support_EXPORT QString getObjectPath(hid_t objId, bool trim = false);

    /**
    * @brief Returns the hdf object type
    * @param objId The hdf5 object id
    * @param objName The path to the data set
    * @param objType The type of the object
    * @return  Negative value on error
    */
    static H5Support_EXPORT herr_t getObjectType(hid_t objId, const QString& objName, int32_t* objType);

    /**
    * @brief Retrieves the object name for a given index
    * @param fileId The hdf5 object id
    * @param idx The index to retrieve the name for
    * @param name The variable to store the name
    * @return Negative value is error
    */
    static H5Support_EXPORT herr_t objectNameAtIndex(hid_t fileId, int32_t idx, QString& name);

    /**
    * @brief Returns if a given hdf5 object is a group
    * @param objId The hdf5 object that contains an object with name objName
    * @param objName The name of the object to check
    * @return True if the given hdf5 object id is a group
    */
    static H5Support_EXPORT bool isGroup(hid_t objId, const QString& objName);


    /**
    * @brief Opens an HDF5 object for hdf5 operations
    * @param locId the Object id of the parent
    * @param objectPath The path of the object to open
    * @return The hdf5 id of the opened object. Negative value is error.
    */
    static H5Support_EXPORT hid_t openHDF5Object(hid_t locId, const QString& objectPath);

    /**
    * @brief Closes the object id
    * @param locId The object id to close
    * @return Negative value is error.
    */
    static H5Support_EXPORT herr_t closeHDF5Object(hid_t locId);


    static H5Support_EXPORT QString HDFClassTypeAsStr(hid_t class_type);

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
    static H5Support_EXPORT herr_t getGroupObjects(hid_t loc_id, int32_t typeFilter, QList<QString>& names);

    /**
     * @brief Creates a HDF Group by checking if the group already exists. If the
     * group already exists then that group is returned otherwise a new group is
     * created.
     * @param loc_id The HDF unique id given to files or groups
     * @param group The name of the group to create. Note that this group name should
     * not be any sort of 'path'. It should be a single group.
     */
    static H5Support_EXPORT hid_t createGroup(hid_t loc_id, const QString& group);

    /**
     * @brief Given a path relative to the Parent ID, this method will create all
     * the intermediate groups if necessary.
     * @param pathToCheck The path to either create or ensure exists.
     * @param parent The HDF unique id for the parent
     * @return Error Condition: Negative is error. Positive is success.
     */
    static H5Support_EXPORT herr_t  createGroupsFromPath(const QString& pathToCheck, hid_t parent);

    /**
     * @brief Given a path relative to the Parent ID, this method will create all
     * the intermediate groups if necessary.
     * @param datasetPath The path to the dataset that you want to make all the intermediate groups for
     * @param parent The HDF unique id for the parent
     * @return Error Condition: Negative is error. Positive is success.
     */
    static H5Support_EXPORT herr_t createGroupsForDataset(const QString& datasetPath, hid_t parent);

    /**
    * @brief Extracts the object name from a given path
    * @param path The path which to extract the object name
    * @return The name of the object
    */
    static H5Support_EXPORT QString extractObjectName(const QString& path);

    // -------------- HDF Attribute Methods ----------------------------
    /**
    * @brief Looks for an attribute with a given name
    * @param loc_id The objects Parent id
    * @param obj_name The name of the object
    * @param attr_name The attribute to look for (by name)
    * @return True if the attribute exists.
    */
    static H5Support_EXPORT bool probeForAttribute(hid_t loc_id,
                                                   const QString& obj_name,
                                                   const QString& attr_name);


    /**
    * @brief Returns a list of all the attribute names
    * @param objId The parent object
    * @param names Variable to hold the list of attribute names
    * @return Negate value is error
    */
    static H5Support_EXPORT herr_t getAllAttributeNames(hid_t objId, QList<QString>& names);

    /**
    * @brief Returns a list of all the attribute names
    * @param objId The parent object
    * @param obj_name The name of the object whose attribute names you want a list
    * @param names Variable to hold the list of attribute names
    * @return Negative value is error
    */
    static H5Support_EXPORT herr_t getAllAttributeNames(hid_t objId, const QString& obj_name,
                                                        QList<QString>& names);


    static H5Support_EXPORT QString fileNameFromFileId(hid_t fileId);
    static H5Support_EXPORT QString absoluteFilePathFromFileId(hid_t fileId);


  protected:
    QH5Utilities() {} //This is just a bunch of Static methods

  private:
    QH5Utilities(const QH5Utilities&);   //Copy Constructor Not Implemented
    void operator=(const QH5Utilities&); //Copy Assignment Not Implemented
};

#endif /* _QH5Utilities_H_ */

