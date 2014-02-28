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

#ifndef FindBoundaryStrengths_H_
#define FindBoundaryStrengths_H_

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

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"
#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"

#include "Statistics/StatisticsConstants.h"
/**
 * @class FindBoundaryStrengths FindBoundaryStrengths.h DREAM3DLib/GenericFilters/FindBoundaryStrengths.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class FindBoundaryStrengths : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(FindBoundaryStrengths)
    DREAM3D_STATIC_NEW_MACRO(FindBoundaryStrengths)
    DREAM3D_TYPE_MACRO_SUPER(FindBoundaryStrengths, AbstractFilter)

    virtual ~FindBoundaryStrengths();
    DREAM3D_INSTANCE_STRING_PROPERTY(DataContainerName)
    DREAM3D_INSTANCE_STRING_PROPERTY(SurfaceDataContainerName)
    DREAM3D_INSTANCE_STRING_PROPERTY(FaceAttributeMatrixName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellFeatureAttributeMatrixName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellEnsembleAttributeMatrixName)

    DREAM3D_FILTER_PARAMETER(FloatVec3_t, Loading)
    Q_PROPERTY(FloatVec3_t Loading READ getLoading WRITE setLoading)

    virtual const QString getCompiledLibraryName() { return Statistics::StatisticsBaseName; }
    virtual const QString getGroupName() { return DREAM3D::FilterGroups::StatisticsFilters; }
    virtual const QString getSubGroupName() { return DREAM3D::FilterSubGroups::CrystallographicFilters; }
    virtual const QString getHumanLabel() { return "Find Feature Boundary Strength Metrics"; }

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
    FindBoundaryStrengths();

  private:
    QVector<OrientationOps::Pointer> m_OrientationOps;
    CubicOps::Pointer m_CubicOps;
    HexagonalOps::Pointer m_HexOps;
    OrthoRhombicOps::Pointer m_OrthoOps;

    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, FeaturePhases)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, AvgQuats)
    DEFINE_PTR_WEAKPTR_DATAARRAY(unsigned int, CrystalStructures)

    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, SurfaceMeshFaceLabels)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, SurfaceMeshF1s)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, SurfaceMeshF1spts)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, SurfaceMeshF7s)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, SurfaceMeshmPrimes)

    void dataCheckVoxel();
    void dataCheckSurfaceMesh();

    FindBoundaryStrengths(const FindBoundaryStrengths&); // Copy Constructor Not Implemented
    void operator=(const FindBoundaryStrengths&); // Operator '=' Not Implemented
};

#endif /* FindBoundaryStrengths_H_ */




