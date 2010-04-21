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
precipitatename(0),
confidence(0.0),
imagequality(0.0),
alreadychecked(0),
euler1(-1.0),
euler2(-1.0),
euler3(-1.0),
hasneighbor(0),
neighbor(-1),
misorientation(0.0),
surfacevoxel(0)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Voxel::~Voxel()
{
}
