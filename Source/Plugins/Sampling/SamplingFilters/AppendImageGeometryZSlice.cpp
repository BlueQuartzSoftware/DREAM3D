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
#include "AppendImageGeometryZSlice.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "Sampling/SamplingConstants.h"
#include "Sampling/SamplingVersion.h"

// Include the MOC generated file for this class
#include "moc_AppendImageGeometryZSlice.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AppendImageGeometryZSlice::AppendImageGeometryZSlice() :
  AbstractFilter(),
  m_InputAttributeMatrix(),
  m_DestinationAttributeMatrix(),
  m_CheckResolution(false)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AppendImageGeometryZSlice::~AppendImageGeometryZSlice()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AppendImageGeometryZSlice::setupFilterParameters()
{
  FilterParameterVector parameters;

  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(SIMPL::AttributeMatrixType::Cell, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Input Cell Data", "InputAttributeMatrix", getInputAttributeMatrix(), FilterParameter::RequiredArray, req));
  }
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(SIMPL::AttributeMatrixType::Cell, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Destination Cell Data", "DestinationAttributeMatrix", getDestinationAttributeMatrix(), FilterParameter::RequiredArray, req));
  }

  parameters.push_back(BooleanFilterParameter::New("Check Resolution", "CheckResolution", getCheckResolution(), FilterParameter::Parameter));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AppendImageGeometryZSlice::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setInputAttributeMatrix(reader->readDataArrayPath("InputAttributeMatrix", getInputAttributeMatrix()));
  setDestinationAttributeMatrix(reader->readDataArrayPath("DestinationAttributeMatrix", getDestinationAttributeMatrix()));
  setCheckResolution(reader->readValue("CheckResolution", getCheckResolution()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AppendImageGeometryZSlice::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(InputAttributeMatrix)
  SIMPL_FILTER_WRITE_PARAMETER(DestinationAttributeMatrix)
  SIMPL_FILTER_WRITE_PARAMETER(CheckResolution)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AppendImageGeometryZSlice::initialize()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AppendImageGeometryZSlice::dataCheck()
{
  setErrorCondition(0);

  // Validate the Source & Destination Attribute Matrix are available
  AttributeMatrix::Pointer inputCellAttrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getInputAttributeMatrix(), -8201);
  if(getErrorCondition() < 0) { return; }

  AttributeMatrix::Pointer destCellAttrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getDestinationAttributeMatrix(), -8200);
  if(getErrorCondition() < 0) { return; }

  // Validate each AttributeMatrix is associated with an Image Geometry.
  ImageGeom::Pointer inputGeometry = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getInputAttributeMatrix().getDataContainerName());
  if (NULL == inputGeometry.get() || getErrorCondition() < 0) { return; }

  ImageGeom::Pointer destGeometry = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getDestinationAttributeMatrix().getDataContainerName());
  if (NULL == destGeometry.get() || getErrorCondition() < 0) { return; }


  // Get the Dimensions of the ImageGeometries

  size_t inputGeomDims[3] = { 0,0,0};
  inputGeometry->getDimensions(inputGeomDims);

  QVector<size_t> destGeomDims(3, 0);
  destGeometry->getDimensions(destGeomDims.data());

  if(getCheckResolution())
  {
    float inputRes[3] = { 0.0f, 0.0f, 0.0f};
    inputGeometry->getResolution(inputRes);

    float destRes[3] = { 0.0f, 0.0f, 0.0f};
    destGeometry->getResolution(destRes);

    if(inputRes[0] != destRes[0]) {
      QString ss = QObject::tr("Input X Resolution (%1) not equal to Destination X Resolution (%2)").arg(inputRes[0]).arg(destRes[0]);
      setErrorCondition(-8205);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
    if(inputRes[1] != destRes[1]) {
      QString ss = QObject::tr("Input Y Resolution (%1) not equal to Destination Y Resolution (%2)").arg(inputRes[1]).arg(destRes[1]);
      setErrorCondition(-8206);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
    if(inputRes[2] != destRes[2]) {
      QString ss = QObject::tr("Input Z Resolution (%1) not equal to Destination Z Resolution (%2)").arg(inputRes[2]).arg(destRes[2]);
      setErrorCondition(-8207);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
  }

  if (destGeomDims[0] != inputGeomDims[0])
  {
    QString ss = QObject::tr("Input X Dim (%1) not equal to Destination X Dim (%2)").arg(inputGeomDims[0]).arg(destGeomDims[0]);
    setErrorCondition(-8202);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if (destGeomDims[0] != inputGeomDims[0])
  {
    QString ss = QObject::tr("Input Y Dim (%1) not equal to Destination Y Dim (%2)").arg(inputGeomDims[1]).arg(destGeomDims[1]);
    setErrorCondition(-8203);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if (getInPreflight())
  {
    // We are only appending in the Z direction
    destGeomDims[2] = destGeomDims[2] + inputGeomDims[2];

    // Update the existing z dimension of the image geometry and set that value back into the Image Geometry
    destGeometry->setDimensions(destGeomDims.data());


    size_t totalPoints = 1;
    for(int i = 0; i < 3; i++) {
      if(destGeomDims[i] != 0) { totalPoints *= destGeomDims[i]; }
    }
    AttributeMatrix::Pointer newCellAttrMat = AttributeMatrix::New(destGeomDims, destCellAttrMat->getName(), destCellAttrMat->getType());
    // Create new DataArrays with the new sizes and place into a temp AttributeMatrix which
    // will be inserted into the data container as a replacement for the existing
    // AttributeMatrix
    QList<QString> voxelArrayNames = destCellAttrMat->getAttributeArrayNames();
    for (QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
    {
      IDataArray::Pointer p = destCellAttrMat->getAttributeArray(*iter);
      //
      IDataArray::Pointer data = p->createNewArray(totalPoints, p->getComponentDimensions(), p->getName(), false);

      destCellAttrMat->removeAttributeArray(*iter);
      newCellAttrMat->addAttributeArray(*iter, data);
    }
    getDataContainerArray()->getDataContainer(getDestinationAttributeMatrix().getDataContainerName())->removeAttributeMatrix(destCellAttrMat->getName());
    getDataContainerArray()->getDataContainer(getDestinationAttributeMatrix().getDataContainerName())->addAttributeMatrix(newCellAttrMat->getName(), newCellAttrMat);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AppendImageGeometryZSlice::preflight()
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
void AppendImageGeometryZSlice::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }


  // Validate each AttributeMatrix is associated with an Image Geometry.
  ImageGeom::Pointer inputGeometry = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getInputAttributeMatrix().getDataContainerName());

  ImageGeom::Pointer destGeometry = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getDestinationAttributeMatrix().getDataContainerName());

  AttributeMatrix::Pointer inputCellAttrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getInputAttributeMatrix(), -8201);

  AttributeMatrix::Pointer destCellAttrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getDestinationAttributeMatrix(), -8200);

  size_t inputGeomDims[3] = { 0,0,0};
  inputGeometry->getDimensions(inputGeomDims);

  QVector<size_t> destGeomDims(3, 0);
  destGeometry->getDimensions(destGeomDims.data());

  size_t tupleOffset = 1;
  for(int i = 0; i < 3; i++) {
    if(destGeomDims[i] != 0) { tupleOffset *= destGeomDims[i]; }
  }

  // We are only appending in the Z direction
  destGeomDims[2] = destGeomDims[2] + inputGeomDims[2];

  // Now update the geometry with new dimension
  // Update the existing z dimension of the image geometry and set that value back into the Image Geometry
  destGeometry->setDimensions(destGeomDims.data());

  destCellAttrMat->resizeAttributeArrays(destGeomDims);

  QList<QString> voxelArrayNames = destCellAttrMat->getAttributeArrayNames();
  for (QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
    IDataArray::Pointer p = destCellAttrMat->getAttributeArray(*iter);
    IDataArray::Pointer inputArray = inputCellAttrMat->getAttributeArray(*iter);
    if(NULL != inputArray.get())
    {
      p->copyData(tupleOffset, inputArray);
    }
    else
    {
      QString ss = QObject::tr("Data Array '%1' does not exist in the Input Cell AttributeMatrix.").arg(*iter);
      setErrorCondition(0);
      notifyWarningMessage(getHumanLabel(), ss, getErrorCondition());
    }
  }



  notifyStatusMessage(getHumanLabel(), "Complete");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer AppendImageGeometryZSlice::newFilterInstance(bool copyFilterParameters)
{
  AppendImageGeometryZSlice::Pointer filter = AppendImageGeometryZSlice::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AppendImageGeometryZSlice::getCompiledLibraryName()
{
  return SamplingConstants::SamplingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AppendImageGeometryZSlice::getBrandingString()
{
  return "Sampling";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AppendImageGeometryZSlice::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  Sampling::Version::Major() << "." << Sampling::Version::Minor() << "." << Sampling::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AppendImageGeometryZSlice::getGroupName()
{ return SIMPL::FilterGroups::CoreFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AppendImageGeometryZSlice::getSubGroupName()
{ return SIMPL::FilterSubGroups::MemoryManagementFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AppendImageGeometryZSlice::getHumanLabel()
{ return "Append Z Slice (Image Geometry)"; }
