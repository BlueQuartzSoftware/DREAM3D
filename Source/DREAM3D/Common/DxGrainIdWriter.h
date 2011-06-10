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

#ifndef DXGRAINIDWRITER_H_
#define DXGRAINIDWRITER_H_


#include <iostream>
#include <fstream>

#include "MXA/Common/MXASetGetMacros.h"

#include "DREAM3D/DREAM3DConfiguration.h"

/*
 *
 */
class DxGrainIdWriter
{
  public:
    MXA_SHARED_POINTERS(DxGrainIdWriter)
    MXA_STATIC_NEW_MACRO(DxGrainIdWriter)
    MXA_TYPE_MACRO(DxGrainIdWriter)

    DxGrainIdWriter();
    virtual ~DxGrainIdWriter();

    MXA_INSTANCE_PROPERTY(bool, AddSurfaceLayer)

    template<typename T>
    int writeGrainFile(const std::string &filename, T* voxels, int xDim, int yDim, int zDim)
    {
      int err = 0;
      int totalPoints = (xDim*yDim*zDim);
      // std::cout << "Write Dx Grain File:  x, y, z: " << xDim << " " << yDim << " " << zDim << std::endl;

      std::ofstream out(filename.c_str(), std::ios_base::binary);
      if (out.is_open() == false)
      {
        return -1;
      }

      int fileXDim = xDim;
      int fileYDim = yDim;
      int fileZDim = zDim;

      int posXDim = fileXDim + 1;
      int posYDim = fileYDim + 1;
      int posZDim = fileZDim + 1;

      if (m_AddSurfaceLayer)
      {
         fileXDim = xDim + 2;
         fileYDim = yDim + 2;
         fileZDim = zDim + 2;

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
      for (int z = 0; z < zDim; ++z)
      {
        // Add a leading surface Row for this plane if needed
        if (m_AddSurfaceLayer)
        {
          for(int i = 0; i < fileXDim; ++i) {
            out << "-4 ";
          }
          out << std::endl;
        }
        for (int y = 0; y < yDim; ++y)
        {
          // write leading surface voxel for this row
          if (m_AddSurfaceLayer)
          {
            out << "-5 ";
          }
          // Write the actual voxel data
          for (int x = 0; x < xDim; ++x)
          {
            if (voxels[index].grain_index == 0) {
                out << "0" << " ";
            } else {
            out << voxels[index].grain_index << " ";
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
#if (__APPLE__)
      out.open("/tmp/m3cmesh.raw", std::ios_base::binary);
      out.write((const char*)(&xDim), 4);
      out.write((const char*)(&yDim), 4);
      out.write((const char*)(&zDim), 4);
      totalPoints = xDim * yDim * zDim;
      int32_t d = 0;
      for(int index = 0; index < totalPoints; ++index)
      {
        d = voxels[index].grain_index;
        if (d == 0) { d = -3;}
        out.write((const char*)(&d), sizeof(d));
      }

      out.close();
#endif
      return err;
    }

  private:
    DxGrainIdWriter(const DxGrainIdWriter&); // Copy Constructor Not Implemented
    void operator=(const DxGrainIdWriter&); // Operator '=' Not Implemented
};

#endif /* DXGRAINIDWRITER_H_ */
