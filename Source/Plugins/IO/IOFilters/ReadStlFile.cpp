/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ReadStlFile.h"

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/TriangleGeom.h"

#include "IO/IOConstants.h"
#include "IO/IOVersion.h"

#define STL_HEADER_LENGTH 80

/**
 * @brief The FindUniqueIdsImpl class implements a threaded algorithm that determines the set of
 * unique vertices in the triangle geometry
 */
class FindUniqueIdsImpl
{
public:
  FindUniqueIdsImpl(SharedVertexList::Pointer vertex, QVector<QVector<size_t>> nodesInBin, int64_t* uniqueIds)
  : m_Vertex(vertex)
  , m_NodesInBin(nodesInBin)
  , m_UniqueIds(uniqueIds)
  {
  }

  void convert(size_t start, size_t end) const
  {
    float* verts = m_Vertex->getPointer(0);
    for(size_t i = start; i < end; i++)
    {
      for(int32_t j = 0; j < m_NodesInBin[i].size(); j++)
      {
        size_t node1 = m_NodesInBin[i][j];
        if(m_UniqueIds[node1] == node1)
        {
          for(int32_t k = j + 1; k < m_NodesInBin[i].size(); k++)
          {
            size_t node2 = m_NodesInBin[i][k];
            if(verts[node1 * 3] == verts[node2 * 3] && verts[node1 * 3 + 1] == verts[node2 * 3 + 1] && verts[node1 * 3 + 2] == verts[node2 * 3 + 2])
            {
              m_UniqueIds[node2] = node1;
            }
          }
        }
      }
    }
  }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  void operator()(const tbb::blocked_range<size_t>& r) const
  {
    convert(r.begin(), r.end());
  }
#endif
private:
  SharedVertexList::Pointer m_Vertex;
  QVector<QVector<size_t>> m_NodesInBin;
  int64_t* m_UniqueIds;
};

// Include the MOC generated file for this class
#include "moc_ReadStlFile.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadStlFile::ReadStlFile()
: AbstractFilter()
, m_SurfaceMeshDataContainerName(SIMPL::Defaults::TriangleDataContainerName)
, m_FaceAttributeMatrixName(SIMPL::Defaults::FaceAttributeMatrixName)
, m_StlFilePath("")
, m_FaceNormalsArrayName(SIMPL::FaceData::SurfaceMeshFaceNormals)
, m_FaceNormals(nullptr)
, m_minXcoord(std::numeric_limits<float>::max())
, m_maxXcoord(-std::numeric_limits<float>::max())
, m_minYcoord(std::numeric_limits<float>::max())
, m_maxYcoord(-std::numeric_limits<float>::max())
, m_minZcoord(std::numeric_limits<float>::max())
, m_maxZcoord(-std::numeric_limits<float>::max())
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadStlFile::~ReadStlFile()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadStlFile::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("STL File", StlFilePath, FilterParameter::Parameter, ReadStlFile, "*.stl", "STL File"));
  parameters.push_back(SIMPL_NEW_STRING_FP("Data Container", SurfaceMeshDataContainerName, FilterParameter::CreatedArray, ReadStlFile));
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Face Attribute Matrix", FaceAttributeMatrixName, FilterParameter::CreatedArray, ReadStlFile));
  parameters.push_back(SIMPL_NEW_STRING_FP("Face Normals", FaceNormalsArrayName, FilterParameter::CreatedArray, ReadStlFile));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadStlFile::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setStlFilePath(reader->readString("StlFilePath", getStlFilePath()));
  setFaceAttributeMatrixName(reader->readString("FaceAttributeMatrixName", getFaceAttributeMatrixName()));
  setSurfaceMeshDataContainerName(reader->readString("SurfaceMeshDataContainerName", getSurfaceMeshDataContainerName()));
  setFaceNormalsArrayName(reader->readString("FaceNormalsArrayName", getFaceNormalsArrayName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadStlFile::updateFaceInstancePointers()
{
  setErrorCondition(0);
  setWarningCondition(0);

  if(nullptr != m_FaceNormalsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FaceNormals = m_FaceNormalsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadStlFile::initialize()
{
  m_minXcoord = std::numeric_limits<float>::max();
  m_maxXcoord = -std::numeric_limits<float>::max();
  m_minYcoord = std::numeric_limits<float>::max();
  m_maxYcoord = -std::numeric_limits<float>::max();
  m_minZcoord = std::numeric_limits<float>::max();
  m_maxZcoord = -std::numeric_limits<float>::max();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadStlFile::dataCheck()
{
  initialize();
  setErrorCondition(0);
  setWarningCondition(0);

  DataArrayPath tempPath;

  if(m_StlFilePath.isEmpty() == true)
  {
    setErrorCondition(-1003);
    notifyErrorMessage(getHumanLabel(), "The input file must be set", -1003);
  }

  // Create a SufaceMesh Data Container with Faces, Vertices, Feature Labels and optionally Phase labels
  DataContainer::Pointer sm = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getSurfaceMeshDataContainerName());
  if(getErrorCondition() < 0)
  {
    return;
  }

  SharedVertexList::Pointer sharedVertList = TriangleGeom::CreateSharedVertexList(0);
  TriangleGeom::Pointer triangleGeom = TriangleGeom::CreateGeometry(0, sharedVertList, SIMPL::Geometry::TriangleGeometry, !getInPreflight());

  sm->setGeometry(triangleGeom);

  QVector<size_t> tDims(1, 0);
  sm->createNonPrereqAttributeMatrix<AbstractFilter>(this, getFaceAttributeMatrixName(), tDims, AttributeMatrix::Type::Face);

  QVector<size_t> cDims(1, 3);
  tempPath.update(getSurfaceMeshDataContainerName(), getFaceAttributeMatrixName(), getFaceNormalsArrayName());
  m_FaceNormalsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(this, tempPath, 0, cDims);
  if(nullptr != m_FaceNormalsPtr.lock().get())
  {
    m_FaceNormals = m_FaceNormalsPtr.lock()->getPointer(0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadStlFile::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadStlFile::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  readFile();
  eliminate_duplicate_nodes();

  setErrorCondition(0);
  setWarningCondition(0);
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadStlFile::readFile()
{
  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(m_SurfaceMeshDataContainerName);

  // Open File
  FILE* f = fopen(m_StlFilePath.toLatin1().data(), "rb");
  if(nullptr == f)
  {
    setErrorCondition(-1003);
    notifyErrorMessage(getHumanLabel(), "Error opening STL file", -1003);
    return;
  }

  // Read Header
  char h[STL_HEADER_LENGTH];
  int32_t triCount = 0;
  fread(h, STL_HEADER_LENGTH, 1, f);

  // Look for the tell-tale signs that the file was written from Magics Materialise
  // If the file was written by Magics as a "Color STL" file then the 2byte int
  // values between each triangle will be NON Zero which will screw up the reading.
  // This NON Zero value does NOT indicate a length but is some sort of color
  // value encoded into the file. Instead of being normal like everyone else and
  // using the STL spec they went off and did their own thing.
  QByteArray headerArray(h, STL_HEADER_LENGTH);
  QString headerString(headerArray);
  bool magicsFile = false;
  static const QString k_ColorHeader("COLOR=");
  static const QString k_MaterialHeader("MATERIAL=");
  if(headerString.contains(k_ColorHeader) && headerString.contains(k_MaterialHeader))
  {
    magicsFile = true;
  }
  // Read the number of triangles in the file.
  fread(&triCount, sizeof(int32_t), 1, f);

  TriangleGeom::Pointer triangleGeom = sm->getGeometryAs<TriangleGeom>();
  triangleGeom->resizeTriList(triCount);
  triangleGeom->resizeVertexList(triCount * 3);
  float* nodes = triangleGeom->getVertexPointer(0);
  int64_t* triangles = triangleGeom->getTriPointer(0);

  // Resize the triangle attribute matrix to hold the normals and update the normals pointer
  QVector<size_t> tDims(1, static_cast<size_t>(triCount));
  sm->getAttributeMatrix(getFaceAttributeMatrixName())->resizeAttributeArrays(tDims);
  updateFaceInstancePointers();

  // Read the triangles
  static const size_t k_StlElementCount = 12;
  float v[k_StlElementCount];
  unsigned short attr;
  for(int32_t t = 0; t < triCount; ++t)
  {
    fread(reinterpret_cast<void*>(v), sizeof(float), k_StlElementCount, f); // Read the Triangle

    fread(reinterpret_cast<void*>(&attr), sizeof(unsigned short), 1, f); // Read the Triangle Attribute Data length
    if(attr > 0 && !magicsFile)
    {
      std::vector<unsigned char> buffer(attr);                       // Allocate a buffer for the STL attribute data to be placed into
      fread(reinterpret_cast<void*>(&(buffer.front())), attr, 1, f); // Read the bytes into the buffer so that we can skip it.
    }
    if(v[3] < m_minXcoord)
    {
      m_minXcoord = v[3];
    }
    if(v[3] > m_maxXcoord)
    {
      m_maxXcoord = v[3];
    }
    if(v[4] < m_minYcoord)
    {
      m_minYcoord = v[4];
    }
    if(v[4] > m_maxYcoord)
    {
      m_maxYcoord = v[4];
    }
    if(v[5] < m_minZcoord)
    {
      m_minZcoord = v[5];
    }
    if(v[5] > m_maxZcoord)
    {
      m_maxZcoord = v[5];
    }
    if(v[6] < m_minXcoord)
    {
      m_minXcoord = v[6];
    }
    if(v[6] > m_maxXcoord)
    {
      m_maxXcoord = v[6];
    }
    if(v[7] < m_minYcoord)
    {
      m_minYcoord = v[7];
    }
    if(v[7] > m_maxYcoord)
    {
      m_maxYcoord = v[7];
    }
    if(v[8] < m_minZcoord)
    {
      m_minZcoord = v[8];
    }
    if(v[8] > m_maxZcoord)
    {
      m_maxZcoord = v[8];
    }
    if(v[9] < m_minXcoord)
    {
      m_minXcoord = v[9];
    }
    if(v[9] > m_maxXcoord)
    {
      m_maxXcoord = v[9];
    }
    if(v[10] < m_minYcoord)
    {
      m_minYcoord = v[10];
    }
    if(v[10] > m_maxYcoord)
    {
      m_maxYcoord = v[10];
    }
    if(v[11] < m_minZcoord)
    {
      m_minZcoord = v[11];
    }
    if(v[11] > m_maxZcoord)
    {
      m_maxZcoord = v[11];
    }
    m_FaceNormals[3 * t + 0] = static_cast<double>(v[0]);
    m_FaceNormals[3 * t + 1] = static_cast<double>(v[1]);
    m_FaceNormals[3 * t + 2] = static_cast<double>(v[2]);
    nodes[3 * (3 * t + 0) + 0] = v[3];
    nodes[3 * (3 * t + 0) + 1] = v[4];
    nodes[3 * (3 * t + 0) + 2] = v[5];
    nodes[3 * (3 * t + 1) + 0] = v[6];
    nodes[3 * (3 * t + 1) + 1] = v[7];
    nodes[3 * (3 * t + 1) + 2] = v[8];
    nodes[3 * (3 * t + 2) + 0] = v[9];
    nodes[3 * (3 * t + 2) + 1] = v[10];
    nodes[3 * (3 * t + 2) + 2] = v[11];
    triangles[t * 3] = 3 * t + 0;
    triangles[t * 3 + 1] = 3 * t + 1;
    triangles[t * 3 + 2] = 3 * t + 2;
  }

  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadStlFile::eliminate_duplicate_nodes()
{
  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(m_SurfaceMeshDataContainerName);

  TriangleGeom::Pointer triangleGeom = sm->getGeometryAs<TriangleGeom>();
  float* vertex = triangleGeom->getVertexPointer(0);
  int64_t nNodes_ = triangleGeom->getNumberOfVertices();
  int64_t* triangles = triangleGeom->getTriPointer(0);
  int64_t nTriangles = triangleGeom->getNumberOfTris();
  size_t nNodes = 0;
  if(nNodes_ > 0)
  {
    nNodes = static_cast<size_t>(nNodes_);
  }
  float stepX = (m_maxXcoord - m_minXcoord) / 100.0f;
  float stepY = (m_maxYcoord - m_minYcoord) / 100.0f;
  float stepZ = (m_maxZcoord - m_minZcoord) / 100.0f;

  QVector<QVector<size_t>> nodesInBin(100 * 100 * 100);

  // determine (xyz) bin each node falls in - used to speed up node comparison
  int32_t bin = 0, xBin = 0, yBin = 0, zBin = 0;
  for(size_t i = 0; i < nNodes; i++)
  {
    xBin = static_cast<int32_t>((vertex[i * 3] - m_minXcoord) / stepX);
    yBin = static_cast<int32_t>((vertex[i * 3 + 1] - m_minYcoord) / stepY);
    zBin = static_cast<int32_t>((vertex[i * 3 + 2] - m_minZcoord) / stepZ);
    if(xBin == 100)
    {
      xBin = 99;
    }
    if(yBin == 100)
    {
      yBin = 99;
    }
    if(zBin == 100)
    {
      zBin = 99;
    }
    bin = (zBin * 10000) + (yBin * 100) + xBin;
    nodesInBin[bin].push_back(i);
  }

  // Create array to hold unique node numbers
  Int64ArrayType::Pointer uniqueIdsPtr = Int64ArrayType::CreateArray(nNodes, "uniqueIds");
  int64_t* uniqueIds = uniqueIdsPtr->getPointer(0);
  for(int64_t i = 0; i < nNodes_; i++)
  {
    uniqueIds[i] = i;
  }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

// Parallel algorithm to find duplicate nodes
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  if(doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, 100 * 100 * 100), FindUniqueIdsImpl(triangleGeom->getVertices(), nodesInBin, uniqueIds), tbb::auto_partitioner());
  }
  else
#endif
  {
    FindUniqueIdsImpl serial(triangleGeom->getVertices(), nodesInBin, uniqueIds);
    serial.convert(0, 100 * 100 * 100);
  }

  // renumber the unique nodes
  int64_t uniqueCount = 0;
  for(size_t i = 0; i < nNodes; i++)
  {
    if(uniqueIds[i] == static_cast<int64_t>(i))
    {
      uniqueIds[i] = uniqueCount;
      uniqueCount++;
    }
    else
    {
      uniqueIds[i] = uniqueIds[uniqueIds[i]];
    }
  }

  // Move nodes to unique Id and then resize nodes array
  for(size_t i = 0; i < nNodes; i++)
  {
    vertex[uniqueIds[i] * 3] = vertex[i * 3];
    vertex[uniqueIds[i] * 3 + 1] = vertex[i * 3 + 1];
    vertex[uniqueIds[i] * 3 + 2] = vertex[i * 3 + 2];
  }
  triangleGeom->resizeVertexList(uniqueCount);

  // Update the triangle nodes to reflect the unique ids
  int64_t node1 = 0, node2 = 0, node3 = 0;
  for(size_t i = 0; i < static_cast<size_t>(nTriangles); i++)
  {
    node1 = triangles[i * 3];
    node2 = triangles[i * 3 + 1];
    node3 = triangles[i * 3 + 2];

    triangles[i * 3] = uniqueIds[node1];
    triangles[i * 3 + 1] = uniqueIds[node2];
    triangles[i * 3 + 2] = uniqueIds[node3];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ReadStlFile::newFilterInstance(bool copyFilterParameters)
{
  ReadStlFile::Pointer filter = ReadStlFile::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadStlFile::getCompiledLibraryName()
{
  return IOConstants::IOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadStlFile::getBrandingString()
{
  return "IO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadStlFile::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << IO::Version::Major() << "." << IO::Version::Minor() << "." << IO::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadStlFile::getGroupName()
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadStlFile::getSubGroupName()
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadStlFile::getHumanLabel()
{
  return "Import STL File";
}
