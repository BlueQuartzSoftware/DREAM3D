/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
*
* Redistribution and use in source and binary forms, with or without
* modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of
* its
* contributors may be used to endorse or promote products derived from this
* software
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

#include "InsertPrecipitatePhases.h"

#include <fstream>

#include <QtCore/QDir>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/StatsData/PrecipitateStatsData.h"
#include "SIMPLib/Utilities/SIMPLibRandom.h"

#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"

#include "SyntheticBuilding/SyntheticBuildingConstants.h"
#include "SyntheticBuilding/SyntheticBuildingVersion.h"

// Include the MOC generated file for this class
#include "moc_InsertPrecipitatePhases.cpp"

const QString PrecipitateSyntheticShapeParametersName("Synthetic Shape Parameters (Precipitate)");

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InsertPrecipitatePhases::InsertPrecipitatePhases()
: AbstractFilter()
, m_ClusteringListArrayName(SIMPL::FeatureData::ClusteringList)
, m_ErrorOutputFile("")
, m_CsvOutputFile("")
, m_MaskArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Mask)
, m_UseMask(false)
, m_HavePrecips(false)
, m_PrecipInputFile("")
, m_PeriodicBoundaries(false)
, m_MatchRDF(false)
, m_WriteGoalAttributes(false)
, m_InputStatsArrayPath(SIMPL::Defaults::StatsGenerator, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::Statistics)
, m_InputPhaseTypesArrayPath(SIMPL::Defaults::StatsGenerator, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::PhaseTypes)
, m_InputShapeTypesArrayPath(SIMPL::Defaults::StatsGenerator, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::ShapeTypes)
, m_FeatureIdsArrayPath(SIMPL::Defaults::SyntheticVolumeDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds)
, m_CellPhasesArrayPath(SIMPL::Defaults::SyntheticVolumeDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Phases)
, m_BoundaryCellsArrayPath(SIMPL::Defaults::SyntheticVolumeDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::BoundaryCells)
, m_FeaturePhasesArrayPath(SIMPL::Defaults::SyntheticVolumeDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::Phases)
, m_NumCellsArrayName(SIMPL::FeatureData::NumCells)
, m_EquivalentDiametersArrayName(SIMPL::FeatureData::EquivalentDiameters)
, m_VolumesArrayName(SIMPL::FeatureData::Volumes)
, m_Omega3sArrayName(SIMPL::FeatureData::Omega3s)
, m_CentroidsArrayName(SIMPL::FeatureData::Centroids)
, m_AxisEulerAnglesArrayName(SIMPL::FeatureData::AxisEulerAngles)
, m_AxisLengthsArrayName(SIMPL::FeatureData::AxisLengths)
, m_NumFeaturesArrayPath(SIMPL::Defaults::SyntheticVolumeDataContainerName, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::NumFeatures)
, m_SaveGeometricDescriptions(false)
, m_NewAttributeMatrixPath(SIMPL::Defaults::SyntheticVolumeDataContainerName, PrecipitateSyntheticShapeParametersName, "")
, m_FeatureIds(nullptr)
, m_CellPhases(nullptr)
, m_Mask(nullptr)
, m_BoundaryCells(nullptr)
, m_AxisEulerAngles(nullptr)
, m_Centroids(nullptr)
, m_AxisLengths(nullptr)
, m_Volumes(nullptr)
, m_Omega3s(nullptr)
, m_EquivalentDiameters(nullptr)
, m_FeaturePhases(nullptr)
, m_NumCells(nullptr)
, m_PhaseTypes(nullptr)
, m_ShapeTypes(nullptr)
, m_NumFeatures(nullptr)
, m_Neighbors(nullptr)
{

  initialize();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InsertPrecipitatePhases::~InsertPrecipitatePhases()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SIMPL_NEW_BOOL_FP("Periodic Boundaries", PeriodicBoundaries, FilterParameter::Parameter, InsertPrecipitatePhases));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Match Radial Distribution Function", MatchRDF, FilterParameter::Parameter, InsertPrecipitatePhases));
  QStringList linkedProps("MaskArrayPath");
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Use Mask", UseMask, FilterParameter::Parameter, InsertPrecipitatePhases, linkedProps));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, InsertPrecipitatePhases, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", CellPhasesArrayPath, FilterParameter::RequiredArray, InsertPrecipitatePhases, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int8, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Boundary Cells", BoundaryCellsArrayPath, FilterParameter::RequiredArray, InsertPrecipitatePhases, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Bool, 1, AttributeMatrix::Category::Element);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Mask", MaskArrayPath, FilterParameter::RequiredArray, InsertPrecipitatePhases, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", FeaturePhasesArrayPath, FilterParameter::RequiredArray, InsertPrecipitatePhases, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::StatsDataArray, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Any);
    IGeometry::Types geomTypes;
    geomTypes.push_back(IGeometry::Type::Image);
    geomTypes.push_back(IGeometry::Type::Unknown);
    req.dcGeometryTypes = geomTypes;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Statistics", InputStatsArrayPath, FilterParameter::RequiredArray, InsertPrecipitatePhases, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt32, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Any);
    IGeometry::Types geomTypes;
    geomTypes.push_back(IGeometry::Type::Image);
    geomTypes.push_back(IGeometry::Type::Unknown);
    req.dcGeometryTypes = geomTypes;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phase Types", InputPhaseTypesArrayPath, FilterParameter::RequiredArray, InsertPrecipitatePhases, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt32, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Any);
    IGeometry::Types geomTypes;
    geomTypes.push_back(IGeometry::Type::Image);
    geomTypes.push_back(IGeometry::Type::Unknown);
    req.dcGeometryTypes = geomTypes;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Shape Types", InputShapeTypesArrayPath, FilterParameter::RequiredArray, InsertPrecipitatePhases, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Number of Features", NumFeaturesArrayPath, FilterParameter::RequiredArray, InsertPrecipitatePhases, req));
  }
  linkedProps.clear();
  linkedProps << "PrecipInputFile";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Already Have Precipitates", HavePrecips, FilterParameter::Parameter, InsertPrecipitatePhases, linkedProps));
  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Precipitate Input File", PrecipInputFile, FilterParameter::Parameter, InsertPrecipitatePhases, "*.txt", "Text File"));
  linkedProps.clear();
  linkedProps << "CsvOutputFile";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Write Goal Attributes", WriteGoalAttributes, FilterParameter::Parameter, InsertPrecipitatePhases, linkedProps));
  parameters.push_back(SIMPL_NEW_OUTPUT_FILE_FP("Goal Attribute CSV File", CsvOutputFile, FilterParameter::Parameter, InsertPrecipitatePhases, "*.csv", "Comma Separated Data"));

  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Save Shape Description Arrays");
    parameter->setPropertyName("SaveGeometricDescriptions");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(InsertPrecipitatePhases, this, SaveGeometricDescriptions));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(InsertPrecipitatePhases, this, SaveGeometricDescriptions));

    QVector<QString> choices;
    choices.push_back("Save To New Attribute Matrix");
    choices.push_back("Append To Existing Attribute Matrix");
    choices.push_back("Do Not Save");
    parameter->setChoices(choices);
    QStringList linkedProps;
    linkedProps << "NewAttributeMatrixPath"
                << "SelectedAttributeMatrixPath";
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }

  {
    AttributeMatrixCreationFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_AM_CREATION_FP("New Attribute Matrix", NewAttributeMatrixPath, FilterParameter::Parameter, InsertPrecipitatePhases, req, 0));
  }

  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Category::Feature);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Selected Attribute Matrix", SelectedAttributeMatrixPath, FilterParameter::Parameter, InsertPrecipitatePhases, req, 1));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNumFeaturesArrayPath(reader->readDataArrayPath("NumFeaturesArrayPath", getNumFeaturesArrayPath()));
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath()));
  setInputStatsArrayPath(reader->readDataArrayPath("InputStatsArrayPath", getInputStatsArrayPath()));
  setInputPhaseTypesArrayPath(reader->readDataArrayPath("InputPhaseTypesArrayPath", getInputPhaseTypesArrayPath()));
  setInputShapeTypesArrayPath(reader->readDataArrayPath("InputShapeTypesArrayPath", getInputShapeTypesArrayPath()));
  setBoundaryCellsArrayPath(reader->readDataArrayPath("BoundaryCellsArrayPath", getBoundaryCellsArrayPath()));
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  setMaskArrayPath(reader->readDataArrayPath("MaskArrayPath", getMaskArrayPath()));
  setPeriodicBoundaries(reader->readValue("PeriodicBoundaries", getPeriodicBoundaries()));
  setMatchRDF(reader->readValue("MatchRDF", getMatchRDF()));
  setUseMask(reader->readValue("UseMask", getUseMask()));
  setHavePrecips(reader->readValue("HavePrecips", getHavePrecips()));
  setPrecipInputFile(reader->readString("PrecipInputFile", getPrecipInputFile()));
  setWriteGoalAttributes(reader->readValue("WriteGoalAttributes", getWriteGoalAttributes()));
  setCsvOutputFile(reader->readString("CsvOutputFile", getCsvOutputFile()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::updateFeatureInstancePointers()
{
  setErrorCondition(0);
  if(nullptr != m_FeaturePhasesPtr.lock().get()) /* Validate the Weak Pointer wraps a
                                                    non-nullptr pointer to a DataArray<T>
                                                    object */
  {
    m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);
  }                                         /* Now assign the raw pointer to data from the DataArray<T> object */
  if(nullptr != m_NumCellsPtr.lock().get()) /* Validate the Weak Pointer wraps a
                                               non-nullptr pointer to a DataArray<T>
                                               object */
  {
    m_NumCells = m_NumCellsPtr.lock()->getPointer(0);
  }                                                    /* Now assign the raw pointer to data from the DataArray<T> object */
  if(nullptr != m_EquivalentDiametersPtr.lock().get()) /* Validate the Weak
                                                          Pointer wraps a
                                                          non-nullptr pointer
                                                          to a DataArray<T>
                                                          object */
  {
    m_EquivalentDiameters = m_EquivalentDiametersPtr.lock()->getPointer(0);
  }                                        /* Now assign the raw pointer to data from the DataArray<T> object */
  if(nullptr != m_VolumesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a
                                              DataArray<T> object */
  {
    m_Volumes = m_VolumesPtr.lock()->getPointer(0);
  }                                        /* Now assign the raw pointer to data from the DataArray<T> object */
  if(nullptr != m_Omega3sPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a
                                              DataArray<T> object */
  {
    m_Omega3s = m_Omega3sPtr.lock()->getPointer(0);
  }                                          /* Now assign the raw pointer to data from the DataArray<T> object */
  if(nullptr != m_CentroidsPtr.lock().get()) /* Validate the Weak Pointer wraps a
                                                non-nullptr pointer to a DataArray<T>
                                                object */
  {
    m_Centroids = m_CentroidsPtr.lock()->getPointer(0);
  }                                                /* Now assign the raw pointer to data from the DataArray<T> object */
  if(nullptr != m_AxisEulerAnglesPtr.lock().get()) /* Validate the Weak Pointer wraps a
                                                      non-nullptr pointer to a
                                                      DataArray<T> object */
  {
    m_AxisEulerAngles = m_AxisEulerAnglesPtr.lock()->getPointer(0);
  }                                            /* Now assign the raw pointer to data from the DataArray<T> object */
  if(nullptr != m_AxisLengthsPtr.lock().get()) /* Validate the Weak Pointer
                                                  wraps a non-nullptr pointer
                                                  to a DataArray<T> object */
  {
    m_AxisLengths = m_AxisLengthsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::initialize()
{
  m_FirstPrecipitateFeature = 1;
  m_SizeX = m_SizeY = m_SizeZ = 0.0f;
  m_XRes = m_YRes = m_ZRes = m_TotalVol = m_UseableTotalVol = 0.0f;
  m_TotalVol = 0.0f;
  m_UseableTotalVol = 0.0f;
  m_XPoints = m_YPoints = m_ZPoints = m_TotalPoints = 0;
  m_TotalPoints = 0;

  m_ClusteringList = NeighborList<float>::NullPointer();

  m_ShapeOps = ShapeOps::getShapeOpsQVector();
  m_UnknownShapeOps = ShapeOps::NullPointer();
  m_CubicOctohedronOps = ShapeOps::NullPointer();
  m_CylinderOps = ShapeOps::NullPointer();
  m_EllipsoidOps = ShapeOps::NullPointer();
  m_SuperEllipsoidOps = ShapeOps::NullPointer();
  m_OrthoOps = OrthoRhombicOps::New();

  m_Neighbors = nullptr;
  m_StatsDataArray = StatsDataArray::NullPointer();

  m_ColumnList.clear();
  m_RowList.clear();
  m_PlaneList.clear();
  m_PointsToAdd.clear();
  m_PointsToRemove.clear();

  m_Seed = QDateTime::currentMSecsSinceEpoch();

  m_FeatureSizeDist.clear();
  m_SimFeatureSizeDist.clear();
  m_RdfTargetDist.clear();
  m_RdfCurrentDist.clear();
  m_RdfCurrentDistNorm.clear();
  m_RandomCentroids.clear();
  m_RdfRandom.clear();
  m_FeatureSizeDistStep.clear();
  m_GSizes.clear();

  m_AvailablePointsCount = 0;
  m_currentRDFerror = m_oldRDFerror = 0.0f;
  m_CurrentSizeDistError = m_OldSizeDistError = 0.0f;
  m_rdfMax = m_rdfMin = m_StepSize = 0.0f;
  m_numRDFbins = 0;

  m_PrecipitatePhases.clear();
  m_PrecipitatePhaseFractions.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

  QVector<DataArrayPath> cellDataArrayPaths;
  QVector<DataArrayPath> ensembleDataArrayPaths;

  QVector<size_t> cDims(1, 1);
  m_PhaseTypesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getInputPhaseTypesArrayPath(), cDims);
  if(nullptr != m_PhaseTypesPtr.lock().get()) /* Validate the Weak Pointer wraps a
                                                 non-nullptr pointer to a DataArray<T>
                                                 object */
  {
    m_PhaseTypes = m_PhaseTypesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    ensembleDataArrayPaths.push_back(getInputPhaseTypesArrayPath());
  }

  m_ShapeTypesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getInputShapeTypesArrayPath(), cDims);
  if(nullptr != m_ShapeTypesPtr.lock().get()) /* Validate the Weak Pointer wraps a
                                                 non-nullptr pointer to a DataArray<T>
                                                 object */
  {
    m_ShapeTypes = m_ShapeTypesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    ensembleDataArrayPaths.push_back(getInputShapeTypesArrayPath());
  }

  m_StatsDataArray = getDataContainerArray()->getPrereqArrayFromPath<StatsDataArray, AbstractFilter>(this, getInputStatsArrayPath(), cDims);
  if(m_StatsDataArray.lock() == nullptr)
  {
    QString ss = QObject::tr("Statistics array is not initialized correctly. The path is %1").arg(getInputStatsArrayPath().serialize());
    setErrorCondition(-308);
    notifyErrorMessage(getHumanLabel(), ss, -308);
  }
  if(getErrorCondition() >= 0)
  {
    ensembleDataArrayPaths.push_back(getInputStatsArrayPath());
  }

  // Cell Data
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that
                                                                                                                                                   is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock().get())                                                                                                   /* Validate the Weak Pointer wraps a
                                                                                                                                                   non-nullptr pointer to a DataArray<T>
                                                                                                                                                   object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    cellDataArrayPaths.push_back(getFeatureIdsArrayPath());
  }

  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that
                                                                                                                                                   is a weak_ptr<> */
  if(nullptr != m_CellPhasesPtr.lock().get())                                                                                                   /* Validate the Weak Pointer wraps a
                                                                                                                                                   non-nullptr pointer to a DataArray<T>
                                                                                                                                                   object */
  {
    m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    cellDataArrayPaths.push_back(getCellPhasesArrayPath());
  }

  m_BoundaryCellsPtr =
      getDataContainerArray()->getPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter>(this, getBoundaryCellsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that
                                                                                                                                       is a weak_ptr<> */
  if(nullptr != m_BoundaryCellsPtr.lock().get())                                                                                    /* Validate the Weak Pointer wraps a
                                                                                                                                       non-nullptr pointer to a DataArray<T>
                                                                                                                                       object */
  {
    m_BoundaryCells = m_BoundaryCellsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    cellDataArrayPaths.push_back(getBoundaryCellsArrayPath());
  }

  if(m_UseMask == true)
  {
    m_MaskPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getMaskArrayPath(), cDims);
    if(nullptr != m_MaskPtr.lock().get()) /* Validate the Weak Pointer wraps a
                                             non-nullptr pointer to a
                                             DataArray<T> object */
    {
      m_Mask = m_MaskPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCondition() >= 0)
    {
      cellDataArrayPaths.push_back(getMaskArrayPath());
    }
  }

  QVector<size_t> tDims(1, 0);
  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getFeaturePhasesArrayPath().getDataContainerName());
  if(getErrorCondition() < 0)
  {
    return;
  }

  InsertPrecipitatePhases::SaveMethod saveMethod = static_cast<InsertPrecipitatePhases::SaveMethod>(getSaveGeometricDescriptions());
  if(saveMethod == InsertPrecipitatePhases::SaveMethod::SaveToNew)
  {
    m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getNewAttributeMatrixPath().getAttributeMatrixName(), tDims, AttributeMatrix::Type::CellFeature);
  }
  else if(saveMethod == InsertPrecipitatePhases::SaveMethod::AppendToExisting)
  {
    int err = 0;
    m->getPrereqAttributeMatrix<AbstractFilter>(this, getSelectedAttributeMatrixPath().getAttributeMatrixName(), err);
  }

  // Feature Data
  m_FeaturePhasesPtr =
      getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that
                                                                                                                                        is a weak_ptr<> */
  if(nullptr != m_FeaturePhasesPtr.lock().get())                                                                                     /* Validate the Weak Pointer wraps a
                                                                                                                                        non-nullptr pointer to a DataArray<T>
                                                                                                                                        object */
  {
    m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getNumCellsArrayName());
  m_NumCellsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, cDims, getNumCellsArrayName()); /* Assigns the shared_ptr<>
                                                                                                                                                                           to an instance variable
                                                                                                                                                                           that is a weak_ptr<> */
  if(nullptr != m_NumCellsPtr.lock().get()) /* Validate the Weak Pointer wraps a
                                               non-nullptr pointer to a DataArray<T>
                                               object */
  {
    m_NumCells = m_NumCellsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getEquivalentDiametersArrayName());
  m_EquivalentDiametersPtr =
      getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, cDims, getEquivalentDiametersArrayName()); /* Assigns the
                                                                                                                                                                      shared_ptr<> to
                                                                                                                                                                      an instance
                                                                                                                                                                      variable that is
                                                                                                                                                                      a weak_ptr<> */
  if(nullptr != m_EquivalentDiametersPtr.lock().get())                                                                                                             /* Validate the Weak
                                                                                                                                                                      Pointer wraps a
                                                                                                                                                                      non-nullptr pointer
                                                                                                                                                                      to a DataArray<T>
                                                                                                                                                                      object */
  {
    m_EquivalentDiameters = m_EquivalentDiametersPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getVolumesArrayName());
  m_VolumesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, cDims, getVolumesArrayName()); /* Assigns the shared_ptr<> to an
                                                                                                                                                                     instance variable that is a
                                                                                                                                                                     weak_ptr<> */
  if(nullptr != m_VolumesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a
                                              DataArray<T> object */
  {
    m_Volumes = m_VolumesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getOmega3sArrayName());
  m_Omega3sPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, cDims, getOmega3sArrayName()); /* Assigns the shared_ptr<> to an
                                                                                                                                                                     instance variable that is a
                                                                                                                                                                     weak_ptr<> */
  if(nullptr != m_Omega3sPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a
                                              DataArray<T> object */
  {
    m_Omega3s = m_Omega3sPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 3;
  tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getCentroidsArrayName());
  m_CentroidsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, cDims, getCentroidsArrayName()); /* Assigns the shared_ptr<> to an
                                                                                                                                                                         instance variable that is a
                                                                                                                                                                         weak_ptr<> */
  if(nullptr != m_CentroidsPtr.lock().get()) /* Validate the Weak Pointer wraps a
                                                non-nullptr pointer to a DataArray<T>
                                                object */
  {
    m_Centroids = m_CentroidsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getAxisEulerAnglesArrayName());
  m_AxisEulerAnglesPtr =
      getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, cDims, getAxisEulerAnglesArrayName()); /* Assigns the shared_ptr<> to an
                                                                                                                                                                  instance variable that is a
                                                                                                                                                                  weak_ptr<> */
  if(nullptr != m_AxisEulerAnglesPtr.lock().get())                                                                                                             /* Validate the Weak Pointer wraps a
                                                                                                                                                                  non-nullptr pointer to a
                                                                                                                                                                  DataArray<T> object */
  {
    m_AxisEulerAngles = m_AxisEulerAnglesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getAxisLengthsArrayName());
  m_AxisLengthsPtr =
      getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, cDims, getAxisLengthsArrayName()); /* Assigns the shared_ptr<> to an
                                                                                                                                                              instance variable that is a
                                                                                                                                                              weak_ptr<> */
  if(nullptr != m_AxisLengthsPtr.lock().get())                                                                                                             /* Validate the Weak Pointer
                                                                                                                                                              wraps a non-nullptr pointer
                                                                                                                                                              to a DataArray<T> object */
  {
    m_AxisLengths = m_AxisLengthsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Ensemble Data
  cDims[0] = 1;
  m_NumFeaturesPtr =
      getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getNumFeaturesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that
                                                                                                                                      is a weak_ptr<> */
  if(nullptr != m_NumFeaturesPtr.lock().get())                                                                                     /* Validate the Weak Pointer
                                                                                                                                      wraps a non-nullptr pointer
                                                                                                                                      to a DataArray<T> object */
  {
    m_NumFeatures = m_NumFeaturesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(m_WriteGoalAttributes == true && getCsvOutputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("The Csv output file must be set").arg(ClassName());
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(m_HavePrecips == true && getPrecipInputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("The precipitate file must be set");
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, cellDataArrayPaths);
  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, ensembleDataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();

  AttributeMatrix::Pointer attrMat = getDataContainerArray()->getAttributeMatrix(getFeaturePhasesArrayPath());
  if(attrMat == nullptr)
  {
    setInPreflight(false);
    return;
  }

  moveShapeDescriptions();

  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::execute()
{
  initialize();

  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  size_t udims[3] = {0, 0, 0};
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);

  int64_t dims[3] = {
      static_cast<int64_t>(udims[0]), static_cast<int64_t>(udims[1]), static_cast<int64_t>(udims[2]),
  };

  m_TotalPoints = static_cast<size_t>(dims[0] * dims[1] * dims[2]);

  if(m_HavePrecips == false)
  {
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), "Packing Precipitates || Generating and Placing Precipitates");
    // this initializes the arrays to hold the details of the locations of all
    // of the features during packing
    Int32ArrayType::Pointer exclusionZonesPtr = Int32ArrayType::CreateArray(m_TotalPoints, "_INTERNAL_USE_ONLY_PackPrimaryFeatures::exclusion_zones");
    exclusionZonesPtr->initializeWithZeros();
    place_precipitates(exclusionZonesPtr);
    if(getErrorCondition() < 0 || getWarningCondition() < 0)
    {
      return;
    }
    if(getCancel() == true)
    {
      return;
    }
  }

  if(m_HavePrecips == true)
  {
    load_precipitates();
    if(getCancel() == true)
    {
      return;
    }
  }

  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), "Packing Precipitates || Assigning Voxels");
  assign_voxels();
  if(getCancel() == true)
  {
    return;
  }

  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), "Packing Precipitates || Filling Gaps");
  assign_gaps();
  if(getCancel() == true)
  {
    return;
  }

  // At this point we are done reassigning values to all arrays, so we are safe
  // to copy
  // down the Feature phases to the cells ; the Feature phases are correct from
  // the
  // generate_precipitate() function, but the cell phases are best done after
  // all
  // assignment since cell values may be cleaned up after Feature generation
  size_t numTuples = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  for(size_t i = 0; i < numTuples; i++)
  {
    m_CellPhases[i] = m_FeaturePhases[m_FeatureIds[i]];
  }

  if(m_WriteGoalAttributes == true)
  {
    write_goal_attributes();
  }

  moveShapeDescriptions();

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::load_precipitates()
{
  AttributeMatrix::Pointer cellFeatureAttrMat = getDataContainerArray()->getAttributeMatrix(getFeaturePhasesArrayPath());

  std::ifstream inFile;
  inFile.open(getPrecipInputFile().toLatin1().data(), std::ios_base::binary);
  if(!inFile)
  {
    QString ss = QObject::tr("Failed to open: %1").arg(getPrecipInputFile());
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, -1);
  }
  int32_t numPrecips = 0;
  inFile >> numPrecips;
  if(0 == numPrecips)
  {
    notifyWarningMessage(getHumanLabel(), "The number of precipitates is 0 and should be greater than 0", -600);
    return;
  }

  m_FirstPrecipitateFeature = cellFeatureAttrMat->getNumberOfTuples();

  QVector<size_t> tDims(1, m_FirstPrecipitateFeature + numPrecips);
  cellFeatureAttrMat->setTupleDimensions(tDims);
  updateFeatureInstancePointers();

  int32_t phase = 0;
  float xC = 0.0f, yC = 0.0f, zC = 0.0f;
  float axisA = 0.0f, axisB = 0.0f, axisC = 0.0f;
  float vol = 0.0f, eqDiam = 0.0f;
  float omega3 = 0.0f;
  float phi1 = 0.0f, PHI = 0.0f, phi2 = 0.0f;
  int32_t currentFeature = m_FirstPrecipitateFeature;
  const float fourThirds = 4.0f / 3.0f;
  for(int32_t i = 0; i < numPrecips; i++)
  {
    inFile >> phase >> xC >> yC >> zC >> axisA >> axisB >> axisC >> omega3 >> phi1 >> PHI >> phi2;
    vol = fourThirds * SIMPLib::Constants::k_Pi * axisA * axisB * axisC;
    eqDiam = 2.0f * powf((vol * (0.75f) * (SIMPLib::Constants::k_1OverPi)), (SIMPLib::Constants::k_1Over3));
    m_Centroids[3 * currentFeature + 0] = xC;
    m_Centroids[3 * currentFeature + 1] = yC;
    m_Centroids[3 * currentFeature + 2] = zC;
    m_Volumes[currentFeature] = vol;
    m_EquivalentDiameters[currentFeature] = eqDiam;
    m_AxisLengths[3 * currentFeature + 0] = axisA / axisA;
    m_AxisLengths[3 * currentFeature + 1] = axisB / axisA;
    m_AxisLengths[3 * currentFeature + 2] = axisC / axisA;
    m_AxisEulerAngles[3 * currentFeature + 0] = phi1;
    m_AxisEulerAngles[3 * currentFeature + 1] = PHI;
    m_AxisEulerAngles[3 * currentFeature + 2] = phi2;
    m_Omega3s[currentFeature] = omega3;
    m_FeaturePhases[currentFeature] = phase;
    currentFeature++;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::place_precipitates(Int32ArrayType::Pointer exclusionZonesPtr)
{
  bool writeErrorFile = true;
  bool write_test_outputs = false;

  std::ofstream outFile;
  if(m_ErrorOutputFile.isEmpty() == false)
  {
    outFile.open(m_ErrorOutputFile.toLatin1().data(), std::ios_base::binary);
    writeErrorFile = true;
  }

  setErrorCondition(0);
  m_Seed = QDateTime::currentMSecsSinceEpoch();
  SIMPL_RANDOMNG_NEW_SEEDED(m_Seed);

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  StatsDataArray& statsDataArray = *(m_StatsDataArray.lock());

  size_t udims[3] = {0, 0, 0};
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);

  int64_t dims[3] = {
      static_cast<int64_t>(udims[0]), static_cast<int64_t>(udims[1]), static_cast<int64_t>(udims[2]),
  };

  m_XPoints = static_cast<int64_t>(dims[0]);
  m_YPoints = static_cast<int64_t>(dims[1]);
  m_ZPoints = static_cast<int64_t>(dims[2]);
  m_TotalPoints = dims[0] * dims[1] * dims[2];
  m_XRes = m->getGeometryAs<ImageGeom>()->getXRes();
  m_YRes = m->getGeometryAs<ImageGeom>()->getYRes();
  m_ZRes = m->getGeometryAs<ImageGeom>()->getZRes();
  m_SizeX = dims[0] * m_XRes;
  m_SizeY = dims[1] * m_YRes;
  m_SizeZ = dims[2] * m_ZRes;
  m_TotalVol = m_SizeX * m_SizeY * m_SizeZ;
  if(m_UseMask == false)
  {
    m_UseableTotalVol = m_TotalVol;
  }
  else if(m_UseMask == true)
  {
    float cellVol = m_XRes * m_YRes * m_ZRes;
    for(int64_t i = 0; i < m_TotalPoints; i++)
    {
      if(m_Mask[i] == true)
      {
        m_UseableTotalVol += cellVol;
      }
    }
  }

  // figure out how many grains we already have so we can start the counter at
  // +1 this

  int32_t currentnumfeatures = static_cast<int32_t>(m_FeaturePhasesPtr.lock()->getNumberOfTuples());
  size_t numensembles = m_PhaseTypesPtr.lock()->getNumberOfTuples();
  QVector<size_t> tDims(1, 1);
  if(currentnumfeatures == 0)
  {
    m->getAttributeMatrix(getFeaturePhasesArrayPath().getAttributeMatrixName())->resizeAttributeArrays(tDims);
    updateFeatureInstancePointers();
    currentnumfeatures = 1;
  }
  m_FirstPrecipitateFeature = static_cast<int32_t>(currentnumfeatures);
  int32_t phase = 0;
  float precipboundaryfraction = 0.0f;
  float random = 0.0f;
  float xc = 0.0f, yc = 0.0f, zc = 0.0f;
  float oldxc = 0.0f, oldyc = 0.0f, oldzc = 0.0f;
  int32_t randomfeature = 0;
  int32_t acceptedmoves = 0;
  double totalprecipitatefractions = 0.0;

  for(size_t i = 1; i < numensembles; ++i)
  {
    if(m_PhaseTypes[i] == static_cast<PhaseType::EnumType>(PhaseType::Type::Precipitate))
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      if(nullptr == pp)
      {
        QString ss = QObject::tr("Tried to cast a statsDataArray[%1].get() to a "
                                 "PrecipitateStatsData* "
                                 "pointer but this resulted in a nullptr pointer. The "
                                 "value at m_PhaseTypes[%2] = %3 does not match up "
                                 "with the type of pointer stored in the StatsDataArray "
                                 "(PrecipitateStatsData)\n")
                         .arg(i)
                         .arg(i)
                         .arg(m_PhaseTypes[i]);
        setErrorCondition(-666);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }
      m_NumFeatures[i] = 0;
      m_PrecipitatePhases.push_back(i);
      m_PrecipitatePhaseFractions.push_back(pp->getPhaseFraction());
      totalprecipitatefractions = totalprecipitatefractions + pp->getPhaseFraction();
    }
  }

  for(size_t i = 0; i < m_PrecipitatePhases.size(); i++)
  {
    m_PrecipitatePhaseFractions[i] = static_cast<float>(m_PrecipitatePhaseFractions[i] / totalprecipitatefractions);
  }

  if(getCancel() == true)
  {
    return;
  }

  // This is the set that we are going to keep updated with the points that are
  // not in an exclusion zone
  std::map<size_t, size_t> availablePoints;
  std::map<size_t, size_t> availablePointsInv;

  // Get a pointer to the Feature Owners that was just initialized in the
  // initialize_packinggrid() method
  int32_t* exclusionZones = exclusionZonesPtr->getPointer(0);
  size_t featureOwnersIdx = 0;

  // initialize the sim and goal size distributions for the precipitate phases
  m_FeatureSizeDist.resize(m_PrecipitatePhases.size());
  m_SimFeatureSizeDist.resize(m_PrecipitatePhases.size());
  m_FeatureSizeDistStep.resize(m_PrecipitatePhases.size());
  for(size_t i = 0; i < m_PrecipitatePhases.size(); i++)
  {
    phase = m_PrecipitatePhases[i];
    PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[phase].get());
    m_FeatureSizeDist[i].resize(40);
    m_SimFeatureSizeDist[i].resize(40);
    m_FeatureSizeDistStep[i] = static_cast<float>(((2.0f * pp->getMaxFeatureDiameter()) - (pp->getMinFeatureDiameter() / 2.0f)) / m_FeatureSizeDist[i].size());
    float input = 0.0f;
    float previoustotal = 0.0f;
    VectorOfFloatArray GSdist = pp->getFeatureSizeDistribution();
    float avg = GSdist[0]->getValue(0);
    float stdev = GSdist[1]->getValue(0);
    float denominatorConst = sqrtf(2.0f * stdev * stdev); // Calculate it here rather than
                                                          // calculating the same thing multiple
                                                          // times below
    for(size_t j = 0; j < m_FeatureSizeDist[i].size(); j++)
    {
      input = (float(j + 1) * m_FeatureSizeDistStep[i]) + (pp->getMinFeatureDiameter() / 2.0f);
      float logInput = logf(input);
      if(logInput <= avg)
      {
        m_FeatureSizeDist[i][j] = 0.5f - 0.5f * (SIMPLibMath::erf((avg - logInput) / denominatorConst)) - previoustotal;
      }
      if(logInput > avg)
      {
        m_FeatureSizeDist[i][j] = 0.5f + 0.5f * (SIMPLibMath::erf((logInput - avg) / denominatorConst)) - previoustotal;
      }
      previoustotal = previoustotal + m_FeatureSizeDist[i][j];
    }
  }

  if(getCancel() == true)
  {
    return;
  }

  // adding precipitates until the volume fraction of precipitates matches the
  // target (while making sure to keep the size distribution error within
  // tolerance)
  Precip_t precip;
  std::vector<float> curphasevol;
  curphasevol.resize(m_PrecipitatePhases.size());
  float change = 0.0f;
  float factor = 1.0f;
  size_t iter = 0;
  for(size_t j = 0; j < m_PrecipitatePhases.size(); ++j)
  {
    curphasevol[j] = 0;
    float curphasetotalvol = static_cast<float>(m_UseableTotalVol * totalprecipitatefractions * m_PrecipitatePhaseFractions[j]);
    while(curphasevol[j] < (factor * curphasetotalvol))
    {
      iter++;
      m_Seed++;
      phase = m_PrecipitatePhases[j];
      generate_precipitate(phase, &precip, static_cast<ShapeType::Type>(m_ShapeTypes[phase]), m_OrthoOps);
      m_CurrentSizeDistError = check_sizedisterror(&precip);
      change = (m_CurrentSizeDistError) - (m_OldSizeDistError);
      if(change > 0.0f || m_CurrentSizeDistError > (1.0f - (float(iter) * 0.001f)) || curphasevol[j] < (0.75f * factor * curphasetotalvol))
      {
        if(currentnumfeatures % 100 == 0)
        {
          QString ss = QObject::tr("Packing Precipitates || Generating Feature #%1").arg(currentnumfeatures);
          notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);

          if(getCancel() == true)
          {
            return;
          }
        }

        tDims[0] = currentnumfeatures + 1;
        m->getAttributeMatrix(getFeaturePhasesArrayPath().getAttributeMatrixName())->resizeAttributeArrays(tDims);
        updateFeatureInstancePointers();
        transfer_attributes(currentnumfeatures, &precip);
        m_OldSizeDistError = m_CurrentSizeDistError;
        curphasevol[j] = curphasevol[j] + m_Volumes[currentnumfeatures];
        // FIXME: Initialize the Feature with some sort of default data
        iter = 0;
        m_NumFeatures[phase]++;
        currentnumfeatures++;
      }
    }
  }

  if(getCancel() == true)
  {
    return;
  }

  QString ss = QObject::tr("Packing Precipitates || Starting Feature Placement...");
  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);

  // initializing the target RDF vector - this is the radial distribution
  // function we are trying to match to
  if(m_MatchRDF == true)
  {
    for(size_t i = 1; i < numensembles; ++i)
    {
      if(m_PhaseTypes[i] == static_cast<PhaseType::EnumType>(PhaseType::Type::Precipitate))
      {
        PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
        RdfData::Pointer rdfTarget = pp->getRadialDistFunction();

        const std::vector<float>& freqs = rdfTarget->getFrequencies();
        //       int test =rdfTarget[0]->getNumberOfTuples();
        m_numRDFbins = static_cast<int32_t>(freqs.size());
        //      std::vector<float> rdfTargetDist;
        m_RdfTargetDist.resize(m_numRDFbins + 1);
        //       m_rdfCurrentDist.resize(m_numRDFbins+2);

        m_RdfTargetDist[0] = 0;

        for(int32_t j = 0; j < m_numRDFbins; j++)
        {
          m_RdfTargetDist[j + 1] = freqs[j];
        }
        m_rdfMax = rdfTarget->getMaxDistance();
        m_rdfMin = rdfTarget->getMinDistance();

        m_StepSize = (m_rdfMax - m_rdfMin) / float(m_numRDFbins);
        float max_box_distance = sqrtf((m_SizeX * m_SizeX) + (m_SizeY * m_SizeY) + (m_SizeZ * m_SizeZ));

        int32_t current_num_bins = static_cast<int32_t>(ceil((max_box_distance - m_rdfMin) / (m_StepSize)));

        m_RdfCurrentDist.resize(current_num_bins + 1);
      }
    }
  }

  // determine initial set of available points
  m_AvailablePointsCount = 0;
  for(int64_t i = 0; i < m_TotalPoints; i++)
  {
    if((exclusionZones[i] == 0 && m_UseMask == false) || (exclusionZones[i] == 0 && m_UseMask == true && m_Mask[i] == true))
    {
      availablePoints[i] = m_AvailablePointsCount;
      availablePointsInv[m_AvailablePointsCount] = i;
      m_AvailablePointsCount++;
    }
  }
  // and clear the pointsToRemove and pointsToAdd vectors from the initial
  // packing
  m_PointsToRemove.clear();
  m_PointsToAdd.clear();

  size_t key = 0;
  int64_t column = 0, row = 0, plane = 0;

  size_t numfeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();

  // initial placement
  m_ColumnList.resize(numfeatures);
  m_RowList.resize(numfeatures);
  m_PlaneList.resize(numfeatures);

  int64_t boundaryVoxels = 0;

  for(int64_t i = 1; i < m_TotalPoints; i++)
  {
    if(m_BoundaryCells[i] != 0)
    {
      boundaryVoxels++;
    }
  }

  float boundaryFraction = (float)boundaryVoxels / (float)m_TotalPoints;

  // boolean used to determine if current placement is acceptable if the
  // precipitates are being treated as "hard"
  //  bool good = false;

  //  int32_t progFeature = 0;
  //  int32_t progPrecipInc = static_cast<int32_t>(numfeatures * 0.01f);
  //  for (size_t i = size_t(m_FirstPrecipitateFeature); i < numfeatures; i++)
  //  {
  //    if (int32_t(i) > progFeature + progPrecipInc)
  //    {
  //      QString ss = QObject::tr("Packing Precipitates || Placing Precipitate
  //      #%1").arg(i);
  //      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
  //      progFeature = i;
  //    }

  //    m_Centroids[3 * i] = m_SizeX * 0.5;
  //    m_Centroids[3 * i + 1] = m_SizeY * 0.5;
  //    m_Centroids[3 * i + 2] = m_SizeZ * 0.5;
  //    insert_precipitate(i);

  //    good = false;
  //    int32_t iterCount = 0;
  //    while (good == false && iterCount < 100000 )
  //    {
  //      PrecipitateStatsData* pp =
  //      PrecipitateStatsData::SafePointerDownCast(statsDataArray[m_FeaturePhases[i]].get());
  //      precipboundaryfraction = pp->getPrecipBoundaryFraction();
  //      random = static_cast<float>(rg.genrand_res53());

  //      if (boundaryFraction != 0)
  //      {
  //        if (random <= precipboundaryfraction)
  //        {
  //          // figure out if we want this to be a boundary centroid voxel or
  //          not for the proposed precipitate
  //          if (availablePointsCount > 0)
  //          {
  //            key = static_cast<size_t>(rg.genrand_res53() *
  //            (availablePointsCount - 1));
  //            featureOwnersIdx = availablePointsInv[key];
  //            while (m_BoundaryCells[featureOwnersIdx] == 0)
  //            {
  //              key = static_cast<size_t>(rg.genrand_res53() *
  //              (availablePointsCount - 1));
  //              featureOwnersIdx = availablePointsInv[key];
  //            }
  //          }
  //          else
  //          {
  //            featureOwnersIdx = static_cast<size_t>(rg.genrand_res53() *
  //            m_TotalPoints);
  //            while (m_BoundaryCells[featureOwnersIdx] == 0)
  //            {
  //              featureOwnersIdx = static_cast<size_t>(rg.genrand_res53() *
  //              m_TotalPoints);
  //            }
  //          }

  //        }
  //        else if (random > precipboundaryfraction)
  //        {
  //          if (availablePointsCount > 0)
  //          {
  //            key = static_cast<size_t>(rg.genrand_res53() *
  //            (availablePointsCount - 1));
  //            featureOwnersIdx = availablePointsInv[key];
  //            while (m_BoundaryCells[featureOwnersIdx] != 0)
  //            {
  //              key = static_cast<size_t>(rg.genrand_res53() *
  //              (availablePointsCount - 1));
  //              featureOwnersIdx = availablePointsInv[key];
  //            }
  //          }
  //          else
  //          {
  //            featureOwnersIdx = static_cast<size_t>(rg.genrand_res53() *
  //            m_TotalPoints);
  //            while (m_BoundaryCells[featureOwnersIdx] != 0)
  //            {
  //              featureOwnersIdx = static_cast<size_t>(rg.genrand_res53() *
  //              m_TotalPoints);
  //            }
  //          }
  //        }
  //      }
  //      else
  //      {
  //        if (precipboundaryfraction > 0)
  //        {
  //          QString msg("There are no Feature boundaries on which to place
  //          precipitates and the target statistics precipitate fraction is
  //          greater than 0. This Filter will run without trying to match
  //          the precipitate fraction");
  //          notifyWarningMessage(getHumanLabel(), msg, -5010);
  //        }

  //        if (availablePointsCount > 0)
  //        {
  //          key = static_cast<size_t>(rg.genrand_res53() *
  //          (availablePointsCount - 1));
  //          featureOwnersIdx = availablePointsInv[key];
  //        }
  //        else
  //        {
  //          featureOwnersIdx = static_cast<size_t>(rg.genrand_res53() *
  //          m_TotalPoints);
  //        }
  //      }

  //      column = static_cast<int64_t>(featureOwnersIdx % m_XPoints);
  //      row = static_cast<int64_t>(featureOwnersIdx / m_XPoints) % m_YPoints;
  //      plane = static_cast<int64_t>(featureOwnersIdx / (m_XPoints *
  //      m_YPoints));
  //      xc = static_cast<float>((column * m_XRes) + (m_XRes * 0.5));
  //      yc = static_cast<float>((row * m_YRes) + (m_YRes * 0.5));
  //      zc = static_cast<float>((plane * m_ZRes) + (m_ZRes * 0.5));
  //      //m_Centroids[3 * i] = xc;
  //      //m_Centroids[3 * i + 1] = yc;
  //      //m_Centroids[3 * i + 2] = zc;
  //      move_precipitate(i, xc, yc, zc);
  //      //    insert_precipitate(i);
  //      good = check_for_overlap(i, exclusionZonesPtr);
  //      iterCount++;
  //      if(getCancel() == true) { return; }
  //    }
  //    if (getCancel() == true) { return; }
  //    update_exclusionZones(i, -1000, exclusionZonesPtr);
  //    update_availablepoints(availablePoints, availablePointsInv);
  //    if (iterCount >= 100000)
  //    {
  //      tDims[0] = i + 1;
  //      m->getAttributeMatrix(getFeaturePhasesArrayPath().getAttributeMatrixName())->resizeAttributeArrays(tDims);
  //      updateFeatureInstancePointers();
  //      numfeatures =
  //      m->getAttributeMatrix(getFeaturePhasesArrayPath().getAttributeMatrixName())->getNumberOfTuples();
  //    }
  //  }

  for(size_t i = size_t(m_FirstPrecipitateFeature); i < numfeatures; i++)
  {
    if(getCancel())
    {
      return;
    }
    QString ss = QObject::tr("Packing Precipitates || Placing Precipitate #%1").arg(i);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);

    PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[m_FeaturePhases[i]].get());
    precipboundaryfraction = pp->getPrecipBoundaryFraction();
    random = static_cast<float>(rg.genrand_res53());

    if(boundaryFraction != 0)
    {
      if(random <= precipboundaryfraction)
      {
        // figure out if we want this to be a boundary centroid voxel or not for
        // the proposed precipitate
        if(m_AvailablePointsCount > 0)
        {
          key = static_cast<size_t>(rg.genrand_res53() * (m_AvailablePointsCount - 1));
          featureOwnersIdx = availablePointsInv[key];
          while(m_BoundaryCells[featureOwnersIdx] == 0)
          {
            key = static_cast<size_t>(rg.genrand_res53() * (m_AvailablePointsCount - 1));
            featureOwnersIdx = availablePointsInv[key];
          }
        }
        else
        {
          featureOwnersIdx = static_cast<size_t>(rg.genrand_res53() * m_TotalPoints);
          while(m_BoundaryCells[featureOwnersIdx] == 0)
          {
            featureOwnersIdx = static_cast<size_t>(rg.genrand_res53() * m_TotalPoints);
          }
        }
      }
      else if(random > precipboundaryfraction)
      {
        if(m_AvailablePointsCount > 0)
        {
          key = static_cast<size_t>(rg.genrand_res53() * (m_AvailablePointsCount - 1));
          featureOwnersIdx = availablePointsInv[key];
          while(m_BoundaryCells[featureOwnersIdx] != 0)
          {
            key = static_cast<size_t>(rg.genrand_res53() * (m_AvailablePointsCount - 1));
            featureOwnersIdx = availablePointsInv[key];
          }
        }
        else
        {
          featureOwnersIdx = static_cast<size_t>(rg.genrand_res53() * m_TotalPoints);
          while(m_BoundaryCells[featureOwnersIdx] != 0)
          {
            featureOwnersIdx = static_cast<size_t>(rg.genrand_res53() * m_TotalPoints);
          }
        }
      }
    }
    else
    {

      if(precipboundaryfraction > 0)
      {
        QString msg("There are no Feature boundaries on which to place "
                    "precipitates and the target statistics precipitate "
                    "fraction is greater than 0. This Filter will run without "
                    "trying to match the "
                    "precipitate fraction");
        notifyWarningMessage(getHumanLabel(), msg, -5010);
      }

      if(m_AvailablePointsCount > 0)
      {
        key = static_cast<size_t>(rg.genrand_res53() * (m_AvailablePointsCount - 1));
        featureOwnersIdx = availablePointsInv[key];
      }
      else
      {
        featureOwnersIdx = static_cast<size_t>(rg.genrand_res53() * m_TotalPoints);
      }
    }

    column = static_cast<int64_t>(featureOwnersIdx % m_XPoints);
    row = static_cast<int64_t>(featureOwnersIdx / m_XPoints) % m_YPoints;
    plane = static_cast<int64_t>(featureOwnersIdx / (m_XPoints * m_YPoints));
    xc = static_cast<float>((column * m_XRes) + (m_XRes * 0.5));
    yc = static_cast<float>((row * m_YRes) + (m_YRes * 0.5));
    zc = static_cast<float>((plane * m_ZRes) + (m_ZRes * 0.5));
    m_Centroids[3 * i] = xc;
    m_Centroids[3 * i + 1] = yc;
    m_Centroids[3 * i + 2] = zc;
    insert_precipitate(i);
    update_exclusionZones(i, -1000, exclusionZonesPtr);
    update_availablepoints(availablePoints, availablePointsInv);
  }

  notifyStatusMessage(getHumanLabel(), "Packing Features - Initial Feature Placement Complete");

  if(m_MatchRDF == true)
  {
    /*RANDOM: Figure out the RDF for randomly distributed particles.
    We always keep the same stepsize as the target RDF,
    but change (increase if the current box is bigger than what the target dist
    was built on and vice versa)
    the number of bins to account for smaller and larger (up to the max distance
    i.e. the box diagonal)
    distances that can occur when particles are just randomly placed in a box.
    This is true for both m_rdfRandom and m_rdfCurrentDist.*/

    // initialize boxdims and boxres vectors
    std::vector<float> boxdims(3);
    boxdims[0] = m_SizeX;
    boxdims[1] = m_SizeY;
    boxdims[2] = m_SizeZ;

    std::vector<float> boxres(3);
    boxres[0] = m->getGeometryAs<ImageGeom>()->getXRes();
    boxres[1] = m->getGeometryAs<ImageGeom>()->getYRes();
    boxres[2] = m->getGeometryAs<ImageGeom>()->getZRes();

    float max_box_distance = sqrtf((m_SizeX * m_SizeX) + (m_SizeY * m_SizeY) + (m_SizeZ * m_SizeZ));

    int32_t current_num_bins = static_cast<int32_t>(ceil((max_box_distance - m_rdfMin) / (m_StepSize)));

    // resize box to include all the possible distances but using the same
    // stepsize as the target RDF. The zero bin includes all distances smaller
    // than the smallest from the targetRDF
    m_RdfRandom.resize(current_num_bins + 1);

    // Call this function to generate the random distribution, which is
    // normalized by the total number of distances
    m_RdfRandom = RadialDistributionFunction::GenerateRandomDistribution(m_rdfMin, m_rdfMax, m_numRDFbins, boxdims, boxres);

    size_t numPPTfeatures = static_cast<size_t>(numfeatures - m_FirstPrecipitateFeature);

    // Scale the randomRDF to have the same number of particles (and therfore
    // distances) as the current distribution.
    for(size_t i = 0; i < m_RdfRandom.size(); i++)
    {
      m_RdfRandom[i] = m_RdfRandom[i] * numPPTfeatures * (numPPTfeatures - 1);
    }

    if(write_test_outputs == true)
    {

      std::ofstream testFile6;
      testFile6.open("/Users/Shared/Data/PW_Work/OUTFILE/randomRDFCurrent.txt");
      for(size_t i = 0; i < m_RdfRandom.size(); i++)
      {
        testFile6 << "\n" << m_RdfRandom[i];
      }
      testFile6.close();
    }
  }

  if(m_MatchRDF == true)
  {
    // calculate the initial current RDF - this will change as we move particles
    // around
    for(size_t i = size_t(m_FirstPrecipitateFeature); i < numfeatures; i++)
    {
      m_oldRDFerror = check_RDFerror(int32_t(i), -1000, false);
    }

    if(true)
    {
      std::ofstream testFile;
      if(write_test_outputs == true)
      {
        testFile.open("/Users/Shared/Data/PW_Work/OUTFILE/BC.txt");
      }

      // begin swaping/moving/adding/removing features to try to improve packing
      // The totalAdjustments are roughly equal to the prefactor (1000, right
      // now) times the number of precipitates.
      // This is not based on convergence or any physics - it's just a factor
      // and there's probably room for improvement here
      int32_t totalAdjustments = static_cast<int32_t>(1000 * ((numfeatures - m_FirstPrecipitateFeature) - 1));
      for(int32_t iteration = 0; iteration < totalAdjustments; ++iteration)
      {
        QString ss;
        ss = QObject::tr("Packing Features - Swapping/Moving/Adding/Removing "
                         "Features Iteration %1/%2")
                 .arg(iteration)
                 .arg(totalAdjustments);
        if(iteration % 100 == 0)
        {
          notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
        }

        if(writeErrorFile == true && iteration % 25 == 0)
        {
          outFile << iteration << " " << m_oldRDFerror << " " << acceptedmoves << "\n";
        }

        // JUMP - this one feature to a random spot in the volume
        randomfeature = m_FirstPrecipitateFeature + int32_t(rg.genrand_res53() * (int32_t(numfeatures) - m_FirstPrecipitateFeature));
        if(randomfeature < m_FirstPrecipitateFeature)
        {
          randomfeature = m_FirstPrecipitateFeature;
        }
        if(randomfeature >= static_cast<int32_t>(numfeatures))
        {
          randomfeature = static_cast<int32_t>(numfeatures) - 1;
        }
        m_Seed++;

        PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[m_FeaturePhases[randomfeature]].get());
        if(nullptr == pp)
        {
          continue;
        }

        precipboundaryfraction = pp->getPrecipBoundaryFraction();
        random = static_cast<float>(rg.genrand_res53());
        if(boundaryFraction != 0)
        {
          if(random <= precipboundaryfraction)
          {
            // figure out if we want this to be a boundary centroid voxel or not
            // for the proposed precipitate
            if(m_AvailablePointsCount > 0)
            {
              key = static_cast<size_t>(rg.genrand_res53() * (m_AvailablePointsCount - 1));
              featureOwnersIdx = availablePointsInv[key];
              while(m_BoundaryCells[featureOwnersIdx] == 0)
              {
                key = static_cast<size_t>(rg.genrand_res53() * (m_AvailablePointsCount - 1));
                featureOwnersIdx = availablePointsInv[key];
              }
            }
            else
            {
              featureOwnersIdx = static_cast<size_t>(rg.genrand_res53() * m_TotalPoints);
              while(m_BoundaryCells[featureOwnersIdx] == 0)
              {
                featureOwnersIdx = static_cast<size_t>(rg.genrand_res53() * m_TotalPoints);
              }
            }
          }
          else if(random > precipboundaryfraction)
          {
            if(m_AvailablePointsCount > 0)
            {
              key = static_cast<size_t>(rg.genrand_res53() * (m_AvailablePointsCount - 1));
              featureOwnersIdx = availablePointsInv[key];
              while(m_BoundaryCells[featureOwnersIdx] != 0)
              {
                key = static_cast<size_t>(rg.genrand_res53() * (m_AvailablePointsCount - 1));
                featureOwnersIdx = availablePointsInv[key];
              }
            }
            else
            {
              featureOwnersIdx = static_cast<size_t>(rg.genrand_res53() * m_TotalPoints);
              while(m_BoundaryCells[featureOwnersIdx] != 0)
              {
                featureOwnersIdx = static_cast<size_t>(rg.genrand_res53() * m_TotalPoints);
              }
            }
          }
        }
        else
        {

          if(precipboundaryfraction > 0)
          {
            QString msg("There are no Feature boundaries to place precipitates "
                        "on and the target statistics precipitate fraction is "
                        "greater than 0. This Filter will run without trying "
                        "to match the "
                        "precipitate fraction");
            notifyWarningMessage(getHumanLabel(), msg, -5010);
          }

          if(m_AvailablePointsCount > 0)
          {
            key = static_cast<size_t>(rg.genrand_res53() * (m_AvailablePointsCount - 1));
            featureOwnersIdx = availablePointsInv[key];
          }
          else
          {
            featureOwnersIdx = static_cast<size_t>(rg.genrand_res53() * m_TotalPoints);
          }
        }
        column = static_cast<int64_t>(featureOwnersIdx % m_XPoints);
        row = static_cast<int64_t>(featureOwnersIdx / m_XPoints) % m_YPoints;
        plane = static_cast<int64_t>(featureOwnersIdx / (m_XPoints * m_YPoints));
        xc = static_cast<float>((column * m_XRes) + (m_XRes * 0.5));
        yc = static_cast<float>((row * m_YRes) + (m_YRes * 0.5));
        zc = static_cast<float>((plane * m_ZRes) + (m_ZRes * 0.5));
        oldxc = m_Centroids[3 * randomfeature];
        oldyc = m_Centroids[3 * randomfeature + 1];
        oldzc = m_Centroids[3 * randomfeature + 2];
        m_currentRDFerror = check_RDFerror(-1000, randomfeature, true);
        update_exclusionZones(-1000, randomfeature, exclusionZonesPtr);
        move_precipitate(randomfeature, xc, yc, zc);
        m_currentRDFerror = check_RDFerror(randomfeature, -1000, true);
        update_exclusionZones(randomfeature, -1000, exclusionZonesPtr);
        if(m_currentRDFerror >= m_oldRDFerror)
        {
          m_oldRDFerror = m_currentRDFerror;
          update_availablepoints(availablePoints, availablePointsInv);
          acceptedmoves++;
        }
        else
        {
          m_currentRDFerror = check_RDFerror(-1000, randomfeature, true);
          update_exclusionZones(-1000, randomfeature, exclusionZonesPtr);
          move_precipitate(randomfeature, oldxc, oldyc, oldzc);
          m_currentRDFerror = check_RDFerror(randomfeature, -1000, true);
          update_exclusionZones(randomfeature, -1000, exclusionZonesPtr);
          m_oldRDFerror = m_currentRDFerror;
          m_PointsToRemove.clear();
          m_PointsToAdd.clear();
        }

        if(write_test_outputs == true && iteration % 100 == 0)
        {
          testFile << "\n" << m_oldRDFerror;
        }
      }
      if(write_test_outputs == true)
      {
        testFile.close();
      }
    }
  }

  if(write_test_outputs == true)
  {
    std::ofstream testFile3;
    testFile3.open("/Users/Shared/Data/PW_Work/OUTFILE/current.txt");
    for(size_t i = 0; i < m_RdfCurrentDistNorm.size(); i++)
    {
      testFile3 << "\n" << m_RdfCurrentDistNorm[i];
    }
    testFile3.close();

    std::ofstream testFile2;
    testFile2.open("/Users/Shared/Data/PW_Work/OUTFILE/target.txt");
    for(size_t i = 0; i < m_RdfTargetDist.size(); i++)
    {
      testFile2 << "\n" << m_RdfTargetDist[i];
    }
    testFile2.close();
  }

  // std::cout << "Done Jumping" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::generate_precipitate(int32_t phase, Precip_t* precip, ShapeType::Type shapeclass, LaueOps::Pointer OrthoOps)
{
  SIMPL_RANDOMNG_NEW_SEEDED(m_Seed)

  StatsDataArray& statsDataArray = *(m_StatsDataArray.lock());

  float r1 = 1.0f;
  float a2 = 0.0f, a3 = 0.0f;
  float b2 = 0.0f, b3 = 0.0f;
  float diam = 0.0f;
  float vol = 0.0f;
  bool volgood = false;
  float fourThirdsPi = static_cast<float>((4.0f / 3.0f) * (SIMPLib::Constants::k_Pi));
  PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[phase].get());
  VectorOfFloatArray GSdist = pp->getFeatureSizeDistribution();
  float avg = GSdist[0]->getValue(0);
  float stdev = GSdist[1]->getValue(0);
  while(volgood == false)
  {
    volgood = true;
    diam = static_cast<float>(rg.genrand_norm(avg, stdev));
    diam = expf(diam);
    if(diam >= pp->getMaxFeatureDiameter())
    {
      volgood = false;
    }
    if(diam < pp->getMinFeatureDiameter())
    {
      volgood = false;
    }
    vol = fourThirdsPi * ((diam / 2.0f) * (diam / 2.0f) * (diam / 2.0f));
  }
  int32_t diameter = int32_t((diam - pp->getMinFeatureDiameter()) / pp->getBinStepSize());
  float r2 = 0.0f, r3 = 1.0f;
  VectorOfFloatArray bovera = pp->getFeatureSize_BOverA();
  VectorOfFloatArray covera = pp->getFeatureSize_COverA();
  if(diameter >= static_cast<int32_t>(bovera[0]->getSize()))
  {
    diameter = static_cast<int32_t>(bovera[0]->getSize()) - 1;
  }
  while(r2 < r3)
  {
    r2 = 0.0f, r3 = 0.0f;
    a2 = bovera[0]->getValue(diameter);
    b2 = bovera[1]->getValue(diameter);
    a3 = covera[0]->getValue(diameter);
    b3 = covera[1]->getValue(diameter);
    int32_t tmpDiameter = diameter;
    int32_t increment = -1;
    while(a2 == 0 || b2 == 0 || a3 == 0 || b3 == 0)
    {
      tmpDiameter += increment;
      if(tmpDiameter < 0)
      {
        tmpDiameter = diameter + 1;
        increment = 1;
      }
      if(tmpDiameter >= static_cast<int32_t>(bovera[0]->getSize()))
      {
        a2 = 1.0f;
        b2 = 0.0f;
        a3 = 1.0f;
        b3 = 0.0f;
        break;
      }
      a2 = bovera[0]->getValue(tmpDiameter);
      b2 = bovera[1]->getValue(tmpDiameter);
      a3 = covera[0]->getValue(tmpDiameter);
      b3 = covera[1]->getValue(tmpDiameter);
    }
    r2 = static_cast<float>(rg.genrand_beta(a2, b2));
    r3 = static_cast<float>(rg.genrand_beta(a3, b3));
  }
  FloatArrayType::Pointer axisodf = pp->getAxisOrientation();
  int32_t numbins = axisodf->getNumberOfTuples();
  float random = static_cast<float>(rg.genrand_res53());
  float totaldensity = 0.0f;
  int32_t bin = 0;
  for(int32_t j = 0; j < numbins; j++)
  {
    float density = axisodf->getValue(j);
    float td1 = totaldensity;
    totaldensity = totaldensity + density;
    if(random < totaldensity && random >= td1)
    {
      bin = j;
      break;
    }
  }
  FOrientArrayType eulers = OrthoOps->determineEulerAngles(m_Seed, bin);
  VectorOfFloatArray omega3 = pp->getFeatureSize_Omegas();
  float mf = omega3[0]->getValue(diameter);
  float s = omega3[1]->getValue(diameter);
  float omega3f = static_cast<float>(rg.genrand_beta(mf, s));
  if(shapeclass == ShapeType::Type::Ellipsoid)
  {
    omega3f = 1;
  }

  precip->m_Volumes = vol;
  precip->m_EquivalentDiameters = diam;
  precip->m_AxisLengths[0] = r1;
  precip->m_AxisLengths[1] = r2;
  precip->m_AxisLengths[2] = r3;
  precip->m_AxisEulerAngles[0] = eulers[0];
  precip->m_AxisEulerAngles[1] = eulers[1];
  precip->m_AxisEulerAngles[2] = eulers[2];
  precip->m_Omega3s = omega3f;
  precip->m_FeaturePhases = phase;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::transfer_attributes(int32_t gnum, Precip_t* precip)
{
  m_Volumes[gnum] = precip->m_Volumes;
  m_EquivalentDiameters[gnum] = precip->m_EquivalentDiameters;
  m_AxisLengths[3 * gnum + 0] = precip->m_AxisLengths[0];
  m_AxisLengths[3 * gnum + 1] = precip->m_AxisLengths[1];
  m_AxisLengths[3 * gnum + 2] = precip->m_AxisLengths[2];
  m_AxisEulerAngles[3 * gnum + 0] = precip->m_AxisEulerAngles[0];
  m_AxisEulerAngles[3 * gnum + 1] = precip->m_AxisEulerAngles[1];
  m_AxisEulerAngles[3 * gnum + 2] = precip->m_AxisEulerAngles[2];
  m_Omega3s[gnum] = precip->m_Omega3s;
  m_FeaturePhases[gnum] = precip->m_FeaturePhases;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::move_precipitate(int32_t gnum, float xc, float yc, float zc)
{
  int64_t occolumn = 0, ocrow = 0, ocplane = 0;
  int64_t nccolumn = 0, ncrow = 0, ncplane = 0;
  int64_t shiftcolumn = 0, shiftrow = 0, shiftplane = 0;
  float oxc = m_Centroids[3 * gnum];
  float oyc = m_Centroids[3 * gnum + 1];
  float ozc = m_Centroids[3 * gnum + 2];
  occolumn = static_cast<int64_t>((oxc - (m_XRes / 2.0f)) / m_XRes);
  ocrow = static_cast<int64_t>((oyc - (m_YRes / 2.0f)) / m_YRes);
  ocplane = static_cast<int64_t>((ozc - (m_ZRes / 2.0f)) / m_ZRes);
  nccolumn = static_cast<int64_t>((xc - (m_XRes / 2.0f)) / m_XRes);
  ncrow = static_cast<int64_t>((yc - (m_YRes / 2.0f)) / m_YRes);
  ncplane = static_cast<int64_t>((zc - (m_ZRes / 2.0f)) / m_ZRes);
  shiftcolumn = nccolumn - occolumn;
  shiftrow = ncrow - ocrow;
  shiftplane = ncplane - ocplane;
  m_Centroids[3 * gnum] = xc;
  m_Centroids[3 * gnum + 1] = yc;
  m_Centroids[3 * gnum + 2] = zc;
  size_t size = m_ColumnList[gnum].size();

  for(size_t i = 0; i < size; i++)
  {
    int64_t& cl = m_ColumnList[gnum][i];
    cl += shiftcolumn;
    int64_t& rl = m_RowList[gnum][i];
    rl += shiftrow;
    int64_t& pl = m_PlaneList[gnum][i];
    pl += shiftplane;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::update_exclusionZones(int32_t gadd, int32_t gremove, Int32ArrayType::Pointer exclusionZonesPtr)
{
  size_t featureOwnersIdx = 0;
  int32_t* exclusionZones = exclusionZonesPtr->getPointer(0);
  int64_t col = 0, row = 0, plane = 0;

  if(gadd > 0)
  {
    size_t size = m_ColumnList[gadd].size();
    std::vector<int64_t>& cl = m_ColumnList[gadd];
    std::vector<int64_t>& rl = m_RowList[gadd];
    std::vector<int64_t>& pl = m_PlaneList[gadd];
    for(size_t i = 0; i < size; i++)
    {
      col = cl[i];
      row = rl[i];
      plane = pl[i];
      if(m_PeriodicBoundaries == true)
      {
        if(col < 0)
        {
          col = col + m_XPoints;
        }
        if(col > m_XPoints - 1)
        {
          col = col - m_XPoints;
        }
        if(row < 0)
        {
          row = row + m_YPoints;
        }
        if(row > m_YPoints - 1)
        {
          row = row - m_YPoints;
        }
        if(plane < 0)
        {
          plane = plane + m_ZPoints;
        }
        if(plane > m_ZPoints - 1)
        {
          plane = plane - m_ZPoints;
        }
        featureOwnersIdx = (m_XPoints * m_YPoints * plane) + (m_XPoints * row) + col;
        if(exclusionZones[featureOwnersIdx] > 0)
        {
        }
        if(exclusionZones[featureOwnersIdx] == 0)
        {
          m_PointsToRemove.push_back(featureOwnersIdx);
        }
        exclusionZones[featureOwnersIdx]++;
      }
      else
      {
        if(col >= 0 && col < m_XPoints && row >= 0 && row < m_YPoints && plane >= 0 && plane < m_ZPoints)
        {
          featureOwnersIdx = (m_XPoints * m_YPoints * plane) + (m_XPoints * row) + col;
          if(exclusionZones[featureOwnersIdx] > 0)
          {
          }
          if(exclusionZones[featureOwnersIdx] == 0)
          {
            m_PointsToRemove.push_back(featureOwnersIdx);
          }
          exclusionZones[featureOwnersIdx]++;
        }
      }
    }
  }
  if(gremove > 0)
  {
    size_t size = m_ColumnList[gremove].size();
    std::vector<int64_t>& cl = m_ColumnList[gremove];
    std::vector<int64_t>& rl = m_RowList[gremove];
    std::vector<int64_t>& pl = m_PlaneList[gremove];
    for(size_t i = 0; i < size; i++)
    {
      col = cl[i];
      row = rl[i];
      plane = pl[i];
      if(m_PeriodicBoundaries == true)
      {
        if(col < 0)
        {
          col = col + m_XPoints;
        }
        if(col > m_XPoints - 1)
        {
          col = col - m_XPoints;
        }
        if(row < 0)
        {
          row = row + m_YPoints;
        }
        if(row > m_YPoints - 1)
        {
          row = row - m_YPoints;
        }
        if(plane < 0)
        {
          plane = plane + m_ZPoints;
        }
        if(plane > m_ZPoints - 1)
        {
          plane = plane - m_ZPoints;
        }
        featureOwnersIdx = (m_XPoints * m_YPoints * plane) + (m_XPoints * row) + col;
        if(exclusionZones[featureOwnersIdx] > 1)
        {
        }
        exclusionZones[featureOwnersIdx]--;
        if(exclusionZones[featureOwnersIdx] == 0)
        {
          m_PointsToAdd.push_back(featureOwnersIdx);
        }
      }
      else
      {
        if(col >= 0 && col < m_XPoints && row >= 0 && row < m_YPoints && plane >= 0 && plane < m_ZPoints)
        {
          featureOwnersIdx = (m_XPoints * m_YPoints * plane) + (m_XPoints * row) + col;
          if(exclusionZones[featureOwnersIdx] > 1)
          {
          }
          exclusionZones[featureOwnersIdx]--;
          if(exclusionZones[featureOwnersIdx] == 0)
          {
            m_PointsToAdd.push_back(featureOwnersIdx);
          }
        }
      }
    }
  }
}

////
///-----------------------------------------------------------------------------
////
////
///-----------------------------------------------------------------------------
// bool InsertPrecipitatePhases::check_for_overlap(size_t gNum,
// Int32ArrayType::Pointer exclusionZonesPtr)
//{
//  size_t featureOwnersIdx = 0;
//  int32_t* exclusionZones = exclusionZonesPtr->getPointer(0);
//  int64_t col = 0, row = 0, plane = 0;
//  int64_t overlapCount = 0;

//  size_t size = columnlist[gNum].size();
//  std::vector<int64_t>& cl = columnlist[gNum];
//  std::vector<int64_t>& rl = rowlist[gNum];
//  std::vector<int64_t>& pl = planelist[gNum];
//  for (size_t i = 0; i < size; i++)
//  {
//    col = cl[i];
//    row = rl[i];
//    plane = pl[i];
//    if (m_PeriodicBoundaries == true)
//    {
//      if (col < 0) { col = col + m_XPoints; }
//      if (col > m_XPoints - 1) { col = col - m_XPoints; }
//      if (row < 0) { row = row + m_YPoints; }
//      if (row > m_YPoints - 1) { row = row - m_YPoints; }
//      if (plane < 0) { plane = plane + m_ZPoints; }
//      if (plane > m_ZPoints - 1) { plane = plane - m_ZPoints; }
//      featureOwnersIdx = (m_XPoints * m_YPoints * plane) + (m_XPoints * row) +
//      col;
//      if (exclusionZones[featureOwnersIdx] > 0) { overlapCount++; }
//    }
//    else
//    {
//      if (col >= 0 && col < m_XPoints && row >= 0 && row < m_YPoints && plane
//      >= 0 && plane < m_ZPoints)
//      {
//        featureOwnersIdx = (m_XPoints * m_YPoints * plane) + (m_XPoints * row)
//        + col;
//        if (exclusionZones[featureOwnersIdx] > 0) { overlapCount++; }
//      }
//    }
//  }
//  if ((static_cast<float>(overlapCount) / static_cast<float>(size)) > 0.1) {
//  return false; }
//  return true;
//}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::update_availablepoints(std::map<size_t, size_t>& availablePoints, std::map<size_t, size_t>& availablePointsInv)
{
  size_t removeSize = m_PointsToRemove.size();
  size_t addSize = m_PointsToAdd.size();
  size_t featureOwnersIdx = 0;
  size_t key = 0, val = 0;
  for(size_t i = 0; i < addSize; i++)
  {
    featureOwnersIdx = m_PointsToAdd[i];
    availablePoints[featureOwnersIdx] = m_AvailablePointsCount;
    availablePointsInv[m_AvailablePointsCount] = featureOwnersIdx;
    m_AvailablePointsCount++;
  }
  for(size_t i = 0; i < removeSize; i++)
  {
    featureOwnersIdx = m_PointsToRemove[i];
    key = availablePoints[featureOwnersIdx];
    //  availablePoints.erase(featureOwnersIdx);
    val = availablePointsInv[m_AvailablePointsCount - 1];
    //  availablePointsInv.erase(availablePointsCount-1);
    if(key < m_AvailablePointsCount - 1)
    {
      availablePointsInv[key] = val;
      availablePoints[val] = key;
    }
    m_AvailablePointsCount--;
  }
  m_PointsToRemove.clear();
  m_PointsToAdd.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::determine_currentRDF(int32_t gnum, int32_t add, bool double_count)
{
  float x = 0.0f, y = 0.0f, z = 0.0f;
  float xn = 0.0f, yn = 0.0f, zn = 0.0f;
  float r = 0.0f;

  size_t iter = 0;
  int32_t numPPTfeatures = 1;
  int32_t rdfBin = 0;

  int32_t phase = m_FeaturePhases[gnum];
  while(phase != m_PrecipitatePhases[iter])
  {
    iter++;
  }

  x = m_Centroids[3 * gnum];
  y = m_Centroids[3 * gnum + 1];
  z = m_Centroids[3 * gnum + 2];
  size_t numFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();

  for(size_t n = size_t(m_FirstPrecipitateFeature); n < numFeatures; n++)
  {
    if(m_FeaturePhases[n] == phase && n != gnum)
    {
      xn = m_Centroids[3 * n];
      yn = m_Centroids[3 * n + 1];
      zn = m_Centroids[3 * n + 2];
      r = sqrtf((x - xn) * (x - xn) + (y - yn) * (y - yn) + (z - zn) * (z - zn));

      rdfBin = (r - m_rdfMin) / m_StepSize;

      if(r < m_rdfMin)
      {
        rdfBin = -1;
      }
      // if (r < 6.0)
      //{
      //  int stop = 0;
      //}
      // if (rdfBin >= m_numRDFbins) {rdfBin = m_numRDFbins;}
      if(double_count == true)
      {
        m_RdfCurrentDist[rdfBin + 1] += 2 * add;
      }
      else if(double_count == false)
      {
        m_RdfCurrentDist[rdfBin + 1] += add;
      }

      numPPTfeatures += 1;
    }
  }

  m_RdfCurrentDistNorm = normalizeRDF(m_RdfCurrentDist, m_numRDFbins, m_StepSize, m_rdfMin, numPPTfeatures);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::determine_randomRDF(size_t gnum, int32_t add, bool double_count, int32_t largeNumber)
{

  float x = 0.0f, y = 0.0f, z = 0.0f;
  float xn = 0.0f, yn = 0.0f, zn = 0.0f;
  float r = 0.0f;

  int32_t rdfBin = 0;

  x = m_RandomCentroids[3 * gnum];
  y = m_RandomCentroids[3 * gnum + 1];
  z = m_RandomCentroids[3 * gnum + 2];

  size_t ln = static_cast<size_t>(largeNumber);

  for(size_t n = 0; n < ln; n++)
  {
    if(n != gnum)
    {
      xn = m_RandomCentroids[3 * n];
      yn = m_RandomCentroids[3 * n + 1];
      zn = m_RandomCentroids[3 * n + 2];
      r = sqrtf((x - xn) * (x - xn) + (y - yn) * (y - yn) + (z - zn) * (z - zn));

      rdfBin = (r - m_rdfMin) / m_StepSize;

      if(r < m_rdfMin)
      {
        rdfBin = -1;
      }
      if(double_count == true)
      {
        m_RdfRandom[rdfBin + 1] += 2 * add;
      }
      else if(double_count == false)
      {
        m_RdfRandom[rdfBin + 1] += add;
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<float> InsertPrecipitatePhases::normalizeRDF(std::vector<float> rdf, int32_t num_bins, float m_StepSize, float rdfmin, int32_t numPPTfeatures)
{
  //  //Normalizing the RDF by number density of particles
  //  (4/3*pi*(r2^3-r1^3)*numPPTfeatures/volume)
  //  float normfactor;
  //  float r1;
  //  float r2;
  //  float oneovervolume = 1.0f/volume;
  //  float finiteAdjFactor = .5;

  //  r1 = 0*finiteAdjFactor;
  //  r2 = rdfmin*finiteAdjFactor;
  //  normfactor = 4.0f/3.0f*SIMPLib::Constants::k_Pi*((r2*r2*r2) -
  //  (r1*r1*r1))*numPPTfeatures*oneovervolume;
  //  rdf[0] = rdf[0];

  //  for (size_t i = 1; i < num_bins+2; i++)
  //  {
  //    r1 = (rdfmin + (i-1)*m_StepSize);
  //    r2 = (r1 + m_StepSize);
  //    r1 = r1*finiteAdjFactor;
  //    r2 = r2*finiteAdjFactor;
  //    normfactor = 4.0f/3.0f*SIMPLib::Constants::k_Pi*((r2*r2*r2) -
  //    (r1*r1*r1))*numPPTfeatures*oneovervolume;
  //    rdf[i] = rdf[i]/normfactor;
  //  }

  for(size_t i = 0; i < rdf.size(); i++)
  {
    rdf[i] = rdf[i] / m_RdfRandom[i];
  }

  return rdf;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float InsertPrecipitatePhases::check_RDFerror(int32_t gadd, int32_t gremove, bool double_count)
{
  float rdferror = 0.0f;
  float bhattdist = 0.0f;

  if(gadd > 0)
  {
    determine_currentRDF(gadd, 1, double_count);
  }
  if(gremove > 0)
  {
    determine_currentRDF(gremove, -1, double_count);
  }

  if(m_RdfCurrentDistNorm.size() > m_RdfTargetDist.size())
  {
    compare_1Ddistributions(m_RdfTargetDist, m_RdfCurrentDistNorm, bhattdist);
  }
  else
  {
    compare_1Ddistributions(m_RdfCurrentDistNorm, m_RdfTargetDist, bhattdist);
  }
  rdferror = bhattdist;
  return rdferror;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::compare_1Ddistributions(std::vector<float> array1, std::vector<float> array2, float& bhattdist)
{
  bhattdist = 0;
  float sum_array1 = 0.0f;
  float sum_array2 = 0.0f;

  //  for (std::vector<float>::iterator j=array1.begin(); j!=array1.end(); j++)
  //  {sum_array1 += *j;}

  //  for (std::vector<float>::iterator j=array2.begin(); j!=array2.end(); j++)
  //  {sum_array2 += *j;}

  size_t array1Size = array1.size();
  for(size_t i = 0; i < array1Size; i++)
  {
    sum_array1 = sum_array1 + array1[i];
    sum_array2 = sum_array2 + array2[i];
  }

  for(size_t i = 0; i < array1Size; i++)
  {
    array1[i] = array1[i] / sum_array1;
    array2[i] = array2[i] / sum_array2;
    bhattdist = bhattdist + sqrtf((array1[i] * array2[i]));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::compare_2Ddistributions(std::vector<std::vector<float>> array1, std::vector<std::vector<float>> array2, float& bhattdist)
{
  bhattdist = 0;
  size_t array1Size = array1.size();
  for(size_t i = 0; i < array1Size; i++)
  {
    size_t array2Size = array1[i].size();
    for(size_t j = 0; j < array2Size; j++)
    {
      bhattdist = bhattdist + sqrtf((array1[i][j] * array2[i][j]));
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::compare_3Ddistributions(std::vector<std::vector<std::vector<float>>> array1, std::vector<std::vector<std::vector<float>>> array2, float& bhattdist)
{
  bhattdist = 0;
  std::vector<std::vector<float>> counts1(array1.size());
  std::vector<std::vector<float>> counts2(array2.size());
  size_t array1Size = array1.size();
  for(size_t i = 0; i < array1Size; i++)
  {
    counts1[i].resize(array1[i].size());
    counts2[i].resize(array2[i].size());
    size_t array2Size = array1[i].size();
    for(size_t j = 0; j < array2Size; j++)
    {
      size_t array3Size = array1[i][j].size();
      for(size_t k = 0; k < array3Size; k++)
      {
        counts1[i][j] += array1[i][j][k];
        counts2[i][j] += array2[i][j][k];
      }
    }
  }
  for(size_t i = 0; i < array1Size; i++)
  {
    size_t array2Size = array1[i].size();
    for(size_t j = 0; j < array2Size; j++)
    {
      size_t array3Size = array1[i][j].size();
      for(size_t k = 0; k < array3Size; k++)
      {
        bhattdist = bhattdist + sqrtf(((array1[i][j][k] / counts1[i][j]) * (array2[i][j][k] / counts2[i][j])));
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float InsertPrecipitatePhases::check_sizedisterror(Precip_t* precip)
{
  // DataContainer::Pointer m =
  // getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  StatsDataArray& statsDataArray = *(m_StatsDataArray.lock());

  float dia;
  float sizedisterror = 0;
  float bhattdist;
  int index;
  int count = 0;
  int phase;
  size_t featureSizeDist_Size = m_FeatureSizeDist.size();
  for(size_t iter = 0; iter < featureSizeDist_Size; ++iter)
  {
    phase = m_PrecipitatePhases[iter];
    PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[phase].get());
    count = 0;
    std::vector<float>& curFeatureSizeDist = m_FeatureSizeDist[iter];
    std::vector<float>::size_type curFeatureSizeDistSize = curFeatureSizeDist.size();
    std::vector<float>& curSimFeatureSizeDist = m_SimFeatureSizeDist[iter];
    // Initialize all Values to Zero
    for(size_t i = 0; i < curFeatureSizeDistSize; i++)
    {
      curSimFeatureSizeDist[i] = 0.0f;
    }

    size_t nFeatureTuples = m_FeaturePhasesPtr.lock()->getNumberOfTuples();
    float oneOverCurFeatureSizeDistStep = 1.0f / m_FeatureSizeDistStep[iter];
    float halfMinFeatureDiameter = pp->getMinFeatureDiameter() * 0.5f;
    for(size_t b = m_FirstPrecipitateFeature; b < nFeatureTuples; b++)
    {
      index = b;
      if(m_FeaturePhases[index] == phase)
      {
        dia = m_EquivalentDiameters[index];
        dia = (dia - halfMinFeatureDiameter) * oneOverCurFeatureSizeDistStep;
        if(dia < 0)
        {
          dia = 0;
        }
        if(dia > curFeatureSizeDistSize - 1.0f)
        {
          dia = curFeatureSizeDistSize - 1.0f;
        }
        curSimFeatureSizeDist[int(dia)]++;
        count++;
      }
    }

    if(precip->m_FeaturePhases == phase)
    {
      dia = precip->m_EquivalentDiameters;
      dia = (dia - halfMinFeatureDiameter) * oneOverCurFeatureSizeDistStep;
      if(dia < 0)
      {
        dia = 0;
      }
      if(dia > curFeatureSizeDistSize - 1.0f)
      {
        dia = curFeatureSizeDistSize - 1.0f;
      }
      curSimFeatureSizeDist[int(dia)]++;
      count++;
    }
    float oneOverCount = 1.0f / count;

    if(count == 0)
    {
      for(size_t i = 0; i < curFeatureSizeDistSize; i++)
      {
        curSimFeatureSizeDist[i] = 0.0;
      }
    }
    else
    {
      for(size_t i = 0; i < curFeatureSizeDistSize; i++)
      {
        curSimFeatureSizeDist[i] = curSimFeatureSizeDist[i] * oneOverCount;
      }
    }
  }
  compare_2Ddistributions(m_SimFeatureSizeDist, m_FeatureSizeDist, bhattdist);
  sizedisterror = bhattdist;
  return sizedisterror;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::insert_precipitate(size_t gnum)
{
  SIMPL_RANDOMNG_NEW()

  float inside = -1.0f;
  int64_t column = 0, row = 0, plane = 0;
  int64_t centercolumn = 0, centerrow = 0, centerplane = 0;
  int64_t xmin = 0, xmax = 0, ymin = 0, ymax = 0, zmin = 0, zmax = 0;
  float xc = 0.0f, yc = 0.0f, zc = 0.0f;
  float coordsRotated[3] = {0.0f, 0.0f, 0.0f};
  float coords[3] = {0.0f, 0.0f, 0.0f};
  float volcur = m_Volumes[gnum];
  float bovera = m_AxisLengths[3 * gnum + 1];
  float covera = m_AxisLengths[3 * gnum + 2];
  float omega3 = m_Omega3s[gnum];
  float radcur1 = 1.0f;
  ShapeType::Type shapeclass = static_cast<ShapeType::Type>(m_ShapeTypes[m_FeaturePhases[gnum]]);

  // Bail if the shapeclass is not one of our enumerated types
  if(shapeclass >= ShapeType::Type::ShapeTypeEnd)
  {
    QString ss = QObject::tr("Undefined shape class in shape types array with path %1").arg(m_InputShapeTypesArrayPath.serialize());
    setErrorCondition(-667);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  // init any values for each of the Shape Ops
  for(int iter = 0; iter < m_ShapeOps.size(); iter++)
  {
    m_ShapeOps[iter]->init();
  }
  // Create our Argument Map
  QMap<ShapeOps::ArgName, float> shapeArgMap;
  shapeArgMap[ShapeOps::Omega3] = omega3;
  shapeArgMap[ShapeOps::VolCur] = volcur;
  shapeArgMap[ShapeOps::B_OverA] = bovera;
  shapeArgMap[ShapeOps::C_OverA] = covera;

  radcur1 = m_ShapeOps[static_cast<ShapeType::EnumType>(shapeclass)]->radcur1(shapeArgMap);

  // adjust radcur1 to make larger exclusion zone to prevent precipitate overlap
  radcur1 = radcur1 * 2.0f;

  float radcur2 = (radcur1 * bovera);
  float radcur3 = (radcur1 * covera);
  float ga[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  FOrientArrayType om(9, 0.0);
  FOrientTransformsType::eu2om(FOrientArrayType(&(m_AxisEulerAngles[3 * gnum]), 3), om);
  om.toGMatrix(ga);

  xc = m_Centroids[3 * gnum];
  yc = m_Centroids[3 * gnum + 1];
  zc = m_Centroids[3 * gnum + 2];
  centercolumn = static_cast<int64_t>((xc - (m_XRes / 2)) / m_XRes);
  centerrow = static_cast<int64_t>((yc - (m_YRes / 2)) / m_YRes);
  centerplane = static_cast<int64_t>((zc - (m_ZRes / 2)) / m_ZRes);
  xmin = int64_t(centercolumn - ((radcur1 / m_XRes) + 1));
  xmax = int64_t(centercolumn + ((radcur1 / m_XRes) + 1));
  ymin = int64_t(centerrow - ((radcur1 / m_YRes) + 1));
  ymax = int64_t(centerrow + ((radcur1 / m_YRes) + 1));
  zmin = int64_t(centerplane - ((radcur1 / m_ZRes) + 1));
  zmax = int64_t(centerplane + ((radcur1 / m_ZRes) + 1));
  if(xmin < -m_XPoints)
  {
    xmin = -m_XPoints;
  }
  if(xmax > 2 * m_XPoints - 1)
  {
    xmax = (2 * m_XPoints - 1);
  }
  if(ymin < -m_YPoints)
  {
    ymin = -m_YPoints;
  }
  if(ymax > 2 * m_YPoints - 1)
  {
    ymax = (2 * m_YPoints - 1);
  }
  if(zmin < -m_ZPoints)
  {
    zmin = -m_ZPoints;
  }
  if(zmax > 2 * m_ZPoints - 1)
  {
    zmax = (2 * m_ZPoints - 1);
  }
  for(int64_t iter1 = xmin; iter1 < xmax + 1; iter1++)
  {
    for(int64_t iter2 = ymin; iter2 < ymax + 1; iter2++)
    {
      for(int64_t iter3 = zmin; iter3 < zmax + 1; iter3++)
      {
        column = iter1;
        row = iter2;
        plane = iter3;
        coords[0] = float(column) * m_XRes;
        coords[1] = float(row) * m_YRes;
        coords[2] = float(plane) * m_ZRes;
        inside = -1;
        coords[0] = coords[0] - xc;
        coords[1] = coords[1] - yc;
        coords[2] = coords[2] - zc;
        MatrixMath::Multiply3x3with3x1(ga, coords, coordsRotated);
        float axis1comp = coordsRotated[0] / radcur1;
        float axis2comp = coordsRotated[1] / radcur2;
        float axis3comp = coordsRotated[2] / radcur3;
        inside = m_ShapeOps[static_cast<ShapeType::EnumType>(shapeclass)]->inside(axis1comp, axis2comp, axis3comp);
        if(inside >= 0)
        {
          m_ColumnList[gnum].push_back(column);
          m_RowList[gnum].push_back(row);
          m_PlaneList[gnum].push_back(plane);
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::assign_voxels()
{

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  size_t udims[3] = {0, 0, 0};
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);

  int64_t dims[3] = {
      static_cast<int64_t>(udims[0]), static_cast<int64_t>(udims[1]), static_cast<int64_t>(udims[2]),
  };

  int64_t index;

  float totalPoints = dims[0] * dims[1] * dims[2];
  float xRes = m->getGeometryAs<ImageGeom>()->getXRes();
  float yRes = m->getGeometryAs<ImageGeom>()->getYRes();
  float zRes = m->getGeometryAs<ImageGeom>()->getZRes();

  int64_t column = 0, row = 0, plane = 0;
  float inside = 0.0f;
  float xc = 0.0f, yc = 0.0f, zc = 0.0f;
  float coordsRotated[3] = {0.0f, 0.0f, 0.0f};
  float coords[3] = {0.0f, 0.0f, 0.0f};
  int64_t xmin = 0, xmax = 0, ymin = 0, ymax = 0, zmin = 0, zmax = 0;
  size_t numFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();
  m_GSizes.resize(numFeatures);

  for(size_t i = size_t(m_FirstPrecipitateFeature); i < numFeatures; i++)
  {
    m_GSizes[i] = 0;
  }
  for(size_t i = size_t(m_FirstPrecipitateFeature); i < numFeatures; i++)
  {
    float volcur = m_Volumes[i];
    float bovera = m_AxisLengths[3 * i + 1];
    float covera = m_AxisLengths[3 * i + 2];
    float omega3 = m_Omega3s[i];
    xc = m_Centroids[3 * i];
    yc = m_Centroids[3 * i + 1];
    zc = m_Centroids[3 * i + 2];
    float radcur1 = 0.0f;
    // Unbounded Check for the size of shapeTypes. We assume a 1:1 with phase ;
    // this has been checked in insert_precipitate
    ShapeType::Type shapeclass = static_cast<ShapeType::Type>(m_ShapeTypes[m_FeaturePhases[i]]);

    // init any values for each of the Shape Ops
    for(size_t iter = 0; iter < m_ShapeOps.size(); iter++)
    {
      m_ShapeOps[iter]->init();
    }
    // Create our Argument Map
    QMap<ShapeOps::ArgName, float> shapeArgMap;
    shapeArgMap[ShapeOps::Omega3] = omega3;
    shapeArgMap[ShapeOps::VolCur] = volcur;
    shapeArgMap[ShapeOps::B_OverA] = bovera;
    shapeArgMap[ShapeOps::C_OverA] = covera;

    radcur1 = m_ShapeOps[static_cast<ShapeType::EnumType>(shapeclass)]->radcur1(shapeArgMap);

    float radcur2 = (radcur1 * bovera);
    float radcur3 = (radcur1 * covera);
    float ga[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    FOrientArrayType om(9, 0.0);
    FOrientTransformsType::eu2om(FOrientArrayType(&(m_AxisEulerAngles[3 * i]), 3), om);
    om.toGMatrix(ga);

    column = static_cast<int64_t>((xc - (xRes / 2.0f)) / xRes);
    row = static_cast<int64_t>((yc - (yRes / 2.0f)) / yRes);
    plane = static_cast<int64_t>((zc - (zRes / 2.0f)) / zRes);
    xmin = static_cast<int64_t>(column - ((radcur1 / xRes) + 1));
    xmax = static_cast<int64_t>(column + ((radcur1 / xRes) + 1));
    ymin = static_cast<int64_t>(row - ((radcur1 / yRes) + 1));
    ymax = static_cast<int64_t>(row + ((radcur1 / yRes) + 1));
    zmin = static_cast<int64_t>(plane - ((radcur1 / zRes) + 1));
    zmax = static_cast<int64_t>(plane + ((radcur1 / zRes) + 1));
    if(m_PeriodicBoundaries == true)
    {
      if(xmin < -dims[0])
      {
        xmin = -dims[0];
      }
      if(xmax > 2 * dims[0] - 1)
      {
        xmax = (2 * dims[0] - 1);
      }
      if(ymin < -dims[1])
      {
        ymin = -dims[1];
      }
      if(ymax > 2 * dims[1] - 1)
      {
        ymax = (2 * dims[1] - 1);
      }
      if(zmin < -dims[2])
      {
        zmin = -dims[2];
      }
      if(zmax > 2 * dims[2] - 1)
      {
        zmax = (2 * dims[2] - 1);
      }
    }
    if(m_PeriodicBoundaries == false)
    {
      if(xmin < 0)
      {
        xmin = 0;
      }
      if(xmax > dims[0] - 1)
      {
        xmax = dims[0] - 1;
      }
      if(ymin < 0)
      {
        ymin = 0;
      }
      if(ymax > dims[1] - 1)
      {
        ymax = dims[1] - 1;
      }
      if(zmin < 0)
      {
        zmin = 0;
      }
      if(zmax > dims[2] - 1)
      {
        zmax = dims[2] - 1;
      }
    }
    for(int64_t iter1 = xmin; iter1 < xmax + 1; iter1++)
    {
      for(int64_t iter2 = ymin; iter2 < ymax + 1; iter2++)
      {
        for(int64_t iter3 = zmin; iter3 < zmax + 1; iter3++)
        {
          column = iter1;
          row = iter2;
          plane = iter3;
          if(iter1 < 0)
          {
            column = iter1 + dims[0];
          }
          if(iter1 > dims[0] - 1)
          {
            column = iter1 - dims[0];
          }
          if(iter2 < 0)
          {
            row = iter2 + dims[1];
          }
          if(iter2 > dims[1] - 1)
          {
            row = iter2 - dims[1];
          }
          if(iter3 < 0)
          {
            plane = iter3 + dims[2];
          }
          if(iter3 > dims[2] - 1)
          {
            plane = iter3 - dims[2];
          }
          index = (plane * dims[0] * dims[1]) + (row * dims[0]) + column;
          inside = -1.0f;
          coords[0] = float(column) * xRes;
          coords[1] = float(row) * yRes;
          coords[2] = float(plane) * zRes;
          if(iter1 < 0)
          {
            coords[0] = coords[0] - m_SizeX;
          }
          if(iter1 > dims[0] - 1)
          {
            coords[0] = coords[0] + m_SizeX;
          }
          if(iter2 < 0)
          {
            coords[1] = coords[1] - m_SizeY;
          }
          if(iter2 > dims[1] - 1)
          {
            coords[1] = coords[1] + m_SizeY;
          }
          if(iter3 < 0)
          {
            coords[2] = coords[2] - m_SizeZ;
          }
          if(iter3 > dims[2] - 1)
          {
            coords[2] = coords[2] + m_SizeZ;
          }
          coords[0] = coords[0] - xc;
          coords[1] = coords[1] - yc;
          coords[2] = coords[2] - zc;
          MatrixMath::Multiply3x3with3x1(ga, coords, coordsRotated);
          float axis1comp = coordsRotated[0] / radcur1;
          float axis2comp = coordsRotated[1] / radcur2;
          float axis3comp = coordsRotated[2] / radcur3;
          inside = m_ShapeOps[static_cast<ShapeType::EnumType>(shapeclass)]->inside(axis1comp, axis2comp, axis3comp);
          if(inside >= 0)
          {
            int64_t currentpoint = index;
            if(m_FeatureIds[currentpoint] > m_FirstPrecipitateFeature)
            {
              // oldname = m_FeatureIds[currentpoint];
              m_FeatureIds[currentpoint] = -2;
            }
            if(m_FeatureIds[currentpoint] < m_FirstPrecipitateFeature && m_FeatureIds[currentpoint] != -2)
            {
              m_FeatureIds[currentpoint] = static_cast<int32_t>(i);
            }
          }
        }
      }
    }
  }

  QVector<bool> activeObjects(numFeatures, false);
  int32_t gnum = 0;
  for(size_t i = 0; i < static_cast<size_t>(totalPoints); i++)
  {
    if(m_UseMask == true && m_Mask[i] == false)
    {
      m_FeatureIds[i] = 0;
    }
    gnum = m_FeatureIds[i];
    if(gnum >= 0)
    {
      activeObjects[gnum] = true;
    }
  }

  AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(getFeaturePhasesArrayPath().getAttributeMatrixName());
  cellFeatureAttrMat->removeInactiveObjects(activeObjects, m_FeatureIdsPtr.lock());
  // need to update pointers after resize, but do not need to run full data
  // check because pointers are still valid
  updateFeatureInstancePointers();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::assign_gaps()
{

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getFeatureIdsArrayPath().getDataContainerName());

  int32_t featurename = 0, feature = 0;
  int32_t current = 0;
  int32_t most = 0;
  int64_t gapVoxelCount = 1;
  int32_t iterationCounter = 0;
  int64_t neighpoint;
  bool good = false;
  int32_t neighbor = 0;

  int64_t xPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getXPoints());
  int64_t yPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getYPoints());
  int64_t zPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getZPoints());
  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  size_t totalFeatures = m->getAttributeMatrix(getFeaturePhasesArrayPath().getAttributeMatrixName())->getNumberOfTuples();

  int64_t neighpoints[6] = {0, 0, 0, 0, 0, 0};
  neighpoints[0] = -xPoints * yPoints;
  neighpoints[1] = -xPoints;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = xPoints;
  neighpoints[5] = xPoints * yPoints;

  Int64ArrayType::Pointer neighborsPtr = Int64ArrayType::CreateArray(m->getGeometryAs<ImageGeom>()->getNumberOfElements(), "_INTERNAL_USE_ONLY_Neighbors");
  neighborsPtr->initializeWithValue(-1);
  m_Neighbors = neighborsPtr->getPointer(0);

  std::vector<int32_t> n(totalFeatures + 1, 0);

  while(gapVoxelCount != 0)
  {
    iterationCounter++;
    gapVoxelCount = 0;
    int64_t zStride, yStride;
    for(int64_t i = 0; i < zPoints; i++)
    {
      zStride = i * xPoints * yPoints;
      for(int64_t j = 0; j < yPoints; j++)
      {
        yStride = j * xPoints;
        for(int64_t k = 0; k < xPoints; k++)
        {
          featurename = m_FeatureIds[zStride + yStride + k];
          if(featurename < 0)
          {
            gapVoxelCount++;
            current = 0;
            most = 0;
            for(int32_t l = 0; l < 6; l++)
            {
              good = true;
              neighpoint = zStride + yStride + k + neighpoints[l];
              if(l == 0 && i == 0)
              {
                good = false;
              }
              if(l == 5 && i == (zPoints - 1))
              {
                good = false;
              }
              if(l == 1 && j == 0)
              {
                good = false;
              }
              if(l == 4 && j == (yPoints - 1))
              {
                good = false;
              }
              if(l == 2 && k == 0)
              {
                good = false;
              }
              if(l == 3 && k == (xPoints - 1))
              {
                good = false;
              }
              if(good == true)
              {
                feature = m_FeatureIds[neighpoint];
                if(feature > 0)
                {
                  n[feature]++;
                  current = n[feature];
                  if(current > most)
                  {
                    most = current;
                    m_Neighbors[zStride + yStride + k] = neighpoint;
                  }
                }
              }
            }
            for(int32_t l = 0; l < 6; l++)
            {
              good = true;
              neighpoint = zStride + yStride + k + neighpoints[l];
              if(l == 0 && i == 0)
              {
                good = false;
              }
              if(l == 5 && i == (zPoints - 1))
              {
                good = false;
              }
              if(l == 1 && j == 0)
              {
                good = false;
              }
              if(l == 4 && j == (yPoints - 1))
              {
                good = false;
              }
              if(l == 2 && k == 0)
              {
                good = false;
              }
              if(l == 3 && k == (xPoints - 1))
              {
                good = false;
              }
              if(good == true)
              {
                feature = m_FeatureIds[neighpoint];
                if(feature > 0)
                {
                  n[feature] = 0;
                }
              }
            }
          }
        }
      }
    }
    for(size_t j = 0; j < totalPoints; j++)
    {
      featurename = m_FeatureIds[j];
      neighbor = m_Neighbors[j];
      if(featurename < 0 && neighbor != -1 && m_FeatureIds[neighbor] > 0)
      {
        m_FeatureIds[j] = m_FeatureIds[neighbor];
      }
    }
    if(iterationCounter >= 1)
    {
      QString ss = QObject::tr("Assign Gaps || Cycle#: %1 || Remaining "
                               "Unassigned Voxel Count: %2")
                       .arg(iterationCounter)
                       .arg(gapVoxelCount);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    }
    if(getCancel() == true)
    {
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float InsertPrecipitatePhases::find_xcoord(int64_t index)
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  float x = m->getGeometryAs<ImageGeom>()->getXRes() * float(index % m->getGeometryAs<ImageGeom>()->getXPoints());
  return x;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float InsertPrecipitatePhases::find_ycoord(int64_t index)
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  float y = m->getGeometryAs<ImageGeom>()->getYRes() * float((index / m->getGeometryAs<ImageGeom>()->getXPoints()) % m->getGeometryAs<ImageGeom>()->getYPoints());
  return y;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float InsertPrecipitatePhases::find_zcoord(int64_t index)
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  float z = m->getGeometryAs<ImageGeom>()->getZRes() * float(index / (m->getGeometryAs<ImageGeom>()->getXPoints() * m->getGeometryAs<ImageGeom>()->getYPoints()));
  return z;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::write_goal_attributes()
{
  int err = 0;
  setErrorCondition(err);
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  // Make sure any directory path is also available as the user may have just
  // typed
  // in a path without actually creating the full path

  QFileInfo fi(m_CsvOutputFile);
  QString parentPath = fi.path();
  QDir dir;
  if(!dir.mkpath(parentPath))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath);
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QFile outFile(getCsvOutputFile());
  if(!outFile.open(QIODevice::WriteOnly))
  {
    QString msg = QObject::tr("CSV Output file could not be opened: %1").arg(getCsvOutputFile());
    setErrorCondition(-200);
    notifyErrorMessage(getHumanLabel(), "", getErrorCondition());
    return;
  }

  int64_t numFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();

  QTextStream dStream(&outFile);

  char space = ',';
  // Write the total number of features
  dStream << static_cast<qint32>(numFeatures - m_FirstPrecipitateFeature);
  // Get all the names of the arrays from the Data Container
  QList<QString> headers = m->getAttributeMatrix(getFeaturePhasesArrayPath().getAttributeMatrixName())->getAttributeArrayNames();

  std::vector<IDataArray::Pointer> data;

  // For checking if an array is a neighborlist
  NeighborList<float>::Pointer neighborlistPtr = NeighborList<float>::New();

  // Print the FeatureIds Header before the rest of the headers
  dStream << SIMPL::FeatureData::FeatureID;
  // Loop throught the list and print the rest of the headers, ignoring those we
  // don't want
  for(QList<QString>::iterator iter = headers.begin(); iter != headers.end(); ++iter)
  {
    // Only get the array if the name does NOT match those listed
    IDataArray::Pointer p = m->getAttributeMatrix(getFeaturePhasesArrayPath().getAttributeMatrixName())->getAttributeArray(*iter);
    if(p->getNameOfClass().compare(neighborlistPtr->getNameOfClass()) != 0)
    {
      if(p->getNumberOfComponents() == 1)
      {
        dStream << space << (*iter);
      }
      else // There are more than a single component so we need to add
           // multiple header values
      {
        for(int k = 0; k < p->getNumberOfComponents(); ++k)
        {
          dStream << space << (*iter) << "_" << k;
        }
      }
      // Get the IDataArray from the DataContainer
      data.push_back(p);
    }
  }
  dStream << "\n";

  // Get the number of tuples in the arrays
  size_t numTuples = data[0]->getNumberOfTuples();

  float threshold = 0.0f;

  // Skip the first feature
  for(size_t i = m_FirstPrecipitateFeature; i < numTuples; ++i)
  {
    if(((float)i / numTuples) * 100.0f > threshold)
    {
      QString ss = QObject::tr("Writing Feature Data - %1% Complete").arg(((float)i / numTuples) * 100);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
      threshold = threshold + 5.0f;
      if(threshold < ((float)i / numTuples) * 100.0f)
      {
        threshold = ((float)i / numTuples) * 100.0f;
      }
    }

    // Print the feature id
    dStream << i;
    // Print a row of data
    for(size_t p = 0; p < data.size(); ++p)
    {
      dStream << space;
      data[p]->printTuple(dStream, i, space);
    }
    dStream << "\n";
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::moveShapeDescriptions()
{
  QStringList names;
  names << m_EquivalentDiametersArrayName << m_Omega3sArrayName << m_AxisEulerAnglesArrayName << m_AxisLengthsArrayName << m_VolumesArrayName << m_CentroidsArrayName;

  AttributeMatrix::Pointer cellFeatureAttrMat = getDataContainerArray()->getAttributeMatrix(getFeaturePhasesArrayPath());

  QList<IDataArray::Pointer> attrArrays;
  foreach(const QString name, names)
  {
    IDataArray::Pointer arrayPtr = cellFeatureAttrMat->removeAttributeArray(name);
    if(arrayPtr != IDataArray::NullPointer())
    {
      attrArrays.push_back(arrayPtr);
    }
  }

  InsertPrecipitatePhases::SaveMethod saveMethod = static_cast<InsertPrecipitatePhases::SaveMethod>(getSaveGeometricDescriptions());
  if(saveMethod == InsertPrecipitatePhases::SaveMethod::SaveToNew)
  {
    saveToNewAttributeMatrix(attrArrays);
  }
  else if(saveMethod == InsertPrecipitatePhases::SaveMethod::AppendToExisting)
  {
    appendToExistingAttributeMatrix(attrArrays);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::saveToNewAttributeMatrix(QList<IDataArray::Pointer> incomingArrays)
{
  QVector<size_t> tDims(1, 0);

  AttributeMatrix::Pointer newAM = getDataContainerArray()->getAttributeMatrix(getNewAttributeMatrixPath());
  if(newAM != AttributeMatrix::NullPointer())
  {
    if(incomingArrays.size() > 0)
    {
      size_t incomingArrayTupleCount = incomingArrays[0]->getNumberOfTuples();
      size_t newAMTupleCount = newAM->getTupleDimensions()[0];
      tDims[0] = incomingArrayTupleCount + newAMTupleCount;
      newAM->resizeAttributeArrays(tDims);
    }

    foreach(IDataArray::Pointer incomingArray, incomingArrays)
    {
      newAM->addAttributeArray(incomingArray->getName(), incomingArray);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::appendToExistingAttributeMatrix(QList<IDataArray::Pointer> incomingArrays)
{
  QVector<size_t> tDims(1, 0);

  AttributeMatrix::Pointer existingAM = getDataContainerArray()->getAttributeMatrix(getSelectedAttributeMatrixPath());
  if(existingAM != AttributeMatrix::NullPointer())
  {
    size_t existingAMTupleCount = existingAM->getTupleDimensions()[0];
    if(incomingArrays.size() > 0)
    {
      size_t incomingArrayTupleCount = incomingArrays[0]->getNumberOfTuples();
      tDims[0] = incomingArrayTupleCount;
      existingAM->resizeAttributeArrays(tDims);
    }

    foreach(IDataArray::Pointer incomingArray, incomingArrays)
    {
      int err = 0;
      IDataArray::Pointer existingArray = existingAM->getPrereqIDataArray<IDataArray, AbstractFilter>(this, incomingArray->getName(), err);
      if(existingArray != IDataArray::NullPointer())
      {
        // ATTENTION: This "append" code is making the assumption that there will be 0's preceding the precipitate phase data.  The number of preceding
        // 0's is equal to the number of elements in the existing array that will be appended to.  If this changes, then this "append" code needs to be updated!
        existingArray->copyFromArray(existingAMTupleCount, incomingArray, existingAMTupleCount, existingArray->getNumberOfTuples() - existingAMTupleCount);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer InsertPrecipitatePhases::newFilterInstance(bool copyFilterParameters)
{
  InsertPrecipitatePhases::Pointer filter = InsertPrecipitatePhases::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InsertPrecipitatePhases::getCompiledLibraryName()
{
  return SyntheticBuildingConstants::SyntheticBuildingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InsertPrecipitatePhases::getBrandingString()
{
  return "SyntheticBuilding";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InsertPrecipitatePhases::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << SyntheticBuilding::Version::Major() << "." << SyntheticBuilding::Version::Minor() << "." << SyntheticBuilding::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InsertPrecipitatePhases::getGroupName()
{
  return SIMPL::FilterGroups::SyntheticBuildingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InsertPrecipitatePhases::getSubGroupName()
{
  return SIMPL::FilterSubGroups::PackingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InsertPrecipitatePhases::getHumanLabel()
{
  return "Insert Precipitate Phases";
}
