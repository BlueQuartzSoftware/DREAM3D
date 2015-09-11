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


#ifndef _NEIGHBORLIST_H_
#define _NEIGHBORLIST_H_

#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QTextStream>

#include <vector>

#include <boost/shared_ptr.hpp>

#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"
#include "H5Support/QH5Lite.h"

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataArrays/DataArray.hpp"


/**
 * @class NeighborList NeighborList.hpp DREAM3DLib/Common/NeighborList.hpp
 * @brief Template class for wrapping raw arrays of data.
 * @author mjackson
 * @date July 3, 2008
 * @version 1.0
 */
template<typename T>
class NeighborList : public IDataArray
{
  public:
    SIMPL_SHARED_POINTERS(NeighborList<T> )
    SIMPL_TYPE_MACRO_SUPER(NeighborList<T>, IDataArray)
    SIMPL_CLASS_VERSION(2)

    SIMPL_INSTANCE_STRING_PROPERTY(NumNeighborsArrayName)

    static Pointer New()
    {
      return CreateArray(0, "NeighborList", false);
    }

    /**
     * @brief CreateArray
     * @param numTuples
     * @param name
     * @param allocate
     * @return
     */
    static Pointer CreateArray(size_t numTuples, const QString& name, bool allocate = true)
    {
      //std::cout << "NeighborList::CreateArray  name= " << name.toStdString() << "   numTuples= " << numTuples << std::endl;
      if (name.isEmpty() == true)
      {
        return NullPointer();
      }
      Pointer ptr = Pointer(new NeighborList<T>(numTuples, name));
      if (allocate) { ptr->resize(numTuples); }
      return ptr;
    }

    /**
     * @brief CreateArray
     * @param numTuples
     * @param rank
     * @param dims
     * @param name
     * @param allocate
     * @return
     */
    static Pointer CreateArray(size_t numTuples, int rank, size_t* dims, const QString& name, bool allocate = true)
    {
      //std::cout << "NeighborList::CreateArray  name= " << name.toStdString() << "   numTuples= " << numTuples << std::endl;
      if (name.isEmpty() == true)
      {
        return NullPointer();
      }

      size_t numElements = numTuples;
      for(int iter = 0; iter < rank; iter++)
      {
        numElements *= dims[iter];
      }
      Pointer ptr = Pointer(new NeighborList<T>(numElements, name));
      if (allocate) { ptr->resize(numElements); }
      return ptr;
    }

    /**
     * @brief CreateArray
     * @param numTuples
     * @param cDims
     * @param name
     * @param allocate
     * @return
     */
    static Pointer CreateArray(size_t numTuples, std::vector<size_t> cDims, const QString& name, bool allocate = true)
    {
      if (name.isEmpty() == true)
      {
        return NullPointer();
      }
      size_t numElements = numTuples;
      for(size_t iter = 0; iter < cDims.size(); iter++)
      {
        numElements *= cDims[iter];
      }
      Pointer ptr = Pointer(new NeighborList<T>(numElements, name));
      if (allocate) { ptr->resize(numElements); }
      return ptr;
    }

    /**
     * @brief CreateArray
     * @param numTuples
     * @param cDims
     * @param name
     * @param allocate
     * @return
     */
    static Pointer CreateArray(size_t numTuples, QVector<size_t> cDims, const QString& name, bool allocate = true)
    {
      if (name.isEmpty() == true)
      {
        return NullPointer();
      }
      size_t numElements = numTuples;
      for(int iter = 0; iter < cDims.size(); iter++)
      {
        numElements *= cDims[iter];
      }
      Pointer ptr = Pointer(new NeighborList<T>(numElements, name));
      if (allocate) { ptr->resize(numElements); }
      return ptr;
    }

    /**
     * @brief CreateArray
     * @param tDims
     * @param cDims
     * @param name
     * @param allocate
     * @return
     */
    static Pointer CreateArray(QVector<size_t> tDims, QVector<size_t> cDims, const QString& name, bool allocate = true)
    {
      if (name.isEmpty() == true)
      {
        return NullPointer();
      }
      size_t numElements = tDims[0];
      for(int iter = 1; iter < tDims.size(); iter++)
      {
        numElements *= tDims[iter];
      }
      for(int iter = 0; iter < cDims.size(); iter++)
      {
        numElements *= cDims[iter];
      }
      Pointer ptr = Pointer(new NeighborList<T>(numElements, name));
      if (allocate) { ptr->resize(numElements); }
      return ptr;
    }

    /**
     * @brief createNewArray
     * @param numElements
     * @param rank
     * @param dims
     * @param name
     * @return
     */
    virtual IDataArray::Pointer createNewArray(size_t numElements, int rank, size_t* dims, const QString& name, bool allocate = true)
    {
      return NeighborList<T>::CreateArray(numElements, rank, dims, name, allocate);
    }

    /**
     * @brief createNewArray
     * @param numElements
     * @param dims
     * @param name
     * @return
     */
    virtual IDataArray::Pointer createNewArray(size_t numElements, std::vector<size_t> dims, const QString& name, bool allocate = true)
    {
      return NeighborList<T>::CreateArray(numElements, dims, name, allocate);
    }

    /**
     * @brief createNewArray
     * @param numElements
     * @param dims
     * @param name
     * @return
     */
    virtual IDataArray::Pointer createNewArray(size_t numElements, QVector<size_t> dims, const QString& name, bool allocate = true)
    {
      return NeighborList<T>::CreateArray(numElements, dims, name, allocate);
    }


    typedef std::vector<T> VectorType;
    typedef boost::shared_ptr<VectorType> SharedVectorType;

    virtual ~NeighborList()
    {
      //std::cout << "~NeighborList<T> size()=" << _data.size() << std::endl;
    }

    /**
     * @brief isAllocated
     * @return
     */
    virtual bool isAllocated() { return true; }

    /**
     * @brief Gives this array a human readable name
     * @param name The name of this array
     */
    virtual void setInitValue(T initValue)
    {
      m_InitValue = initValue;
    }

    /**
     * @brief Sets all the values to value.
     */
    virtual void initializeWithValue(T initValue, size_t offset = 0)
    {

    }

    /**
     * @brief getTypeName Returns a string representation of the type of data that is stored by this class. This
     * can be a primitive like char, float, int or the name of a class.
     * @return
     */
    void getXdmfTypeAndSize(QString& xdmfTypeName, int& precision)
    {
      T value = 0x00;
      xdmfTypeName = "UNKNOWN";
      precision = 0;
      if (typeid(value) == typeid(int8_t)) { xdmfTypeName = "Char"; precision = 1;}
      if (typeid(value) == typeid(uint8_t)) { xdmfTypeName = "UChar"; precision = 1;}

      if (typeid(value) == typeid(int16_t)) { xdmfTypeName = "16 BIT NOT SUPPORTED BY XDMF"; precision = 0;}
      if (typeid(value) == typeid(uint16_t)) { xdmfTypeName = "16 BIT NOT SUPPORTED BY XDMF"; precision = 0;}

      if (typeid(value) == typeid(int32_t)) { xdmfTypeName = "Int"; precision = 4;}
      if (typeid(value) == typeid(uint32_t)) { xdmfTypeName = "UInt"; precision = 4;}

      if (typeid(value) == typeid(int64_t)) { xdmfTypeName = "Int"; precision = 8;}
      if (typeid(value) == typeid(uint64_t)) { xdmfTypeName = "UInt"; precision = 8;}

      if (typeid(value) == typeid(float)) { xdmfTypeName = "Float"; precision = 4;}
      if (typeid(value) == typeid(double)) { xdmfTypeName = "Float"; precision = 8;}

      if (typeid(value) == typeid(bool)) { xdmfTypeName = "uchar"; precision = 1;}
    }


    /**
     * @brief getTypeAsString
     * @return
     */
    virtual QString getTypeAsString() { return NeighborList<T>::ClassName();}

    /**
     * @brief setName
     * @param name
     */
    void setName(const QString& name) { m_Name = name; }

    /**
     * @brief getName
     * @return
     */
    QString getName() { return m_Name; }

    /**
     * @brief takeOwnership
     */
    void takeOwnership() {    }

    /**
     * @brief releaseOwnership
     */
    void releaseOwnership()  { }

    /**
     * @brief getVoidPointer
     * @param i
     * @return
     */
    void* getVoidPointer(size_t i) { return NULL; }

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

      size_t idxsSize = static_cast<size_t>(idxs.size());
      if (idxsSize >= getNumberOfTuples() )
      {
        resize(0);
        return 0;
      }

      size_t arraySize = m_Array.size();
      // Sanity Check the Indices in the vector to make sure we are not trying to remove any indices that are
      // off the end of the array and return an error code.
      for(QVector<size_t>::size_type i = 0; i < idxs.size(); ++i)
      {
        if (idxs[i] >= arraySize) { return -100; }
      }

      std::vector<SharedVectorType> replacement(arraySize - idxsSize);

      size_t idxsIndex = 0;
      size_t rIdx = 0;
      for(size_t dIdx = 0; dIdx < arraySize; ++dIdx)
      {
        if (dIdx != idxs[idxsIndex])
        {
          replacement[rIdx] = m_Array[dIdx];
          ++rIdx;
        }
        else
        {
          ++idxsIndex;
          if (idxsIndex == idxsSize ) { idxsIndex--;}
        }
      }
      m_Array = replacement;
      m_NumTuples = m_Array.size();
      return err;
    }

    /**
     * @brief copyTuple
     * @param currentPos
     * @param newPos
     * @return
     */
    virtual int copyTuple(size_t currentPos, size_t newPos)
    {
      m_Array[newPos] = m_Array[currentPos];
      return 0;
    }

    /**
     * @brief reorderCopy
     * @param newOrderMap
     * @return
     */
    virtual IDataArray::Pointer reorderCopy(QVector<size_t> newOrderMap)
    {
      size_t newOrderMapSize = static_cast<size_t>(newOrderMap.size());
      if( newOrderMapSize != getNumberOfTuples())
      {
        return IDataArray::NullPointer();
      }

      typename NeighborList<T>::Pointer daCopyPtr = NeighborList<T>::CreateArray(getNumberOfTuples(), "Copy of NeighborList", true);
      daCopyPtr->initializeWithZeros();
      size_t numTuples = getNumberOfTuples();
      for(size_t i = 0; i < numTuples; i++)
      {
        typename NeighborList<T>::SharedVectorType sharedNeiLst; //(new std::vector<T>);
        sharedNeiLst = m_Array[i];
        daCopyPtr->setList(newOrderMap[i], sharedNeiLst);
      }

      return daCopyPtr;
    }

    /**
     * @brief Splats the same value c across all values in the Tuple
     * @param i The index of the Tuple
     * @param c The value to splat across all components in the tuple
     */
    void initializeTuple(size_t i, double p)
    {
      BOOST_ASSERT(false);
    }

    /**
     * @brief Returns the number of elements in the internal array.
     */
    virtual size_t getNumberOfTuples()
    {
      return m_NumTuples;
    }


    /**
     * @brief getSize Returns the total number of data items that are being stored. This is the sum of all the sizes
     * of the internal storage arrays for this class.
     * @return
     */
    size_t getSize()
    {
      size_t total = 0;
      for(size_t dIdx = 0; dIdx < m_Array.size(); ++dIdx)
      {
        total += m_Array[dIdx]->size();
      }
      return total;
    }

    /**
     * @brief setNumberOfComponents
     * @param nc
     */
    void setNumberOfComponents(int nc) { }

    /**
     * @brief getNumberOfComponents
     * @return
     */
    int getNumberOfComponents() { return 1; }

    /**
     * @brief getComponentDimensions
     * @return
     */
    QVector<size_t> getComponentDimensions()
    {
      QVector<size_t> dims(1, 1);
      return dims;
    }

    /**
     * @brief SetRank
     * @param rnk
     */
    void SetRank(int rnk) { }

    /**
     * @brief getRank
     * @return
     */
    int getRank() { return 1; }

    /**
     * @brief getTypeSize
     * @return
     */
    size_t getTypeSize()  { return sizeof(SharedVectorType); }

    /**
     * @brief initializeWithZeros
     */
    void initializeWithZeros() {
      m_Array.clear();
      m_IsAllocated = false;
    }

    /**
     * @brief deepCopy
     * @return
     */
    IDataArray::Pointer deepCopy(bool forceNoAllocate = false)
    {
      typename NeighborList<T>::Pointer daCopyPtr = NeighborList<T>::CreateArray(getNumberOfTuples(), getName(), true);

      if(forceNoAllocate == false)
      {
        size_t count = (m_IsAllocated ? getNumberOfTuples(): 0);
        for(size_t i = 0; i < count; i++)
        {
          typename NeighborList<T>::SharedVectorType sharedNeiLst(new std::vector<T>(*(m_Array[i])));
          daCopyPtr->setList(static_cast<int>(i), sharedNeiLst);
        }
      }
      return daCopyPtr;
    }

    /**
     * @brief resizeTotalElements
     * @param size
     * @return
     */
    int32_t resizeTotalElements(size_t size)
    {
      //std::cout << "NeighborList::resizeTotalElements(" << size << ")" << std::endl;
      size_t old = m_Array.size();
      m_Array.resize(size);
      m_NumTuples = size;
      if (size == 0) { m_IsAllocated = false; }
      else { m_IsAllocated = true; }
      // Initialize with zero length Vectors
      for (size_t i = old; i < m_Array.size(); ++i)
      {
        m_Array[i] = SharedVectorType(new VectorType);
      }
      return 1;
    }

    /**
     * @brief Resize
     * @param numTuples
     * @return
     */
    virtual int32_t resize(size_t numTuples) { return resizeTotalElements(numTuples); }

    //FIXME: These need to be implemented
    virtual void printTuple(QTextStream& out, size_t i, char delimiter = ',')
    {
      SharedVectorType sharedVec = m_Array[i];
      VectorType* vec = sharedVec.get();
      size_t size = vec->size();
      out << size;
      for(size_t j = 0; j < size; j++)
      {
        out << delimiter << vec->at(j);
      }
    }

    /**
     * @brief printComponent
     * @param out
     * @param i
     * @param j
     */
    virtual void printComponent(QTextStream& out, size_t i, int j)
    {
      BOOST_ASSERT(false);
    }

    /**
     *
     * @param parentId
     * @return
     */
    virtual int writeH5Data(hid_t parentId, QVector<size_t> tDims)
    {
      int err = 0;

      // Generate the NumNeighbors array and also compute the total number
      // of elements that would be needed to flatten the array so we
      // can compare this with what is written in the file. If they are
      // different we are going to overwrite what is in the file with what
      // we compute here.
      Int32ArrayType::Pointer numNeighborsPtr = Int32ArrayType::CreateArray(m_Array.size(), m_NumNeighborsArrayName);
      int32_t* numNeighbors = numNeighborsPtr->getPointer(0);
      size_t total = 0;
      for(size_t dIdx = 0; dIdx < m_Array.size(); ++dIdx)
      {
        numNeighbors[dIdx] = static_cast<int32_t>(m_Array[dIdx]->size());
        total += m_Array[dIdx]->size();
      }

      // Check to see if the NumNeighbors is already written to the file
      bool rewrite = false;
      if (QH5Lite::datasetExists(parentId, m_NumNeighborsArrayName) == false)
      {
        // The NumNeighbors Array is NOT already in the file so write it to the file
        numNeighborsPtr->writeH5Data(parentId, tDims);
      }
      else
      {
        // The NumNeighbors array is in the dream3d file so read it up into memory and compare with what
        // we have in memory.
        std::vector<int32_t> fileNumNeigh(m_Array.size());
        err = QH5Lite::readVectorDataset(parentId, m_NumNeighborsArrayName, fileNumNeigh);
        if (err < 0)
        {
          return -602;
        }

        // Compare the 2 vectors to make sure they are exactly the same;
        if (fileNumNeigh.size() != numNeighborsPtr->getNumberOfTuples())
        {
          rewrite = true;
        }
        // The sizes are the same, now compare each value;
        int32_t* fileNumNeiPtr = &(fileNumNeigh.front());
        size_t nBytes = numNeighborsPtr->getNumberOfTuples() * sizeof(int32_t);
        if (::memcmp(numNeighbors, fileNumNeiPtr, nBytes) != 0)
        {
          rewrite = true;
        }
      }

      // Write out the NumNeighbors Array because something was different between what we computed at
      // the top of the function versus what is in memory
      if(rewrite == true)
      {
        numNeighborsPtr->writeH5Data(parentId, tDims);
      }

      // Allocate an array of the proper size so we can concatenate all the arrays together into a single array that
      // can be written to the HDF5 File. This operation can ballon the memory size temporarily until this operation
      // is complete.
      QVector<T> flat (total);
      size_t currentStart = 0;
      for(size_t dIdx = 0; dIdx < m_Array.size(); ++dIdx)
      {
        size_t nEle = m_Array[dIdx]->size();
        if (nEle == 0) { continue; }
        T* start = &(m_Array[dIdx]->front()); // get the pointer to the front of the array
        //    T* end = start + nEle; // get the pointer to the end of the array
        T* dst = &(flat.front()) + currentStart;
        ::memcpy(dst, start, nEle * sizeof(T));

        currentStart += m_Array[dIdx]->size();
      }

      // Now we can actually write the actual array data.
      int32_t rank = 1;
      hsize_t dims[1] = { total };
      if (total > 0)
      {
        err = QH5Lite::writePointerDataset(parentId, getName(), rank, dims, &(flat.front()));
        if(err < 0)
        {
          return -605;
        }

        err = QH5Lite::writeScalarAttribute(parentId, getName(), DREAM3D::HDF5::DataArrayVersion, getClassVersion());
        if(err < 0)
        {
          return -604;
        }
        err = QH5Lite::writeStringAttribute(parentId, getName(), DREAM3D::HDF5::ObjectType, getNameOfClass());
        if(err < 0)
        {
          return -607;
        }

        // Write the tuple dimensions as an attribute
        hsize_t size = tDims.size();
        err = QH5Lite::writePointerAttribute(parentId, getName(), DREAM3D::HDF5::TupleDimensions, 1, &size, tDims.data());
        if (err < 0)
        {
          return -609;
        }

        QVector<size_t> cDims = getComponentDimensions();
        // write the component dimensions as  an attribute
        size = cDims.size();
        err = QH5Lite::writePointerAttribute(parentId, getName(), DREAM3D::HDF5::ComponentDimensions, 1, &size, cDims.data());
        if (err < 0)
        {
          return -610;
        }

        err = QH5Lite::writeStringAttribute(parentId, getName(), "Linked NumNeighbors Dataset", m_NumNeighborsArrayName);
        if(err < 0)
        {
          return -608;
        }
      }
      return err;
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
                                   const QString& groupPath, const QString& label)
    {
      int precision = 0;
      QString xdmfTypeName;
      getXdmfTypeAndSize(xdmfTypeName, precision);

      out << "    <Attribute Name=\"" << getName() << label << "\" AttributeType=\"Scalar\" Center=\"Node\">";
      out << "      <DataItem Format=\"HDF\" Dimensions=\"" << volDims[0] << " " << volDims[1] << " " << volDims[2] << "\" ";
      out << "NumberType=\"" << xdmfTypeName << "\" " << "Precision=\"" << precision << "\" >";
      out << "        " << hdfFileName.toLatin1().data() << groupPath.toLatin1().data() << "/" << getName();
      out << "      </DataItem>";
      out << "    </Attribute>";
      return 1;
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
        ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Number of Lists:</th><td>" << getNumberOfLists() << "</td></tr>";

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
     *
     * @param parentId
     * @return
     */
    virtual int readH5Data(hid_t parentId)
    {
      int err = 0;

      // Generate the number of neighbors array and also compute the total number
      // of elements that would be needed to flatten the array
      std::vector<int32_t> numNeighbors;

      // Check to see if the NumNeighbors exists in the file, which it must.
      if(QH5Lite::datasetExists(parentId, m_NumNeighborsArrayName) == true)
      {
        err = QH5Lite::readVectorDataset(parentId, m_NumNeighborsArrayName, numNeighbors);
        if(err < 0)
        {
          return -702;
        }

      }
      else
      {
        return -703;
      }

      std::vector<T> flat;
      err = QH5Lite::readVectorDataset(parentId, getName(), flat);
      if (err < 0)
      {
        return err;
      }
      // Loop over all the entries and make new Vectors to hold the incoming data
      m_Array.resize(numNeighbors.size());
      m_IsAllocated = true;
      size_t currentStart = 0;
      qint32 count = static_cast<qint32>(numNeighbors.size());
      for(QVector<int32_t>::size_type dIdx = 0; dIdx < count; ++dIdx)
      {
        size_t nEle = numNeighbors[dIdx];
        if(nEle > 0)
        {
          m_Array[dIdx] = SharedVectorType(new VectorType(numNeighbors[dIdx]));

          T* dst = &(m_Array[dIdx]->front()); // get the pointer to the front of the array
          //    T* end = start + nEle; // get the pointer to the end of the array
          T* start = &(flat.front()) + currentStart;
          ::memcpy(dst, start, nEle * sizeof(T));
          currentStart += nEle;
        }
        else
        {
          m_Array[dIdx] = SharedVectorType(new VectorType(0));
        }
      }
      m_NumTuples = m_Array.size(); // Sync up the numTuples property with the size of the internal array
      return err;
    }

    /**
     * @brief addEntry
     * @param grainId
     * @param value
     */
    void addEntry(int grainId, int value)
    {
      if(grainId >= static_cast<int>(m_Array.size()) )
      {
        size_t old = m_Array.size();
        m_Array.resize(grainId + 1);
        m_IsAllocated = true;
        // Initialize with zero length Vectors
        for(size_t i = old; i < m_Array.size(); ++i)
        {
          m_Array[i] = SharedVectorType(new VectorType);
        }
      }
      m_Array[grainId]->push_back(value);
      m_NumTuples = m_Array.size();
    }

    /**
     * @brief clearAllLists
     */
    void clearAllLists()
    {
      m_Array.clear();
      m_IsAllocated = false;
    }


    /**
     * @brief setList
     * @param grainId
     * @param neighborList
     */
    void setList(int grainId, SharedVectorType neighborList)
    {
      if(grainId >= static_cast<int>(m_Array.size()) )
      {
        size_t old = m_Array.size();
        m_Array.resize(grainId + 1);
        m_IsAllocated = true;
        // Initialize with zero length Vectors
        for(size_t i = old; i < m_Array.size(); ++i)
        {
          m_Array[i] = SharedVectorType(new VectorType);
        }
      }
      m_Array[grainId] = neighborList;
    }

    /**
     * @brief getValue
     * @param grainId
     * @param index
     * @param ok
     * @return
     */
    T getValue(int grainId, int index, bool& ok)
    {
#ifndef NDEBUG
      if (m_Array.size() > 0u) { BOOST_ASSERT(grainId < static_cast<int>(m_Array.size()));}
#endif
      SharedVectorType vec = m_Array[grainId];
      if(index < 0 || static_cast<size_t>(index) >= vec->size())
      {
        ok = false;
        return -1;
      }
      return (*vec)[index];
    }

    /**
     * @brief getNumberOfLists
     * @return
     */
    int getNumberOfLists()
    {
      return static_cast<int>(m_Array.size());
    }

    /**
     * @brief getListSize
     * @param grainId
     * @return
     */
    int getListSize(int grainId)
    {
#ifndef NDEBUG
      if (m_Array.size() > 0u) { BOOST_ASSERT(grainId < static_cast<int>(m_Array.size()));}
#endif
      return static_cast<int>(m_Array[grainId]->size());
    }

    VectorType& getListReference(int grainId)
    {
#ifndef NDEBUG
      if (m_Array.size() > 0u) { BOOST_ASSERT(grainId < static_cast<int>(m_Array.size()));}
#endif
      return *(m_Array[grainId]);
    }

    /**
     * @brief getList
     * @param grainId
     * @return
     */
    SharedVectorType getList(int grainId)
    {
#ifndef NDEBUG
      if (m_Array.size() > 0u) { BOOST_ASSERT(grainId < static_cast<int>(m_Array.size()));}
#endif
      return m_Array[grainId];
    }

    /**
     * @brief copyOfList
     * @param grainId
     * @return
     */
    VectorType copyOfList(int grainId)
    {
#ifndef NDEBUG
      if (m_Array.size() > 0u) { BOOST_ASSERT(grainId < static_cast<int>(m_Array.size()));}
#endif

      VectorType copy(*(m_Array[grainId]));
      return copy;
    }

    /**
     * @brief operator []
     * @param grainId
     * @return
     */
    VectorType& operator[](int grainId)
    {
#ifndef NDEBUG
      if (m_Array.size() > 0u) { BOOST_ASSERT(grainId < static_cast<int>(m_Array.size()));}
#endif
      return *(m_Array[grainId]);
    }

    /**
     * @brief operator []
     * @param grainId
     * @return
     */
    VectorType& operator[](size_t grainId)
    {
#ifndef NDEBUG
      if (m_Array.size() > 0ul) { BOOST_ASSERT(grainId < m_Array.size());}
#endif
      return *(m_Array[grainId]);

    }


  protected:
    /**
     * @brief NeighborList
     */
    NeighborList(size_t numTuples, const QString name) :
      m_NumNeighborsArrayName(DREAM3D::FeatureData::NumNeighbors),
      m_Name(name),
      m_NumTuples(numTuples),
      m_IsAllocated(false)
    {    }

  private:
    std::vector<SharedVectorType> m_Array;
    QString m_Name;
    size_t m_NumTuples;
    bool m_IsAllocated;
    T m_InitValue;


    NeighborList(const NeighborList&); // Copy Constructor Not Implemented
    void operator=(const NeighborList&); // Operator '=' Not Implemented
};

typedef NeighborList<int32_t> Int32NeighborListType;
typedef NeighborList<float> FloatNeighborListType;

#endif /* NEIGHBORLIST_H_ */

