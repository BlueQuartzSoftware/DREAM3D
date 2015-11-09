/*
 * Your License or Copyright can go here
 */

#include "ImportASCIIData.h"

#include <QtCore/QFileInfo>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"

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

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportASCIIData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);

  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImportASCIIData::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);

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
    return;
  }

  QStringList headers = wizardData.dataHeaders;
  QStringList dataTypes = wizardData.dataTypes;
  int numLines = wizardData.numberOfLines;

  QVector<size_t> tDims(1, numLines);
  QVector<size_t> cDims(1, 1);
  DataContainer::Pointer dc = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, "ImportDataContainer");
  AttributeMatrix::Pointer am = dc->createNonPrereqAttributeMatrix<AbstractFilter>(this, "ImportAttributeMatrix", tDims, DREAM3D::AttributeMatrixType::Cell);

  // Create the arrays
  QList<AbstractDataParser::Pointer> dataParsers;
  DataArrayPath arrayPath(dc->getName(), am->getName(), "");
  for (int i = 0; i < dataTypes.size(); i++)
  {
    QString dataType = dataTypes[i];
    QString name = headers[i];

    arrayPath.setDataArrayName(name);

    if (dataType == "Double")
    {
      DoubleArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<DoubleArrayType, AbstractFilter>(this, arrayPath, 0, cDims);
      m_ASCIIArrayMap.insert(i, ptr);
    }
    else if (dataType == "Float")
    {
      FloatArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<FloatArrayType, AbstractFilter>(this, arrayPath, 0, cDims);
      m_ASCIIArrayMap.insert(i, ptr);
    }
    else if (dataType == "Int8")
    {
      Int8ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<Int8ArrayType, AbstractFilter>(this, arrayPath, 0, cDims);
      m_ASCIIArrayMap.insert(i, ptr);
    }
    else if (dataType == "Int16")
    {
      Int16ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<Int16ArrayType, AbstractFilter>(this, arrayPath, 0, cDims);
      m_ASCIIArrayMap.insert(i, ptr);
    }
    else if (dataType == "Int32")
    {
      Int32ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<Int32ArrayType, AbstractFilter>(this, arrayPath, 0, cDims);
      m_ASCIIArrayMap.insert(i, ptr);
    }
    else if (dataType == "Int64")
    {
      Int64ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<Int64ArrayType, AbstractFilter>(this, arrayPath, 0, cDims);
      m_ASCIIArrayMap.insert(i, ptr);
    }
    else if (dataType == "UInt8")
    {
      UInt8ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<UInt8ArrayType, AbstractFilter>(this, arrayPath, 0, cDims);
      m_ASCIIArrayMap.insert(i, ptr);
    }
    else if (dataType == "UInt16")
    {
      UInt16ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<UInt16ArrayType, AbstractFilter>(this, arrayPath, 0, cDims);
      m_ASCIIArrayMap.insert(i, ptr);
    }
    else if (dataType == "UInt32")
    {
      UInt32ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<UInt32ArrayType, AbstractFilter>(this, arrayPath, 0, cDims);
      m_ASCIIArrayMap.insert(i, ptr);
    }
    else if (dataType == "UInt64")
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

  int numOfDataItems = numLines - beginIndex;
  QList<AbstractDataParser::Pointer> dataParsers;
  for (int i = 0; i < headers.size(); i++)
  {
    QString dataType = dataTypes[i];
    QString name = headers[i];

    if (dataType == "Double")
    {
      DoubleArrayType::Pointer data = boost::dynamic_pointer_cast<DoubleArrayType>(m_ASCIIArrayMap.value(i));
      DoubleParserType::Pointer parser = DoubleParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
    else if (dataType == "Float")
    {
      FloatArrayType::Pointer data = boost::dynamic_pointer_cast<FloatArrayType>(m_ASCIIArrayMap.value(i));
      FloatParserType::Pointer parser = FloatParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
    else if (dataType == "Int8")
    {
      Int8ArrayType::Pointer data = boost::dynamic_pointer_cast<Int8ArrayType>(m_ASCIIArrayMap.value(i));
      Int8ParserType::Pointer parser = Int8ParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
    else if (dataType == "Int16")
    {
      Int16ArrayType::Pointer data = boost::dynamic_pointer_cast<Int16ArrayType>(m_ASCIIArrayMap.value(i));
      Int16ParserType::Pointer parser = Int16ParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
    else if (dataType == "Int32")
    {
      Int32ArrayType::Pointer data = boost::dynamic_pointer_cast<Int32ArrayType>(m_ASCIIArrayMap.value(i));
      Int32ParserType::Pointer parser = Int32ParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
    else if (dataType == "Int64")
    {
      Int64ArrayType::Pointer data = boost::dynamic_pointer_cast<Int64ArrayType>(m_ASCIIArrayMap.value(i));
      Int64ParserType::Pointer parser = Int64ParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
    else if (dataType == "UInt8")
    {
      UInt8ArrayType::Pointer data = boost::dynamic_pointer_cast<UInt8ArrayType>(m_ASCIIArrayMap.value(i));
      UInt8ParserType::Pointer parser = UInt8ParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
    else if (dataType == "UInt16")
    {
      UInt16ArrayType::Pointer data = boost::dynamic_pointer_cast<UInt16ArrayType>(m_ASCIIArrayMap.value(i));
      UInt16ParserType::Pointer parser = UInt16ParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
    else if (dataType == "UInt32")
    {
      UInt32ArrayType::Pointer data = boost::dynamic_pointer_cast<UInt32ArrayType>(m_ASCIIArrayMap.value(i));
      UInt32ParserType::Pointer parser = UInt32ParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
    else if (dataType == "UInt64")
    {
      UInt64ArrayType::Pointer data = boost::dynamic_pointer_cast<UInt64ArrayType>(m_ASCIIArrayMap.value(i));
      UInt64ParserType::Pointer parser = UInt64ParserType::New(data, name, i);
      dataParsers.push_back(parser);
    }
  }

  int insertIndex = 0;

  QFile inputFile(inputFilePath);
  if (inputFile.open(QIODevice::ReadOnly))
  {
    QTextStream in(&inputFile);
    
    float threshold = 0.0f;
    size_t numTuples = numLines - beginIndex + 1;

    for (int lineNum = 0; lineNum < beginIndex + numLines - 1; lineNum++)
    {
      while (lineNum < beginIndex - 1)
      {
        // Skip to the first data line
        QString line = in.readLine();
        lineNum++;
      }

      QString line = in.readLine();
      QStringList tokens = ImportASCIIDataWizard::TokenizeLine(line, delimiters, isFixedWidth, consecutiveDelimiters);

      if (dataTypes.size() != tokens.size())
      {
        QString ss = "Line " + QString::number(lineNum+1) + " has an inconsistent number of columns.";
        setErrorCondition(-100);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }

      for (int i = 0; i < dataParsers.size(); i++)
      {
        AbstractDataParser::Pointer parser = dataParsers[i];
        int index = parser->getColumnIndex();
        QString name = parser->getColumnName();
        QString token = tokens[index];
        QByteArray tokenByteArray = QByteArray::fromStdString(token.toStdString());

        parser->parse(tokenByteArray, insertIndex);
      }
      
      if (((float)lineNum / numTuples) * 100.0f > threshold)
      {
        // Print the status of the import
        QString ss = QObject::tr("Importing ASCII Data || %1% Complete").arg(((float)lineNum / numTuples) * 100.0f);
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
{ return DREAM3D::FilterGroups::Unsupported; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportASCIIData::getSubGroupName()
{ return "IO"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportASCIIData::getHumanLabel()
{ return "Import ASCII Data"; }

