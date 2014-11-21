/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2014 Dr. Joseph C. Tucker (UES, Inc.)
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

#include "FindSaltykovSizes.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

#include "Statistics/StatisticsConstants.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSaltykovSizes::FindSaltykovSizes() :
  AbstractFilter(),
  m_NumberOfBins(10),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, ""),
  m_EquivalentDiametersArrayPath(DREAM3D::FeatureData::EquivalentDiameters),
  m_EquivalentDiameters(NULL),
  m_SaltykovEquivalentDiametersArrayName(DREAM3D::FeatureData::SaltykovEquivalentDiameters),
  m_SaltykovEquivalentDiameters(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSaltykovSizes::~FindSaltykovSizes()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSaltykovSizes::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Number Of Bins", "NumberOfBins", FilterParameterWidgetType::IntWidget, getNumberOfBins(), false, ""));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Cell Feature Attribute Matrix Name", "CellFeatureAttributeMatrixName", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getCellFeatureAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("Equivalent Diameters", "EquivalentDiametersArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getEquivalentDiametersArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Saltykov Equivalent Diameters", "SaltykovEquivalentDiametersArrayName", FilterParameterWidgetType::StringWidget, getSaltykovEquivalentDiametersArrayName(), true, ""));
  setFilterParameters(parameters);
}


// -----------------------------------------------------------------------------
void FindSaltykovSizes::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNumberOfBins(reader->readValue("AlphaGlobPhase", getNumberOfBins()) );
  setCellFeatureAttributeMatrixName(reader->readDataArrayPath("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName()));
  setEquivalentDiametersArrayPath(reader->readDataArrayPath("EquivalentDiametersArrayPath", getEquivalentDiametersArrayPath() ) );
  setSaltykovEquivalentDiametersArrayName(reader->readString("SaltykovEquivalentDiametersArrayName", getSaltykovEquivalentDiametersArrayName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindSaltykovSizes::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(NumberOfBins)
  DREAM3D_FILTER_WRITE_PARAMETER(CellFeatureAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(EquivalentDiametersArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(SaltykovEquivalentDiametersArrayName)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSaltykovSizes::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  QVector<size_t> dims(1, 1);
  m_EquivalentDiametersPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getEquivalentDiametersArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_EquivalentDiametersPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_EquivalentDiameters = m_EquivalentDiametersPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getSaltykovEquivalentDiametersArrayName() );
  m_SaltykovEquivalentDiametersPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SaltykovEquivalentDiametersPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SaltykovEquivalentDiameters = m_SaltykovEquivalentDiametersPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSaltykovSizes::preflight()
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
void FindSaltykovSizes::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  find_saltykov_sizes();

  notifyStatusMessage(getHumanLabel(), "Find Feature Saltykov Sizes Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSaltykovSizes::find_saltykov_sizes()
{
  DREAM3D_RANDOMNG_NEW()

  float minEqDia = FLT_MAX;
  float maxEqDia = 0.0f;
  float binLength = 0.0f;
  float temp1 = 0.0f, temp2 = 0.0f, temp3 = 0.0f;
  int binIndex = 0;
  int saltykovLength = 0;
  int saltykovIndex = 1;
  double random = 0.0;
  int attempts = 0;
  int binToAddTo = 0;

 // int* binLengths = NULL;
 // binLengths = new int[m_NumberOfBins];

//  int* saltykovBinLengths = NULL;
 // saltykovBinLengths = new int[m_NumberOfBins];

  size_t numfeatures = m_EquivalentDiametersPtr.lock()->getNumberOfTuples();
  int numberofbins = m_NumberOfBins;

  std::vector<int> binLengths;
  binLengths.resize(m_NumberOfBins);
  std::vector<int> saltykovBinLengths;
  saltykovBinLengths.resize(m_NumberOfBins);
  std::vector<float> saltykovEquivalentDiameters;
  saltykovEquivalentDiameters.resize(numfeatures);

  for (size_t i = 1; i < numfeatures; i++)
  {
	// find the min size
	if (m_EquivalentDiameters[i] < minEqDia) { minEqDia = m_EquivalentDiameters[i]; }
	// find the max size
	if (m_EquivalentDiameters[i] > maxEqDia) { maxEqDia = m_EquivalentDiameters[i]; }
  }

  float currentmiminum = minEqDia;
  float nextminimum = maxEqDia;

  while (saltykovLength != numfeatures-1)
  {
	// find the bin length
	binLength = (maxEqDia - minEqDia) / numberofbins;

	for (int i = 0; i < numberofbins; i++) { binLengths[i] = 0; }

	for (size_t i = 1; i < numfeatures; i++)
	{
	  temp1 = m_EquivalentDiameters[i] - minEqDia;
	  temp2 = 1.0f / binLength;
	  temp3 = temp1 * temp2;
	  binIndex = int (temp3);
	  if (binIndex == numberofbins) { binIndex--; };
	  binLengths[binIndex]++;
	}
  
	saltykovLength = 0;
	for (int i = numberofbins-1; i >= 0; i--)
	{
	  saltykovBinLengths[i] = do_saltykov(binLengths, maxEqDia, i);
	  saltykovLength += saltykovBinLengths[i];
	}

	if (saltykovLength != numfeatures-1)
	{
	  int difference = saltykovLength - (numfeatures-1);
	  if (attempts >= 10  && difference < 0)
	  {
		for (int i = 0; i > difference; i--)
		{
		  binToAddTo = rg.genrand_int32() % numberofbins + 1;
		  saltykovBinLengths[binToAddTo]++;
		  saltykovLength++;
		  int stop = 0;
		}
	  }
	  else
	  {
		attempts++;
		numberofbins -= difference;
		binLengths.resize(numberofbins);
		saltykovBinLengths.resize(numberofbins);
	  }
	}
  }

  for (int i = 0; i < numberofbins; i++)
  {
	for (int j = 0; j < saltykovBinLengths[i]; j++)
	{
      if (saltykovIndex == numfeatures) { break; }
	  unsigned long number = rg.genrand_int32();
	  unsigned long number2 = 0xffffffff;
	  random = (double(number) / double(number2));	
	  saltykovEquivalentDiameters[saltykovIndex] = (float(random) * binLength) + (minEqDia + (i * binLength));
	  saltykovIndex++;
	}
  }

  std::sort(saltykovEquivalentDiameters.begin(), saltykovEquivalentDiameters.end(), std::less<float>());

  for (size_t i = 1; i < numfeatures; i++)
  {
	for (size_t j = 1; j < numfeatures; j++)
	{
	  if (m_EquivalentDiameters[j] == currentmiminum)
	  {
		m_SaltykovEquivalentDiameters[j] = saltykovEquivalentDiameters[i];
	  }
	  if (m_EquivalentDiameters[j] > currentmiminum && m_EquivalentDiameters[j] < nextminimum)
	  {
		nextminimum = m_EquivalentDiameters[j];
	  }
	}
	currentmiminum = nextminimum;
	nextminimum = maxEqDia;
	int stop = 0;
  }

 // delete [] binLengths;
//  delete [] saltykovBinLengths;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindSaltykovSizes::do_saltykov(std::vector<int> nA, float Dmax, int k)
{
  double saltyCoefs[12] = {1.6461,-0.4561,-0.1162,-0.0415,-0.0173,-0.0079,-0.0038,-0.0018,-0.0010,-0.0003,-0.0002,-0.0002};
  double Dk = Dmax / k;
  int i = 0;
  double temp1 = 0.0, temp2 = 0.0, temp3 = 0.0;
  int temp4 = 0;

  while ((i < 12) && ((k+1-i) > 0))
  {
	temp1 = double (nA[k-i]);
	temp2 += saltyCoefs[i] * temp1;
	int shownA = nA[k-i];
	  int stop2 = 0;
	i++;
  }
  temp2 /= Dk;
  temp3 = ceil(temp2);
  temp4 = int(temp3);
  int stop = 0;
  return temp4;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindSaltykovSizes::newFilterInstance(bool copyFilterParameters)
{
  FindSaltykovSizes::Pointer filter = FindSaltykovSizes::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSaltykovSizes::getCompiledLibraryName()
{ return Statistics::StatisticsBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSaltykovSizes::getGroupName()
{ return DREAM3D::FilterGroups::StatisticsFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSaltykovSizes::getSubGroupName()
{ return DREAM3D::FilterSubGroups::MorphologicalFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSaltykovSizes::getHumanLabel()
{ return "Find Feature Saltykov Sizes"; }

