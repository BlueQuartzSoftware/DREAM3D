/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2015 Dr. Joseph C. Tucker (UES, Inc.)
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

#include "RenumberFeaturesBinnedOrientations.h"

#include <QtCore/QMap>

#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/Utilities/DREAM3DRandom.h"
#include "DREAM3DLib/Common/Texture.hpp"

#include "OrientationLib/OrientationOps/OrientationOps.h"
#include "OrientationLib/OrientationOps/CubicOps.h"
#include "OrientationLib/OrientationOps/HexagonalOps.h"
#include "OrientationLib/OrientationOps/HexagonalOps.cpp"
#include "OrientationLib/OrientationOps/OrthoRhombicOps.h"
	
using namespace DREAM3D;
using namespace Detail;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RenumberFeaturesBinnedOrientations::RenumberFeaturesBinnedOrientations() :
  AbstractFilter(),
  m_CellFeatureAttributeMatrixPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, ""),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_ParentIdsArrayName(DREAM3D::FeatureData::ParentIds),
  m_ParentIds(NULL),
  m_FeatureEulerAnglesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::EulerAngles),
  m_FeatureEulerAnglesArrayName(DREAM3D::FeatureData::EulerAngles),
  m_FeatureEulerAngles(NULL),
  m_FeaturePhasesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_FeaturePhases(NULL),
  m_CrystalStructuresArrayPath(Defaults::StatsGenerator, Defaults::CellEnsembleAttributeMatrixName, EnsembleData::CrystalStructures),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CrystalStructures(NULL),
  m_BinWidth(5)
{
  m_OrientationOps = OrientationOps::getOrientationOpsQVector();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RenumberFeaturesBinnedOrientations::~RenumberFeaturesBinnedOrientations()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenumberFeaturesBinnedOrientations::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Bin Width", "BinWidth", FilterParameterWidgetType::IntWidget, getBinWidth(), false, "Degrees"));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Cell Feature Attribute Matrix", "CellFeatureAttributeMatrixPath", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getCellFeatureAttributeMatrixPath(), true));
  parameters.push_back(FilterParameter::New("FeatureIds", "FeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeatureIdsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Feature Euler Angles", "FeatureEulerAnglesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeatureEulerAnglesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Feature Phases", "FeaturePhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeaturePhasesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Crystal Structure", "CrystalStructuresArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCrystalStructuresArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Feature ParentIds", "ParentIdsArrayName", FilterParameterWidgetType::StringWidget, getParentIdsArrayName(), true, ""));
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void RenumberFeaturesBinnedOrientations::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellFeatureAttributeMatrixPath(reader->readDataArrayPath("CellFeatureAttributeMatrixPath", getCellFeatureAttributeMatrixPath() ) );
  setBinWidth(reader->readValue("BinWidth", getBinWidth() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setParentIdsArrayName(reader->readString("ParentIdsArrayName", getParentIdsArrayName() ) );
  setFeatureEulerAnglesArrayPath(reader->readDataArrayPath("FeatureEulerAnglesArrayPath", getFeatureEulerAnglesArrayPath() ) );
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RenumberFeaturesBinnedOrientations::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(CellFeatureAttributeMatrixPath)
  DREAM3D_FILTER_WRITE_PARAMETER(BinWidth)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(ParentIdsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureEulerAnglesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeaturePhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenumberFeaturesBinnedOrientations::dataCheck()
{
  DataArrayPath tempPath;
  int err = getErrorCondition();
  if(getErrorCondition() < 0) { return; }
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, m_FeatureIdsArrayPath.getDataContainerName(), false);
  if(getErrorCondition() < 0 || m == NULL) { return; }
  AttributeMatrix::Pointer AttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixPath().getAttributeMatrixName(), -301);
  if(getErrorCondition() < 0 || AttrMat == NULL) { return; }

  QVector<size_t> dims(1, 1);
  // Required Cell Data
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Required Feature Data
  dims[0] = 3;
  m_FeatureEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getFeatureEulerAnglesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  
  dims[0] = 1;
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {
    m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Required Ensemble Data
  dims[0] = 1;
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Created Feature Data
  tempPath.update(getCellFeatureAttributeMatrixPath().getDataContainerName(), getCellFeatureAttributeMatrixPath().getAttributeMatrixName(), getParentIdsArrayName() );
  m_ParentIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, -1, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ParentIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_ParentIds = m_ParentIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenumberFeaturesBinnedOrientations::preflight()
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
void RenumberFeaturesBinnedOrientations::execute()
{
  int err = 0;
  setErrorCondition(err);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* CellDataContainer = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getCellFeatureAttributeMatrixPath().getDataContainerName());

  if(NULL == CellDataContainer || getErrorCondition() < 0)
  {
    return;
  }

  // This just sanity checks to make sure there were existing features before the renumbering
  AttributeMatrix::Pointer cellFeatureAttrMat = CellDataContainer->getAttributeMatrix(getCellFeatureAttributeMatrixPath().getAttributeMatrixName());
  size_t totalFeatures = cellFeatureAttrMat->getNumTuples();
  if (0 == totalFeatures)
  {
    setErrorCondition(-600);
    notifyErrorMessage(getHumanLabel(), "The number of features is Zero and should be greater than Zero", getErrorCondition());
    return;
  }

  float ea1 = 0.0f, ea2 = 0.0f, ea3 = 0.0f, r1 = 0.0f, r2 = 0.0f, r3 = 0.0f;
  std::vector<int> binid;
  binid.resize(totalFeatures);
  float dim[3];
  float bins[3];
  float step[3];
  float binFactor = 0.0f;
  // hardcoded for hexagonal for now
  // defaulted at 5 degree bins
  dim[0] = HexDim1InitValue;
  dim[1] = HexDim2InitValue;
  dim[2] = HexDim3InitValue;

  binFactor = float(m_BinWidth) * 0.2f;

  step[0] = HexDim1StepValue * binFactor;
  step[1] = HexDim2StepValue * binFactor;
  step[2] = HexDim3StepValue * binFactor;
  bins[0] = 36.0f / binFactor;
  bins[1] = 36.0f / binFactor;
  bins[2] = 12.0f / binFactor;
  for (size_t i = 0; i < totalFeatures; i++)
  {
	binid[i] = 0;
  }

  // now bin orientation space
  for (size_t i = 1; i < totalFeatures; i++)
  {
	ea1 = m_FeatureEulerAngles[3 * i];
	ea2 = m_FeatureEulerAngles[3 * i + 1];
	ea3 = m_FeatureEulerAngles[3 * i + 2];
	OrientationMath::EulertoRod(ea1, ea2, ea3, r1, r2, r3);
	OrientationMath::RodtoHomochoric(r1, r2, r3);
	// had to reproduce the below function since it is protected in OrientationOps
	binid[i] = calcODFBin(dim, bins, step, r1, r2, r3);
	m_ParentIds[i] = binid[i];
  }

  notifyStatusMessage(getHumanLabel(), "Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RenumberFeaturesBinnedOrientations::newFilterInstance(bool copyFilterParameters)
{
  RenumberFeaturesBinnedOrientations::Pointer filter = RenumberFeaturesBinnedOrientations::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RenumberFeaturesBinnedOrientations::getCompiledLibraryName()
{ return Processing::ProcessingBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RenumberFeaturesBinnedOrientations::getGroupName()
{ return DREAM3D::FilterGroups::ProcessingFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RenumberFeaturesBinnedOrientations::getHumanLabel()
{ return "Renumber Features (Binned Orientations)"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RenumberFeaturesBinnedOrientations::calcODFBin(float dim[3], float bins[3], float step[3], float r1, float r2, float r3)
{
  int g1euler1bin;
  int g1euler2bin;
  int g1euler3bin;
  int g1odfbin;
  g1euler1bin = int((r1 + dim[0]) / step[0]);
  g1euler2bin = int((r2 + dim[1]) / step[1]);
  g1euler3bin = int((r3 + dim[2]) / step[2]);
  if(g1euler1bin >= bins[0])
  {
    g1euler1bin = static_cast<int>( bins[0] - 1 );
  }
  if(g1euler2bin >= bins[1])
  {
    g1euler2bin = static_cast<int>( bins[1] - 1 );
  }
  if(g1euler3bin >= bins[2])
  {
    g1euler3bin = static_cast<int>( bins[2] - 1 );
  }
  if(g1euler1bin < 0)
  {
    g1euler1bin = 0;
  }
  if(g1euler2bin < 0)
  {
    g1euler2bin = 0;
  }
  if(g1euler3bin < 0)
  {
    g1euler3bin = 0;
  }
  g1odfbin = static_cast<int>( (g1euler3bin * bins[0] * bins[1]) + (g1euler2bin * bins[0]) + (g1euler1bin) );
  return g1odfbin;
}
