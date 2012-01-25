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
    DREAM3D_SHARED_POINTERS(NeighborList<T> );
    DREAM3D_STATIC_NEW_MACRO(NeighborList<T> );
    DREAM3D_TYPE_MACRO_SUPER(NeighborList<T>, IDataArray);


    typedef std::vector<T> VectorType;
    typedef boost::shared_ptr<VectorType> SharedVectorType;

    virtual ~NeighborList() {}

    void SetName(const std::string &name) { m_Name = name; }


    std::string GetName() { return m_Name; }


    void takeOwnership() {    }


    void releaseOwnership()  { }


    void* GetVoidPointer(size_t i) { return NULL; }

    virtual int EraseTuples(std::vector<size_t> &idxs)
    {
      int err = 0;
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
      return err;
    }

    virtual int CopyTuple(size_t currentPos, size_t newPos)
    {
      _data[newPos] = _data[currentPos];
      return 0;
    }

    size_t GetNumberOfTuples() {   return _data.size(); }
    size_t GetSize() { return _data.size(); }


    void SetNumberOfComponents(int nc) { }


    int GetNumberOfComponents() { return 1; }


    size_t GetTypeSize()  { return sizeof(SharedVectorType); }


    void initializeWithZeros() { _data.clear(); }


    int32_t RawResize(size_t size)  { return 0; }

    virtual int32_t Resize(size_t numTuples) { return 0; }


    //FIXME: These need to be implemented
    virtual void printTuple(std::ostream &out, size_t i, char delimiter = ',')
    {
        assert(false);
    }
    virtual void printComponent(std::ostream &out, size_t i, int j)
    {
      assert(false);
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
      std::vector<size_t> numNeighbors(_data.size());
      size_t total = 0;
      for(size_t dIdx = 0; dIdx < _data.size(); ++dIdx)
      {
        numNeighbors[dIdx] = _data[dIdx]->size();
        total += _data[dIdx]->size();
      }

      //FIXME: Check to see if the NumNeighbors is already written to the file
      //FIXME: and if it is then validate that we have the correct values


      // Allocate an array of the proper size
      std::vector<T> flat (total);
      size_t currentStart = 0;
      for(size_t dIdx = 0; dIdx < _data.size(); ++dIdx)
      {
        size_t nEle = _data[dIdx]->size();
        T* start = &(_data[dIdx]->front()); // Get the pointer to the front of the array
    //    T* end = start + nEle; // Get the pointer to the end of the array
        T* dst = &(flat.front()) + currentStart;
        ::memcpy(dst, start, nEle*sizeof(T));

        currentStart += _data[dIdx]->size();
      }

      int32_t rank = 0;
      hsize_t dims[1] = { total };
      err = H5Lite::writePointerDataset(parentId, GetName(), rank, dims, &(flat.front()));
      if(err < 0)
      {
        //FIXME: Add Error Handling Code
      }
      err = H5Lite::writeScalarAttribute(parentId, GetName(), std::string(H5_NUMCOMPONENTS), 1);
      if(err < 0)
      {
        //FIXME: Add Error Handling Code
      }

      err = H5Lite::writeStringAttribute(parentId, GetName(), DREAM3D::HDF5::ObjectType, getNameOfClass());
      if(err < 0)
      {
        //FIXME: Add Error Handling Code
      }

      return err;
    }

    virtual int readH5Data(hid_t parentId)
    {
      assert(false);
      return -1;
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
    void removeList(int grainId)
    {
      typename std::vector<SharedVectorType>::iterator it = _data.begin();
      it = it + grainId;
      _data.erase(it);
    }


    /**
     *
     */
    void setList(int grainId, SharedVectorType neighborList)
    {
      _data[grainId] = neighborList;
    }

    /**
     *
     */
    T getValue(int grainId, int index, bool &ok)
    {

      SharedVectorType vec = _data[grainId];
      if(index < 0 || static_cast<size_t>(index) >= vec->size())
      {
        ok = false;
        return -1;
      }
      return (*vec)[index];
    }

    /**
     *
     */
    int getNumberOfLists()
    {
      return static_cast<int>(_data.size());
    }

    /**
     *
     */
    int getListSize(int grainId)
    {
#ifndef NDEBUG
      if (_data.size() > 0u) { assert(grainId < static_cast<int>(_data.size()));}
#endif
      return static_cast<int>(_data[grainId]->size());
    }


    /**
     *
     */
    SharedVectorType pointerToList(int grainId)
    {
#ifndef NDEBUG
      if (_data.size() > 0u) { assert(grainId < static_cast<int>(_data.size()));}
#endif
      return _data[grainId];
    }

    /**
     *
     */
    VectorType copyOfList(int grainId)
    {
#ifndef NDEBUG
      if (_data.size() > 0u) { assert(grainId < static_cast<int>(_data.size()));}
#endif

      VectorType copy(*(_data[grainId]));
      return copy;
    }

    VectorType& operator[](int grainId)
    {
      return *(_data[grainId]);
    }

  protected:
    NeighborList() :
        m_Name("NeighborList")  {    }

  private:
    std::string m_Name;

    std::vector<SharedVectorType> _data;

    NeighborList(const NeighborList&); // Copy Constructor Not Implemented
    void operator=(const NeighborList&); // Operator '=' Not Implemented
};

#endif /* NEIGHBORLIST_H_ */
