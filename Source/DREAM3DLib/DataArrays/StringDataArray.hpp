/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#ifndef _StrignDataArray_H_
#define _StrignDataArray_H_

#include <QtCore/QString>
#include <vector>




#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"

/**
 * @class StringDataArray StringDataArray.h DREAM3DLib/Common/StringDataArray.h
 * @brief Stores an array of QString objects
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Nov 13, 2012
 * @version 1.0
 */
class StringDataArray : public IDataArray
{
  public:
    DREAM3D_SHARED_POINTERS(StringDataArray )
    DREAM3D_TYPE_MACRO_SUPER(StringDataArray, IDataArray)

    static Pointer CreateArray(size_t numElements, const QString &name)
    {
      if (name.isEmpty() == true)
      {
        return NullPointer();
      }
      StringDataArray* d = new StringDataArray(numElements, true);
      d->SetName(name);
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
    virtual IDataArray::Pointer createNewArray(size_t numElements, int numComponents, const QString &name)
    {
      IDataArray::Pointer p = StringDataArray::CreateArray(numElements, name);
      return p;
    }

    virtual ~StringDataArray(){}

    /**
     * @brief isAllocated
     * @return
     */
    virtual bool isAllocated() { return true; }


    /**
     * @brief GetTypeName Returns a string representation of the type of data that is stored by this class. This
     * can be a primitive like char, float, int or the name of a class.
     * @return
     */
    void GetXdmfTypeAndSize(QString &xdmfTypeName, int &precision)
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
    void SetName(const QString &name)
    {
      m_Name = name;
    }

    /**
     * @brief Returns the human readable name of this array
     * @return
     */
    QString GetName()
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
    virtual void* GetVoidPointer ( size_t i)
    {
        return static_cast<void*>( &(m_Array[i]));
    }

     /**
     * @brief Returns the number of Tuples in the array.
     */
    virtual size_t GetNumberOfTuples ()
    {
    return m_Array.size();
    }


    /**
     * @brief Return the number of elements in the array
     * @return
     */
    virtual size_t GetSize()
    {
      return m_Array.size();
    }

    virtual void SetNumberOfComponents(int nc)
    {
      ;
    }

    virtual int GetNumberOfComponents()
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
    virtual size_t GetTypeSize()
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
    virtual int EraseTuples(QVector<size_t> &idxs)
    {

      int err = 0;

      // If nothing is to be erased just return
      if(idxs.size() == 0)
      {
        return 0;
      }

      if (idxs.size() >= GetNumberOfTuples() )
      {
        Resize(0);
        return 0;
      }

      // Sanity Check the Indices in the vector to make sure we are not trying to remove any indices that are
      // off the end of the array and return an error code.
      for(QVector<size_t>::size_type i = 0; i < idxs.size(); ++i)
      {
        if (idxs[i] >= m_Array.size()) { return -100; }
      }


      // Create a new Array to copy into
      QVector<QString> newArray;
      QVector<size_t>::size_type start = 0;
      for(QVector<QString>::size_type i = 0; i < m_Array.size(); ++i)
      {
        bool keep = true;
        for(QVector<size_t>::size_type j = start; j < idxs.size(); ++j)
        {
          if (i == idxs[j]) { keep = false; break;}
        }
        if (keep)
        {
          newArray.push_back(m_Array[i]);
        }
      }
      return err;
    }

    /**
     * @brief Copies a Tuple from one position to another.
     * @param currentPos The index of the source data
     * @param newPos The destination index to place the copied data
     * @return
     */
    virtual int CopyTuple(size_t currentPos, size_t newPos)
    {
      QString s = m_Array[currentPos];
      m_Array[newPos] = s;
      return 0;
    }


    /**
     * @brief Does Nothing
     * @param pos The index of the Tuple
     * @param value pointer to value
     */
    virtual void InitializeTuple(size_t pos, double value)
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
     * @brief Reseizes the internal array
     * @param size The new size of the internal array
     * @return 1 on success, 0 on failure
     */
    virtual int32_t RawResize(size_t size)
    {
      m_Array.resize(size);
      return 1;
    }

    /**
     * @brief Reseizes the internal array
     * @param size The new size of the internal array
     * @return 1 on success, 0 on failure
     */
    virtual int32_t Resize(size_t numTuples)
    {
      m_Array.resize(numTuples);
      return 1;
    }

    /**
     * @brief printTuple
     * @param out
     * @param i
     * @param delimiter
     */
    virtual void printTuple(QTextStream &out, size_t i, char delimiter = ',')
    {
      out << m_Array[i];
    }

    /**
     * @brief printComponent
     * @param out
     * @param i
     * @param j
     */
    virtual void printComponent(QTextStream &out, size_t i, int j)
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
    virtual int writeH5Data(hid_t parentId)
    {
      size_t totalSize = 0;
      for(size_t i = 0; i < m_Array.size(); ++i)
      {
        totalSize += 1 + m_Array[i].size();
      }
      DataArray<int8_t>::Pointer strPtr = DataArray<int8_t>::CreateArray(totalSize, "Strings");
      strPtr->initializeWithZeros();
      int8_t* str = strPtr->GetPointer(0);

      for(size_t i = 0; i < m_Array.size(); ++i)
      {
        ::memcpy(str, m_Array[i].toLatin1().data(), m_Array[i].size());
        str = str + m_Array[i].size() + 1;
      }

      return H5DataArrayWriter<int8_t>::writeArray(parentId, GetName(), totalSize, 1, strPtr->GetPointer(0), getFullNameOfClass());
    }

    /**
     * @brief writeXdmfAttribute
     * @param out
     * @param volDims
     * @param hdfFileName
     * @param groupPath
     * @return
     */
    virtual int writeXdmfAttribute(QTextStream &out, int64_t* volDims, const QString &hdfFileName,
                                    const QString &groupPath, const QString &labelb)
    {
      out << "<!-- Xdmf is not supported for " << getNameOfClass() << " with type " << getTypeAsString() << " --> ";
      return -1;
    }

    /**
     * @brief readH5Data
     * @param parentId
     * @return
     */
    virtual int readH5Data(hid_t parentId)
    {
      int err = 0;
      this->Resize(0);
      IDataArray::Pointer p = H5DataArrayReader::readStringDataArray(parentId, GetName());
      if (p.get() == NULL)
      {
        return -1;
      }
      StringDataArray* srcPtr = StringDataArray::SafePointerDownCast(p.get());
      size_t count = srcPtr->GetNumberOfTuples();
      for (size_t i = 0; i < count; ++i)
      {
        m_Array.push_back( srcPtr->GetValue(i) );
      }
      return err;
    }

    /**
     * @brief SetValue
     * @param i
     * @param value
     */
    void SetValue(size_t i, const QString &value)
    {
      m_Array[i] = value;
    }

    /**
     * @brief GetValue
     * @param i
     * @return
     */
    QString GetValue(size_t i)
    {
      return m_Array.at(i);
    }

  protected:
        /**
     * @brief Protected Constructor
     * @param numElements The number of elements in the internal array.
     * @param takeOwnership Will the class clean up the memory. Default=true
     */
    StringDataArray(size_t numElements, bool ownsData = true) :
      _ownsData(ownsData)
    {
      m_Array.resize(numElements);
    //  MUD_FLAP_0 = MUD_FLAP_1 = MUD_FLAP_2 = MUD_FLAP_3 = MUD_FLAP_4 = MUD_FLAP_5 = 0xABABABABABABABABul;
    }

  private:
      //  unsigned long long int MUD_FLAP_0;
    QVector<QString> m_Array;
  //  unsigned long long int MUD_FLAP_1;
    //size_t Size;
  //  unsigned long long int MUD_FLAP_4;
    bool _ownsData;
  //  unsigned long long int MUD_FLAP_2;
  //  size_t MaxId;
 //   unsigned long long int MUD_FLAP_3;
    QString m_Name;
  //  unsigned long long int MUD_FLAP_5;

    StringDataArray(const StringDataArray&); //Not Implemented
    void operator=(const StringDataArray&); //Not Implemented

};

#endif /* _StrignDataArray_H_ */
