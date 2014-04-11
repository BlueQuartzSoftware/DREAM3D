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

#include "FeatureDataCSVWriter.h"


#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"
#include "DREAM3DLib/DataArrays/NeighborList.hpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FeatureDataCSVWriter::FeatureDataCSVWriter() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
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
  parameters.push_back(FilterParameter::New("Output File", "FeatureDataFile", FilterParameterWidgetType::OutputFileWidget,"QString", false, "", "*.csv", "Comma Separated Data"));
  parameters.push_back(FilterParameter::New("Write Neighbor Data", "WriteNeighborListData", FilterParameterWidgetType::BooleanWidget,"bool", false));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureDataCSVWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFeatureDataFile( reader->readString( "FeatureDataFile", getFeatureDataFile() ) );
  setWriteNeighborListData( reader->readValue("WriteNeighborListData", getWriteNeighborListData()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FeatureDataCSVWriter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("FeatureDataFile", getFeatureDataFile() );
  writer->writeValue("WriteNeighborData", getWriteNeighborListData() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureDataCSVWriter::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }

  if (getFeatureDataFile().isEmpty() == true)
  {
    QString ss = QObject::tr(": The output file must be set before executing this filter.");
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
  }

  QFileInfo fi(getFeatureDataFile());
  QDir parentPath(fi.path());
  if (parentPath.exists() == false)
  {
    QString ss = QObject::tr("The directory path for the output file does not exist.");
    notifyWarningMessage(getHumanLabel(), ss, -1);
  }
  if (fi.suffix().compare("") == 0)
  {
    setFeatureDataFile(getFeatureDataFile().append(".dx"));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureDataCSVWriter::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureDataCSVWriter::execute()
{
  int err = 0;
  setErrorCondition(err);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(getFeatureDataFile());
  QDir parentPath(fi.path());
  if(!parentPath.mkpath("."))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath.absolutePath());
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
    return;
  }


  QFile file(getFeatureDataFile());
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("Feature Data CSV Output file could not be opened: %1").arg(getFeatureDataFile());
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QTextStream outFile(&file);

  // Write the total number of features
  outFile << m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples() - 1 << "\n";
  // Get all the names of the arrays from the Data Container
  QList<QString> headers = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getAttributeArrayNameList();

  std::vector<IDataArray::Pointer> data;

  //For checking if an array is a neighborlist
  NeighborList<int>::Pointer neighborlistPtr = NeighborList<int>::New();

  // Print the FeatureIds Header before the rest of the headers
  outFile << DREAM3D::FeatureData::FeatureID;
  // Loop throught the list and print the rest of the headers, ignoring those we don't want
  for(QList<QString>::iterator iter = headers.begin(); iter != headers.end(); ++iter)
  {
    // Only get the array if the name does NOT match those listed
    IDataArray::Pointer p = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getAttributeArray(*iter);
    if(p->getNameOfClass().compare(neighborlistPtr->getNameOfClass()) != 0)
    {
      if (p->getNumberOfComponents() == 1)
      {
        outFile << m_Delimiter << (*iter);
      }
      else // There are more than a single component so we need to add multiple header values
      {
        for(int k = 0; k < p->getNumberOfComponents(); ++k)
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
  size_t numTuples = data[0]->getNumberOfTuples();

  float threshold = 0.0f;

  // Skip the first feature
  for(size_t i = 1; i < numTuples; ++i)
  {
    if (((float)i / numTuples) * 100.0f > threshold)
    {

      QString ss = QObject::tr("Writing Feature Data - %1% Complete").arg(((float)i / numTuples) * 100);
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
    for( std::vector<IDataArray::Pointer>::iterator p = data.begin(); p != data.end(); ++p)
    {
      outFile << m_Delimiter;
      (*p)->printTuple(outFile, i, m_Delimiter);
    }
    outFile << "\n";
  }

  if(m_WriteNeighborListData == true)
  {
    // Print the FeatureIds Header before the rest of the headers
    // Loop throught the list and print the rest of the headers, ignoring those we don't want
    for(QList<QString>::iterator iter = headers.begin(); iter != headers.end(); ++iter)
    {
      // Only get the array if the name does NOT match those listed
      IDataArray::Pointer p = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getAttributeArray(*iter);
      if(p->getNameOfClass().compare(neighborlistPtr->getNameOfClass()) == 0)
      {
        outFile << DREAM3D::FeatureData::FeatureID << m_Delimiter << DREAM3D::FeatureData::NumNeighbors << m_Delimiter << (*iter) << "\n";
        numTuples = p->getNumberOfTuples();
        //    float threshold = 0.0f;

        // Skip the first feature
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
  notifyStatusMessage(getHumanLabel(), "FeatureDataCSVWriter Completed");

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
    filter->setFilterParameters(getFilterParameters() );
    
    //Loop over each Filter Parameter that is registered to the filter either through this class or a parent class
    // and copy the value from the current instance of the object into the "new" instance that was just created
    QVector<FilterParameter::Pointer> options = getFilterParameters(); // Get the current set of filter parameters
    for (QVector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
    {
      FilterParameter* parameter = (*iter).get();
      if (parameter->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0 )
      {
        continue; // Skip this type of filter parameter as it has nothing to do with anything in the filter.
      }
      // Get the property from the current instance of the filter
      QVariant var = property(parameter->getPropertyName().toLatin1().constData());
      bool ok = filter->setProperty(parameter->getPropertyName().toLatin1().constData(), var);
      if(false == ok)
      {
        QString ss = QString("Error occurred transferring the Filter Parameter '%1' in Filter '%2' to the filter instance. The pipeline may run but the underlying filter will NOT be using the values from the GUI."
                             " Please report this issue to the developers of this filter.").arg(parameter->getPropertyName()).arg(filter->getHumanLabel());
        Q_ASSERT_X(ok, __FILE__, ss.toLatin1().constData());
      }

      if(parameter->isConditional() == true)
      {
        QVariant cond = property(parameter->getConditionalProperty().toLatin1().constData() );
        ok = filter->setProperty(parameter->getConditionalProperty().toLatin1().constData(), cond);
        if(false == ok)
        {
          QString ss = QString("%1::newFilterInstance()\nError occurred transferring the Filter Parameter '%2' in Filter '%3' to the filter instance. "
                              " The filter parameter has a conditional property '%4'. The transfer of this property from the old filter to the new filter failed."
                              " Please report this issue to the developers of this filter.").arg(filter->getNameOfClass())
                              .arg(parameter->getPropertyName())
                              .arg(filter->getHumanLabel())
                              .arg(parameter->getConditionalProperty());
          Q_ASSERT_X(ok, __FILE__, ss.toLatin1().constData());
        }
      }
    }
  }
  return filter;
}
