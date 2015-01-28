/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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


#include "SharedFeatureFaceFilter.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedFeatureFaceFilter::SharedFeatureFaceFilter() :
  SurfaceMeshFilter(),
  m_FaceFeatureAttributeMatrixName(DREAM3D::Defaults::FaceFeatureAttributeMatrixName),
  m_SurfaceMeshFeatureFaceIdsArrayName(DREAM3D::FaceData::SurfaceMeshFeatureFaceId),
  m_SurfaceMeshFeatureFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFeatureFaceNumTrianglesArrayName("NumTriangles"),
  m_SurfaceMeshFaceLabelsArrayPath(DREAM3D::Defaults::SurfaceDataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceLabels(NULL),
  m_SurfaceMeshFeatureFaceIds(NULL),
  m_SurfaceMeshFeatureFaceLabels(NULL),
  m_SurfaceMeshFeatureFaceNumTriangles(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedFeatureFaceFilter::~SharedFeatureFaceFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SharedFeatureFaceFilter::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("SurfaceMeshFaceLabels", "SurfaceMeshFaceLabelsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSurfaceMeshFaceLabelsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Face Feature Attribute Matrix Name", "FaceFeatureAttributeMatrixName", FilterParameterWidgetType::StringWidget, getFaceFeatureAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("SurfaceMeshFeatureFaceIds", "SurfaceMeshFeatureFaceIdsArrayName", FilterParameterWidgetType::StringWidget, getSurfaceMeshFeatureFaceIdsArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("SurfaceMeshFeatureFaceLabels", "SurfaceMeshFeatureFaceLabelsArrayName", FilterParameterWidgetType::StringWidget, getSurfaceMeshFeatureFaceLabelsArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("SurfaceMeshFeatureFaceNumTriangles", "SurfaceMeshFeatureFaceNumTrianglesArrayName", FilterParameterWidgetType::StringWidget, getSurfaceMeshFeatureFaceNumTrianglesArrayName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SharedFeatureFaceFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFaceFeatureAttributeMatrixName(reader->readString("FaceFeatureAttributeMatrixName", getFaceFeatureAttributeMatrixName() ) );
  setSurfaceMeshFeatureFaceIdsArrayName(reader->readString("SurfaceMeshFeatureFaceIdsArrayName", getSurfaceMeshFeatureFaceIdsArrayName() ) );
  setSurfaceMeshFeatureFaceLabelsArrayName(reader->readString("SurfaceMeshFeatureFaceLabelsArrayName", getSurfaceMeshFeatureFaceLabelsArrayName() ) );
  setSurfaceMeshFeatureFaceNumTrianglesArrayName(reader->readString("SurfaceMeshFeatureFaceNumTrianglesArrayName", getSurfaceMeshFeatureFaceNumTrianglesArrayName() ) );
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SharedFeatureFaceFilter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FaceFeatureAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshFeatureFaceIdsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshFeatureFaceLabelsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshFeatureFaceNumTrianglesArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshFaceLabelsArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SharedFeatureFaceFilter::dataCheck()
{
  DataArrayPath tempPath;
  SurfaceDataContainer* sm = getDataContainerArray()->getPrereqDataContainer<SurfaceDataContainer, AbstractFilter>(this, m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }
  QVector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer facefeatureAttrMat = sm->createNonPrereqAttributeMatrix<AbstractFilter>(this, getFaceFeatureAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::FaceEnsemble);
  if(getErrorCondition() < 0) { return; }

  if(NULL == sm)
  {
    setErrorCondition(-10000);
    notifyErrorMessage(getHumanLabel(), "Surface Data Container is NULL", getErrorCondition());
    return;
  }
  // We MUST have Nodes
  if(sm->getVertices().get() == NULL)
  {
    setErrorCondition(-384);
    notifyErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", getErrorCondition());
  }

  // We MUST have Triangles defined also.
  if(sm->getFaces().get() == NULL)
  {
    setErrorCondition(-385);
    notifyErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", getErrorCondition());
  }

  QVector<size_t> dims(1, 2);
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // List any arrays that are created during this filter
  dims[0] = 1;
  tempPath.update(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), m_SurfaceMeshFaceLabelsArrayPath.getAttributeMatrixName(), getSurfaceMeshFeatureFaceIdsArrayName() );
  m_SurfaceMeshFeatureFaceIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFeatureFaceIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFeatureFaceIds = m_SurfaceMeshFeatureFaceIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  
  tempPath.update(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), getFaceFeatureAttributeMatrixName(), getSurfaceMeshFeatureFaceNumTrianglesArrayName() );
  m_SurfaceMeshFeatureFaceNumTrianglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFeatureFaceNumTrianglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFeatureFaceNumTriangles = m_SurfaceMeshFeatureFaceNumTrianglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  
  dims[0] = 2;
  tempPath.update(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), getFaceFeatureAttributeMatrixName(), getSurfaceMeshFeatureFaceLabelsArrayName() );
  m_SurfaceMeshFeatureFaceLabelsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFeatureFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFeatureFaceLabels = m_SurfaceMeshFeatureFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SharedFeatureFaceFilter::preflight()
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
void SharedFeatureFaceFilter::execute()
{
  int err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());
  AttributeMatrix::Pointer faceFeatureAttrMat = sm->getAttributeMatrix(getFaceFeatureAttributeMatrixName());
  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), "Starting");

  /* Place all your code to execute your filter here. */

  FaceArray::Pointer trianglesPtr = sm->getFaces();
//  FaceArray::Face_t* triangles = trianglesPtr->getPointer(0);
  size_t totalPoints = trianglesPtr->getNumberOfTuples();

  QMap<uint64_t, int> faceSizeMap;
  QMap<uint64_t, int32_t> faceIdMap; // This maps a unique 64 bit integer to an increasing 32 bit integer
  uint32_t index = 1;
  struct { int g; int r; } faceId;
  uint64_t* faceId_64 = reinterpret_cast<uint64_t*>(&faceId);
  int32_t fl0 = -1;
  int32_t fl1 = -1;

  std::vector< std::pair<int, int> > faceLabelMap;
  faceLabelMap.push_back( std::pair<int, int>(0, 0) );
  // Loop through all the Triangles and figure out how many triangles we have in each one.
  for(size_t t = 0; t < totalPoints; ++t)
  {
    fl0 = m_SurfaceMeshFaceLabels[t * 2];
    fl1 = m_SurfaceMeshFaceLabels[t * 2 + 1];
    if (fl0 < fl1)
    {
      faceId.g = fl0;
      faceId.r = fl1;
    }
    else
    {
      faceId.g = fl1;
      faceId.r = fl0;
    }

    QMap<uint64_t, int>::iterator iter = faceSizeMap.find(*faceId_64);
    if(iter == faceSizeMap.end())
    {
      faceSizeMap[*faceId_64] = 1;
      faceIdMap[*faceId_64] = index;
      m_SurfaceMeshFeatureFaceIds[t] = index;
      faceLabelMap.push_back( std::pair<int, int>(faceId.g, faceId.r) );
      ++index;
    }
    else
    {
      // faceSizeMap[*faceId_64]++;
      iter.value()++;
      m_SurfaceMeshFeatureFaceIds[t] = faceIdMap[*faceId_64];
    }
  }

  //resize + update pointers
  QVector<size_t> tDims(1, index);
  faceFeatureAttrMat->resizeAttributeArrays(tDims);
  m_SurfaceMeshFeatureFaceLabels = m_SurfaceMeshFeatureFaceLabelsPtr.lock()->getPointer(0);
  m_SurfaceMeshFeatureFaceNumTriangles = m_SurfaceMeshFeatureFaceNumTrianglesPtr.lock()->getPointer(0);

  for(size_t i = 0; i < index; i++)
  {
    //get feature face labels
    m_SurfaceMeshFeatureFaceLabels[2 * i + 0] = faceLabelMap[i].first;
    m_SurfaceMeshFeatureFaceLabels[2 * i + 1] = faceLabelMap[i].second;
    
    //get feature triangle count
    faceId.g = faceLabelMap[i].first;
    faceId.r = faceLabelMap[i].second;
    m_SurfaceMeshFeatureFaceNumTriangles[i] = faceSizeMap[*faceId_64];
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer SharedFeatureFaceFilter::newFilterInstance(bool copyFilterParameters)
{
  SharedFeatureFaceFilter::Pointer filter = SharedFeatureFaceFilter::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SharedFeatureFaceFilter::getCompiledLibraryName()
{ return SurfaceMeshing::SurfaceMeshingBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SharedFeatureFaceFilter::getGroupName()
{ return DREAM3D::FilterGroups::SurfaceMeshingFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SharedFeatureFaceFilter::getSubGroupName()
{ return DREAM3D::FilterSubGroups::ConnectivityArrangementFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SharedFeatureFaceFilter::getHumanLabel()
{ return "Generate Triangle Face Ids"; }

