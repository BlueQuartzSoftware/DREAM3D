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


#include "AppendZSlice.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"

// Include the MOC generated file for this class
#include "moc_AppendZSlice.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AppendZSlice::AppendZSlice() :
  AbstractFilter(),
  m_InputAttributeMatrix(),
  m_DestinationAttributeMatrix()
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AppendZSlice::~AppendZSlice()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AppendZSlice::setupFilterParameters()
{
  FilterParameterVector parameters;

  {
    AttributeMatrixSelectionFilterParameter::RequirementType req;
    parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Input Cell Data", "InputAttributeMatrix", getInputAttributeMatrix(), FilterParameter::RequiredArray, req));
  }
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req;
    parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Destination Cell Data", "DestinationAttributeMatrix", getDestinationAttributeMatrix(), FilterParameter::RequiredArray, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AppendZSlice::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setInputAttributeMatrix(reader->readDataArrayPath("InputAttributeMatrix", getInputAttributeMatrix()));
  setDestinationAttributeMatrix(reader->readDataArrayPath("DestinationAttributeMatrix", getDestinationAttributeMatrix()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AppendZSlice::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(InputAttributeMatrix)
  SIMPL_FILTER_WRITE_PARAMETER(DestinationAttributeMatrix)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AppendZSlice::dataCheck()
{
  setErrorCondition(0);

  if (m_InputAttributeMatrix.isEmpty() == true)
  {
    setErrorCondition(-11009);
    QString ss = QObject::tr("The Input Attribute Matrix name must be set");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  if (m_DestinationAttributeMatrix.isEmpty() == true)
  {
    setErrorCondition(-11009);
    QString ss = QObject::tr("The Destination Attribute Matrix name must be set");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }



}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AppendZSlice::preflight()
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
void AppendZSlice::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  QString attrMatName;

  // new slice to be appended
  DataContainer::Pointer mNew = getDataContainerArray()->getDataContainer(m_InputAttributeMatrix.getDataContainerName());
  attrMatName = m_InputAttributeMatrix.getAttributeMatrixName();
  QList<QString> arrayNamesNew = mNew->getAttributeMatrix(attrMatName)->getAttributeArrayNames();
// iterate through
  IDataArray::Pointer pNew = mNew->getAttributeMatrix(attrMatName)->getAttributeArray(arrayNamesNew.at(0));
  if (NULL == pNew ) { return; }
  Int32ArrayType::Pointer newPtr = boost::dynamic_pointer_cast<Int32ArrayType>(pNew);
  if (NULL ==  newPtr) { return; }
  ImageGeom::Pointer imageNew = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getInputAttributeMatrix().getDataContainerName());
  if (NULL == imageNew.get()) { return; }
 


  // existing volume of slices
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_DestinationAttributeMatrix.getDataContainerName());
  attrMatName = m_DestinationAttributeMatrix.getAttributeMatrixName();
  QList<QString> arrayNames = m->getAttributeMatrix(attrMatName)->getAttributeArrayNames();
// iterate
  IDataArray::Pointer pExisting = m->getAttributeMatrix(attrMatName)->getAttributeArray(arrayNames.at(0));
  if (NULL ==  pExisting) { return; }
  Int32ArrayType::Pointer existingPtr = boost::dynamic_pointer_cast<Int32ArrayType>(pExisting);
  if (NULL == existingPtr) { return; }
  ImageGeom::Pointer imageExisting = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getDestinationAttributeMatrix().getDataContainerName());
  if (NULL == imageExisting.get()) { return; }


// match
  QString newType = pNew->getTypeAsString();
  int newTypeSize = pNew->getTypeSize();
  int newNumComponenets = pNew->getNumberOfComponents();
  unsigned int newGeometryType = imageNew->getGeometryType();

  QString existingType = pExisting->getTypeAsString();
  int existingTypeSize = pExisting->getTypeSize();
  int existingNumComponents = pExisting->getNumberOfComponents();
  unsigned int existingGeometryType = imageExisting->getGeometryType();


  // append new to existing
  int sizeExisting = pExisting->getSize();
  int sizeNew = pNew->getSize();
  int sizeTotal = sizeExisting + sizeNew;
  pExisting->resize(sizeExisting + sizeNew);
  for (int i = sizeExisting; i < sizeTotal; i++)
  {
    existingPtr->setValue(i, newPtr->getValue(i - sizeExisting));
  }


  notifyStatusMessage(getHumanLabel(), "Complete");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer AppendZSlice::newFilterInstance(bool copyFilterParameters)
{
  AppendZSlice::Pointer filter = AppendZSlice::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AppendZSlice::getCompiledLibraryName()
{ return Core::CoreBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AppendZSlice::getGroupName()
{ return DREAM3D::FilterGroups::CoreFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AppendZSlice::getSubGroupName()
{ return DREAM3D::FilterSubGroups::MemoryManagementFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AppendZSlice::getHumanLabel()
{ return "Append Z Slice (Image Geometry)"; }
