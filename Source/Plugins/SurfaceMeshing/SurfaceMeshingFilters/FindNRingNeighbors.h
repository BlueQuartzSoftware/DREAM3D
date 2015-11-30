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


#ifndef _FindNRingNeighbors_H_
#define _FindNRingNeighbors_H_

#include <set>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Geometry/TriangleGeom.h"

/**
 * @brief The FindNRingNeighbors class calculates the set of triangles that are "N" rings (based on vertex) from a seed triangle
 */
class FindNRingNeighbors
{
  public:
    SIMPL_SHARED_POINTERS(FindNRingNeighbors)
    SIMPL_STATIC_NEW_MACRO(FindNRingNeighbors)
    SIMPL_TYPE_MACRO(FindNRingNeighbors)

    virtual ~FindNRingNeighbors();

    typedef std::set<int64_t> UniqueFaceIds_t;

    SIMPL_INSTANCE_PROPERTY(int64_t, TriangleId)

    /**
     * @brief This sets the region id (Feature Id) that we are interested in.
     */
    SIMPL_INSTANCE_PROPERTY(int32_t, RegionId0)

    /**
     * @brief This sets the region id (Feature Id) that we are interested in.
     */
    SIMPL_INSTANCE_PROPERTY(int32_t, RegionId1)

    /**
     * @brief setRegionIds Sets the local variables for region Ids
     * @param g Id for region 0
     * @param r Id for region 1
     */
    void setRegionIds(int32_t g, int32_t r);

    /**
     * @brief This is the number of rings to find
     **/
    SIMPL_INSTANCE_PROPERTY(int64_t, Ring)

    /**
     * @brief getNRingTriangles Returns the N ring set
     * @return Set of N ring Ids
     */
    UniqueFaceIds_t& getNRingTriangles();

    /**
     * @brief generate Generates the N rings based on the supplied TriangleGeom
     * @param triangleGeom Incoming TriangleGeom object
     * @param faceLabels Feature Id labels for the TriangleGeom
     * @return Integer error value
     */
    int32_t generate(TriangleGeom::Pointer triangleGeom, int32_t* faceLabels);

    SIMPL_INSTANCE_PROPERTY(bool, WriteBinaryFile)
    SIMPL_INSTANCE_PROPERTY(bool, WriteConformalMesh)

  protected:
    FindNRingNeighbors();

  private:
    UniqueFaceIds_t  m_NRingTriangles;

    FindNRingNeighbors(const FindNRingNeighbors&); // Copy Constructor Not Implemented
    void operator=(const FindNRingNeighbors&); // Operator '=' Not Implemented
};

#endif /* _FindNRingNeighbors_H_ */

