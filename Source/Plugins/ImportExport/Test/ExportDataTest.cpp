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

#include <QtCore/QDir>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/CoreFilters/DataContainerReader.h"
#include "SIMPLib/CoreFilters/DataContainerWriter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "UnitTestSupport.hpp"

#include "ImportExportTestFileLocations.h"

class ExportDataTest
{
public:
  ExportDataTest()
  {
  }
  virtual ~ExportDataTest()
  {
  }
  SIMPL_TYPE_MACRO(ExportDataTest)

  enum ErrorCodes
  {
    NO_ERROR = 0,
    DIFF_MATRICES = -11008
  };

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::ExportDataTest::TestTempDir + QDir::separator() + "ExportTest.dream3d");
    QFile::remove(UnitTest::ExportDataTest::TestTempDir + QDir::separator() + "ExportTest.xdmf");
    QFile::remove(UnitTest::ExportDataTest::TestTempDir + QDir::separator() + "ConfidenceIndex.txt");
    QFile::remove(UnitTest::ExportDataTest::TestTempDir + QDir::separator() + "Phases.txt");
    QFile::remove(UnitTest::ExportDataTest::TestTempDir + QDir::separator() + "ThresholdArray.txt");
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the WriteASCIIData Filter from the FilterManager
    QString filtName = "WriteASCIIData";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The ExportDataTest Requires the use of the " << filtName.toStdString() << " filter which is found in the IO Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestExportDataWriter()
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();

    // A DataContainer that mimics some real data
    DataContainer::Pointer m = DataContainer::New(SIMPL::Defaults::DataContainerName);
    dca->addOrReplaceDataContainer(m);

    AttributeMatrix::Pointer attrMatrix = AttributeMatrix::New(std::vector<size_t>(1, 20), SIMPL::Defaults::AttributeMatrixName, AttributeMatrix::Type::Generic);
    m->addOrReplaceAttributeMatrix(attrMatrix);

    AttributeMatrix::Pointer attrMatrix2 = AttributeMatrix::New(std::vector<size_t>(1, 20), SIMPL::Defaults::CellAttributeMatrixName, AttributeMatrix::Type::Cell);
    m->addOrReplaceAttributeMatrix(attrMatrix2);
    int size = 20;

    {
      Int32ArrayType::Pointer intArray = Int32ArrayType::CreateArray(size, SIMPL::CellData::CellPhases, true);
      for(int i = 0; i < size; ++i) // create an array with values 20 to 39
      {
        intArray->setValue(i, i + 20);
      }
      attrMatrix->insertOrAssign(intArray);
    }
    {
      Int32ArrayType::Pointer intArray = Int32ArrayType::CreateArray(size, SIMPL::CellData::ConfidenceIndexNoSpace, true);
      for(int i = 0; i < size; ++i) // create an array with values 20 to 39
      {
        intArray->setValue(i, i + 20);
      }
      attrMatrix->insertOrAssign(intArray);
    }
    {
      BoolArrayType::Pointer boolArray = BoolArrayType::CreateArray(size, SIMPL::GeneralData::ThresholdArray, true);
      for(int i = 0; i < size; ++i) // create an bool array with true and false values
      {
        if(i % 2 == 0)
        {
          boolArray->setValue(i, 0);
        }
        else
        {
          boolArray->setValue(i, 1);
        }
      }
      attrMatrix->insertOrAssign(boolArray);
    }
    {
      Int32ArrayType::Pointer intArray = Int32ArrayType::CreateArray(size, SIMPL::CellData::ConfidenceIndexNoSpace, true);
      for(int i = 0; i < size; ++i) // create an array with values 20 to 39
      {
        intArray->setValue(i, i + 20);
      }
      attrMatrix2->insertOrAssign(intArray);
    }

    Observer obs;
    // Send progress messages from PipelineBuilder to this object for display
    DataContainerWriter::Pointer writer = DataContainerWriter::New();
    writer->setDataContainerArray(dca);
    QString exportArrayFile = UnitTest::ExportDataTest::TestTempDir + QDir::separator() + "ExportTest.dream3d";
    writer->setOutputFile(exportArrayFile);

    // Since we are NOT using the Pipeline Object to execute the filter but instead we are directly executing the filter
    // and we want to know about any error/warning/progress messages we need to connect the filter to our Observer object
    // manually. Normally the Pipeline Object would do this for us. We are NOT using a Pipeline Object because using the
    // Pipeline Object would over write the DataContainer Array that we have created with a blank one thus defeating the
    // entire purpose of the test.
    QObject::connect(writer.get(), SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), &obs, SLOT(processPipelineMessage(const AbstractMessage::Pointer&)));

    writer->execute();
    int err = writer->getErrorCode();
    DREAM3D_REQUIRE_EQUAL(err, NO_ERROR);

    // Now instantiate the EnsembleInfoReader Filter from the FilterManager
    QString filtName = "WriteASCIIData";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if(nullptr != filterFactory.get())
    {
      // If we get this far, the Factory is good so creating the filter should not fail unless something has
      // horribly gone wrong in which case the system is going to come down quickly after this.
      AbstractFilter::Pointer filter = filterFactory->create();

      QVariant var;
      int err = 0;
      bool propWasSet;
      filter->setDataContainerArray(dca);

      var.setValue(2);
      propWasSet = filter->setProperty("Delimiter", var); // delimiter is a space
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(10);
      propWasSet = filter->setProperty("MaxValPerLine", var); // 10 array elements per line
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(UnitTest::ExportDataTest::TestTempDir);
      propWasSet = filter->setProperty("OutputPath", var); // output file to write array
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      DataArrayPath path1 = DataArrayPath(SIMPL::Defaults::DataContainerName, SIMPL::Defaults::AttributeMatrixName, SIMPL::CellData::CellPhases);

      DataArrayPath path2 = DataArrayPath(SIMPL::Defaults::DataContainerName, SIMPL::Defaults::AttributeMatrixName, SIMPL::CellData::ConfidenceIndexNoSpace);

      DataArrayPath path4 = DataArrayPath(SIMPL::Defaults::DataContainerName, SIMPL::Defaults::AttributeMatrixName, SIMPL::GeneralData::ThresholdArray);

      QVector<DataArrayPath> vector;
      vector.push_back(path1);
      vector.push_back(path2);
      vector.push_back(path4);

      var.setValue(vector);
      propWasSet = filter->setProperty("SelectedDataArrayPaths", var); // arrays just created above
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      filter->execute();
      err = filter->getErrorCode();
      DREAM3D_REQUIRE_EQUAL(err, NO_ERROR);

      DataArrayPath path3 = DataArrayPath(SIMPL::Defaults::DataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::ConfidenceIndexNoSpace);

      vector.push_back(path3);

      var.setValue(vector);
      propWasSet = filter->setProperty("SelectedDataArrayPaths", var); // arrays just created above
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      filter->execute();
      err = filter->getErrorCode();
      DREAM3D_REQUIRE_EQUAL(err, DIFF_MATRICES);
    }
    else
    {
      QString ss = QObject::tr("ExportDataTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
      DREAM3D_REQUIRE_EQUAL(0, 1)
    }
    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestExportDataReader()
  {
    {
      int num = 0;
      FILE* f;
      QString exportArrayFile = UnitTest::ExportDataTest::TestTempDir + QDir::separator() + SIMPL::CellData::CellPhases + ".txt";
      f = fopen(exportArrayFile.toLatin1().data(), "r"); // open file created from array
      DREAM3D_REQUIRE_VALID_POINTER(f)

      for(int i = 0; i < 20; i++) // compare file to what was written in array
      {
        fscanf(f, "%d,", &num);
        if(i + 20 != num)
        {
          DREAM3D_REQUIRE_EQUAL(0, -3)
        }
      }
      fclose(f);
    }
    {
      int num = 0;
      FILE* f;
      QString exportArrayFile = UnitTest::ExportDataTest::TestTempDir + QDir::separator() + SIMPL::CellData::ConfidenceIndexNoSpace + ".txt";
      f = fopen(exportArrayFile.toLatin1().data(), "r"); // open file created from array
      DREAM3D_REQUIRE_VALID_POINTER(f)

      for(int i = 0; i < 20; i++) // compare file to what was written in array
      {
        fscanf(f, "%d,", &num);
        if(i + 20 != num)
        {
          DREAM3D_REQUIRE_EQUAL(0, -3)
        }
      }
      fclose(f);
    }
    {
      int num = 0;
      FILE* f;
      QString exportArrayFile = UnitTest::ExportDataTest::TestTempDir + QDir::separator() + SIMPL::GeneralData::ThresholdArray + ".txt";
      f = fopen(exportArrayFile.toLatin1().data(), "r"); // open file created from array
      DREAM3D_REQUIRE_VALID_POINTER(f)
      for(int i = 0; i < 20; i++) // compare file to what was written in bool array
      {
        fscanf(f, "%d,", &num);
        if((i % 2 == 0 && num == 1) || (i % 2 != 0 && num == 0))
        {
          DREAM3D_REQUIRE_EQUAL(0, -3)
        }
      }
      fclose(f);
    }
    return EXIT_SUCCESS;
  }

  /**
   * @brief
   */
  void operator()()
  {
    int err = EXIT_SUCCESS;
    std::cout << "<===== Start " << getNameOfClass().toStdString() << std::endl;
    DREAM3D_REGISTER_TEST(TestFilterAvailability());

    DREAM3D_REGISTER_TEST(TestExportDataWriter())
    DREAM3D_REGISTER_TEST(TestExportDataReader())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

private:
  ExportDataTest(const ExportDataTest&); // Copy Constructor Not Implemented
  void operator=(const ExportDataTest&); // Move assignment Not Implemented
};
