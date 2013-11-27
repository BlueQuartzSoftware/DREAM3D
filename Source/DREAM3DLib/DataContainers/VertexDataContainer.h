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

#ifndef VERTEXDATACONTAINER_H_
#define VERTEXDATACONTAINER_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#endif

//-- C++ includes
#include <QtCore/QVector>
#include <QtCore/QMap>
#include <QtCore/QTextStream>
#include <QtCore/QList>

//-- Boost includes
#include <boost/shared_array.hpp>

//-- DREAM3D Includes
#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataContainers/DataContainer.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/DataArrays/StructArray.hpp"
#include "DREAM3DLib/DataContainers/VertexArray.h"

/**
 * @class VertexDataContainer VertexDataContainer.h DREAM3DLib/DataContainers/VertexDataContainer.h
 * @brief This data container holds data the represents a SurfaceMesh
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Sep 28, 2012
 * @version 1.0
 */
class DREAM3DLib_EXPORT VertexDataContainer : public DataContainer
{
  public:
    DREAM3D_SHARED_POINTERS (VertexDataContainer)
    DREAM3D_STATIC_NEW_MACRO (VertexDataContainer)
    DREAM3D_TYPE_MACRO_SUPER(VertexDataContainer, DataContainer)

    virtual ~VertexDataContainer();

    DREAM3D_INSTANCE_PROPERTY(VertexArray::Pointer, Vertices)

    virtual int writeMeshToHDF5(hid_t dcGid);
    virtual int writeVerticesToHDF5(hid_t dcGid);
    virtual int writeXdmf(QTextStream& out);

  protected:
     VertexDataContainer();

    virtual void writeXdmfMeshStructure();
    virtual void writeXdmfAttributeData(const QString& groupName, IDataArray::Pointer array, const QString& centering);
    virtual QString writeXdmfAttributeDataHelper(int numComp, const QString& attrType, const QString& groupName, IDataArray::Pointer array, const QString& centering, int precision, const QString& xdmfTypeName);

   private:

     VertexDataContainer(const VertexDataContainer&);
     void operator =(const VertexDataContainer&);

};

#endif /* VERTEXDATACONTAINER_H_ */
