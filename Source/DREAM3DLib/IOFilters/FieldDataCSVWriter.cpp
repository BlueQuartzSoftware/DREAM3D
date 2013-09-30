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

#include "FieldDataCSVWriter.h"


#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"
#include "DREAM3DLib/DataArrays/NeighborList.hpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FieldDataCSVWriter::FieldDataCSVWriter() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_FieldDataFile(""),
  m_WriteNeighborListData(false)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FieldDataCSVWriter::~FieldDataCSVWriter()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FieldDataCSVWriter::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output File");
    option->setPropertyName("FieldDataFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setFileExtension("*.csv");
    option->setFileType("Comma Separated Data");
    option->setValueType("string");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Neighbor Data");
    option->setPropertyName("WriteNeighborListData");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FieldDataCSVWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setFieldDataFile( reader->readValue( "FieldDataFile", getFieldDataFile() ) );
  setWriteNeighborListData( reader->readValue("WriteNeighborListData", getWriteNeighborListData()) );
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FieldDataCSVWriter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("FieldDataFile", getFieldDataFile() );
  writer->writeValue("WriteNeighborData", getWriteNeighborListData() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FieldDataCSVWriter::preflight()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  if (getFieldDataFile().isEmpty() == true)
  {
    QString ss = QObject::tr(": The output file must be set before executing this filter.");
    addErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
  }

  QFileInfo fi(getFieldDataFile());
  QDir parentPath(fi.path());
  if (parentPath.exists() == false)
  {
    QString ss = QObject::tr("The directory path for the output file does not exist.");
    addWarningMessage(getHumanLabel(), ss, -1);
  }
  if (fi.suffix().compare("") == 0)
  {
    setFieldDataFile(getFieldDataFile().append(".dx"));
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FieldDataCSVWriter::execute()
{
  int err = 0;
  setErrorCondition(err);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(getFieldDataFile());
  QDir parentPath(fi.path());
  if(!parentPath.mkpath("."))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath.absolutePath());
    notifyErrorMessage(ss, -1);
    setErrorCondition(-1);
    return;
  }


  QFile file(getFieldDataFile());
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("Field Data CSV Output file could not be opened: %1").arg(getFieldDataFile());
    setErrorCondition(-100);
    notifyErrorMessage(ss, getErrorCondition());
    return;
  }

  QTextStream outFile(&file);

  char space = DREAM3D::GrainData::Delimiter;
  // Write the total number of grains
  outFile << m->getNumCellFieldTuples()-1 ;
  // Get all the names of the arrays from the Data Container
  QList<QString> headers = m->getCellFieldArrayNameList();

  std::vector<IDataArray::Pointer> data;

  //For checking if an array is a neighborlist
  NeighborList<int>::Pointer neighborlistPtr = NeighborList<int>::New();

  // Print the GrainIds Header before the rest of the headers
  outFile << DREAM3D::GrainData::GrainID;
  // Loop throught the list and print the rest of the headers, ignoring those we don't want
  for(QList<QString>::iterator iter = headers.begin(); iter != headers.end(); ++iter)
  {
    // Only get the array if the name does NOT match those listed
    IDataArray::Pointer p = m->getCellFieldData(*iter);
    if(p->getNameOfClass().compare(neighborlistPtr->getNameOfClass()) != 0)
    {
      if (p->GetNumberOfComponents() == 1) {
        outFile << space << (*iter);
      }
      else // There are more than a single component so we need to add multiple header values
      {
        for(int k = 0; k < p->GetNumberOfComponents(); ++k)
        {
          outFile << space << (*iter) << "_" << k;
        }
      }
      // Get the IDataArray from the DataContainer
      data.push_back(p);
    }
  }
  outFile << "\n";

  // Get the number of tuples in the arrays
  size_t numTuples = data[0]->getNumberOfTuples();
  QString ss;
  float threshold = 0.0f;

  // Skip the first grain
  for(size_t i = 1; i < numTuples; ++i)
  {
    if (((float)i / numTuples) * 100.0f > threshold) {

      QString ss = QObject::tr("Writing Field Data - %1% Complete").arg(((float)i / numTuples) * 100);
      notifyStatusMessage(ss);
      threshold = threshold + 5.0f;
      if (threshold < ((float)i / numTuples) * 100.0f) {
        threshold = ((float)i / numTuples) * 100.0f;
      }
    }

    // Print the grain id
    outFile << i;
    // Print a row of data
    for( std::vector<IDataArray::Pointer>::iterator p = data.begin(); p != data.end(); ++p)
    {
      outFile << space;
      (*p)->printTuple(outFile, i, space);
    }
    outFile << "\n";
  }

  if(m_WriteNeighborListData == true)
  {
    // Print the GrainIds Header before the rest of the headers
    // Loop throught the list and print the rest of the headers, ignoring those we don't want
    for(QList<QString>::iterator iter = headers.begin(); iter != headers.end(); ++iter)
    {
      // Only get the array if the name does NOT match those listed
      IDataArray::Pointer p = m->getCellFieldData(*iter);
      if(p->getNameOfClass().compare(neighborlistPtr->getNameOfClass()) == 0)
      {
        outFile << DREAM3D::GrainData::GrainID << space << DREAM3D::GrainData::NumNeighbors << space << (*iter) << "\n";
        size_t numTuples = p->getNumberOfTuples();
        //	  float threshold = 0.0f;

        // Skip the first grain
        for(size_t i = 1; i < numTuples; ++i)
        {
          // Print the grain id
          outFile << i;
          // Print a row of data
          outFile << space;
          p->printTuple(outFile, i, space);
          outFile << "\n";
        }
      }
    }
  }
  file.close();

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage("FieldDataCSVWriter Completed");

}







