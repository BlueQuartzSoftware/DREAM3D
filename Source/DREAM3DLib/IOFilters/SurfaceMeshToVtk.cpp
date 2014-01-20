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

#include "SurfaceMeshToVtk.h"



#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>

#include "DREAM3DLib/Utilities/DREAM3DEndian.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshToVtk::SurfaceMeshToVtk() :
  AbstractFilter(),
  m_SurfaceDataContainerName(DREAM3D::Defaults::SurfaceDataContainerName),
  m_FaceAttributeMatrixName(DREAM3D::Defaults::FaceAttributeMatrixName),
  m_VertexAttributeMatrixName(DREAM3D::Defaults::VertexAttributeMatrixName),
  m_OutputVtkFile(""),
  m_WriteBinaryFile(false),
  m_WriteConformalMesh(true),
  m_SurfaceMeshFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceLabels(NULL),
  m_SurfaceMeshNodeTypeArrayName(DREAM3D::VertexData::SurfaceMeshNodeType),
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
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Output Vtk File");
    parameter->setPropertyName("OutputVtkFile");
    parameter->setWidgetType(FilterParameterWidgetType::OutputFileWidget);
    parameter->setValueType("QString");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Write Binary Vtk File");
    parameter->setPropertyName("WriteBinaryFile");
    parameter->setWidgetType(FilterParameterWidgetType::BooleanWidget);
    parameter->setValueType("bool");
    parameters.push_back(parameter);
  }
  //   {
  //     FilterParameter::Pointer parameter = FilterParameter::New();
  //     parameter->setHumanLabel("Write Conformal Mesh");
  //     parameter->setPropertyName("WriteConformalMesh");
  //     parameter->setWidgetType(FilterParameterWidgetType::BooleanWidget);
  //     parameter->setValueType("bool");
  //     parameters.push_back(parameter);
  //   }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToVtk::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setOutputVtkFile( reader->readString( "OutputVtkFile", getOutputVtkFile() ) );
  setWriteBinaryFile( reader->readValue("WriteBinaryFile", getWriteBinaryFile()) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshToVtk::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("OutputVtkFile", getOutputVtkFile() );
  writer->writeValue("WriteBinaryFile", getWriteBinaryFile() );
  writer->writeValue("WriteConformalMesh", getWriteConformalMesh() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToVtk::dataCheck()
{
  setErrorCondition(0);

  SurfaceDataContainer* sm = getDataContainerArray()->getPrereqDataContainer<SurfaceDataContainer, AbstractFilter>(this, getSurfaceDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer amV = sm->getPrereqAttributeMatrix<AbstractFilter>(this, getVertexAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer amF = sm->getPrereqAttributeMatrix<AbstractFilter>(this, getFaceAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }

  if (m_OutputVtkFile.isEmpty() == true)
  {
    setErrorCondition(-1003);
    notifyErrorMessage(getHumanLabel(), "Vtk Output file is Not set correctly", -1003);
  }

  if (sm->getVertices().get() == NULL)
  {
    setErrorCondition(-384);
    notifyErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", getErrorCondition());
  }
  QVector<size_t> dims(1, 2);
  m_SurfaceMeshFaceLabelsPtr = amF->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_SurfaceMeshFaceLabelsArrayName, -386, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  m_SurfaceMeshNodeTypePtr = amV->getPrereqArray<DataArray<int8_t>, AbstractFilter>(this, m_SurfaceMeshNodeTypeArrayName, -386, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshNodeTypePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshNodeType = m_SurfaceMeshNodeTypePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToVtk::preflight()
{
  dataCheck();
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
  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());  /* Place all your code to execute your filter here. */
  VertexArray& nodes = *(sm->getVertices());
  int nNodes = nodes.getNumberOfTuples();

  FaceArray& triangles = *(sm->getFaces().get());
  int triangleCount = triangles.getNumberOfTuples();

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(getOutputVtkFile());
  QDir parentPath = fi.path();

  if(!parentPath.mkpath("."))
  {

    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath.absolutePath());
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
    return;
  }

  // Open the output VTK File for writing
  FILE* vtkFile = NULL;
  vtkFile = fopen(getOutputVtkFile().toLatin1().data(), "wb");
  if (NULL == vtkFile)
  {

    QString ss = QObject::tr("Error creating file '%1'").arg(getOutputVtkFile());
    notifyErrorMessage(getHumanLabel(), ss, -18542);
    setErrorCondition(-18542);
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

  int numberWrittenNodes = 0;
  for (int i = 0; i < nNodes; i++)
  {
    //  Node& n = nodes[i]; // Get the current Node
    if (m_SurfaceMeshNodeType[i] > 0) { ++numberWrittenNodes; }
  }

  fprintf(vtkFile, "POINTS %d float\n", numberWrittenNodes);

  float pos[3] = {0.0f, 0.0f, 0.0f};

  size_t totalWritten = 0;

  // Write the POINTS data (Vertex)
  for (int i = 0; i < nNodes; i++)
  {
    VertexArray::Vert_t& n = nodes[i]; // Get the current Node
    if (m_SurfaceMeshNodeType[i] > 0)
    {
      pos[0] = static_cast<float>(n.pos[0]);
      pos[1] = static_cast<float>(n.pos[1]);
      pos[2] = static_cast<float>(n.pos[2]);

      if (m_WriteBinaryFile == true)
      {
        DREAM3D::Endian::FromSystemToBig::convert(pos[0]);
        DREAM3D::Endian::FromSystemToBig::convert(pos[1]);
        DREAM3D::Endian::FromSystemToBig::convert(pos[2]);
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
  //  int tn1, tn2, tn3;
  if (false == m_WriteConformalMesh)
  {
    triangleCount = triangleCount * 2;
  }
  // Write the POLYGONS
  fprintf(vtkFile, "\nPOLYGONS %d %d\n", triangleCount, (triangleCount * 4));
  for (int j = 0; j < triangleCount; j++)
  {
    //  Triangle& t = triangles[j];
    tData[1] = triangles[j].verts[0];
    tData[2] = triangles[j].verts[1];
    tData[3] = triangles[j].verts[2];

    if (m_WriteBinaryFile == true)
    {
      tData[0] = 3; // Push on the total number of entries for this entry
      DREAM3D::Endian::FromSystemToBig::convert(tData[0]);
      DREAM3D::Endian::FromSystemToBig::convert(tData[1]); // Index of Vertex 0
      DREAM3D::Endian::FromSystemToBig::convert(tData[2]); // Index of Vertex 1
      DREAM3D::Endian::FromSystemToBig::convert(tData[3]); // Index of Vertex 2
      fwrite(tData, sizeof(int), 4, vtkFile);
      if (false == m_WriteConformalMesh)
      {
        tData[0] = tData[1];
        tData[1] = tData[3];
        tData[3] = tData[0];
        tData[0] = 3;
        DREAM3D::Endian::FromSystemToBig::convert(tData[0]);
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
template<typename DataContainer, typename T>
void writePointScalarData(DataContainer* dc, const QString& vertexAttributeMatrixName, const QString& dataName, const QString& dataType,
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
        DREAM3D::Endian::FromSystemToBig::convert(swapped);
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
template<typename DataContainer, typename T>
void writePointVectorData(DataContainer* dc, const QString& vertexAttributeMatrixName, const QString& dataName, const QString& dataType,
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
        DREAM3D::Endian::FromSystemToBig::convert(s0);
        DREAM3D::Endian::FromSystemToBig::convert(s1);
        DREAM3D::Endian::FromSystemToBig::convert(s2);
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

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());

  // Write the Node Type Data to the file
  VertexArray& nodes = *(sm)->getVertices();
  int numNodes = nodes.getNumberOfTuples();
  int nNodes = 0;
  // int swapped;
  for (int i = 0; i < numNodes; i++)
  {
    if (m_SurfaceMeshNodeType[i] > 0) { ++nNodes; }
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
        // DREAM3D::Endian::FromSystemToBig::convert(swapped);
        fwrite(m_SurfaceMeshNodeType + i, sizeof(char), 1, vtkFile);
      }
      else
      {
        fprintf(vtkFile, "%d ", m_SurfaceMeshNodeType[i]);
      }
    }
  }

#if 1
  // This is from the Goldfeather Paper
  writePointVectorData<SurfaceDataContainer, double>(sm, getVertexAttributeMatrixName(), "Principal_Direction_1",
                                                     "double", m_WriteBinaryFile, m_WriteConformalMesh, "VECTORS", vtkFile, numNodes);
  // This is from the Goldfeather Paper
  writePointVectorData<SurfaceDataContainer, double>(sm, getVertexAttributeMatrixName(), "Principal_Direction_2",
                                                     "double", m_WriteBinaryFile, m_WriteConformalMesh, "VECTORS", vtkFile, numNodes);

  // This is from the Goldfeather Paper
  writePointScalarData<SurfaceDataContainer, double>(sm, getVertexAttributeMatrixName(), "Principal_Curvature_1",
                                                     "double", m_WriteBinaryFile, m_WriteConformalMesh, vtkFile, numNodes);

  // This is from the Goldfeather Paper
  writePointScalarData<SurfaceDataContainer, double>(sm, getVertexAttributeMatrixName(), "Principal_Curvature_2",
                                                     "double", m_WriteBinaryFile, m_WriteConformalMesh, vtkFile, numNodes);
#endif

  // This is from the Goldfeather Paper
  writePointVectorData<SurfaceDataContainer, double>(sm, getVertexAttributeMatrixName(), DREAM3D::VertexData::SurfaceMeshNodeNormals,
                                                     "double", m_WriteBinaryFile, m_WriteConformalMesh, "VECTORS", vtkFile, numNodes);


  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename DataContainer, typename T>
void writeCellScalarData(DataContainer* dc, const QString& faceAttributeMatrixName, const QString& dataName, const QString& dataType,
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
        DREAM3D::Endian::FromSystemToBig::convert(swapped);
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
template<typename DataContainer, typename T>
void writeCellVectorData(DataContainer* dc, const QString& faceAttributeMatrixName, const QString& dataName, const QString& dataType,
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
        DREAM3D::Endian::FromSystemToBig::convert(s0);
        DREAM3D::Endian::FromSystemToBig::convert(s1);
        DREAM3D::Endian::FromSystemToBig::convert(s2);
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
template<typename DataContainer, typename T>
void writeCellNormalData(DataContainer* dc, const QString& faceAttributeMatrixName, const QString& dataName, const QString& dataType,
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
        DREAM3D::Endian::FromSystemToBig::convert(s0);
        DREAM3D::Endian::FromSystemToBig::convert(s1);
        DREAM3D::Endian::FromSystemToBig::convert(s2);
        fwrite(&s0, sizeof(T), 1, vtkFile);
        fwrite(&s1, sizeof(T), 1, vtkFile);
        fwrite(&s2, sizeof(T), 1, vtkFile);
        if(false == writeConformalMesh)
        {
          s0 = static_cast<T>(m[i * 3 + 0]) * -1.0;
          s1 = static_cast<T>(m[i * 3 + 1]) * -1.0;
          s2 = static_cast<T>(m[i * 3 + 2]) * -1.0;
          DREAM3D::Endian::FromSystemToBig::convert(s0);
          DREAM3D::Endian::FromSystemToBig::convert(s1);
          DREAM3D::Endian::FromSystemToBig::convert(s2);
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

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());

  // Write the triangle region ids
  FaceArray& triangles = *(sm->getFaces());

  int nT = triangles.getNumberOfTuples();
  int triangleCount = nT;
  int swapped;
  if (false == m_WriteConformalMesh)
  {
    triangleCount = nT * 2;
  }

  // This is like a "section header"
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "CELL_DATA %d\n", triangleCount);

  // Write the FeatureId Data to the file
  fprintf(vtkFile, "SCALARS FeatureID int 1\n");
  fprintf(vtkFile, "LOOKUP_TABLE default\n");
  for(int i = 0; i < nT; ++i)
  {
    //FaceArray::Face_t& t = triangles[i]; // Get the current Node

    if(m_WriteBinaryFile == true)
    {
      swapped = m_SurfaceMeshFaceLabels[i * 2];
      DREAM3D::Endian::FromSystemToBig::convert(swapped);
      fwrite(&swapped, sizeof(int), 1, vtkFile);
      if(false == m_WriteConformalMesh)
      {
        swapped = m_SurfaceMeshFaceLabels[i * 2 + 1];
        DREAM3D::Endian::FromSystemToBig::convert(swapped);
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
      DREAM3D::Endian::FromSystemToBig::convert(swapped);
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

  writeCellScalarData<SurfaceDataContainer, int32_t>(sm, getFaceAttributeMatrixName(), DREAM3D::FaceData::SurfaceMeshFeatureFaceId,
                                                     "int", m_WriteBinaryFile, m_WriteConformalMesh, vtkFile, nT);

  writeCellScalarData<SurfaceDataContainer, double>(sm, getFaceAttributeMatrixName(), DREAM3D::FaceData::SurfaceMeshPrincipalCurvature1,
                                                    "double", m_WriteBinaryFile, m_WriteConformalMesh, vtkFile, nT);

  writeCellScalarData<SurfaceDataContainer, double>(sm, getFaceAttributeMatrixName(), DREAM3D::FaceData::SurfaceMeshPrincipalCurvature2,
                                                    "double", m_WriteBinaryFile, m_WriteConformalMesh, vtkFile, nT);

  writeCellVectorData<SurfaceDataContainer, double>(sm, getFaceAttributeMatrixName(), DREAM3D::FaceData::SurfaceMeshPrincipalDirection1,
                                                    "double", m_WriteBinaryFile, m_WriteConformalMesh, "VECTORS", vtkFile, nT);

  writeCellVectorData<SurfaceDataContainer, double>(sm, getFaceAttributeMatrixName(), DREAM3D::FaceData::SurfaceMeshPrincipalDirection2,
                                                    "double", m_WriteBinaryFile, m_WriteConformalMesh, "VECTORS", vtkFile, nT);

  writeCellScalarData<SurfaceDataContainer, double>(sm, getFaceAttributeMatrixName(), DREAM3D::FaceData::SurfaceMeshGaussianCurvatures,
                                                    "double", m_WriteBinaryFile, m_WriteConformalMesh, vtkFile, nT);

  writeCellScalarData<SurfaceDataContainer, double>(sm, getFaceAttributeMatrixName(), DREAM3D::FaceData::SurfaceMeshMeanCurvatures,
                                                    "double", m_WriteBinaryFile, m_WriteConformalMesh, vtkFile, nT);

  writeCellNormalData<SurfaceDataContainer, double>(sm, getFaceAttributeMatrixName(), DREAM3D::FaceData::SurfaceMeshFaceNormals,
                                                    "double", m_WriteBinaryFile, m_WriteConformalMesh, vtkFile, nT);

  writeCellNormalData<SurfaceDataContainer, double>(sm, getFaceAttributeMatrixName(), "Goldfeather_Triangle_Normals",
                                                    "double", m_WriteBinaryFile, m_WriteConformalMesh, vtkFile, nT);

  return err;
}

