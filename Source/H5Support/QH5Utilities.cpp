/* ============================================================================
 * Copyright (c) 2013 Michael A. Jackson (BlueQuartz Software)
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

#include <H5Support/QH5Utilities.h>

#include <QtCore/QtDebug>
#include <QtCore/QFileInfo>

#define CheckValidLocId(locId)\
  if (locId < 0 ) {qDebug() << "Invalid HDF Location ID: " << locId;return -1;}

#if defined (H5Support_NAMESPACE)
using namespace H5Support_NAMESPACE;
#endif


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
hid_t QH5Utilities::createFile(const QString& filename)
{
  return H5Utilities::createFile(filename.toStdString());
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
hid_t QH5Utilities::openFile(const QString& filename, bool readOnly)
{
  return H5Utilities::openFile(filename.toStdString(), readOnly);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t QH5Utilities::closeFile(hid_t& fileId)
{
  return H5Utilities::closeFile(fileId);
}

// -----------------------------------------------------------------------------
//  Returns the full path to the object referred to by the
// -----------------------------------------------------------------------------
QString QH5Utilities::getObjectPath(hid_t loc_id, bool trim)
{
  return QString::fromStdString(H5Utilities::getObjectPath(loc_id, trim));
}


// -----------------------------------------------------------------------------
// @brief Retrieves the HDF object type for obj_name at loc_id and stores
//    it in the parameter obj_type passed in.
// -----------------------------------------------------------------------------
herr_t QH5Utilities::getObjectType(hid_t objId, const QString& objName, int32_t* objType)
{
  return H5Utilities::getObjectType(objId, objName.toStdString(), objType);
}



// Opens and returns the HDF object (since the HDF api requires
//  different open and close methods for different types of objects
hid_t QH5Utilities::openHDF5Object(hid_t loc_id, const QString& objName)
{
  return H5Utilities::openHDF5Object(loc_id, objName.toStdString());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t QH5Utilities::closeHDF5Object(hid_t obj_id)
{
  return H5Utilities::closeHDF5Object(obj_id);
}


//--------------------------------------------------------------------//
// HDF Group Methods
//--------------------------------------------------------------------//
herr_t QH5Utilities::getGroupObjects(hid_t loc_id, int32_t typeFilter, QList<QString>& names)
{

  std::list<std::string> sNames;
  herr_t err = H5Utilities::getGroupObjects(loc_id, typeFilter, sNames);

  names.clear();
  for(std::list<std::string>::iterator name = sNames.begin(); name != sNames.end(); ++name)
  {
    names.push_back( QString::fromStdString(*name));
  }

  return err;
}


// -----------------------------------------------------------------------------
// HDF Creation/Modification Methods
// -----------------------------------------------------------------------------
hid_t QH5Utilities::createGroup(hid_t loc_id, const QString& group)
{
  return H5Utilities::createGroup(loc_id, group.toStdString());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t QH5Utilities::createGroupsForDataset(const QString& datasetPath, hid_t parent)
{
  return H5Utilities::createGroupsForDataset(datasetPath.toStdString(), parent);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t QH5Utilities::createGroupsFromPath(const QString& pathToCheck, hid_t parent)
{
  return H5Utilities::createGroupsFromPath(pathToCheck.toStdString(), parent);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QH5Utilities::extractObjectName(const QString& path)
{
  return QString::fromStdString(H5Utilities::extractObjectName(path.toStdString()));
}


//--------------------------------------------------------------------//
// HDF Attribute Methods
//--------------------------------------------------------------------//
bool QH5Utilities::probeForAttribute(hid_t loc_id,
                                     const QString& obj_name,
                                     const QString& attr_name)
{
  return H5Utilities::probeForAttribute(loc_id, obj_name.toStdString(), attr_name.toStdString());
}

//--------------------------------------------------------------------//
// Returns a QList of all attribute names attached to the object
//  referred to by obj_id
//--------------------------------------------------------------------//
herr_t QH5Utilities::getAllAttributeNames(hid_t obj_id,
                                          QList<QString>& names)
{
  names.clear();
  std::list<std::string> sResults;
  herr_t err = H5Utilities::getAllAttributeNames(obj_id, sResults);
  for(std::list<std::string>::iterator iter = sResults.begin(); iter != sResults.end(); ++iter)
  {
    names.push_back(QString::fromStdString(*iter));
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t QH5Utilities::getAllAttributeNames(hid_t loc_id,
                                          const QString& obj_name,
                                          QList<QString>& names)
{
  names.clear();
  std::list<std::string> sResults;
  herr_t err = H5Utilities::getAllAttributeNames(loc_id, obj_name.toStdString(), sResults);
  for(std::list<std::string>::iterator iter = sResults.begin(); iter != sResults.end(); ++iter)
  {
    names.push_back(QString::fromStdString(*iter));
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QH5Utilities::HDFClassTypeAsStr(hid_t class_type)
{
  return QString::fromStdString(H5Utilities::HDFClassTypeAsStr(class_type));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QH5Utilities::printHDFClassType(H5T_class_t class_type)
{
  std::string hType = H5Utilities::HDFClassTypeAsStr(class_type);
  qDebug() << QString::fromStdString(hType);
}


// -----------------------------------------------------------------------------
//  Returns a QString that is the name of the object at the given index
// -----------------------------------------------------------------------------
herr_t QH5Utilities::objectNameAtIndex(hid_t fileId, int32_t idx, QString& name)
{
  std::string sName;
  herr_t err = H5Utilities::objectNameAtIndex(fileId, idx, sName);
  name = QString::fromStdString(sName);
  return err;
}

// -----------------------------------------------------------------------------
// Checks the given name object to see what type of HDF5 object it is.
// -----------------------------------------------------------------------------
bool QH5Utilities::isGroup(hid_t nodeId, const QString& objName)
{
  return H5Utilities::isGroup(nodeId, objName.toStdString());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QH5Utilities::fileNameFromFileId(hid_t fileId)
{
// Get the name of the .dream3d file that we are writing to:
  ssize_t nameSize = H5Fget_name(fileId, NULL, 0) + 1;
  QByteArray nameBuffer(nameSize, 0);
  nameSize = H5Fget_name(fileId, nameBuffer.data(), nameSize);

  QString hdfFileName(nameBuffer);
  QFileInfo fi(hdfFileName);
  hdfFileName = fi.fileName();
  return hdfFileName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QH5Utilities::absoluteFilePathFromFileId(hid_t fileId)
{
// Get the name of the .dream3d file that we are writing to:
  ssize_t nameSize = H5Fget_name(fileId, NULL, 0) + 1;
  QByteArray nameBuffer(nameSize, 0);
  nameSize = H5Fget_name(fileId, nameBuffer.data(), nameSize);

  QString hdfFileName(nameBuffer);
  QFileInfo fi(hdfFileName);
  return fi.absoluteFilePath();
}
