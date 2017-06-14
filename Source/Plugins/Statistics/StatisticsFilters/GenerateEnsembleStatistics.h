/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#ifndef _generateensemblestatistics_h_
#define _generateensemblestatistics_h_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/NeighborList.hpp"
#include "SIMPLib/DataArrays/StatsDataArray.h"

#include "OrientationLib/LaueOps/LaueOps.h"

#include "Statistics/DistributionAnalysisOps/DistributionAnalysisOps.h"

/**
 * @brief The GenerateEnsembleStatistics class. See [Filter documentation](@ref generateensemblestatistics) for details.
 */
class GenerateEnsembleStatistics : public AbstractFilter
{
    Q_OBJECT
  public:
    SIMPL_SHARED_POINTERS(GenerateEnsembleStatistics)
    SIMPL_STATIC_NEW_MACRO(GenerateEnsembleStatistics)
    SIMPL_TYPE_MACRO_SUPER(GenerateEnsembleStatistics, AbstractFilter)

    virtual ~GenerateEnsembleStatistics();

    SIMPL_FILTER_PARAMETER(DataArrayPath, CellEnsembleAttributeMatrixPath)
    Q_PROPERTY(DataArrayPath CellEnsembleAttributeMatrixPath READ getCellEnsembleAttributeMatrixPath WRITE setCellEnsembleAttributeMatrixPath)

    SIMPL_FILTER_PARAMETER(QString, PhaseTypesArrayName)
    Q_PROPERTY(QString PhaseTypesArrayName READ getPhaseTypesArrayName WRITE setPhaseTypesArrayName)

    SIMPL_FILTER_PARAMETER(PhaseType::Types, PhaseTypeArray)
    Q_PROPERTY(PhaseType::Types PhaseTypeArray READ getPhaseTypeArray WRITE setPhaseTypeArray)

    SIMPL_INSTANCE_PROPERTY(PhaseType::Types, PhaseTypeData)
    Q_PROPERTY(PhaseType::Types PhaseTypeData READ getPhaseTypeData WRITE setPhaseTypeData)

    int getPhaseCount();
    Q_PROPERTY(int PhaseCount READ getPhaseCount)

    SIMPL_FILTER_PARAMETER(DataArrayPath, NeighborListArrayPath)
    Q_PROPERTY(DataArrayPath NeighborListArrayPath READ getNeighborListArrayPath WRITE setNeighborListArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, SharedSurfaceAreaListArrayPath)
    Q_PROPERTY(DataArrayPath SharedSurfaceAreaListArrayPath READ getSharedSurfaceAreaListArrayPath WRITE setSharedSurfaceAreaListArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
    Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, BiasedFeaturesArrayPath)
    Q_PROPERTY(DataArrayPath BiasedFeaturesArrayPath READ getBiasedFeaturesArrayPath WRITE setBiasedFeaturesArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, EquivalentDiametersArrayPath)
    Q_PROPERTY(DataArrayPath EquivalentDiametersArrayPath READ getEquivalentDiametersArrayPath WRITE setEquivalentDiametersArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, NeighborhoodsArrayPath)
    Q_PROPERTY(DataArrayPath NeighborhoodsArrayPath READ getNeighborhoodsArrayPath WRITE setNeighborhoodsArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, AspectRatiosArrayPath)
    Q_PROPERTY(DataArrayPath AspectRatiosArrayPath READ getAspectRatiosArrayPath WRITE setAspectRatiosArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, Omega3sArrayPath)
    Q_PROPERTY(DataArrayPath Omega3sArrayPath READ getOmega3sArrayPath WRITE setOmega3sArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, AxisEulerAnglesArrayPath)
    Q_PROPERTY(DataArrayPath AxisEulerAnglesArrayPath READ getAxisEulerAnglesArrayPath WRITE setAxisEulerAnglesArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
    Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, SurfaceFeaturesArrayPath)
    Q_PROPERTY(DataArrayPath SurfaceFeaturesArrayPath READ getSurfaceFeaturesArrayPath WRITE setSurfaceFeaturesArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, VolumesArrayPath)
    Q_PROPERTY(DataArrayPath VolumesArrayPath READ getVolumesArrayPath WRITE setVolumesArrayPath)

    // These filter parameters are tied to RDF data, but the filter that can calculate them (FindFeatureClustering)
    // is not slated for public release yet, so turning these off for now
    SIMPL_FILTER_PARAMETER(DataArrayPath, RDFArrayPath)
    Q_PROPERTY(DataArrayPath RDFArrayPath READ getRDFArrayPath WRITE setRDFArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, MaxMinRDFArrayPath)
    Q_PROPERTY(DataArrayPath MaxMinRDFArrayPath READ getMaxMinRDFArrayPath WRITE setMaxMinRDFArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureEulerAnglesArrayPath)
    Q_PROPERTY(DataArrayPath FeatureEulerAnglesArrayPath READ getFeatureEulerAnglesArrayPath WRITE setFeatureEulerAnglesArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, AvgQuatsArrayPath)
    Q_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)

    SIMPL_FILTER_PARAMETER(QString, StatisticsArrayName)
    Q_PROPERTY(QString StatisticsArrayName READ getStatisticsArrayName WRITE setStatisticsArrayName)

    SIMPL_FILTER_PARAMETER(bool, IncludeRadialDistFunc)
    Q_PROPERTY(bool IncludeRadialDistFunc READ getIncludeRadialDistFunc WRITE setIncludeRadialDistFunc)

    SIMPL_FILTER_PARAMETER(bool, CalculateMorphologicalStats)
    Q_PROPERTY(bool CalculateMorphologicalStats READ getCalculateMorphologicalStats WRITE setCalculateMorphologicalStats)

    SIMPL_INSTANCE_PROPERTY(bool, ComputeSizeDistribution)
    //Q_PROPERTY(bool ComputeSizeDistribution READ getComputeSizeDistribution WRITE setComputeSizeDistribution)

    SIMPL_FILTER_PARAMETER(int, SizeDistributionFitType)
    Q_PROPERTY(int SizeDistributionFitType READ getSizeDistributionFitType WRITE setSizeDistributionFitType)

    SIMPL_FILTER_PARAMETER(bool, ComputeAspectRatioDistribution)
    //Q_PROPERTY(bool ComputeAspectRatioDistribution READ getComputeAspectRatioDistribution WRITE setComputeAspectRatioDistribution)

    SIMPL_FILTER_PARAMETER(int, AspectRatioDistributionFitType)
    Q_PROPERTY(int AspectRatioDistributionFitType READ getAspectRatioDistributionFitType WRITE setAspectRatioDistributionFitType)

    SIMPL_FILTER_PARAMETER(bool, ComputeOmega3Distribution)
    //Q_PROPERTY(bool ComputeOmega3Distribution READ getComputeOmega3Distribution WRITE setComputeOmega3Distribution)

    SIMPL_FILTER_PARAMETER(int, Omega3DistributionFitType)
    Q_PROPERTY(int Omega3DistributionFitType READ getOmega3DistributionFitType WRITE setOmega3DistributionFitType)

    SIMPL_FILTER_PARAMETER(bool, ComputeNeighborhoodDistribution)
    //Q_PROPERTY(bool ComputeNeighborhoodDistribution READ getComputeNeighborhoodDistribution WRITE setComputeNeighborhoodDistribution)

    SIMPL_FILTER_PARAMETER(int, NeighborhoodDistributionFitType)
    Q_PROPERTY(int NeighborhoodDistributionFitType READ getNeighborhoodDistributionFitType WRITE setNeighborhoodDistributionFitType)

    SIMPL_FILTER_PARAMETER(bool, CalculateCrystallographicStats)
    Q_PROPERTY(bool CalculateCrystallographicStats READ getCalculateCrystallographicStats WRITE setCalculateCrystallographicStats)

    SIMPL_FILTER_PARAMETER(bool, CalculateODF)
    //Q_PROPERTY(bool CalculateODF READ getCalculateODF WRITE setCalculateODF)

    SIMPL_FILTER_PARAMETER(bool, CalculateMDF)
    //Q_PROPERTY(bool CalculateMDF READ getCalculateMDF WRITE setCalculateMDF)

    SIMPL_FILTER_PARAMETER(bool, CalculateAxisODF)
    //Q_PROPERTY(bool CalculateAxisODF READ getCalculateAxisODF WRITE setCalculateAxisODF)

    SIMPL_FILTER_PARAMETER(float, SizeCorrelationResolution)
    Q_PROPERTY(float SizeCorrelationResolution READ getSizeCorrelationResolution WRITE setSizeCorrelationResolution)

    /**
     * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getCompiledLibraryName();

    /**
     * @brief getBrandingString Returns the branding string for the filter, which is a tag
     * used to denote the filter's association with specific plugins
     * @return Branding string
    */
    virtual const QString getBrandingString();

    /**
     * @brief getFilterVersion Returns a version string for this filter. Default
     * value is an empty string.
     * @return
     */
    virtual const QString getFilterVersion();

    /**
     * @brief newFilterInstance Reimplemented from @see AbstractFilter class
     */
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);

    /**
     * @brief getGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getGroupName();

    /**
     * @brief getSubGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getSubGroupName();

    /**
     * @brief getHumanLabel Reimplemented from @see AbstractFilter class
     */
    virtual const QString getHumanLabel();

    /**
     * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void setupFilterParameters();

    /**
     * @brief readFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    /**
    * @brief readFilterParametersFromJson Reads the filter parameters from a file
    * @param reader Reader that is used to read the parameters from a file
    */
    virtual void readFilterParameters(QJsonObject &obj);

    /**
    * @brief writeFilterParametersToJson Writes the filter parameters to a file
    * @param root The root json object
    */
    virtual void writeFilterParameters(QJsonObject &obj);

    /**
     * @brief execute Reimplemented from @see AbstractFilter class
     */
    virtual void execute();

    /**
    * @brief preflight Reimplemented from @see AbstractFilter class
    */
    virtual void preflight();

  signals:
    /**
     * @brief updateFilterParameters Emitted when the Filter requests all the latest Filter parameters
     * be pushed from a user-facing control (such as a widget)
     * @param filter Filter instance pointer
     */
    void updateFilterParameters(AbstractFilter* filter);

    /**
     * @brief parametersChanged Emitted when any Filter parameter is changed internally
     */
    void parametersChanged();

    /**
     * @brief preflightAboutToExecute Emitted just before calling dataCheck()
     */
    void preflightAboutToExecute();

    /**
     * @brief preflightExecuted Emitted just after calling dataCheck()
     */
    void preflightExecuted();

  protected:
    GenerateEnsembleStatistics();
    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

    /**
     * @brief Initializes all the private instance variables.
     */
    void initialize();


    /**
     * @brief gatherSizeStats Consolidates Feature size statistics
     */
    void gatherSizeStats();

    /**
     * @brief gatherAspectRatioStats Consolidates Feature aspect ratio statistics
     */
    void gatherAspectRatioStats();

    /**
     * @brief gatherOmega3Stats Consolidates Feature Omega3 statistics
     */
    void gatherOmega3Stats();

    /**
     * @brief gatherNeighborhoodStats Consolidates Feature neighborhood statistics
     */
    void gatherNeighborhoodStats();

    /**
     * @brief gatherMDFStats Consolidates Feature MDF statistics
     */
    void gatherMDFStats();

    /**
     * @brief gatherODFStats Consolidates Feature ODF statistics
     */
    void gatherODFStats();

    /**
     * @brief gatherAxisODFStats Consolidates Feature axis ODF statistics
     */
    void gatherAxisODFStats();

    /**
     * @brief gatherRadialDistFunc Consolidates Feature RDF statistics
     */
    void gatherRadialDistFunc();

    /**
     * @brief calculatePPTBoundaryFrac Consolidates Feature precipitate boundary fraction statistics
     */
    void calculatePPTBoundaryFrac();

  private:
    DEFINE_DATAARRAY_VARIABLE(float, AvgQuats)
    DEFINE_DATAARRAY_VARIABLE(float, FeatureEulerAngles)
    DEFINE_DATAARRAY_VARIABLE(float, Volumes)
    DEFINE_DATAARRAY_VARIABLE(bool, BiasedFeatures)
    DEFINE_DATAARRAY_VARIABLE(bool, SurfaceFeatures)
    DEFINE_DATAARRAY_VARIABLE(int32_t, FeaturePhases)
    DEFINE_DATAARRAY_VARIABLE(float, AxisEulerAngles)
    DEFINE_DATAARRAY_VARIABLE(float, RadialDistFunc)
    DEFINE_DATAARRAY_VARIABLE(float, MaxMinRadialDistFunc)
    DEFINE_DATAARRAY_VARIABLE(float, Omega3s)
    DEFINE_DATAARRAY_VARIABLE(float, AspectRatios)
    DEFINE_DATAARRAY_VARIABLE(float, EquivalentDiameters)
    DEFINE_DATAARRAY_VARIABLE(int32_t, Neighborhoods)
    DEFINE_DATAARRAY_VARIABLE(unsigned int, CrystalStructures)
    DEFINE_DATAARRAY_VARIABLE(PhaseType::EnumType, PhaseTypes)

    QVector<LaueOps::Pointer> m_OrientationOps;

    NeighborList<int32_t>::WeakPointer m_NeighborList;
    NeighborList<float>::WeakPointer m_SharedSurfaceAreaList;

    StatsDataArray::Pointer m_StatsDataArray;

    QVector<DistributionAnalysisOps::Pointer> m_DistributionAnalysis;

    GenerateEnsembleStatistics(const GenerateEnsembleStatistics&); // Copy Constructor Not Implemented
    void operator=(const GenerateEnsembleStatistics&); // Operator '=' Not Implemented
};

#endif /* GenerateEnsembleStatistics_H_ */
