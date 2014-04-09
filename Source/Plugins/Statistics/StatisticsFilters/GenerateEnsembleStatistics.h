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
 *Find
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

#ifndef GenerateEnsembleStatistics_H_
#define GenerateEnsembleStatistics_H_

#include <vector>
#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/DataArrays/StatsDataArray.h"
#include "DREAM3DLib/StatsData/StatsData.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"
#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"
#include "DREAM3DLib/DataArrays/NeighborList.hpp"
#include "DREAM3DLib/DistributionAnalysisOps/DistributionAnalysisOps.h"

#include "Statistics/StatisticsConstants.h"
/**
 * @class GenerateEnsembleStatistics GenerateEnsembleStatistics.h DREAM3DLib/GenericFilters/GenerateEnsembleStatistics.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class GenerateEnsembleStatistics : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(GenerateEnsembleStatistics)
    DREAM3D_STATIC_NEW_MACRO(GenerateEnsembleStatistics)
    DREAM3D_TYPE_MACRO_SUPER(GenerateEnsembleStatistics, AbstractFilter)

    virtual ~GenerateEnsembleStatistics();
    DREAM3D_FILTER_PARAMETER(DataArrayPath, CellEnsembleAttributeMatrixName)
    Q_PROPERTY(DataArrayPath CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, NeighborListArrayPath)
    Q_PROPERTY(DataArrayPath NeighborListArrayPath READ getNeighborListArrayPath WRITE setNeighborListArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, SharedSurfaceAreaListArrayPath)
    Q_PROPERTY(DataArrayPath SharedSurfaceAreaListArrayPath READ getSharedSurfaceAreaListArrayPath WRITE setSharedSurfaceAreaListArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
    Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, BiasedFeaturesArrayPath)
    Q_PROPERTY(DataArrayPath BiasedFeaturesArrayPath READ getBiasedFeaturesArrayPath WRITE setBiasedFeaturesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, EquivalentDiametersArrayPath)
    Q_PROPERTY(DataArrayPath EquivalentDiametersArrayPath READ getEquivalentDiametersArrayPath WRITE setEquivalentDiametersArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, NeighborhoodsArrayPath)
    Q_PROPERTY(DataArrayPath NeighborhoodsArrayPath READ getNeighborhoodsArrayPath WRITE setNeighborhoodsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, AspectRatiosArrayPath)
    Q_PROPERTY(DataArrayPath AspectRatiosArrayPath READ getAspectRatiosArrayPath WRITE setAspectRatiosArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, Omega3sArrayPath)
    Q_PROPERTY(DataArrayPath Omega3sArrayPath READ getOmega3sArrayPath WRITE setOmega3sArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, AxisEulerAnglesArrayPath)
    Q_PROPERTY(DataArrayPath AxisEulerAnglesArrayPath READ getAxisEulerAnglesArrayPath WRITE setAxisEulerAnglesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
    Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, SurfaceFeaturesArrayPath)
    Q_PROPERTY(DataArrayPath SurfaceFeaturesArrayPath READ getSurfaceFeaturesArrayPath WRITE setSurfaceFeaturesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, VolumesArrayPath)
    Q_PROPERTY(DataArrayPath VolumesArrayPath READ getVolumesArrayPath WRITE setVolumesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, FeatureEulerAnglesArrayPath)
    Q_PROPERTY(DataArrayPath FeatureEulerAnglesArrayPath READ getFeatureEulerAnglesArrayPath WRITE setFeatureEulerAnglesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, AvgQuatsArrayPath)
    Q_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)

    DREAM3D_FILTER_PARAMETER(QString, PhaseTypesArrayName)
    Q_PROPERTY(QString PhaseTypesArrayName READ getPhaseTypesArrayName WRITE setPhaseTypesArrayName)

    DREAM3D_FILTER_PARAMETER(QString, StatisticsArrayName)
    Q_PROPERTY(QString StatisticsArrayName READ getStatisticsArrayName WRITE setStatisticsArrayName)

    virtual const QString getCompiledLibraryName() { return Statistics::StatisticsBaseName; }
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName() { return DREAM3D::FilterGroups::StatisticsFilters; }
    virtual const QString getSubGroupName() { return DREAM3D::FilterSubGroups::EnsembleStatsFilters; }
    virtual const QString getHumanLabel() { return "Generate Ensemble Statistics"; }

    /**
    * @brief This method will instantiate all the end user settable options/parameters
    * for this filter
    */
    virtual void setupFilterParameters();

    DREAM3D_INSTANCE_PROPERTY(float, SizeCorrelationResolution)
    DREAM3D_INSTANCE_PROPERTY(bool, SizeDistribution)
    DREAM3D_INSTANCE_PROPERTY(int, SizeDistributionFitType)
    DREAM3D_INSTANCE_PROPERTY(bool, AspectRatioDistribution)
    DREAM3D_INSTANCE_PROPERTY(int, AspectRatioDistributionFitType)
    DREAM3D_INSTANCE_PROPERTY(bool, Omega3Distribution)
    DREAM3D_INSTANCE_PROPERTY(int, Omega3DistributionFitType)
    DREAM3D_INSTANCE_PROPERTY(bool, NeighborhoodDistribution)
    DREAM3D_INSTANCE_PROPERTY(int, NeighborhoodDistributionFitType)
    DREAM3D_INSTANCE_PROPERTY(bool, CalculateODF)
    DREAM3D_INSTANCE_PROPERTY(bool, CalculateMDF)
    DREAM3D_INSTANCE_PROPERTY(bool, CalculateAxisODF)
    DREAM3D_INSTANCE_PROPERTY(QVector<unsigned int>, PhaseTypeArray)

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
    GenerateEnsembleStatistics();

    void gatherSizeStats();
    void gatherAspectRatioStats();
    void gatherOmega3Stats();
    void gatherNeighborhoodStats();
    void gatherMDFStats();
    void gatherODFStats();
    void gatherAxisODFStats();

  private:
    QVector<OrientationOps::Pointer> m_OrientationOps;
    CubicOps::Pointer m_CubicOps;
    HexagonalOps::Pointer m_HexOps;
    OrthoRhombicOps::Pointer m_OrthoOps;

    DEFINE_PTR_WEAKPTR_DATAARRAY(float, AvgQuats)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, FeatureEulerAngles)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, Volumes)
    DEFINE_PTR_WEAKPTR_DATAARRAY(bool, BiasedFeatures)
    DEFINE_PTR_WEAKPTR_DATAARRAY(bool, SurfaceFeatures)
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, FeaturePhases)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, AxisEulerAngles)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, Omega3s)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, AspectRatios)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, EquivalentDiameters)
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, Neighborhoods)
    DEFINE_PTR_WEAKPTR_DATAARRAY(unsigned int, CrystalStructures)
    DEFINE_CREATED_DATAARRAY(uint32_t, PhaseTypes)

    NeighborList<int>::WeakPointer m_NeighborList;
    NeighborList<float>::WeakPointer m_SharedSurfaceAreaList;

    StatsDataArray::WeakPointer m_StatsDataArray;

    QVector<DistributionAnalysisOps::Pointer> m_DistributionAnalysis;

    void dataCheck();

    GenerateEnsembleStatistics(const GenerateEnsembleStatistics&); // Copy Constructor Not Implemented
    void operator=(const GenerateEnsembleStatistics&); // Operator '=' Not Implemented
};

#endif /* GenerateEnsembleStatistics_H_ */



