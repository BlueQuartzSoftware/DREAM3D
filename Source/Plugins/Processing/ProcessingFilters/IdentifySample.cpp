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
#include "IdentifySample.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "Processing/ProcessingConstants.h"
#include "Processing/ProcessingVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IdentifySample::IdentifySample()
: m_FillHoles(false)
, m_GoodVoxelsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Mask)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IdentifySample::~IdentifySample() = default;
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifySample::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SIMPL_NEW_BOOL_FP("Fill Holes in Largest Feature", FillHoles, FilterParameter::Parameter, IdentifySample));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Bool, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Mask", GoodVoxelsArrayPath, FilterParameter::RequiredArray, IdentifySample, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifySample::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFillHoles(reader->readValue("FillHoles", getFillHoles()));
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifySample::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifySample::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getGoodVoxelsArrayPath().getDataContainerName());

  std::vector<size_t> cDims(1, 1);
  m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getGoodVoxelsArrayPath(),
                                                                                                     cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_GoodVoxelsPtr.lock())                                                                      /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifySample::preflight()
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
void IdentifySample::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_GoodVoxelsArrayPath.getDataContainerName());
  int64_t totalPoints = static_cast<int64_t>(m_GoodVoxelsPtr.lock()->getNumberOfTuples());

  SizeVec3Type udims = m->getGeometryAs<ImageGeom>()->getDimensions();

  int64_t dims[3] = {
      static_cast<int64_t>(udims[0]), static_cast<int64_t>(udims[1]), static_cast<int64_t>(udims[2]),
  };

  int64_t neighpoints[6] = {0, 0, 0, 0, 0, 0};
  int64_t xp = dims[0];
  int64_t yp = dims[1];
  int64_t zp = dims[2];

  neighpoints[0] = -(xp * yp);
  neighpoints[1] = -xp;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = xp;
  neighpoints[5] = (xp * yp);
  std::vector<int64_t> currentvlist;
  std::vector<bool> checked(totalPoints, false);
  std::vector<bool> sample(totalPoints, false);
  int64_t biggestBlock = 0;
  size_t count = 0;
  int32_t good = 0;
  int64_t neighbor = 0;
  int64_t column = 0, row = 0, plane = 0;
  int64_t index = 0;

  // In this loop over the data we are finding the biggest contiguous set of GoodVoxels and calling that the 'sample'  All GoodVoxels that do not touch the 'sample'
  // are flipped to be called 'bad' voxels or 'not sample'
  for(int64_t i = 0; i < totalPoints; i++)
  {
    if(!checked[i] && m_GoodVoxels[i])
    {
      currentvlist.push_back(i);
      count = 0;
      while(count < currentvlist.size())
      {
        index = currentvlist[count];
        column = index % xp;
        row = (index / xp) % yp;
        plane = index / (xp * yp);
        for(int32_t j = 0; j < 6; j++)
        {
          good = 1;
          neighbor = index + neighpoints[j];
          if(j == 0 && plane == 0)
          {
            good = 0;
          }
          if(j == 5 && plane == (zp - 1))
          {
            good = 0;
          }
          if(j == 1 && row == 0)
          {
            good = 0;
          }
          if(j == 4 && row == (yp - 1))
          {
            good = 0;
          }
          if(j == 2 && column == 0)
          {
            good = 0;
          }
          if(j == 3 && column == (xp - 1))
          {
            good = 0;
          }
          if(good == 1 && !checked[neighbor] && m_GoodVoxels[neighbor])
          {
            currentvlist.push_back(neighbor);
            checked[neighbor] = true;
          }
        }
        count++;
      }
      if(static_cast<int64_t>(currentvlist.size()) >= biggestBlock)
      {
        biggestBlock = currentvlist.size();
        sample.assign(totalPoints, false);
        for(int64_t j = 0; j < biggestBlock; j++)
        {
          sample[currentvlist[j]] = true;
        }
      }
      currentvlist.clear();
    }
  }
  for(int64_t i = 0; i < totalPoints; i++)
  {
    if(!sample[i] && m_GoodVoxels[i])
    {
      m_GoodVoxels[i] = false;
    }
  }
  sample.clear();
  checked.assign(totalPoints, false);

  // In this loop we are going to 'close' all of the 'holes' inside of the region already identified as the 'sample' if the user chose to do so.
  // This is done by flipping all 'bad' voxel features that do not touch the outside of the sample (i.e. they are fully contained inside of the 'sample'.
  if(m_FillHoles)
  {
    bool touchesBoundary = false;
    for(int64_t i = 0; i < totalPoints; i++)
    {
      if(!checked[i] && !m_GoodVoxels[i])
      {
        currentvlist.push_back(i);
        count = 0;
        touchesBoundary = false;
        while(count < currentvlist.size())
        {
          index = currentvlist[count];
          column = index % xp;
          row = (index / xp) % yp;
          plane = index / (xp * yp);
          if(column == 0 || column == (xp - 1) || row == 0 || row == (yp - 1) || plane == 0 || plane == (zp - 1))
          {
            touchesBoundary = true;
          }
          for(int32_t j = 0; j < 6; j++)
          {
            good = 1;
            neighbor = index + neighpoints[j];
            if(j == 0 && plane == 0)
            {
              good = 0;
            }
            if(j == 5 && plane == (zp - 1))
            {
              good = 0;
            }
            if(j == 1 && row == 0)
            {
              good = 0;
            }
            if(j == 4 && row == (yp - 1))
            {
              good = 0;
            }
            if(j == 2 && column == 0)
            {
              good = 0;
            }
            if(j == 3 && column == (xp - 1))
            {
              good = 0;
            }
            if(good == 1 && !checked[neighbor] && !m_GoodVoxels[neighbor])
            {
              currentvlist.push_back(neighbor);
              checked[neighbor] = true;
            }
          }
          count++;
        }
        if(!touchesBoundary)
        {
          for(size_t j = 0; j < currentvlist.size(); j++)
          {
            m_GoodVoxels[currentvlist[j]] = true;
          }
        }
        currentvlist.clear();
      }
    }
  }
  checked.clear();


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer IdentifySample::newFilterInstance(bool copyFilterParameters) const
{
  IdentifySample::Pointer filter = IdentifySample::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString IdentifySample::getCompiledLibraryName() const
{
  return ProcessingConstants::ProcessingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString IdentifySample::getBrandingString() const
{
  return "Processing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString IdentifySample::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Processing::Version::Major() << "." << Processing::Version::Minor() << "." << Processing::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString IdentifySample::getGroupName() const
{
  return SIMPL::FilterGroups::ProcessingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid IdentifySample::getUuid()
{
  return QUuid("{0e8c0818-a3fb-57d4-a5c8-7cb8ae54a40a}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString IdentifySample::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::CleanupFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString IdentifySample::getHumanLabel() const
{
  return "Isolate Largest Feature (Identify Sample)";
}
