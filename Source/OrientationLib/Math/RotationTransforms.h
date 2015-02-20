#ifndef _RotationTransforms_H_
#define _RotationTransforms_H_

#include <string>
#include <iostream>
#include <algorithm>
#include <complex>

#if __APPLE__
#include <Accelerate/Accelerate.h>
#else
#include <cblas.h>
#endif


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
// The C++ Math include MUST be after the above "DREAM3DMath.h" include.
#include <cmath>


#include "OrientationLib/OrientationLib.h"
#include "OrientationLib/Math/RotArray.hpp"

#include "SGEEV.h"

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
|  e      |  #   |  X   |  X   |  X   |  X   |  -   |  -   |
|  o      |  X   |  #   |  -   |  -   |  -   |  -   |  -   |
|  a      |  -   |  -   |  #   |  -   |  -   |  -   |  -   |
|  r      |  -   |  -   |  X   |  #   |  -   |  -   |  -   |
|  q      |  -   |  -   |  -   |  -   |  #   |  -   |  -   |
|  h      |  -   |  -   |  -   |  -   |  -   |  #   |  -   |
|  c      |  -   |  -   |  -   |  -   |  -   |  -   |  #   |


*/




namespace LambertParametersType {
  double Pi=3.1415926535897930;     // pi
  double iPi=0.3183098861837910 ;   // 1/pi
  double sPi=1.7724538509055160 ;   // sqrt(pi)
  double sPio2=1.2533141373155000;  // sqrt(pi/2)
  double sPi2=0.8862269254527580;   // sqrt(pi)/2
  double srt=0.8660254037844390;    // sqrt(3)/2
  double isrt=0.5773502691896260;  // 1/sqrt(3)
  double alpha=1.3467736870885980; // sqrt(pi)/3^(1/4)
  double rtt=1.7320508075688770;    // sqrt(3)
  double prea=0.5250375679043320;  // 3^(1/4)/sqrt(2pi)
  double preb=1.0500751358086640;   // 3^(1/4)sqrt(2/pi)
  double prec=0.9068996821171090;  // pi/2sqrt(3)
  double pred=2.0943951023931950;   // 2pi/3
  double pree=0.7598356856515930;   // 3^(-1/4)
  double pref=1.3819765978853420;   // sqrt(6/pi)
  // ! the following constants are used for the cube to quaternion hemisphere mapping
  double a=1.9257490199582530;      //pi^(5/6)/6^(1/6)
  double ap=2.1450293971110250 ;    //pi^(2/3)
  double sc=0.8977727869612860 ;    //a/ap
  double beta=0.9628745099791260;   //pi^(5/6)/6^(1/6)/2
  double R1=1.3306700394914690 ;    //(3pi/4)^(1/3)
  double r2=1.4142135623730950 ;    //sqrt(2)
  double r22=0.7071067811865470;    //1/sqrt(2)
  double pi12=0.2617993877991490;   //pi/12
  double pi8=0.3926990816987240;    //pi/8
  double prek=1.6434564029725040;   //R1 2^(1/4)/beta
  double r24=4.8989794855663560;    //sqrt(24)
  double tfit[16] = {1.00000000000188520, -0.50000000021948470,
                     -0.0249999921275931260, - 0.0039287015447813740,
                     -0.00081527015354504380, - 0.00020095004261197120,
                     -0.000023979867760717560, - 0.000082028689266058410,
                     +0.000124487150420900920, - 0.00017491142148225770,
                     +0.00017034819341400540, - 0.000120620650041168280,
                     +0.0000597197058686608260, - 0.000019807567239656470,
                     +0.0000039537146842128740, - 0.000000365550014397195440 };
  double BP[6]= { 0.0, 1.0, 0.5773502691896260, 0.4142135623730950, 0.0,
                  0.2679491924311230 };     //used for Fundamental Zone determination in so3 module
}





namespace LPs = LambertParametersType;



namespace Rotations {
  namespace Constants {
    //! uncomment these for an alternative way of doing things
    //  static float epsijk = -1.0f;
    //  static double epsijkd = -1.0;

    // uncomment these for the Morawiec version.
    static float  epsijk = 1.0f;
    static float  epsijkd = 1.0;
  }
}

namespace RConst = Rotations::Constants;

#define ROT_SCALAR_VECTOR 0
#define ROT_VECTOR_SCALAR 1

namespace DConst = DREAM3D::Constants;


/**
 * @brief The RotationTransforms class
 */
class OrientationLib_EXPORT RotationTransforms
{
  public:
    RotationTransforms();
    virtual ~RotationTransforms();


    typedef struct {
        int result;
        std::string msg;
    } ResultType;


    void FatalError(const std::string &func, const std::string &msg);
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
    * @param eu 3-component vector (single precision)
    *
    *
    * @date 9/30/14   MDG 1.0 original
    */
    template<typename T>
    ResultType eu_check(const T& eu)
    {
      ResultType res;
      res.result = 1;

      if ((eu[0] < 0.0) || (eu[0] > (2.0*M_PI))) {
        res.msg = "rotations:eu_check:: phi1 Euler angle outside of valid range [0,2pi]";
        res.result = 0;
      }
      if ((eu[1] < 0.0) || (eu[1] > M_PI)) {
        res.msg = "rotations:eu_check:: phi Euler angle outside of valid range [0,pi]";
        res.result = 0;
      }
      if ((eu[2] < 0.0) || (eu[2] > (2.0*M_PI))) {
        res.msg = "rotations:eu_check:: phi2 Euler angle outside of valid range [0,2pi]";
        res.result = 0;
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

    template<typename T, typename K>
    ResultType ro_check(const T& ro)
    {
      K eps = std::numeric_limits<K>::epsilon();
      ResultType res;
      res.result = 1;
      if (ro[3] < 0.0) {
        res.msg = "rotations:ro_check:: Rodrigues-Frank vector has negative length: ";
        res.result = 0;
        return res;
      }
      T out = multiply(ro, ro, 3);
      K ttl = sum<T, K>(out);
      ttl = sqrt(ttl);
      if (std::abs(ttl-1.0) > eps) {
        res.msg = "rotations:ro_check:: Rodrigues-Frank axis vector not normalized";
        res.result = 0;
      }
      return res;
    }


    /**: ho_check
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief verify that the homochoric vector is inside or on the homochoric ball
    *
    * @param ho 3-component vector (single precision)
    *
    *
    * @date 9/30/14   MDG 1.0 original
    */
    template<typename T, typename K>
    ResultType ho_check(const T& ro)
    {
      ResultType res;
      res.result = 1;
      T out = multiply(ro, ro);
      K ttl = sum<T, K>(out);
      K r = sqrt(ttl);
      if (r > static_cast<float>(LPs::R1)) {
        res.msg = "rotations:ho_check: homochoric vector outside homochoric ball";
        res.result = 0;
      }
      return res;
    }

    /**: cu_check
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief verify that the cubochoric vector is inside or on the cube
    *
    * @param cu 3-component vector (single precision)
    *
    *
    * @date 9/30/14   MDG 1.0 original
    */
    template<typename T, typename K>
    ResultType cu_check(const T& cu)
    {
      ResultType res;
      res.result = 1;

      K r = maxval<T, K>(absValue(cu));
      if (r > static_cast<float>(LPs::ap/2.0)) {
        res.msg = "rotations:cu_check: cubochoric vector outside cube";
        res.result = 0;
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
    template<typename T, typename K>
    ResultType qu_check(const T& qu)
    {
      ResultType res;
      res.result = 1;
      K eps = std::numeric_limits<K>::epsilon();

      T out = multiply<T>(qu, qu);
      K r = sqrt(sum<T,K>(out));
      if (qu[0] < 0.0) {
        res.msg = "rotations:qu_check: quaternion must have positive scalar part";
        res.result = 0;
        return res;
      }
      if (std::abs(r-1.0) > eps) {
        res.msg = "rotations:qu_check: quaternion must have unit norm";
        res.result = 0;
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
    template<typename T, typename K>
    ResultType ax_check(const T& ax)
    {
      ResultType res;
      res.result = 1;
      if ((ax[3] < 0.0) || (ax[3] > M_PI)) {
        res.msg = "rotations:ax_check: angle must be in range [0,pi]";
        res.result = 0;
        return res;
      }
      K eps = std::numeric_limits<K>::epsilon();
      T out = multiply<T>(ax, ax, 3);
      K r = sqrt(sum<T,K>(out));
      K absv = std::abs(r-1.0);
      if (absv > eps) {
        res.msg = "rotations:ax_check: axis-angle axis vector must have unit norm";
        res.result = 0;
      }
      return res;
    }


    /**: om_check
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief verify that the rotation matrix is actually a proper rotation matrix
    *
    * @param om 3x3-component matrix (single precision)
    *
    *
    * @date 9/30/14   MDG 1.0 original
    */
    template<typename T, typename K>
    ResultType om_check(const T& om)
    {
      ResultType res;
      res.result = 1;
      K eps = std::numeric_limits<K>::epsilon();
      K det = om[4]*(om[0]*om[8] - om[6]*om[2]) + om[5]*(om[1]*om[6] - om[7]*om[0]) + om[3]*(om[2]*om[0] - om[8]*om[1]);

      if (det < 0.0) {
        res.msg = "rotations:om_check: Determinant of rotation matrix must be positive";
        res.result = 0;
        return res;
      }

      if (std::abs(det-1.0) > eps) {
        res.msg = "rotations:om_check: Determinant of rotation matrix must be unity";
        res.result = 0;
        return res;
      }

      T tr = transpose<T>(om);
      T mm = matmul3x3<T>(om, tr);
      T abv = absValue<T>(mm);

      K r = sum<T, K>(abv);
      r = std::abs(r - 3.0);
      if (r > eps) {
        res.msg = "rotations:om_check: rotation matrix times transpose must be identity matrix";
        res.result = 0;
      }
      return res;
    }


    /**: genrot
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief generate a passive rotation representation, given the unit axis vector and the rotation angle
    *
    * @param av 3-component vector (single precision)
    * @param omega rotation angle (radians)
    *
    *
    * @date 9/30/14   MDG 1.0 original
    */

    void genrot(float* av, float omega, float* res);


    /**: genrot_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief generate a passive rotation representation, given the unit axis vector and the rotation angle
    *
    * @param av 3-component vector (double precision)
    * @param omega rotation angle (radians)
    *
    *
    * @date 9/30/14   MDG 1.0 original
    */

    void genrot(double* av, double omega, double* res);


    /**: init_orientation
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief take an orientation representation with 3 components and init all others (single precision)
    *
    * @param orient 3-component vector (single precision)
    * @param intype input type ['eu', 'ro', 'ho', 'cu']
    * @param rotcheck  optional parameter to enforce strict range checking
    *
    * @date 8/04/13   MDG 1.0 original
    * @date 9/30/14   MDG 1.1 added testing of valid ranges
    */

    void init_orientation(float* orient, char intype[2], bool rotcheck, float* res);

    /**: init_orientation_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief take an orientation representation with 3 components and init all others (double precision)
    *
    * @param orient 3-component vector (double precision)
    * @param intype input type ['eu', 'ro', 'ho', 'cu']
    * @param rotcheck  optional parameter to enforce strict range checking
    *
    *
    * @date 8/04/13   MDG 1.0 original
    * @date 9/30/14   MDG 1.1 added testing of valid ranges
    */

    void init_orientation(double* orient, char intype[2], bool rotcheck, double* res);

    /**: init_orientation_om
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief take an orientation representation with 3x3 components and init all others (single precision)
    *
    * @param orient r-component vector (single precision)
    * @param intype input type ['om']
    * @param rotcheck  optional parameter to enforce strict range checking
    *
    *
    * @date 8/04/13   MDG 1.0 original
    */

    void init_orientation_om(float* orient, char intype[2], bool rotcheck, float* res);

    /**: init_orientation_om_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief take an orientation representation with 3x3 components and init all others (double precision)
    *
    * @param orient r-component vector (double precision)
    * @param intype input type ['om']
    * @param rotcheck  optional parameter to enforce strict range checking
    *
    *
    * @date 8/04/13   MDG 1.0 original
    */

    void init_orientation_om(double* orient, char intype[2], bool rotcheck, double* res);

    /**: eu2om
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief Euler angles to orientation matrix (single precision) [Morawiec, page 28]
    *
    * @param e 3 Euler angles in radians (single precision)
    *
    *
    * @date 8/04/13   MDG 1.0 original
    * @data 7/23/14   MDG 1.1 verified
    */
    template<typename T, typename K>
    void eu2om(const T& e, T& res)
    {
      K eps = std::numeric_limits<K>::epsilon();

      K c1 = cos(e[0]);
      K c2 = cos(e[1]);
      K c3 = cos(e[2]);
      K s1 = sin(e[0]);
      K s2 = sin(e[1]);
      K s3 = sin(e[2]);
      res[0] = c1*c3-s1*s3*c2;
      res[1] = s1*c3+c1*s3*c2;
      res[2] = s3*s2;
      res[3] = -c1*s3-s1*c3*c2;
      res[4] = -s1*s3+c1*c3*c2;
      res[5] = c3*s2;
      res[6] = s1*s2;
      res[7] = -c1*s2;
      res[8] = c2;
      for(size_t i = 0; i < 9; i++) {
        if(std::abs(res[i]) < eps) { res[i] = 0.0; }
      }
    }

    /**: eu2ax
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert euler to axis angle
    *
    * @param e 3 euler angles (single precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    * @date 7/23/14   MDG 2.0 explicit implementation
    * @date 7/23/14   MDG 2.1 exception for zero rotation angle
    */
    template<typename T, typename K>
    void eu2ax(const T& e, T& res)
    {
      K thr = 1.0E-6;
      K alpha = 0.0;
      K t = tan(e[1]*0.5);
      K sig = 0.5*(e[0]+e[2]);
      K del = 0.5*(e[0]-e[2]);
      K tau = sqrt(t*t+sin(sig)*sin(sig));
      if (sig == DREAM3D::Constants::k_PiOver2) {  //Infinity
        alpha = DREAM3D::Constants::k_Pi;
      } else {
        alpha = 2.0 * atan(tau/cos(sig)); //! return a default identity axis-angle pair
      }

      if (std::abs(alpha) < thr) {
        res[0] = 0.0; res[1] = 0.0; res[2] = 1.0; res[3] = 0.0;
      } else {
        //! passive axis-angle pair so a minus sign in front
        res[0] = -RConst::epsijk * t * cos(del) / tau;
        res[1] = -RConst::epsijk * t * sin(del) / tau;
        res[2] = -RConst::epsijk * sin(sig) / tau;
        res[3] = alpha;
      }

      if (alpha < 0.0) {
        res[0] = -res[0]; res[1] = -res[1]; res[2] = -res[2]; res[3] = -res[3];
      }
    }

    /**: eu2ro
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief Euler angles to Rodrigues vector (single precision) [Morawiec, page 40]
    *
    * @param e 3 Euler angles in radians (single precision)
    *
    *
    * @date 8/04/13   MDG 1.0 original
    */
    template<typename T, typename K>
    void eu2ro(const T& e, T& res)
    {
      K thr = 1.0E-6;

      eu2ax<T, K>(e, res);
      K t = res[3];
      if (std::abs(t - DREAM3D::Constants::k_Pi) < thr) {
        res[3] = std::numeric_limits<K>::infinity();
        return;
      }
      if (t == 0.0) {
        res[0] = 0.0; res[1] = 0.0; res[2] = 0.0; res[3] = 0.0;
      } else {
        res[3] = tan(t*0.5);
      }
    }

    /**: eu2qu
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief Euler angles to quaternion (single precision) [Morawiec, page 40]
    *
    * @note verified 8/5/13
    *
    * @param e 3 Euler angles in radians (single precision)
    * @param Quaternion can be of form Scalar<Vector> or <Vector>Scalar in memory. The
    * default is (Scalar, <Vector>)
    *
    * @date 8/04/13   MDG 1.0 original
    * @date 8/07/14   MDG 1.1 verified
    */

    template<typename T, typename K>
    void eu2qu(const T& e, T& res, int layout=ROT_SCALAR_VECTOR)
    {
      size_t a = 0, b=1, c=2, d=3;
      if(layout == ROT_VECTOR_SCALAR)
      {
        a = 3;
        b = 2;
        c = 1;
        d = 0;
      }

      float ee[3];
      float cPhi;
      float cp;
      float cm;
      float sPhi;
      float sp;
      float sm;

      ee[0] = 0.5*e[0];
      ee[1] = 0.5*e[1];
      ee[2] = 0.5*e[2];

      cPhi = cos(ee[1]);
      sPhi = sin(ee[1]);
      cm = cos(ee[0]-ee[2]);
      sm = sin(ee[0]-ee[2]);
      cp = cos(ee[0]+ee[2]);
      sp = sin(ee[0]+ee[2]);
      res[a] = cPhi*cp;
      res[b] = -RConst::epsijk*sPhi*cm;
      res[c] = -RConst::epsijk*sPhi*sm;
      res[d] = -RConst::epsijk*cPhi*sp;
      if (res[a] < 0.0) {
        res[a] = -res[a];
        res[b] = -res[b];
        res[c] = -res[c];
        res[d] = -res[d];
      }
    }



    /**: om2eu
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief orientation matrix to euler angles (single precision)
    *
    * @note verified 8/19/14 using Mathematica
    *
    * @param o orientation matrix (single precision)
    * @param res Euler Angles
    *
    * @date 8/04/13   MDG 1.0 original
    * @date 8/19/14   MDG 1.1 verification using Mathematica
    */
    template<typename T, typename K>
    void om2eu(const T& o, T& res)
    {
      //*** use local
      //*** use constants
      //*** IMPLICIT NONE
      //real(kind=sgl),INTENT(IN)       :: o[8]               // orientation matrix
      float zeta;

      if (abs(o[8]) != 1.0) {
        res[1] = acos(o[8]);
        zeta = 1.0/sqrt(1.0-o[8]*o[8]);
        res[0] = atan2(o[6]*zeta,-o[7]*zeta);
        res[2] = atan2(o[2]*zeta, o[5]*zeta);
      } else {
        if (o[8] == 1.0) {
          res[0] = atan2( o[1],o[0]);
          res[1] = 0.0;
          res[2] = 0.0;
        } else {
          res[0] =-atan2(-o[1],o[0]);
          res[1] = M_PI;
          res[2] = 0.0;
        }
      }

      if (res[0] < 0.0) res[0] = fmod(res[0]+100.0*DConst::k_Pi, DConst::k_2Pi);
      if (res[1] < 0.0) res[1] = fmod(res[1]+100.0*DConst::k_Pi, DConst::k_Pi);
      if (res[2] < 0.0) res[2] = fmod(res[2]+100.0*DConst::k_Pi, DConst::k_2Pi);
    }




    /**: ax2om
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief Axis angle pair to orientation matrix (single precision)
    *
    * @note verified 8/5/13.
    *
    * @param a axis angle pair (single precision)
    *
    *
    * @date 8/04/13   MDG 1.0 original
    */

    void ax2om(float* a, float* res);

    /**: ax2om_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief Axis angle pair to orientation matrix (double precision)
    *
    * @note verified 8/5/13.
    *
    * @param a axis angle pair (double precision)
    *
    *
    * @date 8/04/13   MDG 1.0 original
    */

    void ax2om(double* a, double* res);

    /**: qu2eu
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief Quaternion to Euler angles (single precision) [Morawiec page 40, with errata !!!! ]
    *
    * @param q quaternion (single precision)
    *
    *
    * @date 8/04/13   MDG 1.0 original
    */

    void qu2eu(float* q, float* res);

    /**: qu2eu_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief Quaternion to Euler angles (double precision) [Morawiec page 40, with errata !!!! ]
    *
    * @param q quaternion (double precision)
    *
    *
    * @date 8/04/13   MDG 1.0 original
    */

    void qu2eu(double* q, double* res);

    /**: ax2ho
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief Axis angle pair to homochoric (single precision)
    *
    * @param a axis-angle pair (single precision)
    *
    * !
    * @date 8/04/13   MDG 1.0 original
    */

    void ax2ho(float* a, float* res);

    /**: ax2ho_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief Axis angle pair to homochoric (double precision)
    *
    * @param a axis-angle pair (double precision)
    *
    * !
    * @date 8/04/13   MDG 1.0 original
    */

    void ax2ho(double* a, double* res);

    /**: ho2ax
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief Homochoric to axis angle pair (single precision)
    *
    * @param h homochoric coordinates (single precision)
    *
    *
    *
    * @date 8/04/13  MDG 1.0 original
    * @date 07/21/14 MDG 1.1 double precision fit coefficients
    */

    void ho2ax(float* h, float* res);


    /**: ho2ax_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief Homochoric to axis angle pair (double precision)
    *
    * @param h homochoric coordinates (double precision)
    *
    *
    * @date 8/04/13   MDG 1.0 original
    * @date 07/21/14 MDG 1.1 double precision fit coefficients
    */

    void ho2ax(double* h, double* res);

    template<typename T>
    T transfer_sign(T a, T b)
    {
      if( a > 0.0 && b > 0.0) return a;
      if( a < 0.0 && b > 0.0) return -a;

      if( a < 0.0 && b < 0.0) return a;

      return -a;

    }

    /**: om2ax
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert orientation matrix to axis angle
    *
    * @details this assumes that the matrix represents a passive rotation.
    *
    * @param om 3x3 orientation matrix (single precision)
    *
    *
    * @date 8/12/13  MDG 1.0 original
    * @date 07/08/14 MDG 2.0 replaced by direct solution
    */
    template<typename T, typename K>
    void om2ax(const T& om, T& res)
    {
#ifndef __APPLE__
      res[0] = std::numeric_limits<float>::signaling_NaN();
      res[1] = std::numeric_limits<float>::signaling_NaN();
      res[2] = std::numeric_limits<float>::signaling_NaN();
      res[3] = std::numeric_limits<float>::signaling_NaN();
#else
      //*** use local
      //*** use constants
      //*** use error
      //*** use io
      //real(kind=sgl), INTENT(IN)              :: om(3,3)


      K t;
      K omega;
      K qq[4];


      // Apparently no way to wrap existing arrays with Eigen (stupid) so we have to
      // make a copy
#if 1
      // K* o; //[9];

#else
      typedef Eigen::Matrix<K, 3, 3, Eigen::RowMajor> Matrix3x3f;
      Matrix3x3f o;
      for(int i = 0; i < 9; i++)
      {
        o(i) = om[i];
      }
      std::cout << "om: " << ref << std::endl;
#endif

      std::complex<K> ev;
      std::complex<K> cone(1.0,0.0);

      K thr = 1.0E-7;


      //o = &(om[0]);
      t = 0.50*(om[0]+om[4]+om[8] - 1.0);
      if (t > 1.0) { t = 1.0; }
      if (t < -1.0) {t = -1.0; }
      res[3] = acos(t);
      if (res[3] == 0.0)
      {
        res[0] = 0.0;
        res[1] = 0.0;
        res[2] = 1.0;
        return;
      } else {
        K o[9];
        K VL[9];
        K VR[9];
        K Wr[3];
        K Wi[3];
        K WORK[10];
        char JOBVL = 'N';
        char JOBVR = 'V';
        int nn = 3;
        int LDA = nn;
        int LDVL = nn;
        int LDVR = nn;
        int INFO = 0;
        JOBVL = 'N';   //do not compute the left eigenvectors
        JOBVR = 'V';   //do compute the right eigenvectors
        int LWORK = 20;

        ::memcpy(o, &(om[0]), sizeof(K)*9);

        sgeev_(&JOBVL, &JOBVR, &nn, o, &LDA, Wr, Wi, VL, &LDVL, VR, &LDVR, WORK, &LWORK, &INFO);
        std::cout << VR[0] << " " << VR[1] << " " << VR[2] << std::endl;
        std::cout << VR[3] << " " << VR[4] << " " << VR[5] << std::endl;
        std::cout << VR[6] << " " << VR[7] << " " << VR[8] << std::endl;
        std::cout << "" << std::endl;

        std::cout << VL[0] << " " << VL[1] << " " << VL[2] << std::endl;
        std::cout << VL[3] << " " << VL[4] << " " << VL[5] << std::endl;
        std::cout << VL[6] << " " << VL[7] << " " << VL[8] << std::endl;
        std::cout << "" << std::endl;

        std::cout << Wr[0] << " " << Wr[1] << " " << Wr[2] << std::endl;
        std::cout << Wi[0] << " " << Wi[1] << " " << Wi[2] << std::endl;


        K V[9];
        K d[9];
        eigen_decomposition<K>(&(om[0]), V, d);
        std::cout << V[0] << " " << V[1] << " " << V[2] << std::endl;
        std::cout << V[3] << " " << V[4] << " " << V[5] << std::endl;
        std::cout << V[6] << " " << V[7] << " " << V[8] << std::endl;
        std::cout << "" << std::endl;
        std::cout << d[0] << " " << d[1] << " " << d[2] << std::endl;


        if (INFO != 0) {
          res[0] = std::numeric_limits<float>::signaling_NaN();
          res[1] = std::numeric_limits<float>::signaling_NaN();
          res[2] = std::numeric_limits<float>::signaling_NaN();
          res[3] = std::numeric_limits<float>::signaling_NaN();
          //FatalError('Error in om2ax/dgeev : ','DGEEV return not zero');
          exit(0);
        }
        for(int i = 0; i < 3; i++)
        {
          ev = std::complex<K>( Wr[i], Wi[i] );
          if (std::abs(ev-cone) < thr)
          {
            //            res(1:3) = VR(1:3,i);

            res[0] = VR[3*i+0];
            res[1] = VR[3*i+1];
            res[2] = VR[3*i+2];

            if ((om[5]-om[7]) != 0.0) { res[0] = transfer_sign(res[0],-RConst::epsijk*(om[5]-om[7])); }
            if ((om[6]-om[2]) != 0.0) { res[1] = transfer_sign(res[1],-RConst::epsijk*(om[6]-om[2])); }
            if ((om[1]-om[3]) != 0.0) { res[2] = transfer_sign(res[2],-RConst::epsijk*(om[1]-om[3])); }
            return;
          }

        }
#endif
      }
    }

    /**: ro2ax
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief Rodrigues vector to axis angle pair (single precision)
    *
    * @param r Rodrigues vector (single precision)
    *
    *
    * @date 8/04/13   MDG 1.0 original
    * @date 8/11/14   MDG 1.1 added infty handling
    */
    template<typename T, typename K>
    void ro2ax(const T& r, T& res)
    {
#if 0
      //*** use local
      //*** use constants
      //*** IMPLICIT NONE
      //real(kind=sgl),INTENT(IN)       :: r[3]         // input Rodrigues vector

      float  ta;
      float angle;

      ta = r[2];
      if (ta == 0.0) {
        res[0] = 0.0; res[1] = 0.0; res[2] = 1.0; res[3] = 0.0;
        return;
      }
      if (ta == std::numeric_limits<K>::infinity() ) {
        res[0] = r[0]; res[1] = r[1]; res[2] = r[2]; res[3] = DConst::k_Pi;
      } else {
        angle = 2.0*atan(ta);
        ta = 1.0/sqrt(sum(r[0]*r[0] + r[1]*r[1] + r[2]*r[2]));
        res[0] = r[0]/ta; res[1] = r[1]/ta; res[2] = r[2]/ta; res[3] = angle;
      }
#endif
    }


    /**: ax2ro
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert axis angle to Rodrigues
    *
    * @param a axis angle pair (single precision)
    *
    *
    * @date 8/12/13 MDG 1.0 original
    * @date 7/6/14  MDG 2.0 simplified
    * @date 8/11/14 MDG 2.1 added infty handling
    */

    void ax2ro(float* a, float* res);


    /**: ax2ro_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert axis angle to Rodrigues
    *
    * @param a axis angle pair (double precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    * @date 7/6/14  MDG 2.0 simplified
    * @date 8/11/14 MDG 2.1 added infty handling
    */

    void ax2ro(double* a, double* res);


    /**: ax2qu
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert axis angle to quaternion
    *
    * @param a axis angle pair (single precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    * @date 7/23/14   MDG 1.1 explicit transformation
    */

    void ax2qu(float* a, float* res);

    /**: ax2qu_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert axis angle to quaternion
    *
    * @param a axis angle pair (double precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    * @date 7/23/14   MDG 1.1 explicit transformation
    */

    void ax2qu(double* a, double* res);

    /**: ro2ho
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert rodrigues to homochoric
    *
    * @param r Rodrigues vector (single precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    * @date 7/24/14   MDG 2.0 explicit transformation
    * @date 8/11/14   MDG 3.0 added infty handling
    */

    void ro2ho(float* r, float* res);

    /**: ro2ho_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert rodrigues to homochoric
    *
    * @param r Rodrigues vector (double precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    * @date 7/24/14   MDG 2.0 explicit transformation
    */

    void ro2ho(double* r, double* res);

    /**: qu2om
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert a quaternion to a 3x3 matrix
    *
    * @param q quaternion (single precision)
    *
    *
    * @note verified 8/5/13
    *
    * @date 6/03/13   MDG 1.0 original
    */

    void qu2om(float* q, float* res);

    /**: qu2om_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert a quaternion to a 3x3 matrix (double precision)
    *
    * @param q quaternion (double precision)
    *
    *
    * @note verified 8/5/13
    *
    * @date 6/03/13   MDG 1.0 original
    */

    void qu2om(double* q, double* res);

    /**: om2qu
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert a 3x3 rotation matrix to a unit quaternion (see Morawiec, page 37)
    *
    * @param x 3x3 matrix to be converted (single precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    * @date 8/18/14   MDG 2.0 new version
    */

    void om2qu(float* x, float* res);


    /**: om2qu_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert a 3x3 rotation matrix to a unit quaternion (see Morawiec, page 37)
    *
    * @param x 3x3 matrix to be converted (double precision)
    *
    * @date 8/12/13   MDG 1.0 original
    * @date 8/18/14   MDG 2.0 new version
    */

    void om2qu(double* x, double* res);


    /**: qu2ax
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert quaternion to axis angle
    *
    * @param q quaternion (single precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    * @date 7/23/14   MDG 2.0 explicit transformation
    */

    void qu2ax(float* q, float* res);


    /**: qu2ax_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert quaternion to axis angle
    *
    * @param q quaternion (double precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    * @date 7/23/14   MDG 2.0 explicit transformation
    */

    void qu2ax(double* q, double* res);

    /**: qu2ro
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert quaternion to Rodrigues
    *
    * @param q quaternion (single precision)
    *
    * @date 8/12/13   MDG 1.0 original
    * @date 7/23/14   MDG 2.0 direct transformation
    * @date 8/11/14   MDG 2.1 added infty handling
    */

    void qu2ro(float* q, float* res);

    /**: qu2ro_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert quaternion to Rodrigues
    *
    * @param q quaternion (double precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    * @date 7/23/14   MDG 2.0 direct transformation
    * @date 8/11/14   MDG 2.1 added infty handling
    */

    void qu2ro(double* q, double* res);

    /**: qu2ho
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert quaternion to homochoric
    *
    * @param q quaternion (single precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    * @date 7/23/14   MDG 2.0 explicit transformation
    */

    void qu2ho(float* q, float* res);

    /**: qu2ho_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert quaternion to homochoric
    *
    * @param q quaternion (double precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    * @date 7/23/14   MDG 2.0 explicit transformation
    */

    void qu2ho(double* q, double* res);

    /**: ho2cu
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert homochoric to cubochoric
    *
    * @param h homochoric coordinates (single precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void ho2cu(float* h, float* res);

    /**: ho2cu_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert homochoric to cubochoric
    *
    * @param h homochoric coordinates (double precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void ho2cu(double* h, double* res);

    /**: cu2ho
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert cubochoric to homochoric
    *
    * @param c cubochoric coordinates (single precision)
    *
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void cu2ho(float* c, float* res);


    /**: cu2ho_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert cubochoric to homochoric
    *
    * @param c cubochoric coordinates (double precision)
    *
    *
    * @note calling program MUST have initialized the Lambert parameters first!!!
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void cu2ho(double* c, double* res);


    /**: ro2eu
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief Rodrigues vector to Euler angles (single precision)
    *
    * @param r Rodrigues vector (single precision)
    *
    *
    * @date 8/04/13   MDG 1.0 original
    * @date 8/11/14   MDG 1.1 added infty handling
    */

    void ro2eu(float* r, float* res);

    /**: ro2eu_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief Rodrigues vector to Euler angles (double precision)
    *
    * @param r Rodrigues vector (double precision)
    *
    *
    * @date 8/04/13   MDG 1.0 original
    * @date 8/11/14   MDG 1.1 added infty handling
    */

    void ro2eu(double* r, double* res);

    /**: eu2ho
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert euler to homochoric
    *
    * @param e 3 euler angles (single precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void eu2ho(float* e, float* res);

    /**: eu2ho_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert euler to homochoric
    *
    * @param e 3 euler angles (double precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void eu2ho(double* e, double* res);

    /**: om2ro
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert orientation matrix to Rodrigues
    *
    * @param om 3x3 orientation matrix (single precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */
    template<typename T, typename K>
    void om2ro(const T& r, T& res)
    {
      //*** use local
      //real(kind=sgl), INTENT(IN)              :: om(3,3)
      T eu(3); // Create a temp array to store the Euler Angles
      // Convert the OM to Euler
      om2eu<T, K>(r, eu);
      eu2ro<T, K>(eu, res);

      //res = eu2ro(om2eu(om));
    }


    /**: om2ho
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert orientation matrix to homochoric
    *
    * @param om 3x3 orientation matrix (single precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    * @date 07/08/14 MDG 2.0 simplification via ax (shorter path)
    */

    void om2ho(float* om, float* res);

    /**: om2ho_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert orientation matrix to homochoric
    *
    * @param om 3x3 orientation matrix (double precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    * @date 07/08/14 MDG 2.0 simplification via ax (shorter path)
    */

    void om2ho(double* om, double* res);

    /**: ax2eu
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert axis angle to euler
    *
    * @param a axis angle pair (single precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    * @date 07/08/14 MDG 2.0 simplification via ro (shorter path)
    */

    void ax2eu(float* a, float* res);

    /**: ax2eu_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert axis angle to euler
    *
    * @param a axis angle pair (double precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    * @date 07/08/14 MDG 2.0 simplification via ro (shorter path)
    */

    void ax2eu(double* a, double* res);

    /**: ro2om
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert rodrigues to orientation matrix
    *
    * @param r Rodrigues vector (single precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void ro2om(float* r, float* res);

    /**: ro2om_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert rodrigues to orientation matrix
    *
    * @param r Rodrigues vector (double precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void ro2om(double* r, double* res);

    /**: ro2qu
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert rodrigues to quaternion
    *
    * @param r Rodrigues vector (single precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void ro2qu(float* r, float* res);

    /**: ro2qu_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert rodrigues to quaternion
    *
    * @param r Rodrigues vector (double precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void ro2qu(double* r, double* res);

    /**: ho2eu
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert homochoric to euler
    *
    * @param h homochoric coordinates (single precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void ho2eu(float* h, float* res);

    /**: ho2eu_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert homochoric to euler
    *
    * @param h homochoric coordinates (double precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void ho2eu(double* h, double* res);

    /**: ho2om
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert homochoric to orientation matrix
    *
    * @param h homochoric coordinates (single precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void ho2om(float* h, float* res);

    /**: ho2om_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert homochoric to orientation matrix
    *
    * @param h homochoric coordinates (double precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void ho2om(double* h, double* res);

    /**: ho2ro
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert homochoric to Rodrigues
    *
    * @param h homochoric coordinates (single precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void ho2ro(float* h, float* res);

    /**: ho2ro_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert homochoric to Rodrigues
    *
    * @param h homochoric coordinates (double precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void ho2ro(double* h, double* res);

    /**: ho2qu
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert homochoric to quaternion
    *
    * @param h homochoric coordinates (single precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void ho2qu(float* h, float* res);

    /**: ho2qu_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert homochoric to quaternion
    *
    * @param h homochoric coordinates (double precision)
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void ho2qu(double* h, double* res);

    /**: eu2cu
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert euler angles to cubochoric
    *
    * @param e euler angles (single precision)
    *
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void eu2cu(float* e, float* res);

    /**: eu2cu_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert euler angles to cubochoric
    *
    * @param e euler angles (double precision)
    *
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void eu2cu(double* e, double* res);

    /**: om2cu
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert orientation matrix to cubochoric
    *
    * @param o orientation matrix (single precision)
    *
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void om2cu(float* o, float* res);

    /**: om2cu_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert orientation matrix to cubochoric
    *
    * @param o orientation matrix (double precision)
    *
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void om2cu(double* o, double* res);

    /**: ax2cu
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert axis angle to cubochoric
    *
    * @param a axis angle (single precision)
    *
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void ax2cu(float* a, float* res);

    /**: ax2cu_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert axis angle to cubochoric
    *
    * @param a axis angle (double precision)
    *
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void ax2cu(double* a, double* res);

    /**: ro2cu
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert Rodrigues to cubochoric
    *
    * @param r Rodrigues (single precision)
    *
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void ro2cu(float* r, float* res);

    /**: ro2cu_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert Rodrigues to cubochoric
    *
    * @param r Rodrigues (double precision)
    *
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void ro2cu(double* r, double* res);

    /**: qu2cu
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert quaternion to cubochoric
    *
    * @param q quaternion (single precision)
    *
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void qu2cu(float* q, float* res);

    /**: qu2cu_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert quaternion to cubochoric
    *
    * @param q quaternion (double precision)
    *
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void qu2cu(double* q, double* res);

    /**: cu2eu
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert cubochoric to euler angles
    *
    * @param c cubochoric coordinates (single precision)
    *
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void cu2eu(float* c, float* res);

    /**: cu2eu_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert cubochoric to euler angles
    *
    * @param c cubochoric coordinates (double precision)
    *
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void cu2eu(double* c, double* res);

    /**: cu2om
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert cubochoric to orientation matrix
    *
    * @param c cubochoric coordinates (single precision)
    *
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void cu2om(float* c, float* res);

    /**: cu2om_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert cubochoric to orientation matrix
    *
    * @param c cubochoric coordinates  (double precision)
    *
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void cu2om(double* c, double* res);

    /**: cu2ax
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert cubochoric to axis angle
    *
    * @param c cubochoric coordinates (single precision)
    *
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void cu2ax(float* c, float* res);

    /**: cu2ax_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert cubochoric to axis angle
    *
    * @param c cubochoric coordinates  (double precision)
    *
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void cu2ax(double* c, double* res);

    /**: cu2ro
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert cubochoric to Rodrigues
    *
    * @param c cubochoric coordinates (single precision)
    *
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void cu2ro(float* c, float* res);

    /**: cu2ro_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert cubochoric to Rodrigues
    *
    * @param c cubochoric coordinates  (double precision)
    *
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void cu2ro(double* c, double* res);

    /**: cu2qu
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert cubochoric to quaternion
    *
    * @param c cubochoric coordinates (single precision)
    *
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void cu2qu(float* c, float* res);

    /**: cu2qu_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief convert cubochoric to quaternion
    *
    * @param c cubochoric coordinates  (double precision)
    *
    *
    *
    * @date 8/12/13   MDG 1.0 original
    */

    void cu2qu(double* c, double* res);

    /**: RotVec_om
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief rotate a vector using a rotation matrix, active or passive (single precision)
    *
    * @details This routine provides a way for the user to transform a vector
    * and it returns the new vector components.  The user can use either a
    * rotation matrix or a quaternion to define the transformation, and must
    * also specifiy whether an active or passive result is needed.
    *
    *
    * @param vec input vector components (single precision)
    * @param om orientation matrix (single precision)
    * @param ap active/passive switch
    *
    *
    * @date 8/18/14   MDG 1.0 original
    */

    void RotVec_om(float* vec, float* om, char ap, float* res);

    /**: RotVec_om_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief rotate a vector using a rotation matrix, active or passive (double precision)
    *
    * @details This routine provides a way for the user to transform a vector
    * and it returns the new vector components.  The user can use either a
    * rotation matrix or a quaternion to define the transformation, and must
    * also specifiy whether an active or passive result is needed.
    *
    *
    * @param vec input vector components (double precision)
    * @param om orientation matrix (double precision)
    * @param ap active/passive switch
    *
    *
    * @date 8/18/14   MDG 1.0 original
    */

    void RotVec_om(double* vec, double* om, char ap, double* res);

    /**: RotVec_qu
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief rotate a vector using a quaternion, active or passive (single precision)
    *
    * @details This routine provides a way for the user to transform a vector
    * and it returns the new vector components.  The user can use either a
    * rotation matrix or a quaternion to define the transformation, and must
    * also specifiy whether an active or passive result is needed.
    *
    *
    * @param vec input vector components (single precision)
    * @param qu quaternion (single precision)
    * @param ap active/passive switch
    *
    *
    * @date 8/18/14   MDG 1.0 original
    */

    void RotVec_qu(float* vec, float* qu, char ap, float* res);

    /**: RotVec_qu_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief rotate a vector using a quaternion, active or passive (double precision)
    *
    * @details This routine provides a way for the user to transform a vector
    * and it returns the new vector components.  The user can use either a
    * rotation matrix or a quaternion to define the transformation, and must
    * also specifiy whether an active or passive result is needed.
    *
    *
    * @param vec input vector components (double precision)
    * @param qu quaternion (double precision)
    * @param ap active/passive switch
    *
    *
    * @date 8/18/14   MDG 1.0 original
    */

    void RotVec_qu(double* vec, float* qu, char ap, double* res);

    /**: RotTensor2_om
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief rotate a second rank tensor using a rotation matrix, active or passive (single precision)
    *
    * @param tensor input tensor components (single precision)
    * @param om orientation matrix (single precision)
    * @param ap active/passive switch
    *
    *
    * @date 8/18/14   MDG 1.0 original
    */

    void RotTensor2_om(float* tensor, float* om, char ap, float* res);

    /**: RotTensor2_om_d
    *
    * @author Marc De Graef, Carnegie Mellon University
    *
    * @brief rotate a second rank tensor using a rotation matrix, active or passive (double precision)
    *
    * @param tensor input tensor components (double precision)
    * @param om orientation matrix (double precision)
    * @param ap active/passive switch
    *
    *
    * @date 8/18/14   MDG 1.0 original
    */

    void RotTensor2_om(double* tensor, double* om, char ap, double* res);

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
    template<typename T>
    T multiply(const T& a, const T& b)
    {
      T c(a.size());
      for(size_t i = 0; i < c.size(); i++)
      {
        c[i] = a[i] * b[i];
      }
      return c;
    }

    template<typename T>
    T multiply(const T& a, const T& b, size_t max)
    {
      T c(max);
      for(size_t i = 0; i < max; i++)
      {
        c[i] = a[i] * b[i];
      }
      return c;
    }

    template<typename T, typename K>
    K sum(const T& a)
    {
      K s = static_cast<K>(0);
      for(size_t i = 0; i < a.size(); i++)
      {
        s += a[i];
      }
      return s;
    }

    template<typename T, typename K>
    K sum(const T& a, size_t max)
    {
      K s = static_cast<K>(0);
      for(size_t i = 0; i < max; i++)
      {
        s += a[i];
      }
      return s;
    }

    template<typename T, typename K>
    K maxval(const T& a)
    {
      K s = a[0];
      for(size_t i = 1; i < a.size(); i++)
      {
        if (a[i] > s) { s = a[i]; }
      }
      return s;
    }

    template<typename T>
    T absValue(const T& a)
    {
      T c(a.size());
      for(size_t i = 0; i < c.size(); i++)
      {
        c[i] = (a[i] < 0.0 ? -a[i] : a[i]);
      }
      return c;
    }

    template<typename T>
    T transpose(const T& a)
    {
      T c(a.size());
      c[0] = a[0];
      c[1] = a[3];
      c[2] = a[6];
      c[3] = a[1];
      c[4] = a[4];
      c[5] = a[7];
      c[6] = a[2];
      c[7] = a[5];
      c[8] = a[8];
      return c;
    }

    template<typename T>
    T matmul3x3(const T& a, const T& b)
    {
      T c(a.size());
      c[0] = a[0] * b[0] + a[1] * b[3] + a[2] * b[6];
      c[1] = a[0] * b[1] + a[1] * b[4] + a[2] * b[7];
      c[2] = a[0] * b[2] + a[1] * b[5] + a[2] * b[8];
      c[3] = a[3] * b[0] + a[4] * b[3] + a[5] * b[6];
      c[4] = a[3] * b[1] + a[4] * b[4] + a[5] * b[7];
      c[5] = a[3] * b[2] + a[4] * b[5] + a[5] * b[8];
      c[6] = a[6] * b[0] + a[7] * b[3] + a[8] * b[6];
      c[7] = a[6] * b[1] + a[7] * b[4] + a[8] * b[7];
      c[8] = a[6] * b[2] + a[7] * b[5] + a[8] * b[8];
      return c;
    }


  private:
    RotationTransforms(const RotationTransforms&); // Copy Constructor Not Implemented
    void operator=(const RotationTransforms&); // Operator '=' Not Implemented
};
#endif /* _RotationTransforms_H_ */
