
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


#include "OrientationLib/Test/TestFileLocations.h"


  typedef RotArray<float> RotArrayType;
  typedef std::vector<float> FloatVectorType;
  typedef QVector<float> FloatQVectorType;


void TestRotArray()
{
  RotArrayF ro(3);
  ro[0] = 2;
  ro[1] = 4;
  ro[2] = 6;
  float sum = ro.sum();
  DREAM3D_REQUIRE_EQUAL(sum, 12);

  DREAM3D_REQUIRE_EQUAL(ro.size(), 3);

  DREAM3D_REQUIRE_EQUAL(ro.maxval(), 6);
  DREAM3D_REQUIRE_EQUAL(ro.minval(), 2);

  RotArrayF ro2(3, 0);
  RotArrayF result = ro * ro2;
  DREAM3D_REQUIRE_EQUAL(result[0], 0);
  DREAM3D_REQUIRE_EQUAL(result[1], 0);
  DREAM3D_REQUIRE_EQUAL(result[2], 0);

  ro2 = ro;
  result = ro * ro2;
  DREAM3D_REQUIRE_EQUAL(result[0], 4);
  DREAM3D_REQUIRE_EQUAL(result[1], 16);
  DREAM3D_REQUIRE_EQUAL(result[2], 36);

  float pro = ro.product();
  DREAM3D_REQUIRE_EQUAL(pro, 48);
  sum = result.sum();
  float max = result.maxval();
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

  RotArrayF eu_r(3);
  eu_r[0] = 1.0f;
  eu_r[1] = 0.4f;
  eu_r[2] = 0.9f;
  result = rt.eu_check<RotArrayF>(eu_r);
  DREAM3D_REQUIRE_EQUAL(result.result, 1);
  eu_r[0] = -1.0;
  result = rt.eu_check<RotArrayF>(eu_r);
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
    RotArrayF ro(4);
    ro[0] = 1.0f;
    ro[1] = 1.0f;
    ro[2] = 1.0f;
    ro[3] = 1.0f;
    MatrixMath::Normalize3x1(&(ro[0]));
    result = rt.ro_check<RotArrayF, float>(ro);
    DREAM3D_REQUIRE_EQUAL(result.result, 1);
    ro[3] = -1.0;
    result = rt.ro_check<RotArrayF, float>(ro);
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
    RotArrayF ho(3);
    ho[0] = 0.5f;
    ho[1] = 0.5f;
    ho[2] = 0.5f;
    result = rt.ho_check<RotArrayF, float>(ho);
    DREAM3D_REQUIRE_EQUAL(result.result, 1);
    ho[2] = 8.0;
    result = rt.ho_check<RotArrayF, float>(ho);
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
    RotArrayF v(3);
    v[0] = 0.5f;
    v[1] = 0.5f;
    v[2] = 0.5f;
    result = rt.cu_check<RotArrayF, float>(v);
    DREAM3D_REQUIRE_EQUAL(result.result, 1);
    v[2] = 8.0;
    result = rt.cu_check<RotArrayF, float>(v);
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
    RotArrayF qu(4);
    qu[0] = quat.x;
    qu[1] = quat.y;
    qu[2] = quat.z;
    qu[3] = quat.w;
    MatrixMath::Normalize3x1(&(qu[0]));
    result = rt.qu_check<RotArrayF, float>(qu);
    DREAM3D_REQUIRE_EQUAL(result.result, 1);

    qu[0] = 1.5f;
    qu[1] = 3.0f;
    qu[2] = 2.0f;
    result = rt.qu_check<RotArrayF, float>(qu);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);

    qu[0] = -1.0;
    result = rt.qu_check<RotArrayF, float>(qu);
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
    RotArrayF ax(4);
    ax[0] = 0.0f;
    ax[1] = 0.0f;
    ax[2] = 1.0f;
    ax[3] = M_PI-0.00001;
    result = rt.ax_check<RotArrayF, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 1);

    ax[0] = 1.0;
    result = rt.ax_check<RotArrayF, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);

    ax[3] = M_PI + 1.0;
    result = rt.ax_check<RotArrayF, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);
  }

  {
    FloatVectorType ax(4);
    ax[0] = 0.0f;
    ax[1] = 0.0f;
    ax[2] = 1.0f;
    ax[3] = M_PI-0.00001;
    result = rt.ax_check<FloatVectorType, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 1);

    ax[0] = 1.0;
    result = rt.ax_check<FloatVectorType, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);

    ax[3] = M_PI + 1.0;
    result = rt.ax_check<FloatVectorType, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);
  }

  {
    FloatQVectorType ax(4);
    ax[0] = 0.0f;
    ax[1] = 0.0f;
    ax[2] = 1.0f;
    ax[3] = M_PI-0.00001;
    result = rt.ax_check<FloatQVectorType, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 1);

    ax[0] = 1.0;
    result = rt.ax_check<FloatQVectorType, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);

    ax[3] = M_PI + 1.0;
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
    RotArrayF ax(9);
    ax[0] = 1.0f;
    ax[1] = 0.0f;
    ax[2] = 0.0f;
    ax[3] = 0.0f;
    ax[4] = 1.0f;
    ax[5] = 0.0f;
    ax[6] = 0.0f;
    ax[7] = 0.0f;
    ax[8] = 1.0f;
    result = rt.om_check<RotArrayF, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 1);

    ax[1] = -16.0;
    ax[6] = -12.0;
    result = rt.om_check<RotArrayF, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);

    ax[3] = M_PI + 1.0;
    result = rt.om_check<RotArrayF, float>(ax);
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

    ax[1] = -16.0;
    ax[6] = -12.0;
    result = rt.om_check<FloatVectorType, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);

    ax[3] = M_PI + 1.0;
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

    ax[1] = -16.0;
    ax[6] = -12.0;
    result = rt.om_check<FloatQVectorType, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);

    ax[3] = M_PI + 1.0;
    result = rt.om_check<FloatQVectorType, float>(ax);
    DREAM3D_REQUIRE_EQUAL(result.result, 0);
  }

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
template<typename T>
void Print_RO(const T& om)
{
  printf("Rodrigues vector                 :   % 3.6f    % 3.6f    % 3.6f\n", om[0], om[1], om[2] );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void Print_QU(const T& om)
{
  printf("Quaternion (Scalar<vector>)     :   % 3.6f   <% 3.6f    % 3.6f    % 3.6f>\n", om[0], om[1], om[2], om[3] );
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
  Print_RO<T>(res);

  // Convert to Quaternion
  rt.eu2qu<T, float>(eu, res);
  Print_QU<T>(res);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void OM_2_XXX(float* in)
{
  T om(9);
  for(size_t i = 0; i < 9; i++) { om[i] = in[i]; }
  T res(9); // Just size to 9 as we are going to reuse the variable
  RotationTransforms rt;
  // Convert to Euler
  rt.om2eu<T, float>(om, res);
  Print_EU<T>(res);

  // Transpose array to be Column Major Format for the BLAS/LAPACK routine
  om[1] = in[3];
  om[3] = in[1];
  om[2] = in[6];
  om[6] = in[2];
  om[7] = in[5];
  om[5] = in[7];
  //Convert to Axis Angle
  rt.om2ax<T, float>(om, res);
  Print_AX<T>(res);


#if 0
  // Convert to Rodriques
  rt.om2ro<T, float>(om, res);
  Print_RO<T>(res);

  // Convert to Quaternion
  rt.om2qu<T, float>(om, res);
  Print_QU<T>(res);
#endif
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
#if 0
  // Convert to Euler
  rt.ro2eu<T, float>(ro, res);
  Print_EU<T>(res);

  // Convert to Orientation Matrix
  rt.ro2om<T, float>(ro, res);
  Print_OM<T>(res);
#endif

  //Convert to Axis Angle
  rt.ro2ax<T, float>(ro, res);
  Print_AX<T>(res);

#if 0
  // Convert to Quaternion
  rt.ro2qu<T, float>(ro, res);
  Print_QU<T>(res);
#endif
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
//  EU_2_XXX<FloatVectorType>(eu);
//  EU_2_XXX<FloatQVectorType>(eu);


}

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
void Test_om2_XXX()
{
/*
                                 : /     0.000000     1.000000     0.000000    \
Orientation Matrix               : |    -1.000000     0.000000     0.000000    |
                                 : \     0.000000     0.000000     1.000000    /
*/
  std::cout << "Test_om2_XXX  $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << std::endl;
  float om[9] = { 0.0000, 1.0000, 0.0000,
                  -1.0000, 0.0000, 0.0000,
                  0.0000, 0.0000, 1.0000};
  Print_OM<float*>(om);
  OM_2_XXX<RotArrayType>(om);
//  OM_2_XXX<FloatVectorType>(om);
//  OM_2_XXX<FloatQVectorType>(om);



//  om[0] = 0.0000; om[1] = 1.0000; om[2] =  0.0000;
//  om[3] = -1.0000; om[4] = 0.0000; om[5] =  0.0000;
//  om[6] = 0.0000; om[7] =  0.0000; om[8] =  1.0000;


  typedef Eigen::Matrix<float, 3, 3, Eigen::RowMajor> Matrix_t;

  Eigen::Map<Matrix_t> testMat(const_cast<float*>(&(om[0])));

  Eigen::EigenSolver<Matrix_t> solver;
  solver.compute(testMat);
  Eigen::EigenSolver<Matrix_t>::EigenvalueType w = solver.eigenvalues();
  Eigen::EigenSolver<Matrix_t>::EigenvectorsType VR = solver.eigenvectors();
//  std::cout << "00000000000000000000000000000000000000000000000000000" << std::endl;
//  for(int c = 0; c < bar2.cols(); c++)
//  {
//    for(int r = 0; r < bar2.rows(); r++)
//    {
//      std::cout << bar2(r,c)<< "\t";
//    }
//    std::cout << std::endl;

//  }


  //     std::cout << "The eigenvalues of A are:" << std::endl << solver.eigenvalues() << std::endl;
  std::cout << "The matrix of eigenvectors, V2, is:" << std::endl << VR << std::endl;
#if 1
  float thr = 1.0E-7;
  float res[3] = { 0.0, 0.0, 0.0};
  for(int i = 0; i < 3; i++)
  {
    std::complex<float> cone(1.0,0.0);
    std::complex<float> ev = std::complex<float>( w(i).real(), w(i).imag() );
    if (std::abs(ev-cone) < thr)
    {
      res[0] = VR(i, 0).real();
      res[1] = VR(i, 1).real();
      res[2] = VR(i, 2).real();

      if ((om[5]-om[7]) != 0.0) { res[0] = transfer_sign(res[0],-RConst::epsijk*(om[7]-om[5])); }
      if ((om[6]-om[2]) != 0.0) { res[1] = transfer_sign(res[1],-RConst::epsijk*(om[2]-om[6 ])); }
      if ((om[1]-om[3]) != 0.0) { res[2] = transfer_sign(res[2],-RConst::epsijk*(om[3]-om[1])); }

      return;
    }

  }
std::cout << "=========================================================" << std::endl;
  std::cout << res[0] << "\t" << res[1] << "\t" << res[2] << std::endl;
#endif


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Test_ro2_XXX()
{
  std::cout << "Test_ro2_XXX  $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << std::endl;
  float ro[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  Print_RO<float*>(ro);
  RO_2_XXX<RotArrayType>(ro);
  RO_2_XXX<FloatVectorType>(ro);
  RO_2_XXX<FloatQVectorType>(ro);
}


void foo()
{
#define LDA 5
#define N 5

      float a[LDA*N] = {
           -1.01f,  0.86f, -4.60f,  3.31f, -4.81f,
            3.98f,  0.53f, -7.04f,  5.29f,  3.55f,
            3.30f,  8.26f, -3.89f,  8.20f, -1.51f,
            4.43f,  4.96f, -7.66f, -7.33f,  6.18f,
            7.31f, -6.43f, -6.16f,  2.47f,  5.58f
        };
  typedef Eigen::Matrix<float, LDA, N, Eigen::RowMajor> Matrix_t;

  Eigen::Map<Matrix_t> testMat(a);

  Eigen::EigenSolver<Matrix_t> solver;
  solver.compute(testMat);

  Eigen::EigenSolver<Matrix_t>::EigenvectorsType bar2 = solver.eigenvectors();
  std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;
  for(int c = 0; c < bar2.cols(); c++)
  {
    for(int r = 0; r < bar2.rows(); r++)
    {
      std::cout << bar2(r,c)<< "\t";
    }
    std::cout << std::endl;

  }

  Eigen::EigenSolver<Matrix_t>::EigenvalueType ev = solver.eigenvalues();
  std::cout << "Eigin Values: " << std::endl << ev << std::endl;

  //     std::cout << "The eigenvalues of A are:" << std::endl << solver.eigenvalues() << std::endl;
  std::cout << "The matrix of eigenvectors, V2, is:" << std::endl << bar2 << std::endl;
}


// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  foo();

  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( TestRotArray() );
  DREAM3D_REGISTER_TEST( Test_eu_check() );
  DREAM3D_REGISTER_TEST( Test_ro_check() );
  DREAM3D_REGISTER_TEST( Test_ho_check() );
  DREAM3D_REGISTER_TEST( Test_cu_check() );
  DREAM3D_REGISTER_TEST( Test_qu_check() );
  DREAM3D_REGISTER_TEST( Test_ax_check() );
  DREAM3D_REGISTER_TEST( Test_om_check() );


  DREAM3D_REGISTER_TEST( Test_eu2_XXX() );
  DREAM3D_REGISTER_TEST( Test_om2_XXX() );
  DREAM3D_REGISTER_TEST( Test_ro2_XXX() );


  return err;
}
