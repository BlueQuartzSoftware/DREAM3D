/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "SampleSurfaceMesh.h"

#include <QtCore/QMap>


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/GeometryMath.h"

#include "DREAM3DLib/Utilities/DREAM3DRandom.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SampleSurfaceMesh::SampleSurfaceMesh() :
  AbstractFilter(),
  m_SurfaceMeshFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_SurfaceDataContainerName(DREAM3D::HDF5::SurfaceDataContainerName)
{

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SampleSurfaceMesh::~SampleSurfaceMesh()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMesh::setupFilterParameters()
{
  FilterParameterVector parameters;

  setFilterParameters(parameters);
}


// -----------------------------------------------------------------------------
void SampleSurfaceMesh::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int SampleSurfaceMesh::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMesh::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());

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
  else
  {
    GET_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshFaceLabels, -386, int32_t, Int32ArrayType, fields, 2)
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMesh::preflight()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    VolumeDataContainer::Pointer vdc = VolumeDataContainer::New();
    vdc->setName(getDataContainerName());
    getDataContainerArray()->pushBack(vdc);
    m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  }
  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());
  if(NULL == sm)
  {
    setErrorCondition(-383);
    addErrorMessage(getHumanLabel(), "SurfaceDataContainer is missing", getErrorCondition());
  }

  dataCheck(false, 1, 1, 1);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMesh::execute()
{
  int err = 0;
  setErrorCondition(err);
  DREAM3D_RANDOMNG_NEW()
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    VolumeDataContainer::Pointer vdc = VolumeDataContainer::New();
    vdc->setName(getDataContainerName());
    getDataContainerArray()->pushBack(vdc);
    m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  }
  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());
  if(NULL == sm)
  {
    setErrorCondition(-383);
    addErrorMessage(getHumanLabel(), "SurfaceDataContainer is missing", getErrorCondition());
  }

  setErrorCondition(0);

  m->setDimensions(128, 128, 128);
  m->setOrigin(0.0, 0.0, 0.0);
  m->setResolution(0.1, 0.1, 0.1);

  Int32ArrayType::Pointer iArray = Int32ArrayType::NullPointer();
  iArray = Int32ArrayType::CreateArray((128*128*128), 1, DREAM3D::CellData::GrainIds);
  iArray->initializeWithZeros();
  int32_t* grainIds = iArray->getPointer(0);

  FaceArray::Pointer faces = sm->getFaces();
  int numFaces = faces->count();

  dataCheck(true, 0, numFaces, 0); 

  QVector<int> curFace(1);
  QVector<QVector<int> > faceLists(1);


  VertexArray::Vert_t ll, ur;
  VertexArray::Vert_t point;
  VertexArray::Pointer faceBBs = VertexArray::CreateArray(2*numFaces, "faceBBs");

  int g1, g2;
  for(int i=0;i<numFaces;i++)
  {
    g1 = m_SurfaceMeshFaceLabels[2*i];
    g2 = m_SurfaceMeshFaceLabels[2*i+1];
    if((g1+1) > faceLists.size()) faceLists.resize(g1+1);
    if((g2+1) > faceLists.size()) faceLists.resize(g2+1);
    if(g1 > 0) faceLists[g1].push_back(i);
    if(g2 > 0) faceLists[g2].push_back(i);
    curFace[0] = i;
    GeometryMath::FindBoundingBoxOfFaces(faces, curFace, ll, ur);
    faceBBs->setCoords(2*i, ll.pos);
    faceBBs->setCoords(2*i+1, ur.pos);
  }

  char code;
  float radius;

  for(int i=0;i<3;i++)
  {
     ll.pos[i] = 0.0;
     ur.pos[i] = 0.0;
  }

  int minx, miny, minz, maxx, maxy, maxz;
  int zStride, yStride;

  int count = 0;
  float coords[3];
  for(int iter=1;iter<2;iter++)
  {
    GeometryMath::FindBoundingBoxOfFaces(faces, faceLists[iter], ll, ur);
    GeometryMath::FindDistanceBetweenPoints(ll, ur, radius);

    minx = int(ll.pos[0]/0.1);
    miny = int(ll.pos[1]/0.1);
    minz = int(ll.pos[2]/0.1);
    maxx = int(ur.pos[0]/0.1);
    maxy = int(ur.pos[1]/0.1);
    maxz = int(ur.pos[2]/0.1);

    count = 0;
    for(int i=minz;i<maxz+1;i++)
    {
      zStride = i*128*128;
      for(int j=miny;j<maxy+1;j++)
      {
        yStride = j*128;
        for(int k=minx;k<maxx+1;k++)
        {
          point.pos[0] = k*0.1+0.05;
          point.pos[1] = j*0.1+0.05;
          point.pos[2] = i*0.1+0.05;
          code = GeometryMath::PointInPolyhedron(faces, faceLists[iter], faceBBs, point, ll, ur, radius);
          if(code == 'i') grainIds[zStride+yStride+k] = iter;
        }
      }
    }
  }

  m->addCellData(DREAM3D::CellData::GrainIds, iArray);

  notifyStatusMessage("Complete");
}
