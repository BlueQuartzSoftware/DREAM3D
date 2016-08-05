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

#include "SurfaceMeshToVtk.h"



#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/MultiDataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Utilities/SIMPLibEndian.h"
#include "SIMPLib/Geometry/TriangleGeom.h"

#include "IO/IOConstants.h"
#include "IO/IOVersion.h"


// Include the MOC generated file for this class
#include "moc_SurfaceMeshToVtk.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshToVtk::SurfaceMeshToVtk() :
  AbstractFilter(),
  m_OutputVtkFile(""),
  m_WriteBinaryFile(false),
  m_WriteConformalMesh(true),
  m_SurfaceMeshFaceLabelsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshNodeTypeArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::VertexAttributeMatrixName, SIMPL::VertexData::SurfaceMeshNodeType),
  m_SurfaceMeshFaceLabels(NULL),
  m_SurfaceMeshNodeType(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshToVtk::~SurfaceMeshToVtk()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToVtk::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;

  parameters.push_back(SIMPL_NEW_OUTPUT_FILE_FP("Output Vtk File", OutputVtkFile, FilterParameter::Parameter, SurfaceMeshToVtk));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Write Binary Vtk File", WriteBinaryFile, FilterParameter::Parameter, SurfaceMeshToVtk));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Write Conformal Mesh", WriteConformalMesh, FilterParameter::Parameter, SurfaceMeshToVtk));
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("SurfaceMeshFaceLabels", SurfaceMeshFaceLabelsArrayPath, FilterParameter::RequiredArray, SurfaceMeshToVtk, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("SurfaceMeshNodeType", SurfaceMeshNodeTypeArrayPath, FilterParameter::RequiredArray, SurfaceMeshToVtk, req));
  }

  parameters.push_back(SeparatorFilterParameter::New("Vertex Data", FilterParameter::RequiredArray));
  {
    MultiDataArraySelectionFilterParameter::RequirementType req;
    req.dcGeometryTypes = QVector<unsigned int>(1, SIMPL::GeometryType::TriangleGeometry);
    req.amTypes = QVector<unsigned int>(1, SIMPL::AttributeMatrixType::Vertex);
    parameters.push_back(SIMPL_NEW_MDA_SELECTION_FP("Vertex Arrays", SelectedVertexArrays, FilterParameter::RequiredArray, SurfaceMeshToVtk, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::RequiredArray));
  {
    MultiDataArraySelectionFilterParameter::RequirementType req;
    req.dcGeometryTypes = QVector<unsigned int>(1, SIMPL::GeometryType::TriangleGeometry);
    req.amTypes = QVector<unsigned int>(1, SIMPL::AttributeMatrixType::Face);
    parameters.push_back(SIMPL_NEW_MDA_SELECTION_FP("Face Arrays", SelectedFaceArrays, FilterParameter::RequiredArray, SurfaceMeshToVtk, req));
  }


  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToVtk::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSurfaceMeshNodeTypeArrayPath(reader->readDataArrayPath("SurfaceMeshNodeTypeArrayPath", getSurfaceMeshNodeTypeArrayPath() ) );
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath() ) );
  setOutputVtkFile( reader->readString( "OutputVtkFile", getOutputVtkFile() ) );
  setWriteBinaryFile( reader->readValue("WriteBinaryFile", getWriteBinaryFile()) );
  setWriteConformalMesh( reader->readValue("WriteConformalMesh", getWriteConformalMesh() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToVtk::initialize()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToVtk::dataCheck()
{
  setErrorCondition(0);
  if (m_OutputVtkFile.isEmpty() == true)
  {
    setErrorCondition(-1003);
    notifyErrorMessage(getHumanLabel(), "Vtk Output file is Not set correctly", -1003);
  }

  QString dcName;
  if (m_SelectedFaceArrays.size() > 0)
  {
    dcName = m_SelectedFaceArrays[0].getDataContainerName();
  }
  else if(m_SelectedVertexArrays.size() > 0)
  {
    dcName = m_SelectedVertexArrays[0].getDataContainerName();
  }

  foreach(DataArrayPath dap, m_SelectedFaceArrays)
  {
    if(dap.getDataContainerName().compare(dcName) != 0)
    {
      setErrorCondition(-385);
      notifyErrorMessage(getHumanLabel(), "The Face arrays and Vertex arrays must come from the same Data Container.", getErrorCondition());
      return;
    }
  }
  foreach(DataArrayPath dap, m_SelectedVertexArrays)
  {
    if(dap.getDataContainerName().compare(dcName) != 0)
    {
      setErrorCondition(-386);
      notifyErrorMessage(getHumanLabel(), "The Face arrays and Vertex arrays must come from the same Data Container.", getErrorCondition());
      return;
    }
  }

  DataContainer::Pointer sm = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, dcName, false);
  if(getErrorCondition() < 0) { return; }

  TriangleGeom::Pointer triangles =  sm->getPrereqGeometry<TriangleGeom, AbstractFilter>(this);
  if(getErrorCondition() < 0) { return; }

  // We MUST have Nodes
  if (NULL == triangles->getVertices().get())
  {
    setErrorCondition(-386);
    notifyErrorMessage(getHumanLabel(), "DataContainer Geometry missing Vertices", getErrorCondition());
  }
  // We MUST have Triangles defined also.
  if (NULL == triangles->getTriangles().get())
  {
    setErrorCondition(-387);
    notifyErrorMessage(getHumanLabel(), "DataContainer Geometry missing Triangles", getErrorCondition());
  }

  QVector<size_t> dims(1, 2);
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  m_SurfaceMeshNodeTypePtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter>(this, getSurfaceMeshNodeTypeArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshNodeTypePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshNodeType = m_SurfaceMeshNodeTypePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToVtk::preflight()
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
    ScopedFileMonitor(FILE* f) : m_File(f) {}
    virtual ~ScopedFileMonitor() { fclose(m_File);}
  private:
    FILE* m_File;
    ScopedFileMonitor(const ScopedFileMonitor&); // Copy Constructor Not Implemented
    void operator=(const ScopedFileMonitor&); // Operator '=' Not Implemented
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToVtk::execute()
{
  int err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  setErrorCondition(0);
  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName());  /* Place all your code to execute your filter here. */
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
  FILE* vtkFile = NULL;
  vtkFile = fopen(getOutputVtkFile().toLatin1().data(), "wb");
  if (NULL == vtkFile)
  {

    QString ss = QObject::tr("Error creating file '%1'").arg(getOutputVtkFile());
    setErrorCondition(-18542);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  ScopedFileMonitor vtkFileMonitor(vtkFile);

  fprintf(vtkFile, "# vtk DataFile Version 2.0\n");
  fprintf(vtkFile, "Data set from DREAM.3D Surface Meshing Module\n");
  if (m_WriteBinaryFile)
  {
    fprintf(vtkFile, "BINARY\n");
  }
  else
  {
    fprintf(vtkFile, "ASCII\n");
  }
  fprintf(vtkFile, "DATASET POLYDATA\n");

  int numberWrittenumNodes = 0;
  for (int i = 0; i < numNodes; i++)
  {
    //  Node& n = nodes[i]; // Get the current Node
    if (m_SurfaceMeshNodeType[i] > 0) { ++numberWrittenumNodes; }
  }

  fprintf(vtkFile, "POINTS %d float\n", numberWrittenumNodes);

  float pos[3] = {0.0f, 0.0f, 0.0f};

  size_t totalWritten = 0;

  // Write the POINTS data (Vertex)
  for (int i = 0; i < numNodes; i++)
  {
    if (m_SurfaceMeshNodeType[i] > 0)
    {
      pos[0] = static_cast<float>(nodes[i * 3]);
      pos[1] = static_cast<float>(nodes[i * 3 + 1]);
      pos[2] = static_cast<float>(nodes[i * 3 + 2]);

      if (m_WriteBinaryFile == true)
      {
        SIMPLib::Endian::FromSystemToBig::convert(pos[0]);
        SIMPLib::Endian::FromSystemToBig::convert(pos[1]);
        SIMPLib::Endian::FromSystemToBig::convert(pos[2]);
        totalWritten = fwrite(pos, sizeof(float), 3, vtkFile);
        if (totalWritten != sizeof(float) * 3)
        {

        }
      }
      else
      {
        fprintf(vtkFile, "%f %f %f\n", pos[0], pos[1], pos[2]); // Write the positions to the output file
      }
    }
  }

  int tData[4];
  int triangleCount = numTriangles;
  //  int tn1, tn2, tn3;
  if (false == m_WriteConformalMesh)
  {
    triangleCount = numTriangles * 2;
  }
  // Write the POLYGONS
  fprintf(vtkFile, "\nPOLYGONS %d %d\n", triangleCount, (triangleCount * 4));
  for (int j = 0; j < numTriangles; j++)
  {
    //  Triangle& t = triangles[j];
    tData[1] = triangles[j * 3];
    tData[2] = triangles[j * 3 + 1];
    tData[3] = triangles[j * 3 + 2];

    if (m_WriteBinaryFile == true)
    {
      tData[0] = 3; // Push on the total number of entries for this entry
      SIMPLib::Endian::FromSystemToBig::convert(tData[0]);
      SIMPLib::Endian::FromSystemToBig::convert(tData[1]); // Index of Vertex 0
      SIMPLib::Endian::FromSystemToBig::convert(tData[2]); // Index of Vertex 1
      SIMPLib::Endian::FromSystemToBig::convert(tData[3]); // Index of Vertex 2
      fwrite(tData, sizeof(int), 4, vtkFile);
      if (false == m_WriteConformalMesh)
      {
        tData[0] = tData[1];
        tData[1] = tData[3];
        tData[3] = tData[0];
        tData[0] = 3;
        SIMPLib::Endian::FromSystemToBig::convert(tData[0]);
        fwrite(tData, sizeof(int), 4, vtkFile);
      }
    }
    else
    {
      fprintf(vtkFile, "3 %d %d %d\n", tData[1], tData[2], tData[3]);
      if (false == m_WriteConformalMesh)
      {
        fprintf(vtkFile, "3 %d %d %d\n", tData[3], tData[2], tData[1]);
      }
    }
  }

  // Write the POINT_DATA section
  err = writePointData(vtkFile);
  // Write the CELL_DATA section
  err = writeCellData(vtkFile);

  fprintf(vtkFile, "\n");

  setErrorCondition(0);
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void writePointScalarData(DataContainer::Pointer dc, const QString& vertexAttributeMatrixName, const QString& dataName, const QString& dataType,
                          bool writeBinaryData, bool writeConformalMesh, FILE* vtkFile, int nT)
{
  IDataArray::Pointer data = dc->getAttributeMatrix(vertexAttributeMatrixName)->getAttributeArray(dataName);
  QString ss;
  if (NULL != data.get())
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
        //if (i%50 == 0)
        { fprintf(vtkFile, "\n"); }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void writePointVectorData(DataContainer::Pointer dc, const QString& vertexAttributeMatrixName, const QString& dataName, const QString& dataType,
                          bool writeBinaryData, bool writeConformalMesh, const QString& vtkAttributeType,
                          FILE* vtkFile, int nT)
{
  IDataArray::Pointer data = dc->getAttributeMatrix(vertexAttributeMatrixName)->getAttributeArray(dataName);
  QString ss;
  if (NULL != data.get())
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

        ss = QString::number(m[i * 3 + 0]) + " " + QString::number(m[i * 3 + 1]) + " " + QString::number(m[i * 3 + 2]) + " ";
        fprintf(vtkFile, "%s ", ss.toLatin1().data());
        //if (i%50 == 0)
        { fprintf(vtkFile, "\n"); }
      }

    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshToVtk::writePointData(FILE* vtkFile)
{
  int err = 0;
  if (NULL == vtkFile)
  {
    return -1;
  }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName());

  TriangleGeom::Pointer triangleGeom = getDataContainerArray()->getDataContainer(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName())->getGeometryAs<TriangleGeom>();
  qint64 numNodes = triangleGeom->getNumberOfVertices();
  int64_t numberWrittenumNodes = 0;

  // Write the Node Type Data to the file
  for (int i = 0; i < numNodes; i++)
  {
    if (m_SurfaceMeshNodeType[i] > 0) { ++numberWrittenumNodes; }
  }
  // This is the section header
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "POINT_DATA %lld\n", (long long int)(numberWrittenumNodes));


  fprintf(vtkFile, "SCALARS Node_Type char 1\n");
  fprintf(vtkFile, "LOOKUP_TABLE default\n");

  for(int i = 0; i < numNodes; ++i)
  {
    if(m_SurfaceMeshNodeType[i] > 0)
    {
      if(m_WriteBinaryFile == true)
      {
        // Normally, we would byte swap to big endian but since we are only writing
        // 1 byte Char values, nothing to swap.
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
  writePointVectorData<double>(sm, attrMatName, "Principal_Direction_1",
                               "double", m_WriteBinaryFile, m_WriteConformalMesh, "VECTORS", vtkFile, numNodes);
  // This is from the Goldfeather Paper
  writePointVectorData<double>(sm, attrMatName, "Principal_Direction_2",
                               "double", m_WriteBinaryFile, m_WriteConformalMesh, "VECTORS", vtkFile, numNodes);

  // This is from the Goldfeather Paper
  writePointScalarData<double>(sm, attrMatName, "Principal_Curvature_1",
                               "double", m_WriteBinaryFile, m_WriteConformalMesh, vtkFile, numNodes);

  // This is from the Goldfeather Paper
  writePointScalarData<double>(sm, attrMatName, "Principal_Curvature_2",
                               "double", m_WriteBinaryFile, m_WriteConformalMesh, vtkFile, numNodes);
#endif

  // This is from the Goldfeather Paper
  writePointVectorData<double>(sm, attrMatName, SIMPL::VertexData::SurfaceMeshNodeNormals,
                               "double", m_WriteBinaryFile, m_WriteConformalMesh, "VECTORS", vtkFile, numNodes);


  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void writeCellScalarData(DataContainer::Pointer dc, const QString& faceAttributeMatrixName, const QString& dataName, const QString& dataType,
                         bool writeBinaryData, bool writeConformalMesh, FILE* vtkFile, int nT)
{
  // Write the Feature Face ID Data to the file
  IDataArray::Pointer data = dc->getAttributeMatrix(faceAttributeMatrixName)->getAttributeArray(dataName);
  QString buf;
  QTextStream ss(&buf);
  if (NULL != data.get())
  {
    T* m = reinterpret_cast<T*>(data->getVoidPointer(0));
    fprintf(vtkFile, "\n");
    fprintf(vtkFile, "SCALARS %s %s 1\n", dataName.toLatin1().data(), dataType.toLatin1().data());
    fprintf(vtkFile, "LOOKUP_TABLE default\n");
    for(int i = 0; i < nT; ++i)
    {
      T swapped = 0x00;
      if(writeBinaryData == true)
      {
        swapped = static_cast<T>(m[i]);
        SIMPLib::Endian::FromSystemToBig::convert(swapped);
        fwrite(&swapped, sizeof(T), 1, vtkFile);
        if(false == writeConformalMesh)
        {
          fwrite(&swapped, sizeof(T), 1, vtkFile);
        }
      }
      else
      {

        ss << m[i] << " ";
        if(false == writeConformalMesh)
        {
          ss << m[i] << " ";
        }
        fprintf(vtkFile, "%s", buf.toLatin1().data());
        buf.clear();
        if (i % 50 == 0) { fprintf(vtkFile, "\n"); }
      }
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void writeCellVectorData(DataContainer::Pointer dc, const QString& faceAttributeMatrixName, const QString& dataName, const QString& dataType,
                         bool writeBinaryData, bool writeConformalMesh, const QString& vtkAttributeType,
                         FILE* vtkFile, int nT)
{
  IDataArray::Pointer data = dc->getAttributeMatrix(faceAttributeMatrixName)->getAttributeArray(dataName);
  QString buf;
  QTextStream ss(&buf);
  if (NULL != data.get())
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
        fwrite(&s2, sizeof(T), 1, vtkFile);
        if(false == writeConformalMesh)
        {
          fwrite(&s0, sizeof(T), 1, vtkFile);
          fwrite(&s1, sizeof(T), 1, vtkFile);
          fwrite(&s2, sizeof(T), 1, vtkFile);
        }
      }
      else
      {
        ss << m[i * 3 + 0] << " " << m[i * 3 + 1] << " " << m[i * 3 + 2] << " ";
        if(false == writeConformalMesh)
        {
          ss << m[i * 3 + 0] << " " << m[i * 3 + 1] << " " << m[i * 3 + 2] << " ";
        }
        fprintf(vtkFile, "%s ",  buf.toLatin1().data());
        buf.clear();
        if (i % 25 == 0) { fprintf(vtkFile, "\n"); }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template< typename T>
void writeCellNormalData(DataContainer::Pointer dc, const QString& faceAttributeMatrixName, const QString& dataName, const QString& dataType,
                         bool writeBinaryData, bool writeConformalMesh,
                         FILE* vtkFile, int nT)
{
  IDataArray::Pointer data = dc->getAttributeMatrix(faceAttributeMatrixName)->getAttributeArray(dataName);
  QString buf;
  QTextStream ss(&buf);
  if (NULL != data.get())
  {
    T* m = reinterpret_cast<T*>(data->getVoidPointer(0));
    fprintf(vtkFile, "\n");
    fprintf(vtkFile, "NORMALS %s %s\n", dataName.toLatin1().data(), dataType.toLatin1().data());
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
        fwrite(&s2, sizeof(T), 1, vtkFile);
        if(false == writeConformalMesh)
        {
          s0 = static_cast<T>(m[i * 3 + 0]) * -1.0;
          s1 = static_cast<T>(m[i * 3 + 1]) * -1.0;
          s2 = static_cast<T>(m[i * 3 + 2]) * -1.0;
          SIMPLib::Endian::FromSystemToBig::convert(s0);
          SIMPLib::Endian::FromSystemToBig::convert(s1);
          SIMPLib::Endian::FromSystemToBig::convert(s2);
          fwrite(&s0, sizeof(T), 1, vtkFile);
          fwrite(&s1, sizeof(T), 1, vtkFile);
          fwrite(&s2, sizeof(T), 1, vtkFile);
        }
      }
      else
      {

        ss << m[i * 3 + 0] << " " << m[i * 3 + 1] << " " << m[i * 3 + 2] << " ";
        if(false == writeConformalMesh)
        {
          ss << -1.0 * m[i * 3 + 0] << " " << -1.0 * m[i * 3 + 1] << " " << -1.0 * m[i * 3 + 2] << " ";
        }
        fprintf(vtkFile, "%s ", buf.toLatin1().data());
        buf.clear();
        if (i % 50 == 0) { fprintf(vtkFile, "\n"); }
      }
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshToVtk::writeCellData(FILE* vtkFile)
{
  int err = 0;
  if (NULL == vtkFile)
  {
    return -1;
  }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName());

  // Write the triangle region ids
  TriangleGeom::Pointer triangleGeom = getDataContainerArray()->getDataContainer(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName())->getGeometryAs<TriangleGeom>();

  int64_t nT = triangleGeom->getNumberOfTris();

  int numTriangles = nT;
  int swapped;
  if (false == m_WriteConformalMesh)
  {
    numTriangles = nT * 2;
  }

  // This is like a "section header"
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "CELL_DATA %d\n", numTriangles);

  // Write the FeatureId Data to the file
  fprintf(vtkFile, "SCALARS FeatureID int 1\n");
  fprintf(vtkFile, "LOOKUP_TABLE default\n");
  for(int i = 0; i < nT; ++i)
  {
    //FaceArray::Face_t& t = triangles[i]; // Get the current Node

    if(m_WriteBinaryFile == true)
    {
      swapped = m_SurfaceMeshFaceLabels[i * 2];
      SIMPLib::Endian::FromSystemToBig::convert(swapped);
      fwrite(&swapped, sizeof(int), 1, vtkFile);
      if(false == m_WriteConformalMesh)
      {
        swapped = m_SurfaceMeshFaceLabels[i * 2 + 1];
        SIMPLib::Endian::FromSystemToBig::convert(swapped);
        fwrite(&swapped, sizeof(int), 1, vtkFile);
      }
    }
    else
    {
      fprintf(vtkFile, "%d\n", m_SurfaceMeshFaceLabels[i * 2]);
      if(false == m_WriteConformalMesh)
      {
        fprintf(vtkFile, "%d\n", m_SurfaceMeshFaceLabels[i * 2 + 1]);
      }
    }

  }

#if 0
  // Write the Original Triangle ID Data to the file
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "SCALARS TriangleID int 1\n");
  fprintf(vtkFile, "LOOKUP_TABLE default\n");
  for(int i = 0; i < nT; ++i)
  {
    //Triangle& t = triangles[i]; // Get the current Node

    if(m_WriteBinaryFile == true)
    {
      swapped = i;
      SIMPLib::Endian::FromSystemToBig::convert(swapped);
      fwrite(&swapped, sizeof(int), 1, vtkFile);
      if(false == m_WriteConformalMesh)
      {
        fwrite(&swapped, sizeof(int), 1, vtkFile);
      }
    }
    else
    {
      fprintf(vtkFile, "%d\n", i);
      if(false == m_WriteConformalMesh)
      {
        fprintf(vtkFile, "%d\n", i);
      }
    }
  }
#endif

  QString attrMatName = m_SurfaceMeshFaceLabelsArrayPath.getAttributeMatrixName();

  writeCellScalarData<int32_t>(sm, attrMatName, SIMPL::FaceData::SurfaceMeshFeatureFaceId,
                               "int", m_WriteBinaryFile, m_WriteConformalMesh, vtkFile, nT);

  writeCellScalarData<double>(sm, attrMatName, SIMPL::FaceData::SurfaceMeshPrincipalCurvature1,
                              "double", m_WriteBinaryFile, m_WriteConformalMesh, vtkFile, nT);

  writeCellScalarData<double>(sm, attrMatName, SIMPL::FaceData::SurfaceMeshPrincipalCurvature2,
                              "double", m_WriteBinaryFile, m_WriteConformalMesh, vtkFile, nT);

  writeCellVectorData<double>(sm, attrMatName, SIMPL::FaceData::SurfaceMeshPrincipalDirection1,
                              "double", m_WriteBinaryFile, m_WriteConformalMesh, "VECTORS", vtkFile, nT);

  writeCellVectorData<double>(sm, attrMatName, SIMPL::FaceData::SurfaceMeshPrincipalDirection2,
                              "double", m_WriteBinaryFile, m_WriteConformalMesh, "VECTORS", vtkFile, nT);

  writeCellScalarData<double>(sm, attrMatName, SIMPL::FaceData::SurfaceMeshGaussianCurvatures,
                              "double", m_WriteBinaryFile, m_WriteConformalMesh, vtkFile, nT);

  writeCellScalarData<double>(sm, attrMatName, SIMPL::FaceData::SurfaceMeshMeanCurvatures,
                              "double", m_WriteBinaryFile, m_WriteConformalMesh, vtkFile, nT);

  writeCellNormalData<double>(sm, attrMatName, SIMPL::FaceData::SurfaceMeshFaceNormals,
                              "double", m_WriteBinaryFile, m_WriteConformalMesh, vtkFile, nT);

  writeCellNormalData<double>(sm, attrMatName, "Goldfeather_Triangle_Normals",
                              "double", m_WriteBinaryFile, m_WriteConformalMesh, vtkFile, nT);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer SurfaceMeshToVtk::newFilterInstance(bool copyFilterParameters)
{
  SurfaceMeshToVtk::Pointer filter = SurfaceMeshToVtk::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SurfaceMeshToVtk::getCompiledLibraryName()
{
  return IOConstants::IOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SurfaceMeshToVtk::getBrandingString()
{
  return "IO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SurfaceMeshToVtk::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  IO::Version::Major() << "." << IO::Version::Minor() << "." << IO::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SurfaceMeshToVtk::getGroupName()
{ return SIMPL::FilterGroups::IOFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SurfaceMeshToVtk::getSubGroupName()
{ return SIMPL::FilterSubGroups::OutputFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SurfaceMeshToVtk::getHumanLabel()
{ return "Write Vtk PolyData from Triangle Geometry"; }

