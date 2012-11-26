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

#include "Patch.h"

using namespace meshing;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Patch::Patch()
{
  doFlip = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Patch::~Patch()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Patch::flipWinding()
{
  // Flip the order of the nodes - which possibly screws up the edge tables?
  int tmp = node_id[0];
  node_id[0] = node_id[2];
  node_id[2] = tmp;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int Patch::getintIndex(int label)
{
  if (label == nSpin[0]) return 0;
  if (label == nSpin[1]) return 1;
  return 2; // Error condition. Valid values are 0 or 1 since there are only 2 elements to the array.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<int> Patch::getNodeIndices(int label)
{
  std::vector<int> tNodes(3);
  int idx = getintIndex(label);
  if (idx == 1)
  {
    tNodes[0] = node_id[2];
    tNodes[1] = node_id[1];
    tNodes[2] = node_id[0];
  }
  else
  {
    tNodes[0] = node_id[0];
    tNodes[1] = node_id[1];
    tNodes[2] = node_id[2];
  }
  return tNodes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Patch::getWindingIndices(int ids[4], int label)
{
  int idx = getintIndex(label);

  if (idx == 1)
  {
    ids[0] = node_id[2];
    ids[1] = node_id[1];
    ids[2] = node_id[0];
    ids[3] = node_id[2];
  }
  else
  {
    ids[0] = node_id[0];
    ids[1] = node_id[1];
    ids[2] = node_id[2];
    ids[3] = node_id[0];
  }
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Patch::verifyWinding(Patch* tri, int label)
{
  int ids[4];
  int nids[4];
  getWindingIndices(ids, label);
  tri->getWindingIndices(nids, label);
  //    int idx = 0;
  // There are 2 node_id that are shared between the two triangles
  // Find them
  int i0, i1;
  //  bool flip = false;
  bool done = false;
  for (int i = 0; i < 3; ++i)
  {
    i0 = ids[i];
    i1 = ids[i + 1];
    for (int j = 0; j < 3; ++j)
    {
      if (i0 == nids[j + 1] && i1 == nids[j])
      {
        //    std::cout << ">>>>>> Winding OK "<< tIndex << " <-> "<< tri.tIndex << std::endl;
        done = true;
        break;
      }
      else if (i0 == nids[j] && i1 == nids[j + 1])
      {
//            std::cout << "!!!!!! Winding Bad " << tIndex << " <-> " << tri.tIndex << std::endl;
 //           std::cout << "  Grain ID: " << label << std::endl;
//            std::cout << "  Triangle ID: " << tIndex << std::endl;
//            std::cout << "  Neighbor ID: " << tri.tIndex << std::endl;
//            std::cout << "  Vert IDs: " << i0 << " & " << i1 << std::endl;
        done = true;
        if (doFlip == true) {
          tri->flipWinding();
        }
        break;
      }
    }
    if (done) break;
  }
}
