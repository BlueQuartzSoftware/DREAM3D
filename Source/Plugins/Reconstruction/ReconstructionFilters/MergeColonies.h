/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2013 Dr. Joseph C. Tucker (UES, Inc.)
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

#ifndef MERGECOLONIES_H_
#define MERGECOLONIES_H_

#include <vector>
#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"

#include "Reconstruction/ReconstructionFilters/GroupFeatures.h"
/**
 * @class MergeColonies MergeColonies.h DREAM3DLib/ReconstructionFilters/MergeColonies.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class MergeColonies : public GroupFeatures
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(MergeColonies)
    DREAM3D_STATIC_NEW_MACRO(MergeColonies)
    DREAM3D_TYPE_MACRO_SUPER(MergeColonies, AbstractFilter)

    virtual ~MergeColonies();
    DREAM3D_INSTANCE_STRING_PROPERTY(DataContainerName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellFeatureAttributeMatrixName)
    DREAM3D_INSTANCE_STRING_PROPERTY(NewCellFeatureAttributeMatrixName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellEnsembleAttributeMatrixName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellAttributeMatrixName)

    virtual const QString getGroupName() { return DREAM3D::FilterGroups::ReconstructionFilters; }
    virtual const QString getSubGroupName() {return DREAM3D::FilterSubGroups::GroupingFilters;}
    virtual const QString getHumanLabel() { return "Reconstruct Prior Beta Features"; }

    DREAM3D_FILTER_PARAMETER(float, AxisTolerance)
    Q_PROPERTY(float AxisTolerance READ getAxisTolerance WRITE setAxisTolerance NOTIFY parametersChanged)
    DREAM3D_FILTER_PARAMETER(float, AngleTolerance)
    Q_PROPERTY(float AngleTolerance READ getAngleTolerance WRITE setAngleTolerance NOTIFY parametersChanged)
    DREAM3D_INSTANCE_PROPERTY(bool, RandomizeParentIds)
    DREAM3D_FILTER_PARAMETER(bool, IdentifyGlobAlpha)
    Q_PROPERTY(bool IdentifyGlobAlpha READ getIdentifyGlobAlpha WRITE setIdentifyGlobAlpha NOTIFY parametersChanged)

    virtual void setupFilterParameters();
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
    MergeColonies();

    virtual int getSeed(int newFid);
    virtual bool determineGrouping(int referenceFeature, int neighborFeature, int newFid);

    bool check_for_burgers(QuatF betaQuat, QuatF alphaQuat);
    void characterize_colonies();
    void identify_globAlpha();

  private:
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, FeatureIds)
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, CellPhases)
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, CellParentIds)
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, FeatureParentIds)
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, GlobAlpha)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, AvgQuats)
    DEFINE_PTR_WEAKPTR_DATAARRAY(bool, Active)
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, FeaturePhases)

    DEFINE_PTR_WEAKPTR_DATAARRAY(unsigned int, CrystalStructures)

    QVector<OrientationOps::Pointer> m_OrientationOps;

    void dataCheck();
    void updateFeatureInstancePointers();

    MergeColonies(const MergeColonies&); // Copy Constructor Not Implemented
    void operator=(const MergeColonies&); // Operator '=' Not Implemented
};

#endif /* MERGECOLONIES_H_ */

