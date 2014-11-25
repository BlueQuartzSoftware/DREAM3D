/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "MoveData.h"


static const int k_MoveAttributeMatrix = 0;
static const int k_MoveDataArray = 1;



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

    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("What to Move");
    parameter->setPropertyName("WhatToMove");
    parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    parameter->setDefaultValue(getWhatToMove()); // Just set the first index

    QVector<QString> choices;
    choices.push_back("Attribute Matrix");
    choices.push_back("Data Array");

    parameter->setChoices(choices);
    QStringList linkedProps;
    linkedProps << "DataContainerDestination" << "AttributeMatrixSource" << "AttributeMatrixDestination" << "DataArraySource";
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameters.push_back(parameter);

    parameters.push_back(FilterParameter::New("Attribute Matrix Source", "AttributeMatrixSource", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getAttributeMatrixSource(), false, "", 0));

    parameters.push_back(FilterParameter::New("Data Container Destination", "DataContainerDestination", FilterParameterWidgetType::DataContainerSelectionWidget, getDataContainerDestination(), false, "", 0));


    parameters.push_back(FilterParameter::New("Data Array Source", "DataArraySource", FilterParameterWidgetType::DataArraySelectionWidget, getDataArraySource(), false, "", 1));

    parameters.push_back(FilterParameter::New("Attribute Matrix Destination", "AttributeMatrixDestination", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getAttributeMatrixDestination(), false, "", 1));


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
    int val = getWhatToMove();
    writer->writeValue("WhatToMove", val );
    QString val2 = getDataContainerDestination();
    writer->writeValue("DataContainerDestination", val2 );
    DataArrayPath val3 = getAttributeMatrixSource();
    writer->writeValue("AttributeMatrixSource", val3 );
    DataArrayPath val4 = getAttributeMatrixDestination();
    writer->writeValue("AttributeMatrixDestination", val4 );
    DataArrayPath val5 = getDataArraySource();
    writer->writeValue("DataArraySource", val5 );
    writer->closeFilterGroup();
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

    if( getWhatToMove() == k_MoveAttributeMatrix )
    {
        DataContainer::Pointer amDestDataContainer = getDataContainerArray()->getDataContainer(getDataContainerDestination());
        DataContainer::Pointer amSrcDataContainer = getDataContainerArray()->getDataContainer(amSrcPath);
        AttributeMatrix::Pointer amSrcAttributeMatrix = getDataContainerArray()->getAttributeMatrix(amSrcPath);

        if (NULL == amDestDataContainer.get())
        {
            setErrorCondition(-11011);
            QString ss = QObject::tr("The destination Data Container for the specified Attribute Matrix was not found in the DataContainerArray");
            notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
            return;
        }
        else if (NULL == amSrcDataContainer.get())
        {
            setErrorCondition(-11012);
            QString ss = QObject::tr("The source Data Container for the specified Attribute Matrix was not found in the DataContainerArray");
            notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
            return;
        }
        else if (NULL == amSrcAttributeMatrix.get())
        {
            setErrorCondition(-11013);
            QString ss = QObject::tr("The specified Attribute Matrix was not found inside its Data Container");
            notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
            return;
        }
        else if(amSrcDataContainer->getName() == amDestDataContainer->getName())
        {
            QString ss = QObject::tr("The source and destination Data Container are the same.  Is this what you meant to do?");
            notifyWarningMessage(getHumanLabel(), ss, getErrorCondition());
            return;
        }

        amDestDataContainer->addAttributeMatrix(amSrcAttributeMatrix->getName(), amSrcAttributeMatrix);
        amSrcDataContainer->removeAttributeMatrix(amSrcAttributeMatrix->getName());
    }
    else if( getWhatToMove() == k_MoveDataArray )
    {
        AttributeMatrix::Pointer daSrcAttributeMatrix = getDataContainerArray()->getAttributeMatrix(daSrcPath);
        IDataArray::Pointer daSrcDataArray = getDataContainerArray()->getExistingPrereqArrayFromPath<IDataArray, AbstractFilter>(this, daSrcPath);

        if (getErrorCondition() < 0)
        {
            return;
        }

        AttributeMatrix::Pointer daDestAttributeMatrix = getDataContainerArray()->getPrereqAttributeMatrixFromPath<DataContainer>(this, amDestPath, -11013);

        if (getErrorCondition() < 0)
        {
            return;
        }

        if (daDestAttributeMatrix->getNumTuples() != daSrcDataArray->getNumberOfTuples())
        {
            setErrorCondition(-11019);
            QString ss = QObject::tr("The number of tuples of source and destination do not match");
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
        QString ss = QObject::tr("Neither an Attribute Matrix nor a Data Array was selected to be moved.  Please contact the DREAM3D developers to resolve this issue");
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
    int err = 0;
    QString ss;
    setErrorCondition(err);

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
{ return DREAM3D::FilterSubGroups::MemoryManagementFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MoveData::getHumanLabel()
{ return "Move Data"; }

