/* ============================================================================
 * Copyright (c) 2015 BlueQuartz Softwae, LLC
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
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
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "SO3Sampler.h"


#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Math/ArrayHelpers.hpp"

#include "OrientationLib/OrientationLibConstants.h"
#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"



namespace {
  static const int AnorthicType = 0; // Triclinic
  static const int CyclicType = 1;
  static const int DihedralType = 2;
  static const int TetrahedralType = 3;
  static const int OctahedralType = 4;
}


namespace
{
  static const int NoAxisOrder = 0;
  static const int TwoFoldAxisOrder = 2;
  static const int ThreeFoldAxisOrder = 3;
  static const int FourFoldAxisOrder = 4;
  static const int SixFoldAxisOrder = 6;
}

// Following numbers are coefficients used to calculate the exponential of a matrix


// The following two arrays are used to determine the FZtype (FZtarray) and primary rotation axis order (FZoarray)
// for each of the 32 crystallographic point group symmetries (in the order of the International Tables)
//
//                                       '    1','   -1','    2','    m','  2/m','  222', &
//                                       '  mm2','  mmm','    4','   -4','  4/m','  422', &
//                                       '  4mm',' -42m','4/mmm','    3','   -3','   32', &
//                                       '   3m','  -3m','    6','   -6','  6/m','  622', &
//                                       '  6mm',' -6m2','6/mmm','   23','   m3','  432', &
//                                       ' -43m',' m-3m'/
//
// 1 (C1), -1 (Ci), [triclinic]
// 2 (C2), m (Cs), 2/m (C2h), [monoclinic]
// 222 (D2), mm2 (C2v), mmm (D2h), [orthorhombic]
// 4 (C4), -4 (S4), 4/m (C4h), 422 (D4), 4mm (C4v), -42m (D2d), 4/mmm (D4h), [tetragonal]
// 3 (C3), -3 (C3i), 32 (D3), 3m (C3v), -3m (D3d), [trigonal]
// 6 (C6), -6 (C3h), 6/m (C6h), 622 (D6), 6mm (C6v), -6m2 (D3h), 6/mmm (D6h), [hexagonal]
// 23 (T), m3 (Th), 432 (O), -43m (Td), m-3m (Oh) [cubic]
//
// FZtype
// 0        no symmetry at all
// 1        cyclic symmetry
// 2        dihedral symmetry
// 3        tetrahedral symmetry
// 4        octahedral symmetry
//
// these parameters are used in the so3 module
//
static const int FZtarray[32] = { AnorthicType, AnorthicType,CyclicType,CyclicType,CyclicType,
                                  DihedralType,DihedralType,DihedralType,CyclicType,CyclicType,CyclicType,
                                  DihedralType,DihedralType,DihedralType,DihedralType,CyclicType,CyclicType,
                                  DihedralType,DihedralType,DihedralType,CyclicType,CyclicType,CyclicType,
                                  DihedralType,DihedralType,DihedralType,DihedralType,TetrahedralType,
                                  TetrahedralType,OctahedralType,TetrahedralType,OctahedralType };

static const int FZoarray[32] = { NoAxisOrder,NoAxisOrder,TwoFoldAxisOrder,TwoFoldAxisOrder,
                                  TwoFoldAxisOrder,TwoFoldAxisOrder,TwoFoldAxisOrder,TwoFoldAxisOrder,FourFoldAxisOrder,
                                  FourFoldAxisOrder,FourFoldAxisOrder,FourFoldAxisOrder,FourFoldAxisOrder,FourFoldAxisOrder,
                                  FourFoldAxisOrder,ThreeFoldAxisOrder,ThreeFoldAxisOrder,ThreeFoldAxisOrder,ThreeFoldAxisOrder,
                                  ThreeFoldAxisOrder,SixFoldAxisOrder,SixFoldAxisOrder,SixFoldAxisOrder,SixFoldAxisOrder,SixFoldAxisOrder,
                                  SixFoldAxisOrder,SixFoldAxisOrder,NoAxisOrder,NoAxisOrder,NoAxisOrder,NoAxisOrder,NoAxisOrder};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SO3Sampler::SO3Sampler()
{


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SO3Sampler::~SO3Sampler()
{

}

// ###################################################################
// Copyright (c) 2013-2014, Marc De Graef/Carnegie Mellon University
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
//     - Redistributions of source code must retain the above copyright notice, this list
//        of conditions and the following disclaimer.
//     - Redistributions in binary form must reproduce the above copyright notice, this
//        list of conditions and the following disclaimer in the documentation and/or
//        other materials provided with the distribution.
//     - Neither the names of Marc De Graef, Carnegie Mellon University nor the names
//        of its contributors may be used to endorse or promote products derived from
//        this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ###################################################################

//--------------------------------------------------------------------------
// EMsoft:so3.f90
//--------------------------------------------------------------------------
//
// MODULE: so3
//
//> @author Marc De Graef, Carnegie Mellon University
//
//> @brief everything that has to do with sampling of rotation space SO(3)
//
//> @todo verify that this is correct for point groups with multiple settings, eg, 3m, 32, ...
//
//> @date 05/29/14 MDG 1.0 original
//> @date 10/02/14 MDG 2.0 removed globals + rewrite
//> @date 01/01/15 MDG 2.1 added IsinsideFZ function, also used in dictionary indexing approach
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// We define a number of logical routines, that decide whether or not
// a point in Rodrigues representation lies inside the fundamental zone (FZ)
// for a given crystal symmetry. This follows the Morawiec@Field paper:
//
// A. Morawiec & D. P. Field (1996) Rodrigues parameterization for orientation
// and misorientation distributions, Philosophical Magazine A, 73:4, 1113-1130,
// DOI: 10.1080/01418619608243708
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//
// FUNCTION: IsinsideFZ
//
//> @author Marc De Graef, Carnegie Mellon University
//
//> @brief does Rodrigues point lie inside the relevant FZ
//
//> @param rod Rodrigues coordinates  (double precision)
//> @param FZtype FZ type
//> @param FZorder FZ order
//
//> @date 01/01/15 MDG 1.0 new routine, needed for dictionary indexing approach
//> @date 06/04/15 MDG 1.1 corrected infty to inftyd (double precision infinity)
//--------------------------------------------------------------------------
bool SO3Sampler::IsinsideFZ(double* rod, int FZtype, int FZorder)
{
  bool insideFZ = false;
  // dealing with 180 rotations is needed only for
  // FZtypes 0 and 1; the other FZs are always finite.
  switch(FZtype)
  {
    case AnorthicType:
      insideFZ = true;   // all points are inside the FZ
      break;
    case CyclicType:
      insideFZ = insideCyclicFZ(rod,FZorder);      // infinity is checked inside this function
      break;
    case DihedralType:
      if (rod[3] != std::numeric_limits<double>::infinity()) insideFZ = insideDihedralFZ(rod,FZorder);
      break;
    case TetrahedralType:
      if (rod[3] != std::numeric_limits<double>::infinity()) insideFZ = insideCubicFZ(rod,TetrahedralType);
      break;
    case OctahedralType:
      if (rod[3] != std::numeric_limits<double>::infinity()) insideFZ = insideCubicFZ(rod,OctahedralType);
      break;
    default:
      insideFZ = false;
      break;

  }
  return insideFZ;
}


//--------------------------------------------------------------------------
//
// FUNCTION: insideCyclicFZ
//
//> @author Marc De Graef, Carnegie Mellon University
//
//> @brief does Rodrigues point lie inside cyclic FZ (for 2, 3, 4, and 6-fold)?
//
//> @param rod Rodrigues coordinates  (double precision)
//> @param order depending on main symmetry axis
//
//> @date 05/12/14 MDG 1.0 original
//> @date 10/02/14 MDG 2.0 rewrite
//> @date 06/04/15 MDG 2.1 corrected infty to inftyd (double precision infinity)
//--------------------------------------------------------------------------
bool SO3Sampler::insideCyclicFZ(double* rod, int order)
{

  bool insideFZ = false;

  if (rod[3] != std::numeric_limits<double>::infinity())
  {
    // check the z-component vs. tan(pi/2n)
    insideFZ = fabs(rod[2]*rod[3]) <= LPs::BP[order - 1];
  }
  else if (rod[2] == 0.0)
  {
    insideFZ = true;
  }
  return insideFZ;
}

//--------------------------------------------------------------------------
//
// FUNCTION: insideDihedralFZ
//
//> @author Marc De Graef, Carnegie Mellon University
//
//> @brief does Rodrigues point lie inside dihedral FZ (for 2, 3, 4, and 6-fold)?
//
//> @param rod Rodrigues coordinates (double precision)
//> @param order depending on main symmetry axis
//
//> @todo for now, we ignore here the fact that, among others, the 3m point group can be oriented in two ways;
//> @todo this should be fixable in the future with an additional optional argument
//
//> @date 05/12/14  MDG 1.0 original
//> @date 10/02/14  MDG 2.0 rewrite
//--------------------------------------------------------------------------
bool SO3Sampler::insideDihedralFZ(double* rod, int order)
{


  bool                                  res = false, c1 = false, c2 = false;
  double                     r[3] = { rod[0] * rod[3], rod[1] * rod[3], rod[2] * rod[3]};
  const double r1 = 1.0;

  // first, check the z-component vs. tan(pi/2n)  (same as insideCyclicFZ)
  c1 = fabs(r[2]) <= LPs::BP[order -1];
  res = false;

  // check the square boundary planes if c1=true
  if (c1) {
    switch (order)
    {
      case TwoFoldAxisOrder:
        c2 = (fabs(r[0]) <= r1) && (fabs(r[1]) <= r1);
        break;
      case ThreeFoldAxisOrder:
        c2 = fabs( LPs::srt*r[0]+0.50*r[1]) <= r1;
        c2 = c2 && ( fabs( LPs::srt*r[0]-0.50*r[1]) <= r1 );
        c2 = c2 && ( fabs(r[1]) <= r1 );
        break;
      case FourFoldAxisOrder:
        c2 = (fabs(r[0]) <= r1) && (fabs(r[1]) <= r1);
        c2 = c2 && ((LPs::r22*fabs(r[0]+r[1]) <= r1) && (LPs::r22*fabs(r[0]-r[1]) <= r1));
        break;
      case SixFoldAxisOrder:
        c2 =          fabs( 0.50*r[0]+LPs::srt*r[1]) <= r1;
        c2 = c2 && ( fabs( LPs::srt*r[0]+0.50*r[1]) <= r1 );
        c2 = c2 && ( fabs( LPs::srt*r[0]-0.50*r[1]) <= r1 );
        c2 = c2 && ( fabs( 0.50*r[0]-LPs::srt*r[1]) <= r1 );
        c2 = c2 && ( fabs(r[1]) <= r1 );
        c2 = c2 && ( fabs(r[0]) <= r1 );
        break;
      default:
        res = false;
        break;

    }
    res = c2;
  }
  return res;
}

//--------------------------------------------------------------------------
//
// FUNCTION: insideCubicFZ
//
//> @author Marc De Graef, Carnegie Mellon University
//
//> @brief does Rodrigues point lie inside cubic FZ (octahedral or tetrahedral)?
//
//> @param rod Rodrigues coordinates  (double precision)
//> @param ot 'oct' or 'tet', depending on symmetry
//
//> @date 05/12/14 MDG 1.0 original
//> @date 10/02/14 MDG 2.0 rewrite
//> @date 01/03/15 MDG 2.1 correction of boundary error; simplification of octahedral planes
//> @date 06/04/15 MDG 2.2 simplified handling of components of r
//--------------------------------------------------------------------------
bool SO3Sampler::insideCubicFZ(double* rod, int ot)
{
  bool res = false, c1 = false, c2 = false;
  std::vector<double> r(3);
  r[0] = rod[0] * rod[3];
  r[1] = rod[1] * rod[3];
  r[2] = rod[2] * rod[3];
  const double r1 = 1.0;

  // primary cube planes (only needed for octahedral case)
  if (ot == OctahedralType) {

    typedef OrientationTransforms<std::vector<double>, double> OrientationTransformsType;

    // std::vector<double> absR = OrientationTransformsType::OMHelperType::absValue(r);
    // double maxVal = OrientationTransformsType::OMHelperType::maxval(absR);
    c1 = OrientationTransformsType::OMHelperType::maxval(OrientationTransformsType::OMHelperType::absValue(r)) <= LPs::BP[3];

  } else {
    c1 = true;
  }

  // octahedral truncation planes, both for tetrahedral and octahedral point groups
  c2 = ((fabs(r[0])+fabs(r[1])+fabs(r[2])) <= r1);

  // if both c1 and c2, then the point is inside
  if (c1 && c2) { res = true;}

  return res;
}

//--------------------------------------------------------------------------
//
// FUNCTION: SampleRFZ
//
//> @author Marc De Graef, Carnegie Mellon University
//
//> @brief Generate a uniform sampling of a Rodriguess FZ
//
//> @note This routine fills in a linked list FZlist of Rodrigues points that
//> are inside a specific fundamental zone determined by the sample point group;
//> this list can then be further dealt with in the calling program.
//>
//> Here's how you would use this routine in a main program:
//>
//> use so3
//>
//> integer(kind=irg)       :: FZcnt, nsteps, pgnum
//> type(FZpointd),pointer  :: FZlist, FZtmp
//>
//> nullify(FZlist)
//> FZcnt = 0
//> nsteps = 10
//> pgnum = 32
//> call sampleRFZ(nsteps, pgnum, FZcnt, FZlist)
//>
//> Then you can access all the entries in the list and, for instance, convert them to Euler angles...
//>
//> FZtmp => FZlist                        // point to the top of the list
//> do i = 1, FZcnt                        // loop over all entries
//>   eu = ro2eu(FZtmp%rod)                // convert to Euler angles
//>   do something with eu                 // for instance, write eu to a file
//>   FZtmp => FZtmp%next                  // point to the next entry
//> end do
//>
//> If you just want to look at the first 10 entries on the list and show all other orientation representations:
//>
//> type(orientationtyped):: ot
//>
//> FZtmp => FZlist
//> do i = 1,10
//>   ot = init_orientation(FZtmp%rod,'ro')
//>   call print_orientation(ot)
//>   FZtmp => FZtmp%next
//> end do
//
//> @param nsteps number of steps along semi-edge in cubochoric grid
//> @param pgnum point group number to determine the appropriate Rodrigues fundamental zone
//> @param FZcnt (output) number of points inside fundamental zone
//> @param FZlist (output) linked list of points inside fundamental zone
//
//> @date 05/12/14  MDG 1.0 original
//> @date 10/02/14  MDG 2.0 rewrite, removed all globals, added function arguments
//--------------------------------------------------------------------------
SO3Sampler::OrientationListArrayType SO3Sampler::SampleRFZ(int nsteps,int pgnum)
{

  double                        x, y, z, delta;
  OrientationListArrayType              FZlist;
  int32_t                 FZtype, FZorder;

  // step size for sampling of grid; total number of samples = (2*nsteps+1)**3
  delta = ( 0.50 * LPs::ap)/static_cast<double>(nsteps);

  // determine which function we should call for this point group symmetry
  FZtype = FZtarray[pgnum-1];
  FZorder = FZoarray[pgnum-1];
  typedef OrientationTransforms<DOrientArrayType, double> OrientationTransformsType;

  // loop over the cube of volume pi^2; note that we do not want to include
  // the opposite edges/facets of the cube, to avoid double counting rotations
  // with a rotation angle of 180 degrees.  This only affects the cyclic groups.
  for(int i = -nsteps; i < nsteps; i++)
  {
    x = static_cast<double>(i) * delta;

    for(int j = -nsteps; j < nsteps; j++)
    {
      y = static_cast<double>(j) * delta;

      for (int k = -nsteps; k < nsteps; k++)
      {
        z = static_cast<double>(k) * delta;

        // convert to Rodrigues representation
        DOrientArrayType cu(x, y, z);
        DOrientArrayType rod(4);
        OrientationTransformsType::cu2ro(cu, rod);

        // If insideFZ=true, then add this point to the linked list FZlist and keep
        // track of how many points there are on this list
        bool b = IsinsideFZ(rod.data(), FZtype, FZorder);
        if (b)
        {
          FZlist.push_back(rod);
        }

      }
    }
  }

  return FZlist;
}
