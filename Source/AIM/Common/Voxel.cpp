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
alreadychecked(0),
xc(-1.0),
yc(-1.0),
zc(-1.0),
euler1(-1.0),
euler2(-1.0),
euler3(-1.0),
hasneighbor(0),
neighbor(-1),
misorientation(0.0),
surfacevoxel(0),
neighbor1(-1),
neighbor2(-1),
neighbor3(-1),
neighbor4(-1),
neighbor5(-1),
neighbor6(-1),
available(0),
available90(0),
inside(0)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Voxel::~Voxel()
{
}
