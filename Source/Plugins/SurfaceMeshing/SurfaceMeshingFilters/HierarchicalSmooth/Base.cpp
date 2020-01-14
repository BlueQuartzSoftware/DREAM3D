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
 * Base.cpp -- implementation of routines in Base.h
 *
 */

#include "Base.h"

// int is the same as unsigned long

//============================================================================================

TriMesh HSmoothBase::isMember(const TriMesh& array1, const std::vector<int>& array2)
{
  std::unordered_map<int, int> MyDict; // dictionary lookup for faster access
  for(int i = 0; i < array2.size(); i++)
    MyDict.insert({array2[i], i});
  TriMesh NewTri = TriMesh::Zero(array1.rows(), array1.cols());
  // running through Array1 in column-major order, Eigen's default
  for(int col = 0; col < array1.cols(); col++)
  {
    for(int row = 0; row < array1.rows(); row++)
    {
      std::unordered_map<int, int>::const_iterator got = MyDict.find(array1(row, col));
      NewTri(row, col) = got->second;
    }
  }
  return NewTri;
}

//============================================================================================

MatIndex HSmoothBase::getIndex(const std::vector<int>& inputVec)
{
  MatIndex I(inputVec.size());
  for(int i = 0; i < inputVec.size(); i++)
    I(i) = inputVec[i];

  return I;
}

//============================================================================================

MatIndex HSmoothBase::getIndex(const std::vector<int>& inputVec, const MatIndex& matIndex)
{
  std::unordered_map<int, int> dict;
  for(int i = 0; i < matIndex.rows(); i++)
    dict.insert({matIndex(i), i});
  std::vector<int> vtemp;
  for(int i = 0; i < inputVec.size(); i++)
  {
    std::unordered_map<int, int>::iterator got = dict.find(inputVec[i]);
    vtemp.push_back(got->second);
  }
  return getIndex(vtemp);
}

//============================================================================================

MatIndex HSmoothBase::getComplement(const MatIndex& nSet, int N)
{
  MatIndex nAll = -1 * MatIndex::Ones(N, 1);
  for(int i = 0; i < nSet.size(); i++)
    nAll(nSet(i)) = nSet(i);
  std::vector<int> nComplement;
  for(int i = 0; i < nAll.size(); i++)
    if(nAll(i) < 0)
      nComplement.push_back(i);

  return getIndex(nComplement);
}

//============================================================================================

MatIndex HSmoothBase::matUnion(const MatIndex& mat1, const MatIndex& mat2)
{
  std::vector<int> v;
  for(int i = 0; i < mat1.size(); i++)
    v.push_back(mat1(i));
  for(int i = 0; i < mat2.size(); i++)
    v.push_back(mat2(i));
  std::sort(v.begin(), v.end());
  v.erase(std::unique(v.begin(), v.end()), v.end());

  return getIndex(v);
}

//============================================================================================

void HSmoothBase::merge(const MeshNode& source, MeshNode& target, const MatIndex& locations)
{
  for(int i = 0; i < source.cols(); i++)
    target.col(locations(i)) << source.col(i);
  return;
}

//============================================================================================

void HSmoothBase::merge(const SparseMatrixD& source, SparseMatrixD& target, const MatIndex& locations)
{
  MeshNode src = Eigen::MatrixXd(source).transpose();
  MeshNode trg = Eigen::MatrixXd(target).transpose();
  merge(src, trg, locations);

  target = trg.transpose().sparseView();
  target.makeCompressed();
  return;
}
