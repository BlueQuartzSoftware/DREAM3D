/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef YSCHOIABAQUSREADER_H_
#define YSCHOIABAQUSREADER_H_

#include <string.h> // needed for the ::memcpy function below
#include <string>

#include "MXA/Common/MXAEndian.h"
#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/IOFilters/FileReader.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"


class DREAM3DLib_EXPORT YSChoiAbaqusReader : public FileReader
{
  public:
  DREAM3D_SHARED_POINTERS(YSChoiAbaqusReader)
  DREAM3D_STATIC_NEW_MACRO(YSChoiAbaqusReader)
  DREAM3D_TYPE_MACRO_SUPER(YSChoiAbaqusReader, FileReader)


    virtual ~YSChoiAbaqusReader();

    DREAM3D_INSTANCE_STRING_PROPERTY(InputFile)
    DREAM3D_INSTANCE_STRING_PROPERTY(InputGrainInfoFile)

	//------ Created Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(GrainIdsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellPhasesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(SurfaceFieldsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(QuatsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellEulerAnglesArrayName)
	//------ Created Field Data
    DREAM3D_INSTANCE_STRING_PROPERTY(AvgQuatsArrayName)
    //------ Created Ensemble Data
    DREAM3D_INSTANCE_STRING_PROPERTY(CrystalStructuresArrayName)

    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::IOFilters; }
	virtual const std::string getSubGroupName() { return DREAM3D::FilterSubGroups::InputFilters; }
    virtual const std::string getHumanLabel() { return "Read YS Choi Abaqus Vtk Output File"; }

	virtual void setupFilterParameters();
	virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);
    
    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    virtual void preflight();
	virtual void execute();

  protected:
    YSChoiAbaqusReader();


  private:
    int32_t* m_GrainIds;
    int32_t* m_CellPhases;
    bool* m_SurfaceFields;
    float* m_Quats;
    float* m_AvgQuats;
    float* m_CellEulerAngles;

	unsigned int* m_CrystalStructures;

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

    YSChoiAbaqusReader(const YSChoiAbaqusReader&); // Copy Constructor Not Implemented
    void operator=(const YSChoiAbaqusReader&); // Operator '=' Not Implemented
};

#endif /* YSCHOIABAQUSREADER_H_ */
