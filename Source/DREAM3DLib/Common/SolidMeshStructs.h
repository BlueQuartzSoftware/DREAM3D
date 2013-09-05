/* ============================================================================
* Copyright (c) 2012 Dr. Sukbin Lee (Carnegie-Mellon University)
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
* Neither the name of Anthony Rollet, Sukbin Lee, Jason Gruber, Stephen Sintay
* nor the names of its contributors may be used to endorse
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
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _Solid_MESH_STRUCTS_H_
#define _Solid_MESH_STRUCTS_H_


#include <QtCore/QString>

#define num_neigh 26

typedef struct {
  int node_id[4];    // stores three new node id for vertices of the triangles...
  int e_id[6];       // stores three new edge id for sides of the triangles...
  int nSpin;
  int tIndex;
} Tetrahedron;


namespace SolidMesh
{
  namespace NodesFile
  {
    typedef struct
    {
        int nodeId;
        int nodeKind;
        float x;
        float y;
        float z;
    } NodesFileRecord_t;
    const int ByteCount = sizeof(NodesFileRecord_t);
  }

  namespace TetrahedronsFile
  {
    typedef struct
    {
      int tetId;
      int nodeId_0;
      int nodeId_1;
      int nodeId_2;
      int nodeId_3;
      int label_0;
    } TetrahedronsFileRecord_t;
    const int ByteCount = sizeof(TetrahedronsFileRecord_t);
  }
}

#endif /* _Solid_MESH_STRUCTS_H_ */
