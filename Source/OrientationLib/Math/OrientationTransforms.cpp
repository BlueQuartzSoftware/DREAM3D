/* ============================================================================
 * Copyright (c) 2015 BlueQuartz Software, LLC
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
 * Neither the name of Michael A. Jackson, BlueQuartz Software nor the names of
 * its contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
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
 *  This code was written under United States Air Force Contract number
 *                 FA8650-07-D-5800 & FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "OrientationTransforms.h"


#include <iostream>


#if 0
/**
 *@brief Calculates the Inner Product of the 2 arrays.
 */
template<typename T>
T sum(const RotArray<T>& a, const RotArray<T>& b)
{
  RotArray<T> out = a * b;
  return out.sum();
}

/**
 *@ brief Finds the maximum value of an array
 */
template<typename T>
T maxval(T* val, size_t c)
{
  T max = val[0];
  for(size_t i = 1; i < c; i++)
  {
    if(val[i] > max) { max = val[1]; }
  }
  return max;
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationTransforms::OrientationTransforms()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationTransforms::~OrientationTransforms()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::FatalError(const std::string &func, const std::string &msg)
{
  std::cout << func << "::" << msg << std::endl;
}


#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::eu_check(const RotArrayF& eu, int* res)
{
  //*** use local
  //*** use constants
  //*** use error
  //*** IMPLICIT NONE
  //real(kind=sgl),INTENT(IN)       :: eu[2]
  //integer(kind=irg)               :: res
  *res = 1;
  if ((eu[0] < 0.0) || (eu[0] > (2.0*M_PI))) {
    FatalError("rotations:eu_check","phi1 Euler angle outside of valid range [0,2pi]");  *res = 0;
  }
  if ((eu[1] < 0.0) || (eu[1] > M_PI)) {
    FatalError("rotations:eu_check","Phi  Euler angle outside of valid range [0,pi]");  *res = 0;
  }
  if ((eu[2] < 0.0) || (eu[2] > (2.0*M_PI))) {
    FatalError("rotations:eu_check","phi2 Euler angle outside of valid range [0,2pi]");  *res = 0;
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::eu_check(const RotArrayD &eu, int* res)
{
  //*** use local
  //*** use constants
  //*** use error
  //*** IMPLICIT NONE
  //real(kind=dbl),INTENT(IN)       :: eu[2]
  //integer(kind=irg)               :: res
  *res = 1;
  if ((eu[0] < 0.0) || (eu[0] > (2.0*M_PI))) {
    FatalError("rotations:eu_check_d","phi1 Euler angle outside of valid range [0,2pi]");  *res = 0;
  }
  if ((eu[1] < 0.0) || (eu[1] > M_PI)) {
    FatalError("rotations:eu_check_d","Phi  Euler angle outside of valid range [0,pi]");  *res = 0;
  }
  if ((eu[2] < 0.0) || (eu[2] > (2.0*M_PI))) {
    FatalError("rotations:eu_check_d","phi2 Euler angle outside of valid range [0,2pi]");  *res = 0;
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ro_check(const RotArrayF &ro, int* res)
{
  //*** use local
  //*** use constants
  //*** use error
  //*** IMPLICIT NONE
  //real(kind=sgl),INTENT(IN)       :: ro[3]
  //integer(kind=irg)               :: res
  float  eps = 1.e-10;

  *res = 1;
  if (ro[3] < 0.0) {
    FatalError("rotations:ro_check","Rodrigues-Frank vector has negative length");  *res = 0;
  }

  RotArrayF out = ro * ro;
  float ttl = out.sum();
  if (abs(sqrt(ttl)-1.0) > eps) {
    FatalError("rotations:ro_check","Rodrigues-Frank axis vector not normalized");  *res = 0;
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ro_check(const RotArrayD &ro, int* res)
{
  //*** use local
  //*** use constants
  //*** use error
  //*** IMPLICIT NONE
  //real(kind=dbl),INTENT(IN)       :: ro[3]
  //integer(kind=irg)               :: res
  double  eps = 1E-15;

  *res = 1;
  if (ro[3] < 0.0) {
    FatalError("rotations:ro_check_d","Rodrigues-Frank vector has negative length");  *res = 0;
  }
  RotArrayD out = ro * ro;
  double ttl = out.sum();
  if (abs(sqrt(ttl)-1.0) > eps) {
    FatalError("rotations:ro_check_d","Rodrigues-Frank axis vector not normalized");  *res = 0;
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ho_check(const RotArrayF& ho, int* res)
{
  //*** use local
  //*** use constants
  //*** use error
  //*** IMPLICIT NONE
  //real(kind=sgl),INTENT(IN)       :: ho[2]
  //integer(kind=irg)               :: res
  float  r;

  *res = 1;
  RotArrayF out = ho * ho;
  float ttl = out.sum();
  r = sqrt(ttl);
  if (r > static_cast<float>(LPs::R1)) {
    FatalError("rotations:ho_check","homochoric vector outside homochoric ball");
    *res = 0;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ho_check(const RotArrayD& ho, int* res)
{
  //*** use local
  //*** use constants
  //*** use error
  //*** IMPLICIT NONE
  //real(kind=dbl),INTENT(IN)       :: ho[2]
  //integer(kind=irg)               :: res
  double  r;

  *res = 1;
  RotArrayD out = ho * ho;
  double ttl = out.sum();
  r = sqrt(ttl);
  if (r > LPs::R1) {
    FatalError("rotations:ho_check","homochoric vector outside homochoric ball");
    *res = 0;
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::cu_check(const RotArrayF& cu, int* res)
{
  //*** use local
  //*** use constants
  //*** use error
  //*** IMPLICIT NONE
  //real(kind=sgl),INTENT(IN)       :: cu[2]
  //integer(kind=irg)               :: res
  float  r;

  *res = 1;
  r = Detail::maxval(Detail::abs(cu));
  if (r > static_cast<float>(LPs::ap/2.0)) {
    FatalError("rotations:cu_check","cubochoric vector outside cube");
  }
  *res = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::cu_check(const RotArrayD& cu, int* res)
{
  //*** use local
  //*** use constants
  //*** use error
  //*** IMPLICIT NONE
  //real(kind=dbl),INTENT(IN)       :: cu[2]
  //integer(kind=irg)               :: res
  double  r;

  *res = 1;
  r = Detail::maxval(Detail::abs(cu));
  if (r > (LPs::ap/2.0)) {
    FatalError("rotations:cu_check_d","cubochoric vector outside cube");
  }
  *res = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::qu_check(float* qu, float* res)
{
  //*** use local
  //*** use constants
  //*** use error
  //*** IMPLICIT NONE
  //real(kind=sgl),INTENT(IN)       :: qu[2]
  //integer(kind=irg)               :: res
  float  r;

  float  eps = 1.e-10;

  *res = 1;
  r = sqrt(sum(qu*qu));
  if (qu[0] < 0.0) {
    FatalError("rotations:qu_check","quaternion must have positive scalar part");
  }
  if (abs(r-1.0) > eps) {
    FatalError("rotations:qu_check","quaternion must have unit norm");
  }
  *res = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::qu_check(double* qu, double* res)
{
  //*** use local
  //*** use constants
  //*** use error
  //*** IMPLICIT NONE
  //real(kind=dbl),INTENT(IN)       :: qu[2]
  //integer(kind=irg)               :: res
  double  r;

  double  eps = 1.d-15;

  *res = 1;
  r = sqrt(sum(qu*qu))
      if (qu[0] < 0.0) {
    FatalError("rotations:qu_check_d","quaternion must have positive scalar part");
  }
  if (abs(r-1.0) > eps) {
    FatalError("rotations:qu_check_d","quaternion must have unit norm");
  }
  *res = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ax_check(float* ax, float* res)
{
  //*** use local
  //*** use constants
  //*** use error
  //*** IMPLICIT NONE
  //real(kind=sgl),INTENT(IN)       :: ax[3]
  //integer(kind=irg)               :: res
  float  r;

  float  eps = 1.e-10;

  *res = 1;
  r = sqrt(sum(ax(1:3)*ax(1:3)));
  if ((ax[3] < 0.0) || (ax[3] > M_PI)) {
    FatalError("rotations:ax_check","angle must be in range [0,pi]");
  }
  if (abs(r-1.0) > eps) {
    FatalError("rotations:ax_check","axis-angle axis vector must have unit norm");
  }
  *res = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ax_check_D(float* ax, float* res)
{
  //*** use local
  //*** use constants
  //*** use error
  //*** IMPLICIT NONE
  //real(kind=dbl),INTENT(IN)       :: ax[3]
  //integer(kind=irg)               :: res
  float  r;

  float  eps = 1.e-15;

  *res = 1;
  r = sqrt(sum(ax(1:3)*ax(1:3)));
      if ((ax[3] < 0.0) || (ax[3] > M_PI)) {
    FatalError("rotations:ax_check_d","angle must be in range [0,pi]");
  }
  if (abs(r-1.0) > eps) {
    FatalError("rotations:ax_check_d","axis-angle axis vector must have unit norm");
  }
  *res = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::om_check(float* om, float* res)
{
  //*** use local
  //*** use constants
  //*** use error
  //*** IMPLICIT NONE
  //real(kind=sgl),INTENT(IN)       :: om(3,3)
  //integer(kind=irg)               :: res
  float  r;
  float det;

  float  eps = 1.e-10;

  float i;
  *res = 1;
  det = om(1,1)*om(2,2)*om(3,3) + om(1,2)*om(2,3)*om(3,1) + om(1,3)*om(2,1)*om(3,2) - &;
  om(1,3)*om(2,2)*om(3,1) - om(1,2)*om(2,1)*om(3,3) - om(1,1)*om(2,3)*om(3,2) ;
  if (det < 0.0) {
    FatalError("rotations:om_check","Determinant of rotation matrix must be positive");
  }
  if (abs(det-1.0) > eps) {
    FatalError("rotations:om_check","Determinant of rotation matrix must be unity");
  }
  r = sum(abs(matmul(om,transpose(om))));
  if (abs(r-3.0) > eps) {
    FatalError("rotations:om_check","rotation matrix times transpose must be identity matrix");
  }
  *res = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::om_check(double* om, double* res)
{
  //*** use local
  //*** use constants
  //*** use error
  //*** IMPLICIT NONE
  //real(kind=dbl),INTENT(IN)       :: om(3,3)
  //integer(kind=irg)               :: res
  double  r;
  double det;

  double  eps = 1.e-15;

  double i;
  *res = 1;
  det = om(1,1)*om(2,2)*om(3,3) + om(1,2)*om(2,3)*om(3,1) + om(1,3)*om(2,1)*om(3,2) - &;
  om(1,3)*om(2,2)*om(3,1) - om(1,2)*om(2,1)*om(3,3) - om(1,1)*om(2,3)*om(3,2) ;
  if (det < 0.0) {
    FatalError("rotations:om_check_d","Determinant of rotation matrix must be positive");
  }
  if (abs(det-1.0) > eps) {
    FatalError("rotations:om_check_d","Determinant of rotation matrix must be unity");
  }
  r = sum(abs(matmul(om,transpose(om))))
      if (abs(r-3.0) > eps) {
      FatalError("rotations:om_check_d","rotation matrix times transpose must be identity matrix");
}
      *res = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::genrot(float* av, float omega, float* res)
{
  //*** use local
  //*** use constants
  //*** use error
  //*** IMPLICIT NONE
  //real(kind=sgl),INTENT(IN)       :: av[2]
  //real(kind=sgl),INTENT(IN)       :: omega
  type(orientationtype)           :: res;
  float  axang[4];
  float s;

  if ((omega < 0.0) || (omega > M_PI)) {
    FatalError("rotations:getrot","rotation angle must be in range [0,pi]");
    STOP;
  }
  axang(1:3) = -epsijk * av(1:3);
  axang[3] = omega
      s = sqrt(sum(av*av));
  if (s != 0.0) {
    axang(1:3) = axang(1:3)/s;
  } else {
    FatalError("rotations:getrot","axis unit vector can not be [0,0,0]");
    STOP;
  }
  res = init_orientation(axang,'ax');
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::genrot(double* av, double omega, double* res)
{
  //*** use local
  //*** use constants
  //*** use error
  //*** IMPLICIT NONE
  //real(kind=dbl),INTENT(IN)       :: av[2]
  //real(kind=dbl),INTENT(IN)       :: omega
  type(orientationtyped)          :: res;
  double  axang[4];
  double s;

  if ((omega < 0.0) || (omega > M_PI)) {
    FatalError("rotations:getrot_d","rotation angle must be in range [0,pi]");
    STOP;
  }
  axang(1:3) = -epsijk * av(1:3);
  axang[3] = omega
      s = sqrt(sum(av*av))
      if (s != 0.0) {
      axang(1:3) = axang(1:3)/s;
} else {
      FatalError("rotations:getrot","axis unit vector can not be [0,0,0]");
      STOP;
}
      res = init_orientation_d(axang,'ax');
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::init_orientation(float* orient, char intype[2], bool rotcheck, float* res)
{
  //*** use local
  //*** IMPLICIT NONE
  //real(kind=sgl),INTENT(IN)       :: orient(*)            //3 or 4 component orientation descriptor
  //character(2),INTENT(IN)         :: intype               //describes input type
  //logical,INTENT(IN),OPTIONAL     :: rotcheck             //check valid range if present
  type(orientationtype)           :: res;
  float i;
  select case (intype);
  case ('eu')     //Euler angles
  //verify the Euler angle ranges; this will abort program if values are outside range
  if (present(rotcheck)) {
  if (rotcheck == true ) i = eu_check(orient)
}
  res.eulang = orient(1:3)
    res.om = eu2om(orient(1:3))
        res.quat = eu2qu(orient(1:3))
        res.rodrigues = eu2ro(orient(1:3))
        res.axang = eu2ax(res.eulang)
        res.homochoric = ax2ho(res.axang)
        res.cubochoric = ho2cu(res.homochoric)
      case ('ro')     //Rodrigues vector
        //verify the Rodrigues-Frank vector; this will abort program if values are outside range
        if (present(rotcheck)) {
        if (rotcheck == true ) i = ro_check(orient)
}
        res.rodrigues = orient(1:4)
        res.eulang = ro2eu(orient(1:4))
        res.om = eu2om(res.eulang)
        res.quat = eu2qu(res.eulang)
        res.axang = ro2ax(res.rodrigues)
        res.homochoric = ax2ho(res.axang)
        res.cubochoric = ho2cu(res.homochoric)
      case ('ho')     //homochoric
        //verify the homochoric vector; this will abort program if values are outside range
        if (present(rotcheck)) {
        if (rotcheck == true ) i = ho_check(orient)
}
        res.homochoric = orient(1:3)
        res.axang = ho2ax(orient(1:3))
        res.om = ax2om(res.axang)
        res.eulang = om2eu(res.om)
        res.rodrigues = eu2ro(res.eulang)
        res.quat = eu2qu(res.eulang)
        res.cubochoric = ho2cu(res.homochoric)
      case ('cu')     //cubochoric
        //verify the cubochoric vector; this will abort program if values are outside range
        if (present(rotcheck)) {
        if (rotcheck == true ) i = cu_check(orient)
}
        res.cubochoric = orient(1:3)
        res.homochoric = cu2ho(res.cubochoric)
        res.eulang = cu2eu(res.cubochoric)
        res.om = cu2om(res.cubochoric)
        res.quat = cu2qu(res.cubochoric)
        res.axang = cu2ax(res.cubochoric)
        res.rodrigues = cu2ro(res.cubochoric)
      case ('qu')     //quaternion
        //verify the quaternion; this will abort program if values are outside range
        if (present(rotcheck)) {
        if (rotcheck == true ) i = qu_check(orient)
}
        res.quat = orient(1:4)
        res.eulang = qu2eu(res.quat)
        res.om = eu2om(res.eulang)
        res.rodrigues = eu2ro(res.eulang)
        res.axang = ro2ax(res.rodrigues)
        res.homochoric = ax2ho(res.axang)
        res.cubochoric = ho2cu(res.homochoric)
      case ('ax')     //axis angle pair
        //verify the axis angle pair; this will abort program if values are outside range
        if (present(rotcheck)) {
        if (rotcheck == true ) i = ax_check(orient)
}
        res.axang = orient(1:4)
        res.om = ax2om(orient(1:4))
        res.eulang = om2eu(res.om)
        res.rodrigues = eu2ro(res.eulang)
        res.quat = eu2qu(res.eulang)
        res.homochoric = ax2ho(res.axang)
        res.cubochoric = ho2cu(res.homochoric)
        end select ;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::init_orientation(double* orient, char intype[2], bool rotcheck, double* res)
{
  //*** use local
  //*** use error
  //*** use constants
  //*** IMPLICIT NONE
  //real(kind=dbl),INTENT(IN)       :: orient(*)            //3-component orientation descriptor
  //character(2),INTENT(IN)         :: intype               //describes input type
  //logical,INTENT(IN),OPTIONAL     :: rotcheck             //check valid range if present
  type(orientationtyped)          :: res;
  double i;
  select case (intype);
  case ('eu')     //Euler angles
  //verify the Euler angle ranges; this will abort program if values are outside range
  if (present(rotcheck)) {
  if (rotcheck == true ) i = eu_check_d(orient)
}
  res.eulang = orient(1:3)
    res.om = eu2om_d(orient(1:3))
        res.quat = eu2qu_d(orient(1:3))
        res.rodrigues = eu2ro_d(orient(1:3))
        res.axang = eu2ax_d(res.eulang)
        res.homochoric = ax2ho_d(res.axang)
        res.cubochoric = ho2cu_d(res.homochoric)
      case ('ro')     //Rodrigues vector
        //verify the Rodrigues-Frank vector; this will abort program if values are outside range
        if (present(rotcheck)) {
        if (rotcheck == true ) i = ro_check_d(orient)
}
        res.rodrigues = orient(1:4)
        res.eulang = ro2eu_d(orient(1:4))
        res.om = eu2om_d(res.eulang)
        res.quat = eu2qu_d(res.eulang)
        res.axang = ro2ax_d(res.rodrigues)
        res.homochoric = ax2ho_d(res.axang)
        res.cubochoric = ho2cu_d(res.homochoric)
      case ('ho')     //homochoric
        //verify the homochoric vector; this will abort program if values are outside range
        if (present(rotcheck)) {
        if (rotcheck == true ) i = ho_check_d(orient)
}
        res.homochoric = orient(1:3)
        res.axang = ho2ax_d(orient(1:3))
        res.om = ax2om_d(res.axang)
        res.eulang = om2eu_d(res.om)
        res.rodrigues = eu2ro_d(res.eulang)
        res.quat = eu2qu_d(res.eulang)
        res.cubochoric = ho2cu_d(res.homochoric)
      case ('cu')     //cubochoric
        //verify the cubochoric vector; this will abort program if values are outside range
        if (present(rotcheck)) {
        if (rotcheck == true ) i = cu_check_d(orient)
}
        res.cubochoric = orient(1:3)
        res.homochoric = cu2ho_d(res.cubochoric)
        res.eulang = cu2eu_d(res.cubochoric)
        res.om = cu2om_d(res.cubochoric)
        res.quat = cu2qu_d(res.cubochoric)
        res.axang = cu2ax_d(res.cubochoric)
        res.rodrigues = cu2ro_d(res.cubochoric)
      case ('qu')     //quaternion
        //verify the quaternion; this will abort program if values are outside range
        if (present(rotcheck)) {
        if (rotcheck == true ) i = qu_check_d(orient)
}
        res.quat = orient(1:4)
        res.eulang = qu2eu_d(res.quat)
        res.om = eu2om_d(res.eulang)
        res.rodrigues = eu2ro_d(res.eulang)
        res.axang = ro2ax_d(res.rodrigues)
        res.homochoric = ax2ho_d(res.axang)
        res.cubochoric = ho2cu_d(res.homochoric)
      case ('ax')     //axis angle pair
        //verify the axis angle pair; this will abort program if values are outside range
        if (present(rotcheck)) {
        if (rotcheck == true ) i = ax_check_d(orient)
}
        res.axang = orient(1:4)
        res.om = ax2om_d(orient(1:4))
        res.eulang = om2eu_d(res.om)
        res.rodrigues = eu2ro_d(res.eulang)
        res.quat = eu2qu_d(res.eulang)
        res.homochoric = ax2ho_d(res.axang)
        res.cubochoric = ho2cu_d(res.homochoric)
        end select ;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::init_orientation_om(float* orient, char intype[2], bool rotcheck, float* res)
{
  //*** use local
  //*** IMPLICIT NONE
  //real(kind=sgl),INTENT(IN)       :: orient(3,3)          //3x3-component orientation descriptor
  //character(2),INTENT(IN)         :: intype               //describes input type
  //logical,INTENT(IN),OPTIONAL     :: rotcheck             //check valid range if present
  type(orientationtype)           :: res;
  float i;
  select case (intype);
  case ('om')     //orientation matrix
  //verify the rotation matrix; this will abort program if values are outside range
  if (present(rotcheck)) {
  if (rotcheck == true ) i = om_check(orient)
}
  res.om = orient
  res.quat = om2qu(orient)
  res.eulang = qu2eu(res.quat)
  res.rodrigues = eu2ro(res.eulang)
  res.axang = ro2ax(res.rodrigues)
  res.homochoric = ax2ho(res.axang)
  res.cubochoric = ho2cu(res.homochoric)
  end select ;
}

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void OrientationTransforms::init_orientation_om(double* orient, char intype[2], bool rotcheck, double* res)
  {
  //*** use local
  //*** IMPLICIT NONE
  //real(kind=dbl),INTENT(IN)       :: orient(3,3)          //3x3-component orientation descriptor
  //character(2),INTENT(IN)         :: intype               //describes input type
  //logical,INTENT(IN),OPTIONAL     :: rotcheck             //check valid range if present
  type(orientationtyped)          :: res;
  double i;
  select case (intype);
  case ('om')     //orientation matrix
  //verify the rotation matrix; this will abort program if values are outside range
  if (present(rotcheck)) {
  if (rotcheck == true ) i = om_check_d(orient)
}
  res.om = orient
  res.quat = om2qu_d(orient)
  res.eulang = qu2eu_d(res.quat)
  res.rodrigues = eu2ro_d(res.eulang)
  res.axang = ro2ax_d(res.rodrigues)
  res.homochoric = ax2ho_d(res.axang)
  res.cubochoric = ho2cu_d(res.homochoric)
  end select ;
}

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void OrientationTransforms::eu2om(float* e, float* res)
  {
  //*** use local
  //*** IMPLICIT NONE
  //real(kind=sgl),INTENT(IN)       :: e[2]         // Euler angles in radians

  float  c1;
  float c2;
  float c3;
  float s1;
  float s2;
  float s3;

  float i,j;
  float  thr = 1.0E-8;

  c1 = cos(e[0])
  c2 = cos(e[1])
  c3 = cos(e[2])
  s1 = sin(e[0])
  s2 = sin(e[1])
  s3 = sin(e[2])
  res(1,1) = c1*c3-s1*s3*c2;
  res(1,2) = s1*c3+c1*s3*c2;
  res(1,3) = s3*s2;
  res(2,1) = -c1*s3-s1*c3*c2;
  res(2,2) = -s1*s3+c1*c3*c2;
  res(2,3) = c3*s2;
  res(3,1) = s1*s2;
  res(3,2) = -c1*s2;
  res(3,3) = c2;
  do i=1,3;
  do j=1,3;
  if (abs(res(i,j)) < thr) res(i,j) = 0.0
  end do;
  end do;
}

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void OrientationTransforms::eu2om(double* e, double* res)
  {
  //*** use local
  //*** IMPLICIT NONE
  //real(kind=dbl),INTENT(IN)       :: e[2]         // Euler angles in radians

  double  c1;
  double c2;
  double c3;
  double s1;
  double s2;
  double s3;

  double i,j;
  double  thr = 1.0D-10;

  c1 = cos(e[0])
  c2 = cos(e[1])
  c3 = cos(e[2])
  s1 = sin(e[0])
  s2 = sin(e[1])
  s3 = sin(e[2])
  res(1,1) = c1*c3-s1*s3*c2;
  res(1,2) = s1*c3+c1*s3*c2;
  res(1,3) = s3*s2;
  res(2,1) = -c1*s3-s1*c3*c2;
  res(2,2) = -s1*s3+c1*c3*c2;
  res(2,3) = c3*s2;
  res(3,1) = s1*s2;
  res(3,2) = -c1*s2;
  res(3,3) = c2;
  do i=1,3;
  do j=1,3;
  if (abs(res(i,j)) < thr) res(i,j) = 0.0
  end do;
  end do;
}

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void OrientationTransforms::eu2ax(float* e, float* res)
  {
  //*** use local
  //*** use constants
  //real(kind=sgl), INTENT(IN)              :: e[2]
  float t;
  float del;
  float tau;
  float alpha;
  float sig;

  float  thr = 1.0E-6;

  t = tan(e[1]*0.5)
  sig = 0.5*(e[0]+e[2])
  del = 0.5*(e[0]-e[2])
  tau = sqrt(t*t+sin(sig)**2);
  if (sig == M_PI*0.5) {  //Infinity
  alpha = M_PI
} else {
  alpha = 2.0 * atan(tau/cos(sig));
}
  if (abs(alpha) < thr) {
  res = (/ 0.0, 0.0, 1.0, 0.0 /);
} else {
  res(1:3) = - epsijk * (/ t*cos(del), t*sin(del), sin(sig) /) / tau;
    res(4) = alpha;
}
if (alpha < 0.0) res = -res
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::eu2ax(double* e, double* res)
{
  //*** use local
  //*** use constants
  //real(kind=dbl), INTENT(IN)              :: e[2]
  double del;
  double t;
  double tau;
  double alpha;
  double sig;

  double  thr = 1.0D-10;

  t = tan(e[1]*0.5D0)
      sig = 0.5D0*(e[0]+e[2])
      del = 0.5D0*(e[0]-e[2])
      tau = sqrt(t*t+sin(sig)**2)
      if (sig == M_PI*0.5D0) {  //Infinity
      alpha = M_PI
} else {
      alpha = 2.0 * atan(tau/cos(sig))
}
      if (abs(alpha) < thr) {
      res = (/ 0.0, 0.0, 1.0, 0.0 /)
} else {
      res(1:3) = - epsijkd *(/ t*cos(del), t*sin(del), sin(sig) /) / tau
      res(4) = alpha;
}
      if (alpha < 0.0) res = -res
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::eu2ro(float* e, float* res)
{
  //*** use local
  //*** use constants
  //*** IMPLICIT NONE
  //real(kind=sgl),INTENT(IN)       :: e[2]         // input Euler angles (radians)

  float  t;

  float  thr = 1.0E-7;

  res = eu2ax(e);
  t = res(4);
  if (abs(t-M_PI) < thr) {
    res(4) = infty;
    return;
  }
  if (t == 0.0) {
    res = (/ 0.0, 0.0, 0.0, 0.0 /);
  } else {
    res(4) = tan(t*0.5);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::eu2ro(double* e, double* res)
{
  //*** use local
  //*** use constants
  //*** IMPLICIT NONE
  //real(kind=dbl),INTENT(IN)       :: e[2]         // input Euler angles (radians)

  double  t;

  double  thr = 1.0D-10;

  res = eu2ax_d(e);
  t = res(4);
  if (abs(t-M_PI) < thr) {
    res(4) = infty;
    return;
  }
  if (t == 0.0) {
    res = (/ 0.0, 0.0, 0.0, 0.0 /)
  } else {
    res(4) = tan(t*0.5D0)
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::eu2qu(float* e, float* res)
{
  //*** use local
  //*** use constants
  //*** IMPLICIT NONE
  //real(kind=sgl),INTENT(IN)       :: e[2]         // input Euler angles in radians

  float  ee[3];
  float cPhi;
  float cp;
  float cm;
  float sPhi;
  float sp;
  float sm;

  ee = 0.5*e;
  cPhi = cos(ee[1])
      sPhi = sin(ee[1])
      cm = cos(ee[0]-ee[2])
      sm = sin(ee[0]-ee[2])
      cp = cos(ee[0]+ee[2])
      sp = sin(ee[0]+ee[2])
      res = (/ cPhi*cp, -epsijk*sPhi*cm, -epsijk*sPhi*sm, -epsijk*cPhi*sp /);
  if (res(1) < 0.0) res = -res
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::eu2qu(double* e, double* res)
{
  //*** use local
  //*** use constants
  //*** IMPLICIT NONE
  //real(kind=dbl),INTENT(IN)       :: e[2]         // input Euler angles in radians

  double  ee[3];
  double cPhi;
  double cp;
  double cm;
  double sPhi;
  double sp;
  double sm;

  ee = 0.5D0*e;
  cPhi = cos(ee[1])
      sPhi = sin(ee[1])
      cm = cos(ee[0]-ee[2])
      sm = sin(ee[0]-ee[2])
      cp = cos(ee[0]+ee[2])
      sp = sin(ee[0]+ee[2])
      res = (/ cPhi*cp, -epsijkd*sPhi*cm, -epsijkd*sPhi*sm, -epsijkd*cPhi*sp /);
  if (res(1) < 0.0) res = -res
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::om2eu(float* o, float* res)
{
  //*** use local
  //*** use constants
  //*** IMPLICIT NONE
  //real(kind=sgl),INTENT(IN)       :: o(3,3)               // orientation matrix
  float zeta;

  if (abs(o(3,3)) != 1.0) {
    res(2) = acos(o(3,3));
    zeta = 1.0/sqrt(1.0-o(3,3)**2);
    res(1) = atan2(o(3,1)*zeta,-o(3,2)*zeta);
    res(3) = atan2(o(1,3)*zeta, o(2,3)*zeta);
  } else {
    if (o(3,3) == 1.0) {
      res(1) = atan2( o(1,2),o(1,1));
      res(2) = 0.0;
      res(3) = 0.0;
    } else {
      res(1) =-atan2(-o(1,2),o(1,1));
      res(2) = M_PI
          res(3) = 0.0;
    }
  }
  if (res(1) < 0.0) res(1) = mod(res(1)+100.0*M_PI,2.0*M_PI)
      if (res(2) < 0.0) res(2) = mod(res(2)+100.0*M_PI,M_PI)
      if (res(3) < 0.0) res(3) = mod(res(3)+100.0*M_PI,2.0*M_PI)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::om2eu(double* o, double* res)
{
  //*** use local
  //*** use constants
  //*** IMPLICIT NONE
  //real(kind=dbl),INTENT(IN)       :: o(3,3)               // orientation matrix
  double zeta;

  double  thr = 1.0D-8;

  if (abs((abs(o(3,3))-1.0)) > thr) {
    res(2) = acos(o(3,3))
        zeta = 1.0/sqrt(1.0-o(3,3)**2)
        res(1) = atan2(o(3,1)*zeta,-o(3,2)*zeta)
        res(3) = atan2(o(1,3)*zeta, o(2,3)*zeta)
  } else {
    if (abs(o(3,3)-1.0) < thr) {
      res(1) = atan2( o(1,2),o(1,1))
          res(2) = 0.0
          res(3) = 0.0
    } else {
      res(1) =-atan2(-o(1,2),o(1,1))
          res(2) = M_PI
          res(3) = 0.0
    }
  }
  if (res(1) < 0.0) res(1) = mod(res(1)+100.0*M_PI,2.0*M_PI)
      if (res(2) < 0.0) res(2) = mod(res(2)+100.0*M_PI,M_PI)
      if (res(3) < 0.0) res(3) = mod(res(3)+100.0*M_PI,2.0*M_PI)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ax2om(float* a, float* res)
{
  //*** use local
  //*** use constants
  //*** IMPLICIT NONE
  //real(kind=sgl),INTENT(IN)       :: a[3]         // axis angle pair

  float  q;
  float c;
  float s;
  float omc;

  float i;
  c = cos(a[3])
      s = sin(a[3])
      omc = 1.0-c;
  do i=1,3;
  res(i,i) = a(i)**2*omc + c;
  end do;
  q = omc*a[0]*a[1]
      res(1,2) = q + s*a[2]
      res(2,1) = q - s*a[2]
      q = omc*a[1]*a[2]
      res(2,3) = q + s*a[0]
      res(3,2) = q - s*a[0]
      q = omc*a[2]*a[0]
      res(3,1) = q + s*a[1]
      res(1,3) = q - s*a[1]
      if (epsijk == 1.0) res = transpose(res)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ax2om(double* a, double* res)
{
  //*** use local
  //*** use constants
  //*** IMPLICIT NONE
  //real(kind=dbl),INTENT(IN)       :: a[3]         // axis angle pair

  double  q;
  double c;
  double s;
  double omc;

  double i;
  c = cos(a[3])
      s = sin(a[3])
      omc = 1.0-c
      do i=1,3;
  res(i,i) = a(i)**2*omc + c;
  end do;
  q = omc*a[0]*a[1]
      res(1,2) = q + s*a[2]
      res(2,1) = q - s*a[2]
      q = omc*a[1]*a[2]
      res(2,3) = q + s*a[0]
      res(3,2) = q - s*a[0]
      q = omc*a[2]*a[0]
      res(3,1) = q + s*a[1]
      res(1,3) = q - s*a[1]
      if (epsijkd == 1.0) res = transpose(res)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::qu2eu(float* q, float* res)
{
  //*** use local
  //*** use constants
  //*** IMPLICIT NONE
  //real(kind=sgl),INTENT(IN)       :: q[3]         // quaternion

  float  qq[4];
  float q12;
  float q03;
  float chi;
  float Phi;
  float phi1;
  float phi2;

  qq = q;
  q03 = qq[0]**2+qq[3]**2
      q12 = qq[1]**2+qq[2]**2
      chi = sqrt(q03*q12);
  if (chi == 0.0) {
    if (q12 == 0.0) {
      if (epsijk == 1.0) {
        Phi = 0.0;
        phi2 = 0.0                  //arbitrarily due to degeneracy
            phi1 = atan2(-2.0*qq[0]*qq[3],qq[0]**2-qq[3]**2)
      } else {
        Phi = 0.0;
        phi2 = 0.0                  //arbitrarily due to degeneracy
            phi1 = atan2( 2.0*qq[0]*qq[3],qq[0]**2-qq[3]**2)
      }
    } else {
      if (epsijk == 1.0) {
        Phi = M_PI
            phi2 = 0.0                  //arbitrarily due to degeneracy
            phi1 = atan2(2.0*qq[1]*qq[2],qq[1]**2-qq[2]**2)
      } else {
        Phi = M_PI
            phi2 = 0.0                  //arbitrarily due to degeneracy
            phi1 = atan2(2.0*qq[1]*qq[2],qq[1]**2-qq[2]**2)
      }
    }
  } else {
    if (epsijk == 1.0) {
      Phi = atan2( 2.0*chi, q03-q12 );
      chi = 1.0/chi;
      phi1 = atan2( (-qq[0]*qq[2]+qq[1]*qq[3])*chi, (-qq[0]*qq[1]-qq[2]*qq[3])*chi )
          phi2 = atan2( (qq[0]*qq[2]+qq[1]*qq[3])*chi, (-qq[0]*qq[1]+qq[2]*qq[3])*chi )
    } else {
      Phi = atan2( 2.0*chi, q03-q12 );
      chi = 1.0/chi;
      phi1 = atan2( (qq[0]*qq[2]+qq[1]*qq[3])*chi, (qq[0]*qq[1]-qq[2]*qq[3])*chi )
          phi2 = atan2( (-qq[0]*qq[2]+qq[1]*qq[3])*chi, (qq[0]*qq[1]+qq[2]*qq[3])*chi )
    }
  }
  res = (/ phi1, Phi, phi2 /);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::qu2eu(double* q, double* res)
{
  //*** use local
  //*** use constants
  //*** IMPLICIT NONE
  //real(kind=dbl),INTENT(IN)       :: q[3]         // quaternion

  double  qq[4];
  double q12;
  double q03;
  double chi;
  double Phi;
  double phi1;
  double phi2;

  qq = q;
  q03 = qq[0]**2+qq[3]**2
      q12 = qq[1]**2+qq[2]**2
      chi = sqrt(q03*q12)
      if (chi == 0.0) {
      if (q12 == 0.0) {
      if (epsijkd == 1.0) {
      Phi = 0.0
      phi2 = 0.0                 //arbitrarily due to degeneracy
      phi1 = atan2(-2.0*qq[0]*qq[3],qq[0]**2-qq[3]**2)
} else {
Phi = 0.0
    phi2 = 0.0                 //arbitrarily due to degeneracy
    phi1 = atan2( 2.0*qq[0]*qq[3],qq[0]**2-qq[3]**2)
}
} else {
if (epsijkd == 1.0) {
  Phi = M_PI
      phi2 = 0.0                 //arbitrarily due to degeneracy
      phi1 = atan2(2.0*qq[1]*qq[2],qq[1]**2-qq[2]**2)
} else {
Phi = M_PI
    phi2 = 0.0                 //arbitrarily due to degeneracy
    phi1 = atan2(2.0*qq[1]*qq[2],qq[1]**2-qq[2]**2)
}
}
} else {
if (epsijkd == 1.0) {
  Phi = atan2( 2.0*chi, q03-q12 )
      chi = 1.0/chi
      phi1 = atan2( (-qq[0]*qq[2]+qq[1]*qq[3])*chi, (-qq[0]*qq[1]-qq[2]*qq[3])*chi )
      phi2 = atan2( (qq[0]*qq[2]+qq[1]*qq[3])*chi, (-qq[0]*qq[1]+qq[2]*qq[3])*chi )
} else {
Phi = atan2( 2.0*chi, q03-q12 )
    chi = 1.0/chi
    phi1 = atan2( (qq[0]*qq[2]+qq[1]*qq[3])*chi, (qq[0]*qq[1]-qq[2]*qq[3])*chi )
phi2 = atan2( (-qq[0]*qq[2]+qq[1]*qq[3])*chi, (qq[0]*qq[1]+qq[2]*qq[3])*chi )
}
}
res = (/ phi1, Phi, phi2 /);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ax2ho(float* a, float* res)
{
  //*** use local
  //*** use constants
  //*** IMPLICIT NONE
  //real(kind=sgl),INTENT(IN)       :: a[3]         // axis angle pair

  float  f;

  f = 0.75 * ( a[3] - sin(a[3]) )
      f = f**(1.0/3.0);
  res = a(1:3) * f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ax2ho(double* a, double* res)
{
  //*** use local
  //*** IMPLICIT NONE
  //real(kind=dbl),INTENT(IN)       :: a[3]         // axis angle pair

  double  f;

  f = 0.75D0 * ( a[3] - sin(a[3]) )
      f = f**(1.0/3.0)
      res = a(1:3) * f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ho2ax(float* h, float* res)
{
  //*** use local
  //*** use constants
  //*** IMPLICIT NONE
  //real(kind=sgl),INTENT(IN)       :: h[2]         // homochoric coordinates

  float i;
  float  hn[3];
  float hmag;
  float s;
  float hm;

  float  thr = 1.0E-6;

  hmag = sum(h*h);
  if (hmag == 0.0) {
    res = (/ 0.0, 0.0, 1.0, 0.0 /);
  } else {
    hm = hmag;
    hn = h/sqrt(hmag);
    s = LPs::tfit(1) + LPs::tfit(2) * hmag
        do i=3,16;
    hm = hm*hmag;
    s = s + LPs::tfit(i) * hm
        end do;
    s = 2.0*acos(s);
    if (abs(s-M_PI) < thr) {
      res = (/ hn[0], hn[1], hn[2], M_PI /)
    } else {
      res = (/ hn[0], hn[1], hn[2], s /)
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ho2ax(double* h, double* res)
{
  //*** use local
  //*** use constants
  //*** IMPLICIT NONE
  //real(kind=dbl),INTENT(IN)       :: h[2]         // homochoric coordinates

  double i;
  double  hn[3];
  double hmag;
  double s;
  double hm;

  double  thr = 1.0E-8;

  hmag = sum(h*h);
  if (hmag == 0.0) {
    res = (/ 0.0, 0.0, 1.0, 0.0 /)
  } else {
    hm = hmag;
    hn = h/sqrt(hmag)
        s = LPs::tfit(1) + LPs::tfit(2) * hmag
        do i=3,16;
    hm = hm*hmag;
    s = s + LPs::tfit(i) * hm
        end do;
    s = 2.0*acos(s)
        if (abs(s-M_PI) < thr) {
        res = (/ hn[0], hn[1], hn[2], M_PI /)
  } else {
    res = (/ hn[0], hn[1], hn[2], s /)
  }
}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::om2ax(float* om, float* res)
{
  //*** use local
  //*** use constants
  //*** use error
  //*** use io
  //real(kind=sgl), INTENT(IN)              :: om(3,3)

  float  t;
  float omega;
  float qq[4];
  float o[3,3];

  float  VL[3,3];
  float VR[3,3];
  float Wr[3];
  float Wi[3];
  float WORK[10];

  complex(kind=sgl)                       :: ev;
  complex(kind=sgl),parameter             :: cone = cmplx(1.0,0.0);
  float  thr = 1.0E-7;

  float nn, LDA, LDVL, LDVR, INFO, LWORK;
  character(1)                            :: JOBVL, JOBVR;
  o = om;
  t = 0.50*(om(1,1)+om(2,2)+om(3,3) - 1.0);
  if (t > 1.0) t = 1.0
      if (t < -1.0) t = -1.0
      res(4) = acos(t);
  if (res(4) == 0.0) {
    res(1:3) = (/ 0.0, 0.0, 1.0 /);
    return;
  } else {
    nn = 3;
    LDA = nn;
    LDVL = nn;
    LDVR = nn;
    INFO = 0;
    JOBVL = 'N'   //do not compute the left eigenvectors
        JOBVR = 'V'   //do compute the right eigenvectors
        LWORK = 20   ;
    dgeev(JOBVL,JOBVR,nn,o,LDA,Wr,Wi,VL,LDVL,VR,LDVR,WORK,LWORK,INFO);
    if (INFO != 0) call FatalError('Error in om2ax/dgeev : ','DGEEV return not zero')
        do i=1,3 ;
    ev = cmplx(Wr(i),Wi(i));
    if (abs(ev-cone) < thr) {
      res(1:3) = VR(1:3,i);
      if ((om(2,3)-om(3,2)) != 0.0) res(1) = sign(res(1),-epsijk*(om(2,3)-om(3,2)))
          if ((om(3,1)-om(1,3)) != 0.0) res(2) = sign(res(2),-epsijk*(om(3,1)-om(1,3)))
          if ((om(1,2)-om(2,1)) != 0.0) res(3) = sign(res(3),-epsijk*(om(1,2)-om(2,1)))
          return;
    }
    end do;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::om2ax(double* om, double* res)
{
  //*** use local
  //*** use constants
  //*** use error
  //*** use io
  //real(kind=dbl), INTENT(IN)              :: om(3,3)

  double  t;
  double omega;
  double qq[4];
  double o[3,3];

  double  VL[3,3];
  double VR[3,3];
  double Wr[3];
  double Wi[3];
  double WORK[10];

  complex(kind=dbl)                       :: ev;
  complex(kind=dbl),parameter             :: cone = cmplx(1.0,0.0)
      double  thr = 1.0D-10;

  double nn, LDA, LDVL, LDVR, INFO, LWORK;
  character(1)                            :: JOBVL, JOBVR;
  o = om;
  t = 0.5D0*(om(1,1)+om(2,2)+om(3,3) - 1.0)
      if (t > 1.0) t = 1.0
      if (t < -1.0) t = -1.0
      res(4) = acos(t)
      if (res(4) == 0.0) {
      res(1:3) = (/ 0.0, 0.0, 1.0 /)
      return;
} else {
      nn = 3;
      LDA = nn;
      LDVL = nn;
      LDVR = nn;
      INFO = 0;
      JOBVL = 'N'   //do not compute the left eigenvectors
      JOBVR = 'V'   //do compute the right eigenvectors
      LWORK = 20   ;
      dgeev(JOBVL,JOBVR,nn,o,LDA,Wr,Wi,VL,LDVL,VR,LDVR,WORK,LWORK,INFO);
      if (INFO != 0) call FatalError('Error in om2ax/dgeev : ','DGEEV return not zero')
      do i=1,3 ;
      if ((abs(Wr(i)-1.0) < thr).and.(abs(Wi(i)) < thr)) {
      res(1:3) = VR(1:3,i);
      if ((om(2,3)-om(3,2)) != 0.0) res(1) = dsign(res(1),-epsijkd*(om(2,3)-om(3,2)))
      if ((om(3,1)-om(1,3)) != 0.0) res(2) = dsign(res(2),-epsijkd*(om(3,1)-om(1,3)))
      if ((om(1,2)-om(2,1)) != 0.0) res(3) = dsign(res(3),-epsijkd*(om(1,2)-om(2,1)))
      return;
}
      end do;
}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ro2ax(float* r, float* res)
{
  //*** use local
  //*** use constants
  //*** IMPLICIT NONE
  //real(kind=sgl),INTENT(IN)       :: r[3]         // input Rodrigues vector

  float  ta;
  float angle;

  ta = r[3]
      if (ta == 0.0) {
      res = (/ 0.0, 0.0, 1.0, 0.0 /);
      return;
}
      if (ta == infty) {
      res = (/ r[0], r[1], r[2], M_PI /)
} else {
angle = 2.0*atan(ta);
ta = 1.0/sqrt(sum(r(1:3)*r(1:3)));
res = (/ r[0]/ta, r[1]/ta, r[2]/ta, angle /)
}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ro2ax(double* r, double* res)
{
  //*** use local
  //*** use constants
  //*** IMPLICIT NONE
  //real(kind=dbl),INTENT(IN)       :: r[3]         // input Rodrigues vector

  double  ta;
  double angle;

  ta = r[3]
      if (ta == 0.0) {
      res = (/ 0.0, 0.0, 1.0, 0.0 /)
      return;
}
      if (ta == infty) {
      res = (/ r[0], r[1], r[2], M_PI /)
} else {
angle = 2.0*atan(ta)
    ta = 1.0/sqrt(sum(r(1:3)*r(1:3)))
    res = (/ r[0]/ta, r[1]/ta, r[2]/ta, angle /)
}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ax2ro(float* a, float* res)
{
  //*** use local
  //*** use constants
  //real(kind=sgl), INTENT(IN)              :: a[3]

  float  t;

  float  thr = 1.0E-7;

  if (a[3] == 0.0) {
    res = (/ 0.0, 0.0, 0.0, 0.0 /);
    return;
  }
  res(1:3) =  a(1:3);
  if (abs(a[3]-M_PI) < thr) {
    res(4) = infty;
  } else {
    res(4) = tan( a[3] * 0.5 )
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ax2ro(double* a, double* res)
{
  //*** use local
  //*** use constants
  //real(kind=dbl), INTENT(IN)              :: a[3]

  double  t;

  double  thr = 1.0E-7;

  if (a[3] == 0.0) {
    res = (/ 0.0, 0.0, 0.0, 0.0 /)
        return;
  }
  res(1:3) =  a(1:3);
  if (abs(a[3]-M_PI) < thr) {
    res(4) = infty;
  } else {
    res(4) = tan( a[3] * 0.5D0 )
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ax2qu(float* a, float* res)
{
  //*** use local
  //real(kind=sgl), INTENT(IN)              :: a[3]
  float c;
  float s;

  if (a[3] == 0.0) {
    res = (/ 1.0, 0.0, 0.0, 0.0 /);
  } else {
    c = cos(a[3]*0.5)
        s = sin(a[3]*0.5)
        res = (/ c, a[0]*s, a[1]*s, a[2]*s /)
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ax2qu(double* a, double* res)
{
  //*** use local
  //real(kind=dbl), INTENT(IN)              :: a[3]
  double c;
  double s;

  if (a[3] == 0.0) {
    res = (/ 1.0, 0.0, 0.0, 0.0 /)
  } else {
    c = cos(a[3]*0.5D0)
        s = sin(a[3]*0.5D0)
        res = (/ c, a[0]*s, a[1]*s, a[2]*s /)
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ro2ho(float* r, float* res)
{
  //*** use local
  //*** use constants
  //real(kind=sgl), INTENT(IN)              :: r[3]
  float rv;
  float f;
  float t;

  rv = sum(r(1:3)*r(1:3));
  if (rv == 0.0) {
    res = (/ 0.0, 0.0, 0.0 /);
    return;
  }
  if (r[3] == infty) {
    f = 0.750 * M_PI
  } else {
    t = 2.0*atan(r[3])
        f = 0.750*(t- sin(t));
  }
  res(1:3) = r(1:3) * f**0.333333333;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ro2ho(double* r, double* res)
{
  //*** use local
  //*** use constants
  //real(kind=dbl), INTENT(IN)              :: r[3]
  double rv;
  double f;
  double t;

  rv = sum(r(1:3)*r(1:3));
  if (rv == 0.0) {
    res = (/ 0.0, 0.0, 0.0 /)
        return;
  }
  if (r[3] == infty) {
    f = 0.75D0 * M_PI
  } else {
    t = 2.0*atan(r[3])
        f = 0.75D0*(t- sin(t))
  }
  res(1:3) = r(1:3) * f**0.333333333D0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::qu2om(float* q, float* res)
{
  //*** use local
  //*** use constants
  //real(kind=sgl),INTENT(IN)               :: q[3]

  float  qq;
  float qc[4];

  qq=q[0]*q[0]-(q[1]*q[1]+q[2]*q[2]+q[3]*q[3])
      qc = q;
  res(1,1) = qq+2.0*qc[1]*qc[1]
      res(2,2) = qq+2.0*qc[2]*qc[2]
      res(3,3) = qq+2.0*qc[3]*qc[3]
      res(1,2) = 2.0*(qc[1]*qc[2]-qc[0]*qc[3])
      res(2,3) = 2.0*(qc[2]*qc[3]-qc[0]*qc[1])
      res(3,1) = 2.0*(qc[3]*qc[1]-qc[0]*qc[2])
      res(2,1) = 2.0*(qc[2]*qc[1]+qc[0]*qc[3])
      res(3,2) = 2.0*(qc[3]*qc[2]+qc[0]*qc[1])
      res(1,3) = 2.0*(qc[1]*qc[3]+qc[0]*qc[2])
      if (epsijk != 1.0) res = transpose(res)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::qu2om(double* q, double* res)
{
  //*** use local
  //*** use constants
  //real(kind=dbl),INTENT(IN)               :: q[3]

  double  qq;
  double qc[4];

  qq=q[0]*q[0]-(q[1]*q[1]+q[2]*q[2]+q[3]*q[3])
      qc = q;
  res(1,1) = qq+2.0*qc[1]*qc[1]
      res(2,2) = qq+2.0*qc[2]*qc[2]
      res(3,3) = qq+2.0*qc[3]*qc[3]
      res(1,2) = 2.0*(qc[1]*qc[2]-qc[0]*qc[3])
      res(2,3) = 2.0*(qc[2]*qc[3]-qc[0]*qc[1])
      res(3,1) = 2.0*(qc[3]*qc[1]-qc[0]*qc[2])
      res(2,1) = 2.0*(qc[2]*qc[1]+qc[0]*qc[3])
      res(3,2) = 2.0*(qc[3]*qc[2]+qc[0]*qc[1])
      res(1,3) = 2.0*(qc[1]*qc[3]+qc[0]*qc[2])
      if (epsijkd != 1.0) res = transpose(res)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::om2qu(float* x, float* res)
{
  //*** use local
  //*** use constants
  //real(kind=sgl), INTENT(IN)              :: x(3,3)               // input matrix

  float  s;
  float s1;
  float s2;
  float s3;
  float oax[4];

  s = x(1,1) + x(2,2) + x(3,3) + 1.0;
  if (abs(s) < thr) s = 0.0
      s = sqrt(s);
  s1 = x(1,1) - x(2,2) - x(3,3) + 1.0;
  if (abs(s1) < thr) s1 = 0.0
      s1 = sqrt(s1);
  s2 = -x(1,1) + x(2,2) - x(3,3) + 1.0;
  if (abs(s2) < thr) s2 = 0.0
      s2 = sqrt(s2);
  s3 = -x(1,1) - x(2,2) + x(3,3) + 1.0;
  if (abs(s3) < thr) s3 = 0.0
      s3 = sqrt(s3);
  res = (/ s, s1, s2, s3 /) * 0.5;
  if (x(3,2) < x(2,3)) res(2) = -epsijk * res(2)
      if (x(1,3) < x(3,1)) res(3) = -epsijk * res(3)
      if (x(2,1) < x(1,2)) res(4) = -epsijk * res(4)
      s = sqrt(sum(res*res));
  if (s != 0.0) res = res/s
      oax = om2ax(x);
  if (oax[0]*res(2) < 0.0) res(2) = -res(2)
      if (oax[1]*res(3) < 0.0) res(3) = -res(3)
      if (oax[2]*res(4) < 0.0) res(4) = -res(4)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::om2qu(double* x, double* res)
{
  //*** use local
  //*** use constants
  //real(kind=dbl), INTENT(IN)              :: x(3,3)               // input matrix

  double  s;
  double s1;
  double s2;
  double s3;
  double oax[4];

  double  thr = 1.0D-10;

  s = x(1,1) + x(2,2) + x(3,3) + 1.0
      if (abs(s) < thr) s = 0.0
      s = sqrt(s)
      s1 = x(1,1) - x(2,2) - x(3,3) + 1.0
      if (abs(s1) < thr) s1 = 0.0
      s1 = sqrt(s1)
      s2 = -x(1,1) + x(2,2) - x(3,3) + 1.0
      if (abs(s2) < thr) s2 = 0.0
      s2 = sqrt(s2)
      s3 = -x(1,1) - x(2,2) + x(3,3) + 1.0
      if (abs(s3) < thr) s3 = 0.0
      s3 = sqrt(s3)
      res = (/ s, s1, s2, s3 /) * 0.5;
  if (epsijkd == 1.0) {
    if (x(3,2) < x(2,3)) res(2) = -res(2)
        if (x(1,3) < x(3,1)) res(3) = -res(3)
        if (x(2,1) < x(1,2)) res(4) = -res(4)
  }
  s = sqrt(sum(res*res))
      if (s != 0.0) res = res/s
      oax = om2ax(x);
  if (oax[0]*res(2) < 0.0) res(2) = -res(2)
      if (oax[1]*res(3) < 0.0) res(3) = -res(3)
      if (oax[2]*res(4) < 0.0) res(4) = -res(4)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::qu2ax(float* q, float* res)
{
  //*** use local
  //*** use constants
  //real(kind=sgl), INTENT(IN)              :: q[3]
  float omega;
  float s;

  omega = 2.0 * acos(q[0])
      if (omega == 0.0) {
    res = (/ 0.0, 0.0, 1.0, 0.0 /);
  } else {
    if (q[0] != 0.0) {
      s =  (q[0]/abs(q[0]))/sqrt(q[1]**2+q[2]**2+q[3]**2)
          res = (/ q[1]*s, q[2]*s, q[3]*s, omega /)
    } else {
      res = (/ q[1], q[2], q[3], M_PI /)
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::qu2ax(double* q, double* res)
{
  //*** use local
  //*** use constants
  //real(kind=dbl), INTENT(IN)              :: q[3]
  double omega;
  double s;

  omega = 2.0 * acos(q[0])
      if (omega == 0.0) {
    res = (/ 0.0, 0.0, 1.0, 0.0 /)
  } else {
    if (q[0] != 0.0) {
      s =  (q[0]/abs(q[0]))/sqrt(q[1]**2+q[2]**2+q[3]**2)
          res = (/ q[1]*s, q[2]*s, q[3]*s, omega /)
    } else {
      res = (/ q[1], q[2], q[3], M_PI /)
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::qu2ro(float* q, float* res)
{
  //*** use local
  //*** use constants
  //real(kind=sgl), INTENT(IN)              :: q[3]
  float omega;
  float t;
  float s;

  float  thr = 1.0E-8;

  res(1:3) = q(2:4);
  res(4) = 0.0;
  if (q[0] < thr) {
    res(4)=infty;
    return;
  }
  s = sqrt(sum(res(1:3)*res(1:3)));
  if (s < thr) {
    res = (/ 0.0, 0.0, 0.0, 0.0 /);
    return;
  } else {
    t = tan(acos(q[0]))
        res = (/ res(1)/s, res(2)/s, res(3)/s, t /);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::qu2ro(double* q, double* res)
{
  //*** use local
  //*** use constants
  //real(kind=dbl), INTENT(IN)              :: q[3]
  double omega;
  double s;
  double t;

  double  thr = 1.0D-10;

  res(1:3) = q(2:4);
  res(4) = 0.0
      if (q[0] < thr) {
    res(4)=infty;
    return;
  }
  s = sqrt(sum(res(1:3)*res(1:3)))
      if (s < thr) {
      res = (/ 0.0, 0.0, 0.0, 0.0 /)
      return;
} else {
      t = tan(acos(q[0]))
      res = (/ res(1)/s, res(2)/s, res(3)/s, t /);
}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::qu2ho(float* q, float* res)
{
  //*** use local
  //real(kind=sgl), INTENT(IN)              :: q[3]
  float omega;
  float s;
  float f;

  omega = 2.0 * acos(q[0])
      if (omega == 0.0) {
    res = (/ 0.0, 0.0, 0.0 /);
  } else {
    res = q(2:4);
    s = 1.0/sqrt(sum(res**2));
    res = res * s;
    f = 0.75 * ( omega - sin(omega) );
    res = res * f**0.333333333;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::qu2ho(double* q, double* res)
{
  //*** use local
  //real(kind=dbl), INTENT(IN)              :: q[3]
  double omega;
  double s;
  double f;

  omega = 2.0 * acos(q[0])
      if (omega == 0.0) {
    res = (/ 0.0, 0.0, 0.0 /)
  } else {
    res = q(2:4);
    s = 1.0/sqrt(sum(res**2))
        res = res * s;
    f = 0.75D0 * ( omega - sin(omega) )
        res = res * f**0.333333333D0;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ho2cu(float* h, float* res)
{
  //*** use local
  //*** use Lambert, only: LambertBallToCube
  //real(kind=sgl), INTENT(IN)              :: h[2]         // input coordinates

  float ierr;
  res = LambertBallToCube(h,ierr);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ho2cu(double* h, double* res)
{
  //*** use local
  //*** use Lambert, only: LambertBallToCube
  //real(kind=dbl), INTENT(IN)              :: h[2]         // input coordinates

  double ierr;
  res = LambertBallToCube(h,ierr);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::cu2ho(float* c, float* res)
{
  //*** use local
  //*** use Lambert, only: LambertCubeToBall
  //real(kind=sgl), INTENT(IN)              :: c[2]         // input coordinates

  float ierr;
  res = LambertCubeToBall(c,ierr);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::cu2ho(double* c, double* res)
{
  //*** use local
  //*** use Lambert, only: LambertCubeToBall
  //real(kind=dbl), INTENT(IN)              :: c[2]         // input coordinates

  double ierr;
  res = LambertCubeToBall(c,ierr);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ro2eu(float* r, float* res)
{
  //*** use local
  //*** use constants
  //*** IMPLICIT NONE
  //real(kind=sgl),INTENT(IN)       :: r[3]         // Rodrigues vector

  float  rr[3];
  float s;
  float d;

  res = om2eu(ro2om(r));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ro2eu(double* r, double* res)
{
  //*** use local
  //*** use constants
  //*** IMPLICIT NONE
  //real(kind=dbl),INTENT(IN)       :: r[3]         // Rodrigues vector

  double  rr[3];
  double s;
  double d;

  res = om2eu_d(ro2om_d(r));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::eu2ho(float* e, float* res)
{
  //*** use local
  //real(kind=sgl), INTENT(IN)              :: e[2]

  res = ax2ho(eu2ax(e));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::eu2ho(double* e, double* res)
{
  //*** use local
  //real(kind=dbl), INTENT(IN)              :: e[2]

  res = ax2ho_d(eu2ax_d(e));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::om2ro(float* om, float* res)
{
  //*** use local
  //real(kind=sgl), INTENT(IN)              :: om(3,3)

  res = eu2ro(om2eu(om));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::om2ro(double* om, double* res)
{
  //*** use local
  //real(kind=dbl), INTENT(IN)              :: om(3,3)

  res = eu2ro_d(om2eu_d(om));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::om2ho(float* om, float* res)
{
  //*** use local
  //real(kind=sgl), INTENT(IN)              :: om(3,3)

  res = ax2ho(om2ax(om));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::om2ho(double* om, double* res)
{
  //*** use local
  //real(kind=dbl), INTENT(IN)              :: om(3,3)

  res = ax2ho(om2ax(om));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ax2eu(float* a, float* res)
{
  //*** use local
  //real(kind=sgl), INTENT(IN)              :: a[3]

  res = om2eu(ax2om(a));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ax2eu(double* a, double* res)
{
  //*** use local
  //real(kind=dbl), INTENT(IN)              :: a[3]

  res = om2eu(ax2om(a));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ro2om(float* r, float* res)
{
  //*** use local
  //real(kind=sgl), INTENT(IN)              :: r[3]

  res = ax2om(ro2ax(r));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ro2om(double* r, double* res)
{
  //*** use local
  //real(kind=dbl), INTENT(IN)              :: r[3]

  res = ax2om_d(ro2ax_d(r));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ro2qu(float* r, float* res)
{
  //*** use local
  //real(kind=sgl), INTENT(IN)              :: r[3]

  res = ax2qu(ro2ax(r));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ro2qu(double* r, double* res)
{
  //*** use local
  //real(kind=dbl), INTENT(IN)              :: r[3]

  res = ax2qu_d(ro2ax_d(r));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ho2eu(float* h, float* res)
{
  //*** use local
  //real(kind=sgl), INTENT(IN)              :: h[2]

  res = ax2eu(ho2ax(h));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ho2eu(double* h, double* res)
{
  //*** use local
  //real(kind=dbl), INTENT(IN)              :: h[2]

  res = ax2eu_d(ho2ax_d(h));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ho2om(float* h, float* res)
{
  //*** use local
  //real(kind=sgl), INTENT(IN)              :: h[2]

  res = ax2om(ho2ax(h));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ho2om(double* h, double* res)
{
  //*** use local
  //real(kind=dbl), INTENT(IN)              :: h[2]

  res = ax2om_d(ho2ax_d(h));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ho2ro(float* h, float* res)
{
  //*** use local
  //real(kind=sgl), INTENT(IN)              :: h[2]

  res = ax2ro(ho2ax(h));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ho2ro(double* h, double* res)
{
  //*** use local
  //real(kind=dbl), INTENT(IN)              :: h[2]

  res = ax2ro_d(ho2ax_d(h));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ho2qu(float* h, float* res)
{
  //*** use local
  //real(kind=sgl), INTENT(IN)              :: h[2]

  res = ax2qu(ho2ax(h));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ho2qu(double* h, double* res)
{
  //*** use local
  //real(kind=dbl), INTENT(IN)              :: h[2]

  res = ax2qu_d(ho2ax_d(h));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::eu2cu(float* e, float* res)
{
  //*** use local
  //real(kind=sgl), INTENT(IN)              :: e[2]         // input coordinates

  res = ho2cu(eu2ho(e));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::eu2cu(double* e, double* res)
{
  //*** use local
  //real(kind=dbl), INTENT(IN)              :: e[2]         // input coordinates

  res = ho2cu_d(eu2ho_d(e));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::om2cu(float* o, float* res)
{
  //*** use local
  //real(kind=sgl), INTENT(IN)              :: o(3,3)               // input coordinates

  res = ho2cu(om2ho(o));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::om2cu(double* o, double* res)
{
  //*** use local
  //real(kind=dbl), INTENT(IN)              :: o(3,3)               // input coordinates

  res = ho2cu_d(om2ho_d(o));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ax2cu(float* a, float* res)
{
  //*** use local
  //real(kind=sgl), INTENT(IN)              :: a[3]         // input coordinates

  res = ho2cu(ax2ho(a));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ax2cu(double* a, double* res)
{
  //*** use local
  //real(kind=dbl), INTENT(IN)              :: a[3]         // input coordinates

  res = ho2cu_d(ax2ho_d(a));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ro2cu(float* r, float* res)
{
  //*** use local
  //real(kind=sgl), INTENT(IN)              :: r[3]         // input coordinates

  res = ho2cu(ro2ho(r));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::ro2cu(double* r, double* res)
{
  //*** use local
  //real(kind=dbl), INTENT(IN)              :: r[3]         // input coordinates

  res = ho2cu_d(ro2ho_d(r));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::qu2cu(float* q, float* res)
{
  //*** use local
  //real(kind=sgl), INTENT(IN)              :: q[3]         // input coordinates

  res = ho2cu(qu2ho(q));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::qu2cu(double* q, double* res)
{
  //*** use local
  //real(kind=dbl), INTENT(IN)              :: q[3]         // input coordinates

  res = ho2cu_d(qu2ho_d(q));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::cu2eu(float* c, float* res)
{
  //*** use local
  //real(kind=sgl), INTENT(IN)              :: c[2]         // input coordinates

  res = ho2eu(cu2ho(c));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::cu2eu(double* c, double* res)
{
  //*** use local
  //real(kind=dbl), INTENT(IN)              :: c[2]         // input coordinates

  res = ho2eu_d(cu2ho_d(c));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::cu2om(float* c, float* res)
{
  //*** use local
  //real(kind=sgl), INTENT(IN)              :: c[2]         // input coordinates

  res = ho2om(cu2ho(c));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::cu2om(double* c, double* res)
{
  //*** use local
  //real(kind=dbl), INTENT(IN)              :: c[2]         // input coordinates

  res = ho2om_d(cu2ho_d(c));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::cu2ax(float* c, float* res)
{
  //*** use local
  //real(kind=sgl), INTENT(IN)              :: c[2]         // input coordinates

  res = ho2ax(cu2ho(c));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::cu2ax(double* c, double* res)
{
  //*** use local
  //real(kind=dbl), INTENT(IN)              :: c[2]         // input coordinates

  res = ho2ax_d(cu2ho_d(c));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::cu2ro(float* c, float* res)
{
  //*** use local
  //real(kind=sgl), INTENT(IN)              :: c[2]         // input coordinates

  res = ho2ro(cu2ho(c));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::cu2ro(double* c, double* res)
{
  //*** use local
  //real(kind=dbl), INTENT(IN)              :: c[2]         // input coordinates

  res = ho2ro_d(cu2ho_d(c));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::cu2qu(float* c, float* res)
{
  //*** use local
  //real(kind=sgl), INTENT(IN)              :: c[2]         // input coordinates

  res = ho2qu(cu2ho(c));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::cu2qu(double* c, double* res)
{
  //*** use local
  //real(kind=dbl), INTENT(IN)              :: c[2]         // input coordinates

  res = ho2qu_d(cu2ho_d(c));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::RotVec_om(float* vec, float* om, char ap, float* res)
{
  //*** use local
  //real(kind=sgl),INTENT(IN)       :: vec[2]
  //real(kind=sgl),INTENT(IN)       :: om(3,3)
  //character(1),INTENT(IN)         :: ap

  if (ap == 'p') {
    res = matmul(om,vec);
  } else {
    res = matmul(transpose(om),vec);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::RotVec_om(double* vec, double* om, char ap, double* res)
{
  //*** use local
  //real(kind=dbl),INTENT(IN)       :: vec[2]
  //real(kind=dbl),INTENT(IN)       :: om(3,3)
  //character(1),INTENT(IN)         :: ap

  if (ap == 'p') {
    res = matmul(om,vec);
  } else {
    res = matmul(transpose(om),vec);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::RotVec_qu(float* vec, float* qu, char ap, float* res)
{
  //*** use local
  //*** use quaternions
  //*** use constants
  //real(kind=sgl),INTENT(IN)       :: vec[2]
  //real(kind=sgl),INTENT(IN)       :: qu[3]
  //character(1),INTENT(IN)         :: ap

  float  rq[4];
  float rr[4];

  rq = (/ 0.0, vec[0], vec[1], vec[2] /)
      if (epsijk < 0) {
    if (ap == 'a') {
      rr = quat_mult(qu,quat_mult(rq,conjg(qu)) );
    } else {
      rr = quat_mult(conjg(qu),quat_mult(rq,qu) );
    }
  } else {
    if (ap == 'p') {
      rr = quat_mult(qu,quat_mult(rq,conjg(qu)) );
    } else {
      rr = quat_mult(conjg(qu),quat_mult(rq,qu) );
    }
  }
  res = rr(2:4);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::RotVec_qu(double* vec, float* qu, char ap, double* res)
{
  //*** use local
  //*** use quaternions
  //*** use constants
  //real(kind=dbl),INTENT(IN)       :: vec[2]
  //real(kind=dbl),INTENT(IN)       :: qu[3]
  //character(1),INTENT(IN)         :: ap

  double  rq[4];
  double rr[4];

  rq = (/ 0.0, vec[0], vec[1], vec[2] /)
      if (epsijk < 0) {
    if (ap == 'a') {
      rr = quat_mult(qu,quat_mult(rq,conjg(qu)) );
    } else {
      rr = quat_mult(conjg(qu),quat_mult(rq,qu) );
    }
  } else {
    if (ap == 'p') {
      rr = quat_mult(qu,quat_mult(rq,conjg(qu)) );
    } else {
      rr = quat_mult(conjg(qu),quat_mult(rq,qu) );
    }
  }
  res = rr(2:4);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::RotTensor2_om(float* tensor, float* om, char ap, float* res)
{
  //*** use local
  //real(kind=sgl),INTENT(IN)       :: tensor(3,3)
  //real(kind=sgl),INTENT(IN)       :: om(3,3)
  //character(1),INTENT(IN)         :: ap

  if (ap == 'p') {
    res = matmul(matmul(om,tensor),transpose(om));
  } else {
    res = matmul(matmul(transpose(om),tensor),om);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationTransforms::RotTensor2_om(double* tensor, double* om, char ap, double* res)
{
  //*** use local
  //real(kind=dbl),INTENT(IN)       :: tensor(3,3)
  //real(kind=dbl),INTENT(IN)       :: om(3,3)
  //character(1),INTENT(IN)         :: ap

  if (ap == 'p') {
    res = matmul(matmul(om,tensor),transpose(om));
  } else {
    res = matmul(matmul(transpose(om),tensor),om);
  }
}

#endif
