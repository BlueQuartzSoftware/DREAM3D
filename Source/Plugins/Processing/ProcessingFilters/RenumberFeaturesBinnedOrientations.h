/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2015 Dr. Joseph C. Tucker (UES, Inc.)
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
 * Neither the name of Joseph C. Tucker, Michael A. Groeber, Michael A. Jackson,
 * UES, Inc., the US Air Force, BlueQuartz Software nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
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
 *
 *  This code was written under United States Air Force Contract number
 *                   FA8650-07-D-5800 and FA8650-10-D-5226
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _RenumberFeaturesBinnedOrientations_H_
#define _RenumberFeaturesBinnedOrientations_H_

#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/Common/AbstractFilter.h"

#include "OrientationLib/OrientationOps/OrientationOps.h"


/**
 * @class RenumberFeaturesBinnedOrientations RenumberFeaturesBinnedOrientations.h /Processing/RenumberFeaturesBinnedOrientations.h
 * @brief
 * @author Joseph C. Tucker
 * @date Feb 5, 2015
 * @version 5.1
 */
class RenumberFeaturesBinnedOrientations : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(RenumberFeaturesBinnedOrientations)
    DREAM3D_STATIC_NEW_MACRO(RenumberFeaturesBinnedOrientations)
    DREAM3D_TYPE_MACRO_SUPER(RenumberFeaturesBinnedOrientations, AbstractFilter)

    virtual ~RenumberFeaturesBinnedOrientations();
    DREAM3D_FILTER_PARAMETER(DataArrayPath, CellFeatureAttributeMatrixPath)
    Q_PROPERTY(DataArrayPath CellFeatureAttributeMatrixPath READ getCellFeatureAttributeMatrixPath WRITE setCellFeatureAttributeMatrixPath)

    DREAM3D_FILTER_PARAMETER(int, BinWidth)
    Q_PROPERTY(int BinWidth READ getBinWidth WRITE setBinWidth)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
    Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

    DREAM3D_FILTER_PARAMETER(QString, ParentIdsArrayName)
    Q_PROPERTY(QString ParentIdsArrayName READ getParentIdsArrayName WRITE setParentIdsArrayName)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, FeatureEulerAnglesArrayPath)
    Q_PROPERTY(DataArrayPath FeatureEulerAnglesArrayPath READ getFeatureEulerAnglesArrayPath WRITE setFeatureEulerAnglesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
    Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
    Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName();
    virtual const QString getHumanLabel();

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

    /**
       * @brief Reimplemented from @see AbstractFilter class
       */
    virtual void execute();
    virtual void preflight();

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    int calcODFBin(float dim[3], float bins[3], float step[3], float r1, float r2, float r3);

    RenumberFeaturesBinnedOrientations();


  private:
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, FeatureIds)
    DEFINE_CREATED_DATAARRAY_VARIABLE(int32_t, ParentIds)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, FeatureEulerAngles)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, FeaturePhases)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(uint32_t, CrystalStructures)

    QVector<OrientationOps::Pointer> m_OrientationOps;

    void dataCheck();

    RenumberFeaturesBinnedOrientations(const RenumberFeaturesBinnedOrientations&); // Copy Constructor Not Implemented
    void operator=(const RenumberFeaturesBinnedOrientations&); // Operator '=' Not Implemented
};

#endif /* RenumberFeaturesBinnedOrientations_H_ */








