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

#ifndef _QH5Lite_H_
#define _QH5Lite_H_


//--C++ Headers
#include <typeinfo>

//-- HDF Headers
#include <hdf5.h>
#include <H5Tpublic.h>

#include <vector>
#include <limits>

#include "H5Support/H5Support.h"
#include "H5Support/H5SupportDLLExport.h"
#include "H5Support/H5Lite.h"

#if !defined (H5Support_USE_QT)
#error THIS FILE SHOULD NOT BE INCLUDED UNLESS THE H5Support_USE_QT is also defined
#endif


#include <QtCore/QtDebug>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QList>


#if defined (H5Support_NAMESPACE)
namespace H5Support_NAMESPACE
{
#endif

  /**
   * @brief Class to bring together some high level methods to read/write data to HDF5 files.
   * @class QH5Lite
   * @author Mike Jackson
   * @date April 2007
   * @version $Revision: 1.3 $
   */
  class QH5Lite
  {
    public:
      /**
       * @brief Turns off the global error handler/reporting objects. Note that once
       * they are turned off using this method they CAN NOT be turned back on. If you
       * would like to turn them off for a piece of code then surround your code with
       * the HDF_ERROR_HANDLER_OFF and HDF_ERROR_HANDLER_ON macros defined in
       * QH5Lite.h
       */
      static H5Support_EXPORT void disableErrorHandlers();

      /**
       * @brief Opens an object for HDF5 operations
       * @param loc_id The parent object that holds the true object we want to open
       * @param objName The string name of the object
       * @param obj_type The HDF5_TYPE of object
       * @return Standard HDF5 Error Conditions
       */
      static H5Support_EXPORT herr_t openId(hid_t loc_id, const QString& obj_name, H5O_type_t obj_type);

      /**
       * @brief Opens an HDF5 Object
       * @param obj_id The Object id
       * @param obj_type Basic Object Type
       * @return Standard HDF5 Error Conditions
       */
      static H5Support_EXPORT herr_t closeId( hid_t obj_id, int32_t obj_type );

      /**
       * @brief Given one of the HDF Types as a string, this will return the HDF Type
       * as an hid_t value.
       * @param value The HDF_Type as a string
       * @return the hid_t value for the given type. -1 if the string does not match a type.
       */
      static hid_t HDFTypeFromString(const QString& value)
      {
        return H5Lite::HDFTypeFromString(value.toStdString());
      }


      /**
       * @brief Returns a string version of the HDF Type
       * @param type The HDF5 Type to query
       * @return
       */
      static QString StringForHDFType(hid_t type)
      {
        return QString::fromStdString(H5Lite::StringForHDFType(type));
      }

      /**
      * @brief Returns the HDF Type for a given primitive value.
       * @param value A value to use. Can be anything. Just used to get the type info
       * from
       * @return A QString representing the HDF5 Type
       */
      template<typename T>
      static QString HDFTypeForPrimitiveAsStr(T value)
      {
        return QString::fromStdString(H5Lite::HDFTypeForPrimitiveAsStr(value));
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
        return H5Lite::HDFTypeForPrimitive(value);
      }



      /**
       * @brief Inquires if an attribute named attr_name exists attached to the object loc_id.
       * @param loc_id The location to search
       * @param attrName The attribute to search for
       * @return Standard HDF5 Error condition
       */
      static H5Support_EXPORT herr_t findAttribute( hid_t loc_id, const QString& attrName );

      /**
       * @brief Finds a Data set given a data set name
       * @param loc_id The location to search
       * @param name The dataset to search for
       * @return Standard HDF5 Error condition. Negative=DataSet
       */
      static H5Support_EXPORT bool datasetExists( hid_t loc_id, const QString& name );

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
       * dimensions array. By using a QVector<hsize_t> that stores the values of
       * each of the dimensions we can reduce the number of arguments to this method as
       * the value of the "rank" simply becomes dims.length(). So to create a Dims variable
       * for a 3D data space of size(x,y,z) = {10,20,30} I would use the following code:
       * <code>
       * QVector<hsize_t> dims;
       * dims.push_back(10);
       * dims.push_back(20);
       * dims.push_back(30);
       * </code>
       *
       * Also when passing data BE SURE that the type of data and the data type match.
       * For example if I create some data in a QVector<UInt8Type> you would need to
       * pass H5T_NATIVE_UINT8 as the dataType.
       *
       * Also note that QVector is 32 bit limited in that the most data that it can hold is 2^31 elements. If you
       * are trying to write a data set that has more than 2^31 elements then use the H5Lite::writeVectorDataset()
       * instead which takes a std::vector() and is probably more suited for large data.
       */
      template <typename T>
      static herr_t writeVectorDataset (hid_t loc_id,
                                        const QString& dsetName,
                                        QVector<hsize_t>& dims,
                                        QVector<T>& data)
      {
        return H5Lite::writePointerDataset(loc_id, dsetName.toStdString(), dims.size(), dims.data(), data.data());
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
                                         const QString& dsetName,
                                         int32_t   rank,
                                         hsize_t* dims,
                                         T* data)
      {
        return H5Lite::writePointerDataset(loc_id, dsetName.toStdString(), rank, dims, data);
      }

      /**
       * @brief replacePointerDataset
       * @param loc_id
       * @param dsetName
       * @param rank
       * @param dims
       * @param data
       * @return
       */
      template <typename T>
      static herr_t replacePointerDataset (hid_t loc_id,
                                           const QString& dsetName,
                                           int32_t   rank,
                                           hsize_t* dims,
                                           T* data)
      {
        return H5Lite::replacePointerDataset(loc_id, dsetName.toStdString(), rank, dims, data);
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
       * dimensions array. By using a QVector<hsize_t> that stores the values of
       * each of the dimensions we can reduce the number of arguments to this method as
       * the value of the "rank" simply becomes dims.length(). So to create a Dims variable
       * for a 3D data space of size(x,y,z) = {10,20,30} I would use the following code:
       * <code>
       * QVector<hsize_t> dims;
       * dims.push_back(10);
       * dims.push_back(20);
       * dims.push_back(30);
       * </code>
       *
       */
      template<typename T>
      static herr_t writeDataset(hid_t loc_id,
                                 const QString& dsetName,
                                 int32_t& rank,
                                 hsize_t* dims,
                                 T* data)
      {
        return H5Lite::writeDataset(loc_id, dsetName.toStdString(), rank, dims, data);
      }


      /**
       * @brief Creates a Dataset with the given name at the location defined by loc_id.
       * This version of writeDataset should be used with a single scalar value. If you
       * need to write an array of values, use the form that takes an QVector<>
       *
       * @param loc_id The Parent location to store the data
       * @param dsetName The name of the dataset
       * @param value The value to write to the HDF5 dataset
       * @return Standard HDF5 error conditions
       */
      template <typename T>
      static herr_t writeScalarDataset (hid_t loc_id,
                                        const QString& dsetName,
                                        T& value)
      {
        return H5Lite::writeScalarDataset(loc_id, dsetName.toStdString(), value);
      }

      /**
       * @brief Writes a QString as a HDF Dataset.
       * @param loc_id The Parent location to write the dataset
       * @param dsetName The Name to use for the dataset
       * @param data The actual data to write as a null terminated string
       * @return Standard HDF5 error conditions
       */
      static H5Support_EXPORT herr_t  writeStringDataset (hid_t loc_id,
                                                          const QString& dsetName,
                                                          const QString& data);

      /**
       * @brief Writes a null terminated 'C String' to an HDF Dataset.
       * @param loc_id The Parent location to write the dataset
       * @param dsetName The Name to use for the dataset
       * @param data const char pointer to write as a null terminated string
       * @param size The number of characters in the string
       * @return Standard HDF5 error conditions
       */
      static H5Support_EXPORT herr_t  writeStringDataset (hid_t loc_id,
                                                          const QString& dsetName,
                                                          size_t size,
                                                          const char* data);

      /**
      * @brief
      * @param loc_id
      * @param dsetName
      * @param size
      * @param data
      * @return
      */
      static H5Support_EXPORT herr_t writeVectorOfStringsDataset(hid_t loc_id,
                                                                 const QString& dsetName,
                                                                 const QVector<QString>& data);
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
                                          const QString& objName,
                                          const QString& attrName,
                                          int32_t  rank,
                                          hsize_t* dims,
                                          T* data)
      {
        return H5Lite::writePointerAttribute(loc_id, objName.toStdString(), attrName.toStdString(), rank, dims, data);
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
                                         const QString& objName,
                                         const QString& attrName,
                                         QVector<hsize_t>& dims,
                                         QVector<T>& data )
      {
        return H5Lite::writePointerAttribute(loc_id, objName.toStdString(), attrName.toStdString(), dims.size(), dims.data(), data.data());
      }

      /**
       * @brief Writes a string as a null terminated attribute.
       * @param loc_id The location to look for objName
       * @param objName The Object to write the attribute to
       * @param attrName The name of the Attribute
       * @param data The string to write as the attribute
       * @return Standard HDF error conditions
       */
      static H5Support_EXPORT herr_t  writeStringAttribute(hid_t loc_id,
                                                           const QString& objName,
                                                           const QString& attrName,
                                                           const QString& data);
      /**
       * @brief Writes a null terminated string as an attribute
       * @param loc_id The location to look for objName
       * @param objName The Object to write the attribute to
       * @param attrName The name of the Attribute
       * @param size The number of characters  in the string
       * @param data pointer to a const char array
       * @return Standard HDF error conditions
       */
      static H5Support_EXPORT herr_t  writeStringAttribute(hid_t loc_id,
                                                           const QString& objName,
                                                           const QString& attrName,
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
      static H5Support_EXPORT herr_t writeStringAttributes(hid_t loc_id,
                                                           const QString& objName,
                                                           const QMap<QString, QString>& attributes);

      /**
       * @brief Returns the total number of elements in the supplied dataset
       * @param loc_id The parent location that contains the dataset to read
       * @param dsetName The name of the dataset to read
       * @param data A std::vector<T>. Note the vector WILL be resized to fit the data.
       * The best idea is to just allocate the vector but not to size it. The method
       * will size it for you.
       * @return Number of elements in dataset
       */
      static H5Support_EXPORT hsize_t getNumberOfElements(hid_t loc_id,
                                      const QString& dsetName);

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
      static herr_t writeScalarAttribute(hid_t loc_id,
                                         const QString& objName,
                                         const QString& attrName,
                                         T data )
      {
        return H5Lite::writeScalarAttribute(loc_id, objName.toStdString(), attrName.toStdString(), data);
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
                                       const QString& dsetName,
                                       T* data)
      {
        return H5Lite::readPointerDataset(loc_id, dsetName.toStdString(), data);
      }


      /**
       * @brief Reads data from the HDF5 File into an QVector<T> object. If the dataset
       * is very large this can be an expensive method to use. It is here for convenience
       * using STL with hdf5.
       * @param loc_id The parent location that contains the dataset to read
       * @param dsetName The name of the dataset to read
       * @param data A QVector<T>. Note the vector WILL be resized to fit the data.
       * The best idea is to just allocate the vector but not to size it. The method
       * will size it for you.
       * @return Standard HDF error condition
       */
      template <typename T>
      static herr_t readVectorDataset(hid_t loc_id,
                                      const QString& dsetName,
                                      std::vector<T>& data)
      {
        hid_t   did;
        herr_t  err = 0;
        herr_t retErr = 0;
        hid_t spaceId;
        hid_t dataType;
        T test = static_cast<T>(0x00);
        dataType = QH5Lite::HDFTypeForPrimitive(test);
        if (dataType == -1)
        {
          return -1;
        }
        //qDebug() << "HDF5 Data Type: " << QH5Lite::HDFTypeForPrimitiveAsStr(test);
        /* Open the dataset. */
// qDebug() << "  Opening " << dsetName << " for data Retrieval.  ";
        did = H5Dopen( loc_id, dsetName.toLatin1().data(), H5P_DEFAULT);
        if ( did < 0 )
        {
          qDebug() << "QH5Lite::readStringDataset(" << __LINE__ << ") Error opening Dataset at loc_id (" << loc_id << ") with object name (" << dsetName << ")";
          return -1;
        }
        if ( did >= 0 )
        {
          spaceId = H5Dget_space(did);
          if ( spaceId > 0 )
          {
            int32_t rank = H5Sget_simple_extent_ndims(spaceId);
            if (rank > 0)
            {
              QVector<hsize_t> dims;
              dims.resize(rank);// Allocate enough room for the dims
              err = H5Sget_simple_extent_dims(spaceId, &(dims.front()), NULL);
              hsize_t numElements = 1;
              for (QVector<hsize_t>::iterator iter = dims.begin(); iter < dims.end(); ++iter )
              {
                numElements = numElements * (*iter);
              }
              // qDebug() << "NumElements: " << numElements;
              //Resize the vector
              data.resize( static_cast<int>(numElements) );
              // for (uint32_t i = 0; i<numElements; ++i) { data[i] = 55555555;  }
              err = H5Dread(did, dataType, H5S_ALL, H5S_ALL, H5P_DEFAULT, &( data.front() ) );
              if (err < 0)
              {
                qDebug() << "Error Reading Data.'" << dsetName << "'";
                retErr = err;
              }
            }
            err = H5Sclose(spaceId);
            if (err < 0 )
            {
              qDebug() << "Error Closing Data Space";
              retErr = err;
            }
          }
          else
          {
            qDebug() << "Error Opening SpaceID";
            retErr = spaceId;
          }
          err = H5Dclose( did );
          if (err < 0 )
          {
            qDebug() << "Error Closing Dataset";
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
                                      const QString& dsetName,
                                      T& data)
      {
        hid_t   did;
        herr_t  err = 0;
        herr_t retErr = 0;
        hid_t spaceId;

        hid_t dataType = QH5Lite::HDFTypeForPrimitive(data);
        if (dataType == -1)
        {
          return -1;
        }
        /* Open the dataset. */
        did = H5Dopen( loc_id, dsetName.toLatin1().data(), H5P_DEFAULT );
        if ( did < 0 )
        {
          qDebug() << "QH5Lite::readStringDataset(" << __LINE__ << ") Error opening Dataset at loc_id (" << loc_id << ") with object name (" << dsetName << ")";
          return -1;
        }
        if ( did >= 0 )
        {
          spaceId = H5Dget_space(did);
          if ( spaceId > 0 )
          {
            int32_t rank = H5Sget_simple_extent_ndims(spaceId);
            if (rank > 0)
            {
              QVector<hsize_t> dims;
              dims.resize(rank);// Allocate enough room for the dims
              err = H5Sget_simple_extent_dims(spaceId, &(dims.front()), NULL);
              hsize_t numElements = 1;
              for (QVector<hsize_t>::iterator iter = dims.begin(); iter < dims.end(); ++iter )
              {
                numElements = numElements * (*iter);
              }
              err = H5Dread(did, dataType, H5S_ALL, H5S_ALL, H5P_DEFAULT, &data );
              if (err < 0)
              {
                qDebug() << "Error Reading Data at loc_id (" << loc_id << ") with object name (" << dsetName << ")";
                retErr = err;
              }
            }
            err = H5Sclose(spaceId);
            if (err < 0 )
            {
              qDebug() << "Error Closing Data Space at loc_id (" << loc_id << ") with object name (" << dsetName << ")";
              retErr = err;
            }
          }
          else
          {
            retErr = spaceId;
          }
          err = H5Dclose( did );
          if (err < 0 )
          {
            qDebug() << "Error Closing Dataset at loc_id (" << loc_id << ") with object name (" << dsetName << ")";
            retErr = err;
          }
        }
        return retErr;
      }


      /**
       * @brief Reads a string dataset into the supplied string. Any data currently in the 'data' variable
       * is cleared first before the new data is read into the string.
       * @param loc_id The parent group that holds the data object to read
       * @param dsetName The name of the dataset.
       * @param data The QString to hold the data
       * @return Standard HDF error condition
       */
      static H5Support_EXPORT herr_t readStringDataset(hid_t loc_id,
                                                       const QString& dsetName,
                                                       QString& data);


      /**
        * @brief
        * @param loc_id
        * @param dsetName
        * @param data
        * @return
        */
      static H5Support_EXPORT herr_t readVectorOfStringDataset(hid_t loc_id,
                                                               const QString& dsetName,
                                                               QVector<QString>& data);
      /**
       * @brief reads a null terminated string dataset into the supplied buffer. The buffer
       * should be already preallocated.
       * @param loc_id The parent group that holds the data object to read
       * @param dsetName The name of the dataset.
       * @param data pointer to the buffer
       * @return Standard HDF error condition
       */
      static H5Support_EXPORT herr_t readStringDataset(hid_t loc_id,
                                                       const QString& dsetName,
                                                       char* data);

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
                                        const QString& objName,
                                        const QString& attrName,
                                        QVector<T>& data)
      {
        /* identifiers */
        hid_t      obj_id;
        H5O_info_t statbuf;
        herr_t err = 0;
        herr_t retErr = 0;
        hid_t attr_id;
        hid_t tid;
        T test = 0x00;
        hid_t dataType = QH5Lite::HDFTypeForPrimitive(test);
        if (dataType == -1)
        {
          return -1;
        }
        //qDebug() << "   Reading Vector Attribute at Path '" << objName << "' with Key: '" << attrName << "'";
        /* Get the type of object */
        err = H5Oget_info_by_name(loc_id, objName.toLatin1().data(),  &statbuf, H5P_DEFAULT);
        if (err < 0)
        { return err; }
        /* Open the object */
        obj_id = QH5Lite::openId( loc_id, objName, statbuf.type);
        if ( obj_id >= 0)
        {
          attr_id = H5Aopen_by_name( loc_id, objName.toLatin1().data(), attrName.toLatin1().data(), H5P_DEFAULT, H5P_DEFAULT );
          if ( attr_id >= 0 )
          {
            //Need to allocate the array size
            H5T_class_t type_class;
            size_t type_size;
            QVector<hsize_t> dims;
            err = QH5Lite::getAttributeInfo(loc_id, objName, attrName, dims, type_class, type_size, tid);
            hsize_t numElements = 1;
            for (QVector<hsize_t>::iterator iter = dims.begin(); iter < dims.end(); ++iter )
            {
              numElements *= *(iter);
            }
            //qDebug() << "    Vector Attribute has " << numElements << " elements.";
            if (numElements > static_cast<hsize_t>(std::numeric_limits<qint32>::max() ) )
            {
              qDebug() << "Number of Elements in Array is larger than QVector can handle. Suggest using a std::vector<> instead";
              err = -1000;
            }
            else
            {
              data.resize( static_cast<signed int>(numElements) );
              err = H5Aread( attr_id, dataType, &(data.front()) );
            }
            if ( err < 0 )
            {
              qDebug() << "Error Reading Attribute." << err;
              retErr = err;
            }
            err = H5Aclose( attr_id );
            if ( err < 0 )
            {
              qDebug() << "Error Closing Attribute";
              retErr = err;
            }
          }
          else
          {
            retErr = attr_id;
          }
          err = QH5Lite::closeId( obj_id, statbuf.type );
          if ( err < 0 )
          {
            qDebug() << "Error Closing Object";
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
                                         const QString& objName,
                                         const QString& attrName,
                                         T& data)
      {

        /* identifiers */
        hid_t      obj_id;
        H5O_info_t statbuf;
        herr_t err = 0;
        herr_t retErr = 0;
        hid_t attr_id;
        T test = 0x00;
        hid_t dataType = QH5Lite::HDFTypeForPrimitive(test);
        if (dataType == -1)
        {
          return -1;
        }
        //qDebug() << "Reading Scalar style Attribute at Path '" << objName << "' with Key: '" << attrName << "'";
        /* Get the type of object */
        err = H5Oget_info_by_name(loc_id, objName.toLatin1().data(),  &statbuf, H5P_DEFAULT);
        if (err < 0)
        { return err; }
        /* Open the object */
        obj_id = QH5Lite::openId( loc_id, objName, statbuf.type);
        if ( obj_id >= 0)
        {
          attr_id = H5Aopen_by_name( loc_id, objName.toLatin1().data(), attrName.toLatin1().data(), H5P_DEFAULT, H5P_DEFAULT );
          if ( attr_id >= 0 )
          {
            err = H5Aread( attr_id, dataType, &data );
            if ( err < 0 )
            {
              qDebug() << "Error Reading Attribute.";
              retErr = err;
            }
            err = H5Aclose( attr_id );
            if ( err < 0 )
            {
              qDebug() << "Error Closing Attribute";
              retErr = err;
            }
          }
          else
          {
            retErr = attr_id;
          }
          err = QH5Lite::closeId( obj_id, statbuf.type );
          if ( err < 0 )
          {
            qDebug() << "Error Closing Object";
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
                                         const QString& objName,
                                         const QString& attrName,
                                         T* data)
      {
        /* identifiers */
        hid_t      obj_id;
        H5O_info_t statbuf;
        herr_t err = 0;
        herr_t retErr = 0;
        hid_t attr_id;
        T test = 0x00;
        hid_t dataType = QH5Lite::HDFTypeForPrimitive(test);
        if (dataType == -1)
        {
          return -1;
        }
        //qDebug() << "   Reading Vector Attribute at Path '" << objName << "' with Key: '" << attrName << "'";
        /* Get the type of object */
        err = H5Oget_info_by_name(loc_id, objName.toLatin1().data(),  &statbuf, H5P_DEFAULT);
        if (err < 0)
        { return err; }
        /* Open the object */
        obj_id = QH5Lite::openId( loc_id, objName, statbuf.type);
        if ( obj_id >= 0)
        {
          attr_id = H5Aopen_by_name( loc_id, objName.toLatin1().data(), attrName.toLatin1().data(), H5P_DEFAULT, H5P_DEFAULT );
          if ( attr_id >= 0 )
          {
            err = H5Aread( attr_id, dataType, data);
            if ( err < 0 )
            {
              qDebug() << "Error Reading Attribute." << err;
              retErr = err;
            }
            err = H5Aclose( attr_id );
            if ( err < 0 )
            {
              qDebug() << "Error Closing Attribute";
              retErr = err;
            }
          }
          else
          {
            retErr = attr_id;
          }
          err = QH5Lite::closeId( obj_id, statbuf.type );
          if ( err < 0 )
          {
            qDebug() << "Error Closing Object";
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
      static H5Support_EXPORT herr_t readStringAttribute(hid_t loc_id,
                                                         const QString& objName,
                                                         const QString& attrName,
                                                         QString& data);

      /**
       * @brief Reads a string attribute from an HDF object into a precallocated buffer
       * @param loc_id The Parent object that holds the object to which you want to read an attribute
       * @param objName The name of the object to which the attribute is to be read
       * @param attrName The name of the Attribute to read
       * @param data The memory to store the data into
       * @return Standard HDF Error condition
       */
      static H5Support_EXPORT herr_t readStringAttribute(hid_t loc_id,
                                                         const QString& objName,
                                                         const QString& attrName,
                                                         char* data);
      /**
       * @brief Returns the number of dimensions for a given attribute
       * @param loc_id The HDF5 id of the parent group/file for the objName
       * @param objName The name of the dataset
       * @param attrName The name of the attribute
       * @param rank (out) Number of dimensions is store into this variable
       */
      static H5Support_EXPORT hid_t getAttributeNDims(hid_t loc_id, const QString& objName, const QString& attrName, hid_t& rank);

      /**
       * @brief Returns the number of dimensions for a given dataset
       * @param loc_id The HDF5 id of the parent group/file for the objName
       * @param objName The name of the dataset
       * @param rank (out) Number of dimensions is store into this variable
       */
      static H5Support_EXPORT hid_t getDatasetNDims(hid_t loc_id, const QString& objName, hid_t& rank);

      /**
       * @brief Returns the H5T value for a given dataset.
       *
       * Returns the type of data stored in the dataset. You MUST use H5Tclose(tid)
       * on the returned value or resource leaks will occur.
       * @param loc_id A Valid HDF5 file or group id.
       * @param dsetName Path to the dataset
       * @return
       */
      static H5Support_EXPORT hid_t getDatasetType(hid_t loc_id, const QString& dsetName);

      /**
       * @brief Get the information about a dataset.
       *
       * @param loc_id The parent location of the Dataset
       * @param dsetName The name of the dataset
       * @param dims A QVector that will hold the sizes of the dimensions
       * @param type_class The HDF5 class type
       * @param type_size THe HDF5 size of the data
       * @return Negative value is Failure. Zero or Positive is success;
       */
      static H5Support_EXPORT herr_t getDatasetInfo( hid_t loc_id,
                                                     const QString& dsetName,
                                                     QVector<hsize_t>& dims,
                                                     H5T_class_t& type_class,
                                                     size_t& type_size );

      /**
       * @brief Returns the information about an attribute.
       * You must close the attributeType argument or resource leaks will occur. Use
       *  H5Tclose(attr_type); after your call to this method if you do not need the id for
       *   anything.
       * @param loc_id The parent location of the Dataset
       * @param objName The name of the dataset
       * @param attr_name The name of the attribute
       * @param dims A QVector that will hold the sizes of the dimensions
       * @param type_class The HDF5 class type
       * @param type_size THe HDF5 size of the data
       * @param attr_type The Attribute ID - which needs to be closed after you are finished with the data
       * @return
       */
      static H5Support_EXPORT herr_t getAttributeInfo(hid_t loc_id,
                                                      const QString& objName,
                                                      const QString& attr_name,
                                                      QVector<hsize_t>& dims,
                                                      H5T_class_t& type_class,
                                                      size_t& type_size,
                                                      hid_t& attr_type);


// -----------------------------------------------------------------------------
    protected:
      QH5Lite();
      ~QH5Lite();


    private:

  };


#if defined (H5Support_NAMESPACE)
}
#endif

#endif
