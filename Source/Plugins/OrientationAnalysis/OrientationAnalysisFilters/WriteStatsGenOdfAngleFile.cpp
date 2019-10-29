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

#include <memory>

#include "WriteStatsGenOdfAngleFile.h"

#include <cstdio>

#include <iostream>

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtCore/QString>

#include <QtCore/QTextStream>

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"

#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Utilities/FileSystemPathHelper.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

#define SET_DIRECTION(i, j, k)                                                                                                                                                                         \
  direction[0] = i;                                                                                                                                                                                    \
  direction[1] = j;                                                                                                                                                                                    \
  direction[2] = k;

#define WRITE_EULERS_TEXT_FILE 1

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteStatsGenOdfAngleFile::WriteStatsGenOdfAngleFile()
: m_OutputFile("")
, m_Weight(1.0f)
, m_Sigma(1)
, m_Delimiter(2)
, m_CellPhasesArrayPath("", "", "")
, m_CellEulerAnglesArrayPath("", "", "")
, m_GoodVoxelsArrayPath("", "", "")
, m_ConvertToDegrees(false)
, m_UseGoodVoxels(false)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteStatsGenOdfAngleFile::~WriteStatsGenOdfAngleFile() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteStatsGenOdfAngleFile::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  /* For String input use this code */
  parameters.push_back(SIMPL_NEW_OUTPUT_FILE_FP("Output File", OutputFile, FilterParameter::Parameter, WriteStatsGenOdfAngleFile));
  parameters.push_back(SIMPL_NEW_FLOAT_FP("Default Weight", Weight, FilterParameter::Parameter, WriteStatsGenOdfAngleFile));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Default Sigma", Sigma, FilterParameter::Parameter, WriteStatsGenOdfAngleFile));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New(); // Delimiter choice
    parameter->setHumanLabel("Delimiter");
    parameter->setPropertyName("Delimiter");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(WriteStatsGenOdfAngleFile, this, Delimiter));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(WriteStatsGenOdfAngleFile, this, Delimiter));

    QVector<QString> choices;
    choices.push_back(", (comma)");
    choices.push_back("; (semicolon)");
    choices.push_back("  (space)");
    choices.push_back(": (colon)");
    choices.push_back("\\t (Tab)");
    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  parameters.push_back(SIMPL_NEW_BOOL_FP("Convert to Degrees", ConvertToDegrees, FilterParameter::Parameter, WriteStatsGenOdfAngleFile));
  QStringList linkedProps("GoodVoxelsArrayPath");
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Only Write Good Elements", UseGoodVoxels, FilterParameter::Parameter, WriteStatsGenOdfAngleFile, linkedProps));

  parameters.push_back(SeparatorFilterParameter::New("Element Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Float, 3, AttributeMatrix::Category::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Euler Angles", CellEulerAnglesArrayPath, FilterParameter::RequiredArray, WriteStatsGenOdfAngleFile, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Category::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", CellPhasesArrayPath, FilterParameter::RequiredArray, WriteStatsGenOdfAngleFile, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Bool, 1, AttributeMatrix::Category::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Mask", GoodVoxelsArrayPath, FilterParameter::RequiredArray, WriteStatsGenOdfAngleFile, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteStatsGenOdfAngleFile::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setConvertToDegrees(reader->readValue("ConvertToDegrees", getConvertToDegrees()));
  setOutputFile(reader->readString("OutputFile", getOutputFile()));
  setUseGoodVoxels(reader->readValue("UseGoodVoxels", getUseGoodVoxels()));
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath()));
  setCellEulerAnglesArrayPath(reader->readDataArrayPath("CellEulerAnglesArrayPath", getCellEulerAnglesArrayPath()));
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteStatsGenOdfAngleFile::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteStatsGenOdfAngleFile::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  FileSystemPathHelper::CheckOutputFile(this, "Output File Path", getOutputFile(), true);

  QString ss;
  if(getWeight() < 1.0f)
  {
    ss = QObject::tr("The default 'Weight' value should be at least 1.0. Undefined results will occur from this filter.");
    setErrorCondition(-94002, ss);
  }

  if(getSigma() < 1)
  {
    ss = QObject::tr("The default 'Sigma' value should be at least 1. Undefined results will occur from this filter.");
    setErrorCondition(-94003, ss);
  }

  std::vector<size_t> cDims(1, 1);
  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CellPhasesPtr.lock())                                                                         /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 3;
  m_CellEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCellEulerAnglesArrayPath(),
                                                                                                           cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CellEulerAnglesPtr.lock())                                                                       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(getUseGoodVoxels())
  {
    // The good voxels array is optional, If it is available we are going to use it, otherwise we are going to create it
    cDims[0] = 1;
    m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getGoodVoxelsArrayPath(),
                                                                                                       cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_GoodVoxelsPtr.lock())                                                                      /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  else
  {
    m_GoodVoxels = nullptr;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteStatsGenOdfAngleFile::preflight()
{
  setInPreflight(true);
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteStatsGenOdfAngleFile::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  // Figure out how many unique phase values we have by looping over all the phase values
  int64_t totalPoints = m_CellPhasesPtr.lock()->getNumberOfTuples();
  std::set<int32_t> uniquePhases;
  for(int64_t i = 0; i < totalPoints; i++)
  {
    uniquePhases.insert(m_CellPhases[i]);
  }

  uniquePhases.erase(0); // remove Phase 0 as this is a Special Phase for DREAM3D
  std::vector<QFile*> oFiles;

  QFileInfo fi(getOutputFile());

  QDir dir(fi.path());
  if(!dir.mkpath("."))
  {
    QString ss;
    ss = QObject::tr("Error creating parent path '%1'").arg(dir.path());
    setErrorCondition(-45001, ss);
    return;
  }


  QString absPath = fi.absolutePath();
  QString fname = fi.completeBaseName();
  QString suffix = fi.suffix();

  for(std::set<int32_t>::iterator iter = uniquePhases.begin(); iter != uniquePhases.end(); iter++)
  {
    // Dry run to figure out how many lines we are going to have
    int32_t lineCount = determineOutputLineCount(totalPoints, *iter);
    if(lineCount == 0)
    {
      QString ss = QObject::tr("No valid data for phase '%1'. No ODF Angle file written for phase.").arg(*iter);
      setWarningCondition(0, ss);
      continue;
    }

    QString ss = QObject::tr("Writing file for phase '%1'").arg(*iter);
    notifyStatusMessage(ss);

    QString absFilePath = absPath + "/" + fname + "_Phase_" + QString::number(*iter) + "." + suffix;

    QFile file(absFilePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
      QString ss = QObject::tr("Error creating output file '%1'").arg(absFilePath);
      setErrorCondition(-99000, ss);
      return;
    }

    QTextStream out(&file);

    int err = writeOutputFile(out, lineCount, totalPoints, *iter);
    if(err < 0)
    {
      QString ss = QObject::tr("Error writing output file '%1'").arg(absFilePath);
      setErrorCondition(-99001, ss);
      return;
    }
    out.flush();
    file.close(); // Close the file
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int WriteStatsGenOdfAngleFile::determineOutputLineCount(int64_t totalPoints, int32_t phase)
{
  int32_t lineCount = 0;
  for(int64_t i = 0; i < totalPoints; i++)
  {
    if(m_CellPhases[i] == phase)
    {
      if(!m_UseGoodVoxels)
      {
        lineCount++;
      }
      else if(m_UseGoodVoxels && m_GoodVoxels[i])
      {
        lineCount++;
      }
    }
  }

  return lineCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int WriteStatsGenOdfAngleFile::writeOutputFile(QTextStream& out, int32_t lineCount, int64_t totalPoints, int32_t phase)
{
  bool writeLine = false;
  out << "# All lines starting with '#' are comments and should come before the data.\n";
  out << "# DREAM.3D StatsGenerator ODF Angles Input File\n";
  out << "# DREAM.3D Version " << OrientationAnalysis::Version::Complete() << "\n";

  QChar delimiter[5] = {',', ';', ' ', '"', '\t'};
  QString delStr[5] = {"Comma", "Semicolon", "Space", "Colon", "Tab"};

  out << "# Angle Data is " << delStr[m_Delimiter] << " delimited.\n";

  if(m_ConvertToDegrees)
  {
    out << "# Euler angles are expressed in degrees\n";
  }
  else
  {
    out << "# Euler angles are expressed in radians\n";
  }
  out << "# Euler0 Euler1 Euler2 Weight Sigma\n";
  out << "Angle Count:" << lineCount << "\n";

  for(int64_t i = 0; i < totalPoints; i++)
  {
    writeLine = false;

    if(m_CellPhases[i] == phase)
    {
      if(!m_UseGoodVoxels)
      {
        writeLine = true;
      }
      else if(m_UseGoodVoxels && m_GoodVoxels[i])
      {
        writeLine = true;
      }
    }

    if(writeLine)
    {
      float e0 = m_CellEulerAngles[i * 3 + 0];
      float e1 = m_CellEulerAngles[i * 3 + 1];
      float e2 = m_CellEulerAngles[i * 3 + 2];
      if(m_ConvertToDegrees)
      {
        e0 = e0 * static_cast<float>(SIMPLib::Constants::k_180OverPi);
        e1 = e1 * static_cast<float>(SIMPLib::Constants::k_180OverPi);
        e2 = e2 * static_cast<float>(SIMPLib::Constants::k_180OverPi);
      }
      out << e0 << delimiter[m_Delimiter] << e1 << delimiter[m_Delimiter] << e2 << delimiter[m_Delimiter] << m_Weight << delimiter[m_Delimiter] << m_Sigma << "\n";
    }
  }

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer WriteStatsGenOdfAngleFile::newFilterInstance(bool copyFilterParameters) const
{
  WriteStatsGenOdfAngleFile::Pointer filter = WriteStatsGenOdfAngleFile::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString WriteStatsGenOdfAngleFile::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString WriteStatsGenOdfAngleFile::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString WriteStatsGenOdfAngleFile::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString WriteStatsGenOdfAngleFile::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid WriteStatsGenOdfAngleFile::getUuid() const
{
  return QUuid("{a4952f40-22dd-54ec-8c38-69c3fcd0e6f7}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString WriteStatsGenOdfAngleFile::getSubGroupName() const
{
  return "Output";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString WriteStatsGenOdfAngleFile::getHumanLabel() const
{
  return "Export StatsGenerator ODF Angle File";
}

// -----------------------------------------------------------------------------
WriteStatsGenOdfAngleFile::Pointer WriteStatsGenOdfAngleFile::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<WriteStatsGenOdfAngleFile> WriteStatsGenOdfAngleFile::New()
{
  struct make_shared_enabler : public WriteStatsGenOdfAngleFile
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString WriteStatsGenOdfAngleFile::getNameOfClass() const
{
  return QString("WriteStatsGenOdfAngleFile");
}

// -----------------------------------------------------------------------------
QString WriteStatsGenOdfAngleFile::ClassName()
{
  return QString("WriteStatsGenOdfAngleFile");
}

// -----------------------------------------------------------------------------
void WriteStatsGenOdfAngleFile::setOutputFile(const QString& value)
{
  m_OutputFile = value;
}

// -----------------------------------------------------------------------------
QString WriteStatsGenOdfAngleFile::getOutputFile() const
{
  return m_OutputFile;
}

// -----------------------------------------------------------------------------
void WriteStatsGenOdfAngleFile::setWeight(float value)
{
  m_Weight = value;
}

// -----------------------------------------------------------------------------
float WriteStatsGenOdfAngleFile::getWeight() const
{
  return m_Weight;
}

// -----------------------------------------------------------------------------
void WriteStatsGenOdfAngleFile::setSigma(int value)
{
  m_Sigma = value;
}

// -----------------------------------------------------------------------------
int WriteStatsGenOdfAngleFile::getSigma() const
{
  return m_Sigma;
}

// -----------------------------------------------------------------------------
void WriteStatsGenOdfAngleFile::setDelimiter(int value)
{
  m_Delimiter = value;
}

// -----------------------------------------------------------------------------
int WriteStatsGenOdfAngleFile::getDelimiter() const
{
  return m_Delimiter;
}

// -----------------------------------------------------------------------------
void WriteStatsGenOdfAngleFile::setCellPhasesArrayPath(const DataArrayPath& value)
{
  m_CellPhasesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath WriteStatsGenOdfAngleFile::getCellPhasesArrayPath() const
{
  return m_CellPhasesArrayPath;
}

// -----------------------------------------------------------------------------
void WriteStatsGenOdfAngleFile::setCellEulerAnglesArrayPath(const DataArrayPath& value)
{
  m_CellEulerAnglesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath WriteStatsGenOdfAngleFile::getCellEulerAnglesArrayPath() const
{
  return m_CellEulerAnglesArrayPath;
}

// -----------------------------------------------------------------------------
void WriteStatsGenOdfAngleFile::setGoodVoxelsArrayPath(const DataArrayPath& value)
{
  m_GoodVoxelsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath WriteStatsGenOdfAngleFile::getGoodVoxelsArrayPath() const
{
  return m_GoodVoxelsArrayPath;
}

// -----------------------------------------------------------------------------
void WriteStatsGenOdfAngleFile::setConvertToDegrees(bool value)
{
  m_ConvertToDegrees = value;
}

// -----------------------------------------------------------------------------
bool WriteStatsGenOdfAngleFile::getConvertToDegrees() const
{
  return m_ConvertToDegrees;
}

// -----------------------------------------------------------------------------
void WriteStatsGenOdfAngleFile::setUseGoodVoxels(bool value)
{
  m_UseGoodVoxels = value;
}

// -----------------------------------------------------------------------------
bool WriteStatsGenOdfAngleFile::getUseGoodVoxels() const
{
  return m_UseGoodVoxels;
}
