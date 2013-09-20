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

#ifndef _CalculateTriangleGroupCurvatures_H_
#define _CalculateTriangleGroupCurvatures_H_

#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/Common/AbstractFilter.h"

class SurfaceDataContainer;


/**
 * @class CalculateTriangleGroupCurvatures CalculateTriangleGroupCurvatures.h /SurfaceMeshFilters/CalculateTriangleGroupCurvatures.h
 * @brief Calculates the curvature values for a group of triangles where each triangle in the group will have the 2 Principal Curvature
 * values computed and optionally the 2 Principal Directions and optionally the Mean and Gaussian Curvature computed.
 * @author Mike Jackson (BlueQuartz Software)s
 * @date Dec 2012.
 * @version 1.0
 */
class DREAM3DLib_EXPORT CalculateTriangleGroupCurvatures
{
  public:
    CalculateTriangleGroupCurvatures(int nring,
                                     std::vector<int> triangleIds, bool useNormalsForCurveFitting,
                                     DoubleArrayType::Pointer principleCurvature1,
                                     DoubleArrayType::Pointer principleCurvature2,
                                     DoubleArrayType::Pointer principleDirection1,
                                     DoubleArrayType::Pointer principleDirection2,
                                     DoubleArrayType::Pointer gaussianCurvature,
                                     DoubleArrayType::Pointer meanCurvature,
                                     SurfaceDataContainer* sm,
                                     AbstractFilter* parent);

    virtual ~CalculateTriangleGroupCurvatures();

    void operator()() const;

  protected:
    CalculateTriangleGroupCurvatures();

    /**
     * @brief This method extracts out the needed data values from the global arrays.
     * @param triId The seed triangle id
     * @param triPatch The group of triangles being used
     * @param data The data to extract from
     * @param name The name of the data array being used.
     * @return
     */
    DataArray<double>::Pointer extractPatchData(int triId, DREAM3D::Mesh::UniqueFaceIds_t &triPatch,
                                                double* data,
                                                const QString &name) const;
  private:
    int m_NRing;
    std::vector<int> m_TriangleIds;
    bool m_UseNormalsForCurveFitting;
    DoubleArrayType::Pointer m_PrincipleCurvature1;
    DoubleArrayType::Pointer m_PrincipleCurvature2;
    DoubleArrayType::Pointer m_PrincipleDirection1;
    DoubleArrayType::Pointer m_PrincipleDirection2;
    DoubleArrayType::Pointer m_GaussianCurvature;
    DoubleArrayType::Pointer m_MeanCurvature;
    SurfaceDataContainer* m_SurfaceDataContainer;
    AbstractFilter* m_ParentFilter;
};

#endif /* _CalculateTriangleGroupCurvatures_H_ */


