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

#include "FeatureFaceCurvatureFilter.h"



//#include "DREAM3DLib/SurfaceMeshingFilters/GenerateNodeFaceConnectivity.h"
#include "DREAM3DLib/SurfaceMeshingFilters/TriangleCentroidFilter.h"
#include "DREAM3DLib/SurfaceMeshingFilters/TriangleNormalFilter.h"

#include "CalculateTriangleGroupCurvatures.h"
#include "SharedFeatureFaceFilter.h"

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/task_scheduler_init.h>
#include <tbb/task_group.h>
#include <tbb/task.h>
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FeatureFaceCurvatureFilter::FeatureFaceCurvatureFilter() :
  SurfaceMeshFilter(),
  m_SurfaceDataContainerName(DREAM3D::HDF5::SurfaceDataContainerName),
  m_SurfaceMeshUniqueEdgesArrayName(DREAM3D::EdgeData::SurfaceMeshUniqueEdges),
  //m_SurfaceMeshFaceEdgesArrayName(DREAM3D::EdgeData::SurfaceMeshFaceEdges),
  m_PrincipalCurvature1ArrayName(DREAM3D::FaceData::SurfaceMeshPrincipalCurvature1),
  m_PrincipalCurvature2ArrayName(DREAM3D::FaceData::SurfaceMeshPrincipalCurvature2),
  m_SurfaceMeshTriangleNormalsArrayName(DREAM3D::FaceData::SurfaceMeshFaceNormals),
  m_SurfaceMeshTriangleCentroidsArrayName(DREAM3D::FaceData::SurfaceMeshFaceCentroids),
  m_SurfaceMeshFeatureFaceIdArrayName(DREAM3D::FaceData::SurfaceMeshFeatureFaceId),
  m_NRing(3),
  m_ComputePrincipalDirectionVectors(true),
  m_ComputeMeanCurvature(false),
  m_ComputeGaussianCurvature(false),
  m_UseNormalsForCurveFitting(true),
  m_SurfaceMeshUniqueEdges(NULL),
  m_SurfaceMeshFaceEdges(NULL),
  m_TotalFeatureFaces(0),
  m_CompletedFeatureFaces(0)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FeatureFaceCurvatureFilter::~FeatureFaceCurvatureFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureFaceCurvatureFilter::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> options;
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Neighborhood Ring Count");
    parameter->setPropertyName("NRing");
    parameter->setWidgetType(FilterParameter::IntWidget);
    //parameter->setUnits("Zero will Lock them in Place");
    parameter->setValueType("int");
    parameter->setCastableValueType("int");
    options.push_back(parameter);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Compute Principal Direction Vectors");
    option->setPropertyName("ComputePrincipalDirectionVectors");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    options.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Compute Gaussian Curvature");
    option->setPropertyName("ComputeGaussianCurvature");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    options.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Compute Mean Curvature");
    option->setPropertyName("ComputeMeanCurvature");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    options.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Use Face Normals for Curve Fitting");
    option->setPropertyName("UseNormalsForCurveFitting");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    options.push_back(option);
  }
  setFilterParameters(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureFaceCurvatureFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setNRing( reader->readValue("NRing", getNRing()) );
  setComputePrincipalDirectionVectors( reader->readValue("ComputePrincipalDirectionVectors", false) );
  setComputeGaussianCurvature( reader->readValue("ComputeGaussianCurvature", false) );
  setComputeMeanCurvature( reader->readValue("ComputeMeanCurvature", false) );
  setUseNormalsForCurveFitting( reader->readValue("UseNormalsForCurveFitting", false) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FeatureFaceCurvatureFilter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  writer->writeValue("NRing", getNRing() );
  writer->writeValue("ComputePrincipalDirectionVectors", getComputePrincipalDirectionVectors());
  writer->writeValue("ComputeGaussianCurvature", getComputeGaussianCurvature() );
  writer->writeValue("ComputeMeanCurvature", getComputeMeanCurvature() );
  writer->writeValue("UseNormalsForCurveFitting", getUseNormalsForCurveFitting() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureFaceCurvatureFilter::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());

  // We MUST have Nodes
  if(sm->getVertices().get() == NULL)
  {
    setErrorCondition(-384);
    addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", getErrorCondition());
  }

  // We MUST have Triangles defined also.
  if(sm->getFaces().get() == NULL)
  {
    setErrorCondition(-385);
    addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", getErrorCondition());
  }

  // We do not know the size of the array so we can not use the macro so we just manually call
  // the needed methods that will propagate these array additions to the pipeline
  QVector<int> dims(1, 2);
  DataArray<int>::Pointer uniqueEdgesArray = DataArray<int>::CreateArray(1, dims, DREAM3D::EdgeData::SurfaceMeshUniqueEdges);
  sm->addEdgeData(DREAM3D::EdgeData::SurfaceMeshUniqueEdges, uniqueEdgesArray);

  DoubleArrayType::Pointer principalCurv1 = DoubleArrayType::CreateArray(1, DREAM3D::FaceData::SurfaceMeshPrincipalCurvature1);
  sm->addFaceData(DREAM3D::FaceData::SurfaceMeshPrincipalCurvature1, principalCurv1);

  DoubleArrayType::Pointer principalCurv2 = DoubleArrayType::CreateArray(1, DREAM3D::FaceData::SurfaceMeshPrincipalCurvature2);
  sm->addFaceData(DREAM3D::FaceData::SurfaceMeshPrincipalCurvature2, principalCurv2);

  if (m_ComputeGaussianCurvature == true)
  {
    DoubleArrayType::Pointer gaussianCurv = DoubleArrayType::CreateArray(1, DREAM3D::FaceData::SurfaceMeshGaussianCurvatures);
    sm->addFaceData(DREAM3D::FaceData::SurfaceMeshGaussianCurvatures, gaussianCurv);
  }

  if (m_ComputeMeanCurvature == true)
  {
    DoubleArrayType::Pointer meanCurv = DoubleArrayType::CreateArray(1, DREAM3D::FaceData::SurfaceMeshMeanCurvatures);
    sm->addFaceData(DREAM3D::FaceData::SurfaceMeshMeanCurvatures, meanCurv);
  }

  if (m_ComputePrincipalDirectionVectors == true)
  {
    QVector<int> dims(1, 3);
    DoubleArrayType::Pointer prinDir1 = DoubleArrayType::CreateArray(1, dims, DREAM3D::FaceData::SurfaceMeshPrincipalDirection1);
    sm->addFaceData(DREAM3D::FaceData::SurfaceMeshPrincipalDirection1, prinDir1);

    DoubleArrayType::Pointer prinDir2 = DoubleArrayType::CreateArray(1, dims, DREAM3D::FaceData::SurfaceMeshPrincipalDirection2);
    sm->addFaceData(DREAM3D::FaceData::SurfaceMeshPrincipalDirection2, prinDir2);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureFaceCurvatureFilter::preflight()
{
  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());
  if(NULL == sm)
  {
    setErrorCondition(-383);
    addErrorMessage(getHumanLabel(), "SurfaceDataContainer is missing", getErrorCondition());
  }

  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureFaceCurvatureFilter::execute()
{
  int err = 0;

  setErrorCondition(err);
  SurfaceDataContainer* m = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The SurfaceMesh DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  // Just to double check we have everything.
  dataCheck(false, 0, 0, 0);
  if (getErrorCondition() < 0)
  {
    return;
  }

  // Get our Reference counted Array of Face Structures
  FaceArray::Pointer trianglesPtr = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName())->getFaces();
  if(NULL == trianglesPtr.get())
  {
    setErrorCondition(-556);
    notifyErrorMessage("The SurfaceMesh DataContainer Does NOT contain Faces", -556);
    return;
  }


  // We need to make sure we have up to date values for the Face Centroids and Face Normals
  // so we are going to recalculate them all just in case they are stale
  TriangleCentroidFilter::Pointer centroidFilter = TriangleCentroidFilter::New();
  centroidFilter->setSurfaceMeshTriangleCentroidsArrayName(getSurfaceMeshTriangleCentroidsArrayName());
  centroidFilter->setDataContainerArray(getDataContainerArray());
  centroidFilter->setObservers(getObservers());
  centroidFilter->setMessagePrefix(getMessagePrefix());
  centroidFilter->execute();
  if (centroidFilter->getErrorCondition() < 0)
  {
    notifyErrorMessage("Error Generating the triangle centroids", -801);
    return;
  }

  // Calculate/update the Face Normals
  bool clearFaceNormals = false;
  if (getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName())->getFaceData(m_SurfaceMeshTriangleNormalsArrayName).get() == NULL)
  {
    clearFaceNormals = true;
  }
  TriangleNormalFilter::Pointer normalsFilter = TriangleNormalFilter::New();
  normalsFilter->setSurfaceMeshTriangleNormalsArrayName(getSurfaceMeshTriangleNormalsArrayName());
  normalsFilter->setDataContainerArray(getDataContainerArray());
  normalsFilter->setObservers(getObservers());
  normalsFilter->setMessagePrefix(getMessagePrefix());
  normalsFilter->execute();
  if (normalsFilter->getErrorCondition() < 0)
  {
    notifyErrorMessage("Error Generating the triangle normals", -802);
    return;
  }

  // Make sure the Face Connectivity is created because the FindNRing algorithm needs this and will
  // assert if the data is NOT in the SurfaceMesh Data Container
  bool clearMeshLinks = false;
  FaceArray::Pointer facesPtr = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName())->getFaces();
  if(facesPtr == NULL)
  {
    return;
  }
  Int32DynamicListArray::Pointer vertLinks = facesPtr->getFacesContainingVert();
  if (NULL == vertLinks.get())
  {
    clearMeshLinks = true; // This was not explicitly set in the pipeline so we are going to clear it when the filter is complete
    getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName())->getFaces()->findFacesContainingVert();
  }

  // Group the Faces by common neighboring feature face. This means that each group of triangles
  // each share the same set of Feature Ids. Since each triangle can only have 2 Feature Ids
  SharedFeatureFaceFilter::Pointer sharedFeatureFacesFilter = SharedFeatureFaceFilter::New();
  sharedFeatureFacesFilter->setSurfaceMeshFeatureFaceIdArrayName(getSurfaceMeshFeatureFaceIdArrayName());
  sharedFeatureFacesFilter->setDataContainerArray(getDataContainerArray());
  sharedFeatureFacesFilter->setObservers(getObservers());
  sharedFeatureFacesFilter->setMessagePrefix(getMessagePrefix());
  sharedFeatureFacesFilter->execute();
  if (sharedFeatureFacesFilter->getErrorCondition() < 0)
  {
    notifyErrorMessage("Error Generating the Shared Feature Faces", -803);
    return;
  }

  SharedFeatureFaceFilter::SharedFeatureFaces_t& sharedFeatureFaces = sharedFeatureFacesFilter->getSharedFeatureFaces();

  DoubleArrayType::Pointer principalCurvature1 = DoubleArrayType::CreateArray(trianglesPtr->getNumberOfTuples(), DREAM3D::FaceData::SurfaceMeshPrincipalCurvature1);
  principalCurvature1->initializeWithZeros();
  DoubleArrayType::Pointer principalCurvature2 = DoubleArrayType::CreateArray(trianglesPtr->getNumberOfTuples(), DREAM3D::FaceData::SurfaceMeshPrincipalCurvature2);
  principalCurvature2->initializeWithZeros();

  DoubleArrayType::Pointer principalDirection1;
  DoubleArrayType::Pointer principalDirection2;
  if (m_ComputePrincipalDirectionVectors == true)
  {
    QVector<int> dims(1, 3);
    principalDirection1 = DoubleArrayType::CreateArray(trianglesPtr->getNumberOfTuples(), dims, DREAM3D::FaceData::SurfaceMeshPrincipalDirection1);
    principalDirection1->initializeWithZeros();

    principalDirection2 = DoubleArrayType::CreateArray(trianglesPtr->getNumberOfTuples(), dims, DREAM3D::FaceData::SurfaceMeshPrincipalDirection2);
    principalDirection2->initializeWithZeros();
  }

  // Check if the user wants to calculate the Gaussian Curvature
  DoubleArrayType::Pointer gaussianCurvature;
  if (m_ComputeGaussianCurvature == true)
  {
    gaussianCurvature = DoubleArrayType::CreateArray(trianglesPtr->getNumberOfTuples(), DREAM3D::FaceData::SurfaceMeshGaussianCurvatures);
    gaussianCurvature->initializeWithZeros();
  }
  // Check if the user wants to calculate the Mean Curvature
  DoubleArrayType::Pointer meanCurvature;
  if (m_ComputeGaussianCurvature == true)
  {
    meanCurvature = DoubleArrayType::CreateArray(trianglesPtr->getNumberOfTuples(), DREAM3D::FaceData::SurfaceMeshMeanCurvatures);
    meanCurvature->initializeWithZeros();
  }

  int index = 0;
  m_TotalFeatureFaces = sharedFeatureFaces.size();
  m_CompletedFeatureFaces = 0;

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif


#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_group* g = new tbb::task_group;
//  if(true)
//  {
//    qDebug() << "Default Number of Threads to Use: " << init.default_num_threads() << "\n";
//    qDebug() << "FeatureFaceCurvatureFilter Running in Parallel." << "\n";
//  }
#else
  //if()
//  {
//    qDebug() << "CalculateFaceGroupCurvatures Running in Serial." << "\n";
//  }
#endif
  // typedef here for conveneince
  typedef SharedFeatureFaceFilter::SharedFeatureFaces_t::iterator SharedFeatureFaceIterator_t;

  for(SharedFeatureFaceIterator_t iter = sharedFeatureFaces.begin(); iter != sharedFeatureFaces.end(); ++iter)
  {
    SharedFeatureFaceFilter::FaceIds_t& triangleIds = iter.value();
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      g->run(CalculateTriangleGroupCurvatures(m_NRing, triangleIds, m_UseNormalsForCurveFitting,
                                              principalCurvature1, principalCurvature2,
                                              principalDirection1, principalDirection2,
                                              gaussianCurvature, meanCurvature,
                                              getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName()), this ) );
    }
    else
#endif
    {
      CalculateTriangleGroupCurvatures curvature(m_NRing, triangleIds, m_UseNormalsForCurveFitting,
                                                 principalCurvature1, principalCurvature2,
                                                 principalDirection1, principalDirection2,
                                                 gaussianCurvature, meanCurvature,
                                                 getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName()), this );
      curvature();
      index++;
    }
  }
  // *********************** END END END END END END  ********************************************************************

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  g->wait(); // Wait for all the threads to complete before moving on.
  delete g;
#endif

  getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName())->addFaceData(principalCurvature1->GetName(), principalCurvature1);
  getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName())->addFaceData(principalCurvature2->GetName(), principalCurvature2);
  if (m_ComputePrincipalDirectionVectors == true)
  {
    getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName())->addFaceData(principalDirection1->GetName(), principalDirection1);
    getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName())->addFaceData(principalDirection2->GetName(), principalDirection2);
  }

  if (m_ComputeGaussianCurvature == true)
  {
    getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName())->addFaceData(gaussianCurvature->GetName(), gaussianCurvature);
  }
  if (m_ComputeMeanCurvature == true)
  {
    getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName())->addFaceData(meanCurvature->GetName(), meanCurvature);
  }

  // Now clear up some temp arrays that were created for this filter
  if (clearFaceNormals == true)
  {
    getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName())->removeFaceData(getSurfaceMeshTriangleNormalsArrayName());
  }
  getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName())->removeFaceData(getSurfaceMeshTriangleCentroidsArrayName() );
  getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName())->removeFaceData(getSurfaceMeshFeatureFaceIdArrayName() );
  if (clearMeshLinks == true)
  {
    getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName())->getFaces()->deleteFacesContainingVert();
  }
  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
void FeatureFaceCurvatureFilter::tbbTaskProgress()
{
  m_CompletedFeatureFaces++;

  QString ss = QObject::tr("%1/%2 Complete").arg(m_CompletedFeatureFaces).arg(m_TotalFeatureFaces);
  notifyStatusMessage(ss);
}

#endif
