/*
 * Your License or Copyright can go here
 */

#include "ImportASCIIData.h"

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
  m_ASCIIArrays.clear();

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
      m_ASCIIArrays.push_back(ptr);
    }
    if (dataType == "Float")
    {
      FloatArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<FloatArrayType, AbstractFilter>(this, arrayPath, 0, cDims);
      m_ASCIIArrays.push_back(ptr);
    }
    else if (dataType == "Int8")
    {
      Int8ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<Int8ArrayType, AbstractFilter>(this, arrayPath, 0, cDims);
      m_ASCIIArrays.push_back(ptr);
    }
    else if (dataType == "Int16")
    {
      Int16ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<Int16ArrayType, AbstractFilter>(this, arrayPath, 0, cDims);
      m_ASCIIArrays.push_back(ptr);
    }
    else if (dataType == "Int32")
    {
      Int32ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<Int32ArrayType, AbstractFilter>(this, arrayPath, 0, cDims);
      m_ASCIIArrays.push_back(ptr);
    }
    else if (dataType == "Int64")
    {
      Int64ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<Int64ArrayType, AbstractFilter>(this, arrayPath, 0, cDims);
      m_ASCIIArrays.push_back(ptr);
    }
    else if (dataType == "UInt8")
    {
      UInt8ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<UInt8ArrayType, AbstractFilter>(this, arrayPath, 0, cDims);
      m_ASCIIArrays.push_back(ptr);
    }
    else if (dataType == "UInt16")
    {
      UInt16ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<UInt16ArrayType, AbstractFilter>(this, arrayPath, 0, cDims);
      m_ASCIIArrays.push_back(ptr);
    }
    else if (dataType == "UInt32")
    {
      UInt32ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<UInt32ArrayType, AbstractFilter>(this, arrayPath, 0, cDims);
      m_ASCIIArrays.push_back(ptr);
    }
    else if (dataType == "UInt64")
    {
      UInt64ArrayType::Pointer ptr = getDataContainerArray()->createNonPrereqArrayFromPath<UInt64ArrayType, AbstractFilter>(this, arrayPath, 0, cDims);
      m_ASCIIArrays.push_back(ptr);
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

  //Int32ArrayType::Pointer data = Int32ArrayType::CreateArray(0, name, false);
  //Int32ParserType::Pointer parser = Int32ParserType::New(data, name, i);
  //dataParsers.push_back(parser);

  //QStringList lines = ImportASCIIDataWizard::ReadLines(inputFilePath, beginLineNum, numOfDataLines);
  //QList<QStringList> tokenizedLines = ImportASCIIDataWizard::TokenizeLines(lines, m_ImportWizard->getHasFixedWidth(), m_ImportWizard->getTabAsDelimiter(), m_ImportWizard->getSemicolonAsDelimiter(), m_ImportWizard->getCommaAsDelimiter(), m_ImportWizard->getSpaceAsDelimiter(), m_ImportWizard->getConsecutiveDelimiters());

  //for (int row = 0; row < tokenizedLines.size(); row++)
  //{
  //  QStringList tokens = tokenizedLines[row];
  //  if (dataTypes.size() != tokens.size())
  //  {
  //    // Throw an error
  //  }

  //  for (int i = 0; i < dataParsers.size(); i++)
  //  {
  //    IO::AbstractDataParser::Pointer parser = dataParsers[i];
  //    int index = parser->getColumnIndex();
  //    QString name = parser->getColumnName();

  //    parser->parse(tokens[index], row);
  //  }
  //}

  if (getErrorCondition() < 0)
  {
    QString ss = QObject::tr("Some error message");
    setErrorCondition(-99999999);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
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

