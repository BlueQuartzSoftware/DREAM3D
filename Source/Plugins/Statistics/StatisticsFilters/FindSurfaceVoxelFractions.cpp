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

#include "FindSurfaceVoxelFractions.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSurfaceVoxelFractions::FindSurfaceVoxelFractions() :
  AbstractFilter(),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, ""),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_SurfaceVoxelsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::SurfaceVoxels),
  m_SurfaceVoxelFractionsArrayName(DREAM3D::FeatureData::SurfaceVoxelFractions),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_SurfaceVoxelsArrayName(DREAM3D::CellData::SurfaceVoxels),
  m_SurfaceVoxels(NULL),
  m_SurfaceVoxelFractions(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSurfaceVoxelFractions::~FindSurfaceVoxelFractions()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceVoxelFractions::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
  parameters.push_back(FilterParameter::New("FeatureIds", "FeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  parameters.push_back(FilterParameter::New("SurfaceVoxels", "SurfaceVoxelsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  parameters.push_back(FilterParameter::New("Cell Feature Attribute Matrix Name", "CellFeatureAttributeMatrixName", FilterParameterWidgetType::AttributeMatrixSelectionWidget, "DataArrayPath", true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
  parameters.push_back(FilterParameter::New("SurfaceVoxelFractions", "SurfaceVoxelFractionsArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  setFilterParameters(parameters);
}


// -----------------------------------------------------------------------------
void FindSurfaceVoxelFractions::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellFeatureAttributeMatrixName(reader->readDataArrayPath("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName()));
  setSurfaceVoxelFractionsArrayName(reader->readString("SurfaceVoxelFractionsArrayName", getSurfaceVoxelFractionsArrayName() ) );
  setSurfaceVoxelsArrayPath(reader->readDataArrayPath("SurfaceVoxelsArrayPath", getSurfaceVoxelsArrayPath() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindSurfaceVoxelFractions::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName());
  writer->writeValue("SurfaceVoxelFractionsArrayName", getSurfaceVoxelFractionsArrayName() );
  writer->writeValue("SurfaceVoxelsArrayPath", getSurfaceVoxelsArrayPath() );
  writer->writeValue("FeatureIdsArrayPath", getFeatureIdsArrayPath() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceVoxelFractions::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  QVector<size_t> dims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_SurfaceVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter>(this, getSurfaceVoxelsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceVoxels = m_SurfaceVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getSurfaceVoxelFractionsArrayName() );
  m_SurfaceVoxelFractionsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceVoxelFractionsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceVoxelFractions = m_SurfaceVoxelFractionsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceVoxelFractions::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceVoxelFractions::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  find_surface_voxel_fractions();

  notifyStatusMessage(getHumanLabel(), "FindSurfaceVoxelFractions Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceVoxelFractions::find_surface_voxel_fractions()
{
  int64_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  size_t numfeatures = m_SurfaceVoxelFractionsPtr.lock()->getNumberOfTuples();

  DataArray<float>::Pointer m_SurfVoxCounts = DataArray<float>::CreateArray(numfeatures, "SurfVoxCounts");
  float* surfvoxcounts = m_SurfVoxCounts->getPointer(0);
  DataArray<float>::Pointer m_VoxCounts = DataArray<float>::CreateArray(numfeatures, "VoxCounts");
  float* voxcounts = m_VoxCounts->getPointer(0);

  // Initialize every element to 0.0
  for (size_t i = 0; i < numfeatures * 1; i++)
  {
    surfvoxcounts[i] = 0.0f;
    voxcounts[i] = 0.0f;
  }
  for (int64_t j = 0; j < totalPoints; j++)
  {
    int gnum = m_FeatureIds[j];
    voxcounts[gnum]++;
    if(m_SurfaceVoxels[j] > 0) { surfvoxcounts[gnum]++; }
  }
  for (size_t i = 1; i < numfeatures; i++)
  {
    m_SurfaceVoxelFractions[i] = surfvoxcounts[i] / voxcounts[i];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindSurfaceVoxelFractions::newFilterInstance(bool copyFilterParameters)
{
  FindSurfaceVoxelFractions::Pointer filter = FindSurfaceVoxelFractions::New();
  if(true == copyFilterParameters)
  {
    filter->setFilterParameters(getFilterParameters() );
    
    //Loop over each Filter Parameter that is registered to the filter either through this class or a parent class
    // and copy the value from the current instance of the object into the "new" instance that was just created
    QVector<FilterParameter::Pointer> options = getFilterParameters(); // Get the current set of filter parameters
    for (QVector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
    {
      FilterParameter* parameter = (*iter).get();
      if (parameter->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0 )
      {
        continue; // Skip this type of filter parameter as it has nothing to do with anything in the filter.
      }
      // Get the property from the current instance of the filter
      QVariant var = property(parameter->getPropertyName().toLatin1().constData());
      bool ok = filter->setProperty(parameter->getPropertyName().toLatin1().constData(), var);
      if(false == ok)
      {
        QString ss = QString("Error occurred transferring the Filter Parameter '%1' in Filter '%2' to the filter instance. The pipeline may run but the underlying filter will NOT be using the values from the GUI."
                             " Please report this issue to the developers of this filter.").arg(parameter->getPropertyName()).arg(filter->getHumanLabel());
        Q_ASSERT_X(ok, __FILE__, ss.toLatin1().constData());
      }

      if(parameter->isConditional() == true)
      {
        QVariant cond = property(parameter->getConditionalProperty().toLatin1().constData() );
        ok = filter->setProperty(parameter->getConditionalProperty().toLatin1().constData(), cond);
        if(false == ok)
        {
          QString ss = QString("%1::newFilterInstance()\nError occurred transferring the Filter Parameter '%2' in Filter '%3' to the filter instance. "
                              " The filter parameter has a conditional property '%4'. The transfer of this property from the old filter to the new filter failed."
                              " Please report this issue to the developers of this filter.").arg(filter->getNameOfClass())
                              .arg(parameter->getPropertyName())
                              .arg(filter->getHumanLabel())
                              .arg(parameter->getConditionalProperty());
          Q_ASSERT_X(ok, __FILE__, ss.toLatin1().constData());
        }
      }
    }
  }
  return filter;
}
