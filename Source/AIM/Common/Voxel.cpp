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
grainname(-1),
confidence(0.0),
imagequality(0.0),
ellipfunc(0.0),
alreadychecked(0),
nearestneighbor(-1),
nearestneighbordistance(-1),
euler1(-1.0),
euler2(-1.0),
euler3(-1.0),
neighbor(-1),
numowners(0),
misorientation(0.0),
kernelmisorientation(0.0),
surfacevoxel(0),
unassigned(0)
{
	grainlist = new std::vector<int>;
	ellipfunclist = new std::vector<double>;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Voxel::~Voxel()
{
  delete grainlist;
  delete ellipfunclist;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Voxel::operator=(Voxel& v)
{
  v.grainname = grainname;
  v.ellipfunc = ellipfunc;
  v.confidence = confidence;
  v.nearestneighbor = nearestneighbor;
  v.nearestneighbordistance = nearestneighbordistance;
  v.imagequality = imagequality;
  v.alreadychecked = alreadychecked;
  v.euler1 = euler1;
  v.euler2 = euler2;
  v.euler3 = euler3;
  v.neighbor = neighbor;
  v.numowners = numowners;
  v.misorientation = misorientation;
  v.kernelmisorientation = kernelmisorientation;
  v.surfacevoxel = surfacevoxel;
  v.unassigned = unassigned;
 // v.grainlist->clear();
  v.grainlist->assign(grainlist->begin(), grainlist->end());
//  v.ellipfunclist->clear();
  v.ellipfunclist->assign(ellipfunclist->begin(), ellipfunclist->end() );
}


