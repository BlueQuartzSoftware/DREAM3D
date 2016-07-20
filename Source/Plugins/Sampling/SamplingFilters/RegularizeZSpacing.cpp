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

#include "RegularizeZSpacing.h"

#include <fstream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "Sampling/SamplingConstants.h"
#include "Sampling/SamplingVersion.h"

// Include the MOC generated file for this class
#include "moc_RegularizeZSpacing.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RegularizeZSpacing::RegularizeZSpacing() :
  AbstractFilter(),
  m_CellAttributeMatrixPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, ""),
  m_InputFile(""),
  m_NewZRes(1.0f)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RegularizeZSpacing::~RegularizeZSpacing()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegularizeZSpacing::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(InputFileFilterParameter::New("Current Z Positions File", "InputFile", getInputFile(), FilterParameter::Parameter, SIMPL_BIND_SETTER(RegularizeZSpacing, this, InputFile), SIMPL_BIND_GETTER(RegularizeZSpacing, this, InputFile), "*.txt"));
  parameters.push_back(DoubleFilterParameter::New("New Z Resolution", "NewZRes", getNewZRes(), FilterParameter::Parameter, SIMPL_BIND_SETTER(RegularizeZSpacing, this, NewZRes), SIMPL_BIND_GETTER(RegularizeZSpacing, this, NewZRes)));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(SIMPL::AttributeMatrixType::Cell, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Cell Attribute Matrix", "CellAttributeMatrixPath", getCellAttributeMatrixPath(), FilterParameter::RequiredArray, req, SIMPL_BIND_SETTER(RegularizeZSpacing, this, CellAttributeMatrixPath), SIMPL_BIND_GETTER(RegularizeZSpacing, this, CellAttributeMatrixPath)));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegularizeZSpacing::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellAttributeMatrixPath( reader->readDataArrayPath("CellAttributeMatrixPath", getCellAttributeMatrixPath() ) );
  setInputFile( reader->readString( "InputFile", getInputFile() ) );
  setNewZRes( reader->readValue("NewZRes", getNewZRes()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegularizeZSpacing::initialize()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegularizeZSpacing::dataCheck()
{
  setErrorCondition(0);

  if (getNewZRes() <= 0)
  {
    QString ss = QObject::tr("The new Z resolution Y (%1) must be positive").arg(getNewZRes());
    setErrorCondition(-5555);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  std::ifstream inFile;
  inFile.open(m_InputFile.toLatin1().data());

  if (!inFile.good())
  {
    QString ss = QObject::tr("Unable to open input file with name '%1'").arg(getInputFile());
    setErrorCondition(-5556);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  ImageGeom::Pointer image = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getCellAttributeMatrixPath().getDataContainerName());
  AttributeMatrix::Pointer cellAttrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getCellAttributeMatrixPath(), -301);
  if(getErrorCondition() < 0) { return; }

  float zval = 0.0f;
  for (size_t iter = 0; iter < image->getZPoints() + 1; iter++)
  {
    inFile >> zval;
  }
  size_t zP = static_cast<size_t>(zval / getNewZRes());
  if(zP == 0) { zP = 1; }

  if (getInPreflight())
  {
    image->setDimensions(image->getXPoints(), image->getYPoints(), zP);
    QVector<size_t> tDims(3, 0);
    tDims[0] = image->getXPoints();
    tDims[1] = image->getYPoints();
    tDims[2] = zP;
    cellAttrMat->resizeAttributeArrays(tDims);
  }

  inFile.close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegularizeZSpacing::preflight()
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
void RegularizeZSpacing::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getCellAttributeMatrixPath().getDataContainerName());

  size_t dims[3];
  m->getGeometryAs<ImageGeom>()->getDimensions(dims);

  std::ifstream inFile;
  inFile.open(m_InputFile.toLatin1().data());

  float zval = 0.0f;
  std::vector<float> zboundvalues(dims[2] + 1, 0.0);
  for (size_t iter = 0; iter < dims[2] + 1; iter++)
  {
    inFile >> zval;
    zboundvalues[iter] = zval;
  }
  inFile.close();

  float xRes = m->getGeometryAs<ImageGeom>()->getXRes();
  float yRes = m->getGeometryAs<ImageGeom>()->getYRes();

  float sizez = zboundvalues[dims[2]];
  size_t m_XP = dims[0];
  size_t m_YP = dims[1];
  size_t m_ZP = static_cast<size_t>(sizez / m_NewZRes);
  if (m_ZP == 0) { m_ZP = 1; }
  size_t totalPoints = m_XP * m_YP * m_ZP;

  size_t index = 0, oldindex = 0;
  size_t plane = 0;
  std::vector<size_t> newindicies(totalPoints, 0);
  for (size_t i = 0; i < m_ZP; i++)
  {
    plane = 0;
    for (size_t iter = 1; iter < dims[2]; iter++)
    {
      if ((i * m_NewZRes) > zboundvalues[iter]) { plane = iter; }
    }
    for (size_t j = 0; j < m_YP; j++)
    {
      for (size_t k = 0; k < m_XP; k++)
      {
        oldindex = (plane * dims[0] * dims[1]) + (j * dims[0]) + k;
        index = (i * dims[0] * dims[1]) + (j * dims[0]) + k;
        newindicies[index] = oldindex;
      }
    }
  }

  AttributeMatrix::Pointer cellAttrMat = m->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());
  QVector<size_t> tDims(3, 0);
  tDims[0] = m_XP;
  tDims[1] = m_YP;
  tDims[2] = m_ZP;
  AttributeMatrix::Pointer newCellAttrMat = AttributeMatrix::New(tDims, cellAttrMat->getName(), cellAttrMat->getType());

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

      source = p->getVoidPointer((nComp * newIndicies_I));
      destination = data->getVoidPointer((data->getNumberOfComponents() * i));
      ::memcpy(destination, source, p->getTypeSize() * data->getNumberOfComponents());
    }
    cellAttrMat->removeAttributeArray(*iter);
    newCellAttrMat->addAttributeArray(*iter, data);
  }
  m->getGeometryAs<ImageGeom>()->setResolution(xRes, yRes, m_NewZRes);
  m->getGeometryAs<ImageGeom>()->setDimensions(m_XP, m_YP, m_ZP);
  m->removeAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());
  m->addAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName(), newCellAttrMat);

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RegularizeZSpacing::newFilterInstance(bool copyFilterParameters)
{
  RegularizeZSpacing::Pointer filter = RegularizeZSpacing::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RegularizeZSpacing::getCompiledLibraryName()
{
  return SamplingConstants::SamplingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RegularizeZSpacing::getBrandingString()
{
  return "Sampling";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RegularizeZSpacing::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  Sampling::Version::Major() << "." << Sampling::Version::Minor() << "." << Sampling::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RegularizeZSpacing::getGroupName()
{ return SIMPL::FilterGroups::SamplingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RegularizeZSpacing::getSubGroupName()
{ return SIMPL::FilterSubGroups::ResolutionFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RegularizeZSpacing::getHumanLabel()
{ return "Regularize Z Spacing"; }
