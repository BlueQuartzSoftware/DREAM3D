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

#ifndef _FindSlipTransmissionMetrics_H_
#define _FindSlipTransmissionMetrics_H_

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include <cstddef>
#include <vector>
#include <QtCore/QString>
#include <iostream>
#include <cmath>
#include <fstream>
#include <list>
#include <algorithm>
#include <numeric>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/DataContainers/DataContainer.h"
#include "DREAM3DLib/DataArrays/NeighborList.hpp"
#include "DREAM3DLib/Common/AbstractFilter.h"

#include "OrientationLib/OrientationOps/OrientationOps.h"
#include "OrientationLib/OrientationOps/CubicOps.h"
#include "OrientationLib/OrientationOps/HexagonalOps.h"
#include "OrientationLib/OrientationOps/OrthoRhombicOps.h"


#include "Statistics/StatisticsConstants.h"
/**
 * @class FindSlipTransmissionMetrics FindSlipTransmissionMetrics.h DREAM3DLib/GenericFilters/FindSlipTransmissionMetrics.h
 * @brief
 * @author
 * @date Mar 13, 2014
 * @version 4.2.4818
 */
class FindSlipTransmissionMetrics : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(FindSlipTransmissionMetrics)
    DREAM3D_STATIC_NEW_MACRO(FindSlipTransmissionMetrics)
    DREAM3D_TYPE_MACRO_SUPER(FindSlipTransmissionMetrics, AbstractFilter)

    virtual ~FindSlipTransmissionMetrics();

    DREAM3D_FILTER_PARAMETER(DataArrayPath, NeighborListArrayPath)
    Q_PROPERTY(DataArrayPath NeighborListArrayPath READ getNeighborListArrayPath WRITE setNeighborListArrayPath)

    DREAM3D_FILTER_PARAMETER(QString, F1ListArrayName)
    Q_PROPERTY(QString F1ListArrayName READ getF1ListArrayName WRITE setF1ListArrayName)

    DREAM3D_FILTER_PARAMETER(QString, F1sptListArrayName)
    Q_PROPERTY(QString F1sptListArrayName READ getF1sptListArrayName WRITE setF1sptListArrayName)

    DREAM3D_FILTER_PARAMETER(QString, F7ListArrayName)
    Q_PROPERTY(QString F7ListArrayName READ getF7ListArrayName WRITE setF7ListArrayName)

    DREAM3D_FILTER_PARAMETER(QString, mPrimeListArrayName)
    Q_PROPERTY(QString mPrimeListArrayName READ getmPrimeListArrayName WRITE setmPrimeListArrayName)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, AvgQuatsArrayPath)
    Q_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
    Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
    Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName();
    virtual const QString getHumanLabel();

    /**
    * @brief This method will instantiate all the end user settable options/parameters
    * for this filter
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
    FindSlipTransmissionMetrics();

  private:
    QVector<OrientationOps::Pointer> m_OrientationOps;
    CubicOps::Pointer m_CubicOps;
    HexagonalOps::Pointer m_HexOps;
    OrthoRhombicOps::Pointer m_OrthoOps;

    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, FeaturePhases)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, AvgQuats)
    NeighborList<float>::WeakPointer m_F1List;
    NeighborList<float>::WeakPointer m_F1sptList;
    NeighborList<float>::WeakPointer m_F7List;
    NeighborList<float>::WeakPointer m_mPrimeList;
    NeighborList<int>::WeakPointer m_NeighborList;
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(unsigned int, CrystalStructures)

    void dataCheck();

    FindSlipTransmissionMetrics(const FindSlipTransmissionMetrics&); // Copy Constructor Not Implemented
    void operator=(const FindSlipTransmissionMetrics&); // Operator '=' Not Implemented
};

#endif /* FindSlipTransmissionMetrics_H_ */



