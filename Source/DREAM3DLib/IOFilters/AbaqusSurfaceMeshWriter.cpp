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

#include "AbaqusSurfaceMeshWriter.h"

#include <stdio.h>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>


#include "DREAM3DLib/Common/ScopedFileMonitor.hpp"

#define TRI_ELEMENT_TYPE "SFM3D3"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbaqusSurfaceMeshWriter::AbaqusSurfaceMeshWriter() :
  AbstractFilter(),
  m_SurfaceDataContainerName(DREAM3D::Defaults::SurfaceDataContainerName),
  m_FaceAttributeMatrixName(DREAM3D::Defaults::FaceAttributeMatrixName),
  m_OutputFile(""),
  m_SurfaceMeshFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceLabels(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbaqusSurfaceMeshWriter::~AbaqusSurfaceMeshWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbaqusSurfaceMeshWriter::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Output File", "OutputFile", FilterParameterWidgetType::OutputFileWidget,"QString", false, "", "*.inp"));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbaqusSurfaceMeshWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setOutputFile( reader->readString( "OutputFile", getOutputFile() ) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbaqusSurfaceMeshWriter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("OutputFile", getOutputFile() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbaqusSurfaceMeshWriter::dataCheck()
{
  setErrorCondition(0);
  if (m_OutputFile.isEmpty() == true)
  {
    setErrorCondition(-1003);
    notifyErrorMessage(getHumanLabel(), "Stl Output Directory is Not set correctly", -1003);
  }

  SurfaceDataContainer* sm = getDataContainerArray()->getPrereqDataContainer<SurfaceDataContainer, AbstractFilter>(this, getSurfaceDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer attrMat = sm->getPrereqAttributeMatrix<AbstractFilter>(this, getFaceAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }

  if (sm->getFaces().get() == NULL)
  {
    setErrorCondition(-384);
    notifyErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", getErrorCondition());
  }
  else
  {
    QVector<size_t> dims(1, 2);
    m_SurfaceMeshFaceLabelsPtr = attrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_SurfaceMeshFaceLabelsArrayName, -386, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  if (sm->getVertices().get() == NULL)
  {
    setErrorCondition(-384);
    notifyErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", getErrorCondition());
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbaqusSurfaceMeshWriter::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbaqusSurfaceMeshWriter::execute()
{
  int err = 0;

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(getOutputFile());
  QDir parentPath = fi.path();
  if(!parentPath.mkpath("."))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath.absolutePath());
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
    return;
  }

  VertexArray::Pointer nodesPtr = sm->getVertices();
  FaceArray::Pointer trianglePtr = sm->getFaces();

  // Store all the unique Spins
  QSet<int> uniqueSpins;
  for (int i = 0; i < trianglePtr->getNumberOfTuples(); i++)
  {
    uniqueSpins.insert(m_SurfaceMeshFaceLabels[i * 2]);
    uniqueSpins.insert(m_SurfaceMeshFaceLabels[i * 2 + 1]);
  }

  FILE* f = fopen(m_OutputFile.toLatin1().data(), "wb");
  ScopedFileMonitor fileMonitor(f);

  err = writeHeader(f, nodesPtr->getNumberOfTuples(), trianglePtr->getNumberOfTuples(), uniqueSpins.size() - 1);
  err = writeNodes(f);
  err = writeTriangles(f);
  err = writeFeatures(f);

  setErrorCondition(0);
  notifyStatusMessage(getHumanLabel(), "Complete");

  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbaqusSurfaceMeshWriter::writeHeader(FILE* f, int nodeCount, int triCount, int featureCount)
{
  if (NULL == f)
  {
    return -1;
  }
  fprintf(f, "*HEADING\n");
  fprintf(f, "** File Created with DREAM3D Version %s\n", DREAM3DLib::Version::Complete().toStdString().c_str());
  fprintf(f, "** There are 3 Sections to this INP File: Nodes, Elements and Sets of Elements for each grain\n");
  fprintf(f, "** This file represents a trianguglar based mesh. The element type selected is %s for the triangles\n", TRI_ELEMENT_TYPE);
  fprintf(f, "** This file is an experimental output from DREAM3D. The user is responsible for verifying all elements in Abaqus\n");
  fprintf(f, "** We have selected to use a 'shell' element type currently. No boundary elements are written\n");
  fprintf(f, "**Number of Nodes: %d     Number of Triangles: %d   Number of Grains: %d\n", nodeCount, triCount, featureCount);
  fprintf(f, "*PREPRINT,ECHO=NO,HISTORY=NO,MODEL=NO\n");
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbaqusSurfaceMeshWriter::writeNodes(FILE* f)
{
  VertexArray::Pointer nodesPtr = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName())->getVertices();
  VertexArray::Vert_t* nodes = nodesPtr->getPointer(0);
  size_t numNodes = nodesPtr->getNumberOfTuples();
  int err = 0;
  fprintf(f, "*Node,NSET=NALL\n");
  //1, 72.520433763730, 70.306420652241, 100.000000000000


  for(size_t i = 1; i <= numNodes; ++i)
  {
    VertexArray::Vert_t& n = nodes[i - 1];
    fprintf(f, "%lu, %0.6f, %0.6f, %0.6f\n", i, n.pos[0], n.pos[1], n.pos[2]);
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbaqusSurfaceMeshWriter::writeTriangles(FILE* f)
{
  int err = 0;
  FaceArray& triangles = *(getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName())->getFaces());
  size_t numTri = triangles.getNumberOfTuples();

  fprintf(f, "*ELEMENT, TYPE=%s\n", TRI_ELEMENT_TYPE);
  for(size_t i = 1; i <= numTri; ++i)
  {
    // When we get the node index, add 1 to it because Abaqus number is 1 based.
    int nId0 = triangles[i - 1].verts[0] + 1;
    int nId1 = triangles[i - 1].verts[1] + 1;
    int nId2 = triangles[i - 1].verts[2] + 1;
    fprintf(f, "%lu, %d, %d, %d\n", i, nId0, nId1, nId2);
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbaqusSurfaceMeshWriter::writeFeatures(FILE* f)
{

  //*Elset, elset=Feature1
  //1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16

  int err = 0;

  VertexArray::Pointer nodesPtr = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName())->getVertices();
  FaceArray::Pointer trianglePtr = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName())->getFaces();
  // Get the Labels(FeatureIds or Region Ids) for the triangles

  int nTriangles = trianglePtr->getNumberOfTuples();

  // Store all the unique Spins
  QSet<int> uniqueSpins;
  for (int i = 0; i < nTriangles; i++)
  {
    uniqueSpins.insert(m_SurfaceMeshFaceLabels[i * 2]);
    uniqueSpins.insert(m_SurfaceMeshFaceLabels[i * 2 + 1]);
  }

  int spin = 0;

  //Loop over the unique Spins
  for (QSet<int>::iterator spinIter = uniqueSpins.begin(); spinIter != uniqueSpins.end(); ++spinIter )
  {
    spin = *spinIter;
    if(spin < 0) { continue; }

    fprintf(f, "*ELSET, ELSET=Feature%d\n", spin);

    {
      QString ss = QObject::tr("Writing ELSET for Feature Id %1").arg(spin);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    }

    // Loop over all the triangles for this spin
    int lineCount = 0;
    for(int t = 0; t < nTriangles; ++t)
    {
      if (m_SurfaceMeshFaceLabels[t * 2] != spin && m_SurfaceMeshFaceLabels[t * 2 + 1] != spin)
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
        fprintf(f, "%d", t);
        lineCount++;
      }
      else
      {
        fprintf(f, ", %d", t);
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




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer AbaqusSurfaceMeshWriter::newFilterInstance(bool copyFilterParameters)
{
  /*
  * OutputFile
  */
  AbaqusSurfaceMeshWriter::Pointer filter = AbaqusSurfaceMeshWriter::New();
  if(true == copyFilterParameters)
  {
    filter->setOutputFile( getOutputFile() );
  }
  return filter;
}
