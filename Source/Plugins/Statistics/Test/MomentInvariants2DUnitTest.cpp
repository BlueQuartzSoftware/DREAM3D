
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <cmath>

#include <Eigen/Dense>

#include "Statistics/StatisticsFilters/util/MomentInvariants2D.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestMatrixMultiply()
{
  std::cout << "TestMatrixMultiply" << std::endl;
  std::vector<int> A = {1,4,2,5,3,6}; // Row Major, 3Row x 2Col
  std::vector<int> B = {7,8,9,10,11,12}; // Row Major, 2Row x 3Col

  std::vector<int> c = MomentInvariants2D::MatrixMultiply(A, 2, 3, B, 3, 2);
  MomentInvariants2D::print2D(c, 3, 3);

  std::vector<int> d = {5,4,4,0,7,10,-1,8};
  MomentInvariants2D::print2D(d, 2, 4);
  std::vector<int> dT = MomentInvariants2D::Transpose(d, 2, 4);
  MomentInvariants2D::print2D(dT, 4, 2);
  std::vector<int> dTT = MomentInvariants2D::Transpose(dT, 4, 2);
  MomentInvariants2D::print2D(dTT, 2, 4);



}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestCopyRow()
{
  std::cout << "TestCopyRow" << std::endl;
  std::vector<int> src(4 * 1); // COLS = 4  ROWS=1
  size_t i = 0;
  std::transform(std::begin(src),std::end(src),std::begin(src), [&i](int n){ return i++; });
  std::cout << "Input Matrix" << std::endl;
  MomentInvariants2D::print2D(src, 4, 1);

  std::vector<int> tgt(2 * 4, 0); // COLS=2 ROWS=4
  std::cout << "Target Matrix" << std::endl;
  MomentInvariants2D::print2D(tgt, 2, 4);

  MomentInvariants2D::copyRowIntoColumn(src, 0, 4, 1, tgt, 1, 2, 4);
  std::cout << "Finished Matrix" << std::endl;
  MomentInvariants2D::print2D(tgt, 2, 4);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestBinomial()
{

  MomentInvariants2D moments;
  size_t max_order = 2;
  {
    std::vector<double> bn;

    moments.binomial(max_order, bn);
    std::cout << "Binomial Test" << std::endl;
    MomentInvariants2D::print2D(bn, max_order+1, max_order+1);
  }
  {
    MomentInvariants2D::DoubleMatrixType binomial = moments.binomial(max_order);
    std::cout << "binomial" << std::endl;
    std::cout << binomial << std::endl;
  }


}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestBigX()
{
  MomentInvariants2D moments;
  size_t max_order = 2;
  size_t dim = 16;
  {
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
    MomentInvariants2D::DoubleMatrixType bigX = moments.getBigX(max_order, dim);
    std::cout << "bigX=" << std::endl;
    std::cout << bigX << std::endl;
  }

  {
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
    std::vector<double> bigx;
    moments.getBigX(max_order, dim, bigx);
  }



}

#define MI_IDX(X,Y,W)\
  (Y*W+X)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestComputeMoments2D()
{
  MomentInvariants2D moments;
  size_t max_order = 2;
  size_t mDim = max_order + 1;
  size_t imageDim = 5; // The algorithm takes a square image
  //  std::vector<double> bigx;
  //  moments.getBigX(max_order, imageDim, bigx);

  size_t inputDims[2] = {imageDim, imageDim};

{
  std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;

  MomentInvariants2D::DoubleMatrixType input2D(5, 5);
  input2D << 0,0,0,0,0, /*Row*/0,1,1,1,0, /*Row*/0,1,1,1,0, /*Row*/0,1,1,1,0, /*Row*/0,0,0,0,0;
  MomentInvariants2D::DoubleMatrixType m2D = moments.computeMoments2D(input2D, inputDims, max_order);
  std::cout << "centralMoments=\n" << m2D << std::endl;
  // compute the second order moment invariants
  double omega1 = 2.0 * (m2D(0,0)* m2D(0,0)) / (m2D(0,2) + m2D(2,0));
  double omega2 = std::pow(m2D(0,0), 4) / ( m2D(2,0)*m2D(0,2) - std::pow(m2D(1,1), 2) );
  std::cout << ",'2D moment invariants : " << omega1 << "\t" << omega2 << " (should be 12 and 144)" << std::endl;


  // normalize the invariants by those of the circle
  double circle_omega[2] = { 4.0*M_PI, 16.0 * M_PI*M_PI };
  omega1 /= circle_omega[0];
  omega2 /= circle_omega[1];
  std::cout << "normalized moment invariants: " << omega1 << "\t" << omega2 << std::endl;
}
{

  std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;

  std::vector<double> input = {0,0,0,0,0, /*Row*/0,1,1,1,0, /*Row*/0,1,1,1,0, /*Row*/0,1,1,1,0, /*Row*/0,0,0,0,0};

  std::vector<double> m = moments.computeMoments2D(input, inputDims, max_order);

  std::cout << "centralMoments=" << std::endl;
  moments.print2D(m, max_order + 1, max_order + 1);

  // compute the second order moment invariants
  double omega1 = 2.0 * (m[MI_IDX(0,0,mDim)]* m[MI_IDX(0,0,mDim)]) / (m[MI_IDX(2,0,mDim)] + m[MI_IDX(0,2,mDim)]);
  double omega2 = std::pow(m[MI_IDX(0,0,mDim)], 4) / ( m[MI_IDX(2,0,mDim)]*m[MI_IDX(0,2,mDim)] - std::pow(m[MI_IDX(1,1,mDim)], 2) );
  std::cout << ",'2D moment invariants : " << omega1 << "\t" << omega2 << " (should be 12 and 144)" << std::endl;


  // normalize the invariants by those of the circle
  double circle_omega[2] = { 4.0*M_PI, 16.0 * M_PI*M_PI };
  omega1 /= circle_omega[0];
  omega2 /= circle_omega[1];
  std::cout << "normalized moment invariants: " << omega1 << "\t" << omega2 << std::endl;
}


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  //  TestMatrixMultiply();
  //  TestCopyRow();

//  TestBinomial();
//  TestBigX();

  TestComputeMoments2D();
  return EXIT_SUCCESS;
}
