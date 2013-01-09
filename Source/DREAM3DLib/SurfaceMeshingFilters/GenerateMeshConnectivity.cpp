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

#include "GenerateMeshConnectivity.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateMeshConnectivity::GenerateMeshConnectivity() :
AbstractFilter(),
m_SurfaceMeshUniqueEdgesArrayName(DREAM3D::CellData::SurfaceMeshUniqueEdges),
m_SurfaceMeshTriangleEdgesArrayName(DREAM3D::CellData::SurfaceMeshTriangleEdges),
m_SurfaceMeshUniqueEdges(NULL),
m_SurfaceMeshTriangleEdges(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateMeshConnectivity::~GenerateMeshConnectivity()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateMeshConnectivity::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  /* Place all your option initialization code here */
  /* For String input use this code */
  /* {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("STL Output Prefix");
    parameter->setPropertyName("StlFilePrefix");
    parameter->setWidgetType(FilterParameter::StringWidget);
    parameter->setValueType("string");
    parameters.push_back(parameter);
  }*/
  /*  For an Integer use this code*/
  /* {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Max Iterations");
    parameter->setPropertyName("MaxIterations");
    parameter->setWidgetType(FilterParameter::IntWidget);
    parameter->setValueType("int");
    parameters.push_back(parameter);
  }*/
  /*  For a Floating point value use this code*/
  /* {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Misorientation Tolerance");
    parameter->setPropertyName("MisorientationTolerance");
    parameter->setWidgetType(FilterParameter::DoubleWidget);
    parameter->setValueType("float");
    parameter->setCastableValueType("double");
    parameters.push_back(parameter);
  }*/
  /*   For an input file use this code*/
  /*  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Input File");
    parameter->setPropertyName("InputFile");
    parameter->setWidgetType(FilterParameter::InputFileWidget);
    parameter->setValueType("string");
    parameters.push_back(parameter);
  }*/
  /*   For an output file use this code*/
  /* {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Alignment File");
    parameter->setPropertyName("AlignmentShiftFileName");
    parameter->setWidgetType(FilterParameter::OutputFileWidget);
    parameter->setValueType("string");
    parameters.push_back(parameter);
  }*/
  /*   For a simple true/false boolean use this code*/
  /* {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Write Alignment Shift File");
    parameter->setPropertyName("WriteAlignmentShifts");
    parameter->setWidgetType(FilterParameter::BooleanWidget);
    parameter->setValueType("bool");
    parameters.push_back(parameter);
  }*/
  /*   For presenting a set of choices to the user use this code*/
  /* {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Conversion Type");
    parameter->setPropertyName("ConversionType");
    parameter->setWidgetType(FilterParameter::ChoiceWidget);
    parameter->setValueType("unsigned int");
    std::vector<std::string> choices;
    choices.push_back("Degrees To Radians");
    choices.push_back("Radians To Degrees");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }*/


  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateMeshConnectivity::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  /*  writer->writeValue("OutputFile", getOutputFile() ); */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateMeshConnectivity::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", -383);
      setErrorCondition(-384);
    }
    else
    {
      // This depends on the triangles array already being created
      int size = sm->getTriangles()->GetNumberOfTuples();
      CREATE_NON_PREREQ_DATA(sm, DREAM3D, CellData, SurfaceMeshTriangleEdges, ss, int32_t, Int32ArrayType, 0, size, 3)
    }

    // We do not know the size of the array so we can not use the macro so we just manually call
    // the needed methods that will propagate these array additions to the pipeline
    DataArray<int>::Pointer uniqueEdgesArray = DataArray<int>::CreateArray(1, 2, DREAM3D::CellData::SurfaceMeshUniqueEdges);
    sm->addCellData(DREAM3D::CellData::SurfaceMeshUniqueEdges, uniqueEdgesArray);

    // This is just for tracking what Arrays are being created by this filter. Normally the macro
    // would do this for us.
    addCreatedCellData(DREAM3D::CellData::SurfaceMeshUniqueEdges);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateMeshConnectivity::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateMeshConnectivity::execute()
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

  /* Place all your code to execute your filter here. */
  generateConnectivity();


  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateMeshConnectivity::generateConnectivity()
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
    return;
  }
  int ntri = trianglesPtr->GetNumberOfTuples();


  // get the triangle definitions - use the pointer to the start of the Struct Array
  Triangle* triangles = trianglesPtr->GetPointer(0);


  // need to make a list of triangle edges
  // each triangle has three edges, made up of two pairs of vertices
  std::map<int64_t, int> uedges_id_map;

  int edge_id = 0;
  int cur_edge_id = 0;

  struct  { int32_t e0; int32_t e1; } edge;
  int64_t* u64Edge = reinterpret_cast<int64_t*>(&edge); // This pointer is a 64 bit integer interpretation of the above struct variable

  // Create our array that will hold the 3 edges for each triangle. The actual value stored will be the
  // index into the SurfaceMeshUniqueEdges array that describes the 2 vertices of each Edge
  DataArray<int>::Pointer triangleEdgesArrayPtr = DataArray<int>::CreateArray(trianglesPtr->GetNumberOfTuples(), 3, DREAM3D::CellData::SurfaceMeshTriangleEdges);
  triangleEdgesArrayPtr->initializeWithZeros();
  m_SurfaceMeshTriangleEdges = triangleEdgesArrayPtr->GetPointer(0);

  typedef std::map<int64_t, int>::iterator EdgesIdMapIterator_t;
  for(int i = 0; i < ntri; ++i)
  {
    Triangle& tri = triangles[i];
    // Edge 0
    edge.e0 = tri.node_id[0];
    edge.e1 = tri.node_id[1];
    EdgesIdMapIterator_t iter = uedges_id_map.find(*u64Edge);
    if ( iter == uedges_id_map.end())
    {
       uedges_id_map[*u64Edge] = edge_id;
       cur_edge_id = edge_id;
       ++edge_id;
    }
    else
    {
      cur_edge_id = iter->second;
    }
   // triangleEdgesArray->SetComponent(i, 0, cur_edge_id);
    m_SurfaceMeshTriangleEdges[i*3] = cur_edge_id;

      // Edge 1
    edge.e0 = tri.node_id[1];
    edge.e1 = tri.node_id[2];
    iter = uedges_id_map.find(*u64Edge);
    if ( iter == uedges_id_map.end())
    {
       uedges_id_map[*u64Edge] = edge_id;
       cur_edge_id = edge_id;
       ++edge_id;
    }
    else
    {
      cur_edge_id = iter->second;
    }
    //triangleEdgesArray->SetComponent(i, 1, cur_edge_id);
    m_SurfaceMeshTriangleEdges[i*3 + 1] = cur_edge_id;
        // Edge 2
    edge.e0 = tri.node_id[2];
    edge.e1 = tri.node_id[0];
    iter = uedges_id_map.find(*u64Edge);
    if ( iter == uedges_id_map.end())
    {
       uedges_id_map[*u64Edge] = edge_id;
       cur_edge_id = edge_id;
       ++edge_id;
    }
    else
    {
      cur_edge_id = iter->second;
    }
   // triangleEdgesArray->SetComponent(i, 2, cur_edge_id);
    m_SurfaceMeshTriangleEdges[i*3 + 2] = cur_edge_id;
  }

  // Now copy the unique Edges out of the map and into an array at the proper index (which is the "value" that goes with the "key" to the map.
  int index = 0;
  DataArray<int>::Pointer uniqueEdgesArrayPtr = DataArray<int>::CreateArray(uedges_id_map.size(), 2, "Laplacian_Smoothing_Unique_Edges_Array");
  m_SurfaceMeshUniqueEdges = uniqueEdgesArrayPtr->GetPointer(0);
  for(std::map<int64_t, int>::iterator iter = uedges_id_map.begin(); iter != uedges_id_map.end(); ++iter)
  {
    *u64Edge = iter->first;
    index = iter->second;
//    uniqueEdgesArray->SetComponent(index, 0, edge.e0);
//    uniqueEdgesArray->SetComponent(index, 1, edge.e1);
    m_SurfaceMeshUniqueEdges[index*2] = edge.e0;
    m_SurfaceMeshUniqueEdges[index*2 + 1] = edge.e1;

  }


  // Finally push both the arrays into the Data Container for the pipeline
  getSurfaceMeshDataContainer()->addCellData(DREAM3D::CellData::SurfaceMeshUniqueEdges, uniqueEdgesArrayPtr);
  getSurfaceMeshDataContainer()->addCellData(DREAM3D::CellData::SurfaceMeshTriangleEdges, triangleEdgesArrayPtr);

  return;
}

