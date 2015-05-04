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

#include "FindAvgCAxes.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Math/GeometryMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "OrientationLib/Math/OrientationMath.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindAvgCAxes::FindAvgCAxes() :
  AbstractFilter(),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, ""),
  m_QuatsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Quats),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_AvgCAxesArrayName(DREAM3D::FeatureData::AvgCAxes),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_QuatsArrayName(DREAM3D::CellData::Quats),
  m_Quats(NULL),
  m_AvgCAxes(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsQVector();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindAvgCAxes::~FindAvgCAxes()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgCAxes::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Quats", "QuatsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getQuatsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("FeatureIds", "FeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeatureIdsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Cell Feature Attribute Matrix Name", "CellFeatureAttributeMatrixName", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getCellFeatureAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("AvgCAxes", "AvgCAxesArrayName", FilterParameterWidgetType::StringWidget, getAvgCAxesArrayName(), true, ""));
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void FindAvgCAxes::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellFeatureAttributeMatrixName(reader->readDataArrayPath("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName()));
  setAvgCAxesArrayName(reader->readString("AvgCAxesArrayName", getAvgCAxesArrayName() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setQuatsArrayPath(reader->readDataArrayPath("QuatsArrayPath", getQuatsArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindAvgCAxes::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(CellFeatureAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(AvgCAxesArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(QuatsArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgCAxes::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  QVector<size_t> dims(1, 4);
  m_QuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getQuatsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_QuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Quats = m_QuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 3;
  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getAvgCAxesArrayName() );
  m_AvgCAxesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgCAxesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgCAxes = m_AvgCAxesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAvgCAxes::preflight()
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
void FindAvgCAxes::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  int64_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  size_t totalFeatures = m_AvgCAxesPtr.lock()->getNumberOfTuples();

  //int phase;
  QuatF q1;
  QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);
  float g1[3][3];
  float g1t[3][3];
  float caxis[3] = {0, 0, 1};
  float c1[3];

  std::vector<int> counter(totalFeatures, 0);

  for (size_t i = 1; i < totalFeatures; i++)
  {
    m_AvgCAxes[3 * i] = 0.0;
    m_AvgCAxes[3 * i + 1] = 0.0;
    m_AvgCAxes[3 * i + 2] = 0.0;
  }
  float curCAxis[3];
  size_t index;
  float w;
  for(int i = 0; i < totalPoints; i++)
  {
    if(m_FeatureIds[i] > 0)
    {
      index = 3 * m_FeatureIds[i];
      QuaternionMathF::Copy(quats[i], q1);

      OrientationMath::QuattoMat(q1, g1);
      //transpose the g matricies so when caxis is multiplied by it
      //it will give the sample direction that the caxis is along
      MatrixMath::Transpose3x3(g1, g1t);
      MatrixMath::Multiply3x3with3x1(g1t, caxis, c1);
      //normalize so that the magnitude is 1
      MatrixMath::Normalize3x1(c1);
      curCAxis[0] = m_AvgCAxes[index] / counter[m_FeatureIds[i]];
      curCAxis[1] = m_AvgCAxes[index + 1] / counter[m_FeatureIds[i]];
      curCAxis[2] = m_AvgCAxes[index + 2] / counter[m_FeatureIds[i]];
      MatrixMath::Normalize3x1(curCAxis);
      w = GeometryMath::CosThetaBetweenVectors(c1, curCAxis);
      if (w < 0) { MatrixMath::Multiply3x1withConstant(c1, -1); }
      counter[m_FeatureIds[i]]++;
      m_AvgCAxes[index] += c1[0];
      m_AvgCAxes[index + 1] += c1[1];
      m_AvgCAxes[index + 2] += c1[2];
    }
  }
  for (size_t i = 1; i < totalFeatures; i++)
  {
    if(counter[i] == 0)
    {
      m_AvgCAxes[3 * i] = 0;
      m_AvgCAxes[3 * i + 1] = 0;
      m_AvgCAxes[3 * i + 2] = 1;
    }
    else
    {
      m_AvgCAxes[3 * i] /= counter[i];
      m_AvgCAxes[3 * i + 1] /= counter[i];
      m_AvgCAxes[3 * i + 2] /= counter[i];
    }
  }

  notifyStatusMessage(getHumanLabel(), "Completed");
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindAvgCAxes::newFilterInstance(bool copyFilterParameters)
{
  FindAvgCAxes::Pointer filter = FindAvgCAxes::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindAvgCAxes::getCompiledLibraryName()
{ return OrientationAnalysisConstants::OrientationAnalysisBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindAvgCAxes::getGroupName()
{ return DREAM3D::FilterGroups::StatisticsFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindAvgCAxes::getSubGroupName()
{ return DREAM3D::FilterSubGroups::CrystallographicFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindAvgCAxes::getHumanLabel()
{ return "Find Average C-Axis Orientations"; }

