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
 * VolumeSolver.h -- contains the definition of the object that takes the
 * surface mesh data of the entire volume and returns an array containing
 * hierarchical-smoothed mesh nodes. This object does all the necessary
 * bookkeeping with node types, connectivity and smoothed status.
 */

#pragma once

#include <fstream>

#include "Types.h"

namespace VolumeSolver
{

class VolumeSolver
{

public:
  // constructor
  VolumeSolver(const TriMesh& volumeMesh, const MeshNode& surfaceNodes, const FaceLabel& faceLabels, const NodeType& nodeType, int iterations = 53);
  // the last integer default is the number of bisections in each call
  // to the core smoothing routine. Obtained from a typical machine
  // zero value of ~10^16.
  // TODO: Generalize this to work for machine zero of the specific
  // machine in use.

  // smoother
  MeshNode hierarchicalSmooth(bool logging = false, const std::string& logfile = "Smooth.Default.log");

  // writers
  MeshNode getSmoothed() const;
  IsSmoothed getNodeSmoothStatus() const;

private:
  // member objects; all these are instantiated in the constructor
  IsSmoothed Status;
  TriMesh vsMesh;
  MeshNode vsNode;
  MeshNode vsNodeSmooth;
  FaceLabel vsLabel;
  NodeType vsType;
  int MaxIterations;
  double fError;
  double fErrorThreshold;
  std::ofstream fout; // log file handle
  DictBase<std::vector<int>>::EdgeDict vsBoundaryDict;

  // member functions
  TriMesh sliceMesh(const std::vector<int>& patches) const;
  void markSectionAsComplete(const MatIndex& idx);

  // scratch
  MatIndex one;
  MatIndex three;
};

} // namespace VolumeSolver
