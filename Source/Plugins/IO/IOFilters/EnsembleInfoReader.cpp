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


#include "EnsembleInfoReader.h"

#include <QtCore/QtDebug>
#include <fstream>
#include <sstream>

#include <QtCore/QFileInfo>

#include "DREAM3DLib/DataArrays/DataArray.hpp"

#include "IO/IOConstants.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnsembleInfoReader::EnsembleInfoReader() :
  FileReader(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellEnsembleAttributeMatrixName),
  m_InputFile(""),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_PhaseTypesArrayName(DREAM3D::EnsembleData::PhaseTypes),
  m_CrystalStructures(NULL),
  m_PhaseTypes(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnsembleInfoReader::~EnsembleInfoReader()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoReader::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FileSystemFilterParameter::New("Input Ensemble Info File", "InputFile", FilterParameterWidgetType::InputFileWidget, getInputFile(), false, "", "*.txt"));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Data Container Name", "DataContainerName", FilterParameterWidgetType::DataContainerSelectionWidget, getDataContainerName(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Cell Ensemble Attribute Matrix", "CellEnsembleAttributeMatrixName", FilterParameterWidgetType::StringWidget, getCellEnsembleAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("Crystal Structures", "CrystalStructuresArrayName", FilterParameterWidgetType::StringWidget, getCrystalStructuresArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("PhaseTypes", "PhaseTypesArrayName", FilterParameterWidgetType::StringWidget, getPhaseTypesArrayName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDataContainerName(reader->readString("DataContainerName", getDataContainerName() ) );
  setCellEnsembleAttributeMatrixName(reader->readString("CellEnsembleAttributeMatrixName", getCellEnsembleAttributeMatrixName() ) );
  setPhaseTypesArrayName(reader->readString("PhaseTypesArrayName", getPhaseTypesArrayName() ) );
  setCrystalStructuresArrayName(reader->readString("CrystalStructuresArrayName", getCrystalStructuresArrayName() ) );
  setInputFile( reader->readString( "InputFile", getInputFile() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EnsembleInfoReader::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(DataContainerName)
  DREAM3D_FILTER_WRITE_PARAMETER(CellEnsembleAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(PhaseTypesArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(CrystalStructuresArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(InputFile)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoReader::updateEnsembleInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_PhaseTypesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_PhaseTypes = m_PhaseTypesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoReader::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName());
  if(getErrorCondition() < 0) { return; }
  QVector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer cellEnsembleAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::CellEnsemble);
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

  typedef DataArray<unsigned int> XTalStructArrayType;
  typedef DataArray<unsigned int> PTypeArrayType;
  QVector<size_t> dims(1, 1);
  tempPath.update(getDataContainerName(), getCellEnsembleAttributeMatrixName(), getCrystalStructuresArrayName() );
  m_CrystalStructuresPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(this,  tempPath, Ebsd::CrystalStructure::UnknownCrystalStructure, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getDataContainerName(), getCellEnsembleAttributeMatrixName(), getPhaseTypesArrayName() );
  m_PhaseTypesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(this,  tempPath, DREAM3D::PhaseType::UnknownPhaseType, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_PhaseTypesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_PhaseTypes = m_PhaseTypesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoReader::preflight()
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
int EnsembleInfoReader::readHeader()
{
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EnsembleInfoReader::readFile()
{
  dataCheck();

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  AttributeMatrix::Pointer cellensembleAttrMat = m->getAttributeMatrix(getCellEnsembleAttributeMatrixName());

  std::ifstream inFile;
  inFile.open(getInputFile().toLatin1().data(), std::ios_base::binary);
  if(!inFile)
  {
    QString ss = QObject::tr("Failed to open: ").arg(getInputFile());
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return -1;
  }
  int numphases, pnum;
  unsigned int crystruct, ptype;
  inFile >> numphases;

  QVector<size_t> tDims(1, numphases + 1);
  cellensembleAttrMat->resizeAttributeArrays(tDims);
  updateEnsembleInstancePointers();

  for(int i = 0; i < numphases; i++)
  {
    inFile >> pnum >> crystruct >> ptype;
    m_CrystalStructures[pnum] = crystruct;
    m_PhaseTypes[pnum] = ptype;
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
  return 0;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer EnsembleInfoReader::newFilterInstance(bool copyFilterParameters)
{
  EnsembleInfoReader::Pointer filter = EnsembleInfoReader::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EnsembleInfoReader::getCompiledLibraryName()
{ return IO::IOBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EnsembleInfoReader::getGroupName()
{ return DREAM3D::FilterGroups::IOFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EnsembleInfoReader::getSubGroupName()
{ return DREAM3D::FilterSubGroups::InputFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EnsembleInfoReader::getHumanLabel()
{ return "Read Ensemble Info File"; }

