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

#include "ErodeDilateMask.h"


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"


#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ErodeDilateMask::ErodeDilateMask() :
  AbstractFilter(),
  m_Direction(0),
  m_NumIterations(1),
  m_XDirOn(true),
  m_YDirOn(true),
  m_ZDirOn(true),
  m_MaskArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::GoodVoxels),
  m_MaskArrayName(DREAM3D::CellData::GoodVoxels),
  m_Mask(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ErodeDilateMask::~ErodeDilateMask()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErodeDilateMask::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Cell Mask Array", "MaskArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getMaskArrayPath(), false));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Operation");
    parameter->setPropertyName("Direction");
    parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    QVector<QString> choices;
    choices.push_back("Dilate");
    choices.push_back("Erode");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }
  parameters.push_back(FilterParameter::New("Number of Iterations", "NumIterations", FilterParameterWidgetType::IntWidget, getNumIterations(), false));
  parameters.push_back(FilterParameter::New("X Direction", "XDirOn", FilterParameterWidgetType::BooleanWidget, getXDirOn(), false));
  parameters.push_back(FilterParameter::New("Y Direction", "YDirOn", FilterParameterWidgetType::BooleanWidget, getYDirOn(), false));
  parameters.push_back(FilterParameter::New("Z Direction", "ZDirOn", FilterParameterWidgetType::BooleanWidget, getZDirOn(), false));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErodeDilateMask::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setMaskArrayPath(reader->readDataArrayPath("MaskArrayPath", getMaskArrayPath() ) );
  setDirection( reader->readValue("Direction", getDirection()) );
  setNumIterations( reader->readValue("NumIterations", getNumIterations()) );
  setXDirOn(reader->readValue("XDirOn", getXDirOn()) );
  setYDirOn(reader->readValue("YDirOn", getYDirOn()) );
  setZDirOn(reader->readValue("ZDirOn", getZDirOn()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ErodeDilateMask::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(MaskArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(Direction)
  DREAM3D_FILTER_WRITE_PARAMETER(NumIterations)
  DREAM3D_FILTER_WRITE_PARAMETER(XDirOn)
  DREAM3D_FILTER_WRITE_PARAMETER(YDirOn)
  DREAM3D_FILTER_WRITE_PARAMETER(ZDirOn)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErodeDilateMask::dataCheck()
{
  setErrorCondition(0);

  QVector<size_t> dims(1, 1);
  m_MaskPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getMaskArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_MaskPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Mask = m_MaskPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErodeDilateMask::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);

  /* *** THIS FILTER NEEDS TO BE CHECKED *** */
  setErrorCondition(0xABABABAB);
  QString ss = QObject::tr("Filter is NOT updated for IGeometry Redesign. A Programmer needs to check this filter. Please report this to the DREAM3D developers.");
  notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  /* *** THIS FILTER NEEDS TO BE CHECKED *** */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErodeDilateMask::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_MaskArrayPath.getDataContainerName());
  int64_t totalPoints = m_MaskPtr.lock()->getNumberOfTuples();

  BoolArrayType::Pointer maskCopyPtr = BoolArrayType::CreateArray(totalPoints, "MaskCopy");
  m_MaskCopy = maskCopyPtr->getPointer(0);

  size_t udims[3] = {0, 0, 0};
  /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->getDimensions(udims);
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

  int good = 1;
  size_t count = 0;
  int kstride, jstride;
  int neighpoint;

  int neighpoints[6];
  neighpoints[0] = static_cast<int>(-dims[0] * dims[1]);
  neighpoints[1] = static_cast<int>(-dims[0]);
  neighpoints[2] = static_cast<int>(-1);
  neighpoints[3] = static_cast<int>(1);
  neighpoints[4] = static_cast<int>(dims[0]);
  neighpoints[5] = static_cast<int>(dims[0] * dims[1]);

  for (int iteration = 0; iteration < m_NumIterations; iteration++)
  {
    for (int k = 0; k < dims[2]; k++)
    {
      kstride = static_cast<int>( dims[0] * dims[1] * k );
      for (int j = 0; j < dims[1]; j++)
      {
        jstride = static_cast<int>( dims[0] * j );
        for (int i = 0; i < dims[0]; i++)
        {
          count = kstride + jstride + i;

          m_MaskCopy[count] = m_Mask[count];
          if (m_Mask[count] == false)
          {
            for (int l = 0; l < 6; l++)
            {
              good = 1;
              neighpoint = static_cast<int>( count + neighpoints[l] );
              if (l == 0 && (k == 0 || m_ZDirOn == false)) { good = 0; }
              else if (l == 5 && (k == (dims[2] - 1) || m_ZDirOn == false)) { good = 0; }
              else if (l == 1 && (j == 0 || m_YDirOn == false)) { good = 0; }
              else if (l == 4 && (j == (dims[1] - 1) || m_YDirOn == false)) { good = 0; }
              else if (l == 2 && (i == 0 || m_XDirOn == false)) { good = 0; }
              else if (l == 3 && (i == (dims[0] - 1) || m_XDirOn == false)) { good = 0; }
              if (good == 1)
              {
                if (m_Direction == 0 && m_Mask[neighpoint] == true)
                {
                  m_MaskCopy[count] = true;
                }
                if (m_Direction == 1 && m_Mask[neighpoint] == true)
                {
                  m_MaskCopy[neighpoint] = false;
                }
              }
            }
          }
        }
      }
    }
    for (int64_t j = 0; j < totalPoints; j++)
    {
      m_Mask[j] = m_MaskCopy[j];
    }
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ErodeDilateMask::newFilterInstance(bool copyFilterParameters)
{
  ErodeDilateMask::Pointer filter = ErodeDilateMask::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ErodeDilateMask::getCompiledLibraryName()
{ return Processing::ProcessingBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ErodeDilateMask::getGroupName()
{ return DREAM3D::FilterGroups::ProcessingFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ErodeDilateMask::getHumanLabel()
{ return "Erode/Dilate Mask"; }

