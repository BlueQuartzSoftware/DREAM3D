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

#include "FindArrayHistogram.h"

#include "DREAM3DLib/Common/Constants.h"

//#include "Statistics/DistributionAnalysisOps/BetaOps.h"
//#include "Statistics/DistributionAnalysisOps/PowerLawOps.h"
//#include "Statistics/DistributionAnalysisOps/LogNormalOps.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindArrayHistogram::FindArrayHistogram() :
  AbstractFilter(),
	m_SelectedArrayPath("", "", ""),
	m_NumberOfBins(1),
	m_Normalize(false),
	m_NewDataArrayName(""),
	m_NewAttributeMatrixName(""),
	m_NewArray(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindArrayHistogram::~FindArrayHistogram()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindArrayHistogram::setupFilterParameters()
{
  FilterParameterVector parameters;
	parameters.push_back(FilterParameter::New("Array to Histogram", "SelectedArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSelectedArrayPath(), false));
	parameters.push_back(FilterParameter::New("Number of Bins", "NumberOfBins", FilterParameterWidgetType::IntWidget, getNumberOfBins(), false));
	parameters.push_back(FilterParameter::New("Normalize", "Normalize", FilterParameterWidgetType::BooleanWidget, getNormalize(), false));
	parameters.push_back(FilterParameter::New("New Data Array Name", "NewDataArrayName", FilterParameterWidgetType::StringWidget, getNewDataArrayName(), false, ""));
	parameters.push_back(FilterParameter::New("New Attribute Matrix Name", "NewAttributeMatrixName", FilterParameterWidgetType::StringWidget, getNewAttributeMatrixName(), false, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindArrayHistogram::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
	setSelectedArrayPath(reader->readDataArrayPath("SelectedArrayPath", getSelectedArrayPath()));
	setNumberOfBins(reader->readValue("NumberOfBins", getNumberOfBins()));
	setNormalize(reader->readValue("Normalize", false));
	setNewDataArrayName(reader->readString("NewDataArrayName", getNewDataArrayName()));
	setNewAttributeMatrixName(reader->readString("NewAttributeMatrixName", getNewAttributeMatrixName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindArrayHistogram::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
	DREAM3D_FILTER_WRITE_PARAMETER(SelectedArrayPath)
	DREAM3D_FILTER_WRITE_PARAMETER(NewAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(NumberOfBins)
	DREAM3D_FILTER_WRITE_PARAMETER(Normalize)
	DREAM3D_FILTER_WRITE_PARAMETER(NewDataArrayName)
	writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindArrayHistogram::dataCheck()
{
  DataArrayPath tempPath;
	setErrorCondition(0);

	if (m_NewDataArrayName.isEmpty() == true)
	{
		setErrorCondition(-11008);
		QString ss = QObject::tr("The New Histogram Array name can not be empty. Please set a value.");
		notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
		return;
	}
	
	if (m_NewAttributeMatrixName.isEmpty() == true)
	{
		setErrorCondition(-11009);
		QString ss = QObject::tr("The New Histogram Attribute Matrix name can not be empty. Please set a value.");
		notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
		return;
	}

	if (m_SelectedArrayPath.isEmpty() == true)
	{
		setErrorCondition(-11010);
		QString ss = QObject::tr("The complete path to the Histogtam Array can not be empty. Please set an appropriate path.");
		notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
		return;
	}
	else
	{
		QString dcName = m_SelectedArrayPath.getDataContainerName();
		QString amName = m_SelectedArrayPath.getAttributeMatrixName();
		QString daName = m_SelectedArrayPath.getDataArrayName();

		DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(dcName);
		if (NULL == dc.get())
		{
			setErrorCondition(-11007);
			QString ss = QObject::tr("The DataContainer '%1' was not found in the DataContainerArray").arg(dcName);
			notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
			return;
		}

		AttributeMatrix::Pointer attrMat = dc->getAttributeMatrix(amName);
		if (NULL == attrMat.get())
		{
			setErrorCondition(-11008);
			QString ss = QObject::tr("The AttributeMatrix '%1' was not found in the DataContainer '%2'").arg(amName).arg(dcName);
			notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
			return;
		}

		RenameErrorCodes code = attrMat->renameAttributeArray(daName, m_NewDataArrayName, false);
		switch (code)
		{
		case OLD_DOES_NOT_EXIST:
		{
			setErrorCondition(-11016);
			QString ss = QObject::tr("A DataArray with the name '%1' was not found in the AttributeMatrix").arg(daName);
			notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
			return;
		}
		case NEW_EXISTS:
		{
			setErrorCondition(-11017);
			QString ss = QObject::tr("A DataArray with the name '%1' already exists in the AttributeMatrix").arg(m_NewDataArrayName);
			notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
			return;
		}
		case SUCCESS:
		{
			setErrorCondition(0);
			return;
		}
		}
	}

//  QVector<size_t> dims(1, 1);
//  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
//  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
//  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

//  int numComp = m_NumberOfBins;
//  m_NewEnsembleArrayArrayName = m_SelectedFeatureArrayPath.getDataArrayName() + QString("Histogram");
//  dims[0] = numComp;
//  tempPath.update(getCellEnsembleAttributeMatrixName().getDataContainerName(), getCellEnsembleAttributeMatrixName().getAttributeMatrixName(), getNewEnsembleArrayArrayName() );
//  m_NewEnsembleArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int>, AbstractFilter>(this, tempPath, 0, dims); /* Assigns the shared_ptr<>(this, tempPath, 0, dims); Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
//  if( NULL != m_NewEnsembleArrayPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
//  { m_NewEnsembleArray = m_NewEnsembleArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindArrayHistogram::preflight()
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
void findHistogram(IDataArray::Pointer inputData, int numberOfBins, bool normalize, QString m_NewDataArrayName, QString m_NewAttributeMatrixName)
{
  DataArray<T>* sourceArray = DataArray<T>::SafePointerDownCast(inputData.get());
	if (NULL == sourceArray)
  {
    return;
  }

	T* fPtr = sourceArray->getPointer(0);
	size_t numTuples = sourceArray->getNumberOfTuples();

  int32_t bin;
  int32_t ensemble;
  float min = 1000000.0f;
  float max = 0.0f;
  float value;
	for (size_t i = 1; i < numTuples; i++)
  {
    value = fPtr[i];
    if(value > max) { max = value; }
    if(value < min) { min = value; }
  }
  float stepsize = (max - min) / numberOfBins;

	for (size_t i = 1; i < numTuples; i++)
  {
/*     ensemble = eIds[i];
     bin = (fPtr[i] - min) / stepsize;
     if(bin >= numberOfBins) { bin = numberOfBins - 1; }
     ensembleArray[(numberOfBins * ensemble) + bin]++;*/   
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindArrayHistogram::execute()
{
	float a[10];
	int foo1[20] = { 5, 8, 9, 3, 4, 7, 1, 7, 7, 3, 4, 5, 7, 3, 1, 1, 9, 9, 8, 8 };
	int foo[10] = { 5, 5, 9, 3, 4, 7, 1, 7, 7, 3 };
	int* pp = foo;
	size_t numTuples = 10;
	int32_t bin;
	int32_t ensemble;
	float min = 1000000.0f;
	float max = 0.0f;
	float value;
	int numberOfBins = 1;
	for (size_t i = 1; i < numTuples; i++)
	{
		value = pp[i];
		if (value > max) { max = value; }
		if (value < min) { min = value; }
	}
	float stepsize = (max - min) / numberOfBins; // number of bins

	for (size_t i = 1; i < numTuples; i++)
	{
		//     ensemble = eIds[i];
		ensemble = 1;
		bin = (pp[i] - min) / stepsize;
		if(bin >= numberOfBins) { bin = numberOfBins - 1; }
		a[(numberOfBins * ensemble) + bin]++;
	}

	// declare and initialize an array named n with size SIZE...
//	int n[10] = { 19, 3, 15, 7, 11, 9, 13, 5, 17, 1 };
//	float a1[10];
//	int i, j;
	// display the table header...
//	printf("%s%13s%17s\n", "Element/index", "Value", "Histogram");
	// do the iteration...
	// the outer for loop, read row by row...
//	for (i = 0; i <= (10 - 1); i++)
//	{
//		printf("%9d%15d       ", i, n[i]);
		// the inner for loop, for every row, read column by column and print the bar...
//		for (j = 1; j <= n[i]; j++)
			// print the asterisk bar...repeat...
//			printf("*");
		// go to new line for new row...repeats...
//		printf("\n");
//	}



  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

	QString dcName = m_SelectedArrayPath.getDataContainerName();
	QString amName = m_SelectedArrayPath.getAttributeMatrixName();
	QString daName = m_SelectedArrayPath.getDataArrayName();

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_SelectedArrayPath.getDataContainerName());

  QString ss;

  IDataArray::Pointer inputData = m->getAttributeMatrix(m_SelectedArrayPath.getAttributeMatrixName())->getAttributeArray(m_SelectedArrayPath.getDataArrayName());
  if (NULL == inputData.get())
  {
    ss = QObject::tr("Selected array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_SelectedArrayPath.getDataArrayName());
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QString dType = inputData->getTypeAsString();
  IDataArray::Pointer p = IDataArray::NullPointer();
  if (dType.compare("int8_t") == 0)
  {
    findHistogram<int8_t>(inputData, m_NumberOfBins, m_Normalize, m_NewDataArrayName, m_NewAttributeMatrixName);
  }
  else if (dType.compare("uint8_t") == 0)
  {
    findHistogram<uint8_t>(inputData, m_NumberOfBins, m_Normalize, m_NewDataArrayName, m_NewAttributeMatrixName);
  }
  else if (dType.compare("int16_t") == 0)
  {
    findHistogram<int16_t>(inputData, m_NumberOfBins, m_Normalize, m_NewDataArrayName, m_NewAttributeMatrixName);
  }
  else if (dType.compare("uint16_t") == 0)
  {
    findHistogram<uint16_t>(inputData, m_NumberOfBins, m_Normalize, m_NewDataArrayName, m_NewAttributeMatrixName);
  }
  else if (dType.compare("int32_t") == 0)
  {
    findHistogram<int32_t>(inputData, m_NumberOfBins, m_Normalize, m_NewDataArrayName, m_NewAttributeMatrixName);
  }
  else if (dType.compare("uint32_t") == 0)
  {
    findHistogram<uint32_t>(inputData, m_NumberOfBins, m_Normalize, m_NewDataArrayName, m_NewAttributeMatrixName);
  }
  else if (dType.compare("int64_t") == 0)
  {
    findHistogram<int64_t>(inputData, m_NumberOfBins, m_Normalize, m_NewDataArrayName, m_NewAttributeMatrixName);
  }
  else if (dType.compare("uint64_t") == 0)
  {
    findHistogram<uint64_t>(inputData, m_NumberOfBins, m_Normalize, m_NewDataArrayName, m_NewAttributeMatrixName);
  }
  else if (dType.compare("float") == 0)
  {
    findHistogram<float>(inputData, m_NumberOfBins, m_Normalize, m_NewDataArrayName, m_NewAttributeMatrixName);
  }
  else if (dType.compare("double") == 0)
  {
    findHistogram<double>(inputData, m_NumberOfBins, m_Normalize, m_NewDataArrayName, m_NewAttributeMatrixName);
  }
  else if (dType.compare("bool") == 0)
  {
    findHistogram<bool>(inputData, m_NumberOfBins, m_Normalize, m_NewDataArrayName, m_NewAttributeMatrixName);
  }
	else
	{
		ss = QObject::tr("Incorrect type for selected input array");//arg(m_SelectedCellArrayPath.getDataArrayName());
		setErrorCondition(-11002);
		notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
		return;
	}

	if (p.get() != NULL)
	{
//		p->setName(getCreatedArrayName());
//		AttributeMatrix::Pointer am = getDataContainerArray()->getAttributeMatrix(getFeatureIdsArrayPath());
//		am->addAttributeArray(p->getName(), p);
	}
	else
	{
		setErrorCondition(-14000);
		notifyErrorMessage(getHumanLabel(), "There was an issue creating a cell data array from a feature data array. Either the types did not match or we could not cast the input array to the expected output type", getErrorCondition());
	}
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindArrayHistogram::newFilterInstance(bool copyFilterParameters)
{
  FindArrayHistogram::Pointer filter = FindArrayHistogram::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindArrayHistogram::getCompiledLibraryName()
{ return Statistics::StatisticsBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindArrayHistogram::getGroupName()
{ return DREAM3D::FilterGroups::StatisticsFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindArrayHistogram::getSubGroupName()
{ return DREAM3D::FilterSubGroups::EnsembleStatsFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindArrayHistogram::getHumanLabel()
{ return "Find Array Histogram"; }

