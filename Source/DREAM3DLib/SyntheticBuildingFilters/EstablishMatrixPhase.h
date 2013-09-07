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

#ifndef EstablishMatrixPhase_H_
#define EstablishMatrixPhase_H_

#include <QtCore/QString>
#include <vector>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/IDataArray.h"
#include "DREAM3DLib/Common/StatsDataArray.h"
#include "DREAM3DLib/Common/StatsData.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/VolumeDataContainer.h"


/**
 * @class EstablishMatrixPhases EstablishMatrixPhases.h DREAM3DLib/SyntheticBuilderFilters/EstablishMatrixPhases.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT EstablishMatrixPhase : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(EstablishMatrixPhase)
    DREAM3D_STATIC_NEW_MACRO(EstablishMatrixPhase)
    DREAM3D_TYPE_MACRO_SUPER(EstablishMatrixPhase, AbstractFilter)

    virtual ~EstablishMatrixPhase();

	//------ Required Cell Data
	DREAM3D_INSTANCE_STRING_PROPERTY(GrainIdsArrayName)
	DREAM3D_INSTANCE_STRING_PROPERTY(CellPhasesArrayName)
	//------ Created Field Data
	DREAM3D_INSTANCE_STRING_PROPERTY(ActiveArrayName)
	DREAM3D_INSTANCE_STRING_PROPERTY(FieldPhasesArrayName)
	//------ Required Ensemble Data
	DREAM3D_INSTANCE_STRING_PROPERTY(PhaseTypesArrayName)

    virtual const QString getGroupName() { return DREAM3D::FilterGroups::SyntheticBuildingFilters; }
	virtual const QString getSubGroupName() { return DREAM3D::FilterSubGroups::PackingFilters; }
    virtual const QString getHumanLabel() { return "Establish Matrix Phase"; }

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
    EstablishMatrixPhase();

    void establish_matrix();

    std::vector<int> matrixphases;
    std::vector<float> matrixphasefractions;

  private:


	size_t firstMatrixField;
    unsigned long long int Seed;
    float sizex;
    float sizey;
    float sizez;
    float totalvol;


    int32_t* m_GrainIds;
    int32_t* m_CellPhases;

    bool* m_Active;
    int32_t* m_FieldPhases;

    unsigned int* m_PhaseTypes;
    StatsDataArray* m_StatsDataArray;

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

    EstablishMatrixPhase(const EstablishMatrixPhase&); // Copy Constructor Not Implemented
    void operator=(const EstablishMatrixPhase&); // Operator '=' Not Implemented
};

#endif /* EstablishMatrixPhase_H_ */
