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

#include "SurfaceMeshToStl.h"

#include <QtCore/QSet>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>


#include <QtCore/QDir>
#include <QtCore/QFile>
#include "DREAM3DLib/Math/DREAM3DMath.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshToStl::SurfaceMeshToStl() :
  AbstractFilter(),
  m_SurfaceDataContainerName(DREAM3D::Defaults::SurfaceDataContainerName),
  m_FaceAttributeMatrixName(DREAM3D::Defaults::FaceAttributeMatrixName),
  m_OutputStlDirectory(""),
  m_OutputStlPrefix(""),
  m_SurfaceMeshFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceLabels(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshToStl::~SurfaceMeshToStl()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToStl::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output STL Directory");
    option->setPropertyName("OutputStlDirectory");
    option->setWidgetType(FilterParameter::OutputPathWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Stl File Prefix");
    option->setPropertyName("OutputStlPrefix");
    option->setWidgetType(FilterParameter::StringWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToStl::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setOutputStlDirectory( reader->readString( "OutputStlDirectory", getOutputStlDirectory() ) );
  setOutputStlPrefix( reader->readString( "OutputStlPrefix", getOutputStlPrefix() ) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshToStl::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("OutputStlDirectory", getOutputStlDirectory() );
  writer->writeValue("OutputStlPrefix", getOutputStlPrefix() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToStl::dataCheck()
{
  setErrorCondition(0);
  if (m_OutputStlDirectory.isEmpty() == true)
  {
    setErrorCondition(-1003);
    notifyErrorMessage(getHumanLabel(), "Stl Output Directory is Not set correctly", -1003);
  }

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix* attrMat = sm->getPrereqAttributeMatrix<AbstractFilter>(this, getFaceAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }

  if (sm->getFaces().get() == NULL)
  {
    setErrorCondition(-384);
    notifyErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", getErrorCondition());
  }
  if (sm->getVertices().get() == NULL)
  {
    setErrorCondition(-384);
    notifyErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", getErrorCondition());
  }
  QVector<size_t> dims(1, 2);
  m_SurfaceMeshFaceLabelsPtr = attrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_SurfaceMeshFaceLabelsArrayName, -386, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToStl::preflight()
{
  dataCheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToStl::execute()
{
  int err = 0;

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QDir stlDir(getOutputStlDirectory());
  if(!stlDir.mkpath("."))
  {

    QString ss = QObject::tr("Error creating parent path '%1'").arg(getOutputStlDirectory());
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
    return;
  }

  VertexArray& nodes = *(sm->getVertices());
  FaceArray& triangles = *(sm->getFaces());

  int nTriangles = triangles.getNumberOfTuples();

  // Store all the unique Spins
  QSet<int> uniqueSpins;
  for (int i = 0; i < nTriangles; i++)
  {
    uniqueSpins.insert(m_SurfaceMeshFaceLabels[i * 2]);
    uniqueSpins.insert(m_SurfaceMeshFaceLabels[i * 2 + 1]);
  }

  unsigned char data[50];
  float* normal = (float*)data;
  float* vert1 = (float*)(data + 12);
  float* vert2 = (float*)(data + 24);
  float* vert3 = (float*)(data + 36);
  uint16_t* attrByteCount = (uint16_t*)(data + 48);
  *attrByteCount = 0;

  size_t totalWritten = 0;
  float u[3], w[3];
  float length;

  int spin = 0;
  int triCount = 0;

  //Loop over the unique Spins
  for (QSet<int>::iterator spinIter = uniqueSpins.begin(); spinIter != uniqueSpins.end(); ++spinIter )
  {
    spin = *spinIter;

    // Generate the output file name
    QString filename = getOutputStlDirectory() + "/" + getOutputStlPrefix() + QString::number(spin) + ".stl";
    FILE* f = fopen(filename.toLatin1().data(), "wb");

    {
      QString ss = QObject::tr("Writing STL for Feature Id ").arg(spin);
      notifyStatusMessage(getHumanLabel(), ss);
    }

    QString header = "DREAM3D Generated For Feature ID " + QString::number(spin);
    err = writeHeader(f, header, 0);
    if(err < 0)
    {
    }
    triCount = 0; // Reset this to Zero. Increment for every triangle written

    // Loop over all the triangles for this spin
    for(int t = 0; t < nTriangles; ++t)
    {
      char winding = 2; // 2 = Do NOT write this triangle
      // Get the true indices of the 3 nodes
      int nId0 = triangles[t].verts[0];
      int nId1 = triangles[t].verts[1];
      int nId2 = triangles[t].verts[2];

      vert1[0] = static_cast<float>(nodes[nId0].pos[0]);
      vert1[1] = static_cast<float>(nodes[nId0].pos[1]);
      vert1[2] = static_cast<float>(nodes[nId0].pos[2]);

      if (m_SurfaceMeshFaceLabels[t * 2] == spin)
      {
        winding = 0; // 0 = Write it using forward spin
      }
      else if (m_SurfaceMeshFaceLabels[t * 2 + 1] == spin)
      {
        winding = 1; // Write it using backward spin
        // Switch the 2 node indices
        int temp = nId1;
        nId1 = nId2;
        nId2 = temp;
      }
      else
      {
        continue; // We do not match either spin so move to the next triangle
      }

      vert2[0] = static_cast<float>(nodes[nId1].pos[0]);
      vert2[1] = static_cast<float>(nodes[nId1].pos[1]);
      vert2[2] = static_cast<float>(nodes[nId1].pos[2]);

      vert3[0] = static_cast<float>(nodes[nId2].pos[0]);
      vert3[1] = static_cast<float>(nodes[nId2].pos[1]);
      vert3[2] = static_cast<float>(nodes[nId2].pos[2]);

      //
      // Compute the normal
      u[0] = vert2[0] - vert1[0];
      u[1] = vert2[1] - vert1[1];
      u[2] = vert2[2] - vert1[2];

      w[0] = vert3[0] - vert1[0];
      w[1] = vert3[1] - vert1[1];
      w[2] = vert3[2] - vert1[2];

      normal[0] = u[1] * w[2] - u[2] * w[1];
      normal[1] = u[2] * w[0] - u[0] * w[2];
      normal[2] = u[0] * w[1] - u[1] * w[0];

      length = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
      normal[0] = normal[0] / length;
      normal[1] = normal[1] / length;
      normal[2] = normal[2] / length;

      totalWritten = fwrite(data, 1, 50, f);
      if (totalWritten != 50)
      {

        QString ss = QObject::tr("Error Writing STL File. Not enough elements written for feature id %1 Wrote %2 of 50.").arg(spin).arg(totalWritten);
        notifyErrorMessage(getHumanLabel(), ss, -1201);
      }
      triCount++;
    }
    fclose(f);
    err = writeNumTrianglesToFile(filename, triCount);
  }

  setErrorCondition(0);
  notifyStatusMessage(getHumanLabel(), "Complete");

  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshToStl::writeHeader(FILE* f, const QString& header, int triCount)
{
  if (NULL == f)
  {
    return -1;
  }
  char h[80];
  size_t headlength = 80;
  if(header.length() < 80) { headlength = header.length(); }
  ::memset(h, 0, 80);
  ::memcpy(h, header.data(), headlength);
  // Return the number of bytes written - which should be 80
  fwrite(h, 1, 80, f);
  fwrite(&triCount, 1, 4, f);
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshToStl::writeNumTrianglesToFile(const QString& filename, int triCount)
{
  // We need to update the number of triangles in the file
  int err = 0;

  FILE* out = fopen(filename.toLatin1().data(), "r+b");
  fseek(out, 80L, SEEK_SET);
  fwrite( (char*)(&triCount), 1, 4, out);
  fclose(out);

  return err;
}



