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


#include "WarpRegularGrid.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "SIMPLib/FilterParameters/SecondOrderPolynomialFilterParameter.h"
#include "SIMPLib/FilterParameters/ThirdOrderPolynomialFilterParameter.h"
#include "SIMPLib/FilterParameters/FourthOrderPolynomialFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"


#include "Sampling/SamplingConstants.h"

// Include the MOC generated file for this class
#include "moc_WarpRegularGrid.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WarpRegularGrid::WarpRegularGrid() :
  AbstractFilter(),
  m_NewDataContainerName(DREAM3D::Defaults::NewImageDataContainerName),
  m_CellAttributeMatrixPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, ""),
  m_PolyOrder(0),
  m_SaveAsNewDataContainer(false)
{
  m_SecondOrderACoeff.c20 = 0.0f, m_SecondOrderACoeff.c02 = 0.0f, m_SecondOrderACoeff.c11 = 0.0f;
  m_SecondOrderACoeff.c10 = 1.0f, m_SecondOrderACoeff.c01 = 1.0f, m_SecondOrderACoeff.c00 = 0.0f;
  m_SecondOrderBCoeff.c20 = 0.0f, m_SecondOrderBCoeff.c02 = 0.0f, m_SecondOrderBCoeff.c11 = 0.0f;
  m_SecondOrderBCoeff.c10 = 1.0f, m_SecondOrderBCoeff.c01 = 1.0f, m_SecondOrderBCoeff.c00 = 0.0f;

  m_ThirdOrderACoeff.c30 = 0.0f, m_ThirdOrderACoeff.c03 = 0.0f, m_ThirdOrderACoeff.c21 = 0.0f, m_ThirdOrderACoeff.c12 = 0.0f, m_ThirdOrderACoeff.c20 = 0.0f;
  m_ThirdOrderACoeff.c02 = 0.0f, m_ThirdOrderACoeff.c11 = 0.0f, m_ThirdOrderACoeff.c10 = 1.0f;
  m_ThirdOrderACoeff.c01 = 1.0f, m_ThirdOrderACoeff.c00 = 0.0f;
  m_ThirdOrderBCoeff.c30 = 0.0f, m_ThirdOrderBCoeff.c03 = 0.0f, m_ThirdOrderBCoeff.c21 = 0.0f, m_ThirdOrderBCoeff.c12 = 0.0f, m_ThirdOrderBCoeff.c20 = 0.0f;
  m_ThirdOrderBCoeff.c02 = 0.0f, m_ThirdOrderBCoeff.c11 = 0.0f, m_ThirdOrderBCoeff.c10 = 1.0f;
  m_ThirdOrderBCoeff.c01 = 1.0f, m_ThirdOrderBCoeff.c00 = 0.0f;

  m_FourthOrderACoeff.c40 = 0.0f, m_FourthOrderACoeff.c04 = 0.0f, m_FourthOrderACoeff.c31 = 0.0f;
  m_FourthOrderACoeff.c13 = 0.0f, m_FourthOrderACoeff.c22 = 0.0f;
  m_FourthOrderACoeff.c30 = 0.0f, m_FourthOrderACoeff.c03 = 0.0f, m_FourthOrderACoeff.c21 = 0.0f, m_FourthOrderACoeff.c12 = 0.0f, m_FourthOrderACoeff.c20 = 0.0f;
  m_FourthOrderACoeff.c02 = 0.0f, m_FourthOrderACoeff.c11 = 0.0f, m_FourthOrderACoeff.c10 = 1.0f;
  m_FourthOrderACoeff.c01 = 1.0f, m_FourthOrderACoeff.c00 = 0.0f;
  m_FourthOrderBCoeff.c40 = 0.0f, m_FourthOrderBCoeff.c04 = 0.0f, m_FourthOrderBCoeff.c31 = 0.0f;
  m_FourthOrderBCoeff.c13 = 0.0f, m_FourthOrderBCoeff.c22 = 0.0f;
  m_FourthOrderBCoeff.c30 = 0.0f, m_FourthOrderBCoeff.c03 = 0.0f, m_FourthOrderBCoeff.c21 = 0.0f, m_FourthOrderBCoeff.c12 = 0.0f, m_FourthOrderBCoeff.c20 = 0.0f;
  m_FourthOrderBCoeff.c02 = 0.0f, m_FourthOrderBCoeff.c11 = 0.0f, m_FourthOrderBCoeff.c10 = 1.0f;
  m_FourthOrderBCoeff.c01 = 1.0f, m_FourthOrderBCoeff.c00 = 0.0f;

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

  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Polynomial Order");
    parameter->setPropertyName("PolyOrder");

    QVector<QString> choices;
    choices.push_back("2nd");
    choices.push_back("3rd");
    choices.push_back("4th");
    parameter->setChoices(choices);
    QStringList linkedProps;
    linkedProps << "SecondOrderACoeff" << "ThirdOrderACoeff" << "FourthOrderACoeff";
    linkedProps << "SecondOrderBCoeff" << "ThirdOrderBCoeff" << "FourthOrderBCoeff";
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  parameters.push_back(SecondOrderPolynomialFilterParameter::New("Second Order A Coefficients", "SecondOrderACoeff", getSecondOrderACoeff(), FilterParameter::Parameter, 0));
  parameters.push_back(SecondOrderPolynomialFilterParameter::New("Second Order B Coefficients", "SecondOrderBCoeff", getSecondOrderBCoeff(), FilterParameter::Parameter, 0));
  parameters.push_back(ThirdOrderPolynomialFilterParameter::New("Third Order A Coefficients", "ThirdOrderACoeff", getThirdOrderACoeff(), FilterParameter::Parameter, 1));
  parameters.push_back(ThirdOrderPolynomialFilterParameter::New("Third Order B Coefficients", "ThirdOrderBCoeff", getThirdOrderBCoeff(), FilterParameter::Parameter, 1));
  parameters.push_back(FourthOrderPolynomialFilterParameter::New("Fourth Order A Coefficients", "FourthOrderACoeff", getFourthOrderACoeff(), FilterParameter::Parameter, 2));
  parameters.push_back(FourthOrderPolynomialFilterParameter::New("Fourth Order B Coefficients", "FourthOrderBCoeff", getFourthOrderBCoeff(), FilterParameter::Parameter, 2));
  QStringList linkedProps;
  linkedProps << "NewDataContainerName";
  parameters.push_back(LinkedBooleanFilterParameter::New("Save as New Data Container", "SaveAsNewDataContainer", getSaveAsNewDataContainer(), linkedProps, FilterParameter::Parameter));
  parameters.push_back(StringFilterParameter::New("Data Container", "NewDataContainerName", getNewDataContainerName(), FilterParameter::CreatedArray));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(DREAM3D::AttributeMatrixType::Cell, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Cell Attribute Matrix", "CellAttributeMatrixPath", getCellAttributeMatrixPath(), FilterParameter::RequiredArray, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WarpRegularGrid::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNewDataContainerName( reader->readString("NewDataContainerName", getNewDataContainerName() ) );
  setCellAttributeMatrixPath( reader->readDataArrayPath("CellAttributeMatrixPath", getCellAttributeMatrixPath() ) );
  setSecondOrderACoeff( reader->readFloat2ndOrderPoly("SecondOrderACoeff", getSecondOrderACoeff() ) );
  setSecondOrderBCoeff( reader->readFloat2ndOrderPoly("SecondOrderBCoeff", getSecondOrderBCoeff() ) );
  setThirdOrderACoeff( reader->readFloat3rdOrderPoly("ThirdOrderACoeff", getThirdOrderACoeff() ) );
  setThirdOrderBCoeff( reader->readFloat3rdOrderPoly("ThirdOrderBCoeff", getThirdOrderBCoeff() ) );
  setFourthOrderACoeff( reader->readFloat4thOrderPoly("FourthOrderACoeff", getFourthOrderACoeff() ) );
  setFourthOrderBCoeff( reader->readFloat4thOrderPoly("FourthOrderBCoeff", getFourthOrderBCoeff() ) );
  setSaveAsNewDataContainer( reader->readValue("SaveAsNewDataContainer", getSaveAsNewDataContainer()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int WarpRegularGrid::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(NewDataContainerName)
  SIMPL_FILTER_WRITE_PARAMETER(CellAttributeMatrixPath)
  SIMPL_FILTER_WRITE_PARAMETER(SecondOrderACoeff)
  SIMPL_FILTER_WRITE_PARAMETER(SecondOrderBCoeff)
  SIMPL_FILTER_WRITE_PARAMETER(ThirdOrderACoeff)
  SIMPL_FILTER_WRITE_PARAMETER(ThirdOrderBCoeff)
  SIMPL_FILTER_WRITE_PARAMETER(FourthOrderACoeff)
  SIMPL_FILTER_WRITE_PARAMETER(FourthOrderBCoeff)
  SIMPL_FILTER_WRITE_PARAMETER(SaveAsNewDataContainer)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WarpRegularGrid::dataCheck()
{
  getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getCellAttributeMatrixPath(), -301);

  if(m_SaveAsNewDataContainer == false)
  {
    getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getCellAttributeMatrixPath().getDataContainerName());
  }
  else
  {
    getDataContainerArray()->duplicateDataContainer(getCellAttributeMatrixPath().getDataContainerName(), getNewDataContainerName());
  }
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
void WarpRegularGrid::determine_warped_coordinates(float x, float y, float& newX, float& newY)
{
  if(m_PolyOrder == 0)
  {
    newX = x * x * m_SecondOrderACoeff.c20 + y * y * m_SecondOrderACoeff.c02 + x * y * m_SecondOrderACoeff.c11 + x * m_SecondOrderACoeff.c10 + y * m_SecondOrderACoeff.c01 + m_SecondOrderACoeff.c00;
    newY = x * x * m_SecondOrderBCoeff.c20 + y * y * m_SecondOrderBCoeff.c02 + x * y * m_SecondOrderBCoeff.c11 + x * m_SecondOrderBCoeff.c10 + y * m_SecondOrderBCoeff.c01 + m_SecondOrderBCoeff.c00;
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
void WarpRegularGrid::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m;
  if (m_SaveAsNewDataContainer == false) { m = getDataContainerArray()->getDataContainer(getCellAttributeMatrixPath().getDataContainerName()); }
  else { m = getDataContainerArray()->getDataContainer(getNewDataContainerName()); }

  AttributeMatrix::Pointer cellAttrMat = m->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());
  AttributeMatrix::Pointer newCellAttrMat = cellAttrMat->deepCopy();

  size_t dims[3] = { 0, 0, 0 };
  m->getGeometryAs<ImageGeom>()->getDimensions(dims);
  float res[3] = { 0.0f, 0.0f, 0.0f };
  m->getGeometryAs<ImageGeom>()->getResolution(res);
  size_t totalPoints = m->getGeometryAs<ImageGeom>()->getNumberOfElements();

  float x = 0.0f, y = 0.0f, z = 0.0f;
  float newX = 0.0f, newY = 0.0f;
  int col = 0.0f, row = 0.0f, plane = 0.0f;
  size_t index;
  size_t index_old;
  std::vector<size_t> newindicies(totalPoints);
  std::vector<bool> goodPoint(totalPoints, true);

  for (size_t i = 0; i < dims[2]; i++)
  {
    QString ss = QObject::tr("Warping Data - %1 Percent Complete").arg(((float)i / dims[2]) * 100);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    for (size_t j = 0; j < dims[1]; j++)
    {
      for (size_t k = 0; k < dims[0]; k++)
      {
        x = static_cast<float>((k * res[0]));
        y = static_cast<float>((j * res[1]));
        z = static_cast<float>((i * res[2]));
        index = (i * dims[0] * dims[1]) + (j * dims[0]) + k;

        determine_warped_coordinates(x, y, newX, newY);
        col = newX / res[0];
        row = newY / res[1];
        plane = i;

        index_old = (plane * dims[0] * dims[1]) + (row * dims[0]) + col;
        newindicies[index] = index_old;
        if (col > 0 && col < dims[0] && row > 0 && row < dims[1]) { goodPoint[index] = true; }
        else { goodPoint[index] = false; }
      }
    }
  }

  QList<QString> voxelArrayNames = cellAttrMat->getAttributeArrayNames();
  for (QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
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
{ return SamplingConstants::SamplingBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WarpRegularGrid::getGroupName()
{ return DREAM3D::FilterGroups::SamplingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WarpRegularGrid::getSubGroupName()
{ return DREAM3D::FilterSubGroups::WarpingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WarpRegularGrid::getHumanLabel()
{ return "Warp Rectilinear Grid"; }

