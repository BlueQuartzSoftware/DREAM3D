/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#pragma once

#include <vector>
#include <set>

#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Geometry/TriangleGeom.h"

#include "SurfaceMeshing/SurfaceMeshingFilters/util/Vector3.h"


class SurfaceDataContainer;

/**
 * @brief The TriangleOps class
 */
class TriangleOps
{
  public:
    virtual ~TriangleOps();

    static int getLabelIndex(int32_t* triLabels, int label);

    QVector<MeshIndexType> getNodeIndices(MeshIndexType t[], int32_t* faceLabel, int32_t label);

    static void flipWinding(MeshIndexType triangle[3]);

    static VectorType computeNormal(float n0[3], float n1[3], float n2[3]);

    static QSet<int32_t> generateUniqueLabels(DataArray<int32_t>::Pointer faceLabelsPtr);

    static QVector<MeshIndexType> findAdjacentTriangles(TriangleGeom::Pointer triangles, MeshIndexType triangleIndex, DataArray<int32_t>::Pointer faceLabelsPtr, int32_t label);

    static bool verifyWinding(MeshIndexType source[], MeshIndexType tri[], int32_t* faceLabelSource, int32_t* faceLabelTri, int32_t label);

    static void getWindingIndices4(MeshIndexType triangle[], int32_t* faceLabel, int ids[4], int32_t label);

  protected:
    TriangleOps();

  public:
    TriangleOps(const TriangleOps&) = delete;    // Copy Constructor Not Implemented
    TriangleOps(TriangleOps&&) = delete;         // Move Constructor Not Implemented
    TriangleOps& operator=(const TriangleOps&) = delete; // Copy Assignment Not Implemented
    TriangleOps& operator=(TriangleOps&&) = delete;      // Move Assignment Not Implemented
};


