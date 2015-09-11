/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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


#ifndef _CalculateTriangleGroupCurvatures_H_
#define _CalculateTriangleGroupCurvatures_H_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Common/AbstractFilter.h"

/**
 * @brief The CalculateTriangleGroupCurvatures class calculates the curvature values for a group of triangles
 * where each triangle in the group will have the 2 Principal Curvature values computed and optionally
 * the 2 Principal Directions and optionally the Mean and Gaussian Curvature computed.
 */
class CalculateTriangleGroupCurvatures
{
  public:
    CalculateTriangleGroupCurvatures(int64_t nring,
                                     std::vector<int64_t> triangleIds, bool useNormalsForCurveFitting,
                                     DoubleArrayType::Pointer principleCurvature1,
                                     DoubleArrayType::Pointer principleCurvature2,
                                     DoubleArrayType::Pointer principleDirection1,
                                     DoubleArrayType::Pointer principleDirection2,
                                     DoubleArrayType::Pointer gaussianCurvature,
                                     DoubleArrayType::Pointer meanCurvature,
                                     TriangleGeom::Pointer trianglesGeom,
                                     DataArray<int32_t>::Pointer surfaceMeshFaceLabels,
                                     DataArray<double>::Pointer surfaceMeshFaceNormals,
                                     DataArray<double>::Pointer surfaceMeshTriangleCentroids,
                                     AbstractFilter* parent);

    virtual ~CalculateTriangleGroupCurvatures();

    void operator()() const;

    typedef std::set<int64_t> UniqueFaceIds_t;

  protected:
    CalculateTriangleGroupCurvatures();

    /**
     * @brief extractPatchData Extracts out the needed data values from the global arrays
     * @param triId The seed triangle Id
     * @param triPatch The group of triangles being used
     * @param data The data to extract from
     * @param name The name of the data array being used
     * @return Shared pointer to the extracted data
     */
    DataArray<double>::Pointer extractPatchData(int64_t triId, UniqueFaceIds_t& triPatch,
                                                double* data,
                                                const QString& name) const;

  private:
    int64_t m_NRing;
    std::vector<int64_t> m_TriangleIds;
    bool m_UseNormalsForCurveFitting;
    DoubleArrayType::Pointer m_PrincipleCurvature1;
    DoubleArrayType::Pointer m_PrincipleCurvature2;
    DoubleArrayType::Pointer m_PrincipleDirection1;
    DoubleArrayType::Pointer m_PrincipleDirection2;
    DoubleArrayType::Pointer m_GaussianCurvature;
    DoubleArrayType::Pointer m_MeanCurvature;
    TriangleGeom::Pointer m_TrianglesPtr;
    DataArray<int32_t>::Pointer m_SurfaceMeshFaceLabels;
    DataArray<double>::Pointer m_SurfaceMeshFaceNormals;
    DataArray<double>::Pointer m_SurfaceMeshTriangleCentroids;
    AbstractFilter* m_ParentFilter;
};

#endif /* _CalculateTriangleGroupCurvatures_H_ */
