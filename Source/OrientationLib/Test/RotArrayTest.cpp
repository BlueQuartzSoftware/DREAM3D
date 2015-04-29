
#include <stdlib.h>

#include <vector>
#include <iomanip>
#include <string>
#include <iostream>
#include <algorithm>
#include <complex>

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Eigen>

#include <QtCore/QVector>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Math/QuaternionMath.hpp"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Utilities/UnitTestSupport.hpp"

using namespace DREAM3D::Constants;

#include "OrientationLib/OrientationLib.h"
#include "OrientationLib/Math/RotArray.hpp"
#include "OrientationLib/Math/RotationTransforms.h"


#include "OrientationLib/Test/OrientationLibTestFileLocations.h"


typedef RotationRepresentation<float> RotArrayType;
typedef std::vector<float> FloatVectorType;
typedef QVector<float> FloatQVectorType;


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
T transfer_sign(T a, T b)
{
  if( a > 0.0 && b > 0.0) return a;
  if( a < 0.0 && b > 0.0) return -1*a;

  if( a < 0.0 && b < 0.0) return a;

  return -1*a;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestRotArray()
{
  FloatRotationRepresentation_t ro(3);
  ro[0] = 2;
  ro[1] = 4;
  ro[2] = 6;
//  float sum = ro.sum();
//  DREAM3D_REQUIRE_EQUAL(sum, 12);

//  DREAM3D_REQUIRE_EQUAL(ro.size(), 3);

//  DREAM3D_REQUIRE_EQUAL(ro.maxval(), 6);
//  DREAM3D_REQUIRE_EQUAL(ro.minval(), 2);

//  float pro = ro.product();
//  DREAM3D_REQUIRE_EQUAL(pro, 48);
//  sum = result.sum();
//  float max = result.maxval();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Test_eu_check()
{
  int res = 0;
  RotationTransforms rt;
  RotationTransforms::ResultType result;

  typedef std::vector<float> FloatVectorType;

  float eu[3] = {1.0f, 0.4f, 0.9f};
  result = rt.eu_check<float*>(eu);
  DREAM3D_REQUIRE_EQUAL(result.result, 1);
  eu[0] = -1.0;
  result = rt.eu_check<float*>(eu);
  DREAM3D_REQUIRE_EQUAL(result.result, 0);

  FloatRotationRepresentation_t eu_r(3);
  eu_r[0] = 1.0f;
  eu_r[1] = 0.4f;
  eu_r[2] = 0.9f;
  result = rt.eu_check<FloatRotationRepresentation_t>(eu_r);
  DREAM3D_REQUIRE_EQUAL(result.result, 1);
  eu_r[0] = -1.0;
  result = rt.eu_check<FloatRotationRepresentation_t>(eu_r);
  DREAM3D_REQUIRE_EQUAL(result.result, 0);

  FloatVectorType eu_v(3);
  eu_v[0] = 1.0f;
  eu_v[1] = 0.4f;
  eu_v[2] = 0.9f;
  result = rt.eu_check<FloatVectorType>(eu_v);
  DREAM3D_REQUIRE_EQUAL(result.result, 1);
  eu_v[0] = -1.0;
  result = rt.eu_check<FloatVectorType>(eu_v);
  DREAM3D_REQUIRE_EQUAL(result.result, 0);

  typedef QVector<float> FloatQVectorType;
  FloatQVectorType eu_q(3);
  eu_q[0] = 1.0f;
  eu_q[1] = 0.4f;
  eu_q[2] = 0.9f;
  result = rt.eu_check<FloatQVectorType>(eu_q);
  DREAM3D_REQUIRE_EQUAL(result.result, 1);
  eu_q[0] = -1.0;
  result = rt.eu_check<FloatQVectorType>(eu_q);
  DREAM3D_REQUIRE_EQUAL(result.result, 0);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Test_ro_check()
{

  RotationTransforms rt;
  RotationTransforms::ResultType result;

  typedef std::vector<float> FloatVectorType;
  typedef QVector<float> FloatQVectorType;

  {
    FloatRotationRepresentation_t ro(4);
    ro[0] = 1.0f;
    ro[1] = 1.0f;
    ro[2] = 1.0f;
    ro[3] = 1.0f;
    MatrixMath::Normalize3x1(&(ro[0]));
    result = rt.ro_check<FloatRotationRepresentation_t, float>(ro);
    DREAM3D_REQUIRE_EQUAL(result.result, 1);
    ro[3] = -1.0;
    result = rt.ro_check<FloatRotationRepresentation_t, float>(ro);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);
  }

  {
    FloatVectorType ro(4);
    ro[0] = 1.0f;
    ro[1] = 1.0f;
    ro[2] = 1.0f;
    ro[3] = 1.0f;
    MatrixMath::Normalize3x1(&(ro[0]));
    result = rt.ro_check<FloatVectorType, float>(ro);
    DREAM3D_REQUIRE_EQUAL(result.result, 1);
    ro[3] = -1.0;
    result = rt.ro_check<FloatVectorType, float>(ro);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);
  }

  {
    FloatQVectorType ro(4);
    ro[0] = 1.0f;
    ro[1] = 1.0f;
    ro[2] = 1.0f;
    ro[3] = 1.0f;
    MatrixMath::Normalize3x1(&(ro[0]));
    result = rt.ro_check<FloatQVectorType, float>(ro);
    DREAM3D_REQUIRE_EQUAL(result.result, 1);
    ro[3] = -1.0;
    result = rt.ro_check<FloatQVectorType, float>(ro);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Test_ho_check()
{

  RotationTransforms rt;
  RotationTransforms::ResultType result;

  typedef std::vector<float> FloatVectorType;
  typedef QVector<float> FloatQVectorType;

  {
    FloatRotationRepresentation_t ho(3);
    ho[0] = 0.5f;
    ho[1] = 0.5f;
    ho[2] = 0.5f;
    result = rt.ho_check<FloatRotationRepresentation_t, float>(ho);
    DREAM3D_REQUIRE_EQUAL(result.result, 1);
    ho[2] = 8.0;
    result = rt.ho_check<FloatRotationRepresentation_t, float>(ho);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);
  }

  {
    FloatVectorType ho(3);
    ho[0] = 0.5f;
    ho[1] = 0.5f;
    ho[2] = 0.5f;
    result = rt.ho_check<FloatVectorType, float>(ho);
    DREAM3D_REQUIRE_EQUAL(result.result, 1);
    ho[2] = 8.0;
    result = rt.ho_check<FloatVectorType, float>(ho);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);
  }

  {
    FloatQVectorType ho(3);
    ho[0] = 0.5f;
    ho[1] = 0.5f;
    ho[2] = 0.5f;
    result = rt.ho_check<FloatQVectorType, float>(ho);
    DREAM3D_REQUIRE_EQUAL(result.result, 1);
    ho[2] = 8.0;
    result = rt.ho_check<FloatQVectorType, float>(ho);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Test_cu_check()
{

  RotationTransforms rt;
  RotationTransforms::ResultType result;

  typedef std::vector<float> FloatVectorType;
  typedef QVector<float> FloatQVectorType;

  {
    FloatRotationRepresentation_t v(3);
    v[0] = 0.5f;
    v[1] = 0.5f;
    v[2] = 0.5f;
    result = rt.cu_check<FloatRotationRepresentation_t, float>(v);
    DREAM3D_REQUIRE_EQUAL(result.result, 1);
    v[2] = 8.0;
    result = rt.cu_check<FloatRotationRepresentation_t, float>(v);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);
  }

  {
    FloatVectorType v(3);
    v[0] = 0.5f;
    v[1] = 0.5f;
    v[2] = 0.5f;
    result = rt.cu_check<FloatVectorType, float>(v);
    DREAM3D_REQUIRE_EQUAL(result.result, 1);
    v[2] = 8.0;
    result = rt.cu_check<FloatVectorType, float>(v);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);
  }

  {
    FloatQVectorType v(3);
    v[0] = 0.5f;
    v[1] = 0.5f;
    v[2] = 0.5f;
    result = rt.cu_check<FloatQVectorType, float>(v);
    DREAM3D_REQUIRE_EQUAL(result.result, 1);
    v[2] = 8.0;
    result = rt.cu_check<FloatQVectorType, float>(v);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Test_qu_check()
{

  RotationTransforms rt;
  RotationTransforms::ResultType result;

  typedef std::vector<float> FloatVectorType;
  typedef QVector<float> FloatQVectorType;
  QuatF quat = QuaternionMathF::New(1.0, 1.0, 1.0, 0.0);
  QuaternionMathF::UnitQuaternion(quat);

  {
    FloatRotationRepresentation_t qu(4);
    qu[0] = quat.x;
    qu[1] = quat.y;
    qu[2] = quat.z;
    qu[3] = quat.w;
    MatrixMath::Normalize3x1(&(qu[0]));
    result = rt.qu_check<FloatRotationRepresentation_t, float>(qu);
    DREAM3D_REQUIRE_EQUAL(result.result, 1);

    qu[0] = 1.5f;
    qu[1] = 3.0f;
    qu[2] = 2.0f;
    result = rt.qu_check<FloatRotationRepresentation_t, float>(qu);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);

    qu[0] = -1.0;
    result = rt.qu_check<FloatRotationRepresentation_t, float>(qu);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);
  }

  {
    FloatVectorType qu(4);
    qu[0] = quat.x;
    qu[1] = quat.y;
    qu[2] = quat.z;
    qu[3] = quat.w;
    MatrixMath::Normalize3x1(&(qu[0]));
    result = rt.qu_check<FloatVectorType, float>(qu);
    DREAM3D_REQUIRE_EQUAL(result.result, 1);

    qu[0] = 1.5f;
    qu[1] = 3.0f;
    qu[2] = 2.0f;
    result = rt.qu_check<FloatVectorType, float>(qu);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);

    qu[0] = -1.0;
    result = rt.qu_check<FloatVectorType, float>(qu);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);
  }

  {
    FloatQVectorType qu(4);
    qu[0] = quat.x;
    qu[1] = quat.y;
    qu[2] = quat.z;
    qu[3] = quat.w;
    MatrixMath::Normalize3x1(&(qu[0]));
    result = rt.qu_check<FloatQVectorType, float>(qu);
    DREAM3D_REQUIRE_EQUAL(result.result, 1);

    qu[0] = 1.5f;
    qu[1] = 3.0f;
    qu[2] = 2.0f;
    result = rt.qu_check<FloatQVectorType, float>(qu);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);

    qu[0] = -1.0;
    result = rt.qu_check<FloatQVectorType, float>(qu);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Test_ax_check()
{

  RotationTransforms rt;
  RotationTransforms::ResultType result;

  typedef std::vector<float> FloatVectorType;
  typedef QVector<float> FloatQVectorType;

  {
    FloatRotationRepresentation_t ax(4);
    ax[0] = 0.0f;
    ax[1] = 0.0f;
    ax[2] = 1.0f;
    ax[3] = DREAM3D::Constants::k_Pi-0.00001f;
    result = rt.ax_check<FloatRotationRepresentation_t, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 1);

    ax[0] = 1.0;
    result = rt.ax_check<FloatRotationRepresentation_t, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);

    ax[3] = DREAM3D::Constants::k_Pi + 1.0f;
    result = rt.ax_check<FloatRotationRepresentation_t, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);
  }

  {
    FloatVectorType ax(4);
    ax[0] = 0.0f;
    ax[1] = 0.0f;
    ax[2] = 1.0f;
    ax[3] = DREAM3D::Constants::k_Pi-0.00001f;
    result = rt.ax_check<FloatVectorType, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 1);

    ax[0] = 1.0;
    result = rt.ax_check<FloatVectorType, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);

    ax[3] = DREAM3D::Constants::k_Pi + 1.0;
    result = rt.ax_check<FloatVectorType, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);
  }

  {
    FloatQVectorType ax(4);
    ax[0] = 0.0f;
    ax[1] = 0.0f;
    ax[2] = 1.0f;
    ax[3] = DREAM3D::Constants::k_Pi-0.00001f;
    result = rt.ax_check<FloatQVectorType, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 1);

    ax[0] = 1.0f;
    result = rt.ax_check<FloatQVectorType, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);

    ax[3] = DREAM3D::Constants::k_Pi + 1.0f;
    result = rt.ax_check<FloatQVectorType, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Test_om_check()
{
  RotationTransforms rt;
  RotationTransforms::ResultType result;

  typedef std::vector<float> FloatVectorType;
  typedef QVector<float> FloatQVectorType;

  {
    FloatRotationRepresentation_t ax(9);
    ax[0] = 1.0f;
    ax[1] = 0.0f;
    ax[2] = 0.0f;
    ax[3] = 0.0f;
    ax[4] = 1.0f;
    ax[5] = 0.0f;
    ax[6] = 0.0f;
    ax[7] = 0.0f;
    ax[8] = 1.0f;
    result = rt.om_check<FloatRotationRepresentation_t, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 1);

    ax[1] = -16.0f;
    ax[6] = -12.0f;
    result = rt.om_check<FloatRotationRepresentation_t, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);

    ax[3] = DREAM3D::Constants::k_Pi + 1.0f;
    result = rt.om_check<FloatRotationRepresentation_t, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);
  }

  {
    FloatVectorType ax(9);
    ax[0] = 1.0f;
    ax[1] = 0.0f;
    ax[2] = 0.0f;
    ax[3] = 0.0f;
    ax[4] = 1.0f;
    ax[5] = 0.0f;
    ax[6] = 0.0f;
    ax[7] = 0.0f;
    ax[8] = 1.0f;
    result = rt.om_check<FloatVectorType, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 1);

    ax[1] = -16.0f;
    ax[6] = -12.0f;
    result = rt.om_check<FloatVectorType, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);

    ax[3] = DREAM3D::Constants::k_Pi + 1.0f;
    result = rt.om_check<FloatVectorType, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);
  }

  {
    FloatQVectorType ax(9);
    ax[0] = 1.0f;
    ax[1] = 0.0f;
    ax[2] = 0.0f;
    ax[3] = 0.0f;
    ax[4] = 1.0f;
    ax[5] = 0.0f;
    ax[6] = 0.0f;
    ax[7] = 0.0f;
    ax[8] = 1.0f;
    result = rt.om_check<FloatQVectorType, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 1);

    ax[1] = -16.0f;
    ax[6] = -12.0f;
    result = rt.om_check<FloatQVectorType, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);

    ax[3] = DREAM3D::Constants::k_Pi + 1.0f;
    result = rt.om_check<FloatQVectorType, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T, typename K>
void GenRotTest(K* in, K omega)
{
  T eu(3);
  eu[0] = in[0];
  eu[1] = in[1];
  eu[2] = in[2];

  T res(4);
  RotationTransforms rt;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Test_GenRot()
{

  float eu[3] = { 1.0f, 0.0f, 0.0f};
  float omega = DREAM3D::Constants::k_PiOver2;
  GenRotTest<RotArrayType, float>(eu, omega);
  GenRotTest<FloatVectorType, float>(eu, omega);
  GenRotTest<FloatQVectorType, float>(eu, omega);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void Print_EU(const T& om)
{
  printf("Euler angles [deg]               :   % 3.6f    % 3.6f    % 3.6f\n", om[0]*k_180OverPi, om[1]*k_180OverPi, om[2]*k_180OverPi );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void Print_OM(const T& om)
{
  printf("                                 : /    % 3.6f    % 3.6f    % 3.6f    \\\n", om[0], om[1], om[2]);
  printf("Orientation Matrix               : |    % 3.6f    % 3.6f    % 3.6f    |\n", om[3], om[4], om[5]);
  printf("                                 : \\    % 3.6f    % 3.6f    % 3.6f    /\n", om[6], om[7], om[8]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void Print_AX(const T& om)
{
  printf("Axis angle pair [n; angle (deg)] :   % 3.6f    % 3.6f    % 3.6f  :  % 3.6f\n", om[0], om[1], om[2], om[3]*DREAM3D::Constants::k_180OverPi);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T, typename K>
void Print_RO(const T& om)
{
  //if(om[3] != std::numeric_limits<K>::infinity())
  //  printf("Rodrigues vector                 :   % 3.6f    % 3.6f    % 3.6f\n", om[0], om[1], om[2] );
  //else
  printf("Rodrigues vector                 :   % 3.6f    % 3.6f    % 3.6f    % 3.6f\n", om[0], om[1], om[2], om[3] );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void Print_QU(const T& om, typename QuaternionMath<float>::Order layout = QuaternionMath<float>::QuaternionVectorScalar)
{
  if(layout == QuaternionMath<float>::QuaternionVectorScalar)
   {
    printf("Quaternion (<vector>scalar)      :   <% 3.6f   % 3.6f    % 3.6f>    % 3.6f\n", om[0], om[1], om[2], om[3] );
   }

  else if(layout == QuaternionMath<float>::QuaternionScalarVector)
  {
    printf("Quaternion (scalar<vector>)      :   % 3.6f   <% 3.6f    % 3.6f    % 3.6f>\n", om[0], om[1], om[2], om[3] );
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void Print_HO(const T& om)
{
  printf("Homochoric ()                    :   % 3.10f    % 3.10f    % 3.10f\n", om[0], om[1], om[2] );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void Print_CU(const T& om)
{
  printf("Cubochoric ()                    :   % 3.10f    % 3.10f    % 3.10f\n", om[0], om[1], om[2] );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void EU_2_XXX(float* in)
{
  T eu(3);
  eu[0] = in[0];
  eu[1] = in[1];
  eu[2] = in[2];

  T res(9);
  RotationTransforms rt;
  // Convert to Orientation Matrix
  rt.eu2om<T, float>(eu, res);
  Print_OM<T>(res);

  //Convert to Axis Angle
  rt.eu2ax<T, float>(eu, res);
  Print_AX<T>(res);

  // Convert to Rodriques
  rt.eu2ro<T, float>(eu, res);
  Print_RO<T, float>(res);

  // Convert to Quaternion
  rt.eu2qu<T, float>(eu, res);
  Print_QU<T>(res);
  rt.eu2qu<T, float>(eu, res, QuaternionMath<float>::QuaternionVectorScalar);
  Print_QU<T>(res, QuaternionMath<float>::QuaternionVectorScalar);

  // Convert to HomoChoric
  rt.eu2ho<T, float>(eu, res);
  Print_HO<T>(res);

  // Convert to HomoChoric
  rt.eu2cu<T, float>(eu, res);
  Print_CU<T>(res);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Test_eu2_XXX()
{
  std::cout << "Test_eu2_XXX  $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << std::endl;
  float eu[3] = {k_PiOver2, 0.0f, 0.0f};
  Print_EU<float*>(eu);
  EU_2_XXX<RotArrayType>(eu);
  EU_2_XXX<FloatVectorType>(eu);
  EU_2_XXX<FloatQVectorType>(eu);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void OM_2_XXX(float* in)
{
  std::cout << "   " << std::endl;
  T om(9);
  for(size_t i = 0; i < 9; i++) { om[i] = in[i]; }
  T res(3); // Just size to 9 as we are going to reuse the variable
  RotationTransforms rt;
  // Convert to Euler
  rt.om2eu<T, float>(om, res);
  Print_EU<T>(res);

  // Convert to Rodriques
  res.resize(4);
  rt.om2ro<T, float>(om, res);
  Print_RO<T, float>(res);

  // Convert to Quaternion
  res.resize(4);
  rt.om2qu<T, float>(om, res);
  Print_QU<T>(res, QuaternionMath<float>::QuaternionScalarVector);

  //Convert to Axis Angle
  res.resize(4);
  rt.om2ax<T, float>(om, res);
  Print_AX<T>(res);

  //Convert to Homochoric
  res.resize(3);
  rt.om2ho<T, float>(om, res);
  Print_HO<T>(res);

  // Convert to HomoChoric
  rt.om2cu<T, float>(om, res);
  Print_CU<T>(res);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Test_om2_XXX()
{
  /*
 ----------------------------------------------------
Euler angles                     :  90.0000000    0.0000000    0.0000000
Axis angle pair [n; angle]       :  -0.0000000   -0.0000000   -1.0000000 ;   90.0000000
Rodrigues vector                 :      -0.0000000       -0.0000000       -1.0000000
Homochoric representation        :  -0.0000000   -0.0000000   -0.7536693
Cubochoric representation        :   0.0000000    0.0000000   -0.6074544
Quaternion                       :   0.7071068   -0.0000000   -0.0000000   -0.7071068
                                   /  0.0000   1.0000   0.0000 \
Orientation Matrix               : | -1.0000   0.0000   0.0000 |
                                   \  0.0000   0.0000   1.0000 /
*/
  std::cout << "Test_om2_XXX  $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << std::endl;
  float om[9] = { 0.0000, 1.0000, 0.0000,
                  -1.0000, 0.0000, 0.0000,
                  0.0000, 0.0000, 1.0000};
  Print_OM<float*>(om);
  OM_2_XXX<RotArrayType>(om);
  OM_2_XXX<FloatVectorType>(om);
  OM_2_XXX<FloatQVectorType>(om);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void RO_2_XXX(float* in)
{
  T ro(9);
  for(size_t i = 0; i < 4; i++) { ro[i] = in[i]; }

  T res(9); // Just size to 9 as we are going to reuse the variable

  RotationTransforms rt;

  // Convert to Euler
  res.resize(3);
  rt.ro2eu<T, float>(ro, res);
  Print_EU<T>(res);

  // Convert to Orientation Matrix
  res.resize(9);
  rt.ro2om<T, float>(ro, res);
  Print_OM<T>(res);

  //Convert to Axis Angle
  res.resize(4);
  rt.ro2ax<T, float>(ro, res);
  Print_AX<T>(res);

  // Convert to Quaternion
  res.resize(4);
  rt.ro2qu<T, float>(ro, res);
  Print_QU<T>(res);

  //Convert to Homochoric
  res.resize(3);
  rt.ro2ho<T, float>(ro, res);
  Print_HO<T>(res);

  // Convert to HomoChoric
  rt.ro2cu<T, float>(ro, res);
  Print_CU<T>(res);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Test_ro2_XXX()
{
  std::cout << "Test_ro2_XXX  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;
  float ro[4] = {0.0f, 0.0f, -1.0f, 1.0f};
  Print_RO<float*, float>(ro);
  RO_2_XXX<RotArrayType>(ro);
  RO_2_XXX<FloatVectorType>(ro);
  RO_2_XXX<FloatQVectorType>(ro);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T, typename K>
void AX_2_XXX(K* in)
{
  T ax(4);
  ax[0] = in[0];
  ax[1] = in[1];
  ax[2] = in[2];
  ax[3] = in[3];

  T res(9);
  RotationTransforms rt;
  // Convert to Orientation Matrix
  rt.ax2om<T, K>(ax, res);
  Print_OM<T>(res);

  //Convert to Axis Angle
  rt.ax2eu<T, K>(ax, res);
  Print_EU<T>(res);

  // Convert to Rodriques
  rt.ax2ro<T, K>(ax, res);
  Print_RO<T, K>(res);

  // Convert to Quaternion
  rt.ax2qu<T, K>(ax, res);
  Print_QU<T>(res);

  // Convert to homochoric
  rt.ax2ho<T, K>(ax, res);
  Print_HO<T>(res);

  // Convert to HomoChoric
  rt.ax2cu<T, float>(ax, res);
  Print_CU<T>(res);

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Test_ax2_XXX()
{
  std::cout << "Test_ax2_XXX  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
  float ax[4] = {0.0f, 0.0f, -1.0f, k_PiOver2};
  Print_AX<float*>(ax);
  AX_2_XXX<RotArrayType>(ax);
  AX_2_XXX<std::vector<float> >(ax);
  AX_2_XXX<FloatQVectorType>(ax);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T, typename K>
void QU_2_XXX(K* in, typename QuaternionMath<K>::Order layout=QuaternionMath<K>::QuaternionVectorScalar)
{
  T qu(4);
  qu[0] = in[0];
  qu[1] = in[1];
  qu[2] = in[2];
  qu[3] = in[3];

  T res(9);
  RotationTransforms rt;
  // Convert to Orientation Matrix
  res.resize(9);
  rt.qu2om<T, K>(qu, res, layout);
  Print_OM<T>(res);

  //Convert to Axis Angle
  res.resize(4);
  rt.qu2eu<T, K>(qu, res, layout);
  Print_EU<T>(res);

  // Convert to Rodriques
  res.resize(4);
  rt.qu2ro<T, K>(qu, res, layout);
  Print_RO<T, K>(res);

  // Convert to Quaternion
  res.resize(4);
  rt.qu2ax<T, K>(qu, res, layout);
  Print_AX<T>(res);

  //Convert to Homochoric
  res.resize(3);
  rt.qu2ho<T, float>(qu, res, layout);
  Print_HO<T>(res);

  // Convert to HomoChoric
  rt.qu2cu<T, float>(qu, res, layout);
  Print_CU<T>(res);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Test_qu2_XXX()
{
  {
    std::cout << "Test_qu2_XXX  (SCALAR, <X, Y, Z>) ***************************************" << std::endl;
    float qu[4] = {DREAM3D::Constants::k_1OverRoot2, 0.0f, 0.0f, -DREAM3D::Constants::k_1OverRoot2};
    Print_QU<float*>(qu, QuaternionMathF::QuaternionScalarVector);
    QU_2_XXX<RotArrayType>(qu, QuaternionMathF::QuaternionScalarVector);
    //  QU_2_XXX<std::vector<float> >(qu);
    //  QU_2_XXX<FloatQVectorType>(qu);
  }

  {
    std::cout << "Test_qu2_XXX  (<X, Y, Z>, SCALAR) ***************************************" << std::endl;
    float qu[4] = {0.0f, 0.0f, -DREAM3D::Constants::k_1OverRoot2, DREAM3D::Constants::k_1OverRoot2};
    Print_QU<float*>(qu);
    QU_2_XXX<RotArrayType>(qu);
    //  QU_2_XXX<std::vector<float> >(qu);
    //  QU_2_XXX<FloatQVectorType>(qu);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T, typename K>
void HO_2_XXX(K* in)
{
  T ho(3);
  ho[0] = in[0];
  ho[1] = in[1];
  ho[2] = in[2];

  T res(9);
  RotationTransforms rt;

  //Convert to Euler
  res.resize(3);
  rt.ho2eu<T, float>(ho, res);
  Print_EU<T>(res);

  // Convert to Orientation Matrix
  res.resize(9);
  rt.ho2om<T, K>(ho, res);
  Print_OM<T>(res);

  //Convert to Axis Angle
  res.resize(4);
  rt.ho2ax<T, K>(ho, res);
  Print_AX<T>(res);

  // Convert to Rodriques
  res.resize(4);
  rt.ho2ro<T, K>(ho, res);
  Print_RO<T, K>(res);

  // Convert to Quaternion
  res.resize(4);
  rt.ho2qu<T, K>(ho, res);
  Print_QU<T>(res);

  // Convert to HomoChoric
  rt.ho2cu<T, float>(ho, res);
  Print_CU<T>(res);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Test_ho2_XXX()
{
  std::cout << "Test_ho2_XXX  &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&" << std::endl;
  float ho[3] = {0.000000f, 0.000000f, -0.7536693215f};
  Print_HO<float*>(ho);
  HO_2_XXX<RotArrayType>(ho);
  HO_2_XXX<std::vector<float> >(ho);
  HO_2_XXX<FloatQVectorType>(ho);
}

// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{

  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( TestRotArray() );
  DREAM3D_REGISTER_TEST( Test_eu_check() );
  DREAM3D_REGISTER_TEST( Test_ro_check() );
  DREAM3D_REGISTER_TEST( Test_ho_check() );
  DREAM3D_REGISTER_TEST( Test_cu_check() );
  DREAM3D_REGISTER_TEST( Test_qu_check() );
  DREAM3D_REGISTER_TEST( Test_ax_check() );
  DREAM3D_REGISTER_TEST( Test_om_check() );
  DREAM3D_REGISTER_TEST( Test_GenRot() )


//  DREAM3D_REGISTER_TEST( Test_eu2_XXX() );
//  DREAM3D_REGISTER_TEST( Test_ax2_XXX() );
//  DREAM3D_REGISTER_TEST( Test_om2_XXX() );
//  DREAM3D_REGISTER_TEST( Test_ro2_XXX() );
  DREAM3D_REGISTER_TEST( Test_qu2_XXX() );
//  DREAM3D_REGISTER_TEST( Test_ho2_XXX() );

  return err;
}
