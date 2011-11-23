/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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

#ifndef DXGRAINIDWRITER_H_
#define DXGRAINIDWRITER_H_


#include <iostream>
#include <fstream>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AIMArray.hpp"
#include "DREAM3DLib/IO/FileWriter.h"


/*
 *
 */

class DxWriter : public DREAM3D::FileWriter
{
  public:
    DREAM3D_SHARED_POINTERS(DxWriter)
    DREAM3D_STATIC_NEW_MACRO(DxWriter)
    DREAM3D_TYPE_MACRO_SUPER(DxWriter, DREAM3D::FileWriter)

    static DREAM3D::FileWriter::Pointer NewDREAM3DFileWriter()
    {
      DxWriter* ptr = new DxWriter();
      DREAM3D::FileWriter::Pointer shared_ptr(dynamic_cast<DREAM3D::FileWriter*>(ptr));
      return shared_ptr;
    }

    virtual ~DxWriter() {}

    DREAM3D_INSTANCE_PROPERTY(bool, AddSurfaceLayer)
    DREAM3D_INSTANCE_PROPERTY(AIMArray<int>::Pointer, Data);


    virtual int writeFile()
    {
      int err = 0;
      int dims[3];
      getDimensions(dims);
      int totalPoints = dims[0] * dims[1] * dims[2];
      // std::cout << "Write Dx Grain File:  x, y, z: " << dims[0] << " " << dims[1] << " " << dims[2] << std::endl;

      std::ofstream out(getFileName().c_str(), std::ios_base::binary);
      if (out.is_open() == false)
      {
        return -1;
      }

      int fileXDim = dims[0];
      int fileYDim = dims[1];
      int fileZDim = dims[2];

      int posXDim = fileXDim + 1;
      int posYDim = fileYDim + 1;
      int posZDim = fileZDim + 1;

      if (m_AddSurfaceLayer)
      {
         fileXDim = dims[0] + 2;
         fileYDim = dims[1] + 2;
         fileZDim = dims[2] + 2;

         posXDim = fileXDim + 1;
         posYDim = fileYDim + 1;
         posZDim = fileZDim + 1;
      }
      totalPoints = fileXDim * fileYDim * fileZDim;
      //Write the header
      out << "object 1 class gridpositions counts " << posZDim << " " << posYDim << " " << posXDim << std::endl;
      out << "origin 0 0 0" << std::endl;
      out << "delta  1 0 0" << std::endl;
      out << "delta  0 1 0" << std::endl;
      out << "delta  0 0 1" << std::endl;
      out << std::endl;
      out << "object 2 class gridconnections counts " << posZDim << " " << posYDim << " " << posXDim << std::endl;
      out << std::endl;
      out << "object 3 class array type int rank 0 items " << totalPoints << " data follows" << std::endl;


      // Add a complete layer of surface voxels
      if (m_AddSurfaceLayer)
      {
        for(int i = 0; i < (fileXDim*fileYDim); ++i)
        {
           out << "-3 ";
           if (i%fileXDim == 0) { out << std::endl; }
        }
      }

      int index = 0;
      for (int z = 0; z < dims[2]; ++z)
      {
        // Add a leading surface Row for this plane if needed
        if (m_AddSurfaceLayer)
        {
          for(int i = 0; i < fileXDim; ++i) {
            out << "-4 ";
          }
          out << std::endl;
        }
        for (int y = 0; y < dims[1]; ++y)
        {
          // write leading surface voxel for this row
          if (m_AddSurfaceLayer)
          {
            out << "-5 ";
          }
          // Write the actual voxel data
          for (int x = 0; x < dims[0]; ++x)
          {
            if (m_Data->GetValue(index) == 0) {
                out << "0" << " ";
            } else {
            out << m_Data->GetValue(index) << " ";
            }
            ++index;
          }
          // write trailing surface voxel for this row
          if (m_AddSurfaceLayer)
          {
            out << "-6 ";
          }
          out << std::endl;
        }
        // Add a trailing surface Row for this plane if needed
        if (m_AddSurfaceLayer)
        {
          for(int i = 0; i < fileXDim; ++i) {
            out << "-7 ";
          }
          out << std::endl;
        }
      }

      // Add a complete layer of surface voxels
      if (m_AddSurfaceLayer)
      {
        for(int i = 0; i < (fileXDim*fileYDim); ++i)
        {
           out << "-8 ";
           if (i%fileXDim == 0) { out << std::endl; }
        }
      }
      out << std::endl;
      out << "attribute \"dep\" string \"connections\"" << std::endl;
      out << std::endl;
      out << "object \"DREAM3D Generated\" class field" << std::endl;
      out << "component  \"positions\"    value 1" << std::endl;
      out << "component  \"connections\"  value 2" << std::endl;
      out << "component  \"data\"         value 3" << std::endl;
      out << "" << std::endl;
      out << "end" << std::endl;


      out.close();
#if 0
      out.open("/tmp/m3cmesh.raw", std::ios_base::binary);
      out.write((const char*)(&dims[0]), 4);
      out.write((const char*)(&dims[1]), 4);
      out.write((const char*)(&dims[2]), 4);
      totalPoints = dims[0] * dims[1] * dims[2];
      int32_t d = 0;
      for(int index = 0; index < totalPoints; ++index)
      {
        d = grainIds[index];
        if (d == 0) { d = -3;}
        out.write((const char*)(&d), sizeof(d));
      }

      out.close();
#endif
      return err;
    }

  protected:
    DxWriter() : DREAM3D::FileWriter(), m_AddSurfaceLayer(false) {}

  private:
    DxWriter(const DxWriter&); // Copy Constructor Not Implemented
    void operator=(const DxWriter&); // Operator '=' Not Implemented
};

#endif /* DXGRAINIDWRITER_H_ */
