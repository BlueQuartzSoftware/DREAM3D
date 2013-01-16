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

#include "DREAM3DLib/Common/DREAM3DMath.h"

#include "MXA/Common/MXAEndian.h"
#include "MXA/Utilities/MXAFileInfo.h"
#include "MXA/Utilities/MXADir.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/SurfaceMeshStructs.h"
#include "DREAM3DLib/SurfaceMeshingFilters/GenerateUniqueEdges.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LaplacianSmoothing::LaplacianSmoothing() :
AbstractFilter(),
m_SurfaceMeshUniqueEdgesArrayName(DREAM3D::CellData::SurfaceMeshUniqueEdges),
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
    writer->writeValue("TripleLineLambda", getTripleLineLambda());
    writer->writeValue("QuadPointLambda", getQuadPointLambda());
    writer->writeValue("SurfacePointLambda", getSurfacePointLambda());
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
      // We MUST have Nodes
    if(sm->getNodes().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", -384);
      setErrorCondition(-384);
    }

    // We MUST have Triangles defined also.
    if(sm->getTriangles().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", -385);
      setErrorCondition(-385);
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
  notifyStatusMessage("Generating Lambda values");
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
  std::stringstream ss;

  // Generate the Unique Edges
  if (m_DoConnectivityFilter == true)
  {
    GenerateUniqueEdges::Pointer conn = GenerateUniqueEdges::New();
    ss.str("");
    ss << getMessagePrefix() << "|->Generating Unique Edge Ids |->";
    conn->setMessagePrefix(ss.str());
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
  if (getCancel() == true) { return -1; }

  notifyStatusMessage("Starting to Smooth Vertices");
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


  double dlta = 0.0;

//  char buf[8];
//  ::memset(buf, 0, 8);
//int8_t* nodeType = nodeTypePtr->GetPointer(0);
  for (int q=0; q<m_IterationSteps; q++)
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
        assert( 3*in1+j < nvert*3);
        assert( 3*in2+j < nvert*3);
        dlta = vsm[in2].coord[j] - vsm[in1].coord[j];
//        if (dlta == 0xFFFFFA0000000)
//        {
//          std::cout << "NAN found." << std::endl;
//        }
        delta[3*in1+j] += dlta;
        delta[3*in2+j] += -1.0*dlta;
      }
      ncon[in1] += 1;
      ncon[in2] += 1;
    }


    float ll = 0.0f;
    for (int i=0; i < nvert; i++)
    {

//          if (nodeType[i] != 2 && nodeType[i] != 3 && nodeType[i] != 4
//              && nodeType[i] != 12 && nodeType[i] != 13 && nodeType[i] != 14)
//              {
//                assert(true);
//              }

//       if ((int)(nodeType[i]) == 5 )
//       {
//         std::cout << "Bad Node Type" << std::endl;
//        assert(true);
//       }
      for (int j = 0; j < 3; j++)
      {
        int in0 = 3*i+j;
        dlta = delta[in0] / ncon[i];

        ll = lambda[i];
        Node& node = vsm[i];
        node.coord[j] += ll*dlta;
//        ::memset(buf, 0, 8);
//        snprintf(buf, 8, "%f", node.coord[j]);
//        if (strcmp("nan", buf) == 0)
//        {
//          std::cout << "first nan encountered." << std::endl;
//          std::cout << "Node ID = " << i << std::endl;
//          std::cout << "Node Type: " << (int)(nodeType[i]) << std::endl;
//          std::cout << "Lambda for Node: " << lambda[i] << std::endl;
//        }

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
  StructArray<Node>::Pointer nodesPtr = m->getNodes();
  StructArray<Node>& nodes = *(nodesPtr);
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
    Node& n = nodes[i]; // Get the current Node
    //  if (m_SurfaceMeshNodeType[i] > 0)
    {
      pos[0] = static_cast<float>(n.coord[0]);
      pos[1] = static_cast<float>(n.coord[1]);
      pos[2] = static_cast<float>(n.coord[2]);
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
  StructArray<Triangle>& triangles = *(m->getTriangles());
  int triangleCount = 0;
  int end = triangles.GetNumberOfTuples();
  for(int i = 0; i < end; ++i)
  {
    Triangle* tri = triangles.GetPointer(i);
    if (tri->nSpin[0] == 227 || tri->nSpin[1] == 227)
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
    Triangle* tri = triangles.GetPointer(tid);
    if (tri->nSpin[0] == 227 || tri->nSpin[1] == 227)
    {
      tData[1] = triangles[tid].node_id[0];
      tData[2] = triangles[tid].node_id[1];
      tData[3] = triangles[tid].node_id[2];
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


