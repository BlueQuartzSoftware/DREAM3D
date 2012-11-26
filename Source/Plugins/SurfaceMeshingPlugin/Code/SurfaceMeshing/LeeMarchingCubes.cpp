/* ============================================================================
 * Copyright (c)  2010 Dr.Sukbin Lee slee@andrew.cmu.edu
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
 * Neither the name of Dr.Sukbin Lee, Carnegie-Mellon University nor the names of
 * its contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ============================================================================
 * Copyright (c) 2009, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2009, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "LeeMarchingCubes.h"

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#endif

// Include this FIRST because there is a needed define for some compiles
// to expose some of the constants needed below
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/PipelineMessage.h"

// C Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-- C++ STL
#include <vector>
#include <map>
#include <sstream>
#include <queue>
#include <vector>
#include <map>


#include "MXA/Common/LogTime.h"
#include "MXA/Common/MXAEndian.h"
#include "MXA/Utilities/MXADir.h"
#include "MXA/Utilities/MXAFileInfo.h"
#include "MXA/Utilities/StringUtils.h"

#include "SMVtkPolyDataWriter.h"



#define WRITE_BINARY_TEMP_FILES 1

namespace Detail
{
  const std::string NodesFile("Nodes.bin");
  const std::string TrianglesFile("Triangles.bin");
}

  int edgeTable_2d[20][8] =
  {
  { -1, -1, -1, -1, -1, -1, -1, -1 },
  { -1, -1, -1, -1, -1, -1, -1, -1 },
  { -1, -1, -1, -1, -1, -1, -1, -1 },
  { 0, 1, -1, -1, -1, -1, -1, -1 },
  { -1, -1, -1, -1, -1, -1, -1, -1 },
  { 0, 2, -1, -1, -1, -1, -1, -1 },
  { 1, 2, -1, -1, -1, -1, -1, -1 },
  { 0, 4, 2, 4, 1, 4, -1, -1 },
  { -1, -1, -1, -1, -1, -1, -1, -1 },
  { 3, 0, -1, -1, -1, -1, -1, -1 },
  { 3, 1, -1, -1, -1, -1, -1, -1 },
  { 3, 4, 0, 4, 1, 4, -1, -1 },
  { 2, 3, -1, -1, -1, -1, -1, -1 },
  { 3, 4, 0, 4, 2, 4, -1, -1 },
  { 3, 4, 1, 4, 2, 4, -1, -1 },
  { 3, 0, 1, 2, -1, -1, -1, -1 },
  { 0, 1, 2, 3, -1, -1, -1, -1 },
  { 0, 1, 2, 3, -1, -1, -1, -1 },
  { 3, 0, 1, 2, -1, -1, -1, -1 },
  { 3, 4, 1, 4, 0, 4, 2, 4 } };

  int nsTable_2d[20][8] =
  {
  { -1, -1, -1, -1, -1, -1, -1, -1 },
  { -1, -1, -1, -1, -1, -1, -1, -1 },
  { -1, -1, -1, -1, -1, -1, -1, -1 },
  { 1, 0, -1, -1, -1, -1, -1, -1 },
  { -1, -1, -1, -1, -1, -1, -1, -1 },
  { 1, 0, -1, -1, -1, -1, -1, -1, },
  { 2, 1, -1, -1, -1, -1, -1, -1 },
  { 1, 0, 3, 2, 2, 1, -1, -1 },
  { -1, -1, -1, -1, -1, -1, -1, -1 },
  { 0, 3, -1, -1, -1, -1, -1, -1 },
  { 0, 3, -1, -1, -1, -1, -1, -1 },
  { 0, 3, 1, 0, 2, 1, -1, -1 },
  { 3, 2, -1, -1, -1, -1, -1, -1 },
  { 0, 3, 1, 0, 3, 2, -1, -1 },
  { 0, 3, 2, 1, 3, 2, -1, -1 },
  { 0, 3, 2, 1, -1, -1, -1, -1 },
  { 1, 0, 3, 2, -1, -1, -1, -1 },
  { 1, 0, 3, 2, -1, -1, -1, -1 },
  { 0, 3, 2, 1, -1, -1, -1, -1 },
  { 0, 3, 2, 1, 1, 0, 3, 2 } };



class GrainChecker
{
  public:
    DREAM3D_SHARED_POINTERS(GrainChecker)
    DREAM3D_STATIC_NEW_MACRO(GrainChecker)
    virtual ~GrainChecker(){}
    typedef std::map<int, int>  MapType;

    template<typename T>
    static void TripletSort(T a, T b, T c, T* sorted)
    {
      if ( a > b && a > c)
      {
        sorted[2] = a;
        if (b > c) { sorted[1] = b; sorted[0] = c; }
        else { sorted[1] = c; sorted[0] = b; }
      }
      else if ( b > a && b > c)
      {
        sorted[2] = b;
        if (a > c) { sorted[1] = a; sorted[0] = c; }
        else { sorted[1] = c; sorted[0] = a; }
      }
      else if ( a > b )
      {
        sorted[1] = a; sorted[0] = b; sorted[2] = c;
      }
      else if (a >= c && b >=c)
      {
        sorted[0] = c; sorted[1] = a; sorted[2] = b;
      }
      else
      { sorted[0] = a; sorted[1] = b; sorted[2] = c;}
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
    void addData(int numTriangles, int ctid, const std::vector<meshing::Patch::Pointer>& cTriangle, meshing::Node* cVertex)
    {

      int n1, n2, n3;
      int gid;
      int size;
      for (int i = 0; i < numTriangles; ++i)
      {
        gid = cTriangle[i]->nSpin[0];
        size = static_cast<int>(grainMaps.size());
        if (gid >= size)
        {
          grainMaps.resize(gid + 1);
          tCounts.resize(gid + 1);
        }
        if (gid > 0)
        {
          // Get a reference to the map for this grain
          MapType& vertMap = grainMaps[gid];
          MapType& triMap = tCounts[gid];

          n1 = cTriangle[i]->node_id[0];
          n2 = cTriangle[i]->node_id[1];
          n3 = cTriangle[i]->node_id[2];
          n1 = cVertex[n1].NodeID;
          n2 = cVertex[n2].NodeID;
          n3 = cVertex[n3].NodeID;

          vertMap[n1] = 0;
          vertMap[n2] = 0;
          vertMap[n3] = 0;
          triMap[ctid] = 0;
        }

        gid = cTriangle[i]->nSpin[1];
        size = static_cast<int>(grainMaps.size());
        if (gid >= size)
        {
          grainMaps.resize(gid + 1);
          tCounts.resize(gid + 1);
        }
        if (gid > 0)
        { // Get a reference to the map for this grain
          MapType& vertMap = grainMaps[gid];
          MapType& triMap = tCounts[gid];

          n1 = cTriangle[i]->node_id[0];
          n2 = cTriangle[i]->node_id[1];
          n3 = cTriangle[i]->node_id[2];
          n1 = cVertex[n1].NodeID;
          n2 = cVertex[n2].NodeID;
          n3 = cVertex[n3].NodeID;

          vertMap[n1] = 0;
          vertMap[n2] = 0;
          vertMap[n3] = 0;
          triMap[ctid] = 0;
        }
        //
        ++ctid;
      }

    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
    void analyzeGrains()
    {
      size_t size = tCounts.size();
      size_t vCount = 0;
      size_t tCount = 0;
      for (size_t i = 1; i < size; ++i)
      {
        MapType& t = tCounts[i];
        tCount = t.size();
        MapType& m = grainMaps[i];
        vCount = m.size();
        if (tCount + 4 != vCount * 2)
        {
          std::cout << "Grain ID: " << i << " Does not satisfy equation T=2V-4    " << "  tCount: " << tCount << "   " << "  vCount: " << vCount << std::endl;
        }
      }
    }

  protected:
    GrainChecker() {}

  private:
    std::vector<MapType> grainMaps;
    std::vector<MapType> tCounts;

    GrainChecker(const GrainChecker&); // Copy Constructor Not Implemented
    void operator=(const GrainChecker&); // Operator '=' Not Implemented
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LeeMarchingCubes::LeeMarchingCubes() :
AbstractFilter(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_DeleteTempFiles(true),
m_WriteSTLFile(true),
m_StlOutputDirectory(""),
m_StlFilePrefix(""),
m_VtkOutputFile(""),
m_WriteBinaryVTKFiles(false),
m_WriteConformalMesh(true),
neigh(NULL),
voxels(NULL),
cSquare(NULL),
cVertex(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LeeMarchingCubes::~LeeMarchingCubes()
{
  if (neigh) delete[] neigh;
  if (cSquare) delete[] cSquare;
  if (cVertex) delete[] cVertex;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LeeMarchingCubes::setupFilterParameters()
{

  std::vector<FilterParameter::Pointer> parameters;
#if 0
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("DREAM3D Input Data File");
    option->setPropertyName("InputFile");
    option->setWidgetType(FilterParameter::InputFileWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }

   This should be read from the file so we are NOT going to expose it to the user
  {
    ChoiceFilterParameter::Pointer option = ChoiceFilterParameter::New();
    option->setHumanLabel("Reference Frame");
    option->setPropertyName("RefFrameZDir");
    option->setWidgetType(FilterParameter::ChoiceWidget);
    option->setValueType("Ebsd::RefFrameZDir");
    option->setCastableValueType("unsigned int");
    std::vector<std::string> choices;
    choices.push_back("Low To High");
    choices.push_back("High To Low");
    option->setChoices(choices);
    parameters.push_back(option);
  }
#endif
   {
     FilterParameter::Pointer option = FilterParameter::New();
     option->setHumanLabel("Vtk PolyData Output File");
     option->setPropertyName("VtkOutputFile");
     option->setWidgetType(FilterParameter::OutputFileWidget);
     option->setValueType("string");
     parameters.push_back(option);
   }

   {
     FilterParameter::Pointer option = FilterParameter::New();
     option->setHumanLabel("Write STL Files");
     option->setPropertyName("WriteSTLFile");
     option->setWidgetType(FilterParameter::BooleanWidget);
     option->setValueType("bool");
     parameters.push_back(option);
   }
   {
     FilterParameter::Pointer option = FilterParameter::New();
     option->setHumanLabel("STL Output Directory");
     option->setPropertyName("StlOutputDirectory");
     option->setWidgetType(FilterParameter::OutputFileWidget);
     option->setValueType("string");
     parameters.push_back(option);
   }
   {
     FilterParameter::Pointer option = FilterParameter::New();
     option->setHumanLabel("STL Output Prefix");
     option->setPropertyName("StlFilePrefix");
     option->setWidgetType(FilterParameter::StringWidget);
     option->setValueType("string");
     parameters.push_back(option);
   }
   {
     FilterParameter::Pointer option = FilterParameter::New();
     option->setHumanLabel("Delete Temp Files");
     option->setPropertyName("DeleteTempFiles");
     option->setWidgetType(FilterParameter::BooleanWidget);
     option->setValueType("bool");
     parameters.push_back(option);
   }
   {
     FilterParameter::Pointer option = FilterParameter::New();
     option->setHumanLabel("Write Binary Vtk File");
     option->setPropertyName("WriteBinaryVTKFiles");
     option->setWidgetType(FilterParameter::BooleanWidget);
     option->setValueType("bool");
     parameters.push_back(option);
   }
   {
     FilterParameter::Pointer option = FilterParameter::New();
     option->setHumanLabel("Write Conformal Mesh");
     option->setPropertyName("WriteConformalMesh");
     option->setWidgetType(FilterParameter::BooleanWidget);
     option->setValueType("bool");
     parameters.push_back(option);
   }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LeeMarchingCubes::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("DeleteTempFiles", getDeleteTempFiles() );
  writer->writeValue("WriteSTLFile", getWriteSTLFile() );
  writer->writeValue("StlOutputDirectory", getStlOutputDirectory() );
  writer->writeValue("StlFilePrefix", getStlFilePrefix() );
  writer->writeValue("VtkOutputFile", getVtkOutputFile() );
  writer->writeValue("WriteBinaryVTKFiles", getWriteBinaryVTKFiles() );
  writer->writeValue("WriteConformalMesh", getWriteConformalMesh() );

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LeeMarchingCubes::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -300, int32_t, Int32ArrayType, voxels, 1)

  if (true == m_WriteSTLFile )
  {
    if (true == m_StlOutputDirectory.empty())
    {
      ss.str(""); ss << getHumanLabel() << " needs the STL Output directory set";
      setErrorCondition(-387);
      addErrorMessage(getHumanLabel(), ss.str(), -387);
    }
    if (true == m_StlFilePrefix.empty())
    {
      ss.str(""); ss << getHumanLabel() << " needs the STL File Prefix set";
      setErrorCondition(-388);
      addErrorMessage(getHumanLabel(), ss.str(), -388);
    }
  }
  if (true == m_VtkOutputFile.empty())
  {
    ss.str(""); ss << getHumanLabel() << " needs the VTK output file set";
    setErrorCondition(-387);
    addErrorMessage(getHumanLabel(), ss.str(), -387);
  }



}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LeeMarchingCubes::preflight()
{
  dataCheck(true, 1, 1, 1);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LeeMarchingCubes::execute()
{
  setErrorCondition(0);
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  int64_t totalPoints = m->getTotalPoints();
  dataCheck(false, totalPoints, m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  int err = 0;
  std::stringstream ss;


  std::string NodesFile = MXADir::tempPath() + Detail::NodesFile;
  m_NodesFile = SMTempFile::New();
  m_NodesFile->setFilePath(NodesFile);
  m_NodesFile->setAutoDelete(this->m_DeleteTempFiles);

  std::string TrianglesFile = MXADir::tempPath() + Detail::TrianglesFile;
  m_TrianglesFile = SMTempFile::New();
  m_TrianglesFile->setFilePath(TrianglesFile);
  m_TrianglesFile->setAutoDelete(this->m_DeleteTempFiles);

//  MAKE_OUTPUT_FILE_PATH( VisualizationFile, DREAM3D::SurfaceMesh::VisualizationVizFile)

  // Initialize some benchmark timers
  START_CLOCK()

  // Create the output directory if needed
  if(m_WriteSTLFile == true && m_StlOutputDirectory.empty() == false)
  {
    if (MXADir::exists(m_StlOutputDirectory) == false)
    {
      if (MXADir::mkdir(m_StlOutputDirectory, true) == false)
      {
        ss.str("");
        ss << "Error creating path '" << m_StlOutputDirectory << "'";
        notifyErrorMessage(ss.str(), -1);
        setErrorCondition(-1);
        return;
      }
    }
  }

  int cNodeID = 0;
  int cTriID = 0;
  int nTriangle = 0; // number of triangles...
  int nEdge = 0; // number of edges...
  int nNodes = 0; // number of total meshing::Nodes used...

  GrainChecker::Pointer m_GrainChecker = GrainChecker::New();

  size_t dims[3];
  float scaling[3];
  float origin[3];

  m->getDimensions(dims);
  m->getResolution(scaling);
  m->getOrigin(origin);

  // Initialize our LeeMarchingCubes Variable
  // Add a layer of padding around the volume which are going to be our boundary voxels
  xDim = dims[0] + 2;
  yDim = dims[1] + 2;
  zDim = dims[2] + 2;

  NS = xDim * yDim * zDim;
  NSP = xDim * yDim;

  DataArray<int32_t>::Pointer voxelsPtr = DataArray<int32_t>::CreateArray( 2 * NSP + 1, 1, "LeeMarchingCubes_Working_Voxels");
  voxels = voxelsPtr->GetPointer(0);

  neigh = new meshing::Neighbor[2 * NSP + 1];
  cSquare = new meshing::Face[3 * 2 * NSP];
  cVertex = new meshing::Node[2 * 7 * NSP];

  xOrigin = 0.0f;
  yOrigin = 0.0f;
  zOrigin = 0.0f;

  xRes = scaling[0];
  yRes = scaling[1];
  zRes = scaling[2];

  // Prime the working voxels (2 layers worth) with -3 values indicating border voxels
  int size = 2 * NSP + 1;
  for (int i = 0; i < size; ++i)
  {
    voxels[i] = -3;
  }

  std::map<int, meshing::SMStlWriter::Pointer> gidToSTLWriter;

  // Save the actual volume dimensions from the input file
//  int xFileDim = dims[0];
//  int yFileDim = dims[1];
//  int zFileDim = dims[2];
//  size_t totalBytes = dims[0] * yFileDim * sizeof(int32_t);

//  DataArray<int>::Pointer m_FileVoxelLayer = DataArray<int>::CreateArray(0, "FileVoxelLayer");
//  int* fileVoxelLayer = m_FileVoxelLayer->WritePointer(0, totalBytes);
  size_t offset = 0;

  for (size_t i = 0; i < dims[2]; i++)
  {
    ss.str("");
    ss << " Layers " << i << " and " << i + 1 << " of " << dims[2];
    notifyProgressValue((i * 90 / dims[2]));
    notifyStatusMessage(ss.str());

    // Get a pointer into the GrainIds Array at the appropriate offset
    int32_t* fileVoxelLayer = m_GrainIds + (i * dims[0] * dims[1]);
    if (getCancel() == true)
    {
        ss.str("");
        ss << "Cancelling filter";
        notifyErrorMessage(ss.str(), -1);
        setErrorCondition(-1);
        break;
    }
    // Copy the Voxels from layer 2 to Layer 1;
    ::memcpy(&(voxels[1]), &(voxels[1 + NSP]), NSP * sizeof(int));

    // now splice the data into the 2nd z layer for our marching cubes remembering
    // that we have a layer of border voxels.
    int* vxPtr = voxels;
    vxPtr = voxels + 1; // Pointer to start of layer 2 - Should be 4 bytes farther in memory because of junk voxel at voxel[0]
    int* fVxPtr = fileVoxelLayer; // Pointer to actual Grain Ids
    for (size_t y = 0; y < dims[1]; ++y)
    {
      // Get the offset into the data just read from the file
      fVxPtr = fileVoxelLayer + (y * dims[0]);
      // Get the offset into the second layer remembering the border voxel and
      // the fact that we do not use voxel[0] for anything.
      offset = ((y + 1) * xDim) + 1 + (NSP + 1);
      // Use a straight memory copy to move the values from the temp array into the
      // array used for the meshing
      vxPtr = voxels + offset;

      ::memcpy((void*)vxPtr, (void*)fVxPtr, dims[0] * sizeof(int));
    }
#if 0
    int32_t maxGrainId =0;
    for (size_t a = 0; a < 2 * NSP + 1; ++a)
    {
      if (voxels[a] > maxGrainId) {
      maxGrainId = voxels[a];
      std::cout << "here" << std::endl;
      }
    }

    std::cout << "maxGainId for z slice: " << i << " = " << maxGrainId << std::endl;
#endif
    get_neighbor_list();
    initialize_nodes(i);
    initialize_squares(i);

    // find meshing::Face edges of each square of marching cubes in each layer...
    nEdge = get_nodes_Edges(edgeTable_2d, nsTable_2d, i);

    // find triangles and arrange the spins across each triangle...
    nTriangle = get_triangles();
    arrange_grainnames(nTriangle, i);

    // assign new, cumulative meshing::Node id...
    nNodes = assign_nodeID(cNodeID);
//    std::cout << "LeeMarchingCubes nNodes: " << nNodes << std::endl;

    analyzeWinding();
    eMap.clear();
    labelTriangleMap.clear();

    // std::cout << "nNodes: " << nNodes << std::endl;
    // Output meshing::Nodes and triangles...
    err = writeNodesFile(i, cNodeID, NodesFile);
    if (err < 0)
    {
        ss.str("");
        ss << "Error writing meshing::Nodes file '" << NodesFile << "'";
        notifyErrorMessage(ss.str(), -1);
        setErrorCondition(-1);
        return;
    }

    err = writeTrianglesFile(i, cTriID, TrianglesFile, nTriangle);
    if (err < 0)
    {
        ss.str("");
        ss << "Error writing triangles file '" << TrianglesFile << "'";
        notifyErrorMessage(ss.str(), -1);
        setErrorCondition(-1);
        return;
    }

    if (m_WriteSTLFile == true)
    {
      m_GrainChecker->addData(nTriangle, cTriID, cTriangle, cVertex);
      writeSTLFiles(nTriangle, gidToSTLWriter);
    }
    cNodeID = nNodes;
    cTriID = cTriID + nTriangle;
    if (nTriangle > 0)
    {
      cTriangle.clear();
    }
  }

  //  CHECK_FOR_CANCELED(LeeMarchingCubes, "Surface Mesh was canceled", readHyperSlab);

  ss.str("");
  ss << "Marching Cubes Between Layers " << dims[2] - 1 << " and " << dims[2] << " of " << dims[2];
  notifyProgressValue( (dims[2] * 90 / dims[2]));
  notifyStatusMessage(ss.str());

  // ---------------------------------------------------------------
  // Run one more with the top layer being -3
  ::memcpy(&(voxels[1]), &(voxels[1 + NSP]), NSP * sizeof(int));

  //Make this last layer all border values
  for (int i = NSP; i < 2 * NSP + 1; ++i)
  {
    voxels[i] = -3;
  }

  int i = dims[2];
  get_neighbor_list();
  initialize_nodes(i);
  initialize_squares(i);
  // find meshing::Face edges of each square of marching cubes in each layer...
  get_nodes_Edges(edgeTable_2d, nsTable_2d, i);
  // find triangles and arrange the spins across each triangle...
  if (nTriangle > 0)
  {
    nTriangle = get_triangles();
    arrange_grainnames(nTriangle, i);
  }
  // assign new, cumulative meshing::Node id...
  nNodes = assign_nodeID(cNodeID);
//  std::cout << "LeeMarchingCubes nNodes: " << nNodes << std::endl;

  analyzeWinding();
  eMap.clear();
  labelTriangleMap.clear();


  // std::cout << "nNodes: " << nNodes << std::endl;
  // Output meshing::Nodes and triangles...
  err = writeNodesFile(i, cNodeID, NodesFile);
  if (err < 0)
  {
        ss.str("");
        ss << "Error writing meshing::Nodes file '" << NodesFile << "'";
        notifyErrorMessage(ss.str(), -1);
        setErrorCondition(-1);
        return;
  }

  err = writeTrianglesFile(i, cTriID, TrianglesFile, nTriangle);
    if (err < 0)
    {
        ss.str("");
        ss << "Error writing triangles file '" << TrianglesFile << "'";
        notifyErrorMessage(ss.str(), -1);
        setErrorCondition(-1);
        return;
    }

  // Write the last layers of the STL Files
  if (m_WriteSTLFile == true)
  {
    m_GrainChecker->addData(nTriangle, cTriID, cTriangle, cVertex);
    err |= writeSTLFiles(nTriangle, gidToSTLWriter);
    for (std::map<int, meshing::SMStlWriter::Pointer>::iterator iter = gidToSTLWriter.begin(); iter != gidToSTLWriter.end(); ++iter )
    {
      err |= (*iter).second->writeNumTrianglesToFile();
    }
        if (err < 0)
    {
        ss.str("");
        ss << "Error writing STL file";
        notifyErrorMessage(ss.str(), -1);
        setErrorCondition(-1);
        return;
    }
  }

  m_GrainChecker->analyzeGrains();

  cNodeID = nNodes;
  cTriID = cTriID + nTriangle;

  //std::cout << "Total Number of Triangles Created: " << cTriID << std::endl;

  if (nTriangle > 0)
  {
    cTriangle.clear();
  }

  SMVtkPolyDataWriter::Pointer writer = SMVtkPolyDataWriter::New();
  writer->setOutputVtkFile(m_VtkOutputFile);
  writer->setNodesFile(NodesFile);
  writer->setTrianglesFile(TrianglesFile);
  writer->setWriteBinaryFile(m_WriteBinaryVTKFiles);
  writer->setWriteConformalMesh(m_WriteConformalMesh);
  writer->execute();
  setErrorCondition(writer->getErrorCondition());


  // This will possibly delete the triangles and meshing::Nodes file depending on the
  // DeleteTempFiles setting
  m_TrianglesFile = SMTempFile::NullPointer();
  m_NodesFile = SMTempFile::NullPointer();


  setErrorCondition(0);
  notifyStatusMessage("Surface Meshing Complete");
}


void LeeMarchingCubes::get_neighbor_list()
{
  // NSP = number of sites in a plane of xDim by yDim...
  // neigh[][] = 2 dimeNSional array storing its site number and meshing::Neighbors...
  // site_id = id number for each site...starting from 1 to xDim*yDim*zDim....
  //
  // I assumed the square lattice...so the order of meshing::Neighbors as follows...
  //
  //    4   3   2         13  12  11          22  21  20
  //    5 site  1         14   9  10          23  18  19
  //    6   7   8         15  16  17          24  25  26
  //
  //    in-plane          upper plane         lower plane
  int ii; // loop index...
  int i, j, k, r; // position indices...
  int site_id; // id number for each site...
  for (ii = 1; ii <= 2 * NSP; ii++)
  {
    site_id = ii;
    neigh[ii].csiteid = site_id;
    k = (site_id - 1) / NSP;
    k = k * NSP;
    r = (site_id - 1) % NSP;
    j = r / xDim;
    j = j * xDim;
    i = r % xDim + 1;
    //same plane...
    neigh[ii].neigh_id[1] = k + j + i % xDim + 1;
    neigh[ii].neigh_id[2] = k + (j - xDim + NSP) % NSP + i % xDim + 1;
    neigh[ii].neigh_id[3] = k + (j - xDim + NSP) % NSP + i;
    neigh[ii].neigh_id[4] = k + (j - xDim + NSP) % NSP + (i - 2 + xDim) % xDim + 1;
    neigh[ii].neigh_id[5] = k + j + (i - 2 + xDim) % xDim + 1;
    neigh[ii].neigh_id[6] = k + (j + xDim) % NSP + (i - 2 + xDim) % xDim + 1;
    neigh[ii].neigh_id[7] = k + (j + xDim) % NSP + i;
    neigh[ii].neigh_id[8] = k + (j + xDim) % NSP + i % xDim + 1;
    //upper plane...
    //move the plane up and use the same scheme...
    neigh[ii].neigh_id[9] = (k - NSP + NS) % NS + j + i;
    neigh[ii].neigh_id[10] = (k - NSP + NS) % NS + j + i % xDim + 1;
    neigh[ii].neigh_id[11] = (k - NSP + NS) % NS + (j - xDim + NSP) % NSP + i % xDim + 1;
    neigh[ii].neigh_id[12] = (k - NSP + NS) % NS + (j - xDim + NSP) % NSP + i;
    neigh[ii].neigh_id[13] = (k - NSP + NS) % NS + (j - xDim + NSP) % NSP + (i - 2 + xDim) % xDim + 1;
    neigh[ii].neigh_id[14] = (k - NSP + NS) % NS + j + (i - 2 + xDim) % xDim + 1;
    neigh[ii].neigh_id[15] = (k - NSP + NS) % NS + (j + xDim) % NSP + (i - 2 + xDim) % xDim + 1;
    neigh[ii].neigh_id[16] = (k - NSP + NS) % NS + (j + xDim) % NSP + i;
    neigh[ii].neigh_id[17] = (k - NSP + NS) % NS + (j + xDim) % NSP + i % xDim + 1;
    //lower plane...
    neigh[ii].neigh_id[18] = (k + NSP) % NS + j + i;
    neigh[ii].neigh_id[19] = (k + NSP) % NS + j + i % xDim + 1;
    neigh[ii].neigh_id[20] = (k + NSP) % NS + (j - xDim + NSP) % NSP + i % xDim + 1;
    neigh[ii].neigh_id[21] = (k + NSP) % NS + (j - xDim + NSP) % NSP + i;
    neigh[ii].neigh_id[22] = (k + NSP) % NS + (j - xDim + NSP) % NSP + (i - 2 + xDim) % xDim + 1;
    neigh[ii].neigh_id[23] = (k + NSP) % NS + j + (i - 2 + xDim) % xDim + 1;
    neigh[ii].neigh_id[24] = (k + NSP) % NS + (j + xDim) % NSP + (i - 2 + xDim) % xDim + 1;
    neigh[ii].neigh_id[25] = (k + NSP) % NS + (j + xDim) % NSP + i;
    neigh[ii].neigh_id[26] = (k + NSP) % NS + (j + xDim) % NSP + i % xDim + 1;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float LeeMarchingCubes::find_xcoord(int index)
{
  index = index - 1;
  float x = xRes * float(index % xDim);
  return x;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float LeeMarchingCubes::find_ycoord(int index)
{
  index = index - 1;
  float y = yRes * float((index / xDim) % yDim);
  return y;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float LeeMarchingCubes::find_zcoord(int index)
{
  index = index - 1;
  float z = zRes * float(index / (xDim * yDim));
  return z;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LeeMarchingCubes::initialize_nodes(int zID)
{

  // Finds the coordinates of meshing::Nodes...
  int i, j;
  int id, oid;
  int tsite, locale;
  float x, y, z;
  int start = NSP + 1;
  if (zID == 0)
  {
   start = 1;
   numgrains = 0;
   }
  // meshing::Node id starts with 0....
  if (zID > 0)
  {
    for (i = 1; i <= NSP; i++)
    {
      id = 7 * (i - 1);
      oid = 7 * ((i + NSP) - 1);
      for (j = 0; j < 7; j++)
      {
        cVertex[id + j] = cVertex[oid + j];
      }
    }
  }
  int shift = (zID * NSP);
  for (i = start; i <= 2 * NSP; i++)
  {
    id = 7 * (i - 1);
    tsite = i;
    locale = tsite + shift;
    x = find_xcoord(locale);
    y = find_ycoord(locale);
    z = find_zcoord(locale);
    int grainid = voxels[tsite];
    if (grainid > numgrains) numgrains = grainid;
    cVertex[id].coord[0] = x + (0.5f * xRes);
    cVertex[id].coord[1] = y;
    cVertex[id].coord[2] = z;
    cVertex[id].nodeKind = 0;
    cVertex[id].NodeID = -1;
    cVertex[id + 1].coord[0] = x;
    cVertex[id + 1].coord[1] = y + (0.5f * yRes);
    cVertex[id + 1].coord[2] = z;
    cVertex[id + 1].nodeKind = 0;
    cVertex[id + 1].NodeID = -1;
    cVertex[id + 2].coord[0] = x;
    cVertex[id + 2].coord[1] = y;
    cVertex[id + 2].coord[2] = z + (0.5f * zRes);
    cVertex[id + 2].nodeKind = 0;
    cVertex[id + 2].NodeID = -1;
    cVertex[id + 3].coord[0] = x + (0.5f * xRes);
    cVertex[id + 3].coord[1] = y + (0.5f * yRes);
    cVertex[id + 3].coord[2] = z;
    cVertex[id + 3].nodeKind = 0;
    cVertex[id + 3].NodeID = -1;
    cVertex[id + 4].coord[0] = x + (0.5f * xRes);
    cVertex[id + 4].coord[1] = y;
    cVertex[id + 4].coord[2] = z + (0.5f * zRes);
    cVertex[id + 4].nodeKind = 0;
    cVertex[id + 4].NodeID = -1;
    cVertex[id + 5].coord[0] = x;
    cVertex[id + 5].coord[1] = y + (0.5f * yRes);
    cVertex[id + 5].coord[2] = z + (0.5f * zRes);
    cVertex[id + 5].nodeKind = 0;
    cVertex[id + 5].NodeID = -1;
    cVertex[id + 6].coord[0] = x + (0.5f * xRes);
    cVertex[id + 6].coord[1] = y + (0.5f * yRes);
    cVertex[id + 6].coord[2] = z + (0.5f * zRes);
    cVertex[id + 6].nodeKind = 0;
    cVertex[id + 6].NodeID = -1;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LeeMarchingCubes::initialize_squares(int zID)
{

  // Gather initial information on each square...
  int id;
  int i, j;
  int csite;
  // square id starts with 0....
  // notice that point at the surface will have the wrong values of meshing::Node at the other end...
  // since it includes periodic boundary condition...
  // but, since the structure surrounded by ghost layer of grainname -3, it's OK...
  for (i = 1; i <= 2 * NSP; i++)
  {
    id = 3 * (i - 1);
    csite = i;
    // store 4 corners of square...
    cSquare[id].site_id[0] = csite;
    cSquare[id].site_id[1] = neigh[i].neigh_id[1];
    cSquare[id].site_id[2] = neigh[i].neigh_id[8];
    cSquare[id].site_id[3] = neigh[i].neigh_id[7];
    cSquare[id + 1].site_id[0] = csite;
    cSquare[id + 1].site_id[1] = neigh[i].neigh_id[1];
    cSquare[id + 1].site_id[2] = neigh[i].neigh_id[19];
    cSquare[id + 1].site_id[3] = neigh[i].neigh_id[18];
    cSquare[id + 2].site_id[0] = neigh[i].neigh_id[7];
    cSquare[id + 2].site_id[1] = csite;
    cSquare[id + 2].site_id[2] = neigh[i].neigh_id[18];
    cSquare[id + 2].site_id[3] = neigh[i].neigh_id[25];
    // initialize meshing::Node, edge...-1 is dummy initial value...
    for (j = 0; j < 4; j++)
    {
      cSquare[id].edge_id[j] = -1;
      cSquare[id + 1].edge_id[j] = -1;
      cSquare[id + 2].edge_id[j] = -1;
    }
    for (int j = 0; j < 3; j++)
    {
      cSquare[id + j].nEdge = 0;
      cSquare[id + j].FCnode = -1;
      cSquare[id + j].effect = 0;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t LeeMarchingCubes::get_nodes_Edges(int eT2d[20][8], int NST2d[20][8], int zID)
{
  int j, k, m, ii;
  int tsite;
  int cubeOrigin; // stores the site id of 0th corner of each cube...
  int sqOrder; // stores the 0th, 1st and 2nd square...
  int tNSite[4];
  int tnSpin[4];
  int atBulk; // if it's 0, the square is at Bulk...
  int quot, rmd, rmd1;
  int sqIndex;
  int anFlag;
  int NodeIndex[2];
  int tnode;
  int tn1, tn2;
  int tnk;
  int pixIndex[2];
  int eid; // edge id...
//  int tnode1, tnode2;
  int edgeCount; // number of edges for each square...
  int NodeID[2];
  int pixgrainname[2];
  eid = 0;
  for (k = 0; k < 3 * 2 * NSP; k++)
  { // for each square...
    quot = k / (3 * NSP);
    rmd = k % (3 * NSP);
    rmd1 = rmd % 3;
    if (quot == 0 || (quot == 1 && rmd1 == 0))
    {
      cubeOrigin = k / 3 + 1;
      sqOrder = k % 3;
   //   csite = cSquare[k].site_id[0];
  //    cgrainname = voxels[csite];
      tNSite[0] = cSquare[k].site_id[0];
      tNSite[1] = cSquare[k].site_id[1];
      tNSite[2] = cSquare[k].site_id[2];
      tNSite[3] = cSquare[k].site_id[3];
      atBulk = 0;
      for (m = 0; m < 4; m++)
      {
        tsite = tNSite[m];
        tnSpin[m] = voxels[tsite];
        if (tnSpin[m] < 0) atBulk++;
      }
      edgeCount = 0;
      // Let's find the edges...
      if (atBulk != 4)
      { // coNSider the square inside the bulk only...
        cSquare[k].effect = 1;
        sqIndex = get_square_index(tnSpin);
        if (sqIndex == 15)
        {
          anFlag = treat_anomaly(tNSite, zID);
          sqIndex = sqIndex + anFlag;
        }
        if (sqIndex != 0)
        {
          for (j = 0; j < 8; j = j + 2)
          {
            if (eT2d[sqIndex][j] != -1)
            {
              NodeIndex[0] = eT2d[sqIndex][j];
              NodeIndex[1] = eT2d[sqIndex][j + 1];
              pixIndex[0] = NST2d[sqIndex][j];
              pixIndex[1] = NST2d[sqIndex][j + 1];
              get_nodes(cubeOrigin, sqOrder, NodeIndex, NodeID);
              get_grainnames(cubeOrigin, sqOrder, pixIndex, pixgrainname);
              if (pixgrainname[0] > 0 || pixgrainname[1] > 0)
              {
                cEdge.resize(eid + 1);
                cEdge[eid] = meshing::Segment::New();
                cEdge[eid]->node_id[0] = NodeID[0]; // actual meshing::Node ids for each edge...
                cEdge[eid]->node_id[1] = NodeID[1];
                cEdge[eid]->neigh_spin[0] = pixgrainname[0];
                cEdge[eid]->neigh_spin[1] = pixgrainname[1];
                cEdge[eid]->edgeKind = 2; // edges of the open loops are always binary...
                // triple lines only occurs inside the marching cubes...
                cSquare[k].edge_id[edgeCount] = eid;
                edgeCount++;
                eid++;
              }
              else
              {
                tn1 = NodeID[0];
                tn2 = NodeID[1];
                cVertex[tn1].nodeKind = -1; // extra meshing::Nodes from meshing the surface of the box...
                cVertex[tn2].nodeKind = -1; // we don't need them...
              }
              // Categorize the meshing::Node...if it's triple junction or not...
              for (ii = 0; ii < 2; ii++)
              {
                if (NodeIndex[ii] == 4)
                {
                  if (sqIndex == 7 || sqIndex == 11 || sqIndex == 13 || sqIndex == 14)
                  {
                    tnode = NodeID[ii];
                    cVertex[tnode].nodeKind = 3;
                    cSquare[k].FCnode = tnode;
                  }
                  else if (sqIndex == 19)
                  {
                    tnode = NodeID[ii];
                    cVertex[tnode].nodeKind = 4;
                    cSquare[k].FCnode = tnode;
                  }
                }
                else
                {
                  tnode = NodeID[ii];
                  tnk = cVertex[tnode].nodeKind;
                  if (tnk != -1)
                  {
                    cVertex[tnode].nodeKind = 2;
                  }
                }
              }
            }
          }
        }
      }
      cSquare[k].nEdge = edgeCount;
    }
  }
//  std::cout << "cEdge.size() " << cEdge.size() << std::endl;
  return (cEdge.size());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LeeMarchingCubes::get_square_index(int tNS[4])
{
  // identify each square configuration using binary bit...
  // returNS the unique decimal integer for each configuration...
  int aBit[6];
  int tempIndex;
  int subIndex;
  if (tNS[0] == tNS[1]) aBit[0] = 0;
  else aBit[0] = 1;
  if (tNS[1] == tNS[2]) aBit[1] = 0;
  else aBit[1] = 1;
  if (tNS[2] == tNS[3]) aBit[2] = 0;
  else aBit[2] = 1;
  if (tNS[3] == tNS[0]) aBit[3] = 0;
  else aBit[3] = 1;
  if (tNS[0] == tNS[2]) aBit[4] = 0;
  else aBit[4] = 1;
  if (tNS[1] == tNS[3]) aBit[5] = 0;
  else aBit[5] = 1;
  tempIndex = 8 * aBit[3] + 4 * aBit[2] + 2 * aBit[1] + 1 * aBit[0];
  if (tempIndex == 15)
  {
    subIndex = 2 * aBit[4] + 1 * aBit[5];
    if (subIndex != 0)
    {
      tempIndex = tempIndex + subIndex + 1;
    }
  }
  return tempIndex;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LeeMarchingCubes::treat_anomaly(int tNSt[4], int zID1)
{
  int i, j, k, ii;
  int csite, cgrainname;
  int NSite, nSpin;
  int temp;
  int tempFlag;
  int min, minid;
  int numNeigh[4];
  min = 1000;
  minid = -1;
  for (k = 0; k < 4; k++)
  {
    numNeigh[k] = 0;
  }
  for (i = 0; i < 4; i++)
  {
    csite = tNSt[i];
    cgrainname = voxels[csite];
//    for (j = 1; j <= num_neigh; j++)
    for (j = 1; j <= 8; j++)
    {
      NSite = neigh[csite].neigh_id[j];
      nSpin = voxels[NSite];
      if (cgrainname == nSpin)
      {
        numNeigh[i] = numNeigh[i] + 1;
      }
    }
  }
  for (ii = 0; ii < 4; ii++)
  {
    temp = numNeigh[ii];
    if (temp < min)
    {
      min = temp;
      minid = ii;
    }
  }
  if (minid == 1 || minid == 3)
  { // we can reverse this...let's try this later...
    tempFlag = 0;
  }
  else
  {
    tempFlag = 1;
  }
  return tempFlag;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LeeMarchingCubes::get_nodes(int cst, int ord, int nidx[2], int *nid)
{
  int ii;
  int tempIndex;
  for (ii = 0; ii < 2; ii++)
  {
    tempIndex = nidx[ii];
    if (ord == 0)
    { // if the square index is 0 for corner site...
      switch(tempIndex)
      {
        case 0:
          nid[ii] = 7 * (cst - 1);
          break;
        case 1:
          nid[ii] = 7 * cst + 1;
          break;
        case 2:
          nid[ii] = 7 * (cst + xDim - 1);
          break;
        case 3:
          nid[ii] = 7 * (cst - 1) + 1;
          break;
        case 4:
          nid[ii] = 7 * (cst - 1) + 3;
          break;
      }
    }
    else if (ord == 1)
    { // if the square index is 1...
      switch(tempIndex)
      {
        case 0:
          nid[ii] = 7 * (cst - 1);
          break;
        case 1:
          nid[ii] = 7 * cst + 2;
          break;
        case 2:
          nid[ii] = 7 * (cst + NSP - 1);
          break;
        case 3:
          nid[ii] = 7 * (cst - 1) + 2;
          break;
        case 4:
          nid[ii] = 7 * (cst - 1) + 4;
          break;
      }
    }
    else
    { // if the square index is 2...
      switch(tempIndex)
      {
        case 0:
          nid[ii] = 7 * (cst - 1) + 1;
          break;
        case 1:
          nid[ii] = 7 * (cst - 1) + 2;
          break;
        case 2:
          nid[ii] = 7 * (cst + NSP - 1) + 1;
          break;
        case 3:
          nid[ii] = 7 * (cst + xDim - 1) + 2;
          break;
        case 4:
          nid[ii] = 7 * (cst - 1) + 5;
          break;
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LeeMarchingCubes::get_grainnames(int cst, int ord, int pID[2], int *pgrainname)
{
  int i;
  int pixTemp, tempgrainname;
  for (i = 0; i < 2; i++)
  {
    pixTemp = pID[i];
    if (ord == 0)
    {
      switch(pixTemp)
      {
        case 0:
          tempgrainname = voxels[cst];
          pgrainname[i] = tempgrainname;
          break;
        case 1:
          tempgrainname = voxels[cst + 1];
          pgrainname[i] = tempgrainname;
          break;
        case 2:
          tempgrainname = voxels[cst + xDim + 1];
          pgrainname[i] = tempgrainname;
          break;
        case 3:
          tempgrainname = voxels[cst + xDim];
          pgrainname[i] = tempgrainname;
          break;
      }
    }
    else if (ord == 1)
    {
      switch(pixTemp)
      {
        case 0:
          tempgrainname = voxels[cst];
          pgrainname[i] = tempgrainname;
          break;
        case 1:
          tempgrainname = voxels[cst + 1];
          pgrainname[i] = tempgrainname;
          break;
        case 2:
          tempgrainname = voxels[cst + NSP + 1];
          pgrainname[i] = tempgrainname;
          break;
        case 3:
          tempgrainname = voxels[cst + NSP];
          pgrainname[i] = tempgrainname;
          break;
      }
    }
    else if (ord == 2)
    {
      switch(pixTemp)
      {
        case 0:
          tempgrainname = voxels[cst + xDim];
          pgrainname[i] = tempgrainname;
          break;
        case 1:
          tempgrainname = voxels[cst];
          pgrainname[i] = tempgrainname;
          break;
        case 2:
          tempgrainname = voxels[cst + NSP];
          pgrainname[i] = tempgrainname;
          break;
        case 3:
          tempgrainname = voxels[cst + NSP + xDim];
          pgrainname[i] = tempgrainname;
          break;
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LeeMarchingCubes::get_triangles()
{
  int i, ii, i1, i2;
  int sqID[6];
  int tsq; // current sq id...
  int tnE; // temp number of edges...
  int nFC; // number of FC turned on...
  int nE; // number of meshing::Face edges...
  int eff; // all the squares effective?...
  int cubeFlag; // if 1, we can do marching cube; if 0, useless...
  int BCnode; // cube center meshing::Node...
  int nkFlag;
  int tsqid1, tsqid2;
  int *arrayE;
  int fcid;
  int tindex;
  int nds, nburnt;
  int tsite1, tsite2;
  int tgrainname1, tgrainname2;
  int cgrainname, nSpin;
  int arraygrainname[8];
  int tidIn, tidOut;
  int arrayFC[6];
  tidIn = 0;
  for (i = 1; i <= NSP; i++)
  {
    cubeFlag = 0;
    // for each marching cube, collect meshing::Faces...
    sqID[0] = 3 * (i - 1);
    sqID[1] = 3 * (i - 1) + 1;
    sqID[2] = 3 * (i - 1) + 2;
    sqID[3] = 3 * i + 2;
    sqID[4] = 3 * (i + xDim - 1) + 1;
    sqID[5] = 3 * (i + NSP - 1);
    BCnode = 7 * (i - 1) + 6;
    nFC = 0;
    nE = 0;
    eff = 0;
    //initialize meshing::Face center array...
    for (ii = 0; ii < 6; ii++)
    {
      arrayFC[ii] = -1;
    }
    // Count the number of meshing::Face center turned on and meshing::Face edges...
    fcid = 0;
    ii = 0;
    for (ii = 0; ii < 6; ii++)
    {
      tsq = sqID[ii];
      if (cSquare[tsq].FCnode != -1)
      {
        arrayFC[fcid] = cSquare[tsq].FCnode;
        fcid++;
      }
//      nFC = nFC + 1;
      nE = nE + cSquare[tsq].nEdge;
      eff = eff + cSquare[tsq].effect;
    }
    nFC = fcid;
    if (eff > 0) cubeFlag = 1;
    if (nFC >= 3)
    {
      // If number of meshing::Face centers turned on is more than 2...
      // let's update the meshing::NodeKind of body center meshing::Node...
      tsqid1 = sqID[0];
      tsqid2 = sqID[5];
      nkFlag = 0;
      // get spin numbers for 8 corners for each marching cube...
      for (int j = 0; j < 4; j++)
      {
        tsite1 = cSquare[tsqid1].site_id[j];
        tsite2 = cSquare[tsqid2].site_id[j];
        tgrainname1 = voxels[tsite1];
        tgrainname2 = voxels[tsite2];
        arraygrainname[j] = tgrainname1;
        arraygrainname[j + 4] = tgrainname2;
        if (tgrainname1 < 0 || tgrainname2 < 0) nkFlag++;
      }
      nds = 0; // number of different spins in each marching cube...
      nburnt = 0; // so nds = meshing::NodeKind of body center position...
      for (int k = 0; k < 8; k++)
      {
        // arraySpin contains no -1 before any of it is burnt...
        cgrainname = arraygrainname[k];
        if (cgrainname != -1)
        {
          nds++;
          arraygrainname[k] = -1; // burn...
          nburnt++;
          for (int kk = 0; kk < 8; kk++)
          {
            nSpin = arraygrainname[kk];
            if (cgrainname == nSpin)
            {
              arraygrainname[kk] = -1; //burn...
              nburnt++;
            }
          }
        }
      }
      // update meshing::NodeKind of body center meshing::Node in the current marching cube...
      if (nkFlag > 0)
      {
        cVertex[BCnode].nodeKind = nds;
      }
      else
      {
        cVertex[BCnode].nodeKind = nds;
      }
    }
    // Checking the number of edges for loops in the cube...
    // if the current marching cube is a collection of 6 effective squares...and
    // the number of meshing::Face edges at least 3...
    // when nE==2, it doen't happen
    // when nE==1, the edge will contribute for the meshing::Neighboring marching cube...
    // when nE==0, it meaNS the cube is inside a grain...
    if (cubeFlag == 1 && nE > 2)
    {
      // Make edge array for each marching cube...
      arrayE = new int[nE];
      tindex = 0;
      for (i1 = 0; i1 < 6; i1++)
      {
        tsq = sqID[i1];
        tnE = cSquare[tsq].nEdge;
        if (tnE != 0)
        {
          for (i2 = 0; i2 < tnE; i2++)
          {
            arrayE[tindex] = cSquare[tsq].edge_id[i2];
            tindex++;
          }
        }
      }
      // CoNSider each case as Z. Wu's paper...
      if (nFC == 0)
      { // when there's no meshing::Face center
        get_case0_triangles(i, arrayE, nE, tidIn, &tidOut);
        tidIn = tidOut;
      }
      else if (nFC == 2)
      {
        get_case2_triangles(i, arrayE, nE, arrayFC, nFC, tidIn, &tidOut);
        tidIn = tidOut;
      }
      else if (nFC > 2 && nFC <= 6)
      {
        get_caseM_triangles(i, arrayE, nE, arrayFC, nFC, tidIn, &tidOut, BCnode);
        tidIn = tidOut;
      }
      delete[] arrayE;
    }
  }
  cEdge.clear();
  return (cTriangle.size());
}

#define ADD_TRIANGLE(cTriangle, ctid, n0, n1, n2, label0, label1)\
  cTriangle.resize(ctid + 1);\
  cTriangle[ctid] = meshing::Patch::New();\
  cTriangle[ctid]->node_id[0] = n0;\
  cTriangle[ctid]->node_id[1] = n1;\
  cTriangle[ctid]->node_id[2] = n2;\
  cTriangle[ctid]->nSpin[0] = label0;\
  cTriangle[ctid]->nSpin[1] = label1;\
  cTriangle[ctid]->tIndex = ctid;\
  meshing::SharedEdge::Pointer e0 = meshing::SharedEdge::New(cTriangle[ctid]->node_id[0], cTriangle[ctid]->node_id[1]);\
  meshing::SharedEdge::Pointer e = eMap[e0->getId()];\
  if (NULL == e.get()) { eMap[e0->getId()] = e0; }else{ e0 = e; }\
  e0->triangles.insert(ctid);\
  meshing::SharedEdge::Pointer e1 = meshing::SharedEdge::New(cTriangle[ctid]->node_id[1], cTriangle[ctid]->node_id[2]);\
  e = eMap[e1->getId()];\
  if (NULL == e.get()) { eMap[e1->getId()] = e1; }else{ e1 = e;}\
  e1->triangles.insert(ctid);\
  meshing::SharedEdge::Pointer e2 = meshing::SharedEdge::New(cTriangle[ctid]->node_id[2], cTriangle[ctid]->node_id[0]);\
  e = eMap[e2->getId()]; \
  if (NULL == e.get()){ eMap[e2->getId()] = e2; }else{ e2 = e; }\
  e2->triangles.insert(ctid);\
  cTriangle[ctid]->edges[0] = e0;  cTriangle[ctid]->edges[1] = e1;  cTriangle[ctid]->edges[2] = e2;\
  labelTriangleMap[label0] = ctid;\
  labelTriangleMap[label1] = ctid;


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LeeMarchingCubes::get_case0_triangles(int site, int *ae, int nedge, int tin, int *tout)
{
  int ii, i, j, jj, k, kk, k1, mm;
  int loopID;
  int tail, head, coin;
  int ce, ne;
  int cgrainname1, cgrainname2, cnode1, cnode2;
  int nSpin1, nSpin2, nnode1, nnode2;
  //int nucleus;
  int chaser;
  int grainnameFlag, NodeFlag, flip;
  int bflag, nbflag;
  int *burnt;
  int *burnt_list;
  int *count;
  int numN, sumN;
  int from;
  int *loop;
  int ctid;
  int front, back;
  int te0, te1, te2, tv0, tcVertex, tv2;
  int numT, cnumT, new_node0;
  burnt = new int[nedge];
  burnt_list = new int[nedge];
  // initialize burn flags for meshing::Face edges...
  for (ii = 0; ii < nedge; ii++)
  {
    burnt[ii] = 0;
    burnt_list[ii] = -1;
  }
  loopID = 1;
  tail = 0;
  head = 0;
  for (i = 0; i < nedge; i++)
  {
    ce = ae[i];
    bflag = burnt[i];
    if (bflag == 0)
    {
    //  nucleus = ce;
      burnt[i] = loopID;
      burnt_list[tail] = ce;
      coin = 1;
      while (coin)
      {
        chaser = burnt_list[tail];
        cgrainname1 = cEdge[chaser]->neigh_spin[0];
        cgrainname2 = cEdge[chaser]->neigh_spin[1];
        cnode1 = cEdge[chaser]->node_id[0];
        cnode2 = cEdge[chaser]->node_id[1];
        for (j = 0; j < nedge; j++)
        {
          ne = ae[j];
          nbflag = burnt[j];
          if (nbflag == 0)
          {
            nSpin1 = cEdge[ne]->neigh_spin[0];
            nSpin2 = cEdge[ne]->neigh_spin[1];
            nnode1 = cEdge[ne]->node_id[0];
            nnode2 = cEdge[ne]->node_id[1];
            // checking if meshing::Neighbor edge has same meshing::Neighboring grainnames...
            if (((cgrainname1 == nSpin1) && (cgrainname2 == nSpin2)) || ((cgrainname1 == nSpin2) && (cgrainname2 == nSpin1)))
            {
              grainnameFlag = 1;
            }
            else
            {
              grainnameFlag = 0;
            }
            // checking if neighor egde is contiguous...
            if ((cnode2 == nnode1) && (cnode1 != nnode2))
            {
              NodeFlag = 1;
              flip = 0;
            }
            else if ((cnode2 == nnode2) && (cnode1 != nnode1))
            {
              NodeFlag = 1;
              flip = 1;
            }
            else
            {
              NodeFlag = 0;
              flip = 0;
            }
            if (grainnameFlag == 1 && NodeFlag == 1)
            {
              head = head + 1;
              burnt_list[head] = ne;
              burnt[j] = loopID;
              if (flip == 1)
              {
                cEdge[ne]->neigh_spin[0] = nSpin2;
                cEdge[ne]->neigh_spin[1] = nSpin1;
                cEdge[ne]->node_id[0] = nnode2;
                cEdge[ne]->node_id[1] = nnode1;
              }
            }
          }
        }
        if (tail == head)
        {
          coin = 0;
          tail = tail + 1;
          head = tail;
          loopID++;
        }
        else
        {
          tail = tail + 1;
          coin = 1;
        }
      }
    }
  }
  count = new int[loopID];
  for (k1 = 1; k1 < loopID; k1++)
  {
    count[k1] = 0;
  }
  for (k = 1; k < loopID; k++)
  {
    for (kk = 0; kk < nedge; kk++)
    {
      if (k == burnt[kk])
      {
        count[k] = count[k] + 1;
      }
    }
  }
  // Let's do triangulation...
  sumN = 0;
  ctid = tin;
  for (jj = 1; jj < loopID; jj++)
  {
    numN = count[jj];
    sumN = sumN + numN;
    from = sumN - numN;
    loop = new int[numN];
    for (mm = 0; mm < numN; mm++)
    {
      loop[mm] = burnt_list[from + mm];
    }
    if (numN == 3)
    {
      te0 = loop[0];
      te1 = loop[1];
      te2 = loop[2];

      ADD_TRIANGLE(cTriangle, ctid, cEdge[te0]->node_id[0], cEdge[te1]->node_id[0], cEdge[te2]->node_id[0], cEdge[te0]->neigh_spin[0], cEdge[te0]->neigh_spin[1] )
      ctid++;
    }
    else if (numN > 3)
    {
      numT = numN - 2;
      cnumT = 0;
      front = 0;
      back = numN - 1;
      te0 = loop[front];
      te1 = loop[back];
      tv0 = cEdge[te0]->node_id[0];
      tcVertex = cEdge[te0]->node_id[1];
      tv2 = cEdge[te1]->node_id[0];
      ADD_TRIANGLE(cTriangle, ctid, tv0, tcVertex, tv2, cEdge[te0]->neigh_spin[0], cEdge[te0]->neigh_spin[1] )

      new_node0 = tv2;
    //  new_node1 = tcVertex;
      cnumT++;
      ctid++;
      while (cnumT < numT)
      {
        if ((cnumT % 2) != 0)
        {
          front = front + 1;
          ce = loop[front];
          tv0 = cEdge[ce]->node_id[0];
          tcVertex = cEdge[ce]->node_id[1];
          tv2 = new_node0;
          ADD_TRIANGLE(cTriangle, ctid, tv0, tcVertex, tv2, cEdge[ce]->neigh_spin[0], cEdge[ce]->neigh_spin[1] )

          new_node0 = tcVertex;
          cnumT++;
          ctid++;
        }
        else
        {
          back = back - 1;
          ce = loop[back];
          tv0 = cEdge[ce]->node_id[0];
          tcVertex = cEdge[ce]->node_id[1];
          tv2 = new_node0;
          ADD_TRIANGLE(cTriangle, ctid, tv0, tcVertex, tv2, cEdge[ce]->neigh_spin[0], cEdge[ce]->neigh_spin[1] )

          new_node0 = tv0;
          cnumT++;
          ctid++;
        }
      }
    }
    delete[] loop;
  }
  *tout = ctid;
  delete[] burnt;
  delete[] burnt_list;
  delete[] count;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LeeMarchingCubes::get_case2_triangles(int site, int *ae, int nedge, int *afc, int nfctr, int tin, int *tout)
{
  int ii, i, j, k, kk, k1, n, i1, j1;
  int loopID;
  int tail, head, coin;
  int ce, ne;
  int cgrainname1, cgrainname2, cnode1, cnode2;
  int nSpin1, nSpin2, nnode1, nnode2;
  int tgrainname, tnode;
  int nucleus;
  int chaser;
  int start;
  int end;
  int from, to;
  int grainnameFlag, NodeFlag, flip;
  int bflag, nbflag;
  int *burnt;
  int *burnt_list;
  int *count;
  int numN;
  int *burnt_loop;
  int openL; // if a loop is an open loop, it's 1; if closed, it's 0...
  int startEdge;
  int endNode;
  int index;
  int ctid;
  int front, back;
  int te0, te1, te2, tv0, tcVertex, tv2;
  int numT, cnumT, new_node0;
  burnt = new int[nedge];
  burnt_list = new int[nedge];
  // initialize burn flags for meshing::Face edges...
  for (ii = 0; ii < nedge; ii++)
  {
    burnt[ii] = 0;
    burnt_list[ii] = -1;
  }
  loopID = 1;
  tail = 0;
  head = 0;
  for (i = 0; i < nedge; i++)
  {
    ce = ae[i];
    bflag = burnt[i];
    if (bflag == 0)
    {
      nucleus = ce;
      burnt[i] = loopID;
      burnt_list[tail] = ce;
      coin = 1;
      while (coin)
      {
        chaser = burnt_list[tail];
        cgrainname1 = cEdge[chaser]->neigh_spin[0];
        cgrainname2 = cEdge[chaser]->neigh_spin[1];
        cnode1 = cEdge[chaser]->node_id[0];
        cnode2 = cEdge[chaser]->node_id[1];
        for (j = 0; j < nedge; j++)
        {
          ne = ae[j];
          nbflag = burnt[j];
          if (nbflag == 0)
          {
            nSpin1 = cEdge[ne]->neigh_spin[0];
            nSpin2 = cEdge[ne]->neigh_spin[1];
            nnode1 = cEdge[ne]->node_id[0];
            nnode2 = cEdge[ne]->node_id[1];
            // checking if meshing::Neighbor edge has same meshing::Neighboring grainnames...
            if (((cgrainname1 == nSpin1) && (cgrainname2 == nSpin2)) || ((cgrainname1 == nSpin2) && (cgrainname2 == nSpin1)))
            {
              grainnameFlag = 1;
            }
            else
            {
              grainnameFlag = 0;
            }
            // checking if neighor egde is contiguous...
            if ((cnode1 == nnode1) && (cnode2 != nnode2))
            {
              NodeFlag = 1;
            }
            else if ((cnode1 == nnode2) && (cnode2 != nnode1))
            {
              NodeFlag = 1;
            }
            else if ((cnode2 == nnode1) && (cnode1 != nnode2))
            {
              NodeFlag = 1;
            }
            else if ((cnode2 == nnode2) && (cnode1 != nnode1))
            {
              NodeFlag = 1;
            }
            else
            {
              NodeFlag = 0;
            }
            if (grainnameFlag == 1 && NodeFlag == 1)
            {
              head = head + 1;
              burnt_list[head] = ne;
              burnt[j] = loopID;
            }
          }
        }
        if (tail == head)
        {
          coin = 0;
          tail = tail + 1;
          head = tail;
          loopID++;
        }
        else
        {
          tail = tail + 1;
          coin = 1;
        }
      }
    }
  }
  count = new int[loopID];
  for (k1 = 1; k1 < loopID; k1++)
  {
    count[k1] = 0;
  }
  for (k = 1; k < loopID; k++)
  {
    for (kk = 0; kk < nedge; kk++)
    {
      if (k == burnt[kk])
      {
        count[k] = count[k] + 1;
      }
    }
  }
  // Let's make complete loops...
//  numTri = 0;
  start = afc[0];
  end = afc[1];
  to = 0;
  from = 0;
  j1 = 1;
  // Let's do triangulation...
  ctid = tin;
  for (j1 = 1; j1 < loopID; j1++)
  {
    openL = 0; // current loop is closed...
    numN = count[j1];
    to = to + numN;
    from = to - numN;
    burnt_loop = new int[numN];
    for (i1 = from; i1 < to; i1++)
    {
      ce = burnt_list[i1];
      cnode1 = cEdge[ce]->node_id[0];
      cnode2 = cEdge[ce]->node_id[1];
      if (start == cnode1)
      {
        openL = 1; // loop is open...
        startEdge = ce;
        flip = 0;
      }
      else if (start == cnode2)
      {
        openL = 1;
        startEdge = ce;
        flip = 1;
      }
    }
    if (openL == 1)
    { // if current loop is an open loop...
      if (flip == 1)
      {
        tnode = cEdge[startEdge]->node_id[0];
        tgrainname = cEdge[startEdge]->neigh_spin[0];
        cEdge[startEdge]->node_id[0] = cEdge[startEdge]->node_id[1];
        cEdge[startEdge]->node_id[1] = tnode;
        cEdge[startEdge]->neigh_spin[0] = cEdge[startEdge]->neigh_spin[1];
        cEdge[startEdge]->neigh_spin[1] = tgrainname;
      }
      burnt_loop[0] = startEdge;
      index = 1;
      endNode = cEdge[startEdge]->node_id[1];
      chaser = startEdge;
      while (index < numN)
      {
        for (n = from; n < to; n++)
        {
          ce = burnt_list[n];
          cnode1 = cEdge[ce]->node_id[0];
          cnode2 = cEdge[ce]->node_id[1];
          if ((ce != chaser) && (endNode == cnode1))
          {
            burnt_loop[index] = ce;
            index++;
          }
          else if ((ce != chaser) && (endNode == cnode2))
          {
            burnt_loop[index] = ce;
            index++;
            // flip...
            tnode = cEdge[ce]->node_id[0];
            tgrainname = cEdge[ce]->neigh_spin[0];
            cEdge[ce]->node_id[0] = cEdge[ce]->node_id[1];
            cEdge[ce]->node_id[1] = tnode;
            cEdge[ce]->neigh_spin[0] = cEdge[ce]->neigh_spin[1];
            cEdge[ce]->neigh_spin[1] = tgrainname;
          }
        }
        chaser = burnt_loop[index - 1];
        endNode = cEdge[chaser]->node_id[1];
      }
      if (numN == 2)
      {
        te0 = burnt_loop[0];
        te1 = burnt_loop[1];
        ADD_TRIANGLE(cTriangle, ctid, cEdge[te0]->node_id[0], cEdge[te1]->node_id[0], cEdge[te1]->node_id[1], cEdge[te0]->neigh_spin[0], cEdge[te0]->neigh_spin[1] )

        ctid++;
      }
      else if (numN > 2)
      {
        numT = numN - 1; // Note that it's open loop...
        cnumT = 0;
        front = 0;
        back = numN;
        te0 = burnt_loop[front];
        te1 = burnt_loop[back - 1];
        tv0 = cEdge[te0]->node_id[0];
        tcVertex = cEdge[te0]->node_id[1];
        tv2 = cEdge[te1]->node_id[1];
        ADD_TRIANGLE(cTriangle, ctid, tv0, tcVertex, tv2, cEdge[te0]->neigh_spin[0], cEdge[te0]->neigh_spin[1] )

        new_node0 = tv2;
    //    new_node1 = tcVertex;
        cnumT++;
        ctid++;
        while (cnumT < numT)
        {
          if ((cnumT % 2) != 0)
          {
            front = front + 1;
            ce = burnt_loop[front];
            tv0 = cEdge[ce]->node_id[0];
            tcVertex = cEdge[ce]->node_id[1];
            tv2 = new_node0;
            ADD_TRIANGLE(cTriangle, ctid, tv0, tcVertex, tv2, cEdge[ce]->neigh_spin[0], cEdge[ce]->neigh_spin[1] )

            new_node0 = tcVertex;
            cnumT++;
            ctid++;
          }
          else
          {
            back = back - 1;
            ce = burnt_loop[back];
            tv0 = cEdge[ce]->node_id[0];
            tcVertex = cEdge[ce]->node_id[1];
            tv2 = new_node0;
            ADD_TRIANGLE(cTriangle, ctid, tv0, tcVertex, tv2, cEdge[ce]->neigh_spin[0], cEdge[ce]->neigh_spin[1] )

            new_node0 = tv0;
            cnumT++;
            ctid++;
          }
        }
      }
      delete[] burnt_loop;
    }
    else
    { // if current loop is a closed one....i.e., openL = 0...
      startEdge = burnt_list[from];
      burnt_loop[0] = startEdge;
      index = 1;
      endNode = cEdge[startEdge]->node_id[1];
      chaser = startEdge;
      while (index < numN)
      {
        for (n = from; n < to; n++)
        {
          ce = burnt_list[n];
          cnode1 = cEdge[ce]->node_id[0];
          cnode2 = cEdge[ce]->node_id[1];
          if ((ce != chaser) && (endNode == cnode1))
          {
            burnt_loop[index] = ce;
            index++;
          }
          else if ((ce != chaser) && (endNode == cnode2))
          {
            burnt_loop[index] = ce;
            index++;
            // flip...
            tnode = cEdge[ce]->node_id[0];
            tgrainname = cEdge[ce]->neigh_spin[0];
            cEdge[ce]->node_id[0] = cEdge[ce]->node_id[1];
            cEdge[ce]->node_id[1] = tnode;
            cEdge[ce]->neigh_spin[0] = cEdge[ce]->neigh_spin[1];
            cEdge[ce]->neigh_spin[1] = tgrainname;
          }
        }
        chaser = burnt_loop[index - 1];
        endNode = cEdge[chaser]->node_id[1];
      }
      if (numN == 3)
      {
        te0 = burnt_loop[0];
        te1 = burnt_loop[1];
        te2 = burnt_loop[2];
        ADD_TRIANGLE(cTriangle, ctid, cEdge[te0]->node_id[0], cEdge[te1]->node_id[0], cEdge[te2]->node_id[0], cEdge[te0]->neigh_spin[0], cEdge[te0]->neigh_spin[1] )

        ctid++;
      }
      else if (numN > 3)
      {
        numT = numN - 2;
        cnumT = 0;
        front = 0;
        back = numN - 1;
        te0 = burnt_loop[front];
        te1 = burnt_loop[back];
        tv0 = cEdge[te0]->node_id[0];
        tcVertex = cEdge[te0]->node_id[1];
        tv2 = cEdge[te1]->node_id[0];
        ADD_TRIANGLE(cTriangle, ctid, tv0, tcVertex, tv2, cEdge[te0]->neigh_spin[0], cEdge[te0]->neigh_spin[1] )

        new_node0 = tv2;
      //  new_node1 = tcVertex;
        cnumT++;
        ctid++;
        while (cnumT < numT)
        {
          if ((cnumT % 2) != 0)
          {
            front = front + 1;
            ce = burnt_loop[front];
            tv0 = cEdge[ce]->node_id[0];
            tcVertex = cEdge[ce]->node_id[1];
            tv2 = new_node0;
            ADD_TRIANGLE(cTriangle, ctid, tv0, tcVertex, tv2, cEdge[ce]->neigh_spin[0], cEdge[ce]->neigh_spin[1] )

            new_node0 = tcVertex;
            cnumT++;
            ctid++;
          }
          else
          {
            back = back - 1;
            ce = burnt_loop[back];
            tv0 = cEdge[ce]->node_id[0];
            tcVertex = cEdge[ce]->node_id[1];
            tv2 = new_node0;
            ADD_TRIANGLE(cTriangle, ctid, tv0, tcVertex, tv2, cEdge[ce]->neigh_spin[0], cEdge[ce]->neigh_spin[1] )

            new_node0 = tv0;
            cnumT++;
            ctid++;
          }
        }

      }
      delete[] burnt_loop;
    }
  }
  *tout = ctid;
  delete[] burnt;
  delete[] burnt_list;
  delete[] count;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LeeMarchingCubes::get_caseM_triangles(int site, int *ae, int nedge, int *afc, int nfctr, int tin, int *tout, int ccn)
{
  int ii, i, j, k, kk, k1, n, i1, j1, n1, iii;
  int loopID;
  int tail, head, coin;
  int ce, ne;
  int cgrainname1, cgrainname2, cnode1, cnode2;
  int nSpin1, nSpin2, nnode1, nnode2;
  int tgrainname, tnode;
  int nucleus, chaser;
  int start;
  int from, to;
  int grainnameFlag, NodeFlag, flip;
  int bflag, nbflag;
  int *burnt;
  int *burnt_list;
  int *count;
  int numN, numTri;
  int tn0, tn1;
  int *burnt_loop;
  int openL; // if a loop is an open loop, it's 1; if closed, it's 0...
  int startEdge;
  int endNode;
  int index;
  int ctid;
  int front, back;
  int te0, te1, te2, tv0, tcVertex, tv2, ts0, ts1;
  int numT, cnumT, new_node0, new_node1;
  burnt = new int[nedge];
  burnt_list = new int[nedge];
  // initialize burn flags for meshing::Face edges...
  for (ii = 0; ii < nedge; ii++)
  {
    burnt[ii] = 0;
    burnt_list[ii] = -1;
  }
  loopID = 1;
  tail = 0;
  head = 0;
  for (i = 0; i < nedge; i++)
  {
    ce = ae[i];
    bflag = burnt[i];
    if (bflag == 0)
    {
      nucleus = ce;
      burnt[i] = loopID;
      burnt_list[tail] = ce;
      coin = 1;
      while (coin)
      {
        chaser = burnt_list[tail];
        cgrainname1 = cEdge[chaser]->neigh_spin[0];
        cgrainname2 = cEdge[chaser]->neigh_spin[1];
        cnode1 = cEdge[chaser]->node_id[0];
        cnode2 = cEdge[chaser]->node_id[1];
        for (j = 0; j < nedge; j++)
        {
          ne = ae[j];
          nbflag = burnt[j];
          if (nbflag == 0)
          {
            nSpin1 = cEdge[ne]->neigh_spin[0];
            nSpin2 = cEdge[ne]->neigh_spin[1];
            nnode1 = cEdge[ne]->node_id[0];
            nnode2 = cEdge[ne]->node_id[1];
            // checking if meshing::Neighbor edge has same meshing::Neighboring grainnames...
            if (((cgrainname1 == nSpin1) && (cgrainname2 == nSpin2)) || ((cgrainname1 == nSpin2) && (cgrainname2 == nSpin1)))
            {
              grainnameFlag = 1;
            }
            else
            {
              grainnameFlag = 0;
            }
            // checking if neighor egde is contiguous...
            if ((cnode1 == nnode1) && (cnode2 != nnode2))
            {
              NodeFlag = 1;
            }
            else if ((cnode1 == nnode2) && (cnode2 != nnode1))
            {
              NodeFlag = 1;
            }
            else if ((cnode2 == nnode1) && (cnode1 != nnode2))
            {
              NodeFlag = 1;
            }
            else if ((cnode2 == nnode2) && (cnode1 != nnode1))
            {
              NodeFlag = 1;
            }
            else
            {
              NodeFlag = 0;
            }
            if (grainnameFlag == 1 && NodeFlag == 1)
            {
              head = head + 1;
              burnt_list[head] = ne;
              burnt[j] = loopID;
            }
          }
        }
        if (tail == head)
        {
          coin = 0;
          tail = tail + 1;
          head = tail;
          loopID++;
        }
        else
        {
          tail = tail + 1;
          coin = 1;
        }
      }
    }
  }
  count = new int[loopID];
  for (k1 = 1; k1 < loopID; k1++)
  {
    count[k1] = 0;
  }
  for (k = 1; k < loopID; k++)
  {
    for (kk = 0; kk < nedge; kk++)
    {
      if (k == burnt[kk])
      {
        count[k] = count[k] + 1;
      }
    }
  }
  // Let's make complete loops...
  numTri = 0;
  to = 0;
  from = 0;
  j1 = 1;
  ctid = tin;
  for (j1 = 1; j1 < loopID; j1++)
  {
    openL = 0; // current loop is closed...
    numN = count[j1];
    to = to + numN;
    from = to - numN;
    burnt_loop = new int[numN];
    for (i1 = from; i1 < to; i1++)
    {
      ce = burnt_list[i1];
      cnode1 = cEdge[ce]->node_id[0];
      cnode2 = cEdge[ce]->node_id[1];
      for (n1 = 0; n1 < nfctr; n1++)
      {
        start = afc[n1];
        if (start == cnode1)
        {
          openL = 1; // loop is open...
          startEdge = ce;
          flip = 0;
        }
        else if (start == cnode2)
        {
          openL = 1;
          startEdge = ce;
          flip = 1;
        }
      }
    }
    if (openL == 1)
    { // if current loop is an open loop...
      if (flip == 1)
      {
        tnode = cEdge[startEdge]->node_id[0];
        tgrainname = cEdge[startEdge]->neigh_spin[0];
        cEdge[startEdge]->node_id[0] = cEdge[startEdge]->node_id[1];
        cEdge[startEdge]->node_id[1] = tnode;
        cEdge[startEdge]->neigh_spin[0] = cEdge[startEdge]->neigh_spin[1];
        cEdge[startEdge]->neigh_spin[1] = tgrainname;
      }
      burnt_loop[0] = startEdge;
      index = 1;
      endNode = cEdge[startEdge]->node_id[1];
      chaser = startEdge;
      while (index < numN)
      {
        for (n = from; n < to; n++)
        {
          ce = burnt_list[n];
          cnode1 = cEdge[ce]->node_id[0];
          cnode2 = cEdge[ce]->node_id[1];
          if ((ce != chaser) && (endNode == cnode1))
          {
            burnt_loop[index] = ce;
            index++;
          }
          else if ((ce != chaser) && (endNode == cnode2))
          {
            burnt_loop[index] = ce;
            index++;
            // flip...
            tnode = cEdge[ce]->node_id[0];
            tgrainname = cEdge[ce]->neigh_spin[0];
            cEdge[ce]->node_id[0] = cEdge[ce]->node_id[1];
            cEdge[ce]->node_id[1] = tnode;
            cEdge[ce]->neigh_spin[0] = cEdge[ce]->neigh_spin[1];
            cEdge[ce]->neigh_spin[1] = tgrainname;
          }
        }
        chaser = burnt_loop[index - 1];
        endNode = cEdge[chaser]->node_id[1];
      }
      //triangulation...
      for (iii = 0; iii < numN; iii++)
      {
        ce = burnt_loop[iii];
        tn0 = cEdge[ce]->node_id[0];
        tn1 = cEdge[ce]->node_id[1];
        ts0 = cEdge[ce]->neigh_spin[0];
        ts1 = cEdge[ce]->neigh_spin[1];
        ADD_TRIANGLE(cTriangle, ctid, ccn, tn0, tn1, ts0, ts1)

        ctid++;
      }
      delete[] burnt_loop;
    }
    else
    { // if current loop is a closed one....i.e., openL = 0...
      startEdge = burnt_list[from];
      burnt_loop[0] = startEdge;
      index = 1;
      endNode = cEdge[startEdge]->node_id[1];
      chaser = startEdge;
      while (index < numN)
      {
        for (n = from; n < to; n++)
        {
          ce = burnt_list[n];
          cnode1 = cEdge[ce]->node_id[0];
          cnode2 = cEdge[ce]->node_id[1];
          if ((ce != chaser) && (endNode == cnode1))
          {
            burnt_loop[index] = ce;
            index++;
          }
          else if ((ce != chaser) && (endNode == cnode2))
          {
            burnt_loop[index] = ce;
            index++;
            // flip...
            tnode = cEdge[ce]->node_id[0];
            tgrainname = cEdge[ce]->neigh_spin[0];
            cEdge[ce]->node_id[0] = cEdge[ce]->node_id[1];
            cEdge[ce]->node_id[1] = tnode;
            cEdge[ce]->neigh_spin[0] = cEdge[ce]->neigh_spin[1];
            cEdge[ce]->neigh_spin[1] = tgrainname;
          }
        }
        chaser = burnt_loop[index - 1];
        endNode = cEdge[chaser]->node_id[1];
      }
      if (numN == 3)
      {
        te0 = burnt_loop[0];
        te1 = burnt_loop[1];
        te2 = burnt_loop[2];
        ADD_TRIANGLE(cTriangle, ctid, cEdge[te0]->node_id[0], cEdge[te1]->node_id[0], cEdge[te2]->node_id[0], cEdge[te0]->neigh_spin[0], cEdge[te0]->neigh_spin[1])

        ctid++;
      }
      else if (numN > 3)
      {
        numT = numN - 2;
        cnumT = 0;
        front = 0;
        back = numN - 1;
        te0 = burnt_loop[front];
        te1 = burnt_loop[back];
        tv0 = cEdge[te0]->node_id[0];
        tcVertex = cEdge[te0]->node_id[1];
        tv2 = cEdge[te1]->node_id[0];
        ADD_TRIANGLE(cTriangle, ctid, tv0, tcVertex, tv2, cEdge[te0]->neigh_spin[0], cEdge[te0]->neigh_spin[1])

        new_node0 = tv2;
        new_node1 = tcVertex;
        cnumT++;
        ctid++;
        while (cnumT < numT)
        {
          if ((cnumT % 2) != 0)
          {
            front = front + 1;
            ce = burnt_loop[front];
            tv0 = cEdge[ce]->node_id[0];
            tcVertex = cEdge[ce]->node_id[1];
            tv2 = new_node0;
            ADD_TRIANGLE(cTriangle, ctid, tv0, tcVertex, tv2, cEdge[ce]->neigh_spin[0], cEdge[ce]->neigh_spin[1])

            new_node0 = tcVertex;
            cnumT++;
            ctid++;
          }
          else
          {
            back = back - 1;
            ce = burnt_loop[back];
            tv0 = cEdge[ce]->node_id[0];
            tcVertex = cEdge[ce]->node_id[1];
            tv2 = new_node0;
            ADD_TRIANGLE(cTriangle, ctid, tv0, tcVertex, tv2, cEdge[ce]->neigh_spin[0], cEdge[ce]->neigh_spin[1])

            new_node0 = tv0;
            cnumT++;
            ctid++;
          }
        }
      }
      delete[] burnt_loop;
    }
  }
  *tout = ctid;
  delete[] burnt;
  delete[] burnt_list;
  delete[] count;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LeeMarchingCubes::arrange_grainnames(int numT, int zID)
{
  //int i, j;
  int cnode;
  int csite, kind;
    int tsite1[3] = { -1, -1, -1};
    int tsite2[3] = { -1, -1, -1};
//  int nSpin1, nSpin2;
  int tgrainname1[3], tgrainname2[3];
  float cx, cy, cz;
  float xSum, ySum, zSum;
  float vcoord[3][3];
  float u[3], w[3];
  float x, y, z;
  float a, b, c, d, length;
  float sidecheck;
  int shift = (zID * NSP);
  int locale;
  for (int i = 0; i < numT; i++)
  { // for each triangle...
    xSum = 0.0;
    ySum = 0.0;
    zSum = 0.0;
  //  nSpin1 = cTriangle[i]->nSpin[0];
  //  nSpin2 = cTriangle[i]->nSpin[1];
    cTriangle[i]->nSpin[0] = -1;
    cTriangle[i]->nSpin[1] = -1;
    for (int j = 0; j < 3; j++)
    { // for each meshing::Node inside the triangle...
      tsite1[j] = -1;
      tsite2[j] = -1;
      tgrainname1[j] = -1;
      tgrainname2[j] = -1;
      cnode = cTriangle[i]->node_id[j];
      csite = cnode / 7 + 1;
      kind = cnode % 7;
      xSum = xSum + cVertex[cnode].coord[0];
      ySum = ySum + cVertex[cnode].coord[1];
      zSum = zSum + cVertex[cnode].coord[2];
      vcoord[j][0] = cVertex[cnode].coord[0];
      vcoord[j][1] = cVertex[cnode].coord[1];
      vcoord[j][2] = cVertex[cnode].coord[2];
      if (kind == 0)
      {
        tsite1[j] = csite;
        tsite2[j] = neigh[csite].neigh_id[1];
        tgrainname1[j] = voxels[tsite1[j]];
        tgrainname2[j] = voxels[tsite2[j]];
      }
      else if (kind == 1)
      {
        tsite1[j] = csite;
        tsite2[j] = neigh[csite].neigh_id[7];
        tgrainname1[j] = voxels[tsite1[j]];
        tgrainname2[j] = voxels[tsite2[j]];
      }
      else if (kind == 2)
      {
        tsite1[j] = csite;
        tsite2[j] = neigh[csite].neigh_id[18];
        tgrainname1[j] = voxels[tsite1[j]];
        tgrainname2[j] = voxels[tsite2[j]];
      }
    }
    // Getting the center of triangle...
    cx = xSum / 3.0f;
    cy = ySum / 3.0f;
    cz = zSum / 3.0f;
    // Getting normal vector of the triangle...(Right-hand Rule!!!)
    // Getting 2 edge vectors of triangle originating from vertex 0...
    u[0] = vcoord[1][0] - vcoord[0][0];
    u[1] = vcoord[1][1] - vcoord[0][1];
    u[2] = vcoord[1][2] - vcoord[0][2];
    w[0] = vcoord[2][0] - vcoord[0][0];
    w[1] = vcoord[2][1] - vcoord[0][1];
    w[2] = vcoord[2][2] - vcoord[0][2];
    // Cross product, UxW = aX + bY + cZ...
    // Note that UxW follows the same direction of triangle loop...
    a = u[1] * w[2] - u[2] * w[1];
    b = u[2] * w[0] - u[0] * w[2];
    c = u[0] * w[1] - u[1] * w[0];
    length = sqrt(a * a + b * b + c * c);
    a = a / length;
    b = b / length;
    c = c / length;
    if (fabs(a) < 0.00001) a = 0.0;
    if (fabs(b) < 0.00001) b = 0.0;
    if (fabs(c) < 0.00001) c = 0.0;
    // update meshing::Patch info...
    cTriangle[i]->normal[0] = a;
    cTriangle[i]->normal[1] = b;
    cTriangle[i]->normal[2] = c;
    cTriangle[i]->area = 0.5 * length;
    // determine which way normal should point...
    d = -(a * cx + b * cy + c * cz);
    for (int j = 0; j < 3; j++)
    {
      if (tsite1[j] != -1)
      {
        locale = tsite1[j] + shift;
        x = find_xcoord(locale);
        y = find_ycoord(locale);
        z = find_zcoord(locale);
        sidecheck = (a * x + b * y + c * z + d);
        if (sidecheck < -0.000001)
        {
          cTriangle[i]->nSpin[0] = tgrainname2[j];
          cTriangle[i]->nSpin[1] = tgrainname1[j];
        }
        else if (sidecheck > 0.000001)
        {
          cTriangle[i]->nSpin[0] = tgrainname1[j];
          cTriangle[i]->nSpin[1] = tgrainname2[j];
        }
      }
    }
    int k = 0;
    int index = 0;
    int testtsite = 0;
    //int sidechecked = 1;
    while (cTriangle[i]->nSpin[0] == -1 && k < 6)
    {
      while (tsite1[k] == -1)
      {
        k++;
      }
      if (k == 3) a = -a, b = -b, c = -c;
      if (k < 3) index = k;
      if (k >= 3) index = k - 3;
      if (a != 0 && (tsite1[index] % xDim + a) >= 0 && (tsite1[index] % xDim + a) < xDim) testtsite = tsite1[index] + (a * 1);
      if (b != 0 && ((tsite1[index] / xDim) % yDim + b) >= 0 && ((tsite1[index] / xDim) % yDim + b) < yDim) testtsite = tsite1[index] + (b * xDim);
      if (c != 0 && (tsite1[index] / (xDim * yDim) + c) >= 0 && (tsite1[index] / (xDim * yDim) + c) < 2) testtsite = tsite1[index] + (c * NSP);
      int gname = voxels[testtsite];
      if (gname == tgrainname1[index])
      {
        if (k < 3) cTriangle[i]->nSpin[0] = tgrainname1[index], cTriangle[i]->nSpin[1] = tgrainname2[index];
        if (k >= 3) cTriangle[i]->nSpin[0] = tgrainname2[index], cTriangle[i]->nSpin[1] = tgrainname1[index];
      }
      if (gname == tgrainname2[index])
      {
        if (k < 3) cTriangle[i]->nSpin[0] = tgrainname2[index], cTriangle[i]->nSpin[1] = tgrainname1[index];
        if (k >= 3) cTriangle[i]->nSpin[0] = tgrainname1[index], cTriangle[i]->nSpin[1] = tgrainname2[index];
      }
      if (gname != tgrainname1[index] && gname != tgrainname2[index]) k++;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LeeMarchingCubes::assign_nodeID(int nN)
{
  int i;
  int nid;
  int nkind;
  int cnid;
 // int plane;
  nid = nN;
  for (i = 0; i < (7 * 2 * NSP); i++)
  {
    nkind = cVertex[i].nodeKind;
    cnid = cVertex[i].NodeID;
  //  plane = i % 7;
    if (nkind != 0 && cnid == -1)
    {
      cVertex[i].NodeID = nid;
      nid++;
    }
  }
  return (nid);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LeeMarchingCubes::writeNodesFile(int zID, int cNodeID, const std::string &nodesFile)
{
  static const size_t BYTE_COUNT = 20;
//  int count;
//  count = 0;

  unsigned char data[BYTE_COUNT];
  int* tId = (int*)(&data[0]);
  int* nk = (int*)(&data[4]);
  float* vec3f = (float*)(&data[8]);
  size_t totalWritten = 0;
  FILE* f = NULL;

//  std::cout << "LeeMarchingCubes writing Nodes file " << cNodeID << std::endl;
  // Create a new file if this is our first slice
  if (zID == 0)
  {
    f = fopen(nodesFile.c_str(), "wb");
    if (NULL == f)
    {
      return -1;
    }
  }
  // Append to existing file if we are on z>0 slice
  if (zID > 0)
  {
    f = fopen(nodesFile.c_str(), "ab");
    if (NULL == f)
    {
      return -1;
    }
  }
  int total = (7 * 2 * NSP);
  for (int k = 0; k < total; k++)
  {
    *tId = cVertex[k].NodeID;
    if (*tId > cNodeID - 1)
    {
      *nk = cVertex[k].nodeKind;
      vec3f[0] = cVertex[k].coord[0];
      vec3f[1] = cVertex[k].coord[1];
      vec3f[2] = cVertex[k].coord[2];
//      fprintf(f, "%d %d %f %f %f\n", tID, nk, x, y, z);
      totalWritten = fwrite(data, sizeof(unsigned char), BYTE_COUNT, f);
      if (totalWritten != BYTE_COUNT) {
        std::cout << "Not enough data written to the meshing::Nodes file." << std::endl;
        return -1;
      }
    }
  }
  fclose(f);
  return 0;
}

// -----------------------------------------------------------------------------
//  Write a BINARY file which is only TEMP during the surface meshing
// -----------------------------------------------------------------------------
int LeeMarchingCubes::writeTrianglesFile(int zID, int ctid, const std::string &trianglesFile, int nt)
{
  static const size_t DATA_COUNT = 9;
 // int tag;
  int end;
  int n1, n2, n3;
  int data[DATA_COUNT];

//  std::cout << "LeeMarchingCubes::Writing Triangles starting at " << ctid << std::endl;
  end = nt;
  data[0] = ctid;
  FILE* f = NULL;

  // Create a new file if this is our first slice
  if (zID == 0)
  {
    f = fopen(trianglesFile.c_str(), "wb");
    if (NULL == f)
    {
      return -1;
    }
#if (WRITE_BINARY_TEMP_FILES == 0)
      fprintf(f, "XXXXXXXXXXXX\n"); // This should get over written at some point with the actual number of triangles
#endif
  }
  // Append to existing file if we are on z>0 slice
  if (zID > 0)
  {
    f = fopen(trianglesFile.c_str(), "ab");
    if (NULL == f)
    {
      return -1;
    }
  }
  size_t totalWritten = 0;
  //  outFile << nt <<endl;
  for (int i = 0; i < end; i++)
  {

    n1 = cTriangle[i]->node_id[0];
    n2 = cTriangle[i]->node_id[1];
    n3 = cTriangle[i]->node_id[2];
#if WRITE_BINARY_TEMP_FILES
    data[1] = cVertex[n1].NodeID;
    data[2] = cVertex[n2].NodeID;
    data[3] = cVertex[n3].NodeID;
    data[4] = cTriangle[i]->edges[0]->getId();
    data[5] = cTriangle[i]->edges[1]->getId();
    data[6] = cTriangle[i]->edges[2]->getId();
    data[7] = cTriangle[i]->nSpin[0];
    data[8] = cTriangle[i]->nSpin[1];

    totalWritten = fwrite(data, sizeof(int), DATA_COUNT, f);
    if (totalWritten != DATA_COUNT)
    {
      std::cout << "Error Writing Triangles Temp File. Not enough elements written. Wrote " << totalWritten << " of 6." << std::endl;
      return -1;
    }
#else

    fprintf(f, "%d    %d    %d    %d    %llu    %llu    %llu    %d    %d\n", data[0], cVertex[n1].NodeID,cVertex[n2].NodeID,cVertex[n3].NodeID,
            cTriangle[i]->edges[0]->getId(), cTriangle[i]->edges[1]->getId(), cTriangle[i]->edges[2]->getId(),
            cTriangle[i]->nSpin[0], cTriangle[i]->nSpin[1]);
#endif
//    fprintf(f, "%d %d %d %d %d %d\n", newID, n1, n2, n3, s1, s2);
    data[0] = data[0] + 1;
  }
  // Go to beginning of file and update the number of triangles
  //fseek(f, 0, SEEK_SET);
  fclose(f);

#if (WRITE_BINARY_TEMP_FILES == 0)
  f = fopen(trianglesFile.c_str(), "rb+");
  std::cout << "data[0]: " << data[0] << std::endl;
  fprintf(f, "%012d\n", data[0]);
  fclose(f);
#endif

  return 0;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LeeMarchingCubes::writeSTLFiles(int nTriangle, std::map<int, meshing::SMStlWriter::Pointer> &gidToSTLWriter)
{
// First loop through All the triangles adding up how many triangles are
// in each grain and create STL Files for each Grain if needed
  std::map<int, int> grainIdMap;
  int g0, g1;
  int err =0;
  for (int i = 0; i < nTriangle; ++i)
  {
    g0 = cTriangle[i]->nSpin[0];
    g1 = cTriangle[i]->nSpin[1];
    if (gidToSTLWriter[g0].get() == NULL)
    {
      std::string stlFile;
      stlFile = m_StlOutputDirectory + MXADir::Separator + m_StlFilePrefix + "STL_Files/";
      MXADir::mkdir(stlFile, true);
      stlFile.append(m_StlFilePrefix).append(StringUtils::numToString(g0)).append(".stl");
      gidToSTLWriter[g0] = meshing::SMStlWriter::CreateNewSTLWriter(g0, stlFile);
    }
    if (gidToSTLWriter[g1].get() == NULL)
    {
      std::string stlFile = m_StlOutputDirectory + MXADir::Separator + m_StlFilePrefix + "STL_Files/";
      MXADir::mkdir(stlFile, true);
      stlFile.append(m_StlFilePrefix).append(StringUtils::numToString(g1)).append(".stl");
      gidToSTLWriter[g1] = meshing::SMStlWriter::CreateNewSTLWriter(g1, stlFile);
    }
    grainIdMap[cTriangle[i]->nSpin[0]]++;
    grainIdMap[cTriangle[i]->nSpin[1]]++;
  }

// Allocate all the new meshing::Patch Blocks
  std::map<int, meshing::Patch::ContainerType> gidToPatch;
  std::map<int, int> gidToCurIdx;
  for (std::map<int, int>::iterator iter = grainIdMap.begin(); iter != grainIdMap.end(); ++iter)
  {
    int gid = (*iter).first;
    int count = (*iter).second;
    gidToPatch[gid] = meshing::Patch::ContainerType(count);
    gidToCurIdx[gid] = 0;
  }

  int idx = 0;

// Loop over all the triangles and copy the meshing::Patch Pointers into our new Pointers
  for (int i = 0; i < nTriangle; ++i)
  {
    g0 = cTriangle[i]->nSpin[0];
    meshing::Patch::ContainerType& frontG0 = gidToPatch[g0];
    idx = gidToCurIdx[g0];
    frontG0[idx] = cTriangle[i];
    gidToCurIdx[g0]++;

    g1 = cTriangle[i]->nSpin[1];
    meshing::Patch::ContainerType& frontG1 = gidToPatch[g1];
    idx = gidToCurIdx[g1];
    frontG1[idx] = cTriangle[i];
    gidToCurIdx[g1]++;
  }

  for (std::map<int, meshing::Patch::ContainerType >::iterator iter = gidToPatch.begin(); iter != gidToPatch.end(); ++iter)
  {
    int gid = (*iter).first;
    meshing::Patch::ContainerType& cTriangle = (*iter).second;
    int nTriangle = grainIdMap[gid];

    meshing::SMStlWriter::Pointer writer = gidToSTLWriter[gid];
    if (NULL != writer.get())
    {
      err |= writer->writeTriangleBlock(nTriangle, cTriangle, cVertex);
    }
  }
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LeeMarchingCubes::analyzeWinding()
{
//  std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
//  std::cout << " Edge Count: " << eMap.size() << std::endl;
//  std::cout << " Triangle Count: " << cTriangle.size() << std::endl;
//  std::cout << "labelTriangleMap.size(): " << labelTriangleMap.size() << std::endl;

  float total = (float)(labelTriangleMap.size());
  // Keeps a list of all the triangles that have been visited.
  std::vector<bool> masterVisited(cTriangle.size(), false);

  // Start with first triangle in the master list:
  int masterTriangleIndex = 0;

  // Get the first 2 labels to visit from the first triangle. We use these 2 labels
  // because this triangle shares these 2 labels and guarantees these 2 labels are
  // connected to each other.
  float curPercent = 0.0;
  int progressIndex = 0;
  for (LabelTriangleMapType::iterator cLabel = labelTriangleMap.begin(); cLabel != labelTriangleMap.end(); ++cLabel )
  {

    int currentLabel = cLabel->first;
  //  if (currentLabel != 1) { continue; }
    masterTriangleIndex = cLabel->second;
    meshing::Patch::Pointer t = cTriangle[masterTriangleIndex];

    if ( (progressIndex/total * 100.0f) > (curPercent) )
    {
   //   std::cout << "Verifying Winding: " << curPercent << "% Complete" << std::endl;
      curPercent += 5.0f;
    }
    ++progressIndex;
  //  std::cout << "Current Label: " << currentLabel << std::endl;
  //  int seedTriIndex = masterTriangleIndex;

    if (NULL == t.get() )
    {
      std::cout << "Could not find a triangle with the winding set. This should NOT happen" << std::endl;
      assert(1 == 0);
    }

    std::set<int> localVisited; // Keep track of which triangles have been visited
    std::deque<int> triangleDeque;
    triangleDeque.push_back(t->tIndex);

    while (triangleDeque.empty() == false)
    {
      meshing::Patch::Pointer currentTri = cTriangle[triangleDeque.front()];
  //    std::cout << "tIndex = " << t->tIndex << std::endl;
      localVisited.insert(currentTri->tIndex);
      std::vector<int> adjTris = findAdjacentTriangles(currentTri, currentLabel);
      for ( std::vector<int>::iterator adjTri = adjTris.begin(); adjTri != adjTris.end(); ++adjTri )
      {
        //  std::cout << "  ^ AdjTri index: " << (*adjTri)->tIndex << std::endl;
        if (masterVisited[*adjTri] == false)
        {
          //   std::cout << "   * Checking Winding: " << (*adjTri)->tIndex << std::endl;
          meshing::Patch::Pointer triToVerify = cTriangle[*adjTri];
          currentTri->verifyWinding( triToVerify.get(), currentLabel);
        }

        if (localVisited.find(*adjTri) == localVisited.end()
          && find(triangleDeque.begin(), triangleDeque.end(), *adjTri) == triangleDeque.end())
        {
          // std::cout << "   # Adding to Deque: " << (*adjTri)->tIndex << std::endl;
          triangleDeque.push_back(*adjTri);
          localVisited.insert(*adjTri);
          masterVisited[*adjTri] = true;
        }
      }

      triangleDeque.pop_front();
    }

  }

 // std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<int> LeeMarchingCubes::findAdjacentTriangles(meshing::Patch::Pointer triangle, int label)
{
  std::vector<int> adjacentTris;
  typedef meshing::SharedEdge::Pointer EdgeType;
  // Get the 3 edges from the triangle


  // Iterate over the 3 Edges of the triangle
  for (int i = 0; i < 3; ++i)
  {
    // Get the current edge
    EdgeType e = triangle->edges[i];
    // Get the list of indices of the triangles that belong to that edge
    std::set<int> tIndices = e->triangles;
    // Iterate over the indices to find triangles that match the label and are NOT the current triangle index
    for (std::set<int>::iterator iter = tIndices.begin(); iter != tIndices.end(); ++iter )
    {
      meshing::Patch::Pointer t = cTriangle.at(*iter);
      if ( (t->nSpin[0] == label || t->nSpin[1] == label)
          && (t->tIndex != triangle->tIndex) )
      {
     //   std::cout << "    Found Adjacent Triangle: " << t->tIndex << std::endl;
        adjacentTris.push_back(t->tIndex);
      }
    }
  }

  return adjacentTris;

}
