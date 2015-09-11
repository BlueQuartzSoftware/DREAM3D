/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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


#include "GenerateEnsembleStatistics.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/PhaseType.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/PhaseTypeSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"

#include "SIMPLib/StatsData/PrimaryStatsData.h"
#include "SIMPLib/StatsData/PrecipitateStatsData.h"
#include "SIMPLib/StatsData/TransformationStatsData.h"
#include "SIMPLib/StatsData/BoundaryStatsData.h"
#include "SIMPLib/StatsData/MatrixStatsData.h"

#include "OrientationLib/OrientationMath/OrientationMath.h"
#include "OrientationLib/OrientationMath/OrientationArray.hpp"
#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"

#include "Statistics/DistributionAnalysisOps/BetaOps.h"
#include "Statistics/DistributionAnalysisOps/PowerLawOps.h"
#include "Statistics/DistributionAnalysisOps/LogNormalOps.h"

#include "Statistics/StatisticsConstants.h"

//FIXME: #1 Need to update this to link the phase selectionwidget to the rest of the GUI, so that it preflights after it's updated.
//FIXME: #2 Need to fix phase selectionWidget to not show phase 0
//FIXME: #3 Need to link phase selectionWidget to option to include Radial Distribution Function instead of an extra linkedProps boolean.
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateEnsembleStatistics::GenerateEnsembleStatistics()  :
  AbstractFilter(),
  m_CellEnsembleAttributeMatrixPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, ""),
  m_PhaseTypesArrayName(DREAM3D::EnsembleData::PhaseTypes),
  m_NeighborListArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::FeatureAttributeMatrixName, DREAM3D::FeatureData::NeighborList),
  m_SharedSurfaceAreaListArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::FeatureAttributeMatrixName, DREAM3D::FeatureData::SharedSurfaceAreaList),
  m_FeaturePhasesArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::FeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_BiasedFeaturesArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::FeatureAttributeMatrixName, DREAM3D::FeatureData::BiasedFeatures),
  m_EquivalentDiametersArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::FeatureAttributeMatrixName, DREAM3D::FeatureData::EquivalentDiameters),
  m_NeighborhoodsArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::FeatureAttributeMatrixName, DREAM3D::FeatureData::Neighborhoods),
  m_AspectRatiosArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::FeatureAttributeMatrixName, DREAM3D::FeatureData::AspectRatios),
  m_Omega3sArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::FeatureAttributeMatrixName, DREAM3D::FeatureData::Omega3s),
  m_AxisEulerAnglesArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::FeatureAttributeMatrixName, DREAM3D::FeatureData::AxisEulerAngles),
  m_CrystalStructuresArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_SurfaceFeaturesArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::FeatureAttributeMatrixName, DREAM3D::FeatureData::SurfaceFeatures),
  m_VolumesArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::FeatureAttributeMatrixName, DREAM3D::FeatureData::Volumes),
  m_FeatureEulerAnglesArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::FeatureAttributeMatrixName, DREAM3D::FeatureData::EulerAngles),
  m_AvgQuatsArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::FeatureAttributeMatrixName, DREAM3D::FeatureData::AvgQuats),
  m_StatisticsArrayName(DREAM3D::EnsembleData::Statistics),
  m_CalculateMorphologicalStats(true),
  m_ComputeSizeDistribution(false),
  m_SizeDistributionFitType(DREAM3D::DistributionType::LogNormal),
  m_ComputeAspectRatioDistribution(false),
  m_AspectRatioDistributionFitType(DREAM3D::DistributionType::Beta),
  m_ComputeOmega3Distribution(false),
  m_Omega3DistributionFitType(DREAM3D::DistributionType::Beta),
  m_ComputeNeighborhoodDistribution(false),
  m_NeighborhoodDistributionFitType(DREAM3D::DistributionType::LogNormal),
  m_CalculateCrystallographicStats(true),
  m_CalculateODF(false),
  m_CalculateMDF(false),
  m_CalculateAxisODF(false),
  m_SizeCorrelationResolution(1),
  m_AvgQuats(NULL),
  m_FeatureEulerAngles(NULL),
  m_Volumes(NULL),
  m_BiasedFeatures(NULL),
  m_SurfaceFeatures(NULL),
  m_FeaturePhases(NULL),
  m_AxisEulerAngles(NULL),
  m_RadialDistFunc(NULL),
  m_Omega3s(NULL),
  m_AspectRatios(NULL),
  m_EquivalentDiameters(NULL),
  m_Neighborhoods(NULL),
  m_CrystalStructures(NULL),
  m_PhaseTypes(NULL)
{
  m_DistributionAnalysis.push_back(BetaOps::New());
  m_DistributionAnalysis.push_back(LogNormalOps::New());
  m_DistributionAnalysis.push_back(PowerLawOps::New());

  m_OrientationOps = SpaceGroupOps::getOrientationOpsQVector();

  m_NeighborList = NeighborList<int32_t>::NullPointer();
  m_SharedSurfaceAreaList = NeighborList<float>::NullPointer();

  m_StatsDataArray = StatsDataArray::NullPointer();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateEnsembleStatistics::~GenerateEnsembleStatistics()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateEnsembleStatistics::setupFilterParameters()
{
  QVector<QString> choices;
  choices.push_back("Beta");
  choices.push_back("Lognormal");
  choices.push_back("Power");
  FilterParameterVector parameters;
  QStringList phaseTypeStrings;
  PhaseType::getPhaseTypeStrings(phaseTypeStrings);
  PhaseTypeSelectionFilterParameter::Pointer phaseType_parameter = PhaseTypeSelectionFilterParameter::New(
        "Phase Types",
        "PhaseTypesArrayName",
        "PhaseCount",
        "PhaseTypeData",
        "CellEnsembleAttributeMatrixPath",
        getCellEnsembleAttributeMatrixPath(),
        phaseTypeStrings,
        FilterParameter::Parameter);
  parameters.push_back(phaseType_parameter);
  parameters.push_back(DoubleFilterParameter::New("Size Correlation Resolution", "SizeCorrelationResolution", getSizeCorrelationResolution(), FilterParameter::Parameter));
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::Int32, 1, DREAM3D::AttributeMatrixObjectType::Feature);
    parameters.push_back(DataArraySelectionFilterParameter::New("Feature Phases", "FeaturePhasesArrayPath", getFeaturePhasesArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::NeighborList, 1, DREAM3D::AttributeMatrixObjectType::Feature);
    parameters.push_back(DataArraySelectionFilterParameter::New("Neighbor List", "NeighborListArrayPath", getNeighborListArrayPath(), FilterParameter::RequiredArray, req));
  }

  QStringList linkedProps;

  linkedProps.clear();
  linkedProps << "SizeDistributionFitType" << "BiasedFeaturesArrayPath" << "EquivalentDiametersArrayPath";
  linkedProps << "AspectRatioDistributionFitType" << "AspectRatiosArrayPath";
  linkedProps << "Omega3DistributionFitType" << "Omega3sArrayPath";
  linkedProps << "NeighborhoodDistributionFitType" << "NeighborhoodsArrayPath";
  linkedProps << "CalculateAxisODF" << "AxisEulerAnglesArrayPath";
  parameters.push_back(LinkedBooleanFilterParameter::New("Calculate Morphological Statistics", "CalculateMorphologicalStats", getCalculateMorphologicalStats(), linkedProps, FilterParameter::Parameter));
  parameters.push_back(ChoiceFilterParameter::New("Size Distribution Fit Type", "SizeDistributionFitType", getSizeDistributionFitType(), choices, false, FilterParameter::Parameter));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::Bool, 1, DREAM3D::AttributeMatrixObjectType::Feature);
    parameters.push_back(DataArraySelectionFilterParameter::New("Biased Features", "BiasedFeaturesArrayPath", getBiasedFeaturesArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::Float, 1, DREAM3D::AttributeMatrixObjectType::Feature);
    parameters.push_back(DataArraySelectionFilterParameter::New("Equivalent Diameters", "EquivalentDiametersArrayPath", getEquivalentDiametersArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(ChoiceFilterParameter::New("Aspect Ratio Distribution Fit Type", "AspectRatioDistributionFitType", getAspectRatioDistributionFitType(), choices, false, FilterParameter::Parameter));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::Float, 2, DREAM3D::AttributeMatrixObjectType::Feature);
    parameters.push_back(DataArraySelectionFilterParameter::New("Aspect Ratios", "AspectRatiosArrayPath", getAspectRatiosArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(ChoiceFilterParameter::New("Omega3 Distribution Fit Type", "Omega3DistributionFitType", getOmega3DistributionFitType(), choices, false, FilterParameter::Parameter));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::Float, 1, DREAM3D::AttributeMatrixObjectType::Feature);
    parameters.push_back(DataArraySelectionFilterParameter::New("Omega3s", "Omega3sArrayPath", getOmega3sArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(ChoiceFilterParameter::New("Neighborhood Distribution Fit Type", "NeighborhoodDistributionFitType", getNeighborhoodDistributionFitType(), choices, false, FilterParameter::Parameter));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::Int32, 1, DREAM3D::AttributeMatrixObjectType::Feature);
    parameters.push_back(DataArraySelectionFilterParameter::New("Neighborhoods", "NeighborhoodsArrayPath", getNeighborhoodsArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::Float, 3, DREAM3D::AttributeMatrixObjectType::Feature);
    parameters.push_back(DataArraySelectionFilterParameter::New("Axis Euler Angles", "AxisEulerAnglesArrayPath", getAxisEulerAnglesArrayPath(), FilterParameter::RequiredArray, req));
  }

  linkedProps.clear();
  linkedProps << "CrystalStructuresArrayPath" << "SurfaceFeaturesArrayPath";
  linkedProps << "CalculateODF" << "VolumesArrayPath" << "FeatureEulerAnglesArrayPath";
  linkedProps << "CalculateMDF" << "SharedSurfaceAreaListArrayPath" << "AvgQuatsArrayPath";
  parameters.push_back(LinkedBooleanFilterParameter::New("Calculate Crystallographic Statistics", "CalculateCrystallographicStats", getCalculateCrystallographicStats(), linkedProps, FilterParameter::Parameter));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::Bool, 1, DREAM3D::AttributeMatrixObjectType::Feature);
    parameters.push_back(DataArraySelectionFilterParameter::New("Surface Features", "SurfaceFeaturesArrayPath", getSurfaceFeaturesArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::Float, 1, DREAM3D::AttributeMatrixObjectType::Feature);
    parameters.push_back(DataArraySelectionFilterParameter::New("Volumes", "VolumesArrayPath", getVolumesArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::Float, 3, DREAM3D::AttributeMatrixObjectType::Feature);
    parameters.push_back(DataArraySelectionFilterParameter::New("Average Euler Angles", "FeatureEulerAnglesArrayPath", getFeatureEulerAnglesArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::Float, 4, DREAM3D::AttributeMatrixObjectType::Feature);
    parameters.push_back(DataArraySelectionFilterParameter::New("Average Quaternions", "AvgQuatsArrayPath", getAvgQuatsArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::NeighborList, 1, DREAM3D::AttributeMatrixObjectType::Feature);
    parameters.push_back(DataArraySelectionFilterParameter::New("Shared Surface Area List", "SharedSurfaceAreaListArrayPath", getSharedSurfaceAreaListArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::UInt32, 1, DREAM3D::AttributeMatrixObjectType::Ensemble);
    parameters.push_back(DataArraySelectionFilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", getCrystalStructuresArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::CreatedArray));
  // The user types in an array name for the Phase Types
  parameters.push_back(StringFilterParameter::New("Phase Types", "PhaseTypesArrayName", getPhaseTypesArrayName(), FilterParameter::CreatedArray));
  // The user types in an array name for Statistics
  parameters.push_back(StringFilterParameter::New("Statistics", "StatisticsArrayName", getStatisticsArrayName(), FilterParameter::CreatedArray));
  linkedProps << "RDFArrayPath" << "MaxMinRDFArrayPath";
  parameters.push_back(LinkedBooleanFilterParameter::New("Include Radial Distribution Function", "IncludeRadialDistFunc", getIncludeRadialDistFunc(), linkedProps, FilterParameter::Parameter));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::Float, 1, DREAM3D::AttributeMatrixObjectType::Ensemble);
    parameters.push_back(DataArraySelectionFilterParameter::New("Radial Distribution Function", "RDFArrayPath", getRDFArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::Float, 2, DREAM3D::AttributeMatrixObjectType::Ensemble);
    parameters.push_back(DataArraySelectionFilterParameter::New("Max and Min Separation Distances", "MaxMinRDFArrayPath", getMaxMinRDFArrayPath(), FilterParameter::RequiredArray, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateEnsembleStatistics::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellEnsembleAttributeMatrixPath(reader->readDataArrayPath("CellEnsembleAttributeMatrixPath", getCellEnsembleAttributeMatrixPath()));
  setCalculateMorphologicalStats(reader->readValue("CalculateMorphologicalStats", getCalculateMorphologicalStats()));
  setIncludeRadialDistFunc(reader->readValue("IncludeRadialDistFunc", getIncludeRadialDistFunc()));
  setCellEnsembleAttributeMatrixPath(reader->readDataArrayPath("CellEnsembleAttributeMatrixPath", getCellEnsembleAttributeMatrixPath()));
  setPhaseTypesArrayName(reader->readString("PhaseTypesArrayName", getPhaseTypesArrayName() ) );
  setNeighborListArrayPath(reader->readDataArrayPath("NeighborListArrayPath", getNeighborListArrayPath()));
  setStatisticsArrayName(reader->readString("StatisticsArrayName", getStatisticsArrayName() ) );
  setAvgQuatsArrayPath(reader->readDataArrayPath("AvgQuatsArrayPath", getAvgQuatsArrayPath() ) );
  setFeatureEulerAnglesArrayPath(reader->readDataArrayPath("FeatureEulerAnglesArrayPath", getFeatureEulerAnglesArrayPath() ) );
  setVolumesArrayPath(reader->readDataArrayPath("VolumesArrayPath", getVolumesArrayPath() ) );
  setSurfaceFeaturesArrayPath(reader->readDataArrayPath("SurfaceFeaturesArrayPath", getSurfaceFeaturesArrayPath() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setAxisEulerAnglesArrayPath(reader->readDataArrayPath("AxisEulerAnglesArrayPath", getAxisEulerAnglesArrayPath() ) );
  setOmega3sArrayPath(reader->readDataArrayPath("Omega3sArrayPath", getOmega3sArrayPath() ) );
  setRDFArrayPath(reader->readDataArrayPath("RDFArrayPath", getRDFArrayPath()));
  setMaxMinRDFArrayPath(reader->readDataArrayPath("MaxMinRDFArrayPath", getMaxMinRDFArrayPath()));
  setAspectRatiosArrayPath(reader->readDataArrayPath("AspectRatiosArrayPath", getAspectRatiosArrayPath() ) );
  setNeighborhoodsArrayPath(reader->readDataArrayPath("NeighborhoodsArrayPath", getNeighborhoodsArrayPath() ) );
  setSharedSurfaceAreaListArrayPath(reader->readDataArrayPath("SharedSurfaceAreaListArrayPath", getSharedSurfaceAreaListArrayPath()));
  setEquivalentDiametersArrayPath(reader->readDataArrayPath("EquivalentDiametersArrayPath", getEquivalentDiametersArrayPath() ) );
  setBiasedFeaturesArrayPath(reader->readDataArrayPath("BiasedFeaturesArrayPath", getBiasedFeaturesArrayPath() ) );
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() ) );
  setComputeSizeDistribution( reader->readValue("ComputeSizeDistribution", getComputeSizeDistribution() ) );
  setSizeDistributionFitType( reader->readValue("SizeDistributionFitType", getSizeDistributionFitType() ) );
  setComputeAspectRatioDistribution( reader->readValue("ComputeAspectRatioDistribution", getComputeAspectRatioDistribution() ) );
  setAspectRatioDistributionFitType( reader->readValue("AspectRatioDistributionFitType", getAspectRatioDistributionFitType() ) );
  setComputeOmega3Distribution( reader->readValue("ComputeOmega3Distribution", getComputeOmega3Distribution() ) );
  setOmega3DistributionFitType( reader->readValue("Omega3DistributionFitType", getOmega3DistributionFitType() ) );
  setComputeNeighborhoodDistribution( reader->readValue("ComputeNeighborhoodDistribution", getComputeNeighborhoodDistribution() ) );
  setNeighborhoodDistributionFitType( reader->readValue("NeighborhoodDistributionFitType", getNeighborhoodDistributionFitType() ) );
  setCalculateCrystallographicStats( reader->readValue("CalculateCrystallographicStats", getCalculateCrystallographicStats() ) );
  setCalculateODF( reader->readValue("CalculateODF", getCalculateODF() ) );
  setCalculateMDF( reader->readValue("CalculateMDF", getCalculateMDF() ) );
  setCalculateAxisODF( reader->readValue("CalculateAxisODF", getCalculateAxisODF() ) );
  setSizeCorrelationResolution( reader->readValue("SizeCorrelationResolution", getSizeCorrelationResolution() ) );

  QVector<uint32_t> data = getPhaseTypeData().d;
  data = reader->readArray("PhaseTypeArray", data);
  UInt32Vector_t vec;
  vec.d = data;
  setPhaseTypeData(vec);

  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GenerateEnsembleStatistics::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(CalculateMorphologicalStats)
  SIMPL_FILTER_WRITE_PARAMETER(IncludeRadialDistFunc)
  SIMPL_FILTER_WRITE_PARAMETER(CellEnsembleAttributeMatrixPath)
  SIMPL_FILTER_WRITE_PARAMETER(PhaseTypesArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(StatisticsArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(AvgQuatsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(FeatureEulerAnglesArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(VolumesArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceFeaturesArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(AxisEulerAnglesArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(Omega3sArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(RDFArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(MaxMinRDFArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(AspectRatiosArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(NeighborhoodsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(NeighborListArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(EquivalentDiametersArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(BiasedFeaturesArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(FeaturePhasesArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(SharedSurfaceAreaListArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(ComputeSizeDistribution)
  SIMPL_FILTER_WRITE_PARAMETER(SizeDistributionFitType)
  SIMPL_FILTER_WRITE_PARAMETER(ComputeAspectRatioDistribution)
  SIMPL_FILTER_WRITE_PARAMETER(AspectRatioDistributionFitType)
  SIMPL_FILTER_WRITE_PARAMETER(ComputeOmega3Distribution)
  SIMPL_FILTER_WRITE_PARAMETER(Omega3DistributionFitType)
  SIMPL_FILTER_WRITE_PARAMETER(ComputeNeighborhoodDistribution)
  SIMPL_FILTER_WRITE_PARAMETER(NeighborhoodDistributionFitType)
  SIMPL_FILTER_WRITE_PARAMETER(CalculateCrystallographicStats)
  SIMPL_FILTER_WRITE_PARAMETER(CalculateODF)
  SIMPL_FILTER_WRITE_PARAMETER(CalculateMDF)
  SIMPL_FILTER_WRITE_PARAMETER(CalculateAxisODF)
  SIMPL_FILTER_WRITE_PARAMETER(SizeCorrelationResolution)
  writer->writeValue("PhaseTypeArray", getPhaseTypeData().d );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateEnsembleStatistics::dataCheck()
{
  setErrorCondition(0);

  DataArrayPath tempPath;

  QVector<size_t> cDims(1, 1);

  setComputeSizeDistribution(getCalculateMorphologicalStats());
  setComputeAspectRatioDistribution(getCalculateMorphologicalStats());
  setComputeOmega3Distribution(getCalculateMorphologicalStats());
  setComputeNeighborhoodDistribution(getCalculateMorphologicalStats());
  setCalculateAxisODF(getCalculateMorphologicalStats());
  setCalculateODF(getCalculateCrystallographicStats());
  setCalculateMDF(getCalculateCrystallographicStats());

  QVector<DataArrayPath> dataArrayPaths;

  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getFeaturePhasesArrayPath()); }

  if (m_ComputeSizeDistribution == true || m_ComputeOmega3Distribution == true
      || m_ComputeAspectRatioDistribution == true || m_ComputeNeighborhoodDistribution == true || m_CalculateAxisODF == true)
  {
    m_BiasedFeaturesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getBiasedFeaturesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_BiasedFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_BiasedFeatures = m_BiasedFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getBiasedFeaturesArrayPath()); }

    m_EquivalentDiametersPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getEquivalentDiametersArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_EquivalentDiametersPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_EquivalentDiameters = m_EquivalentDiametersPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getEquivalentDiametersArrayPath()); }
  }

  if (m_ComputeNeighborhoodDistribution == true)
  {
    m_NeighborhoodsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getNeighborhoodsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_NeighborhoodsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_Neighborhoods = m_NeighborhoodsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getNeighborhoodsArrayPath()); }
  }

  if (m_ComputeAspectRatioDistribution == true)
  {
    cDims[0] = 2;
    m_AspectRatiosPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAspectRatiosArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_AspectRatiosPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_AspectRatios = m_AspectRatiosPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getAspectRatiosArrayPath()); }
  }

  if (m_ComputeOmega3Distribution == true)
  {
    cDims[0] = 1;
    m_Omega3sPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getOmega3sArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_Omega3sPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_Omega3s = m_Omega3sPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getOmega3sArrayPath()); }
  }

  if (m_CalculateAxisODF == true)
  {
    cDims[0] = 3;
    m_AxisEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAxisEulerAnglesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_AxisEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_AxisEulerAngles = m_AxisEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getAxisEulerAnglesArrayPath()); }
  }

  if (m_IncludeRadialDistFunc == true)
  {
    cDims[0] = 1;
    DataArray<float>::Pointer tempPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getRDFArrayPath(), cDims);
    if (NULL != tempPtr.get())
    {
      m_RadialDistFuncPtr = tempPtr;
      m_RadialDistFunc = tempPtr->getPointer(0);
    }

    cDims[0] = 2;
    m_MaxMinRadialDistFuncPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getMaxMinRDFArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_MaxMinRadialDistFuncPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_MaxMinRadialDistFunc = m_MaxMinRadialDistFuncPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  if (m_CalculateODF == true || m_CalculateMDF == true)
  {
    cDims[0] = 1;
    m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */


    m_SurfaceFeaturesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getSurfaceFeaturesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceFeatures = m_SurfaceFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getSurfaceFeaturesArrayPath()); }
  }

  if (m_CalculateODF == true)
  {
    cDims[0] = 1;
    m_VolumesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getVolumesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_VolumesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_Volumes = m_VolumesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getVolumesArrayPath()); }

    cDims[0] = 3;
    m_FeatureEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getFeatureEulerAnglesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_FeatureEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getFeatureEulerAnglesArrayPath()); }
  }

  if (m_CalculateMDF == true)
  {
    cDims[0] = 4;
    m_AvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgQuatsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getAvgQuatsArrayPath()); }

    // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
    cDims[0] = 1;
    m_SharedSurfaceAreaList = getDataContainerArray()->getPrereqArrayFromPath<NeighborList<float>, AbstractFilter>(this, getSharedSurfaceAreaListArrayPath(), cDims);
    if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getSharedSurfaceAreaListArrayPath()); }
  }

  cDims[0] = 1;
  m_NeighborList = getDataContainerArray()->getPrereqArrayFromPath<NeighborList<int>, AbstractFilter>(this, getNeighborListArrayPath(), cDims);
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getNeighborListArrayPath()); }

  if (m_PhaseTypeData.d.size() == 0)
  {
    setErrorCondition(-1000);
    notifyErrorMessage(getHumanLabel(), "The phase type array must contain at least one member. An Ensemble Attribute Matrix must be selected", getErrorCondition());
    return;
  }
  else if (m_PhaseTypeData.d.size() == 1 && m_PhaseTypeData.d[0] ==  DREAM3D::PhaseType::UnknownPhaseType)
  {
    setErrorCondition(-1001);
    notifyErrorMessage(getHumanLabel(), "The phase type array must contain at least one member. An Ensemble Attribute Matrix must be selected", getErrorCondition());
    return;
  }
  else
  {
    cDims[0] = 1;
    tempPath.update(getCellEnsembleAttributeMatrixPath().getDataContainerName(), getCellEnsembleAttributeMatrixPath().getAttributeMatrixName(), getPhaseTypesArrayName() );
    m_PhaseTypesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(this, tempPath, DREAM3D::PhaseType::UnknownPhaseType, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_PhaseTypesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    {
      m_PhaseTypes = m_PhaseTypesPtr.lock()->getPointer(0);
      m_PhaseTypeData.d.resize(m_PhaseTypesPtr.lock()->getNumberOfTuples());
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  // now create and add the stats array itself
  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getCellEnsembleAttributeMatrixPath().getDataContainerName());
  if(getErrorCondition() < 0 || m == NULL) { return; }
  AttributeMatrix::Pointer attrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixPath().getAttributeMatrixName(), -301);
  if(getErrorCondition() < 0 || attrMat == NULL) { return; }

  m_StatsDataArray = StatsDataArray::CreateArray(m_PhaseTypesPtr.lock()->getNumberOfTuples(), getStatisticsArrayName());
  m_StatsDataArray->fillArrayWithNewStatsData(m_PhaseTypesPtr.lock()->getNumberOfTuples(), m_PhaseTypes);
  attrMat->addAttributeArray(getStatisticsArrayName(), m_StatsDataArray);

  if (m_SizeDistributionFitType != DREAM3D::DistributionType::LogNormal)
  {
    notifyWarningMessage(getHumanLabel(), "The size distribution needs to be a lognormal distribution otherwise unpredictable results may occur", -1000);
  }
  if (m_AspectRatioDistributionFitType != DREAM3D::DistributionType::Beta)
  {
    notifyWarningMessage(getHumanLabel(), "The aspect ratio distribution needs to be a beta distribution otherwise unpredictable results may occur", -1000);
  }
  if (m_Omega3DistributionFitType != DREAM3D::DistributionType::Beta)
  {
    notifyWarningMessage(getHumanLabel(), "The Omega3 distribution needs to be a beta distribution otherwise unpredictable results may occur", -1000);
  }
  if (m_NeighborhoodDistributionFitType != DREAM3D::DistributionType::LogNormal)
  {
    notifyWarningMessage(getHumanLabel(), "The neighborhood distribution type needs to be a lognormal distribution otherwise unpredictable results may occur", -1000);
  }

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateEnsembleStatistics::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateEnsembleStatistics::gatherSizeStats()
{
  StatsDataArray& statsDataArray = *(m_StatsDataArray);

  float maxdiam = 0.0f;
  float mindiam = 0.0f;
  float totalUnbiasedVolume = 0.0f;
  QVector<VectorOfFloatArray> sizedist;
  std::vector<std::vector<std::vector<float> > > values;

  FloatArrayType::Pointer binnumbers;
  size_t numfeatures = m_EquivalentDiametersPtr.lock()->getNumberOfTuples();
  size_t numensembles = m_PhaseTypesPtr.lock()->getNumberOfTuples();

  std::vector<float> fractions(numensembles, 0.0f);
  sizedist.resize(numensembles);
  values.resize(numensembles);

  for (size_t i = 1; i < numensembles; i++)
  {
    sizedist[i] = statsDataArray[i]->CreateCorrelatedDistributionArrays(m_SizeDistributionFitType, 1);
    values[i].resize(1);
  }

  float vol = 0.0f;
  for (size_t i = 1; i < numfeatures; i++)
  {
    if (m_BiasedFeatures[i] == false)
    {
      values[m_FeaturePhases[i]][0].push_back(m_EquivalentDiameters[i]);
    }
    vol = (1.0f / 6.0f) * SIMPLib::Constants::k_Pi * m_EquivalentDiameters[i] * m_EquivalentDiameters[i] * m_EquivalentDiameters[i];
    fractions[m_FeaturePhases[i]] = fractions[m_FeaturePhases[i]] + vol;
    totalUnbiasedVolume = totalUnbiasedVolume + vol;
  }
  for (size_t i = 1; i < numensembles; i++)
  {
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::MatrixPhase)
    {
      MatrixStatsData* pp = MatrixStatsData::SafePointerDownCast(statsDataArray[i].get());
      pp->setPhaseFraction((fractions[i] / totalUnbiasedVolume));
    }
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      pp->setPhaseFraction((fractions[i] / totalUnbiasedVolume));
      m_DistributionAnalysis[m_SizeDistributionFitType]->calculateCorrelatedParameters(values[i], sizedist[i]);
      pp->setFeatureSizeDistribution(sizedist[i]);
      DistributionAnalysisOps::determineMaxAndMinValues(values[i][0], maxdiam, mindiam);
      int32_t numbins = int32_t(maxdiam / m_SizeCorrelationResolution) + 1;
      pp->setFeatureDiameterInfo(m_SizeCorrelationResolution, maxdiam, mindiam);
      binnumbers = FloatArrayType::CreateArray(numbins, DREAM3D::StringConstants::BinNumber);
      DistributionAnalysisOps::determineBinNumbers(maxdiam, mindiam, m_SizeCorrelationResolution, binnumbers);
      pp->setBinNumbers(binnumbers);
    }
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      pp->setPhaseFraction((fractions[i] / totalUnbiasedVolume));
      m_DistributionAnalysis[m_SizeDistributionFitType]->calculateCorrelatedParameters(values[i], sizedist[i]);
      pp->setFeatureSizeDistribution(sizedist[i]);
      DistributionAnalysisOps::determineMaxAndMinValues(values[i][0], maxdiam, mindiam);
      int32_t numbins = int32_t(maxdiam / m_SizeCorrelationResolution) + 1;
      pp->setFeatureDiameterInfo(m_SizeCorrelationResolution, maxdiam, mindiam);
      binnumbers = FloatArrayType::CreateArray(numbins, DREAM3D::StringConstants::BinNumber);
      DistributionAnalysisOps::determineBinNumbers(maxdiam, mindiam, m_SizeCorrelationResolution, binnumbers);
      pp->setBinNumbers(binnumbers);
    }
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
      tp->setPhaseFraction((fractions[i] / totalUnbiasedVolume));
      m_DistributionAnalysis[m_SizeDistributionFitType]->calculateCorrelatedParameters(values[i], sizedist[i]);
      tp->setFeatureSizeDistribution(sizedist[i]);
      DistributionAnalysisOps::determineMaxAndMinValues(values[i][0], maxdiam, mindiam);
      int numbins = int(maxdiam / m_SizeCorrelationResolution) + 1;
      tp->setFeatureDiameterInfo(m_SizeCorrelationResolution, maxdiam, mindiam);
      binnumbers = FloatArrayType::CreateArray(numbins, DREAM3D::StringConstants::BinNumber);
      DistributionAnalysisOps::determineBinNumbers(maxdiam, mindiam, m_SizeCorrelationResolution, binnumbers);
      tp->setBinNumbers(binnumbers);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateEnsembleStatistics::gatherAspectRatioStats()
{
  StatsDataArray& statsDataArray = *(m_StatsDataArray);

  size_t bin = 0;
  QVector<VectorOfFloatArray> boveras;
  QVector<VectorOfFloatArray> coveras;
  std::vector<std::vector<std::vector<float> > > bvalues;
  std::vector<std::vector<std::vector<float> > > cvalues;
  std::vector<float> mindiams;
  std::vector<float> binsteps;
  size_t numfeatures = m_AspectRatiosPtr.lock()->getNumberOfTuples();
  size_t numensembles = m_PhaseTypesPtr.lock()->getNumberOfTuples();

  boveras.resize(numensembles);
  coveras.resize(numensembles);
  bvalues.resize(numensembles);
  cvalues.resize(numensembles);
  mindiams.resize(numensembles);
  binsteps.resize(numensembles);
  for (size_t i = 1; i < numensembles; i++)
  {
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      boveras[i] = pp->CreateCorrelatedDistributionArrays(m_AspectRatioDistributionFitType, pp->getBinNumbers()->getSize());
      coveras[i] = pp->CreateCorrelatedDistributionArrays(m_AspectRatioDistributionFitType, pp->getBinNumbers()->getSize());
      bvalues[i].resize(pp->getBinNumbers()->getSize());
      cvalues[i].resize(pp->getBinNumbers()->getSize());
      mindiams[i] = pp->getMinFeatureDiameter();
      binsteps[i] = pp->getBinStepSize();
    }
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      boveras[i] = pp->CreateCorrelatedDistributionArrays(m_AspectRatioDistributionFitType, pp->getBinNumbers()->getSize());
      coveras[i] = pp->CreateCorrelatedDistributionArrays(m_AspectRatioDistributionFitType, pp->getBinNumbers()->getSize());
      bvalues[i].resize(pp->getBinNumbers()->getSize());
      cvalues[i].resize(pp->getBinNumbers()->getSize());
      mindiams[i] = pp->getMinFeatureDiameter();
      binsteps[i] = pp->getBinStepSize();
    }
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
      boveras[i] = tp->CreateCorrelatedDistributionArrays(m_AspectRatioDistributionFitType, tp->getBinNumbers()->getSize());
      coveras[i] = tp->CreateCorrelatedDistributionArrays(m_AspectRatioDistributionFitType, tp->getBinNumbers()->getSize());
      bvalues[i].resize(tp->getBinNumbers()->getSize());
      cvalues[i].resize(tp->getBinNumbers()->getSize());
      mindiams[i] = tp->getMinFeatureDiameter();
      binsteps[i] = tp->getBinStepSize();
    }
  }
  for (size_t i = 1; i < numfeatures; i++)
  {
    if (m_PhaseTypes[m_FeaturePhases[i]] == DREAM3D::PhaseType::PrimaryPhase ||
        m_PhaseTypes[m_FeaturePhases[i]] == DREAM3D::PhaseType::PrecipitatePhase ||
        m_PhaseTypes[m_FeaturePhases[i]] == DREAM3D::PhaseType::TransformationPhase)
    {
      if (m_BiasedFeatures[i] == false)
      {
        bin = size_t((m_EquivalentDiameters[i] - mindiams[m_FeaturePhases[i]]) / binsteps[m_FeaturePhases[i]]);
        bvalues[m_FeaturePhases[i]][bin].push_back(m_AspectRatios[2 * i]);
        cvalues[m_FeaturePhases[i]][bin].push_back(m_AspectRatios[2 * i + 1]);
      }
    }
  }
  for (size_t i = 1; i < numensembles; i++)
  {
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      m_DistributionAnalysis[m_AspectRatioDistributionFitType]->calculateCorrelatedParameters(bvalues[i], boveras[i]);
      m_DistributionAnalysis[m_AspectRatioDistributionFitType]->calculateCorrelatedParameters(cvalues[i], coveras[i]);
      pp->setFeatureSize_BOverA(boveras[i]);
      pp->setFeatureSize_COverA(coveras[i]);
    }
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      m_DistributionAnalysis[m_AspectRatioDistributionFitType]->calculateCorrelatedParameters(bvalues[i], boveras[i]);
      m_DistributionAnalysis[m_AspectRatioDistributionFitType]->calculateCorrelatedParameters(cvalues[i], coveras[i]);
      pp->setFeatureSize_BOverA(boveras[i]);
      pp->setFeatureSize_COverA(coveras[i]);
    }
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
      m_DistributionAnalysis[m_AspectRatioDistributionFitType]->calculateCorrelatedParameters(bvalues[i], boveras[i]);
      m_DistributionAnalysis[m_AspectRatioDistributionFitType]->calculateCorrelatedParameters(cvalues[i], coveras[i]);
      tp->setFeatureSize_BOverA(boveras[i]);
      tp->setFeatureSize_COverA(coveras[i]);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateEnsembleStatistics::gatherOmega3Stats()
{
  StatsDataArray& statsDataArray = *(m_StatsDataArray);

  size_t bin = 0;
  QVector<VectorOfFloatArray> omega3s;
  std::vector<std::vector<std::vector<float> > > values;
  QVector<float> mindiams;
  QVector<float> binsteps;
  size_t numfeatures = m_Omega3sPtr.lock()->getNumberOfTuples();
  size_t numensembles = m_PhaseTypesPtr.lock()->getNumberOfTuples();

  omega3s.resize(numensembles);
  values.resize(numensembles);
  mindiams.resize(numensembles);
  binsteps.resize(numensembles);
  for (size_t i = 1; i < numensembles; i++)
  {
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      omega3s[i] = pp->CreateCorrelatedDistributionArrays(m_Omega3DistributionFitType, pp->getBinNumbers()->getSize());
      values[i].resize(pp->getBinNumbers()->getSize());
      mindiams[i] = pp->getMinFeatureDiameter();
      binsteps[i] = pp->getBinStepSize();
    }
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      omega3s[i] = pp->CreateCorrelatedDistributionArrays(m_Omega3DistributionFitType, pp->getBinNumbers()->getSize());
      values[i].resize(pp->getBinNumbers()->getSize());
      mindiams[i] = pp->getMinFeatureDiameter();
      binsteps[i] = pp->getBinStepSize();
    }
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
      omega3s[i] = tp->CreateCorrelatedDistributionArrays(m_Omega3DistributionFitType, tp->getBinNumbers()->getSize());
      values[i].resize(tp->getBinNumbers()->getSize());
      mindiams[i] = tp->getMinFeatureDiameter();
      binsteps[i] = tp->getBinStepSize();
    }
  }
  for (size_t i = 1; i < numfeatures; i++)
  {
    if (m_PhaseTypes[m_FeaturePhases[i]] == DREAM3D::PhaseType::PrimaryPhase ||
        m_PhaseTypes[m_FeaturePhases[i]] == DREAM3D::PhaseType::PrecipitatePhase ||
        m_PhaseTypes[m_FeaturePhases[i]] == DREAM3D::PhaseType::TransformationPhase)
    {
      if (m_BiasedFeatures[i] == false)
      {
        bin = size_t((m_EquivalentDiameters[i] - mindiams[m_FeaturePhases[i]]) / binsteps[m_FeaturePhases[i]]);
        values[m_FeaturePhases[i]][bin].push_back(m_Omega3s[i]);
      }
    }
  }
  for (size_t i = 1; i < numensembles; i++)
  {
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      m_DistributionAnalysis[m_Omega3DistributionFitType]->calculateCorrelatedParameters(values[i], omega3s[i]);
      pp->setFeatureSize_Omegas(omega3s[i]);
    }
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      m_DistributionAnalysis[m_Omega3DistributionFitType]->calculateCorrelatedParameters(values[i], omega3s[i]);
      pp->setFeatureSize_Omegas(omega3s[i]);
    }
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
      m_DistributionAnalysis[m_Omega3DistributionFitType]->calculateCorrelatedParameters(values[i], omega3s[i]);
      tp->setFeatureSize_Omegas(omega3s[i]);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateEnsembleStatistics::gatherNeighborhoodStats()
{
  StatsDataArray& statsDataArray = *(m_StatsDataArray);

  size_t bin = 0;
  QVector<VectorOfFloatArray> neighborhoods;
  std::vector<std::vector<std::vector<float> > > values;
  std::vector<float> mindiams;
  std::vector<float> binsteps;
  size_t numfeatures = m_NeighborhoodsPtr.lock()->getNumberOfTuples();
  size_t numensembles = m_PhaseTypesPtr.lock()->getNumberOfTuples();

  neighborhoods.resize(numensembles);
  values.resize(numensembles);
  mindiams.resize(numensembles);
  binsteps.resize(numensembles);
  for (size_t i = 1; i < numensembles; i++)
  {
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      neighborhoods[i] = pp->CreateCorrelatedDistributionArrays(m_NeighborhoodDistributionFitType, pp->getBinNumbers()->getSize());
      values[i].resize(pp->getBinNumbers()->getSize());
      mindiams[i] = pp->getMinFeatureDiameter();
      binsteps[i] = pp->getBinStepSize();
    }
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      neighborhoods[i] = pp->CreateCorrelatedDistributionArrays(m_NeighborhoodDistributionFitType, pp->getBinNumbers()->getSize());
      values[i].resize(pp->getBinNumbers()->getSize());
      mindiams[i] = pp->getMinFeatureDiameter();
      binsteps[i] = pp->getBinStepSize();
    }
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
      neighborhoods[i] = tp->CreateCorrelatedDistributionArrays(m_NeighborhoodDistributionFitType, tp->getBinNumbers()->getSize());
      values[i].resize(tp->getBinNumbers()->getSize());
      mindiams[i] = tp->getMinFeatureDiameter();
      binsteps[i] = tp->getBinStepSize();
    }
  }

  for (size_t i = 1; i < numfeatures; i++)
  {
    if (m_PhaseTypes[m_FeaturePhases[i]] == DREAM3D::PhaseType::PrimaryPhase ||
        m_PhaseTypes[m_FeaturePhases[i]] == DREAM3D::PhaseType::PrecipitatePhase ||
        m_PhaseTypes[m_FeaturePhases[i]] == DREAM3D::PhaseType::TransformationPhase)
    {
      if (m_BiasedFeatures[i] == false)
      {
        bin = size_t((m_EquivalentDiameters[i] - mindiams[m_FeaturePhases[i]]) / binsteps[m_FeaturePhases[i]]);
        values[m_FeaturePhases[i]][bin].push_back(static_cast<float>( m_Neighborhoods[i] ));
      }
    }
  }
  for (size_t i = 1; i < numensembles; i++)
  {
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      m_DistributionAnalysis[m_NeighborhoodDistributionFitType]->calculateCorrelatedParameters(values[i], neighborhoods[i]);
      pp->setFeatureSize_Neighbors(neighborhoods[i]);
    }
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      m_DistributionAnalysis[m_NeighborhoodDistributionFitType]->calculateCorrelatedParameters(values[i], neighborhoods[i]);
      pp->setFeatureSize_Clustering(neighborhoods[i]);
    }
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
      m_DistributionAnalysis[m_NeighborhoodDistributionFitType]->calculateCorrelatedParameters(values[i], neighborhoods[i]);
      tp->setFeatureSize_Neighbors(neighborhoods[i]);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateEnsembleStatistics::gatherODFStats()
{
  StatsDataArray& statsDataArray = *(m_StatsDataArray);

  size_t bin = 0;
  size_t numfeatures = m_FeatureEulerAnglesPtr.lock()->getNumberOfTuples();
  size_t numensembles = m_PhaseTypesPtr.lock()->getNumberOfTuples();
  int32_t phase = 0;
  std::vector<float> totalvol;
  std::vector<FloatArrayType::Pointer> eulerodf;

  totalvol.resize(numensembles);
  eulerodf.resize(numensembles);
  uint64_t dims = 0;
  for (size_t i = 1; i < numensembles; i++)
  {
    totalvol[i] = 0;
    if (m_CrystalStructures[i] == Ebsd::CrystalStructure::Hexagonal_High)
    {
      dims = 36 * 36 * 12;
      eulerodf[i] = FloatArrayType::CreateArray(dims, DREAM3D::StringConstants::ODF);
      for (uint64_t j = 0; j < dims; j++)
      {
        eulerodf[i]->setValue(j, 0.0);
      }
    }
    else if (m_CrystalStructures[i] == Ebsd::CrystalStructure::Cubic_High)
    {
      dims = 18 * 18 * 18;
      eulerodf[i] = FloatArrayType::CreateArray(dims, DREAM3D::StringConstants::ODF);
      for (uint64_t j = 0; j < dims; j++)
      {
        eulerodf[i]->setValue(j, 0.0);
      }
    }
  }
  for (size_t i = 1; i < numfeatures; i++)
  {
    if (m_SurfaceFeatures[i] == false)
    {
      totalvol[m_FeaturePhases[i]] = totalvol[m_FeaturePhases[i]] + m_Volumes[i];
    }
  }
  for (size_t i = 1; i < numfeatures; i++)
  {
    if (m_SurfaceFeatures[i] == false)
    {
      phase = m_CrystalStructures[m_FeaturePhases[i]];
      FOrientArrayType eu( &(m_FeatureEulerAngles[3 * i]), 3); // Wrap the pointer
      FOrientArrayType rod(4);
      OrientationTransforms<FOrientArrayType, float>::eu2ro(eu, rod);
      bin = m_OrientationOps[phase]->getOdfBin(rod);
      eulerodf[m_FeaturePhases[i]]->setValue(bin, (eulerodf[m_FeaturePhases[i]]->getValue(bin) + (m_Volumes[i] / totalvol[m_FeaturePhases[i]])));
    }
  }
  for (size_t i = 1; i < numensembles; i++)
  {
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      pp->setODF(eulerodf[i]);
    }
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      pp->setODF(eulerodf[i]);
    }
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
      tp->setODF(eulerodf[i]);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateEnsembleStatistics::gatherMDFStats()
{
  StatsDataArray& statsDataArray = *(m_StatsDataArray);

  // But since a pointer is difficult to use operators with we will now create a
  // reference variable to the pointer with the correct variable name that allows
  // us to use the same syntax as the "vector of vectors"
  NeighborList<int32_t>& neighborlist = *(m_NeighborList.lock());
  // And we do the same for the SharedSurfaceArea list
  NeighborList<float>& neighborsurfacearealist = *(m_SharedSurfaceAreaList.lock());

  float n1 = 0.0f, n2 = 0.0f, n3 = 0.0f;
  int32_t mbin = 0;
  float w = 0.0f;
  QuatF q1 = QuaternionMathF::New();
  QuatF q2 = QuaternionMathF::New();
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  size_t numfeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();
  size_t numensembles = m_PhaseTypesPtr.lock()->getNumberOfTuples();
  uint32_t phase1 = 0, phase2 = 0;
  QVector<float> totalSurfaceArea;
  QVector<FloatArrayType::Pointer> misobin;
  int32_t numbins = 0;

  misobin.resize(numensembles);
  totalSurfaceArea.resize(numensembles);
  for (size_t i = 1; i < numensembles; ++i)
  {
    totalSurfaceArea[i] = 0;
    if (Ebsd::CrystalStructure::Hexagonal_High == m_CrystalStructures[i] )
    {
      numbins = 36 * 36 * 12;
      misobin[i] = FloatArrayType::CreateArray(numbins, DREAM3D::StringConstants::MisorientationBins);
    }
    else if (Ebsd::CrystalStructure::Cubic_High == m_CrystalStructures[i] )
    {
      numbins = 18 * 18 * 18;
      misobin[i] = FloatArrayType::CreateArray(numbins, DREAM3D::StringConstants::MisorientationBins);
    }
    // Now initialize all bins to 0.0
    for (int32_t j = 0; j < numbins; j++)
    {
      misobin[i]->setValue(j, 0.0);
    }
  }
  int32_t nname = 0;
  float nsa = 0.0f;
  for (size_t i = 1; i < numfeatures; i++)
  {
    QuaternionMathF::Copy(avgQuats[i], q1);
    phase1 = m_CrystalStructures[m_FeaturePhases[i]];
    for (size_t j = 0; j < neighborlist[i].size(); j++)
    {
      w = 10000.0f;
      nname = neighborlist[i][j];
      QuaternionMathF::Copy(avgQuats[nname], q2);
      phase2 = m_CrystalStructures[m_FeaturePhases[nname]];
      if (phase1 == phase2) { w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3); }
      if (phase1 == phase2)
      {
        FOrientArrayType rod(4);
        FOrientTransformsType::ax2ro(FOrientArrayType(n1, n2, n3, w), rod);

        if ((nname > i || m_SurfaceFeatures[nname] == true))
        {
          mbin = m_OrientationOps[phase1]->getMisoBin(rod);
          nsa = neighborsurfacearealist[i][j];
          misobin[m_FeaturePhases[i]]->setValue(mbin, (misobin[m_FeaturePhases[i]]->getValue(mbin) + nsa));
          totalSurfaceArea[m_FeaturePhases[i]] = totalSurfaceArea[m_FeaturePhases[i]] + nsa;
        }
      }
    }
  }

  for (size_t i = 1; i < numensembles; i++)
  {
    for (size_t j = 0; j < misobin[i]->getSize(); j++)
    {
      misobin[i]->setValue(j, (misobin[i]->getValue(j) / totalSurfaceArea[i]));
    }
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      pp->setMisorientationBins(misobin[i]);
      pp->setBoundaryArea(totalSurfaceArea[i]);
    }
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      pp->setMisorientationBins(misobin[i]);
      pp->setBoundaryArea(totalSurfaceArea[i]);
    }
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
      tp->setMisorientationBins(misobin[i]);
      tp->setBoundaryArea(totalSurfaceArea[i]);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateEnsembleStatistics::gatherAxisODFStats()
{
  StatsDataArray& statsDataArray = *(m_StatsDataArray);

  int32_t bin = 0;
  QVector<FloatArrayType::Pointer> axisodf;
  QVector<float> totalaxes;
  size_t numfeatures = m_AxisEulerAnglesPtr.lock()->getNumberOfTuples();
  size_t numXTals = m_PhaseTypesPtr.lock()->getNumberOfTuples();
  axisodf.resize(numXTals);
  totalaxes.resize(numXTals);
  for (size_t i = 1; i < numXTals; i++)
  {
    totalaxes[i] = 0.0;
    axisodf[i] = FloatArrayType::CreateArray((36 * 36 * 36), DREAM3D::StringConstants::AxisOrientation);
    for (int32_t j = 0; j < (36 * 36 * 36); j++)
    {
      axisodf[i]->setValue(j, 0.0);
    }
  }
  for (size_t i = 1; i < numfeatures; i++)
  {
    if (m_BiasedFeatures[i] == false)
    {
      totalaxes[m_FeaturePhases[i]]++;
    }
  }
  for (size_t i = 1; i < numfeatures; i++)
  {
    if (m_BiasedFeatures[i] == false)
    {
      FOrientArrayType rod(4);
      FOrientTransformsType::eu2ro( FOrientArrayType( &(m_AxisEulerAngles[3 * i]), 3), rod);
      m_OrientationOps[Ebsd::CrystalStructure::OrthoRhombic]->getODFFZRod(rod);
      bin = m_OrientationOps[Ebsd::CrystalStructure::OrthoRhombic]->getOdfBin(rod);
      axisodf[m_FeaturePhases[i]]->setValue(bin, (axisodf[m_FeaturePhases[i]]->getValue(bin) + static_cast<float>((1.0 / totalaxes[m_FeaturePhases[i]]))));
    }
  }

  for (size_t i = 1; i < numXTals; i++)
  {
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      pp->setAxisOrientation(axisodf[i]);
    }
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      pp->setAxisOrientation(axisodf[i]);
    }
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsDataArray[i].get());
      tp->setAxisOrientation(axisodf[i]);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateEnsembleStatistics::gatherRadialDistFunc()
{
  StatsDataArray& statsDataArray = *(m_StatsDataArray);

  size_t numBins = m_RadialDistFuncPtr.lock()->getNumberOfComponents();
  size_t numensembles = m_PhaseTypesPtr.lock()->getNumberOfTuples();

  for (size_t i = 1; i < numensembles; i++)
  {
    if (m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      RdfData::Pointer rdfData = RdfData::New();
      std::vector<float> freqs(numBins);

      for(size_t j = 0; j < numBins; j++)
      {
        freqs[j] = m_RadialDistFunc[i * numBins + j];
      }
      rdfData->setFrequencies(freqs);

      //std::cout << "index" << i << std::endl;
      //std::cout << "Rad Dist" << m_MaxMinRadialDistFunc[i * 2] << std::endl;
      //std::cout << "Rad Dist" << m_MaxMinRadialDistFunc[i * 2 + 1] << std::endl;

      rdfData->setMaxDistance(m_MaxMinRadialDistFunc[i * 2]);
      rdfData->setMinDistance(m_MaxMinRadialDistFunc[i * 2 + 1]);

      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      if (NULL == pp)
      {
        Q_ASSERT_X(NULL != pp, "StatsDataArray could not be down-cast to a PrecipitatesStatsDataArray", "");
      }
      pp->setRadialDistFunction(rdfData);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateEnsembleStatistics::calculatePPTBoundaryFrac()
{
  StatsDataArray& statsDataArray = *(m_StatsDataArray);

  NeighborList<int32_t>& neighborlist = *(m_NeighborList.lock());
  size_t numensembles = m_PhaseTypesPtr.lock()->getNumberOfTuples();
  size_t numfeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();
  std::vector<int32_t> boundaryPPT(numensembles, 0);
  std::vector<int32_t> totalNumPPT(numensembles, 0);
  std::vector<float> PPTBoundaryFrac(numensembles, 0);
  int32_t count = 0;

  for (size_t k = 1; k < numensembles; k++)
  {
    if (m_PhaseTypes[k] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      for (size_t i = 1; i < numfeatures; i++)
      {
        if (m_FeaturePhases[i] == k)
        {
          totalNumPPT[k]++;

          for (size_t j = 0; j < neighborlist[i].size(); j++)
          {
            if (m_FeaturePhases[i] != m_FeaturePhases[neighborlist[i][j]] &&
                m_PhaseTypes[m_FeaturePhases[neighborlist[i][j]]] != DREAM3D::PhaseType::MatrixPhase)  // Currently counts something as on the boundary if it has at least two neighbors of a different non-matrix phase. Might want to specify which phase in the future.
            {
              count++;
            }
          }
          if (count >= 2)
          {
            boundaryPPT[k]++;
          }
          count = 0;

        }
      }
      PPTBoundaryFrac[k] = (float)boundaryPPT[k] / (float)totalNumPPT[k];
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[k].get());
      pp->setPrecipBoundaryFraction(PPTBoundaryFrac[k]);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GenerateEnsembleStatistics::getPhaseCount()
{
  DataContainerArray::Pointer dca = getDataContainerArray();
  if(NULL == dca.get())
  {
    //  qDebug() << getNameOfClass() <<  "::getPhaseCount()  dca was NULL";
    return -1;
  }

  AttributeMatrix::Pointer inputAttrMat = dca->getAttributeMatrix(getCellEnsembleAttributeMatrixPath());
  if (NULL == inputAttrMat.get() )
  {
    //  qDebug() << getNameOfClass() << "::getPhaseCount()  CellEnsembleAttributeMatrix was NULL";
    //  qDebug() << "     " << getCellEnsembleAttributeMatrixPath().serialize("/");
    return -2;
  }

  if(inputAttrMat->getType() < DREAM3D::AttributeMatrixType::VertexEnsemble
      || inputAttrMat->getType() > DREAM3D::AttributeMatrixType::CellEnsemble )
  {
    //  qDebug() << getNameOfClass() << "::getPhaseCount() CellEnsembleAttributeMatrix was not correct Type";
    //  qDebug() << "     " << getCellEnsembleAttributeMatrixPath().serialize("/");
    //  qDebug() << "     " << (int)(inputAttrMat->getType());
    return -3;
  }

  // qDebug() << getNameOfClass() << "::getPhaseCount() data->getNumberOfTuples(): " << inputAttrMat->getTupleDimensions();
  // qDebug() << "Name" << inputAttrMat->getName();
  QVector<size_t> tupleDims = inputAttrMat->getTupleDimensions();

  size_t phaseCount = 1;
  for(int32_t i = 0; i < tupleDims.size(); i++)
  {
    phaseCount = phaseCount * tupleDims[i];
  }
  return phaseCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateEnsembleStatistics::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  size_t totalEnsembles = m_PhaseTypesPtr.lock()->getNumberOfTuples();

  // Check to see if the user has over ridden the phase types for this filter
  if (m_PhaseTypeData.d.size() > 0)
  {
    if( static_cast<int32_t>(m_PhaseTypeData.d.size()) < totalEnsembles)
    {
      setErrorCondition(-999);
      notifyErrorMessage(getHumanLabel(), "The number of phase types entered is less than the number of Ensembles", -999);
      return;
    }
    if (static_cast<int32_t>(m_PhaseTypeData.d.size()) > totalEnsembles)
    {
      QString ss = QObject::tr("The number of phase types entered is more than the number of Ensembles. Only the first %1 will be used").arg(totalEnsembles - 1);
      notifyErrorMessage(getHumanLabel(), ss, -999);
    }
    for (int32_t r = 0; r < totalEnsembles; ++r)
    {
      m_PhaseTypes[r] = m_PhaseTypeData.d[r];
    }
    m_StatsDataArray->fillArrayWithNewStatsData(m_PhaseTypesPtr.lock()->getNumberOfTuples(), m_PhaseTypes);
  }

  if(m_ComputeSizeDistribution == true)
  {
    gatherSizeStats();
  }
  if(m_ComputeAspectRatioDistribution == true)
  {
    gatherAspectRatioStats();
  }
  if(m_ComputeOmega3Distribution == true)
  {
    gatherOmega3Stats();
  }
  if(m_ComputeNeighborhoodDistribution == true)
  {
    gatherNeighborhoodStats();
  }
  if(m_CalculateODF == true)
  {
    gatherODFStats();
  }
  if(m_CalculateMDF == true)
  {
    gatherMDFStats();
  }
  if(m_CalculateAxisODF == true)
  {
    gatherAxisODFStats();
  }
  if(m_IncludeRadialDistFunc == true)
  {
    gatherRadialDistFunc();
  }

  calculatePPTBoundaryFrac();

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GenerateEnsembleStatistics::newFilterInstance(bool copyFilterParameters)
{
  GenerateEnsembleStatistics::Pointer filter = GenerateEnsembleStatistics::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
    // Here we need to set all sorts of stuff that is going to get missed. This
    // is predominantly for FilterParameters that take multiple SIMPL_FILTER_PARAMETERS
    // Those will get missed.
    filter->setPhaseTypeData(getPhaseTypeData());
    filter->setCellEnsembleAttributeMatrixPath(getCellEnsembleAttributeMatrixPath());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateEnsembleStatistics::getCompiledLibraryName()
{ return StatisticsConstants::StatisticsBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateEnsembleStatistics::getGroupName()
{ return DREAM3D::FilterGroups::StatisticsFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateEnsembleStatistics::getSubGroupName()
{ return DREAM3D::FilterSubGroups::EnsembleStatsFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateEnsembleStatistics::getHumanLabel()
{ return "Generate Ensemble Statistics"; }
