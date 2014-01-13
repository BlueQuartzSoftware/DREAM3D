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

#ifndef MERGETWINS_H_
#define MERGETWINS_H_

#include <vector>
#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"

#include "DREAM3DLib/ReconstructionFilters/GroupFeatures.h"

/**
 * @class MergeTwins MergeTwins.h DREAM3DLib/ReconstructionFilters/MergeTwins.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT MergeTwins : public GroupFeatures
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(MergeTwins)
    DREAM3D_STATIC_NEW_MACRO(MergeTwins)
    DREAM3D_TYPE_MACRO_SUPER(MergeTwins, AbstractFilter)

    virtual ~MergeTwins();
    DREAM3D_INSTANCE_STRING_PROPERTY(DataContainerName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellFeatureAttributeMatrixName)
    DREAM3D_INSTANCE_STRING_PROPERTY(NewCellFeatureAttributeMatrixName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellEnsembleAttributeMatrixName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellAttributeMatrixName)

    DREAM3D_INSTANCE_PROPERTY(float, AxisTolerance)
    DREAM3D_INSTANCE_PROPERTY(float, AngleTolerance)
    DREAM3D_INSTANCE_PROPERTY(bool, RandomizeParentIds)

    virtual const QString getGroupName() { return DREAM3D::FilterGroups::ReconstructionFilters; }
    virtual const QString getSubGroupName() {return DREAM3D::FilterSubGroups::GroupingFilters;}
    virtual const QString getHumanLabel() { return "Merge Twins"; }

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

  protected:
    MergeTwins();

    virtual size_t getSeed(size_t newFid);
    virtual bool determineGrouping(size_t referenceFeature, size_t neighborFeature, size_t newFid);
    void characterize_twins();

  private:
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, FeatureIds)
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, CellParentIds)
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, FeatureParentIds)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, AvgQuats)
    DEFINE_PTR_WEAKPTR_DATAARRAY(bool, Active)
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, FeaturePhases)
    DEFINE_PTR_WEAKPTR_DATAARRAY(unsigned int, CrystalStructures)

    unsigned long long int Seed;
    QVector<OrientationOps::Pointer> m_OrientationOps;

    void dataCheck();
    void updateFeatureInstancePointers();

    MergeTwins(const MergeTwins&); // Copy Constructor Not Implemented
    void operator=(const MergeTwins&); // Operator '=' Not Implemented
};

#endif /* MERGETWINS_H_ */
