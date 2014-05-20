/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2014 Dr. Joseph C. Tucker (UES, Inc.)
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

#ifndef GroupFeatures_H_
#define GroupFeatures_H_

#include <vector>
#include <QtCore/QString>


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/DataArrays/NeighborList.hpp"
#include "Reconstruction/ReconstructionConstants.h"

/**
 * @class GroupFeatures GroupFeatures.h DREAM3DLib/ReconstructionFilters/GroupFeatures.h
 * @brief
 * @author Joseph C Tucker (UES) & Michael A Groeber (AFRL)
 * @date Mar 11, 2014
 * @version 5.0
 */
class GroupFeatures : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(GroupFeatures)
    DREAM3D_STATIC_NEW_MACRO(GroupFeatures)
    DREAM3D_TYPE_MACRO_SUPER(GroupFeatures, AbstractFilter)

    virtual ~GroupFeatures();

    DREAM3D_FILTER_PARAMETER(DataArrayPath, ContiguousNeighborListArrayPath)
    Q_PROPERTY(DataArrayPath ContiguousNeighborListArrayPath READ getContiguousNeighborListArrayPath WRITE setContiguousNeighborListArrayPath)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, NonContiguousNeighborListArrayPath)
    Q_PROPERTY(DataArrayPath NonContiguousNeighborListArrayPath READ getNonContiguousNeighborListArrayPath WRITE setNonContiguousNeighborListArrayPath)

    DREAM3D_FILTER_PARAMETER(bool, UseNonContiguousNeighbors)
    Q_PROPERTY(float UseNonContiguousNeighbors READ getUseNonContiguousNeighbors WRITE setUseNonContiguousNeighbors)
    DREAM3D_FILTER_PARAMETER(bool, PatchGrouping)
    Q_PROPERTY(float PatchGrouping READ getPatchGrouping WRITE setPatchGrouping)

    virtual const QString getGroupName() {return DREAM3D::FilterGroups::ReconstructionFilters;}
    virtual const QString getSubGroupName() {return DREAM3D::FilterSubGroups::SegmentationFilters;}
    virtual const QString getHumanLabel() {return "Group Features";}
    virtual const QString getBrandingString() { return "DREAM3D Reconstruction Plugin"; }

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
    GroupFeatures();

    virtual int getSeed(int newFid);
    virtual bool determineGrouping(int referenceFeature, int neighborFeature, int newFid);
    virtual bool growPatch(int currentPatch);
    virtual bool growGrouping(int referenceFeature, int neighborFeature, int newFid);

  private:
    NeighborList<int>::WeakPointer m_ContiguousNeighborList;
    NeighborList<int>::WeakPointer m_NonContiguousNeighborList;

    void dataCheck();

    GroupFeatures(const GroupFeatures&); // Copy Constructor Not Implemented
    void operator=(const GroupFeatures&); // Operator '=' Not Implemented
};

#endif /* GroupFeatures_H_ */

