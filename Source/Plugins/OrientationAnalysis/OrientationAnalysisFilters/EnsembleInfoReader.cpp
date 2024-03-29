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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "EnsembleInfoReader.h"

#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

#include "EbsdLib/Core/EbsdLibConstants.h"

/* Create Enumerations to allow the created Attribute Arrays to take part in renaming */
enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID21 = 21,

  DataArrayID30 = 30,
  DataArrayID31 = 31,
  DataArrayID32 = 32,
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnsembleInfoReader::EnsembleInfoReader() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnsembleInfoReader::~EnsembleInfoReader() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoReader::setupFilterParameters()
{
  FileReader::setupFilterParameters();
  FilterParameterVectorType parameters;
  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input Ensemble Info File", InputFile, FilterParameter::Category::Parameter, EnsembleInfoReader, "*.ini *.txt"));
  {
    DataContainerSelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("Data Container", DataContainerName, FilterParameter::Category::RequiredArray, EnsembleInfoReader, req));
  }
  parameters.push_back(SeparatorFilterParameter::Create("Ensemble Data", FilterParameter::Category::CreatedArray));
  parameters.push_back(SIMPL_NEW_AM_WITH_LINKED_DC_FP("Ensemble Attribute Matrix", CellEnsembleAttributeMatrixName, DataContainerName, FilterParameter::Category::CreatedArray, EnsembleInfoReader));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Crystal Structures", CrystalStructuresArrayName, DataContainerName, CellEnsembleAttributeMatrixName, FilterParameter::Category::CreatedArray,
                                                      EnsembleInfoReader));
  parameters.push_back(
      SIMPL_NEW_DA_WITH_LINKED_AM_FP("Phase Types", PhaseTypesArrayName, DataContainerName, CellEnsembleAttributeMatrixName, FilterParameter::Category::CreatedArray, EnsembleInfoReader));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDataContainerName(reader->readDataArrayPath("DataContainerName", getDataContainerName()));
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
  clearErrorCode();
  clearWarningCode();

  if(nullptr != m_CrystalStructuresPtr.lock())
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
    m_CrystalStructuresPtr.lock()->initializeWithValue(EbsdLib::CrystalStructure::UnknownCrystalStructure);

  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(nullptr != m_PhaseTypesPtr.lock())
  {
    m_PhaseTypes = m_PhaseTypesPtr.lock()->getPointer(0);
    m_PhaseTypesPtr.lock()->initializeWithValue(static_cast<PhaseType::EnumType>(PhaseType::Type::Unknown));
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoReader::initialize()
{
  m_ptype = PhaseType::Type::Unknown;
  m_crystruct = 999;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoReader::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  initialize();
  DataArrayPath tempPath;

  QFileInfo fi(getInputFile());
  if(getInputFile().isEmpty())
  {
    QString ss = QObject::tr("The input file must be set");
    setErrorCondition(-387, ss);
  }
  else if(!fi.exists())
  {
    QString ss = QObject::tr("The input file does not exist");
    setErrorCondition(-388, ss);
  }

  QString ext = fi.suffix();
  if(ext != "ini" && ext != "txt")
  {
    QString ss = QObject::tr("Incorrect file extension in '%1'. The file extension must be .ini or .txt").arg(getInputFile());
    setErrorCondition(-10018, ss);
  }

  if(m_CellEnsembleAttributeMatrixName.isEmpty())
  {
    QString ss = QObject::tr("Ensemble Attribute Matrix name must be set");
    setErrorCondition(-1, ss);
  }

  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer(this, getDataContainerName());
  if(getErrorCode() < 0)
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
    setErrorCondition(-10003, ss);
  }

  std::vector<size_t> tDims(1, numphases + 1);
  m->createNonPrereqAttributeMatrix(this, getCellEnsembleAttributeMatrixName(), tDims, AttributeMatrix::Type::CellEnsemble, AttributeMatrixID21);
  if(getErrorCode() < 0)
  {
    return;
  }

  std::vector<size_t> cDims(1, 1);
  tempPath.update(getDataContainerName().getDataContainerName(), getCellEnsembleAttributeMatrixName(), getCrystalStructuresArrayName());
  m_CrystalStructuresPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>>(this, tempPath, EbsdLib::CrystalStructure::UnknownCrystalStructure, cDims, "", DataArrayID31);
  if(nullptr != m_CrystalStructuresPtr.lock())
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getDataContainerName().getDataContainerName(), getCellEnsembleAttributeMatrixName(), getPhaseTypesArrayName());
  m_PhaseTypesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>>(this, tempPath, static_cast<PhaseType::EnumType>(PhaseType::Type::Unknown), cDims, "", DataArrayID32);
  if(nullptr != m_PhaseTypesPtr.lock())
  {
    m_PhaseTypes = m_PhaseTypesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
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
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return getErrorCode();
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
    setErrorCondition(-10003, ss);
    return -1;
  }

  // Figure out if we are reading contiguous groups
  std::vector<bool> visited(numphases + 1, false);
  visited[0] = true; // this is DREAM3D's internal, which is always visited.

  std::vector<size_t> tDims(1, numphases + 1);
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
      setErrorCondition(-10008, ss);
      return -1;
    }

    if(phaseTypeString.compare("MissingPhaseType") == 0)
    {
      QString ss = QObject::tr("Missing phase type for phase '%1'").arg(group);
      setErrorCondition(-10009, ss);
      return -1;
    }

    // Past that sanity check, so we have values, lets parse them
    QStringList values;
    values << xtalString << phaseTypeString;

    ensembleLookup(values); // Lookup number for the crystal number string and the phase type string read from the file

    // Check to see if the Crystal Structure string was valid
    if(m_crystruct == EbsdLib::CrystalStructure::UnknownCrystalStructure) // The crystal structure name read from the file was not found in the lookup table
    {
      QString ss = QObject::tr("Incorrect crystal structure name '%1'").arg(xtalString);
      setErrorCondition(-10006, ss);
      return -1;
    }

    m_CrystalStructures[index] = m_crystruct;

    // now check to see if the Phase type string was valid.
    if(m_ptype == PhaseType::Type::Unknown)
    {
      QString ss = QObject::tr("Incorrect phase type name '%1'").arg(phaseTypeString); // The phase type name read from the file was not found in the lookup table
      setErrorCondition(-10007, ss);
      return -1;
    }

    m_PhaseTypes[index] = static_cast<PhaseType::EnumType>(m_ptype);

    visited[index] = true;
    // Close up this group
    settings.endGroup();
  }

  // Make sure we visited all the groups.
  for(std::vector<bool>::size_type i = 0; i < visited.size(); i++)
  {
    if(!visited[i])
    {
      QString ss = QObject::tr("Phase '%1' did not have entries in the file. Phase numbering must start at 1 and no phases may be skipped")
                       .arg(i); // The phase type name read from the file was not found in the lookup table
      setErrorCondition(-10005, ss);
      return -1;
    }
  }

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
    m_crystruct = EbsdLib::CrystalStructure::Hexagonal_High;
  }
  else if(QString::compare(list.at(0), "Cubic_High", Qt::CaseInsensitive) == 0)
  {
    m_crystruct = EbsdLib::CrystalStructure::Cubic_High;
  }
  else if(QString::compare(list.at(0), "Hexagonal_Low", Qt::CaseInsensitive) == 0)
  {
    m_crystruct = EbsdLib::CrystalStructure::Hexagonal_Low;
  }
  else if(QString::compare(list.at(0), "Cubic_Low", Qt::CaseInsensitive) == 0)
  {
    m_crystruct = EbsdLib::CrystalStructure::Cubic_Low;
  }
  else if(QString::compare(list.at(0), "Triclinic", Qt::CaseInsensitive) == 0)
  {
    m_crystruct = EbsdLib::CrystalStructure::Triclinic;
  }
  else if(QString::compare(list.at(0), "Monoclinic", Qt::CaseInsensitive) == 0)
  {
    m_crystruct = EbsdLib::CrystalStructure::Monoclinic;
  }
  else if(QString::compare(list.at(0), "OrthoRhombic", Qt::CaseInsensitive) == 0)
  {
    m_crystruct = EbsdLib::CrystalStructure::OrthoRhombic;
  }
  else if(QString::compare(list.at(0), "Tetragonal_Low", Qt::CaseInsensitive) == 0)
  {
    m_crystruct = EbsdLib::CrystalStructure::Tetragonal_Low;
  }
  else if(QString::compare(list.at(0), "Tetragonal_High", Qt::CaseInsensitive) == 0)
  {
    m_crystruct = EbsdLib::CrystalStructure::Tetragonal_High;
  }
  else if(QString::compare(list.at(0), "Trigonal_Low", Qt::CaseInsensitive) == 0)
  {
    m_crystruct = EbsdLib::CrystalStructure::Trigonal_Low;
  }
  else if(QString::compare(list.at(0), "Trigonal_High", Qt::CaseInsensitive) == 0)
  {
    m_crystruct = EbsdLib::CrystalStructure::Trigonal_High;
  }
  else
  {
    m_crystruct = EbsdLib::CrystalStructure::UnknownCrystalStructure; // no match for crystal structure name read from file
  }

  // assign the corresponding number to the phase type string read from the input file
  if(QString::compare(list.at(1), "PrimaryPhase", Qt::CaseInsensitive) == 0)
  {
    m_ptype = PhaseType::Type::Primary;
  }
  else if(QString::compare(list.at(1), "PrecipitatePhase", Qt::CaseInsensitive) == 0)
  {
    m_ptype = PhaseType::Type::Precipitate;
  }
  else if(QString::compare(list.at(1), "TransformationPhase", Qt::CaseInsensitive) == 0)
  {
    m_ptype = PhaseType::Type::Transformation;
  }
  else if(QString::compare(list.at(1), "MatrixPhase", Qt::CaseInsensitive) == 0)
  {
    m_ptype = PhaseType::Type::Matrix;
  }
  else if(QString::compare(list.at(1), "BoundaryPhase", Qt::CaseInsensitive) == 0)
  {
    m_ptype = PhaseType::Type::Boundary;
  }
  else if(QString::compare(list.at(1), "Unknown", Qt::CaseInsensitive) == 0)
  {
    m_ptype = PhaseType::Type::Unknown;
  }
  else
  {
    m_ptype = PhaseType::Type::Unknown; // no match for phase type name read from file
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer EnsembleInfoReader::newFilterInstance(bool copyFilterParameters) const
{
  EnsembleInfoReader::Pointer filter = EnsembleInfoReader::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EnsembleInfoReader::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EnsembleInfoReader::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EnsembleInfoReader::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EnsembleInfoReader::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid EnsembleInfoReader::getUuid() const
{
  return QUuid("{33a37a47-d002-5c18-b270-86025881fe1e}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EnsembleInfoReader::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EnsembleInfoReader::getHumanLabel() const
{
  return "Import Ensemble Info File";
}

// -----------------------------------------------------------------------------
EnsembleInfoReader::Pointer EnsembleInfoReader::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<EnsembleInfoReader> EnsembleInfoReader::New()
{
  struct make_shared_enabler : public EnsembleInfoReader
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString EnsembleInfoReader::getNameOfClass() const
{
  return QString("EnsembleInfoReader");
}

// -----------------------------------------------------------------------------
QString EnsembleInfoReader::ClassName()
{
  return QString("EnsembleInfoReader");
}

// -----------------------------------------------------------------------------
void EnsembleInfoReader::setDataContainerName(const DataArrayPath& value)
{
  m_DataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath EnsembleInfoReader::getDataContainerName() const
{
  return m_DataContainerName;
}

// -----------------------------------------------------------------------------
void EnsembleInfoReader::setCellEnsembleAttributeMatrixName(const QString& value)
{
  m_CellEnsembleAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString EnsembleInfoReader::getCellEnsembleAttributeMatrixName() const
{
  return m_CellEnsembleAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void EnsembleInfoReader::setInputFile(const QString& value)
{
  m_InputFile = value;
}

// -----------------------------------------------------------------------------
QString EnsembleInfoReader::getInputFile() const
{
  return m_InputFile;
}

// -----------------------------------------------------------------------------
void EnsembleInfoReader::setCrystalStructuresArrayName(const QString& value)
{
  m_CrystalStructuresArrayName = value;
}

// -----------------------------------------------------------------------------
QString EnsembleInfoReader::getCrystalStructuresArrayName() const
{
  return m_CrystalStructuresArrayName;
}

// -----------------------------------------------------------------------------
void EnsembleInfoReader::setPhaseTypesArrayName(const QString& value)
{
  m_PhaseTypesArrayName = value;
}

// -----------------------------------------------------------------------------
QString EnsembleInfoReader::getPhaseTypesArrayName() const
{
  return m_PhaseTypesArrayName;
}
