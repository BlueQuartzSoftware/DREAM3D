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


#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"

#include "IO/IOFilters/ImportASCIIData.h"
#include "IO/Widgets/ImportASCIIDataWizard/ASCIIWizardData.hpp"

#include "IOTestFileLocations.h"

const QString DataContainerName = "DataContainer";
const QString AttributeMatrixName = "AttributeMatrix";
const QString DataArrayName = "Array1";

const QVector<QString> inputIntVector({ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" });
const QVector<QString> inputHexVector({ "0x01", "0x02", "0x03", "0x04", "0x05", "0x06", "0x07", "0x08", "0x09", "0x0A" });
const QVector<QString> inputOctVector({ "001", "002", "003", "004", "005", "006", "007", "008", "009", "010" });
const QVector<QString> inputDoubleVector({ "1.5", "2.2", "3.65", "4.34", "5.76", "6.534", "7.0", "8.342", "9.8723", "10.89" });
const QVector<QString> inputCharErrorVector({ "sdstrg", "2.2", "3.65", "&(^$#", "5.76", "sjtyr", "7.0", "8.342", "&*^#F", "youikjhgf" });
const QVector<QString> inputScientificNotation({ "0.15e1", "2.2e0", "3.65", "43.4e-1", "0.576e1", "653.4e-2", "7.0", "8.342", "9.8723", "10.89" });
const QVector<int> outputIntVector({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
const QVector<double> outputDoubleVector({ 1.5, 2.2, 3.65, 4.34, 5.76, 6.534, 7.0, 8.342, 9.8723, 10.89 });
const QVector<double> outputIntAsDoubleVector({ 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0 });
const QVector<QString> mixVector({ "1", "2.32", "3.84", "4", "5", "6.79", "7", "8", "9", "10.65" });

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  QFile::remove(UnitTest::ImportASCIIDataTest::TestFile1);
  QFile::remove(UnitTest::ImportASCIIDataTest::TestFile2);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestFilterAvailability()
{
	// Now instantiate the ImportASCIIData Filter from the FilterManager
	QString filtName = "ImportASCIIData";
	FilterManager* fm = FilterManager::Instance();
	IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
	if (NULL == filterFactory.get())
	{
		std::stringstream ss;
		ss << "The ImportASCIIDataTest Requires the use of the " << filtName.toStdString() << " filter which is found in the IO Plugin";
		DREAM3D_TEST_THROW_EXCEPTION(ss.str())
	}
	return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateFile(const QString &filePath, QVector<QString> values, char delimiter)
{
  QFile data(filePath);
  if (data.open(QFile::WriteOnly))
  {
    QTextStream out(&data);

    for (int row = 0; row < values.size(); row++)
    {
      out << values[row];
      if (row + 1 < values.size())
      {
        out << "\n";
      }
    }
    data.close();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateFile(const QString &filePath, QVector<QVector<QString> > values, char delimiter)
{
  QFile data(filePath);
  if (data.open(QFile::WriteOnly))
  {
    QTextStream out(&data);

    for (int col = 0; col < values.size(); col++)
    {
      for (int row = 0; row < values[col].size(); row++)
      {
        out << values[row][col];
        if (col + 1 < values[row].size())
        {
          out << delimiter;
        }
      }

      out << "\n";
    }
    data.close();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer PrepFilter(ASCIIWizardData data)
{
  DataContainerArray::Pointer dca = DataContainerArray::New();
  DataContainer::Pointer dc = DataContainer::New(DataContainerName);
  AttributeMatrix::Pointer am = AttributeMatrix::New(QVector<size_t>(1, data.numberOfLines), AttributeMatrixName, 3);
  dc->addAttributeMatrix(AttributeMatrixName, am);
  dca->addDataContainer(dc);

  // Now instantiate the DxWriter Filter from the FilterManager
  QString filtName = "ImportASCIIData";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
  if (NULL != filterFactory.get())
  {
    // If we get this far, the Factory is good so creating the filter should not fail unless something has
    // horribly gone wrong in which case the system is going to come down quickly after this.
    AbstractFilter::Pointer importASCIIData = filterFactory->create();
    importASCIIData->preflight();

    int err = importASCIIData->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, ImportASCIIData::EMPTY_FILE)

    QVariant var;
    var.setValue(data);
    bool propWasSet = importASCIIData->setProperty("WizardData", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    importASCIIData->preflight();
    err = importASCIIData->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, ImportASCIIData::EMPTY_ATTR_MATRIX)

    var.setValue(DataArrayPath(dc->getName(), am->getName(), ""));
    propWasSet = importASCIIData->setProperty("AttributeMatrixPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    importASCIIData->setDataContainerArray(dca);

    return importASCIIData;
  }

  return AbstractFilter::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename I, typename O>
void ConvertTypes()
{
  char delimiter = '\t';

  DataArray<I>::Pointer input = DataArray<I>::CreateArray(1, "InputArray", false);
  QString inputType = input->getTypeAsString();

  DataArray<O>::Pointer output = DataArray<O>::CreateArray(1, "OutputArray", false);
  QString outputType = output->getTypeAsString();

  // Test Using Expected Input
  {
    QVector<QString> inputVector;
    if (inputType == DREAM3D::TypeNames::Double || inputType == DREAM3D::TypeNames::Float)
    {
      inputVector = inputDoubleVector;
      CreateFile(UnitTest::ImportASCIIDataTest::TestFile1, inputDoubleVector, delimiter);
    }
    else
    {
      inputVector = inputIntVector;
      CreateFile(UnitTest::ImportASCIIDataTest::TestFile1, inputIntVector, delimiter);
    }

    ASCIIWizardData data;
    data.beginIndex = 1;
    data.consecutiveDelimiters = false;
    data.dataHeaders.push_back(DataArrayName);
    data.dataTypes.push_back(outputType);
    data.delimiters.push_back(delimiter);
    data.inputFilePath = UnitTest::ImportASCIIDataTest::TestFile1;
    data.isFixedWidth = false;
    data.numberOfLines = 10;
    AbstractFilter::Pointer importASCIIData = PrepFilter(data);
    DREAM3D_REQUIRE_NE(importASCIIData.get(), NULL)

      importASCIIData->execute();
    int err = importASCIIData->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, 0)

      AttributeMatrix::Pointer am = importASCIIData->getDataContainerArray()->getAttributeMatrix(DataArrayPath(DataContainerName, AttributeMatrixName, ""));
    DataArray<O>::Pointer results = boost::dynamic_pointer_cast<DataArray<O> >(am->getAttributeArray(DataArrayName));

    DREAM3D_REQUIRE_EQUAL(results->getSize(), inputVector.size())

      O* resultsRaw = results->getPointer(0);
    for (int i = 0; i < results->getSize(); i++)
    {
      if (inputType == DREAM3D::TypeNames::Double || inputType == DREAM3D::TypeNames::Float)
      {
        if (outputType == DREAM3D::TypeNames::Double || outputType == DREAM3D::TypeNames::Float)
        {
          long absValue = abs(outputDoubleVector[i] - resultsRaw[i]);
          DREAM3D_REQUIRED(absValue, <, 0.01)
        }
        else
        {
          DREAM3D_REQUIRE_EQUAL(resultsRaw[i], outputIntVector[i])
        }
      }
      else
      {
        if (outputType == DREAM3D::TypeNames::Double || outputType == DREAM3D::TypeNames::Float)
        {
          DREAM3D_REQUIRE_EQUAL(resultsRaw[i], outputIntAsDoubleVector[i])
        }
        else
        {
          DREAM3D_REQUIRE_EQUAL(resultsRaw[i], outputIntVector[i])
        }
      }
    }
  }

  RemoveTestFiles();

  // Test Using Unexpected Input - Alphabetical Characters, Special Characters, etc.
  {
    CreateFile(UnitTest::ImportASCIIDataTest::TestFile1, inputCharErrorVector, delimiter);

    ASCIIWizardData data;
    data.beginIndex = 1;
    data.consecutiveDelimiters = false;
    data.dataHeaders.push_back(DataArrayName);
    data.dataTypes.push_back(outputType);
    data.delimiters.push_back(delimiter);
    data.inputFilePath = UnitTest::ImportASCIIDataTest::TestFile1;
    data.isFixedWidth = false;
    data.numberOfLines = 10;
    AbstractFilter::Pointer importASCIIData = PrepFilter(data);
    DREAM3D_REQUIRE_NE(importASCIIData.get(), NULL)

      importASCIIData->execute();
    int err = importASCIIData->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, ImportASCIIData::CONVERSION_FAILURE)
  }

  RemoveTestFiles();

  // Scientific Notation Test - Only if the input is a double or float
  if (outputType == DREAM3D::TypeNames::Double || outputType == DREAM3D::TypeNames::Float)
  {
    CreateFile(UnitTest::ImportASCIIDataTest::TestFile1, inputScientificNotation, delimiter);

    ASCIIWizardData data;
    data.beginIndex = 1;
    data.consecutiveDelimiters = false;
    data.dataHeaders.push_back(DataArrayName);
    data.dataTypes.push_back(outputType);
    data.delimiters.push_back(delimiter);
    data.inputFilePath = UnitTest::ImportASCIIDataTest::TestFile1;
    data.isFixedWidth = false;
    data.numberOfLines = 10;
    AbstractFilter::Pointer importASCIIData = PrepFilter(data);
    DREAM3D_REQUIRE_NE(importASCIIData.get(), NULL)

      importASCIIData->execute();
    int err = importASCIIData->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, 0)

      AttributeMatrix::Pointer am = importASCIIData->getDataContainerArray()->getAttributeMatrix(DataArrayPath(DataContainerName, AttributeMatrixName, ""));
    DataArray<O>::Pointer results = boost::dynamic_pointer_cast<DataArray<O> >(am->getAttributeArray(DataArrayName));

    DREAM3D_REQUIRE_EQUAL(results->getSize(), inputScientificNotation.size())

      O* resultsRaw = results->getPointer(0);
    for (int i = 0; i < results->getSize(); i++)
    {
      if (outputType == DREAM3D::TypeNames::Double || outputType == DREAM3D::TypeNames::Float)
      {
        long absValue = abs(outputDoubleVector[i] - resultsRaw[i]);
        DREAM3D_REQUIRED(absValue, < , 0.01)
      }
      else
      {
        DREAM3D_REQUIRE_EQUAL(resultsRaw[i], outputIntVector[i])
      }
    }
  }

  RemoveTestFiles();

  //// Hexadecimal Test - Only if the input is an integer
  //if (inputType != DREAM3D::TypeNames::Double && inputType != DREAM3D::TypeNames::Float)
  //{
  //  CreateFile(UnitTest::ImportASCIIDataTest::TestFile1, inputHexVector, delimiter);

  //  ASCIIWizardData data;
  //  data.beginIndex = 1;
  //  data.consecutiveDelimiters = false;
  //  data.dataHeaders.push_back(DataArrayName);
  //  data.dataTypes.push_back(outputType);
  //  data.delimiters.push_back(delimiter);
  //  data.inputFilePath = UnitTest::ImportASCIIDataTest::TestFile1;
  //  data.isFixedWidth = false;
  //  data.numberOfLines = 10;
  //  AbstractFilter::Pointer importASCIIData = PrepFilter(data);
  //  DREAM3D_REQUIRE_NE(importASCIIData.get(), NULL)

  //    importASCIIData->execute();
  //  int err = importASCIIData->getErrorCondition();
  //  DREAM3D_REQUIRE_EQUAL(err, 0)

  //    AttributeMatrix::Pointer am = importASCIIData->getDataContainerArray()->getAttributeMatrix(DataArrayPath(DataContainerName, AttributeMatrixName, ""));
  //  DataArray<O>::Pointer results = boost::dynamic_pointer_cast<DataArray<O> >(am->getAttributeArray(DataArrayName));

  //  DREAM3D_REQUIRE_EQUAL(results->getSize(), inputHexVector.size())

  //    O* resultsRaw = results->getPointer(0);
  //  for (int i = 0; i < results->getSize(); i++)
  //  {
  //    if (outputType != DREAM3D::TypeNames::Double && outputType != DREAM3D::TypeNames::Float)
  //    {
  //      DREAM3D_REQUIRE_EQUAL(resultsRaw[i], outputIntVector[i])
  //    }
  //  }
  //}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
void TestConversion()
{
  ConvertTypes<T, int8_t>();
  ConvertTypes<T, int16_t>();
  ConvertTypes<T, int32_t>();
  ConvertTypes<T, int64_t>();
  ConvertTypes<T, uint8_t>();
  ConvertTypes<T, uint16_t>();
  ConvertTypes<T, uint32_t>();
  ConvertTypes<T, uint64_t>();
  ConvertTypes<T, float>();
  ConvertTypes<T, double>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImportASCIIDataTest()
{
  TestConversion<int8_t>();
  TestConversion<int16_t>();
  TestConversion<int32_t>();
  TestConversion<int64_t>();
  TestConversion<uint8_t>();
  TestConversion<uint16_t>();
  TestConversion<uint32_t>();
  TestConversion<uint64_t>();
  TestConversion<float>();
  TestConversion<double>();

  return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void loadFilterPlugins()
{
  // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();
  SIMPLibPluginLoader::LoadPluginFilters(fm);

  // Send progress messages from PipelineBuilder to this object for display
  QMetaObjectUtilities::RegisterMetaTypes();
}

// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  // Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("ImportASCIIDataTest");

  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( loadFilterPlugins() );
  DREAM3D_REGISTER_TEST( TestFilterAvailability() );

  DREAM3D_REGISTER_TEST(RemoveTestFiles())  // In case the previous test asserted or stopped prematurely

  DREAM3D_REGISTER_TEST( ImportASCIIDataTest() )

  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
  PRINT_TEST_SUMMARY();
  return err;
}
