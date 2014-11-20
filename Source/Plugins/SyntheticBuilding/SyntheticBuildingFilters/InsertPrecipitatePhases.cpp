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
#include "DREAM3DLib/Math/OrientationMath.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"
#include "DREAM3DLib/ShapeOps/CubeOctohedronOps.h"
#include "DREAM3DLib/ShapeOps/CylinderOps.h"
#include "DREAM3DLib/ShapeOps/EllipsoidOps.h"
#include "DREAM3DLib/ShapeOps/SuperEllipsoidOps.h"
#include "DREAM3DLib/StatsData/PrecipitateStatsData.h"



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
  DREAM3D_FILTER_WRITE_PARAMETER(NumFeaturesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeaturePhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(InputStatsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(InputPhaseTypesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(InputShapeTypesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(BoundaryCellsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CellPhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(PeriodicBoundaries)
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
  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, m_FeatureIdsArrayPath.getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  //Input Ensemble Data That we require
  typedef DataArray<unsigned int> PhaseTypeArrayType;
  typedef DataArray<unsigned int> ShapeTypeArrayType;

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

  if(m_HavePrecips == false)
  {
    notifyStatusMessage(getHumanLabel(), "Packing Precipitates - Generating and Placing Precipitates");
    // this initializes the arrays to hold the details of the locations of all of the features during packing
    Int32ArrayType::Pointer featureOwnersPtr = initialize_packinggrid();

    place_precipitates(featureOwnersPtr);
  }

  if(m_HavePrecips == true)
  {
    load_precipitates();
  }

  notifyStatusMessage(getHumanLabel(), "Packing Precipitates - Assigning Voxels");
  assign_voxels();

  notifyStatusMessage(getHumanLabel(), "Packing Precipitates - Filling Gaps");
  assign_gaps();

  notifyStatusMessage(getHumanLabel(), "Packing Precipitates - Cleaning Up Volume");
  cleanup_features();

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

  //VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_FeatureIdsArrayPath.getDataContainerName());
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

  firstPrecipitateFeature = cellFeatureAttrMat->getNumTuples();

  QVector<size_t> tDims(1, firstPrecipitateFeature + numPrecips);
  cellFeatureAttrMat->setTupleDimensions(tDims);
  updateFeatureInstancePointers();

  int phase;
  float xC, yC, zC;
  float axisA, axisB, axisC;
  float vol, eqDiam;
  float omega3;
  float phi1, PHI, phi2;
  size_t currentFeature = firstPrecipitateFeature;
  const float fourThirds = 4.0f / 3.0f;
  for(int i = 0; i < numPrecips; i++)
  {
    inFile >> phase >> xC >> yC >> zC >> axisA >> axisB >> axisC >> omega3 >> phi1 >> PHI >> phi2;
    vol = fourThirds * DREAM3D::Constants::k_Pi * axisA * axisB * axisC;
    eqDiam = 2.0f*powf((vol * (0.75f) * (DREAM3D::Constants::k_1OverPi)), (DREAM3D::Constants::k_1Over3));
    m_Centroids[3 * currentFeature + 0] = xC;
    m_Centroids[3 * currentFeature + 1] = yC;
    m_Centroids[3 * currentFeature + 2] = zC;
    m_Volumes[currentFeature] = vol;
    m_EquivalentDiameters[currentFeature] = eqDiam;
    m_AxisLengths[3 * currentFeature + 0] = axisA/axisA;
    m_AxisLengths[3 * currentFeature + 1] = axisB/axisA;
    m_AxisLengths[3 * currentFeature + 2] = axisC/axisA;
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
void  InsertPrecipitatePhases::place_precipitates(Int32ArrayType::Pointer featureOwnersPtr)
{
  bool writeErrorFile = true;
  std::ofstream outFile;
  if(m_ErrorOutputFile.isEmpty() == false)
  {
    outFile.open(m_ErrorOutputFile.toLatin1().data(), std::ios_base::binary);
    writeErrorFile = true;
  }

  notifyStatusMessage(getHumanLabel(), "Placing Precipitates");
  DREAM3D_RANDOMNG_NEW()

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_FeatureIdsArrayPath.getDataContainerName());

  StatsDataArray& statsDataArray = *(m_StatsDataArray.lock());

  size_t udims[3] =
  { 0, 0, 0 };
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  { static_cast<DimType>(udims[0]), static_cast<DimType>(udims[1]), static_cast<DimType>(udims[2]), };

  sizex = dims[0] * m->getXRes();
  sizey = dims[1] * m->getYRes();
  sizez = dims[2] * m->getZRes();
  totalvol = sizex * sizey * sizez;

  int64_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();

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
  firstPrecipitateFeature = currentnumfeatures;
  int phase;
  float precipboundaryfraction = 0.0;
  float random = 0.0f;
  int random2 = 0;
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

  Precip precip;
  std::vector<float> curphasevol;
  curphasevol.resize(precipitatephases.size());
  float change = 0.0f;
  float factor = 1.0;
  size_t iter = 0;
  for (size_t j = 0; j < precipitatephases.size(); ++j)
  {
    curphasevol[j] = 0;
    float curphasetotalvol = static_cast<float>(totalvol * totalprecipitatefractions * precipitatephasefractions[j]);
    while (curphasevol[j] < (factor * curphasetotalvol))
    {
      iter++;
      Seed++;
      phase = precipitatephases[j];
      generate_precipitate(phase, static_cast<int>(Seed), &precip, m_ShapeTypes[phase], m_OrthoOps);
      currentsizedisterror = check_sizedisterror(&precip);
      change = (currentsizedisterror) - (oldsizedisterror);
      if(change > 0 || currentsizedisterror > (1.0 - (float(iter) * 0.001)) || curphasevol[j] < (0.75 * factor * curphasetotalvol))
      {
        QString ss = QObject::tr("Packing Precipitates - Generating Feature #%1").arg(currentnumfeatures);
        notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);

        tDims[0] = currentnumfeatures + 1;
        m->getAttributeMatrix(getFeaturePhasesArrayPath().getAttributeMatrixName())->resizeAttributeArrays(tDims);
        updateFeatureInstancePointers();
        transfer_attributes(currentnumfeatures, &precip);
        oldsizedisterror = currentsizedisterror;
        curphasevol[j] = curphasevol[j] + m_Volumes[currentnumfeatures];
        //FIXME: Initialize the Feature with some sort of default data
        iter = 0;
        m_NumFeatures[phase]++;
        currentnumfeatures++;
      }
    }
  }

  // initialize the sim and goal clustering distribution for the primary phases
  clusteringdist.resize(precipitatephases.size());
  simclusteringdist.resize(precipitatephases.size());
  clusteringdiststep.resize(precipitatephases.size());
  for (size_t i = 0; i < precipitatephases.size(); i++)
  {
    phase = precipitatephases[i];
    PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[phase].get());
    clusteringdist[i].resize(pp->getBinNumbers()->getSize());
    //    int binNumbers = pp->getBinNumbers();
    simclusteringdist[i].resize(pp->getBinNumbers()->getSize());
    VectorOfFloatArray Neighdist = pp->getFeatureSize_Clustering();
    float normalizer = 0;
    for (size_t j = 0; j < clusteringdist[i].size(); j++)
    {
      clusteringdist[i][j].resize(40);
      simclusteringdist[i][j].resize(40);
      float input = 0;
      float previoustotal = 0;
      float avg = Neighdist[0]->getValue(j);
      float stdev = Neighdist[1]->getValue(j);
      clusteringdiststep[i] = 2;
      float denominatorConst = sqrtf(2.0f * stdev * stdev); // Calculate it here rather than calculating the same thing multiple times below
      for (size_t k = 0; k < clusteringdist[i][j].size(); k++)
      {
        input = (float(k + 1) * clusteringdiststep[i]);
        float logInput = logf(input);
        if(logInput <= avg)
        {
          clusteringdist[i][j][k] = 0.5f - 0.5f * (DREAM3DMath::erf((avg - logInput) / denominatorConst)) - previoustotal;
        }
        if(logInput > avg)
        {
          clusteringdist[i][j][k] = 0.5f + 0.5f * (DREAM3DMath::erf((logInput - avg) / denominatorConst)) - previoustotal;
        }
        previoustotal = previoustotal + clusteringdist[i][j][k];
      }
      normalizer = normalizer + previoustotal;
    }
    for (size_t j = 0; j < clusteringdist[i].size(); j++)
    {
      for (size_t k = 0; k < clusteringdist[i][j].size(); k++)
      {
        clusteringdist[i][j][k] = clusteringdist[i][j][k] / normalizer;
        simclusteringdist[i][j][k] = 0;
      }
    }
  }



  //initializing the target RDF vector - this is the radial distribution function we are trying to match to
   for (int64_t i = 1; i < numensembles; ++i)
   {
     if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
     {
       PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[i].get());
       VectorOfFloatArray rdfTarget = pp->getRadialDistFunction();
       VectorOfFloatArray maxmin = pp->getMaxMinRDF();

       m_numRDFbins = rdfTarget[0]->getNumberOfTuples();
 //      std::vector<float> rdfTargetDist;
       m_rdfTargetDist.resize(m_numRDFbins);
       m_rdfCurrentDist.resize(m_numRDFbins);

       for (size_t j = 0; j < m_numRDFbins; j++)
       {
           m_rdfTargetDist[j] = rdfTarget[0]->getValue(j);
       }

        m_rdfMax = maxmin[0]->getValue(0);
        m_rdfMin = maxmin[0]->getValue(1);

     }
   }








  //  for each feature : select centroid, determine voxels in feature, monitor filling error and decide of the 10 placements which
  // is the most beneficial, then the feature is added and its clustering are determined

  size_t numfeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();

  columnlist.resize(numfeatures);
  rowlist.resize(numfeatures);
  planelist.resize(numfeatures);
  packqualities.resize(numfeatures);
  fillingerror = 1;
  for (size_t i = firstPrecipitateFeature; i < numfeatures; i++)
  {

    QString ss = QObject::tr("Packing Precipitates - Placing Precipitate #%1").arg(i);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);

    PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[m_FeaturePhases[i]].get());
    precipboundaryfraction = pp->getPrecipBoundaryFraction();
    random = static_cast<float>(rg.genrand_res53());
    if(random <= precipboundaryfraction)
    {
      // figure out if we want this to be a boundary centroid voxel or not for the proposed precipitate
      random2 = int(rg.genrand_res53() * double(totalPoints - 1));
      while (m_BoundaryCells[random2] == 0 || m_FeatureIds[random2] >= firstPrecipitateFeature)
      {
        random2++;
        if(random2 >= totalPoints) { random2 = static_cast<int>(random2 - totalPoints); }
      }
    }
    else if(random > precipboundaryfraction)
    {
      random2 = static_cast<int>(rg.genrand_res53() * (totalPoints - 1));
      while (m_BoundaryCells[random2] != 0 || m_FeatureIds[random2] >= firstPrecipitateFeature)
      {
        random2++;
        if(random2 >= totalPoints) { random2 = static_cast<int>(random2 - totalPoints); }
      }
    }
    xc = find_xcoord(random2);
    yc = find_ycoord(random2);
    zc = find_zcoord(random2);
    m_Centroids[3 * i] = xc;
    m_Centroids[3 * i + 1] = yc;
    m_Centroids[3 * i + 2] = zc;
    insert_precipitate(i);
    //    fillingerror = check_fillingerror(i, -1000, featureOwnersPtr);

    // try moving precipitate around 10 times, figure out which one of those is the best
    //    for (int iter_fill = 0; iter_fill < 10; iter_fill++)
    //    {
    //      random = static_cast<float>(rg.genrand_res53());
    //      if(random <= precipboundaryfraction)
    //      {
    //        random2 = int(rg.genrand_res53() * double(totalPoints - 1));
    //        while (m_BoundaryCells[random2] == 0 || m_FeatureIds[random2] >= firstPrecipitateFeature)
    //        {
    //          random2++;
    //          if(random2 >= totalPoints) { random2 = static_cast<int>(random2 - totalPoints); }
    //        }
    //      }
    //      else if(random > precipboundaryfraction)
    //      {
    //        random2 = static_cast<int>(rg.genrand_res53() * (totalPoints - 1));
    //        while (m_BoundaryCells[random2] != 0 || m_FeatureIds[random2] >= firstPrecipitateFeature)
    //        {
    //          random2++;
    //          if(random2 >= totalPoints) { random2 = static_cast<int>(random2 - totalPoints); }
    //        }
    //      }
    //      xc = find_xcoord(random2);
    //      yc = find_ycoord(random2);
    //      zc = find_zcoord(random2);
    //      oldxc = m_Centroids[3 * i];
    //      oldyc = m_Centroids[3 * i + 1];
    //      oldzc = m_Centroids[3 * i + 2];
    //      oldfillingerror = fillingerror;
    //      fillingerror = check_fillingerror(-1000, i, featureOwnersPtr);
    //      move_precipitate(i, xc, yc, zc);
    //      fillingerror = check_fillingerror(i, -1000, featureOwnersPtr);
    //      if(fillingerror > oldfillingerror)
    //      {
    //        fillingerror = check_fillingerror(-1000, i, featureOwnersPtr);
    //        move_precipitate(i, oldxc, oldyc, oldzc);
    //        fillingerror = check_fillingerror(i, -1000, featureOwnersPtr);
    //      }
    //    }
  }
  for (size_t i = firstPrecipitateFeature; i < numfeatures; i++)
  {
    oldclusteringerror = check_clusteringerror(i, -1000);
    for (size_t k = 0; k < clusteringdist[0][0].size(); k++)
    {
      outFile << clusteringdist[0][0][k] << " " << simclusteringdist[0][0][k] << "\n";
    }
    outFile << "\n\n";
  }

  notifyStatusMessage(getHumanLabel(), "Packing Features - Initial Feature Placement Complete");


  // begin swaping/moving/adding/removing features to try to improve packing
  int totalAdjustments = static_cast<int>(10 * ((numfeatures - firstPrecipitateFeature) - 1));
  for (int iteration = 0; iteration < totalAdjustments; ++iteration)
  {

    QString ss;
    ss = QObject::tr("Packing Features - Swapping/Moving/Adding/Removing Features Iteration %1/%2").arg(iteration).arg(totalAdjustments);
    if(iteration % 100 == 0) { notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss); }

    //    change1 = 0;
    //    change2 = 0;
    int option = iteration % 2;

    if(writeErrorFile == true && iteration % 25 == 0)
    {
      outFile << iteration << " " << oldclusteringerror << " " << acceptedmoves
              << "\n";

      for (size_t k = 0; k < clusteringdist[0][0].size(); k++)
      {
        outFile << clusteringdist[0][0][k] << " " << simclusteringdist[0][0][k] << "\n";
      }
    }

    // JUMP - this option moves one feature to a random spot in the volume
    if(option == 0)
    {
      randomfeature = firstPrecipitateFeature + int(rg.genrand_res53() * (numfeatures - firstPrecipitateFeature));
      if(randomfeature < firstPrecipitateFeature) { randomfeature = firstPrecipitateFeature; }
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
      if(random <= precipboundaryfraction)
      {
        random2 = int(rg.genrand_res53() * double(totalPoints - 1));
        while (m_BoundaryCells[random2] == 0 || m_FeatureIds[random2] >= firstPrecipitateFeature)
        {
          random2++;
          if(random2 >= totalPoints) { random2 = static_cast<int>(random2 - totalPoints); }
        }
      }
      else if(random > precipboundaryfraction)
      {
        random2 = static_cast<int>(rg.genrand_res53() * (totalPoints - 1));
        while (m_BoundaryCells[random2] != 0 || m_FeatureIds[random2] >= firstPrecipitateFeature)
        {
          random2++;
          if(random2 >= totalPoints) { random2 = static_cast<int>(random2 - totalPoints); }
        }
      }
      xc = find_xcoord(random2);
      yc = find_ycoord(random2);
      zc = find_zcoord(random2);
      oldxc = m_Centroids[3 * randomfeature];
      oldyc = m_Centroids[3 * randomfeature + 1];
      oldzc = m_Centroids[3 * randomfeature + 2];
//      oldfillingerror = fillingerror;
//      fillingerror = check_fillingerror(-1000, static_cast<int>(randomfeature), featureOwnersPtr);
//      move_precipitate(randomfeature, xc, yc, zc);
//      fillingerror = check_fillingerror(static_cast<int>(randomfeature), -1000, featureOwnersPtr);

      currentclusteringerror = check_clusteringerror(-1000, randomfeature);
      move_precipitate(randomfeature, xc, yc, zc);
      currentclusteringerror = check_clusteringerror(randomfeature, -1000);
      if(currentclusteringerror >= oldclusteringerror)

//      if(fillingerror >= oldfillingerror)
      {
        oldclusteringerror = currentclusteringerror;
//        oldfillingerror = fillingerror;
        acceptedmoves++;
      }
      else
      {
//        fillingerror = check_fillingerror(-1000, static_cast<int>(randomfeature), featureOwnersPtr);
//        move_precipitate(randomfeature, oldxc, oldyc, oldzc);
//        fillingerror = check_fillingerror(static_cast<int>(randomfeature), -1000, featureOwnersPtr);
          //        oldfillingerror = fillingerror;
        currentclusteringerror = check_clusteringerror(-1000, randomfeature);
        move_precipitate(randomfeature, oldxc, oldyc, oldzc);
        currentclusteringerror = check_clusteringerror(randomfeature, -1000);
          oldclusteringerror = currentclusteringerror;

      }
    }


    // NUDGE - this option moves one feature to a spot close to its current centroid
    if(option == 1)
    {
      randomfeature = firstPrecipitateFeature + int(rg.genrand_res53() * (numfeatures - firstPrecipitateFeature));
      if(randomfeature < firstPrecipitateFeature) { randomfeature = firstPrecipitateFeature; }
      if(randomfeature >= static_cast<int>(numfeatures))
      {
        randomfeature = static_cast<int>(numfeatures) - 1;
      }
      Seed++;
      oldxc = m_Centroids[3 * randomfeature];
      oldyc = m_Centroids[3 * randomfeature + 1];
      oldzc = m_Centroids[3 * randomfeature + 2];
      xc = static_cast<float>(oldxc + ((2.0f * (rg.genrand_res53() - 0.5f)) * (2.0f * m_PackingRes[0])));
      yc = static_cast<float>(oldyc + ((2.0f * (rg.genrand_res53() - 0.5f)) * (2.0f * m_PackingRes[1])));
      zc = static_cast<float>(oldzc + ((2.0f * (rg.genrand_res53() - 0.5f)) * (2.0f * m_PackingRes[2])));
//      oldfillingerror = fillingerror;
//      fillingerror = check_fillingerror(-1000, static_cast<int>(randomfeature), featureOwnersPtr);
//      move_precipitate(randomfeature, xc, yc, zc);
//      fillingerror = check_fillingerror(static_cast<int>(randomfeature), -1000, featureOwnersPtr);
      currentclusteringerror = check_clusteringerror(-1000, randomfeature);
      move_precipitate(randomfeature, xc, yc, zc);
      currentclusteringerror = check_clusteringerror(randomfeature, -1000);
      if(currentclusteringerror >= oldclusteringerror)
//      if(fillingerror >= oldfillingerror)
      {
        oldclusteringerror = currentclusteringerror;
//        oldfillingerror = fillingerror;
        acceptedmoves++;
      }
      else
      {
//        fillingerror = check_fillingerror(-1000, static_cast<int>(randomfeature), featureOwnersPtr);
//        move_precipitate(randomfeature, oldxc, oldyc, oldzc);
//        fillingerror = check_fillingerror(static_cast<int>(randomfeature), -1000, featureOwnersPtr);
          //        oldfillingerror = fillingerror;
        currentclusteringerror = check_clusteringerror(-1000, randomfeature);
        move_precipitate(randomfeature, oldxc, oldyc, oldzc);
        currentclusteringerror = check_clusteringerror(randomfeature, -1000);
  oldclusteringerror = currentclusteringerror;

      }
    }
  }

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
  occolumn = static_cast<int>( (oxc - (m_PackingRes[0] / 2.0f)) / m_PackingRes[0] );
  ocrow = static_cast<int>( (oyc - (m_PackingRes[1] / 2.0f)) / m_PackingRes[1] );
  ocplane = static_cast<int>( (ozc - (m_PackingRes[2] / 2.0f)) / m_PackingRes[2] );
  nccolumn = static_cast<int>( (xc - (m_PackingRes[0] / 2.0f)) / m_PackingRes[0] );
  ncrow = static_cast<int>( (yc - (m_PackingRes[1] / 2.0f)) / m_PackingRes[1] );
  ncplane = static_cast<int>( (zc - (m_PackingRes[2] / 2.0f)) / m_PackingRes[2] );
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
void InsertPrecipitatePhases::determine_clustering(size_t gnum, int add)
{
  //VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_FeatureIdsArrayPath.getDataContainerName());

  float x, y, z;
  float xn, yn, zn;
  float r;
  float dia;
  int iter = 0;
  int diabin, clusterbin;

  int phase = m_FeaturePhases[gnum];
  while (phase != precipitatephases[iter]) { iter++; }

  StatsDataArray& statsDataArray = *(m_StatsDataArray.lock());
  typedef std::vector<std::vector<float> > VectOfVectFloat_t;

  PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[phase].get());
  //  VectOfVectFloat_t& curSimClusteringDist = simclusteringdist[iter];
  //  size_t curSImClusteringDist_Size = curSimClusteringDist.size();
  float oneOverClusteringDistStep = 1.0f / clusteringdiststep[iter];

  float maxFeatureDia = pp->getMaxFeatureDiameter();
  float minFeatureDia = pp->getMinFeatureDiameter();
  float oneOverBinStepSize = 1.0f / pp->getBinStepSize();

  x = m_Centroids[3 * gnum];
  y = m_Centroids[3 * gnum + 1];
  z = m_Centroids[3 * gnum + 2];
  size_t numFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();

  for (size_t n = firstPrecipitateFeature; n < numFeatures; n++)
  {
    if (m_FeaturePhases[n] == phase && n != gnum)
    {
      xn = m_Centroids[3 * n];
      yn = m_Centroids[3 * n + 1];
      zn = m_Centroids[3 * n + 2];
      r = sqrtf((x - xn) * (x - xn) + (y - yn) * (y - yn) + (z - zn) * (z - zn));

      dia = m_EquivalentDiameters[gnum];
      //      dia2 = m_EquivalentDiameters[n];
      if(dia > maxFeatureDia) { dia = maxFeatureDia; }
      if(dia < minFeatureDia) { dia = minFeatureDia; }
      //      if(dia2 > maxFeatureDia) { dia2 = maxFeatureDia; }
      //      if(dia2 < minFeatureDia) { dia2 = minFeatureDia; }
      diabin = static_cast<size_t>(((dia - minFeatureDia) * oneOverBinStepSize) );
      //      dia2bin = static_cast<size_t>(((dia2 - minFeatureDia) * oneOverBinStepSize) );
      clusterbin = static_cast<size_t>( r * oneOverClusteringDistStep );
      if(clusterbin >= 40) { clusterbin = 39; }
      simclusteringdist[iter][diabin][clusterbin] += add;
            //      simclusteringdist[iter][dia2bin][clusterbin] += add;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float InsertPrecipitatePhases::check_clusteringerror(int gadd, int gremove)
{

  float clusteringerror;
  float bhattdist;

  for (size_t iter = 0; iter < simclusteringdist.size(); ++iter)
  {
    if(gadd > 0)
    {
      determine_clustering(gadd, 1);
    }
    if(gremove > 0)
    {
      determine_clustering(gremove, -1);
    }
  }
  compare_3Ddistributions(simclusteringdist, clusteringdist, bhattdist);
  clusteringerror = bhattdist;
  return clusteringerror;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::determine_currentRDF(size_t gnum, int add)
{
  //VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_FeatureIdsArrayPath.getDataContainerName());

  float x, y, z;
  float xn, yn, zn;
  float r;
//  float dia;
  int iter = 0;
  int diabin, clusterbin;

  int phase = m_FeaturePhases[gnum];
  while (phase != precipitatephases[iter]) { iter++; }

  StatsDataArray& statsDataArray = *(m_StatsDataArray.lock());
  typedef std::vector<std::vector<float> > VectOfVectFloat_t;

  PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[phase].get());
  //  VectOfVectFloat_t& curSimClusteringDist = simclusteringdist[iter];
  //  size_t curSImClusteringDist_Size = curSimClusteringDist.size();
  float oneOverClusteringDistStep = 1.0f / clusteringdiststep[iter];

//  float maxFeatureDia = pp->getMaxFeatureDiameter();
//  float minFeatureDia = pp->getMinFeatureDiameter();
//  float oneOverBinStepSize = 1.0f / pp->getBinStepSize();

  x = m_Centroids[3 * gnum];
  y = m_Centroids[3 * gnum + 1];
  z = m_Centroids[3 * gnum + 2];
  size_t numFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();

  for (size_t n = firstPrecipitateFeature; n < numFeatures; n++)
  {
    if (m_FeaturePhases[n] == phase && n != gnum)
    {
      xn = m_Centroids[3 * n];
      yn = m_Centroids[3 * n + 1];
      zn = m_Centroids[3 * n + 2];
      r = sqrtf((x - xn) * (x - xn) + (y - yn) * (y - yn) + (z - zn) * (z - zn));

//      dia = m_EquivalentDiameters[gnum];
//      //      dia2 = m_EquivalentDiameters[n];
//      if(dia > maxFeatureDia) { dia = maxFeatureDia; }
//      if(dia < minFeatureDia) { dia = minFeatureDia; }
      //      if(dia2 > maxFeatureDia) { dia2 = maxFeatureDia; }
      //      if(dia2 < minFeatureDia) { dia2 = minFeatureDia; }
//      diabin = static_cast<size_t>(((dia - minFeatureDia) * oneOverBinStepSize) );
      //      dia2bin = static_cast<size_t>(((dia2 - minFeatureDia) * oneOverBinStepSize) );
      clusterbin = static_cast<size_t>( r * oneOverClusteringDistStep );
      if(clusterbin >= 40) { clusterbin = 39; }
      simclusteringdist[iter][diabin][clusterbin] += add;
            //      simclusteringdist[iter][dia2bin][clusterbin] += add;
    }
  }
}







//}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float InsertPrecipitatePhases::check_RDFerror(int gadd, int gremove)
{

  float clusteringerror;
  float bhattdist;

  for (size_t iter = 0; iter < simclusteringdist.size(); ++iter)
  {
    if(gadd > 0)
    {
      determine_clustering(gadd, 1);
    }
    if(gremove > 0)
    {
      determine_clustering(gremove, -1);
    }
  }
  compare_1Ddistributions(m_rdfTargetDist, m_rdfCurrentDist, bhattdist);
  clusteringerror = bhattdist;
  return clusteringerror;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::compare_1Ddistributions(std::vector<float> array1, std::vector<float> array2, float& bhattdist)
{
  bhattdist = 0;
  for (size_t i = 0; i < array1.size(); i++)
  {
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
  //VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_FeatureIdsArrayPath.getDataContainerName());

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
    for (size_t b = firstPrecipitateFeature; b < nFeatureTuples; b++)
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
float InsertPrecipitatePhases::check_fillingerror(int gadd, int gremove, Int32ArrayType::Pointer featureOwnersPtr)
{
  size_t featureOwnersIdx = 0;
  int32_t* featureOwners = featureOwnersPtr->getPointer(0);
  fillingerror = fillingerror * float(m_TotalPackingPoints);
  int col, row, plane;
  if(gadd > 0)
  {
    size_t size = columnlist[gadd].size();
    std::vector<int>& cl_gadd = columnlist[gadd];
    std::vector<int>& rl_gadd = rowlist[gadd];
    std::vector<int>& pl_gadd = planelist[gadd];
    float packquality = 0;
    for (size_t i = 0; i < size; i++)
    {
      col = cl_gadd[i];
      row = rl_gadd[i];
      plane = pl_gadd[i];

      if(m_PeriodicBoundaries == true)
      {
        if(col < 0) { col = col + m_PackingPoints[0]; }
        if(col > m_PackingPoints[0] - 1) { col = col - m_PackingPoints[0]; }
        if(row < 0) { row = row + m_PackingPoints[1]; }
        if(row > m_PackingPoints[1] - 1) { row = row - m_PackingPoints[1]; }
        if(plane < 0) { plane = plane + m_PackingPoints[2]; }
        if(plane > m_PackingPoints[2] - 1) { plane = plane - m_PackingPoints[2]; }
        featureOwnersIdx = (m_PackingPoints[0] * m_PackingPoints[1] * plane) + (m_PackingPoints[0] * row) + col;
        int currentFeatureOwner = featureOwners[featureOwnersIdx];
        fillingerror = fillingerror + (2 * currentFeatureOwner - 1);
        packquality = packquality + ((currentFeatureOwner) * (currentFeatureOwner));
        ++currentFeatureOwner;
      }
      else
      {
        if(col >= 0 && col <= m_PackingPoints[0] - 1 && row >= 0 && row <= m_PackingPoints[1] - 1 && plane >= 0 && plane <= m_PackingPoints[2] - 1)
        {
          featureOwnersIdx = (m_PackingPoints[0] * m_PackingPoints[1] * plane) + (m_PackingPoints[0] * row) + col;
          int currentFeatureOwner = featureOwners[featureOwnersIdx];
          fillingerror = fillingerror + (2 * currentFeatureOwner - 1);
          packquality = packquality + ((currentFeatureOwner) * (currentFeatureOwner));
          ++currentFeatureOwner;
        }
      }
    }
    packqualities[gadd] = static_cast<int>( packquality / float(size) );
  }
  if(gremove > 0)
  {
    size_t size = columnlist[gremove].size();
    std::vector<int>& cl_gremove = columnlist[gremove];
    std::vector<int>& rl_gremove = rowlist[gremove];
    std::vector<int>& pl_gremove = planelist[gremove];
    for (size_t i = 0; i < size; i++)
    {
      col = cl_gremove[i];
      row = rl_gremove[i];
      plane = pl_gremove[i];
      if(m_PeriodicBoundaries == true)
      {
        if(col < 0) { col = col + m_PackingPoints[0]; }
        if(col > m_PackingPoints[0] - 1) { col = col - m_PackingPoints[0]; }
        if(row < 0) { row = row + m_PackingPoints[1]; }
        if(row > m_PackingPoints[1] - 1) { row = row - m_PackingPoints[1]; }
        if(plane < 0) { plane = plane + m_PackingPoints[2]; }
        if(plane > m_PackingPoints[2] - 1) { plane = plane - m_PackingPoints[2]; }
        featureOwnersIdx = (m_PackingPoints[0] * m_PackingPoints[1] * plane) + (m_PackingPoints[0] * row) + col;
        int currentFeatureOwner = featureOwners[featureOwnersIdx];
        fillingerror = fillingerror + (-2 * currentFeatureOwner + 3);
        currentFeatureOwner = currentFeatureOwner - 1;
      }
      else
      {
        if(col >= 0 && col <= m_PackingPoints[0] - 1 && row >= 0 && row <= m_PackingPoints[1] - 1 && plane >= 0 && plane <= m_PackingPoints[2] - 1)
        {
          featureOwnersIdx = (m_PackingPoints[0] * m_PackingPoints[1] * plane) + (m_PackingPoints[0] * row) + col;
          int currentFeatureOwner = featureOwners[featureOwnersIdx];
          fillingerror = fillingerror + (-2 * currentFeatureOwner + 3);
          currentFeatureOwner = currentFeatureOwner - 1;
        }
      }
    }
  }
  fillingerror = fillingerror / float(m_TotalPackingPoints);
  return fillingerror;
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
  centercolumn = static_cast<int>( (xc - (m_PackingRes[0] / 2)) / m_PackingRes[0] );
  centerrow = static_cast<int>( (yc - (m_PackingRes[1] / 2)) / m_PackingRes[1] );
  centerplane = static_cast<int>( (zc - (m_PackingRes[2] / 2)) / m_PackingRes[2] );
  xmin = int(centercolumn - ((radcur1 / m_PackingRes[0]) + 1));
  xmax = int(centercolumn + ((radcur1 / m_PackingRes[0]) + 1));
  ymin = int(centerrow - ((radcur1 / m_PackingRes[1]) + 1));
  ymax = int(centerrow + ((radcur1 / m_PackingRes[1]) + 1));
  zmin = int(centerplane - ((radcur1 / m_PackingRes[2]) + 1));
  zmax = int(centerplane + ((radcur1 / m_PackingRes[2]) + 1));
  if(xmin < -m_PackingPoints[0]) { xmin = -m_PackingPoints[0]; }
  if(xmax > 2 * m_PackingPoints[0] - 1) { xmax = (2 * m_PackingPoints[0] - 1); }
  if(ymin < -m_PackingPoints[1]) { ymin = -m_PackingPoints[1]; }
  if(ymax > 2 * m_PackingPoints[1] - 1) { ymax = (2 * m_PackingPoints[1] - 1); }
  if(zmin < -m_PackingPoints[2]) { zmin = -m_PackingPoints[2]; }
  if(zmax > 2 * m_PackingPoints[2] - 1) { zmax = (2 * m_PackingPoints[2] - 1); }
  for (int iter1 = xmin; iter1 < xmax + 1; iter1++)
  {
    for (int iter2 = ymin; iter2 < ymax + 1; iter2++)
    {
      for (int iter3 = zmin; iter3 < zmax + 1; iter3++)
      {
        column = iter1;
        row = iter2;
        plane = iter3;
        coords[0] = float(column) * m_PackingRes[0];
        coords[1] = float(row) * m_PackingRes[1];
        coords[2] = float(plane) * m_PackingRes[2];
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

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_FeatureIdsArrayPath.getDataContainerName());

  int index;
  size_t udims[3] = {0, 0, 0};
  m->getDimensions(udims);
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

  DimType neighpoints[6];
  neighpoints[0] = -dims[0] * dims[1];
  neighpoints[1] = -dims[0];
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = dims[0];
  neighpoints[5] = dims[0] * dims[1];

  float totalPoints = dims[0] * dims[1] * dims[2];
  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();

  int oldname;
  size_t column, row, plane;
  float inside;
  float xc, yc, zc;
  float coordsRotated[3];
  //float dist;
  float coords[3];
  DimType xmin, xmax, ymin, ymax, zmin, zmax;
  int64_t numFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();
  gsizes.resize(numFeatures);

  for (int64_t i = firstPrecipitateFeature; i < numFeatures; i++)
  {
    gsizes[i] = 0;
  }
  for (int64_t i = firstPrecipitateFeature; i < numFeatures; i++)
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
          if (iter1 < 0) { coords[0] = coords[0] - sizex; }
          if (iter1 > dims[0] - 1) { coords[0] = coords[0] + sizex; }
          if (iter2 < 0) { coords[1] = coords[1] - sizey; }
          if (iter2 > dims[1] - 1) { coords[1] = coords[1] + sizey; }
          if (iter3 < 0) { coords[2] = coords[2] - sizez; }
          if (iter3 > dims[2] - 1) { coords[2] = coords[2] + sizez; }
          //          dist = ((coords[0] - xc) * (coords[0] - xc)) + ((coords[1] - yc) * (coords[1] - yc)) + ((coords[2] - zc) * (coords[2] - zc));
          //          dist = sqrtf(dist);
          //          if (dist < radcur1)
          //          {
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
            if (m_FeatureIds[currentpoint] > firstPrecipitateFeature)
            {
              oldname = m_FeatureIds[currentpoint];
              m_FeatureIds[currentpoint] = -2;
            }
            if (m_FeatureIds[currentpoint] < firstPrecipitateFeature && m_FeatureIds[currentpoint] != -2)
            {
              m_FeatureIds[currentpoint] = static_cast<int32_t>(i);
            }
          }
          //          }
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

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_FeatureIdsArrayPath.getDataContainerName());

  int64_t totpoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();

  size_t udims[3] = {0, 0, 0};
  m->getDimensions(udims);
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

  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();

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
    for (int64_t i = firstPrecipitateFeature; i < numFeatures; i++)
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
              if (iter1 < 0) { coords[0] = coords[0] - sizex; }
              if (iter1 > dims[0] - 1) { coords[0] = coords[0] + sizex; }
              if (iter2 < 0) { coords[1] = coords[1] - sizey; }
              if (iter2 > dims[1] - 1) { coords[1] = coords[1] + sizey; }
              if (iter3 < 0) { coords[2] = coords[2] - sizez; }
              if (iter3 > dims[2] - 1) { coords[2] = coords[2] + sizez; }
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
void InsertPrecipitatePhases::cleanup_features()
{
  notifyStatusMessage(getHumanLabel(), "Cleaning Up Features");

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_FeatureIdsArrayPath.getDataContainerName());

  StatsDataArray& statsDataArray = *(m_StatsDataArray.lock());

  int64_t totpoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  size_t udims[3] = {0, 0, 0};
  m->getDimensions(udims);
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

  DimType neighpoints[6];
  DimType xp = dims[0];
  DimType yp = dims[1];
  DimType zp = dims[2];

  size_t numFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();

  neighpoints[0] = -(xp * yp);
  neighpoints[1] = -xp;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = xp;
  neighpoints[5] = (xp * yp);
  std::vector<std::vector<int> > vlists;
  vlists.resize(numFeatures);
  std::vector<int> currentvlist;
  std::vector<bool> checked(totpoints, false);
  QVector<bool> activeObjects(numFeatures, true);
  size_t count;
  int touchessurface = 0;
  int good;
  int neighbor;
  DimType column, row, plane;
  int index;
  float minsize = 0;
  gsizes.resize(numFeatures);
  for (size_t i = 1; i < numFeatures; i++)
  {
    gsizes[i] = 0;
  }

  float resConst = m->getXRes() * m->getYRes() * m->getZRes();
  for (int i = 0; i < totpoints; i++)
  {
    touchessurface = 0;
    if(checked[i] == false && m_FeatureIds[i] > firstPrecipitateFeature)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[m_CellPhases[i]].get());
      minsize = static_cast<float>( pp->getMinFeatureDiameter() * pp->getMinFeatureDiameter() * pp->getMinFeatureDiameter() * M_PI / 6.0f );
      minsize = static_cast<float>( int(minsize / (resConst)) );
      currentvlist.push_back(i);
      count = 0;
      while (count < currentvlist.size())
      {
        index = currentvlist[count];
        column = index % xp;
        row = (index / xp) % yp;
        plane = index / (xp * yp);
        if(column == 0 || column == xp || row == 0 || row == yp || plane == 0 || plane == zp) { touchessurface = 1; }
        for (int j = 0; j < 6; j++)
        {
          good = 1;
          neighbor = static_cast<int>( index + neighpoints[j] );
          if(m_PeriodicBoundaries == false)
          {
            if(j == 0 && plane == 0) { good = 0; }
            if(j == 5 && plane == (zp - 1)) { good = 0; }
            if(j == 1 && row == 0) { good = 0; }
            if(j == 4 && row == (yp - 1)) { good = 0; }
            if(j == 2 && column == 0) { good = 0; }
            if(j == 3 && column == (xp - 1)) { good = 0; }
            if(good == 1 && m_FeatureIds[neighbor] == m_FeatureIds[index] && checked[neighbor] == false)
            {
              currentvlist.push_back(neighbor);
              checked[neighbor] = true;
            }
          }
          else if(m_PeriodicBoundaries == true)
          {
            if(j == 0 && plane == 0) { neighbor = static_cast<int>( neighbor + (xp * yp * zp) ); }
            if(j == 5 && plane == (zp - 1)) { neighbor = static_cast<int>( neighbor - (xp * yp * zp) ); }
            if(j == 1 && row == 0) { neighbor = static_cast<int>( neighbor + (xp * yp) ); }
            if(j == 4 && row == (yp - 1)) { neighbor = static_cast<int>( neighbor - (xp * yp) ); }
            if(j == 2 && column == 0) { neighbor = static_cast<int>( neighbor + (xp) ); }
            if(j == 3 && column == (xp - 1)) { neighbor = static_cast<int>( neighbor - (xp) ); }
            if(m_FeatureIds[neighbor] == m_FeatureIds[index] && checked[neighbor] == false)
            {
              currentvlist.push_back(neighbor);
              checked[neighbor] = true;
            }
          }
        }
        count++;
      }
      size_t size = vlists[m_FeatureIds[i]].size();
      if(size > 0)
      {
        if(size < currentvlist.size())
        {
          for (size_t k = 0; k < vlists[m_FeatureIds[i]].size(); k++)
          {
            m_FeatureIds[vlists[m_FeatureIds[i]][k]] = -1;
          }
          vlists[m_FeatureIds[i]].resize(currentvlist.size());
          vlists[m_FeatureIds[i]].swap(currentvlist);
        }
        else if(size >= currentvlist.size())
        {
          for (size_t k = 0; k < currentvlist.size(); k++)
          {
            m_FeatureIds[currentvlist[k]] = -1;
          }
        }
      }
      else if(size == 0)
      {
        if(currentvlist.size() >= minsize || touchessurface == 1)
        {
          vlists[m_FeatureIds[i]].resize(currentvlist.size());
          vlists[m_FeatureIds[i]].swap(currentvlist);
        }
        if(currentvlist.size() < minsize && touchessurface == 0)
        {
          for (size_t k = 0; k < currentvlist.size(); k++)
          {
            m_FeatureIds[currentvlist[k]] = -1;
          }
        }
      }
      currentvlist.clear();
    }
  }
  assign_gaps();
  for (int i = 0; i < totpoints; i++)
  {
    if(m_FeatureIds[i] > 0) { gsizes[m_FeatureIds[i]]++; }
  }
  for (size_t i = firstPrecipitateFeature; i < numFeatures; i++)
  {
    if(gsizes[i] == 0) { activeObjects[i] = false; }
  }

  AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(getFeaturePhasesArrayPath().getAttributeMatrixName());
  cellFeatureAttrMat->removeInactiveObjects(activeObjects, m_FeatureIdsPtr.lock());
  updateFeatureInstancePointers();

  for (int i = 0; i < totpoints; i++)
  {
    if(m_FeatureIds[i] > 0) { m_CellPhases[i] = m_FeaturePhases[m_FeatureIds[i]]; }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Int32ArrayType::Pointer  InsertPrecipitatePhases::initialize_packinggrid()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_FeatureIdsArrayPath.getDataContainerName());

  m_PackingRes[0] = m->getXRes() * 2.0f;
  m_PackingRes[1] = m->getYRes() * 2.0f;
  m_PackingRes[2] = m->getZRes() * 2.0f;

  m_HalfPackingRes[0] = m_PackingRes[0] * 0.5;
  m_HalfPackingRes[1] = m_PackingRes[1] * 0.5;
  m_HalfPackingRes[2] = m_PackingRes[2] * 0.5;

  m_OneOverHalfPackingRes[0] = 1.0f / m_HalfPackingRes[0];
  m_OneOverHalfPackingRes[1] = 1.0f / m_HalfPackingRes[1];
  m_OneOverHalfPackingRes[2] = 1.0f / m_HalfPackingRes[2];

  m_PackingPoints[0] = m->getXPoints() / 2;
  m_PackingPoints[1] = m->getYPoints() / 2;
  m_PackingPoints[2] = m->getZPoints() / 2;

  m_TotalPackingPoints = m_PackingPoints[0] * m_PackingPoints[1] * m_PackingPoints[2];

  Int32ArrayType::Pointer featureOwnersPtr = Int32ArrayType::CreateArray(m_TotalPackingPoints, "PackPrimaryFeatures::feature_owners");
  featureOwnersPtr->initializeWithZeros();

  return featureOwnersPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float InsertPrecipitatePhases::find_xcoord(long long int index)
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_FeatureIdsArrayPath.getDataContainerName());

  float x = m->getXRes() * float(index % m->getXPoints());
  return x;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float InsertPrecipitatePhases::find_ycoord(long long int index)
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_FeatureIdsArrayPath.getDataContainerName());

  float y = m->getYRes() * float((index / m->getXPoints()) % m->getYPoints());
  return y;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float InsertPrecipitatePhases::find_zcoord(long long int index)
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_FeatureIdsArrayPath.getDataContainerName());

  float z = m->getZRes() * float(index / (m->getXPoints() * m->getYPoints()));
  return z;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::write_goal_attributes()
{
  int err = 0;
  setErrorCondition(err);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_FeatureIdsArrayPath.getDataContainerName());

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
  dStream << static_cast<qint32>(numFeatures - firstPrecipitateFeature);
  // Get all the names of the arrays from the Data Container
  QList<QString> headers = m->getAttributeMatrix(getFeaturePhasesArrayPath().getAttributeMatrixName())->getAttributeArrayNameList();

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
  for(qint32 i = firstPrecipitateFeature; i < numTuples; ++i)
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
    for(qint32 p = 0; p < data.size(); ++p)
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

