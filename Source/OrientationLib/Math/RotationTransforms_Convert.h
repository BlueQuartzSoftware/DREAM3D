#ifndef _RotationTransforms_H_
#define _RotationTransforms_H_
#include <string>
#include "DREAM3DLib/DREAM3DLib.h"
#include "OrientationLib/OrientationLib.h"
class OrientationLib_EXPORT RotationTransforms
{
public:
  RotationTransforms();
  virtual ~RotationTransforms();
void FatalError(char* func, char* msg);
// ! ###################################################################
// ! Copyright (c) 2013-2014, Marc De Graef/Carnegie Mellon University
// ! All rights reserved.
// !
// ! Redistribution and use in source and binary forms, with or without modification, are
// ! permitted provided that the following conditions are met:
// !
// !     - Redistributions of source code must retain the above copyright notice, this list
// !        of conditions and the following disclaimer.
// !     - Redistributions in binary form must reproduce the above copyright notice, this
// !        list of conditions and the following disclaimer in the documentation and/or
// !        other materials provided with the distribution.
// !     - Neither the names of Marc De Graef, Carnegie Mellon University nor the names
// !        of its contributors may be used to endorse or promote products derived from
// !        this software without specific prior written permission.
// !
// ! THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// ! AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// ! IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ! ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// ! LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// ! DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// ! SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// ! CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// ! OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
// ! USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ! ###################################################################

// !--------------------------------------------------------------------------
// ! CTEMsoft2013:rotations.f90
// !--------------------------------------------------------------------------
// !
// ! MODULE: rotations
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief everything that has to do with rotations and conversions between rotations
// !
// !> @details This file relies a lot on the relations listed in the book "Orientations
// !> and Rotations" by Adam Morawiec [Springer 2004].  I've tried to implement every
// !> available representation for rotations in a way that makes it easy to convert
// !> between any pair.  Needless to say, this needs extensive testing and debugging...
// !>
// !> Instead of converting all the time between representations, I've opted to
// !> "waste" a little more memory and time and provide the option to precompute all the representations.
// !> This way all representations are available via a single data structure.
// !>
// !> Obviously, the individual conversion routines also exist and can be called either in
// !> single or in double precision (using a function interface for each call, so that only
// !> one function name is used).  The conversion routines use the following format for their
// !> call name:  ab2cd, where (ab and cd are two-characters strings selected from the following
// !> possibilities: [the number in parenthesis lists the number of entries that need to be provided]
// !>
// !> eu : euler angle representation (3)
// !> om : orientation matrix representation (3x3)
// !> ax : axis angle representation (4)
// !> ro : Rodrigues vector representation (3)
// !> qu : unit quaternion representation (4)
// !> ho : homochoric representation (3)
// !> cu : cubochoric representation (3).
// !>
// !> hence, conversion from homochoric to euler angle is called as ho2eu(); the argument of
// !> each routine must have the correct number of dimensions and entries.
// !> All 42 conversion routines exist in both single and double precision.
// !>
// !> Some routines were modified in July 2014, to simplify the paths in case the direct conversion
// !> routine does not exist.  Given the complexity of the cubochoric transformations, all routines
// !> going to and from this representation will require at least one and sometimes two or three
// !> intermediate representations.  cu2eu and qu2cu currently represent the longest computation
// !> paths with three intermediate steps each.
// !>
// !> In August 2014, all routines were modified to account for active vs. passive rotations,
// !> after some inconsistencies were discovered that could be traced back to that distinction.
// !> The default is for a rotation to be passive, and only those transformation rules have been
// !> implemented.  For active rotations, the user needs to explicitly take action in the calling
// !> program by setting the correct option in the ApplyRotation function.
// !>
// !> Testing: the program rotationtest.f90 was generated by an IDL script and contains all possible
// !> pairwise and triplet transformations, using a series of input angle combinations; for now, these
// !> are potentially problematic Euler combinations.
// !>
// !> The conventions of this module are:
// !>
// !> - all reference frames are right-handed and orthonormal (except for the Bravais frames)
// !> - a rotation angle is positive for a counterclockwise rotation when viewing along the positive rotation axis towards the origin
// !> - all rotations are interpreted in the passive way
// !> - Euler angles follow the Bunge convention, with phi1 in [0,2pi], Phi in [0,pi], and phi2 in [0,2pi]
// !> - rotation angles (in axis-angle derived representations) are limited to the range [0,pi]
// !>
// !> To make things easier for the user, this module provides a routine to create a rotation
// !> representation starting from an axis, described by a unit axis vector, and a rotation angle.
// !> This routine properly takes the sign of epsijk into account, and always produces a passive rotation.
// !> The user must explicitly take action to interpret a rotation a being active.
// !>
// !> @date 08/04/13 MDG 1.0 original
// !> @date 07/08/14 MDG 2.0 modifications to several routines (mostly simplifications)
// !> @date 08/08/14 MDG 3.0 added active/passive handling (all routines passive)
// !> @date 08/11/14 MDG 3.1 modified Rodrigues vector to 4 components (n and length) to accomodate Infinity
// !> @date 08/18/14 MDG 3.2 added RotateVector, RotateTensor2 routines with active/passive switch
// !> @date 08/20/14 MDG 3.3 completed extensive testing of epsijk<0 mode; all tests passed for the first time !
// !> @date 08/21/14 MDG 3.4 minor correction in om2ax to get things to work for epsijk>0 mode; all tests passed!
// !> @date 09/30/14 MDG 3.5 added routines to make rotation definitions easier
// !> @date 09/30/14 MDG 3.6 added strict range checking routines for all representations (tested on 10/1/14)
// !--------------------------------------------------------------------------
// module rotations

// use local
// use typedefs
// use quaternions


// !--------------------------------
// ! routines to check the validity range of rotation representations
// public :: eu_check
// interface eu_check
//         module procedure eu_check
//         module procedure eu_check_d
// end interface

// public :: ro_check
// interface ro_check
//         module procedure ro_check
//         module procedure ro_check_d
// end interface

// public :: ho_check
// interface ho_check
//         module procedure ho_check
//         module procedure ho_check_d
// end interface

// public :: cu_check
// interface cu_check
//         module procedure cu_check
//         module procedure cu_check_d
// end interface

// public :: qu_check
// interface qu_check
//         module procedure qu_check
//         module procedure qu_check_d
// end interface

// public :: ax_check
// interface ax_check
//         module procedure ax_check
//         module procedure ax_check_d
// end interface

// public :: om_check
// interface om_check
//         module procedure om_check
//         module procedure om_check_d
// end interface

// !--------------------------------
// ! general rotation creation routine, to make sure that a rotation representation is
// ! correctly initialized, takes an axis and an angle as input, returns an orientationtype structure
// interface genrot
//         module procedure genrot
//         module procedure genrot_d
// end interface

// ! general interface routine to populate the orientation type
// interface init_orientation
//         module procedure init_orientation
//         module procedure init_orientation_om
//         module procedure init_orientation_d
//         module procedure init_orientation_om_d
// end interface

// !--------------------------------
// ! convert Euler angles to 3x3 orientation matrix
// public :: eu2om
// interface eu2om
//         module procedure eu2om
//         module procedure eu2om_d
// end interface

// ! convert Euler angles to axis angle
// public :: eu2ax
// interface eu2ax
//         module procedure eu2ax
//         module procedure eu2ax_d
// end interface

// ! convert Euler angles to Rodrigues vector
// public :: eu2ro
// interface eu2ro
//         module procedure eu2ro
//         module procedure eu2ro_d
// end interface

// ! convert Euler angles to quaternion
// public :: eu2qu
// interface eu2qu
//         module procedure eu2qu
//         module procedure eu2qu_d
// end interface

// ! convert Euler angles to homochoric
// public :: eu2ho
// interface eu2ho
//         module procedure eu2ho
//         module procedure eu2ho_d
// end interface

// ! convert Euler angles to cubochoric
// public :: eu2cu
// interface eu2cu
//         module procedure eu2cu
//         module procedure eu2cu_d
// end interface

// !--------------------------------
// ! convert 3x3 orientation matrix to Euler angles
// public :: om2eu
// interface om2eu
//         module procedure om2eu
//         module procedure om2eu_d
// end interface

// ! convert 3x3 orientation matrix to axis angle
// public :: om2ax
// interface om2ax
//         module procedure om2ax
//         module procedure om2ax_d
// end interface

// ! convert 3x3 orientation matrix to Rodrigues
// public :: om2ro
// interface om2ro
//         module procedure om2ro
//         module procedure om2ro_d
// end interface

// ! convert 3x3 rotation matrix to quaternion
// public :: om2qu
// interface om2qu
//         module procedure om2qu
//         module procedure om2qu_d
// end interface

// ! convert 3x3 rotation matrix to homochoric
// public :: om2ho
// interface om2ho
//         module procedure om2ho
//         module procedure om2ho_d
// end interface

// ! convert 3x3 rotation matrix to cubochoric
// public :: om2cu
// interface om2cu
//         module procedure om2cu
//         module procedure om2cu_d
// end interface

// !--------------------------------
// ! convert axis angle pair to euler
// public :: ax2eu
// interface ax2eu
//         module procedure ax2eu
//         module procedure ax2eu_d
// end interface

// ! convert axis angle pair to orientation matrix
// public :: ax2om
// interface ax2om
//         module procedure ax2om
//         module procedure ax2om_d
// end interface

// ! convert axis angle pair to Rodrigues
// public :: ax2ro
// interface ax2ro
//         module procedure ax2ro
//         module procedure ax2ro_d
// end interface

// ! convert axis angle pair to quaternion
// public :: ax2qu
// interface ax2qu
//         module procedure ax2qu
//         module procedure ax2qu_d
// end interface

// ! convert axis angle pair to homochoric representation
// public :: ax2ho
// interface ax2ho
//         module procedure ax2ho
//         module procedure ax2ho_d
// end interface

// ! convert axis angle pair to cubochoric
// public :: ax2cu
// interface ax2cu
//         module procedure ax2cu
//         module procedure ax2cu_d
// end interface

// !--------------------------------
// ! convert Rodrigues vector to Euler angles
// public :: ro2eu
// interface ro2eu
//         module procedure ro2eu
//         module procedure ro2eu_d
// end interface

// ! convert Rodrigues vector to orientation matrix
// public :: ro2om
// interface ro2om
//         module procedure ro2om
//         module procedure ro2om_d
// end interface

// ! convert Rodrigues vector to axis angle pair
// public :: ro2ax
// interface ro2ax
//         module procedure ro2ax
//         module procedure ro2ax_d
// end interface

// ! convert Rodrigues vector to quaternion
// public :: ro2qu
// interface ro2qu
//         module procedure ro2qu
//         module procedure ro2qu_d
// end interface

// ! convert Rodrigues vector to homochoric
// public :: ro2ho
// interface ro2ho
//         module procedure ro2ho
//         module procedure ro2ho_d
// end interface

// ! convert Rodrigues vector to cubochoric
// public :: ro2cu
// interface ro2cu
//         module procedure ro2cu
//         module procedure ro2cu_d
// end interface

// !--------------------------------
// ! convert quaternion to Euler angles
// public :: qu2eu
// interface qu2eu
//         module procedure qu2eu
//         module procedure qu2eu_d
// end interface

// ! convert quaternion to orientation matrix
// public :: qu2om
// interface qu2om
//         module procedure qu2om
//         module procedure qu2om_d
// end interface

// ! convert quaternion to axis angle
// public :: qu2ax
// interface qu2ax
//         module procedure qu2ax
//         module procedure qu2ax_d
// end interface

// ! convert quaternion to Rodrigues
// public :: qu2ro
// interface qu2ro
//         module procedure qu2ro
//         module procedure qu2ro_d
// end interface

// ! convert quaternion to homochoric
// public :: qu2ho
// interface qu2ho
//         module procedure qu2ho
//         module procedure qu2ho_d
// end interface

// ! convert quaternion to cubochoric
// public :: qu2cu
// interface qu2cu
//         module procedure qu2cu
//         module procedure qu2cu_d
// end interface

// !--------------------------------
// ! convert homochoric to euler
// public :: ho2eu
// interface ho2eu
//         module procedure ho2eu
//         module procedure ho2eu_d
// end interface

// ! convert homochoric to orientation matrix
// public :: ho2om
// interface ho2om
//         module procedure ho2om
//         module procedure ho2om_d
// end interface

// ! convert homochoric to axis angle pair
// public :: ho2ax
// interface ho2ax
//         module procedure ho2ax
//         module procedure ho2ax_d
// end interface

// ! convert homochoric to Rodrigues
// public :: ho2ro
// interface ho2ro
//         module procedure ho2ro
//         module procedure ho2ro_d
// end interface

// ! convert homochoric to quaternion
// public :: ho2qu
// interface ho2qu
//         module procedure ho2qu
//         module procedure ho2qu_d
// end interface

// ! convert homochoric to cubochoric
// public :: ho2cu
// interface ho2cu
//         module procedure ho2cu
//         module procedure ho2cu_d
// end interface

// !--------------------------------
// ! convert cubochoric to euler
// public :: cu2eu
// interface cu2eu
//         module procedure cu2eu
//         module procedure cu2eu_d
// end interface

// ! convert cubochoric to orientation matrix
// public :: cu2om
// interface cu2om
//         module procedure cu2om
//         module procedure cu2om_d
// end interface

// ! convert cubochoric to axis angle
// public :: cu2ax
// interface cu2ax
//         module procedure cu2ax
//         module procedure cu2ax_d
// end interface

// ! convert cubochoric to Rodrigues
// public :: cu2ro
// interface cu2ro
//         module procedure cu2ro
//         module procedure cu2ro_d
// end interface

// ! convert cubochoric to quaternion
// public :: cu2qu
// interface cu2qu
//         module procedure cu2qu
//         module procedure cu2qu_d
// end interface

// ! convert cubochoric to homochoric
// public :: cu2ho
// interface cu2ho
//         module procedure cu2ho
//         module procedure cu2ho_d
// end interface

// ! apply a rotation to a vector
// public :: RotateVector
// interface RotateVector
//         module procedure RotVec_om
//         module procedure RotVec_om_d
//         module procedure RotVec_qu
//         module procedure RotVec_qu_d
// end interface

// ! apply a rotation to a second rank tensor
// public :: RotateTensor2
// interface RotateTensor2
//         module procedure RotTensor2_om
//         module procedure RotTensor2_om_d
// end interface


// !--------------------------------
// ! print quaternion and equivalent 3x3 rotation matrix
// public :: print_orientation
// interface print_orientation
//         module procedure print_orientation
//         module procedure print_orientation_d
// end interface




// !--------------------------------------------------------------------------
// !
// ! Function: eu_check
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief verify that the Euler angles are in the correct respective ranges
// !
// !> @param eu 3-component vector (single precision)
// !>
// !
// !> @date 9/30/14   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::eu_check(float* eu, float* res);

// use local
// use constants
// use error

// IMPLICIT NONE







// !--------------------------------------------------------------------------
// !
// ! Function: eu_check_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief verify that the Euler angles are in the correct respective ranges
// !
// !> @param eu 3-component vector (double precision)
// !>
// !
// !> @date 9/30/14   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::eu_check(double* eu, double* res);

// use local
// use constants
// use error

// IMPLICIT NONE







// !--------------------------------------------------------------------------
// !
// ! Function: ro_check
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief verify that the Rodrigues vector has positive length and unit axis vector
// !
// !> @param ro 4-component vector (single precision)
// !>
// !
// !> @date 9/30/14   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ro_check(float* ro, float* res);

// use local
// use constants
// use error

// IMPLICIT NONE







// !--------------------------------------------------------------------------
// !
// ! Function: ro_check_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief verify that the Rodrigues vector has positive length and unit axis vector
// !
// !> @param ro 4-component vector (double precision)
// !>
// !
// !> @date 9/30/14   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ro_check(double* ro, double* res);

// use local
// use constants
// use error

// IMPLICIT NONE







// !--------------------------------------------------------------------------
// !
// ! Function: ho_check
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief verify that the homochoric vector is inside or on the homochoric ball
// !
// !> @param ho 3-component vector (single precision)
// !>
// !
// !> @date 9/30/14   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ho_check(float* ho, float* res);

// use local
// use constants
// use error

// IMPLICIT NONE







// !--------------------------------------------------------------------------
// !
// ! Function: ho_check_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief verify that the homochoric vector is inside or on the homochoric ball
// !
// !> @param ho 3-component vector (double precision)
// !>
// !
// !> @date 9/30/14   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ho_check(double* ho, double* res);

// use local
// use constants
// use error

// IMPLICIT NONE







// !--------------------------------------------------------------------------
// !
// ! Function: cu_check
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief verify that the cubochoric vector is inside or on the cube
// !
// !> @param cu 3-component vector (single precision)
// !>
// !
// !> @date 9/30/14   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::cu_check(float* cu, float* res);

// use local
// use constants
// use error

// IMPLICIT NONE







// !--------------------------------------------------------------------------
// !
// ! Function: cu_check_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief verify that the cubochoric vector is inside or on the cube
// !
// !> @param cu 3-component vector (double precision)
// !>
// !
// !> @date 9/30/14   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::cu_check(double* cu, double* res);

// use local
// use constants
// use error

// IMPLICIT NONE







// !--------------------------------------------------------------------------
// !
// ! Function: qu_check
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief verify that the quaternion has unit length and positive scalar part
// !
// !> @param qu 4-component vector (single precision)
// !>
// !
// !> @date 9/30/14   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::qu_check(float* qu, float* res);

// use local
// use constants
// use error

// IMPLICIT NONE







// !--------------------------------------------------------------------------
// !
// ! Function: qu_check_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief verify that the quaternion has unit length and positive scalar part
// !
// !> @param qu 4-component vector (double precision)
// !>
// !
// !> @date 9/30/14   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::qu_check(double* qu, double* res);

// use local
// use constants
// use error

// IMPLICIT NONE







// !--------------------------------------------------------------------------
// !
// ! Function: ax_check
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief verify that the axis angle pair has a unit vector and angle in the correct range
// !
// !> @param ax 4-component vector (single precision)
// !>
// !
// !> @date 9/30/14   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ax_check(float* ax, float* res);

// use local
// use constants
// use error

// IMPLICIT NONE







// !--------------------------------------------------------------------------
// !
// ! Function: ax_check_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief verify that the axis angle pair has a unit vector and angle in the correct range
// !
// !> @param ax 4-component vector (double precision)
// !>
// !
// !> @date 9/30/14   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ax_check_D(float* ax, float* res);

// use local
// use constants
// use error

// IMPLICIT NONE







// !--------------------------------------------------------------------------
// !
// ! Function: om_check
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief verify that the rotation matrix is actually a proper rotation matrix
// !
// !> @param om 3x3-component matrix (single precision)
// !>
// !
// !> @date 9/30/14   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::om_check(float* om, float* res);

// use local
// use constants
// use error

// IMPLICIT NONE




// ! compute the determinant (must be +1)

// ! next check the row and column sums



// !--------------------------------------------------------------------------
// !
// ! Function: om_check_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief verify that the rotation matrix is actually a proper rotation matrix
// !
// !> @param om 3x3-component matrix (double precision)
// !>
// !
// !> @date 9/30/14   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::om_check(double* om, double* res);

// use local
// use constants
// use error

// IMPLICIT NONE




// ! compute the determinant (must be +1)

// ! next check the row and column sums



// !--------------------------------------------------------------------------
// !--------------------------------------------------------------------------
// ! here we start with a series of input routines
// !--------------------------------------------------------------------------
// !--------------------------------------------------------------------------

// !--------------------------------------------------------------------------
// !
// ! Function: genrot
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief generate a passive rotation representation, given the unit axis vector and the rotation angle
// !
// !> @param av 3-component vector (single precision)
// !> @param omega rotation angle (radians)
// !>
// !
// !> @date 9/30/14   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::genrot(float* av,omega, float* res);

// use local
// use constants
// use error

// IMPLICIT NONE



// ! first make sure that the rotation angle is in the interval [0,pi]

// ! define the axis-angle pair with the correct sign of epsijk

// ! normalize the unit vector



// !--------------------------------------------------------------------------
// !
// ! Function: genrot_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief generate a passive rotation representation, given the unit axis vector and the rotation angle
// !
// !> @param av 3-component vector (double precision)
// !> @param omega rotation angle (radians)
// !>
// !
// !> @date 9/30/14   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::genrot(double* av,omega, double* res);

// use local
// use constants
// use error

// IMPLICIT NONE



// ! first make sure that the rotation angle is in the interval [0,pi]

// ! define the axis-angle pair with the correct sign of epsijk

// ! normalize the unit vector



// !--------------------------------------------------------------------------
// !
// ! Function: init_orientation
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief take an orientation representation with 3 components and init all others (single precision)
// !
// !> @param orient 3-component vector (single precision)
// !> @param intype input type ['eu', 'ro', 'ho', 'cu']
// !> @param rotcheck  optional parameter to enforce strict range checking
// !
// !> @date 8/04/13   MDG 1.0 original
// !> @date 9/30/14   MDG 1.1 added testing of valid ranges
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::init_orientation(float* orient,intype,rotcheck, float* res);

// use local

// IMPLICIT NONE





// !--------------------------------------------------------------------------
// !
// ! Function: init_orientation_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief take an orientation representation with 3 components and init all others (double precision)
// !
// !> @param orient 3-component vector (double precision)
// !> @param intype input type ['eu', 'ro', 'ho', 'cu']
// !> @param rotcheck  optional parameter to enforce strict range checking
// !>
// !
// !> @date 8/04/13   MDG 1.0 original
// !> @date 9/30/14   MDG 1.1 added testing of valid ranges
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::init_orientation(double* orient,intype,rotcheck, double* res);

// use local
// use error
// use constants

// IMPLICIT NONE





// !--------------------------------------------------------------------------
// !
// ! Function: init_orientation_om
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief take an orientation representation with 3x3 components and init all others (single precision)
// !
// !> @param orient r-component vector (single precision)
// !> @param intype input type ['om']
// !> @param rotcheck  optional parameter to enforce strict range checking
// !>
// !
// !> @date 8/04/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::init_orientation_om(float* orient,intype,rotcheck, float* res);

// use local

// IMPLICIT NONE






// !--------------------------------------------------------------------------
// !
// ! Function: init_orientation_om_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief take an orientation representation with 3x3 components and init all others (double precision)
// !
// !> @param orient r-component vector (double precision)
// !> @param intype input type ['om']
// !> @param rotcheck  optional parameter to enforce strict range checking
// !>
// !
// !> @date 8/04/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::init_orientation_om(double* orient,intype,rotcheck, double* res);

// use local

// IMPLICIT NONE







// !--------------------------------------------------------------------------
// !--------------------------------------------------------------------------
// ! here we start with a series of conversion routines between representations
// !--------------------------------------------------------------------------
// !--------------------------------------------------------------------------

// !--------------------------------------------------------------------------
// !
// ! Function: eu2om
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief Euler angles to orientation matrix (single precision) [Morawiec, page 28]
// !
// !> @param e 3 Euler angles in radians (single precision)
// !>
// !
// !> @date 8/04/13   MDG 1.0 original
// !> @data 7/23/14   MDG 1.1 verified
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::eu2om(float* e, float* res);

// use local

// IMPLICIT NONE







// !--------------------------------------------------------------------------
// !
// ! Function: eu2om_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief Euler angles to orientation matrix (double precision) [Morawiec, page 28]
// !
// !> @param e 3 Euler angles in radians (double precision)
// !>
// !
// !> @date 8/04/13   MDG 1.0 original
// !> @data 7/23/14   MDG 1.1 verified
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::eu2om(double* e, double* res);

// use local

// IMPLICIT NONE







// !--------------------------------------------------------------------------
// !
// ! FUNCTION: eu2ax
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert euler to axis angle
// !
// !> @param e 3 euler angles (single precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !> @date 7/23/14   MDG 2.0 explicit implementation
// !> @date 7/23/14   MDG 2.1 exception for zero rotation angle
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::eu2ax(float* e, float* res);

// use local
// use constants




// ! return a default identity axis-angle pair
// ! passive axis-angle pair so a minus sign in front

// ! make sure alpha is positive


// !--------------------------------------------------------------------------
// !
// ! FUNCTION: eu2ax_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert euler to axis angle
// !
// !> @param e 3 euler angles (double precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !> @date 7/23/14   MDG 1.1 exception for zero rotation angle
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::eu2ax(double* e, double* res);

// use local
// use constants




// ! return a default identity axis-angle pair
// ! passive axis-angle pair so a minus sign in front

// ! make sure alpha is positive


// !--------------------------------------------------------------------------
// !
// ! Function: eu2ro
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief Euler angles to Rodrigues vector (single precision) [Morawiec, page 40]
// !
// !> @param e 3 Euler angles in radians (single precision)
// !>
// !
// !> @date 8/04/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::eu2ro(float* e, float* res);

// use local
// use constants

// IMPLICIT NONE



// ! first go to the axis-angle representation

// ! then adjust the fourth component to be tan(omega/2)



// !--------------------------------------------------------------------------
// !
// ! Function: eu2ro_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief Euler angles to Rodrigues vector (double precision) [Morawiec, page 40]
// !
// !> @param e 3 Euler angles in radians (double precision)
// !>
// !
// !> @date 8/04/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::eu2ro(double* e, double* res);

// use local
// use constants

// IMPLICIT NONE



// ! first go to the axis-angle representation

// ! then adjust the fourth component to be tan(omega/2)



// !--------------------------------------------------------------------------
// !
// ! Function: eu2qu
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief Euler angles to quaternion (single precision) [Morawiec, page 40]
// !
// !> @note verified 8/5/13
// !
// !> @param e 3 Euler angles in radians (single precision)
// !>
// !
// !> @date 8/04/13   MDG 1.0 original
// !> @date 8/07/14   MDG 1.1 verified
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::eu2qu(float* e, float* res);

// use local
// use constants

// IMPLICIT NONE





// ! passive quaternion

// ! first component must be positive


// !--------------------------------------------------------------------------
// !
// ! Function: eu2qu_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief Euler angles to quaternion (double precision) [Morawiec, page 40]
// !
// !> @note verified 8/5/13
// !
// !> @param e 3 Euler angles in radians (double precision)
// !>
// !
// !> @date 8/04/13   MDG 1.0 original
// !> @date 8/07/14   MDG 1.1 verified
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::eu2qu(double* e, double* res);

// use local
// use constants

// IMPLICIT NONE





// ! passive quaternion

// ! first component must be positive


// !--------------------------------------------------------------------------
// !
// ! Function: om2eu
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief orientation matrix to euler angles (single precision)
// !
// !> @note verified 8/19/14 using Mathematica
// !
// !> @param o orientation matrix (single precision)
// !
// !> @date 8/04/13   MDG 1.0 original
// !> @date 8/19/14   MDG 1.1 verification using Mathematica
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::om2eu(float* o, float* res);

// use local
// use constants

// IMPLICIT NONE


// ! we arbitrarily assign the entire angle to phi_1

// ! reduce Euler angles to definition ranges (and positive values only)


// !--------------------------------------------------------------------------
// !
// ! Function: om2eu_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief orientation matrix to euler angles (double precision)
// !
// !> @note verified 8/19/14 using Mathematica
// !
// !> @param o orientation matrix (double precision)
// !
// !> @date 8/04/13   MDG 1.0 original
// !> @date 8/19/14   MDG 1.1 verification using Mathematica
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::om2eu(double* o, double* res);

// use local
// use constants

// IMPLICIT NONE


// ! we arbitrarily assign the entire angle to phi_1

// ! reduce Euler angles to definition ranges (and positive values only)


// !--------------------------------------------------------------------------
// !
// ! Function: ax2om
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief Axis angle pair to orientation matrix (single precision)
// !
// !> @note verified 8/5/13.
// !
// !> @param a axis angle pair (single precision)
// !>
// !
// !> @date 8/04/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ax2om(float* a, float* res);

// use local
// use constants

// IMPLICIT NONE










// !--------------------------------------------------------------------------
// !
// ! Function: ax2om_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief Axis angle pair to orientation matrix (double precision)
// !
// !> @note verified 8/5/13.
// !
// !> @param a axis angle pair (double precision)
// !>
// !
// !> @date 8/04/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ax2om(double* a, double* res);

// use local
// use constants

// IMPLICIT NONE











// !--------------------------------------------------------------------------
// !
// ! Function: qu2eu
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief Quaternion to Euler angles (single precision) [Morawiec page 40, with errata !!!! ]
// !
// !> @param q quaternion (single precision)
// !>
// !
// !> @date 8/04/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::qu2eu(float* q, float* res);

// use local
// use constants

// IMPLICIT NONE








// !--------------------------------------------------------------------------
// !
// ! Function: qu2eu_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief Quaternion to Euler angles (double precision) [Morawiec page 40, with errata !!!! ]
// !
// !> @param q quaternion (double precision)
// !>
// !
// !> @date 8/04/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::qu2eu(double* q, double* res);

// use local
// use constants

// IMPLICIT NONE










// !--------------------------------------------------------------------------
// !
// ! Function: ax2ho
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief Axis angle pair to homochoric (single precision)
// !
// !> @param a axis-angle pair (single precision)
// !>
// ! !
// !> @date 8/04/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ax2ho(float* a, float* res);

// use local
// use constants

// IMPLICIT NONE



// ! a(4) must be in [0,pi] !




// !--------------------------------------------------------------------------
// !
// ! Function: ax2ho_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief Axis angle pair to homochoric (double precision)
// !
// !> @param a axis-angle pair (double precision)
// !>
// ! !
// !> @date 8/04/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ax2ho(double* a, double* res);

// use local

// IMPLICIT NONE



// ! a(4) must be in [0,pi] !




// !--------------------------------------------------------------------------
// !
// ! Function: ho2ax
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief Homochoric to axis angle pair (single precision)
// !
// !> @param h homochoric coordinates (single precision)
// !>
// !
// !
// !> @date 8/04/13  MDG 1.0 original
// !> @date 07/21/14 MDG 1.1 double precision fit coefficients
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ho2ax(float* h, float* res);

// use local
// use constants

// IMPLICIT NONE



// ! normalize h and store the magnitude

// ! convert the magnitude to the rotation angle



// !--------------------------------------------------------------------------
// !
// ! Function: ho2ax_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief Homochoric to axis angle pair (double precision)
// !
// !> @param h homochoric coordinates (double precision)
// !>
// !
// !> @date 8/04/13   MDG 1.0 original
// !> @date 07/21/14 MDG 1.1 double precision fit coefficients
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ho2ax(double* h, double* res);

// use local
// use constants

// IMPLICIT NONE



// ! normalize h and store the magnitude

// ! convert the magnitude to the rotation angle



// !--------------------------------------------------------------------------
// !
// ! FUNCTION: om2ax
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert orientation matrix to axis angle
// !
// !> @details this assumes that the matrix represents a passive rotation.
// !
// !> @param om 3x3 orientation matrix (single precision)
// !>
// !
// !> @date 8/12/13  MDG 1.0 original
// !> @date 07/08/14 MDG 2.0 replaced by direct solution
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::om2ax(float* om, float* res);

// use local
// use constants
// use error
// use io




// ! first get the rotation angle

// ! set some initial LAPACK variables

// ! first initialize the parameters for the LAPACK DGEEV routines

// !     SUBROUTINE DGEEV( JOBVL, JOBVR, N, A, LDA, WR, WI, VL, LDVL, VR,
// !    $                  LDVR, WORK, LWORK, INFO )

// ! call the eigenvalue solver

// ! next, find the eigenvalue complex(1,0)


// !--------------------------------------------------------------------------
// !
// ! FUNCTION: om2ax_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert orientation matrix to axis angle
// !
// !> @param om 3x3 orientation matrix (double precision)
// !>
// !
// !> @date 8/12/13  MDG 1.0 original
// !> @date 07/08/14 MDG 2.0 replaced by direct solution
// !> @date 08/20/14 MDG 3.0 replaced by eigenvalue-based method
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::om2ax(double* om, double* res);

// use local
// use constants
// use error
// use io




// ! first get the rotation angle

// ! set some initial LAPACK variables

// ! first initialize the parameters for the LAPACK DGEEV routines

// !     SUBROUTINE DGEEV( JOBVL, JOBVR, N, A, LDA, WR, WI, VL, LDVL, VR,
// !    $                  LDVR, WORK, LWORK, INFO )

// ! call the eigenvalue solver

// ! next, find the eigenvalue complex(1,0)


// !--------------------------------------------------------------------------
// !
// ! Function: ro2ax
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief Rodrigues vector to axis angle pair (single precision)
// !
// !> @param r Rodrigues vector (single precision)
// !>
// !
// !> @date 8/04/13   MDG 1.0 original
// !> @date 8/11/14   MDG 1.1 added infty handling
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ro2ax(float* r, float* res);

// use local
// use constants

// IMPLICIT NONE







// !--------------------------------------------------------------------------
// !
// ! Function: ro2ax_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief Rodrigues vector to axis angle pair (double precision)
// !
// !> @param r Rodrigues vector (double precision)
// !>
// !
// !> @date 8/04/13   MDG 1.0 original
// !> @date 8/11/14   MDG 1.1 added infty handling
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ro2ax(double* r, double* res);

// use local
// use constants

// IMPLICIT NONE








// !--------------------------------------------------------------------------
// !
// ! FUNCTION: ax2ro
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert axis angle to Rodrigues
// !
// !> @param a axis angle pair (single precision)
// !>
// !
// !> @date 8/12/13 MDG 1.0 original
// !> @date 7/6/14  MDG 2.0 simplified
// !> @date 8/11/14 MDG 2.1 added infty handling
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ax2ro(float* a, float* res);

// use local
// use constants





// ! we need to deal with the 180 degree case


// !--------------------------------------------------------------------------
// !
// ! FUNCTION: ax2ro_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert axis angle to Rodrigues
// !
// !> @param a axis angle pair (double precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !> @date 7/6/14  MDG 2.0 simplified
// !> @date 8/11/14 MDG 2.1 added infty handling
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ax2ro(double* a, double* res);

// use local
// use constants





// ! we need to deal with the 180 degree case




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: ax2qu
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert axis angle to quaternion
// !
// !> @param a axis angle pair (single precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !> @date 7/23/14   MDG 1.1 explicit transformation
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ax2qu(float* a, float* res);

// use local





// !--------------------------------------------------------------------------
// !
// ! FUNCTION: ax2qu_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert axis angle to quaternion
// !
// !> @param a axis angle pair (double precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !> @date 7/23/14   MDG 1.1 explicit transformation
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ax2qu(double* a, double* res);

// use local







// !--------------------------------------------------------------------------
// !
// ! FUNCTION: ro2ho
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert rodrigues to homochoric
// !
// !> @param r Rodrigues vector (single precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !> @date 7/24/14   MDG 2.0 explicit transformation
// !> @date 8/11/14   MDG 3.0 added infty handling
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ro2ho(float* r, float* res);

// use local
// use constants






// !--------------------------------------------------------------------------
// !
// ! FUNCTION: ro2ho_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert rodrigues to homochoric
// !
// !> @param r Rodrigues vector (double precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !> @date 7/24/14   MDG 2.0 explicit transformation
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ro2ho(double* r, double* res);

// use local
// use constants








// !--------------------------------------------------------------------------
// !
// ! FUNCTION: qu2om
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert a quaternion to a 3x3 matrix
// !
// !> @param q quaternion (single precision)
// !>
// !
// !> @note verified 8/5/13
// !
// !> @date 6/03/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::qu2om(float* q, float* res);

// use local
// use constants










// !--------------------------------------------------------------------------
// !
// ! FUNCTION: qu2om_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert a quaternion to a 3x3 matrix (double precision)
// !
// !> @param q quaternion (double precision)
// !>
// !
// !> @note verified 8/5/13
// !
// !> @date 6/03/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::qu2om(double* q, double* res);

// use local
// use constants










// !--------------------------------------------------------------------------
// !
// ! FUNCTION: om2qu
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert a 3x3 rotation matrix to a unit quaternion (see Morawiec, page 37)
// !
// !> @param x 3x3 matrix to be converted (single precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !> @date 8/18/14   MDG 2.0 new version
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::om2qu(float* x, float* res);

// use local
// use constants








// ! verify the signs (q0 always positive)

// ! normalize

// ! we need to do a quick test here to make sure that the
// ! sign of the vector part is the same as that of the
// ! corresponding vector in the axis-angle representation;
// ! these two can end up being different, presumably due to rounding
// ! issues, but this needs to be further analyzed...
// ! This adds a little bit of computation overhead but for now it
// ! is the easiest way to make sure the signs are correct.


// !--------------------------------------------------------------------------
// !
// ! FUNCTION: om2qu_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert a 3x3 rotation matrix to a unit quaternion (see Morawiec, page 37)
// !
// !> @param x 3x3 matrix to be converted (double precision)
// !
// !> @date 8/12/13   MDG 1.0 original
// !> @date 8/18/14   MDG 2.0 new version
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::om2qu(double* x, double* res);

// use local
// use constants








// ! verify the signs (q0 always positive)

// ! normalize

// ! we need to do a quick test here to make sure that the
// ! sign of the vector part is the same as that of the
// ! corresponding vector in the axis-angle representation;
// ! these two can end up being different, presumably due to rounding
// ! issues, but this needs to be further analyzed...
// ! This adds a little bit of computation overhead but for now it
// ! is the easiest way to make sure the signs are correct.



// !--------------------------------------------------------------------------
// !
// ! FUNCTION: qu2ax
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert quaternion to axis angle
// !
// !> @param q quaternion (single precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !> @date 7/23/14   MDG 2.0 explicit transformation
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::qu2ax(float* q, float* res);

// use local
// use constants


// ! if the angle equals zero, then we return the rotation axis as [001]


// !--------------------------------------------------------------------------
// !
// ! FUNCTION: qu2ax_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert quaternion to axis angle
// !
// !> @param q quaternion (double precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !> @date 7/23/14   MDG 2.0 explicit transformation
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::qu2ax(double* q, double* res);

// use local
// use constants


// ! if the angle equals zero, then we return the rotation axis as [001]



// !--------------------------------------------------------------------------
// !
// ! FUNCTION: qu2ro
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert quaternion to Rodrigues
// !
// !> @param q quaternion (single precision)
// !
// !> @date 8/12/13   MDG 1.0 original
// !> @date 7/23/14   MDG 2.0 direct transformation
// !> @date 8/11/14   MDG 2.1 added infty handling
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::qu2ro(float* q, float* res);

// use local
// use constants






// !--------------------------------------------------------------------------
// !
// ! FUNCTION: qu2ro_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert quaternion to Rodrigues
// !
// !> @param q quaternion (double precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !> @date 7/23/14   MDG 2.0 direct transformation
// !> @date 8/11/14   MDG 2.1 added infty handling
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::qu2ro(double* q, double* res);

// use local
// use constants






// !--------------------------------------------------------------------------
// !
// ! FUNCTION: qu2ho
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert quaternion to homochoric
// !
// !> @param q quaternion (single precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !> @date 7/23/14   MDG 2.0 explicit transformation
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::qu2ho(float* q, float* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: qu2ho_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert quaternion to homochoric
// !
// !> @param q quaternion (double precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !> @date 7/23/14   MDG 2.0 explicit transformation
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::qu2ho(double* q, double* res);

// use local



// !--------------------------------------------------------------------------
// !
// ! FUNCTION: ho2cu
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert homochoric to cubochoric
// !
// !> @param h homochoric coordinates (single precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ho2cu(float* h, float* res);

// use local
// use Lambert, only: LambertBallToCube



// ! calling program must have initialized the Lambert parameters!!!!
// !>


// !--------------------------------------------------------------------------
// !
// ! FUNCTION: ho2cu_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert homochoric to cubochoric
// !
// !> @param h homochoric coordinates (double precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ho2cu(double* h, double* res);

// use local
// use Lambert, only: LambertBallToCube



// ! calling program must have initialized the Lambert parameters!!!!
// !>


// !--------------------------------------------------------------------------
// !
// ! FUNCTION: cu2ho
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert cubochoric to homochoric
// !
// !> @param c cubochoric coordinates (single precision)
// !>
// !
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::cu2ho(float* c, float* res);

// use local
// use Lambert, only: LambertCubeToBall



// ! calling program must have initialized the Lambert parameters!!!!
// !>


// !--------------------------------------------------------------------------
// !
// ! FUNCTION: cu2ho_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert cubochoric to homochoric
// !
// !> @param c cubochoric coordinates (double precision)
// !>
// !
// !> @note calling program MUST have initialized the Lambert parameters first!!!
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::cu2ho(double* c, double* res);

// use local
// use Lambert, only: LambertCubeToBall



// ! calling program must have initialized the Lambert parameters!!!!
// !>



// !--------------------------------------------------------------------------
// !--------------------------------------------------------------------------
// !--------------------------------------------------------------------------
// !--------------------------------------------------------------------------
// !--------------------------------------------------------------------------
// !--------------------------------------------------------------------------
// ! and here are a bunch of transformation routines that are derived from the others
// !--------------------------------------------------------------------------
// !--------------------------------------------------------------------------
// !--------------------------------------------------------------------------
// !--------------------------------------------------------------------------
// !--------------------------------------------------------------------------
// !--------------------------------------------------------------------------

// !--------------------------------------------------------------------------
// !
// ! Function: ro2eu
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief Rodrigues vector to Euler angles (single precision)
// !
// !> @param r Rodrigues vector (single precision)
// !>
// !
// !> @date 8/04/13   MDG 1.0 original
// !> @date 8/11/14   MDG 1.1 added infty handling
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ro2eu(float* r, float* res);

// use local
// use constants

// IMPLICIT NONE





// !--------------------------------------------------------------------------
// !
// ! Function: ro2eu_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief Rodrigues vector to Euler angles (double precision)
// !
// !> @param r Rodrigues vector (double precision)
// !>
// !
// !> @date 8/04/13   MDG 1.0 original
// !> @date 8/11/14   MDG 1.1 added infty handling
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ro2eu(double* r, double* res);

// use local
// use constants

// IMPLICIT NONE






// !--------------------------------------------------------------------------
// !
// ! FUNCTION: eu2ho
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert euler to homochoric
// !
// !> @param e 3 euler angles (single precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::eu2ho(float* e, float* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: eu2ho_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert euler to homochoric
// !
// !> @param e 3 euler angles (double precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::eu2ho(double* e, double* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: om2ro
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert orientation matrix to Rodrigues
// !
// !> @param om 3x3 orientation matrix (single precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::om2ro(float* om, float* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: om2ro_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert orientation matrix to Rodrigues
// !
// !> @param om 3x3 orientation matrix (double precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::om2ro(double* om, double* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: om2ho
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert orientation matrix to homochoric
// !
// !> @param om 3x3 orientation matrix (single precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !> @date 07/08/14 MDG 2.0 simplification via ax (shorter path)
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::om2ho(float* om, float* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: om2ho_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert orientation matrix to homochoric
// !
// !> @param om 3x3 orientation matrix (double precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !> @date 07/08/14 MDG 2.0 simplification via ax (shorter path)
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::om2ho(double* om, double* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: ax2eu
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert axis angle to euler
// !
// !> @param a axis angle pair (single precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !> @date 07/08/14 MDG 2.0 simplification via ro (shorter path)
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ax2eu(float* a, float* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: ax2eu_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert axis angle to euler
// !
// !> @param a axis angle pair (double precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !> @date 07/08/14 MDG 2.0 simplification via ro (shorter path)
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ax2eu(double* a, double* res);

// use local






// !--------------------------------------------------------------------------
// !
// ! FUNCTION: ro2om
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert rodrigues to orientation matrix
// !
// !> @param r Rodrigues vector (single precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ro2om(float* r, float* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: ro2om_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert rodrigues to orientation matrix
// !
// !> @param r Rodrigues vector (double precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ro2om(double* r, double* res);

// use local





// !--------------------------------------------------------------------------
// !
// ! FUNCTION: ro2qu
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert rodrigues to quaternion
// !
// !> @param r Rodrigues vector (single precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ro2qu(float* r, float* res);

// use local





// !--------------------------------------------------------------------------
// !
// ! FUNCTION: ro2qu_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert rodrigues to quaternion
// !
// !> @param r Rodrigues vector (double precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ro2qu(double* r, double* res);

// use local






// !--------------------------------------------------------------------------
// !
// ! FUNCTION: ho2eu
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert homochoric to euler
// !
// !> @param h homochoric coordinates (single precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ho2eu(float* h, float* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: ho2eu_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert homochoric to euler
// !
// !> @param h homochoric coordinates (double precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ho2eu(double* h, double* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: ho2om
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert homochoric to orientation matrix
// !
// !> @param h homochoric coordinates (single precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ho2om(float* h, float* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: ho2om_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert homochoric to orientation matrix
// !
// !> @param h homochoric coordinates (double precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ho2om(double* h, double* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: ho2ro
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert homochoric to Rodrigues
// !
// !> @param h homochoric coordinates (single precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ho2ro(float* h, float* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: ho2ro_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert homochoric to Rodrigues
// !
// !> @param h homochoric coordinates (double precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ho2ro(double* h, double* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: ho2qu
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert homochoric to quaternion
// !
// !> @param h homochoric coordinates (single precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ho2qu(float* h, float* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: ho2qu_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert homochoric to quaternion
// !
// !> @param h homochoric coordinates (double precision)
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ho2qu(double* h, double* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: eu2cu
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert euler angles to cubochoric
// !
// !> @param e euler angles (single precision)
// !>
// !
// !> @note calling program MUST have initialized the Lambert parameters first!!!
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::eu2cu(float* e, float* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: eu2cu_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert euler angles to cubochoric
// !
// !> @param e euler angles (double precision)
// !>
// !
// !> @note calling program MUST have initialized the Lambert parameters first!!!
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::eu2cu(double* e, double* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: om2cu
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert orientation matrix to cubochoric
// !
// !> @param o orientation matrix (single precision)
// !>
// !
// !> @note calling program MUST have initialized the Lambert parameters first!!!
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::om2cu(float* o, float* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: om2cu_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert orientation matrix to cubochoric
// !
// !> @param o orientation matrix (double precision)
// !>
// !
// !> @note calling program MUST have initialized the Lambert parameters first!!!
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::om2cu(double* o, double* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: ax2cu
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert axis angle to cubochoric
// !
// !> @param a axis angle (single precision)
// !>
// !
// !> @note calling program MUST have initialized the Lambert parameters first!!!
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ax2cu(float* a, float* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: ax2cu_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert axis angle to cubochoric
// !
// !> @param a axis angle (double precision)
// !>
// !
// !> @note calling program MUST have initialized the Lambert parameters first!!!
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ax2cu(double* a, double* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: ro2cu
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert Rodrigues to cubochoric
// !
// !> @param r Rodrigues (single precision)
// !>
// !
// !> @note calling program MUST have initialized the Lambert parameters first!!!
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ro2cu(float* r, float* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: ro2cu_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert Rodrigues to cubochoric
// !
// !> @param r Rodrigues (double precision)
// !>
// !
// !> @note calling program MUST have initialized the Lambert parameters first!!!
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::ro2cu(double* r, double* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: qu2cu
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert quaternion to cubochoric
// !
// !> @param q quaternion (single precision)
// !>
// !
// !> @note calling program MUST have initialized the Lambert parameters first!!!
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::qu2cu(float* q, float* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: qu2cu_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert quaternion to cubochoric
// !
// !> @param q quaternion (double precision)
// !>
// !
// !> @note calling program MUST have initialized the Lambert parameters first!!!
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::qu2cu(double* q, double* res);

// use local





// !--------------------------------------------------------------------------
// !
// ! FUNCTION: cu2eu
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert cubochoric to euler angles
// !
// !> @param c cubochoric coordinates (single precision)
// !>
// !
// !> @note calling program MUST have initialized the Lambert parameters first!!!
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::cu2eu(float* c, float* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: cu2eu_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert cubochoric to euler angles
// !
// !> @param c cubochoric coordinates (double precision)
// !>
// !
// !> @note calling program MUST have initialized the Lambert parameters first!!!
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::cu2eu(double* c, double* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: cu2om
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert cubochoric to orientation matrix
// !
// !> @param c cubochoric coordinates (single precision)
// !>
// !
// !> @note calling program MUST have initialized the Lambert parameters first!!!
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::cu2om(float* c, float* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: cu2om_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert cubochoric to orientation matrix
// !
// !> @param c cubochoric coordinates  (double precision)
// !>
// !
// !> @note calling program MUST have initialized the Lambert parameters first!!!
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::cu2om(double* c, double* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: cu2ax
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert cubochoric to axis angle
// !
// !> @param c cubochoric coordinates (single precision)
// !>
// !
// !> @note calling program MUST have initialized the Lambert parameters first!!!
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::cu2ax(float* c, float* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: cu2ax_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert cubochoric to axis angle
// !
// !> @param c cubochoric coordinates  (double precision)
// !>
// !
// !> @note calling program MUST have initialized the Lambert parameters first!!!
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::cu2ax(double* c, double* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: cu2ro
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert cubochoric to Rodrigues
// !
// !> @param c cubochoric coordinates (single precision)
// !>
// !
// !> @note calling program MUST have initialized the Lambert parameters first!!!
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::cu2ro(float* c, float* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: cu2ro_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert cubochoric to Rodrigues
// !
// !> @param c cubochoric coordinates  (double precision)
// !>
// !
// !> @note calling program MUST have initialized the Lambert parameters first!!!
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::cu2ro(double* c, double* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: cu2qu
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert cubochoric to quaternion
// !
// !> @param c cubochoric coordinates (single precision)
// !>
// !
// !> @note calling program MUST have initialized the Lambert parameters first!!!
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::cu2qu(float* c, float* res);

// use local




// !--------------------------------------------------------------------------
// !
// ! FUNCTION: cu2qu_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief convert cubochoric to quaternion
// !
// !> @param c cubochoric coordinates  (double precision)
// !>
// !
// !> @note calling program MUST have initialized the Lambert parameters first!!!
// !>
// !
// !> @date 8/12/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::cu2qu(double* c, double* res);

// use local





// !--------------------------------------------------------------------------
// !--------------------------------------------------------------------------
// ! routines for rotating a vector, tensor, ...
// !--------------------------------------------------------------------------
// !--------------------------------------------------------------------------

// !--------------------------------------------------------------------------
// !
// ! FUNCTION: RotVec_om
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief rotate a vector using a rotation matrix, active or passive (single precision)
// !
// !> @details This routine provides a way for the user to transform a vector
// !> and it returns the new vector components.  The user can use either a
// !> rotation matrix or a quaternion to define the transformation, and must
// !> also specifiy whether an active or passive result is needed.
// !>
// !
// !> @param vec input vector components (single precision)
// !> @param om orientation matrix (single precision)
// !> @param ap active/passive switch
// !>
// !
// !> @date 8/18/14   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::RotVec_om(float* vec,om,ap, float* res);

// use local





// !--------------------------------------------------------------------------
// !
// ! FUNCTION: RotVec_om_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief rotate a vector using a rotation matrix, active or passive (double precision)
// !
// !> @details This routine provides a way for the user to transform a vector
// !> and it returns the new vector components.  The user can use either a
// !> rotation matrix or a quaternion to define the transformation, and must
// !> also specifiy whether an active or passive result is needed.
// !>
// !
// !> @param vec input vector components (double precision)
// !> @param om orientation matrix (double precision)
// !> @param ap active/passive switch
// !>
// !
// !> @date 8/18/14   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::RotVec_om(double* vec,om,ap, double* res);

// use local






// !--------------------------------------------------------------------------
// !
// ! FUNCTION: RotVec_qu
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief rotate a vector using a quaternion, active or passive (single precision)
// !
// !> @details This routine provides a way for the user to transform a vector
// !> and it returns the new vector components.  The user can use either a
// !> rotation matrix or a quaternion to define the transformation, and must
// !> also specifiy whether an active or passive result is needed.
// !>
// !
// !> @param vec input vector components (single precision)
// !> @param qu quaternion (single precision)
// !> @param ap active/passive switch
// !>
// !
// !> @date 8/18/14   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::RotVec_qu(float* vec,qu,ap, float* res);

// use local
// use quaternions
// use constants








// !--------------------------------------------------------------------------
// !
// ! FUNCTION: RotVec_qu_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief rotate a vector using a quaternion, active or passive (double precision)
// !
// !> @details This routine provides a way for the user to transform a vector
// !> and it returns the new vector components.  The user can use either a
// !> rotation matrix or a quaternion to define the transformation, and must
// !> also specifiy whether an active or passive result is needed.
// !>
// !
// !> @param vec input vector components (double precision)
// !> @param qu quaternion (double precision)
// !> @param ap active/passive switch
// !>
// !
// !> @date 8/18/14   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::RotVec_qu(double* vec,qu,ap, double* res);

// use local
// use quaternions
// use constants







// !--------------------------------------------------------------------------
// !
// ! FUNCTION: RotTensor2_om
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief rotate a second rank tensor using a rotation matrix, active or passive (single precision)
// !
// !> @param tensor input tensor components (single precision)
// !> @param om orientation matrix (single precision)
// !> @param ap active/passive switch
// !>
// !
// !> @date 8/18/14   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::RotTensor2_om(float* tensor,om,ap, float* res);

// use local






// !--------------------------------------------------------------------------
// !
// ! FUNCTION: RotTensor2_om_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief rotate a second rank tensor using a rotation matrix, active or passive (double precision)
// !
// !> @param tensor input tensor components (double precision)
// !> @param om orientation matrix (double precision)
// !> @param ap active/passive switch
// !>
// !
// !> @date 8/18/14   MDG 1.0 original
// !--------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotationTransforms::RotTensor2_om(double* tensor,om,ap, double* res);

// use local






// !--------------------------------------------------------------------------
// !--------------------------------------------------------------------------
// ! and finally some printing routines, mostly used for debugging
// !--------------------------------------------------------------------------
// !--------------------------------------------------------------------------

// !--------------------------------------------------------------------------
// !
// ! SUBROUTINE: print_orientation
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief  prints a complete orientationtype record or a single entry
// !
// !> @param o orientationtype record
// !> @param outtype (optional) indicates which representation to print
// !> @param pretext (optional) up to 10 characters that will precede each line
// !
// !> @date  8/4/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// use local
// use io
// use constants

// IMPLICIT NONE











// ! print the entire record with all representations



// !--------------------------------------------------------------------------
// !
// ! SUBROUTINE: print_orientation_d
// !
// !> @author Marc De Graef, Carnegie Mellon University
// !
// !> @brief  prints a complete orientationtype record or a single entry (double precision)
// !
// !> @param o orientationtype record
// !> @param outtype (optional) indicates which representation to print
// !> @param pretext (optional) up to 10 characters that will precede each line
// !
// !> @date  8/4/13   MDG 1.0 original
// !--------------------------------------------------------------------------

// use local
// use io
// use constants

// IMPLICIT NONE











// ! print the entire record with all representations






};
#endif /* _RotationTransforms_H_ */
