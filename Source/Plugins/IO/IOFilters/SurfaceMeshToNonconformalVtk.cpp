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

#include "SurfaceMeshToNonconformalVtk.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Utilities/SIMPLibEndian.h"

#include "IO/IOConstants.h"
#include "IO/IOVersion.h"

// Include the MOC generated file for this class
#include "moc_SurfaceMeshToNonconformalVtk.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshToNonconformalVtk::SurfaceMeshToNonconformalVtk()
: AbstractFilter()
, m_OutputVtkFile("")
, m_WriteBinaryFile(false)
, m_SurfaceMeshFaceLabelsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceLabels)
, m_SurfaceMeshNodeTypeArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::VertexAttributeMatrixName, SIMPL::VertexData::SurfaceMeshNodeType)
, m_SurfaceMeshFaceLabels(nullptr)
, m_SurfaceMeshNodeType(nullptr)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshToNonconformalVtk::~SurfaceMeshToNonconformalVtk()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToNonconformalVtk::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;

  parameters.push_back(SIMPL_NEW_OUTPUT_FILE_FP("Output Vtk File", OutputVtkFile, FilterParameter::Parameter, SurfaceMeshToNonconformalVtk));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Write Binary Vtk File", WriteBinaryFile, FilterParameter::Parameter, SurfaceMeshToNonconformalVtk));
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("SurfaceMeshFaceLabels", SurfaceMeshFaceLabelsArrayPath, FilterParameter::RequiredArray, SurfaceMeshToNonconformalVtk, req));
  }

  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("SurfaceMeshNodeType", SurfaceMeshNodeTypeArrayPath, FilterParameter::RequiredArray, SurfaceMeshToNonconformalVtk, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToNonconformalVtk::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSurfaceMeshNodeTypeArrayPath(reader->readDataArrayPath("SurfaceMeshNodeTypeArrayPath", getSurfaceMeshNodeTypeArrayPath()));
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath()));
  setOutputVtkFile(reader->readString("OutputVtkFile", getOutputVtkFile()));
  setWriteBinaryFile(reader->readValue("WriteBinaryFile", getWriteBinaryFile()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToNonconformalVtk::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToNonconformalVtk::dataCheck()
{
  setErrorCondition(0);
  QString ss;

  if(m_OutputVtkFile.isEmpty() == true)
  {
    setErrorCondition(-1003);
    notifyErrorMessage(getHumanLabel(), "Vtk Output file is Not set correctly", -1003);
  }

  DataContainer::Pointer sm = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), false);
  if(getErrorCondition() < 0)
  {
    return;
  }

  TriangleGeom::Pointer triangles = sm->getPrereqGeometry<TriangleGeom, AbstractFilter>(this);
  if(getErrorCondition() < 0)
  {
    return;
  }

  // We MUST have Nodes
  if(nullptr == triangles->getVertices().get())
  {
    setErrorCondition(-386);
    notifyErrorMessage(getHumanLabel(), "DataContainer Geometry missing Vertices", getErrorCondition());
  }
  // We MUST have Triangles defined also.
  if(nullptr == triangles->getTriangles().get())
  {
    setErrorCondition(-387);
    notifyErrorMessage(getHumanLabel(), "DataContainer Geometry missing Triangles", getErrorCondition());
  }
  QVector<size_t> dims(1, 2);
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(),
                                                                                                                   dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshFaceLabelsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  m_SurfaceMeshNodeTypePtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter>(this, getSurfaceMeshNodeTypeArrayPath(),
                                                                                                                dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshNodeTypePtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshNodeType = m_SurfaceMeshNodeTypePtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToNonconformalVtk::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

/**
 * @brief The ScopedFileMonitor class will automatically close an open FILE pointer
 * when the object goes out of scope.
 */
class ScopedFileMonitor
{
public:
  ScopedFileMonitor(FILE* f)
  : m_File(f)
  {
  }
  virtual ~ScopedFileMonitor()
  {
    fclose(m_File);
  }

private:
  FILE* m_File;
  ScopedFileMonitor(const ScopedFileMonitor&); // Copy Constructor Not Implemented
  void operator=(const ScopedFileMonitor&);    // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToNonconformalVtk::execute()
{
  int err = 0;
  setErrorCondition(err);

  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName());

  TriangleGeom::Pointer triangleGeom = getDataContainerArray()->getDataContainer(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName())->getGeometryAs<TriangleGeom>();
  float* nodes = triangleGeom->getVertexPointer(0);
  int64_t* triangles = triangleGeom->getTriPointer(0);

  qint64 numNodes = triangleGeom->getNumberOfVertices();
  int64_t numTriangles = triangleGeom->getNumberOfTris();

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(getOutputVtkFile());
  QDir parentPath = fi.path();
  if(!parentPath.mkpath("."))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath.absolutePath());
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  // Open the output VTK File for writing
  FILE* vtkFile = nullptr;
  vtkFile = fopen(getOutputVtkFile().toLatin1().data(), "wb");
  if(nullptr == vtkFile)
  {
    QString ss = QObject::tr("Error creating file '%1'").arg(getOutputVtkFile());
    setErrorCondition(-18542);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  ScopedFileMonitor vtkFileMonitor(vtkFile);

  notifyStatusMessage(getHumanLabel(), "Writing Vertex Data ....");

  fprintf(vtkFile, "# vtk DataFile Version 2.0\n");
  fprintf(vtkFile, "Data set from DREAM.3D Surface Meshing Module\n");
  if(m_WriteBinaryFile)
  {
    fprintf(vtkFile, "BINARY\n");
  }
  else
  {
    fprintf(vtkFile, "ASCII\n");
  }
  fprintf(vtkFile, "DATASET POLYDATA\n");

  int numberWrittenNodes = 0;
  for(int i = 0; i < numNodes; i++)
  {
    //  Node& n = nodes[i]; // Get the current Node
    if(m_SurfaceMeshNodeType[i] > 0)
    {
      ++numberWrittenNodes;
    }
    else
    {
      qDebug() << "Node Type Invalid: " << i << "::" << (int)(m_SurfaceMeshNodeType[i]);
    }
  }

  fprintf(vtkFile, "POINTS %d float\n", numberWrittenNodes);

  float pos[3] = {0.0f, 0.0f, 0.0f};

  size_t totalWritten = 0;

  // Write the POINTS data (Vertex)
  for(int i = 0; i < numNodes; i++)
  {
    if(m_SurfaceMeshNodeType[i] > 0)
    {
      pos[0] = static_cast<float>(nodes[i * 3]);
      pos[1] = static_cast<float>(nodes[i * 3 + 1]);
      pos[2] = static_cast<float>(nodes[i * 3 + 2]);

      if(m_WriteBinaryFile == true)
      {
        SIMPLib::Endian::FromSystemToBig::convert(pos[0]);
        SIMPLib::Endian::FromSystemToBig::convert(pos[1]);
        SIMPLib::Endian::FromSystemToBig::convert(pos[2]);
        totalWritten = fwrite(pos, sizeof(float), 3, vtkFile);
        if(totalWritten != 3)
        {
        }
      }
      else
      {
        fprintf(vtkFile, "%f %f %f\n", pos[0], pos[1], pos[2]); // Write the positions to the output file
      }
    }
  }

  // Write the triangle indices into the vtk File
  notifyStatusMessage(getHumanLabel(), "Writing Faces ....");

  int tData[4];

  // Store all the unique Spins
  QMap<int32_t, int32_t> featureTriangleCount;
  for(int i = 0; i < numTriangles; i++)
  {
    if(featureTriangleCount.find(m_SurfaceMeshFaceLabels[i * 2]) == featureTriangleCount.end())
    {
      featureTriangleCount[m_SurfaceMeshFaceLabels[i * 2]] = 1;
    }
    else
    {
      featureTriangleCount[m_SurfaceMeshFaceLabels[i * 2]]++;
    }
    if(featureTriangleCount.find(m_SurfaceMeshFaceLabels[i * 2 + 1]) == featureTriangleCount.end())
    {
      featureTriangleCount[m_SurfaceMeshFaceLabels[i * 2 + 1]] = 1;
    }
    else
    {
      featureTriangleCount[m_SurfaceMeshFaceLabels[i * 2 + 1]]++;
    }
  }

  // Write the POLYGONS
  fprintf(vtkFile, "\nPOLYGONS %lld %lld\n", (long long int)(numTriangles * 2), (long long int)(numTriangles * 2 * 4));

  size_t totalCells = 0;
  // Loop over all the features
  for(QMap<int32_t, int32_t>::iterator featureIter = featureTriangleCount.begin(); featureIter != featureTriangleCount.end(); ++featureIter)
  {
    totalCells += featureIter.value();
  }
  Q_ASSERT(totalCells == (size_t)(numTriangles * 2));

  // Loop over all the features
  for(QMap<int32_t, int32_t>::iterator featureIter = featureTriangleCount.begin(); featureIter != featureTriangleCount.end(); ++featureIter)
  {
    int32_t gid = featureIter.key();             // The current Feature Id
    int32_t numTriToWrite = featureIter.value(); // The number of triangles for this feature
    uint8_t doWrite = 0;

    // Loop over all the triangles looking for the current feature id
    // this is probably sub-optimal as if we have 1000 features we are going to loop 1000 times but this will use the
    // least amount of memory. We could run a filter to group the triangles by feature but then we would need an
    // additional amount of memory equal to 3X the memory used for the triangle list because every triangle will be listed
    // twice. We could get some slightly better performance if we buffered 4K worth of data then wrote out that data
    // in one chunk versus what we are doing here.
    for(int j = 0; j < numTriangles; j++)
    {
      doWrite = 0;

      if(m_SurfaceMeshFaceLabels[j * 2] == gid)
      {
        doWrite = 1;
      }
      else if(m_SurfaceMeshFaceLabels[j * 2 + 1] == gid)
      {
        doWrite = 2;
      } // We need to flip the winding of the triangle
      if(doWrite == 0)
      {
        continue;
      } // Labels in the triangle did match the current feature id.

      if(doWrite == 1)
      {
        tData[0] = 3; // Push on the total number of entries for this entry
        tData[1] = triangles[j * 3];
        tData[2] = triangles[j * 3 + 1];
        tData[3] = triangles[j * 3 + 2];
      }
      else
      {
        tData[0] = 3; // Push on the total number of entries for this entry
        tData[1] = triangles[j * 3 + 2];
        tData[2] = triangles[j * 3 + 1];
        tData[3] = triangles[j * 3];
      }
      if(m_WriteBinaryFile == true)
      {

        SIMPLib::Endian::FromSystemToBig::convert(tData[0]);
        SIMPLib::Endian::FromSystemToBig::convert(tData[1]); // Index of Vertex 0
        SIMPLib::Endian::FromSystemToBig::convert(tData[2]); // Index of Vertex 1
        SIMPLib::Endian::FromSystemToBig::convert(tData[3]); // Index of Vertex 2
        fwrite(tData, sizeof(int), 4, vtkFile);
      }
      else
      {
        fprintf(vtkFile, "3 %d %d %d\n", tData[1], tData[2], tData[3]);
      }
      numTriToWrite--;
    }
    if(numTriToWrite != 0)
    {
      qDebug() << "Not enough triangles written: " << gid << "::" << numTriToWrite << " Total Triangles to Write " << featureIter.value();
    }
  }

  // Write the POINT_DATA section
  err = writePointData(vtkFile);

  // Write the CELL_DATA section
  err = writeCellData(vtkFile, featureTriangleCount);

  fprintf(vtkFile, "\n");

  setErrorCondition(0);
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
void writePointScalarData(DataContainer::Pointer dc, const QString& vertexAttributeMatrixName, const QString& dataName, const QString& dataType, bool writeBinaryData, FILE* vtkFile, int nT)
{
  IDataArray::Pointer data = dc->getAttributeMatrix(vertexAttributeMatrixName)->getAttributeArray(dataName);
  QString ss;
  if(nullptr != data.get())
  {
    T* m = reinterpret_cast<T*>(data->getVoidPointer(0));
    fprintf(vtkFile, "\n");
    fprintf(vtkFile, "SCALARS %s %s\n", dataName.toLatin1().data(), dataType.toLatin1().data());
    fprintf(vtkFile, "LOOKUP_TABLE default\n");
    for(int i = 0; i < nT; ++i)
    {
      T swapped = 0x00;
      if(writeBinaryData == true)
      {
        swapped = static_cast<T>(m[i]);
        SIMPLib::Endian::FromSystemToBig::convert(swapped);
        fwrite(&swapped, sizeof(T), 1, vtkFile);
      }
      else
      {

        ss = QString::number(m[i]) + " ";
        fprintf(vtkFile, "%s ", ss.toLatin1().data());
        // if (i%50 == 0)
        {
          fprintf(vtkFile, "\n");
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
void writePointVectorData(DataContainer::Pointer dc, const QString& vertexAttributeMatrixName, const QString& dataName, const QString& dataType, bool writeBinaryData, const QString& vtkAttributeType,
                          FILE* vtkFile, int nT)
{
  IDataArray::Pointer data = dc->getAttributeMatrix(vertexAttributeMatrixName)->getAttributeArray(dataName);
  QString buf;
  QTextStream ss(&buf);
  if(nullptr != data.get())
  {
    T* m = reinterpret_cast<T*>(data->getVoidPointer(0));
    fprintf(vtkFile, "\n");
    fprintf(vtkFile, "%s %s %s\n", vtkAttributeType.toLatin1().data(), dataName.toLatin1().data(), dataType.toLatin1().data());
    for(int i = 0; i < nT; ++i)
    {
      T s0 = 0x00;
      T s1 = 0x00;
      T s2 = 0x00;
      if(writeBinaryData == true)
      {
        s0 = static_cast<T>(m[i * 3 + 0]);
        s1 = static_cast<T>(m[i * 3 + 1]);
        s2 = static_cast<T>(m[i * 3 + 2]);
        SIMPLib::Endian::FromSystemToBig::convert(s0);
        SIMPLib::Endian::FromSystemToBig::convert(s1);
        SIMPLib::Endian::FromSystemToBig::convert(s2);
        fwrite(&s0, sizeof(T), 1, vtkFile);
        fwrite(&s1, sizeof(T), 1, vtkFile);
        fwrite(&s1, sizeof(T), 1, vtkFile);
      }
      else
      {

        ss << m[i * 3 + 0] << " " << m[i * 3 + 1] << " " << m[i * 3 + 2] << " ";
        fprintf(vtkFile, "%s ", buf.toLatin1().data());
        buf.clear();
        // if (i%50 == 0)
        {
          fprintf(vtkFile, "\n");
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshToNonconformalVtk::writePointData(FILE* vtkFile)
{
  int err = 0;
  if(nullptr == vtkFile)
  {
    return -1;
  }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(m_SurfaceMeshNodeTypeArrayPath.getDataContainerName());

  TriangleGeom::Pointer triangleGeom = getDataContainerArray()->getDataContainer(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName())->getGeometryAs<TriangleGeom>();

  // Get the Number of Vertex points in the mesh with a valid node type
  qint64 numNodes = triangleGeom->getNumberOfVertices();

  int nNodes = 0;
  // int swapped;
  for(int i = 0; i < numNodes; i++)
  {
    if(m_SurfaceMeshNodeType[i] > 0)
    {
      ++nNodes;
    }
    else
    {
      qDebug() << "Node Type Invalid: " << i << "::" << (int)(m_SurfaceMeshNodeType[i]);
    }
  }
  // This is the section header
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "POINT_DATA %d\n", nNodes);

  fprintf(vtkFile, "SCALARS Node_Type char 1\n");
  fprintf(vtkFile, "LOOKUP_TABLE default\n");

  for(int i = 0; i < numNodes; ++i)
  {
    if(m_SurfaceMeshNodeType[i] > 0)
    {
      if(m_WriteBinaryFile == true)
      {
        // swapped = m_SurfaceMeshNodeType[i];
        // SIMPLib::Endian::FromSystemToBig::convert(swapped);
        fwrite(m_SurfaceMeshNodeType + i, sizeof(char), 1, vtkFile);
      }
      else
      {
        fprintf(vtkFile, "%d ", m_SurfaceMeshNodeType[i]);
      }
    }
  }

  QString attrMatName = m_SurfaceMeshNodeTypeArrayPath.getAttributeMatrixName();
#if 1
  // This is from the Goldfeather Paper
  writePointVectorData<double>(sm, attrMatName, "Principal_Direction_1", "double", m_WriteBinaryFile, "VECTORS", vtkFile, numNodes);
  // This is from the Goldfeather Paper
  writePointVectorData<double>(sm, attrMatName, "Principal_Direction_2", "double", m_WriteBinaryFile, "VECTORS", vtkFile, numNodes);

  // This is from the Goldfeather Paper
  writePointScalarData<double>(sm, attrMatName, "Principal_Curvature_1", "double", m_WriteBinaryFile, vtkFile, numNodes);

  // This is from the Goldfeather Paper
  writePointScalarData<double>(sm, attrMatName, "Principal_Curvature_2", "double", m_WriteBinaryFile, vtkFile, numNodes);
#endif

  // This is from the Goldfeather Paper
  writePointVectorData<double>(sm, attrMatName, SIMPL::VertexData::SurfaceMeshNodeNormals, "double", m_WriteBinaryFile, "VECTORS", vtkFile, numNodes);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

template <typename T>
void writeCellScalarData(DataContainer::Pointer dc, const QString& faceAttributeMatrixName, const QString& dataName, const QString& dataType, bool writeBinaryData, FILE* vtkFile,
                         QMap<int32_t, int32_t>& featureIds, int32_t* m_SurfaceMeshFaceLabels)
{
  TriangleGeom::Pointer triangleGeom = dc->getGeometryAs<TriangleGeom>();

  int64_t numTriangles = triangleGeom->getNumberOfTris();
  IDataArray::Pointer data = dc->getAttributeMatrix(faceAttributeMatrixName)->getAttributeArray(dataName);

  QString ss;
  if(nullptr != data.get())
  {
    int32_t totalCellsWritten = 0;

    T* m = reinterpret_cast<T*>(data->getVoidPointer(0));
    fprintf(vtkFile, "\n");
    fprintf(vtkFile, "SCALARS %s %s 1\n", dataName.toLatin1().data(), dataType.toLatin1().data());
    fprintf(vtkFile, "LOOKUP_TABLE default\n");
    // Loop over all the features
    for(QMap<int32_t, int32_t>::iterator featureIter = featureIds.begin(); featureIter != featureIds.end(); ++featureIter)
    {
      int32_t gid = featureIter.key();   // The current Feature Id
      size_t size = featureIter.value(); // The number of triangles for this feature id
      std::vector<T> buffer(size, 0);
      totalCellsWritten += size;
      size_t index = 0;

      for(int j = 0; j < numTriangles; j++)
      {
        if(m_SurfaceMeshFaceLabels[j * 2] != gid && m_SurfaceMeshFaceLabels[j * 2 + 1] != gid)
        {
          continue;
        }
        // Get the data
        T s0 = static_cast<T>(m[j]);
        if(m_SurfaceMeshFaceLabels[j * 2 + 1] == gid)
        {
          s0 = s0 * -1;
        }

        // Write the values to the buffer after an Endian swap.
        if(writeBinaryData == true)
        {
          SIMPLib::Endian::FromSystemToBig::convert(s0);
          buffer[index] = s0;
          ++index;
        }
        else
        {

          ss = QString::number(s0);
          fprintf(vtkFile, "%s\n", ss.toLatin1().data());
        }
      }

      // Write the Buffer
      if(writeBinaryData == true)
      {
        fwrite(&(buffer.front()), sizeof(T), size, vtkFile);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
void writeCellNormalData(DataContainer::Pointer dc, const QString& faceAttributeMatrixName, const QString& dataName, const QString& dataType, bool writeBinaryData, FILE* vtkFile,
                         QMap<int32_t, int32_t>& featureIds, int32_t* m_SurfaceMeshFaceLabels)
{
  TriangleGeom::Pointer triangleGeom = dc->getGeometryAs<TriangleGeom>();

  int64_t numTriangles = triangleGeom->getNumberOfTris();
  IDataArray::Pointer data = dc->getAttributeMatrix(faceAttributeMatrixName)->getAttributeArray(dataName);
  QString buf;
  QTextStream ss(&buf);
  if(nullptr != data.get())
  {
    int32_t totalCellsWritten = 0;

    T* m = reinterpret_cast<T*>(data->getVoidPointer(0));
    fprintf(vtkFile, "\n");
    fprintf(vtkFile, "NORMALS %s %s\n", dataName.toLatin1().data(), dataType.toLatin1().data());
    // Loop over all the features
    for(QMap<int32_t, int32_t>::iterator featureIter = featureIds.begin(); featureIter != featureIds.end(); ++featureIter)
    {
      int32_t gid = featureIter.key();   // The current Feature Id
      size_t size = featureIter.value(); // The number of triangles for this feature id
      std::vector<T> buffer(size * 3, 0);
      totalCellsWritten += size * 3;
      size_t index = 0;

      for(int j = 0; j < numTriangles; j++)
      {
        if(m_SurfaceMeshFaceLabels[j * 2] != gid && m_SurfaceMeshFaceLabels[j * 2 + 1] != gid)
        {
          continue;
        }
        // Get the data
        T s0 = static_cast<T>(m[j * 3 + 0]);
        T s1 = static_cast<T>(m[j * 3 + 1]);
        T s2 = static_cast<T>(m[j * 3 + 2]);
        // Flip the normal if needed because the current feature id is assigned to the triangle.labels[1]
        if(m_SurfaceMeshFaceLabels[j * 2 + 1] == gid)
        {
          s0 *= -1.0;
          s1 *= -1.0;
          s2 *= -1.0;
        }
        // Write the values to the buffer after an Endian swap.
        if(writeBinaryData == true)
        {
          SIMPLib::Endian::FromSystemToBig::convert(s0);
          buffer[index] = s0;
          ++index;

          SIMPLib::Endian::FromSystemToBig::convert(s1);
          buffer[index] = s1;
          ++index;

          SIMPLib::Endian::FromSystemToBig::convert(s2);
          buffer[index] = s2;
          ++index;
        }
        else
        {

          ss << s0 << " " << s1 << " " << s2;
          fprintf(vtkFile, "%s\n", buf.toLatin1().data());
          buf.clear();
        }
      }

      // Write the Buffer
      if(writeBinaryData == true)
      {
        fwrite(&(buffer.front()), sizeof(T), size * 3, vtkFile);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
void writeCellVectorData(DataContainer::Pointer dc, const QString& faceAttributeMatrixName, const QString& dataName, const QString& dataType, bool writeBinaryData, const QString& vtkAttributeType,
                         FILE* vtkFile, QMap<int32_t, int32_t>& featureIds)
{
  TriangleGeom::Pointer triangleGeom = dc->getGeometryAs<TriangleGeom>();

  int64_t numTriangles = triangleGeom->getNumberOfTris();

  IDataArray::Pointer data = dc->getAttributeMatrix(faceAttributeMatrixName)->getAttributeArray(dataName);
  QString ss;
  if(nullptr != data.get())
  {
    T* m = reinterpret_cast<T*>(data->getVoidPointer(0));
    fprintf(vtkFile, "\n");
    fprintf(vtkFile, "%s %s %s\n", vtkAttributeType.toLatin1().data(), dataName.toLatin1().data(), dataType.toLatin1().data());
    for(int i = 0; i < numTriangles; ++i)
    {
      T s0 = 0x00;
      T s1 = 0x00;
      T s2 = 0x00;
      if(writeBinaryData == true)
      {
        s0 = static_cast<T>(m[i * 3 + 0]);
        s1 = static_cast<T>(m[i * 3 + 1]);
        s2 = static_cast<T>(m[i * 3 + 2]);
        SIMPLib::Endian::FromSystemToBig::convert(s0);
        SIMPLib::Endian::FromSystemToBig::convert(s1);
        SIMPLib::Endian::FromSystemToBig::convert(s2);
        fwrite(&s0, sizeof(T), 1, vtkFile);
        fwrite(&s1, sizeof(T), 1, vtkFile);
        fwrite(&s2, sizeof(T), 1, vtkFile);
      }
      else
      {

        ss << m[i * 3 + 0] << " " << m[i * 3 + 1] << " " << m[i * 3 + 2] << " ";

        fprintf(vtkFile, "%s ", ss.toLatin1().data());
        if(i % 25 == 0)
        {
          fprintf(vtkFile, "\n");
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshToNonconformalVtk::writeCellData(FILE* vtkFile, QMap<int32_t, int32_t>& featureIds)
{
  int err = 0;
  if(nullptr == vtkFile)
  {
    return -1;
  }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName());

  // Write the triangle region ids
  TriangleGeom::Pointer triangleGeom = getDataContainerArray()->getDataContainer(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName())->getGeometryAs<TriangleGeom>();
  int64_t numTriangles = triangleGeom->getNumberOfTris();

  int swapped;

  // This is like a "section header"
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "CELL_DATA %lld\n", (long long int)(numTriangles * 2));

  int32_t totalCellsWritten = 0;

  // Write the FeatureId Data to the file
  fprintf(vtkFile, "SCALARS FeatureID int 1\n");
  fprintf(vtkFile, "LOOKUP_TABLE default\n");

  // Loop over all the features
  for(QMap<int32_t, int32_t>::iterator featureIter = featureIds.begin(); featureIter != featureIds.end(); ++featureIter)
  {
    int32_t gid = featureIter.key();   // The current Feature Id
    size_t size = featureIter.value(); // The number of triangles for this feature id
    std::vector<int32_t> buffer(size, 0);
    totalCellsWritten += size;

    // Endian Swap our current feature Id since we are going to write it a bunch of times.
    swapped = gid;
    SIMPLib::Endian::FromSystemToBig::convert(swapped);
    size_t index = 0;

    // Loop over all the triangles looking for the current feature id
    // this is probably sub-optimal as if we have 1000 features we are going to loop 1000 times but this will use the
    // least amount of memory. We could run a filter to group the triangles by feature but then we would need an
    // additional amount of memory equal to 3X the memory used for the triangle list because every triangle will be listed
    // twice. We could get some slightly better performance if we buffered 4K worth of data then wrote out that data
    // in one chunk versus what we are doing here.
    for(int j = 0; j < numTriangles; j++)
    {
      if(m_SurfaceMeshFaceLabels[j * 2] == gid || m_SurfaceMeshFaceLabels[j * 2 + 1] == gid)
      {
        if(m_WriteBinaryFile == true)
        {
          buffer[index] = swapped;
          ++index;
        }
        else
        {
          fprintf(vtkFile, "%d\n", gid);
        }
      }
    }
    // Write the Buffer
    if(m_WriteBinaryFile == true)
    {
      fwrite(&(buffer.front()), sizeof(int32_t), size, vtkFile);
    }
  }
#if 0
  // Write the Original Triangle ID Data to the file
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "SCALARS TriangleID int 1\n");
  fprintf(vtkFile, "LOOKUP_TABLE default\n");
  for(int i = 0; i < triangleCount * 2; ++i)
  {
    //Triangle& t = triangles[i]; // Get the current Node

    if(m_WriteBinaryFile == true)
    {
      swapped = i;
      SIMPLib::Endian::FromSystemToBig::convert(swapped);
      fwrite(&swapped, sizeof(int), 1, vtkFile);
      fwrite(&swapped, sizeof(int), 1, vtkFile);
    }
    else
    {
      fprintf(vtkFile, "%d\n", i);
      fprintf(vtkFile, "%d\n", i);
    }
  }
#endif

  QString attrMatName = m_SurfaceMeshFaceLabelsArrayPath.getAttributeMatrixName();

  notifyStatusMessage(getHumanLabel(), "Writing Face Normals...");
  writeCellNormalData<double>(sm, attrMatName, SIMPL::FaceData::SurfaceMeshFaceNormals, "double", m_WriteBinaryFile, vtkFile, featureIds, m_SurfaceMeshFaceLabels);

  notifyStatusMessage(getHumanLabel(), "Writing Principal Curvature 1");
  writeCellScalarData<double>(sm, attrMatName, SIMPL::FaceData::SurfaceMeshPrincipalCurvature1, "double", m_WriteBinaryFile, vtkFile, featureIds, m_SurfaceMeshFaceLabels);
  notifyStatusMessage(getHumanLabel(), "Writing Principal Curvature 2");
  writeCellScalarData<double>(sm, attrMatName, SIMPL::FaceData::SurfaceMeshPrincipalCurvature2, "double", m_WriteBinaryFile, vtkFile, featureIds, m_SurfaceMeshFaceLabels);

  notifyStatusMessage(getHumanLabel(), "Writing Feature Face Id");
  writeCellScalarData<int32_t>(sm, attrMatName, SIMPL::FaceData::SurfaceMeshFeatureFaceId, "int", m_WriteBinaryFile, vtkFile, featureIds, m_SurfaceMeshFaceLabels);

  notifyStatusMessage(getHumanLabel(), "Writing Gaussian Curvature");
  writeCellScalarData<double>(sm, attrMatName, SIMPL::FaceData::SurfaceMeshGaussianCurvatures, "double", m_WriteBinaryFile, vtkFile, featureIds, m_SurfaceMeshFaceLabels);

  notifyStatusMessage(getHumanLabel(), "Writing Mean Curvature");
  writeCellScalarData<double>(sm, attrMatName, SIMPL::FaceData::SurfaceMeshMeanCurvatures, "double", m_WriteBinaryFile, vtkFile, featureIds, m_SurfaceMeshFaceLabels);
#if 0
  writeCellVectorData<double>(sm, attrMatName, SIMPL::CellData::SurfaceMeshPrincipalDirection1,
                              "double", m_WriteBinaryFile, "VECTORS", vtkFile, nT);

  writeCellVectorData<double>(sm, attrMatName, SIMPL::CellData::SurfaceMeshPrincipalDirection2,
                              "double", m_WriteBinaryFile, "VECTORS", vtkFile, nT);

  writeCellNormalData<double>(sm, attrMatName, "Goldfeather_Triangle_Normals",
                              "double", m_WriteBinaryFile, vtkFile, nT);
#endif

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer SurfaceMeshToNonconformalVtk::newFilterInstance(bool copyFilterParameters)
{
  SurfaceMeshToNonconformalVtk::Pointer filter = SurfaceMeshToNonconformalVtk::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SurfaceMeshToNonconformalVtk::getCompiledLibraryName()
{
  return IOConstants::IOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SurfaceMeshToNonconformalVtk::getBrandingString()
{
  return "IO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SurfaceMeshToNonconformalVtk::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << IO::Version::Major() << "." << IO::Version::Minor() << "." << IO::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SurfaceMeshToNonconformalVtk::getGroupName()
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SurfaceMeshToNonconformalVtk::getSubGroupName()
{
  return SIMPL::FilterSubGroups::OutputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SurfaceMeshToNonconformalVtk::getHumanLabel()
{
  return "Write Vtk PolyData (NonConformal) from SurfaceMesh";
}
