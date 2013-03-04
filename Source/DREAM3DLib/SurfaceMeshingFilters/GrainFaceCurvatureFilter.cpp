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

#include "GrainFaceCurvatureFilter.h"



//#include "DREAM3DLib/SurfaceMeshingFilters/GenerateNodeTriangleConnectivity.h"
#include "DREAM3DLib/SurfaceMeshingFilters/TriangleCentroidFilter.h"
#include "DREAM3DLib/SurfaceMeshingFilters/TriangleNormalFilter.h"

#include "CalculateTriangleGroupCurvatures.h"
#include "SharedGrainFaceFilter.h"

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/task_scheduler_init.h>
#include <tbb/task_group.h>
#include <tbb/task.h>
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainFaceCurvatureFilter::GrainFaceCurvatureFilter() :
  SurfaceMeshFilter(),
  m_SurfaceMeshUniqueEdgesArrayName(DREAM3D::EdgeData::SurfaceMeshUniqueEdges),
  //m_SurfaceMeshTriangleEdgesArrayName(DREAM3D::EdgeData::SurfaceMeshTriangleEdges),
  m_PrincipalCurvature1ArrayName(DREAM3D::FaceData::SurfaceMeshPrincipalCurvature1),
  m_PrincipalCurvature2ArrayName(DREAM3D::FaceData::SurfaceMeshPrincipalCurvature2),
  m_SurfaceMeshTriangleNormalsArrayName(DREAM3D::FaceData::SurfaceMeshTriangleNormals),
  m_SurfaceMeshTriangleCentroidsArrayName(DREAM3D::FaceData::SurfaceMeshTriangleCentroids),
  m_SurfaceMeshGrainFaceIdArrayName(DREAM3D::FaceData::SurfaceMeshGrainFaceId),
  m_NRing(3),
  m_ComputePrincipalDirectionVectors(true),
  m_ComputeMeanCurvature(false),
  m_ComputeGaussianCurvature(false),
  m_UseNormalsForCurveFitting(true),
  m_SurfaceMeshUniqueEdges(NULL),
  m_SurfaceMeshTriangleEdges(NULL),
  m_TotalGrainFaces(0),
  m_CompletedGrainFaces(0)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainFaceCurvatureFilter::~GrainFaceCurvatureFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainFaceCurvatureFilter::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> options;
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
    option->setHumanLabel("Use Triangle Normals for Curve Fitting");
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
void GrainFaceCurvatureFilter::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  writer->writeValue("NRing", getNRing() );
  writer->writeValue("ComputePrincipalDirectionVectors", getComputePrincipalDirectionVectors());
  writer->writeValue("ComputeGaussianCurvature", getComputeGaussianCurvature() );
  writer->writeValue("ComputeMeanCurvature", getComputeMeanCurvature() );
  writer->writeValue("UseNormalsForCurveFitting", getUseNormalsForCurveFitting() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainFaceCurvatureFilter::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if(NULL == sm)
  {
    addErrorMessage(getHumanLabel(), "SurfaceMeshDataContainer is missing", -383);
    setErrorCondition(-384);
  }
  else
  {
    // We MUST have Nodes
    if(sm->getVertices().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", -384);
      setErrorCondition(-384);
    }

    // We MUST have Triangles defined also.
    if(sm->getFaces().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", -383);
      setErrorCondition(-384);
    }


    // We do not know the size of the array so we can not use the macro so we just manually call
    // the needed methods that will propagate these array additions to the pipeline
    DataArray<int>::Pointer uniqueEdgesArray = DataArray<int>::CreateArray(1, 2, DREAM3D::EdgeData::SurfaceMeshUniqueEdges);
    sm->addEdgeData(DREAM3D::EdgeData::SurfaceMeshUniqueEdges, uniqueEdgesArray);

    // This is just for tracking what Arrays are being created by this filter. Normally the macro
    // would do this for us.
    addCreatedEdgeData(DREAM3D::EdgeData::SurfaceMeshUniqueEdges);


    DoubleArrayType::Pointer principalCurv1 = DoubleArrayType::CreateArray(1, 1, DREAM3D::FaceData::SurfaceMeshPrincipalCurvature1);
    sm->addFaceData(DREAM3D::FaceData::SurfaceMeshPrincipalCurvature1, principalCurv1);
    addCreatedFaceData(DREAM3D::FaceData::SurfaceMeshPrincipalCurvature1);

    DoubleArrayType::Pointer principalCurv2 = DoubleArrayType::CreateArray(1, 1, DREAM3D::FaceData::SurfaceMeshPrincipalCurvature2);
    sm->addFaceData(DREAM3D::FaceData::SurfaceMeshPrincipalCurvature2, principalCurv2);
    addCreatedFaceData(DREAM3D::FaceData::SurfaceMeshPrincipalCurvature2);

    if (m_ComputeGaussianCurvature == true)
    {
      DoubleArrayType::Pointer gaussianCurv = DoubleArrayType::CreateArray(1, 1, DREAM3D::FaceData::SurfaceMeshGaussianCurvatures);
      sm->addFaceData(DREAM3D::FaceData::SurfaceMeshGaussianCurvatures, gaussianCurv);
      addCreatedFaceData(DREAM3D::FaceData::SurfaceMeshGaussianCurvatures);
    }

    if (m_ComputeMeanCurvature == true)
    {
      DoubleArrayType::Pointer meanCurv = DoubleArrayType::CreateArray(1, 1, DREAM3D::FaceData::SurfaceMeshMeanCurvatures);
      sm->addFaceData(DREAM3D::FaceData::SurfaceMeshMeanCurvatures, meanCurv);
      addCreatedFaceData(DREAM3D::FaceData::SurfaceMeshMeanCurvatures);
    }

    if (m_ComputePrincipalDirectionVectors == true)
    {
      DoubleArrayType::Pointer prinDir1 = DoubleArrayType::CreateArray(1, 3, DREAM3D::FaceData::SurfaceMeshPrincipalDirection1);
      sm->addFaceData(DREAM3D::FaceData::SurfaceMeshPrincipalDirection1, prinDir1);
      addCreatedFaceData(DREAM3D::FaceData::SurfaceMeshPrincipalDirection1);

      DoubleArrayType::Pointer prinDir2 = DoubleArrayType::CreateArray(1, 3, DREAM3D::FaceData::SurfaceMeshPrincipalDirection2);
      sm->addFaceData(DREAM3D::FaceData::SurfaceMeshPrincipalDirection2, prinDir2);
      addCreatedFaceData(DREAM3D::FaceData::SurfaceMeshPrincipalDirection2);
    }

  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainFaceCurvatureFilter::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainFaceCurvatureFilter::execute()
{
  int err = 0;
  std::stringstream ss;
  setErrorCondition(err);
  SurfaceMeshDataContainer* m = getSurfaceMeshDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The SurfaceMesh DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  // Just to double check we have everything.
  dataCheck(false, 0,0,0);
  if (getErrorCondition() < 0)
  {
    return;
  }

  // Get our Reference counted Array of Triangle Structures
  StructArray<SurfaceMesh::DataStructures::Face_t>::Pointer trianglesPtr = getSurfaceMeshDataContainer()->getFaces();
  if(NULL == trianglesPtr.get())
  {
    setErrorCondition(-556);
    notifyErrorMessage("The SurfaceMesh DataContainer Does NOT contain Triangles", -556);
    return;
  }


  // We need to make sure we have up to date values for the Triangle Centroids and Triangle Normals
  // so we are going to recalculate them all just in case they are stale
  TriangleCentroidFilter::Pointer centroidFilter = TriangleCentroidFilter::New();
  centroidFilter->setSurfaceMeshTriangleCentroidsArrayName(getSurfaceMeshTriangleCentroidsArrayName());
  centroidFilter->setSurfaceMeshDataContainer(getSurfaceMeshDataContainer());
  centroidFilter->setObservers(getObservers());
  centroidFilter->setMessagePrefix(getMessagePrefix());
  centroidFilter->execute();
  if (centroidFilter->getErrorCondition() < 0)
  {
    notifyErrorMessage("Error Generating the triangle centroids", -801);
    return;
  }

  // Calculate/update the Triangle Normals
  bool clearTriangleNormals = false;
  if (getSurfaceMeshDataContainer()->getFaceData(DREAM3D::FaceData::SurfaceMeshTriangleNormals).get() == NULL)
  {
    clearTriangleNormals = true;
  }
  TriangleNormalFilter::Pointer normalsFilter = TriangleNormalFilter::New();
  normalsFilter->setSurfaceMeshTriangleNormalsArrayName(getSurfaceMeshTriangleNormalsArrayName());
  normalsFilter->setSurfaceMeshDataContainer(getSurfaceMeshDataContainer());
  normalsFilter->setObservers(getObservers());
  normalsFilter->setMessagePrefix(getMessagePrefix());
  normalsFilter->execute();
  if (normalsFilter->getErrorCondition() < 0)
  {
    notifyErrorMessage("Error Generating the triangle normals", -802);
    return;
  }

  // Make sure the Triangle Connectivity is created because the FindNRing algorithm needs this and will
  // assert if the data is NOT in the SurfaceMesh Data Container
  bool clearMeshVertLinks = false;
  MeshVertLinks::Pointer vertLinks = getSurfaceMeshDataContainer()->getMeshVertLinks();
  if (NULL == vertLinks.get())
  {
    clearMeshVertLinks = true; // This was not explicitly set in the pipeline so we are going to clear it when the filter is complete
    getSurfaceMeshDataContainer()->buildMeshVertLinks();
  }

  // Group the Triangles by common neighboring grain face. This means that each group of triangles
  // each share the same set of Grain Ids. Since each triangle can only have 2 Grain Ids
  SharedGrainFaceFilter::Pointer sharedGrainFacesFilter = SharedGrainFaceFilter::New();
  sharedGrainFacesFilter->setSurfaceMeshGrainFaceIdArrayName(getSurfaceMeshGrainFaceIdArrayName());
  sharedGrainFacesFilter->setSurfaceMeshDataContainer(getSurfaceMeshDataContainer());
  sharedGrainFacesFilter->setObservers(getObservers());
  sharedGrainFacesFilter->setMessagePrefix(getMessagePrefix());
  sharedGrainFacesFilter->execute();
  if (sharedGrainFacesFilter->getErrorCondition() < 0)
  {
    notifyErrorMessage("Error Generating the Shared Grain Faces", -803);
    return;
  }

  SharedGrainFaceFilter::SharedGrainFaces_t& sharedGrainFaces = sharedGrainFacesFilter->getSharedGrainFaces();

  DoubleArrayType::Pointer principalCurvature1 = DoubleArrayType::CreateArray(trianglesPtr->GetNumberOfTuples(), DREAM3D::FaceData::SurfaceMeshPrincipalCurvature1);
  principalCurvature1->initializeWithZeros();
  DoubleArrayType::Pointer principalCurvature2 = DoubleArrayType::CreateArray(trianglesPtr->GetNumberOfTuples(), DREAM3D::FaceData::SurfaceMeshPrincipalCurvature2);
  principalCurvature2->initializeWithZeros();

  DoubleArrayType::Pointer principalDirection1;
  DoubleArrayType::Pointer principalDirection2;
  if (m_ComputePrincipalDirectionVectors == true)
  {
    principalDirection1 = DoubleArrayType::CreateArray(trianglesPtr->GetNumberOfTuples(), 3, DREAM3D::FaceData::SurfaceMeshPrincipalDirection1);
    principalDirection1->initializeWithZeros();

    principalDirection2 = DoubleArrayType::CreateArray(trianglesPtr->GetNumberOfTuples(), 3, DREAM3D::FaceData::SurfaceMeshPrincipalDirection2);
    principalDirection2->initializeWithZeros();
  }

  // Check if the user wants to calculate the Gaussian Curvature
  DoubleArrayType::Pointer gaussianCurvature;
  if (m_ComputeGaussianCurvature == true)
  {
    gaussianCurvature = DoubleArrayType::CreateArray(trianglesPtr->GetNumberOfTuples(), DREAM3D::FaceData::SurfaceMeshGaussianCurvatures);
    gaussianCurvature->initializeWithZeros();
  }
  // Check if the user wants to calculate the Mean Curvature
  DoubleArrayType::Pointer meanCurvature;
  if (m_ComputeGaussianCurvature == true)
  {
    meanCurvature = DoubleArrayType::CreateArray(trianglesPtr->GetNumberOfTuples(), DREAM3D::FaceData::SurfaceMeshMeanCurvatures);
    meanCurvature->initializeWithZeros();
  }

  int index = 0;
  m_TotalGrainFaces = sharedGrainFaces.size();
  m_CompletedGrainFaces = 0;

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
#else
  //   int m_NumThreads = 1;
#endif


#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_group* g = new tbb::task_group;
//  if(true)
//  {
//    std::cout << "Default Number of Threads to Use: " << init.default_num_threads() << std::endl;
//    std::cout << "GrainFaceCurvatureFilter Running in Parallel." << std::endl;
//  }
#else
  //if()
//  {
//    std::cout << "CalculateTriangleGroupCurvatures Running in Serial." << std::endl;
//  }
#endif
  // typedef here for conveneince
  typedef SharedGrainFaceFilter::SharedGrainFaces_t::iterator SharedGrainFaceIterator_t;

  for(SharedGrainFaceIterator_t iter = sharedGrainFaces.begin(); iter != sharedGrainFaces.end(); ++iter)
  {
    SharedGrainFaceFilter::TriangleIds_t& triangleIds = (*iter).second;
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    g->run(CalculateTriangleGroupCurvatures(m_NRing, triangleIds, m_UseNormalsForCurveFitting,
                                            principalCurvature1, principalCurvature2,
                                            principalDirection1, principalDirection2,
                                            gaussianCurvature, meanCurvature,
                                            getSurfaceMeshDataContainer(), this ) );
#else



    CalculateTriangleGroupCurvatures curvature(m_NRing, triangleIds,
                                               principalCurvature1, principalCurvature2,
                                               principalDirection1, principalDirection2,
                                               gaussianCurvature, meanCurvature,
                                               getSurfaceMeshDataContainer(), this );
    curvature();
#endif
    index++;
  }
  // *********************** END END END END END END  ********************************************************************

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  g->wait(); // Wait for all the threads to complete before moving on.
  delete g;
#endif

  getSurfaceMeshDataContainer()->addFaceData(principalCurvature1->GetName(), principalCurvature1);
  getSurfaceMeshDataContainer()->addFaceData(principalCurvature2->GetName(), principalCurvature2);
  if (m_ComputePrincipalDirectionVectors == true)
  {
    getSurfaceMeshDataContainer()->addFaceData(principalDirection1->GetName(), principalDirection1);
    getSurfaceMeshDataContainer()->addFaceData(principalDirection2->GetName(), principalDirection2);
  }

  if (m_ComputeGaussianCurvature == true)
  {
    getSurfaceMeshDataContainer()->addFaceData(gaussianCurvature->GetName(), gaussianCurvature);
  }
  if (m_ComputeMeanCurvature == true)
  {
    getSurfaceMeshDataContainer()->addFaceData(meanCurvature->GetName(), meanCurvature);
  }

  // Now clear up some temp arrays that were created for this filter
  if (clearTriangleNormals == true)
  {
    getSurfaceMeshDataContainer()->removeFaceData(getSurfaceMeshTriangleNormalsArrayName());
  }
  getSurfaceMeshDataContainer()->removeFaceData(getSurfaceMeshTriangleCentroidsArrayName() );
  getSurfaceMeshDataContainer()->removeFaceData(getSurfaceMeshGrainFaceIdArrayName() );
  if (clearMeshVertLinks == true)
  {
    getSurfaceMeshDataContainer()->removeMeshVertLinks();
  }
  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
void GrainFaceCurvatureFilter::tbbTaskProgress()
{
  m_CompletedGrainFaces++;
  std::stringstream ss;
  ss << m_CompletedGrainFaces << "/" << m_TotalGrainFaces << " Complete" << std::endl;
  notifyStatusMessage(ss.str());
}

#endif
