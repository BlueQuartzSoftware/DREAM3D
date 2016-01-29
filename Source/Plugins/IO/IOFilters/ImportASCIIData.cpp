/*
 * Your License or Copyright can go here
 */

#include "ImportASCIIData.h"

#include <QtCore/QFileInfo>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"

#include "FilterParameters/ImportASCIIDataFilterParameter.h"

#include "IO/IOConstants.h"

#include "Widgets/ImportASCIIDataWizard/AbstractDataParser.hpp"
#include "Widgets/ImportASCIIDataWizard/ImportASCIIDataWizard.h"

// Include the MOC generated file for this class
#include "moc_ImportASCIIData.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportASCIIData::ImportASCIIData() :
  AbstractFilter()
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportASCIIData::~ImportASCIIData()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportASCIIData::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(ImportASCIIDataFilterParameter::New("ASCII Wizard Data", "WizardData", "", FilterParameter::Parameter));

  {
    AttributeMatrixSelectionFilterParameter::RequirementType req;
    parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Attribute Matrix", "AttributeMatrixPath", getAttributeMatrixPath(), FilterParameter::Parameter, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportASCIIData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);

  QString prefix = "Wizard_";

  ASCIIWizardData data;

  QString delimitersStr = reader->readString(prefix + "Delimiters", "");
  QList<char> delimiters;
  for (int i = 0; i < delimitersStr.size(); i++)
  {
    delimiters.push_back(delimitersStr[i].toLatin1());
  }

  data.delimiters = delimiters;
  data.beginIndex = reader->readValue(prefix + "BeginIndex", -1);
  data.consecutiveDelimiters = reader->readValue(prefix + "ConsecutiveDelimiters", false);
  data.dataHeaders = reader->readStringList(prefix + "DataHeaders", QStringList());
  data.dataTypes = reader->readStringList(prefix + "DataTypes", QStringList());
  data.inputFilePath = reader->readString(prefix + "InputFilePath", "");
  data.isFixedWidth = reader->readValue(prefix + "IsFixedWidth", false);
  data.numberOfLines = reader->readValue(prefix + "NumberOfLines", -1);

  QVector<uint64_t> tmpVec;
  QVector<size_t> tDims;
  tmpVec = reader->readArray(prefix + "TupleDims", QVector<uint64_t>());
  for (int i = 0; i < tmpVec.size(); i++) 
  {
    tDims.push_back(static_cast<size_t>(tmpVec[i]));
  }
  data.tupleDims = tDims;

  setWizardData(data);
  setAttributeMatrixPath(reader->readDataArrayPath("AttributeMatrixPath", getAttributeMatrixPath()));

  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImportASCIIData::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);

  QString prefix = "Wizard_";

  QString delimitersStr = "";
  for (int i = 0; i < m_WizardData.delimiters.size(); i++)
  {
    delimitersStr.append(m_WizardData.delimiters[i]);
  }

  writer->writeValue(prefix + "Delimiters", delimitersStr);
  writer->writeValue(prefix + "BeginIndex", m_WizardData.beginIndex);
  writer->writeValue(prefix + "ConsecutiveDelimiters", m_WizardData.consecutiveDelimiters);
  writer->writeValue(prefix + "DataHeaders", m_WizardData.dataHeaders);
  writer->writeValue(prefix + "DataTypes", m_WizardData.dataTypes);
  writer->writeValue(prefix + "InputFilePath", m_WizardData.inputFilePath);
  writer->writeValue(prefix + "IsFixedWidth", m_WizardData.isFixedWidth);
  writer->writeValue(prefix + "NumberOfLines", m_WizardData.numberOfLines);
  QVector<uint64_t> tDims;
  QVector<size_t> tmpVec = m_WizardData.tupleDims;

  for (int i = 0; i < tmpVec.size(); i++)
  {
    tDims.push_back(static_cast<uint64_t>(tmpVec[i]));
  }

  writer->writeValue(prefix + "TupleDims", tDims);

  SIMPL_FILTER_WRITE_PARAMETER(AttributeMatrixPath)

  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportASCIIData::dataCheck()
{
  setErrorCondition(0);
  m_ASCIIArrayMap.clear();

  ASCIIWizardData wizardData = getWizardData();
  if (wizardData.isEmpty() == true)
  {
    QString ss = "A file has not been chosen to import.  Please pick a file to import.";
    setErrorCondition(EMPTY_FILE);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QString inputFilePath = wizardData.inputFilePath;
  QStringList headers = wizardData.dataHeaders;
  QStringList dataTypes = wizardData.dataTypes;
//  int numLines = wizardData.numberOfLines;
//  int beginIndex = wizardData.beginIndex;
  QVector<size_t> tDims = wizardData.tupleDims;
  QVector<size_t> cDims(1, 1);

  QFileInfo fi(inputFilePath);

  AttributeMatrix::Pointer am = getDataContainerArray()->getAttributeMatrix(m_AttributeMatrixPath);
  if (NULL == am.get())
  {
    QString ss = "The attribute matrix input is empty.  Please select an attribute matrix.";
    setErrorCondition(EMPTY_ATTR_MATRIX);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  else if (am->getTupleDimensions() != tDims)
  {
    QString ss = "The attribute matrix '" + m_AttributeMatrixPath.getAttributeMatrixName() + "' does not have the same tuple dimensions as the data in the file '" + fi.fileName() + "'.";
    setErrorCondition(INCONSISTENT_TUPLES);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QStringList amArrays = am->getAttributeArrayNames();
  for (int i = 0; i < amArrays.size(); i++)
  {
    QString amArrayName = amArrays[i];
    for (int j = 0; j < headers.size(); j++)
    {
      QString headerName = headers[j];
      if (amArrayName == headerName)
      {
        QString ss = "The header name \"" + headerName + "\" matches an array name that already exists in the selected attribute matrix.";
        setErrorCondition(DUPLICATE_NAMES);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }
    }
  }

  // Create the arrays
  QList<AbstractDataParser::Pointer> dataParsers;
  for (int i = 0; i < dataTypes.size(); i++)
  {
    QString dataType = dataTypes[i];
    QString name = headers[i];

    DataArrayPath arrayPath = m_AttributeMatrixPath;
    arrayPath.setDataArrayName(name);

    if (dataType == SIMPL::TypeNames::Double)
    {
      DoubleArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DoubleArrayType, AbstractFilter>(this, arrayPath, 0, cDims);
      m_ASCIIArrayMap.insert(i, ptr);
    }
    else if (dataType == SIMPL::TypeNames::Float)
    {
      FloatArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<FloatArrayType, AbstractFilter>(this, arrayPath, 0, cDims);
      m_ASCIIArrayMap.insert(i, ptr);
    }
    else if (dataType == SIMPL::TypeNames::Int8)
    {
      Int8ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<Int8ArrayType, AbstractFilter>(this, arrayPath, 0, cDims);
      m_ASCIIArrayMap.insert(i, ptr);
    }
    else if (dataType == SIMPL::TypeNames::Int16)
    {
      Int16ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<Int16ArrayType, AbstractFilter>(this, arrayPath, 0, cDims);
      m_ASCIIArrayMap.insert(i, ptr);
    }
    else if (dataType == SIMPL::TypeNames::Int32)
    {
      Int32ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<Int32ArrayType, AbstractFilter>(this, arrayPath, 0, cDims);
      m_ASCIIArrayMap.insert(i, ptr);
    }
    else if (dataType == SIMPL::TypeNames::Int64)
    {
      Int64ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<Int64ArrayType, AbstractFilter>(this, arrayPath, 0, cDims);
      m_ASCIIArrayMap.insert(i, ptr);
    }
    else if (dataType == SIMPL::TypeNames::UInt8)
    {
      UInt8ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<UInt8ArrayType, AbstractFilter>(this, arrayPath, 0, cDims);
      m_ASCIIArrayMap.insert(i, ptr);
    }
    else if (dataType == SIMPL::TypeNames::UInt16)
    {
      UInt16ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<UInt16ArrayType, AbstractFilter>(this, arrayPath, 0, cDims);
      m_ASCIIArrayMap.insert(i, ptr);
    }
    else if (dataType == SIMPL::TypeNames::UInt32)
    {
      UInt32ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<UInt32ArrayType, AbstractFilter>(this, arrayPath, 0, cDims);
      m_ASCIIArrayMap.insert(i, ptr);
    }
    else if (dataType == SIMPL::TypeNames::UInt64)
    {
      UInt64ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<UInt64ArrayType, AbstractFilter>(this, arrayPath, 0, cDims);
      m_ASCIIArrayMap.insert(i, ptr);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportASCIIData::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportASCIIData::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  ASCIIWizardData wizardData = getWizardData();
  QString inputFilePath = wizardData.inputFilePath;
  QStringList headers = wizardData.dataHeaders;
  QStringList dataTypes = wizardData.dataTypes;
  QList<char> delimiters = wizardData.delimiters;
  bool isFixedWidth = wizardData.isFixedWidth;
  bool consecutiveDelimiters = wizardData.consecutiveDelimiters;
  int numLines = wizardData.numberOfLines;
  int beginIndex = wizardData.beginIndex;

  QFileInfo fi(inputFilePath);
  QString fileName = fi.fileName();

  QList<AbstractDataParser::Pointer> dataParsers;
  for (int i = 0; i < headers.size(); i++)
  {
    QString dataType = dataTypes[i];
    QString name = headers[i];

    if (dataType == SIMPL::TypeNames::Double)
    {
      DoubleArrayType::Pointer data = std::dynamic_pointer_cast<DoubleArrayType>(m_ASCIIArrayMap.value(i));
      DoubleParserType::Pointer parser = DoubleParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
    else if (dataType == SIMPL::TypeNames::Float)
    {
      FloatArrayType::Pointer data = std::dynamic_pointer_cast<FloatArrayType>(m_ASCIIArrayMap.value(i));
      FloatParserType::Pointer parser = FloatParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
    else if (dataType == SIMPL::TypeNames::Int8)
    {
      Int8ArrayType::Pointer data = std::dynamic_pointer_cast<Int8ArrayType>(m_ASCIIArrayMap.value(i));
      Int8ParserType::Pointer parser = Int8ParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
    else if (dataType == SIMPL::TypeNames::Int16)
    {
      Int16ArrayType::Pointer data = std::dynamic_pointer_cast<Int16ArrayType>(m_ASCIIArrayMap.value(i));
      Int16ParserType::Pointer parser = Int16ParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
    else if (dataType == SIMPL::TypeNames::Int32)
    {
      Int32ArrayType::Pointer data = std::dynamic_pointer_cast<Int32ArrayType>(m_ASCIIArrayMap.value(i));
      Int32ParserType::Pointer parser = Int32ParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
    else if (dataType == SIMPL::TypeNames::Int64)
    {
      Int64ArrayType::Pointer data = std::dynamic_pointer_cast<Int64ArrayType>(m_ASCIIArrayMap.value(i));
      Int64ParserType::Pointer parser = Int64ParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
    else if (dataType == SIMPL::TypeNames::UInt8)
    {
      UInt8ArrayType::Pointer data = std::dynamic_pointer_cast<UInt8ArrayType>(m_ASCIIArrayMap.value(i));
      UInt8ParserType::Pointer parser = UInt8ParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
    else if (dataType == SIMPL::TypeNames::UInt16)
    {
      UInt16ArrayType::Pointer data = std::dynamic_pointer_cast<UInt16ArrayType>(m_ASCIIArrayMap.value(i));
      UInt16ParserType::Pointer parser = UInt16ParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
    else if (dataType == SIMPL::TypeNames::UInt32)
    {
      UInt32ArrayType::Pointer data = std::dynamic_pointer_cast<UInt32ArrayType>(m_ASCIIArrayMap.value(i));
      UInt32ParserType::Pointer parser = UInt32ParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
    else if (dataType == SIMPL::TypeNames::UInt64)
    {
      UInt64ArrayType::Pointer data = std::dynamic_pointer_cast<UInt64ArrayType>(m_ASCIIArrayMap.value(i));
      UInt64ParserType::Pointer parser = UInt64ParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
  }

  int insertIndex = 0;

  QFile inputFile(inputFilePath);
  if (inputFile.open(QIODevice::ReadOnly))
  {
    QTextStream in(&inputFile);

    for (int i = 1; i < beginIndex; i++)
    {
      // Skip to the first data line
      in.readLine();
    }

    float threshold = 0.0f;
    size_t numTuples = numLines - beginIndex + 1;

    for (int lineNum = beginIndex; lineNum <= numLines; lineNum++)
    {
      QString line = in.readLine();
      QStringList tokens = ImportASCIIDataWizard::TokenizeLine(line, delimiters, isFixedWidth, consecutiveDelimiters);

      if (dataTypes.size() != tokens.size())
      {
        QString ss = "Line " + QString::number(lineNum) + " has an inconsistent number of columns.";
        setErrorCondition(INCONSISTENT_COLS);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }

      for (int i = 0; i < dataParsers.size(); i++)
      {
        AbstractDataParser::Pointer parser = dataParsers[i];
        int index = parser->getColumnIndex();
        QString name = parser->getColumnName();

        ParserFunctor::ErrorObject obj = parser->parse(tokens[index], insertIndex);
        if (!obj.ok)
        {
          QString dataType = parser->getDataArray()->getTypeAsString();
          QString errorMessage = obj.errorMessage;
          QString ss = errorMessage + "(line " + QString::number(lineNum) + ", column " + QString::number(index) + ").";
          setErrorCondition(CONVERSION_FAILURE);
          notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
          return;
        }
      }

      if (((float)lineNum / numTuples) * 100.0f > threshold)
      {
        // Print the status of the import
        QString ss = QObject::tr("Importing ASCII Data || %1% Complete").arg(((float)lineNum / numTuples) * 100.0f, 0, 'f', 0);
        notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
        threshold = threshold + 5.0f;
        if (threshold < ((float)lineNum / numTuples) * 100.0f)
        {
          threshold = ((float)lineNum / numTuples) * 100.0f;
        }
      }

      if (getCancel() == true) { return; }

      insertIndex++;

    }
    inputFile.close();
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ImportASCIIData::newFilterInstance(bool copyFilterParameters)
{
  ImportASCIIData::Pointer filter = ImportASCIIData::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportASCIIData::getCompiledLibraryName()
{ return IOConstants::IOBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportASCIIData::getGroupName()
{ return SIMPL::FilterGroups::IOFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportASCIIData::getSubGroupName()
{ return SIMPL::FilterSubGroups::InputFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportASCIIData::getHumanLabel()
{ return "Import ASCII Data"; }

