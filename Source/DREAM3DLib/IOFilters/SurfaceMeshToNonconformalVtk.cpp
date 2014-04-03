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

#include "SurfaceMeshToNonconformalVtk.h"



#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>


#include "DREAM3DLib/Utilities/DREAM3DEndian.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshToNonconformalVtk::SurfaceMeshToNonconformalVtk() :
  AbstractFilter(),
  m_SurfaceDataContainerName(DREAM3D::Defaults::SurfaceDataContainerName),
  m_FaceAttributeMatrixName(DREAM3D::Defaults::FaceAttributeMatrixName),
  m_VertexAttributeMatrixName(DREAM3D::Defaults::VertexAttributeMatrixName),
  m_OutputVtkFile(""),
  m_WriteBinaryFile(false),
  m_SurfaceMeshFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceLabels(NULL),
  m_SurfaceMeshNodeTypeArrayName(DREAM3D::VertexData::SurfaceMeshNodeType),
  m_SurfaceMeshNodeType(NULL),
/*[]*/m_SurfaceMeshFaceLabelsArrayPath(DREAM3D::Defaults::SomePath),
/*[]*/m_SurfaceMeshNodeTypeArrayPath(DREAM3D::Defaults::SomePath)
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
  parameters.push_back(FilterParameter::New("Output Vtk File", "OutputVtkFile", FilterParameterWidgetType::OutputFileWidget,"QString", false));
  parameters.push_back(FilterParameter::New("Write Binary Vtk File", "WriteBinaryFile", FilterParameterWidgetType::BooleanWidget,"bool", false));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
/*[]*/parameters.push_back(FilterParameter::New("SurfaceMeshFaceLabels", "SurfaceMeshFaceLabelsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
/*[]*/parameters.push_back(FilterParameter::New("SurfaceMeshNodeType", "SurfaceMeshNodeTypeArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToNonconformalVtk::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSurfaceMeshNodeTypeArrayPath(reader->readDataArrayPath("SurfaceMeshNodeTypeArrayPath", getSurfaceMeshNodeTypeArrayPath() ) );
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath() ) );
  setOutputVtkFile( reader->readString( "OutputVtkFile", getOutputVtkFile() ) );
  setWriteBinaryFile( reader->readValue("WriteBinaryFile", getWriteBinaryFile()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshToNonconformalVtk::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("SurfaceMeshNodeTypeArrayPath", getSurfaceMeshNodeTypeArrayPath() );
  writer->writeValue("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath() );
  writer->writeValue("OutputVtkFile", getOutputVtkFile() );
  writer->writeValue("WriteBinaryFile", getWriteBinaryFile() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToNonconformalVtk::dataCheck()
{
  setErrorCondition(0);
  QString ss;

  if (m_OutputVtkFile.isEmpty() == true)
  {
    setErrorCondition(-1003);
    notifyErrorMessage(getHumanLabel(), "Vtk Output file is Not set correctly", -1003);
  }

  SurfaceDataContainer* sm = getDataContainerArray()->getPrereqDataContainer<SurfaceDataContainer, AbstractFilter>(this, getSurfaceDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer amV = sm->getPrereqAttributeMatrix<AbstractFilter>(this, getVertexAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer amF = sm->getPrereqAttributeMatrix<AbstractFilter>(this, getFaceAttributeMatrixName(), -301);
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
////====>REMOVE THIS    m_SurfaceMeshFaceLabelsPtr = amF->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_SurfaceMeshFaceLabelsArrayName, -386, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
////====>REMOVE THIS    m_SurfaceMeshNodeTypePtr = amV->getPrereqArray<DataArray<int8_t>, AbstractFilter>(this, m_SurfaceMeshNodeTypeArrayName, -386, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_SurfaceMeshNodeTypePtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter>(this, getSurfaceMeshNodeTypeArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshNodeTypePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshNodeType = m_SurfaceMeshNodeTypePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToNonconformalVtk::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
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
void SurfaceMeshToNonconformalVtk::execute()
{
  int err = 0;
  setErrorCondition(err);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  SurfaceDataContainer* m = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());
  VertexArray& nodes = *(m->getVertices());
  int nNodes = nodes.getNumberOfTuples();
  FaceArray& triangles = *(m->getFaces());
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

  notifyStatusMessage(getHumanLabel(), "Writing Vertex Data ....");

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
    else { qDebug() << "Node Type Invalid: " << i << "::" << (int)(m_SurfaceMeshNodeType[i]) ;}
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
  for (int i = 0; i < triangleCount; i++)
  {
    if (featureTriangleCount.find(m_SurfaceMeshFaceLabels[i * 2]) == featureTriangleCount.end())
    {
      featureTriangleCount[m_SurfaceMeshFaceLabels[i * 2]] = 1;
    }
    else
    {
      featureTriangleCount[m_SurfaceMeshFaceLabels[i * 2]]++;
    }
    if (featureTriangleCount.find(m_SurfaceMeshFaceLabels[i * 2 + 1]) == featureTriangleCount.end())
    {
      featureTriangleCount[m_SurfaceMeshFaceLabels[i * 2 + 1]] = 1;
    }
    else
    {
      featureTriangleCount[m_SurfaceMeshFaceLabels[i * 2 + 1]]++;
    }
  }

  // Write the POLYGONS
  fprintf(vtkFile, "\nPOLYGONS %d %d\n", triangleCount * 2, (triangleCount * 2 * 4));

  size_t totalCells = 0;
  // Loop over all the features
  for(QMap<int32_t, int32_t>::iterator featureIter = featureTriangleCount.begin(); featureIter != featureTriangleCount.end(); ++featureIter)
  {
    totalCells += featureIter.value();
  }
  BOOST_ASSERT(totalCells == (size_t)(triangleCount * 2) );


  // Loop over all the features
  for(QMap<int32_t, int32_t>::iterator featureIter = featureTriangleCount.begin(); featureIter != featureTriangleCount.end(); ++featureIter)
  {
    int32_t gid = featureIter.key(); // The current Feature Id
    int32_t numTriToWrite = featureIter.value(); // The number of triangles for this feature
    uint8_t doWrite = 0;

    // Loop over all the triangles looking for the current feature id
    // this is probably sub-optimal as if we have 1000 features we are going to loop 1000 times but this will use the
    // least amount of memory. We could run a filter to group the triangles by feature but then we would need an
    // additional amount of memory equal to 3X the memory used for the triangle list because every triangle will be listed
    // twice. We could get some slightly better performance if we buffered 4K worth of data then wrote out that data
    // in one chunk versus what we are doing here.
    for (int j = 0; j < triangleCount; j++)
    {
      doWrite = 0;

      if (m_SurfaceMeshFaceLabels[j * 2] == gid ) { doWrite = 1; }
      else if (m_SurfaceMeshFaceLabels[j * 2 + 1] == gid) { doWrite = 2; } // We need to flip the winding of the triangle
      if (doWrite == 0) { continue; } // Labels in the triangle did match the current feature id.

      if (doWrite == 1)
      {
        tData[0] = 3; // Push on the total number of entries for this entry
        tData[1] = triangles[j].verts[0];
        tData[2] = triangles[j].verts[1];
        tData[3] = triangles[j].verts[2];
      }
      else
      {
        tData[0] = 3; // Push on the total number of entries for this entry
        tData[1] = triangles[j].verts[2];
        tData[2] = triangles[j].verts[1];
        tData[3] = triangles[j].verts[0];
      }
      if (m_WriteBinaryFile == true)
      {

        DREAM3D::Endian::FromSystemToBig::convert(tData[0]);
        DREAM3D::Endian::FromSystemToBig::convert(tData[1]); // Index of Vertex 0
        DREAM3D::Endian::FromSystemToBig::convert(tData[2]); // Index of Vertex 1
        DREAM3D::Endian::FromSystemToBig::convert(tData[3]); // Index of Vertex 2
        fwrite(tData, sizeof(int), 4, vtkFile);
      }
      else
      {
        fprintf(vtkFile, "3 %d %d %d\n", tData[1], tData[2], tData[3]);
      }
      numTriToWrite--;
    }
    if (numTriToWrite != 0)
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
template<typename DataContainer, typename T>
void writePointScalarData(DataContainer* dc, const QString& vertexAttributeMatrixName, const QString& dataName, const QString& dataType,
                          bool writeBinaryData, FILE* vtkFile, int nT)
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
                          bool writeBinaryData, const QString& vtkAttributeType,
                          FILE* vtkFile, int nT)
{
  IDataArray::Pointer data = dc->getAttributeMatrix(vertexAttributeMatrixName)->getAttributeArray(dataName);
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
        fwrite(&s1, sizeof(T), 1, vtkFile);
      }
      else
      {

        ss << m[i * 3 + 0] << " " << m[i * 3 + 1] << " " << m[i * 3 + 2] << " ";
        fprintf(vtkFile, "%s ", buf.toLatin1().data());
        buf.clear();
        //if (i%50 == 0)
        { fprintf(vtkFile, "\n"); }
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
  if (NULL == vtkFile)
  {
    return -1;
  }

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());

  VertexArray::Pointer nodesPtr = sm->getVertices();

  //Get the Number of Vertex points in the mesh with a valid node type
  VertexArray& nodes = *(nodesPtr);
  int numNodes = nodes.getNumberOfTuples();
  int nNodes = 0;
  // int swapped;
  for (int i = 0; i < numNodes; i++)
  {
    if (m_SurfaceMeshNodeType[i] > 0) { ++nNodes; }
    else { qDebug() << "Node Type Invalid: " << i << "::" << (int)(m_SurfaceMeshNodeType[i]) ;}
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
                                                     "double", m_WriteBinaryFile, "VECTORS", vtkFile, numNodes);
  // This is from the Goldfeather Paper
  writePointVectorData<SurfaceDataContainer, double>(sm, getVertexAttributeMatrixName(), "Principal_Direction_2",
                                                     "double", m_WriteBinaryFile, "VECTORS", vtkFile, numNodes);

  // This is from the Goldfeather Paper
  writePointScalarData<SurfaceDataContainer, double>(sm, getVertexAttributeMatrixName(), "Principal_Curvature_1",
                                                     "double", m_WriteBinaryFile, vtkFile, numNodes);

  // This is from the Goldfeather Paper
  writePointScalarData<SurfaceDataContainer, double>(sm, getVertexAttributeMatrixName(), "Principal_Curvature_2",
                                                     "double", m_WriteBinaryFile, vtkFile, numNodes);
#endif

  // This is from the Goldfeather Paper
  writePointVectorData<SurfaceDataContainer, double>(sm, getVertexAttributeMatrixName(), DREAM3D::VertexData::SurfaceMeshNodeNormals,
                                                     "double", m_WriteBinaryFile, "VECTORS", vtkFile, numNodes);


  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

template<typename SurfaceDataContainer, typename T>
void writeCellScalarData(SurfaceDataContainer* dc, const QString& faceAttributeMatrixName, const QString& dataName, const QString& dataType,
                         bool writeBinaryData, FILE* vtkFile, QMap<int32_t, int32_t>& featureIds, int32_t* m_SurfaceMeshFaceLabels)
{
  FaceArray& triangles = *(dc->getFaces());

  int triangleCount = triangles.getNumberOfTuples();
  IDataArray::Pointer data = dc->getAttributeMatrix(faceAttributeMatrixName)->getAttributeArray(dataName);

  QString ss;
  if (NULL != data.get())
  {
    int32_t totalCellsWritten = 0;

    T* m = reinterpret_cast<T*>(data->getVoidPointer(0));
    fprintf(vtkFile, "\n");
    fprintf(vtkFile, "SCALARS %s %s 1\n", dataName.toLatin1().data(), dataType.toLatin1().data());
    fprintf(vtkFile, "LOOKUP_TABLE default\n");
    // Loop over all the features
    for(QMap<int32_t, int32_t>::iterator featureIter = featureIds.begin(); featureIter != featureIds.end(); ++featureIter)
    {
      int32_t gid = featureIter.key(); // The current Feature Id
      size_t size = featureIter.value(); // The number of triangles for this feature id
      std::vector<T> buffer(size, 0);
      totalCellsWritten += size;
      size_t index = 0;

      for (int j = 0; j < triangleCount; j++)
      {
        if (m_SurfaceMeshFaceLabels[j * 2] != gid && m_SurfaceMeshFaceLabels[j * 2 + 1] != gid) { continue; }
        // Get the data
        T s0 = static_cast<T>(m[j]);
        if (m_SurfaceMeshFaceLabels[j * 2 + 1] == gid)
        { s0 = s0 * -1; }


        // Write the values to the buffer after an Endian swap.
        if(writeBinaryData == true)
        {
          DREAM3D::Endian::FromSystemToBig::convert(s0);
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
template<typename DataContainer, typename T>
void writeCellNormalData(DataContainer* dc, const QString& faceAttributeMatrixName, const QString& dataName, const QString& dataType,
                         bool writeBinaryData, FILE* vtkFile, QMap<int32_t, int32_t>& featureIds, int32_t* m_SurfaceMeshFaceLabels)
{

  FaceArray& triangles = *(dc->getFaces());

  int triangleCount = triangles.getNumberOfTuples();
  IDataArray::Pointer data = dc->getAttributeMatrix(faceAttributeMatrixName)->getAttributeArray(dataName);
  QString buf;
  QTextStream ss(&buf);
  if (NULL != data.get())
  {
    int32_t totalCellsWritten = 0;

    T* m = reinterpret_cast<T*>(data->getVoidPointer(0));
    fprintf(vtkFile, "\n");
    fprintf(vtkFile, "NORMALS %s %s\n", dataName.toLatin1().data(), dataType.toLatin1().data());
    // Loop over all the features
    for(QMap<int32_t, int32_t>::iterator featureIter = featureIds.begin(); featureIter != featureIds.end(); ++featureIter)
    {
      int32_t gid = featureIter.key(); // The current Feature Id
      size_t size = featureIter.value(); // The number of triangles for this feature id
      std::vector<T> buffer(size * 3, 0);
      totalCellsWritten += size * 3;
      size_t index = 0;

      for (int j = 0; j < triangleCount; j++)
      {
        if (m_SurfaceMeshFaceLabels[j * 2] != gid && m_SurfaceMeshFaceLabels[j * 2 + 1] != gid) { continue; }
        // Get the data
        T s0 = static_cast<T>(m[j * 3 + 0]);
        T s1 = static_cast<T>(m[j * 3 + 1]);
        T s2 = static_cast<T>(m[j * 3 + 2]);
        // Flip the normal if needed because the current feature id is assigned to the triangle.labels[1]
        if (m_SurfaceMeshFaceLabels[j * 2 + 1] == gid )
        {
          s0 *= -1.0;
          s1 *= -1.0;
          s2 *= -1.0;
        }
        // Write the values to the buffer after an Endian swap.
        if(writeBinaryData == true)
        {
          DREAM3D::Endian::FromSystemToBig::convert(s0);
          buffer[index] = s0;
          ++index;

          DREAM3D::Endian::FromSystemToBig::convert(s1);
          buffer[index] = s1;
          ++index;

          DREAM3D::Endian::FromSystemToBig::convert(s2);
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
template<typename DataContainer, typename T>
void writeCellVectorData(DataContainer* dc, const QString& faceAttributeMatrixName, const QString& dataName, const QString& dataType,
                         bool writeBinaryData, const QString& vtkAttributeType,
                         FILE* vtkFile, QMap<int32_t, int32_t>& featureIds)
{
  StructArray<FaceArray::Face_t>& triangles = *(dc->getFaces());

  int triangleCount = triangles.getNumberOfTuples();

  IDataArray::Pointer data = dc->getAttributeMatrix(faceAttributeMatrixName)->getAttributeArray(dataName);
  QString ss;
  if (NULL != data.get())
  {
    T* m = reinterpret_cast<T*>(data->getVoidPointer(0));
    fprintf(vtkFile, "\n");
    fprintf(vtkFile, "%s %s %s\n", vtkAttributeType.toLatin1().data(), dataName.toLatin1().data(), dataType.toLatin1().data());
    for(int i = 0; i < triangleCount; ++i)
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

      }
      else
      {

        ss << m[i * 3 + 0] << " " << m[i * 3 + 1] << " " << m[i * 3 + 2] << " ";

        fprintf(vtkFile, "%s ", ss.toLatin1().data());
        if (i % 25 == 0) { fprintf(vtkFile, "\n"); }
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
  if (NULL == vtkFile)
  {
    return -1;
  }

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());

  // Write the triangle region ids
  FaceArray& triangles = *(sm->getFaces());

  int triangleCount = triangles.getNumberOfTuples();
  int swapped;

  // This is like a "section header"
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "CELL_DATA %d\n", triangleCount * 2);

  int32_t totalCellsWritten = 0;

  // Write the FeatureId Data to the file
  fprintf(vtkFile, "SCALARS FeatureID int 1\n");
  fprintf(vtkFile, "LOOKUP_TABLE default\n");

  // Loop over all the features
  for(QMap<int32_t, int32_t>::iterator featureIter = featureIds.begin(); featureIter != featureIds.end(); ++featureIter)
  {
    int32_t gid = featureIter.key(); // The current Feature Id
    size_t size = featureIter.value(); // The number of triangles for this feature id
    std::vector<int32_t> buffer(size, 0);
    totalCellsWritten += size;

    // Endian Swap our current feature Id since we are going to write it a bunch of times.
    swapped = gid;
    DREAM3D::Endian::FromSystemToBig::convert(swapped);
    size_t index = 0;

    // Loop over all the triangles looking for the current feature id
    // this is probably sub-optimal as if we have 1000 features we are going to loop 1000 times but this will use the
    // least amount of memory. We could run a filter to group the triangles by feature but then we would need an
    // additional amount of memory equal to 3X the memory used for the triangle list because every triangle will be listed
    // twice. We could get some slightly better performance if we buffered 4K worth of data then wrote out that data
    // in one chunk versus what we are doing here.
    for (int j = 0; j < triangleCount; j++)
    {
      if (m_SurfaceMeshFaceLabels[j * 2] == gid || m_SurfaceMeshFaceLabels[j * 2 + 1] == gid)
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
      DREAM3D::Endian::FromSystemToBig::convert(swapped);
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
  notifyStatusMessage(getHumanLabel(), "Writing Face Normals...");
  writeCellNormalData<SurfaceDataContainer, double>(sm, getFaceAttributeMatrixName(), DREAM3D::FaceData::SurfaceMeshFaceNormals,
                                                    "double", m_WriteBinaryFile, vtkFile, featureIds, m_SurfaceMeshFaceLabels);

  notifyStatusMessage(getHumanLabel(), "Writing Principal Curvature 1");
  writeCellScalarData<SurfaceDataContainer, double>(sm, getFaceAttributeMatrixName(), DREAM3D::FaceData::SurfaceMeshPrincipalCurvature1,
                                                    "double", m_WriteBinaryFile, vtkFile, featureIds, m_SurfaceMeshFaceLabels);
  notifyStatusMessage(getHumanLabel(), "Writing Principal Curvature 2");
  writeCellScalarData<SurfaceDataContainer, double>(sm, getFaceAttributeMatrixName(), DREAM3D::FaceData::SurfaceMeshPrincipalCurvature2,
                                                    "double", m_WriteBinaryFile, vtkFile, featureIds, m_SurfaceMeshFaceLabels);

  notifyStatusMessage(getHumanLabel(), "Writing Feature Face Id");
  writeCellScalarData<SurfaceDataContainer, int32_t>(sm, getFaceAttributeMatrixName(), DREAM3D::FaceData::SurfaceMeshFeatureFaceId,
                                                     "int", m_WriteBinaryFile, vtkFile, featureIds, m_SurfaceMeshFaceLabels);

  notifyStatusMessage(getHumanLabel(), "Writing Gaussian Curvature");
  writeCellScalarData<SurfaceDataContainer, double>(sm, getFaceAttributeMatrixName(), DREAM3D::FaceData::SurfaceMeshGaussianCurvatures,
                                                    "double", m_WriteBinaryFile, vtkFile, featureIds, m_SurfaceMeshFaceLabels);

  notifyStatusMessage(getHumanLabel(), "Writing Mean Curvature");
  writeCellScalarData<SurfaceDataContainer, double>(sm, getFaceAttributeMatrixName(), DREAM3D::FaceData::SurfaceMeshMeanCurvatures,
                                                    "double", m_WriteBinaryFile, vtkFile, featureIds, m_SurfaceMeshFaceLabels);
#if 0
  writeCellVectorData<SurfaceDataContainer, double>(sm, getFaceAttributeMatrixName(), DREAM3D::CellData::SurfaceMeshPrincipalDirection1,
                                                    "double", m_WriteBinaryFile, "VECTORS", vtkFile, nT);

  writeCellVectorData<SurfaceDataContainer, double>(sm, getFaceAttributeMatrixName(), DREAM3D::CellData::SurfaceMeshPrincipalDirection2,
                                                    "double", m_WriteBinaryFile, "VECTORS", vtkFile, nT);

  writeCellNormalData<SurfaceDataContainer, double>(sm, getFaceAttributeMatrixName(), "Goldfeather_Triangle_Normals",
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
    filter->setSurfaceMeshNodeTypeArrayPath(getSurfaceMeshNodeTypeArrayPath());
    filter->setSurfaceMeshFaceLabelsArrayPath(getSurfaceMeshFaceLabelsArrayPath());
    filter->setOutputVtkFile( getOutputVtkFile() );
    filter->setWriteBinaryFile( getWriteBinaryFile() );
  }
  return filter;
}
