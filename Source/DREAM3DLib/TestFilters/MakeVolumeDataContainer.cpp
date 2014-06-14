/* ============================================================================
* Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
* Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
*                           FA8650-10-D-5210
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "MakeVolumeDataContainer.h"


#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/TSL/AngFields.h"
#include "EbsdLib/TSL/AngReader.h"

#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"
#include "DREAM3DLib/DataArrays/StringDataArray.hpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MakeVolumeDataContainer::MakeVolumeDataContainer() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellEnsembleAttributeMatrixName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_PhaseNameArrayName("Phase"),
  m_MaterialNameArrayName(DREAM3D::EnsembleData::MaterialName),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_LatticeConstantsArrayName(DREAM3D::EnsembleData::LatticeConstants),
  m_FeatureIds(NULL),
  m_CellPhases(NULL),
  m_CellEulerAngles(NULL),
  m_CrystalStructures(NULL),
  m_LatticeConstants(NULL)
{

  setupFilterParameters();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MakeVolumeDataContainer::~MakeVolumeDataContainer()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MakeVolumeDataContainer::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", false));
  /*##*/parameters.push_back(FilterParameter::New("FeatureIds", "FeatureIdsArrayName", FilterParameterWidgetType::StringWidget, getFeatureIdsArrayName(), false, ""));
  /*##*/parameters.push_back(FilterParameter::New("Cell Euler Angles", "CellEulerAnglesArrayName", FilterParameterWidgetType::StringWidget, getCellEulerAnglesArrayName(), false, ""));
  /*##*/parameters.push_back(FilterParameter::New("Cell Phases", "CellPhasesArrayName", FilterParameterWidgetType::StringWidget, getCellPhasesArrayName(), true, ""));
  /*##*/parameters.push_back(FilterParameter::New("Crystal Structures", "CrystalStructuresArrayName", FilterParameterWidgetType::StringWidget, getCrystalStructuresArrayName(), true, ""));
  /*##*/parameters.push_back(FilterParameter::New("LatticeConstants", "LatticeConstantsArrayName", FilterParameterWidgetType::StringWidget, getLatticeConstantsArrayName(), true, ""));
  setFilterParameters(parameters);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MakeVolumeDataContainer::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MakeVolumeDataContainer::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  /*[]*/writer->writeValue("LatticeConstantsArrayName", getLatticeConstantsArrayName() );
  /*[]*/writer->writeValue("CrystalStructuresArrayName", getCrystalStructuresArrayName() );
  /*[]*/writer->writeValue("CellPhasesArrayName", getCellPhasesArrayName() );
  /*[]*/writer->writeValue("CellEulerAnglesArrayName", getCellEulerAnglesArrayName() );
  /*[]*/writer->writeValue("FeatureIdsArrayName", getFeatureIdsArrayName() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MakeVolumeDataContainer::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->createNonPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName());
  if (getErrorCondition() < 0) { return; }
  QVector<size_t> tDims(3, 0);
  AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Cell);
  if (getErrorCondition() < 0) { return; }
  tDims.resize(1);
  tDims[0] = 0;
  AttributeMatrix::Pointer cellEnsembleAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::CellEnsemble);
  if(getErrorCondition() < 0) { return; }

  QVector<size_t> dims(1, 1);
  m_FeatureIdsPtr = cellAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this,  m_FeatureIdsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
////==>MIKE_GROEBER_FIX tempPath.update(DATACONTAINER_NAME, ATTRIBUTEMATRIX_NAME, getFeatureIdsArrayName() );
////==>MIKE_GROEBER_FIX m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  AngReader reader;
  AngFields angfeatures;
  QVector<QString> names = angfeatures.getFilterFeatures<QVector<QString> > ();
  dims.resize(1);
  dims[0] = 1;
  for (size_t i = 0; i < names.size(); ++i)
  {
    if (reader.getPointerType(names[i]) == Ebsd::Int32)
    {
      cellAttrMat->createAndAddAttributeArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, names[i], 0, dims);
    }
    else if (reader.getPointerType(names[i]) == Ebsd::Float)
    {
      cellAttrMat->createAndAddAttributeArray<DataArray<float>, AbstractFilter, float>(this, names[i], 0, dims);
    }
  }



  m->setResolution(0.1f, 0.2f, 0.3f);
  m->setOrigin(100.3f, 987.234f, 0.0f);


  QVector<size_t> dim(1, 3);
  m_CellEulerAnglesPtr = cellAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this,  m_CellEulerAnglesArrayName, 0, dim); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
////==>MIKE_GROEBER_FIX tempPath.update(DATACONTAINER_NAME, ATTRIBUTEMATRIX_NAME, getCellEulerAnglesArrayName() );
////==>MIKE_GROEBER_FIX m_CellEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0, dim); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dim[0] = 1;
  m_CellPhasesPtr = cellAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this,  m_CellPhasesArrayName, 0, dim); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
////==>MIKE_GROEBER_FIX tempPath.update(DATACONTAINER_NAME, ATTRIBUTEMATRIX_NAME, getCellPhasesArrayName() );
////==>MIKE_GROEBER_FIX m_CellPhasesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this,  tempPath, 0, dim); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = cellEnsembleAttrMat->createNonPrereqArray<DataArray<uint32_t>, AbstractFilter, uint32_t>(this,  m_CrystalStructuresArrayName, Ebsd::CrystalStructure::UnknownCrystalStructure, dim); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
////==>MIKE_GROEBER_FIX tempPath.update(DATACONTAINER_NAME, ATTRIBUTEMATRIX_NAME, getCrystalStructuresArrayName() );
////==>MIKE_GROEBER_FIX m_CrystalStructuresPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(this,  tempPath, Ebsd::CrystalStructure::UnknownCrystalStructure, dim); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dim[0] = 6;
  m_LatticeConstantsPtr = cellEnsembleAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this,  m_LatticeConstantsArrayName, 0.0, dim); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
////==>MIKE_GROEBER_FIX tempPath.update(DATACONTAINER_NAME, ATTRIBUTEMATRIX_NAME, getLatticeConstantsArrayName() );
////==>MIKE_GROEBER_FIX m_LatticeConstantsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0.0, dim); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_LatticeConstantsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_LatticeConstants = m_LatticeConstantsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  StringDataArray::Pointer materialNames = StringDataArray::CreateArray(cellEnsembleAttrMat->getNumTuples(), DREAM3D::EnsembleData::MaterialName);
  cellEnsembleAttrMat->addAttributeArray( DREAM3D::EnsembleData::MaterialName, materialNames);

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MakeVolumeDataContainer::preflight()
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
void MakeVolumeDataContainer::execute()
{
  int err = 0;
  setErrorCondition(err);

  // Run the data check to get references to all of our data arrays initialized to the values stored in memory
  dataCheck();
  if (getErrorCondition() < 0) { return; }


  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");

}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer MakeVolumeDataContainer::newFilterInstance(bool copyFilterParameters)
{
  MakeVolumeDataContainer::Pointer filter = MakeVolumeDataContainer::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}
