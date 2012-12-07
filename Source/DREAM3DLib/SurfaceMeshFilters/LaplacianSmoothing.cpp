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

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/SurfaceMeshStructs.h"
#include "DREAM3DLib/SurfaceMeshFilters/GenerateMeshConnectivity.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LaplacianSmoothing::LaplacianSmoothing() :
AbstractFilter(),
m_SurfaceMeshUniqueEdgesArrayName(DREAM3D::CellData::SurfaceMeshUniqueEdges),
//m_SurfaceMeshTriangleEdgesArrayName(DREAM3D::CellData::SurfaceMeshTriangleEdges),
m_IterationSteps(1),
m_Lambda(0.01),
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
void LaplacianSmoothing::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */

    writer->writeValue("IterationSteps", getIterationSteps());
    writer->writeValue("Lambda", getLambda() );
    writer->writeValue("SurfacePointLambda", getSurfacePointLambda());
    writer->writeValue("TripleLineLambda", getTripleLineLambda());
    writer->writeValue("QuadPointLambda", getQuadPointLambda());
    writer->writeValue("SurfaceTripleLineLambda", getSurfaceTripleLineLambda());
    writer->writeValue("SurfaceQuadPointLambda", getSurfaceQuadPointLambda());
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
    addErrorMessage(getHumanLabel(), "SurfaceMeshDataContainer is missing", -383);
    setErrorCondition(-384);
  }
  else
  {
    if(sm->getTriangles().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", -383);
      setErrorCondition(-384);
    }
    if(sm->getNodes().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", -384);
      setErrorCondition(-384);
    }

    if (sm->getCellData(m_SurfaceMeshUniqueEdgesArrayName).get() == NULL)
    {
        m_DoConnectivityFilter = true;
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
  SurfaceMeshDataContainer* m = getSurfaceMeshDataContainer();
  StructArray<Node>::Pointer vertexPtr = m->getNodes();
  StructArray<Triangle>::Pointer trianglesPtr = m->getTriangles();

  /* Place all your code to execute your filter here. */
  err = smooth();

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
  StructArray<Node>::Pointer nodesPtr = getSurfaceMeshDataContainer()->getNodes();
  if(NULL == nodesPtr.get())
  {
    setErrorCondition(-555);
    notifyErrorMessage("The SurfaceMesh DataContainer Does NOT contain Nodes", -555);
    return -1;
  }
  int numNodes = nodesPtr->GetNumberOfTuples();
  int8_t* nodeType = nodeTypePtr->GetPointer(0);

  DataArray<float>::Pointer lambdas = DataArray<float>::CreateArray(numNodes, "Laplacian_Smoothing_Lambda_Array");
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
int LaplacianSmoothing::smooth()
{
  int err = 0;


  // Convert the 32 bit float Nodes into 64 bit floating point nodes.
  StructArray<Node>::Pointer nodesPtr = getSurfaceMeshDataContainer()->getNodes();
  int nvert = nodesPtr->GetNumberOfTuples();
  Node* vsm = nodesPtr->GetPointer(0); // Get the pointer to the from of the array so we can use [] notation


  DataArray<int8_t>::Pointer nodeTypeSharedPtr = DataArray<int8_t>::NullPointer();
  DataArray<int8_t>* nodeTypePtr = nodeTypeSharedPtr.get();
  IDataArray::Pointer iNodeTypePtr = getSurfaceMeshDataContainer()->getCellData(DREAM3D::CellData::SurfaceMeshNodeType);

  if (NULL == iNodeTypePtr.get() )
  {
    // The node type array does not exist so create one with the default node type populated
    nodeTypeSharedPtr = DataArray<int8_t>::CreateArray(nodesPtr->GetNumberOfTuples(), DREAM3D::CellData::SurfaceMeshNodeType);
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
  // Get a Pointer to the Lambdas
  DataArray<float>::Pointer lambdas = getLambdaArray();
  float* lambda = lambdas->GetPointer(0);

  // Generate the Unique Edges
  if (m_DoConnectivityFilter == true)
  {
    GenerateMeshConnectivity::Pointer conn = GenerateMeshConnectivity::New();
    conn->setMessagePrefix(getMessagePrefix());
    conn->setObservers(getObservers());
    conn->setVoxelDataContainer(getVoxelDataContainer());
    conn->setSurfaceMeshDataContainer(getSurfaceMeshDataContainer());
    conn->setSolidMeshDataContainer(getSolidMeshDataContainer());
    conn->execute();
    if(conn->getErrorCondition() < 0)
    {
      return conn->getErrorCondition();
    }
  }

  // Get the unique Edges from the data container
  IDataArray::Pointer uniqueEdgesPtr = getSurfaceMeshDataContainer()->getCellData(m_SurfaceMeshUniqueEdgesArrayName);
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

  std::stringstream ss;
  double dlta = 0.0;

  for (int q=0; q<m_IterationSteps; q++)
  {
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
        assert( 3*in1+j < nvert*3);
        assert( 3*in2+j < nvert*3);
        dlta = vsm[in2].coord[j] - vsm[in1].coord[j];
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
        Node& node = vsm[i];
        node.coord[j] += ll*dlta;
//        if (node.coord[j] > 1000.0)
//        {
//          std::cout << "Here: " << node.coord[j] << std::endl;
//        }
        delta[in0] = 0.0; //reset for next iteration
      }
      ncon[i] = 0;//reset for next iteration
    }

  }

  return 1;
}


