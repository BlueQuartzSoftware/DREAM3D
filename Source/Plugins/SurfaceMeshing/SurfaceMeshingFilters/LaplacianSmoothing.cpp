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


#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#endif


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Utilities/DREAM3DEndian.h"
#include "DREAM3DLib/DataContainers/MeshStructs.h"
#include "SurfaceMeshing/SurfaceMeshingFilters/GenerateUniqueEdges.h"
#include "SurfaceMeshing/SurfaceMeshingFilters/util/Vector3.h"

/**
 * @brief The LaplacianSmoothingImpl class is the actual code that does the computation and can be called either
 * from serial code or from Parallelized code (using TBB).
 */
class LaplacianSmoothingImpl
{
    VertexArray::Pointer m_vertsPtr;
    VertexArray::Pointer m_newPositions;
    Int32DynamicListArray::Pointer m_MeshLinks;
    FaceArray::Pointer m_facesPtr;
    DataArray<float>::Pointer m_lambdasPtr;

  public:
    LaplacianSmoothingImpl(VertexArray::Pointer vertsPtr,
                           VertexArray::Pointer newPositions,
                           Int32DynamicListArray::Pointer MeshLinks,
                           FaceArray::Pointer facesPtr,
                           DataArray<float>::Pointer lambdasPtr) :
      m_vertsPtr(vertsPtr),
      m_newPositions(newPositions),
      m_MeshLinks(MeshLinks),
      m_facesPtr(facesPtr),
      m_lambdasPtr(lambdasPtr)
    {}

    virtual ~LaplacianSmoothingImpl() {}

    /**
     * @brief generate Generates the Normals for the triangles
     * @param start The starting FaceArray::Face_t Index
     * @param end The ending FaceArray::Face_t Index
     */
    void generate(size_t start, size_t end) const
    {
      VertexArray& vertices = *(m_vertsPtr); // Get the pointer to the from of the array so we can use [] notation
      FaceArray& faces = *(m_facesPtr);
      VertexArray& newPositions = *(m_newPositions);
      Int32DynamicListArray::Pointer vertLinks = faces.getFacesContainingVert();

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
        DynamicListArray<int32_t>::ElementList& list = vertLinks->getElementList(v);
        QSet<int32_t> neighbours;
        // Create the unique List of Vertices that are directly connected to this vertex (vert)
        for(int32_t t = 0; t < list.ncells; ++t )
        {
          neighbours.insert(faces[list.cells[t]].verts[0]);
          neighbours.insert(faces[list.cells[t]].verts[1]);
          neighbours.insert(faces[list.cells[t]].verts[2]);
        }
        neighbours.remove(v); // Remove the current vertex id from the list as we don't need it

        float konst1 = lambdas[v] / neighbours.size();

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
    void operator()(const tbb::blocked_range<size_t>& r) const
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
  m_SurfaceDataContainerName(DREAM3D::Defaults::SurfaceDataContainerName),
  m_FaceAttributeMatrixName(DREAM3D::Defaults::FaceAttributeMatrixName),
  m_EdgeAttributeMatrixName(DREAM3D::Defaults::EdgeAttributeMatrixName),
  m_VertexAttributeMatrixName(DREAM3D::Defaults::VertexAttributeMatrixName),
  m_SurfaceMeshUniqueEdgesArrayName(DREAM3D::EdgeData::SurfaceMeshUniqueEdges),
  m_IterationSteps(1),
  m_Lambda(0.1),
  m_SurfacePointLambda(0.0),
  m_TripleLineLambda(0.0),
  m_QuadPointLambda(0.0),
  m_SurfaceTripleLineLambda(0.0),
  m_SurfaceQuadPointLambda(0.0),
  m_GenerateIterationOutputFiles(false),
  m_DoConnectivityFilter(true)
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
  FilterParameterVector parameters;
  /* Place all your option initialization code here */
  parameters.push_back(FilterParameter::New("Iteration Steps", "IterationSteps", FilterParameterWidgetType::IntWidget,"int", false));
  parameters.push_back(FilterParameter::New("Default Lambda", "Lambda", FilterParameterWidgetType::DoubleWidget,"float", false));
  parameters.push_back(FilterParameter::New("Triple Line Lambda", "TripleLineLambda", FilterParameterWidgetType::DoubleWidget,"float", false));
  parameters.push_back(FilterParameter::New("Quad Points Lambda", "QuadPointLambda", FilterParameterWidgetType::DoubleWidget,"float", false));
  parameters.push_back(FilterParameter::New("Outer Points Lambda", "SurfacePointLambda", FilterParameterWidgetType::DoubleWidget,"float", false));
  parameters.push_back(FilterParameter::New("Outer Triple Line Lambda", "SurfaceTripleLineLambda", FilterParameterWidgetType::DoubleWidget,"float", false));
  parameters.push_back(FilterParameter::New("Outer Quad Points Lambda", "SurfaceQuadPointLambda", FilterParameterWidgetType::DoubleWidget,"float", false));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LaplacianSmoothing::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setIterationSteps( reader->readValue("IterationSteps", getIterationSteps()) );
  setLambda( reader->readValue("Lambda", getLambda()) );
  setTripleLineLambda( reader->readValue("TripleLineLambda", getTripleLineLambda()) );
  setQuadPointLambda( reader->readValue("QuadPointLambda", getQuadPointLambda()) );
  setSurfacePointLambda( reader->readValue("SurfacePointLambda", getSurfacePointLambda()) );
  setSurfaceTripleLineLambda( reader->readValue("SurfaceTripleLineLambda", getSurfaceTripleLineLambda()) );
  setSurfaceQuadPointLambda( reader->readValue("SurfaceQuadPointLambda", getSurfaceQuadPointLambda()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LaplacianSmoothing::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
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
void LaplacianSmoothing::dataCheck()
{
  SurfaceDataContainer* sm = getDataContainerArray()->getPrereqDataContainer<SurfaceDataContainer, AbstractFilter>(this, getSurfaceDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }

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

  AttributeMatrix::Pointer attrMat = sm->getPrereqAttributeMatrix<AbstractFilter>(NULL, getEdgeAttributeMatrixName(), -386);
  if (NULL != attrMat)
  {
    if (attrMat->getAttributeArray(m_SurfaceMeshUniqueEdgesArrayName).get() == NULL)
    {
      m_DoConnectivityFilter = true;
    }
    else
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
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LaplacianSmoothing::execute()
{
  int err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  /* Place all your code to execute your filter here. */
  err = edgeBasedSmoothing();

  if (err < 0)
  {
    notifyErrorMessage(getHumanLabel(), "Error smoothing the surface mesh", getErrorCondition());
    return;
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LaplacianSmoothing::generateLambdaArray(DataArray<int8_t>* nodeTypePtr)
{
  notifyStatusMessage(getHumanLabel(), "Generating Lambda values");
  VertexArray::Pointer nodesPtr = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName())->getVertices();

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
        lambdas->setValue(i, m_Lambda);
        break;
      case DREAM3D::SurfaceMesh::NodeType::TriplePoint:
        lambdas->setValue(i, m_TripleLineLambda);
        break;
      case DREAM3D::SurfaceMesh::NodeType::QuadPoint:
        lambdas->setValue(i, m_QuadPointLambda);
        break;
      case DREAM3D::SurfaceMesh::NodeType::SurfaceDefault:
        lambdas->setValue(i, m_SurfacePointLambda);
        break;
      case DREAM3D::SurfaceMesh::NodeType::SurfaceTriplePoint:
        lambdas->setValue(i, m_SurfaceTripleLineLambda);
        break;
      case DREAM3D::SurfaceMesh::NodeType::SurfaceQuadPoint:
        lambdas->setValue(i, m_SurfaceQuadPointLambda);
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

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());
  //
  VertexArray::Pointer nodesPtr = sm->getVertices();
  int nvert = nodesPtr->getNumberOfTuples();
  VertexArray::Vert_t* vsm = nodesPtr->getPointer(0); // Get the pointer to the from of the array so we can use [] notation


  DataArray<int8_t>::Pointer nodeTypeSharedPtr = DataArray<int8_t>::NullPointer();
  DataArray<int8_t>* nodeTypePtr = nodeTypeSharedPtr.get();
  IDataArray::Pointer iNodeTypePtr = sm->getAttributeMatrix(getVertexAttributeMatrixName())->getAttributeArray(DREAM3D::VertexData::SurfaceMeshNodeType);

  if (NULL == iNodeTypePtr.get() )
  {
    // The node type array does not exist so create one with the default node type populated
    nodeTypeSharedPtr = DataArray<int8_t>::CreateArray(nodesPtr->getNumberOfTuples(), DREAM3D::VertexData::SurfaceMeshNodeType);
    nodeTypeSharedPtr->initializeWithValue(DREAM3D::SurfaceMesh::NodeType::Default);
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
    notifyErrorMessage(getHumanLabel(), "Error generating the Lambda Array", getErrorCondition());
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
    QString ss = QObject::tr("%1 |->Generating Unique Edge Ids |->").arg(getMessagePrefix());
    conn->setMessagePrefix(ss);
    connect(conn.get(), SIGNAL(filterGeneratedMessage(const PipelineMessage&)),
            this, SLOT(broadcastPipelineMessage(const PipelineMessage&)));
    conn->setDataContainerArray(getDataContainerArray());
    conn->setSurfaceDataContainerName(getSurfaceDataContainerName());
    conn->setEdgeAttributeMatrixName(getEdgeAttributeMatrixName());
    conn->setVertexAttributeMatrixName(getVertexAttributeMatrixName());
    conn->setSurfaceMeshUniqueEdgesArrayName(getSurfaceMeshUniqueEdgesArrayName());
    conn->execute();
    if(conn->getErrorCondition() < 0)
    {
      return conn->getErrorCondition();
    }
  }
  if (getCancel() == true) { return -1; }

  notifyStatusMessage(getHumanLabel(), "Starting to Smooth Vertices");
  // Get the unique Edges from the data container

  IDataArray::Pointer uniqueEdgesPtr = sm->getAttributeMatrix(getEdgeAttributeMatrixName())->getAttributeArray(m_SurfaceMeshUniqueEdgesArrayName);
  DataArray<int32_t>* uniqueEdges = DataArray<int32_t>::SafePointerDownCast(uniqueEdgesPtr.get());
  if (NULL == uniqueEdges)
  {
    setErrorCondition(-560);
    notifyErrorMessage(getHumanLabel(), "Error retrieving the Unique Edge List", getErrorCondition());
    return -560;
  }
  // Get a pointer to the Unique Edges
  int32_t* uedges = uniqueEdges->getPointer(0);
  int nedges = uniqueEdges->getNumberOfTuples();


  DataArray<int>::Pointer numConnections = DataArray<int>::CreateArray(nvert, "Laplacian_Smoothing_NumberConnections_Array");
  numConnections->initializeWithZeros();
  int* ncon = numConnections->getPointer(0);

  QVector<size_t> dims(1, 3);
  DataArray<double>::Pointer deltaArray = DataArray<double>::CreateArray(nvert, dims, "Laplacian_Smoothing_Delta_Array");
  deltaArray->initializeWithZeros();
  double* delta = deltaArray->getPointer(0);


  double dlta = 0.0;
  for (int q = 0; q < m_IterationSteps; q++)
  {
    if (getCancel() == true) { return -1; }
    QString ss = QObject::tr("Iteration %1").arg(q);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    for (int i = 0; i < nedges; i++)
    {
      int in_edge = 2 * i;
      size_t in1 = uedges[in_edge]; // row of the first vertex
      size_t in2 = uedges[in_edge + 1]; //row the second vertex

      for (int j = 0; j < 3; j++)
      {
        BOOST_ASSERT( 3 * in1 + j < nvert * 3);
        BOOST_ASSERT( 3 * in2 + j < nvert * 3);
        dlta = vsm[in2].pos[j] - vsm[in1].pos[j];
        delta[3 * in1 + j] += dlta;
        delta[3 * in2 + j] += -1.0 * dlta;
      }
      ncon[in1] += 1;
      ncon[in2] += 1;
    }


    float ll = 0.0f;
    for (int i = 0; i < nvert; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        int in0 = 3 * i + j;
        dlta = delta[in0] / ncon[i];

        ll = lambda[i];
        VertexArray::Vert_t& node = vsm[i];
        node.pos[j] += ll * dlta;
        delta[in0] = 0.0; //reset for next iteration
      }
      ncon[i] = 0;//reset for next iteration
    }

    if(m_GenerateIterationOutputFiles)
    {
      QString testFile = QString("LaplacianSmoothing_") + QString::number(q) + QString(".vtk");
      writeVTKFile(testFile);
    }
  }


  // This filter had to generate the edge connectivity data so delete it when we are done with it.
  if (m_DoConnectivityFilter == true)
  {
    IDataArray::Pointer removedConnectviity = sm->getAttributeMatrix(getEdgeAttributeMatrixName())->removeAttributeArray(m_SurfaceMeshUniqueEdgesArrayName);
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

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());

  // Convert the 32 bit float Nodes into 64 bit floating point nodes.
  VertexArray::Pointer vertsPtr = sm->getVertices();
  int numVerts = vertsPtr->getNumberOfTuples();
  //  VertexArray::Vert_t* vertices = vertsPtr->getPointer(0); // Get the pointer to the from of the array so we can use [] notation

  //Make sure the Triangle Connectivity is created because the FindNRing algorithm needs this and will
  // assert if the data is NOT in the SurfaceMesh Data Container
  FaceArray::Pointer facesPtr = sm->getFaces();
  Int32DynamicListArray::Pointer MeshLinks = facesPtr->getFacesContainingVert();
  if (NULL == MeshLinks.get())
  {
    sm->getFaces()->findFacesContainingVert();
  }

  //  FaceArray::Face_t* faces = facesPtr->getPointer(0);

  DataArray<int8_t>::Pointer nodeTypeSharedPtr = DataArray<int8_t>::NullPointer();
  DataArray<int8_t>* nodeTypePtr = nodeTypeSharedPtr.get();
  IDataArray::Pointer iNodeTypePtr = sm->getAttributeMatrix(getVertexAttributeMatrixName())->getAttributeArray(DREAM3D::VertexData::SurfaceMeshNodeType);

  if (NULL == iNodeTypePtr.get() )
  {
    // The node type array does not exist so create one with the default node type populated
    nodeTypeSharedPtr = DataArray<int8_t>::CreateArray(vertsPtr->getNumberOfTuples(), DREAM3D::VertexData::SurfaceMeshNodeType);
    nodeTypeSharedPtr->initializeWithValue(DREAM3D::SurfaceMesh::NodeType::Default);
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
    notifyErrorMessage(getHumanLabel(), "Error generating the Lambda Array", getErrorCondition());
    return err;
  }


  notifyStatusMessage(getHumanLabel(), "Starting to Smooth Vertices");

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  bool doParallel = true;
#endif

  // Get a Pointer to the Lambdas
  DataArray<float>::Pointer lambdasPtr = getLambdaArray();

  // We need an array to store the new positions
  VertexArray::Pointer newPositionsPtr = VertexArray::CreateArray(vertsPtr->getNumberOfTuples(), "New Vertex Positions");
  newPositionsPtr->initializeWithZeros();



  for (int q = 0; q < m_IterationSteps; q++)
  {
    if (getCancel() == true) { return -1; }
    QString ss = QObject::tr("Iteration %1").arg(q);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, numVerts),
                        LaplacianSmoothingImpl(vertsPtr, newPositionsPtr, MeshLinks, facesPtr, lambdasPtr), tbb::auto_partitioner());

    }
    else
#endif
    {
      LaplacianSmoothingImpl serial(vertsPtr, newPositionsPtr, MeshLinks, facesPtr, lambdasPtr);
      serial.generate(0, numVerts);
    }

    // SERIAL ONLY
    ::memcpy(vertsPtr->getPointer(0), newPositionsPtr->getPointer(0), sizeof(VertexArray::Vert_t) * vertsPtr->getNumberOfTuples());
    // -----------
#if OUTPUT_DEBUG_VTK_FILES
    QString testFile = QString("/tmp/LaplacianSmoothing_") + QString::number(q) + QString(".vtk");
    writeVTKFile(testFile);
#endif
  }
  return 1;
}


#if OUTPUT_DEBUG_VTK_FILES
namespace Detail
{
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
void LaplacianSmoothing::writeVTKFile(const QString& outputVtkFile)
{

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());  /* Place all your code to execute your filter here. */
  VertexArray& nodes = *(sm->getVertices());
  int nNodes = nodes.getNumberOfTuples();
  bool m_WriteBinaryFile = true;


  IDataArray::Pointer flPtr = sm->getAttributeMatrix(getFaceAttributeMatrixName())->getAttributeArray(DREAM3D::FaceData::SurfaceMeshFaceLabels);
  DataArray<int32_t>* faceLabelsPtr = DataArray<int32_t>::SafePointerDownCast(flPtr.get());
  int32_t* faceLabels = faceLabelsPtr->getPointer(0);

  FILE* vtkFile = NULL;
  vtkFile = fopen(outputVtkFile.toLatin1().data(), "wb");
  if (NULL == vtkFile)
  {
    setErrorCondition(-90123);
    QString ss = QObject::tr("Error creating file '%1'").arg(outputVtkFile);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  Detail::ScopedFileMonitor vtkFileMonitor(vtkFile);

  fprintf(vtkFile, "# vtk DataFile Version 2.0\n");
  fprintf(vtkFile, "Data set from DREAM.3D Surface Meshing Module\n");
  if (m_WriteBinaryFile)
  {
    fprintf(vtkFile, "BINARY\n");
  }
  else
  {
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
      else
      {
        fprintf(vtkFile, "%4.4f %4.4f %4.4f\n", pos[0], pos[1], pos[2]); // Write the positions to the output file
      }
    }
  }

  // Write the triangle indices into the vtk File
  FaceArray& triangles = *(sm->getFaces());
  int triangleCount = 0;
  int end = triangles.getNumberOfTuples();
  int featureInterest = 9;
  for(int i = 0; i < end; ++i)
  {
    //FaceArray::Face_t* tri = triangles.getPointer(i);
    if (faceLabels[i * 2] == featureInterest || faceLabels[i * 2 + 1] == featureInterest)
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
    if (faceLabels[tid * 2] == featureInterest || faceLabels[tid * 2 + 1] == featureInterest)
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer LaplacianSmoothing::newFilterInstance(bool copyFilterParameters)
{
  /*
  * IterationSteps
  * Lambda
  * SurfacePointLambda
  * TripleLineLambda
  * QuadPointLambda
  * SurfaceTripleLineLambda
  * SurfaceQuadPointLambda
  */
  LaplacianSmoothing::Pointer filter = LaplacianSmoothing::New();
  if(true == copyFilterParameters)
  {
    filter->setFilterParameters(getFilterParameters() );
    
    //Loop over each Filter Parameter that is registered to the filter either through this class or a parent class
    // and copy the value from the current instance of the object into the "new" instance that was just created
    QVector<FilterParameter::Pointer> options = getFilterParameters(); // Get the current set of filter parameters
    for (QVector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
    {
      FilterParameter* parameter = (*iter).get();
      if (parameter->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0 )
      {
        continue; // Skip this type of filter parameter as it has nothing to do with anything in the filter.
      }
      // Get the property from the current instance of the filter
      QVariant var = property(parameter->getPropertyName().toLatin1().constData());
      bool ok = filter->setProperty(parameter->getPropertyName().toLatin1().constData(), var);
      if(false == ok)
      {
        QString ss = QString("Error occurred transferring the Filter Parameter '%1' in Filter '%2' to the filter instance. The pipeline may run but the underlying filter will NOT be using the values from the GUI."
                             " Please report this issue to the developers of this filter.").arg(parameter->getPropertyName()).arg(filter->getHumanLabel());
        Q_ASSERT_X(ok, __FILE__, ss.toLatin1().constData());
      }

      if(parameter->isConditional() == true)
      {
        QVariant cond = property(parameter->getConditionalProperty().toLatin1().constData() );
        ok = filter->setProperty(parameter->getConditionalProperty().toLatin1().constData(), cond);
        if(false == ok)
        {
          QString ss = QString("%1::newFilterInstance()\nError occurred transferring the Filter Parameter '%2' in Filter '%3' to the filter instance. "
                              " The filter parameter has a conditional property '%4'. The transfer of this property from the old filter to the new filter failed."
                              " Please report this issue to the developers of this filter.").arg(filter->getNameOfClass())
                              .arg(parameter->getPropertyName())
                              .arg(filter->getHumanLabel())
                              .arg(parameter->getConditionalProperty());
          Q_ASSERT_X(ok, __FILE__, ss.toLatin1().constData());
        }
      }
    }
  }
  return filter;
}
