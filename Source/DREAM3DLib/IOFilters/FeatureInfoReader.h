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


#ifndef _FeatureInfoReader_h_
#define _FeatureInfoReader_h_

#include <QtCore/QString>
#include <vector>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/IOFilters/FileReader.h"

/**
 * @class FeatureInfoReader FeatureInfoReader.h DREAM3DLib/IO/FeatureInfoReader.h
 * @brief
 * @author mjackson
 * @date Sep 28, 2011
 * @version $Revision$
 */
class DREAM3DLib_EXPORT FeatureInfoReader : public FileReader
{
  public:
    DREAM3D_SHARED_POINTERS(FeatureInfoReader)
    DREAM3D_STATIC_NEW_MACRO(FeatureInfoReader)
    DREAM3D_TYPE_MACRO_SUPER(FeatureInfoReader, FileReader)

    virtual ~FeatureInfoReader();
    DREAM3D_INSTANCE_STRING_PROPERTY(DataContainerName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellAttributeMatrixName)
    
   DREAM3D_INSTANCE_STRING_PROPERTY(FeatureIdsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellEulerAnglesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellPhasesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(FeatureEulerAnglesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(FeaturePhasesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(ActiveArrayName)

    /* Input Parameters */
    DREAM3D_INSTANCE_STRING_PROPERTY(InputFile)
    DREAM3D_INSTANCE_PROPERTY(bool, CreateCellLevelArrays)
    DREAM3D_INSTANCE_PROPERTY(bool, RenumberFeatures)

    virtual const QString getGroupName() { return DREAM3D::FilterGroups::IOFilters; }
    virtual const QString getSubGroupName() { return DREAM3D::FilterSubGroups::InputFilters; }
    virtual const QString getHumanLabel() { return "Read Feature Info File"; }

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

  protected:
    FeatureInfoReader();

    virtual int readHeader();
    virtual int readFile();

    void dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles);

  private:
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, FeatureIds)
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, CellPhases)
    float* m_CellEulerAngles;
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, FeaturePhases)
    float* m_FeatureEulerAngles;
    DEFINE_PTR_WEAKPTR_DATAARRAY(bool, Active)

    FeatureInfoReader(const FeatureInfoReader&); //Not Implemented
    void operator=(const FeatureInfoReader&); //Not Implemented

};

#endif //_FeatureInfoReader_h_


