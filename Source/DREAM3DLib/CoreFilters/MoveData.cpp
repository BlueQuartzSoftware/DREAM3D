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


#include "MoveData.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "DREAM3DLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/LinkedChoicesFilterParameter.h"

namespace
{
  static const int32_t k_MoveAttributeMatrix = 0;
  static const int32_t k_MoveDataArray = 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MoveData::MoveData() :
  AbstractFilter(),
  m_WhatToMove(k_MoveAttributeMatrix),
  m_DataContainerDestination(""),
  m_AttributeMatrixSource(),
  m_AttributeMatrixDestination(),
  m_DataArraySource()
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MoveData::~MoveData()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MoveData::setupFilterParameters()
{
  FilterParameterVector parameters;

  QStringList linkedProps;
  linkedProps << "DataContainerDestination" << "AttributeMatrixSource" << "AttributeMatrixDestination" << "DataArraySource";
  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Object to Move");
    parameter->setPropertyName("WhatToMove");

    parameter->setDefaultValue(getWhatToMove()); // Just set the first index
    QVector<QString> choices;
    choices.push_back("Attribute Matrix");
    choices.push_back("Attribute Array");
    parameter->setChoices(choices);
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }

  {
    AttributeMatrixSelectionFilterParameter::DataStructureRequirements req;
    parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Attribute Matrix Source", "AttributeMatrixSource", getAttributeMatrixSource(), FilterParameter::RequiredArray, req, 0));
  }
  {
    DataContainerSelectionFilterParameter::DataStructureRequirements req;
    parameters.push_back(DataContainerSelectionFilterParameter::New("Data Container Destination", "DataContainerDestination", getDataContainerDestination(), FilterParameter::RequiredArray, req, 0));
  }
  {
    DataArraySelectionFilterParameter::DataStructureRequirements req;
    parameters.push_back(DataArraySelectionFilterParameter::New("Attribute Array Source", "DataArraySource", getDataArraySource(), FilterParameter::RequiredArray, req, 1));
  }
  {
    AttributeMatrixSelectionFilterParameter::DataStructureRequirements req;
    parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Attribute Matrix Destination", "AttributeMatrixDestination", getAttributeMatrixDestination(), FilterParameter::RequiredArray, req, 1));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MoveData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setWhatToMove( reader->readValue("WhatToMove", getWhatToMove() ) );
  setDataContainerDestination( reader->readString("DataContainerDestination", getDataContainerDestination() ) );
  setAttributeMatrixSource( reader->readDataArrayPath("AttributeMatrixSource", getAttributeMatrixSource() ) );
  setAttributeMatrixDestination( reader->readDataArrayPath("AttributeMatrixDestination", getAttributeMatrixDestination() ) );
  setDataArraySource( reader->readDataArrayPath("DataArraySource", getDataArraySource() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MoveData::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(WhatToMove)
  DREAM3D_FILTER_WRITE_PARAMETER(DataContainerDestination)
  DREAM3D_FILTER_WRITE_PARAMETER(AttributeMatrixSource)
  DREAM3D_FILTER_WRITE_PARAMETER(AttributeMatrixDestination)
  DREAM3D_FILTER_WRITE_PARAMETER(DataArraySource)
  writer->closeFilterGroup(); // we want to return the next index that was just written to
  return ++index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MoveData::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath amSrcPath = getAttributeMatrixSource();
  DataArrayPath amDestPath = getAttributeMatrixDestination();
  DataArrayPath daSrcPath = getDataArraySource();

  if (getWhatToMove() == k_MoveAttributeMatrix)
  {
    DataContainer::Pointer amDestDataContainer = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getDataContainerDestination());
    DataContainer::Pointer amSrcDataContainer = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, amSrcPath.getDataContainerName());
    AttributeMatrix::Pointer amSrcAttributeMatrix = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, amSrcPath, -301);

    if(getErrorCondition() < 0) { return; }

    if (amSrcDataContainer->getName() == amDestDataContainer->getName())
    {
      QString ss = QObject::tr("The source and destination Data Container are the same.  Is this what you meant to do?");
      notifyWarningMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    amDestDataContainer->addAttributeMatrix(amSrcAttributeMatrix->getName(), amSrcAttributeMatrix);
    amSrcDataContainer->removeAttributeMatrix(amSrcAttributeMatrix->getName());
  }
  else if (getWhatToMove() == k_MoveDataArray )
  {
    AttributeMatrix::Pointer daSrcAttributeMatrix = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, daSrcPath, -301);
    AttributeMatrix::Pointer daDestAttributeMatrix = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, amDestPath, -301);
    IDataArray::Pointer daSrcDataArray = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, daSrcPath);

    if(getErrorCondition() < 0) { return; }

    if (daDestAttributeMatrix->getNumTuples() != daSrcDataArray->getNumberOfTuples())
    {
      setErrorCondition(-11019);
      QString ss = QObject::tr("The number of tuples of source Attribute Array (%1) and destination Attribute Matrix (%2) do not match").arg(daSrcDataArray->getNumberOfTuples()).arg(daDestAttributeMatrix->getNumTuples());
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
    else if (daSrcAttributeMatrix->getName() == daDestAttributeMatrix->getName())
    {
      QString ss = QObject::tr("The source and destination Attribute Matrix are the same.  Is this what you meant to do?");
      notifyWarningMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    daDestAttributeMatrix->addAttributeArray(daSrcPath.getDataArrayName(), daSrcDataArray);
    daSrcAttributeMatrix->removeAttributeArray(daSrcPath.getDataArrayName());
  }
  else
  {
    setErrorCondition(-11020);
    QString ss = QObject::tr("Neither an Attribute Matrix nor an Attribute Array was selected to be moved");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MoveData::preflight()
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
void MoveData::execute()
{
  setErrorCondition(0);
  // Simply running the preflight will do what we need it to.
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer MoveData::newFilterInstance(bool copyFilterParameters)
{
  MoveData::Pointer filter = MoveData::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MoveData::getCompiledLibraryName()
{ return Core::CoreBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MoveData::getGroupName()
{ return DREAM3D::FilterGroups::CoreFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MoveData::getSubGroupName()
{ return DREAM3D::FilterSubGroups::MemoryManagementFilters;}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MoveData::getHumanLabel()
{ return "Move Data";}
