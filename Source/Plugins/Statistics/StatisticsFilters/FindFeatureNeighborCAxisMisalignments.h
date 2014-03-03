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

#ifndef FINDFIELDNEIGHBORCAXISMISALIGNMENTS_H_
#define FINDFIELDNEIGHBORCAXISMISALIGNMENTS_H_

#include <vector>
#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"
#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"
#include "DREAM3DLib/DataArrays/NeighborList.hpp"

#include "Statistics/StatisticsConstants.h"
/**
 * @class FindFeatureNeighborCAxisMisalignments FindFeatureNeighborCAxisMisalignments.h Plugins/Statistics/StatisticsFilters/FindFeatureNeighborCAxisMisalignments.h
 * @brief
 * @author Michael A Groeber (AFRL) & Joseph C Tucker (UES)
 * @date Jan 29, 2014
 * @version 5.0
 */
class FindFeatureNeighborCAxisMisalignments : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(FindFeatureNeighborCAxisMisalignments)
    DREAM3D_STATIC_NEW_MACRO(FindFeatureNeighborCAxisMisalignments)
    DREAM3D_TYPE_MACRO_SUPER(FindFeatureNeighborCAxisMisalignments, AbstractFilter)

    virtual ~FindFeatureNeighborCAxisMisalignments();

    DREAM3D_INSTANCE_STRING_PROPERTY(DataContainerName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellEnsembleAttributeMatrixName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellFeatureAttributeMatrixName)

    DREAM3D_INSTANCE_STRING_PROPERTY(NeighborListArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CAxisMisalignmentListArrayName)
    DREAM3D_FILTER_PARAMETER(bool, FindAvgMisals)
    Q_PROPERTY(bool FindAvgMisals READ getFindAvgMisals WRITE setFindAvgMisals)

    virtual const QString getCompiledLibraryName() { return Statistics::StatisticsBaseName; }
    virtual const QString getGroupName() { return DREAM3D::FilterGroups::StatisticsFilters; }
    virtual const QString getSubGroupName() { return DREAM3D::FilterSubGroups::CrystallographicFilters; }
    virtual const QString getHumanLabel() { return "Find Feature Neighbor C-Axis Misalignments"; }


    /**
     * @brief setupFilterParameters
     */
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
    FindFeatureNeighborCAxisMisalignments();

  private:
    QVector<OrientationOps::Pointer> m_OrientationOps;
    CubicOps::Pointer m_CubicOps;
    HexagonalOps::Pointer m_HexOps;
    OrthoRhombicOps::Pointer m_OrthoOps;

    DEFINE_PTR_WEAKPTR_DATAARRAY(float, AvgQuats)
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, FeaturePhases)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, AvgCAxisMisalignments)
    DEFINE_PTR_WEAKPTR_DATAARRAY(unsigned int, CrystalStructures)
    NeighborList<int>* m_NeighborList;
    NeighborList<float>* m_CAxisMisalignmentList;

    void dataCheck();

    FindFeatureNeighborCAxisMisalignments(const FindFeatureNeighborCAxisMisalignments&); // Copy Constructor Not Implemented
    void operator=(const FindFeatureNeighborCAxisMisalignments&); // Operator '=' Not Implemented
};

#endif /* FINDFIELDNEIGHBORCAXISMISALIGNMENTS_H_ */


