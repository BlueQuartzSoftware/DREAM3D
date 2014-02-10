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

#include <map>
#include <set>

#include "MXA/MXA.h"
#include "MXA/Common/MXAEndian.h"
#include "MXA/Utilities/MXADir.h"
#include "MXA/Common/MXAMath.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshToStl::SurfaceMeshToStl() :
  AbstractFilter(),
  m_SurfaceMeshFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshPhaseLabelsArrayName(DREAM3D::FaceData::SurfaceMeshPhaseLabels),
  m_OutputStlDirectory(""),
  m_OutputStlPrefix(""),
  m_GroupByPhase(false),
  m_SurfaceMeshFaceLabels(NULL),
  m_SurfaceMeshPhaseLabels(NULL)
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
  std::vector<FilterParameter::Pointer> parameters;
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
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Group Files By Phase");
    option->setPropertyName("GroupByPhase");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
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
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshToStl::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->openFilterGroup(this, index);
  writer->writeValue("OutputStlDirectory", getOutputStlDirectory() );
  writer->writeValue("OutputStlPrefix", getOutputStlPrefix() );
  writer->writeValue("GroupByPhase", getGroupByPhase() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToStl::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  if (m_OutputStlDirectory.empty() == true)
  {
    setErrorCondition(-1003);
    addErrorMessage(getHumanLabel(), "Stl Output Directory is Not set correctly", -382);
  }

  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if (NULL == sm)
  {
    addErrorMessage(getHumanLabel(), "SurfaceMeshDataContainer is missing", -383);
    setErrorCondition(-384);
  }
  else
  {
    if (sm->getFaces().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", -384);
      setErrorCondition(-384);
    }
    if (sm->getVertices().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", -385);
      setErrorCondition(-384);
    }
    std::stringstream ss;
    GET_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshFaceLabels, ss, -30, int32_t, Int32ArrayType, sm->getNumFaceTuples(), 2)

        if(m_GroupByPhase == true)
    {
      GET_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshPhaseLabels, ss, -30, int32_t, Int32ArrayType, sm->getNumFaceTuples(), 2)
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToStl::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToStl::execute()
{
  int err = 0;
  std::stringstream ss;

  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  dataCheck(false, 1, 1, 1);
  if(getErrorCondition() < 0)
  {
    return;
  }



  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  if(!MXADir::mkdir(getOutputStlDirectory(), true))
  {
    std::stringstream ss;
    ss << "Error creating parent path '" << getOutputStlDirectory() << "'";
    notifyErrorMessage(ss.str(), -1);
    setErrorCondition(-1);
    return;
  }

  DREAM3D::SurfaceMesh::VertListPointer_t nodesPtr = sm->getVertices();
  DREAM3D::SurfaceMesh::Vert_t* nodes = nodesPtr->GetPointer(0);
  DREAM3D::SurfaceMesh::FaceListPointer_t trianglePtr = sm->getFaces();
  DREAM3D::SurfaceMesh::Face_t* triangles = trianglePtr->GetPointer(0);
  // Get the Labels(feature ids) for the triangles
  Int32ArrayType::Pointer faceLabelsPtr = boost::dynamic_pointer_cast<Int32ArrayType>(sm->getFaceData(DREAM3D::FaceData::SurfaceMeshFaceLabels));
  int32_t* faceLabels = faceLabelsPtr->GetPointer(0);

  Int32ArrayType::Pointer phaseLabelsPtr = Int32ArrayType::NullPointer();
  int32_t* phaseLabels = NULL;
  if (m_GroupByPhase == true)
  {
    phaseLabelsPtr = boost::dynamic_pointer_cast<Int32ArrayType>(sm->getFaceData(DREAM3D::FaceData::SurfaceMeshPhaseLabels));
    phaseLabels = phaseLabelsPtr->GetPointer(0);
  }

  int nTriangles = trianglePtr->GetNumberOfTuples();

  typedef std::pair<int, int> PairType;
  // Store all the unique Feature Ids
  std::map<int, int> uniqueGrainIdtoPhase;
  if (m_GroupByPhase == true) {
    for (int i = 0; i < nTriangles; i++)
    {
      uniqueGrainIdtoPhase.insert(PairType(faceLabels[i*2], phaseLabels[i*2]));
      uniqueGrainIdtoPhase.insert(PairType(faceLabels[i*2+1], phaseLabels[i*2]));
    }
  }
  else
  {
    for (int i = 0; i < nTriangles; i++)
    {
      uniqueGrainIdtoPhase.insert(PairType(faceLabels[i*2], 0));
      uniqueGrainIdtoPhase.insert(PairType(faceLabels[i*2+1], 0));
    }
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

  int grainId = 0;
  int triCount = 0;

  //Loop over the unique Feature Ids
  for (std::map<int, int>::iterator spinIter = uniqueGrainIdtoPhase.begin(); spinIter != uniqueGrainIdtoPhase.end(); ++spinIter )
  {
    PairType grainPhase = *spinIter;
    grainId = grainPhase.first;
    ss.str("");
    // Generate the output file name
    ss << getOutputStlDirectory() << MXADir::Separator << getOutputStlPrefix();
    if(m_GroupByPhase == true)
    {
      ss << "Phase_" << grainPhase.second << "_";
    }
    ss << "Grain_" << grainId << ".stl";

    std::string filename = ss.str();
    FILE* f = fopen(filename.c_str(), "wb");

    ss.str("");
    ss << "Writing STL for Grain Id " << grainId;
    notifyStatusMessage(ss.str());


    ss.str("");
    ss << "DREAM3D Generated For Grain ID " << grainId;
    if (m_GroupByPhase == true) {
      ss << " Phase " << grainPhase.second;
    }
    err = writeHeader(f, ss.str(), 0);
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

      if (faceLabels[t*2] == grainId)
      {
        winding = 0; // 0 = Write it using forward spin
      }
      else if (faceLabels[t*2+1] == grainId)
      {
        winding = 1; // Write it using backward spin
        // Switch the 2 node indices
        int t = nId1;
        nId1 = nId2;
        nId2 = t;
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
        ss.str("");
        ss << "Error Writing STL File. Not enough elements written for grain id " << grainId << " Wrote " << totalWritten << " of 50.";
        notifyErrorMessage(ss.str(), -1201);
      }
      triCount++;
    }
    fclose(f);
    err = writeNumTrianglesToFile(filename, triCount);
  }

  setErrorCondition(0);
  notifyStatusMessage("Complete");

  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshToStl::writeHeader(FILE* f, const std::string &header, int triCount)
{
  if (NULL == f)
  {
    return -1;
  }
  char h[80];
  size_t headlength = 80;
  if(header.length() < 80) headlength = header.length();
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
int SurfaceMeshToStl::writeNumTrianglesToFile(const std::string &filename, int triCount)
{
  // We need to update the number of triangles in the file
  int err =0;

  FILE* out = fopen(filename.c_str(), "r+b");
  fseek(out, 80L, SEEK_SET);
  fwrite( (char*)(&triCount), 1, 4, out);
  fclose(out);

  return err;
}


