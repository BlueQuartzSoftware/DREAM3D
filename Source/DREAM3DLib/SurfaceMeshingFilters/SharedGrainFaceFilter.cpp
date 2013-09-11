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


#include "SharedGrainFaceFilter.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedGrainFaceFilter::SharedGrainFaceFilter() :
  SurfaceMeshFilter(),
  m_SurfaceMeshGrainFaceIdArrayName(DREAM3D::FaceData::SurfaceMeshGrainFaceId)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedGrainFaceFilter::~SharedGrainFaceFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SharedGrainFaceFilter::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SharedGrainFaceFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int SharedGrainFaceFilter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  /*  writer->writeValue("OutputFile", getOutputFile() ); */
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SharedGrainFaceFilter::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  QString ss;
  SurfaceDataContainer* sm = getSurfaceDataContainer();
  if(NULL == sm)
  {
    setErrorCondition(-383);
    addErrorMessage(getHumanLabel(), "SurfaceDataContainer is missing", getErrorCondition());
  }
  else
  {
    // We MUST have Nodes
    if(sm->getVertices().get() == NULL)
    {
      setErrorCondition(-384);
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", getErrorCondition());
    }

    // We MUST have Triangles defined also.
    if(sm->getFaces().get() == NULL)
    {
      setErrorCondition(-385);
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", getErrorCondition());
    }

    // List any arrays that are created during this filter
    Int32ArrayType::Pointer grainFaceId = Int32ArrayType::CreateArray(1, 1, DREAM3D::FaceData::SurfaceMeshGrainFaceId);
    sm->addFaceData(DREAM3D::FaceData::SurfaceMeshGrainFaceId, grainFaceId);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SharedGrainFaceFilter::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SharedGrainFaceFilter::execute()
{
  int err = 0;
  QString ss;
  setErrorCondition(err);
  SurfaceDataContainer* m = getSurfaceDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The Voxel DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  notifyStatusMessage("Starting");

  /* Place all your code to execute your filter here. */

  DREAM3D::Mesh::FaceListPointer_t trianglesPtr = getSurfaceDataContainer()->getFaces();
//  DREAM3D::Mesh::Face_t* triangles = trianglesPtr->GetPointer(0);
  size_t totalPoints = trianglesPtr->GetNumberOfTuples();

  IDataArray::Pointer flPtr = getSurfaceDataContainer()->getFaceData(DREAM3D::FaceData::SurfaceMeshFaceLabels);
  DataArray<int32_t>* faceLabelsPtr = DataArray<int32_t>::SafePointerDownCast(flPtr.get());
  int32_t* faceLabels = faceLabelsPtr->GetPointer(0);

  Int32ArrayType::Pointer grainFaceId = Int32ArrayType::CreateArray(trianglesPtr->GetNumberOfTuples(), DREAM3D::FaceData::SurfaceMeshGrainFaceId);
  grainFaceId->initializeWithZeros();


  QMap<uint64_t, int> faceSizeMap;
  QMap<uint64_t, int32_t> faceIdMap; // This maps a unique 64 bit integer to an increasing 32 bit integer
  uint32_t index = 0;
  struct { int g; int r; } faceId;
  uint64_t* faceId_64 = reinterpret_cast<uint64_t*>(&faceId);
  int32_t fl0 = -1;
  int32_t fl1 = -1;
  // Loop through all the Triangles and figure out how many triangles we have in each one.
  for(size_t t = 0; t < totalPoints; ++t)
  {
    fl0 = faceLabels[t*2];
    fl1 = faceLabels[t*2+1];
    if (fl0 < fl1)
    {
      faceId.g = fl0;
      faceId.r = fl1;
    }
    else
    {
      faceId.g = fl1;
      faceId.r = fl0;
    }

    QMap<uint64_t, int>::iterator iter = faceSizeMap.find(*faceId_64);
    if(iter==faceSizeMap.end())
    {
      faceSizeMap[*faceId_64] = 1;
      faceIdMap[*faceId_64] = index;
      grainFaceId->SetValue(t, index);
      ++index;
    }
    else
    {
      faceSizeMap[*faceId_64]++;
      grainFaceId->SetValue(t, faceIdMap[*faceId_64]);
    }
  }


  SharedGrainFaces_t faces;

  // Allocate all the vectors that we need
  for(QMap<uint64_t, int>::iterator iter = faceSizeMap.begin(); iter != faceSizeMap.end(); ++iter)
  {
    FaceIds_t v;
    v.reserve((*iter).second);
    index = faceIdMap[(*iter).first];
    faces[index] = v;
  }


  // Loop through all the Triangles and assign each one to a unique Grain Face Id.
  for(size_t t = 0; t < totalPoints; ++t)
  {
  #if 0
    Triangle& tri = triangles[t];
    if (tri.nSpin[0] < tri.nSpin[1])
    {
      faceId.g = tri.nSpin[0];
      faceId.r = tri.nSpin[1];
    }
    else
    {
      faceId.g = tri.nSpin[1];
      faceId.r = tri.nSpin[0];
    }
    index = faceIdMap[*faceId_64];
    #else
    index = grainFaceId->GetValue(t);
    #endif
    faces[index].push_back(t);
  }

  m_SharedGrainFaces = faces;

  getSurfaceDataContainer()->addFaceData(DREAM3D::FaceData::SurfaceMeshGrainFaceId, grainFaceId);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedGrainFaceFilter::SharedGrainFaces_t& SharedGrainFaceFilter::getSharedGrainFaces()
{
  return m_SharedGrainFaces;
}
