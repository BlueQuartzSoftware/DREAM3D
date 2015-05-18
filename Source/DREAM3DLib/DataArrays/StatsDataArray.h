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


#ifndef _STATSDATAARRAY_H_
#define _STATSDATAARRAY_H_

#include <QtCore/QString>
#include <vector>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/StatsData/StatsData.h"
#include "DREAM3DLib/StatsData/PrimaryStatsData.h"
#include "DREAM3DLib/StatsData/PrecipitateStatsData.h"
#include "DREAM3DLib/StatsData/TransformationStatsData.h"
#include "DREAM3DLib/StatsData/BoundaryStatsData.h"
#include "DREAM3DLib/StatsData/MatrixStatsData.h"

/*
 *
 */
class DREAM3DLib_EXPORT StatsDataArray : public IDataArray
{
  public:
    DREAM3D_SHARED_POINTERS(StatsDataArray)
    DREAM3D_STATIC_NEW_MACRO(StatsDataArray)
    DREAM3D_TYPE_MACRO_SUPER(StatsDataArray, IDataArray)
    DREAM3D_CLASS_VERSION(2)

    virtual ~StatsDataArray();


    /**
     * @brief Static constructor
     * @param numElements The number of elements in the internal array.
     * @param name The name of the array
     * @return Boost::Shared_Ptr wrapping an instance of DataArrayTemplate<T>
     */
    static Pointer CreateArray(size_t numElements, const QString& name, bool allocate = true)
    {
      if (name.isEmpty() == true)
      {
        return NullPointer();
      }
      StatsDataArray::Pointer ptr = StatsDataArray::New();
      ptr->setName(name);
      std::vector<unsigned int> phase_types(numElements, DREAM3D::PhaseType::UnknownPhaseType);
      if(allocate) { ptr->fillArrayWithNewStatsData(numElements, &(phase_types.front()) ); }
      return ptr;
    }

    static Pointer CreateArray(size_t numTuples, int rank, size_t* dims, const QString& name, bool allocate = true)
    {
      if (name.isEmpty() == true)
      {
        return NullPointer();
      }
      StatsDataArray::Pointer ptr = StatsDataArray::New();
      ptr->setName(name);
      std::vector<unsigned int> phase_types(numTuples, DREAM3D::PhaseType::UnknownPhaseType);
      if(allocate) { ptr->fillArrayWithNewStatsData(numTuples, &(phase_types.front()) ); }
      return ptr;
    }

    static Pointer CreateArray(size_t numTuples, std::vector<size_t> cDims, const QString& name, bool allocate = true)
    {
      if (name.isEmpty() == true)
      {
        return NullPointer();
      }
      StatsDataArray::Pointer ptr = StatsDataArray::New();
      ptr->setName(name);
      std::vector<unsigned int> phase_types(numTuples, DREAM3D::PhaseType::UnknownPhaseType);
      if(allocate) { ptr->fillArrayWithNewStatsData(numTuples, &(phase_types.front()) ); }
      return ptr;
    }

    static Pointer CreateArray(size_t numTuples, QVector<size_t> cDims, const QString& name, bool allocate = true)
    {
      if (name.isEmpty() == true)
      {
        return NullPointer();
      }
      StatsDataArray::Pointer ptr = StatsDataArray::New();
      ptr->setName(name);
      std::vector<unsigned int> phase_types(numTuples, DREAM3D::PhaseType::UnknownPhaseType);
      if(allocate) { ptr->fillArrayWithNewStatsData(numTuples, &(phase_types.front()) ); }
      return ptr;
    }

    static Pointer CreateArray(QVector<size_t> tDims, QVector<size_t> cDims, const QString& name, bool allocate = true)
    {
      if (name.isEmpty() == true)
      {
        return NullPointer();
      }
      size_t numTuples = tDims[0];
      qint32 size = tDims.size();
      for(qint32 iter = 1; iter < size; iter++)
      {
        numTuples *= tDims[iter];
      }
      StatsDataArray::Pointer ptr = StatsDataArray::New();
      ptr->setName(name);
      std::vector<unsigned int> phase_types(numTuples, DREAM3D::PhaseType::UnknownPhaseType);
      if(allocate) { ptr->fillArrayWithNewStatsData(numTuples, &(phase_types.front()) ); }
      return ptr;
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
    virtual QString getTypeAsString() { return "StatsDataArray"; }


    DREAM3D_INSTANCE_PROPERTY(QVector<StatsData::Pointer>, StatsDataArray)

    virtual IDataArray::Pointer createNewArray(size_t numElements, int rank, size_t* dims, const QString& name, bool allocate = true)
    {
      return StatsDataArray::New();
    }

    virtual IDataArray::Pointer createNewArray(size_t numElements, std::vector<size_t> dims, const QString& name, bool allocate = true)
    {
      return StatsDataArray::New();
    }

    virtual IDataArray::Pointer createNewArray(size_t numElements, QVector<size_t> dims, const QString& name, bool allocate = true)
    {
      return StatsDataArray::New();
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
      m_StatsDataArray.clear();
    }

    /**
     *
     */
    void setStatsData(int index, StatsData::Pointer statsData)
    {
      if(index >= static_cast<int>(m_StatsDataArray.size()))
      {
        size_t old = m_StatsDataArray.size();
        m_StatsDataArray.resize(index + 1);
        // Initialize with zero length Vectors
        for (int i = old; i < m_StatsDataArray.size(); ++i)
        {
          m_StatsDataArray[i] = StatsData::New();
        }
      }
      m_StatsDataArray[index] = statsData;
    }

    /**
    *
    */
    void fillArrayWithNewStatsData(size_t n, unsigned int* phase_types)
    {
      m_StatsDataArray.resize(n);
      for (size_t i = 0; i < n; ++i)
      {
        if (phase_types != NULL)
        {
          if(phase_types[i] == DREAM3D::PhaseType::PrimaryPhase) { m_StatsDataArray[i] = PrimaryStatsData::New(); }
          else if(phase_types[i] == DREAM3D::PhaseType::PrecipitatePhase) { m_StatsDataArray[i] = PrecipitateStatsData::New(); }
          else if(phase_types[i] == DREAM3D::PhaseType::TransformationPhase) { m_StatsDataArray[i] = TransformationStatsData::New(); }
          else if(phase_types[i] == DREAM3D::PhaseType::BoundaryPhase) { m_StatsDataArray[i] = BoundaryStatsData::New(); }
          else if(phase_types[i] == DREAM3D::PhaseType::MatrixPhase) { m_StatsDataArray[i] = MatrixStatsData::New(); }
          else { m_StatsDataArray[i] = StatsData::New(); }
        }
        if(phase_types == NULL) { m_StatsDataArray[i] = StatsData::New(); }
      }
    }

    /**
     *
     */
    StatsData::Pointer getStatsData(int idx)
    {
#ifndef NDEBUG
      if(m_StatsDataArray.size() > 0)
      {
        BOOST_ASSERT(idx < static_cast<int>(m_StatsDataArray.size()));
      }
#endif
      return m_StatsDataArray[idx];
    }

    StatsData::Pointer operator[](int idx)
    {
#ifndef NDEBUG
      if(m_StatsDataArray.size() > 0)
      {
        BOOST_ASSERT(idx < static_cast<int>(m_StatsDataArray.size()));
      }
#endif
      return m_StatsDataArray[idx];
    }

    /* **************** This is the interface for the IDataArray Class which MUST
     *  Be implemented. Most of it is useless and will simply ASSERT if called. */

    void setName(const QString& name);
    QString getName();


    /**
     * @brief getFullNameOfClass
     * @return
     */
    QString getFullNameOfClass();

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
    virtual void* getVoidPointer(size_t i);

    /**
     * @brief Returns the number of Tuples in the array.
     */
    virtual size_t getNumberOfTuples();

    /**
     * @brief Return the number of elements in the array
     * @return
     */
    virtual size_t getSize();

    virtual int getNumberOfComponents();
    virtual QVector<size_t> getComponentDimensions();

    /**
     * @brief Returns the number of bytes that make up the data type.
     * 1 = char
     * 2 = 16 bit integer
     * 4 = 32 bit integer/Float
     * 8 = 64 bit integer/Double
     */
    virtual size_t getTypeSize();

    /**
     * @brief Removes Tuples from the Array. If the size of the vector is Zero nothing is done. If the size of the
     * vector is greater than or Equal to the number of Tuples then the Array is Resized to Zero. If there are
     * indices that are larger than the size of the original (before erasing operations) then an error code (-100) is
     * returned from the program.
     * @param idxs The indices to remove
     * @return error code.
     */
    virtual int eraseTuples(QVector<size_t>& idxs);

    /**
     * @brief Copies a Tuple from one position to another.
     * @param currentPos The index of the source data
     * @param newPos The destination index to place the copied data
     * @return
     */
    virtual int copyTuple(size_t currentPos, size_t newPos);

    virtual IDataArray::Pointer reorderCopy(QVector<size_t> newOrderMap);

    /**
     * @brief Splats the same value c across all values in the Tuple
     * @param i The index of the Tuple
     * @param c The value to splat across all components in the tuple
     */
    virtual void initializeTuple(size_t i, double p);

    /**
     * @brief Sets all the values to zero.
     */
    virtual void initializeWithZeros();


    virtual IDataArray::Pointer deepCopy(bool forceNoAllocate = false);

    /**
       * @brief Reseizes the internal array
       * @param size The new size of the internal array
       * @return 1 on success, 0 on failure
       */
    virtual int32_t resizeTotalElements(size_t size);

    /**
     * @brief Reseizes the internal array
     * @param size The new size of the internal array
     * @return 1 on success, 0 on failure
     */
    virtual int32_t resize(size_t numTuples);

    virtual void printTuple(QTextStream& out, size_t i, char delimiter = ',');
    virtual void printComponent(QTextStream& out, size_t i, int j);

    /**
     *
     * @param parentId
     * @return
     */
    virtual int writeH5Data(hid_t parentId, QVector<size_t> tDims);
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
    StatsDataArray();

  private:
    QString m_Name;
    bool m_IsAllocated;

    StatsDataArray(const StatsDataArray&); // Copy Constructor Not Implemented
    void operator=(const StatsDataArray&); // Operator '=' Not Implemented
};

#endif /* _STATSDATAARRAY_H_ */

