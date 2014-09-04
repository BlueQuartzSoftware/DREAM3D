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

#include "IdentifyMicroTextureRegions.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Math/GeometryMath.h"
#include "DREAM3DLib/Math/OrientationMath.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1




#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IdentifyMicroTextureRegions::IdentifyMicroTextureRegions() :
  AbstractFilter(),
  m_NewCellFeatureAttributeMatrixName(DREAM3D::Defaults::NewCellFeatureAttributeMatrixName),
  m_CAxisTolerance(1.0f),
  m_MinMTRSize(1.0f),
  m_MinVolFrac(1.0f),
  m_CAxisLocationsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::CAxisLocation),
  m_CAxisLocationsArrayName(DREAM3D::CellData::CAxisLocation),
  m_MTRIdsArrayName(DREAM3D::CellData::ParentIds),
  m_ActiveArrayName(DREAM3D::FeatureData::Active),
  m_CAxisLocations(NULL),
  m_Active(NULL),
  m_MTRIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IdentifyMicroTextureRegions::~IdentifyMicroTextureRegions()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifyMicroTextureRegions::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("C-Axis Alignment Tolerance", "CAxisTolerance", FilterParameterWidgetType::DoubleWidget, getCAxisTolerance(), false, "Degrees"));
  parameters.push_back(FilterParameter::New("Minimum MicroTextured Region Size (Diameter)", "MinMTRSize", FilterParameterWidgetType::DoubleWidget, getMinMTRSize(), false));
  parameters.push_back(FilterParameter::New("Minimum Volume Fraction in MTR", "MinVolFrac", FilterParameterWidgetType::DoubleWidget, getMinVolFrac(), false));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("CAxisLocations", "CAxisLocationsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCAxisLocationsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("MTR Ids", "MTRIdsArrayName", FilterParameterWidgetType::StringWidget, getMTRIdsArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("New Cell Feature Attribute Matrix Name", "NewCellFeatureAttributeMatrixName", FilterParameterWidgetType::StringWidget, getNewCellFeatureAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("Active", "ActiveArrayName", FilterParameterWidgetType::StringWidget, getActiveArrayName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifyMicroTextureRegions::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setActiveArrayName(reader->readString("ActiveArrayName", getActiveArrayName() ) );
  setNewCellFeatureAttributeMatrixName(reader->readString("NewCellFeatureAttributeMatrixName", getNewCellFeatureAttributeMatrixName() ) );
  setMTRIdsArrayName(reader->readString("MTRIdsArrayName", getMTRIdsArrayName() ) );
  setCAxisLocationsArrayPath(reader->readDataArrayPath("CAxisLocationsArrayPath", getCAxisLocationsArrayPath() ) );
  setCAxisTolerance( reader->readValue("CAxisTolerance", getCAxisTolerance()) );
  setMinMTRSize( reader->readValue("MinMTRSize", getMinMTRSize()) );
  setMinVolFrac( reader->readValue("MinVolFrac", getMinVolFrac()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int IdentifyMicroTextureRegions::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(ActiveArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(MTRIdsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(CAxisLocationsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CAxisTolerance)
  DREAM3D_FILTER_WRITE_PARAMETER(MinMTRSize)
  DREAM3D_FILTER_WRITE_PARAMETER(MinVolFrac)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifyMicroTextureRegions::updateFeatureInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifyMicroTextureRegions::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, m_CAxisLocationsArrayPath.getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  QVector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer newCellFeatureAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getNewCellFeatureAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::CellFeature);
  if(getErrorCondition() < 0) { return; }

  QVector<size_t> dims(1, 3);
  // Cell Data
  m_CAxisLocationsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, m_CAxisLocationsArrayPath, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CAxisLocationsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CAxisLocations = m_CAxisLocationsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  tempPath.update(m_CAxisLocationsArrayPath.getDataContainerName(), m_CAxisLocationsArrayPath.getAttributeMatrixName(), getMTRIdsArrayName() );
  m_MTRIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_MTRIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_MTRIds = m_MTRIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // New Feature Data
  tempPath.update(m_CAxisLocationsArrayPath.getDataContainerName(), getNewCellFeatureAttributeMatrixName(), getActiveArrayName() );
  m_ActivePtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, true, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifyMicroTextureRegions::preflight()
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
void IdentifyMicroTextureRegions::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  // Convert user defined tolerance to radians.
  caxisTolerance = m_CAxisTolerance * DREAM3D::Constants::k_Pi / 180.0f;
  numThetaBins = int(360.0/m_CAxisTolerance);
  numPhiBins = int(90.0/m_CAxisTolerance);

  // Tell the user we are starting the filter
  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), "Starting");
  getCAxisBins();

  QVector<size_t> tDims(1, 1);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getCAxisLocationsArrayPath().getDataContainerName());
  m->getAttributeMatrix(getNewCellFeatureAttributeMatrixName())->resizeAttributeArrays(tDims);
  //need to update pointers after resize, buut do not need to run full data check because pointers are still valid
  updateFeatureInstancePointers();

  size_t totalPoints = m_MTRIdsPtr.lock()->getNumberOfTuples();
  size_t totalFeatures = m_ActivePtr.lock()->getNumberOfTuples();
  if (totalFeatures < 2)
  {
//    setErrorCondition(-87000);
//    notifyErrorMessage(getHumanLabel(), "The number of Grouped Features was 0 or 1 which means no grouped features were detected. Is a grouping value set to high?", getErrorCondition());
//    return;
  }

  m_RandomizeParentIds = false;
  if (true == m_RandomizeParentIds)
  {
    // Generate all the numbers up front
    const int rangeMin = 1;
    const int rangeMax = totalFeatures - 1;
    typedef boost::uniform_int<int> NumberDistribution;
    typedef boost::mt19937 RandomNumberGenerator;
    typedef boost::variate_generator < RandomNumberGenerator&,
            NumberDistribution > Generator;

    NumberDistribution distribution(rangeMin, rangeMax);
    RandomNumberGenerator generator;
    Generator numberGenerator(generator, distribution);
    generator.seed(static_cast<boost::uint32_t>( QDateTime::currentMSecsSinceEpoch() )); // seed with the current time

    DataArray<int32_t>::Pointer rndNumbers = DataArray<int32_t>::CreateArray(totalFeatures, "New ParentIds");
    int32_t* pid = rndNumbers->getPointer(0);
    pid[0] = 0;
    QSet<int32_t> parentIdSet;
    parentIdSet.insert(0);
    for(int i = 1; i < totalFeatures; ++i)
    {
      pid[i] = i; //numberGenerator();
      parentIdSet.insert(pid[i]);
    }

    int r;
    size_t temp;
    //--- Shuffle elements by randomly exchanging each with one other.
    for (int i = 1; i < totalFeatures; i++)
    {
      r = numberGenerator(); // Random remaining position.
      if (r >= totalFeatures)
      {
        continue;
      }
      temp = pid[i];
      pid[i] = pid[r];
      pid[r] = temp;
    }

    // Now adjust all the Feature Id values for each Voxel
    for(size_t i = 0; i < totalPoints; ++i)
    {
      m_MTRIds[i] = pid[ m_MTRIds[i] ];
    }
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "IdentifyMicroTextureRegions Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifyMicroTextureRegions::getCAxisBins()
{
  size_t totalPoints = m_CAxisLocationsPtr.lock()->getNumberOfTuples();
  cAxisBins.resize(totalPoints, -1);
  float theta, phi;
  size_t thetaBin, phiBin, cAxisBin;
  for(size_t i = 0; i< totalPoints; i++)
  {
    theta = DREAM3D::Constants::k_180OverPi * atan2f(m_CAxisLocations[3*i+1], m_CAxisLocations[3*i]);
    if(theta < 0) theta += 360.0;
    phi = DREAM3D::Constants::k_180OverPi * asinf(m_CAxisLocations[3*i+2]);
    thetaBin = size_t(theta/m_CAxisTolerance);
    phiBin = size_t(phi/m_CAxisTolerance);
    if(thetaBin >= numThetaBins) thetaBin = numThetaBins-1;
    if(phiBin >= numPhiBins) phiBin = numPhiBins-1;
    cAxisBin = (phiBin*numThetaBins) + thetaBin;
    cAxisBins[i] = cAxisBin;
    m_MTRIds[i] = cAxisBin;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer IdentifyMicroTextureRegions::newFilterInstance(bool copyFilterParameters)
{
  IdentifyMicroTextureRegions::Pointer filter = IdentifyMicroTextureRegions::New();
  if(true == copyFilterParameters)
  {
    filter->setFilterParameters(getFilterParameters() );
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString IdentifyMicroTextureRegions::getCompiledLibraryName()
{ return Reconstruction::ReconstructionBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString IdentifyMicroTextureRegions::getGroupName()
{ return DREAM3D::FilterGroups::ReconstructionFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString IdentifyMicroTextureRegions::getSubGroupName()
{return DREAM3D::FilterSubGroups::GroupingFilters;}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString IdentifyMicroTextureRegions::getHumanLabel()
{ return "Identify MicroTexture Patches (C-Axis Misalignment)"; }

