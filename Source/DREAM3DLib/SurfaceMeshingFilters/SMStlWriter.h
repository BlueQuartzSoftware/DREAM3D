/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011, Dr. Michael A. Groeber (US Air Force Research Laboratories
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

#ifndef STLWRITER_H_
#define STLWRITER_H_

#include <stdio.h>

#include "MXA/Utilities/StringUtils.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/SurfaceMeshingFilters/Meshing/Patch.h"
#include "DREAM3DLib/SurfaceMeshingFilters/Meshing/Node.h"

class SurfaceMeshFunc;


namespace meshing {

class DREAM3DLib_EXPORT SMStlWriter
{
  public:
    DREAM3D_SHARED_POINTERS(SMStlWriter)
    DREAM3D_STATIC_NEW_MACRO(SMStlWriter)
    DREAM3D_TYPE_MACRO(SMStlWriter)

    static Pointer CreateNewSTLWriter(int gid, const std::string &filename)
    {
      Pointer stlWriter = SMStlWriter::New();
      stlWriter->setFileName(filename);
      stlWriter->setGrainID(gid);
      int err = stlWriter->openFile("wb");
      if (err < 0)
      {
        return NullPointer();
      }
      std::string stlHeader("DREAM.3D Surface Mesh for Grain ID ");
      stlHeader.append(StringUtils::numToString(gid));
      stlWriter->writeHeader(stlHeader);
      stlWriter->closeFile();
      return stlWriter;
    }
    virtual ~SMStlWriter();

    DREAM3D_INSTANCE_STRING_PROPERTY(FileName)
    DREAM3D_INSTANCE_PROPERTY(int, TriangleCount)
    DREAM3D_INSTANCE_PROPERTY(int, GrainID)

    int openFile(const char* mode);
    void closeFile();
    void resetTriangleCount();

    int writeHeader(const std::string &header);

    int writeTriangleBlock(int numTriangles, const std::vector<Patch::Pointer>& cTriangle, Node* cVertex);

    int writeNumTrianglesToFile();

  protected:
    SMStlWriter();

  private:
    FILE* m_File;

    SMStlWriter(const SMStlWriter&); // Copy Constructor Not Implemented
    void operator=(const SMStlWriter&); // Operator '=' Not Implemented
};
 }
#endif /* STLWRITER_H_ */
