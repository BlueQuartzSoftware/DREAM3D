/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "SurfaceMesh.h"

#include <vector>
#include <map>


#include "MXA/Common/LogTime.h"
#include "MXA/Common/MXAEndian.h"
#include "MXA/Utilities/MXADir.h"
#include "MXA/Utilities/MXAFileInfo.h"
#include "MXA/Utilities/StringUtils.h"

#include "SMVtkFileIO.h"
#include "DREAM3D/HDF5/H5VoxelReader.h"

 #include "Winding/SurfaceWinding.h"

#define CHECK_ERROR(name, message)\
    if(err < 0) {\
      setErrorCondition(err);\
      std::string msg = #name;\
      msg += message;\
      pipelineMessage(msg.c_str());\
      pipelineProgress(0);\
      pipelineFinished();\
      return;   }




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMesh::SurfaceMesh() :
    m_InputDirectory("."),
    m_InputFile(""),
    m_ScalarName(AIM::VTK::GrainIdScalarName),
    m_OutputDirectory(""),
    m_OutputFilePrefix("SurfaceMesh_"),
    m_ConformalMesh(true),
    m_BinaryVTKFile(false),
    m_WriteSTLFile(false),
    m_DeleteTempFiles(true),
    m_SmoothMesh(false),
    m_SmoothIterations(0),
    m_SmoothFileOutputIncrement(0),
    m_SmoothLockQuadPoints(false)
{
  m_GrainChecker = GrainChecker::New();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMesh::~SurfaceMesh()
{
}

#define USE_VTK_FILE_UTILS 1

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMesh::execute()
{

  updateProgressAndMessage(("Running Surface Meshing"), 0);
  int err = 0;
//  MAKE_OUTPUT_FILE_PATH (  NodesRawFile , AIM::SurfaceMeshing::NodesRawFile)
  MAKE_OUTPUT_FILE_PATH( NodesFile, AIM::SurfaceMesh::NodesFileBin)
  MAKE_OUTPUT_FILE_PATH( TrianglesFile, AIM::SurfaceMesh::TrianglesFileBin)
//  MAKE_OUTPUT_FILE_PATH (  EdgesFile , AIM::SurfaceMeshing::EdgesFile)
//  MAKE_OUTPUT_FILE_PATH (  EdgesFileIndex , AIM::SurfaceMeshing::EdgesFileIndex)
//  MAKE_OUTPUT_FILE_PATH (  TrianglesFileIndex , AIM::SurfaceMeshing::TrianglesFileIndex)
  MAKE_OUTPUT_FILE_PATH( VisualizationFile, AIM::SurfaceMesh::VisualizationVizFile)
//  MAKE_OUTPUT_FILE_PATH( stlFilename, AIM::SurfaceMesh::STLFile)

  m = SurfaceMeshFunc::New();
  // Initialize some benchmark timers
  START_CLOCK()

  // Create the output directory if needed
  if (MXADir::exists(m_OutputDirectory) == false)
  {
    if (MXADir::mkdir(m_OutputDirectory, true) == false)
    {
      setCancel(true);
      setErrorCondition(1);
      CHECK_FOR_CANCELED(SurfaceMeshFunc, "SurfaceMesh could not create the output directory", outputDirCreation)
      updateProgressAndMessage(("Could not create output directory."), 100);
      return;
    }
  }

  int cNodeID = 0;
  int cTriID = 0;
  int nTriangle = 0; // number of triangles...
  int nEdge = 0; // number of edges...
  int nNodes = 0; // number of total nodes used...
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

#if 0
  SMVtkFileIO::Pointer vtkreader = SMVtkFileIO::New();
  vtkreader->primeFileToScalarDataLocation(m.get(), m_InputFile, m_ScalarName);
#endif

  H5VoxelReader::Pointer reader = H5VoxelReader::New();
  reader->setFilename(m_InputFile);

  int dims[3];
  float scaling[3];
  err = reader->getSizeAndResolution(dims, scaling);
  // Initialize our SurfaceMeshFunc Variable
  // Add a layer of padding around the volume which are going to be our boundary voxels
  m->xDim = dims[0] + 2;
  m->yDim = dims[1] + 2;
  m->zDim = dims[2] + 2;

  m->NS = m->xDim * m->yDim * m->zDim;
  m->NSP = m->xDim * m->yDim;

  m->neigh = new Neighbor[2 * m->NSP + 1];
  m->voxels = new int[2 * m->NSP + 1];
  m->cSquare = new Face[3 * 2 * m->NSP];
  m->cVertex = new Node[2 * 7 * m->NSP];

  m->xOrigin = 0.0f;
  m->yOrigin = 0.0f;
  m->zOrigin = 0.0f;

  m->xRes = scaling[0];
  m->yRes = scaling[1];
  m->zRes = scaling[2];

  // Prime the working voxels (2 layers worth) with -3 values indicating border voxels
  int size = 2 * m->NSP + 1;
  for (int i = 0; i < size; ++i)
  {
    m->voxels[i] = -3;
  }

  std::map<int, STLWriter::Pointer> gidToSTLWriter;

  // Save the actual volume dimensions from the input file
  int xFileDim = m->xDim - 2;
  int yFileDim = m->yDim - 2;
  int zFileDim = m->zDim - 2;
  size_t totalBytes = xFileDim * yFileDim * sizeof(int);
  int* fileVoxelLayer = (int*)(malloc(totalBytes));
  size_t offset = 0;

  std::stringstream ss;
  for (int i = 0; i < zFileDim; i++)
  {
    ss.str("");
    ss << "Marching Cubes Between Layers " << i << " and " << i + 1 << " of " << zFileDim;
    updateProgressAndMessage((ss.str().c_str()), (i * 90 / zFileDim));

#if 0
    err = vtkreader->readZSlice(xFileDim, yFileDim, zFileDim, fileVoxelLayer);
#endif
    err = reader->readHyperSlab(xFileDim, yFileDim, i, fileVoxelLayer);
    CHECK_FOR_ERROR(SurfaceMeshFunc, "Error Loading Slice Data as a Hyperslab from HDF5 file", err)

    // Copy the Voxels from layer 2 to Layer 1;
    ::memcpy(&(m->voxels[1]), &(m->voxels[1 + m->NSP]), m->NSP * sizeof(int));

    // now splice the data into the 2nd z layer for our marching cubes remembering
    // that we have a layer of border voxels.
    int* vxPtr = m->voxels;
    vxPtr = m->voxels + 1; // Should be 4 bytes farther in memory
    int* fVxPtr = fileVoxelLayer;
    for (int y = 0; y < yFileDim; ++y)
    {
      // Get the offset into the data just read from the file
      fVxPtr = fileVoxelLayer + (y * xFileDim);
      // Get the offset into the second layer remembering the border voxel and
      // the fact that we do not use voxel[0] for anything.
      offset = ((y + 1) * m->xDim) + 1 + (m->NSP + 1);
      // Use a straight memory copy to move the values from the temp array into the
      // array used for the meshing
      vxPtr = m->voxels + offset;

      ::memcpy((void*)vxPtr, (void*)fVxPtr, xFileDim * sizeof(int));
    }

    m->get_neighbor_list();
    m->initialize_nodes(i);
    m->initialize_squares(i);

    // find face edges of each square of marching cubes in each layer...
    nEdge = m->get_nodes_Edges(edgeTable_2d, nsTable_2d, i);

    // find triangles and arrange the spins across each triangle...
    nTriangle = m->get_triangles();
    m->arrange_grainnames(nTriangle, i);

    // assign new, cumulative node id...
    nNodes = m->assign_nodeID(cNodeID);
    // std::cout << "nNodes: " << nNodes << std::endl;
    // Output nodes and triangles...
    err = m->writeNodesFile(i, cNodeID, NodesFile);
    if (err < 0)
    {
      setErrorCondition(-1);
      updateProgressAndMessage(("Error Writing Nodes Temp File"), 100);
      return;
    }
    err = m->writeTrianglesFile(i, cTriID, TrianglesFile, nTriangle);
    if (err < 0)
    {
      setErrorCondition(-1);
      updateProgressAndMessage(("Error Writing Triangles Temp File"), 100);
      return;
    }
    if (m_WriteSTLFile == true)
    {
      m_GrainChecker->addData(nTriangle, cTriID, &(m->cTriangle.front()), m->cVertex);
      writeSTLFiles(nTriangle, gidToSTLWriter);
    }
    cNodeID = nNodes;
    cTriID = cTriID + nTriangle;
    if (nTriangle > 0)
    {
      m->cTriangle.clear();
    }
  }
  ss.str("");
  ss << "Marching Cubes Between Layers " << zFileDim - 1 << " and " << zFileDim << " of " << zFileDim;
  updateProgressAndMessage((ss.str().c_str()), (zFileDim * 90 / zFileDim));
// ---------------------------------------------------------------
  // Run one more with the top layer being -3
  ::memcpy(&(m->voxels[1]), &(m->voxels[1 + m->NSP]), m->NSP * sizeof(int));

  //Make this last layer all border values
  for (int i = m->NSP; i < 2 * m->NSP + 1; ++i)
  {
    m->voxels[i] = -3;
  }

  int i = zFileDim;
  m->get_neighbor_list();
  m->initialize_nodes(i);
  m->initialize_squares(i);
  // find face edges of each square of marching cubes in each layer...
  m->get_nodes_Edges(edgeTable_2d, nsTable_2d, i);
  // find triangles and arrange the spins across each triangle...
  if (nTriangle > 0)
  {
    nTriangle = m->get_triangles();
    m->arrange_grainnames(nTriangle, i);
  }
  // assign new, cumulative node id...
  nNodes = m->assign_nodeID(cNodeID);
  // std::cout << "nNodes: " << nNodes << std::endl;
  // Output nodes and triangles...
  err = m->writeNodesFile(i, cNodeID, NodesFile);
  if (err < 0)
  {
    setErrorCondition(-1);
    updateProgressAndMessage(("Error Writing Nodes Temp File"), 100);
    return;
  }
  err = m->writeTrianglesFile(i, cTriID, TrianglesFile, nTriangle);
  if (err < 0)
  {
    setCancel(true);
    updateProgressAndMessage(("Error Writing Triangles Temp File"), 100);
    return;
  }

  // Write the last layers of the STL Files
  if (m_WriteSTLFile == true)
  {
    m_GrainChecker->addData(nTriangle, cTriID, &(m->cTriangle.front()), m->cVertex);
    writeSTLFiles(nTriangle, gidToSTLWriter);
    for (std::map<int, STLWriter::Pointer>::iterator iter = gidToSTLWriter.begin(); iter != gidToSTLWriter.end(); ++iter )
    {
      (*iter).second->writeNumTrianglesToFile();
    }
  }

  m_GrainChecker->analyzeGrains();

  cNodeID = nNodes;
  cTriID = cTriID + nTriangle;

  //std::cout << "Total Number of Triangles Created: " << cTriID << std::endl;

  if (nTriangle > 0)
  {
    m->cTriangle.clear();
  }

//------------ All Done with Marching Cubes-------------------
  free(fileVoxelLayer);
  fileVoxelLayer = NULL;

  if (m_SmoothMesh)
  {
    updateProgressAndMessage(("Smoothing Boundaries"), 90);
  }

  std::string msg("Writing Surface Mesh File: ");
  msg.append(AIM::SurfaceMesh::VisualizationVizFile);
  updateProgressAndMessage(msg.c_str(), 95);
  SMVtkFileIO::Pointer writer = SMVtkFileIO::New();
  writer->setInputFileName(VisualizationFile);
  writer->writeVTKFile(m.get(), nNodes, cTriID, VisualizationFile, NodesFile, TrianglesFile, m_BinaryVTKFile, m_ConformalMesh);



  m = SurfaceMeshFunc::NullPointer(); // Clean up the memory
  updateProgressAndMessage(("Analyzing Winding"), 95);
#if 1
  SurfaceWinding sw;
  sw.debugPrintConnectivity(nNodes, cTriID, TrianglesFile);
#endif

  updateProgressAndMessage(("Surface Meshing Complete"), 100);
  if (m_DeleteTempFiles == true)
  {
    // Delete the intermediate files
    MXADir::remove(NodesFile);
    MXADir::remove(TrianglesFile);
  }

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMesh::writeSTLFiles(int nTriangle, std::map<int, STLWriter::Pointer> &gidToSTLWriter)
{
// First loop through All the triangles adding up how many triangles are
// in each grain and create STL Files for each Grain if needed
  std::map<int, int> grainIdMap;
  int g0, g1;
  for (int i = 0; i < nTriangle; ++i)
  {
    g0 = m->cTriangle[i].ngrainname[0];
    g1 = m->cTriangle[i].ngrainname[1];
    if (gidToSTLWriter[g0].get() == NULL)
    {
      std::string stlFile = m_OutputDirectory + MXADir::Separator + m_OutputFilePrefix + "STLFiles/";
      MXADir::mkdir(stlFile, true);
      stlFile.append(StringUtils::numToString(g0)).append(".stl");
      gidToSTLWriter[g0] = STLWriter::CreateNewSTLWriter(g0, stlFile);
    }
    if (gidToSTLWriter[g1].get() == NULL)
    {
      std::string stlFile = m_OutputDirectory + MXADir::Separator + m_OutputFilePrefix + "STLFiles/";
      MXADir::mkdir(stlFile, true);
      stlFile.append(StringUtils::numToString(g1)).append(".stl");
      gidToSTLWriter[g1] = STLWriter::CreateNewSTLWriter(g1, stlFile);
    }
    grainIdMap[m->cTriangle[i].ngrainname[0]]++;
    grainIdMap[m->cTriangle[i].ngrainname[1]]++;
  }

// Allocate all the new Patch Blocks
  std::map<int, Patch*> gidToPatch;
  std::map<int, int> gidToCurIdx;
  for (std::map<int, int>::iterator iter = grainIdMap.begin(); iter != grainIdMap.end(); ++iter)
  {
    int gid = (*iter).first;
    int count = (*iter).second;
    gidToPatch[gid] = new Patch[count];
    gidToCurIdx[gid] = 0;
  }

  int idx = 0;
  Patch* front = NULL;
// Loop over all the triangles and copy the Patch Pointers into our new Pointers
  for (int i = 0; i < nTriangle; ++i)
  {
    g0 = m->cTriangle[i].ngrainname[0];
    front = gidToPatch[g0];
    idx = gidToCurIdx[g0];
    front[idx] = m->cTriangle[i];
    gidToCurIdx[g0]++;

    g1 = m->cTriangle[i].ngrainname[1];
    front = gidToPatch[g1];
    idx = gidToCurIdx[g1];
    front[idx] = m->cTriangle[i];
    gidToCurIdx[g1]++;
  }

  for (std::map<int, Patch*>::iterator iter = gidToPatch.begin(); iter != gidToPatch.end(); ++iter)
  {
    int gid = (*iter).first;
    Patch* cTriangle = (*iter).second;
    int nTriangle = grainIdMap[gid];

    STLWriter::Pointer writer = gidToSTLWriter[gid];
    if (NULL != writer.get())
    {
      writer->writeTriangleBlock(nTriangle, cTriangle, m->cVertex);
    }
    delete[] cTriangle; // Delete this block of Patch objects as we are done with it
  }

}
