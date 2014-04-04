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

#include "FindCellQuats.h"

#include <sstream>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/Math/OrientationMath.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindCellQuats::FindCellQuats() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellEnsembleAttributeMatrixName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
/*[]*/m_CellEulerAnglesArrayPath(DREAM3D::Defaults::SomePath),
/*[]*/m_CellPhasesArrayPath(DREAM3D::Defaults::SomePath),
/*[]*/m_CrystalStructuresArrayPath(DREAM3D::Defaults::SomePath),
  m_QuatsArrayName(DREAM3D::CellData::Quats),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CellPhases(NULL),
  m_Quats(NULL),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_CellEulerAngles(NULL),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CrystalStructures(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsVector();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindCellQuats::~FindCellQuats()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindCellQuats::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
/*[]*/parameters.push_back(FilterParameter::New("CellEulerAngles", "CellEulerAnglesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
/*[]*/parameters.push_back(FilterParameter::New("CellPhases", "CellPhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
/*[]*/parameters.push_back(FilterParameter::New("CrystalStructures", "CrystalStructuresArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
/*##*/parameters.push_back(FilterParameter::New("Quats", "QuatsArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void FindCellQuats::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
/*[]*/setQuatsArrayName(reader->readString("QuatsArrayName", getQuatsArrayName() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath() ) );
  setCellEulerAnglesArrayPath(reader->readDataArrayPath("CellEulerAnglesArrayPath", getCellEulerAnglesArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindCellQuats::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
/*[]*/writer->writeValue("QuatsArrayName", getQuatsArrayName() );
  writer->writeValue("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() );
  writer->writeValue("CellPhasesArrayPath", getCellPhasesArrayPath() );
  writer->writeValue("CellEulerAnglesArrayPath", getCellEulerAnglesArrayPath() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindCellQuats::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  AttributeMatrix::Pointer cellEnsembleAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -302);
  if(getErrorCondition() < 0) { return; }

  QVector<size_t> dims(1, 3);
  m_CellEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCellEulerAnglesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 4;
  m_QuatsPtr = cellAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this, m_QuatsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
////==>MIKE_GROEBER_FIX tempPath.update(DATACONTAINER_NAME, ATTRIBUTEMATRIX_NAME, getQuatsArrayName() );
////==>MIKE_GROEBER_FIX m_QuatsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_QuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Quats = m_QuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindCellQuats::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindCellQuats::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();

  QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);
  QuatF qr;
  int phase = -1;
  for (int i = 0; i < totalPoints; i++)
  {
    phase = m_CellPhases[i];
    OrientationMath::EulertoQuat( m_CellEulerAngles[3 * i], m_CellEulerAngles[3 * i + 1], m_CellEulerAngles[3 * i + 2], qr);
    QuaternionMathF::UnitQuaternion(qr);
    if (m_CrystalStructures[phase] == Ebsd::CrystalStructure::UnknownCrystalStructure)
    {
      QuaternionMathF::Identity(qr);
    }
    QuaternionMathF::Copy(qr, quats[i]);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindCellQuats::newFilterInstance(bool copyFilterParameters)
{
  FindCellQuats::Pointer filter = FindCellQuats::New();
  if(true == copyFilterParameters)
  {
    filter->setQuatsArrayName(getQuatsArrayName());
    filter->setCrystalStructuresArrayPath(getCrystalStructuresArrayPath());
    filter->setCellPhasesArrayPath(getCellPhasesArrayPath());
    filter->setCellEulerAnglesArrayPath(getCellEulerAnglesArrayPath());
  }
  return filter;
}
