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

#include "LaplacianSmoothing.h"


#include <stdio.h>
#include <sstream>


#include "MXA/Common/MXAEndian.h"
#include "MXA/Utilities/MXAFileInfo.h"
#include "MXA/Utilities/MXADir.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/SurfaceMeshStructs.h"
#include "DREAM3DLib/SurfaceMeshingFilters/GenerateUniqueEdges.h"
#include "DREAM3DLib/SurfaceMeshingFilters/util/Vector3.h"

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#endif


/**
 * @brief The LaplacianSmoothingImpl class is the actual code that does the computation and can be called either
 * from serial code or from Parallelized code (using TBB).
 */
class LaplacianSmoothingImpl
{
    DREAM3D::SurfaceMesh::VertListPointer_t m_vertsPtr;
    DREAM3D::SurfaceMesh::VertList_t::Pointer m_newPositions;
    MeshVertLinks::Pointer m_meshVertLinks;
    DREAM3D::SurfaceMesh::FaceList_t::Pointer m_facesPtr;
    DataArray<float>::Pointer m_lambdasPtr;

  public:
    LaplacianSmoothingImpl(DREAM3D::SurfaceMesh::VertListPointer_t vertsPtr,
                           DREAM3D::SurfaceMesh::VertList_t::Pointer newPositions,
                           MeshVertLinks::Pointer meshVertLinks,
                           DREAM3D::SurfaceMesh::FaceList_t::Pointer facesPtr,
                           DataArray<float>::Pointer lambdasPtr) :
      m_vertsPtr(vertsPtr),
      m_newPositions(newPositions),
      m_meshVertLinks(meshVertLinks),
      m_facesPtr(facesPtr),
      m_lambdasPtr(lambdasPtr)
    {}

    virtual ~LaplacianSmoothingImpl(){}

    /**
     * @brief generate Generates the Normals for the triangles
     * @param start The starting DREAM3D::SurfaceMesh::Face_t Index
     * @param end The ending DREAM3D::SurfaceMesh::Face_t Index
     */
    void generate(size_t start, size_t end) const
    {
      DREAM3D::SurfaceMesh::Vert_t* vertices = m_vertsPtr->GetPointer(0); // Get the pointer to the from of the array so we can use [] notation
      DREAM3D::SurfaceMesh::Face_t* faces = m_facesPtr->GetPointer(0);
      DREAM3D::SurfaceMesh::Vert_t* newPositions = m_newPositions->GetPointer(0);

      float* lambdas = m_lambdasPtr->GetPointer(0);


      for(size_t v = start; v < end; ++v)
      {
        DREAM3D::SurfaceMesh::Vert_t& currentVert = vertices[v];
        DREAM3D::SurfaceMesh::Vert_t& newVert = newPositions[v];
        // Initialize the "newPosition" with the current position
        newVert.pos[0] = currentVert.pos[0];
        newVert.pos[1] = currentVert.pos[1];
        newVert.pos[2] = currentVert.pos[2];
        // Get the Triangles for this vertex
        MeshVertLinks::FaceList& list = m_meshVertLinks->getFaceList(v);
        std::set<int32_t> neighbours;
        // Create the unique List of Vertices that are directly connected to this vertex (vert)
        for(int32_t t = 0; t < list.ncells; ++t )
        {
          neighbours.insert(faces[list.cells[t]].verts[0]);
          neighbours.insert(faces[list.cells[t]].verts[1]);
          neighbours.insert(faces[list.cells[t]].verts[2]);
        }
        neighbours.erase(v); // Remove the current vertex id from the list as we don't need it

        DREAM3D::SurfaceMesh::Float_t konst1 = lambdas[v]/neighbours.size();

        // Now that we have our connectivity iterate over the vertices generating a new position
        for(std::set<int32_t>::iterator iter = neighbours.begin(); iter != neighbours.end(); ++iter)
        {
          DREAM3D::SurfaceMesh::Vert_t& vert = vertices[*iter];

          newVert.pos[0] += konst1 * (vert.pos[0] - currentVert.pos[0]);
          newVert.pos[1] += konst1 * (vert.pos[1] - currentVert.pos[1]);
          newVert.pos[2] += konst1 * (vert.pos[2] - currentVert.pos[2]);
        }
      }
    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    /**
     * @brief operator () This is called from the TBB stye of code
     * @param r The range to compute the values
     */
    void operator()(const tbb::blocked_range<size_t> &r) const
    {
      generate(r.begin(), r.end());
    }
#endif
};




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LaplacianSmoothing::LaplacianSmoothing() :
  SurfaceMeshFilter(),
  m_SurfaceMeshUniqueEdgesArrayName(DREAM3D::EdgeData::SurfaceMeshUniqueEdges),
  m_IterationSteps(1),
  m_Lambda(0.1),
  m_SurfacePointLambda(0.0),
  m_TripleLineLambda(0.0),
  m_QuadPointLambda(0.0),
  m_SurfaceTripleLineLambda(0.0),
  m_SurfaceQuadPointLambda(0.0),
  m_DoConnectivityFilter(false)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LaplacianSmoothing::~LaplacianSmoothing()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LaplacianSmoothing::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  /* Place all your option initialization code here */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Iteration Steps");
    option->setPropertyName("IterationSteps");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Default Lambda");
    parameter->setPropertyName("Lambda");
    parameter->setWidgetType(FilterParameter::DoubleWidget);
    //parameter->setUnits("Bulk Nodes");
    parameter->setValueType("float");
    parameter->setCastableValueType("double");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Triple Line Lambda");
    parameter->setPropertyName("TripleLineLambda");
    parameter->setWidgetType(FilterParameter::DoubleWidget);
    //parameter->setUnits("Zero will Lock them in Place");
    parameter->setValueType("float");
    parameter->setCastableValueType("double");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Quad Points Lambda");
    parameter->setPropertyName("QuadPointLambda");
    parameter->setWidgetType(FilterParameter::DoubleWidget);
    //parameter->setUnits("Zero will Lock them in Place");
    parameter->setValueType("float");
    parameter->setCastableValueType("double");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Outer Points Lambda");
    parameter->setPropertyName("SurfacePointLambda");
    parameter->setWidgetType(FilterParameter::DoubleWidget);
    //parameter->setUnits("Zero will Lock them in Place");
    parameter->setValueType("float");
    parameter->setCastableValueType("double");
    parameters.push_back(parameter);
  }

  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Outer Triple Line Lambda");
    parameter->setPropertyName("SurfaceTripleLineLambda");
    parameter->setWidgetType(FilterParameter::DoubleWidget);
    //parameter->setUnits("Zero will Lock them in Place");
    parameter->setValueType("float");
    parameter->setCastableValueType("double");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Outer Quad Points Lambda");
    parameter->setPropertyName("SurfaceQuadPointLambda");
    parameter->setWidgetType(FilterParameter::DoubleWidget);
    //parameter->setUnits("Zero will Lock them in Place");
    parameter->setValueType("float");
    parameter->setCastableValueType("double");
    parameters.push_back(parameter);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LaplacianSmoothing::readFilterParameters(AbstractFilterParametersReader* reader)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LaplacianSmoothing::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */

  writer->writeValue("IterationSteps", getIterationSteps());
  writer->writeValue("Lambda", getLambda() );
  writer->writeValue("TripleLineLambda", getTripleLineLambda());
  writer->writeValue("QuadPointLambda", getQuadPointLambda());
  writer->writeValue("SurfacePointLambda", getSurfacePointLambda());
  writer->writeValue("SurfaceTripleLineLambda", getSurfaceTripleLineLambda());
  writer->writeValue("SurfaceQuadPointLambda", getSurfaceQuadPointLambda());
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LaplacianSmoothing::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if(NULL == sm)
  {
    setErrorCondition(-383);
    addErrorMessage(getHumanLabel(), "SurfaceMeshDataContainer is missing", getErrorCondition());
  }
  else
  {
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

    if (sm->getEdgeData(m_SurfaceMeshUniqueEdgesArrayName).get() == NULL)
    {
      m_DoConnectivityFilter = true;
    } else
    {
      m_DoConnectivityFilter = false;
    }
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LaplacianSmoothing::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LaplacianSmoothing::execute()
{
  int err = 0;
  std::stringstream ss;
  setErrorCondition(err);

  dataCheck(false, 0, 0, 0);
  if(getErrorCondition() < 0)
  {
    return;
  }

  setErrorCondition(0);



  /* Place all your code to execute your filter here. */
  err = edgeBasedSmoothing();

  if (err < 0)
  {
    notifyErrorMessage("Error smoothing the surface mesh", getErrorCondition());
    return;
  }


  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LaplacianSmoothing::generateLambdaArray(DataArray<int8_t>* nodeTypePtr)
{
  notifyStatusMessage("Generating Lambda values");
  DREAM3D::SurfaceMesh::VertListPointer_t nodesPtr = getSurfaceMeshDataContainer()->getVertices();
  if(NULL == nodesPtr.get())
  {
    setErrorCondition(-555);
    notifyErrorMessage("The SurfaceMesh DataContainer Does NOT contain Nodes", -555);
    return -1;
  }
  int numNodes = nodesPtr->GetNumberOfTuples();
  int8_t* nodeType = nodeTypePtr->GetPointer(0);

  DataArray<float>::Pointer lambdas = DataArray<float>::CreateArray(numNodes, "Laplacian_Smoothing_Lambda_Array");
  lambdas->initializeWithZeros();

  for(int i = 0; i < numNodes; ++i)
  {
    switch(nodeType[i])
    {
      case DREAM3D::SurfaceMesh::NodeType::Unused:
        break;
      case DREAM3D::SurfaceMesh::NodeType::Default:
        lambdas->SetValue(i, m_Lambda);
        break;
      case DREAM3D::SurfaceMesh::NodeType::TriplePoint:
        lambdas->SetValue(i, m_TripleLineLambda);
        break;
      case DREAM3D::SurfaceMesh::NodeType::QuadPoint:
        lambdas->SetValue(i, m_QuadPointLambda);
        break;
      case DREAM3D::SurfaceMesh::NodeType::SurfaceDefault:
        lambdas->SetValue(i, m_SurfacePointLambda);
        break;
      case DREAM3D::SurfaceMesh::NodeType::SurfaceTriplePoint:
        lambdas->SetValue(i, m_SurfaceTripleLineLambda);
        break;
      case DREAM3D::SurfaceMesh::NodeType::SurfaceQuadPoint:
        lambdas->SetValue(i, m_SurfaceQuadPointLambda);
        break;
      default:
        break;
    }
  }

  setLambdaArray(lambdas);
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LaplacianSmoothing::edgeBasedSmoothing()
{
  int err = 0;

  //
  DREAM3D::SurfaceMesh::VertListPointer_t nodesPtr = getSurfaceMeshDataContainer()->getVertices();
  int nvert = nodesPtr->GetNumberOfTuples();
  DREAM3D::SurfaceMesh::Vert_t* vsm = nodesPtr->GetPointer(0); // Get the pointer to the from of the array so we can use [] notation


  DataArray<int8_t>::Pointer nodeTypeSharedPtr = DataArray<int8_t>::NullPointer();
  DataArray<int8_t>* nodeTypePtr = nodeTypeSharedPtr.get();
  IDataArray::Pointer iNodeTypePtr = getSurfaceMeshDataContainer()->getVertexData(DREAM3D::VertexData::SurfaceMeshNodeType);

  if (NULL == iNodeTypePtr.get() )
  {
    // The node type array does not exist so create one with the default node type populated
    nodeTypeSharedPtr = DataArray<int8_t>::CreateArray(nodesPtr->GetNumberOfTuples(), DREAM3D::VertexData::SurfaceMeshNodeType);
    nodeTypeSharedPtr->initializeWithValues(DREAM3D::SurfaceMesh::NodeType::Default);
    nodeTypePtr = nodeTypeSharedPtr.get();
  }
  else
  {
    // The node type array does exist so use that one.
    nodeTypePtr = DataArray<int8_t>::SafeObjectDownCast<IDataArray*, DataArray<int8_t>* >(iNodeTypePtr.get());
  }

  // Generate the Lambda Array
  err = generateLambdaArray(nodeTypePtr);
  if (err < 0)
  {
    setErrorCondition(-557);
    notifyErrorMessage("Error generating the Lambda Array", getErrorCondition());
    return err;
  }
  // Get a Pointer to the Lambda array for conveneince
  DataArray<float>::Pointer lambdas = getLambdaArray();
  float* lambda = lambdas->GetPointer(0);
  std::stringstream ss;

  //  Generate the Unique Edges
  if (m_DoConnectivityFilter == true)
  {
    // There was no Edge connectivity before this filter so delete it when we are done with it
    GenerateUniqueEdges::Pointer conn = GenerateUniqueEdges::New();
    ss.str("");
    ss << getMessagePrefix() << "|->Generating Unique Edge Ids |->";
    conn->setMessagePrefix(ss.str());
    conn->setObservers(getObservers());
    conn->setVoxelDataContainer(getVoxelDataContainer());
    conn->setSurfaceMeshDataContainer(getSurfaceMeshDataContainer());
    conn->setSolidMeshDataContainer(getSolidMeshDataContainer());
    conn->setSurfaceMeshUniqueEdgesArrayName(getSurfaceMeshUniqueEdgesArrayName());
    conn->execute();
    if(conn->getErrorCondition() < 0)
    {
      return conn->getErrorCondition();
    }
  }
  if (getCancel() == true) { return -1; }

  notifyStatusMessage("Starting to Smooth Vertices");
  // Get the unique Edges from the data container

  IDataArray::Pointer uniqueEdgesPtr = getSurfaceMeshDataContainer()->getEdgeData(m_SurfaceMeshUniqueEdgesArrayName);
  DataArray<int>* uniqueEdges = DataArray<int>::SafePointerDownCast(uniqueEdgesPtr.get());
  if (NULL == uniqueEdges)
  {
    setErrorCondition(-560);
    notifyErrorMessage("Error retrieving the Unique Edge List", getErrorCondition());
    return -560;
  }
  // Get a pointer to the Unique Edges
  int* uedges = uniqueEdges->GetPointer(0);
  int nedges = uniqueEdges->GetNumberOfTuples();


  DataArray<int>::Pointer numConnections = DataArray<int>::CreateArray(nvert, "Laplacian_Smoothing_NumberConnections_Array");
  numConnections->initializeWithZeros();
  int* ncon = numConnections->GetPointer(0);

  DataArray<double>::Pointer deltaArray = DataArray<double>::CreateArray(nvert, 3, "Laplacian_Smoothing_Delta_Array");
  deltaArray->initializeWithZeros();
  double* delta = deltaArray->GetPointer(0);


  double dlta = 0.0;
  for (int q = 0; q < m_IterationSteps; q++)
  {
    if (getCancel() == true) { return -1; }
    ss.str("");
    ss << "Iteration " << q;
    notifyStatusMessage(ss.str());
    for (int i = 0; i < nedges; i++)
    {
      int in_edge = 2*i;
      int in1 = uedges[in_edge]; // row of the first vertex
      int in2 = uedges[in_edge+1]; //row the second vertex

      for (int j = 0; j < 3; j++)
      {
        BOOST_ASSERT( 3*in1+j < nvert*3);
        BOOST_ASSERT( 3*in2+j < nvert*3);
        dlta = vsm[in2].pos[j] - vsm[in1].pos[j];
        delta[3*in1+j] += dlta;
        delta[3*in2+j] += -1.0*dlta;
      }
      ncon[in1] += 1;
      ncon[in2] += 1;
    }


    float ll = 0.0f;
    for (int i=0; i < nvert; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        int in0 = 3*i+j;
        dlta = delta[in0] / ncon[i];

        ll = lambda[i];
        DREAM3D::SurfaceMesh::Vert_t& node = vsm[i];
        node.pos[j] += ll*dlta;
        delta[in0] = 0.0; //reset for next iteration
      }
      ncon[i] = 0;//reset for next iteration
    }

#if OUTPUT_DEBUG_VTK_FILES
    std::stringstream testFile;
    testFile << "/tmp/Laplacian_" << q << ".vtk";
    writeVTKFile(testFile.str());
#endif
  }


  // This filter had to generate the edge connectivity data so delete it when we are done with it.
  if (m_DoConnectivityFilter == true)
  {
    IDataArray::Pointer removedConnectviity = getSurfaceMeshDataContainer()->removeEdgeData(m_SurfaceMeshUniqueEdgesArrayName);
    BOOST_ASSERT(removedConnectviity.get() != NULL);
  }

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LaplacianSmoothing::vertexBasedSmoothing()
{
  int err = 0;


  // Convert the 32 bit float Nodes into 64 bit floating point nodes.
  DREAM3D::SurfaceMesh::VertListPointer_t vertsPtr = getSurfaceMeshDataContainer()->getVertices();
  int numVerts = vertsPtr->GetNumberOfTuples();
  //  DREAM3D::SurfaceMesh::Vert_t* vertices = vertsPtr->GetPointer(0); // Get the pointer to the from of the array so we can use [] notation

  //Make sure the Triangle Connectivity is created because the FindNRing algorithm needs this and will
  // assert if the data is NOT in the SurfaceMesh Data Container
  MeshVertLinks::Pointer meshVertLinks = getSurfaceMeshDataContainer()->getMeshVertLinks();
  if (NULL == meshVertLinks.get())
  {
    getSurfaceMeshDataContainer()->buildMeshVertLinks();
  }


  DREAM3D::SurfaceMesh::FaceList_t::Pointer facesPtr = getSurfaceMeshDataContainer()->getFaces();
  //  DREAM3D::SurfaceMesh::Face_t* faces = facesPtr->GetPointer(0);

  DataArray<int8_t>::Pointer nodeTypeSharedPtr = DataArray<int8_t>::NullPointer();
  DataArray<int8_t>* nodeTypePtr = nodeTypeSharedPtr.get();
  IDataArray::Pointer iNodeTypePtr = getSurfaceMeshDataContainer()->getVertexData(DREAM3D::VertexData::SurfaceMeshNodeType);

  if (NULL == iNodeTypePtr.get() )
  {
    // The node type array does not exist so create one with the default node type populated
    nodeTypeSharedPtr = DataArray<int8_t>::CreateArray(vertsPtr->GetNumberOfTuples(), DREAM3D::VertexData::SurfaceMeshNodeType);
    nodeTypeSharedPtr->initializeWithValues(DREAM3D::SurfaceMesh::NodeType::Default);
    nodeTypePtr = nodeTypeSharedPtr.get();
  }
  else
  {
    // The node type array does exist so use that one.
    nodeTypePtr = DataArray<int8_t>::SafeObjectDownCast<IDataArray*, DataArray<int8_t>* >(iNodeTypePtr.get());
  }

  // Generate the Lambdas possible using a subclasses implementation of the method
  err = generateLambdaArray(nodeTypePtr);
  if (err < 0)
  {
    setErrorCondition(-557);
    notifyErrorMessage("Error generating the Lambda Array", getErrorCondition());
    return err;
  }


  notifyStatusMessage("Starting to Smooth Vertices");

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  bool doParallel = true;
#endif

  // Get a Pointer to the Lambdas
  DataArray<float>::Pointer lambdasPtr = getLambdaArray();

  // We need an array to store the new positions
  DREAM3D::SurfaceMesh::VertList_t::Pointer newPositionsPtr = DREAM3D::SurfaceMesh::VertList_t::CreateArray(vertsPtr->GetNumberOfTuples(), "New Vertex Positions");
  newPositionsPtr->initializeWithZeros();


  std::stringstream ss;
  for (int q=0; q<m_IterationSteps; q++)
  {
    if (getCancel() == true) { return -1; }
    ss.str("");
    ss << "Iteration " << q;
    notifyStatusMessage(ss.str());
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, numVerts),
                        LaplacianSmoothingImpl(vertsPtr, newPositionsPtr,meshVertLinks,facesPtr,lambdasPtr), tbb::auto_partitioner());

    }
    else
#endif
    {
      LaplacianSmoothingImpl serial(vertsPtr, newPositionsPtr,meshVertLinks,facesPtr,lambdasPtr);
      serial.generate(0, numVerts);
    }

    // SERIAL ONLY
    ::memcpy(vertsPtr->GetPointer(0), newPositionsPtr->GetPointer(0), sizeof(DREAM3D::SurfaceMesh::Vert_t) * vertsPtr->GetNumberOfTuples());
    // -----------
#if OUTPUT_DEBUG_VTK_FILES
    std::stringstream testFile;
    testFile << "/tmp/Laplacian_" << q << ".vtk";
    writeVTKFile(testFile.str());
#endif
  }
  return 1;
}








#if OUTPUT_DEBUG_VTK_FILES
namespace Detail {
  /**
 * @brief The ScopedFileMonitor class will automatically close an open FILE pointer
 * when the object goes out of scope.
 */
  class ScopedFileMonitor
  {
    public:
      ScopedFileMonitor(FILE* f) : m_File(f) {}
      virtual ~ScopedFileMonitor() { fclose(m_File);}
    private:
      FILE* m_File;
      ScopedFileMonitor(const ScopedFileMonitor&); // Copy Constructor Not Implemented
      void operator=(const ScopedFileMonitor&); // Operator '=' Not Implemented
  };

}



// -----------------------------------------------------------------------------
// This is just here for some debugging issues.
// -----------------------------------------------------------------------------
void LaplacianSmoothing::writeVTKFile(const std::string &outputVtkFile)
{

  SurfaceMeshDataContainer* m = getSurfaceMeshDataContainer();
  /* Place all your code to execute your filter here. */
  DREAM3D::SurfaceMesh::VertListPointer_t nodesPtr = m->getVertices();
  DREAM3D::SurfaceMesh::VertList_t& nodes = *(nodesPtr);
  int nNodes = nodes.GetNumberOfTuples();
  bool m_WriteBinaryFile = true;
  bool m_WriteConformalMesh = true;
  std::stringstream ss;


  FILE* vtkFile = NULL;
  vtkFile = fopen(outputVtkFile.c_str(), "wb");
  if (NULL == vtkFile)
  {
    ss.str("");
    ss << "Error creating file '" << outputVtkFile << "'";
    return;
  }
  Detail::ScopedFileMonitor vtkFileMonitor(vtkFile);

  fprintf(vtkFile, "# vtk DataFile Version 2.0\n");
  fprintf(vtkFile, "Data set from DREAM.3D Surface Meshing Module\n");
  if (m_WriteBinaryFile) {
    fprintf(vtkFile, "BINARY\n");
  }
  else {
    fprintf(vtkFile, "ASCII\n");
  }
  fprintf(vtkFile, "DATASET POLYDATA\n");


  fprintf(vtkFile, "POINTS %d float\n", nNodes);
  float pos[3] = {0.0f, 0.0f, 0.0f};

  size_t totalWritten = 0;
  // Write the POINTS data (Vertex)
  for (int i = 0; i < nNodes; i++)
  {
    DREAM3D::SurfaceMesh::Vert_t& n = nodes[i]; // Get the current Node
    //  if (m_SurfaceMeshNodeType[i] > 0)
    {
      pos[0] = static_cast<float>(n.pos[0]);
      pos[1] = static_cast<float>(n.pos[1]);
      pos[2] = static_cast<float>(n.pos[2]);
      if (m_WriteBinaryFile == true)
      {
        MXA::Endian::FromSystemToBig::convert<float>(pos[0]);
        MXA::Endian::FromSystemToBig::convert<float>(pos[1]);
        MXA::Endian::FromSystemToBig::convert<float>(pos[2]);
        totalWritten = fwrite(pos, sizeof(float), 3, vtkFile);
        if (totalWritten != sizeof(float) * 3)
        {

        }
      }
      else {
        fprintf(vtkFile, "%4.4f %4.4f %4.4f\n", pos[0], pos[1], pos[2]); // Write the positions to the output file
      }
    }
  }

  // Write the triangle indices into the vtk File
  StructArray<DREAM3D::SurfaceMesh::Face_t>& triangles = *(m->getFaces());
  int triangleCount = 0;
  int end = triangles.GetNumberOfTuples();
  int grainInterest = 25;
  for(int i = 0; i < end; ++i)
  {
    DREAM3D::SurfaceMesh::Face_t* tri = triangles.GetPointer(i);
    if (tri->labels[0] == grainInterest || tri->labels[1] == grainInterest)
    {
      ++triangleCount;
    }
  }


  int tData[4];
  // Write the CELLS Data
  //  int start = 3094380;
  //  int end = 3094450;
  //  int triangleCount = end - start;
  std::cout << "---------------------------------------------------------------------------" << std::endl;
  std::cout << outputVtkFile << std::endl;
  fprintf(vtkFile, "\nPOLYGONS %d %d\n", triangleCount, (triangleCount * 4));
  for (int tid = 0; tid < end; ++tid)
  {
    DREAM3D::SurfaceMesh::Face_t* tri = triangles.GetPointer(tid);
    if (tri->labels[0] == grainInterest || tri->labels[1] == grainInterest)
    {
      tData[1] = triangles[tid].verts[0];
      tData[2] = triangles[tid].verts[1];
      tData[3] = triangles[tid].verts[2];
      //      std::cout << tid << "\n  " << nodes[tData[1]].coord[0] << " " << nodes[tData[1]].coord[1]  << " " << nodes[tData[1]].coord[2]  << std::endl;
      //      std::cout << "  " << nodes[tData[2]].coord[0] << " " << nodes[tData[2]].coord[1]  << " " << nodes[tData[2]].coord[2]  << std::endl;
      //      std::cout << "  " << nodes[tData[3]].coord[0] << " " << nodes[tData[3]].coord[1]  << " " << nodes[tData[3]].coord[2]  << std::endl;
      if (m_WriteBinaryFile == true)
      {
        tData[0] = 3; // Push on the total number of entries for this entry
        MXA::Endian::FromSystemToBig::convert<int>(tData[0]);
        MXA::Endian::FromSystemToBig::convert<int>(tData[1]); // Index of Vertex 0
        MXA::Endian::FromSystemToBig::convert<int>(tData[2]); // Index of Vertex 1
        MXA::Endian::FromSystemToBig::convert<int>(tData[3]); // Index of Vertex 2
        fwrite(tData, sizeof(int), 4, vtkFile);
        //      if (false == m_WriteConformalMesh)
        //      {
        //        tData[0] = tData[1];
        //        tData[1] = tData[3];
        //        tData[3] = tData[0];
        //        tData[0] = 3;
        //        MXA::Endian::FromSystemToBig::convert<int>(tData[0]);
        //        fwrite(tData, sizeof(int), 4, vtkFile);
        //      }
      }
      else
      {
        fprintf(vtkFile, "3 %d %d %d\n", tData[1], tData[2], tData[3]);
        //      if (false == m_WriteConformalMesh)
        //      {
        //        fprintf(vtkFile, "3 %d %d %d\n", tData[3], tData[2], tData[1]);
        //      }
      }
    }
  }

  fprintf(vtkFile, "\n");
}

#endif
