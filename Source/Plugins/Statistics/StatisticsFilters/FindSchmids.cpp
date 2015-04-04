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

#include "FindSchmids.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "OrientationLib/Math/OrientationMath.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSchmids::FindSchmids() :
  AbstractFilter(),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, ""),
  m_FeaturePhasesArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_CrystalStructuresArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_AvgQuatsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::AvgQuats),
  m_SchmidsArrayName(DREAM3D::FeatureData::Schmids),
  m_SlipSystemsArrayName(DREAM3D::FeatureData::SlipSystems),
  m_PolesArrayName(DREAM3D::FeatureData::Poles),
  m_PhisArrayName("Schmid_Phis"),
  m_LambdasArrayName("Schmid_Lambdas"),
  m_StoreAngleComponents(false),
  m_OverrideSystem(false),
  m_Schmids(NULL),
  m_Phis(NULL),
  m_Lambdas(NULL),
  m_Poles(NULL),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_FeaturePhases(NULL),
  m_AvgQuatsArrayName(DREAM3D::FeatureData::AvgQuats),
  m_AvgQuats(NULL),
  m_SlipSystems(NULL),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CrystalStructures(NULL)
{
  m_LoadingDirection.x = 1.0f;
  m_LoadingDirection.y = 1.0f;
  m_LoadingDirection.z = 1.0f;

  m_SlipPlane.x = 0.0f;
  m_SlipPlane.y = 0.0f;
  m_SlipPlane.z = 1.0f;

  m_SlipDirection.x = 1.0f;
  m_SlipDirection.y = 0.0f;
  m_SlipDirection.z = 0.0f;

  m_OrientationOps = OrientationOps::getOrientationOpsQVector();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSchmids::~FindSchmids()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSchmids::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Loading Direction", "LoadingDirection", FilterParameterWidgetType::FloatVec3Widget, getLoadingDirection(), false));
  QStringList linkedProps;
  linkedProps << "PhisArrayName" << "LambdasArrayName";
  parameters.push_back(LinkedBooleanFilterParameter::New("Store Angle Components of Schmid Factor", "StoreAngleComponents", getStoreAngleComponents(), linkedProps, false));
  parameters.push_back(FilterParameter::New("FeaturePhases", "FeaturePhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeaturePhasesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCrystalStructuresArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("AvgQuats", "AvgQuatsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getAvgQuatsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Cell Feature Attribute Matrix Name", "CellFeatureAttributeMatrixName", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getCellFeatureAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Schmids", "SchmidsArrayName", FilterParameterWidgetType::StringWidget, getSchmidsArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("SlipSystems", "SlipSystemsArrayName", FilterParameterWidgetType::StringWidget, getSlipSystemsArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("Poles", "PolesArrayName", FilterParameterWidgetType::StringWidget, getPolesArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("Phis", "PhisArrayName", FilterParameterWidgetType::StringWidget, getPhisArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("Lambdas", "LambdasArrayName", FilterParameterWidgetType::StringWidget, getLambdasArrayName(), true, ""));

  QStringList linkedProps2;
  linkedProps2 << "SlipPlane" << "SlipDirection";
  parameters.push_back(LinkedBooleanFilterParameter::New("Override Default Slip System", "OverrideSystem", getOverrideSystem(), linkedProps2, false));
  parameters.push_back(FilterParameter::New("Slip Plane", "SlipPlane", FilterParameterWidgetType::FloatVec3Widget, getSlipPlane(), false, ""));
  parameters.push_back(FilterParameter::New("Slip Direction", "SlipDirection", FilterParameterWidgetType::FloatVec3Widget, getSlipDirection(), false, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSchmids::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellFeatureAttributeMatrixName(reader->readDataArrayPath("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName()));
  setLambdasArrayName(reader->readString("LambdasArrayName", getLambdasArrayName() ) );
  setPhisArrayName(reader->readString("PhisArrayName", getPhisArrayName() ) );
  setPolesArrayName(reader->readString("PolesArrayName", getPolesArrayName() ) );
  setSlipSystemsArrayName(reader->readString("SlipSystemsArrayName", getSlipSystemsArrayName() ) );
  setSchmidsArrayName(reader->readString("SchmidsArrayName", getSchmidsArrayName() ) );
  setAvgQuatsArrayPath(reader->readDataArrayPath("AvgQuatsArrayPath", getAvgQuatsArrayPath() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() ) );
  setLoadingDirection( reader->readFloatVec3("LoadingDirection", getLoadingDirection() ) );
  setStoreAngleComponents( reader->readValue("StoreAngleComponents", getStoreAngleComponents()) );
  setOverrideSystem( reader->readValue("OverrideSystem", getOverrideSystem()) );
  setSlipPlane( reader->readFloatVec3("SlipPlane", getSlipPlane() ) );
  setSlipDirection( reader->readFloatVec3("SlipDirection", getSlipDirection() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindSchmids::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(CellFeatureAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(LambdasArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(PhisArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(PolesArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(SlipSystemsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(SchmidsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(AvgQuatsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeaturePhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(LoadingDirection)
  DREAM3D_FILTER_WRITE_PARAMETER(StoreAngleComponents)
  DREAM3D_FILTER_WRITE_PARAMETER(OverrideSystem)
  DREAM3D_FILTER_WRITE_PARAMETER(SlipPlane)
  DREAM3D_FILTER_WRITE_PARAMETER(SlipDirection)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSchmids::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  QVector<size_t> dims(1, 1);
  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getSchmidsArrayName() );
  m_SchmidsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SchmidsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Schmids = m_SchmidsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getSlipSystemsArrayName() );
  m_SlipSystemsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SlipSystemsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SlipSystems = m_SlipSystemsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
//typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(), dims)
                           ; /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 3;
  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getPolesArrayName() );
  m_PolesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_PolesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Poles = m_PolesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 4;
  m_AvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgQuatsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(m_StoreAngleComponents == true)
  {
    dims[0] = 1;
    tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getPhisArrayName() );
    m_PhisPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, -301, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_PhisPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_Phis = m_PhisPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getLambdasArrayName() );
    m_LambdasPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, -301, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_LambdasPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_Lambdas = m_LambdasPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  if(m_OverrideSystem)
  {
    //make sure direction lies in plane
    float cosVec = m_SlipPlane.x * m_SlipDirection.x + m_SlipPlane.y * m_SlipDirection.y + m_SlipPlane.z * m_SlipDirection.z;
    if( 0 != cosVec )
    {
      QString ss = QObject::tr("Slip Plane and Slip Direction must be normal");
      notifyErrorMessage(getHumanLabel(), ss, -1001);
      setErrorCondition(-1001);
      return;
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSchmids::preflight()
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
void FindSchmids::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  size_t totalFeatures = m_SchmidsPtr.lock()->getNumberOfTuples();

  int ss = 0;
  QuatF q1;
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  float g[3][3];
  float sampleLoading[3];
  float crystalLoading[3];
  float angleComps[2];
  float schmid = 0;

  sampleLoading[0] = m_LoadingDirection.x;
  sampleLoading[1] = m_LoadingDirection.y;
  sampleLoading[2] = m_LoadingDirection.z;
  MatrixMath::Normalize3x1(sampleLoading);

  float plane[3];
  float direction[3];
  if(m_OverrideSystem)
  {
    plane[0] = m_SlipPlane.x;
    plane[1] = m_SlipPlane.y;
    plane[2] = m_SlipPlane.z;
    MatrixMath::Normalize3x1(plane);

    direction[0] = m_SlipDirection.x;
    direction[1] = m_SlipDirection.y;
    direction[2] = m_SlipDirection.z;
    MatrixMath::Normalize3x1(direction);
  }


  for (size_t i = 1; i < totalFeatures; i++)
  {
    QuaternionMathF::Copy(avgQuats[i], q1);
    OrientationMath::QuattoMat(q1, g);

    MatrixMath::Multiply3x3with3x1(g, sampleLoading, crystalLoading);

    size_t xtal = m_CrystalStructures[m_FeaturePhases[i]];
    if(xtal < Ebsd::CrystalStructure::LaueGroupEnd)
    {
      if(!m_OverrideSystem)
      { m_OrientationOps[xtal]->getSchmidFactorAndSS(crystalLoading, schmid, angleComps, ss); }
      else
      { m_OrientationOps[xtal]->getSchmidFactorAndSS(crystalLoading, plane, direction, schmid, angleComps, ss); }

      m_Schmids[i] = schmid;
      if(m_StoreAngleComponents == true)
      {
        m_Phis[i] = angleComps[0];
        m_Lambdas[i] = angleComps[1];
      }
      m_Poles[3 * i] = int32_t(crystalLoading[0] * 100);
      m_Poles[3 * i + 1] = int32_t(crystalLoading[1] * 100);
      m_Poles[3 * i + 2] = int32_t(crystalLoading[2] * 100);
      m_SlipSystems[i] = ss;
    }
  }

  notifyStatusMessage(getHumanLabel(), "Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindSchmids::newFilterInstance(bool copyFilterParameters)
{
  FindSchmids::Pointer filter = FindSchmids::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSchmids::getCompiledLibraryName()
{ return Statistics::StatisticsBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSchmids::getGroupName()
{ return DREAM3D::FilterGroups::StatisticsFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSchmids::getSubGroupName()
{ return DREAM3D::FilterSubGroups::CrystallographicFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSchmids::getHumanLabel()
{ return "Find Schmid Factors"; }

