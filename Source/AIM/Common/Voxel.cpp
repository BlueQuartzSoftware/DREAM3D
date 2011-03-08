///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  Copyright (c) 2009, Michael Groeber, US Air Force Research Laboratory
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
// This code was partly written under US Air Force Contract FA8650-07-D-5800
//
///////////////////////////////////////////////////////////////////////////////

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

