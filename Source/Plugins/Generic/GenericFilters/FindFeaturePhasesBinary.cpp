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

#include "FindFeaturePhasesBinary.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "Generic/GenericConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFeaturePhasesBinary::FindFeaturePhasesBinary() :
  AbstractFilter(),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, ""),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_GoodVoxelsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::GoodVoxels),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellEnsembleAttributeMatrixName),
  m_FeatureIds(NULL),
  m_GoodVoxels(NULL)
{

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFeaturePhasesBinary::~FindFeaturePhasesBinary()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeaturePhasesBinary::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Cell Feature Ids", "FeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeatureIdsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Good Voxels", "GoodVoxelsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getGoodVoxelsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Cell Feature Attribute Matrix", "CellFeatureAttributeMatrixName", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getCellFeatureAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Feature Phases", "FeaturePhasesArrayName", FilterParameterWidgetType::StringWidget, getFeaturePhasesArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("Cell Ensemble Attribute Matrix Name", "CellEnsembleAttributeMatrixName", FilterParameterWidgetType::StringWidget, getCellEnsembleAttributeMatrixName(), true, ""));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeaturePhasesBinary::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellFeatureAttributeMatrixName(reader->readDataArrayPath("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName()));
  setFeaturePhasesArrayName(reader->readString("FeaturePhasesArrayName", getFeaturePhasesArrayName() ) );
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setCellEnsembleAttributeMatrixName(reader->readString("CellEnsembleAttributeMatrixName", getCellEnsembleAttributeMatrixName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindFeaturePhasesBinary::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(CellFeatureAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(FeaturePhasesArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(GoodVoxelsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CellEnsembleAttributeMatrixName)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeaturePhasesBinary::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  QVector<DataArrayPath> dataArrayPaths;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

  QVector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getFeatureIdsArrayPath()); }

  m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getGoodVoxelsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_GoodVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getGoodVoxelsArrayPath()); }

  getDataContainerArray()->validateNumberOfTuples(this, dataArrayPaths);

  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getFeaturePhasesArrayName() );
  m_FeaturePhasesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() < 0) { return; }

  QVector<size_t> tDims(1, 2);
  getDataContainerArray()->getDataContainer(getCellFeatureAttributeMatrixName().getDataContainerName())->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::CellEnsemble);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeaturePhasesBinary::preflight()
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
void FindFeaturePhasesBinary::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();

  int32_t gnum = 0;
  for (size_t i = 0; i < totalPoints; i++)
  {
    gnum = m_FeatureIds[i];
    if (m_GoodVoxels[i] == true)
    {
        m_FeaturePhases[gnum] = 1;
    }
    else
    {
        m_FeaturePhases[gnum] = 0;
    }
    //m_FeaturePhases[gnum] = m_CellPhases[i];
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindFeaturePhasesBinary::newFilterInstance(bool copyFilterParameters)
{
  FindFeaturePhasesBinary::Pointer filter = FindFeaturePhasesBinary::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeaturePhasesBinary::getCompiledLibraryName()
{ return Generic::GenericBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeaturePhasesBinary::getGroupName()
{ return DREAM3D::FilterGroups::GenericFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeaturePhasesBinary::getSubGroupName()
{ return DREAM3D::FilterSubGroups::MiscFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeaturePhasesBinary::getHumanLabel()
{ return "Find Feature Phases Binary"; }

