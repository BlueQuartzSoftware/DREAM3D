/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#ifndef GroupMicroTextureRegions_H_
#define GroupMicroTextureRegions_H_

#include <vector>
#include <string>

///Boost Random Number generator stuff
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

typedef boost::uniform_int<int> NumberDistribution;
typedef boost::mt19937 RandomNumberGenerator;
typedef boost::variate_generator<RandomNumberGenerator&, NumberDistribution> Generator;

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataContainers/VoxelDataContainer.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/DataArrays/NeighborList.hpp"

/**
 * @class GroupMicroTextureRegions GroupMicroTextureRegions.h DREAM3DLib/ReconstructionFilters/GroupMicroTextureRegions.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT GroupMicroTextureRegions : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(GroupMicroTextureRegions)
    DREAM3D_STATIC_NEW_MACRO(GroupMicroTextureRegions)
    DREAM3D_TYPE_MACRO_SUPER(GroupMicroTextureRegions, AbstractFilter)


    virtual ~GroupMicroTextureRegions();

    //------ Required Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(GrainIdsArrayName)
    //------ Created Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(CellParentIdsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(ParentDensityArrayName)
    //------ Required Field Data
    DREAM3D_INSTANCE_STRING_PROPERTY(AvgQuatsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(FieldPhasesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(VolumesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(ContiguousNeighborListArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(NonContiguousNeighborListArrayName)
    //------ Created Field Data
    DREAM3D_INSTANCE_STRING_PROPERTY(ActiveArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(FieldParentIdsArrayName)

    //------ Required Ensemble Data
    DREAM3D_INSTANCE_STRING_PROPERTY(CrystalStructuresArrayName)

    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::ReconstructionFilters; }
    virtual const std::string getSubGroupName() {return DREAM3D::FilterSubGroups::GroupingFilters;}
    virtual const std::string getHumanLabel() { return "Identify MicroTexture (C-Axis Misorientation)"; }

    DREAM3D_INSTANCE_PROPERTY(float, CAxisTolerance)
    DREAM3D_INSTANCE_PROPERTY(bool, UseNonContiguousNeighbors)
    DREAM3D_INSTANCE_PROPERTY(bool, RandomizeGrainIds)

    virtual void setupFilterParameters();
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    /**
     * @brief Reimplemented from @see AbstractFilter class
     */
    virtual void execute();
    virtual void preflight();

  protected:
    GroupMicroTextureRegions();

    int merge_micro_texture_regions();
    void characterize_micro_texture_regions();

    /**
     * @brief randomizeGrainIds
     * @param totalPoints
     * @param totalFields
     */
    void randomizeGrainIds(int64_t totalPoints, size_t totalFields);

    /**
     * @brief initializeVoxelSeedGenerator
     * @param totalPoints
     */
    void initializeVoxelSeedGenerator(const size_t rangeMin, const size_t rangeMax);

  private:
    int32_t* m_GrainIds;
    int32_t* m_CellParentIds;
    int32_t* m_FieldParentIds;
    float* m_ParentDensity;
    float* m_AvgQuats;
    bool* m_Active;
    int32_t* m_FieldPhases;
    float* m_Volumes;
    NeighborList<int>* m_ContiguousNeighborList;
    NeighborList<int>* m_NonContiguousNeighborList;

    unsigned int* m_CrystalStructures;

    std::vector<int> parentnumbers;
    std::vector<bool> beenChecked;
    std::vector<float> densities;

    std::vector<OrientationOps::Pointer> m_OrientationOps;

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

    ///Boost Random Number generator stuff. We use the boost::shared_ptr to ensure the pointers are cleaned up when the
    ///filter is deleted
    boost::shared_ptr<NumberDistribution> m_Distribution;
    boost::shared_ptr<RandomNumberGenerator> m_RandomNumberGenerator;
    boost::shared_ptr<Generator> m_NumberGenerator;
    size_t                       m_TotalRandomNumbersGenerated;

    GroupMicroTextureRegions(const GroupMicroTextureRegions&); // Copy Constructor Not Implemented
    void operator=(const GroupMicroTextureRegions&); // Operator '=' Not Implemented
};

#endif /* GroupMicroTextureRegions_H_ */
