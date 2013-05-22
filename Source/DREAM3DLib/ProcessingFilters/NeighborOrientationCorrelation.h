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

#ifndef NeighborOrientationCorrelation_H_
#define NeighborOrientationCorrelation_H_

#include <vector>
#include <string>


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/IDataArray.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/VoxelDataContainer.h"
#include "DREAM3DLib/Common/OrientationMath.h"
#include "DREAM3DLib/Common/NeighborList.hpp"


/**
 * @class NeighborOrientationCorrelation NeighborOrientationCorrelation.h DREAM3DLib/ReconstructionFilters/NeighborOrientationCorrelation.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT NeighborOrientationCorrelation : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(NeighborOrientationCorrelation)
    DREAM3D_STATIC_NEW_MACRO(NeighborOrientationCorrelation)
    DREAM3D_TYPE_MACRO_SUPER(NeighborOrientationCorrelation, AbstractFilter)

    virtual ~NeighborOrientationCorrelation();

	//------ Required Cell Data
	DREAM3D_INSTANCE_STRING_PROPERTY(QuatsArrayName)
	DREAM3D_INSTANCE_STRING_PROPERTY(ConfidenceIndexArrayName)
	DREAM3D_INSTANCE_STRING_PROPERTY(CellPhasesArrayName)
    //------ Required Ensemble Data
    DREAM3D_INSTANCE_STRING_PROPERTY(CrystalStructuresArrayName)

	DREAM3D_INSTANCE_PROPERTY(float, MisorientationTolerance)
	DREAM3D_INSTANCE_PROPERTY(float, MinConfidence)
	DREAM3D_INSTANCE_PROPERTY(int, Level)

    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::ProcessingFilters; }
    virtual const std::string getSubGroupName()  { return DREAM3D::FilterSubGroups::CleanupFilters; }
    virtual const std::string getHumanLabel() { return "Neighbor Orientation Correlation"; }

    virtual void setupFilterParameters();
	virtual void writeFilterParameters(AbstractFilterParametersWriter* writer);


    virtual void execute();
    virtual void preflight();

  protected:
    NeighborOrientationCorrelation();


  private:
    std::vector<OrientationMath::Pointer> m_OrientationOps;

    float* m_ConfidenceIndex;
    float* m_Quats;
    int32_t* m_CellPhases;
    unsigned int* m_CrystalStructures;

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);


    NeighborOrientationCorrelation(const NeighborOrientationCorrelation&); // Copy Constructor Not Implemented
    void operator=(const NeighborOrientationCorrelation&); // Operator '=' Not Implemented
};

#endif /* NeighborOrientationCorrelation_H_ */
