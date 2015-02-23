/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2013 Dr. Joseph C. Tucker (UES, Inc.)
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
 * Neither the name of Joseph C. Tucker, Michael A. Groeber, Michael A. Jackson,
 * UES, Inc., the US Air Force, BlueQuartz Software nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *                   FA8650-07-D-5800 and FA8650-10-D-5226
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ExportData.h"

#include <QtCore/QDir>

#include "IO/IOConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExportData::ExportData() :
  AbstractFilter(),
  m_SelectedArrayPath("", "", ""),
  m_Delimeter(0),
  m_FileExtension(".txt"),
  m_MaxValPerLine(-1),
  m_OutputPath("")
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExportData::~ExportData()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExportData::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Array to Export", "SelectedArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSelectedArrayPath(), false));
  parameters.push_back(FileSystemFilterParameter::New("Output Path", "OutputPath", FilterParameterWidgetType::OutputPathWidget, getOutputPath(), false));
  parameters.push_back(FilterParameter::New("File Extension", "FileExtension", FilterParameterWidgetType::StringWidget, getFileExtension(), false));
  parameters.push_back(FilterParameter::New("Maximum Values/Line", "MaxValPerLine", FilterParameterWidgetType::IntWidget, getMaxValPerLine(), false));

  ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New(); 	//Delimeter choice
  parameter->setHumanLabel("Delimeter");
  parameter->setPropertyName("Delimeter");
  parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
  QVector<QString> choices;
  choices.push_back(", (comma)");
  choices.push_back("; (semicolon)");
  choices.push_back("  (space)");
  choices.push_back(": (colon)");
  parameter->setChoices(choices);
  parameters.push_back(parameter);

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExportData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedArrayPath(reader->readDataArrayPath("SelectedArrayPath", getSelectedArrayPath()));
  setOutputPath(reader->readString("OutputPath", getOutputPath()));
  setDelimeter(reader->readValue("Delimeter", getDelimeter()));
  setFileExtension(reader->readString("FileExtension", getFileExtension()));
  setMaxValPerLine(reader->readValue("MaxValPerLine", getMaxValPerLine()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ExportData::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(SelectedArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputPath)
  DREAM3D_FILTER_WRITE_PARAMETER(Delimeter)
  DREAM3D_FILTER_WRITE_PARAMETER(FileExtension)
  DREAM3D_FILTER_WRITE_PARAMETER(MaxValPerLine)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExportData::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  if (m_SelectedArrayPath.isEmpty() == true)
  {
    setErrorCondition(-11001);
    QString ss = QObject::tr("The complete path to the Data Array can not be empty. Please set an appropriate path.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if (m_OutputPath.isEmpty() == true)
  {
    setErrorCondition(-11002);
    QString ss = QObject::tr("The output path must be set before executing this filter.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if (m_MaxValPerLine <= 0)
  {
    setErrorCondition(-11003);
    QString ss = QObject::tr("The Maximum Values/Line can not be zero or less. The current value is '%1'. Please set a value greater than zero.").arg(m_MaxValPerLine);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(m_SelectedArrayPath.getDataContainerName());
  if (getErrorCondition() < 0) { return; }

  AttributeMatrix::Pointer cellAttrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, m_SelectedArrayPath, -303);
  if (getErrorCondition() < 0) { return; }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExportData::preflight()
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
template<typename T>
int writeDataFile(IDataArray::Pointer inputData, QString delimeter, QString outputFile, int MaxValPerLine)
{
  DataArray<T>* inputArray = DataArray<T>::SafePointerDownCast(inputData.get());
  if (NULL == inputArray)
  {
    return -11007;
  }

  // Create file name
  FILE* f = NULL;
  f = fopen(outputFile.toLatin1().data(), "wb");
  if (NULL == f)
  {
    return -11008;
  }

  T* inputArrayPtr = inputArray->getPointer(0);
  size_t numPoints = inputArray->getNumberOfTuples();
  QString str;
  size_t elementPerLine = 0;
  for (size_t i = 0; i < numPoints; i++)
  {
    if (elementPerLine != 0) // no delimeter to start
    {
      if (elementPerLine % MaxValPerLine) // maximum value per line
      {
        fprintf(f, "%s", delimeter.toLatin1().data());
      }
      else
      {
        fprintf(f, "\n");
      }
    }
    fprintf(f, "%s", str.setNum(inputArrayPtr[i]).toLatin1().data());
    elementPerLine++;
  }
  fclose(f); // close the file
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExportData::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  QString ss;
  QDir dir;
  if (!dir.mkpath(m_OutputPath))
  {
    ss = QObject::tr("Error creating output path '%1'").arg(m_OutputPath);
    setErrorCondition(-11004);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if (m_FileExtension.startsWith(".") == false) // if no '.', add '.' to file extension
  {
    m_FileExtension = "." + m_FileExtension;
  }


  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_SelectedArrayPath.getDataContainerName());

  IDataArray::Pointer inputData = m->getAttributeMatrix(m_SelectedArrayPath.getAttributeMatrixName())->getAttributeArray(m_SelectedArrayPath.getDataArrayName());
  if (NULL == inputData.get())
  {
    ss = QObject::tr("Selected array '%1' does not exist in the Data Container. Was it spelled correctly?").arg(m_SelectedArrayPath.getDataArrayName());
    setErrorCondition(-11007);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QVector<size_t> dims = inputData->getComponentDimensions(); // write scalar array only
  int numComp = dims[0];
  for (int i = 1; i < dims.size(); i++)
  {
    numComp *= dims[i];
  }
  if (numComp > 1)
  {
    ss = QObject::tr("Selected array '%1' is not a scalar array").arg(m_SelectedArrayPath.getDataArrayName());
    setErrorCondition(-11005);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QString exportArrayFile = m_OutputPath + QDir::separator() + inputData->getName() + m_FileExtension; // the complete output file path, name and extension

  QString delimeter = lookupDelimeter();

  int err;
  QString dType = inputData->getTypeAsString();
  if (dType.compare("int8_t") == 0)
  {
    err = writeDataFile<int8_t>(inputData, delimeter, exportArrayFile, m_MaxValPerLine);
  }
  else if (dType.compare("uint8_t") == 0)
  {
    err = writeDataFile<uint8_t>(inputData, delimeter, exportArrayFile, m_MaxValPerLine);
  }
  else if (dType.compare("int16_t") == 0)
  {
    err = writeDataFile<int16_t>(inputData, delimeter, exportArrayFile, m_MaxValPerLine);
  }
  else if (dType.compare("uint16_t") == 0)
  {
    err = writeDataFile<uint16_t>(inputData, delimeter, exportArrayFile, m_MaxValPerLine);
  }
  else if (dType.compare("int32_t") == 0)
  {
    err = writeDataFile<int32_t>(inputData, delimeter, exportArrayFile, m_MaxValPerLine);
  }
  else if (dType.compare("uint32_t") == 0)
  {
    err = writeDataFile<uint32_t>(inputData, delimeter, exportArrayFile, m_MaxValPerLine);
  }
  else if (dType.compare("int64_t") == 0)
  {
    err = writeDataFile<int64_t>(inputData, delimeter, exportArrayFile, m_MaxValPerLine);
  }
  else if (dType.compare("uint64_t") == 0)
  {
    err = writeDataFile<uint64_t>(inputData, delimeter, exportArrayFile, m_MaxValPerLine);
  }
  else if (dType.compare("float") == 0)
  {
    err = writeDataFile<float>(inputData, delimeter, exportArrayFile, m_MaxValPerLine);
  }
  else if (dType.compare("double") == 0)
  {
    err = writeDataFile<double>(inputData, delimeter, exportArrayFile, m_MaxValPerLine);
  }
  else if (dType.compare("bool") == 0)
  {
    err = writeDataFile<bool>(inputData, delimeter, exportArrayFile, m_MaxValPerLine);
  }
  else
  {
    ss = QObject::tr("Incorrect type for selected array '%1' for the output file ").arg(m_SelectedArrayPath.getDataArrayName());
    setErrorCondition(-11006);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if (-11007 == err)
  {
    ss = QObject::tr("The input array '%1' does not exist in the DataContainer and AttributeMatrix selected.").arg(m_SelectedArrayPath.getDataArrayName());
    setErrorCondition(err);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if (-11008 == err)
  {
    ss = QObject::tr("Could not open Output file '%1' for writing.").arg(exportArrayFile);
    setErrorCondition(err);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExportData::lookupDelimeter()
{
  QString del;
  switch (m_Delimeter)
  {
  case Comma:
    del = ",";
    break;
  case Semicolon:
    del = ";";
    break;
  case Space:
    del = " ";
    break;
  case Colon:
    del = ":";
    break;
  default:
    del = " ";
  }
  return del;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ExportData::newFilterInstance(bool copyFilterParameters)
{
  ExportData::Pointer filter = ExportData::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExportData::getCompiledLibraryName()
{ return IO::IOBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExportData::getGroupName()
{ return DREAM3D::FilterGroups::IOFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExportData::getSubGroupName()
{ return DREAM3D::FilterSubGroups::OutputFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExportData::getHumanLabel()
{ return "Export Data"; }

