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

#include "InsertPrecipitatePhases.h"

#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "OrientationLib/Math/OrientationMath.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"
#include "SyntheticBuilding/ShapeOps/CubeOctohedronOps.h"
#include "SyntheticBuilding/ShapeOps/CylinderOps.h"
#include "SyntheticBuilding/ShapeOps/EllipsoidOps.h"
#include "SyntheticBuilding/ShapeOps/SuperEllipsoidOps.h"
#include "DREAM3DLib/StatsData/PrecipitateStatsData.h"
#include "DREAM3DLib/Math/RadialDistributionFunction.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InsertPrecipitatePhases::InsertPrecipitatePhases() :
  AbstractFilter(),
  m_ClusteringListArrayName(DREAM3D::FeatureData::ClusteringList),
  m_ErrorOutputFile(""),
  m_CsvOutputFile(""),
  m_HavePrecips(false),
  m_PrecipInputFile(""),
  m_PeriodicBoundaries(false),
  m_MatchRDF(false),
  m_WriteGoalAttributes(false),
  m_InputStatsArrayPath(DREAM3D::Defaults::StatsGenerator, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::Statistics),
  m_InputPhaseTypesArrayPath(DREAM3D::Defaults::StatsGenerator, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::PhaseTypes),
  m_InputShapeTypesArrayPath(DREAM3D::Defaults::StatsGenerator, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::ShapeTypes),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_CellPhasesArrayPath(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Phases),
  m_BoundaryCellsArrayPath(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::BoundaryCells),
  m_FeaturePhasesArrayPath(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_NumCellsArrayName(DREAM3D::FeatureData::NumCells),
  m_EquivalentDiametersArrayName(DREAM3D::FeatureData::EquivalentDiameters),
  m_VolumesArrayName(DREAM3D::FeatureData::Volumes),
  m_Omega3sArrayName(DREAM3D::FeatureData::Omega3s),
  m_CentroidsArrayName(DREAM3D::FeatureData::Centroids),
  m_AxisEulerAnglesArrayName(DREAM3D::FeatureData::AxisEulerAngles),
  m_AxisLengthsArrayName(DREAM3D::FeatureData::AxisLengths),
  m_NumFeaturesArrayPath(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::NumFeatures),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CellPhases(NULL),
  m_BoundaryCellsArrayName(DREAM3D::CellData::BoundaryCells),
  m_BoundaryCells(NULL),
  m_AxisEulerAngles(NULL),
  m_Centroids(NULL),
  m_AxisLengths(NULL),
  m_Volumes(NULL),
  m_Omega3s(NULL),
  m_EquivalentDiameters(NULL),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_FeaturePhases(NULL),
  m_NumCells(NULL),
  m_PhaseTypesArrayName(DREAM3D::EnsembleData::PhaseTypes),
  m_PhaseTypes(NULL),
  m_ShapeTypesArrayName(DREAM3D::EnsembleData::ShapeTypes),
  m_ShapeTypes(NULL),
  m_NumFeaturesArrayName(DREAM3D::EnsembleData::NumFeatures),
  m_NumFeatures(NULL)
{
  m_EllipsoidOps = EllipsoidOps::New();
  m_ShapeOps[DREAM3D::ShapeType::EllipsoidShape] = m_EllipsoidOps.get();
  m_SuperEllipsoidOps = SuperEllipsoidOps::New();
  m_ShapeOps[DREAM3D::ShapeType::SuperEllipsoidShape] = m_SuperEllipsoidOps.get();
  m_CubicOctohedronOps = CubeOctohedronOps::New();
  m_ShapeOps[DREAM3D::ShapeType::CubeOctahedronShape] = m_CubicOctohedronOps.get();
  m_CylinderOps = CylinderOps::New();
  m_ShapeOps[DREAM3D::ShapeType::CylinderShape] = m_CylinderOps.get();
  m_UnknownShapeOps = ShapeOps::New();
  m_ShapeOps[DREAM3D::ShapeType::UnknownShapeType] = m_UnknownShapeOps.get();

  m_OrthoOps = OrthoRhombicOps::New();

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
  parameters.push_back(FilterParameter::New("Periodic Boundary", "PeriodicBoundaries", FilterParameterWidgetType::BooleanWidget, getPeriodicBoundaries(), false));
  parameters.push_back(FilterParameter::New("Match Radial Distribution Function", "MatchRDF", FilterParameterWidgetType::BooleanWidget, getMatchRDF(), false));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Statistics Array", "InputStatsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getInputStatsArrayPath(), true));
  parameters.push_back(FilterParameter::New("Phase Types Array", "InputPhaseTypesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getInputPhaseTypesArrayPath(), true));
  parameters.push_back(FilterParameter::New("Shape Types Array", "InputShapeTypesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getInputShapeTypesArrayPath(), true));
  parameters.push_back(FilterParameter::New("FeatureIds", "FeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeatureIdsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Cell Phases", "CellPhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCellPhasesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("BoundaryCells", "BoundaryCellsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getBoundaryCellsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("FeaturePhases", "FeaturePhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeaturePhasesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("NumFeatures", "NumFeaturesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getNumFeaturesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  QStringList linkedProps("PrecipInputFile");
  parameters.push_back(LinkedBooleanFilterParameter::New("Already Have Precipitates", "HavePrecips", getHavePrecips(), linkedProps, false));
  parameters.push_back(FileSystemFilterParameter::New("Precipitate Input File", "PrecipInputFile", FilterParameterWidgetType::InputFileWidget, getPrecipInputFile(), false, "", "*.txt", "Text File"));
  linkedProps.clear();
  linkedProps << "CsvOutputFile";
  parameters.push_back(LinkedBooleanFilterParameter::New("Write Goal Attributes", "WriteGoalAttributes", getWriteGoalAttributes(), linkedProps, false));
  parameters.push_back(FileSystemFilterParameter::New("Goal Attribute CSV File", "CsvOutputFile", FilterParameterWidgetType::OutputFileWidget, getCsvOutputFile(), false, "", "*.csv", "Comma Separated Data"));

  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNumFeaturesArrayPath(reader->readDataArrayPath("NumFeaturesArrayPath", getNumFeaturesArrayPath() ) );
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() ) );
  setInputStatsArrayPath(reader->readDataArrayPath("InputStatsArrayPath", getInputStatsArrayPath() ) );
  setInputPhaseTypesArrayPath(reader->readDataArrayPath("InputPhaseTypesArrayPath", getInputPhaseTypesArrayPath() ) );
  setInputShapeTypesArrayPath(reader->readDataArrayPath("InputShapeTypesArrayPath", getInputShapeTypesArrayPath() ) );
  setBoundaryCellsArrayPath(reader->readDataArrayPath("BoundaryCellsArrayPath", getBoundaryCellsArrayPath() ) );
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setPeriodicBoundaries( reader->readValue("PeriodicBoundaries", getPeriodicBoundaries()) );
  setMatchRDF(reader->readValue("MatchRDF", getMatchRDF()));
  setHavePrecips( reader->readValue("HavePrecips", getHavePrecips()) );
  setPrecipInputFile( reader->readString( "PrecipInputFile", getPrecipInputFile() ) );
  setWriteGoalAttributes( reader->readValue("WriteGoalAttributes", getWriteGoalAttributes()) );
  setCsvOutputFile( reader->readString( "CsvOutputFile", getCsvOutputFile() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int InsertPrecipitatePhases::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(NumFeaturesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeaturePhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(InputStatsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(InputPhaseTypesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(InputShapeTypesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(BoundaryCellsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CellPhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(PeriodicBoundaries)
  DREAM3D_FILTER_WRITE_PARAMETER(MatchRDF)
  DREAM3D_FILTER_WRITE_PARAMETER(HavePrecips)
  DREAM3D_FILTER_WRITE_PARAMETER(WriteGoalAttributes)
  DREAM3D_FILTER_WRITE_PARAMETER(PrecipInputFile)
  DREAM3D_FILTER_WRITE_PARAMETER(CsvOutputFile)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::updateFeatureInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_NumCellsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NumCells = m_NumCellsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_EquivalentDiametersPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_EquivalentDiameters = m_EquivalentDiametersPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_VolumesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Volumes = m_VolumesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_Omega3sPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Omega3s = m_Omega3sPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_CentroidsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Centroids = m_CentroidsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_AxisEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AxisEulerAngles = m_AxisEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_AxisLengthsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AxisLengths = m_AxisLengthsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  // Make sure we have our input DataContainer with the proper Ensemble data
  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, m_FeatureIdsArrayPath.getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }

  ImageGeom::Pointer image = m->getPrereqGeometry<ImageGeom, AbstractFilter>(this);
  if(getErrorCondition() < 0 || NULL == image.get()) { return; }

  //Input Ensemble Data That we require
//  typedef DataArray<unsigned int> PhaseTypeArrayType;
//  typedef DataArray<unsigned int> ShapeTypeArrayType;

  QVector<size_t> dims(1, 1);
  m_PhaseTypesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getInputPhaseTypesArrayPath(), dims);
  if( NULL != m_PhaseTypesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_PhaseTypes = m_PhaseTypesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_ShapeTypesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getInputShapeTypesArrayPath(), dims);
  if( NULL != m_ShapeTypesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_ShapeTypes = m_ShapeTypesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_StatsDataArray = getDataContainerArray()->getPrereqArrayFromPath<StatsDataArray, AbstractFilter>(this, getInputStatsArrayPath(), dims);
  if(m_StatsDataArray.lock() == NULL)
  {
    QString ss = QObject::tr("Stats Array Not Initialized correctly");
    setErrorCondition(-308);
    notifyErrorMessage(getHumanLabel(), ss, -308);
  }

  // Cell Data
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_BoundaryCellsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter>(this, getBoundaryCellsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_BoundaryCellsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_BoundaryCells = m_BoundaryCellsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */


  //Feature Data
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this,  getFeaturePhasesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getNumCellsArrayName() );
  m_NumCellsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this,  tempPath, 0, dims, getNumCellsArrayName()); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NumCellsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NumCells = m_NumCellsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getEquivalentDiametersArrayName() );
  m_EquivalentDiametersPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0, dims, getEquivalentDiametersArrayName()); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_EquivalentDiametersPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_EquivalentDiameters = m_EquivalentDiametersPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getVolumesArrayName() );
  m_VolumesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0, dims, getVolumesArrayName()); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_VolumesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Volumes = m_VolumesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getOmega3sArrayName() );
  m_Omega3sPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0, dims, getOmega3sArrayName()); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_Omega3sPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Omega3s = m_Omega3sPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 3;
  tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getCentroidsArrayName() );
  m_CentroidsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0, dims, getCentroidsArrayName()); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CentroidsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Centroids = m_CentroidsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getAxisEulerAnglesArrayName() );
  m_AxisEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0, dims, getAxisEulerAnglesArrayName()); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AxisEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AxisEulerAngles = m_AxisEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getAxisLengthsArrayName() );
  m_AxisLengthsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0, dims, getAxisLengthsArrayName()); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AxisLengthsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AxisLengths = m_AxisLengthsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  //Ensemble Data
  dims[0] = 1;
  m_NumFeaturesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this,  getNumFeaturesArrayPath(), 0, dims, getNumFeaturesArrayName()); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NumFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NumFeatures = m_NumFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
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

  if (m_WriteGoalAttributes == true && getCsvOutputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("%1 needs the Csv Output File Set and it was not.").arg(ClassName());
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-387);
  }

  if(m_HavePrecips == true && getPrecipInputFile().isEmpty() == true)
  {
    QString ss = QObject::tr(": The precipitate file must be set before executing this filter.");
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
  }

  AttributeMatrix::Pointer attrMat = getDataContainerArray()->getAttributeMatrix(getFeaturePhasesArrayPath());
  if(attrMat == NULL) { setInPreflight(false); return; }

  attrMat->removeAttributeArray(m_EquivalentDiametersArrayName);
  attrMat->removeAttributeArray(m_Omega3sArrayName);
  attrMat->removeAttributeArray(m_AxisEulerAnglesArrayName);
  attrMat->removeAttributeArray(m_AxisLengthsArrayName);
  attrMat->removeAttributeArray(m_VolumesArrayName);
  attrMat->removeAttributeArray(m_CentroidsArrayName);
  attrMat->removeAttributeArray(m_NumCellsArrayName);
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::execute()
{
  int err = 0;
  setErrorCondition(err);
  DREAM3D_RANDOMNG_NEW()
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  //int index;
  size_t udims[3] = {0, 0, 0};
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  m_TotalPoints = dims[0] * dims[1] * dims[2];

  if(m_HavePrecips == false)
  {
    notifyStatusMessage(getHumanLabel(), "Packing Precipitates - Generating and Placing Precipitates");
    // this initializes the arrays to hold the details of the locations of all of the features during packing
    Int32ArrayType::Pointer exlusionZonesPtr = Int32ArrayType::CreateArray(m_TotalPoints, "PackPrimaryFeatures::exclusion_zones");
    exlusionZonesPtr->initializeWithZeros();

    place_precipitates(exlusionZonesPtr);
  }

  if(m_HavePrecips == true)
  {
    load_precipitates();
  }

  notifyStatusMessage(getHumanLabel(), "Packing Precipitates - Assigning Voxels");
  assign_voxels();

  notifyStatusMessage(getHumanLabel(), "Packing Precipitates - Filling Gaps");
  assign_gaps();

  if(m_WriteGoalAttributes == true)
  {
    write_goal_attributes();
  }

  AttributeMatrix::Pointer cellFeatureAttrMat = getDataContainerArray()->getAttributeMatrix(getFeaturePhasesArrayPath());
  cellFeatureAttrMat->removeAttributeArray(m_EquivalentDiametersArrayName);
  cellFeatureAttrMat->removeAttributeArray(m_Omega3sArrayName);
  cellFeatureAttrMat->removeAttributeArray(m_AxisEulerAnglesArrayName);
  cellFeatureAttrMat->removeAttributeArray(m_AxisLengthsArrayName);
  cellFeatureAttrMat->removeAttributeArray(m_VolumesArrayName);
  cellFeatureAttrMat->removeAttributeArray(m_CentroidsArrayName);
  cellFeatureAttrMat->removeAttributeArray(m_NumCellsArrayName);

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "InsertPrecipitatePhases Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void  InsertPrecipitatePhases::load_precipitates()
{

  //DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());
  AttributeMatrix::Pointer cellFeatureAttrMat = getDataContainerArray()->getAttributeMatrix(getFeaturePhasesArrayPath());

  std::ifstream inFile;
  inFile.open(getPrecipInputFile().toLatin1().data(), std::ios_base::binary);
  if(!inFile)
  {
    QString ss = QObject::tr("Failed to open: %1").arg(getPrecipInputFile());
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, -1);
  }
  int numPrecips;
  inFile >> numPrecips;
  if (0 == numPrecips)
  {
    notifyErrorMessage(getHumanLabel(), "The number of precipitates is Zero and should be greater than Zero", -600);
  }

  m_FirstPrecipitateFeature = cellFeatureAttrMat->getNumTuples();

  QVector<size_t> tDims(1, m_FirstPrecipitateFeature + numPrecips);
  cellFeatureAttrMat->setTupleDimensions(tDims);
  updateFeatureInstancePointers();

  int phase;
  float xC, yC, zC;
  float axisA, axisB, axisC;
  float vol, eqDiam;
  float omega3;
  float phi1, PHI, phi2;
  size_t currentFeature = m_FirstPrecipitateFeature;
  const float fourThirds = 4.0f / 3.0f;
  for(int i = 0; i < numPrecips; i++)
  {
    inFile >> phase >> xC >> yC >> zC >> axisA >> axisB >> axisC >> omega3 >> phi1 >> PHI >> phi2;
    vol = fourThirds * DREAM3D::Constants::k_Pi * axisA * axisB * axisC;
    eqDiam = 2.0f * powf((vol * (0.75f) * (DREAM3D::Constants::k_1OverPi)), (DREAM3D::Constants::k_1Over3));
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
void  InsertPrecipitatePhases::place_precipitates(Int32ArrayType::Pointer exclusionZonesPtr)
{
  bool writeErrorFile = true;
  bool write_test_outputs = true;

  std::ofstream outFile;
  if(m_ErrorOutputFile.isEmpty() == false)
  {
    outFile.open(m_ErrorOutputFile.toLatin1().data(), std::ios_base::binary);
    writeErrorFile = true;
  }

  notifyStatusMessage(getHumanLabel(), "Placing Precipitates");
  DREAM3D_RANDOMNG_NEW()

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  StatsDataArray& statsDataArray = *(m_StatsDataArray.lock());

  size_t udims[3] =
  { 0, 0, 0 };
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  { static_cast<DimType>(udims[0]), static_cast<DimType>(udims[1]), static_cast<DimType>(udims[2]), };

  m_XRes = m->getGeometryAs<ImageGeom>()->getXRes();
  m_YRes = m->getGeometryAs<ImageGeom>()->getYRes();
  m_ZRes = m->getGeometryAs<ImageGeom>()->getZRes();
  m_SizeX = dims[0] * m_XRes;
  m_SizeY = dims[1] * m_YRes;
  m_SizeZ = dims[2] * m_ZRes;
  m_TotalVol = m_SizeX * m_SizeY * m_SizeZ;
  m_XPoints = dims[0];
  m_YPoints = dims[1];
  m_ZPoints = dims[2];
  m_TotalPoints = dims[0] * dims[1] * dims[2];

  // figure out how many grains we already have so we can start the counter at +1 this

  int64_t currentnumfeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();
  int64_t numensembles = m_PhaseTypesPtr.lock()->getNumberOfTuples();
  QVector<size_t> tDims(1, 1);
  if(currentnumfeatures == 0)
  {
    m->getAttributeMatrix(getFeaturePhasesArrayPath().getAttributeMatrixName())->resizeAttributeArrays(tDims);
    updateFeatureInstancePointers();
    currentnumfeatures = 1;
  }
  m_FirstPrecipitateFeature = currentnumfeatures;
  int phase;
  float precipboundaryfraction = 0.0;
  float random = 0.0f;
  float xc, yc, zc;
  float oldxc, oldyc, oldzc;
  int randomfeature;
  int acceptedmoves = 0;
  double totalprecipitatefractions = 0.0;

  for (int64_t i = 1; i < numensembles; ++i)
  {
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
      m_NumFeatures[i] = 0;
      precipitatephases.push_back(i);
      precipitatephasefractions.push_back(pp->getPhaseFraction());
      totalprecipitatefractions = totalprecipitatefractions + pp->getPhaseFraction();
    }
  }

  for (size_t i = 0; i < precipitatephases.size(); i++)
  {
    precipitatephasefractions[i] = static_cast<float>(precipitatephasefractions[i] / totalprecipitatefractions);
  }


  //This is the set that we are going to keep updated with the points that are not in an exclusion zone
  std::map<size_t, size_t> availablePoints;
  std::map<size_t, size_t> availablePointsInv;

  // Get a pointer to the Feature Owners that was just initialized in the initialize_packinggrid() method
  int32_t* exclusionZones = exclusionZonesPtr->getPointer(0);
  size_t featureOwnersIdx = 0;

  // initialize the sim and goal size distributions for the precipitate phases
  featuresizedist.resize(precipitatephases.size());
  simfeaturesizedist.resize(precipitatephases.size());
  featuresizediststep.resize(precipitatephases.size());
  for (size_t i = 0; i < precipitatephases.size(); i++)
  {
    phase = precipitatephases[i];
    PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[phase].get());
    featuresizedist[i].resize(40);
    simfeaturesizedist[i].resize(40);
    featuresizediststep[i] = static_cast<float>(((2 * pp->getMaxFeatureDiameter()) - (pp->getMinFeatureDiameter() / 2.0)) / featuresizedist[i].size());
    float input = 0;
    float previoustotal = 0;
    VectorOfFloatArray GSdist = pp->getFeatureSizeDistribution();
    float avg = GSdist[0]->getValue(0);
    float stdev = GSdist[1]->getValue(0);
    float denominatorConst = sqrtf(2.0f * stdev * stdev); // Calculate it here rather than calculating the same thing multiple times below
    for (size_t j = 0; j < featuresizedist[i].size(); j++)
    {
      input = (float(j + 1) * featuresizediststep[i]) + (pp->getMinFeatureDiameter() / 2.0f);
      float logInput = logf(input);
      if(logInput <= avg)
      {
        featuresizedist[i][j] = 0.5f - 0.5f * (DREAM3DMath::erf((avg - logInput) / denominatorConst)) - previoustotal;
      }
      if(logInput > avg)
      {
        featuresizedist[i][j] = 0.5f + 0.5f * (DREAM3DMath::erf((logInput - avg) / denominatorConst)) - previoustotal;
      }
      previoustotal = previoustotal + featuresizedist[i][j];
    }
  }

  // adding precipitates until the volume fraction of precipitates matches the target (while making sure to keep the size distribution error within tolerance)
  Precip precip;
  std::vector<float> curphasevol;
  curphasevol.resize(precipitatephases.size());
  float change = 0.0f;
  float factor = 1.0;
  size_t iter = 0;
  for (size_t j = 0; j < precipitatephases.size(); ++j)
  {
    curphasevol[j] = 0;
    float curphasetotalvol = static_cast<float>(m_TotalVol * totalprecipitatefractions * precipitatephasefractions[j]);
    while (curphasevol[j] < (factor * curphasetotalvol))
    {
      iter++;
      Seed++;
      phase = precipitatephases[j];
      generate_precipitate(phase, static_cast<int>(Seed), &precip, m_ShapeTypes[phase], m_OrthoOps);
      m_CurrentSizeDistError = check_sizedisterror(&precip);
      change = (m_CurrentSizeDistError) - (m_OldSizeDistError);
      if(change > 0 || m_CurrentSizeDistError > (1.0 - (float(iter) * 0.001)) || curphasevol[j] < (0.75 * factor * curphasetotalvol))
      {
        QString ss = QObject::tr("Packing Precipitates - Generating Feature #%1").arg(currentnumfeatures);
        notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);

        tDims[0] = currentnumfeatures + 1;
        m->getAttributeMatrix(getFeaturePhasesArrayPath().getAttributeMatrixName())->resizeAttributeArrays(tDims);
        updateFeatureInstancePointers();
        transfer_attributes(currentnumfeatures, &precip);
        m_OldSizeDistError = m_CurrentSizeDistError;
        curphasevol[j] = curphasevol[j] + m_Volumes[currentnumfeatures];
        //FIXME: Initialize the Feature with some sort of default data
        iter = 0;
        m_NumFeatures[phase]++;
        currentnumfeatures++;
      }
    }
  }

  //initializing the target RDF vector - this is the radial distribution function we are trying to match to
  if(m_MatchRDF == true)
  {
    for (int64_t i = 1; i < numensembles; ++i)
    {
      if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
      {

        PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
        RdfData::Pointer rdfTarget = pp->getRadialDistFunction();

        const std::vector<float>& freqs = rdfTarget->getFrequencies();
        //       int test =rdfTarget[0]->getNumberOfTuples();
        m_numRDFbins = freqs.size();
        //      std::vector<float> rdfTargetDist;
        m_rdfTargetDist.resize(m_numRDFbins + 1);
        //       m_rdfCurrentDist.resize(m_numRDFbins+2);

        m_rdfTargetDist[0] = 0;

        for (int j = 0; j < m_numRDFbins; j++)
        {
          m_rdfTargetDist[j + 1] = freqs[j];
        }
        m_rdfMax = rdfTarget->getMaxDistance();
        m_rdfMin = rdfTarget->getMinDistance();

        m_StepSize = (m_rdfMax - m_rdfMin) / m_numRDFbins;
        float max_box_distance = sqrtf((m_SizeX * m_SizeX) + (m_SizeY * m_SizeY) + (m_SizeZ * m_SizeZ));

        int32_t current_num_bins = ceil((max_box_distance - m_rdfMin) / (m_StepSize));

        m_rdfCurrentDist.resize(current_num_bins + 1);
      }
    }
  }

  //determine initial set of available points
  availablePointsCount = 0;
  for(size_t i = 0; i < m_TotalPoints; i++)
  {
    if(exclusionZones[i] == 0)
    {
      availablePoints[i] = availablePointsCount;
      availablePointsInv[availablePointsCount] = i;
      availablePointsCount++;
    }
  }
  // and clear the pointsToRemove and pointsToAdd vectors from the initial packing
  pointsToRemove.clear();
  pointsToAdd.clear();

  size_t key;
  size_t column, row, plane;

  size_t numfeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();

  // initial placement
  columnlist.resize(numfeatures);
  rowlist.resize(numfeatures);
  planelist.resize(numfeatures);

  int32_t boundaryVoxels = 0;

  for (size_t i = 1; i < m_TotalPoints; i++)
  {
    if (m_BoundaryCells[i] != 0)
    {
      boundaryVoxels++;
    }
  }

  float boundaryFraction = (float)boundaryVoxels / (float)m_TotalPoints;




  for (size_t i = m_FirstPrecipitateFeature; i < numfeatures; i++)
  {

    QString ss = QObject::tr("Packing Precipitates - Placing Precipitate #%1").arg(i);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);

    PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[m_FeaturePhases[i]].get());
    precipboundaryfraction = pp->getPrecipBoundaryFraction();
    random = static_cast<float>(rg.genrand_res53());

    if (boundaryFraction != 0)
    {
      if(random <= precipboundaryfraction)
      {
        // figure out if we want this to be a boundary centroid voxel or not for the proposed precipitate
        if(availablePointsCount > 0)
        {
          key = static_cast<size_t>(rg.genrand_res53() * (availablePointsCount - 1));
          featureOwnersIdx = availablePointsInv[key];
          while (m_BoundaryCells[featureOwnersIdx] == 0)
          {
            key = static_cast<size_t>(rg.genrand_res53() * (availablePointsCount - 1));
            featureOwnersIdx = availablePointsInv[key];
          }
        }
        else
        {
          featureOwnersIdx = static_cast<size_t>(rg.genrand_res53() * m_TotalPoints);
          while (m_BoundaryCells[featureOwnersIdx] == 0)
          {
            featureOwnersIdx = static_cast<size_t>(rg.genrand_res53() * m_TotalPoints);
          }
        }

      }
      else if(random > precipboundaryfraction)
      {
        if(availablePointsCount > 0)
        {
          key = static_cast<size_t>(rg.genrand_res53() * (availablePointsCount - 1));
          featureOwnersIdx = availablePointsInv[key];
          while (m_BoundaryCells[featureOwnersIdx] != 0)
          {
            key = static_cast<size_t>(rg.genrand_res53() * (availablePointsCount - 1));
            featureOwnersIdx = availablePointsInv[key];
          }
        }
        else
        {
          featureOwnersIdx = static_cast<size_t>(rg.genrand_res53() * m_TotalPoints);
          while (m_BoundaryCells[featureOwnersIdx] != 0)
          {
            featureOwnersIdx = static_cast<size_t>(rg.genrand_res53() * m_TotalPoints);
          }
        }
      }
    }
    else
    {

      if (precipboundaryfraction > 0)
      {
        QString msg("There are no grain boundaries to place precipitates on, and the target statistics precipitate fraction is greater than zero. This filter will run without trying to match the precipitate fraction");
        notifyWarningMessage(getHumanLabel(), msg, -5010);
      }


      if(availablePointsCount > 0)
      {
        key = static_cast<size_t>(rg.genrand_res53() * (availablePointsCount - 1));
        featureOwnersIdx = availablePointsInv[key];
      }
      else
      {
        featureOwnersIdx = static_cast<size_t>(rg.genrand_res53() * m_TotalPoints);
      }
    }

    column = featureOwnersIdx % m_XPoints;
    row = int(featureOwnersIdx / m_XPoints) % m_YPoints;
    plane = featureOwnersIdx / (m_XPoints * m_YPoints);
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

  if (m_MatchRDF == true)
  {
    /*RANDOM: Figure out the RDF for randomly distributed particles.
    We always keep the same stepsize as the target RDF,
    but change (increase if the current box is bigger than what the target dist was built on and vice versa)
    the number of bins to account for smaller and larger (up to the max distance i.e. the box diagonal)
    distances that can occur when particles are just randomly placed in a box. This is true for both m_rdfRandom and m_rdfCurrentDist.*/


    //initialize boxdims and boxres vectors
    std::vector<float> boxdims(3);
    boxdims[0] = m_SizeX;
    boxdims[1] = m_SizeY;
    boxdims[2] = m_SizeZ;

    std::vector<float> boxres(3);
    boxres[0] = m->getGeometryAs<ImageGeom>()->getXRes();
    boxres[1] = m->getGeometryAs<ImageGeom>()->getYRes();
    boxres[2] = m->getGeometryAs<ImageGeom>()->getZRes();

    float max_box_distance = sqrtf((m_SizeX * m_SizeX) + (m_SizeY * m_SizeY) + (m_SizeZ * m_SizeZ));

    int32_t current_num_bins = ceil((max_box_distance - m_rdfMin) / (m_StepSize));

    //resize box to include all the possible distances but using the same stepsize as the target RDF. The zero bin includes all distances smaller than the smallest from the targetRDF
    m_rdfRandom.resize(current_num_bins + 1);

    //Call this function to generate the random distribution, which is normalized by the total number of distances
    m_rdfRandom = RadialDistributionFunction::GenerateRandomDistribution(m_rdfMin, m_rdfMax, m_numRDFbins, boxdims, boxres);

    size_t numPPTfeatures = numfeatures - m_FirstPrecipitateFeature;

    //Scale the randomRDF to have the same number of particles (and therfore distances) as the current distribution.
    for (size_t i = 0; i < m_rdfRandom.size(); i++)
    {
      m_rdfRandom[i] = m_rdfRandom[i] * numPPTfeatures * (numPPTfeatures - 1);
    }

    if(write_test_outputs == true)
    {

      std::ofstream testFile6;
      testFile6.open("/Users/Shared/Data/PW_Work/OUTFILE/randomRDFCurrent.txt");
      for (size_t i = 0; i < m_rdfRandom.size(); i++)
      {
        testFile6 << "\n" << m_rdfRandom[i];
      }
      testFile6.close();


    }

  }


  if(m_MatchRDF == true)
  {

    //calculate the initial current RDF - this will change as we move particles around
    for (size_t i = m_FirstPrecipitateFeature; i < numfeatures; i++)
    {
      m_oldRDFerror = check_RDFerror(i, -1000, false);
    }



    if (true)
    {

      std::ofstream testFile;
      if(write_test_outputs == true)
      {
        testFile.open("/Users/Shared/Data/PW_Work/OUTFILE/BC.txt");
      }

      // begin swaping/moving/adding/removing features to try to improve packing
      // The totalAdjustments are roughly equal to the prefactor (1000, right now) times the number of precipitates.
      // This is not based on convergence or any physics - it's just a factor and there's probably room for improvement here
      int totalAdjustments = static_cast<int>(1000 * ((numfeatures - m_FirstPrecipitateFeature) - 1));
      for (int iteration = 0; iteration < totalAdjustments; ++iteration)
      {
        QString ss;
        ss = QObject::tr("Packing Features - Swapping/Moving/Adding/Removing Features Iteration %1/%2").arg(iteration).arg(totalAdjustments);
        if(iteration % 100 == 0) { notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss); }

        //    change1 = 0;
        //    change2 = 0;

        if(writeErrorFile == true && iteration % 25 == 0)
        {
          outFile << iteration << " " << m_oldRDFerror << " " << acceptedmoves << "\n";
        }

        // JUMP - this one feature to a random spot in the volume
        randomfeature = m_FirstPrecipitateFeature + int(rg.genrand_res53() * (numfeatures - m_FirstPrecipitateFeature));
        if(randomfeature < m_FirstPrecipitateFeature) { randomfeature = m_FirstPrecipitateFeature; }
        if(randomfeature >= static_cast<int>(numfeatures))
        {
          randomfeature = static_cast<int>(numfeatures) - 1;
        }
        Seed++;

        PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[m_FeaturePhases[randomfeature]].get());
        if (NULL == pp)
        {
          continue;
        }

        precipboundaryfraction = pp->getPrecipBoundaryFraction();
        random = static_cast<float>(rg.genrand_res53());
        if (boundaryFraction != 0)
        {
          if(random <= precipboundaryfraction)
          {
            // figure out if we want this to be a boundary centroid voxel or not for the proposed precipitate
            if(availablePointsCount > 0)
            {
              key = static_cast<size_t>(rg.genrand_res53() * (availablePointsCount - 1));
              featureOwnersIdx = availablePointsInv[key];
              while (m_BoundaryCells[featureOwnersIdx] == 0)
              {
                key = static_cast<size_t>(rg.genrand_res53() * (availablePointsCount - 1));
                featureOwnersIdx = availablePointsInv[key];
              }
            }
            else
            {
              featureOwnersIdx = static_cast<size_t>(rg.genrand_res53() * m_TotalPoints);
              while (m_BoundaryCells[featureOwnersIdx] == 0)
              {
                featureOwnersIdx = static_cast<size_t>(rg.genrand_res53() * m_TotalPoints);
              }
            }

          }
          else if(random > precipboundaryfraction)
          {
            if(availablePointsCount > 0)
            {
              key = static_cast<size_t>(rg.genrand_res53() * (availablePointsCount - 1));
              featureOwnersIdx = availablePointsInv[key];
              while (m_BoundaryCells[featureOwnersIdx] != 0)
              {
                key = static_cast<size_t>(rg.genrand_res53() * (availablePointsCount - 1));
                featureOwnersIdx = availablePointsInv[key];
              }
            }
            else
            {
              featureOwnersIdx = static_cast<size_t>(rg.genrand_res53() * m_TotalPoints);
              while (m_BoundaryCells[featureOwnersIdx] != 0)
              {
                featureOwnersIdx = static_cast<size_t>(rg.genrand_res53() * m_TotalPoints);
              }
            }
          }
        }
        else
        {

          if (precipboundaryfraction > 0)
          {
            QString msg("There are no grain boundaries to place precipitates on, and the target statistics precipitate fraction is greater than zero. This filter will run without trying to match the precipitate fraction");
            notifyWarningMessage(getHumanLabel(), msg, -5010);
          }

          if(availablePointsCount > 0)
          {
            key = static_cast<size_t>(rg.genrand_res53() * (availablePointsCount - 1));
            featureOwnersIdx = availablePointsInv[key];
          }
          else
          {
            featureOwnersIdx = static_cast<size_t>(rg.genrand_res53() * m_TotalPoints);
          }
        }
        column = featureOwnersIdx % m_XPoints;
        row = int(featureOwnersIdx / m_XPoints) % m_YPoints;
        plane = featureOwnersIdx / (m_XPoints * m_YPoints);
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
          pointsToRemove.clear();
          pointsToAdd.clear();

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
    for (size_t i = 0; i < m_rdfCurrentDistNorm.size(); i++)
    {
      testFile3 << "\n" << m_rdfCurrentDistNorm[i];
    }
    testFile3.close();

    std::ofstream testFile2;
    testFile2.open("/Users/Shared/Data/PW_Work/OUTFILE/target.txt");
    for (size_t i = 0; i < m_rdfTargetDist.size(); i++)
    {
      testFile2 << "\n" << m_rdfTargetDist[i];
    }
    testFile2.close();
  }

  std::cout << "Done Jumping" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::generate_precipitate(int phase, int seed, Precip* precip, unsigned int shapeclass, OrientationOps::Pointer OrthoOps)
{
  DREAM3D_RANDOMNG_NEW_SEEDED(seed)

  StatsDataArray& statsDataArray = *(m_StatsDataArray.lock());

  float r1 = 1;
  float a2 = 0, a3 = 0;
  float b2 = 0, b3 = 0;
  float diam = 0;
  float vol = 0;
  int volgood = 0;
  float phi1, PHI, phi2;
  float fourThirdsPi =  static_cast<float>((4.0f / 3.0f) * (DREAM3D::Constants::k_Pi));
  PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[phase].get());
  VectorOfFloatArray GSdist = pp->getFeatureSizeDistribution();
  float avg = GSdist[0]->getValue(0);
  float stdev = GSdist[1]->getValue(0);
  while (volgood == 0)
  {
    volgood = 1;
    diam = static_cast<float>(rg.genrand_norm(avg, stdev));
    diam = exp(diam);
    if(diam >= pp->getMaxFeatureDiameter()) { volgood = 0; }
    if(diam < pp->getMinFeatureDiameter()) { volgood = 0; }
    vol = fourThirdsPi * ((diam / 2.0f) * (diam / 2.0f) * (diam / 2.0f));
  }
  int diameter = int((diam - pp->getMinFeatureDiameter()) / pp->getBinStepSize());
  float r2 = 0.0f, r3 = 1.0f;
  VectorOfFloatArray bovera = pp->getFeatureSize_BOverA();
  VectorOfFloatArray covera = pp->getFeatureSize_COverA();
  while (r2 < r3)
  {
    r2 = 0.0f, r3 = 0.0f;
    a2 = bovera[0]->getValue(diameter);
    b2 = bovera[1]->getValue(diameter);
    if(a2 == 0)
    {
      a2 = bovera[0]->getValue(diameter - 1);
      b2 = bovera[1]->getValue(diameter - 1);
    }
    r2 = static_cast<float>(rg.genrand_beta(a2, b2));
    a3 = covera[0]->getValue(diameter);
    b3 = covera[1]->getValue(diameter);
    if(a3 == 0)
    {
      a3 = covera[0]->getValue(diameter - 1);
      b3 = covera[1]->getValue(diameter - 1);
    }
    r3 = static_cast<float>( rg.genrand_beta(a3, b3) );
  }
  float random = static_cast<float>( rg.genrand_res53() );
  float totaldensity = 0;
  int bin = 0;
  FloatArrayType::Pointer axisodf = pp->getAxisOrientation();
  while (random > totaldensity && bin < static_cast<int>(axisodf->getSize()))
  {
    totaldensity = totaldensity + axisodf->getValue(bin);
    bin++;
  }
  OrthoOps->determineEulerAngles(bin, phi1, PHI, phi2);
  VectorOfFloatArray omega3 = pp->getFeatureSize_Omegas();
  float mf = omega3[0]->getValue(diameter);
  float s = omega3[1]->getValue(diameter);
  float omega3f = static_cast<float>(rg.genrand_beta(mf, s));
  if(shapeclass == DREAM3D::ShapeType::EllipsoidShape) { omega3f = 1; }

  precip->m_Volumes = vol;
  precip->m_EquivalentDiameters = diam;
  precip->m_AxisLengths[0] = r1;
  precip->m_AxisLengths[1] = r2;
  precip->m_AxisLengths[2] = r3;
  precip->m_AxisEulerAngles[0] = phi1;
  precip->m_AxisEulerAngles[1] = PHI;
  precip->m_AxisEulerAngles[2] = phi2;
  precip->m_Omega3s = omega3f;
  precip->m_FeaturePhases = phase;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::transfer_attributes(int gnum, Precip* precip)
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
void InsertPrecipitatePhases::move_precipitate(size_t gnum, float xc, float yc, float zc)
{
  int occolumn, ocrow, ocplane;
  int nccolumn, ncrow, ncplane;
  int shiftcolumn, shiftrow, shiftplane;
  float oxc = m_Centroids[3 * gnum];
  float oyc = m_Centroids[3 * gnum + 1];
  float ozc = m_Centroids[3 * gnum + 2];
  occolumn = static_cast<int>( (oxc - (m_XRes / 2.0f)) / m_XRes );
  ocrow = static_cast<int>( (oyc - (m_YRes / 2.0f)) / m_YRes );
  ocplane = static_cast<int>( (ozc - (m_ZRes / 2.0f)) / m_ZRes );
  nccolumn = static_cast<int>( (xc - (m_XRes / 2.0f)) / m_XRes );
  ncrow = static_cast<int>( (yc - (m_YRes / 2.0f)) / m_YRes );
  ncplane = static_cast<int>( (zc - (m_ZRes / 2.0f)) / m_ZRes );
  shiftcolumn = nccolumn - occolumn;
  shiftrow = ncrow - ocrow;
  shiftplane = ncplane - ocplane;
  m_Centroids[3 * gnum] = xc;
  m_Centroids[3 * gnum + 1] = yc;
  m_Centroids[3 * gnum + 2] = zc;
  size_t size = columnlist[gnum].size();

  for (size_t i = 0; i < size; i++)
  {
    int& cl = columnlist[gnum][i];
    cl += shiftcolumn;
    int& rl = rowlist[gnum][i];
    rl += shiftrow;
    int& pl = planelist[gnum][i];
    pl += shiftplane;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::update_exclusionZones(int gadd, int gremove, Int32ArrayType::Pointer exclusionZonesPtr)
{
  size_t featureOwnersIdx = 0;
  int32_t* exclusionZones = exclusionZonesPtr->getPointer(0);

  int col, row, plane;
  if(gadd > 0)
  {
    //size_t key, val;
    size_t size = columnlist[gadd].size();
    std::vector<int>& cl = columnlist[gadd];
    std::vector<int>& rl = rowlist[gadd];
    std::vector<int>& pl = planelist[gadd];
    for (size_t i = 0; i < size; i++)
    {
      col = cl[i];
      row = rl[i];
      plane = pl[i];
      if(m_PeriodicBoundaries == true)
      {
        if(col < 0) { col = col + m_XPoints; }
        if(col > m_XPoints - 1) { col = col - m_XPoints; }
        if(row < 0) { row = row + m_YPoints; }
        if(row > m_YPoints - 1) { row = row - m_YPoints; }
        if(plane < 0) { plane = plane + m_ZPoints; }
        if(plane > m_ZPoints - 1) { plane = plane - m_ZPoints; }
        featureOwnersIdx = (m_XPoints * m_YPoints * plane) + (m_XPoints * row) + col;
        if(exclusionZones[featureOwnersIdx] > 0) {  }
        if(exclusionZones[featureOwnersIdx] == 0)
        {
          pointsToRemove.push_back(featureOwnersIdx);
        }
        exclusionZones[featureOwnersIdx]++;
      }
      else
      {
        if(col >= 0 && col < m_XPoints && row >= 0 && row < m_YPoints && plane >= 0 && plane < m_ZPoints)
        {
          featureOwnersIdx = (m_XPoints * m_YPoints * plane) + (m_XPoints * row) + col;
          if(exclusionZones[featureOwnersIdx] > 0) {  }
          if(exclusionZones[featureOwnersIdx] == 0)
          {
            pointsToRemove.push_back(featureOwnersIdx);
          }
          exclusionZones[featureOwnersIdx]++;
        }
      }
    }
  }
  if(gremove > 0)
  {
    size_t size = columnlist[gremove].size();
    std::vector<int>& cl = columnlist[gremove];
    std::vector<int>& rl = rowlist[gremove];
    std::vector<int>& pl = planelist[gremove];
    for (size_t i = 0; i < size; i++)
    {
      col = cl[i];
      row = rl[i];
      plane = pl[i];
      if(m_PeriodicBoundaries == true)
      {
        if(col < 0) { col = col + m_XPoints; }
        if(col > m_XPoints - 1) { col = col - m_XPoints; }
        if(row < 0) { row = row + m_YPoints; }
        if(row > m_YPoints - 1) { row = row - m_YPoints; }
        if(plane < 0) { plane = plane + m_ZPoints; }
        if(plane > m_ZPoints - 1) { plane = plane - m_ZPoints; }
        featureOwnersIdx = (m_XPoints * m_YPoints * plane) + (m_XPoints * row) + col;
        if(exclusionZones[featureOwnersIdx] > 1) { }
        exclusionZones[featureOwnersIdx]--;
        if(exclusionZones[featureOwnersIdx] == 0)
        {
          pointsToAdd.push_back(featureOwnersIdx);
        }
      }
      else
      {
        if(col >= 0 && col < m_XPoints && row >= 0 && row < m_YPoints && plane >= 0 && plane < m_ZPoints)
        {
          featureOwnersIdx = (m_XPoints * m_YPoints * plane) + (m_XPoints * row) + col;
          if(exclusionZones[featureOwnersIdx] > 1) { }
          exclusionZones[featureOwnersIdx]--;
          if(exclusionZones[featureOwnersIdx] == 0)
          {
            pointsToAdd.push_back(featureOwnersIdx);
          }
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::update_availablepoints(std::map<size_t, size_t>& availablePoints, std::map<size_t, size_t>& availablePointsInv)
{
  size_t removeSize = pointsToRemove.size();
  size_t addSize = pointsToAdd.size();
  size_t featureOwnersIdx;
  size_t key, val;
  for(size_t i = 0; i < addSize; i++)
  {
    featureOwnersIdx = pointsToAdd[i];
    availablePoints[featureOwnersIdx] = availablePointsCount;
    availablePointsInv[availablePointsCount] = featureOwnersIdx;
    availablePointsCount++;
  }
  for(size_t i = 0; i < removeSize; i++)
  {
    featureOwnersIdx = pointsToRemove[i];
    key = availablePoints[featureOwnersIdx];
    //  availablePoints.erase(featureOwnersIdx);
    val = availablePointsInv[availablePointsCount - 1];
    //  availablePointsInv.erase(availablePointsCount-1);
    if(key < availablePointsCount - 1)
    {
      availablePointsInv[key] = val;
      availablePoints[val] = key;
    }
    availablePointsCount--;
  }
  pointsToRemove.clear();
  pointsToAdd.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

void InsertPrecipitatePhases::determine_currentRDF(size_t gnum, int add, bool double_count)
{

  float x, y, z;
  float xn, yn, zn;
  float r;

  int iter = 0;
  int numPPTfeatures = 1;
  int32_t rdfBin;

  int phase = m_FeaturePhases[gnum];
  while (phase != precipitatephases[iter]) { iter++; }

  //StatsDataArray& statsDataArray = *(m_StatsDataArray.lock());
  //typedef std::vector<std::vector<float> > VectOfVectFloat_t;


  x = m_Centroids[3 * gnum];
  y = m_Centroids[3 * gnum + 1];
  z = m_Centroids[3 * gnum + 2];
  size_t numFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();

  for (size_t n = m_FirstPrecipitateFeature; n < numFeatures; n++)
  {
    if (m_FeaturePhases[n] == phase && n != gnum)
    {
      xn = m_Centroids[3 * n];
      yn = m_Centroids[3 * n + 1];
      zn = m_Centroids[3 * n + 2];
      r = sqrtf((x - xn) * (x - xn) + (y - yn) * (y - yn) + (z - zn) * (z - zn));

      rdfBin = (r - m_rdfMin) / m_StepSize;

      if (r < m_rdfMin)
      { rdfBin = -1;}
//      if (r < 6.0)
//      {
//        int stop = 0;
//      }
      //if (rdfBin >= m_numRDFbins) {rdfBin = m_numRDFbins;}
      if (double_count == true)
      {
        m_rdfCurrentDist[rdfBin + 1] += 2 * add;
      }
      else if (double_count == false)
      {
        m_rdfCurrentDist[rdfBin + 1] += add;
      }

      numPPTfeatures += 1;
    }
  }


  m_rdfCurrentDistNorm = normalizeRDF(m_rdfCurrentDist, m_numRDFbins, m_StepSize, m_rdfMin, numPPTfeatures, m_TotalVol);

//  std::cout << "test" << std::endl;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

void InsertPrecipitatePhases::determine_randomRDF(size_t gnum, int add, bool double_count, int largeNumber)
{

  float x = 0.0f, y = 0.0f, z = 0.0f;
  float xn = 0.0f, yn = 0.0f, zn = 0.0f;
  float r = 0.0f;

  int32_t rdfBin = 0;

  x = m_RandomCentroids[3 * gnum];
  y = m_RandomCentroids[3 * gnum + 1];
  z = m_RandomCentroids[3 * gnum + 2];

  size_t ln = static_cast<size_t>(largeNumber);


  for (size_t n = 0; n < ln; n++)
  {
    if (n != gnum)
    {
      xn = m_RandomCentroids[3 * n];
      yn = m_RandomCentroids[3 * n + 1];
      zn = m_RandomCentroids[3 * n + 2];
      r = sqrtf((x - xn) * (x - xn) + (y - yn) * (y - yn) + (z - zn) * (z - zn));

      rdfBin = (r - m_rdfMin) / m_StepSize;

      if (r < m_rdfMin)
      {
        rdfBin = -1;
      }
      if (double_count == true)
      {
        m_rdfRandom[rdfBin + 1] += 2 * add;
      }
      else if (double_count == false)
      {
        m_rdfRandom[rdfBin + 1] += add;
      }

    }
  }




}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<float> InsertPrecipitatePhases::normalizeRDF(std::vector<float> rdf, int num_bins, float m_StepSize, float rdfmin, size_t numPPTfeatures, float volume)
{
  //Normalizing the RDF by number density of particles (4/3*pi*(r2^3-r1^3)*numPPTfeatures/volume)
//    float normfactor;
//    float r1;
//    float r2;
//    float oneovervolume = 1.0f/volume;
//    float finiteAdjFactor = .5;

  //r1 = 0*finiteAdjFactor;
  //r2 = rdfmin*finiteAdjFactor;
  //normfactor = 4.0f/3.0f*DREAM3D::Constants::k_Pi*((r2*r2*r2) - (r1*r1*r1))*numPPTfeatures*oneovervolume;
//    rdf[0] = rdf[0];

//    for (size_t i = 1; i < num_bins+2; i++)
//      {
//          r1 = (rdfmin + (i-1)*m_StepSize);
//          r2 = (r1 + m_StepSize);
//          r1 = r1*finiteAdjFactor;
//          r2 = r2*finiteAdjFactor;
//          normfactor = 4.0f/3.0f*DREAM3D::Constants::k_Pi*((r2*r2*r2) - (r1*r1*r1))*numPPTfeatures*oneovervolume;
//          rdf[i] = rdf[i]/normfactor;
//      }

  for (size_t i = 0; i < rdf.size(); i++)
  {
    rdf[i] = rdf[i] / m_rdfRandom[i];
  }

  return rdf;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float InsertPrecipitatePhases::check_RDFerror(int gadd, int gremove, bool double_count)
{

  float rdferror;
  float bhattdist;

  if(gadd > 0)
  {
    determine_currentRDF(gadd, 1, double_count);
  }
  if(gremove > 0)
  {
    determine_currentRDF(gremove, -1, double_count);
  }

  if (m_rdfCurrentDistNorm.size() > m_rdfTargetDist.size())
  {
    compare_1Ddistributions(m_rdfTargetDist, m_rdfCurrentDistNorm, bhattdist);
  }
  else
  {
    compare_1Ddistributions(m_rdfCurrentDistNorm, m_rdfTargetDist, bhattdist);
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
  float sum_array1 = 0;
  float sum_array2 = 0;

//  for (std::vector<float>::iterator j=array1.begin(); j!=array1.end(); j++)
//  {sum_array1 += *j;}

//  for (std::vector<float>::iterator j=array2.begin(); j!=array2.end(); j++)
//  {sum_array2 += *j;}

  for (size_t i = 0; i < array1.size(); i++)
  {
    sum_array1 = sum_array1 + array1[i];
    sum_array2 = sum_array2 + array2[i];

  }

  for (size_t i = 0; i < array1.size(); i++)
  {
    array1[i] = array1[i] / sum_array1;
    array2[i] = array2[i] / sum_array2;

    bhattdist = bhattdist + sqrt((array1[i] * array2[i]));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::compare_2Ddistributions(std::vector<std::vector<float> > array1, std::vector<std::vector<float> > array2, float& bhattdist)
{
  bhattdist = 0;
  for (size_t i = 0; i < array1.size(); i++)
  {
    for (size_t j = 0; j < array1[i].size(); j++)
    {
      bhattdist = bhattdist + sqrt((array1[i][j] * array2[i][j]));
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::compare_3Ddistributions(std::vector<std::vector<std::vector<float> > > array1, std::vector<std::vector<std::vector<float> > > array2, float& bhattdist)
{
  bhattdist = 0;
  std::vector<std::vector<float> > counts1(array1.size());
  std::vector<std::vector<float> > counts2(array2.size());
  for (size_t i = 0; i < array1.size(); i++)
  {
    counts1[i].resize(array1[i].size());
    counts2[i].resize(array2[i].size());
    for (size_t j = 0; j < array1[i].size(); j++)
    {
      for (size_t k = 0; k < array1[i][j].size(); k++)
      {
        counts1[i][j] += array1[i][j][k];
        counts2[i][j] += array2[i][j][k];
      }
    }
  }
  for (size_t i = 0; i < array1.size(); i++)
  {
    for (size_t j = 0; j < array1[i].size(); j++)
    {
      for (size_t k = 0; k < array1[i][j].size(); k++)
      {
        bhattdist = bhattdist + sqrt(((array1[i][j][k] / counts1[i][j]) * (array2[i][j][k] / counts2[i][j])));
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float InsertPrecipitatePhases::check_sizedisterror(Precip* precip)
{
  //DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  StatsDataArray& statsDataArray = *(m_StatsDataArray.lock());

  float dia;
  float sizedisterror = 0;
  float bhattdist;
  int index;
  int count = 0;
  int phase;
  size_t featureSizeDist_Size = featuresizedist.size();
  for (size_t iter = 0; iter < featureSizeDist_Size; ++iter)
  {
    phase = precipitatephases[iter];
    PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[phase].get());
    count = 0;
    std::vector<float>& curFeatureSizeDist = featuresizedist[iter];
    std::vector<float>::size_type curFeatureSizeDistSize = curFeatureSizeDist.size();
    std::vector<float>& curSimFeatureSizeDist = simfeaturesizedist[iter];
    // Initialize all Values to Zero
    for (size_t i = 0; i < curFeatureSizeDistSize; i++)
    {
      curSimFeatureSizeDist[i] = 0.0f;
    }

    size_t nFeatureTuples = m_FeaturePhasesPtr.lock()->getNumberOfTuples();
    float oneOverCurFeatureSizeDistStep = 1.0f / featuresizediststep[iter];
    float halfMinFeatureDiameter = pp->getMinFeatureDiameter() * 0.5f;
    for (size_t b = m_FirstPrecipitateFeature; b < nFeatureTuples; b++)
    {
      index = b;
      if(m_FeaturePhases[index] == phase)
      {
        dia = m_EquivalentDiameters[index];
        dia = (dia - halfMinFeatureDiameter) * oneOverCurFeatureSizeDistStep;
        if(dia < 0) { dia = 0; }
        if(dia > curFeatureSizeDistSize - 1.0f) { dia = curFeatureSizeDistSize - 1.0f; }
        curSimFeatureSizeDist[int(dia)]++;
        count++;
      }
    }

    if(precip->m_FeaturePhases == phase)
    {
      dia = precip->m_EquivalentDiameters;
      dia = (dia - halfMinFeatureDiameter) * oneOverCurFeatureSizeDistStep;
      if(dia < 0) { dia = 0; }
      if(dia > curFeatureSizeDistSize - 1.0f) { dia = curFeatureSizeDistSize - 1.0f; }
      curSimFeatureSizeDist[int(dia)]++;
      count++;
    }
    float oneOverCount = 1.0f / count;

    if (count == 0)
    {
      for (size_t i = 0; i < curFeatureSizeDistSize; i++) { curSimFeatureSizeDist[i] = 0.0; }
    }
    else
    {
      for (size_t i = 0; i < curFeatureSizeDistSize; i++)
      {
        curSimFeatureSizeDist[i] = curSimFeatureSizeDist[i] * oneOverCount;
      }
    }
  }
  compare_2Ddistributions(simfeaturesizedist, featuresizedist, bhattdist);
  sizedisterror = bhattdist;
  return sizedisterror;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::insert_precipitate(size_t gnum)
{
  DREAM3D_RANDOMNG_NEW()

  float inside = -1;
  int column, row, plane;
  int centercolumn, centerrow, centerplane;
  int xmin, xmax, ymin, ymax, zmin, zmax;
  float xc, yc, zc;
  float coordsRotated[3];
  float coords[3];
  float volcur = m_Volumes[gnum];
  float bovera = m_AxisLengths[3 * gnum + 1];
  float covera = m_AxisLengths[3 * gnum + 2];
  float omega3 = m_Omega3s[gnum];
  float radcur1 = 1;
  unsigned int shapeclass = m_ShapeTypes[m_FeaturePhases[gnum]];

  // init any values for each of the Shape Ops
  for (QMap<unsigned int, ShapeOps*>::iterator ops = m_ShapeOps.begin(); ops != m_ShapeOps.end(); ++ops)
  {
    ops.value()->init();
  }
  // Create our Argument Map
  QMap<ShapeOps::ArgName, float> shapeArgMap;
  shapeArgMap[ShapeOps::Omega3] = omega3;
  shapeArgMap[ShapeOps::VolCur] = volcur;
  shapeArgMap[ShapeOps::B_OverA] = bovera;
  shapeArgMap[ShapeOps::C_OverA] = covera;

  radcur1 = m_ShapeOps[shapeclass]->radcur1(shapeArgMap);

  //adjust radcur1 to make larger exclusion zone to prevent precipitate overlap
  radcur1 = radcur1 * 2.0;

  float radcur2 = (radcur1 * bovera);
  float radcur3 = (radcur1 * covera);
  float phi1 = m_AxisEulerAngles[3 * gnum];
  float PHI = m_AxisEulerAngles[3 * gnum + 1];
  float phi2 = m_AxisEulerAngles[3 * gnum + 2];
  float ga[3][3];
  float gaT[3][3];
  OrientationMath::EulertoMat(phi1, PHI, phi2, ga);
  xc = m_Centroids[3 * gnum];
  yc = m_Centroids[3 * gnum + 1];
  zc = m_Centroids[3 * gnum + 2];
  centercolumn = static_cast<int>( (xc - (m_XRes / 2)) / m_XRes );
  centerrow = static_cast<int>( (yc - (m_YRes / 2)) / m_YRes );
  centerplane = static_cast<int>( (zc - (m_ZRes / 2)) / m_ZRes );
  xmin = int(centercolumn - ((radcur1 / m_XRes) + 1));
  xmax = int(centercolumn + ((radcur1 / m_XRes) + 1));
  ymin = int(centerrow - ((radcur1 / m_YRes) + 1));
  ymax = int(centerrow + ((radcur1 / m_YRes) + 1));
  zmin = int(centerplane - ((radcur1 / m_ZRes) + 1));
  zmax = int(centerplane + ((radcur1 / m_ZRes) + 1));
  if(xmin < -m_XPoints) { xmin = -m_XPoints; }
  if(xmax > 2 * m_XPoints - 1) { xmax = (2 * m_XPoints - 1); }
  if(ymin < -m_YPoints) { ymin = -m_YPoints; }
  if(ymax > 2 * m_YPoints - 1) { ymax = (2 * m_YPoints - 1); }
  if(zmin < -m_ZPoints) { zmin = -m_ZPoints; }
  if(zmax > 2 * m_ZPoints - 1) { zmax = (2 * m_ZPoints - 1); }
  for (int iter1 = xmin; iter1 < xmax + 1; iter1++)
  {
    for (int iter2 = ymin; iter2 < ymax + 1; iter2++)
    {
      for (int iter3 = zmin; iter3 < zmax + 1; iter3++)
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
        MatrixMath::Transpose3x3(ga, gaT);
        MatrixMath::Multiply3x3with3x1(gaT, coords, coordsRotated);
        float axis1comp = coordsRotated[0] / radcur1;
        float axis2comp = coordsRotated[1] / radcur2;
        float axis3comp = coordsRotated[2] / radcur3;
        inside = m_ShapeOps[shapeclass]->inside(axis1comp, axis2comp, axis3comp);
        if(inside >= 0)
        {
          columnlist[gnum].push_back(column);
          rowlist[gnum].push_back(row);
          planelist[gnum].push_back(plane);
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
  notifyStatusMessage(getHumanLabel(), "Assigning Voxels");

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  int index;
  size_t udims[3] = {0, 0, 0};
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  float totalPoints = dims[0] * dims[1] * dims[2];
  float xRes = m->getGeometryAs<ImageGeom>()->getXRes();
  float yRes = m->getGeometryAs<ImageGeom>()->getYRes();
  float zRes = m->getGeometryAs<ImageGeom>()->getZRes();

//  int oldname;
  size_t column, row, plane;
  float inside;
  float xc, yc, zc;
  float coordsRotated[3];
  //float dist;
  float coords[3];
  DimType xmin, xmax, ymin, ymax, zmin, zmax;
  int64_t numFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();
  gsizes.resize(numFeatures);

  for (int64_t i = m_FirstPrecipitateFeature; i < numFeatures; i++)
  {
    gsizes[i] = 0;
  }
  for (int64_t i = m_FirstPrecipitateFeature; i < numFeatures; i++)
  {
    float volcur = m_Volumes[i];
    float bovera = m_AxisLengths[3 * i + 1];
    float covera = m_AxisLengths[3 * i + 2];
    float omega3 = m_Omega3s[i];
    xc = m_Centroids[3 * i];
    yc = m_Centroids[3 * i + 1];
    zc = m_Centroids[3 * i + 2];
    float radcur1 = 0.0f;
    //Unbounded Check for the size of shapeTypes. We assume a 1:1 with phase
    unsigned int shapeclass = m_ShapeTypes[m_FeaturePhases[i]];

    // init any values for each of the Shape Ops
    for (QMap<unsigned int, ShapeOps*>::iterator ops = m_ShapeOps.begin(); ops != m_ShapeOps.end(); ++ops )
    {
      ops.value()->init();
    }
    // Create our Argument Map
    QMap<ShapeOps::ArgName, float> shapeArgMap;
    shapeArgMap[ShapeOps::Omega3] = omega3;
    shapeArgMap[ShapeOps::VolCur] = volcur;
    shapeArgMap[ShapeOps::B_OverA] = bovera;
    shapeArgMap[ShapeOps::C_OverA] = covera;

    radcur1 = m_ShapeOps[shapeclass]->radcur1(shapeArgMap);

    float radcur2 = (radcur1 * bovera);
    float radcur3 = (radcur1 * covera);
    float phi1 = m_AxisEulerAngles[3 * i];
    float PHI = m_AxisEulerAngles[3 * i + 1];
    float phi2 = m_AxisEulerAngles[3 * i + 2];
    float ga[3][3];
    float gaT[3][3];
    OrientationMath::EulertoMat(phi1, PHI, phi2, ga);
    column = static_cast<size_t>( (xc - (xRes / 2.0f)) / xRes );
    row = static_cast<size_t>( (yc - (yRes / 2.0f)) / yRes );
    plane = static_cast<size_t>( (zc - (zRes / 2.0f)) / zRes );
    xmin = int(column - ((radcur1 / xRes) + 1));
    xmax = int(column + ((radcur1 / xRes) + 1));
    ymin = int(row - ((radcur1 / yRes) + 1));
    ymax = int(row + ((radcur1 / yRes) + 1));
    zmin = int(plane - ((radcur1 / zRes) + 1));
    zmax = int(plane + ((radcur1 / zRes) + 1));
    if (m_PeriodicBoundaries == true)
    {
      if (xmin < -dims[0]) { xmin = -dims[0]; }
      if (xmax > 2 * dims[0] - 1) { xmax = (2 * dims[0] - 1); }
      if (ymin < -dims[1]) { ymin = -dims[1]; }
      if (ymax > 2 * dims[1] - 1) { ymax = (2 * dims[1] - 1); }
      if (zmin < -dims[2]) { zmin = -dims[2]; }
      if (zmax > 2 * dims[2] - 1) { zmax = (2 * dims[2] - 1); }
    }
    if (m_PeriodicBoundaries == false)
    {
      if (xmin < 0) { xmin = 0; }
      if (xmax > dims[0] - 1) { xmax = dims[0] - 1; }
      if (ymin < 0) { ymin = 0; }
      if (ymax > dims[1] - 1) { ymax = dims[1] - 1; }
      if (zmin < 0) { zmin = 0; }
      if (zmax > dims[2] - 1) { zmax = dims[2] - 1; }
    }
    for (DimType iter1 = xmin; iter1 < xmax + 1; iter1++)
    {
      for (DimType iter2 = ymin; iter2 < ymax + 1; iter2++)
      {
        for (DimType iter3 = zmin; iter3 < zmax + 1; iter3++)
        {
          column = iter1;
          row = iter2;
          plane = iter3;
          if (iter1 < 0) { column = iter1 + dims[0]; }
          if (iter1 > dims[0] - 1) { column = iter1 - dims[0]; }
          if (iter2 < 0) { row = iter2 + dims[1]; }
          if (iter2 > dims[1] - 1) { row = iter2 - dims[1]; }
          if (iter3 < 0) { plane = iter3 + dims[2]; }
          if (iter3 > dims[2] - 1) { plane = iter3 - dims[2]; }
          index = (plane * dims[0] * dims[1]) + (row * dims[0]) + column;
          inside = -1;
          coords[0] = float(column) * xRes;
          coords[1] = float(row) * yRes;
          coords[2] = float(plane) * zRes;
          if (iter1 < 0) { coords[0] = coords[0] - m_SizeX; }
          if (iter1 > dims[0] - 1) { coords[0] = coords[0] + m_SizeX; }
          if (iter2 < 0) { coords[1] = coords[1] - m_SizeY; }
          if (iter2 > dims[1] - 1) { coords[1] = coords[1] + m_SizeY; }
          if (iter3 < 0) { coords[2] = coords[2] - m_SizeZ; }
          if (iter3 > dims[2] - 1) { coords[2] = coords[2] + m_SizeZ; }
          coords[0] = coords[0] - xc;
          coords[1] = coords[1] - yc;
          coords[2] = coords[2] - zc;
          MatrixMath::Transpose3x3(ga, gaT);
          MatrixMath::Multiply3x3with3x1(gaT, coords, coordsRotated);
          float axis1comp = coordsRotated[0] / radcur1;
          float axis2comp = coordsRotated[1] / radcur2;
          float axis3comp = coordsRotated[2] / radcur3;
          inside = m_ShapeOps[shapeclass]->inside(axis1comp, axis2comp, axis3comp);
          if (inside >= 0)
          {
            int currentpoint = index;
            if (m_FeatureIds[currentpoint] > m_FirstPrecipitateFeature)
            {
              //oldname = m_FeatureIds[currentpoint];
              m_FeatureIds[currentpoint] = -2;
            }
            if (m_FeatureIds[currentpoint] < m_FirstPrecipitateFeature && m_FeatureIds[currentpoint] != -2)
            {
              m_FeatureIds[currentpoint] = static_cast<int32_t>(i);
            }
          }
        }
      }
    }
  }

  QVector<bool> activeObjects(numFeatures, false);
  int gnum;
  for (size_t i = 0; i < static_cast<size_t>(totalPoints); i++)
  {
    gnum = m_FeatureIds[i];
    if(gnum >= 0) { activeObjects[gnum] = true; }
  }

  AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(getFeaturePhasesArrayPath().getAttributeMatrixName());
  cellFeatureAttrMat->removeInactiveObjects(activeObjects, m_FeatureIdsPtr.lock());
  //need to update pointers after resize, but do not need to run full data check because pointers are still valid
  updateFeatureInstancePointers();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::assign_gaps()
{
  notifyStatusMessage(getHumanLabel(), "Assigning Gaps");

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  int64_t totpoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();

  size_t udims[3] = {0, 0, 0};
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };


  int index;
  int timestep = 100;
  int unassignedcount = 1;
  DimType column, row, plane;
  float inside;
  float xc, yc, zc;
  float coordsRotated[3];
  float dist;
  float coords[3];

  DimType xmin, xmax, ymin, ymax, zmin, zmax;

  float xRes = m->getGeometryAs<ImageGeom>()->getXRes();
  float yRes = m->getGeometryAs<ImageGeom>()->getYRes();
  float zRes = m->getGeometryAs<ImageGeom>()->getZRes();

  Int32ArrayType::Pointer newownersPtr = Int32ArrayType::CreateArray(totpoints, "newowners");
  int32_t* newowners = newownersPtr->getPointer(0);
  newownersPtr->initializeWithZeros();

  FloatArrayType::Pointer ellipfuncsPtr = FloatArrayType::CreateArray(totpoints, "ellipfuncs");
  float* ellipfuncs = ellipfuncsPtr->getPointer(0);
  ellipfuncsPtr->initializeWithValue(-1);

  int64_t numFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();
  //int64_t numEnsembles = m_PhaseTypesPtr.lock()->getNumberOfTuples();
  while (unassignedcount != 0)
  {
    unassignedcount = 0;
    timestep = timestep + 50;
    for (int64_t i = m_FirstPrecipitateFeature; i < numFeatures; i++)
    {
      float volcur = m_Volumes[i];
      float bovera = m_AxisLengths[3 * i + 1];
      float covera = m_AxisLengths[3 * i + 2];
      float omega3 = m_Omega3s[i];
      xc = m_Centroids[3 * i];
      yc = m_Centroids[3 * i + 1];
      zc = m_Centroids[3 * i + 2];
      float radcur1 = 0.0f;
      //Unbounded Check for the size of shapeTypes. We assume a 1:1 with phase
      unsigned int shapeclass = m_ShapeTypes[m_FeaturePhases[i]];

      // init any values for each of the Shape Ops
      for (QMap<unsigned int, ShapeOps*>::iterator ops = m_ShapeOps.begin(); ops != m_ShapeOps.end(); ++ops )
      {
        ops.value()->init();
      }
      // Create our Argument Map
      QMap<ShapeOps::ArgName, float> shapeArgMap;
      shapeArgMap[ShapeOps::Omega3] = omega3;
      shapeArgMap[ShapeOps::VolCur] = volcur;
      shapeArgMap[ShapeOps::B_OverA] = bovera;
      shapeArgMap[ShapeOps::C_OverA] = covera;

      radcur1 = m_ShapeOps[shapeclass]->radcur1(shapeArgMap);

      float radcur2 = (radcur1 * bovera);
      float radcur3 = (radcur1 * covera);
      radcur1 = (float(timestep) / 100.0f) * radcur1;
      radcur2 = (float(timestep) / 100.0f) * radcur2;
      radcur3 = (float(timestep) / 100.0f) * radcur3;
      float phi1 = m_AxisEulerAngles[3 * i];
      float PHI = m_AxisEulerAngles[3 * i + 1];
      float phi2 = m_AxisEulerAngles[3 * i + 2];
      float ga[3][3];
      OrientationMath::EulertoMat(phi1, PHI, phi2, ga);
      column = static_cast<DimType>( (xc - (xRes / 2.0f)) / xRes );
      row = static_cast<DimType>( (yc - (yRes / 2.0f)) / yRes );
      plane = static_cast<DimType>( (zc - (zRes / 2.0f)) / zRes );
      xmin = int(column - ((radcur1 / xRes) + 1));
      xmax = int(column + ((radcur1 / xRes) + 1));
      ymin = int(row - ((radcur1 / yRes) + 1));
      ymax = int(row + ((radcur1 / yRes) + 1));
      zmin = int(plane - ((radcur1 / zRes) + 1));
      zmax = int(plane + ((radcur1 / zRes) + 1));
      if (m_PeriodicBoundaries == true)
      {
        if (xmin < -dims[0]) { xmin = -dims[0]; }
        if (xmax > 2 * dims[0] - 1) { xmax = (2 * dims[0] - 1); }
        if (ymin < -dims[1]) { ymin = -dims[1]; }
        if (ymax > 2 * dims[1] - 1) { ymax = (2 * dims[1] - 1); }
        if (zmin < -dims[2]) { zmin = -dims[2]; }
        if (zmax > 2 * dims[2] - 1) { zmax = (2 * dims[2] - 1); }
      }
      if (m_PeriodicBoundaries == false)
      {
        if (xmin < 0) { xmin = 0; }
        if (xmax > dims[0] - 1) { xmax = dims[0] - 1; }
        if (ymin < 0) { ymin = 0; }
        if (ymax > dims[1] - 1) { ymax = dims[1] - 1; }
        if (zmin < 0) { zmin = 0; }
        if (zmax > dims[2] - 1) { zmax = dims[2] - 1; }
      }
      for (DimType iter1 = xmin; iter1 < xmax + 1; iter1++)
      {
        for (DimType iter2 = ymin; iter2 < ymax + 1; iter2++)
        {
          for (DimType iter3 = zmin; iter3 < zmax + 1; iter3++)
          {
            column = iter1;
            row = iter2;
            plane = iter3;
            if (iter1 < 0) { column = iter1 + dims[0]; }
            if (iter1 > dims[0] - 1) { column = iter1 - dims[0]; }
            if (iter2 < 0) { row = iter2 + dims[1]; }
            if (iter2 > dims[1] - 1) { row = iter2 - dims[1]; }
            if (iter3 < 0) { plane = iter3 + dims[2]; }
            if (iter3 > dims[2] - 1) { plane = iter3 - dims[2]; }
            index = static_cast<int>( (plane * dims[0] * dims[1]) + (row * dims[0]) + column );
            if(m_FeatureIds[index] <= 0)
            {
              inside = -1;
              coords[0] = float(column) * xRes;
              coords[1] = float(row) * yRes;
              coords[2] = float(plane) * zRes;
              if (iter1 < 0) { coords[0] = coords[0] - m_SizeX; }
              if (iter1 > dims[0] - 1) { coords[0] = coords[0] + m_SizeX; }
              if (iter2 < 0) { coords[1] = coords[1] - m_SizeY; }
              if (iter2 > dims[1] - 1) { coords[1] = coords[1] + m_SizeY; }
              if (iter3 < 0) { coords[2] = coords[2] - m_SizeZ; }
              if (iter3 > dims[2] - 1) { coords[2] = coords[2] + m_SizeZ; }
              dist = ((coords[0] - xc) * (coords[0] - xc)) + ((coords[1] - yc) * (coords[1] - yc)) + ((coords[2] - zc) * (coords[2] - zc));
              dist = sqrtf(dist);
              if (dist < radcur1)
              {
                coords[0] = coords[0] - xc;
                coords[1] = coords[1] - yc;
                coords[2] = coords[2] - zc;
                MatrixMath::Multiply3x3with3x1(ga, coords, coordsRotated);
                float axis1comp = coordsRotated[0] / radcur1;
                float axis2comp = coordsRotated[1] / radcur2;
                float axis3comp = coordsRotated[2] / radcur3;
                inside = m_ShapeOps[shapeclass]->inside(axis1comp, axis2comp, axis3comp);
                if (inside >= 0 && inside > ellipfuncs[index])
                {
                  newowners[index] = i;
                  ellipfuncs[index] = inside;
                }
              }
            }
          }
        }
      }
    }
    for (int i = 0; i < totpoints; i++)
    {
      if (ellipfuncs[i] >= 0) { m_FeatureIds[i] = newowners[i]; }
      if (m_FeatureIds[i] <= 0) { unassignedcount++; }
      newowners[i] = -1;
      ellipfuncs[i] = -1.0;
    }
  }
  for (int i = 0; i < totpoints; i++)
  {
    if(m_FeatureIds[i] > 0) { m_CellPhases[i] = m_FeaturePhases[m_FeatureIds[i]]; }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float InsertPrecipitatePhases::find_xcoord(long long int index)
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  float x = m->getGeometryAs<ImageGeom>()->getXRes() * float(index % m->getGeometryAs<ImageGeom>()->getXPoints());
  return x;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float InsertPrecipitatePhases::find_ycoord(long long int index)
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  float y = m->getGeometryAs<ImageGeom>()->getYRes() * float((index / m->getGeometryAs<ImageGeom>()->getXPoints()) % m->getGeometryAs<ImageGeom>()->getYPoints());
  return y;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float InsertPrecipitatePhases::find_zcoord(long long int index)
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

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path

  QFileInfo fi(m_CsvOutputFile);
  QString parentPath = fi.path();
  QDir dir;
  if(!dir.mkpath(parentPath))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath);
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
    return;
  }


  QFile outFile(getCsvOutputFile());
  if (!outFile.open(QIODevice::WriteOnly))
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

  //For checking if an array is a neighborlist
  NeighborList<float>::Pointer neighborlistPtr = NeighborList<float>::New();

  // Print the FeatureIds Header before the rest of the headers
  dStream << DREAM3D::FeatureData::FeatureID;
  // Loop throught the list and print the rest of the headers, ignoring those we don't want
  for(QList<QString>::iterator iter = headers.begin(); iter != headers.end(); ++iter)
  {
    // Only get the array if the name does NOT match those listed
    IDataArray::Pointer p = m->getAttributeMatrix(getFeaturePhasesArrayPath().getAttributeMatrixName())->getAttributeArray(*iter);
    if(p->getNameOfClass().compare(neighborlistPtr->getNameOfClass()) != 0)
    {
      if (p->getNumberOfComponents() == 1)
      {
        dStream << space << (*iter);
      }
      else // There are more than a single component so we need to add multiple header values
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
    if (((float)i / numTuples) * 100.0f > threshold)
    {
      QString ss = QObject::tr("Writing Feature Data - %1% Complete").arg(((float)i / numTuples) * 100);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
      threshold = threshold + 5.0f;
      if (threshold < ((float)i / numTuples) * 100.0f)
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
{ return SyntheticBuilding::SyntheticBuildingBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InsertPrecipitatePhases::getGroupName()
{ return DREAM3D::FilterGroups::SyntheticBuildingFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InsertPrecipitatePhases::getSubGroupName()
{ return DREAM3D::FilterSubGroups::PackingFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InsertPrecipitatePhases::getHumanLabel()
{ return "Insert Precipitate Phases"; }

