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


#include "FeatureInfoReader.h"

#include <QtCore/QtDebug>
#include <fstream>
#include <sstream>

#include <QtCore/QFileInfo>

#include "DREAM3DLib/DataArrays/DataArray.hpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FeatureInfoReader::FeatureInfoReader() :
  FileReader(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_InputFile(""),
  m_CreateCellLevelArrays(true),
  m_RenumberFeatures(true),
/*[]*/m_FeatureIdsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_FeatureEulerAnglesArrayName(DREAM3D::FeatureData::EulerAngles),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_CellPhases(NULL),
  m_CellEulerAngles(NULL),
  m_FeaturePhases(NULL),
  m_FeatureEulerAngles(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FeatureInfoReader::~FeatureInfoReader()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureInfoReader::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Input Feature Info File", "InputFile", FilterParameterWidgetType::InputFileWidget,"QString", false, "", "*.txt"));
  parameters.push_back(FilterParameter::New("Create Cell Level Arrays", "CreateCellLevelArrays", FilterParameterWidgetType::BooleanWidget,"bool", false));
  parameters.push_back(FilterParameter::New("Renumber Features", "RenumberFeatures", FilterParameterWidgetType::BooleanWidget,"bool", false));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
/*[]*/parameters.push_back(FilterParameter::New("FeatureIds", "FeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
/*##*/parameters.push_back(FilterParameter::New("CellPhases", "CellPhasesArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
/*##*/parameters.push_back(FilterParameter::New("CellEulerAngles", "CellEulerAnglesArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
/*##*/parameters.push_back(FilterParameter::New("FeaturePhases", "FeaturePhasesArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
/*##*/parameters.push_back(FilterParameter::New("FeatureEulerAngles", "FeatureEulerAnglesArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureInfoReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
/*[]*/setFeatureEulerAnglesArrayName(reader->readString("FeatureEulerAnglesArrayName", getFeatureEulerAnglesArrayName() ) );
/*[]*/setFeaturePhasesArrayName(reader->readString("FeaturePhasesArrayName", getFeaturePhasesArrayName() ) );
/*[]*/setCellEulerAnglesArrayName(reader->readString("CellEulerAnglesArrayName", getCellEulerAnglesArrayName() ) );
/*[]*/setCellPhasesArrayName(reader->readString("CellPhasesArrayName", getCellPhasesArrayName() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setInputFile( reader->readString( "InputFile", getInputFile() ) );
  setCreateCellLevelArrays( reader->readValue("CreateCellLevelArrays", getCreateCellLevelArrays()) );
  setRenumberFeatures( reader->readValue("RenumberFeatures", getRenumberFeatures()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FeatureInfoReader::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
/*[]*/writer->writeValue("FeatureEulerAnglesArrayName", getFeatureEulerAnglesArrayName() );
/*[]*/writer->writeValue("FeaturePhasesArrayName", getFeaturePhasesArrayName() );
/*[]*/writer->writeValue("CellEulerAnglesArrayName", getCellEulerAnglesArrayName() );
/*[]*/writer->writeValue("CellPhasesArrayName", getCellPhasesArrayName() );
  writer->writeValue("FeatureIdsArrayPath", getFeatureIdsArrayPath() );
  writer->writeValue("InputFile", getInputFile() );
  writer->writeValue("CreateCellLevelArrays", getCreateCellLevelArrays() );
  writer->writeValue("RenumberFeatures", getRenumberFeatures() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureInfoReader::updateFeatureInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_FeatureEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureInfoReader::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, FeatureInfoReader>(this, getDataContainerName());
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -301);
  if(getErrorCondition() < 0 || NULL == cellAttrMat.get() ) { return; }
  QVector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer cellFeatureAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::CellFeature);
  if(getErrorCondition() < 0) { return; }

  QFileInfo fi(getInputFile());
  if (getInputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("%1 needs the Input File Set and it was not.").arg(ClassName());
    setErrorCondition(-387);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if (fi.exists() == false)
  {
    QString ss = QObject::tr("The input file does not exist.");
    setErrorCondition(-388);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  QVector<size_t> dims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if (m_CreateCellLevelArrays)
  {
    m_CellPhasesPtr = cellAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this,  m_CellPhasesArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
////==>MIKE_GROEBER_FIX tempPath.update(DATACONTAINER_NAME, ATTRIBUTEMATRIX_NAME, getCellPhasesArrayName() );
////==>MIKE_GROEBER_FIX m_CellPhasesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    dims[0] = 3;
    m_CellEulerAnglesPtr = cellAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this,  m_CellEulerAnglesArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
////==>MIKE_GROEBER_FIX tempPath.update(DATACONTAINER_NAME, ATTRIBUTEMATRIX_NAME, getCellEulerAnglesArrayName() );
////==>MIKE_GROEBER_FIX m_CellEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_CellEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  dims[0] = 1;
  m_FeaturePhasesPtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this,  m_FeaturePhasesArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
////==>MIKE_GROEBER_FIX tempPath.update(DATACONTAINER_NAME, ATTRIBUTEMATRIX_NAME, getFeaturePhasesArrayName() );
////==>MIKE_GROEBER_FIX m_FeaturePhasesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 3;
  m_FeatureEulerAnglesPtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this,  m_FeatureEulerAnglesArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
////==>MIKE_GROEBER_FIX tempPath.update(DATACONTAINER_NAME, ATTRIBUTEMATRIX_NAME, getFeatureEulerAnglesArrayName() );
////==>MIKE_GROEBER_FIX m_FeatureEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureInfoReader::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FeatureInfoReader::readHeader()
{
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FeatureInfoReader::readFile()
{
  dataCheck();
  if(getErrorCondition() < 0) { return -999; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  AttributeMatrix::Pointer cellAttrMat = m->getAttributeMatrix(getCellAttributeMatrixName());
  AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(getCellFeatureAttributeMatrixName());

  std::ifstream inFile;
  inFile.open(getInputFile().toLatin1().data(), std::ios_base::binary);
  if(!inFile)
  {
    QString ss = QObject::tr("Failed to open: %1").arg(getInputFile());
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, -1);
    return -1;
  }
  int numfeatures;
  int gnum, phase;
  int maxphase = 0;
  float ea1, ea2, ea3;
  inFile >> numfeatures;
  if (0 == numfeatures)
  {
    notifyErrorMessage(getHumanLabel(), "The number of features is Zero and should be greater than Zero", -600);
    return -999;
  }

  QVector<size_t> tDims(1, numfeatures+1);
  cellFeatureAttrMat->setTupleDimensions(tDims);
  updateFeatureInstancePointers();

  for(int i = 0; i < numfeatures; i++)
  {
    inFile >> gnum >> phase >> ea1 >> ea2 >> ea3;
    if(gnum >= cellFeatureAttrMat->getNumTuples())
    {
      tDims[0] = gnum+1;
      cellFeatureAttrMat->setTupleDimensions(tDims);
      updateFeatureInstancePointers();
    }
    m_FeatureEulerAngles[3 * gnum] = ea1;
    m_FeatureEulerAngles[3 * gnum + 1] = ea2;
    m_FeatureEulerAngles[3 * gnum + 2] = ea3;
    m_FeaturePhases[gnum] = phase;
    if(phase > maxphase) { maxphase = phase; }
  }

  if (m_CreateCellLevelArrays == true)
  {
    int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();
    for(int i = 0; i < totalPoints; i++)
    {
      gnum = m_FeatureIds[i];
      m_CellEulerAngles[3 * i] = m_FeatureEulerAngles[3 * gnum];
      m_CellEulerAngles[3 * i + 1] = m_FeatureEulerAngles[3 * gnum + 1];
      m_CellEulerAngles[3 * i + 2] = m_FeatureEulerAngles[3 * gnum + 2];
      m_CellPhases[i] = m_FeaturePhases[gnum];
    }
  }

  if (m_RenumberFeatures == true)
  {
    int64_t totalPoints = cellAttrMat->getNumTuples();
    size_t totalFeatures = cellFeatureAttrMat->getNumTuples();

    // Find the unique set of feature ids
    QVector<bool> activeObjects(totalFeatures, false);
    for (int64_t i = 0; i < totalPoints; ++i)
    {
      activeObjects[m_FeatureIds[i]] = true;
    }
      cellFeatureAttrMat->removeInactiveObjects(activeObjects, m_FeatureIdsPtr.lock());
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
  return 0;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FeatureInfoReader::newFilterInstance(bool copyFilterParameters)
{
  FeatureInfoReader::Pointer filter = FeatureInfoReader::New();
  if(true == copyFilterParameters)
  {
    filter->setFeatureEulerAnglesArrayName(getFeatureEulerAnglesArrayName());
    filter->setFeaturePhasesArrayName(getFeaturePhasesArrayName());
    filter->setCellEulerAnglesArrayName(getCellEulerAnglesArrayName());
    filter->setCellPhasesArrayName(getCellPhasesArrayName());
    filter->setFeatureIdsArrayPath(getFeatureIdsArrayPath());
    filter->setInputFile( getInputFile() );
    filter->setCreateCellLevelArrays( getCreateCellLevelArrays() );
    filter->setRenumberFeatures( getRenumberFeatures() );
  }
  return filter;
}
