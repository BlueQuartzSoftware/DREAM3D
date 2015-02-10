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
	m_Delimeter(),
	m_FileExtension(".txt"),
	m_MaxRecords(-1),
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
	parameters.push_back(FilterParameter::New("Maximum Records/Line", "MaxRecords", FilterParameterWidgetType::IntWidget, getMaxRecords(), false));
	//Delimeter choice
	{
		ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
		parameter->setHumanLabel("Delimeter");
		parameter->setPropertyName("Delimeter");
		parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
		QVector<QString> choices;
		choices.push_back(",");
		choices.push_back(";");
		choices.push_back(" ");
		choices.push_back(":");
		parameter->setChoices(choices);
		parameters.push_back(parameter);
	}
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
	setMaxRecords(reader->readValue("MaxRecords", getMaxRecords()));
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
	DREAM3D_FILTER_WRITE_PARAMETER(MaxRecords)
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
    setErrorCondition(-11010);
    QString ss = QObject::tr("The complete path to the Data Array can not be empty. Please set an appropriate path.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

	if (m_OutputPath.isEmpty() == true)
	{
		setErrorCondition(-12001);
		QString ss = QObject::tr("The output path must be set before executing this filter.");
		notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
	}

  if (m_MaxRecords <= 0)
  {
    setErrorCondition(-11011);
		QString ss = QObject::tr("The Maximum Records/Line can not be zero or less. The current value is '%1'. Please set a value greater than zero.").arg(m_MaxRecords);
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
void writeDataFile(IDataArray::Pointer inputData, int m_Delimeter, QString m_FileExtension, int m_MaxRecords, QString m_OutputPath)
{
	DataArray<T>* inputArray = DataArray<T>::SafePointerDownCast(inputData.get());
	if (NULL == inputArray)
	{
		return;
	}

	// Create file name
	QString outputFile = m_OutputPath + m_FileExtension;
	FILE* f = NULL;
	f = fopen(outputFile.toLatin1().data(), "wb");
	if (NULL == f)
	{

	}

	fprintf(f, "*Heading\n");

	T* inputArrayPtr = inputArray->getPointer(0);
	size_t numPoints = inputArray->getNumberOfTuples();
	for (size_t i = 0; i < numPoints; i++)
	{
		fprintf(f, "%s%s ", inputArrayPtr[i], m_Delimeter);
	}

	// Close the file
	fclose(f);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExportData::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

	int err;
	setErrorCondition(0);
	dataCheck();
	if (getErrorCondition() < 0) { return; }

	// Check Output Path
	QDir dir;
	if (!dir.mkpath(m_OutputPath))
	{
		QString ss = QObject::tr("Error creating parent path '%1'").arg(m_OutputPath);
		notifyErrorMessage(getHumanLabel(), ss, -1);
		setErrorCondition(-1);
		return;
	}

  QString ss;
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_SelectedArrayPath.getDataContainerName());

  IDataArray::Pointer inputData = m->getAttributeMatrix(m_SelectedArrayPath.getAttributeMatrixName())->getAttributeArray(m_SelectedArrayPath.getDataArrayName());
  if (NULL == inputData.get())
  {
    ss = QObject::tr("Selected array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_SelectedArrayPath.getDataArrayName());
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QVector<size_t> dims = inputData->getComponentDimensions();
  int numComp = dims[0];
  for (int i = 1; i < dims.size(); i++)
  {
    numComp *= dims[i];
  }
  if (numComp > 1)
  {
    ss = QObject::tr("Selected array '%1' is not a scalar array").arg(m_SelectedArrayPath.getDataArrayName());
    setErrorCondition(-11003);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

	QString dType = inputData->getTypeAsString();
	if (dType.compare("int8_t") == 0)
	{
		writeDataFile<int8_t>(inputData, m_Delimeter, m_FileExtension, m_MaxRecords, m_OutputPath);
	}
	else if (dType.compare("uint8_t") == 0)
	{
		writeDataFile<uint8_t>(inputData, m_Delimeter, m_FileExtension, m_MaxRecords, m_OutputPath);
	}
	else if (dType.compare("int16_t") == 0)
	{
		writeDataFile<int16_t>(inputData, m_Delimeter, m_FileExtension, m_MaxRecords, m_OutputPath);
	}
	else if (dType.compare("uint16_t") == 0)
	{
		writeDataFile<uint16_t>(inputData, m_Delimeter, m_FileExtension, m_MaxRecords, m_OutputPath);
	}
	else if (dType.compare("int32_t") == 0)
	{
		writeDataFile<int32_t>(inputData, m_Delimeter, m_FileExtension, m_MaxRecords, m_OutputPath);
	}
	else if (dType.compare("uint32_t") == 0)
	{
		writeDataFile<uint32_t>(inputData, m_Delimeter, m_FileExtension, m_MaxRecords, m_OutputPath);
	}
	else if (dType.compare("int64_t") == 0)
	{
		writeDataFile<int64_t>(inputData, m_Delimeter, m_FileExtension, m_MaxRecords, m_OutputPath);
	}
	else if (dType.compare("uint64_t") == 0)
	{
		writeDataFile<uint64_t>(inputData, m_Delimeter, m_FileExtension, m_MaxRecords, m_OutputPath);
	}
	else if (dType.compare("float") == 0)
	{
		writeDataFile<float>(inputData, m_Delimeter, m_FileExtension, m_MaxRecords, m_OutputPath);
	}
	else if (dType.compare("double") == 0)
	{
		writeDataFile<double>(inputData, m_Delimeter, m_FileExtension, m_MaxRecords, m_OutputPath);
	}
	else if (dType.compare("bool") == 0)
	{
		writeDataFile<bool>(inputData, m_Delimeter, m_FileExtension, m_MaxRecords, m_OutputPath);
	}
	else
	{
		ss = QObject::tr("Incorrect type for selected array '%1' for the output file ").arg(m_SelectedArrayPath.getDataArrayName());
		setErrorCondition(-11003);
		notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
		return;
	}



  notifyStatusMessage(getHumanLabel(), "Complete");
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

