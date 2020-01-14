/*
 This code was extracted from HierarchicalSmooth, https://github.com/siddharth-maddali/HierarchicalSmooth,
 from commit 31ef680011f4bbaef59c0944876e84222ea7c49f
*/

// Copyright (c) 2016-2018, Siddharth Maddali
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of Carnegie Mellon University nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

/*
 *
 * HierarchicalSmooth.cpp -- implementation of the routines declared in HierarchicalSmooth.h
 *
 */

#include "HierarchicalSmooth.h"

#include <cmath>

#include "Base.h"
#include "Slice.h"

//============================================================================================

SparseMatrixD HSmoothMain::laplacian2D(int N, const std::string& type)
{
  std::vector<TripletD> tripletList;
  tripletList.reserve(3 * N); // approx. number of nonzero elements
  for(int i = 0; i < N; i++)
  {
    tripletList.push_back(TripletD(i, i, -1.0));
    if(i != N - 1)
      tripletList.push_back(TripletD(i, i + 1, 1.0));
  }
  SparseMatrixD temp(N, N);
  temp.setFromTriplets(tripletList.begin(), tripletList.end());
  SparseMatrixD L = SparseMatrixD(temp.transpose()) * temp;
  if(type == "serial")
  {
    L.coeffRef(N - 1, N - 1) = 1.0;
  }
  else if(type == "cyclic")
  {
    L.coeffRef(0, 0) = 2.0;
    L.coeffRef(0, N - 1) = L.coeffRef(N - 1, 0) = -1.0;
  }
  else
    std::cerr << "HSmoothMain::Laplacian2D: Unrecognized type. " << std::endl;

  L.makeCompressed();
  return L;
}

//============================================================================================

std::tuple<SparseMatrixD, std::vector<int>> HSmoothMain::graphLaplacian(const TriMesh& tri)
{
  std::vector<int> nUnique;
  for(int i = 0; i < tri.rows(); i++)
    for(int j = 0; j < tri.cols(); j++)
      nUnique.push_back(tri(i, j));
  std::sort(nUnique.begin(), nUnique.end());
  nUnique.erase(std::unique(nUnique.begin(), nUnique.end()), nUnique.end());

  std::vector<TripletD> tripletList;
  tripletList.reserve(nUnique.size() + tri.rows() * tri.cols() * 2);

  std::vector<double> fDiagCount(nUnique.size(), 0.0);

  TriMesh nSubTri = HSmoothBase::isMember(tri, nUnique);

  DictBase<int>::EdgeDict MyDict;
  for(int i = 0; i < nSubTri.rows(); i++)
  {
    for(int j = 0; j < 3; j++)
    {
      int l = (j + 3) % 3;
      int m = (j + 4) % 3;
      int this_row = nSubTri(i, l);
      int this_col = nSubTri(i, m);
      EdgePair EP = std::make_pair(std::min(this_row, this_col), std::max(this_row, this_col));
      DictBase<int>::EdgeDict::const_iterator got = MyDict.find(EP);
      if(got == MyDict.end())
      {
        // not found yet...
        MyDict.insert({EP, i}); // i.e. the edge, and one of the triangles it belongs to.
        tripletList.push_back(TripletD(this_row, this_col, -1.0));
        tripletList.push_back(TripletD(this_col, this_row, -1.0));
        fDiagCount[this_row] += 1.0;
        fDiagCount[this_col] += 1.0;
      }
    }
  }

  for(int i = 0; i < fDiagCount.size(); i++)
    tripletList.push_back(TripletD(i, i, fDiagCount[i]));

  SparseMatrixD MLap(nUnique.size(), nUnique.size());
  MLap.setFromTriplets(tripletList.begin(), tripletList.end());
  MLap.makeCompressed();
  return std::make_tuple(MLap, nUnique);
}

//============================================================================================

MeshNode HSmoothMain::smooth(const MeshNode& nodes, const std::string& type, double threshold, int iterations)
{
  SparseMatrixD L = laplacian2D(nodes.cols(), type);
  std::vector<int> vidx;
  if(type == "serial")
    vidx = std::vector<int>{(int)0, (int)(L.cols() - 1)};
  else
    vidx = std::vector<int>{};
  MatIndex nFixed = HSmoothBase::getIndex(vidx);
  return smooth(nodes, nFixed, L, threshold, iterations);
}

//============================================================================================

MeshNode HSmoothMain::smooth(const MeshNode& nodes, const MatIndex& nFixed, const SparseMatrixD& GL, double threshold, int iterations)
{
  MatIndex nMobile = HSmoothBase::getComplement(nFixed, GL.cols());
  if(nMobile.size() == 0)
    return nodes;

  SparseMatrixD Data = nodes.transpose().sparseView();

  SparseMatrixD GLRed, fConst, D, A, AyIn, yMobile, fSmallEye, LTL, LTK, yOut;

  std::tuple<SparseMatrixD, SparseMatrixD> dbvp = getDirichletBVP(GL, Data, nFixed, nMobile);
  GLRed = std::get<0>(dbvp);
  fConst = std::get<1>(dbvp);

  std::tuple<SparseMatrixD, SparseMatrixD> pieces = analyzeLaplacian(GL);
  D = std::get<0>(pieces);
  A = std::get<1>(pieces);

  AyIn = A * Data;
  Eigen::MatrixXd mtemp = Eigen::MatrixXd::Zero(nMobile.size(), nMobile.size());
  mtemp.setIdentity();
  fSmallEye = mtemp.sparseView();
  fSmallEye.makeCompressed();
  slice::slice(Data, nMobile, 1, yMobile);
  LTL = SparseMatrixD(GLRed.transpose() * GLRed);
  LTK = SparseMatrixD(GLRed.transpose() * fConst); // casting as SparseMatrixD to make column-major
  yOut = Data;

  Smoother smth;

  double fEps = 0.5;
  double fStep = fEps / 2.0;
  int nCount = 1;

  double fobj1, fobj2, fslope;
  fobj1 = getObjFn(smth, fEps, fSmallEye, LTL, LTK, Data, nMobile, yMobile, D, AyIn, yOut); // only the last parameter is actually modified
  fobj2 = getObjFn(smth, fEps + threshold, fSmallEye, LTL, LTK, Data, nMobile, yMobile, D, AyIn, yOut);
  fslope = (fobj2 - fobj1) / threshold;

  while(fabs(fslope) < threshold && nCount < iterations)
  {
    if(fStep > 0.0)
      fEps -= fStep;
    else
      fEps += fStep;

    fEps /= 2.0;
    fobj1 = getObjFn(smth, fEps, fSmallEye, LTL, LTK, Data, nMobile, yMobile, D, AyIn, yOut); // only the last parameter is actually modified
    fobj2 = getObjFn(smth, fEps + threshold, fSmallEye, LTL, LTK, Data, nMobile, yMobile, D, AyIn, yOut);
    fslope = (fobj2 - fobj1) / threshold;
    nCount++;
  }

  return Eigen::MatrixXd(yOut.transpose());
}

//============================================================================================

std::tuple<SparseMatrixD, SparseMatrixD> HSmoothMain::getDirichletBVP(const SparseMatrixD& GL, const SparseMatrixD& y, const MatIndex& nFixed, const MatIndex& nMobile)
{
  MatIndex nAll = HSmoothBase::matUnion(nFixed, nMobile);
  std::vector<int> v;
  for(int i = 0; i < y.cols(); i++)
    v.push_back(i);
  MatIndex dims = HSmoothBase::getIndex(v);

  //	SparseMatrixD GLRed, sm1, sm2, sm3, fConst;
  SparseMatrixD GLRed(nMobile.size(), nMobile.size()), sm1(nAll.size(), nFixed.size()), sm2(nFixed.size(), dims.size()), sm3(nAll.size(), dims.size()), fConst(nMobile.size(), dims.size());

  slice::slice(GL, nMobile, nMobile, GLRed); // thank goodness for igl::slice!
  slice::slice(GL, nAll, nFixed, sm1);
  slice::slice(y, nFixed, dims, sm2);
  sm3 = sm1 * sm2;
  slice::slice(sm3, nMobile, dims, fConst);

  return std::make_tuple(GLRed, fConst);
}

//============================================================================================

std::tuple<SparseMatrixD, SparseMatrixD> HSmoothMain::analyzeLaplacian(const SparseMatrixD& GL)
{
  // NOTE: GL should be column-major

  SparseMatrixD D(GL.rows(), GL.cols()), A(GL.rows(), GL.cols());
  std::vector<TripletD> DT, AT;

  for(int k = 0; k < GL.outerSize(); ++k)
  {
    for(SparseMatrixD::InnerIterator it(GL, k); it; ++it)
    {
      if(it.value() < -0.5)
        AT.push_back(TripletD(it.row(), it.col(), it.value()));
      else if(it.value() > 0.5)
        DT.push_back(TripletD(it.row(), it.col(), it.value()));
    }
  }
  D.setFromTriplets(DT.begin(), DT.end());
  A.setFromTriplets(AT.begin(), AT.end());

  D.makeCompressed();
  A.makeCompressed();

  return std::make_tuple(D, A);
}

//============================================================================================

double HSmoothMain::getObjFn(Smoother& smth, double feps, const SparseMatrixD& fSmallEye, const SparseMatrixD& LTL, const SparseMatrixD& LTK, const SparseMatrixD& Data, const MatIndex& nMobile,
                             const SparseMatrixD& yMobile, const SparseMatrixD& D, const SparseMatrixD& AyIn, SparseMatrixD& yOut)
{
  SparseMatrixD ySmooth;

  SparseMatrixD A = (1.0 - feps) * fSmallEye + feps * LTL;
  SparseMatrixD b = (1.0 - feps) * yMobile - feps * LTK;

  smth.compute(A); // smth in general changes with each function call
  ySmooth = smth.solve(b);

  HSmoothBase::merge(ySmooth, yOut, nMobile);

  Eigen::ArrayXXd yDeltaD = Eigen::MatrixXd(D * yOut + AyIn).array();

  return (yDeltaD * yDeltaD).sum(); // more efficient to calculate trace this way
}
