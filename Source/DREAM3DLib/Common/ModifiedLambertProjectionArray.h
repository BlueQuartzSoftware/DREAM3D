/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#ifndef _ModifiedLambertProjectionArray_H_
#define _ModifiedLambertProjectionArray_H_

#include <QtCore/QString>
#include <vector>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/ModifiedLambertProjection.h"


/*
 *
 */
class DREAM3DLib_EXPORT ModifiedLambertProjectionArray : public IDataArray
{
  public:
    DREAM3D_SHARED_POINTERS(ModifiedLambertProjectionArray)
    DREAM3D_STATIC_NEW_MACRO(ModifiedLambertProjectionArray)
    DREAM3D_TYPE_MACRO_SUPER(ModifiedLambertProjectionArray, IDataArray)

    virtual ~ModifiedLambertProjectionArray();

    /**
     * @brief GetTypeName Returns a string representation of the type of data that is stored by this class. This
     * can be a primitive like char, float, int or the name of a class.
     * @return
     */
    void GetXdmfTypeAndSize(QString& xdmfTypeName, int& precision)
    {
      xdmfTypeName = getNameOfClass();
      precision = 0;
    }

    /**
     * @brief getTypeAsString
     * @return
     */
    virtual QString getTypeAsString() { return "ModifiedLambertProjectionArray"; }

    DREAM3D_INSTANCE_PROPERTY(int, Phase)
    DREAM3D_INSTANCE_PROPERTY(QVector<ModifiedLambertProjection::Pointer>, ModifiedLambertProjectionArray)

    IDataArray::Pointer createNewArray(size_t numElements, int numComponents, const QString& name)
    {
      return ModifiedLambertProjectionArray::New();
    }

    /**
    * @brief
    */
    virtual bool isAllocated()
    {
      return m_IsAllocated;
    }


    /**
     *
     */
    void clearAll()
    {
      m_ModifiedLambertProjectionArray.clear();
    }

    /**
     *
     */
    void setModifiedLambertProjection(int index, ModifiedLambertProjection::Pointer ModifiedLambertProjection)
    {
      if(index >= static_cast<int>(m_ModifiedLambertProjectionArray.size()))
      {
        size_t old = m_ModifiedLambertProjectionArray.size();
        m_ModifiedLambertProjectionArray.resize(index + 1);
        // Initialize with zero length Vectors
        for (size_t i = old; i < m_ModifiedLambertProjectionArray.size(); ++i)
        {
          m_ModifiedLambertProjectionArray[i] = ModifiedLambertProjection::New();
        }
      }
      m_ModifiedLambertProjectionArray[index] = ModifiedLambertProjection;
    }

    /**
    *
    */
    void fillArrayWithNewModifiedLambertProjection(size_t n)
    {
      m_ModifiedLambertProjectionArray.resize(n);
      for (size_t i = 0; i < n; ++i)
      {
        if (m_ModifiedLambertProjectionArray[i].get() == NULL)
        {
          m_ModifiedLambertProjectionArray[i] = ModifiedLambertProjection::New();
        }
      }
    }

    /**
     *
     */
    ModifiedLambertProjection::Pointer getModifiedLambertProjection(int idx)
    {
#ifndef NDEBUG
      if(m_ModifiedLambertProjectionArray.size() > 0u)
      {
        BOOST_ASSERT(idx < static_cast<int>(m_ModifiedLambertProjectionArray.size()));
      }
#endif
      return m_ModifiedLambertProjectionArray[idx];
    }

    ModifiedLambertProjection::Pointer operator[](int idx)
    {
#ifndef NDEBUG
      if(m_ModifiedLambertProjectionArray.size() > 0u)
      {
        BOOST_ASSERT(idx < static_cast<int>(m_ModifiedLambertProjectionArray.size()));
      }
#endif
      return m_ModifiedLambertProjectionArray[idx];
    }

    /* **************** This is the interface for the IDataArray Class which MUST
     *  Be implemented. Most of it is useless and will simply ASSERT if called. */

    void SetName(const QString& name);
    QString GetName();

    /**
     * @brief Makes this class responsible for freeing the memory.
     */
    virtual void takeOwnership();

    /**
     * @brief This class will NOT free the memory associated with the internal pointer.
     * This can be useful if the user wishes to keep the data around after this
     * class goes out of scope.
     */
    virtual void releaseOwnership();

    /**
     * @brief Returns a void pointer pointing to the index of the array. NULL
     * pointers are entirely possible. No checks are performed to make sure
     * the index is with in the range of the internal data array.
     * @param i The index to have the returned pointer pointing to.
     * @return Void Pointer. Possibly NULL.
     */
    virtual void* GetVoidPointer(size_t i);

    /**
     * @brief Returns the number of Tuples in the array.
     */
    virtual size_t getNumberOfTuples();

    /**
     * @brief Return the number of elements in the array
     * @return
     */
    virtual size_t GetSize();

    virtual void SetNumberOfComponents(int nc);
    virtual int GetNumberOfComponents();

    /**
     * @brief Returns the number of bytes that make up the data type.
     * 1 = char
     * 2 = 16 bit integer
     * 4 = 32 bit integer/Float
     * 8 = 64 bit integer/Double
     */
    virtual size_t GetTypeSize();


    /**
     * @brief Removes Tuples from the Array. If the size of the vector is Zero nothing is done. If the size of the
     * vector is greater than or Equal to the number of Tuples then the Array is Resized to Zero. If there are
     * indices that are larger than the size of the original (before erasing operations) then an error code (-100) is
     * returned from the program.
     * @param idxs The indices to remove
     * @return error code.
     */
    virtual int EraseTuples(QVector<size_t>& idxs);

    /**
     * @brief Copies a Tuple from one position to another.
     * @param currentPos The index of the source data
     * @param newPos The destination index to place the copied data
     * @return
     */
    virtual int CopyTuple(size_t currentPos, size_t newPos);

    /**
     * @brief Splats the same value c across all values in the Tuple
     * @param i The index of the Tuple
     * @param c The value to splat across all components in the tuple
     */
    virtual void InitializeTuple(size_t i, double p);

    /**
     * @brief Sets all the values to zero.
     */
    virtual void initializeWithZeros();

    /**
       * @brief Reseizes the internal array
       * @param size The new size of the internal array
       * @return 1 on success, 0 on failure
       */
    virtual int32_t RawResize(size_t size);

    /**
     * @brief Reseizes the internal array
     * @param size The new size of the internal array
     * @return 1 on success, 0 on failure
     */
    virtual int32_t Resize(size_t numTuples);

    virtual void printTuple(QTextStream& out, size_t i, char delimiter = ',');
    virtual void printComponent(QTextStream& out, size_t i, int j);

    /**
     *
     * @param parentId
     * @return
     */
    virtual int writeH5Data(hid_t parentId);
    virtual int readH5Data(hid_t parentId);

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

  protected:
    ModifiedLambertProjectionArray();

  private:
    QString m_Name;
    bool m_IsAllocated;

    ModifiedLambertProjectionArray(const ModifiedLambertProjectionArray&); // Copy Constructor Not Implemented
    void operator=(const ModifiedLambertProjectionArray&); // Operator '=' Not Implemented
};

#endif /* _ModifiedLambertProjectionArray_H_ */
