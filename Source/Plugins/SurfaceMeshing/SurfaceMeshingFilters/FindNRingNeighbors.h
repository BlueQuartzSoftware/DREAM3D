/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson,
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _FindNRingNeighbors_H_
#define _FindNRingNeighbors_H_

#include <set>
#include <QtCore/QString>


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataContainers/DataContainer.h"


#include "SurfaceMeshing/SurfaceMeshingConstants.h"

/**
 * @class FindNRingNeighbors FindNRingNeighbors.h SurfaceMeshFilters/FindNRingNeighbors.h
 * @brief Calculates the set of triangles that are "N" rings (based on vertex) from a seed triangle
 * @author Mike Jackson (BlueQuartz Software)
 * @date,
 * @version 1.0
 */
class FindNRingNeighbors
{
  public:
    DREAM3D_SHARED_POINTERS(FindNRingNeighbors)
    DREAM3D_STATIC_NEW_MACRO(FindNRingNeighbors)
    DREAM3D_TYPE_MACRO(FindNRingNeighbors)

    virtual ~FindNRingNeighbors();

    typedef std::set<int64_t> UniqueFaceIds_t;


    DREAM3D_INSTANCE_PROPERTY(int, TriangleId)

    /**
     * @brief This sets the region id (Feature Id) that we are interested in.
     */
    DREAM3D_INSTANCE_PROPERTY(int, RegionId0)

    /**
     * @brief This sets the region id (Feature Id) that we are interested in.
     */
    DREAM3D_INSTANCE_PROPERTY(int, RegionId1)

    void setRegionIds(int g, int r);

    /**
     * @brief This is the number of rings to find
     **/
    DREAM3D_INSTANCE_PROPERTY(int, Ring)


    UniqueFaceIds_t& getNRingTriangles();

    /**
     * @brief generate
     * @param node2Triangle - Map of QSets. The "key" for the map is the Node Id and the
     * "value" for each key is a QSet that is the set of triangle ids that that particular
     * node is a part of.
     */
    void generate(TriangleGeom::Pointer triangleGeom, int32_t* faceLabels);


    DREAM3D_INSTANCE_PROPERTY(bool, WriteBinaryFile)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteConformalMesh)

  protected:
    FindNRingNeighbors();


  private:
    UniqueFaceIds_t  m_NRingTriangles;

    FindNRingNeighbors(const FindNRingNeighbors&); // Copy Constructor Not Implemented
    void operator=(const FindNRingNeighbors&); // Operator '=' Not Implemented
};

#endif /* _FindNRingNeighbors_H_ */

