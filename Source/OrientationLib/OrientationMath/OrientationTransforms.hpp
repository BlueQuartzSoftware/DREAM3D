/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _OrientationTransforms_H_
#define _OrientationTransforms_H_

#include <assert.h>     /* assert */

#include <string>
#include <iostream>
#include <algorithm>
#include <complex>

#if __APPLE__
#include <Accelerate/Accelerate.h>
#endif


#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Eigen>


#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Math/MatrixMath.h"
#include "SIMPLib/Math/QuaternionMath.hpp"


#include "OrientationLib/OrientationLib.h"
#include "OrientationLib/OrientationLibConstants.h"
#include "OrientationLib/OrientationMath/OrientationArray.hpp"
#include "OrientationLib/Utilities/OrientationMathHelpers.hpp"
#include "OrientationLib/Utilities/ModifiedLambertProjection3D.hpp"


/* This comment block is commented as Markdown. if you paste this into a text
 * editor then render it with a Markdown aware system a nice table should show
 * up for you.

 ## Function Mapping Check List ##

#### Master Table of Conversions ####

| From/To |  e   |  o   |  a   |  r   |  q   |  h   |  c   |
|  -      |  -   |  -   |  -   |  -   |  -   |  -   |  -   |
|  e      |  -   |  X   |  X   |  X   |  X   |  a   | ah   |
|  o      |  X   |  --  |  X   |  e   |  X   |  a   | ah   |
|  a      |  o   |  X   | --   |  X   |  X   |  X   |  h   |
|  r      |  o   |  a   |  X   | --   |  a   |  X   |  h   |
|  q      |  X   |  X   |  X   |  X   | --   |  X   |  h   |
|  h      |  ao  |  a   |  X   |  a   |  a   | --   |  X   |
|  c      | hao  |  ha  |  h   |  ha  | ha   |  X   | --   |


#### DREAM3D Implemented ####


| From/To |  e   |  o   |  a   |  r   |  q   |  h   |  c   |
|  -      |  -   |  -   |  -   |  -   |  -   |  -   |  -   |
|  e      |  #   |  X   |  X   |  X   |  X   |  a   |  X   |
|  o      |  X   |  #   |  @   |  X   |  X   |  a   |  -   |
|  a      |  X   |  X   |  #   |  X   |  X   |  X   |  -   |
|  r      |  X   |  X   |  X   |  #   |  X   |  X   |  -   |
|  q      |  X   |  X   |  X   |  X   |  #   |  X   |  -   |
|  h      |  X   |  X   |  X   |  X   |  X   |  #   |  X   |
|  c      |  -   |  -   |  -   |  -   |  -   |  X   |  #   |

@: Will use LAPACK _sgeev routine On OS X, otherwise will use Eigen Library

*/


/**
* The Orientation codes are written in such a way that the value of -1 indicates
* an Active Rotation and +1 indicates a passive rotation.
*
* DO NOT UNDER ANY CIRCUMSTANCE CHANGE THESE VARIABLES. THERE WILL BE BAD
* CONSEQUENCES IF THESE ARE CHANGED. EVERY PIECE OF CODE THAT RELIES ON THESE
* FUNCTIONS WILL BREAK. YOU HAVE BEEN WARNED.
*
* Adam  Morawiec's book uses Passive rotations.
**/
#define DREAM3D_ACTIVE_ROTATION               -1.0
#define DREAM3D_PASSIVE_ROTATION               1.0

#define DREAM3D_ROTATION_CONVENTION(value)\
  static const float epsijk = static_cast<float>(value);\
  static const double epsijkd = value;\


  namespace Rotations
  {
    namespace Constants
    {

      DREAM3D_ROTATION_CONVENTION( DREAM3D_PASSIVE_ROTATION )
    }
  }


// Add some shortened namespace alias
// Condense some of the namespaces to same some typing later on.
  namespace LPs = LambertParametersType;
  namespace RConst = Rotations::Constants;
  namespace DConst = SIMPLib::Constants;


  /**
   * @brief The OrientationTransforms class
   * template parameter T can be one of std::vector<T>, QVector<T> or OrientationArray<T>
   * and template parameter K is the type specified in T. For example if T is std::vector<float>
   * then K is float.
   */
  template<typename T, typename K>
  class OrientationTransforms
  {
    public:

      typedef OrientationTransforms SelfType;
      virtual ~OrientationTransforms() {}

      typedef OrientationMathHelpers<T, K> OMHelperType;
      typedef struct
      {
        int result;
        std::string msg;
      } ResultType;


      void FatalError(const std::string& func, const std::string& msg)
      {
        std::cout << func << "::" << msg << std::endl;
      }
      /* ###################################################################
      * Copyright (c) 2013-2014, Marc De Graef/Carnegie Mellon University
      * All rights reserved.
      *
      * Redistribution and use in source and binary forms, with or without modification, are
      * permitted provided that the following conditions are met:
      *
      *     - Redistributions of source code must retain the above copyright notice, this list
      *        of conditions and the following disclaimer.
      *     - Redistributions in binary form must reproduce the above copyright notice, this
      *        list of conditions and the following disclaimer in the documentation and/or
      *        other materials provided with the distribution.
      *     - Neither the names of Marc De Graef, Carnegie Mellon University nor the names
      *        of its contributors may be used to endorse or promote products derived from
      *        this software without specific prior written permission.
      *
      * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
      * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
      * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
      * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
      * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
      * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
      * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
      * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
      * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
      * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
      * ###################################################################
      CTEMsoft2013:rotations.f90

      * MODULE: rotations
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief everything that has to do with rotations and conversions between rotations
      *
      * @details This file relies a lot on the relations listed in the book "Orientations
      * and Rotations" by Adam Morawiec [Springer 2004].  I've tried to implement every
      * available representation for rotations in a way that makes it easy to convert
      * between any pair.  Needless to say, this needs extensive testing and debugging...
      *
      * Instead of converting all the time between representations, I've opted to
      * "waste" a little more memory and time and provide the option to precompute all the representations.
      * This way all representations are available via a single data structure.
      *
      * Obviously, the individual conversion routines also exist and can be called either in
      * single or in double precision (using a function interface for each call, so that only
      * one function name is used).  The conversion routines use the following format for their
      * call name:  ab2cd, where (ab and cd are two-characters strings selected from the following
      * possibilities: [the number in parenthesis lists the number of entries that need to be provided]
      *
      * eu : euler angle representation (3)
      * om : orientation matrix representation (3x3)
      * ax : axis angle representation (4)
      * ro : Rodrigues vector representation (3)
      * qu : unit quaternion representation (4)
      * ho : homochoric representation (3)
      * cu : cubochoric representation (3).
      *
      * hence, conversion from homochoric to euler angle is called as ho2eu(); the argument of
      * each routine must have the correct number of dimensions and entries.
      * All 42 conversion routines exist in both single and double precision.
      *
      * Some routines were modified in July 2014, to simplify the paths in case the direct conversion
      * routine does not exist.  Given the complexity of the cubochoric transformations, all routines
      * going to and from this representation will require at least one and sometimes two or three
      * intermediate representations.  cu2eu and qu2cu currently represent the longest computation
      * paths with three intermediate steps each.
      *
      * In August 2014, all routines were modified to account for active vs. passive rotations,
      * after some inconsistencies were discovered that could be traced back to that distinction.
      * The default is for a rotation to be passive, and only those transformation rules have been
      * implemented.  For active rotations, the user needs to explicitly take action in the calling
      * program by setting the correct option in the ApplyRotation function.
      *
      * Testing: the program rotationtest.f90 was generated by an IDL script and contains all possible
      * pairwise and triplet transformations, using a series of input angle combinations; for now, these
      * are potentially problematic Euler combinations.
      *
      * The conventions of this module are:
      *
      * - all reference frames are right-handed and orthonormal (except for the Bravais frames)
      * - a rotation angle is positive for a counterclockwise rotation when viewing along the positive rotation axis towards the origin
      * - all rotations are interpreted in the passive way
      * - Euler angles follow the Bunge convention, with phi1 in [0,2pi], Phi in [0,pi], and phi2 in [0,2pi]
      * - rotation angles (in axis-angle derived representations) are limited to the range [0,pi]
      *
      * To make things easier for the user, this module provides a routine to create a rotation
      * representation starting from an axis, described by a unit axis vector, and a rotation angle.
      * This routine properly takes the sign of epsijk into account, and always produces a passive rotation.
      * The user must explicitly take action to interpret a rotation a being active.
      *
      * @date 08/04/13 MDG 1.0 original
      * @date 07/08/14 MDG 2.0 modifications to several routines (mostly simplifications)
      * @date 08/08/14 MDG 3.0 added active/passive handling (all routines passive)
      * @date 08/11/14 MDG 3.1 modified Rodrigues vector to 4 components (n and length) to accomodate Infinity
      * @date 08/18/14 MDG 3.2 added RotateVector, RotateTensor2 routines with active/passive switch
      * @date 08/20/14 MDG 3.3 completed extensive testing of epsijk<0 mode; all tests passed for the first time !
      * @date 08/21/14 MDG 3.4 minor correction in om2ax to get things to work for epsijk>0 mode; all tests passed!
      * @date 09/30/14 MDG 3.5 added routines to make rotation definitions easier
      * @date 09/30/14 MDG 3.6 added strict range checking routines for all representations (tested on 10/1/14)
      //--------------------------------------------------------------------------
      //--------------------------------
      * routines to check the validity range of rotation representations
      //--------------------------------
      * general rotation creation routine, to make sure that a rotation representation is
      * correctly initialized, takes an axis and an angle as input, returns an orientationtype structure
      * general interface routine to populate the orientation type
      //--------------------------------
      * convert Euler angles to 3x3 orientation matrix
      * convert Euler angles to axis angle
      * convert Euler angles to Rodrigues vector
      * convert Euler angles to quaternion
      * convert Euler angles to homochoric
      * convert Euler angles to cubochoric
      //--------------------------------
      * convert 3x3 orientation matrix to Euler angles
      * convert 3x3 orientation matrix to axis angle
      * convert 3x3 orientation matrix to Rodrigues
      * convert 3x3 rotation matrix to quaternion
      * convert 3x3 rotation matrix to homochoric
      * convert 3x3 rotation matrix to cubochoric
      //--------------------------------
      * convert axis angle pair to euler
      * convert axis angle pair to orientation matrix
      * convert axis angle pair to Rodrigues
      * convert axis angle pair to quaternion
      * convert axis angle pair to homochoric representation
      * convert axis angle pair to cubochoric
      //--------------------------------
      * convert Rodrigues vector to Euler angles
      * convert Rodrigues vector to orientation matrix
      * convert Rodrigues vector to axis angle pair
      * convert Rodrigues vector to quaternion
      * convert Rodrigues vector to homochoric
      * convert Rodrigues vector to cubochoric
      //--------------------------------
      * convert quaternion to Euler angles
      * convert quaternion to orientation matrix
      * convert quaternion to axis angle
      * convert quaternion to Rodrigues
      * convert quaternion to homochoric
      * convert quaternion to cubochoric
      //--------------------------------
      * convert homochoric to euler
      * convert homochoric to orientation matrix
      * convert homochoric to axis angle pair
      * convert homochoric to Rodrigues
      * convert homochoric to quaternion
      * convert homochoric to cubochoric
      //--------------------------------
      * convert cubochoric to euler
      * convert cubochoric to orientation matrix
      * convert cubochoric to axis angle
      * convert cubochoric to Rodrigues
      * convert cubochoric to quaternion
      * convert cubochoric to homochoric
      * apply a rotation to a vector
      * apply a rotation to a second rank tensor
      //--------------------------------
      * print quaternion and equivalent 3x3 rotation matrix
      */


      /**: eu_check
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief verify that the Euler angles are in the correct respective ranges
      *
      * @param eu 3-component vector
      *
      *
      * @date 9/30/14   MDG 1.0 original
      */
      static ResultType eu_check(const T& eu)
      {
        ResultType res;
        res.result = 1;

        if ((eu[0] < 0.0) || (eu[0] > (SIMPLib::Constants::k_2Pi)))
        {
          res.msg = "rotations:eu_check:: phi1 Euler angle outside of valid range [0,2pi]";
          res.result = -1;
        }
        if ((eu[1] < 0.0) || (eu[1] > SIMPLib::Constants::k_Pi))
        {
          res.msg = "rotations:eu_check:: phi Euler angle outside of valid range [0,pi]";
          res.result = -2;
        }
        if ((eu[2] < 0.0) || (eu[2] > (SIMPLib::Constants::k_2Pi)))
        {
          res.msg = "rotations:eu_check:: phi2 Euler angle outside of valid range [0,2pi]";
          res.result = -3;
        }
        return res;
      }



      /**: ro_check
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief verify that the Rodrigues vector has positive length and unit axis vector
      *
      * @param ro 4-component vector ( <v0, v1, v2>, L )
      *
      *
      * @date 9/30/14   MDG 1.0 original
      */
      static ResultType ro_check(const T& ro)
      {
        K eps = static_cast<K>(1.0E-6L);
        ResultType res;
        res.result = 1;
        if (ro[3] < 0.0L)
        {
          res.msg = "rotations:ro_check:: Rodrigues-Frank vector has negative length: ";
          res.result = -1;
          return res;
        }
        T out = OMHelperType::multiply(ro, ro, 3);
        K ttl = OMHelperType::sum(out);
        ttl = sqrt(ttl);
        if (fabs(ttl - 1.0L) > eps)
        {
          res.msg = "rotations:ro_check:: Rodrigues-Frank axis vector not normalized";
          res.result = -2;
        }
        return res;
      }


      /**: ho_check
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief verify that the homochoric vector is inside or on the homochoric ball
      *
      * @param ho 3-component vector
      *
      *
      * @date 9/30/14   MDG 1.0 original
      */
      static ResultType ho_check(const T& ro)
      {
        ResultType res;
        res.result = 1;
        T out = OMHelperType::multiply(ro, ro);
        K ttl = OMHelperType::sum(out);
        K r = sqrt(ttl);
        if (r > static_cast<float>(LPs::R1))
        {
          res.msg = "rotations:ho_check: homochoric vector outside homochoric ball";
          res.result = -1;
        }
        return res;
      }

      /**: cu_check
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief verify that the cubochoric vector is inside or on the cube
      *
      * @param cu 3-component vector
      *
      *
      * @date 9/30/14   MDG 1.0 original
      */
      static ResultType cu_check(const T& cu)
      {
        ResultType res;
        res.result = 1;

        K r = OMHelperType::maxval(OMHelperType::absValue(cu));
        if (r > static_cast<float>(LPs::ap / 2.0))
        {
          res.msg = "rotations:cu_check: cubochoric vector outside cube";
          res.result = -1;
        }
        return res;
      }

      /**: qu_check
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief verify that the quaternion has unit length and positive scalar part
      *
      * @param qu 4-component vector (w, <v0, v1, v2> )
      *
      *
      * @date 9/30/14   MDG 1.0 original
      */
      static ResultType qu_check(const T& qu, typename QuaternionMath<K>::Order layout = QuaternionMath<K>::QuaternionVectorScalar)
      {
        size_t w = 0, x = 1, y = 2, z = 3;
        if(layout == QuaternionMath<K>::QuaternionVectorScalar)
        {
          w = 3;
          x = 0;
          y = 1;
          z = 2;
        }

        ResultType res;
        res.result = 1;

        if (qu[w] < 0.0)
        {
          res.msg = "rotations:qu_check: quaternion must have positive scalar part";
          res.result = -1;
          return res;
        }

        K eps = std::numeric_limits<K>::epsilon();
        T out = OMHelperType::multiply(qu, qu);
        K r = sqrt(OMHelperType::sum(out));
        if (fabs(r - 1.0) > eps)
        {
          res.msg = "rotations:qu_check: quaternion must have unit norm";
          res.result = -2;
        }
        return res;
      }


      /**: ax_check
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief verify that the axis angle pair has a unit vector and angle in the correct range
      *
      * @param ax 4-component vector (<ax0, ax1, ax2>, angle )
      *
      *
      * @date 9/30/14   MDG 1.0 original
      */
      static ResultType ax_check(const T& ax)
      {
        ResultType res;
        res.result = 1;
        if ((ax[3] < 0.0) || (ax[3] > SIMPLib::Constants::k_Pi))
        {
          res.msg = "rotations:ax_check: angle must be in range [0,pi]";
          res.result = -1;
          return res;
        }
        K eps = std::numeric_limits<K>::epsilon();
        T out = OMHelperType::multiply(ax, ax, 3);
        K r = sqrt(OMHelperType::sum(out));
        K absv = fabs(r - 1.0);
        if (absv > eps)
        {
          res.msg = "rotations:ax_check: axis-angle axis vector must have unit norm";
          res.result = -2;
        }
        return res;
      }


      /**: om_check
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief verify that the rotation matrix is actually a proper rotation matrix
      *
      * @param om 3x3-component matrix
      *
      *
      * @date 9/30/14   MDG 1.0 original
      */
      static ResultType om_check(const T& om)
      {
        ResultType res;
        res.result = 1;
        K threshold = static_cast<K>(1.0E-5L);

//        typedef Eigen::Matrix<K, 3, 3, Eigen::RowMajor> OrientationMatrixType;
//        OrientationMatrixType omE;
//        int i = 0;
//        for(int c = 0; c < 3; c++)
//        {
//          for(int r = 0; r < 3; r++)
//          {
//            omE(r, c) = om[i++];
//          }
//        }
        
        typedef Eigen::Matrix<K, 3, 3, Eigen::RowMajor> Matrix_t;
        
        Eigen::Map<Matrix_t> omE(const_cast<K*>(&(om[0])));
        
        
        K det = omE.determinant();
        
        std::stringstream ss;
        if (det < 0.0)
        {
          ss << "rotations:om_check: Determinant of rotation matrix must be positive: " << det;
          res.msg = ss.str();
          res.result = -1;
          return res;
        }
        
        K r = fabs(det - static_cast<K>(1.0L));
        if (!OMHelperType::closeEnough(r, 0.0L, threshold))
        {
          ss <<"rotations:om_check: Determinant (" << det << ") of rotation matrix must be unity (1.0)";
          res.msg = ss.str();
          res.result = -2;
          return res;
        }
        
        Matrix_t abv = (omE * omE.transpose()).cwiseAbs();
        
        Matrix_t identity;
        identity.setIdentity();
        
        identity = identity-abv;
        identity = identity.cwiseAbs();
        
        for(int c = 0; c < 3; c++)
        {
          for(int r = 0; r < 3; r++)
          {
            if(identity(r, c) > threshold)
            {
              std::stringstream ss;
              ss << "rotations:om_check: rotation matrix times transpose must be identity matrix: (";
              ss << r << ", " << c << ") = " << abv(r,c);
              res.msg = ss.str();
              res.result = -3;
            }
          }
        }

        return res;
      }

#if 0
      /**: genrot
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief generate a passive rotation representation, given the unit axis vector and the rotation angle
      *
      * @param av 3-component vector
      * @param omega rotation angle (radians)
      *
      *
      * @date 9/30/14   MDG 1.0 original
      */
      static void genrot(const T& av, K omega, T& res)
      {
        //*** use local
        //*** use constants
        //*** use error
        //*** IMPLICIT NONE
        //real(kind=sgl),INTENT(IN)       :: av[2]
        //real(kind=sgl),INTENT(IN)       :: omega
        type(orientationtype)           :: res;
        K axang[4];
        K s;

        if ((omega < 0.0) || (omega > M_PI))
        {
          assert(false);
        }

        axang[0] = -RConst::epsijk * av[0];
        axang[1] = -RConst::epsijk * av[1];
        axang[2] = -RConst::epsijk * av[2];
        axang[3] = omega;
        s = sqrt(sumofSquares(av));

        if (s != 0.0)
        {
          axang[0] = axang[0] / s;
          axang[1] = axang[1] / s;
          axang[2] = axang[2] / s;
        }
        else
        {
          assert(false);
        }
        init_orientation(axang, 'ax', res);
      }



      /**: init_orientation
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief take an orientation representation with 3 components and init all others
      *
      * @param orient 3-component vector
      * @param intype input type ['eu', 'ro', 'ho', 'cu']
      * @param rotcheck  optional parameter to enforce strict range checking
      *
      * @date 8/04/13   MDG 1.0 original
      * @date 9/30/14   MDG 1.1 added testing of valid ranges
      */

      static void init_orientation(const T& orient, char intype[2], bool rotcheck, T& res)
      {

      }

      /**: init_orientation_om
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief take an orientation representation with 3x3 components and init all others
      *
      * @param orient r-component vector
      * @param intype input type ['om']
      * @param rotcheck  optional parameter to enforce strict range checking
      *
      *
      * @date 8/04/13   MDG 1.0 original
      */

      void init_orientation_om(float* orient, char intype[2], bool rotcheck, float* res);
#endif

      /**: eu2om
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief Euler angles to orientation matrix  [Morawiec, page 28]
      *
      * @param e 3 Euler angles in radians
      *
      *
      * @date 8/04/13   MDG 1.0 original
      * @data 7/23/14   MDG 1.1 verified
      */
      static void eu2om(const T& e, T& res)
      {
        // K eps = std::numeric_limits<K>::epsilon();
        K eps = 1.0E-7f;

        K c1 = cos(e[0]);
        K c = cos(e[1]);
        K c2 = cos(e[2]);
        K s1 = sin(e[0]);
        K s = sin(e[1]);
        K s2 = sin(e[2]);
        res[0] = c1 * c2 - s1 * s2 * c;
        res[1] = s1 * c2 + c1 * s2 * c;
        res[2] = s2 * s;
        res[3] = -c1 * s2 - s1 * c2 * c;
        res[4] = -s1 * s2 + c1 * c2 * c;
        res[5] = c2 * s;
        res[6] = s1 * s;
        res[7] = -c1 * s;
        res[8] = c;
        for(size_t i = 0; i < 9; i++)
        {
          if(fabs(res[i]) < eps) { res[i] = 0.0; }
        }
      }

      /**: eu2ax
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert euler to axis angle
      *
      * @param e 3 euler angles
      *
      *
      * @date 8/12/13   MDG 1.0 original
      * @date 7/23/14   MDG 2.0 explicit implementation
      * @date 7/23/14   MDG 2.1 exception for zero rotation angle
      */
      static void eu2ax(const T& e, T& res)
      {
        K thr = static_cast<K>(1.0E-6);
        K alpha = 0.0f;
        K t = tan(e[1] * 0.5);
        K sig = 0.5 * (e[0] + e[2]);
        K del = 0.5 * (e[0] - e[2]);
        K tau = sqrt(t * t + sin(sig) * sin(sig));
        if (sig == SIMPLib::Constants::k_PiOver2)    //Infinity
        {
          alpha = SIMPLib::Constants::k_Pi;
        }
        else
        {
          alpha = 2.0 * atan(tau / cos(sig)); //! return a default identity axis-angle pair
        }

        if (fabs(alpha) < thr)
        {
          res[0] = 0.0;
          res[1] = 0.0;
          res[2] = 1.0;
          res[3] = 0.0;
        }
        else
        {
          //! passive axis-angle pair so a minus sign in front
          res[0] = -RConst::epsijkd * t * cos(del) / tau;
          res[1] = -RConst::epsijkd * t * sin(del) / tau;
          res[2] = -RConst::epsijkd * sin(sig) / tau;
          res[3] = alpha;

          if (alpha < 0.0)
          {
            res[0] = -res[0];
            res[1] = -res[1];
            res[2] = -res[2];
            res[3] = -res[3];
          }
        }
      }

      /**: eu2ro
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief Euler angles to Rodrigues vector  [Morawiec, page 40]
      *
      * @param e 3 Euler angles in radians
      *
      *
      * @date 8/04/13   MDG 1.0 original
      */
      static void eu2ro(const T& e, T& res)
      {
        K thr = 1.0E-6f;

        SelfType::eu2ax(e, res);
        K t = res[3];
        if (fabs(t - SIMPLib::Constants::k_Pi) < thr)
        {
          res[3] = std::numeric_limits<K>::infinity();
          return;
        }
        if (t == 0.0)
        {
          res[0] = 0.0;
          res[1] = 0.0;
          res[2] = 0.0;
          res[3] = 0.0;
        }
        else
        {
          res[3] = tan(t * 0.5);
        }
      }

      /**: eu2qu
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief Euler angles to quaternion  [Morawiec, page 40]
      *
      * @note verified 8/5/13
      *
      * @param e 3 Euler angles in radians
      * @param Quaternion can be of form Scalar<Vector> or <Vector>Scalar in memory. The
      * default is (Scalar, <Vector>)
      *
      * @date 8/04/13   MDG 1.0 original
      * @date 8/07/14   MDG 1.1 verified
      */

      static void eu2qu(const T& e, T& res, typename QuaternionMath<K>::Order layout = QuaternionMath<K>::QuaternionVectorScalar)
      {
        size_t w = 0, x = 1, y = 2, z = 3;
        if(layout == QuaternionMath<K>::QuaternionVectorScalar)
        {
          w = 3;
          x = 0;
          y = 1;
          z = 2;
        }
        K ee[3] = { 0.0f, 0.0f, 0.0f};
        K cPhi = 0.0f;
        K cp = 0.0f;
        K cm = 0.0f;
        K sPhi = 0.0f;
        K sp = 0.0f;
        K sm = 0.0f;

        ee[0] = 0.5 * e[0];
        ee[1] = 0.5 * e[1];
        ee[2] = 0.5 * e[2];

        cPhi = cos(ee[1]);
        sPhi = sin(ee[1]);
        cm = cos(ee[0] - ee[2]);
        sm = sin(ee[0] - ee[2]);
        cp = cos(ee[0] + ee[2]);
        sp = sin(ee[0] + ee[2]);
        res[w] = cPhi * cp;
        res[x] = -RConst::epsijk * sPhi * cm;
        res[y] = -RConst::epsijk * sPhi * sm;
        res[z] = -RConst::epsijk * cPhi * sp;

//        if(OMHelperType::closeEnough(res[w], 0.0, thr))
//        {
//          if(OMHelperType::closeEnough(fabs(res[x]), 1.0, thr) && OMHelperType::closeEnough(res[y], 0.0, thr) && OMHelperType::closeEnough(res[z], 0.0, thr) )
//          {
//          res[w] = 0.0;
//          res[x] = 1.0;
//          res[y] = 0.0;
//          res[z] = 0.0;
//          }
//          if(OMHelperType::closeEnough(res[x], 0.0, thr) && OMHelperType::closeEnough(fabs(res[y]), 1.0, thr) && OMHelperType::closeEnough(res[z], 0.0, thr) )
//          {
//          res[w] = 0.0;
//          res[x] = 0.0;
//          res[y] = 1.0;
//          res[z] = 0.0;
//          }
//          if(OMHelperType::closeEnough(res[x], 0.0, thr) && OMHelperType::closeEnough(res[y], 0.0, thr) && OMHelperType::closeEnough(fabs(res[z]), 1.0, thr) )
//          {
//          res[w] = 0.0;
//          res[x] = 0.0;
//          res[y] = 0.0;
//          res[z] = 1.0;
//          }
//        }  

        if (
        //  !OMHelperType::closeEnough(res[w], 0.0, 1.0E-5) &&
           res[w] < 0.0
          )
        {
          res[w] = -res[w];
          res[x] = -res[x];
          res[y] = -res[y];
          res[z] = -res[z];
        }
      }



      /**: om2eu
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief orientation matrix to euler angles
      *
      * @note verified 8/19/14 using Mathematica
      *
      * @param o orientation matrix
      * @param res Euler Angles
      *
      * @date 8/04/13   MDG 1.0 original
      * @date 8/19/14   MDG 1.1 verification using Mathematica
      */
      static void om2eu(const T& o, T& res)
      {

        K zeta = 0.0;
        bool close = OMHelperType::closeEnough(std::fabs(o[8]), 1.0f, 1.0E-6f);
        if(!close)
        {
          res[1] = acos(o[8]);
          zeta = 1.0 / sqrt(1.0 - o[8] * o[8]);
          res[0] = atan2(o[6] * zeta, -o[7] * zeta);
          res[2] = atan2(o[2] * zeta, o[5] * zeta);
        }
        else
        {
          close = OMHelperType::closeEnough(o[8], 1.0f, 1.0E-6f);
          if (close)
          {
            res[0] = atan2( o[1], o[0]);
            res[1] = 0.0;
            res[2] = 0.0;
          }
          else
          {
            res[0] = -atan2(-o[1], o[0]);
            res[1] = SIMPLib::Constants::k_Pi;
            res[2] = 0.0;
          }
        }

        if (res[0] < 0.0)
        {
          res[0] = fmod(res[0] + 100.0 * DConst::k_Pi, DConst::k_2Pi);
        }
        if (res[1] < 0.0)
        {
          res[1] = fmod(res[1] + 100.0 * DConst::k_Pi, DConst::k_Pi);
        }
        if (res[2] < 0.0)
        {
          res[2] = fmod(res[2] + 100.0 * DConst::k_Pi, DConst::k_2Pi);
        }
      }

      /**: ax2om
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief Axis angle pair to orientation matrix
      *
      * @note verified 8/5/13.
      *
      * @param a axis angle pair
      *
      *
      * @date 8/04/13   MDG 1.0 original
      */
      static void ax2om(const T& a, T& res)
      {
        K q = 0.0L;
        K c = 0.0L;
        K s = 0.0L;
        K omc = 0.0L;

        c = cos(a[3]);
        s = sin(a[3]);

        omc = 1.0 - c;

        res[0] = a[0] * a[0] * omc + c;
        res[4] = a[1] * a[1] * omc + c;
        res[8] = a[2] * a[2] * omc + c;
        int _01 = 1;
        int _10 = 3;
        int _12 = 5;
        int _21 = 7;
        int _02 = 2;
        int _20 = 6;
        // Check to see if we need to transpose
        if (Rotations::Constants::epsijk == 1.0L) {
          _01 = 3;
          _10 = 1;
          _12 = 7;
          _21 = 5;
          _02 = 6;
          _20 = 2;
        }

        q = omc * a[0] * a[1];
        res[_01] = q + s * a[2];
        res[_10] = q - s * a[2];
        q = omc * a[1] * a[2];
        res[_12] = q + s * a[0];
        res[_21] = q - s * a[0];
        q = omc * a[2] * a[0];
        res[_02] = q - s * a[1];
        res[_20] = q + s * a[1];

      }



      /**: qu2eu
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief Quaternion to Euler angles  [Morawiec page 40, with errata !!!! ]
      *
      * @param q quaternion
      *
      *
      * @date 8/04/13   MDG 1.0 original
      */
      static void qu2eu(const T& q, T& res, typename QuaternionMath<K>::Order layout = QuaternionMath<K>::QuaternionVectorScalar)
      {
        size_t w = 0, x = 1, y = 2, z = 3;
        if(layout == QuaternionMath<K>::QuaternionVectorScalar)
        {
          w = 3;
          x = 0;
          y = 1;
          z = 2;
        }

        T qq(4);
        K q12 = 0.0f;
        K q03 = 0.0f;
        K chi = 0.0f;
        K Phi = 0.0f;
        K phi1 = 0.0f;
        K phi2 = 0.0f;

        qq = q;

        q03 = qq[w] * qq[w] + qq[z] * qq[z];
        q12 = qq[x] * qq[x] + qq[y] * qq[y];
        chi = sqrt(q03 * q12);
        if (chi == 0.0)
        {
          if (q12 == 0.0)
          {
            if (RConst::epsijk == 1.0)
            {
              Phi = 0.0;
              phi2 = 0.0;                //arbitrarily due to degeneracy
              phi1 = atan2(-2.0 * qq[w] * qq[z], qq[w] * qq[w] - qq[z] * qq[z]);
            }
            else
            {
              Phi = 0.0;
              phi2 = 0.0;                //arbitrarily due to degeneracy
              phi1 = atan2( 2.0 * qq[w] * qq[z], qq[w] * qq[w] - qq[z] * qq[z]);
            }
          }
          else
          {
            Phi = SIMPLib::Constants::k_Pi;
            phi2 = 0.0;                //arbitrarily due to degeneracy
            phi1 = atan2(2.0 * qq[x] * qq[y], qq[x] * qq[x] - qq[y] * qq[y]);
          }
        }
        else
        {
          if (RConst::epsijk == 1.0)
          {
            Phi = atan2( 2.0 * chi, q03 - q12 );
            chi = 1.0 / chi;
            phi1 = atan2((-qq[w] * qq[y] + qq[x] * qq[z]) * chi , (-qq[w] * qq[x] - qq[y] * qq[z]) * chi );
            phi2 = atan2( (qq[w] * qq[y] + qq[x] * qq[z]) * chi, (-qq[w] * qq[x] + qq[y] * qq[z]) * chi );
          }
          else
          {
            Phi = atan2( 2.0 * chi, q03 - q12 );
            chi = 1.0 / chi;
            K y1 = (qq[w] * qq[y] + qq[x] * qq[z]) * chi;
            K x1 = (qq[w] * qq[x] - qq[y] * qq[z]) * chi;
            phi1 = atan2(y1, x1 );
            y1 = (-qq[w] * qq[y] + qq[x] * qq[z]) * chi;
            x1 = (qq[w] * qq[x] + qq[y] * qq[z]) * chi;
            phi2 = atan2( y1, x1);
          }
        }

        res[0] = phi1;
        res[1] = Phi;
        res[2] = phi2;


        if (res[0] < 0.0)
        {
          res[0] = fmod(res[0] + 100.0 * DConst::k_Pi, DConst::k_2Pi);
        }
        if (res[1] < 0.0)
        {
          res[1] = fmod(res[1] + 100.0 * DConst::k_Pi, DConst::k_Pi);
        }
        if (res[2] < 0.0)
        {
          res[2] = fmod(res[2] + 100.0 * DConst::k_Pi, DConst::k_2Pi);
        }
      }

      /**: ax2ho
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief Axis angle pair to homochoric
      *
      * @param a axis-angle pair
      *
      * !
      * @date 8/04/13   MDG 1.0 original
      */
      static void ax2ho(const T& a, T& res)
      {
        K f = 0.75 * ( a[3] - sin(a[3]) );
        f = pow(f, (1.0 / 3.0));
        res[0] = a[0] * f;
        res[1] = a[1] * f;
        res[2] = a[2] * f;
      }

      /**: ho2ax
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief Homochoric to axis angle pair
      *
      * @param h homochoric coordinates
      *
      *
      *
      * @date 8/04/13  MDG 1.0 original
      * @date 07/21/14 MDG 1.1 double precision fit coefficients
      */
      static void ho2ax(const T& h, T& res)
      {
        K thr = 1.0E-8f;

        K hmag = OMHelperType::sumofSquares(h);
        if (hmag == 0.0)
        {
          res[0] = 0.0;
          res[1] = 0.0;
          res[2] = 1.0;
          res[3] = 0.0;
        }
        else
        {
          K hm = hmag;
          T hn = h;
          K sqrRtHMag = 1.0 / sqrt(hmag);
          OMHelperType::scalarMultiply(hn, sqrRtHMag); // In place scalar multiply
          K s = LPs::tfit[0] + LPs::tfit[1] * hmag;
          for(int i = 2; i < 16; i++)
          {
            hm = hm * hmag;
            s = s + LPs::tfit[i] * hm;
          }
          s = 2.0 * acos(s);
          res[0] = hn[0];
          res[1] = hn[1];
          res[2] = hn[2];
          K delta = std::fabs(s - SIMPLib::Constants::k_Pi);
          if ( delta < thr)
          {
            res[3] = SIMPLib::Constants::k_Pi;
          }
          else
          {
            res[3] = s;
          }
        }
      }

      /**: om2ax
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert orientation matrix to axis angle
      *
      * @details this assumes that the matrix represents a passive rotation.
      *
      * @param om 3x3 orientation matrix
      *
      *
      * @date 8/12/13  MDG 1.0 original
      * @date 07/08/14 MDG 2.0 replaced by direct solution
      */
    

    static void om2ax(const T& in, T& res)
    {
      // Transpose array to be Column Major Format for the BLAS/LAPACK routine
      T om = OMHelperType::transpose(in);
      // Compute the Angle based on the "Trace" of the Rotation Matrix
      K t = 0.50 * (om[0] + om[4] + om[8] - 1.0);
      if (t > 1.0) { t = 1.0; }
      if (t < -1.0) {t = -1.0; }
      res[3] = acos(t);
      // If the rotation is ZERO degrees, then return an (001) axis.
      if (res[3] == 0.0)
      {
        res[0] = 0.0;
        res[1] = 0.0;
        res[2] = 1.0;
        return;
      }
      else
      {
        typedef Eigen::Matrix<K, 3, 3, Eigen::ColMajor> Matrix_t;
        
        Eigen::Map<Matrix_t> m(const_cast<K*>(&(om[0])));
        
        K angle,x,y,z; // variables for result
        K epsilon = 1.0E-4L; // margin to allow for rounding errors
        K epsilon2 = 1.0E-4L; // margin to distinguish between 0 and 180 degrees
        // optional check that input is pure rotation, 'isRotationMatrix' is defined at:
        // http://www.euclideanspace.com/maths/algebra/matrix/orthogonal/rotation/
        // assert isRotationMatrix(m) : "not valid rotation matrix" ;// for debugging
        
        if ((fabs(m(0,1)-m(1,0)) < epsilon)
            && (fabs(m(0,2)-m(2,0)) < epsilon)
            && (fabs(m(1,2)-m(2,1)) < epsilon))
        {
          // singularity found
          // first check for identity matrix which must have +1 for all terms
          //  in leading diagonal and zero in other terms
          if ((fabs(m(0,1)+m(1,0)) < epsilon2)
              && (fabs(m(0,2)+m(2,0)) < epsilon2)
              && (fabs(m(1,2)+m(2,1)) < epsilon2)
              && (fabs(m(0,0)+m(1,1)+m(2,2)-3) < epsilon2))
          {
            // this singularity is identity matrix so angle = 0
            //return new axisAngle(0,1,0,0); // zero angle, arbitrary axis
            res[0] = 0.0;
            res[1] = 0.0;
            res[2] = 1.0;
            res[3] = 0.0;
            //    printf("Singularity at 0 | 360 deg\n");
            return;
          }
          // otherwise this singularity is angle = 180
          angle = DConst::k_Pi;
          K xx = (m(0,0)+1)/2;
          K yy = (m(1,1)+1)/2;
          K zz = (m(2,2)+1)/2;
          K xy = (m(0,1)+m(1,0))/4;
          K xz = (m(0,2)+m(2,0))/4;
          K yz = (m(1,2)+m(2,1))/4;
          if ((xx > yy) && (xx > zz)) { // m(0,0) is the largest diagonal term
            if (xx< epsilon) {
              x = 0;
              y = 0.7071;
              z = 0.7071;
            } else {
              x = sqrt(xx);
              y = xy/x;
              z = xz/x;
            }
          } else if (yy > zz) { // m(1,1) is the largest diagonal term
            if (yy< epsilon) {
              x = 0.7071;
              y = 0;
              z = 0.7071;
            } else {
              y = sqrt(yy);
              x = xy/y;
              z = yz/y;
            }
          } else { // m(2,2) is the largest diagonal term so base result on this
            if (zz < epsilon) {
              x = 0.7071;
              y = 0.7071;
              z = 0;
            } else {
              z = sqrt(zz);
              x = xz/z;
              y = yz/z;
            }
          }
          //return new axisAngle(angle,x,y,z); // return 180 deg rotation
          res[0] = x;
          res[1] = y;
          res[2] = z;
          res[3] = angle;
          //  printf("Singularity at 180 deg\n");
          return;
        }
        // as we have reached here there are no singularities so we can handle normally
        K s = sqrt((m(2,1) - m(1,2))*(m(2,1) - m(1,2))
                   +(m(0,2) - m(2,0))*(m(0,2) - m(2,0))
                   +(m(1,0) - m(0,1))*(m(1,0) - m(0,1))); // used to normalise
        if (fabs(s) < 0.001) s=1;
        // prevent divide by zero, should not happen if matrix is orthogonal and should be
        // caught by singularity test above, but I've left it in just in case
        angle = acos(( m(0,0) + m(1,1) + m(2,2) - 1)/2);
        x = (m(2,1) - m(1,2))/s;
        y = (m(0,2) - m(2,0))/s;
        z = (m(1,0) - m(0,1))/s;
        //return new axisAngle(angle,x,y,z);
        res[0] = x;
        res[1] = y;
        res[2] = z;
        res[3] = angle;
        return;
      }
    }


      /**: ro2ax
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief Rodrigues vector to axis angle pair
      *
      * @param r Rodrigues vector
      *
      *
      * @date 8/04/13   MDG 1.0 original
      * @date 8/11/14   MDG 1.1 added infty handling
      */
      static void ro2ax(const T& r, T& res)
      {
        K ta = 0.0L;
        K angle = 0.0L;

        ta = r[3];
        if (ta == 0.0L)
        {
          res[0] = 0.0L;
          res[1] = 0.0L;
          res[2] = 1.0L;
          res[3] = 0.0L;
          return;
        }
        if (ta == std::numeric_limits<K>::infinity() )
        {
          res[0] = r[0];
          res[1] = r[1];
          res[2] = r[2];
          res[3] = DConst::k_Pi;
        }
        else
        {
#if 1
          angle = 2.0L * atan(ta);
          ta = r[0] * r[0] + r[1] * r[1] + r[2] * r[2];
          ta = sqrt(ta);
          ta = 1.0L / ta;
          res[0] = r[0] * ta;
          res[1] = r[1] * ta;
          res[2] = r[2] * ta;
          res[3] = angle;
#else
          res[3] = 2.0L * atan(r[3]);
          ta = 0.0L;
          for (int i=0;i<3;i++) {ta += r[i]*r[i];}
          ta = 1.0 / sqrt(ta);
          for (int i=0;i<3;i++) {res[i] = r[i]*ta;}
#endif
        }

      }


      /**: ax2ro
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert axis angle to Rodrigues
      *
      * @param a axis angle pair
      *
      *
      * @date 8/12/13 MDG 1.0 original
      * @date 7/6/14  MDG 2.0 simplified
      * @date 8/11/14 MDG 2.1 added infty handling
      */
      static void ax2ro(const T& r, T& res)
      {
        K  thr = 1.0E-7f;

        if (r[3] == 0.0)
        {
          res[0] = 0.0;
          res[1] = 0.0;
          res[2] = 0.0;
          res[3] = 0.0;
          return;
        }
        res[0] = r[0];
        res[1] = r[1];
        res[2] = r[2];
        if (fabs(r[3] - SIMPLib::Constants::k_Pi) < thr)
        {
          res[3] = std::numeric_limits<K>::infinity();
        }
        else
        {
          res[3] = tan( r[3] * 0.5 );
        }
      }



      /**: ax2qu
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert axis angle to quaternion
      *
      * @param a axis angle pair
      *
      *
      * @date 8/12/13   MDG 1.0 original
      * @date 7/23/14   MDG 1.1 explicit transformation
      */
      static void ax2qu(const T& r, T& res, typename QuaternionMath<K>::Order layout = QuaternionMath<K>::QuaternionVectorScalar)
      {
        size_t w = 0, x = 1, y = 2, z = 3;
        if(layout == QuaternionMath<K>::QuaternionVectorScalar)
        {
          w = 3;
          x = 0;
          y = 1;
          z = 2;
        }
        if (r[3] == 0.0)
        {
          res[w] = 1.0;
          res[x] = 0.0;
          res[y] = 0.0;
          res[z] = 0.0;
        }
        else
        {
          K c = cos(r[3] * 0.5);
          K s = sin(r[3] * 0.5);
          res[w] = c;
          res[x] = r[0] * s;
          res[y] = r[1] * s;
          res[z] = r[2] * s;
        }
      }


      /**: ro2ho
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert rodrigues to homochoric
      *
      * @param r Rodrigues vector
      *
      *
      * @date 8/12/13   MDG 1.0 original
      * @date 7/24/14   MDG 2.0 explicit transformation
      * @date 8/11/14   MDG 3.0 added infty handling
      */
      static void ro2ho(const T& r, T& res)
      {
        K f = 0.0;
        K rv = OMHelperType::sumofSquares(r);
        if (rv == 0.0)
        {
          OMHelperType::splat(res, 0.0);
          return;
        }
        if (r[3] == std::numeric_limits<K>::infinity())
        {
          f = 0.75 * SIMPLib::Constants::k_Pi;
        }
        else
        {
          K t = 2.0 * atan(r[3]);
          f = 0.75 * (t - sin(t));
        }
        f = pow(f, 1.0 / 3.0);
        res[0] = r[0] * f;
        res[1] = r[1] * f;
        res[2] = r[2] * f;
      }

      /**: qu2om
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert a quaternion to a 3x3 matrix
      *
      * @param q quaternion
      *
      *
      * @note verified 8/5/13
      *
      * @date 6/03/13   MDG 1.0 original
      */
      static void qu2om(const T& r, T& res, typename QuaternionMath<K>::Order layout = QuaternionMath<K>::QuaternionVectorScalar)
      {
        size_t w = 0, x = 1, y = 2, z = 3;
        if(layout == QuaternionMath<K>::QuaternionVectorScalar)
        {
          w = 3;
          x = 0;
          y = 1;
          z = 2;
        }
        K qq = r[w] * r[w] - (r[x] * r[x] + r[y] * r[y] + r[z] * r[z]);
        res[0] = qq + 2.0 * r[x] * r[x];
        res[4] = qq + 2.0 * r[y] * r[y];
        res[8] = qq + 2.0 * r[z] * r[z];
        res[1] = 2.0 * (r[x] * r[y] - r[w] * r[z]);
        res[5] = 2.0 * (r[y] * r[z] - r[w] * r[x]);
        res[6] = 2.0 * (r[z] * r[x] - r[w] * r[y]);
        res[3] = 2.0 * (r[y] * r[x] + r[w] * r[z]);
        res[7] = 2.0 * (r[z] * r[y] + r[w] * r[x]);
        res[2] = 2.0 * (r[x] * r[z] + r[w] * r[y]);
        if (Rotations::Constants::epsijk != 1.0) { res = OMHelperType::transpose(res); }
      }



      /**: om2qu
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert a 3x3 rotation matrix to a unit quaternion (see Morawiec, page 37)
      *
      * @param x 3x3 matrix to be converted
      *
      *
      * @date 8/12/13   MDG 1.0 original
      * @date 8/18/14   MDG 2.0 new version
      */
      static void om2qu(const T& om, T& res, typename QuaternionMath<K>::Order layout = QuaternionMath<K>::QuaternionVectorScalar)
      {
        size_t w = 0, x = 1, y = 2, z = 3;
        if(layout == QuaternionMath<K>::QuaternionVectorScalar)
        {
          w = 3;
          x = 0;
          y = 1;
          z = 2;
        }

        K thr = static_cast<K>(1.0E-10);
        if(sizeof(K) == 4) { thr = 1.0E-6; }
        K  s = 0.0;
        K s1 = 0.0;
        K s2 = 0.0;
        K s3 = 0.0;
        T oax(4);
        oax[0] = 0.0f;
        oax[1] = 0.0f;
        oax[2] = 0.0f;
        oax[3] = 0.0f;

        s = om[0] + om[4] + om[8] + 1.0;
        if(OMHelperType::closeEnough(fabs(s), 0.0, thr) ) // Are we close to Zero
        { s = 0.0; }
        s = sqrt(s);
        s1 = om[0] - om[4] - om[8] + 1.0;
        if(OMHelperType::closeEnough(fabs(s1), 0.0, thr) ) // Are we close to Zero
        { s1 = 0.0; }
        s1 = sqrt(s1);
        s2 = -om[0] + om[4] - om[8] + 1.0;
        if(OMHelperType::closeEnough(fabs(s2), 0.0, thr) ) // Are we close to Zero
        { s2 = 0.0; }
        s2 = sqrt(s2);
        s3 = -om[0] - om[4] + om[8] + 1.0;
        if(OMHelperType::closeEnough(fabs(s3), 0.0, thr) ) // Are we close to Zero
        { s3 = 0.0; }
        s3 = sqrt(s3);
        res[w] = s * 0.5;
        res[x] = s1 * 0.5;
        res[y] = s2 * 0.5;
        res[z] = s3 * 0.5;
       // printf("res[z]: % 3.16f \n", res[z]);

        // verify the signs (q0 always positive)
        if (om[7] < om[5]) { res[x] = -Rotations::Constants::epsijk * res[x]; }
        if (om[2] < om[6]) { res[y] = -Rotations::Constants::epsijk * res[y]; }
        if (om[3] < om[1]) { res[z] = -Rotations::Constants::epsijk * res[z]; }
       // printf("res[z]: % 3.16f \n", res[z]);

        s = MatrixMath::Magnitude4x1(&(res[0]));

        if (s != 0.0) { MatrixMath::Divide4x1withConstant<K>(&(res[0]), s); }

        /* we need to do a quick test here to make sure that the
        ! sign of the vector part is the same as that of the
        ! corresponding vector in the axis-angle representation;
        ! these two can end up being different, presumably due to rounding
        ! issues, but this needs to be further analyzed...
        ! This adds a little bit of computation overhead but for now it
        ! is the easiest way to make sure the signs are correct.
        */
        SelfType::om2ax(om, oax);

        if (oax[0]*res[x] < 0.0) { res[x] = -res[x]; }
        if (oax[1]*res[y] < 0.0) { res[y] = -res[y]; }
        if (oax[2]*res[z] < 0.0) { res[z] = -res[z]; }
      }

      /**: qu2ax
      *
      * @author Dr. David Rowenhorst, NRL
      *
      * @brief convert quaternion to axis angle
      *
      * @param q quaternion
      * @param res Result Axis-Angle
      * @param layout The ordering of the data: Vector-Scalar or Scalar-Vector
      */
      static void qu2ax(const T& q, T& res, typename QuaternionMath<K>::Order layout = QuaternionMath<K>::QuaternionVectorScalar)
      {
        size_t w = 0, x = 1, y = 2, z = 3;
        if(layout == QuaternionMath<K>::QuaternionVectorScalar)
        {
          w = 3;
          x = 0;
          y = 1;
          z = 2;
        }
        
        K epsijk = RConst::epsijkd;
        T qo(q);
        // make sure q[0] is >= 0.0
        K sign = 1.0;
        if(q[w] < 0.0) { sign = -1.0; }
        for (int i=0;i<4;i++){qo[i] = sign*q[i];}
        K eps = (K) 1.0e-12;
        K omega = 2.0*acos(qo[w]);
        if (omega < eps){
          res[0] = 0.0;
          res[1] = 0.0;
          res[2] = 1.0 * epsijk;
          res[3] = 0.0;
        } else {
          K mag = 0.0;
          mag = 1.0 / sqrt(q[x] * q[x] + q[y] * q[y] + q[z] * q[z]);;
          
          res[0] = q[x] * mag;
          res[1] = q[y] * mag;
          res[2] = q[z] * mag;
          res[3] = omega;
        }

      }

      /**: qu2ro
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert quaternion to Rodrigues
      *
      * @param q quaternion
      *
      * @date 8/12/13   MDG 1.0 original
      * @date 7/23/14   MDG 2.0 direct transformation
      * @date 8/11/14   MDG 2.1 added infty handling
      */
      static void qu2ro(const T& q, T& res, typename QuaternionMath<K>::Order layout = QuaternionMath<K>::QuaternionVectorScalar)
      {
        size_t w = 0, x = 1, y = 2, z = 3;
        if(layout == QuaternionMath<K>::QuaternionVectorScalar)
        {
          w = 3;
          x = 0;
          y = 1;
          z = 2;
        }
        float thr = 1.0E-8f;
        res[0] = q[x];
        res[1] = q[y];
        res[2] = q[z];
        res[3] = 0.0;

        if (q[w] < thr)
        {
          res[3] = std::numeric_limits<K>::infinity();
          return;
        }
        float s = MatrixMath::Magnitude3x1( &(res[0]) );
        if (s < thr)
        {
          res[0] = 0.0;
          res[1] = 0.0;
          res[2] = 0.0;
          res[3] = 0.0;
          return;
        }
        else
        {
          res[0] = res[0] / s;
          res[1] = res[1] / s;
          res[2] = res[2] / s;
          res[3] = tan(acos(q[w]));
        }
      }



      /**: qu2ho
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert quaternion to homochoric
      *
      * @param q quaternion
      *
      *
      * @date 8/12/13   MDG 1.0 original
      * @date 7/23/14   MDG 2.0 explicit transformation
      */
      static void qu2ho(const T& q, T& res, typename QuaternionMath<K>::Order layout = QuaternionMath<K>::QuaternionVectorScalar)
      {
        size_t w = 0, x = 1, y = 2, z = 3;
        if(layout == QuaternionMath<K>::QuaternionVectorScalar)
        {
          w = 3;
          x = 0;
          y = 1;
          z = 2;
        }
        float s;
        float f;

        K omega = 2.0 * acos(q[w]);
        if (omega == 0.0)
        {
          OMHelperType::splat(res, 0.0);
        }
        else
        {
          res[0] = q[x];
          res[1] = q[y];
          res[2] = q[z];
          s = 1.0 / sqrt(OMHelperType::sumofSquares(res));
          OMHelperType::scalarMultiply(res, s);
          f = 0.75 * ( omega - sin(omega) );
          f = pow(f, 1.0 / 3.0);
          OMHelperType::scalarMultiply(res, f);
        }
      }


      /**: ho2cu
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert homochoric to cubochoric
      *
      * @param h homochoric coordinates
      *
      *
      * @date 8/12/13   MDG 1.0 original
      */
      static void ho2cu(const T& q, T& res)
      {
        int ierr = -1;
        res = ModifiedLambertProjection3D<T,K>::LambertBallToCube(q,ierr);
      }

      //template<typename T, typename K>
      //T LambertBallToCube(const T&q, K ierr)
      //{
      //
      //}

      /**: cu2ho
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert cubochoric to homochoric
      *
      * @param c cubochoric coordinates
      *
      *
      *
      * @date 8/12/13   MDG 1.0 original
      */
      static void cu2ho(const T& cu, T& res)
      {
        int ierr = 0;
        res = ModifiedLambertProjection3D<T, K>::LambertCubeToBall(cu,ierr);
      }


      /**: ro2eu
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief Rodrigues vector to Euler angles
      *
      * @param r Rodrigues vector
      *
      *
      * @date 8/04/13   MDG 1.0 original
      * @date 8/11/14   MDG 1.1 added infty handling
      */
      static void ro2eu(const T& r, T& res)
      {
        T tmp(9);
        SelfType::ro2om(r, tmp);
        SelfType::om2eu(tmp, res);
      }



      /**: eu2ho
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert euler to homochoric
      *
      * @param e 3 euler angles
      *
      *
      * @date 8/12/13   MDG 1.0 original
      */
      static void eu2ho(const T& r, T& res)
      {
        T tmp(4);
        SelfType::eu2ax(r, tmp);
        SelfType::ax2ho(tmp, res);
      }

      /**: om2ro
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert orientation matrix to Rodrigues
      *
      * @param om 3x3 orientation matrix
      *
      *
      * @date 8/12/13   MDG 1.0 original
      */
      static void om2ro(const T& r, T& res)
      {
        T eu(3); // Create a temp array to store the Euler Angles
        SelfType::om2eu(r, eu);// Convert the OM to Euler
        SelfType::eu2ro(eu, res);// Convert Euler to Rodrigues
      }


      /**: om2ho
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert orientation matrix to homochoric
      *
      * @param om 3x3 orientation matrix
      *
      *
      * @date 8/12/13   MDG 1.0 original
      * @date 07/08/14 MDG 2.0 simplification via ax (shorter path)
      */
      static void om2ho(const T& r, T& res)
      {
        T ax(4); // Create a temp array to store the Euler Angles
        SelfType::om2ax(r, ax);// Convert the OM to Axis-Angles
        SelfType::ax2ho(ax, res);// Convert Axis-Angles to Homochoric
      }


      /**: ax2eu
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert axis angle to euler
      *
      * @param a axis angle pair
      *
      *
      * @date 8/12/13   MDG 1.0 original
      * @date 07/08/14 MDG 2.0 simplification via ro (shorter path)
      */
      static void ax2eu(const T& r, T& res)
      {
        T tmp(9); // No initialize since the next line will put its answer into tmp
        SelfType::ax2om(r, tmp);
        SelfType::om2eu(tmp, res);
      }

      /**: ro2om
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert rodrigues to orientation matrix
      *
      * @param r Rodrigues vector
      *
      *
      * @date 8/12/13   MDG 1.0 original
      */
      static void ro2om(const T& r, T& res)
      {
        T tmp(4);
        SelfType::ro2ax(r, tmp);
        SelfType::ax2om(tmp, res);
      }

      /**: ro2qu
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert rodrigues to quaternion
      *
      * @param r Rodrigues vector
      *
      *
      * @date 8/12/13   MDG 1.0 original
      */
      static void ro2qu(const T& r, T& res, typename QuaternionMath<K>::Order layout = QuaternionMath<K>::QuaternionVectorScalar)
      {
        T tmp(4);
        SelfType::ro2ax(r, tmp);
        SelfType::ax2qu(tmp, res, layout);
      }

      /**: ho2eu
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert homochoric to euler
      *
      * @param h homochoric coordinates
      *
      *
      * @date 8/12/13   MDG 1.0 original
      */
      static void ho2eu(const T& r, T& res)
      {
        T tmp(4);
        SelfType::ho2ax(r, tmp);
        SelfType::ax2eu(tmp, res);
      }



      /**: ho2om
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert homochoric to orientation matrix
      *
      * @param h homochoric coordinates
      *
      *
      * @date 8/12/13   MDG 1.0 original
      */
      static void ho2om(const T& r, T& res)
      {
        T tmp(4);
        SelfType::ho2ax(r, tmp);
        SelfType::ax2om(tmp, res);
      }


      /**: ho2ro
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert homochoric to Rodrigues
      *
      * @param h homochoric coordinates
      *
      *
      * @date 8/12/13   MDG 1.0 original
      */
      static void ho2ro(const T& r, T& res)
      {
        T tmp(4);
        SelfType::ho2ax(r, tmp);
        SelfType::ax2ro(tmp, res);
      }


      /**: ho2qu
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert homochoric to quaternion
      *
      * @param r homochoric coordinates
      *
      *
      * @date 8/12/13   MDG 1.0 original
      */
      static void ho2qu(const T& r, T& res, typename QuaternionMath<K>::Order layout = QuaternionMath<K>::QuaternionVectorScalar)
      {
        T tmp(4);
        SelfType::ho2ax(r, tmp);
        SelfType::ax2qu(tmp, res, layout);
      }



      /**: eu2cu
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert euler angles to cubochoric
      *
      * @param e euler angles
      *
      *
      *
      * @date 8/12/13   MDG 1.0 original
      */
      static void eu2cu(const T& r, T& res)
      {
        T tmp(3);
        SelfType::eu2ho(r, tmp);
        SelfType::ho2cu(tmp, res);
      }


      /**: om2cu
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert orientation matrix to cubochoric
      *
      * @param o orientation matrix
      *
      *
      *
      * @date 8/12/13   MDG 1.0 original
      */
      static void om2cu(const T& r, T& res)
      {
        T tmp(3);
        SelfType::om2ho(r, tmp);
        SelfType::ho2cu(tmp, res);
      }

      /**: ax2cu
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert axis angle to cubochoric
      *
      * @param a axis angle
      *
      *
      *
      * @date 8/12/13   MDG 1.0 original
      */

      static void ax2cu(const T& r, T& res)
      {
        T tmp(3);
        SelfType::ax2ho(r, tmp);
        SelfType::ho2cu(tmp, res);
      }


      /**: ro2cu
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert Rodrigues to cubochoric
      *
      * @param r Rodrigues
      *
      *
      *
      * @date 8/12/13   MDG 1.0 original
      */
      static void ro2cu(const T& r, T& res)
      {
        T tmp(3);
        SelfType::ro2ho(r, tmp);
        SelfType::ho2cu(tmp, res);
      }


      /**: qu2cu
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert quaternion to cubochoric
      *
      * @param q quaternion
      *
      *
      *
      * @date 8/12/13   MDG 1.0 original
      */
      static void qu2cu(const T& r, T& res, typename QuaternionMath<K>::Order layout = QuaternionMath<K>::QuaternionVectorScalar)
      {
        T tmp(3);
        SelfType::qu2ho(r, tmp, layout);
        SelfType::ho2cu(tmp, res);
      }

      /**: cu2eu
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert cubochoric to euler angles
      *
      * @param c cubochoric coordinates
      *
      *
      *
      * @date 8/12/13   MDG 1.0 original
      */

      void cu2eu(float* c, float* res);


      /**: cu2om
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert cubochoric to orientation matrix
      *
      * @param c cubochoric coordinates
      *
      *
      *
      * @date 8/12/13   MDG 1.0 original
      */

      void cu2om(float* c, float* res);


      /**: cu2ax
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert cubochoric to axis angle
      *
      * @param c cubochoric coordinates
      *
      *
      *
      * @date 8/12/13   MDG 1.0 original
      */

      void cu2ax(float* c, float* res);

      /**: cu2ro
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert cubochoric to Rodrigues
      *
      * @param c cubochoric coordinates
      *
      *
      *
      * @date 8/12/13   MDG 1.0 original
      */

      static void cu2ro(const T& r, T& res)
      {
        T tmp(3);
        SelfType::cu2ho(r, tmp);
        SelfType::ho2ro(tmp, res);
      }


      /**: cu2qu
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief convert cubochoric to quaternion
      *
      * @param c cubochoric coordinates
      *
      *
      *
      * @date 8/12/13   MDG 1.0 original
      */

      static void cu2qu(const T& r, T& res, typename QuaternionMath<K>::Order layout = QuaternionMath<K>::QuaternionVectorScalar)
      {
        T ho(3); // Create a temp array to store the Euler Angles
        SelfType::cu2ho(r, ho);// Convert the Cuborchoric to Homochoric
        SelfType::ho2qu(ho, res);// Convert Homochoric to Quaternion
      }


      /**: RotVec_om
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief rotate a vector using a rotation matrix, active or passive
      *
      * @details This routine provides a way for the user to transform a vector
      * and it returns the new vector components.  The user can use either a
      * rotation matrix or a quaternion to define the transformation, and must
      * also specifiy whether an active or passive result is needed.
      *
      *
      * @param vec input vector components
      * @param om orientation matrix
      * @param ap active/passive switch
      *
      *
      * @date 8/18/14   MDG 1.0 original
      */

      void RotVec_om(float* vec, float* om, char ap, float* res);


      /**: RotVec_qu
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief rotate a vector using a quaternion, active or passive
      *
      * @details This routine provides a way for the user to transform a vector
      * and it returns the new vector components.  The user can use either a
      * rotation matrix or a quaternion to define the transformation, and must
      * also specifiy whether an active or passive result is needed.
      *
      *
      * @param vec input vector components
      * @param qu quaternion
      * @param ap active/passive switch
      *
      *
      * @date 8/18/14   MDG 1.0 original
      */

      void RotVec_qu(float* vec, float* qu, char ap, float* res);

      /**: RotTensor2_om
      *
      * @author Marc De Graef, Carnegie Mellon University
      *
      * @brief rotate a second rank tensor using a rotation matrix, active or passive
      *
      * @param tensor input tensor components
      * @param om orientation matrix
      * @param ap active/passive switch
      *
      *
      * @date 8/18/14   MDG 1.0 original
      */

      void RotTensor2_om(float* tensor, float* om, char ap, float* res);


      /**
      * SUBROUTINE: print_orientation
                    *
                    * @author Marc De Graef, Carnegie Mellon University
                    *
                    * @brief  prints a complete orientationtype record or a single entry
                    *
                    * @param o orientationtype record
                    * @param outtype (optional) indicates which representation to print
      * @param pretext (optional) up to 10 characters that will precede each line
      *
      * @date  8/4/13   MDG 1.0 original
      print the entire record with all representations

      * SUBROUTINE: print_orientation_d
      *
      * @author Marc De Graef, Carnegie Mellon University
                               *
                               * @brief  prints a complete orientationtype record or a single entry (double precision)
      *
      * @param o orientationtype record
      * @param outtype (optional) indicates which representation to print
      * @param pretext (optional) up to 10 characters that will precede each line
      *
      * @date  8/4/13   MDG 1.0 original
      print the entire record with all representations
      */



    protected:
      /**
      * @brief
      */
      OrientationTransforms() {}

    private:
      OrientationTransforms(const OrientationTransforms&); // Copy Constructor Not Implemented
      void operator=(const OrientationTransforms&); // Operator '=' Not Implemented
  };



typedef OrientationTransforms<FOrientArrayType, float>     FOrientTransformsType;
typedef OrientationTransforms<DOrientArrayType, double>     DOrientTransformsType;


#endif /* _OrientationTransforms_H_ */
