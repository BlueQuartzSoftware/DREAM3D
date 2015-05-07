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

#ifndef _GenerateEnsembleStatistics_H_
#define _GenerateEnsembleStatistics_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/NeighborList.hpp"
#include "DREAM3DLib/DataArrays/StatsDataArray.h"

#include "OrientationLib/OrientationOps/CubicOps.h"
#include "OrientationLib/OrientationOps/HexagonalOps.h"
#include "OrientationLib/OrientationOps/OrientationOps.h"
#include "OrientationLib/OrientationOps/OrthoRhombicOps.h"

#include "Statistics/DistributionAnalysisOps/DistributionAnalysisOps.h"

/**
 * @brief The GenerateEnsembleStatistics class. See [Filter documentation](@ref generateensemblestatistics) for details.
 */
class GenerateEnsembleStatistics : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(GenerateEnsembleStatistics)
    DREAM3D_STATIC_NEW_MACRO(GenerateEnsembleStatistics)
    DREAM3D_TYPE_MACRO_SUPER(GenerateEnsembleStatistics, AbstractFilter)

    virtual ~GenerateEnsembleStatistics();

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CellEnsembleAttributeMatrixPath)
    Q_PROPERTY(DataArrayPath CellEnsembleAttributeMatrixPath READ getCellEnsembleAttributeMatrixPath WRITE setCellEnsembleAttributeMatrixPath)

    DREAM3D_FILTER_PARAMETER(QString, PhaseTypesArrayName)
    Q_PROPERTY(QString PhaseTypesArrayName READ getPhaseTypesArrayName WRITE setPhaseTypesArrayName)

    DREAM3D_INSTANCE_PROPERTY(UInt32Vector_t, PhaseTypeData)
//  private:
//      UInt32Vector_t m_PhaseTypeData;
//  public:
//    void setPhaseTypeData(UInt32Vector_t value) {
//      m_PhaseTypeData = value;
//      std::cout << "@@@ GenerateEnsembleStatistics::setPhaseTypeData(...)-> d.size()=" << m_PhaseTypeData.d.size() << std::endl;
//    }
//    UInt32Vector_t getPhaseTypeData() {
//      std::cout << "@@@ GenerateEnsembleStatistics::getPhaseTypeData()-> d.size()=" << m_PhaseTypeData.d.size() << std::endl;
//      return m_PhaseTypeData;
//    }

    Q_PROPERTY(UInt32Vector_t PhaseTypeData READ getPhaseTypeData WRITE setPhaseTypeData)

    int getPhaseCount();
    Q_PROPERTY(int PhaseCount READ getPhaseCount)

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

    DREAM3D_FILTER_PARAMETER(DataArrayPath, RDFArrayPath)
    Q_PROPERTY(DataArrayPath RDFArrayPath READ getRDFArrayPath WRITE setRDFArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, MaxMinRDFArrayPath)
    Q_PROPERTY(DataArrayPath MaxMinRDFArrayPath READ getMaxMinRDFArrayPath WRITE setMaxMinRDFArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, FeatureEulerAnglesArrayPath)
    Q_PROPERTY(DataArrayPath FeatureEulerAnglesArrayPath READ getFeatureEulerAnglesArrayPath WRITE setFeatureEulerAnglesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, AvgQuatsArrayPath)
    Q_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)

    DREAM3D_FILTER_PARAMETER(QString, StatisticsArrayName)
    Q_PROPERTY(QString StatisticsArrayName READ getStatisticsArrayName WRITE setStatisticsArrayName)

    DREAM3D_FILTER_PARAMETER(bool, IncludeRadialDistFunc)
    Q_PROPERTY(bool IncludeRadialDistFunc READ getIncludeRadialDistFunc WRITE setIncludeRadialDistFunc)

    DREAM3D_FILTER_PARAMETER(bool, CalculateMorphologicalStats)
    Q_PROPERTY(bool CalculateMorphologicalStats READ getCalculateMorphologicalStats WRITE setCalculateMorphologicalStats)

    DREAM3D_INSTANCE_PROPERTY(bool, ComputeSizeDistribution)
    //Q_PROPERTY(bool ComputeSizeDistribution READ getComputeSizeDistribution WRITE setComputeSizeDistribution)

    DREAM3D_FILTER_PARAMETER(int, SizeDistributionFitType)
    Q_PROPERTY(int SizeDistributionFitType READ getSizeDistributionFitType WRITE setSizeDistributionFitType)

    DREAM3D_FILTER_PARAMETER(bool, ComputeAspectRatioDistribution)
    //Q_PROPERTY(bool ComputeAspectRatioDistribution READ getComputeAspectRatioDistribution WRITE setComputeAspectRatioDistribution)

    DREAM3D_FILTER_PARAMETER(int, AspectRatioDistributionFitType)
    Q_PROPERTY(int AspectRatioDistributionFitType READ getAspectRatioDistributionFitType WRITE setAspectRatioDistributionFitType)

    DREAM3D_FILTER_PARAMETER(bool, ComputeOmega3Distribution)
    //Q_PROPERTY(bool ComputeOmega3Distribution READ getComputeOmega3Distribution WRITE setComputeOmega3Distribution)

    DREAM3D_FILTER_PARAMETER(int, Omega3DistributionFitType)
    Q_PROPERTY(int Omega3DistributionFitType READ getOmega3DistributionFitType WRITE setOmega3DistributionFitType)

    DREAM3D_FILTER_PARAMETER(bool, ComputeNeighborhoodDistribution)
    //Q_PROPERTY(bool ComputeNeighborhoodDistribution READ getComputeNeighborhoodDistribution WRITE setComputeNeighborhoodDistribution)

    DREAM3D_FILTER_PARAMETER(int, NeighborhoodDistributionFitType)
    Q_PROPERTY(int NeighborhoodDistributionFitType READ getNeighborhoodDistributionFitType WRITE setNeighborhoodDistributionFitType)

    DREAM3D_FILTER_PARAMETER(bool, CalculateCrystallographicStats)
    Q_PROPERTY(bool CalculateCrystallographicStats READ getCalculateCrystallographicStats WRITE setCalculateCrystallographicStats)

    DREAM3D_FILTER_PARAMETER(bool, CalculateODF)
    //Q_PROPERTY(bool CalculateODF READ getCalculateODF WRITE setCalculateODF)

    DREAM3D_FILTER_PARAMETER(bool, CalculateMDF)
    //Q_PROPERTY(bool CalculateMDF READ getCalculateMDF WRITE setCalculateMDF)

    DREAM3D_FILTER_PARAMETER(bool, CalculateAxisODF)
    //Q_PROPERTY(bool CalculateAxisODF READ getCalculateAxisODF WRITE setCalculateAxisODF)

    DREAM3D_FILTER_PARAMETER(float, SizeCorrelationResolution)
    Q_PROPERTY(float SizeCorrelationResolution READ getSizeCorrelationResolution WRITE setSizeCorrelationResolution)

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
    GenerateEnsembleStatistics();

    void gatherSizeStats();
    void gatherAspectRatioStats();
    void gatherOmega3Stats();
    void gatherNeighborhoodStats();
    void gatherMDFStats();
    void gatherODFStats();
    void gatherAxisODFStats();
    void gatherRadialDistFunc();
    void calculatePPTBoundaryFrac();

  private:
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, AvgQuats)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, FeatureEulerAngles)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, Volumes)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(bool, BiasedFeatures)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(bool, SurfaceFeatures)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, FeaturePhases)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, AxisEulerAngles)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, RadialDistFunc)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, MaxMinRadialDistFunc)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, Omega3s)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, AspectRatios)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, EquivalentDiameters)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, Neighborhoods)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(unsigned int, CrystalStructures)

    DEFINE_CREATED_DATAARRAY_VARIABLE(uint32_t, PhaseTypes)

    QVector<OrientationOps::Pointer> m_OrientationOps;

    NeighborList<int32_t>::WeakPointer m_NeighborList;
    NeighborList<float>::WeakPointer m_SharedSurfaceAreaList;

    StatsDataArray::Pointer m_StatsDataArray;

    QVector<DistributionAnalysisOps::Pointer> m_DistributionAnalysis;

    void dataCheck();

    GenerateEnsembleStatistics(const GenerateEnsembleStatistics&); // Copy Constructor Not Implemented
    void operator=(const GenerateEnsembleStatistics&); // Operator '=' Not Implemented
};

#endif /* GenerateEnsembleStatistics_H_ */
