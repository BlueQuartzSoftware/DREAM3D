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
#include <QtCore/QString>


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/IOFilters/FileReader.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"


class DREAM3DLib_EXPORT YSChoiAbaqusReader : public FileReader
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(YSChoiAbaqusReader)
    DREAM3D_STATIC_NEW_MACRO(YSChoiAbaqusReader)
    DREAM3D_TYPE_MACRO_SUPER(YSChoiAbaqusReader, FileReader)


    virtual ~YSChoiAbaqusReader();
    DREAM3D_INSTANCE_STRING_PROPERTY(DataContainerName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellEnsembleAttributeMatrixName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellFeatureAttributeMatrixName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellAttributeMatrixName)

    DREAM3D_FILTER_PARAMETER(QString, InputFile)
    Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)
    DREAM3D_FILTER_PARAMETER(QString, InputFeatureInfoFile)
    Q_PROPERTY(QString InputFeatureInfoFile READ getInputFeatureInfoFile WRITE setInputFeatureInfoFile)

    virtual const QString getGroupName() { return DREAM3D::FilterGroups::IOFilters; }
    virtual const QString getSubGroupName() { return DREAM3D::FilterSubGroups::InputFilters; }
    virtual const QString getHumanLabel() { return "Read YS Choi Abaqus Vtk Output File"; }

    virtual void setupFilterParameters();
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    virtual void preflight();
    virtual void execute();

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    YSChoiAbaqusReader();


  private:
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, FeatureIds)
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, CellPhases)
    DEFINE_PTR_WEAKPTR_DATAARRAY(bool, SurfaceFeatures)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, Quats)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, AvgQuats)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, CellEulerAngles)

    DEFINE_PTR_WEAKPTR_DATAARRAY(unsigned int, CrystalStructures)

    void dataCheck();

    YSChoiAbaqusReader(const YSChoiAbaqusReader&); // Copy Constructor Not Implemented
    void operator=(const YSChoiAbaqusReader&); // Operator '=' Not Implemented
};

#endif /* YSCHOIABAQUSREADER_H_ */
