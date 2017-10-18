/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Software, LLC
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

#include "MomentInvariants2D.h"
#include <cmath>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MomentInvariants2D::MomentInvariants2D()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MomentInvariants2D::~MomentInvariants2D()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MomentInvariants2D::factorial(int n) const
{
  return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MomentInvariants2D::DoubleMatrixType MomentInvariants2D::binomial(size_t max_order)
{
  int dim = static_cast<int>(max_order + 1);
  DoubleMatrixType bn(dim, dim);
  bn.setZero();

  for(int i = 0; i < dim; i++)
  {
    for(int j = 0; j <= i; j++)
    {
      bn(i, j) = (factorial(i)) / (factorial(j)) / (factorial(i - j));
      bn(j, i) = bn(i, j);
    }
  }
  return bn;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MomentInvariants2D::DoubleMatrixType MomentInvariants2D::getBigX(size_t max_order, size_t dim)
{

  int dRows = static_cast<int>(dim);
  int dCols = static_cast<int>(dim + 1);

  DoubleMatrixType xx(1, dCols);
  for(int c = 0; c < dCols; c++)
  {
    xx(0, c) = c - static_cast<double>(dim) / 2.0 - 0.5;
  }

  double fnorm = xx.maxCoeff();
  xx = xx / fnorm;

  DoubleMatrixType D(dRows, dCols);
  D.setZero();

  IntMatrixType j(1, dRows);
  for(int c = 0; c < dRows; c++)
  {
    j(0, c) = c;
  }

  for(int r = 0; r < dRows; r++)
  {
    D(r, r) = -1.0;
    D(r, r + 1) = 1.0;
  }

  // Set the Scale Factors
  DoubleMatrixType sc(1, max_order + 1);
  int mop1 = static_cast<int>(max_order + 1);
  for(int c = 0; c < mop1; c++)
  {
    sc(0, c) = 1.0 / (c + 1.0);
  }

  DoubleMatrixType bigx(dim, max_order + 1);
  bigx.setZero();

  DoubleMatrixType yy;
  for(int i = 0; i < mop1; i++)
  {
    if(i == 0)
    {
      yy = xx;
    }
    else
    {
      yy = yy.cwiseProduct(xx);
    }

    DoubleMatrixType mm = yy * D.transpose();
    mm = mm * sc(0, i);
    bigx.col(i) = mm.row(0);
  }

  return bigx;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MomentInvariants2D::DoubleMatrixType MomentInvariants2D::computeMomentInvariants(DoubleMatrixType& input, size_t* inputDims, size_t max_order)
{
  assert(inputDims[0] == inputDims[1]);
  size_t dim = inputDims[0];
  DoubleMatrixType bigX = getBigX(max_order, inputDims[0]);

  int mDim = static_cast<int>(max_order + 1);
  double fnorm = static_cast<double>(dim - 1) / 2.0;

  // precompute the binomial coefficients for central moment conversion;  (could be hard-coded for max_order = 2)
  DoubleMatrixType bn = binomial(max_order);

  DoubleMatrixType mnk(mDim, mDim);
  mnk.setZero();

  DoubleMatrixType inter = input * bigX;

  mnk = bigX.transpose() * inter;

  for(int c = 0; c < mDim; c++)
  {
    for(int r = 0; r < mDim; r++)
    {
      mnk(r, c) *= std::pow(fnorm, (2 + c + r));
    }
  }

  // transform the moments to central moments using the binomial theorem
  // first get the center of mass coordinates (xc, yc)
  double xc = mnk(1, 0) / mnk(0, 0); // mnk[0,0] is the area of the object in units of pixels
  double yc = mnk(0, 1) / mnk(0, 0);

  // declare an intermediate array to hold the transformed moment values
  DoubleMatrixType mnknew(mDim, mDim);
  mnknew.setZero();

  // apply the binomial theorem
  for(int p = 0; p < mDim; p++)
  {
    for(int q = 0; q < mDim; q++)
    {
      for(int k = 0; k < p + 1; k++)
      {
        for(int l = 0; l < q + 1; l++)
        {
          mnknew(p, q) += std::pow(-1.0, (p + q - k - l)) * std::pow(xc, (p - k)) * std::pow(yc, (q - l)) * bn(p, k) * bn(q, l) * mnk(k, l);
        }
      }
    }
  }

  return mnknew;
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MomentInvariants2D::binomial(int p, std::vector<double>& bn)
{
  size_t dim = p + 1;
  bn.resize(dim * dim, 0.0);
  size_t index0 = 0;
  size_t index1 = 0;
  for(int i = 0; i < dim; i++)
  {
    for(int j = 0; j <= i; j++)
    {
      index0 = (i * dim) + j;
      index1 = (j * dim) + i;
      bn[index0] = (factorial(i)) / (factorial(j)) / (factorial(i - j));
      bn[index1] = bn[index0];
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MomentInvariants2D::getBigX(size_t max_order, size_t dim, std::vector<double>& bigx)
{
  std::vector<double> xx = dindgen(dim + 1); // COLS=dims+1 ROWS=1
  std::transform(std::begin(xx), std::end(xx), std::begin(xx), [dim](double n) { return n - static_cast<double>(dim) / 2.0 - 0.5; });
  double fnorm = *(std::max_element(xx.begin(), xx.end()));
  std::transform(std::begin(xx), std::end(xx), std::begin(xx), [fnorm](double n) { return n / fnorm; });
  //  std::cout << "xx" << std::endl;
  //  print2D(xx, dim + 1, 1);

  size_t dRows = dim + 1;
  size_t dCols = dim;
  std::vector<double> D = dblarr(dCols, dRows); // COLS=dim, ROWS=dim+1
  std::vector<int> j = indgen(dim);             //
  size_t idx = 0;
  // size_t c = 0;
  // -1 across the diagonal
  for(size_t r = 0; r < dCols; r++)
  {
    idx = r * dCols + r;
    D[idx] = -1.0;
    idx = (r + 1) * dCols + r;
    D[idx] = 1.0;
  }

  //  std::cout << "#----------- D" << std::endl;
  //  print2D(D, dCols, dRows);

  // Set the Scale Factors
  idx = 0;
  std::vector<double> sc = dindgen(max_order + 1); // COLS=max_order+1 ROWS=1
  std::transform(std::begin(sc), std::end(sc), std::begin(sc), [](double n) { return 1.0 / (n + 1.0); });
  //  std::cout << "#----------- sc" << std::endl;
  //  print2D(sc, 1, max_order + 1);

  bigx.resize((max_order + 1) * dim, 0.0); // COLS=max_order+1  ROWS=dim
  std::vector<double> yy;                  // COLS=1 ROWS=dims+1
  for(size_t i = 0; i <= max_order; i++)
  {
    if(i == 0)
    {
      yy = xx;
    }
    else
    {
      yy = multiplyComponents(yy, xx);
    }
    std::cout << "----------------------" << std::endl;
    std::cout << "i=" << i << "  ";
    std::cout << "yy=" << std::endl;
    print2D(yy, dRows, 1);
    //                                                 C  R   C     R
    std::vector<double> mm = MatrixMultiply(yy, dRows, 1, D, dCols, dRows);
    std::cout << "mm=" << std::endl;
    print2D(mm, 1, dCols);

    std::transform(std::begin(mm), std::end(mm), std::begin(mm), [sc, i](double n) { return n * sc[i]; });
    std::cout << "mm*sc[i]=" << std::endl;
    print2D(mm, 1, dCols);

    copyRowIntoColumn(mm, 0, dim, 1, bigx, i, max_order + 1, dim);
  }

  std::cout << "#----------- bigx" << std::endl;
  print2D(bigx, max_order + 1, dim);
}

#define MI_IDX(X, Y, W) (Y * W + X)
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<double> MomentInvariants2D::computeMoments2D(std::vector<double> &input, size_t* inputDims, size_t max_order)
{
  assert(inputDims[0] == inputDims[1]);

  size_t dim = inputDims[0];
  std::vector<double> bigX;
  getBigX(max_order,inputDims[0],bigX);
  // bigX = COLS=max_order+1  ROWS=dim

  size_t mDim = max_order + 1;
  //size_t sz[2] = {mDim, dim};
  double fnorm = static_cast<double>(dim-1) / 2.0;

  // precompute the binomial coefficients for central moment conversion;  (could be hard-coded for max_order = 2)
  std::vector<double> bn;
  binomial(max_order, bn);
  print2D(bn, mDim, mDim);


  // the 2-D computation:  a double tensor contraction written as a double summation loop
  std::vector<double> mnk = dblarr(mDim, mDim); // COLS=max_order + 1, ROWS=max_order+1
  // in IDL, this is the corresponding command: mnk = MATRIX_MULTIPLY(lbigX,MATRIX_MULTIPLY(input,lbigX,/BTRANSPOSE))
  //  bigX  .  input . transpose(bigX) =  mnk
  //  3xdim    dimxdim     dimx3          3x3
  // written as an explicit quadruple summation we have:

  std::vector<double> bigXT = Transpose(bigX, mDim, dim);
  std::vector<double> inter = MatrixMultiply(bigXT, dim, mDim, input, inputDims[0], inputDims[1]);

  mnk = MatrixMultiply(inter, dim, max_order + 1, bigX, mDim, dim);
  std::cout << "mnk=" << std::endl;
  print2D(mnk, mDim, mDim);

  //then we normalize the mnk array by appropriate powers of fnorm
  for (size_t i=0; i < mDim; i++)
  {
    for (size_t j=0; j < mDim; j++)
    {
      //mnk[i,j] *= fnorm^(2+i+j);

      mnk[j*mDim + i] *= std::pow(fnorm,(2+i+j));
    }
  }
  std::cout << "mnk (normalized)=" << std::endl;
  print2D(mnk, mDim, mDim);

  //transform the moments to central moments using the binomial theorem
  //first get the center of mass coordinates (xc, yc)
  double xc = mnk[MI_IDX(1,0,mDim)]/mnk[MI_IDX(0,0,mDim)];    // mnk[0,0] is the area of the object in units of pixels
  double yc = mnk[MI_IDX(0,1,mDim)]/mnk[MI_IDX(0,0,mDim)];

  //declare an intermediate array to hold the transformed moment values
  std::vector<double> mnknew (mDim * mDim, 0.0);

  // apply the binomial theorem
  for(size_t p = 0; p < mDim; p++)
  {
    for(size_t q = 0; q < mDim; q++)
    {
      for(size_t k = 0; k < p + 1; k++)
      {
        for(size_t l = 0; l < q + 1; l++)
        {
          mnknew[MI_IDX(p, q, mDim)] += std::pow(-1.0, (p + q - k - l)) * std::pow(xc, (p - k)) * std::pow(yc, (q - l))
              * bn[MI_IDX(p, k, mDim)] * bn[MI_IDX(q, l, mDim)] * mnk[MI_IDX(k, l, mDim)];
        }
      }
    }
  }

  std::cout << "mnknew=" << std::endl;
  print2D(mnknew, mDim, mDim);

  return mnknew;


}
#endif
