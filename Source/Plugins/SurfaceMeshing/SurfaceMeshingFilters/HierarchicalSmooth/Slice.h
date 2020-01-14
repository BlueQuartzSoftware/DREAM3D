/*
 This code was extracted from libigl, https://github.com/libigl/libigl,
 from commit 87b54fc4dcc5aea182dd842a55ac75536fb69e1a.
 This file is a composition of the following files:
  slice.h
  slice.cpp
  igl_inline.h
  colon.h
  colon.cpp
  LinSpaced.h
*/

// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2013 Alec Jacobson <alecjacobson@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <Eigen/Core>
#include <Eigen/Sparse>
#include <unsupported/Eigen/SparseExtra>

namespace slice
{

template <typename Derived>
// inline typename Eigen::DenseBase< Derived >::RandomAccessLinSpacedReturnType
inline Derived LinSpaced(typename Derived::Index size, const typename Derived::Scalar& low, const typename Derived::Scalar& high)
{
  if(size == 0)
  {
    // Force empty vector with correct "RandomAccessLinSpacedReturnType" type.
    return Derived::LinSpaced(0, 0, 1);
  }
  else if(high < low)
  {
    return low - Derived::LinSpaced(size, low - low, low - high).array();
  }
  else
  {
    return Derived::LinSpaced(size, low, high);
  }
}

template <typename L, typename S, typename H, typename T>
void colon(const L low, const S step, const H hi, Eigen::Matrix<T, Eigen::Dynamic, 1>& I)
{
  const int size = ((hi - low) / step) + 1;
  I = LinSpaced<Eigen::Matrix<T, Eigen::Dynamic, 1>>(size, low, low + step * (size - 1));
}

template <typename L, typename H, typename T>
void colon(const L low, const H hi, Eigen::Matrix<T, Eigen::Dynamic, 1>& I)
{
  return colon(low, (T)1, hi, I);
}

template <typename T, typename L, typename H>
Eigen::Matrix<T, Eigen::Dynamic, 1> colon(const L low, const H hi)
{
  Eigen::Matrix<T, Eigen::Dynamic, 1> I;
  colon(low, hi, I);
  return I;
}

template <typename TX, typename TY>
void slice(const Eigen::SparseMatrix<TX>& X, const Eigen::Matrix<int, Eigen::Dynamic, 1>& R, const Eigen::Matrix<int, Eigen::Dynamic, 1>& C, Eigen::SparseMatrix<TY>& Y)
{
  int xm = X.rows();
  int xn = X.cols();
  int ym = R.size();
  int yn = C.size();

  // special case when R or C is empty
  if(ym == 0 || yn == 0)
  {
    Y.resize(ym, yn);
    return;
  }

  assert(R.minCoeff() >= 0);
  assert(R.maxCoeff() < xm);
  assert(C.minCoeff() >= 0);
  assert(C.maxCoeff() < xn);

  // Build reindexing maps for columns and rows, -1 means not in map
  std::vector<std::vector<int>> RI;
  RI.resize(xm);
  for(int i = 0; i < ym; i++)
  {
    RI[R(i)].push_back(i);
  }
  std::vector<std::vector<int>> CI;
  CI.resize(xn);
  // initialize to -1
  for(int i = 0; i < yn; i++)
  {
    CI[C(i)].push_back(i);
  }
  // Resize output
  Eigen::DynamicSparseMatrix<TY, Eigen::RowMajor> dyn_Y(ym, yn);
  // Take a guess at the number of nonzeros (this assumes uniform distribution
  // not banded or heavily diagonal)
  dyn_Y.reserve((X.nonZeros() / (X.rows() * X.cols())) * (ym * yn));
  // Iterate over outside
  for(int k = 0; k < X.outerSize(); ++k)
  {
    // Iterate over inside
    for(typename Eigen::SparseMatrix<TX>::InnerIterator it(X, k); it; ++it)
    {
      std::vector<int>::iterator rit, cit;
      for(rit = RI[it.row()].begin(); rit != RI[it.row()].end(); rit++)
      {
        for(cit = CI[it.col()].begin(); cit != CI[it.col()].end(); cit++)
        {
          dyn_Y.coeffRef(*rit, *cit) = it.value();
        }
      }
    }
  }
  Y = Eigen::SparseMatrix<TY>(dyn_Y);
}

template <typename MatX, typename DerivedR, typename MatY>
void slice(const MatX& X, const Eigen::DenseBase<DerivedR>& R, const int dim, MatY& Y)
{
  Eigen::Matrix<typename DerivedR::Scalar, Eigen::Dynamic, 1> C;
  switch(dim)
  {
  case 1:
    // boring base case
    if(X.cols() == 0)
    {
      Y.resize(R.size(), 0);
      return;
    }
    colon(0, X.cols() - 1, C);
    return slice(X, R, C, Y);
  case 2:
    // boring base case
    if(X.rows() == 0)
    {
      Y.resize(0, R.size());
      return;
    }
    colon(0, X.rows() - 1, C);
    return slice(X, C, R, Y);
  default:
    assert(false && "Unsupported dimension");
    return;
  }
}

template <typename DerivedX, typename DerivedR, typename DerivedC, typename DerivedY>
void slice(const Eigen::DenseBase<DerivedX>& X, const Eigen::DenseBase<DerivedR>& R, const Eigen::DenseBase<DerivedC>& C, Eigen::PlainObjectBase<DerivedY>& Y)
{
#ifndef NDEBUG
  int xm = X.rows();
  int xn = X.cols();
#endif
  int ym = R.size();
  int yn = C.size();

  // special case when R or C is empty
  if(ym == 0 || yn == 0)
  {
    Y.resize(ym, yn);
    return;
  }

  assert(R.minCoeff() >= 0);
  assert(R.maxCoeff() < xm);
  assert(C.minCoeff() >= 0);
  assert(C.maxCoeff() < xn);

  // Resize output
  Y.resize(ym, yn);
  // loop over output rows, then columns
  for(int i = 0; i < ym; i++)
  {
    for(int j = 0; j < yn; j++)
    {
      Y(i, j) = X(R(i), C(j));
    }
  }
}

template <typename DerivedX, typename DerivedY>
void slice(const Eigen::DenseBase<DerivedX>& X, const Eigen::Matrix<int, Eigen::Dynamic, 1>& R, Eigen::PlainObjectBase<DerivedY>& Y)
{
  // phony column indices
  Eigen::Matrix<int, Eigen::Dynamic, 1> C;
  C.resize(1);
  C(0) = 0;
  return slice(X, R, C, Y);
}

template <typename DerivedX>
DerivedX slice(const Eigen::DenseBase<DerivedX>& X, const Eigen::Matrix<int, Eigen::Dynamic, 1>& R)
{
  DerivedX Y;
  slice(X, R, Y);
  return Y;
}

template <typename DerivedX>
DerivedX slice(const Eigen::DenseBase<DerivedX>& X, const Eigen::Matrix<int, Eigen::Dynamic, 1>& R, const int dim)
{
  DerivedX Y;
  slice(X, R, dim, Y);
  return Y;
}

} // namespace slice
