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

#include "FeatureDataCSVWriter.h"

#include <QtCore/QDir>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataArrays/NeighborList.hpp"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"

#include "IO/IOConstants.h"
#include "IO/IOVersion.h"

// Include the MOC generated file for this class
#include "moc_FeatureDataCSVWriter.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FeatureDataCSVWriter::FeatureDataCSVWriter() :
  AbstractFilter(),
  m_CellFeatureAttributeMatrixPath("", "", ""),
  m_FeatureDataFile(""),
  m_WriteNeighborListData(false),
  m_Delimiter(',')
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FeatureDataCSVWriter::~FeatureDataCSVWriter()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureDataCSVWriter::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(OutputFileFilterParameter::New("Output File", "FeatureDataFile", getFeatureDataFile(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FeatureDataCSVWriter, this, FeatureDataFile), SIMPL_BIND_GETTER(FeatureDataCSVWriter, this, FeatureDataFile), "*.csv", "Comma Separated Data"));
  parameters.push_back(BooleanFilterParameter::New("Write Neighbor Data", "WriteNeighborListData", getWriteNeighborListData(), FilterParameter::Parameter, SIMPL_BIND_SETTER(FeatureDataCSVWriter, this, WriteNeighborListData), SIMPL_BIND_GETTER(FeatureDataCSVWriter, this, WriteNeighborListData)));
  parameters.push_back(SeparatorFilterParameter::New("Feature Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(SIMPL::AttributeMatrixObjectType::Feature);
    parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Feature Attribute Matrix", "CellFeatureAttributeMatrixPath", getCellFeatureAttributeMatrixPath(), FilterParameter::RequiredArray, req, SIMPL_BIND_SETTER(FeatureDataCSVWriter, this, CellFeatureAttributeMatrixPath), SIMPL_BIND_GETTER(FeatureDataCSVWriter, this, CellFeatureAttributeMatrixPath)));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureDataCSVWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellFeatureAttributeMatrixPath( reader->readDataArrayPath( "CellFeatureAttributeMatrixPath", getCellFeatureAttributeMatrixPath() ) );
  setFeatureDataFile( reader->readString( "FeatureDataFile", getFeatureDataFile() ) );
  setWriteNeighborListData( reader->readValue("WriteNeighborListData", getWriteNeighborListData()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureDataCSVWriter::initialize()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureDataCSVWriter::dataCheck()
{
  setErrorCondition(0);

  getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getCellFeatureAttributeMatrixPath(), -301);

  if (getFeatureDataFile().isEmpty() == true)
  {
    QString ss = QObject::tr("The output file must be set");
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  QFileInfo fi(getFeatureDataFile());
  QDir parentPath(fi.path());
  if (parentPath.exists() == false)
  {
    QString ss = QObject::tr("The directory path for the output file does not exist. DREAM.3D will attempt to create this path during execution of the filter");
    notifyWarningMessage(getHumanLabel(), ss, -1);
  }
  if (fi.suffix().compare("") == 0)
  {
    setFeatureDataFile(getFeatureDataFile().append(".csv"));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureDataCSVWriter::preflight()
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
void FeatureDataCSVWriter::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(getFeatureDataFile());
  QDir parentPath(fi.path());
  if(!parentPath.mkpath("."))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath.absolutePath());
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QFile file(getFeatureDataFile());
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("Output file could not be opened: %1").arg(getFeatureDataFile());
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QTextStream outFile(&file);

  AttributeMatrix::Pointer cellFeatureAttrMat = getDataContainerArray()->getAttributeMatrix(getCellFeatureAttributeMatrixPath());

  // Write the total number of features
  outFile << cellFeatureAttrMat->getNumTuples() - 1 << "\n";
  // Get all the names of the arrays from the Data Container
  QList<QString> headers = cellFeatureAttrMat->getAttributeArrayNames();

  std::vector<IDataArray::Pointer> data;

  // For checking if an array is a neighborlist
  NeighborList<int32_t>::Pointer neighborlistPtr = NeighborList<int32_t>::CreateArray(0, "_INTERNAL_USE_ONLY_JunkNeighborList", false);

  // Print the FeatureIds Header before the rest of the headers
  outFile << SIMPL::FeatureData::FeatureID;
  // Loop throught the list and print the rest of the headers, ignoring those we don't want
  for (QList<QString>::iterator iter = headers.begin(); iter != headers.end(); ++iter)
  {
    // Only get the array if the name does NOT match those listed
    IDataArray::Pointer p = cellFeatureAttrMat->getAttributeArray(*iter);
    if (p->getNameOfClass().compare(neighborlistPtr->getNameOfClass()) != 0)
    {
      if (p->getNumberOfComponents() == 1)
      {
        outFile << m_Delimiter << (*iter);
      }
      else // There are more than a single component so we need to add multiple header values
      {
        for (int32_t k = 0; k < p->getNumberOfComponents(); ++k)
        {
          outFile << m_Delimiter << (*iter) << "_" << k;
        }
      }
      // Get the IDataArray from the DataContainer
      data.push_back(p);
    }
  }
  outFile << "\n";

  // Get the number of tuples in the arrays
  size_t numTuples =  0;
  if (!data.empty())
  {
    numTuples = data[0]->getNumberOfTuples();
  }

  float threshold = 0.0f;

  // Skip feature 0
  for (size_t i = 1; i < numTuples; ++i)
  {
    if (((float)i / numTuples) * 100.0f > threshold)
    {
      QString ss = QObject::tr("Writing Feature Data || %1% Complete").arg(((float)i / numTuples) * 100.0f);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
      threshold = threshold + 5.0f;
      if (threshold < ((float)i / numTuples) * 100.0f)
      {
        threshold = ((float)i / numTuples) * 100.0f;
      }
    }

    // Print the feature id
    outFile << i;
    // Print a row of data
    for (std::vector<IDataArray::Pointer>::iterator p = data.begin(); p != data.end(); ++p)
    {
      outFile << m_Delimiter;
      (*p)->printTuple(outFile, i, m_Delimiter);
    }
    outFile << "\n";
  }

  if (m_WriteNeighborListData == true)
  {
    // Print the FeatureIds Header before the rest of the headers
    // Loop throught the list and print the rest of the headers, ignoring those we don't want
    for (QList<QString>::iterator iter = headers.begin(); iter != headers.end(); ++iter)
    {
      // Only get the array if the name does NOT match those listed
      IDataArray::Pointer p = cellFeatureAttrMat->getAttributeArray(*iter);
      if (p->getNameOfClass().compare(neighborlistPtr->getNameOfClass()) == 0)
      {
        outFile << SIMPL::FeatureData::FeatureID << m_Delimiter << SIMPL::FeatureData::NumNeighbors << m_Delimiter << (*iter) << "\n";
        numTuples = p->getNumberOfTuples();

        // Skip feature 0
        for(size_t i = 1; i < numTuples; ++i)
        {
          // Print the feature id
          outFile << i;
          // Print a row of data
          outFile << m_Delimiter;
          p->printTuple(outFile, i, m_Delimiter);
          outFile << "\n";
        }
      }
    }
  }
  file.close();

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FeatureDataCSVWriter::newFilterInstance(bool copyFilterParameters)
{
  /*
  * FeatureDataFile
  * WriteNeighborListData
  */
  FeatureDataCSVWriter::Pointer filter = FeatureDataCSVWriter::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FeatureDataCSVWriter::getCompiledLibraryName()
{
  return IOConstants::IOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FeatureDataCSVWriter::getBrandingString()
{
  return "IO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FeatureDataCSVWriter::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  IO::Version::Major() << "." << IO::Version::Minor() << "." << IO::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FeatureDataCSVWriter::getGroupName()
{ return SIMPL::FilterGroups::IOFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FeatureDataCSVWriter::getSubGroupName()
{ return SIMPL::FilterSubGroups::OutputFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FeatureDataCSVWriter::getHumanLabel()
{ return "Write Feature Data as CSV File"; }
