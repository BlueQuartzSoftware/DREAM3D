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


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LaplacianSmoothing::LaplacianSmoothing() :
AbstractFilter(),
m_IterationSteps(1),
m_Lambda(0.01)
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
    parameter->setHumanLabel("Lambda");
    parameter->setPropertyName("Lambda");
    parameter->setWidgetType(FilterParameter::DoubleWidget);
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
    writer->writeValue("Lambda", getLambda() );
      writer->writeValue("IterationSteps", getIterationSteps());

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LaplacianSmoothing::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);

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
    IDataArray::Pointer edges = sm->getCellData(DREAM3D::CellData::SurfaceMesh::Edges);
    if(edges.get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Edges", -385);
      setErrorCondition(-385);
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
  SurfaceMeshDataContainer* m = getSurfaceMeshDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The SurfaceMesh DataContainer Object was NULL", -999);
    return;
  }
  StructArray<Node>::Pointer floatNodesPtr = m->getNodes();
  if(NULL == floatNodesPtr.get())
  {
    setErrorCondition(-555);
    notifyErrorMessage("The SurfaceMesh DataContainer Does NOT contain Nodes", -555);
    return;
  }

  StructArray<Triangle>::Pointer trianglesPtr = m->getTriangles();
  if(NULL == trianglesPtr.get())
  {
    setErrorCondition(-556);
    notifyErrorMessage("The SurfaceMesh DataContainer Does NOT contain Triangles", -556);
    return;
  }

  /* Place all your code to execute your filter here. */
  err = smooth();
  if (err < 0)
  {
    setErrorCondition(-558);
    notifyErrorMessage("Error smoothing the surface mesh", -558);
    return;
  }


  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LaplacianSmoothing::generateLambdaArray()
{
  StructArray<Node>::Pointer floatNodesPtr = getSurfaceMeshDataContainer()->getNodes();
  if(NULL == floatNodesPtr.get())
  {
    setErrorCondition(-555);
    notifyErrorMessage("The SurfaceMesh DataContainer Does NOT contain Nodes", -555);
    return -1;
  }
  int numNodes = floatNodesPtr->GetNumberOfTuples();
  DataArray<float>::Pointer lambdas = DataArray<float>::CreateArray(numNodes, "Laplacian_Smoothing_Lambda_Array");
  for(int i = 0; i < numNodes; ++i)
  {
    lambdas->SetValue(i, m_Lambda);
  }

  setLambdaArray(lambdas);
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArray<int>::Pointer LaplacianSmoothing::generateUniqueEdges()
{

// this function returns a list of unique edges in the polygon list.  A 2 x M array is returned that lists
// the unique pairs of vertex points that are paired along an edge.  This is the way that this function
// opertates in this program.  The time to cacluate the vertex connectivity arrays is long, and not needed
// for mesh smoothing.


  // Get our Reference counted Array of Triangle Structures
  StructArray<Triangle>::Pointer trianglesPtr = getSurfaceMeshDataContainer()->getTriangles();
  if(NULL == trianglesPtr.get())
  {
    setErrorCondition(-556);
    notifyErrorMessage("The SurfaceMesh DataContainer Does NOT contain Triangles", -556);
    return DataArray<int>::NullPointer();
  }
  int ntri = trianglesPtr->GetNumberOfTuples();


  // get the triangle definitions - use the pointer to the start of the Struct Array
  Triangle* triangles = trianglesPtr->GetPointer(0);


  // need to make a list of triangle edges
  // each triangle has three edges, made up of two pairs of vertices
  std::set<int64_t> uedges;

  struct  { int32_t e0; int32_t e1; } edge;
  int64_t* u64Edge = reinterpret_cast<int64_t*>(&edge);

  for(int i = 0; i < ntri; ++i)
  {
    Triangle& tri = triangles[i];
    // Edge 0
    edge.e0 = tri.node_id[0];
    edge.e1 = tri.node_id[1];
    uedges.insert(*u64Edge); // Insert our 64 bit value into the set. Only unique values can be in a set so ther will be NO duplicates
      // Edge 1
    edge.e0 = tri.node_id[1];
    edge.e1 = tri.node_id[2];
    uedges.insert(*u64Edge);
        // Edge 2
    edge.e0 = tri.node_id[2];
    edge.e1 = tri.node_id[0];
    uedges.insert(*u64Edge);
  }

  // Now copy the unique Edges out of the set and into an array and return that array

  DataArray<int>::Pointer uniqueEdgesArray = DataArray<int>::CreateArray(uedges.size(), 2, "Laplacian_Smoothing_Unique_Edges_Array");
  int i = 0;
  for(std::set<int64_t>::iterator iter = uedges.begin(); iter != uedges.end(); ++iter)
  {
    *u64Edge = *iter;
    uniqueEdgesArray->SetComponent(i, 0, edge.e0);
    uniqueEdgesArray->SetComponent(i, 1, edge.e1);
    ++i;
  }
  return uniqueEdgesArray;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LaplacianSmoothing::smooth()
{
  int err = 0;

  // Generate the Lambdas possible using a subclasses implementation of the method
  err = generateLambdaArray();
  if (err < 0)
  {
    setErrorCondition(-557);
    notifyErrorMessage("Error generating the Lambda Array", -557);
    return err;
  }
  // Get a Pointer to the Lambdas
  DataArray<float>::Pointer lambdas = getLambdaArray();
  float* lambda = lambdas->GetPointer(0);

  // Generate the Unique Edges
  DataArray<int>::Pointer uniqueEdges = generateUniqueEdges();
  if (NULL == uniqueEdges)
  {
    return -1;
  }
  // Get a pointer to the Unique Edges
  int* uedges = uniqueEdges->GetPointer(0);
  int nedges = uniqueEdges->GetNumberOfTuples();


  // Convert the 32 bit float Nodes into 64 bit floating point nodes.
  StructArray<Node>::Pointer floatNodesPtr = getSurfaceMeshDataContainer()->getNodes();
  int nvert = floatNodesPtr->GetNumberOfTuples();
  Node* nodesF = floatNodesPtr->GetPointer(0); // Get the pointer to the from of the array so we can use [] notation
  int numberNodes = floatNodesPtr->GetNumberOfTuples();

  StructArray<NodeD>::Pointer nodesDPtr = StructArray<NodeD>::CreateArray(numberNodes, "MFE_Double_Nodes");
  nodesDPtr->initializeWithZeros();
  NodeD* vsm = nodesDPtr->GetPointer(0);

  // Copy the nodes from the 32 bit floating point to the 64 bit floating point
  for(int n = 0; n < numberNodes; ++n)
  {
    vsm[n].coord[0] = nodesF[n].coord[0];
    vsm[n].coord[1] = nodesF[n].coord[1];
    vsm[n].coord[2] = nodesF[n].coord[2];
    vsm[n].nodeKind = nodesF[n].nodeKind;
  }

  float delta0[3];

  DataArray<int>::Pointer numConnections = DataArray<int>::CreateArray(nvert, "Laplacian_Smoothing_NumberConnections_Array");
  int* ncon = numConnections->GetPointer(0);
  DataArray<double>::Pointer deltaArray = DataArray<double>::CreateArray(nvert, 3, "Laplacian_Smoothing_Delta_Array");
  double* delta = deltaArray->GetPointer(0);
  std::stringstream ss;
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

      for (int j=0;j<3;j++)
      {
        delta0[j] = vsm[in2].coord[j] - vsm[in1].coord[j];
        delta[3*in1+j] += delta0[j];
        delta[3*in2+j] += -1.0*delta0[j];
      }
      ncon[in1] += 1;
      ncon[in2] += 1;
    }


    for (int i=0; i < nvert; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        int in0 = 3*i+j;
        delta[in0] /= ncon[i];
        vsm[i].coord[j] += lambda[i]*delta[in0];
        delta[in0] = 0.0; //reset for next iteration
      }
      ncon[i] = 0;//reset for next iteration
    }

  }



  // Copy the nodes from the 64 bit floating point to the 32 bit floating point
  for(int n = 0; n < numberNodes; ++n)
  {
    nodesF[n].coord[0] = vsm[n].coord[0];
    nodesF[n].coord[1] = vsm[n].coord[1];
    nodesF[n].coord[2] = vsm[n].coord[2];
    nodesF[n].nodeKind = vsm[n].nodeKind;
  }




  return 1;
}


