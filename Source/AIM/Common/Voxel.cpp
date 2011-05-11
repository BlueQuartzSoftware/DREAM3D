/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
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

#include "Voxel.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Voxel::Voxel() :
grainname(0),
confidence(0.0),
imagequality(0.0),
imagequality2(0.0),
ellipfunc(0.0),
alreadychecked(0),
euler1(-1.0),
euler2(-1.0),
euler3(-1.0),
phase(0),
neighbor(-1),
numowners(0),
grainmisorientation(0.0),
misorientationgradient(0.0),
kernelmisorientation(0.0),
surfacevoxel(0),
unassigned(0)
{
  nearestneighbor[0] = 0; nearestneighbor[1] = 0; nearestneighbor[2] = 0;
  nearestneighbordistance[0] = 0.0; nearestneighbordistance[1] = 0.0; nearestneighbordistance[2] = 0.0;
  quat[0] = 1.0; quat[1] = 0.0; quat[2] = 0.0; quat[3] = 0.0; quat[4] = 0.0;
  grainlist = IntVectorType(new std::vector<int>(0) );
  ellipfunclist = DoubleVectorType(new std::vector<double>(0) );
  neighborlist = IntVectorType(new std::vector<int>(0) );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Voxel::~Voxel()
{
#if 0
  if (NULL != grainlist) { delete grainlist; grainlist = NULL;}
  if (NULL != ellipfunclist) { delete ellipfunclist; ellipfunclist = NULL;}
  if (NULL != neighborlist) { delete neighborlist; neighborlist = NULL;}
#endif
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Voxel::deepCopy(Voxel* voxel)
{
  if (voxel == this) { return; } // The pointers are the same just return

   grainname = voxel->grainname;
   confidence = voxel->confidence;
   imagequality = voxel->imagequality;
   imagequality2 = voxel->imagequality2;
   ellipfunc = voxel->ellipfunc;
   alreadychecked = voxel->alreadychecked;
   COPY_ARRAY_3(nearestneighbor, voxel);

   COPY_ARRAY_3(nearestneighbordistance, voxel);
   euler1 = voxel->euler1;
   euler2 = voxel->euler2;
   euler3 = voxel->euler3;
   neighbor = voxel->neighbor;
   numowners = voxel->numowners;
   grainmisorientation = voxel->grainmisorientation;
   misorientationgradient = voxel->misorientationgradient;
   kernelmisorientation = voxel->kernelmisorientation;
   surfacevoxel = voxel->surfacevoxel;
   unassigned = voxel->unassigned;

   COPY_ARRAY_5(quat, voxel);
   DEEP_COPY_SHARED_VECTOR(grainlist, voxel, IntVectorType, int)
   DEEP_COPY_SHARED_VECTOR(ellipfunclist, voxel, DoubleVectorType, double)
   DEEP_COPY_SHARED_VECTOR(neighborlist, voxel, IntVectorType, int)

}

