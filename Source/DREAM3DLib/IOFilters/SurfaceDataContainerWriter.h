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
#ifndef _SurfaceDataContainerWriter_H_
#define _SurfaceDataContainerWriter_H_

#include <sstream>
#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/IDataArray.h"
#include "DREAM3DLib/Common/AbstractFilter.h"


/**
 * @class SurfaceDataContainerWriter SurfaceDataContainerWriter.h DREAm3DLib/IOFilters/SurfaceDataContainerWriter.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class DREAM3DLib_EXPORT SurfaceDataContainerWriter : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(SurfaceDataContainerWriter)
    DREAM3D_STATIC_NEW_MACRO(SurfaceDataContainerWriter)
    DREAM3D_TYPE_MACRO_SUPER(SurfaceDataContainerWriter, AbstractFilter)

    virtual ~SurfaceDataContainerWriter();

    /* Place your input parameters here. You can use some of the DREAM3D Macros if you want to */
    DREAM3D_INSTANCE_PROPERTY(hid_t, HdfFileId)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteXdmfFile)

    typedef QList<QString> NameListType;

    void setXdmfOStream(std::ostream* xdmf);


    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    virtual const QString getGroupName() { return DREAM3D::FilterGroups::IOFilters; }
    virtual const QString getSubGroupName() { return DREAM3D::FilterSubGroups::OutputFilters; }

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual const QString getHumanLabel() { return "SurfaceMesh DataContainer Writer"; }

    /**
    * @brief This method will instantiate all the end user settable options/parameters
    * for this filter
    */
    virtual void setupFilterParameters();

    /**
    * @brief This method will write the options to a file
    * @param writer The writer that is used to write the options to a file
    */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);
    
    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

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
    SurfaceDataContainerWriter();

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param voxels The number of voxels
    * @param fields The number of fields
    * @param ensembles The number of ensembles
    */
    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

    int createVtkObjectGroup(const QString &hdfGroupPath, const char* vtkDataObjectType);

    int writeVertices(hid_t dcGid);
    int writeFaces(hid_t dcGid);
    int writeEdges(hid_t dcGid);
    int writeMeshLinks(hid_t dcGid);
    int writeMeshFaceNeighborLists(hid_t dcGid);
    int writeVertexAttributeData(hid_t dcGid);
    int writeFaceAttributeData(hid_t dcGid);
    int writeEdgeAttributeData(hid_t dcGid);
    int writeFieldData(hid_t dcGid);
    int writeEnsembleData(hid_t dcGid);

    void writeXdmfGridHeader();
    void writeXdmfGridFooter();
    void writeXdmfAttributeData(const QString &groupName, IDataArray::Pointer array, const QString &centering);
    QString writeXdmfAttributeDataHelper(int numComp, const QString &attrType, const QString &groupName, IDataArray::Pointer array, const QString &centering, int precision, const QString &xdmfTypeName);

  private:
    std::ostream* m_XdmfPtr;

    SurfaceDataContainerWriter(const SurfaceDataContainerWriter&); // Copy Constructor Not Implemented
    void operator=(const SurfaceDataContainerWriter&); // Operator '=' Not Implemented
};

#endif /* _SurfaceDataContainerWriter_H_ */
