/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _TriangleOps_H_
#define _TriangleOps_H_

#include <vector>
#include <QtCore/QSet>

#include "DREAM3DLib/Common/DataArray.hpp"
#include "DREAM3DLib/Common/StructArray.hpp"
#include "DREAM3DLib/Common/SurfaceMeshStructs.h"
#include "DREAM3DLib/SurfaceMeshingFilters/util/Vector3.h"


class SurfaceMeshDataContainer;

/**
 * @brief The TriangleOps class
 */
class TriangleOps
{
  public:
    virtual ~TriangleOps();

    static int getLabelIndex(int32_t* t, int label);

    std::vector<int> getNodeIndices(DREAM3D::SurfaceMesh::Face_t &t, int32_t* faceLabel, int label);

    static void flipWinding(DREAM3D::SurfaceMesh::Face_t& triangle);

    static VectorType computeNormal(DREAM3D::SurfaceMesh::Vert_t& n0, DREAM3D::SurfaceMesh::Vert_t& n1, DREAM3D::SurfaceMesh::Vert_t& n2);

    static QSet<int32_t> generateUniqueLabels(DataArray<int32_t>* faceLabelsPtr);

    static std::vector<int32_t> findAdjacentTriangles(SurfaceMeshDataContainer* sm,
                                                        int32_t triangleIndex,
                                                        int32_t label);

    static bool verifyWinding(DREAM3D::SurfaceMesh::Face_t& source, DREAM3D::SurfaceMesh::Face_t& tri,
                                int32_t* faceLabelSource, int32_t* faceLabelTri, int32_t label);


    static void getWindingIndices4(DREAM3D::SurfaceMesh::Face_t &triangle, int32_t* faceLabel, int ids[4], int32_t label);

  protected:
    TriangleOps();

  private:
    TriangleOps(const TriangleOps&); // Copy Constructor Not Implemented
    void operator=(const TriangleOps&); // Operator '=' Not Implemented

};

#endif /* _TriangleOps_H_ */
