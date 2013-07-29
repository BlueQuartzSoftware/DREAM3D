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
 *Find
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

#ifndef GenerateEnsembleStatistics_H_
#define GenerateEnsembleStatistics_H_

#include <vector>
#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/IDataArray.h"
#include "DREAM3DLib/Common/StatsDataArray.h"
#include "DREAM3DLib/Common/StatsData.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"
#include "DREAM3DLib/Common/VoxelDataContainer.h"
#include "DREAM3DLib/Common/NeighborList.hpp"
#include "DREAM3DLib/DistributionAnalysisOps/DistributionAnalysisOps.h"

/**
 * @class GenerateEnsembleStatistics GenerateEnsembleStatistics.h DREAM3DLib/GenericFilters/GenerateEnsembleStatistics.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT GenerateEnsembleStatistics : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(GenerateEnsembleStatistics)
    DREAM3D_STATIC_NEW_MACRO(GenerateEnsembleStatistics)
    DREAM3D_TYPE_MACRO_SUPER(GenerateEnsembleStatistics, AbstractFilter)

    virtual ~GenerateEnsembleStatistics();

    //------ (Possible) Required Field Data
    DREAM3D_INSTANCE_STRING_PROPERTY(AvgQuatsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(BiasedFieldsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(VolumesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(FieldEulerAnglesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(FieldPhasesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(SurfaceFieldsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(EquivalentDiametersArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(AspectRatiosArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(Omega3sArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(NeighborhoodsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(AxisEulerAnglesArrayName)
    //------ (Possible) Required Ensemble Data
    DREAM3D_INSTANCE_STRING_PROPERTY(CrystalStructuresArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(TotalSurfaceAreasArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(PhaseTypesArrayName)

    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::StatisticsFilters; }
    virtual const std::string getSubGroupName() { return DREAM3D::FilterSubGroups::EnsembleStatsFilters; }
    virtual const std::string getHumanLabel() { return "Generate Ensemble Statistics"; }

    DREAM3D_INSTANCE_PROPERTY(int, SizeCorrelationResolution)
    DREAM3D_INSTANCE_PROPERTY(bool, SizeDistribution)
    DREAM3D_INSTANCE_PROPERTY(int, SizeDistributionFitType)
    DREAM3D_INSTANCE_PROPERTY(bool, AspectRatioDistribution)
    DREAM3D_INSTANCE_PROPERTY(int, AspectRatioDistributionFitType)
    DREAM3D_INSTANCE_PROPERTY(bool, Omega3Distribution)
    DREAM3D_INSTANCE_PROPERTY(int, Omega3DistributionFitType)
    DREAM3D_INSTANCE_PROPERTY(bool, NeighborhoodDistribution)
    DREAM3D_INSTANCE_PROPERTY(int, NeighborhoodDistributionFitType)
    DREAM3D_INSTANCE_PROPERTY(bool, CalculateODF)
    DREAM3D_INSTANCE_PROPERTY(bool, CalculateMDF)
    DREAM3D_INSTANCE_PROPERTY(bool, CalculateAxisODF)
    DREAM3D_INSTANCE_PROPERTY(std::vector<unsigned int>, PhaseTypeArray)

    /**
    * @brief This method will write the options to a file
    * @param writer The writer that is used to write the options to a file
    */
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
    GenerateEnsembleStatistics();

    void gatherSizeStats();
    void gatherAspectRatioStats();
    void gatherOmega3Stats();
    void gatherNeighborhoodStats();
    void gatherMDFStats();
    void gatherODFStats();
    void gatherAxisODFStats();

  private:
    std::vector<OrientationOps::Pointer> m_OrientationOps;
    CubicOps::Pointer m_CubicOps;
    HexagonalOps::Pointer m_HexOps;
    OrthoRhombicOps::Pointer m_OrthoOps;

    float* m_AvgQuats;
    float* m_FieldEulerAngles;
    float* m_Volumes;
    bool* m_BiasedFields;
    bool* m_SurfaceFields;
    int32_t* m_FieldPhases;
    float* m_AxisEulerAngles;
    float* m_Omega3s;
    float* m_AspectRatios;
    float* m_EquivalentDiameters;
    int32_t* m_Neighborhoods;
    unsigned int* m_CrystalStructures;
    unsigned int* m_PhaseTypes;

    NeighborList<int>* m_NeighborList;
    NeighborList<float>* m_SharedSurfaceAreaList;

    StatsDataArray* m_StatsDataArray;

    std::vector<DistributionAnalysisOps::Pointer> m_DistributionAnalysis;

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

    GenerateEnsembleStatistics(const GenerateEnsembleStatistics&); // Copy Constructor Not Implemented
    void operator=(const GenerateEnsembleStatistics&); // Operator '=' Not Implemented
};

#endif /* GenerateEnsembleStatistics_H_ */
