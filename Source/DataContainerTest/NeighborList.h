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


#include <vector>

#include <boost/shared_ptr.hpp>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/IDataArray.h"

typedef std::vector<int>  IntVectorType;
typedef boost::shared_ptr< IntVectorType >  SharedVectorType;


class NeighborList : public IDataArray
{
  public:
    DREAM3D_SHARED_POINTERS(NeighborList);
    DREAM3D_STATIC_NEW_MACRO(NeighborList);
    DREAM3D_TYPE_MACRO_SUPER(NeighborList, IDataArray);


    virtual ~NeighborList() {}

    /* ******* IDataArray Interface. These Must be implemented ***** */

    virtual void SetName(const std::string &name) { m_Name = name;}
    virtual std::string GetName() { return m_Name;}

    /* These methods are empty as there is no real way to get at the data */
    void takeOwnership() {}
    void releaseOwnership() {}

    void* GetVoidPointer(size_t i)
    {
      return NULL;
    }

    size_t GetNumberOfTuples()
    {
      return _data.size();
    }

    virtual void SetNumberOfComponents(int nc) {}
    virtual int GetNumberOfComponents(){return 1;}

    virtual size_t getTypeSize() { return sizeof(SharedVectorType);}

    virtual void initializeWithZeros()
    {
      _data.clear();
    }

    int32_t Resize(size_t size)
    {
      return 0;
    }

    void addEntry(int grainId, int value)
    {
      if (_data.find(grainId) == _data.end())
      {
        _data[grainId] = SharedVectorType(new IntVectorType);
      }
      SharedVectorType sharedVector = _data[grainId];
      _data[grainId]->push_back(value);
    }

    int getValue(int grainId, int index, bool &ok)
    {
      if (_data.find(grainId) == _data.end())
      {
        ok = false;
        return -1;
      }
      SharedVectorType vec = _data[grainId];
      if (index < 0 || static_cast<size_t>(index) >= vec->size())
      {
        ok = false;
        return -1;
      }
      return  (*vec)[index];
    }

    int getListCount() { return _data.size(); }

    int getEntryCount(int grainId)
    {
      if (_data.find(grainId) == _data.end())
      {
        return 0;
      }
      return _data[grainId]->size();
    }

    SharedVectorType pointerToList(int grainId)
    {
      if (_data.find(grainId) == _data.end())
       {
         SharedVectorType copy;
         return copy;
       }
      return _data[grainId];
    }

    IntVectorType copyOfList(int grainId)
    {

      if (_data.find(grainId) == _data.end())
      {
        IntVectorType copy;
        return copy;
      }
      IntVectorType copy(*(_data[grainId]));
      return copy;
    }

  protected:
    NeighborList() {}


  private:
    std::string m_Name;

    std::map<int, SharedVectorType>  _data;


    NeighborList(const NeighborList&); // Copy Constructor Not Implemented
    void operator=(const NeighborList&); // Operator '=' Not Implemented
};



#endif /* NEIGHBORLIST_H_ */
