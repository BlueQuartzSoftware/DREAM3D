/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2013 Dr. Joseph C. Tucker (UES, Inc.)
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
 * Neither the name of Joseph C. Tucker, Michael A. Groeber, Michael A. Jackson,
 * UES, Inc., the US Air Force, BlueQuartz Software nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *                   FA8650-07-D-5800 and FA8650-10-D-5226
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "LocalDislocationDensityCalculator.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/GeometryMath.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LocalDislocationDensityCalculator::LocalDislocationDensityCalculator() :
  AbstractFilter(),
  m_EdgeDataContainerName(DREAM3D::Defaults::DataContainerName),
  m_OutputDataContainerName(DREAM3D::Defaults::NewDataContainerName),
  m_OutputAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_OutputArrayName("DislocationLineDensity"),
  m_OutputArray(NULL)
{
  m_CellSize.x = 2.0;
  m_CellSize.y = 2.0;
  m_CellSize.z = 2.0;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LocalDislocationDensityCalculator::~LocalDislocationDensityCalculator()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LocalDislocationDensityCalculator::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Cell Size");
    parameter->setPropertyName("CellSize");
    parameter->setWidgetType(FilterParameterWidgetType::FloatVec3Widget);
    parameter->setUnits("microns");
    parameters.push_back(parameter);
  }

  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
  parameters.push_back(FilterParameter::New("Edge Data Container", "EdgeDataContainerName", FilterParameterWidgetType::DataContainerSelectionWidget, getEdgeDataContainerName(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
  parameters.push_back(FilterParameter::New("Volume Data Container", "OutputDataContainerName", FilterParameterWidgetType::StringWidget, getOutputDataContainerName(), true, ""));
  parameters.push_back(FilterParameter::New("Cell Attribute Matrix", "OutputAttributeMatrixName", FilterParameterWidgetType::StringWidget, getOutputAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("Dislocation Line Density Array Name", "OutputArrayName", FilterParameterWidgetType::StringWidget, getOutputArrayName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LocalDislocationDensityCalculator::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setEdgeDataContainerName( reader->readString( "EdgeDataContainerName", getEdgeDataContainerName() ) );
  setOutputDataContainerName( reader->readString( "OutputDataContainerName", getOutputDataContainerName() ) );
  setOutputAttributeMatrixName( reader->readString( "OutputAttributeMatrixName", getOutputAttributeMatrixName() ) );
  setOutputArrayName( reader->readString( "OutputArrayName", getOutputArrayName() ) );
  setCellSize( reader->readFloatVec3("CellSize", getCellSize() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LocalDislocationDensityCalculator::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(EdgeDataContainerName)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputDataContainerName)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(CellSize)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LocalDislocationDensityCalculator::updateCellInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_OutputArrayPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_OutputArray = m_OutputArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LocalDislocationDensityCalculator::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  // First sanity check the inputs and output names. All must be filled in

  if(getOutputDataContainerName().isEmpty() == true)
  {
    QString ss = QObject::tr("The output DataContainer name is empty. Please assign a name for the created DataContainer");
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(getOutputAttributeMatrixName().isEmpty() == true)
  {
    QString ss = QObject::tr("The output AttributeMatrix name is empty. Please assign a name for the created AttributeMatrix");
    setErrorCondition(-11002);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(getOutputArrayName().isEmpty() == true)
  {
    QString ss = QObject::tr("The output array name is empty. Please assign a name for the created array");
    setErrorCondition(-11003);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  // we can not go any further until all of the names are set.
  if(getErrorCondition() < 0) { return; }

  // Next check the existing DataContainer/AttributeMatrix
DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getEdgeDataContainerName());
  if(getErrorCondition() < 0) { return; }

  // We MUST have Vertices defined.
  if(m->getVertices().get() == NULL)
  {
    setErrorCondition(-384);
    notifyErrorMessage(getHumanLabel(), "Edge DataContainer missing Vertices", getErrorCondition());
  }
  // We MUST have Edges defined also.
  if(m->getEdges().get() == NULL)
  {
    setErrorCondition(-384);
    notifyErrorMessage(getHumanLabel(), "Edge DataContainer missing Edges", getErrorCondition());
  }

  // Create a new DataContainer
  DataContainer::Pointer m2 = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getOutputDataContainerName());
  if(getErrorCondition() < 0) { return; }

  //Create the cell attrMat in the new data container
  QVector<size_t> tDims(3, 0);
  AttributeMatrix::Pointer newCellAttrMat = m2->createNonPrereqAttributeMatrix<AbstractFilter>(this, getOutputAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Cell);
  if(getErrorCondition() < 0) { return; }

  //Get the name and create the array in the new data attrMat
  QVector<size_t> dims(1, 1);
  tempPath.update(getOutputDataContainerName(), getOutputAttributeMatrixName(), getOutputArrayName() );
  m_OutputArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_OutputArrayPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_OutputArray = m_OutputArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LocalDislocationDensityCalculator::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);

  /* *** THIS FILTER NEEDS TO BE CHECKED *** */
  setErrorCondition(0xABABABAB);
  QString ss = QObject::tr("Filter is NOT updated for IGeometry Redesign. A Programmer needs to check this filter. Please report this to the DREAM3D developers.");
  notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  /* *** THIS FILTER NEEDS TO BE CHECKED *** */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LocalDislocationDensityCalculator::execute()
{
  QString ss;
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer edc = getDataContainerArray()->getDataContainer(getEdgeDataContainerName());
  DataContainer::Pointer vdc = getDataContainerArray()->getDataContainer(getOutputDataContainerName());
  AttributeMatrix::Pointer cellAttrMat = vdc->getAttributeMatrix(getOutputAttributeMatrixName());

  VertexArray::Pointer nodesPtr = edc->getVertices();
  size_t numNodes = nodesPtr->getNumberOfTuples();
  VertexArray::Vert_t* nodes = nodesPtr->getPointer(0);

  EdgeArray::Pointer edgesPtr = edc->getEdges();
  size_t numEdges = edgesPtr->getNumberOfTuples();
  EdgeArray::Edge_t* edges = edgesPtr->getPointer(0);

  float xMin = 1000000000.0;
  float yMin = 1000000000.0;
  float zMin = 1000000000.0;
  float xMax = 0.0;
  float yMax = 0.0;
  float zMax = 0.0;
  float x, y, z;
  for(size_t i = 0; i < numNodes; i++)
  {
    x = nodes[i].pos[0];
    y = nodes[i].pos[1];
    z = nodes[i].pos[2];
    if(x < xMin) { xMin = x; }
    if(x > xMax) { xMax = x; }
    if(y < yMin) { yMin = y; }
    if(y > yMax) { yMax = y; }
    if(z < zMin) { zMin = z; }
    if(z > zMax) { zMax = z; }
  }

  FloatVec3_t halfCellSize;
  halfCellSize.x = (m_CellSize.x / 2.0);
  halfCellSize.y = (m_CellSize.y / 2.0);
  halfCellSize.z = (m_CellSize.z / 2.0);
  FloatVec3_t quarterCellSize;
  quarterCellSize.x = (m_CellSize.x / 4.0);
  quarterCellSize.y = (m_CellSize.y / 4.0);
  quarterCellSize.z = (m_CellSize.z / 4.0);

  /* FIXME: ImageGeom */ vdc->getGeometryAs<ImageGeom>()->setOrigin(xMin, yMin, zMin);
  size_t dcDims[3];
  dcDims[0] = size_t((xMax - xMin) / halfCellSize.x);
  dcDims[1] = size_t((yMax - yMin) / halfCellSize.y);
  dcDims[2] = size_t((zMax - zMin) / halfCellSize.z);
  /* FIXME: ImageGeom */ vdc->getGeometryAs<ImageGeom>()->setDimensions(dcDims[0], dcDims[1], dcDims[2]);
  /* FIXME: ImageGeom */ vdc->getGeometryAs<ImageGeom>()->setResolution(m_CellSize.x / 2.0, m_CellSize.y / 2.0, m_CellSize.z / 2.0);

  QVector<size_t> tDims(3, 0);
  tDims[0] = dcDims[0];
  tDims[1] = dcDims[1];
  tDims[2] = dcDims[2];
  cellAttrMat->resizeAttributeArrays(tDims);
  updateCellInstancePointers();

  VertexArray::Vert_t point1, point2, corner1, corner2;
  size_t xCellMin, xCellMax;
  size_t yCellMin, yCellMax;
  size_t zCellMin, zCellMax;
  float x1, y1, z1, x2, y2, z2;
  size_t zStride, yStride;
  float length;
  for(size_t i = 0; i < numEdges; i++)
  {
    point1 = nodes[edges[i].verts[0]];
    point2 = nodes[edges[i].verts[1]];
    x1 = (point1.pos[0] - xMin);
    y1 = (point1.pos[1] - yMin);
    z1 = (point1.pos[2] - zMin);
    x2 = (point2.pos[0] - xMin);
    y2 = (point2.pos[1] - yMin);
    z2 = (point2.pos[2] - zMin);
    if(x1 > x2) { xCellMin = size_t(x2 / quarterCellSize.x), xCellMax = size_t(x1 / quarterCellSize.x); }
    else { xCellMin = size_t(x1 / quarterCellSize.x), xCellMax = size_t(x2 / quarterCellSize.x); }
    if(y1 > y2) { yCellMin = size_t(y2 / quarterCellSize.y), yCellMax = size_t(y1 / quarterCellSize.y); }
    else { yCellMin = size_t(y1 / quarterCellSize.y), yCellMax = size_t(y2 / quarterCellSize.y); }
    if(z1 > z2) { zCellMin = size_t(z2 / quarterCellSize.z), zCellMax = size_t(z1 / quarterCellSize.z); }
    else { zCellMin = size_t(z1 / quarterCellSize.z), zCellMax = size_t(z2 / quarterCellSize.z); }
    xCellMin = (xCellMin - 1) / 2;
    yCellMin = (yCellMin - 1) / 2;
    zCellMin = (zCellMin - 1) / 2;
    xCellMax = ((xCellMax - 1) / 2) + 1;
    yCellMax = ((yCellMax - 1) / 2) + 1;
    zCellMax = ((zCellMax - 1) / 2) + 1;
    for(size_t j = zCellMin; j <= zCellMax; j++)
    {
      zStride = j * tDims[0] * tDims[1];
      corner1.pos[2] = (j * halfCellSize.z) - halfCellSize.z + quarterCellSize.z + zMin;
      corner2.pos[2] = (j * halfCellSize.z) + halfCellSize.z + quarterCellSize.z + zMin;
      for(size_t k = yCellMin; k <= yCellMax; k++)
      {
        yStride = k * tDims[0];
        corner1.pos[1] = (k * halfCellSize.y) - halfCellSize.y + quarterCellSize.y + yMin;
        corner2.pos[1] = (k * halfCellSize.y) + halfCellSize.y + quarterCellSize.y + yMin;
        for(size_t l = xCellMin; l <= xCellMax; l++)
        {
          corner1.pos[0] = (l * halfCellSize.x) - halfCellSize.x + quarterCellSize.x + xMin;
          corner2.pos[0] = (l * halfCellSize.x) + halfCellSize.x + quarterCellSize.x + xMin;
          length = GeometryMath::LengthOfRayInBox(point1, point2, corner1, corner2);
          m_OutputArray[zStride + yStride + l] += length;
        }
      }
    }
  }

  float cellVolume = m_CellSize.x * m_CellSize.y * m_CellSize.z;
  for(size_t j = 0; j < tDims[2]; j++)
  {
    zStride = j * tDims[0] * tDims[1];
    for(size_t k = 0; k < tDims[1]; k++)
    {
      yStride = k * tDims[0];
      for(size_t l = 0; l < tDims[0]; l++)
      {
        m_OutputArray[zStride + yStride + l] /= cellVolume;
        //convert to m/mm^3 from um/um^3
        m_OutputArray[zStride + yStride + l] *= 1.0E12f;
      }
    }
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer LocalDislocationDensityCalculator::newFilterInstance(bool copyFilterParameters)
{
  LocalDislocationDensityCalculator::Pointer filter = LocalDislocationDensityCalculator::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString LocalDislocationDensityCalculator::getCompiledLibraryName()
{ return DDDAnalysisToolbox::DDDAnalysisToolboxBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString LocalDislocationDensityCalculator::getGroupName()
{ return DREAM3D::FilterGroups::DDDAnalysisFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString LocalDislocationDensityCalculator::getSubGroupName()
{ return DREAM3D::FilterSubGroups::StatisticsFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString LocalDislocationDensityCalculator::getHumanLabel()
{ return "Calculate Local Dislocation Densities"; }

