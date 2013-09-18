/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef DATACONTAINERARRAY_H_
#define DATACONTAINERARRAY_H_


#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QTextStream>


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/Observer.h"
#include "DREAM3DLib/DataContainers/DataContainer.h"
#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"
#include "DREAM3DLib/DataContainers/SurfaceDataContainer.h"
#include "DREAM3DLib/DataContainers/VertexDataContainer.h"
#include "DREAM3DLib/DataContainers/EdgeDataContainer.h"

/**
 * @class DataContainerArray DataContainerArray.h DREAM3DLib/Common/DataContainerArray.h
 * @brief  This class holds the list of filters that will be run. This should be
 * the class (or a subclass) that you use to build up a pipeline in order to run
 * it.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Sep 28, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT DataContainerArray : public Observable
{
  public:
    DREAM3D_SHARED_POINTERS(DataContainerArray)
    DREAM3D_TYPE_MACRO_SUPER(DataContainerArray, Observable)
    DREAM3D_STATIC_NEW_MACRO(DataContainerArray)

    virtual ~DataContainerArray();

    /**
     * @brief
     */
    virtual void pushFront(DataContainer::Pointer f);
    virtual void popFront();
    virtual void pushBack(DataContainer::Pointer f);
    virtual void popBack();
    virtual void insert(size_t index, DataContainer::Pointer f);
    virtual void erase(size_t index);
    virtual void clear();
    virtual size_t size();
    virtual bool empty();

    //virtual FilterContainerType& getFilterContainer();

    /**
     * @brief
     */
    virtual DataContainer::Pointer removeDataContainer(const QString &name);
    virtual DataContainer::Pointer getDataContainer(const QString &name);
    QList<DataContainer::Pointer>& getDataContainerArray();

    virtual void printDataContainerNames(QTextStream &out);

    template<typename T>
    T* getDataContainerAs(const QString &name)
    {
      DataContainer::Pointer dc = getDataContainer(name);
      if(NULL == dc.get()) { return NULL; }

      T* m = T::SafePointerDownCast(dc.get());
      return m;
    }


  protected:
    DataContainerArray();


  private:
    QList<DataContainer::Pointer>  m_Array;

    DataContainerArray(const DataContainerArray&); // Copy Constructor Not Implemented
    void operator=(const DataContainerArray&); // Operator '=' Not Implemented
};

#endif /* DATACONTAINERARRAY_H_ */
