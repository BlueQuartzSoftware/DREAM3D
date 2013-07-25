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

#ifndef MATCHCRYSTALLOGRAPHY_H_
#define MATCHCRYSTALLOGRAPHY_H_

#include <string>
#include <numeric>
#include <limits>

#include <boost/shared_array.hpp>

#include "EbsdLib/EbsdConstants.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/IDataArray.h"
#include "DREAM3DLib/Common/StatsDataArray.h"
#include "DREAM3DLib/Common/StatsData.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/VoxelDataContainer.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Common/NeighborList.hpp"

/**
 * @class MatchCrystallography MatchCrystallography.h DREAM3DLib/SyntheticBuilderFilters/MatchCrystallography.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT MatchCrystallography : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(MatchCrystallography)
    DREAM3D_STATIC_NEW_MACRO(MatchCrystallography)
    DREAM3D_TYPE_MACRO_SUPER(MatchCrystallography, AbstractFilter)


    virtual ~MatchCrystallography();

    //------ Required Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(GrainIdsArrayName)
    //------ Created Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(CellEulerAnglesArrayName)
    //------ Required Field Data
    DREAM3D_INSTANCE_STRING_PROPERTY(FieldPhasesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(SurfaceFieldsArrayName)
    //------ Created Field Data
    DREAM3D_INSTANCE_STRING_PROPERTY(AvgQuatsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(FieldEulerAnglesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(VolumesArrayName)
    //------ Required Ensemble Data
    DREAM3D_INSTANCE_STRING_PROPERTY(CrystalStructuresArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(PhaseTypesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(NumFieldsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(NeighborListArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(SharedSurfaceAreaListArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(StatsDataArrayName)

    typedef boost::shared_array<float> SharedFloatArray;
    typedef boost::shared_array<int> SharedIntArray;

    DREAM3D_INSTANCE_PROPERTY(int, MaxIterations)

    virtual const std::string getGroupName() {return DREAM3D::FilterGroups::SyntheticBuildingFilters;}
    virtual const std::string getSubGroupName() { return DREAM3D::FilterSubGroups::CrystallographyFilters; }
    virtual const std::string getHumanLabel() {return "Match Crystallography";}

    virtual void setupFilterParameters();
    /**
    * @brief This method will write the options to a file
    * @param writer The writer that is used to write the options to a file
    */
    virtual void writeFilterParameters(AbstractFilterParametersWriter* writer);

    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader);


    /**
     * @brief Reimplemented from @see AbstractFilter class
     */
    virtual void execute();
    virtual void preflight();

  protected:
    MatchCrystallography();

    void initializeArrays(int ensem);

    void determine_volumes();
    void determine_boundary_areas();
    void assign_eulers(int ensem);
    void MC_LoopBody1(int grain, int ensem, int j, float neighsurfarea, unsigned int sym, QuatF &q1, QuatF &q2);
    void MC_LoopBody2(int grain, int phase, int j, float neighsurfarea, unsigned int sym, QuatF &q1, QuatF &q2);
    void matchCrystallography(int ensem);
    void measure_misorientations(int ensem);

  private:

    // Cell Data
    int32_t* m_GrainIds;
    float*   m_CellEulerAngles;

    // Field Data
    bool* m_SurfaceFields;
    int32_t* m_FieldPhases;
    float* m_Volumes;
    float* m_FieldEulerAngles;
    float* m_AvgQuats;
    NeighborList<int>* m_NeighborList;
    NeighborList<float>* m_SharedSurfaceAreaList;

    //Ensemble Data
    unsigned int* m_CrystalStructures;
    unsigned int* m_PhaseTypes;
    int32_t* m_NumFields;
    StatsDataArray* m_StatsDataArray;

    // All other private instance variables
    float mdfchange;
    float odfchange;

    std::vector<float> unbiasedvol;
    std::vector<float> totalSurfaceArea;


    FloatArrayType::Pointer actualodf;
    FloatArrayType::Pointer simodf;
    FloatArrayType::Pointer actualmdf;
    FloatArrayType::Pointer simmdf;


    std::vector<std::vector<float> > misorientationlists;
    std::vector<OrientationOps::Pointer> m_OrientationOps;

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

    MatchCrystallography(const MatchCrystallography&); // Copy Constructor Not Implemented
    void operator=(const MatchCrystallography&); // Operator '=' Not Implemented
};

#endif /* MATCHCRYSTALLOGRAPHY_H_ */
