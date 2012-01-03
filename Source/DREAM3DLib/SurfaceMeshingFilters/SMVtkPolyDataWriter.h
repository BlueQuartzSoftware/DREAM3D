/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#ifndef _SM_VTK_FILEWRITER_h_
#define _SM_VTK_FILEWRITER_h_


#include <string>
#include <map>
#include <fstream>



#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"



#define kBufferSize 1024



namespace meshing {
/**
 * @class VTKFileUtils VTKFileUtils.h application/VTKFileUtils.h
 * @brief This class contains some useful functions to read from and write to
 * VTK legacy style files.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Aug 13, 2010
 * @version 1.0
 */
class DREAM3DLib_EXPORT SMVtkPolyDataWriter : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(SMVtkPolyDataWriter)
    DREAM3D_TYPE_MACRO(SMVtkPolyDataWriter)
    DREAM3D_STATIC_NEW_MACRO(SMVtkPolyDataWriter)

    virtual ~SMVtkPolyDataWriter();

    DREAM3D_INSTANCE_STRING_PROPERTY(VisualizationFile);
    DREAM3D_INSTANCE_STRING_PROPERTY(NodesFile);
    DREAM3D_INSTANCE_STRING_PROPERTY(TrianglesFile);
    DREAM3D_INSTANCE_PROPERTY(bool, WriteBinaryFile);
    DREAM3D_INSTANCE_PROPERTY(bool, WriteConformalMesh);

    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::SurfaceMeshingFilters; }
    virtual const std::string getHumanLabel() { return "Write VTK PolyData File"; }


#if 0
    /**
     * @brief
     * @param m
     * @param file
     * @param slice
     * @return
     */
    int primeFileToScalarDataLocation(DataContainer* m, const std::string &file, const std::string &scalarName);


    int readZSlice(int xDim, int yDim, int zDim, int* voxels);
#endif


    virtual void execute();

/**
 * @brief Writes a VTK POLYDATA legacy ASCII file
 * @param m
 * @param nNodes
 * @param nTriangles
 * @param VisualizationFile
 * @param NodesFile
 * @param TrianglesFile
 * @param binaryFile
 * @return
 */
#if 0
    int writeVTKFile(const std::string &VisualizationFile,
                     const std::string &NodesFile,
                     const std::string &TrianglesFile,
                     bool binaryFile,
                     bool conformalMesh);
#endif


  protected:
    SMVtkPolyDataWriter();


    int writeBinaryCellData(const std::string &TrianglesFile, FILE* vtkFile, int nTriangles, bool conformalMesh);
    int writeASCIICellData(const std::string &TrianglesFile, FILE* vtkFile, int nTriangles, bool conformalMesh);

    int writeBinaryPointData(const std::string &NodesFile, FILE* vtkFile, int nNodes, bool conformalMesh);
    int writeASCIIPointData(const std::string &NodesFile, FILE* vtkFile, int nNodes, bool conformalMesh);

  private:
    SMVtkPolyDataWriter(const SMVtkPolyDataWriter&); // Copy Constructor Not Implemented
    void operator=(const SMVtkPolyDataWriter&); // Operator '=' Not Implemented

};
}

#endif /* _VTKFileUtils.h_  */
