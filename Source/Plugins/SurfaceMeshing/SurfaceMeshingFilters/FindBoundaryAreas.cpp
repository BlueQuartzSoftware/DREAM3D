/* ============================================================================
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "FindBoundaryAreas.h"

#include "DREAM3DLib/Common/Constants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindBoundaryAreas::FindBoundaryAreas() :
  AbstractFilter(),
  m_FaceFeatureAttributeMatrixPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::FaceFeatureAttributeMatrixName, ""),
  m_SurfaceMeshTriangleAreasArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFaceAreas),
  m_SurfaceMeshFeatureFaceIdsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_SurfaceMeshBoundaryAreasArrayName("BoundaryAreas"),
  m_SurfaceMeshTriangleAreas(NULL),
  m_SurfaceMeshFeatureFaceIds(NULL),
  m_SurfaceMeshBoundaryAreas(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindBoundaryAreas::~FindBoundaryAreas()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryAreas::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Triangle Areas", "SurfaceMeshTriangleAreasArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSurfaceMeshTriangleAreasArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Face Feature Ids", "SurfaceMeshFeatureFaceIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSurfaceMeshFeatureFaceIdsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Face Feature Attribute Matrix Name", "FaceFeatureAttributeMatrixPath", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getFaceFeatureAttributeMatrixPath(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Boundary Areas Array Name", "SurfaceMeshBoundaryAreasArrayName", FilterParameterWidgetType::StringWidget, getSurfaceMeshBoundaryAreasArrayName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryAreas::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFaceFeatureAttributeMatrixPath(reader->readDataArrayPath("FaceFeatureAttributeMatrixPath", getFaceFeatureAttributeMatrixPath()));
  setSurfaceMeshBoundaryAreasArrayName(reader->readString("SurfaceMeshBoundaryAreasArrayName", getSurfaceMeshBoundaryAreasArrayName() ) );
  setSurfaceMeshFeatureFaceIdsArrayPath(reader->readDataArrayPath("SurfaceMeshFeatureFaceIdsArrayPath", getSurfaceMeshFeatureFaceIdsArrayPath() ) );
  setSurfaceMeshTriangleAreasArrayPath(reader->readDataArrayPath("SurfaceMeshTriangleAreasArrayPath", getSurfaceMeshTriangleAreasArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindBoundaryAreas::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FaceFeatureAttributeMatrixPath)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshTriangleAreasArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshFeatureFaceIdsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshBoundaryAreasArrayName)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryAreas::dataCheck()
{
  setErrorCondition(0);

  QVector<size_t> dims(1, 1);
  m_SurfaceMeshFeatureFaceIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFeatureFaceIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFeatureFaceIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFeatureFaceIds = m_SurfaceMeshFeatureFaceIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_SurfaceMeshTriangleAreasPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(this, getSurfaceMeshTriangleAreasArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshTriangleAreasPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshTriangleAreas = m_SurfaceMeshTriangleAreasPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */


  if(m_FaceFeatureAttributeMatrixPath.isEmpty() == true)
  {
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), "An Attribute Matrix from the Surface Mesh DataContainer must be selected.", getErrorCondition());
    return;
  }
  if(m_SurfaceMeshBoundaryAreasArrayName.isEmpty() == true)
  {
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), "A name for the new array must be entered.", getErrorCondition());
    return;
  }

  DataArrayPath tempPath;
  tempPath.update(getFaceFeatureAttributeMatrixPath().getDataContainerName(), getFaceFeatureAttributeMatrixPath().getAttributeMatrixName(), getSurfaceMeshBoundaryAreasArrayName() );
  m_SurfaceMeshBoundaryAreasPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(this, tempPath, 0.0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshBoundaryAreasPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshBoundaryAreas = m_SurfaceMeshBoundaryAreasPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryAreas::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);

  /* *** THIS FILTER NEEDS TO BE CHECKED *** */
  setErrorCondition(0xABABABAB);
  QString ss = QObject::tr("Filter is NOT updated for IGeometry Redesign. A Programmer needs to check this filter. Please report this to the DREAM3D developers.");
  notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  /* *** THIS FILTER NEEDS TO BE CHECKED *** */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryAreas::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  //get number of face features
  int64_t numFaces = m_SurfaceMeshTriangleAreasPtr.lock()->getNumberOfTuples();
  size_t numFeatures = m_SurfaceMeshBoundaryAreasPtr.lock()->getNumberOfTuples();

  //initialize all areas as 0
  for(size_t i = 0; i < numFeatures; i++)
  {
    m_SurfaceMeshBoundaryAreas[i] = 0.0;
  }

  //loop over faces accumulating area
  for(int64_t i = 0; i < numFaces; i++)
  {
    m_SurfaceMeshBoundaryAreas[ m_SurfaceMeshFeatureFaceIds[i] ] += m_SurfaceMeshTriangleAreas[i];
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindBoundaryAreas::newFilterInstance(bool copyFilterParameters)
{
  FindBoundaryAreas::Pointer filter = FindBoundaryAreas::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindBoundaryAreas::getCompiledLibraryName()
{ return SurfaceMeshing::SurfaceMeshingBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindBoundaryAreas::getGroupName()
{ return DREAM3D::FilterGroups::SurfaceMeshingFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindBoundaryAreas::getSubGroupName()
// { return DREAM3D::FilterSubGroups::EnsembleStatsFilters; }
{ return "temp"; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindBoundaryAreas::getHumanLabel()
{ return "Find Face Feature Boundary Areas"; }

