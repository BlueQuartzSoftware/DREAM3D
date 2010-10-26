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

#include "Grain.h"
#include <iostream>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Grain::Grain() :
numvoxels(0),
active(0),
packquality(0.0),
numneighbors(0),
newgrainname(0),
gotcontainedmerged(0),
gottwinmerged(0),
gotcolonymerged(0),
centroidx(0.0),
centroidy(0.0),
centroidz(0.0),
surfacegrain(0),
Ixx(0.0),
Iyy(0.0),
Izz(0.0),
Ixy(0.0),
Iyz(0.0),
Ixz(0.0),
omega3(0.0),
averagemisorientation(0.0),
kernelmisorientation(10000.0),
twinnewnumber(-1),
colonynewnumber(-1),
red(0.0),
green(0.0),
blue(0.0),
schmidfactor(0.0),
convexity(0.0),
euler1(0.0),
euler2(0.0),
euler3(0.0),
axiseuler1(0.0),
axiseuler2(0.0),
axiseuler3(0.0),
volume(0.0),
equivdiameter(0.0),
neigherror(0.0),
radius1(0.0),
radius2(0.0),
radius3(0.0),
lowanglefraction(0.0)
{
  neighborlist = new std::vector<int>;
  voxellist = new std::vector<int>;
  ellipfunclist = new std::vector<double>;
  neighborsurfarealist = new std::vector<double>;
  misorientationlist = new std::vector<double>;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
