/* ============================================================================
 * Copyright (c)  2010 Dr.Sukbin Lee slee@andrew.cmu.edu
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
 * Neither the name of Dr.Sukbin Lee, Carnegie-Mellon University nor the names of
 * its contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

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

#ifndef SURFACEMESH_H_
#define SURFACEMESH_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#endif



#include <MXA/Common/MXASetGetMacros.h>
#include "MXA/MXA.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/AbstractPipeline.h"
#include "DREAM3DLib/Common/Observer.h"


#include "SurfaceMesh/SurfaceMeshFunc.h"
#include "DREAM3DLib/SurfaceMeshingFilters/SMStlWriter.h"

namespace meshing {

class GrainChecker
{
  public:
    MXA_SHARED_POINTERS(GrainChecker);
    MXA_STATIC_NEW_MACRO(GrainChecker);
    virtual ~GrainChecker(){}
    typedef std::map<int, int>  MapType;

    template<typename T>
    static void TripletSort(T a, T b, T c, T* sorted)
    {
      if ( a > b && a > c)
      {
        sorted[2] = a;
        if (b > c) { sorted[1] = b; sorted[0] = c; }
        else { sorted[1] = c; sorted[0] = b; }
      }
      else if ( b > a && b > c)
      {
        sorted[2] = b;
        if (a > c) { sorted[1] = a; sorted[0] = c; }
        else { sorted[1] = c; sorted[0] = a; }
      }
      else if ( a > b )
      {
        sorted[1] = a; sorted[0] = b; sorted[2] = c;
      }
      else if (a >= c && b >=c)
      {
        sorted[0] = c; sorted[1] = a; sorted[2] = b;
      }
      else
      { sorted[0] = a; sorted[1] = b; sorted[2] = c;}
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
    void addData(int numTriangles, int ctid, const std::vector<Patch::Pointer>& cTriangle, Node* cVertex)
    {

      int n1, n2, n3;
      int gid;
      int size;
      for (int i = 0; i < numTriangles; ++i)
      {
        gid = cTriangle[i]->ngrainname[0];
        size = static_cast<int>(grainMaps.size());
        if (gid >= size)
        {
          grainMaps.resize(gid + 1);
          tCounts.resize(gid + 1);
        }
        if (gid > 0)
        {
          // Get a reference to the map for this grain
          MapType& vertMap = grainMaps[gid];
          MapType& triMap = tCounts[gid];

          n1 = cTriangle[i]->node_id[0];
          n2 = cTriangle[i]->node_id[1];
          n3 = cTriangle[i]->node_id[2];
          n1 = cVertex[n1].NodeID;
          n2 = cVertex[n2].NodeID;
          n3 = cVertex[n3].NodeID;

          vertMap[n1] = 0;
          vertMap[n2] = 0;
          vertMap[n3] = 0;
          triMap[ctid] = 0;
        }

        gid = cTriangle[i]->ngrainname[1];
        size = static_cast<int>(grainMaps.size());
        if (gid >= size)
        {
          grainMaps.resize(gid + 1);
          tCounts.resize(gid + 1);
        }
        if (gid > 0)
        { // Get a reference to the map for this grain
          MapType& vertMap = grainMaps[gid];
          MapType& triMap = tCounts[gid];

          n1 = cTriangle[i]->node_id[0];
          n2 = cTriangle[i]->node_id[1];
          n3 = cTriangle[i]->node_id[2];
          n1 = cVertex[n1].NodeID;
          n2 = cVertex[n2].NodeID;
          n3 = cVertex[n3].NodeID;

          vertMap[n1] = 0;
          vertMap[n2] = 0;
          vertMap[n3] = 0;
          triMap[ctid] = 0;
        }
        //
        ++ctid;
      }

    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
    void analyzeGrains()
    {
      size_t size = tCounts.size();
      size_t vCount = 0;
      size_t tCount = 0;
      for (size_t i = 1; i < size; ++i)
      {
        MapType& t = tCounts[i];
        tCount = t.size();
        MapType& m = grainMaps[i];
        vCount = m.size();
        if (tCount + 4 != vCount * 2)
        {
          std::cout << "Grain ID: " << i << " Does not satisfy equation T=2V-4    " << "  tCount: " << tCount << "   " << "  vCount: " << vCount << std::endl;
        }
      }
    }

  protected:
    GrainChecker() {}

  private:
    std::vector<MapType> grainMaps;
    std::vector<MapType> tCounts;

    GrainChecker(const GrainChecker&); // Copy Constructor Not Implemented
    void operator=(const GrainChecker&); // Operator '=' Not Implemented
};
}




/**
* @class SurfaceMesh SurfaceMesh.h AIM/SurfaceMesh/SurfaceMesh.h
* @brief This class serves as the main entry point to execute the Surface Mesh
* codes.
* @author Michael A. Jackson for BlueQuartz Software,
* @author Dr. Michael Groeber, USAFRL
* @date Nov 3, 2009
* @version 1.0
*/
class SurfaceMesh : public AbstractPipeline, public Observer
{
  public:
    MXA_SHARED_POINTERS(SurfaceMesh);
    MXA_TYPE_MACRO(SurfaceMesh);
    MXA_STATIC_NEW_MACRO(SurfaceMesh);

    virtual ~SurfaceMesh();

    MXA_INSTANCE_STRING_PROPERTY(InputDirectory)
    MXA_INSTANCE_STRING_PROPERTY(InputFile)
    MXA_INSTANCE_STRING_PROPERTY(ScalarName)

    MXA_INSTANCE_PROPERTY(int, XDim)
    MXA_INSTANCE_PROPERTY(int, YDim)
    MXA_INSTANCE_PROPERTY(int, ZDim)
    MXA_INSTANCE_PROPERTY(double, XRes)
    MXA_INSTANCE_PROPERTY(double, YRes)
    MXA_INSTANCE_PROPERTY(double, ZRes)

    MXA_INSTANCE_STRING_PROPERTY(OutputDirectory)
    MXA_INSTANCE_STRING_PROPERTY(OutputFilePrefix)
    MXA_INSTANCE_PROPERTY(bool, WriteConformalMesh)
    MXA_INSTANCE_PROPERTY(bool, WriteBinaryVTKFiles)
    MXA_INSTANCE_PROPERTY(bool, WriteSTLFile)

    MXA_INSTANCE_PROPERTY(bool, DeleteTempFiles)

    MXA_INSTANCE_PROPERTY(bool, SmoothMesh)
    MXA_INSTANCE_PROPERTY(int, SmoothIterations)
    MXA_INSTANCE_PROPERTY(int, SmoothFileOutputIncrement)
    MXA_INSTANCE_PROPERTY(bool, SmoothLockQuadPoints)


    /**
     * @brief Main method to run the operation
     */
    void execute();

    int writeSTLFiles(int nTriangle, std::map<int, SMStlWriter::Pointer> &gidToSTLWriter );

  protected:

    SurfaceMesh();

  private:
	  SurfaceMeshFunc::Pointer m;
	  GrainChecker::Pointer        m_GrainChecker;

    SurfaceMesh(const SurfaceMesh&);    // Copy Constructor Not Implemented
    void operator=(const SurfaceMesh&);  // Operator '=' Not Implemented
};


#endif /* SURFACEMESH_H_ */
