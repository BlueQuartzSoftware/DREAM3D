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


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/DREAM3DEndian.h"
#include "DREAM3DLib/Common/MeshStructs.h"
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
    VertexArray::Pointer m_vertsPtr;
    VertexArray::Pointer m_newPositions;
    MeshLinks::Pointer m_MeshLinks;
    FaceArray::Pointer m_facesPtr;
    DataArray<float>::Pointer m_lambdasPtr;

  public:
    LaplacianSmoothingImpl(VertexArray::Pointer vertsPtr,
                           VertexArray::Pointer newPositions,
                           MeshLinks::Pointer MeshLinks,
                           FaceArray::Pointer facesPtr,
                           DataArray<float>::Pointer lambdasPtr) :
      m_vertsPtr(vertsPtr),
      m_newPositions(newPositions),
      m_MeshLinks(MeshLinks),
      m_facesPtr(facesPtr),
      m_lambdasPtr(lambdasPtr)
    {}

    virtual ~LaplacianSmoothingImpl(){}

    /**
     * @brief generate Generates the Normals for the triangles
     * @param start The starting FaceArray::Face_t Index
     * @param end The ending FaceArray::Face_t Index
     */
    void generate(size_t start, size_t end) const
    {
      VertexArray::Vert_t* vertices = m_vertsPtr->getPointer(0); // Get the pointer to the from of the array so we can use [] notation
      FaceArray::Face_t* faces = m_facesPtr->getPointer(0);
      VertexArray::Vert_t* newPositions = m_newPositions->getPointer(0);

      float* lambdas = m_lambdasPtr->getPointer(0);


      for(size_t v = start; v < end; ++v)
      {
        VertexArray::Vert_t& currentVert = vertices[v];
        VertexArray::Vert_t& newVert = newPositions[v];
        // Initialize the "newPosition" with the current position
        newVert.pos[0] = currentVert.pos[0];
        newVert.pos[1] = currentVert.pos[1];
        newVert.pos[2] = currentVert.pos[2];
        // Get the Triangles for this vertex
        MeshLinks::FaceList& list = m_MeshLinks->getFaceList(v);
        QSet<int32_t> neighbours;
        // Create the unique List of Vertices that are directly connected to this vertex (vert)
        for(int32_t t = 0; t < list.ncells; ++t )
        {
          neighbours.insert(faces[list.cells[t]].verts[0]);
          neighbours.insert(faces[list.cells[t]].verts[1]);
          neighbours.insert(faces[list.cells[t]].verts[2]);
        }
        neighbours.erase(v); // Remove the current vertex id from the list as we don't need it

        float konst1 = lambdas[v]/neighbours.size();

        // Now that we have our connectivity iterate over the vertices generating a new position
        for(QSet<int32_t>::iterator iter = neighbours.begin(); iter != neighbours.end(); ++iter)
        {
          VertexArray::Vert_t& vert = vertices[*iter];

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
  m_GenerateIterationOutputFiles(false),
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
  QVector<FilterParameter::Pointer> parameters;
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
void LaplacianSmoothing::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setIterationSteps( reader->readValue("IterationSteps", getIterationSteps()) );
  setLambda( reader->readValue("Lambda", getLambda()) );
  setTripleLineLambda( reader->readValue("TripleLineLambda", getTripleLineLambda()) );
  setQuadPointLambda( reader->readValue("QuadPointLambda", getQuadPointLambda()) );
  setSurfacePointLambda( reader->readValue("SurfacePointLambda", getSurfacePointLambda()) );
  setSurfaceTripleLineLambda( reader->readValue("SurfaceTripleLineLambda", getSurfaceTripleLineLambda()) );
  setSurfaceQuadPointLambda( reader->readValue("SurfaceQuadPointLambda", getSurfaceQuadPointLambda()) );
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
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
  
  SurfaceDataContainer* sm = getSurfaceDataContainer();
  if(NULL == sm)
  {
    setErrorCondition(-383);
    addErrorMessage(getHumanLabel(), "SurfaceDataContainer is missing", getErrorCondition());
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
  VertexArray::Pointer nodesPtr = getSurfaceDataContainer()->getVertices();
  if(NULL == nodesPtr.get())
  {
    setErrorCondition(-555);
    notifyErrorMessage("The SurfaceMesh DataContainer Does NOT contain Nodes", -555);
    return -1;
  }
  int numNodes = nodesPtr->getNumberOfTuples();
  int8_t* nodeType = nodeTypePtr->getPointer(0);

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
  VertexArray::Pointer nodesPtr = getSurfaceDataContainer()->getVertices();
  int nvert = nodesPtr->getNumberOfTuples();
  VertexArray::Vert_t* vsm = nodesPtr->getPointer(0); // Get the pointer to the from of the array so we can use [] notation


  DataArray<int8_t>::Pointer nodeTypeSharedPtr = DataArray<int8_t>::NullPointer();
  DataArray<int8_t>* nodeTypePtr = nodeTypeSharedPtr.get();
  IDataArray::Pointer iNodeTypePtr = getSurfaceDataContainer()->getVertexData(DREAM3D::VertexData::SurfaceMeshNodeType);

  if (NULL == iNodeTypePtr.get() )
  {
    // The node type array does not exist so create one with the default node type populated
    nodeTypeSharedPtr = DataArray<int8_t>::CreateArray(nodesPtr->getNumberOfTuples(), DREAM3D::VertexData::SurfaceMeshNodeType);
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
  float* lambda = lambdas->getPointer(0);
  

  //  Generate the Unique Edges
  if (m_DoConnectivityFilter == true)
  {
    // There was no Edge connectivity before this filter so delete it when we are done with it
    GenerateUniqueEdges::Pointer conn = GenerateUniqueEdges::New();
    ss.str("");
    ss << getMessagePrefix() << "|->Generating Unique Edge Ids |->";
    conn->setMessagePrefix(ss.str());
    conn->setObservers(getObservers());
    conn->setVolumeDataContainer(getVolumeDataContainer());
    conn->setSurfaceDataContainer(getSurfaceDataContainer());
    conn->setVertexDataContainer(getVertexDataContainer());
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

  IDataArray::Pointer uniqueEdgesPtr = getSurfaceDataContainer()->getEdgeData(m_SurfaceMeshUniqueEdgesArrayName);
  DataArray<int>* uniqueEdges = DataArray<int>::SafePointerDownCast(uniqueEdgesPtr.get());
  if (NULL == uniqueEdges)
  {
    setErrorCondition(-560);
    notifyErrorMessage("Error retrieving the Unique Edge List", getErrorCondition());
    return -560;
  }
  // Get a pointer to the Unique Edges
  int* uedges = uniqueEdges->getPointer(0);
  int nedges = uniqueEdges->getNumberOfTuples();


  DataArray<int>::Pointer numConnections = DataArray<int>::CreateArray(nvert, "Laplacian_Smoothing_NumberConnections_Array");
  numConnections->initializeWithZeros();
  int* ncon = numConnections->getPointer(0);

  DataArray<double>::Pointer deltaArray = DataArray<double>::CreateArray(nvert, 3, "Laplacian_Smoothing_Delta_Array");
  deltaArray->initializeWithZeros();
  double* delta = deltaArray->getPointer(0);


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
        VertexArray::Vert_t& node = vsm[i];
        node.pos[j] += ll*dlta;
        delta[in0] = 0.0; //reset for next iteration
      }
      ncon[i] = 0;//reset for next iteration
    }

    if(m_GenerateIterationOutputFiles)
    {
      QTextStream testFile;
      testFile << "LaplacianSmoothing_" << q << ".vtk";
      writeVTKFile(testFile.str());
    }
  }


  // This filter had to generate the edge connectivity data so delete it when we are done with it.
  if (m_DoConnectivityFilter == true)
  {
    IDataArray::Pointer removedConnectviity = getSurfaceDataContainer()->removeEdgeData(m_SurfaceMeshUniqueEdgesArrayName);
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
  VertexArray::Pointer vertsPtr = getSurfaceDataContainer()->getVertices();
  int numVerts = vertsPtr->getNumberOfTuples();
  //  VertexArray::Vert_t* vertices = vertsPtr->getPointer(0); // Get the pointer to the from of the array so we can use [] notation

  //Make sure the Triangle Connectivity is created because the FindNRing algorithm needs this and will
  // assert if the data is NOT in the SurfaceMesh Data Container
  MeshLinks::Pointer MeshLinks = getSurfaceDataContainer()->getMeshLinks();
  if (NULL == MeshLinks.get())
  {
    getSurfaceDataContainer()->buildMeshLinks();
  }


  FaceArray::Pointer facesPtr = getSurfaceDataContainer()->getFaces();
  //  FaceArray::Face_t* faces = facesPtr->getPointer(0);

  DataArray<int8_t>::Pointer nodeTypeSharedPtr = DataArray<int8_t>::NullPointer();
  DataArray<int8_t>* nodeTypePtr = nodeTypeSharedPtr.get();
  IDataArray::Pointer iNodeTypePtr = getSurfaceDataContainer()->getVertexData(DREAM3D::VertexData::SurfaceMeshNodeType);

  if (NULL == iNodeTypePtr.get() )
  {
    // The node type array does not exist so create one with the default node type populated
    nodeTypeSharedPtr = DataArray<int8_t>::CreateArray(vertsPtr->getNumberOfTuples(), DREAM3D::VertexData::SurfaceMeshNodeType);
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
  VertexArray::Pointer newPositionsPtr = VertexArray::CreateArray(vertsPtr->getNumberOfTuples(), "New Vertex Positions");
  newPositionsPtr->initializeWithZeros();


  
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
                        LaplacianSmoothingImpl(vertsPtr, newPositionsPtr,MeshLinks,facesPtr,lambdasPtr), tbb::auto_partitioner());

    }
    else
#endif
    {
      LaplacianSmoothingImpl serial(vertsPtr, newPositionsPtr,MeshLinks,facesPtr,lambdasPtr);
      serial.generate(0, numVerts);
    }

    // SERIAL ONLY
    ::memcpy(vertsPtr->getPointer(0), newPositionsPtr->getPointer(0), sizeof(VertexArray::Vert_t) * vertsPtr->getNumberOfTuples());
    // -----------
#if OUTPUT_DEBUG_VTK_FILES
    QTextStream testFile;
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
void LaplacianSmoothing::writeVTKFile(const QString &outputVtkFile)
{

  SurfaceDataContainer* m = getSurfaceDataContainer();
  /* Place all your code to execute your filter here. */
  VertexArray::Pointer nodesPtr = m->getVertices();
  DREAM3D::Mesh::VertList_t& nodes = *(nodesPtr);
  int nNodes = nodes.getNumberOfTuples();
  bool m_WriteBinaryFile = true;
  

  IDataArray::Pointer flPtr = getSurfaceDataContainer()->getFaceData(DREAM3D::FaceData::SurfaceMeshFaceLabels);
  DataArray<int32_t>* faceLabelsPtr = DataArray<int32_t>::SafePointerDownCast(flPtr.get());
  int32_t* faceLabels = faceLabelsPtr->getPointer(0);

  FILE* vtkFile = NULL;
  vtkFile = fopen(outputVtkFile.toLatin1().data(), "wb");
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
    VertexArray::Vert_t& n = nodes[i]; // Get the current Node
    //  if (m_SurfaceMeshNodeType[i] > 0)
    {
      pos[0] = static_cast<float>(n.pos[0]);
      pos[1] = static_cast<float>(n.pos[1]);
      pos[2] = static_cast<float>(n.pos[2]);
      if (m_WriteBinaryFile == true)
      {
        DREAM3D::Endian::FromSystemToBig::convert<float>(pos[0]);
        DREAM3D::Endian::FromSystemToBig::convert<float>(pos[1]);
        DREAM3D::Endian::FromSystemToBig::convert<float>(pos[2]);
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
  StructArray<FaceArray::Face_t>& triangles = *(m->getFaces());
  int triangleCount = 0;
  int end = triangles.getNumberOfTuples();
  int grainInterest = 9;
  for(int i = 0; i < end; ++i)
  {
    //FaceArray::Face_t* tri = triangles.getPointer(i);
    if (faceLabels[i*2] == grainInterest || faceLabels[i*2+1] == grainInterest)
    {
      ++triangleCount;
    }
  }


  int tData[4];
  // Write the CELLS Data
  //  int start = 3094380;
  //  int end = 3094450;
  //  int triangleCount = end - start;
  qDebug() << "---------------------------------------------------------------------------" << "\n";
  qDebug() << outputVtkFile << "\n";
  fprintf(vtkFile, "\nPOLYGONS %d %d\n", triangleCount, (triangleCount * 4));
  for (int tid = 0; tid < end; ++tid)
  {
    //FaceArray::Face_t* tri = triangles.getPointer(tid);
    if (faceLabels[tid*2] == grainInterest || faceLabels[tid*2+1] == grainInterest)
    {
      tData[1] = triangles[tid].verts[0];
      tData[2] = triangles[tid].verts[1];
      tData[3] = triangles[tid].verts[2];
      //      qDebug() << tid << "\n  " << nodes[tData[1]].coord[0] << " " << nodes[tData[1]].coord[1]  << " " << nodes[tData[1]].coord[2]  << "\n";
      //      qDebug() << "  " << nodes[tData[2]].coord[0] << " " << nodes[tData[2]].coord[1]  << " " << nodes[tData[2]].coord[2]  << "\n";
      //      qDebug() << "  " << nodes[tData[3]].coord[0] << " " << nodes[tData[3]].coord[1]  << " " << nodes[tData[3]].coord[2]  << "\n";
      if (m_WriteBinaryFile == true)
      {
        tData[0] = 3; // Push on the total number of entries for this entry
        DREAM3D::Endian::FromSystemToBig::convert<int>(tData[0]);
        DREAM3D::Endian::FromSystemToBig::convert<int>(tData[1]); // Index of Vertex 0
        DREAM3D::Endian::FromSystemToBig::convert<int>(tData[2]); // Index of Vertex 1
        DREAM3D::Endian::FromSystemToBig::convert<int>(tData[3]); // Index of Vertex 2
        fwrite(tData, sizeof(int), 4, vtkFile);
      }
      else
      {
        fprintf(vtkFile, "3 %d %d %d\n", tData[1], tData[2], tData[3]);
      }
    }
  }

  fprintf(vtkFile, "\n");
}

#endif
