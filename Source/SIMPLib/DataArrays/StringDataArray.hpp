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


#ifndef _StrignDataArray_H_
#define _StrignDataArray_H_

#include <string>
#include <vector>


#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

/**
 * @class StringDataArray StringDataArray.h DREAM3DLib/Common/StringDataArray.h
 * @brief Stores an array of QString objects
 *
 * @date Nov 13, 2012
 * @version 1.0
 */
class StringDataArray : public IDataArray
{
  public:
    SIMPL_SHARED_POINTERS(StringDataArray )
    SIMPL_TYPE_MACRO_SUPER(StringDataArray, IDataArray)
    SIMPL_CLASS_VERSION(2)

    /**
     * @brief CreateArray
     * @param numTuples
     * @param name
     * @param allocate
     * @return
     */
    static Pointer CreateArray(size_t numTuples, const QString& name, bool allocate = true)
    {
      if (name.isEmpty() == true)
      {
        return NullPointer();
      }
      StringDataArray* d = new StringDataArray(numTuples, name, allocate);
      d->setName(name);
      Pointer ptr(d);
      return ptr;
    }

    /**
     * @brief CreateArray
     * @param numTuples
     * @param compDims
     * @param name
     * @param allocate
     * @return
     */
    static Pointer CreateArray(size_t numTuples, QVector<size_t> compDims, const QString& name, bool allocate = true)
    {
      if (name.isEmpty() == true)
      {
        return NullPointer();
      }
      StringDataArray* d = new StringDataArray(numTuples, name, allocate);
      d->setName(name);
      Pointer ptr(d);
      return ptr;
    }

    /**
     * @brief createNewArray
     * @param numElements
     * @param numComponents NOT USED. It is always 1.
     * @param name
     * @return
     */
    virtual IDataArray::Pointer createNewArray(size_t numElements, int rank, size_t* dims, const QString& name, bool allocate = true)
    {
      IDataArray::Pointer p = StringDataArray::CreateArray(numElements, name, allocate);
      return p;
    }

    virtual IDataArray::Pointer createNewArray(size_t numElements, std::vector<size_t> dims, const QString& name, bool allocate = true)
    {
      IDataArray::Pointer p = StringDataArray::CreateArray(numElements, name, allocate);
      return p;
    }

    virtual IDataArray::Pointer createNewArray(size_t numElements, QVector<size_t> dims, const QString& name, bool allocate = true)
    {
      IDataArray::Pointer p = StringDataArray::CreateArray(numElements, name, allocate);
      return p;
    }

    virtual ~StringDataArray() {}

    /**
     * @brief isAllocated
     * @return
     */
    virtual bool isAllocated() { return true; }

    /**
     * @brief Gives this array a human readable name
     * @param name The name of this array
     */
    virtual void setInitValue(const std::string& initValue)
    {
      m_InitValue = QString::fromStdString(initValue);
    }

    /**
     * @brief Gives this array a human readable name
     * @param name The name of this array
     */
    virtual void setInitValue(const QString& initValue)
    {
      m_InitValue = initValue;
    }

    /**
     * @brief GetTypeName Returns a string representation of the type of data that is stored by this class. This
     * can be a primitive like char, float, int or the name of a class.
     * @return
     */
    void getXdmfTypeAndSize(QString& xdmfTypeName, int& precision)
    {
      xdmfTypeName = getNameOfClass();
      precision = 0;
    }

    /**
     * @brief getTypeAsString
     * @return
     */
    virtual QString getTypeAsString() { return "StringDataArray";}

    /**
     * @brief Gives this array a human readable name
     * @param name The name of this array
     */
    void setName(const QString& name)
    {
      m_Name = name;
    }

    /**
     * @brief Returns the human readable name of this array
     * @return
     */
    QString getName()
    {
      return m_Name;
    }

    /**
     * @brief
     */
    virtual void takeOwnership()
    {
      this->_ownsData = true;
    }

    /**
     * @brief
     */
    virtual void releaseOwnership()
    {
      this->_ownsData = false;
    }

    /**
    * @brief Returns a void pointer pointing to the index of the array. NULL
    * pointers are entirely possible. No checks are performed to make sure
    * the index is with in the range of the internal data array.
    * @param i The index to have the returned pointer pointing to.
    * @return Void Pointer. Possibly NULL.
    */
    virtual void* getVoidPointer ( size_t i)
    {
      return static_cast<void*>( &(m_Array[i]));
    }

    /**
     * @brief getStringArray
     * @return
     */
    virtual QVector<QString>& getStringArray()
    {
      return m_Array;
    }

    /**
    * @brief Returns the number of Tuples in the array.
    */
    virtual size_t getNumberOfTuples ()
    {
      return m_NumTuples;
    }


    /**
     * @brief Return the number of elements in the array
     * @return
     */
    virtual size_t getSize()
    {
      return m_Array.size();
    }

    virtual int getNumberOfComponents()
    {
      return 1;
    }

    QVector<size_t> getComponentDimensions()
    {
      QVector<size_t> dims(1, 1);
      return dims;
    }

    // Description:
    // Set/Get the dimension (n) of the rank. Must be >= 1. Make sure that
    // this is set before allocation.
    void SetRank(int rnk)
    {

    }

    /**
     * @brief getRank
     * @return
     */
    int getRank()
    {
      return 1;
    }

    /**
     * @brief Returns the number of bytes that make up the data type.
     * 1 = char
     * 2 = 16 bit integer
     * 4 = 32 bit integer/Float
     * 8 = 64 bit integer/Double
     */
    virtual size_t getTypeSize()
    {
      return sizeof(QString);
    }

    /**
     * @brief Removes Tuples from the Array. If the size of the vector is Zero nothing is done. If the size of the
     * vector is greater than or Equal to the number of Tuples then the Array is Resized to Zero. If there are
     * indices that are larger than the size of the original (before erasing operations) then an error code (-100) is
     * returned from the program.
     * @param idxs The indices to remove
     * @return error code.
     */
    virtual int eraseTuples(QVector<size_t>& idxs)
    {

      int err = 0;

      // If nothing is to be erased just return
      if(idxs.size() == 0)
      {
        return 0;
      }
      size_t idxs_size = static_cast<size_t>(idxs.size());
      if (idxs_size >= getNumberOfTuples() )
      {
        resize(0);
        return 0;
      }

      // Sanity Check the Indices in the vector to make sure we are not trying to remove any indices that are
      // off the end of the array and return an error code.
      for(QVector<size_t>::size_type i = 0; i < idxs.size(); ++i)
      {
        if (idxs[i] >= static_cast<size_t>(m_Array.size())) { return -100; }
      }


      // Create a new Array to copy into
      QVector<QString> newArray;
      QVector<size_t>::size_type start = 0;
      for(QVector<QString>::size_type i = 0; i < m_Array.size(); ++i)
      {
        bool keep = true;
        for(QVector<size_t>::size_type j = start; j < idxs.size(); ++j)
        {
          if (static_cast<size_t>(i) == idxs[j]) { keep = false; break;}
        }
        if (keep)
        {
          newArray.push_back(m_Array[i]);
        }
      }
      m_NumTuples = m_Array.size();
      return err;
    }

    /**
     * @brief Copies a Tuple from one position to another.
     * @param currentPos The index of the source data
     * @param newPos The destination index to place the copied data
     * @return
     */
    virtual int copyTuple(size_t currentPos, size_t newPos)
    {
      QString s = m_Array[currentPos];
      m_Array[newPos] = s;
      return 0;
    }

    /**
     * @brief reorderCopy
     * @param newOrderMap
     * @return
     */
    virtual IDataArray::Pointer reorderCopy(QVector<size_t> newOrderMap)
    {
      if(static_cast<size_t>(newOrderMap.size()) != getNumberOfTuples())
      {
        return IDataArray::NullPointer();
      }
      StringDataArray::Pointer daCopy = StringDataArray::CreateArray(getNumberOfTuples(), getName());
      daCopy->initializeWithZeros();
      for(QVector<QString>::size_type i = 0; i < m_Array.size(); ++i)
      {
        daCopy->setValue(newOrderMap[i], m_Array[i]);
      }
      return daCopy;
    }

    /**
     * @brief Does Nothing
     * @param pos The index of the Tuple
     * @param value pointer to value
     */
    virtual void initializeTuple(size_t pos, double value)
    {

    }

    /**
     * @brief Sets all the values to empty string.
     */
    virtual void initializeWithZeros()
    {
      m_Array.fill(QString(""), m_Array.size());
    }

    /**
     * @brief initializeWithValue
     * @param value
     */
    virtual void initializeWithValue(QString value)
    {
      m_Array.fill(value, m_Array.size());
    }

    /**
     * @brief initializeWithValue
     * @param value
     */
    virtual void initializeWithValue(const std::string& value)
    {
      m_Array.fill(QString::fromStdString(value), m_Array.size());
    }

    /**
     * @brief deepCopy
     * @param forceNoAllocate
     * @return
     */
    virtual IDataArray::Pointer deepCopy(bool forceNoAllocate = false)
    {
      StringDataArray::Pointer daCopy = StringDataArray::CreateArray(getNumberOfTuples(), getName());
      if(forceNoAllocate == false)
      {
        for(QVector<QString>::size_type i = 0; i < m_Array.size(); ++i)
        {
          daCopy->setValue(i, m_Array[i]);
        }
      }
      return daCopy;
    }


    /**
     * @brief Reseizes the internal array
     * @param size The new size of the internal array
     * @return 1 on success, 0 on failure
     */
    virtual int32_t resizeTotalElements(size_t size)
    {
      m_Array.resize(size);
      m_NumTuples = size;
      return 1;
    }

    /**
     * @brief Reseizes the internal array
     * @param size The new size of the internal array
     * @return 1 on success, 0 on failure
     */
    virtual int32_t resize(size_t numTuples)
    {
      m_Array.resize(numTuples);
      m_NumTuples = numTuples;
      return 1;
    }

    /**
     * @brief printTuple
     * @param out
     * @param i
     * @param delimiter
     */
    virtual void printTuple(QTextStream& out, size_t i, char delimiter = ',')
    {
      out << m_Array[i];
    }

    /**
     * @brief printComponent
     * @param out
     * @param i
     * @param j
     */
    virtual void printComponent(QTextStream& out, size_t i, int j)
    {
      out << m_Array[i];
    }

    /**
     * @brief getFullNameOfClass
     * @return
     */
    QString getFullNameOfClass()
    {
      return "StringDataArray";
    }

    /**
     *
     * @param parentId
     * @return
     */
    virtual int writeH5Data(hid_t parentId, QVector<size_t> tDims)
    {
      return H5DataArrayWriter::writeStringDataArray<StringDataArray>(parentId, this);
    }

    /**
     * @brief writeXdmfAttribute
     * @param out
     * @param volDims
     * @param hdfFileName
     * @param groupPath
     * @return
     */
    virtual int writeXdmfAttribute(QTextStream& out, int64_t* volDims, const QString& hdfFileName,
                                   const QString& groupPath, const QString& labelb)
    {
      out << "<!-- Xdmf is not supported for " << getNameOfClass() << " with type " << getTypeAsString() << " --> ";
      return -1;
    }

    /**
     * @brief getInfoString
     * @return Returns a formatted string that contains general infomation about
     * the instance of the object.
     */
    virtual QString getInfoString(DREAM3D::InfoStringFormat format)
    {
      QString info;
      QTextStream ss (&info);
      if(format == DREAM3D::HtmlFormat)
      {
        ss << "<html><head></head>\n";
        ss << "<body>\n";
        ss << "<table cellpadding=\"4\" cellspacing=\"0\" border=\"0\">\n";
        ss << "<tbody>\n";
        ss << "<tr bgcolor=\"#D3D8E0\"><th colspan=2>Attribute Array Info</th></tr>";
        ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Name:</th><td>" << getName() << "</td></tr>";
        ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Type:</th><td>" << getTypeAsString() << "</td></tr>";
        ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Number of Tuples:</th><td>" << getNumberOfTuples() << "</td></tr>";

//        QString compDimStr = "(";
//        for(int i = 0; i < m_CompDims.size(); i++)
//        {
//          compDimStr = compDimStr + QString::number(m_CompDims[i]);
//          if(i < m_CompDims.size() - 1) {
//             compDimStr = compDimStr + QString(", ");
//          }
//        }
//        compDimStr = compDimStr + ")";
//        ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Component Dimensions:</th><td>" << compDimStr << "</td></tr>";
//        ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Total Elements:</th><td>" << m_Size << "</td></tr>";

        ss << "</tbody></table>\n";
        ss << "<br/>";
        ss << "</body></html>";
      }
      else
      {

      }
      return info;
    }


    /**
     * @brief readH5Data
     * @param parentId
     * @return
     */
    virtual int readH5Data(hid_t parentId)
    {
      int err = 0;
      this->resize(0);

      err = QH5Lite::readVectorOfStringDataset(parentId, getName(), m_Array);
#if 0
      IDataArray::Pointer p = H5DataArrayReader::ReadStringDataArray(parentId, getName());
      if (p.get() == NULL)
      {
        return -1;
      }
      StringDataArray* srcPtr = StringDataArray::SafePointerDownCast(p.get());
      size_t count = srcPtr->getNumberOfTuples();
      for (size_t i = 0; i < count; ++i)
      {
        m_Array.push_back( srcPtr->getValue(i) );
      }
#endif
      return err;
    }

    /**
     * @brief setValue
     * @param i
     * @param value
     */
    void setValue(size_t i, const QString& value)
    {
      m_Array[i] = value;
    }

    /**
     * @brief getValue
     * @param i
     * @return
     */
    QString getValue(size_t i)
    {
      return m_Array.at(i);
    }

  protected:
    /**
    * @brief Protected Constructor
    * @param numElements The number of elements in the internal array.
    * @param takeOwnership Will the class clean up the memory. Default=true
    */
    StringDataArray(size_t numTuples, const QString name, bool allocate = true) :
      m_Name(name),
      m_NumTuples(numTuples),
      _ownsData(true)
    {
      if (allocate == true)
      {
        m_Array.resize(numTuples);
      }
    }

  private:
    QVector<QString> m_Array;
    QString m_Name;
    size_t m_NumTuples;
    bool _ownsData;
    QString m_InitValue;

    StringDataArray(const StringDataArray&); //Not Implemented
    void operator=(const StringDataArray&); //Not Implemented

};

#endif /* _StrignDataArray_H_ */

