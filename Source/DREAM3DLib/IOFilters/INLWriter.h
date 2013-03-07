/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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

#ifndef _INLWRITER_H_
#define _INLWRITER_H_



#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/DataArray.hpp"
#include "DREAM3DLib/Common/FileWriter.h"
#include "DREAM3DLib/Common/StringDataArray.hpp"

/**
 * @class INLWriter INLWriter.h DREAM3DLib/IOFilters/INLWriter.h
 * @brief Writes out an Text file according to Brad Fromm at INL.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Nov 13, 2012
 * @version 1.0
 */
class DREAM3DLib_EXPORT INLWriter : public FileWriter
{
  public:
    DREAM3D_SHARED_POINTERS(INLWriter)
    DREAM3D_STATIC_NEW_MACRO(INLWriter)
    DREAM3D_TYPE_MACRO_SUPER(INLWriter, FileWriter)


    virtual ~INLWriter();

    //------ Required Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(GrainIdsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellPhasesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellEulerAnglesArrayName)
    //------ Required Ensemble Data
    DREAM3D_INSTANCE_STRING_PROPERTY(CrystalStructuresArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(MaterialNamesArrayName)


    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::IOFilters; }
	virtual const std::string getSubGroupName() { return DREAM3D::FilterSubGroups::OutputFilters; }
    virtual const std::string getHumanLabel() { return "Write INL File"; }

    virtual void setupFilterParameters();
    virtual void writeFilterParameters(AbstractFilterParametersWriter* writer);

    virtual void preflight();

  protected:
    INLWriter();

    virtual int writeHeader();
    virtual int writeFile();

  private:
    int32_t* m_GrainIds;
    int32_t* m_CellPhases;
    float*   m_CellEulerAngles;
    unsigned int* m_CrystalStructures;
    StringDataArray::Pointer m_MaterialNames;

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

    INLWriter(const INLWriter&); // Copy Constructor Not Implemented
    void operator=(const INLWriter&); // Operator '=' Not Implemented
};

#endif /* _INLWRITER_H_ */
