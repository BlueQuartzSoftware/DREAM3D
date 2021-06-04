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

#include <cstdio>
#include <tuple>

#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Utilities/ParallelDataAlgorithm.h"

#include "ImportExport/ImportExportConstants.h"
#include "ImportExport/ImportExportVersion.h"

#define STL_HEADER_LENGTH 80

enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID21 = 21,

  DataArrayID31 = 31,

  DataContainerID = 1
};

namespace ReadStlFileErrors
{
constexpr int32_t k_InputFileNotSet = -1100;
constexpr int32_t k_InputFileDoesNotExist = -1101;
constexpr int32_t k_UnsupportedFileType = -1102;
constexpr int32_t k_ErrorOpeningFile = -1103;
constexpr int32_t k_StlHeaderParseError = -1104;
constexpr int32_t k_TriangleCountParseError = -1105;
constexpr int32_t k_TriangleParseError = -1106;
constexpr int32_t k_AttributeParseError = -1107;
} // namespace ReadStlFileErrors

/**
 * @brief The FindUniqueIdsImpl class implements a threaded algorithm that determines the set of
 * unique vertices in the triangle geometry
 */
class FindUniqueIdsImpl
{
public:
  FindUniqueIdsImpl(SharedVertexList::Pointer vertex, const std::vector<std::vector<size_t>>& nodesInBin, int64_t* uniqueIds)
  : m_Vertex(vertex)
  , m_NodesInBin(nodesInBin)
  , m_UniqueIds(uniqueIds)
  {
  }

  // -----------------------------------------------------------------------------
  void convert(size_t start, size_t end) const
  {
    float* verts = m_Vertex->getPointer(0);
    for(size_t i = start; i < end; i++)
    {
      for(size_t j = 0; j < m_NodesInBin[i].size(); j++)
      {
        size_t node1 = m_NodesInBin[i][j];
        if(m_UniqueIds[node1] == static_cast<int64_t>(node1))
        {
          for(size_t k = j + 1; k < m_NodesInBin[i].size(); k++)
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

  // -----------------------------------------------------------------------------
  void operator()(const SIMPLRange& range) const
  {
    convert(range.min(), range.max());
  }

private:
  SharedVertexList::Pointer m_Vertex;
  const std::vector<std::vector<size_t>>& m_NodesInBin;
  int64_t* m_UniqueIds = nullptr;
};

// -----------------------------------------------------------------------------
// Returns 0 for Binary, 1 for ASCII, anything else is an error.
int32_t getStlFileType(const std::string& path)
{
  // Open File
  FILE* f = std::fopen(path.c_str(), "rb");
  if(nullptr == f)
  {
    return ReadStlFileErrors::k_ErrorOpeningFile;
  }

  // Read the first 256 bytes of data, that should be enough but I'm sure someone will write
  // an ASCII STL File that contains a really long name which messes this up.
  std::string header(STL_HEADER_LENGTH, 0x00);
  if(std::fread(header.data(), 1, STL_HEADER_LENGTH, f) != STL_HEADER_LENGTH)
  {
    std::ignore = fclose(f);
    return ReadStlFileErrors::k_StlHeaderParseError;
  }
  // close the file
  std::ignore = fclose(f);


  size_t solid_pos = header.find("solid", 0);
  // The word 'solid' was not found ANYWHERE in the first 80 bytes.
  if(solid_pos == std::string::npos)
  {
    return 0;
  }
  // 'solid' was found as the first 5 bytes of the header. This is am ambiguous case so let's try to find 'facet'
  if(solid_pos == 0)
  {
    size_t facet_pos = header.find("facet", solid_pos + 6);
    if(facet_pos == std::string::npos)
    {
      // 'facet' was NOT found so this is a binary file.
      return 0;
    }
    return 1;
  }

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadStlFile::ReadStlFile() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadStlFile::~ReadStlFile() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadStlFile::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("STL File", StlFilePath, FilterParameter::Category::Parameter, ReadStlFile, "*.stl", "STL File"));
  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Data Container", SurfaceMeshDataContainerName, FilterParameter::Category::CreatedArray, ReadStlFile));
  parameters.push_back(SeparatorFilterParameter::Create("Face Data", FilterParameter::Category::CreatedArray));
  parameters.push_back(SIMPL_NEW_AM_WITH_LINKED_DC_FP("Face Attribute Matrix", FaceAttributeMatrixName, SurfaceMeshDataContainerName, FilterParameter::Category::CreatedArray, ReadStlFile));
  parameters.push_back(
      SIMPL_NEW_DA_WITH_LINKED_AM_FP("Face Normals", FaceNormalsArrayName, SurfaceMeshDataContainerName, FaceAttributeMatrixName, FilterParameter::Category::CreatedArray, ReadStlFile));
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
  setSurfaceMeshDataContainerName(reader->readDataArrayPath("SurfaceMeshDataContainerName", getSurfaceMeshDataContainerName()));
  setFaceNormalsArrayName(reader->readString("FaceNormalsArrayName", getFaceNormalsArrayName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadStlFile::updateFaceInstancePointers()
{
  clearErrorCode();
  clearWarningCode();

  if(nullptr != m_FaceNormalsPtr.lock())
  {
    m_FaceNormals = m_FaceNormalsPtr.lock()->getPointer(0);
  }
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
  clearErrorCode();
  clearWarningCode();

  initialize();
  DataArrayPath tempPath;

  QFileInfo fi(getStlFilePath());

  if(getStlFilePath().isEmpty())
  {
    QString ss = QObject::tr("The input file must be set");
    setErrorCondition(ReadStlFileErrors::k_InputFileNotSet, ss);
  }
  else if(!fi.exists())
  {
    QString ss = QObject::tr("The input file does not exist");
    setErrorCondition(ReadStlFileErrors::k_InputFileDoesNotExist, ss);
  }

  int32_t fileType = getStlFileType(getStlFilePath().toStdString());
  if(fileType == 1)
  {
    QString ss = QObject::tr("The Input STL File is ASCII which is not currently supported. Please convert it to a binary STL file using another program.");
    setErrorCondition(ReadStlFileErrors::k_UnsupportedFileType, ss);
  }
  else if(fileType < 0)
  {
    QString ss = QObject::tr("Error reading the STL file.");
    setErrorCondition(fileType, ss);
  }
  if(getErrorCode() < 0)
  {
    return;
  }

  // Create a SufaceMesh Data Container with Faces, Vertices, Feature Labels and optionally Phase labels
  DataContainer::Pointer sm = getDataContainerArray()->createNonPrereqDataContainer(this, getSurfaceMeshDataContainerName(), DataContainerID);
  if(getErrorCode() < 0)
  {
    return;
  }

  SharedVertexList::Pointer sharedVertList = TriangleGeom::CreateSharedVertexList(0);
  TriangleGeom::Pointer triangleGeom = TriangleGeom::CreateGeometry(0, sharedVertList, SIMPL::Geometry::TriangleGeometry, !getInPreflight());

  sm->setGeometry(triangleGeom);

  std::vector<size_t> tDims(1, 0);
  sm->createNonPrereqAttributeMatrix(this, getFaceAttributeMatrixName(), tDims, AttributeMatrix::Type::Face, AttributeMatrixID21);

  std::vector<size_t> cDims(1, 3);
  tempPath.update(getSurfaceMeshDataContainerName().getDataContainerName(), getFaceAttributeMatrixName(), getFaceNormalsArrayName());
  m_FaceNormalsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>>(this, tempPath, 0, cDims, "", DataArrayID31);
  if(nullptr != m_FaceNormalsPtr.lock())
  {
    m_FaceNormals = m_FaceNormalsPtr.lock()->getPointer(0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadStlFile::execute()
{
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  readFile();
  eliminate_duplicate_nodes();

  clearErrorCode();
  clearWarningCode();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadStlFile::readFile()
{
  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(m_SurfaceMeshDataContainerName);

  // Open File
  FILE* f = std::fopen(m_StlFilePath.toLatin1().data(), "rb");
  if(nullptr == f)
  {
    setErrorCondition(ReadStlFileErrors::k_ErrorOpeningFile, "Error opening STL file");
    return;
  }

  // Read Header
  char h[STL_HEADER_LENGTH];
  int32_t triCount = 0;
  if(std::fread(h, STL_HEADER_LENGTH, 1, f) != 1)
  {
    QString msg = QString("Error reading first 8 bytes of STL header. This can't be good.");
    setErrorCondition(ReadStlFileErrors::k_StlHeaderParseError, msg);
    std::ignore = fclose(f);
    return;
  }

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
  if(std::fread(&triCount, sizeof(int32_t), 1, f) != 1)
  {
    QString msg = QString("Error reading number of triangles from file. This is bad.");
    setErrorCondition(ReadStlFileErrors::k_TriangleCountParseError, msg);
    std::ignore = fclose(f);
    return;
  }

  TriangleGeom::Pointer triangleGeom = sm->getGeometryAs<TriangleGeom>();
  triangleGeom->resizeTriList(triCount);
  triangleGeom->resizeVertexList(triCount * 3);
  float* nodes = triangleGeom->getVertexPointer(0);
  MeshIndexType* triangles = triangleGeom->getTriPointer(0);

  // Resize the triangle attribute matrix to hold the normals and update the normals pointer
  std::vector<size_t> tDims(1, static_cast<size_t>(triCount));
  sm->getAttributeMatrix(getFaceAttributeMatrixName())->resizeAttributeArrays(tDims);
  updateFaceInstancePointers();

  // Read the triangles
  static const size_t k_StlElementCount = 12;
  float v[k_StlElementCount];
  uint16_t attr;
  std::vector<uint8_t> triangleAttributeBuffer(std::numeric_limits<uint16_t>::max()); // Just allocate a buffer of max UINT16 elements
  for(int32_t t = 0; t < triCount; ++t)
  {
    size_t objsRead = std::fread(reinterpret_cast<void*>(v), sizeof(float), k_StlElementCount, f); // Read the Triangle
    if(k_StlElementCount != objsRead)
    {
      QString msg = QString("Error reading Triangle '%1'. Object Count was %2 and should have been %3").arg(t, objsRead, k_StlElementCount);
      setErrorCondition(ReadStlFileErrors::k_TriangleParseError, msg);
      std::ignore = fclose(f);
      return;
    }

    objsRead = std::fread(reinterpret_cast<void*>(&attr), sizeof(uint16_t), 1, f); // Read the Triangle Attribute Data length
    if(objsRead != 1)
    {
      QString msg = QString("Error reading Number of attributes for triangle '%1'. Object Count was %2 and should have been 1").arg(t, objsRead);
      setErrorCondition(ReadStlFileErrors::k_AttributeParseError, msg);
      std::ignore = fclose(f);
      return;
    }
    if(attr > 0 && !magicsFile)
    {
      std::ignore = std::fseek(f, static_cast<size_t>(attr), SEEK_CUR); // Skip past the Triangle Attribute data since we don't know how to read it anyways
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
    if(getCancel())
    {
      std::ignore = fclose(f);
      return;
    }
  }

  std::ignore = fclose(f);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadStlFile::eliminate_duplicate_nodes()
{
  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(m_SurfaceMeshDataContainerName);

  TriangleGeom::Pointer triangleGeom = sm->getGeometryAs<TriangleGeom>();
  float* vertex = triangleGeom->getVertexPointer(0);
  MeshIndexType nNodes_ = triangleGeom->getNumberOfVertices();
  MeshIndexType* triangles = triangleGeom->getTriPointer(0);
  MeshIndexType nTriangles = triangleGeom->getNumberOfTris();
  size_t nNodes = 0;
  if(nNodes_ > 0)
  {
    nNodes = static_cast<size_t>(nNodes_);
  }
  float stepX = (m_maxXcoord - m_minXcoord) / 100.0f;
  float stepY = (m_maxYcoord - m_minYcoord) / 100.0f;
  float stepZ = (m_maxZcoord - m_minZcoord) / 100.0f;

  std::vector<std::vector<size_t>> nodesInBin(100 * 100 * 100);

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
  Int64ArrayType::Pointer uniqueIdsPtr = Int64ArrayType::CreateArray(nNodes, std::string("uniqueIds"), true);
  int64_t* uniqueIds = uniqueIdsPtr->getPointer(0);
  for(MeshIndexType i = 0; i < nNodes_; i++)
  {
    uniqueIds[i] = static_cast<int64_t>(i);
  }

  // Parallel algorithm to find duplicate nodes
  ParallelDataAlgorithm dataAlg;
  dataAlg.setRange(0ULL, static_cast<size_t>(100 * 100 * 100));
  dataAlg.execute(FindUniqueIdsImpl(triangleGeom->getVertices(), nodesInBin, uniqueIds));

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
AbstractFilter::Pointer ReadStlFile::newFilterInstance(bool copyFilterParameters) const
{
  ReadStlFile::Pointer filter = ReadStlFile::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReadStlFile::getCompiledLibraryName() const
{
  return ImportExportConstants::ImportExportBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReadStlFile::getBrandingString() const
{
  return "IO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReadStlFile::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << ImportExport::Version::Major() << "." << ImportExport::Version::Minor() << "." << ImportExport::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReadStlFile::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid ReadStlFile::getUuid() const
{
  return QUuid("{980c7bfd-20b2-5711-bc3b-0190b9096c34}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReadStlFile::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReadStlFile::getHumanLabel() const
{
  return "Import STL File";
}

// -----------------------------------------------------------------------------
ReadStlFile::Pointer ReadStlFile::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ReadStlFile> ReadStlFile::New()
{
  struct make_shared_enabler : public ReadStlFile
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ReadStlFile::getNameOfClass() const
{
  return QString("ReadStlFile");
}

// -----------------------------------------------------------------------------
QString ReadStlFile::ClassName()
{
  return QString("ReadStlFile");
}

// -----------------------------------------------------------------------------
void ReadStlFile::setSurfaceMeshDataContainerName(const DataArrayPath& value)
{
  m_SurfaceMeshDataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ReadStlFile::getSurfaceMeshDataContainerName() const
{
  return m_SurfaceMeshDataContainerName;
}

// -----------------------------------------------------------------------------
void ReadStlFile::setFaceAttributeMatrixName(const QString& value)
{
  m_FaceAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString ReadStlFile::getFaceAttributeMatrixName() const
{
  return m_FaceAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void ReadStlFile::setStlFilePath(const QString& value)
{
  m_StlFilePath = value;
}

// -----------------------------------------------------------------------------
QString ReadStlFile::getStlFilePath() const
{
  return m_StlFilePath;
}

// -----------------------------------------------------------------------------
void ReadStlFile::setFaceNormalsArrayName(const QString& value)
{
  m_FaceNormalsArrayName = value;
}

// -----------------------------------------------------------------------------
QString ReadStlFile::getFaceNormalsArrayName() const
{
  return m_FaceNormalsArrayName;
}
