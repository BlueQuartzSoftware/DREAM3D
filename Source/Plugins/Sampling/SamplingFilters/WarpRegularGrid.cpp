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
  m_PolyOrder(0),
  m_SaveAsNewDataContainer(false)
{
  m_SecondOrderACoeff.v11 = 1.0f, m_SecondOrderACoeff.v12 = 0.0f, m_SecondOrderACoeff.v13 = 0.0f;
  m_SecondOrderACoeff.v21 = 0.0f, m_SecondOrderACoeff.v22 = 1.0f, m_SecondOrderACoeff.v23 = 0.0f;
  m_SecondOrderACoeff.v31 = 0.0f, m_SecondOrderACoeff.v32 = 0.0f, m_SecondOrderACoeff.v33 = 1.0f;
  m_SecondOrderBCoeff.v11 = 1.0f, m_SecondOrderBCoeff.v12 = 0.0f, m_SecondOrderBCoeff.v13 = 0.0f;
  m_SecondOrderBCoeff.v21 = 0.0f, m_SecondOrderBCoeff.v22 = 1.0f, m_SecondOrderBCoeff.v23 = 0.0f;
  m_SecondOrderBCoeff.v31 = 0.0f, m_SecondOrderBCoeff.v32 = 0.0f, m_SecondOrderBCoeff.v33 = 1.0f;

  m_ThirdOrderACoeff.v11 = 1.0f, m_ThirdOrderACoeff.v12 = 0.0f, m_ThirdOrderACoeff.v13 = 0.0f, m_ThirdOrderACoeff.v14 = 0.0f;
  m_ThirdOrderACoeff.v21 = 0.0f, m_ThirdOrderACoeff.v22 = 1.0f, m_ThirdOrderACoeff.v23 = 0.0f, m_ThirdOrderACoeff.v24 = 0.0f;
  m_ThirdOrderACoeff.v31 = 0.0f, m_ThirdOrderACoeff.v32 = 0.0f, m_ThirdOrderACoeff.v33 = 1.0f, m_ThirdOrderACoeff.v34 = 0.0f;
  m_ThirdOrderACoeff.v41 = 0.0f, m_ThirdOrderACoeff.v42 = 0.0f, m_ThirdOrderACoeff.v43 = 0.0f, m_ThirdOrderACoeff.v44 = 1.0f;
  m_ThirdOrderBCoeff.v11 = 1.0f, m_ThirdOrderBCoeff.v12 = 0.0f, m_ThirdOrderBCoeff.v13 = 0.0f, m_ThirdOrderBCoeff.v14 = 0.0f;
  m_ThirdOrderBCoeff.v21 = 0.0f, m_ThirdOrderBCoeff.v22 = 1.0f, m_ThirdOrderBCoeff.v23 = 0.0f, m_ThirdOrderBCoeff.v24 = 0.0f;
  m_ThirdOrderBCoeff.v31 = 0.0f, m_ThirdOrderBCoeff.v32 = 0.0f, m_ThirdOrderBCoeff.v33 = 1.0f, m_ThirdOrderBCoeff.v34 = 0.0f;
  m_ThirdOrderBCoeff.v41 = 0.0f, m_ThirdOrderBCoeff.v42 = 0.0f, m_ThirdOrderBCoeff.v43 = 0.0f, m_ThirdOrderBCoeff.v44 = 1.0f;

  m_FourthOrderACoeff.v11 = 1.0f, m_FourthOrderACoeff.v12 = 0.0f, m_FourthOrderACoeff.v13 = 0.0f, m_FourthOrderACoeff.v14 = 0.0f, m_FourthOrderACoeff.v15 = 0.0f;
  m_FourthOrderACoeff.v21 = 0.0f, m_FourthOrderACoeff.v22 = 1.0f, m_FourthOrderACoeff.v23 = 0.0f, m_FourthOrderACoeff.v24 = 0.0f, m_FourthOrderACoeff.v25 = 0.0f;
  m_FourthOrderACoeff.v31 = 0.0f, m_FourthOrderACoeff.v32 = 0.0f, m_FourthOrderACoeff.v33 = 1.0f, m_FourthOrderACoeff.v34 = 0.0f, m_FourthOrderACoeff.v35 = 0.0f;
  m_FourthOrderACoeff.v41 = 0.0f, m_FourthOrderACoeff.v42 = 0.0f, m_FourthOrderACoeff.v43 = 0.0f, m_FourthOrderACoeff.v44 = 1.0f, m_FourthOrderACoeff.v45 = 0.0f;
  m_FourthOrderACoeff.v51 = 0.0f, m_FourthOrderACoeff.v52 = 0.0f, m_FourthOrderACoeff.v53 = 0.0f, m_FourthOrderACoeff.v54 = 0.0f, m_FourthOrderACoeff.v55 = 1.0f;
  m_FourthOrderBCoeff.v11 = 1.0f, m_FourthOrderBCoeff.v12 = 0.0f, m_FourthOrderBCoeff.v13 = 0.0f, m_FourthOrderBCoeff.v14 = 0.0f, m_FourthOrderBCoeff.v15 = 0.0f;
  m_FourthOrderBCoeff.v21 = 0.0f, m_FourthOrderBCoeff.v22 = 1.0f, m_FourthOrderBCoeff.v23 = 0.0f, m_FourthOrderBCoeff.v24 = 0.0f, m_FourthOrderBCoeff.v25 = 0.0f;
  m_FourthOrderBCoeff.v31 = 0.0f, m_FourthOrderBCoeff.v32 = 0.0f, m_FourthOrderBCoeff.v33 = 1.0f, m_FourthOrderBCoeff.v34 = 0.0f, m_FourthOrderBCoeff.v35 = 0.0f;
  m_FourthOrderBCoeff.v41 = 0.0f, m_FourthOrderBCoeff.v42 = 0.0f, m_FourthOrderBCoeff.v43 = 0.0f, m_FourthOrderBCoeff.v44 = 1.0f, m_FourthOrderBCoeff.v45 = 0.0f;
  m_FourthOrderBCoeff.v51 = 0.0f, m_FourthOrderBCoeff.v52 = 0.0f, m_FourthOrderBCoeff.v53 = 0.0f, m_FourthOrderBCoeff.v54 = 0.0f, m_FourthOrderBCoeff.v55 = 1.0f;

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
  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Polynomial Order");
    parameter->setPropertyName("PolyOrder");
    parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    QVector<QString> choices;
    choices.push_back("3rd");
    choices.push_back("4th");
    choices.push_back("5th");
    parameter->setChoices(choices);
    QStringList linkedProps;
    linkedProps << "SecondOrderACoeff" << "ThirdOrderACoeff" << "FourthOrderACoeff";
    linkedProps << "SecondOrderBCoeff" << "ThirdOrderBCoeff" << "FourthOrderBCoeff";
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameters.push_back(parameter);
  }
  parameters.push_back(FilterParameter::New("Second Order A Coefficients", "SecondOrderACoeff", FilterParameterWidgetType::Matrix3x3Widget, getSecondOrderACoeff(), false, "", 0));
  parameters.push_back(FilterParameter::New("Second Order B Coefficients", "SecondOrderBCoeff", FilterParameterWidgetType::Matrix3x3Widget, getSecondOrderBCoeff(), false, "", 0));
  parameters.push_back(FilterParameter::New("Third Order A Coefficients", "ThirdOrderACoeff", FilterParameterWidgetType::Matrix4x4Widget, getThirdOrderACoeff(), false, "", 1));
  parameters.push_back(FilterParameter::New("Third Order B Coefficients", "ThirdOrderBCoeff", FilterParameterWidgetType::Matrix4x4Widget, getThirdOrderBCoeff(), false, "", 1));
  parameters.push_back(FilterParameter::New("Fourth Order A Coefficients", "FourthOrderACoeff", FilterParameterWidgetType::Matrix5x5Widget, getFourthOrderACoeff(), false, "", 2));
  parameters.push_back(FilterParameter::New("Fourth Order B Coefficients", "FourthOrderBCoeff", FilterParameterWidgetType::Matrix5x5Widget, getFourthOrderBCoeff(), false, "", 2));
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
  setSecondOrderACoeff( reader->readFloat3x3("SecondOrderACoeff", getSecondOrderACoeff() ) );
  setSecondOrderBCoeff( reader->readFloat3x3("SecondOrderBCoeff", getSecondOrderBCoeff() ) );
  setThirdOrderACoeff( reader->readFloat4x4("ThirdOrderACoeff", getThirdOrderACoeff() ) );
  setThirdOrderBCoeff( reader->readFloat4x4("ThirdOrderBCoeff", getThirdOrderBCoeff() ) );
  setFourthOrderACoeff( reader->readFloat5x5("FourthOrderACoeff", getFourthOrderACoeff() ) );
  setFourthOrderBCoeff( reader->readFloat5x5("FourthOrderBCoeff", getFourthOrderBCoeff() ) );
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
  DREAM3D_FILTER_WRITE_PARAMETER(SecondOrderACoeff)
  DREAM3D_FILTER_WRITE_PARAMETER(SecondOrderBCoeff)
  DREAM3D_FILTER_WRITE_PARAMETER(ThirdOrderACoeff)
  DREAM3D_FILTER_WRITE_PARAMETER(ThirdOrderBCoeff)
  DREAM3D_FILTER_WRITE_PARAMETER(FourthOrderACoeff)
  DREAM3D_FILTER_WRITE_PARAMETER(FourthOrderBCoeff)
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

  AttributeMatrix::Pointer cellAttrMat = m->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());
  AttributeMatrix::Pointer newCellAttrMat = cellAttrMat->deepCopy();

  size_t dims[3];
  m->getDimensions(dims);
  float res[3];
  m->getResolution(res);
  size_t totalPoints = m->getTotalPoints();

  float x, y, z;
  float newX, newY;
  int col, row, plane;
  int index;
  int index_old;
  std::vector<size_t> newindicies(totalPoints);
  std::vector<bool> goodPoint(totalPoints, true);

  for (int i = 0; i < dims[2]; i++)
  {
    QString ss = QObject::tr("Warping Data - %1 Percent Complete").arg(((float)i / dims[2]) * 100);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    for (int j = 0; j < dims[1]; j++)
    {
      for (int k = 0; k < dims[0]; k++)
      {
        x = (k * res[0]);
        y = (j * res[1]);
        z = (i * res[2]);
        index = (i * dims[0] * dims[1]) + (j * dims[0]) + k;

        determine_warped_coordinates(x, y, newX, newY);
        col = newX/res[0];
        row = newY/res[1];
        plane = i;

        index_old = (plane * dims[0] * dims[1]) + (row * dims[0]) + col;
        newindicies[index] = index_old;
        if(col > 0 && col < dims[0] && row > 0 && row < dims[1]) goodPoint[index] = true;
        else goodPoint[index] = false;
      }
    }
  }

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

      if(goodPoint[i] == true)
      {
        source = p->getVoidPointer((nComp * newIndicies_I));
        destination = data->getVoidPointer((data->getNumberOfComponents() * i));
        ::memcpy(destination, source, p->getTypeSize() * data->getNumberOfComponents());
      }
      else
      {
        data->initializeTuple(i, 0);
      }
    }
    cellAttrMat->removeAttributeArray(*iter);
    newCellAttrMat->addAttributeArray(*iter, data);
  }
  m->removeAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());
  m->addAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName(), newCellAttrMat);

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WarpRegularGrid::determine_warped_coordinates(float x, float y, float &newX, float &newY)
{
  if(m_PolyOrder == 0)
  {
    newX = x*x*m_SecondOrderACoeff.v31 + y*y*m_SecondOrderACoeff.v13 + x*y*m_SecondOrderACoeff.v22 + x*m_SecondOrderACoeff.v21 + y*m_SecondOrderACoeff.v12 + m_SecondOrderACoeff.v11;
    newY = x*x*m_SecondOrderBCoeff.v31 + y*y*m_SecondOrderBCoeff.v13 + x*y*m_SecondOrderBCoeff.v22 + x*m_SecondOrderBCoeff.v21 + y*m_SecondOrderBCoeff.v12 + m_SecondOrderBCoeff.v11;
  }
  if(m_PolyOrder == 1)
  {

  }
  if(m_PolyOrder == 2)
  {

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

