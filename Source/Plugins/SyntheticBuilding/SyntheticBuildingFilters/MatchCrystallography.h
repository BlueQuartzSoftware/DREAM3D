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

#ifndef _MatchCrystallography_H_
#define _MatchCrystallography_H_

#include <QtCore/QString>
#include <numeric>
#include <limits>

#include <boost/shared_array.hpp>

#include "EbsdLib/EbsdConstants.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/DataArrays/NeighborList.hpp"
#include "DREAM3DLib/DataArrays/StatsDataArray.h"
#include "DREAM3DLib/DataContainers/DataContainer.h"
#include "DREAM3DLib/StatsData/StatsData.h"
#include "OrientationLib/OrientationOps/OrientationOps.h"

#include "SyntheticBuilding/SyntheticBuildingConstants.h"

/**
 * @class MatchCrystallography MatchCrystallography.h DREAM3DLib/SyntheticBuilderFilters/MatchCrystallography.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class MatchCrystallography : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(MatchCrystallography)
    DREAM3D_STATIC_NEW_MACRO(MatchCrystallography)
    DREAM3D_TYPE_MACRO_SUPER(MatchCrystallography, AbstractFilter)

    virtual ~MatchCrystallography();

//// Input data from the StatsGenerator Data Container (or something equivalent)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, InputStatsArrayPath)
    Q_PROPERTY(DataArrayPath InputStatsArrayPath READ getInputStatsArrayPath WRITE setInputStatsArrayPath)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
    Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, PhaseTypesArrayPath)
    Q_PROPERTY(DataArrayPath PhaseTypesArrayPath READ getPhaseTypesArrayPath WRITE setPhaseTypesArrayPath)

//// Input data from the Synthetic Data Container (or something equivalent)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
    Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
    Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, SurfaceFeaturesArrayPath)
    Q_PROPERTY(DataArrayPath SurfaceFeaturesArrayPath READ getSurfaceFeaturesArrayPath WRITE setSurfaceFeaturesArrayPath)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, NeighborListArrayPath)
    Q_PROPERTY(DataArrayPath NeighborListArrayPath READ getNeighborListArrayPath WRITE setNeighborListArrayPath)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, SharedSurfaceAreaListArrayPath)
    Q_PROPERTY(DataArrayPath SharedSurfaceAreaListArrayPath READ getSharedSurfaceAreaListArrayPath WRITE setSharedSurfaceAreaListArrayPath)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, NumFeaturesArrayPath)
    Q_PROPERTY(DataArrayPath NumFeaturesArrayPath READ getNumFeaturesArrayPath WRITE setNumFeaturesArrayPath)

//// Created Data Arrays
    DREAM3D_FILTER_PARAMETER(QString, CellEulerAnglesArrayName)
    Q_PROPERTY(QString CellEulerAnglesArrayName READ getCellEulerAnglesArrayName WRITE setCellEulerAnglesArrayName)
    DREAM3D_FILTER_PARAMETER(QString, VolumesArrayName)
    Q_PROPERTY(QString VolumesArrayName READ getVolumesArrayName WRITE setVolumesArrayName)
    DREAM3D_FILTER_PARAMETER(QString, FeatureEulerAnglesArrayName)
    Q_PROPERTY(QString FeatureEulerAnglesArrayName READ getFeatureEulerAnglesArrayName WRITE setFeatureEulerAnglesArrayName)
    DREAM3D_FILTER_PARAMETER(QString, AvgQuatsArrayName)
    Q_PROPERTY(QString AvgQuatsArrayName READ getAvgQuatsArrayName WRITE setAvgQuatsArrayName)


    DREAM3D_FILTER_PARAMETER(int, MaxIterations)
    Q_PROPERTY(int MaxIterations READ getMaxIterations WRITE setMaxIterations)

    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName();
    virtual const QString getHumanLabel();
    virtual const QString getBrandingString() { return SyntheticBuilding::SyntheticBuildingPluginDisplayName + " Filter"; }

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

    typedef boost::shared_array<float> SharedFloatArray;
    typedef boost::shared_array<int> SharedIntArray;

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    MatchCrystallography();

    void initializeArrays(int ensem);

    void determine_volumes();
    void determine_boundary_areas();
    void assign_eulers(int ensem);
    int pick_euler(float random, int numbins);
    void MC_LoopBody1(int feature, int ensem, int j, float neighsurfarea, unsigned int sym, QuatF& q1, QuatF& q2);
    void MC_LoopBody2(int feature, int phase, int j, float neighsurfarea, unsigned int sym, QuatF& q1, QuatF& q2);
    void matchCrystallography(int ensem);
    void measure_misorientations(int ensem);

  private:

    // Cell Data
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, FeatureIds)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, CellEulerAngles)

    // Feature Data
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(bool, SurfaceFeatures)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, FeaturePhases)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, Volumes)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, FeatureEulerAngles)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, AvgQuats)
    DEFINE_CREATED_DATAARRAY_VARIABLE(unsigned int, SyntheticCrystalStructures)
    NeighborList<int>::WeakPointer m_NeighborList;
    NeighborList<float>::WeakPointer m_SharedSurfaceAreaList;

    //Ensemble Data
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(unsigned int, CrystalStructures)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(uint32_t, PhaseTypes)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, NumFeatures)
    StatsDataArray::WeakPointer m_StatsDataArray;

    // All other private instance variables
    float mdfchange;
    float odfchange;

    QVector<float> unbiasedvol;
    QVector<float> m_TotalSurfaceArea;

    FloatArrayType::Pointer actualodf;
    FloatArrayType::Pointer simodf;
    FloatArrayType::Pointer actualmdf;
    FloatArrayType::Pointer simmdf;

    QVector<QVector<float> > m_MisorientationLists;

    QVector<OrientationOps::Pointer> m_OrientationOps;

    void dataCheck();

    MatchCrystallography(const MatchCrystallography&); // Copy Constructor Not Implemented
    void operator=(const MatchCrystallography&); // Operator '=' Not Implemented
};

#endif /* MATCHCRYSTALLOGRAPHY_H_ */





