/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "EnsembleInfoReader.h"

#include <QtCore/QFileInfo>
#include <QtCore/QSettings>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

#include "EbsdLib/EbsdConstants.h"

// Include the MOC generated file for this class
#include "moc_EnsembleInfoReader.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnsembleInfoReader::EnsembleInfoReader()
: FileReader()
, m_DataContainerName("")
, m_CellEnsembleAttributeMatrixName("EnsembleAttributeMatrix")
, m_InputFile("")
, m_CrystalStructuresArrayName(SIMPL::EnsembleData::CrystalStructures)
, m_PhaseTypesArrayName(SIMPL::EnsembleData::PhaseTypes)
, m_CrystalStructures(nullptr)
, m_PhaseTypes(nullptr)
, m_ptype(SIMPL::PhaseType::UnknownPhaseType)
, m_crystruct(999)
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
  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input Ensemble Info File", InputFile, FilterParameter::Parameter, EnsembleInfoReader, "*.ini *.txt"));
  {
    DataContainerSelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("Data Container", DataContainerName, FilterParameter::RequiredArray, EnsembleInfoReader, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Ensemble Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Ensemble Attribute Matrix", CellEnsembleAttributeMatrixName, FilterParameter::CreatedArray, EnsembleInfoReader));
  parameters.push_back(SIMPL_NEW_STRING_FP("Crystal Structures", CrystalStructuresArrayName, FilterParameter::CreatedArray, EnsembleInfoReader));
  parameters.push_back(SIMPL_NEW_STRING_FP("Phase Types", PhaseTypesArrayName, FilterParameter::CreatedArray, EnsembleInfoReader));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDataContainerName(reader->readString("DataContainerName", getDataContainerName()));
  setCellEnsembleAttributeMatrixName(reader->readString("CellEnsembleAttributeMatrixName", getCellEnsembleAttributeMatrixName()));
  setPhaseTypesArrayName(reader->readString("PhaseTypesArrayName", getPhaseTypesArrayName()));
  setCrystalStructuresArrayName(reader->readString("CrystalStructuresArrayName", getCrystalStructuresArrayName()));
  setInputFile(reader->readString("InputFile", getInputFile()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoReader::updateEnsembleInstancePointers()
{
  setErrorCondition(0);

  if(nullptr != m_CrystalStructuresPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
    m_CrystalStructuresPtr.lock()->initializeWithValue(Ebsd::CrystalStructure::UnknownCrystalStructure);

  }                                           /* Now assign the raw pointer to data from the DataArray<T> object */
  if(nullptr != m_PhaseTypesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_PhaseTypes = m_PhaseTypesPtr.lock()->getPointer(0);
    m_PhaseTypesPtr.lock()->initializeWithValue(SIMPL::PhaseType::UnknownPhaseType);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoReader::initialize()
{
  m_ptype = SIMPL::PhaseType::UnknownPhaseType;
  m_crystruct = 999;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoReader::dataCheck()
{
  setErrorCondition(0);
  initialize();
  DataArrayPath tempPath;

  QFileInfo fi(getInputFile());
  if(getInputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("The input file must be set");
    setErrorCondition(-387);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if(fi.exists() == false)
  {
    QString ss = QObject::tr("The input file does not exist");
    setErrorCondition(-388);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  QString ext = fi.suffix();
  if(ext != "ini" && ext != "txt")
  {
    QString ss = QObject::tr("Incorrect file extension in '%1'. The file extension must be .ini or .txt").arg(getInputFile());
    setErrorCondition(-10018);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(m_CellEnsembleAttributeMatrixName.isEmpty() == true)
  {
    QString ss = QObject::tr("Ensemble Attribute Matrix name must be set");
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getDataContainerName());
  if(getErrorCondition() < 0)
  {
    return;
  }

  int32_t numphases = 0;

  QSettings settings(getInputFile(), QSettings::IniFormat); // The .ini or .txt input file
  settings.beginGroup("EnsembleInfo");
  numphases = settings.value("Number_Phases").toInt(); // read number of phases from input file
  settings.endGroup();

  if(0 == numphases) // Either the group name "EnsembleInfo" is incorrect or 0 was entered as the Number_Phases
  {
    QString ss = QObject::tr("Check the group name EnsembleInfo and that Number_Phases > 0");
    setErrorCondition(-10003);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  QVector<size_t> tDims(1, numphases + 1);
  m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), tDims, AttributeMatrix::Type::CellEnsemble);
  if(getErrorCondition() < 0)
  {
    return;
  }

  QVector<size_t> cDims(1, 1);
  tempPath.update(getDataContainerName(), getCellEnsembleAttributeMatrixName(), getCrystalStructuresArrayName());
  m_CrystalStructuresPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(
      this, tempPath, Ebsd::CrystalStructure::UnknownCrystalStructure, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CrystalStructuresPtr.lock().get())                           /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getDataContainerName(), getCellEnsembleAttributeMatrixName(), getPhaseTypesArrayName());
  m_PhaseTypesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(
      this, tempPath, SIMPL::PhaseType::UnknownPhaseType, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_PhaseTypesPtr.lock().get())                     /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_PhaseTypes = m_PhaseTypesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
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
int32_t EnsembleInfoReader::readHeader()
{
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t EnsembleInfoReader::readFile()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return getErrorCondition();
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  AttributeMatrix::Pointer cellensembleAttrMat = m->getAttributeMatrix(getCellEnsembleAttributeMatrixName());

  int32_t numphases = 0;

  QSettings settings(getInputFile(), QSettings::IniFormat); // The .ini or .txt input file
  settings.beginGroup("EnsembleInfo");
  numphases = settings.value("Number_Phases").toInt(); // read number of phases from input file
  settings.endGroup();

  if(0 == numphases) // Either the group name "EnsembleInfo" is incorrect or 0 was entered as the Number_Phases
  {
    QString ss = QObject::tr("Check the group name EnsembleInfo and that Number_Phases > 0");
    setErrorCondition(-10003);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return -1;
  }

  // Figure out if we are reading contiguous groups
  std::vector<bool> visited(numphases + 1, false);
  visited[0] = true; // this is DREAM3D's internal, which is always visited.

  QVector<size_t> tDims(1, numphases + 1);
  cellensembleAttrMat->resizeAttributeArrays(tDims);
  updateEnsembleInstancePointers();
  for(int32_t index = 1; index < numphases + 1; index++)
  {
    QString group = QString::number(index);
    settings.beginGroup(group);

    QString xtalString = settings.value(SIMPL::StringConstants::CrystalStructure, "MissingCrystalStructure").toString();
    QString phaseTypeString = settings.value(SIMPL::StringConstants::PhaseType, "MissingPhaseType").toString();
    // Check to make sure the user has something for each of the Crystal Structure and Phase Type
    if(xtalString.compare("MissingCrystalStructure") == 0)
    {
      QString ss = QObject::tr("Missing crystal structure for phase '%1'").arg(group);
      setErrorCondition(-10008);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return -1;
    }

    if(phaseTypeString.compare("MissingPhaseType") == 0)
    {
      QString ss = QObject::tr("Missing phase type for phase '%1'").arg(group);
      setErrorCondition(-10009);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return -1;
    }

    // Past that sanity check, so we have values, lets parse them
    QStringList values;
    values << xtalString << phaseTypeString;

    ensembleLookup(values); // Lookup number for the crystal number string and the phase type string read from the file

    // Check to see if the Crystal Structure string was valid
    if(m_crystruct == Ebsd::CrystalStructure::UnknownCrystalStructure) // The crystal structure name read from the file was not found in the lookup table
    {
      QString ss = QObject::tr("Incorrect crystal structure name '%1'").arg(xtalString);
      setErrorCondition(-10006);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return -1;
    }
    else
    {
      m_CrystalStructures[index] = m_crystruct;
    }

    // now check to see if the Phase type string was valid.
    if(m_ptype == SIMPL::PhaseType::UnknownPhaseType)
    {
      QString ss = QObject::tr("Incorrect phase type name '%1'").arg(phaseTypeString); // The phase type name read from the file was not found in the lookup table
      setErrorCondition(-10007);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return -1;
    }
    else
    {
      m_PhaseTypes[index] = m_ptype;
    }

    visited[index] = true;
    // Close up this group
    settings.endGroup();
  }

  // Make sure we visited all the groups.
  for(std::vector<bool>::size_type i = 0; i < visited.size(); i++)
  {
    if(visited[i] == false)
    {
      QString ss = QObject::tr("Phase '%1' did not have entries in the file. Phase numbering must start at 1 and no phases may be skipped")
                       .arg(i); // The phase type name read from the file was not found in the lookup table
      setErrorCondition(-10005);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return -1;
    }
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoReader::ensembleLookup(QStringList list)
{
  // assign the corresponding number to the crystal structure string read from the input file
  if(QString::compare(list.at(0), "Hexagonal_High", Qt::CaseInsensitive) == 0)
  {
    m_crystruct = Ebsd::CrystalStructure::Hexagonal_High;
  }
  else if(QString::compare(list.at(0), "Cubic_High", Qt::CaseInsensitive) == 0)
  {
    m_crystruct = Ebsd::CrystalStructure::Cubic_High;
  }
  else if(QString::compare(list.at(0), "Hexagonal_Low", Qt::CaseInsensitive) == 0)
  {
    m_crystruct = Ebsd::CrystalStructure::Hexagonal_Low;
  }
  else if(QString::compare(list.at(0), "Cubic_Low", Qt::CaseInsensitive) == 0)
  {
    m_crystruct = Ebsd::CrystalStructure::Cubic_Low;
  }
  else if(QString::compare(list.at(0), "Triclinic", Qt::CaseInsensitive) == 0)
  {
    m_crystruct = Ebsd::CrystalStructure::Triclinic;
  }
  else if(QString::compare(list.at(0), "Monoclinic", Qt::CaseInsensitive) == 0)
  {
    m_crystruct = Ebsd::CrystalStructure::Monoclinic;
  }
  else if(QString::compare(list.at(0), "OrthoRhombic", Qt::CaseInsensitive) == 0)
  {
    m_crystruct = Ebsd::CrystalStructure::OrthoRhombic;
  }
  else if(QString::compare(list.at(0), "Tetragonal_Low", Qt::CaseInsensitive) == 0)
  {
    m_crystruct = Ebsd::CrystalStructure::Tetragonal_Low;
  }
  else if(QString::compare(list.at(0), "Tetragonal_High", Qt::CaseInsensitive) == 0)
  {
    m_crystruct = Ebsd::CrystalStructure::Tetragonal_High;
  }
  else if(QString::compare(list.at(0), "Trigonal_Low", Qt::CaseInsensitive) == 0)
  {
    m_crystruct = Ebsd::CrystalStructure::Trigonal_Low;
  }
  else if(QString::compare(list.at(0), "Trigonal_High", Qt::CaseInsensitive) == 0)
  {
    m_crystruct = Ebsd::CrystalStructure::Trigonal_High;
  }
  else
  {
    m_crystruct = Ebsd::CrystalStructure::UnknownCrystalStructure; // no match for crystal structure name read from file
  }

  // assign the corresponding number to the phase type string read from the input file
  if(QString::compare(list.at(1), "PrimaryPhase", Qt::CaseInsensitive) == 0)
  {
    m_ptype = SIMPL::PhaseType::PrimaryPhase;
  }
  else if(QString::compare(list.at(1), "PrecipitatePhase", Qt::CaseInsensitive) == 0)
  {
    m_ptype = SIMPL::PhaseType::PrecipitatePhase;
  }
  else if(QString::compare(list.at(1), "TransformationPhase", Qt::CaseInsensitive) == 0)
  {
    m_ptype = SIMPL::PhaseType::TransformationPhase;
  }
  else if(QString::compare(list.at(1), "MatrixPhase", Qt::CaseInsensitive) == 0)
  {
    m_ptype = SIMPL::PhaseType::MatrixPhase;
  }
  else if(QString::compare(list.at(1), "BoundaryPhase", Qt::CaseInsensitive) == 0)
  {
    m_ptype = SIMPL::PhaseType::BoundaryPhase;
  }
  else if(QString::compare(list.at(1), "UnknownPhaseType", Qt::CaseInsensitive) == 0)
  {
    m_ptype = SIMPL::PhaseType::UnknownPhaseType;
  }
  else
  {
    m_ptype = SIMPL::PhaseType::UnknownPhaseType; // no match for phase type name read from file
  }
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
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EnsembleInfoReader::getBrandingString()
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EnsembleInfoReader::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EnsembleInfoReader::getGroupName()
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EnsembleInfoReader::getSubGroupName()
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EnsembleInfoReader::getHumanLabel()
{
  return "Read Ensemble Info File";
}
