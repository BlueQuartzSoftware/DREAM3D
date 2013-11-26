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
#ifndef _EdgeDataContainerWriter_H_
#define _EdgeDataContainerWriter_H_

#include <sstream>
#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/IOFilters/util/VertexDataContainerWriter.h"

/**
 * @class EdgeDataContainerWriter EdgeDataContainerWriter.h DREAm3DLib/IOFilters/EdgeDataContainerWriter.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class DREAM3DLib_EXPORT EdgeDataContainerWriter : public VertexDataContainerWriter
{
  public:
    DREAM3D_SHARED_POINTERS(EdgeDataContainerWriter)
    DREAM3D_STATIC_NEW_MACRO(EdgeDataContainerWriter)
    DREAM3D_TYPE_MACRO_SUPER(EdgeDataContainerWriter, VertexDataContainerWriter)

    virtual ~EdgeDataContainerWriter();

    /* Place your input parameters here. You can use some of the DREAM3D Macros if you want to */


    typedef QList<QString> NameListType;



    /**
    * @brief Reimplemented from @see AbstractFilter class
    */
    virtual void execute();

    /**
    * @brief This function runs some sanity checks on the DataContainer and inputs
    * in an attempt to ensure the filter can process the inputs.
    */
    virtual void preflight();

  protected:
    EdgeDataContainerWriter();

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param voxels The number of voxels
    * @param features The number of features
    * @param ensembles The number of ensembles
    */
    void dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles);

    virtual void writeXdmfMeshStructure();
    virtual void writeXdmfAttributeData(const QString& groupName, IDataArray::Pointer array, const QString& centering);
    virtual QString writeXdmfAttributeDataHelper(int numComp, const QString& attrType, const QString& groupName, IDataArray::Pointer array, const QString& centering, int precision, const QString& xdmfTypeName);

  private:

    EdgeDataContainerWriter(const EdgeDataContainerWriter&); // Copy Constructor Not Implemented
    void operator=(const EdgeDataContainerWriter&); // Operator '=' Not Implemented
};

#endif /* _EdgeDataContainerWriter_H_ */
