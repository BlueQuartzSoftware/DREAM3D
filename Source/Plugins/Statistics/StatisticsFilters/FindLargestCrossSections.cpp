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

#include "FindLargestCrossSections.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindLargestCrossSections::FindLargestCrossSections() :
  AbstractFilter(),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, ""),
  m_Plane(0),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_LargestCrossSectionsArrayName(DREAM3D::FeatureData::LargestCrossSections),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_LargestCrossSections(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindLargestCrossSections::~FindLargestCrossSections()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLargestCrossSections::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Plane of Interest");
    parameter->setPropertyName("Plane");
    parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    //parameter->setValueType("unsigned int");
    QVector<QString> choices;
    choices.push_back("XY");
    choices.push_back("XZ");
    choices.push_back("YZ");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("FeatureIds", "FeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeatureIdsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Cell Feature Attribute Matrix Name", "CellFeatureAttributeMatrixName", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getCellFeatureAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("LargestCrossSections", "LargestCrossSectionsArrayName", FilterParameterWidgetType::StringWidget, getLargestCrossSectionsArrayName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void FindLargestCrossSections::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellFeatureAttributeMatrixName(reader->readDataArrayPath("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName()));
  setLargestCrossSectionsArrayName(reader->readString("LargestCrossSectionsArrayName", getLargestCrossSectionsArrayName() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setPlane( reader->readValue("Plane", getPlane()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindLargestCrossSections::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName());
  writer->writeValue("LargestCrossSectionsArrayName", getLargestCrossSectionsArrayName() );
  writer->writeValue("FeatureIdsArrayPath", getFeatureIdsArrayPath() );
  writer->writeValue("Plane", getPlane());
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLargestCrossSections::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  QVector<size_t> dims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getLargestCrossSectionsArrayName() );
  m_LargestCrossSectionsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_LargestCrossSectionsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_LargestCrossSections = m_LargestCrossSectionsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLargestCrossSections::preflight()
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
void FindLargestCrossSections::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_FeatureIdsArrayPath.getDataContainerName());

  if(m->getXPoints() > 1 && m->getYPoints() > 1 && m->getZPoints() > 1) { find_crosssections(); }
  else
  {
    setErrorCondition(-999);
    notifyErrorMessage(getHumanLabel(), "The volume is not 3D and cannot be run through this filter", -999);
    return;
  }
  notifyStatusMessage(getHumanLabel(), "FindLargestCrossSections Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLargestCrossSections::find_crosssections()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_FeatureIdsArrayPath.getDataContainerName());

  size_t numfeatures = m_LargestCrossSectionsPtr.lock()->getNumberOfTuples();

  DataArray<double>::Pointer m_FeatureCounts = DataArray<double>::CreateArray(numfeatures, "FeatureCounts");
  double* featurecounts = m_FeatureCounts->getPointer(0);

  int outPlane, inPlane1, inPlane2;
  float res_scalar, area;
  int stride1, stride2, stride3;
  int istride, jstride, kstride;
  int point, gnum;

  if(m_Plane == 0)
  {
    outPlane = m->getZPoints();
    inPlane1 = m->getXPoints();
    inPlane2 = m->getYPoints();
    res_scalar = m->getXRes() * m->getYRes();
    stride1 = inPlane1 * inPlane2;
    stride2 = 1;
    stride3 = inPlane1;
  }
  if(m_Plane == 1)
  {
    outPlane = m->getYPoints();
    inPlane1 = m->getXPoints();
    inPlane2 = m->getZPoints();
    res_scalar = m->getXRes() * m->getZRes();
    stride1 = inPlane1;
    stride2 = 1;
    stride3 = inPlane1 * inPlane2;
  }
  if(m_Plane == 2)
  {
    outPlane = m->getXPoints();
    inPlane1 = m->getYPoints();
    inPlane2 = m->getZPoints();
    res_scalar = m->getYRes() * m->getZRes();
    stride1 = 1;
    stride2 = inPlane1;
    stride3 = inPlane1 * inPlane2;
  }
  for(int i = 0; i < outPlane; i++)
  {
    for (size_t g = 0; g < numfeatures * 1; g++)
    {
      featurecounts[g] = 0.0f;
    }
    istride = i * stride1;
    for (int j = 0; j < inPlane1; j++)
    {
      jstride = j * stride2;
      for(int k = 0; k < inPlane2; k++)
      {
        kstride = k * stride3;
        point = istride + jstride + kstride;
        gnum = m_FeatureIds[point];
        featurecounts[gnum]++;
      }
    }
    for (size_t g = 1; g < numfeatures; g++)
    {
      area = featurecounts[g] * res_scalar;
      if(area > m_LargestCrossSections[g]) { m_LargestCrossSections[g] = area; }
    }
  }
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindLargestCrossSections::newFilterInstance(bool copyFilterParameters)
{
  FindLargestCrossSections::Pointer filter = FindLargestCrossSections::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindLargestCrossSections::getCompiledLibraryName()
{ return Statistics::StatisticsBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindLargestCrossSections::getGroupName()
{ return DREAM3D::FilterGroups::StatisticsFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindLargestCrossSections::getSubGroupName()
{ return DREAM3D::FilterSubGroups::MorphologicalFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindLargestCrossSections::getHumanLabel()
{ return "Find Feature Largest Cross-Section Areas"; }

