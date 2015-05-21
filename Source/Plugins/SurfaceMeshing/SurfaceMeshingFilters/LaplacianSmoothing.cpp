/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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


#include "LaplacianSmoothing.h"

#include <stdio.h>
#include <sstream>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LaplacianSmoothing::LaplacianSmoothing() :
  SurfaceMeshFilter(),
  m_SurfaceDataContainerName(DREAM3D::Defaults::DataContainerName),
  m_SurfaceMeshNodeTypeArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::VertexAttributeMatrixName, DREAM3D::VertexData::SurfaceMeshNodeType),
  m_SurfaceMeshFaceLabelsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_IterationSteps(1),
  m_Lambda(0.1f),
  m_SurfacePointLambda(0.0f),
  m_TripleLineLambda(0.0f),
  m_QuadPointLambda(0.0f),
  m_SurfaceTripleLineLambda(0.0f),
  m_SurfaceQuadPointLambda(0.0f),
  m_SurfaceMeshNodeTypeArrayName(DREAM3D::VertexData::SurfaceMeshNodeType),
  m_SurfaceMeshNodeType(NULL),
  m_SurfaceMeshFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceLabels(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LaplacianSmoothing::~LaplacianSmoothing()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LaplacianSmoothing::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Iteration Steps", "IterationSteps", FilterParameterWidgetType::IntWidget, getIterationSteps(), false));
  parameters.push_back(FilterParameter::New("Default Lambda", "Lambda", FilterParameterWidgetType::DoubleWidget, getLambda(), false));
  parameters.push_back(FilterParameter::New("Triple Line Lambda", "TripleLineLambda", FilterParameterWidgetType::DoubleWidget, getTripleLineLambda(), false));
  parameters.push_back(FilterParameter::New("Quadruple Points Lambda", "QuadPointLambda", FilterParameterWidgetType::DoubleWidget, getQuadPointLambda(), false));
  parameters.push_back(FilterParameter::New("Outer Points Lambda", "SurfacePointLambda", FilterParameterWidgetType::DoubleWidget, getSurfacePointLambda(), false));
  parameters.push_back(FilterParameter::New("Outer Triple Line Lambda", "SurfaceTripleLineLambda", FilterParameterWidgetType::DoubleWidget, getSurfaceTripleLineLambda(), false));
  parameters.push_back(FilterParameter::New("Outer Quadruple Points Lambda", "SurfaceQuadPointLambda", FilterParameterWidgetType::DoubleWidget, getSurfaceQuadPointLambda(), false));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Node Type", "SurfaceMeshNodeTypeArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSurfaceMeshNodeTypeArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Face Labels", "SurfaceMeshFaceLabelsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSurfaceMeshFaceLabelsArrayPath(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LaplacianSmoothing::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setIterationSteps( reader->readValue("IterationSteps", getIterationSteps()) );
  setLambda( reader->readValue("Lambda", getLambda()) );
  setTripleLineLambda( reader->readValue("TripleLineLambda", getTripleLineLambda()) );
  setQuadPointLambda( reader->readValue("QuadPointLambda", getQuadPointLambda()) );
  setSurfacePointLambda( reader->readValue("SurfacePointLambda", getSurfacePointLambda()) );
  setSurfaceTripleLineLambda( reader->readValue("SurfaceTripleLineLambda", getSurfaceTripleLineLambda()) );
  setSurfaceQuadPointLambda( reader->readValue("SurfaceQuadPointLambda", getSurfaceQuadPointLambda()) );
  setSurfaceMeshNodeTypeArrayPath(reader->readDataArrayPath("SurfaceMeshNodeTypeArrayPath", getSurfaceMeshNodeTypeArrayPath() ) );
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LaplacianSmoothing::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(IterationSteps)
  DREAM3D_FILTER_WRITE_PARAMETER(Lambda)
  DREAM3D_FILTER_WRITE_PARAMETER(TripleLineLambda)
  DREAM3D_FILTER_WRITE_PARAMETER(QuadPointLambda)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfacePointLambda)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceTripleLineLambda)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceQuadPointLambda)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshNodeTypeArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshFaceLabelsArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LaplacianSmoothing::dataCheck()
{
  // Algorithm should work for ANY surface mesh
  IGeometry2D::Pointer iGeom2D = getDataContainerArray()->getPrereqGeometryFromDataContainer<IGeometry2D>(this, getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());
  if(getErrorCondition() < 0) { return; }

  QVector<size_t> cDims(1, 1);
  m_SurfaceMeshNodeTypePtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter>(this, getSurfaceMeshNodeTypeArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshNodeTypePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshNodeType = m_SurfaceMeshNodeTypePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 2;
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // If we had no problems up to this point, verify that the sizes of the node/face arrays match the chosen geometry
  if (getErrorCondition() >= 0)
  {
    if (m_SurfaceMeshFaceLabelsPtr.lock()->getNumberOfTuples() != iGeom2D->getNumberOfElements())
    {
      QString ss = QObject::tr("The number of tuples (%1) in the selected SurfaceMeshFaceLabels array does not match the number of elements (%2) in the selected Geometry").arg(m_SurfaceMeshFaceLabelsPtr.lock()->getNumberOfTuples()).arg(iGeom2D->getNumberOfElements());
      setErrorCondition(-5555);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    if (m_SurfaceMeshNodeTypePtr.lock()->getNumberOfTuples() != iGeom2D->getNumberOfVertices())
    {
      QString ss = QObject::tr("The number of tuples (%1) in the selected SurfaceMeshNodeType array does not match the number of elements (%2) in the selected Geometry").arg(m_SurfaceMeshNodeTypePtr.lock()->getNumberOfTuples()).arg(iGeom2D->getNumberOfElements());
      setErrorCondition(-5555);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
  }

  setSurfaceDataContainerName(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LaplacianSmoothing::preflight()
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
void LaplacianSmoothing::execute()
{
  int32_t err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  err = edgeBasedSmoothing();

  if (err < 0)
  {
    notifyErrorMessage(getHumanLabel(), "Error smoothing the surface mesh", getErrorCondition());
    return;
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t LaplacianSmoothing::generateLambdaArray()
{
  notifyStatusMessage(getHumanLabel(), "Generating Lambda values");

  size_t numNodes = m_SurfaceMeshNodeTypePtr.lock()->getNumberOfTuples();

  DataArray<float>::Pointer lambdas = DataArray<float>::CreateArray(numNodes, "_INTERNAL_USE_ONLY_Laplacian_Smoothing_Lambda_Array");
  lambdas->initializeWithZeros();

  for (size_t i = 0; i < numNodes; ++i)
  {
    switch (m_SurfaceMeshNodeType[i])
    {
      case DREAM3D::SurfaceMesh::NodeType::Unused:
        break;
      case DREAM3D::SurfaceMesh::NodeType::Default:
        lambdas->setValue(i, m_Lambda);
        break;
      case DREAM3D::SurfaceMesh::NodeType::TriplePoint:
        lambdas->setValue(i, m_TripleLineLambda);
        break;
      case DREAM3D::SurfaceMesh::NodeType::QuadPoint:
        lambdas->setValue(i, m_QuadPointLambda);
        break;
      case DREAM3D::SurfaceMesh::NodeType::SurfaceDefault:
        lambdas->setValue(i, m_SurfacePointLambda);
        break;
      case DREAM3D::SurfaceMesh::NodeType::SurfaceTriplePoint:
        lambdas->setValue(i, m_SurfaceTripleLineLambda);
        break;
      case DREAM3D::SurfaceMesh::NodeType::SurfaceQuadPoint:
        lambdas->setValue(i, m_SurfaceQuadPointLambda);
        break;
      default:
        break;
    }
  }

  setLambdaArray(lambdas);
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t LaplacianSmoothing::edgeBasedSmoothing()
{
  int32_t err = 0;
  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getSurfaceDataContainerName());
  IGeometry2D::Pointer surfaceMesh = sm->getGeometryAs<IGeometry2D>();
  float* verts = surfaceMesh->getVertexPointer(0);
  int64_t nvert = surfaceMesh->getNumberOfVertices();

  // Generate the Lambda Array
  err = generateLambdaArray();
  if (err < 0)
  {
    setErrorCondition(-557);
    notifyErrorMessage(getHumanLabel(), "Error generating the Lambda Array", getErrorCondition());
    return err;
  }

  // Get a Pointer to the Lambda array for conveneince
  DataArray<float>::Pointer lambdas = getLambdaArray();
  float* lambda = lambdas->getPointer(0);

  //  Generate the Unique Edges
  if (NULL == surfaceMesh->getEdges().get())
  {
    err = surfaceMesh->findEdges();
  }
  if (err < 0)
  {
    setErrorCondition(-560);
    notifyErrorMessage(getHumanLabel(), "Error retrieving the SharedEdgeList", getErrorCondition());
    return getErrorCondition();
  }

  int64_t* uedges = surfaceMesh->getEdgePointer(0);
  int64_t nedges = surfaceMesh->getNumberOfEdges();

  notifyStatusMessage(getHumanLabel(), "Starting to Smooth Vertices");

  DataArray<int32_t>::Pointer numConnections = DataArray<int32_t>::CreateArray(nvert, "_INTERNAL_USE_ONLY_Laplacian_Smoothing_NumberConnections_Array");
  numConnections->initializeWithZeros();
  int32_t* ncon = numConnections->getPointer(0);

  QVector<size_t> cDims(1, 3);
  DataArray<double>::Pointer deltaArray = DataArray<double>::CreateArray(nvert, cDims, "_INTERNAL_USE_ONLY_Laplacian_Smoothing_Delta_Array");
  deltaArray->initializeWithZeros();
  double* delta = deltaArray->getPointer(0);

  double dlta = 0.0;
  for (int32_t q = 0; q < m_IterationSteps; q++)
  {
    if (getCancel() == true) { return -1; }
    QString ss = QObject::tr("Iteration %1").arg(q);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    for (int64_t i = 0; i < nedges; i++)
    {
      int64_t in1 = uedges[2 * i];   // row of the first vertex
      int64_t in2 = uedges[2 * i + 1]; // row the second vertex

      for (int32_t j = 0; j < 3; j++)
      {
        BOOST_ASSERT( static_cast<size_t>(3 * in1 + j) < static_cast<size_t>(nvert * 3) );
        BOOST_ASSERT( static_cast<size_t>(3 * in2 + j) < static_cast<size_t>(nvert * 3) );
        dlta = verts[3 * in2 + j] - verts[3 * in1 + j];
        delta[3 * in1 + j] += dlta;
        delta[3 * in2 + j] += -1.0 * dlta;
      }
      ncon[in1] += 1;
      ncon[in2] += 1;
    }

    float ll = 0.0f;
    for (int64_t i = 0; i < nvert; i++)
    {
      for (int32_t j = 0; j < 3; j++)
      {
        int64_t in0 = 3 * i + j;
        dlta = delta[in0] / ncon[i];

        ll = lambda[i];
        verts[3 * i + j] += ll * dlta;
        delta[in0] = 0.0; // reset for next iteration
      }
      ncon[i] = 0; // reset for next iteration
    }
  }

  return err;
}

// -----------------------------------------------------------------------------
// This is just here for some debugging issues.
// -----------------------------------------------------------------------------
#if 0
void LaplacianSmoothing::writeVTKFile(const QString& outputVtkFile)
{

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getSurfaceDataContainerName());  /* Place all your code to execute your filter here. */
  VertexArray& nodes = *(sm->getVertices());
  int nNodes = nodes.getNumberOfTuples();
  bool m_WriteBinaryFile = true;

  FILE* vtkFile = NULL;
  vtkFile = fopen(outputVtkFile.toLatin1().data(), "wb");
  if (NULL == vtkFile)
  {
    setErrorCondition(-90123);
    QString ss = QObject::tr("Error creating file '%1'").arg(outputVtkFile);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  Detail::ScopedFileMonitor vtkFileMonitor(vtkFile);

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


  fprintf(vtkFile, "POINTS %d float\n", nNodes);
  float pos[3] = {0.0f, 0.0f, 0.0f};

  size_t totalWritten = 0;
  // Write the POINTS data (Vertex)
  for (int i = 0; i < nNodes; i++)
  {
    VertexArray::Vert_t& n = nodes[i]; // Get the current Node
    //  if (m_SurfaceMeshNodeType[i] > 0)
    {
      pos[0] = static_cast<float>(n.pos[0]);
      pos[1] = static_cast<float>(n.pos[1]);
      pos[2] = static_cast<float>(n.pos[2]);
      if (m_WriteBinaryFile == true)
      {
        DREAM3D::Endian::FromSystemToBig::convert<float>(pos[0]);
        DREAM3D::Endian::FromSystemToBig::convert<float>(pos[1]);
        DREAM3D::Endian::FromSystemToBig::convert<float>(pos[2]);
        totalWritten = fwrite(pos, sizeof(float), 3, vtkFile);
        if (totalWritten != sizeof(float) * 3)
        {

        }
      }
      else
      {
        fprintf(vtkFile, "%4.4f %4.4f %4.4f\n", pos[0], pos[1], pos[2]); // Write the positions to the output file
      }
    }
  }

  // Write the triangle indices into the vtk File
  FaceArray& triangles = *(sm->getFaces());
  int triangleCount = 0;
  int end = triangles.getNumberOfTuples();
  int featureInterest = 9;
  for(int i = 0; i < end; ++i)
  {
    //FaceArray::Face_t* tri = triangles.getPointer(i);
    if (m_SurfaceMeshFaceLabels[i * 2] == featureInterest || m_SurfaceMeshFaceLabels[i * 2 + 1] == featureInterest)
    {
      ++triangleCount;
    }
  }


  int tData[4];
  // Write the CELLS Data
  //  int start = 3094380;
  //  int end = 3094450;
  //  int triangleCount = end - start;
  qDebug() << "---------------------------------------------------------------------------" << "\n";
  qDebug() << outputVtkFile << "\n";
  fprintf(vtkFile, "\nPOLYGONS %d %d\n", triangleCount, (triangleCount * 4));
  for (int tid = 0; tid < end; ++tid)
  {
    //FaceArray::Face_t* tri = triangles.getPointer(tid);
    if (m_SurfaceMeshFaceLabels[tid * 2] == featureInterest || m_SurfaceMeshFaceLabels[tid * 2 + 1] == featureInterest)
    {
      tData[1] = triangles[tid].verts[0];
      tData[2] = triangles[tid].verts[1];
      tData[3] = triangles[tid].verts[2];
      //      qDebug() << tid << "\n  " << nodes[tData[1]].coord[0] << " " << nodes[tData[1]].coord[1]  << " " << nodes[tData[1]].coord[2]  << "\n";
      //      qDebug() << "  " << nodes[tData[2]].coord[0] << " " << nodes[tData[2]].coord[1]  << " " << nodes[tData[2]].coord[2]  << "\n";
      //      qDebug() << "  " << nodes[tData[3]].coord[0] << " " << nodes[tData[3]].coord[1]  << " " << nodes[tData[3]].coord[2]  << "\n";
      if (m_WriteBinaryFile == true)
      {
        tData[0] = 3; // Push on the total number of entries for this entry
        DREAM3D::Endian::FromSystemToBig::convert<int>(tData[0]);
        DREAM3D::Endian::FromSystemToBig::convert<int>(tData[1]); // Index of Vertex 0
        DREAM3D::Endian::FromSystemToBig::convert<int>(tData[2]); // Index of Vertex 1
        DREAM3D::Endian::FromSystemToBig::convert<int>(tData[3]); // Index of Vertex 2
        fwrite(tData, sizeof(int), 4, vtkFile);
      }
      else
      {
        fprintf(vtkFile, "3 %d %d %d\n", tData[1], tData[2], tData[3]);
      }
    }
  }

  fprintf(vtkFile, "\n");
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer LaplacianSmoothing::newFilterInstance(bool copyFilterParameters)
{
  LaplacianSmoothing::Pointer filter = LaplacianSmoothing::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString LaplacianSmoothing::getCompiledLibraryName()
{ return SurfaceMeshingConstants::SurfaceMeshingBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString LaplacianSmoothing::getGroupName()
{ return DREAM3D::FilterGroups::SurfaceMeshingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString LaplacianSmoothing::getSubGroupName()
{ return DREAM3D::FilterSubGroups::SmoothingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString LaplacianSmoothing::getHumanLabel()
{ return "Laplacian Smoothing"; }
