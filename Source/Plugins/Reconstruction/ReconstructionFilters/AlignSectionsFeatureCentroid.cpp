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

#include "AlignSectionsFeatureCentroid.h"

#include <QtCore/QtDebug>
#include <fstream>
#include <sstream>

#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"

#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1

using namespace std;



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSectionsFeatureCentroid::AlignSectionsFeatureCentroid() :
  AlignSections(),
  m_ReferenceSlice(0),
  m_UseReferenceSlice(false),
  m_GoodVoxelsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::GoodVoxels),
  m_GoodVoxelsArrayName(DREAM3D::CellData::GoodVoxels),
  m_GoodVoxels(NULL)
{
  //only setting up the child parameters because the parent constructor has already been called
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSectionsFeatureCentroid::~AlignSectionsFeatureCentroid()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsFeatureCentroid::setupFilterParameters()
{
  //getting the current parameters that were set by the parent and adding to it before resetting it
  FilterParameterVector parameters = getFilterParameters();
  parameters.push_front(FilterParameter::New("Reference Slice", "ReferenceSlice", FilterParameterWidgetType::IntWidget,"int", false));
  FilterParameter::Pointer param = parameters.front();
  param->setConditional(true);
  param->setConditionalProperty("UseReferenceSlice");
  param->setConditionalLabel("Use Reference Slice");
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
  parameters.push_back(FilterParameter::New("GoodVoxels", "GoodVoxelsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsFeatureCentroid::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  AlignSections::readFilterParameters(reader, index);
  reader->openFilterGroup(this, index);
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath() ) );
  setUseReferenceSlice( reader->readValue("UseReferenceSlice", false) );
  setReferenceSlice( reader->readValue("ReferenceSlice", getReferenceSlice()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AlignSectionsFeatureCentroid::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  AlignSections::writeFilterParameters(writer, index);
  writer->openFilterGroup(this, index);
  writer->writeValue("GoodVoxelsArrayPath", getGoodVoxelsArrayPath() );
  writer->writeValue("UseReferenceSlice", getUseReferenceSlice() );
  writer->writeValue("ReferenceSlice", getReferenceSlice() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsFeatureCentroid::dataCheck()
{
  setErrorCondition(0);

  //Set the DataContainerName and AttributematrixName for the Parent Class (AlignSections) to Use.
  setDataContainerName(m_GoodVoxelsArrayPath.getDataContainerName());
  setCellAttributeMatrixName(m_GoodVoxelsArrayPath.getAttributeMatrixName());

  if(true == getWriteAlignmentShifts() && getAlignmentShiftFileName().isEmpty() == true)
  {
    QString ss = QObject::tr("The Alignment Shift file name must be set before executing this filter.");
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  QVector<size_t> dims(1, 1);
  m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getGoodVoxelsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_GoodVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsFeatureCentroid::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsFeatureCentroid::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  AlignSections::execute();

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Aligning Sections Complete");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsFeatureCentroid::find_shifts(QVector<int>& xshifts, QVector<int>& yshifts)
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  ofstream outFile;
  if (getWriteAlignmentShifts() == true)
  {
    outFile.open(getAlignmentShiftFileName().toLatin1().data());
  }
  size_t udims[3] = {0, 0, 0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  int newxshift = 0;
  int newyshift = 0;
  int count = 0;
  int slice = 0;
  int64_t point;
  //  int xspot, yspot;
  float xRes = m->getXRes();
  float yRes = m->getYRes();
  QVector<float> xCentroid(dims[2], 0.0);
  QVector<float> yCentroid(dims[2], 0.0);

  for (DimType iter = 0; iter < dims[2]; iter++)
  {
    count = 0;
    xCentroid[iter] = 0;
    yCentroid[iter] = 0;

    QString ss = QObject::tr("Aligning Sections - Determining Shifts - %1 Percent Complete").arg(((float)iter / dims[2]) * 100);
    //  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    slice = static_cast<int>( (dims[2] - 1) - iter );
    for (DimType l = 0; l < dims[1]; l++)
    {
      for (DimType n = 0; n < dims[0]; n++)
      {
        point = static_cast<int>( ((slice) * dims[0] * dims[1]) + (l * dims[0]) + n );
        if(m_GoodVoxels[point] == true)
        {
          xCentroid[iter] = xCentroid[iter] + (float(n) * xRes);
          yCentroid[iter] = yCentroid[iter] + (float(l) * yRes);
          count++;
        }
      }
    }
    xCentroid[iter] = xCentroid[iter] / float(count);
    yCentroid[iter] = yCentroid[iter] / float(count);
  }
  //int refSlice;
  for (DimType iter = 1; iter < dims[2]; iter++)
  {
    slice = static_cast<int>( (dims[2] - 1) - iter );
    if(m_UseReferenceSlice == true)
    {
      xshifts[iter] = int((xCentroid[iter] - xCentroid[m_ReferenceSlice]) / xRes);
      yshifts[iter] = int((yCentroid[iter] - yCentroid[m_ReferenceSlice]) / yRes);
    }
    else
    {
      xshifts[iter] = xshifts[iter - 1] + int((xCentroid[iter] - xCentroid[iter - 1]) / xRes);
      yshifts[iter] = yshifts[iter - 1] + int((yCentroid[iter] - yCentroid[iter - 1]) / yRes);
    }
    if (getWriteAlignmentShifts() == true)
    {
      outFile << slice << "	" << slice + 1 << "	" << newxshift << "	" << newyshift << "	" << xshifts[iter] << "	" << yshifts[iter] << " " << xCentroid[iter] << " " << yCentroid[iter] << endl;
    }
  }
  if (getWriteAlignmentShifts() == true)
  {
    outFile.close();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer AlignSectionsFeatureCentroid::newFilterInstance(bool copyFilterParameters)
{
  AlignSectionsFeatureCentroid::Pointer filter = AlignSectionsFeatureCentroid::New();
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
