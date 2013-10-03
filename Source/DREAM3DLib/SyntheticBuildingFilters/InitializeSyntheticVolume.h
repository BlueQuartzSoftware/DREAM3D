/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#ifndef _INITIALIZESYNTHETICVOLUME_H_
#define _INITIALIZESYNTHETICVOLUME_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"

/**
 * @class InitializeSyntheticVolume InitializeSyntheticVolume.h DREAM3DLib/SyntheticBuildingFilters/InitializeSyntheticVolume.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class DREAM3DLib_EXPORT InitializeSyntheticVolume : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(InitializeSyntheticVolume)
    DREAM3D_STATIC_NEW_MACRO(InitializeSyntheticVolume)
    DREAM3D_TYPE_MACRO_SUPER(InitializeSyntheticVolume, AbstractFilter)


    virtual ~InitializeSyntheticVolume();

    //------ Created Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(GrainIdsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellPhasesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(GoodVoxelsArrayName)
    //------ Required Ensemble Data
    DREAM3D_INSTANCE_STRING_PROPERTY(ShapeTypesArrayName)

    virtual const std::string getGroupName() {return DREAM3D::FilterGroups::SyntheticBuildingFilters;}
    virtual const std::string getSubGroupName() { return DREAM3D::FilterSubGroups::PackingFilters; }
    virtual const std::string getHumanLabel() {return "Initialize Synthetic Volume";}


    DREAM3D_INSTANCE_STRING_PROPERTY(InputFile)
    DREAM3D_INSTANCE_PROPERTY(int, XVoxels)
    DREAM3D_INSTANCE_PROPERTY(int, YVoxels)
    DREAM3D_INSTANCE_PROPERTY(int, ZVoxels)
    DREAM3D_INSTANCE_PROPERTY(float, XRes)
    DREAM3D_INSTANCE_PROPERTY(float, YRes)
    DREAM3D_INSTANCE_PROPERTY(float, ZRes)
    DREAM3D_INSTANCE_PROPERTY(std::vector<uint32_t>, ShapeTypes)


    virtual void setupFilterParameters();
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

    virtual void preflight();

    /**
     * @brief Reimplemented from @see AbstractFilter class
     */
    virtual void execute();

  protected:
    InitializeSyntheticVolume();

  private:
    // Cell Data - make sure these are all initialized to NULL in the constructor
    int32_t* m_GrainIds;
    int32_t* m_CellPhases;
    bool* m_GoodVoxels;

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

    InitializeSyntheticVolume(const InitializeSyntheticVolume&); // Copy Constructor Not Implemented
    void operator=(const InitializeSyntheticVolume&); // Operator '=' Not Implemented
};

#endif /* _INITIALIZESYNTHETICVOLUME_H_ */
