/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "SurfaceMesh.h"


#include "DREAM3DLib/SurfaceMeshingFilters/Smoothing/Smoothing.h"
#include "DREAM3DLib/SurfaceMeshingFilters/SMVtkPolyDataWriter.h"
#include "DREAM3DLib/SurfaceMeshingFilters/SMTempFile.hpp"

#define USE_WINDING 0

#define CHECK_ERROR(name, message)\
    if(err < 0) {\
      setErrorCondition(err);\
      std::string msg = #name;\
      msg += message;\
      pipelineMessage(msg.c_str());\
      pipelineProgress(0);\
      pipelineFinished();\
      return;   }


#define USE_VTK_FILE_UTILS 1

#define MAKE_OUTPUT_FILE_PATH(outpath, filename)\
    std::string outpath = m_OutputDirectory + MXADir::Separator + m_OutputFilePrefix + filename;




using namespace meshing;


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMesh::SurfaceMesh() :
m_InputFile(""),
m_WriteConformalMesh(true),
m_WriteBinaryVTKFiles(false),
m_WriteSTLFile(false),
m_OutputDirectory(""),
m_OutputFilePrefix("SurfaceMesh_"),
m_SmoothMesh(false),
m_SmoothIterations(0),
m_SmoothFileOutputIncrement(0),
m_SmoothLockQuadPoints(false),
m_DeleteTempFiles(true)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMesh::~SurfaceMesh()
{
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMesh::execute()
{
  int err = 0;

  SurfaceMeshFilter::Pointer smFilter = SurfaceMeshFilter::New();
  smFilter->addObserver(this);
  smFilter->setInputFile(m_InputFile);
  smFilter->setWriteSTLFile(m_WriteSTLFile);

  // We need this to be false in order to write the VTK Files or Smooth the mesh
  smFilter->setDeleteTempFiles(false);
  smFilter->setOutputDirectory(m_OutputDirectory);
  smFilter->setOutputFilePrefix(m_OutputFilePrefix);
  setCurrentFilter(smFilter);
  smFilter->execute();
  err = smFilter->getErrorCondition();
  if (err < 0)
  {
    updateProgressAndMessage(smFilter->getErrorMessage().c_str(), 100);
    if (m_DeleteTempFiles == true)
    {
    smFilter->getNodesFile()->setAutoDelete(true);
    smFilter->getTrianglesFile()->setAutoDelete(true);
    }
  }

  SMTempFile::Pointer nodesFile = smFilter->getNodesFile();
  SMTempFile::Pointer trianglesFile = smFilter->getTrianglesFile();

  // Smooth the mesh and overwrite the existing Triangles and Nodes files.
  if (m_SmoothMesh)
  {
    updateProgressAndMessage(("Smoothing Boundaries"), 90);
    smooth::Smoothing::Pointer smoothing = smooth::Smoothing::New();
    smoothing->setOutputDirectory(m_OutputDirectory);
    smoothing->setOutputFilePrefix(m_OutputFilePrefix);
    smoothing->setNodesFile(nodesFile->getFilePath());
    smoothing->setTrianglesFile(trianglesFile->getFilePath());
    smoothing->setIterations(m_SmoothIterations);
    smoothing->setOutputInterval(m_SmoothFileOutputIncrement);
    smoothing->setLockQuads(m_SmoothLockQuadPoints);
    setCurrentFilter(smoothing);
    smoothing->execute();
    err = smoothing->getErrorCondition();
    if (err < 0)
    {
      setErrorCondition(-1);
      updateProgressAndMessage("Error Smoothing Surface Mesh", 100);
    }
  }

  if (m_DeleteTempFiles == true)
  {
    nodesFile->setAutoDelete(true);
    trianglesFile->setAutoDelete(true);
  }

  std::string msg("Writing VTK Polydata Surface Mesh File: ");
  msg.append(DREAM3D::SurfaceMesh::VisualizationVizFile);
  updateProgressAndMessage(msg.c_str(), 95);

  MAKE_OUTPUT_FILE_PATH(VisualizationFile, DREAM3D::SurfaceMesh::VisualizationVizFile)

  meshing::SMVtkPolyDataWriter::Pointer writer = SMVtkPolyDataWriter::New();
  writer->setVisualizationFile(VisualizationFile);
  writer->setNodesFile(nodesFile->getFilePath());
  writer->setTrianglesFile(trianglesFile->getFilePath());
  writer->setWriteBinaryFile(m_WriteBinaryVTKFiles);
  writer->setWriteConformalMesh(m_WriteConformalMesh);
  writer->addObserver(this);
  setCurrentFilter(writer);
  writer->execute();
  err = writer->getErrorCondition();
  CHECK_FOR_ERROR(SurfaceMeshFilter, "Error Writing vtk output file", err)


  updateProgressAndMessage("Surface Meshing Complete", 100);
}
