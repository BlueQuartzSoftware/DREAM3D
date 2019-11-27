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

#include <memory>

#include <set>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Geometry/TriangleGeom.h"

/**
 * @brief The FindNRingNeighbors class calculates the set of triangles that are "N" rings (based on vertex) from a seed triangle
 */
class FindNRingNeighbors
{
  public:
    using Self = FindNRingNeighbors;
    using Pointer = std::shared_ptr<Self>;
    using ConstPointer = std::shared_ptr<const Self>;
    using WeakPointer = std::weak_ptr<Self>;
    using ConstWeakPointer = std::weak_ptr<const Self>;
    static Pointer NullPointer();

    static Pointer New();

    /**
     * @brief Returns the name of the class for FindNRingNeighbors
     */
    virtual QString getNameOfClass() const;
    /**
     * @brief Returns the name of the class for FindNRingNeighbors
     */
    static QString ClassName();

    virtual ~FindNRingNeighbors();

    using UniqueFaceIds_t = std::set<int64_t>;

    /**
     * @brief Setter property for TriangleId
     */
    void setTriangleId(int64_t value);
    /**
     * @brief Getter property for TriangleId
     * @return Value of TriangleId
     */
    int64_t getTriangleId() const;

    /**
     * @brief This sets the region id (Feature Id) that we are interested in.
     */
    /**
     * @brief Setter property for RegionId0
     */
    void setRegionId0(int32_t value);
    /**
     * @brief Getter property for RegionId0
     * @return Value of RegionId0
     */
    int32_t getRegionId0() const;

    /**
     * @brief This sets the region id (Feature Id) that we are interested in.
     */
    /**
     * @brief Setter property for RegionId1
     */
    void setRegionId1(int32_t value);
    /**
     * @brief Getter property for RegionId1
     * @return Value of RegionId1
     */
    int32_t getRegionId1() const;

    /**
     * @brief setRegionIds Sets the local variables for region Ids
     * @param g Id for region 0
     * @param r Id for region 1
     */
    void setRegionIds(int32_t g, int32_t r);

    /**
     * @brief This is the number of rings to find
     **/
    /**
     * @brief Setter property for Ring
     */
    void setRing(int64_t value);
    /**
     * @brief Getter property for Ring
     * @return Value of Ring
     */
    int64_t getRing() const;

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

    /**
     * @brief Setter property for WriteBinaryFile
     */
    void setWriteBinaryFile(bool value);
    /**
     * @brief Getter property for WriteBinaryFile
     * @return Value of WriteBinaryFile
     */
    bool getWriteBinaryFile() const;

    /**
     * @brief Setter property for WriteConformalMesh
     */
    void setWriteConformalMesh(bool value);
    /**
     * @brief Getter property for WriteConformalMesh
     * @return Value of WriteConformalMesh
     */
    bool getWriteConformalMesh() const;

  protected:
    FindNRingNeighbors();

  private:
    int64_t m_TriangleId = {};
    int32_t m_RegionId0 = {};
    int32_t m_RegionId1 = {};
    int64_t m_Ring = {};
    bool m_WriteBinaryFile = {};
    bool m_WriteConformalMesh = {};

    UniqueFaceIds_t  m_NRingTriangles;

  public:
    FindNRingNeighbors(const FindNRingNeighbors&) = delete; // Copy Constructor Not Implemented
    FindNRingNeighbors(FindNRingNeighbors&&) = delete;      // Move Constructor Not Implemented
    FindNRingNeighbors& operator=(const FindNRingNeighbors&) = delete; // Copy Assignment Not Implemented
    FindNRingNeighbors& operator=(FindNRingNeighbors&&) = delete;      // Move Assignment Not Implemented
};


