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
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_SelectedArrayPath(""),
  m_Direction(0),
  m_NumIterations(1),
  m_XDirOn(true),
  m_YDirOn(true),
  m_ZDirOn(true),
  m_MaskArrayName(""),
  m_Mask(NULL),
/*[]*/m_MaskArrayPath(DREAM3D::Defaults::SomePath)
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
  parameters.push_back(FilterParameter::New("Cell Mask Array", "SelectedArrayPath", FilterParameterWidgetType::DataArraySelectionWidget,"QString", false));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Operation");
    parameter->setPropertyName("Direction");
    parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    parameter->setValueType("unsigned int");
    QVector<QString> choices;
    choices.push_back("Dilate");
    choices.push_back("Erode");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }
  parameters.push_back(FilterParameter::New("Number of Iterations", "NumIterations", FilterParameterWidgetType::IntWidget,"int", false));
  parameters.push_back(FilterParameter::New("X Direction", "XDirOn", FilterParameterWidgetType::BooleanWidget,"bool", false));
  parameters.push_back(FilterParameter::New("Y Direction", "YDirOn", FilterParameterWidgetType::BooleanWidget,"bool", false));
  parameters.push_back(FilterParameter::New("Z Direction", "ZDirOn", FilterParameterWidgetType::BooleanWidget,"bool", false));
/*[]*/parameters.push_back(FilterParameter::New("Mask", "MaskArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErodeDilateMask::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setMaskArrayPath(reader->readDataArrayPath("MaskArrayPath", getMaskArrayPath() ) );
  setSelectedArrayPath( reader->readString( "SelectedArrayPath", getSelectedArrayPath() ) );
  setDirection( reader->readValue("Direction", getDirection()) );
  setNumIterations( reader->readValue("NumIterations", getNumIterations()) );
  setXDirOn(reader->readValue("X Direction", getXDirOn()) );
  setYDirOn(reader->readValue("Y Direction", getYDirOn()) );
  setZDirOn(reader->readValue("Z Direction", getZDirOn()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ErodeDilateMask::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("MaskArrayPath", getMaskArrayPath() );
  writer->writeValue("SelectedArrayPath", getSelectedArrayPath() );
  writer->writeValue("Direction", getDirection() );
  writer->writeValue("NumIterations", getNumIterations() );
  writer->writeValue("X Direction", getXDirOn() );
  writer->writeValue("Y Direction", getYDirOn() );
  writer->writeValue("Z Direction", getZDirOn() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErodeDilateMask::dataCheck()
{
  setErrorCondition(0);

  if (m_SelectedArrayPath.isEmpty() == true)
  {
    setErrorCondition(-11001);
    QString ss = QObject::tr("The complete path to the Attribute Array can not be empty. Please set an appropriate path.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else
  {

    QString dcName;
    QString amName;
    QString daName;

    QStringList tokens = m_SelectedArrayPath.split(DREAM3D::PathSep);
    // We should end up with 3 Tokens
    if(tokens.size() != 3)
    {
      setErrorCondition(-11002);
      QString ss = QObject::tr("The path to the Attribute Array is malformed. Each part should be separated by a '|' character.");
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
    else
    {
      m_DataContainerName = tokens.at(0);
      m_CellAttributeMatrixName = tokens.at(1);
      m_MaskArrayName = tokens.at(2);

      DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(m_DataContainerName);
      if(NULL == dc.get())
      {
        setErrorCondition(-11003);
        QString ss = QObject::tr("The DataContainer '%1' was not found in the DataContainerArray").arg(dcName);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }

      AttributeMatrix::Pointer cellAttrMat = dc->getAttributeMatrix(m_CellAttributeMatrixName);
       if(NULL == cellAttrMat.get())
      {
        setErrorCondition(-11004);
        QString ss = QObject::tr("The AttributeMatrix '%1' was not found in the DataContainer '%2'").arg(amName).arg(dcName);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }

       QVector<size_t> dims(1, 1);
////====>REMOVE THIS         m_MaskPtr = cellAttrMat->getPrereqArray<DataArray<bool>, AbstractFilter>(this, m_MaskArrayName, -301, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_MaskPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getMaskArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
       if( NULL != m_MaskPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
       { m_Mask = m_MaskPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErodeDilateMask::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErodeDilateMask::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();

  BoolArrayType::Pointer maskCopyPtr = BoolArrayType::CreateArray(totalPoints, "MaskCopy");
  m_MaskCopy = maskCopyPtr->getPointer(0);

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

