/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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


#include "ExportData.h"

#include <QtCore/QDir>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/TemplateHelpers.hpp"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "SIMPLib/FilterParameters/OutputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/MultiDataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"


#include "IO/IOConstants.h"

/**
 * @brief The ExportDataPrivate class is a templated class that implements a method to generically
 * export data to an ASCII file
 */
template<typename TInputType>
class ExportDataPrivate
{
  public:
    typedef DataArray<TInputType> DataArrayType;

    ExportDataPrivate() {}
    virtual ~ExportDataPrivate() {}

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    bool operator()(IDataArray::Pointer p)
    {
      return (boost::dynamic_pointer_cast<DataArrayType>(p).get() != NULL);
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void static Execute(ExportData* filter, IDataArray::Pointer inputData, char delimeter, QString outputFile, int32_t MaxValPerLine)
    {
      typename DataArrayType::Pointer inputArray = boost::dynamic_pointer_cast<DataArrayType>(inputData);

      QFile file(outputFile);
      if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
      {
        QString ss = QObject::tr("The output file could not be opened: '%1'").arg(outputFile);
        filter->setErrorCondition(-11008);
        filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        return;
      }

      QTextStream out(&file);

      int32_t nComp = inputArray->getNumberOfComponents();

      TInputType* inputArrayPtr = inputArray->getPointer(0);
      size_t nTuples = inputArray->getNumberOfTuples();

      int32_t recCount = 0;
      for (size_t i = 0; i < nTuples; i++)
      {
        for (int32_t j = 0; j < nComp; j++)
        {
          out << inputArrayPtr[i * nComp + j];
          if (j < nComp - 1) { out << delimeter; }
        }
        recCount++;

        if (recCount >= MaxValPerLine)
        {
          out << '\n';
          recCount = 0;
        }
        else
        {
          out << delimeter;
        }
      }
    }
};

// Include the MOC generated file for this class
#include "moc_ExportData.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExportData::ExportData() :
  AbstractFilter(),
  m_SelectedDataArrayPaths(QVector<DataArrayPath>()),
  m_OutputPath(""),
  m_Delimeter(0),
  m_FileExtension(".txt"),
  m_MaxValPerLine(-1)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExportData::~ExportData()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExportData::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(OutputPathFilterParameter::New("Output Path", "OutputPath", getOutputPath(), FilterParameter::Parameter));
  parameters.push_back(StringFilterParameter::New("File Extension", "FileExtension", getFileExtension(), FilterParameter::Parameter));
  parameters.push_back(IntFilterParameter::New("Maximum Tuples Per Line", "MaxValPerLine", getMaxValPerLine(), FilterParameter::Parameter));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();  //Delimiter choice
    parameter->setHumanLabel("Delimiter");
    parameter->setPropertyName("Delimeter");

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
  {
    MultiDataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(MultiDataArraySelectionFilterParameter::New("Attribute Arrays to Export", "SelectedDataArrayPaths", getSelectedDataArrayPaths(), FilterParameter::RequiredArray, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExportData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedDataArrayPaths(reader->readDataArrayPathVector("SelectedDataArrayPaths", getSelectedDataArrayPaths()));
  setOutputPath(reader->readString("OutputPath", getOutputPath()));
  setDelimeter(reader->readValue("Delimeter", getDelimeter()));
  setFileExtension(reader->readString("FileExtension", getFileExtension()));
  setMaxValPerLine(reader->readValue("MaxValPerLine", getMaxValPerLine()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ExportData::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(SelectedDataArrayPaths)
  SIMPL_FILTER_WRITE_PARAMETER(OutputPath)
  SIMPL_FILTER_WRITE_PARAMETER(Delimeter)
  SIMPL_FILTER_WRITE_PARAMETER(FileExtension)
  SIMPL_FILTER_WRITE_PARAMETER(MaxValPerLine)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExportData::dataCheck()
{
  // Make sure the weak pointer vector is cleared before we begin...
  m_SelectedWeakPtrVector.clear();

  setErrorCondition(0);

  if (m_SelectedDataArrayPaths.isEmpty() == true)
  {
    setErrorCondition(-11001);
    QString ss = QObject::tr("At least one Attribute Array must be selected");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if (m_OutputPath.isEmpty() == true)
  {
    setErrorCondition(-11002);
    QString ss = QObject::tr("The output path must be set");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if (m_MaxValPerLine <= 0)
  {
    setErrorCondition(-11003);
    QString ss = QObject::tr("The Maximum Tuples Per Line (%1) must be positive").arg(m_MaxValPerLine);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QVector<DataArrayPath> paths = getSelectedDataArrayPaths();

  if (DataArrayPath::ValidateVector(paths) == false)
  {
    setErrorCondition(-11004);
    QString ss = QObject::tr("There are Attribute Arrays selected that are not contained in the same Attribute Matrix. All selected Attribute Arrays must belong to the same Attribute Matrix");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  for (int32_t i = 0; i < paths.count(); i++)
  {
    DataArrayPath path = paths.at(i);
    IDataArray::WeakPointer ptr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, path);
    m_SelectedWeakPtrVector.push_back(ptr);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExportData::preflight()
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
void ExportData::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  if (m_SelectedDataArrayPaths.count() != m_SelectedWeakPtrVector.count())
  {
    QString ss = QObject::tr("The number of selected Attribute Arrays does not equal the number of internal weak pointers");
    setErrorCondition(-11008);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QString ss;
  QDir dir;
  if (!dir.mkpath(m_OutputPath))
  {
    ss = QObject::tr("Error creating output path '%1'").arg(m_OutputPath);
    setErrorCondition(-11004);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if (m_FileExtension.startsWith(".") == false) // if no '.', add '.' to file extension
  {
    m_FileExtension = "." + m_FileExtension;
  }

  for (int32_t i = 0; i < m_SelectedWeakPtrVector.count(); i++)
  {
    IDataArray::WeakPointer selectedArrayPtr = m_SelectedWeakPtrVector.at(i);

    QString message = QObject::tr("|| Exporting Dataset '%1'").arg(selectedArrayPtr.lock()->getName());
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), message);


    QString exportArrayFile = m_OutputPath + QDir::separator() + selectedArrayPtr.lock()->getName() + m_FileExtension; // the complete output file path, name and extension

    char delimeter = lookupDelimeter();

    EXECUTE_TEMPLATE(this, ExportDataPrivate, selectedArrayPtr.lock(), this, selectedArrayPtr.lock(), delimeter, exportArrayFile, m_MaxValPerLine)

    if (getErrorCondition() < 0) { break; }
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char ExportData::lookupDelimeter()
{
  char del = ' ';
  switch (m_Delimeter)
  {
    case Comma:
      del = ',';
      break;
    case Semicolon:
      del = ';';
      break;
    case Space:
      del = ' ';
      break;
    case Colon:
      del = ':';
      break;
    case Tab:
      del = '\t';
      break;
    default:
      del = ' ';
  }
  return del;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ExportData::newFilterInstance(bool copyFilterParameters)
{
  ExportData::Pointer filter = ExportData::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExportData::getCompiledLibraryName()
{ return IOConstants::IOBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExportData::getGroupName()
{ return DREAM3D::FilterGroups::IOFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExportData::getSubGroupName()
{ return DREAM3D::FilterSubGroups::OutputFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExportData::getHumanLabel()
{ return "Export Data (ASCII Text)"; }
