/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#ifndef NEIGHBORLIST_H_
#define NEIGHBORLIST_H_

#include <string>
#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "H5Support/H5Utilities.h"


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/IDataArray.h"

template<typename T>
class NeighborList : public IDataArray
{
  public:
    DREAM3D_SHARED_POINTERS(NeighborList<T> )
    DREAM3D_STATIC_NEW_MACRO(NeighborList<T> )
    DREAM3D_TYPE_MACRO_SUPER(NeighborList<T>, IDataArray)

    IDataArray::Pointer createNewArray(size_t numElements, int numComponents, const std::string &name)
    {
      return NeighborList<T>::New();
    }
    typedef std::vector<T> VectorType;
    typedef boost::shared_ptr<VectorType> SharedVectorType;

    virtual ~NeighborList() {}

    /**
     * @brief GetTypeName Returns a string representation of the type of data that is stored by this class. This
     * can be a primitive like char, float, int or the name of a class.
     * @return
     */
    void GetXdmfTypeAndSize(std::string &xdmfTypeName, int &precision)
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
    virtual std::string getTypeAsString() { return NeighborList<T>::ClassName();}

    void SetName(const std::string &name) { m_Name = name; }
    std::string GetName() { return m_Name; }


    void takeOwnership() {    }
    void releaseOwnership()  { }


    void* GetVoidPointer(size_t i) { return NULL; }

    /**
     * @brief Removes Tuples from the Array. If the size of the vector is Zero nothing is done. If the size of the
     * vector is greater than or Equal to the number of Tuples then the Array is Resized to Zero. If there are
     * indices that are larger than the size of the original (before erasing operations) then an error code (-100) is
     * returned from the program.
     * @param idxs The indices to remove
     * @return error code.
     */
    virtual int EraseTuples(std::vector<size_t> &idxs)
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
      for(std::vector<size_t>::size_type i = 0; i < idxs.size(); ++i)
      {
        if (idxs[i] >= _data.size()) { return -100; }
      }


      std::vector<SharedVectorType> replacement(_data.size() - idxs.size());
      size_t idxsIndex = 0;
      size_t rIdx = 0;
      for(size_t dIdx = 0; dIdx < _data.size(); ++dIdx)
      {
        if (dIdx != idxs[idxsIndex])
        {
          replacement[rIdx] = _data[dIdx];
          ++rIdx;
        }
        else
        {
          ++idxsIndex;
          if (idxsIndex == idxs.size() ) { idxsIndex--;}
        }
      }
      _data = replacement;
      return err;
    }

    virtual int CopyTuple(size_t currentPos, size_t newPos)
    {
      _data[newPos] = _data[currentPos];
      return 0;
    }

    /**
     * @brief Splats the same value c across all values in the Tuple
     * @param i The index of the Tuple
     * @param c The value to splat across all components in the tuple
     */
    void InitializeTuple(size_t i, double p)
    {
      BOOST_ASSERT(false);
    }



    size_t GetNumberOfTuples() {   return _data.size(); }

    /**
     * @brief GetSize Returns the total number of data items that are being stored. This is the sum of all the sizes
     * of the internal storage arrays for this class.
     * @return
     */
    size_t GetSize() {
      size_t total = 0;
      for(size_t dIdx = 0; dIdx < _data.size(); ++dIdx)
      {
        total += _data[dIdx]->size();
      }
      return total;
    }


    void SetNumberOfComponents(int nc) { }


    int GetNumberOfComponents() { return 1; }


    size_t GetTypeSize()  { return sizeof(SharedVectorType); }


    void initializeWithZeros() { _data.clear(); }


    int32_t RawResize(size_t size)
    {
      size_t old = _data.size();
      _data.resize(size);
      // Initialize with zero length Vectors
      for (size_t i = old; i < _data.size(); ++i)
      {
        _data[i] = SharedVectorType(new VectorType);
      }
      return 1;
    }

    /**
     * @brief Resize
     * @param numTuples
     * @return
     */
    virtual int32_t Resize(size_t numTuples) { return RawResize(numTuples); }


    //FIXME: These need to be implemented
    virtual void printTuple(std::ostream &out, size_t i, char delimiter = ',')
    {
      SharedVectorType sharedVec = _data[i];
      VectorType* vec = sharedVec.get();
      size_t size = vec->size();
      out << size;
      for(size_t i=0;i<size;i++)
      {
        out << delimiter << vec->at(i);
      }
    }

    virtual void printComponent(std::ostream &out, size_t i, int j)
    {
      BOOST_ASSERT(false);
    }

    /**
     *
     * @param parentId
     * @return
     */
    virtual int writeH5Data(hid_t parentId)
    {

      int err = 0;

      // Generate the number of neighbors array and also compute the total number
      // of elements that would be needed to flatten the array
      std::vector<int32_t> numNeighbors(_data.size());

      size_t total = 0;
      for(size_t dIdx = 0; dIdx < _data.size(); ++dIdx)
      {
        numNeighbors[dIdx] = static_cast<int32_t>(_data[dIdx]->size());
        total += _data[dIdx]->size();
      }

      // Check to see if the NumNeighbors is already written to the file
      bool rewrite = false;
      if (H5Lite::datasetExists(parentId, DREAM3D::FieldData::NumNeighbors) == false)
      {
        rewrite = true;
      }
      else
      {
        std::vector<int32_t> fileNumNeigh(_data.size());
        err = H5Lite::readVectorDataset(parentId, DREAM3D::FieldData::NumNeighbors, fileNumNeigh);
        if (err < 0)
        {
          return -602;
        }

        // Compare the 2 vectors to make sure they are exactly the same;
        if (fileNumNeigh.size() != numNeighbors.size())
        {
          rewrite = true;
        }
        // The sizes are the same, now compare each value;
        int32_t* numNeighPtr = &(numNeighbors.front());
        int32_t* fileNumNeiPtr = &(fileNumNeigh.front());
        size_t nBytes = numNeighbors.size() * sizeof(int32_t);
        if (::memcmp(numNeighPtr, fileNumNeiPtr, nBytes) != 0)
        {
          rewrite = true;
        }
      }

      // Write out the NumNeighbors Array
      if(rewrite == true)
      {
        std::vector<hsize_t> dims(1, numNeighbors.size());
        err = H5Lite::writeVectorDataset(parentId, DREAM3D::FieldData::NumNeighbors, dims, numNeighbors);
        if(err < 0)
        {
          return -603;
        }
        err = H5Lite::writeScalarAttribute(parentId, DREAM3D::FieldData::NumNeighbors, std::string(H5_NUMCOMPONENTS), 1);
        if(err < 0)
        {
          return -605;
        }
        err = H5Lite::writeStringAttribute(parentId, DREAM3D::FieldData::NumNeighbors, DREAM3D::HDF5::ObjectType, "DataArray<T>");
        if(err < 0)
        {
          return -604;
        }
      }

      // Allocate an array of the proper size to we can concatenate all the arrays together into a single array that
      // can be written to the HDF5 File. This operation can ballon the memory size temporarily until this operation
      // is complete.
      std::vector<T> flat (total);
      size_t currentStart = 0;
      for(size_t dIdx = 0; dIdx < _data.size(); ++dIdx)
      {
        size_t nEle = _data[dIdx]->size();
        if (nEle == 0) { continue; }
        T* start = &(_data[dIdx]->front()); // Get the pointer to the front of the array
        //    T* end = start + nEle; // Get the pointer to the end of the array
        T* dst = &(flat.front()) + currentStart;
        ::memcpy(dst, start, nEle*sizeof(T));

        currentStart += _data[dIdx]->size();
      }

      int32_t rank = 1;
      hsize_t dims[1] = { total };
      err = H5Lite::writePointerDataset(parentId, GetName(), rank, dims, &(flat.front()));
      if(err < 0)
      {
        return -605;
      }
      err = H5Lite::writeScalarAttribute(parentId, GetName(), std::string(H5_NUMCOMPONENTS), 1);
      if(err < 0)
      {
        return -606;
      }

      err = H5Lite::writeStringAttribute(parentId, GetName(), DREAM3D::HDF5::ObjectType, getNameOfClass());
      if(err < 0)
      {
        return -607;
      }

      err = H5Lite::writeStringAttribute(parentId, GetName(), "Linked NumNeighbors Dataset", DREAM3D::FieldData::NumNeighbors);
      if(err < 0)
      {
        return -608;
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
    virtual int writeXdmfAttribute(std::ostream &out, int64_t* volDims, const std::string &hdfFileName,
            const std::string &groupPath, const std::string &label)
    {

      std::stringstream dimStr;
      dimStr << volDims[0] << " " << volDims[1] << " " << volDims[2] << " ";
      int precision = 0;
      std::string xdmfTypeName;
      GetXdmfTypeAndSize(xdmfTypeName, precision);

      /*
     <Attribute Name="MisorientationList" AttributeType="Scalar" Center="Cell">
      <DataItem Format="HDF" Dimensions="52140" NumberType="Float" Precision="4" >
        test40638_01.dream3d:/VoxelDataContainer/FIELD_DATA/MisorientationList
      </DataItem>
    </Attribute>
    */
      out << "    <Attribute Name=\"" << GetName() << label << "\" AttributeType=\"Scalar\" Center=\"Node\">" << std::endl;
      out << "      <DataItem Format=\"HDF\" Dimensions=\"" << dimStr.str() <<  "\" ";
      out << "NumberType=\"" << xdmfTypeName << "\" " << "Precision=\"" << precision << "\" >" << std::endl;
      out << "        " << hdfFileName << groupPath << "/" << GetName() << std::endl;
      out << "      </DataItem>" << std::endl;
      out << "    </Attribute>" << std::endl << std::endl;
      return 1;
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
      if(H5Lite::datasetExists(parentId, DREAM3D::FieldData::NumNeighbors) == true)
      {
        err = H5Lite::readVectorDataset(parentId, DREAM3D::FieldData::NumNeighbors, numNeighbors);
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
      err = H5Lite::readVectorDataset(parentId, GetName(), flat);
      if (err < 0)
      {
        return err;
      }

      // Loop over all the entries and make new Vectors to hold the incoming data
      _data.resize(numNeighbors.size());
      size_t currentStart = 0;
      for(std::vector<int32_t>::size_type dIdx = 0; dIdx < numNeighbors.size(); ++dIdx)
      {
        size_t nEle = numNeighbors[dIdx];
        if(nEle > 0)
        {
          _data[dIdx] = SharedVectorType(new VectorType(numNeighbors[dIdx]));

          T* dst = &(_data[dIdx]->front()); // Get the pointer to the front of the array
          //    T* end = start + nEle; // Get the pointer to the end of the array
          T* start = &(flat.front()) + currentStart;
          ::memcpy(dst, start, nEle * sizeof(T));
          currentStart += nEle;
        }
        else
        {
          _data[dIdx] = SharedVectorType(new VectorType(0));
        }
      }


      return err;
    }

    /**
 *
 */
    void addEntry(int grainId, int value)
    {
      if(grainId >= static_cast<int>(_data.size()) )
      {
        size_t old = _data.size();
        _data.resize(grainId + 1);
        // Initialize with zero length Vectors
        for(size_t i = old; i < _data.size(); ++i)
        {
          _data[i] = SharedVectorType(new VectorType);
        }
      }
      _data[grainId]->push_back(value);
    }

    /**
     *
     */
    void clearAllLists()
    {
      _data.clear();
    }


    /**
     *
     */
    void setList(int grainId, SharedVectorType neighborList)
    {
      if(grainId >= static_cast<int>(_data.size()) )
      {
        size_t old = _data.size();
        _data.resize(grainId + 1);
        // Initialize with zero length Vectors
        for(size_t i = old; i < _data.size(); ++i)
        {
          _data[i] = SharedVectorType(new VectorType);
        }
      }
      _data[grainId] = neighborList;
    }

    /**
     *
     */
    T getValue(int grainId, int index, bool &ok)
    {
#ifndef NDEBUG
      if (_data.size() > 0u) { BOOST_ASSERT(grainId < static_cast<int>(_data.size()));}
#endif
      SharedVectorType vec = _data[grainId];
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
      return static_cast<int>(_data.size());
    }

    /**
     * @brief getListSize
     * @param grainId
     * @return
     */
    int getListSize(int grainId)
    {
#ifndef NDEBUG
      if (_data.size() > 0u) { BOOST_ASSERT(grainId < static_cast<int>(_data.size()));}
#endif
      return static_cast<int>(_data[grainId]->size());
    }


    /**
     * @brief getList
     * @param grainId
     * @return
     */
    SharedVectorType getList(int grainId)
    {
#ifndef NDEBUG
      if (_data.size() > 0u) { BOOST_ASSERT(grainId < static_cast<int>(_data.size()));}
#endif
      return _data[grainId];
    }

    /**
     * @brief copyOfList
     * @param grainId
     * @return
     */
    VectorType copyOfList(int grainId)
    {
#ifndef NDEBUG
      if (_data.size() > 0u) { BOOST_ASSERT(grainId < static_cast<int>(_data.size()));}
#endif

      VectorType copy(*(_data[grainId]));
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
      if (_data.size() > 0u) { BOOST_ASSERT(grainId < static_cast<int>(_data.size()));}
#endif
      return *(_data[grainId]);
    }

    /**
     * @brief operator []
     * @param grainId
     * @return
     */
    VectorType& operator[](size_t grainId)
    {
#ifndef NDEBUG
      if (_data.size() > 0ul) { BOOST_ASSERT(grainId < _data.size());}
#endif
      return *(_data[grainId]);

    }


  protected:
    /**
     * @brief NeighborList
     */
    NeighborList() :
      m_Name("NeighborList")  {    }

  private:
    std::string m_Name;

    std::vector<SharedVectorType> _data;

    NeighborList(const NeighborList&); // Copy Constructor Not Implemented
    void operator=(const NeighborList&); // Operator '=' Not Implemented
};

#endif /* NEIGHBORLIST_H_ */
