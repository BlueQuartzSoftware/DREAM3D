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

#ifndef FINDAXISODF_H_
#define FINDAXISODF_H_

#include <vector>
#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/IDataArray.h"
#include "DREAM3DLib/Common/StatsDataArray.h"
#include "DREAM3DLib/Common/StatsData.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/OrientationMath.h"
#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"
#include "DREAM3DLib/Common/VoxelDataContainer.h"

/**
 * @class FindAxisODF FindAxisODF.h DREAM3DLib/GenericFilters/FindAxisODF.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT FindAxisODF : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(FindAxisODF)
    DREAM3D_STATIC_NEW_MACRO(FindAxisODF)
    DREAM3D_TYPE_MACRO_SUPER(FindAxisODF, AbstractFilter)

    virtual ~FindAxisODF();

	//------ Required Field Data
	DREAM3D_INSTANCE_STRING_PROPERTY(AxisEulerAnglesArrayName)
	DREAM3D_INSTANCE_STRING_PROPERTY(FieldPhasesArrayName)
	DREAM3D_INSTANCE_STRING_PROPERTY(SurfaceFieldsArrayName)
	//------ Required Ensemble Data
	DREAM3D_INSTANCE_STRING_PROPERTY(PhaseTypesArrayName)

    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::StatisticsFilters; }
	 virtual const std::string getSubGroupName() { return DREAM3D::FilterSubGroups::MorphologicalFilters; }
    virtual const std::string getHumanLabel() { return "Find Axis ODF"; }

	virtual void writeFilterParameters(AbstractFilterParametersWriter* writer);

	 /**
     * @brief Reimplemented from @see AbstractFilter class
     */
    virtual void execute();
    virtual void preflight();

  protected:
    FindAxisODF();

  private:
    bool* m_SurfaceFields;
    int32_t* m_FieldPhases;
    float* m_AxisEulerAngles;
    unsigned int* m_PhaseTypes;

    StatsDataArray* m_StatsDataArray;

    std::vector<OrientationMath*> m_OrientationOps;
    CubicOps::Pointer m_CubicOps;
    HexagonalOps::Pointer m_HexOps;
    OrthoRhombicOps::Pointer m_OrthoOps;

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);


    FindAxisODF(const FindAxisODF&); // Copy Constructor Not Implemented
    void operator=(const FindAxisODF&); // Operator '=' Not Implemented
};

#endif /* FINDAXISODF_H_ */
