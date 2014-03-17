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
  m_DataContainerName(DREAM3D::Defaults::EdgeDataContainerName),
  m_OutputDataContainerName(DREAM3D::Defaults::NewVolumeDataContainerName),
  m_OutputAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_OutputArrayName(""),
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
    parameter->setValueType("FloatVec3_t");
    parameter->setUnits("microns");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Output DataContainer Name");
    parameter->setPropertyName("OutputDataContainerName");
    parameter->setWidgetType(FilterParameterWidgetType::StringWidget);
    parameter->setValueType("QString");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Output AttributeMatrix Name");
    parameter->setPropertyName("OutputAttributeMatrixName");
    parameter->setWidgetType(FilterParameterWidgetType::StringWidget);
    parameter->setValueType("QString");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Output Data Array Name");
    parameter->setPropertyName("OutputArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::StringWidget);
    parameter->setValueType("QString");
    parameters.push_back(parameter);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LocalDislocationDensityCalculator::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setOutputDataContainerName( reader->readString( "OutputDataContainerName", getOutputDataContainerName() ) );
  setOutputAttributeMatrixName( reader->readString( "OutputAttributeMatrixName", getOutputAttributeMatrixName() ) );
  setOutputArrayName( reader->readString( "OutputArrayName", getOutputArrayName() ) );
  setCellSize( reader->readFloatVec3("CellSize", getCellSize() ) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LocalDislocationDensityCalculator::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("OutputDataContainerName", getOutputDataContainerName() );
  writer->writeValue("OutputAttributeMatrixName", getOutputAttributeMatrixName() );
  writer->writeValue("OutputArrayName", getOutputArrayName() );
  writer->writeValue("CellSize", getCellSize() );
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
  EdgeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<EdgeDataContainer, AbstractFilter>(this, getDataContainerName());
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
  VolumeDataContainer* m2 = getDataContainerArray()->createNonPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getOutputDataContainerName());
  if(getErrorCondition() < 0) { return; }

  //Create the cell attrMat in the new data container
  QVector<size_t> tDims(3, 0);
  AttributeMatrix::Pointer newCellAttrMat = m2->createNonPrereqAttributeMatrix<AbstractFilter>(this, getOutputAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Cell);
  if(getErrorCondition() < 0) { return; }

  //Get the name and create the array in the new data attrMat
  QVector<size_t> dims(1, 1);
  m_OutputArrayPtr = newCellAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this, getOutputArrayName(), 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_OutputArrayPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_OutputArray = m_OutputArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LocalDislocationDensityCalculator::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
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

  EdgeDataContainer* edc = getDataContainerArray()->getDataContainerAs<EdgeDataContainer>(getDataContainerName());
  VolumeDataContainer* vdc = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getOutputDataContainerName());
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
  for(size_t i=0;i<numNodes;i++)
  {
    x = nodes[i].pos[0];
    y = nodes[i].pos[1];
    z = nodes[i].pos[2];
    if(x < xMin) xMin = x;
    if(x > xMax) xMax = x;
    if(y < yMin) yMin = y;
    if(y > yMax) yMax = y;
    if(z < zMin) zMin = z;
    if(z > zMax) zMax = z;
  }

  vdc->setOrigin(xMin, yMin, zMin);
  size_t dcDims[3];
  dcDims[0] = size_t((xMax-xMin)/m_CellSize.x)+1;
  dcDims[1] = size_t((yMax-yMin)/m_CellSize.y)+1;
  dcDims[2] = size_t((zMax-zMin)/m_CellSize.z)+1;
  vdc->setDimensions(dcDims[0], dcDims[1], dcDims[2]);
  vdc->setResolution(m_CellSize.x, m_CellSize.y, m_CellSize.z);

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
    x1 = (point1.pos[0]-xMin);
    y1 = (point1.pos[1]-yMin);
    z1 = (point1.pos[2]-zMin);
    x2 = (point2.pos[0]-xMin);
    y2 = (point2.pos[1]-yMin);
    z2 = (point2.pos[2]-zMin);
    if(x1 > x2) xCellMin = size_t(x2/m_CellSize.x), xCellMax = size_t(x1/m_CellSize.x);
    else xCellMin = size_t(x1/m_CellSize.x), xCellMax = size_t(x2/m_CellSize.x);
    if(y1 > y2) yCellMin = size_t(y2/m_CellSize.y), yCellMax = size_t(y1/m_CellSize.y);
    else yCellMin = size_t(y1/m_CellSize.y), yCellMax = size_t(y2/m_CellSize.y);
    if(z1 > z2) zCellMin = size_t(z2/m_CellSize.z), zCellMax = size_t(z1/m_CellSize.z);
    else zCellMin = size_t(z1/m_CellSize.z), zCellMax = size_t(z2/m_CellSize.z);
    for(size_t j = zCellMin; j <= zCellMax; j++)
    {
      zStride = j*tDims[0]*tDims[1];
      corner1.pos[2] = j*m_CellSize.z + zMin;
      corner2.pos[2] = (j+1)*m_CellSize.z + zMin;
      for(size_t k = yCellMin; k <= yCellMax; k++)
      {
        yStride = k*tDims[0];
        corner1.pos[1] = k*m_CellSize.y + yMin;
        corner2.pos[1] = (k+1)*m_CellSize.y + yMin;
        for(size_t l = xCellMin; l <= xCellMax; l++)
        {
          corner1.pos[0] = l*m_CellSize.x + xMin;
          corner2.pos[0] = (l+1)*m_CellSize.x  + xMin;
          length = GeometryMath::LengthOfRayInBox(point1, point2, corner1, corner2);
          m_OutputArray[zStride+yStride+l] += length;
        }
      }
    }
  }

  float cellVolume = m_CellSize.x*m_CellSize.y*m_CellSize.z;
  for(size_t j = 0; j < tDims[2]; j++)
  {
    zStride = j*tDims[0]*tDims[1];
    for(size_t k = 0; k < tDims[1]; k++)
    {
      yStride = k*tDims[0];
      for(size_t l = 0; l < tDims[0]; l++)
      {
        m_OutputArray[zStride+yStride+l] /= cellVolume;
        //convert to m/mm^3 from um/um^3
        m_OutputArray[zStride+yStride+l] *= 1000000000000.0;
      }
    }
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}


