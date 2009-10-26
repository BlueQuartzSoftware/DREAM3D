/*
 * Voxel.cpp
 *
 *  Created on: Oct 21, 2009
 *      Author: mjackson
 */

#include "Voxel.h"

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

Voxel::~Voxel()
{
  // TODO Auto-generated destructor stub
}
