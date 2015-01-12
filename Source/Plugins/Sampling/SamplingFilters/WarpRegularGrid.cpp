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

#include "WarpRegularGrid.h"

#include <QtCore/QMap>


#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/Utilities/DREAM3DRandom.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WarpRegularGrid::WarpRegularGrid() :
  AbstractFilter(),
  m_NewDataContainerName(DREAM3D::Defaults::NewVolumeDataContainerName),
  m_CellAttributeMatrixPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, ""),
  m_SaveAsNewDataContainer(false)
{
  m_Resolution.x = 1.0f;
  m_Resolution.y = 1.0f;
  m_Resolution.z = 1.0f;

  m_Coefficients.v11 = 1.0f;
  m_Coefficients.v12 = 0.0f;
  m_Coefficients.v13 = 0.0f;
  m_Coefficients.v21 = 0.0f;
  m_Coefficients.v22 = 1.0f;
  m_Coefficients.v23 = 0.0f;
  m_Coefficients.v31 = 0.0f;
  m_Coefficients.v32 = 0.0f;
  m_Coefficients.v33 = 1.0f;

  m_NewMinX = 100000000000.0f;
  m_NewMinY = 100000000000.0f;
  m_NewMinZ = 100000000000.0f;
  m_NewMaxX = 0.0f;
  m_NewMaxY = 0.0f;
  m_NewMaxZ = 0.0f;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WarpRegularGrid::~WarpRegularGrid()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WarpRegularGrid::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Cell Attribute Matrix", "CellAttributeMatrixPath", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getCellAttributeMatrixPath(), false));
  parameters.push_back(FilterParameter::New("Coefficients", "Coefficients", FilterParameterWidgetType::Matrix3x3Widget, getCoefficients(), false, ""));
  parameters.push_back(FilterParameter::New("Resolution", "Resolution", FilterParameterWidgetType::FloatVec3Widget, getResolution(), false, "Microns"));
  QStringList linkedProps;
  linkedProps << "NewDataContainerName";
  parameters.push_back(LinkedBooleanFilterParameter::New("Save As New Data Container", "SaveAsNewDataContainer", getSaveAsNewDataContainer(), linkedProps, false));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("New Data Container Name", "NewDataContainerName", FilterParameterWidgetType::StringWidget, getNewDataContainerName(), true));
  setFilterParameters(parameters);
}


// -----------------------------------------------------------------------------
void WarpRegularGrid::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNewDataContainerName( reader->readString("NewDataContainerName", getNewDataContainerName() ) );
  setCellAttributeMatrixPath( reader->readDataArrayPath("CellAttributeMatrixPath", getCellAttributeMatrixPath() ) );
  setCoefficients( reader->readFloat3x3("Coefficients", getCoefficients() ) );
  setResolution( reader->readFloatVec3("Resolution", getResolution() ) );
  setSaveAsNewDataContainer( reader->readValue("SaveAsNewDataContainer", getSaveAsNewDataContainer()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int WarpRegularGrid::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(NewDataContainerName)
  DREAM3D_FILTER_WRITE_PARAMETER(CellAttributeMatrixPath)
  DREAM3D_FILTER_WRITE_PARAMETER(Coefficients)
  DREAM3D_FILTER_WRITE_PARAMETER(Resolution)
  DREAM3D_FILTER_WRITE_PARAMETER(SaveAsNewDataContainer)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WarpRegularGrid::dataCheck()
{
  VolumeDataContainer* m;
  if(m_SaveAsNewDataContainer == false) { m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getCellAttributeMatrixPath().getDataContainerName()); }
  else
  {
    getDataContainerArray()->duplicateDataContainer(getCellAttributeMatrixPath().getDataContainerName(), getNewDataContainerName());
    m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getNewDataContainerName());
  }
  if(getErrorCondition() < 0 || NULL == m) { return; }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WarpRegularGrid::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();


  if(getErrorCondition() < 0)
  {
    emit preflightExecuted();
    setInPreflight(false);
    return;
  }

  VolumeDataContainer* m;
  if(m_SaveAsNewDataContainer == false)
  {
    m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getCellAttributeMatrixPath().getDataContainerName());
  }
  else
  {
    m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getNewDataContainerName());
  }

  size_t dims[3];
  m->getDimensions(dims);

  float sizex = (dims[0]) * m->getXRes();
  float sizey = (dims[1]) * m->getYRes();
  float sizez = (dims[2]) * m->getZRes();
  int m_XP = int(sizex / m_Resolution.x);
  int m_YP = int(sizey / m_Resolution.y);
  int m_ZP = int(sizez / m_Resolution.z);
  if(m_XP == 0) { m_XP = 1; }
  if(m_YP == 0) { m_YP = 1; }
  if(m_ZP == 0) { m_ZP = 1; }

  m->setDimensions(m_XP, m_YP, m_ZP);
  m->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);

  QVector<size_t> tDims(3, 0);
  tDims[0] = m_XP;
  tDims[1] = m_YP;
  tDims[2] = m_ZP;
  m->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName())->setTupleDimensions(tDims);

  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WarpRegularGrid::execute()
{
  int err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DREAM3D_RANDOMNG_NEW()

  VolumeDataContainer* m;
  if(m_SaveAsNewDataContainer == false) { m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getCellAttributeMatrixPath().getDataContainerName()); }
  else { m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getNewDataContainerName()); }

  if(m->getXRes() == m_Resolution.x
      && m->getYRes() == m_Resolution.y
      && m->getZRes() == m_Resolution.z)
  {
    return;
  }

  AttributeMatrix::Pointer cellAttrMat = m->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());

  size_t dims[3];
  m->getDimensions(dims);

  float sizex = (dims[0]) * m->getXRes();
  float sizey = (dims[1]) * m->getYRes();
  float sizez = (dims[2]) * m->getZRes();
  int m_XP = int(sizex / m_Resolution.x);
  int m_YP = int(sizey / m_Resolution.y);
  int m_ZP = int(sizez / m_Resolution.z);
  if(m_XP == 0) { m_XP = 1; }
  if(m_YP == 0) { m_YP = 1; }
  if(m_ZP == 0) { m_ZP = 1; }
  int64_t totalPoints = m_XP * m_YP * m_ZP;

  float x, y, z;
  int col, row, plane;
  int index;
  int index_old;
  std::vector<size_t> newindicies(totalPoints);

  for (int i = 0; i < m_ZP; i++)
  {
    QString ss = QObject::tr("Changing Resolution - %1 Percent Complete").arg(((float)i / m->getZPoints()) * 100);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    for (int j = 0; j < m_YP; j++)
    {
      for (int k = 0; k < m_XP; k++)
      {
        x = (k * m_Resolution.x);
        y = (j * m_Resolution.y);
        z = (i * m_Resolution.z);
        col = int(x / m->getXRes());
        row = int(y / m->getYRes());
        plane = int(z / m->getZRes());
        index_old = (plane * m->getXPoints() * m->getYPoints()) + (row * m->getXPoints()) + col;
        index = (i * m_XP * m_YP) + (j * m_XP) + k;
        newindicies[index] = index_old;
      }
    }
  }

  QVector<size_t> tDims(3, 0);
  tDims[0] = m_XP;
  tDims[1] = m_YP;
  tDims[2] = m_ZP;
  AttributeMatrix::Pointer newCellAttrMat = AttributeMatrix::New(tDims, cellAttrMat->getName(), cellAttrMat->getType());

  QList<QString> voxelArrayNames = cellAttrMat->getAttributeArrayNames();
  for (QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
    QString name = *iter;
    IDataArray::Pointer p = cellAttrMat->getAttributeArray(*iter);
    // Make a copy of the 'p' array that has the same name. When placed into
    // the data container this will over write the current array with
    // the same name. At least in theory
    IDataArray::Pointer data = p->createNewArray(p->getNumberOfTuples(), p->getComponentDimensions(), p->getName());
    data->resize(totalPoints);
    void* source = NULL;
    void* destination = NULL;
    size_t newIndicies_I = 0;
    int nComp = data->getNumberOfComponents();
    for (size_t i = 0; i < static_cast<size_t>(totalPoints); i++)
    {
      newIndicies_I = newindicies[i];

      source = p->getVoidPointer((nComp * newIndicies_I));
      destination = data->getVoidPointer((data->getNumberOfComponents() * i));
      ::memcpy(destination, source, p->getTypeSize() * data->getNumberOfComponents());
    }
    cellAttrMat->removeAttributeArray(*iter);
    newCellAttrMat->addAttributeArray(*iter, data);
  }
  m->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
  m->setDimensions(m_XP, m_YP, m_ZP);
  m->removeAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());
  m->addAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName(), newCellAttrMat);

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WarpRegularGrid::determine_warped_points(FloatArrayType::Pointer coordsPtr)
{
  float* coords = coordsPtr->getPointer(0);
  float x, y, z;
  float xW, yW, zW;

  size_t numPoints = coordsPtr->getNumberOfTuples();
  for(size_t i = 0; i < numPoints; i++)
  {
    x = coords[3*i+0];
    y = coords[3*i+1];
    z = coords[3*i+2];
    xW = 0.0f;
    yW = 0.0f;
    zW = 0.0f;
    coords[3*i+0] = xW;
    coords[3*i+1] = yW;
    coords[3*i+2] = zW;
  }
}
  
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer WarpRegularGrid::newFilterInstance(bool copyFilterParameters)
{
  WarpRegularGrid::Pointer filter = WarpRegularGrid::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WarpRegularGrid::getCompiledLibraryName()
{ return Sampling::SamplingBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WarpRegularGrid::getGroupName()
{ return DREAM3D::FilterGroups::SamplingFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WarpRegularGrid::getHumanLabel()
{ return "Warp Rectilinear Grid Dataset"; }

