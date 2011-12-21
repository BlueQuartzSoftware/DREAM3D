/*
 * NeighborList.h
 *
 *  Created on: Dec 20, 2011
 *      Author: mjackson
 */

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
      //TODO: Implement the resizing feature
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

    std::map<int, SharedVectorType >  _data;


    NeighborList(const NeighborList&); // Copy Constructor Not Implemented
    void operator=(const NeighborList&); // Operator '=' Not Implemented
};



#endif /* NEIGHBORLIST_H_ */
