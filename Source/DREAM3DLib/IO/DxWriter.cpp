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

#include "DxWriter.h"

#include <iostream>
#include <fstream>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DxWriter::DxWriter() :
    DREAM3D::FileWriter(),
    m_AddSurfaceLayer(false)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DxWriter::~DxWriter()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DxWriter::writeHeader()
{
  return 0;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DxWriter::writeFile()
{
  DataContainer* m = getDataContainer();
  if (NULL == m)
  {
    std::stringstream ss;
    ss << "DataContainer Pointer was NULL and Must be valid." << __FILE__ << "("<<__LINE__<<")";
    setErrorMessage(ss.str());
    setErrorCondition(-1);
    return -1;
  }
  int64_t totalPoints = m->totalPoints();
  GET_NAMED_ARRAY_SIZE_CHK_NOMSG_RET(m, Voxel, DREAM3D::VoxelData::GrainIds, Int32ArrayType, int32_t, totalPoints, grain_indicies);

  int err = 0;
  size_t udims[3] = {0,0,0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] = {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[0]),
  };
  // std::cout << "Write Dx Grain File:  x, y, z: " << dims[0] << " " << dims[1] << " " << dims[2] << std::endl;

  std::ofstream out(getFileName().c_str(), std::ios_base::binary);
  if(out.is_open() == false)
  {
    return -1;
  }

  DimType fileXDim = dims[0];
  DimType fileYDim = dims[1];
  DimType fileZDim = dims[2];

  DimType posXDim = fileXDim + 1;
  DimType posYDim = fileYDim + 1;
  DimType posZDim = fileZDim + 1;

  if(m_AddSurfaceLayer)
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
  if(m_AddSurfaceLayer)
  {
    for (int i = 0; i < (fileXDim * fileYDim); ++i)
    {
      out << "-3 ";
      if(i % fileXDim == 0)
      {
        out << std::endl;
      }
    }
  }

  int index = 0;
  for (size_t z = 0; z < dims[2]; ++z)
  {
    // Add a leading surface Row for this plane if needed
    if(m_AddSurfaceLayer)
    {
      for (int i = 0; i < fileXDim; ++i)
      {
        out << "-4 ";
      }
      out << std::endl;
    }
    for (size_t y = 0; y < dims[1]; ++y)
    {
      // write leading surface voxel for this row
      if(m_AddSurfaceLayer)
      {
        out << "-5 ";
      }
      // Write the actual voxel data
      for (size_t x = 0; x < dims[0]; ++x)
      {
        if(grain_indicies[index] == 0)
        {
          out << "0" << " ";
        }
        else
        {
          out << grain_indicies[index] << " ";
        }
        ++index;
      }
      // write trailing surface voxel for this row
      if(m_AddSurfaceLayer)
      {
        out << "-6 ";
      }
      out << std::endl;
    }
    // Add a trailing surface Row for this plane if needed
    if(m_AddSurfaceLayer)
    {
      for (int i = 0; i < fileXDim; ++i)
      {
        out << "-7 ";
      }
      out << std::endl;
    }
  }

  // Add a complete layer of surface voxels
  if(m_AddSurfaceLayer)
  {
    for (int i = 0; i < (fileXDim * fileYDim); ++i)
    {
      out << "-8 ";
      if(i % fileXDim == 0)
      {
        out << std::endl;
      }
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
    if (d == 0)
    { d = -3;}
    out.write((const char*)(&d), sizeof(d));
  }

  out.close();
#endif
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------


