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

#include "FindSaltykovSizes.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Math/SIMPLibMath.h"

#include "Statistics/StatisticsConstants.h"
#include "Statistics/StatisticsVersion.h"
#include "SIMPLib/Utilities/SIMPLibRandom.h"

// Include the MOC generated file for this class
#include "moc_FindSaltykovSizes.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSaltykovSizes::FindSaltykovSizes() :
  AbstractFilter(),
  m_EquivalentDiametersArrayPath(SIMPL::FeatureData::EquivalentDiameters),
  m_SaltykovEquivalentDiametersArrayPath(SIMPL::FeatureData::SaltykovEquivalentDiameters),
  m_EquivalentDiameters(NULL),
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

  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(DataArraySelectionFilterParameter::New("Equivalent Diameters", "EquivalentDiametersArrayPath", getEquivalentDiametersArrayPath(), FilterParameter::RequiredArray, req));
  }

  {
    DataArrayCreationFilterParameter::RequirementType req;
    parameters.push_back(DataArrayCreationFilterParameter::New("Saltykov Equivalent Diameters", "SaltykovEquivalentDiametersArrayPath", getSaltykovEquivalentDiametersArrayPath(), FilterParameter::CreatedArray, req));
  }

  setFilterParameters(parameters);
}


// -----------------------------------------------------------------------------
void FindSaltykovSizes::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setEquivalentDiametersArrayPath(reader->readDataArrayPath("EquivalentDiametersArrayPath", getEquivalentDiametersArrayPath() ) );
  setSaltykovEquivalentDiametersArrayPath(reader->readDataArrayPath("SaltykovEquivalentDiametersArrayPath", getSaltykovEquivalentDiametersArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindSaltykovSizes::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(EquivalentDiametersArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(SaltykovEquivalentDiametersArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSaltykovSizes::initialize()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSaltykovSizes::dataCheck()
{
  setErrorCondition(0);

  QVector<size_t> dims(1, 1);
  m_EquivalentDiametersPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getEquivalentDiametersArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_EquivalentDiametersPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_EquivalentDiameters = m_EquivalentDiametersPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_SaltykovEquivalentDiametersPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, getSaltykovEquivalentDiametersArrayPath(), 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
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
  SIMPL_RANDOMNG_NEW()

  const int MaxAttempts = 10;
  const unsigned long RandMaxFloat = 0xffffffff;
  // an initial guess of 10 bins is used
  int numberofbins = 10, numberofbins1 = 0, numberofbins2 = 20;
  // the second guess is 20 bins, after this, a forward difference will ensue

  float minEqDia = std::numeric_limits<float>::max();
  float maxEqDia = 0.0f;
  float binLength = 0.0f;
  int iter = 0;
  int saltykovLength = 0, saltykovLength1 = 0, saltykovLength2 = 0;
  int saltykovIndex = 1;
  int attempts = 0;
  int binToAddTo = 0;
  unsigned long randomLong = 0.0;
  double randomFraction = 0.0;
  int difference = 0;

  size_t numfeatures = m_EquivalentDiametersPtr.lock()->getNumberOfTuples();

  std::vector<int> binLengths;
  binLengths.resize(numberofbins);

  std::vector<int> saltykovBinLengths;
  saltykovBinLengths.resize(numberofbins);

  std::vector<float> equivalentDiameters;
  equivalentDiameters.resize(numfeatures - 1);

  std::vector<float> saltykovEquivalentDiameters;
  saltykovEquivalentDiameters.resize(numfeatures - 1);

  std::vector<float> temp;

  // transfer equivalent diameters to a std::vector
  for (size_t i = 1; i < numfeatures; i++) { equivalentDiameters[i - 1] = m_EquivalentDiameters[i]; }

  std::sort(equivalentDiameters.begin(), equivalentDiameters.end(), std::less<float>());

  // find the min and max
  for (size_t i = 0; i < numfeatures - 1; i++)
  {
    // find the min size
    if (equivalentDiameters[i] < minEqDia) { minEqDia = equivalentDiameters[i]; }
    // find the max size
    if (equivalentDiameters[i] > maxEqDia) { maxEqDia = equivalentDiameters[i]; }
  }

  float currentmiminum = minEqDia;
  float nextminimum = maxEqDia;

  // continue performing Saltkov with changing the number of bins
  // until the number of feature eq dia's to be sampled = the
  // number of features.  We do this so the SaltykovArray length jives
  // with the feature-level level array and then we get to be able to
  // compare each Saltkov eq dia one-for-one against the feature eq dia's
  // so, with that, each Saltykov eq dia is matched in ascending order to
  // the feature eq dia.  But, it is important to note that the Saltykov eq dia
  // is not a direct transformation of the particular eq dia that it is matched
  // up with
  while (saltykovLength != numfeatures - 1)
  {
    // find the bin length
    binLength = maxEqDia / (numberofbins - 1);

    // initialize bin lengths to 0
    for (int i = 0; i < numberofbins; i++) { binLengths[i] = 0; }

    iter = 0;
    for (int i = 0; i < numberofbins; i++)
    {
      temp.resize(0);
      std::copy(
        std::lower_bound(equivalentDiameters.begin(), equivalentDiameters.end(), binLength * i),
        std::upper_bound(equivalentDiameters.begin(), equivalentDiameters.end(), binLength * (i + 1)),
        std::back_inserter(temp)
      );
      binLengths[i] = temp.size();
      iter++;
    }

    std::reverse(binLengths.begin(), binLengths.end());

    // for formal Saltykov we start at the largest bin then unfold our
    // way down to the smallest bin which is why the loop is set up
    // this way
    // saltykovLength is the total number of features to be sampled from
    // the saltykov bins
    saltykovLength = 0;
    for (int i = 1; i <= numberofbins; i++)
    {
      saltykovBinLengths[i - 1] = do_saltykov(binLengths, maxEqDia, i);
      if (saltykovBinLengths[i - 1] < 0) { saltykovBinLengths[i - 1] = 0; }
      saltykovLength += saltykovBinLengths[i - 1];
    }

    std::reverse(saltykovBinLengths.begin(), saltykovBinLengths.end());

    // if the number of features to be sampled from the Saltykov
    // bins (saltykovLength) is not = the number of features
    // then the difference between the two is calculated.  then, either
    // the number of attempts is incremented, the number of bins is increased/decreased
    // based on the "difference" and the bin arrays are resized accordingly.  This
    // will approach us to the correct number of features because the increasing
    // number of bins, increases the number of features eq dia's to be sampled and decreasing
    // the number of bins does the opposite. However, if the number of maximum attempts
    // is reached, hardcoded at 10, then a random Saltykov bin is selected to add one sample to.
    // This is done because it is assumed that after 10 attempts, the solution is oscillating
    // between one less than and one more than feature so, for convienence, only the one less
    // than option triggers this fudge addition to reach our desired number of features
    if (saltykovLength != numfeatures - 1)
    {
      //better minumum formula
      difference = saltykovLength - (numfeatures - 1);
      if (attempts >= MaxAttempts && difference < 0)
      {
        for (int i = 0; i > difference; i--)
        {
          binToAddTo = rg.genrand_int32() % numberofbins + 1;
          if (binToAddTo == numberofbins) { binToAddTo--; };
          // only add to bins that already have values in them
          if (saltykovBinLengths[binToAddTo] < 1)
          {
            saltykovBinLengths[binToAddTo]++;
            saltykovLength = numfeatures - 1;
          }
          else { i++; }
        }
      }
      else
      {
        attempts++;
        if (numberofbins2 == 20 && numberofbins1 == 0)
        {
          numberofbins1 = numberofbins;
          numberofbins = numberofbins2;
          saltykovLength1 = saltykovLength;
        }
        else
        {
          numberofbins2 = numberofbins1;
          numberofbins1 = numberofbins;
          saltykovLength2 = saltykovLength1;
          saltykovLength1 = saltykovLength;
          numberofbins = forward_difference(numfeatures - 1, saltykovLength2, saltykovLength1, numberofbins2, numberofbins1);
        }
        // in case the number of bins is less than 1
        if (numberofbins < 1) { numberofbins = 1; }
        binLengths.resize(numberofbins);
        saltykovBinLengths.resize(numberofbins);
      }
    }
  }

  // only proceed with sampling the Saltykov bins if the Salytkov samples
  // = the number of features, else let the loop cycle again with the
  // new number of bins
  saltykovIndex = 0;
  if (saltykovLength == numfeatures - 1)
  {
    for (int i = 0; i < numberofbins; i++)
    {
      for (int j = 0; j < saltykovBinLengths[i]; j++)
      {
        // generate a random float between the current bin extents
        randomLong = rg.genrand_int32();
        randomFraction = (double(randomLong) / double(RandMaxFloat));
        saltykovEquivalentDiameters[saltykovIndex] = float(randomFraction) * binLength + float(i) * binLength;
        saltykovIndex++;
      }
    }

    // sort the Saltykov eq dia's into ascending order so they can be matched up with their feature eq dia pair
    std::sort(saltykovEquivalentDiameters.begin(), saltykovEquivalentDiameters.end(), std::less<float>());

    // this nested loop matches the Saltykov eq dia's with the feature eq dia's in asecending order
    for (size_t i = 1; i < numfeatures; i++)
    {
      for (size_t j = 1; j < numfeatures; j++)
      {
        if (m_EquivalentDiameters[j] == currentmiminum)
        {
          m_SaltykovEquivalentDiameters[j] = saltykovEquivalentDiameters[i - 1];
        }
        if (m_EquivalentDiameters[j] > currentmiminum && m_EquivalentDiameters[j] < nextminimum)
        {
          nextminimum = m_EquivalentDiameters[j];
        }
      }
      currentmiminum = nextminimum;
      nextminimum = maxEqDia;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindSaltykovSizes::do_saltykov(std::vector<int> nA, float Dmax, int k)
{
  // the below is formal Saltykov method of spheres with the formal variable names and 12 Saltykov coefs.  It was decided
  // that any further contributions from the coefs is negligible
  double saltyCoefs[12] = {1.6461, -0.4561, -0.1162, -0.0415, -0.0173, -0.0079, -0.0038, -0.0018, -0.0010, -0.0003, -0.0002, -0.0002};
  double Dk = Dmax / k;
  int i = 0;
  double temp1 = 0.0, temp2 = 0.0;
  int temp3 = 0;

  while ((i < 12) && ((k - i) > 0))
  {
    temp1 = double (nA[k - i - 1]);
    temp2 += saltyCoefs[i + 1 - 1] * temp1;
    i++;
  }
  temp2 /= Dk;
  temp3 = round_to_nearest_int(temp2);
  return temp3;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindSaltykovSizes::forward_difference(int fx, int f1, int f0, int x1, int x0)
{
  // this is the forward finite difference method
  float temp1 = 0.0f, temp2 = 0.0f;
  int x = 0;
  temp1 = float(fx - f0) / float(f1 - f0);
  temp2 = float(x0) + float(x1 - x0) * temp1;
  x = round_to_nearest_int(temp2);
  return x;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<class T>
int FindSaltykovSizes::round_to_nearest_int(T x)
{
  return int(floor(x + 0.5));
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
{
  return StatisticsConstants::StatisticsBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSaltykovSizes::getBrandingString()
{
  return "Statistics";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSaltykovSizes::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  Statistics::Version::Major() << "." << Statistics::Version::Minor() << "." << Statistics::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSaltykovSizes::getGroupName()
{ return SIMPL::FilterGroups::StatisticsFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSaltykovSizes::getSubGroupName()
{ return SIMPL::FilterSubGroups::MorphologicalFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSaltykovSizes::getHumanLabel()
{ return "Find Feature Saltykov Sizes"; }

