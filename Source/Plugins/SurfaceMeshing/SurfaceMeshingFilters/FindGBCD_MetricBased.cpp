/*
 * Your License or Copyright can go here
 */

#include "FindGBCD_MetricBased.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/AxisAngleFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SurfaceMeshing/SurfaceMeshingConstants.h"

// Include the MOC generated file for this class
#include "moc_FindGBCD_MetricBased.cpp"

#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include <QtCore/QDir>
#include "OrientationLib/SpaceGroupOps/SpaceGroupOps.h"
#include <cmath>
#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindGBCD_MetricBased::FindGBCD_MetricBased() :
  SurfaceMeshFilter(),
  m_PhaseOfInterest(1),
  m_ChosenLimitDists(DEFAULT_RESOL_CHOICE),
  m_NumSamplPts(4000),
  m_AddMorePtsNearEquator(true),
  m_DistOutputFile(""), 
  m_ErrOutputFile(""),
  m_SaveRelativeErr(false),

  m_CrystalStructuresArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_FeatureEulerAnglesArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::AvgEulerAngles),
  m_FeaturePhasesArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_SurfaceMeshFaceLabelsArrayPath(DREAM3D::Defaults::TriangleDataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceNormalsArrayPath(DREAM3D::Defaults::TriangleDataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFaceNormals),
  m_SurfaceMeshFaceAreasArrayPath(DREAM3D::Defaults::TriangleDataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFaceAreas),
  m_SurfaceMeshFeatureFaceIdArrayPath(DREAM3D::Defaults::TriangleDataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFeatureFaceId),
  m_SurfaceMeshFeatureFaceNumTrianglesArrayPath(DREAM3D::Defaults::TriangleDataContainerName, DREAM3D::Defaults::FaceFeatureAttributeMatrixName, "NumTriangles"),

  m_CrystalStructures(NULL),
  m_FeatureEulerAngles(NULL),
  m_FeaturePhases(NULL),
  m_SurfaceMeshFaceLabels(NULL),
  m_SurfaceMeshFaceNormals(NULL),
  m_SurfaceMeshFeatureFaceId(NULL),
  m_SurfaceMeshFaceAreas(NULL),
  m_SurfaceMeshFeatureFaceNumTriangles(NULL)
{
	m_MisorientationRotation.angle = 38.94f;
	m_MisorientationRotation.h = 1.0f;
	m_MisorientationRotation.k = 1.0f;
	m_MisorientationRotation.l = 0.0f;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindGBCD_MetricBased::~FindGBCD_MetricBased()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD_MetricBased::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(IntFilterParameter::New("Phase of Interest", "PhaseOfInterest", getPhaseOfInterest(), FilterParameter::Parameter));
  parameters.push_back(AxisAngleFilterParameter::New("Fixed Misorientation", "MisorientationRotation", getMisorientationRotation(), FilterParameter::Parameter));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Limiting Distances");
    parameter->setPropertyName("ChosenLimitDists");
    
    QVector<QString> choices;          

	for (int choiceIdx = 0; choiceIdx < NUM_RESOL_CHOICES; choiceIdx++)
	{
		QString misorResStr;
		QString planeResStr;
		QString degSymbol = QChar(0x00B0);

		misorResStr.setNum(RESOL_CHOICES[choiceIdx][0], 'f', 0);
		planeResStr.setNum(RESOL_CHOICES[choiceIdx][1], 'f', 0);

		choices.push_back(misorResStr + degSymbol + " for Misorientations; " + planeResStr + degSymbol + " for Plane Inclinations");
	}

    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
	

  }
  parameters.push_back(IntFilterParameter::New("Number of Sampling Points (on a Hemisphere)", "NumSamplPts", getNumSamplPts(), FilterParameter::Parameter));
  parameters.push_back(BooleanFilterParameter::New("Include Points from the Southern Hemisphere from the Equator's Vicinity", "AddMorePtsNearEquator", getAddMorePtsNearEquator(), FilterParameter::Parameter));
  parameters.push_back(OutputFileFilterParameter::New("Save Distribution to", "DistOutputFile", getDistOutputFile(), FilterParameter::Parameter, ""));
  parameters.push_back(OutputFileFilterParameter::New("Save Distribution Errors to", "ErrOutputFile", getErrOutputFile(), FilterParameter::Parameter, ""));
  parameters.push_back(BooleanFilterParameter::New("Save Relative Errors Instead of Their Absolute Values", "SaveRelativeErr", getSaveRelativeErr(), FilterParameter::Parameter));


  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  {
	  DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::UInt32, 1, DREAM3D::AttributeMatrixType::CellEnsemble, DREAM3D::GeometryType::ImageGeometry);
	  parameters.push_back(DataArraySelectionFilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", getCrystalStructuresArrayPath(), FilterParameter::RequiredArray, req));
  }


  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
	  DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Float, 3, DREAM3D::AttributeMatrixType::CellFeature, DREAM3D::GeometryType::ImageGeometry);
	  parameters.push_back(DataArraySelectionFilterParameter::New("Average Euler Angles", "FeatureEulerAnglesArrayPath", getFeatureEulerAnglesArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
	  DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Int32, 1, DREAM3D::AttributeMatrixType::CellFeature, DREAM3D::GeometryType::ImageGeometry);
	  parameters.push_back(DataArraySelectionFilterParameter::New("Phases", "FeaturePhasesArrayPath", getFeaturePhasesArrayPath(), FilterParameter::RequiredArray, req));
  }


  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::RequiredArray));
  {
	  DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Int32, 2, DREAM3D::AttributeMatrixType::Face, DREAM3D::GeometryType::TriangleGeometry);
	  parameters.push_back(DataArraySelectionFilterParameter::New("Face Labels", "SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
	  DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Double, 3, DREAM3D::AttributeMatrixType::Face, DREAM3D::GeometryType::TriangleGeometry);
	  parameters.push_back(DataArraySelectionFilterParameter::New("Face Normals", "SurfaceMeshFaceNormalsArrayPath", getSurfaceMeshFaceNormalsArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
	  DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Double, 1, DREAM3D::AttributeMatrixType::Face, DREAM3D::GeometryType::TriangleGeometry);
	  parameters.push_back(DataArraySelectionFilterParameter::New("Face Areas", "SurfaceMeshFaceAreasArrayPath", getSurfaceMeshFaceAreasArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
	  DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Int32, 1, DREAM3D::AttributeMatrixType::Face, DREAM3D::GeometryType::TriangleGeometry);
	  parameters.push_back(DataArraySelectionFilterParameter::New("Feature Face Ids", "SurfaceMeshFeatureFaceIdArrayPath", getSurfaceMeshFeatureFaceIdArrayPath(), FilterParameter::RequiredArray, req));
  }


  parameters.push_back(SeparatorFilterParameter::New("Face Feature Data", FilterParameter::RequiredArray));
  {
	  DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Int32, 1, DREAM3D::AttributeMatrixType::Face, DREAM3D::GeometryType::TriangleGeometry);
	  parameters.push_back(DataArraySelectionFilterParameter::New("Numbers of Triangles", "SurfaceMeshFeatureFaceNumTrianglesArrayPath", getSurfaceMeshFeatureFaceNumTrianglesArrayPath(), FilterParameter::RequiredArray, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD_MetricBased::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setPhaseOfInterest(reader->readValue("PhaseOfInterest", getPhaseOfInterest()));
  setMisorientationRotation(reader->readAxisAngle("MisorientationRotation", getMisorientationRotation(), -1));
  setChosenLimitDists(reader->readValue("ChosenLimitDists", getChosenLimitDists()));
  setNumSamplPts(reader->readValue("NumSamplPts", getNumSamplPts()));
  setAddMorePtsNearEquator(reader->readValue("AddMorePtsNearEquator", getAddMorePtsNearEquator()));
  setDistOutputFile(reader->readString("DistOutputFile", getDistOutputFile()));
  setErrOutputFile(reader->readString("ErrOutputFile", getErrOutputFile()));
  setSaveRelativeErr(reader->readValue("SaveRelativeErr", getSaveRelativeErr()));

  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructures", getCrystalStructuresArrayPath()));
  setFeatureEulerAnglesArrayPath(reader->readDataArrayPath("FeatureEulerAngles", getFeatureEulerAnglesArrayPath()));
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhases", getFeaturePhasesArrayPath()));
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabels", getSurfaceMeshFaceLabelsArrayPath()));
  setSurfaceMeshFaceNormalsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceNormals", getSurfaceMeshFaceNormalsArrayPath()));
  setSurfaceMeshFeatureFaceIdArrayPath(reader->readDataArrayPath("SurfaceMeshFeatureFaceId", getSurfaceMeshFeatureFaceIdArrayPath()));
  setSurfaceMeshFaceAreasArrayPath(reader->readDataArrayPath("SurfaceMeshFaceAreas", getSurfaceMeshFaceAreasArrayPath()));
  setSurfaceMeshFeatureFaceNumTrianglesArrayPath(reader->readDataArrayPath("SurfaceMeshFeatureFaceNumTriangles", getSurfaceMeshFeatureFaceNumTrianglesArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindGBCD_MetricBased::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(PhaseOfInterest)
  SIMPL_FILTER_WRITE_PARAMETER(MisorientationRotation)
  SIMPL_FILTER_WRITE_PARAMETER(ChosenLimitDists)
  SIMPL_FILTER_WRITE_PARAMETER(NumSamplPts)
  SIMPL_FILTER_WRITE_PARAMETER(AddMorePtsNearEquator)
  SIMPL_FILTER_WRITE_PARAMETER(DistOutputFile)
  SIMPL_FILTER_WRITE_PARAMETER(ErrOutputFile)
  SIMPL_FILTER_WRITE_PARAMETER(SaveRelativeErr)

  SIMPL_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(FeatureEulerAnglesArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(FeaturePhasesArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshFaceLabelsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshFaceNormalsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshFeatureFaceIdArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshFaceAreasArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshFeatureFaceNumTrianglesArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD_MetricBased::dataCheck()
{
  setErrorCondition(0);


  // Fixed Misorientation (filter params.)
  if (getMisorientationRotation().angle <= 0.0 || getMisorientationRotation().angle > 180.0) {

	  QString degSymbol = QChar(0x00B0);
	  QString ss = "The misorientation angle should be in the range (0, 180" + degSymbol + "]";
	  notifyErrorMessage(getHumanLabel(), ss, -1);
	  setErrorCondition(-1);
  }
  if (getMisorientationRotation().h == 0.0f && getMisorientationRotation().k == 0.0f && getMisorientationRotation().l == 0.0f) {

	  QString ss = QObject::tr("All three indices of the misorientation axis cannot be 0");
	  notifyErrorMessage(getHumanLabel(), ss, -1);
	  setErrorCondition(-1);
  }
  

  // Number of Sampling Points (filter params.)
  if (getNumSamplPts() < 1) {

	  QString ss = QObject::tr("The number of sampling points must be greater than zero");
	  notifyErrorMessage(getHumanLabel(), ss, -1);
	  setErrorCondition(-1);
  }
  if (getNumSamplPts() > 5000) { // set some reasonable value, but allow user to use more if he/she knows what he/she does

	  QString ss = QObject::tr("Most likely, you do not need to use that many sampling points");
	  notifyWarningMessage(getHumanLabel(), ss, -1);
  }


  // Output files (filter params.)
  if (getDistOutputFile().isEmpty() == true)
  {
	  QString ss = QObject::tr("The output file for saving the distribution must be set");
	  notifyErrorMessage(getHumanLabel(), ss, -1000);
	  setErrorCondition(-1);
  }
  if (getErrOutputFile().isEmpty() == true)
  {
	  QString ss = QObject::tr("The output file for saving the distribution errors must be set");
	  notifyErrorMessage(getHumanLabel(), ss, -1000);
	  setErrorCondition(-1);
  }
  if (getDistOutputFile().isEmpty() == false && getDistOutputFile() == getErrOutputFile())
  {
	  QString ss = QObject::tr("The output files must be different");
	  notifyErrorMessage(getHumanLabel(), ss, -1);
	  setErrorCondition(-1);
  }

  QFileInfo distOutFileInfo(getDistOutputFile());
  QDir distParentPath = distOutFileInfo.path();
  if (distParentPath.exists() == false)
  {
	  QString ss = QObject::tr("The directory path for the distribution output file does not exist. DREAM.3D will attempt to create this path during execution of the filter");
	  notifyWarningMessage(getHumanLabel(), ss, -1);
  }

  QFileInfo errOutFileInfo(getErrOutputFile());
  QDir errParentPath = errOutFileInfo.path();
  if (errParentPath.exists() == false)
  {
	  QString ss = QObject::tr("The directory path for the distribution errors output file does not exist. DREAM.3D will attempt to create this path during execution of the filter");
	  notifyWarningMessage(getHumanLabel(), ss, -1);
  }


  // Crystal Structures (DREAM file)
  QVector<size_t> cDims(1, 1);
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if (NULL != m_CrystalStructuresPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {
	  m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */


  // Phase of Interest  (filter params.)
  if (NULL != m_CrystalStructuresPtr.lock().get()) {
	  if (getPhaseOfInterest() >= m_CrystalStructuresPtr.lock()->getNumberOfTuples() || getPhaseOfInterest() <= 0)
	  {
		  QString ss = QObject::tr("The phase index is either larger than the number of Ensembles or smaller than 1");
		  notifyErrorMessage(getHumanLabel(), ss, -1);
		  setErrorCondition(-381);
	  }
  }


  // Euler Angels (DREAM file)
  cDims[0] = 3;
  m_FeatureEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getFeatureEulerAnglesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if (NULL != m_FeatureEulerAnglesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {
	  m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */


  // Phases (DREAM file)
  cDims[0] = 1;
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if (NULL != m_FeaturePhasesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {
	  m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */


  // Face Labels (DREAM file)
  cDims[0] = 2;
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if (NULL != m_SurfaceMeshFaceLabelsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {
	  m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */


  // Face Normals (DREAM file)
  cDims[0] = 3;
  m_SurfaceMeshFaceNormalsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(this, getSurfaceMeshFaceNormalsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if (NULL != m_SurfaceMeshFaceNormalsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {
	  m_SurfaceMeshFaceNormals = m_SurfaceMeshFaceNormalsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */


  // Face Areas (DREAM file)
  cDims[0] = 1;
  m_SurfaceMeshFaceAreasPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(this, getSurfaceMeshFaceAreasArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if (NULL != m_SurfaceMeshFaceAreasPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {
	  m_SurfaceMeshFaceAreas = m_SurfaceMeshFaceAreasPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */


  // Feature Face Ids (DREAM file)
  cDims[0] = 1;
  m_SurfaceMeshFeatureFaceIdPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFeatureFaceIdArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if (NULL != m_SurfaceMeshFeatureFaceIdPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {
	  m_SurfaceMeshFeatureFaceId = m_SurfaceMeshFeatureFaceIdPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */


  // NumTriangles (DREAM file)
  cDims[0] = 1;
  m_SurfaceMeshFeatureFaceNumTrianglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFeatureFaceNumTrianglesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if (NULL != m_SurfaceMeshFeatureFaceNumTrianglesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {
	  m_SurfaceMeshFeatureFaceNumTriangles = m_SurfaceMeshFeatureFaceNumTrianglesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

 
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD_MetricBased::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD_MetricBased::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }


  // -------------------- set resolutions and 'ball volumes' based on user's selection --------------------
  float m_misorResol = RESOL_CHOICES[getChosenLimitDists()][0];
  float m_planeResol = RESOL_CHOICES[getChosenLimitDists()][1];
  double ballVolume = BALL_VOLS[getChosenLimitDists()];

  m_misorResol *= SIMPLib::Constants::k_PiOver180;
  m_planeResol *= SIMPLib::Constants::k_PiOver180;

  float m_PlaneResolSq = m_planeResol * m_planeResol;
  
  QVector<SpaceGroupOps::Pointer> m_OrientationOps;
  m_OrientationOps = SpaceGroupOps::getOrientationOpsQVector();
  double totalFaceArea = 0.0; // for the Phase of Interest

  // We want to work with the raw pointers for speed so get those pointers.
  uint32_t* m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  float* m_Eulers = m_FeatureEulerAnglesPtr.lock()->getPointer(0);
  int32_t* m_Phases = m_FeaturePhasesPtr.lock()->getPointer(0);
  int32_t* m_FaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0);
  double* m_FaceNormals = m_SurfaceMeshFaceNormalsPtr.lock()->getPointer(0);
  double* m_FaceAreas = m_SurfaceMeshFaceAreasPtr.lock()->getPointer(0);
  int32_t* m_FeatureFaceId = m_SurfaceMeshFeatureFaceIdPtr.lock()->getPointer(0);
  int32_t* m_NumTriangles = m_SurfaceMeshFeatureFaceNumTrianglesPtr.lock()->getPointer(0);



  // -------------------- check if directiories are ok and if output files can be opened --------------------

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo distOutFileInfo(getDistOutputFile());

  QDir distOutFileDir(distOutFileInfo.path());
  if (!distOutFileDir.mkpath("."))
  {
	  QString ss;
	  ss = QObject::tr("Error creating parent path '%1'").arg(distOutFileDir.path());
	  notifyErrorMessage(getHumanLabel(), ss, -1);
	  setErrorCondition(-1);
	  return;
  }

  QFile distOutFile(getDistOutputFile());
  if (!distOutFile.open(QIODevice::WriteOnly | QIODevice::Text))
  {
	  QString ss = QObject::tr("Error opening output file '%1'").arg(getDistOutputFile());
	  setErrorCondition(-100);
	  notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
	  return;
  }

  QFileInfo errOutFileInfo(getDistOutputFile());

  QDir errOutFileDir(errOutFileInfo.path());
  if (!errOutFileDir.mkpath("."))
  {
	  QString ss;
	  ss = QObject::tr("Error creating parent path '%1'").arg(errOutFileDir.path());
	  notifyErrorMessage(getHumanLabel(), ss, -1);
	  setErrorCondition(-1);
	  return;
  }

  QFile errOutFile(getDistOutputFile());
  if (!errOutFile.open(QIODevice::WriteOnly | QIODevice::Text))
  {
	  QString ss = QObject::tr("Error opening output file '%1'").arg(getDistOutputFile());
	  setErrorCondition(-100);
	  notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
	  return;
  }

  // Open the output files, should be opened and checked before starting computations TODO
  FILE* fDist = NULL;
  fDist = fopen(m_DistOutputFile.toLatin1().data(), "wb");
  if (NULL == fDist)
  {
	  QString ss = QObject::tr("Error opening distribution output file '%1'").arg(m_DistOutputFile);
	  setErrorCondition(-1);
	  notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
	  return;
  }

  FILE* fErr = NULL;
  fErr = fopen(m_ErrOutputFile.toLatin1().data(), "wb");
  if (NULL == fErr)
  {
	  QString ss = QObject::tr("Error opening distribution errors output file '%1'").arg(m_ErrOutputFile);
	  setErrorCondition(-1);
	  notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
	  return;
  }



  // ------------------------------ generation of sampling points ------------------------------

  QString ss = QObject::tr("--> Generating sampling points");
  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);

  // generate "Golden Section Spiral", see http://www.softimageblog.com/archives/115

  int numSamplPts_WholeSph = 2 * m_NumSamplPts; // here we generate points on the whole sphere
  QVector<float> samplPtsX_WholeSph(0);
  QVector<float> samplPtsY_WholeSph(0);
  QVector<float> samplPtsZ_WholeSph(0);

  QVector<float> samplPtsX(0);
  QVector<float> samplPtsY(0);
  QVector<float> samplPtsZ(0);

  float _inc = 2.3999632f; // = pi * (3 - sqrt(5))
  float _off = 2.0f / float(numSamplPts_WholeSph);
  
  for (int ptIdx_WholeSph = 0; ptIdx_WholeSph < numSamplPts_WholeSph; ptIdx_WholeSph++)
  { 
    if (getCancel() == true) { return; }
	
	float _y = (float(ptIdx_WholeSph) * _off) - 1.0f + (0.5f * _off);
	float _r = sqrtf( fmaxf(1.0f - _y*_y, 0.0f) );
	float _phi = float(ptIdx_WholeSph) * _inc;

	samplPtsX_WholeSph.push_back(cosf(_phi) * _r);
	samplPtsY_WholeSph.push_back(_y);
	samplPtsZ_WholeSph.push_back(sinf(_phi) * _r);
  }

  // now, select the points from the upper hemisphere,
  // and - if needed - from lower hemisphere from the neighborhood of equator
  
  float howFarBelowEquator = 0.0f;
  if (getAddMorePtsNearEquator() == true) howFarBelowEquator = -1.5001f * m_planeResol; 
	
  for (int ptIdx_WholeSph = 0; ptIdx_WholeSph < numSamplPts_WholeSph; ptIdx_WholeSph++)
  {
	  if (getCancel() == true) { return; }

	  if (samplPtsZ_WholeSph[ptIdx_WholeSph] > howFarBelowEquator)
	  {
		  samplPtsX.push_back(samplPtsX_WholeSph[ptIdx_WholeSph]);
		  samplPtsY.push_back(samplPtsY_WholeSph[ptIdx_WholeSph]);
		  samplPtsZ.push_back(samplPtsZ_WholeSph[ptIdx_WholeSph]);
	  }
  }
	  
	  

  int32_t feature1 = 0;
  int32_t feature2 = 0;

  float g1ea[3] = { 0.0f, 0.0f, 0.0f };
  float g2ea[3] = { 0.0f, 0.0f, 0.0f };

  float g1[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
  float g2[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };

  float g1s[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
  float g2s[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };

  float sym1[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
  float sym2[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };

  float g2sT[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };

  float dg[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
  float dgT[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };

  float gFixed[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
  float gFixedT[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }; 

  float diffFromFixed[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };

  float normal_lab[3] = { 0.0f, 0.0f, 0.0f };
  float normal_grain1[3] = { 0.0f, 0.0f, 0.0f };
  float normal_grain2[3] = { 0.0f, 0.0f, 0.0f };



  // convert axis angle to matrix representation of misorientation
  {
	  float gFixedAngle = m_MisorientationRotation.angle * SIMPLib::Constants::k_PiOver180;
	  float gFixedAxis[3] = { m_MisorientationRotation.h, m_MisorientationRotation.k, m_MisorientationRotation.l };
	  MatrixMath::Normalize3x1(gFixedAxis);
	  FOrientArrayType om(9);
	  FOrientTransformsType::ax2om(FOrientArrayType(gFixedAxis[0], gFixedAxis[1], gFixedAxis[2], gFixedAngle), om);
	  om.toGMatrix(gFixed);
  }
	  
  MatrixMath::Transpose3x3(gFixed, gFixedT);

  uint32_t cryst = m_CrystalStructures[m_PhaseOfInterest];
  int32_t nsym = m_OrientationOps[cryst]->getNumSymOps();
  int32_t numMeshTris = m_SurfaceMeshFaceAreasPtr.lock()->getNumberOfTuples();
  int32_t numDistinctGBs = m_SurfaceMeshFeatureFaceNumTrianglesPtr.lock()->getNumberOfTuples();
  int32_t numGoodTris = 0;
  
  QVector<double> accTrisAreas(0);
  QVector<float> normals_grain1_x(0);
  QVector<float> normals_grain1_y(0);
  QVector<float> normals_grain1_z(0);
  QVector<float> normals_grain2_x(0);
  QVector<float> normals_grain2_y(0);
  QVector<float> normals_grain2_z(0);

  // ---------  find triangles (and equivalent crystallographic parameters) with +- the fixed misorientation ---------
  for (int triIdx = 0; triIdx < numMeshTris; triIdx++) {

	  if (getCancel() == true) { return; }
	  
	  if (triIdx % 1000 == 0) {
		ss = QObject::tr("--> Step 1/2: selecting boundaries with the specified misorientation (%1\% completed)").arg(int(100.0 * float(triIdx) / float(numMeshTris)));
		notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
	  }

	  feature1 = m_FaceLabels[2 * triIdx];
	  feature2 = m_FaceLabels[2 * triIdx + 1];

	  if (feature1 < 1 || feature2 < 1) { continue; }
	  if (m_FeaturePhases[feature1] != m_FeaturePhases[feature2])  { continue; }
	  if (m_FeaturePhases[feature1] != m_PhaseOfInterest || m_FeaturePhases[feature2] != m_PhaseOfInterest) { continue; }

	  totalFaceArea += m_FaceAreas[triIdx];
	  numGoodTris++;
	  
	  normal_lab[0] = m_FaceNormals[3 * triIdx];
	  normal_lab[1] = m_FaceNormals[3 * triIdx + 1];
	  normal_lab[2] = m_FaceNormals[3 * triIdx + 2];

	  for (int whichEa = 0; whichEa < 3; whichEa++)
	  {
		  g1ea[whichEa] = m_Eulers[3 * feature1 + whichEa];
		  g2ea[whichEa] = m_Eulers[3 * feature2 + whichEa];
	  }

	  FOrientArrayType om(9, 0.0f);
	  FOrientTransformsType::eu2om(FOrientArrayType(g1ea, 3), om);
	  om.toGMatrix(g1); 
	  FOrientTransformsType::eu2om(FOrientArrayType(g2ea, 3), om);
	  om.toGMatrix(g2);


	  for (int j = 0; j < nsym; j++)
	  {
		  // rotate g1 by symOp
		  m_OrientationOps[cryst]->getMatSymOp(j, sym1);
		  MatrixMath::Multiply3x3with3x3(sym1, g1, g1s);
		  // get the crystal directions along the triangle normals
		  MatrixMath::Multiply3x3with3x1(g1s, normal_lab, normal_grain1);

		  for (int k = 0; k < nsym; k++)
		  {
			  // calculate the symmetric misorienation
			  m_OrientationOps[cryst]->getMatSymOp(k, sym2);
			  // rotate g2 by symOp
			  MatrixMath::Multiply3x3with3x3(sym2, g2, g2s);
			  // transpose rotated g2
			  MatrixMath::Transpose3x3(g2s, g2sT);
			  // calculate delta g
			  MatrixMath::Multiply3x3with3x3(g1s, g2sT, dg); //dg -- the misorientation between adjacent grains
			  MatrixMath::Transpose3x3(dg, dgT);

			  for (int transpose = 0; transpose <= 1; transpose++)
			  {
				  if (getCancel() == true) { return; }

				  // check if dg is close to gFix
				  if (transpose == 0) 
				  {
					  MatrixMath::Multiply3x3with3x3(dg, gFixedT, diffFromFixed);
				  } 
				  else 
				  {
					  MatrixMath::Multiply3x3with3x3(dgT, gFixedT, diffFromFixed);
				  }

				  float diffAngle = acosf((diffFromFixed[0][0] + diffFromFixed[1][1] + diffFromFixed[2][2] - 1.0f) * 0.5f);

				  if (diffAngle < m_misorResol) 
				  {
					  MatrixMath::Multiply3x3with3x1(dgT, normal_grain1, normal_grain2); // minus sign before normal_grain2 will be added later

					  accTrisAreas.push_back(m_FaceAreas[triIdx]);

					  if (transpose == 0) 
					  {
						  normals_grain1_x.push_back(normal_grain1[0]);
						  normals_grain1_y.push_back(normal_grain1[1]);
						  normals_grain1_z.push_back(normal_grain1[2]);

						  normals_grain2_x.push_back(-normal_grain2[0]);
						  normals_grain2_y.push_back(-normal_grain2[1]);
						  normals_grain2_z.push_back(-normal_grain2[2]);
					  }
					  else
					  {
						  normals_grain1_x.push_back(-normal_grain2[0]);
						  normals_grain1_y.push_back(-normal_grain2[1]);
						  normals_grain1_z.push_back(-normal_grain2[2]);

						  normals_grain2_x.push_back(normal_grain1[0]);
						  normals_grain2_y.push_back(normal_grain1[1]);
						  normals_grain2_z.push_back(normal_grain1[2]);
					  }
				  }
			  }
		  }
	  }
  }




  // ----------------- determining distribution values at the sampling points (and their errors) -------------------
  QVector<double> distribValues(samplPtsX.size(), 0.0);
  QVector<double> errorValues(samplPtsX.size(), 0.0);
  
  for (int ptIdx = 0; ptIdx < samplPtsX.size(); ptIdx++)
  {
	  if (getCancel() == true) { return; }

	  ss = QObject::tr("--> step2/2: computing distribution values at the section of interest (%1\% completed)").arg(int(100.0 * float(ptIdx) / float(samplPtsX.size())));
	  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);

	  float fixedNormal1[3] = { samplPtsX.at(ptIdx), samplPtsY.at(ptIdx), samplPtsZ.at(ptIdx) };
	  float fixedNormal2[3] = { 0.0f, 0.0f, 0.0f };
	  MatrixMath::Multiply3x3with3x1(gFixedT, fixedNormal1, fixedNormal2);

	  for (int triRepresIdx = 0; triRepresIdx < normals_grain1_x.size(); triRepresIdx++) 
	  {
		  for (int inversion = 0; inversion <= 1; inversion++)
		  {
			  float sign = 1.0f;
			  if (inversion == 1) sign = -1.0f;

			  float theta1 = acosf(sign * (
				  normals_grain1_x.at(triRepresIdx) * fixedNormal1[0] +
				  normals_grain1_y.at(triRepresIdx) * fixedNormal1[1] +
				  normals_grain1_z.at(triRepresIdx) * fixedNormal1[2]));

			  float theta2 = acosf(- sign * (
				  normals_grain2_x.at(triRepresIdx) * fixedNormal2[0] +
				  normals_grain2_y.at(triRepresIdx) * fixedNormal2[1] +
				  normals_grain2_z.at(triRepresIdx) * fixedNormal2[2]));

			  float distSq = 0.5f * (theta1 * theta1 + theta2 * theta2);

			  if (distSq < m_PlaneResolSq)
			  {
				  distribValues[ptIdx] += accTrisAreas.at(triRepresIdx);
			  }
		  }
	  }
	  errorValues[ptIdx] = sqrt(distribValues[ptIdx] / totalFaceArea / double(numDistinctGBs) ) / ballVolume;

	  distribValues[ptIdx] /= totalFaceArea;
	  distribValues[ptIdx] /= ballVolume; 
  }



  // ------------------------------------------- writing the output --------------------------------------------
  for (int ptIdx = 0; ptIdx < samplPtsX.size(); ptIdx++) {

	  float zenith = acosf(samplPtsZ.at(ptIdx));
	  float azimuth = atan2f(samplPtsY.at(ptIdx), samplPtsX.at(ptIdx));

	  float rStereoProj = tanf(0.5 * zenith);
	  float xStereoProj = rStereoProj * cos(azimuth);
	  float yStereoProj = rStereoProj * sin(azimuth);

	  float zenithDeg = SIMPLib::Constants::k_180OverPi * zenith;
	  float azimuthDeg = SIMPLib::Constants::k_180OverPi * azimuth;

	  fprintf(fDist, "%.4f %.4f %.2f %.2f %.4f\n", xStereoProj, yStereoProj, zenithDeg, azimuthDeg, distribValues[ptIdx]);

	  if (m_SaveRelativeErr == false)
	  {
		  fprintf(fErr, "%.4f %.4f %.2f %.2f %.4f\n", xStereoProj, yStereoProj, zenithDeg, azimuthDeg, errorValues[ptIdx]);
	  }
	  else
	  {
		  float saneErr = 100.0;
		  if (distribValues[ptIdx] > 1e-12) {
			  saneErr = fmin(100.0, errorValues[ptIdx] / distribValues[ptIdx]);
		  }
		  fprintf(fErr, "%.4f %.4f %.2f %.2f %.1f\n", xStereoProj, yStereoProj, zenithDeg, azimuthDeg, saneErr );
	  }
  }
  fclose(fDist);
  fclose(fErr);

  if (getErrorCondition() < 0)
  {
    QString ss = QObject::tr("Something went wrong");
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindGBCD_MetricBased::newFilterInstance(bool copyFilterParameters)
{
  FindGBCD_MetricBased::Pointer filter = FindGBCD_MetricBased::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindGBCD_MetricBased::getCompiledLibraryName()
{ return SurfaceMeshingConstants::SurfaceMeshingBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindGBCD_MetricBased::getGroupName()
{ return DREAM3D::FilterGroups::Unsupported; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindGBCD_MetricBased::getSubGroupName()
{ return "Surface Meshing"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindGBCD_MetricBased::getHumanLabel()
{ return "Find GBCD (Metric-based Approach)"; }



