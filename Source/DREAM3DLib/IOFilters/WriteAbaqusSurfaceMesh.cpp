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

#include "WriteAbaqusSurfaceMesh.h"

#include <stdio.h>

#include "DREAM3DLib/Common/ScopedFileMonitor.hpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteAbaqusSurfaceMesh::WriteAbaqusSurfaceMesh() :
  AbstractFilter(),
  m_OutputFile("")
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteAbaqusSurfaceMesh::~WriteAbaqusSurfaceMesh()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteAbaqusSurfaceMesh::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output File");
    option->setPropertyName("OutputFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setValueType("string");
    option->setFileExtension("*.inp");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteAbaqusSurfaceMesh::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setOutputFile( reader->readValue( "OutputFile", getOutputFile() ) );
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int WriteAbaqusSurfaceMesh::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("OutputFile", getOutputFile() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteAbaqusSurfaceMesh::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  if (m_OutputFile.isEmpty() == true)
  {
    setErrorCondition(-1003);
    addErrorMessage(getHumanLabel(), "Stl Output Directory is Not set correctly", -1003);
  }

  SurfaceDataContainer* sm = getSurfaceDataContainer();
  if (NULL == sm)
  {
    addErrorMessage(getHumanLabel(), "SurfaceDataContainer is missing", -383);
    setErrorCondition(-384);
  }
  else {
    if (sm->getFaces().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", -383);
      setErrorCondition(-384);
    }
    if (sm->getVertices().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", -384);
      setErrorCondition(-384);
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteAbaqusSurfaceMesh::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteAbaqusSurfaceMesh::execute()
{
  int err = 0;
  

  SurfaceDataContainer* sm = getSurfaceDataContainer();
  if(NULL == sm)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The SurfaceDataContainer Object was NULL", -999);
    return;
  }


  dataCheck(false, 1, 1, 1);
  if(getErrorCondition() < 0)
  {
    return;
  }

   // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QString parentPath = QFileInfo::parentPath(getOutputFile());
  if(!MXADir::mkdir(parentPath, true))
  {
      
      ss << "Error creating parent path '" << parentPath << "'";
      notifyErrorMessage(ss.str(), -1);
      setErrorCondition(-1);
      return;
  }

  VertexArray::Pointer nodesPtr = sm->getVertices();
  FaceArray::Pointer trianglePtr = sm->getFaces();

  // Get the Labels(GrainIds or Region Ids) for the triangles
  IDataArray::Pointer flPtr = getSurfaceDataContainer()->getFaceData(DREAM3D::FaceData::SurfaceMeshFaceLabels);
  DataArray<int32_t>* faceLabelsPtr = DataArray<int32_t>::SafePointerDownCast(flPtr.get());
  int32_t* faceLabels = faceLabelsPtr->GetPointer(0);

  // Store all the unique Spins
  QSet<int> uniqueSpins;
  for (int i = 0; i < trianglePtr->GetNumberOfTuples(); i++)
  {
    uniqueSpins.insert(faceLabels[i*2]);
    uniqueSpins.insert(faceLabels[i*2+1]);
  }

  FILE* f = fopen(m_OutputFile.toLatin1().data(), "wb");
  ScopedFileMonitor fileMonitor(f);

  err = writeHeader(f, nodesPtr->GetNumberOfTuples(), trianglePtr->GetNumberOfTuples(), uniqueSpins.size()-1);
  err = writeNodes(f);
  err = writeTriangles(f);
  err = writeGrains(f);

  setErrorCondition(0);
  notifyStatusMessage("Complete");

  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int WriteAbaqusSurfaceMesh::writeHeader(FILE* f, int nodeCount, int triCount, int grainCount)
{
  if (NULL == f)
  {
    return -1;
  }
  fprintf(f, "*HEADING\n");
  fprintf(f, "** File Created with DREAM3D Version %s.%s\n", DREAM3DLib::Version::Major().toLatin1().data(), DREAM3DLib::Version::Minor().toLatin1().data());
  fprintf(f, "**Number of Nodes: %d     Number of Triangles: %d   Number of Grains: %d\n", nodeCount, triCount, grainCount);
  fprintf(f, "*PREPRINT,ECHO=NO,HISTORY=NO,MODEL=NO\n");
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int WriteAbaqusSurfaceMesh::writeNodes(FILE* f)
{
  VertexArray::Pointer nodesPtr = getSurfaceDataContainer()->getVertices();
  VertexArray::Vert_t* nodes = nodesPtr->GetPointer(0);
  size_t numNodes = nodesPtr->GetNumberOfTuples();
  int err = 0;
  fprintf(f, "*Node,NSET=NALL\n");
//1, 72.520433763730, 70.306420652241, 100.000000000000


  for(size_t i = 1; i <= numNodes; ++i)
  {
    VertexArray::Vert_t& n = nodes[i-1];
    fprintf(f, "%lu, %0.6f, %0.6f, %0.6f\n", i, n.pos[0], n.pos[1], n.pos[2]);
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int WriteAbaqusSurfaceMesh::writeTriangles(FILE* f)
{
  int err = 0;
  FaceArray::Pointer trianglePtr = getSurfaceDataContainer()->getFaces();
  FaceArray::Face_t* triangles = trianglePtr->GetPointer(0);
  size_t numTri = trianglePtr->GetNumberOfTuples();

  fprintf(f, "*ELEMENT, TYPE=SFM3D3\n");
  for(size_t i = 1; i <= numTri; ++i)
  {

    // When we get the node index, add 1 to it because Abaqus number is 1 based.
    int nId0 = triangles[i-1].verts[0] + 1;
    int nId1 = triangles[i-1].verts[1] + 1;
    int nId2 = triangles[i-1].verts[2] + 1;
    fprintf(f, "%lu, %d, %d, %d\n", i, nId0, nId1, nId2);
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int WriteAbaqusSurfaceMesh::writeGrains(FILE* f)
{

//*Elset, elset=Grain1
//1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16

  int err = 0;

  

  VertexArray::Pointer nodesPtr = getSurfaceDataContainer()->getVertices();
  FaceArray::Pointer trianglePtr = getSurfaceDataContainer()->getFaces();
  // Get the Labels(GrainIds or Region Ids) for the triangles
  IDataArray::Pointer flPtr = getSurfaceDataContainer()->getFaceData(DREAM3D::FaceData::SurfaceMeshFaceLabels);
  DataArray<int32_t>* faceLabelsPtr = DataArray<int32_t>::SafePointerDownCast(flPtr.get());
  int32_t* faceLabels = faceLabelsPtr->GetPointer(0);




  int nTriangles = trianglePtr->GetNumberOfTuples();

  // Store all the unique Spins
  QSet<int> uniqueSpins;
  for (int i = 0; i < nTriangles; i++)
  {
    uniqueSpins.insert(faceLabels[i*2]);
    uniqueSpins.insert(faceLabels[i*2+1]);
  }

  int spin = 0;

  //Loop over the unique Spins
  for (QSet<int>::iterator spinIter = uniqueSpins.begin(); spinIter != uniqueSpins.end(); ++spinIter )
  {
    spin = *spinIter;
    if(spin < 0) { continue; }

    fprintf(f, "*ELSET, ELSET=Grain%d\n", spin);

    ss.str("");
    ss << "Writing ELSET for Grain Id " << spin;
    notifyStatusMessage(ss.str());


    // Loop over all the triangles for this spin
    int lineCount = 0;
    for(int t = 0; t < nTriangles; ++t)
    {
      if (faceLabels[t*2] != spin && faceLabels[t*2+1] != spin)
      {
        continue; // We do not match either spin so move to the next triangle
      }

      // Only print 15 Triangles per line
      if (lineCount == 15)
      {
        fprintf(f, ", %d\n", t);
        lineCount = 0;
      }
      else if(lineCount == 0) // First value on the line
      {
        fprintf(f,"%d", t);
        lineCount++;
      }
      else
      {
        fprintf(f,", %d", t);
        lineCount++;
      }

    }
    // Make sure we have a new line at the end of the section
    if (lineCount != 0)
    {
    fprintf(f, "\n");
    }
  }
  return err;
}



