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
 * HierarchicalSmooth.h -- Contains main and auxiliary routine prototypes
 * for smoothing according to the hierarchical filter method.
 *
 */

#pragma once

#include "Types.h"

namespace HSmoothMain
{

SparseMatrixD laplacian2D(int N, const std::string& type = "serial");           // the options are 'serial' and 'cyclic'.
std::tuple<SparseMatrixD, std::vector<int>> graphLaplacian(const TriMesh& tri); // multiple returns

MeshNode smooth(const MeshNode& nodes, const std::string& types = "serial", double threshold = 0.001, int iterations = 53);
MeshNode smooth(const MeshNode& nodes, const MatIndex& nFixed, const SparseMatrixD& GL, double threshold = 0.001, int iterations = 53);

std::tuple<SparseMatrixD, SparseMatrixD> getDirichletBVP(const SparseMatrixD& GL, const SparseMatrixD& y, const MatIndex& nFixed, const MatIndex& nMobile);
std::tuple<SparseMatrixD, SparseMatrixD> analyzeLaplacian(const SparseMatrixD& GL);

double getObjFn(Smoother& smth, double feps, const SparseMatrixD& fSmallEye, const SparseMatrixD& LTL, const SparseMatrixD& LTK, const SparseMatrixD& Data, const MatIndex& nMobile,
                const SparseMatrixD& yMobile, const SparseMatrixD& D, const SparseMatrixD& AyIn, SparseMatrixD& yOut);

} // namespace HSmoothMain
