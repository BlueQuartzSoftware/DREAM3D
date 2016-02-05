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

#include <stdlib.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QString>

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

#include "IOTestFileLocations.h"
#include "GenerateFeatureIds.h"

class EnsembleInfoReaderTest
{
  public:
    EnsembleInfoReaderTest(){}
    virtual ~EnsembleInfoReaderTest(){}
    SIMPL_TYPE_MACRO(EnsembleInfoReaderTest)

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void RemoveTestFiles()
    {
#if REMOVE_TEST_FILES
      QFile::remove(UnitTest::EnsembleInfoReaderTest::TestFileIni);
      QFile::remove(UnitTest::EnsembleInfoReaderTest::TestFileTxt);
      QFile::remove(UnitTest::EnsembleInfoReaderTest::TestFileDoc);
#endif
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    int TestFilterAvailability()
    {
      // Now instantiate the EnsembleInfoReader Filter from the FilterManager
      QString filtName = "EnsembleInfoReader";
      FilterManager* fm = FilterManager::Instance();
      IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
      if (NULL == filterFactory.get() )
      {
        std::stringstream ss;
        ss << "The EnsembleInfoReaderTest Requires the use of the " << filtName.toStdString() << " filter which is found in the IO Plugin";
        DREAM3D_TEST_THROW_EXCEPTION(ss.str())
      }
      return 0;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void WriteTestFile(const QString& filePath, const QString& groups)
    {
      FILE* f = NULL;
      f = fopen(filePath.toLatin1().data(), "wb");
      if (NULL == f)
      {
        QString ss = QObject::tr("Error writing output file '%1'\n ").arg(filePath);
        DREAM3D_REQUIRE_EQUAL(0, 1)
      }
      fprintf(f, "%s", groups.toLatin1().data());
      fclose(f);
    }


    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    int TestEnsembleInfoWriter()
    {
      QString groups = "[EnsembleInfo]\nNumber_Phases = 2\n\n\
          [1]\nCrystalStructure = Cubic_High\nPhaseType = PrimaryPhase\n\n\
          [2]\nCrystalStructure = Hexagonal_High\nPhaseType = MatrixPhase\n"; // the correct groups and keys for writing the .ini file

          // make .doc, .txt, .ini files
          WriteTestFile(UnitTest::EnsembleInfoReaderTest::TestFileDoc, groups);
      WriteTestFile(UnitTest::EnsembleInfoReaderTest::TestFileTxt, groups);
      WriteTestFile(UnitTest::EnsembleInfoReaderTest::TestFileIni, groups);

      DataContainerArray::Pointer dca = DataContainerArray::New();
      DataContainer::Pointer vdc = DataContainer::New(SIMPL::Defaults::DataContainerName);

      // Now instantiate the EnsembleInfoReader Filter from the FilterManager
      QString filtName = "EnsembleInfoReader";
      FilterManager* fm = FilterManager::Instance();
      IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
      if (NULL != filterFactory.get())
      {
        // If we get this far, the Factory is good so creating the filter should not fail unless something has
        // horribly gone wrong in which case the system is going to come down quickly after this.
        AbstractFilter::Pointer filter = filterFactory->create();

        QVariant var;
        int err = 0;
        bool propWasSet;

        dca->addDataContainer(vdc);
        filter->setDataContainerArray(dca);
        var.setValue(UnitTest::EnsembleInfoReaderTest::TestFileDoc); // should return -10018, .doc extension
        propWasSet = filter->setProperty("InputFile", var);
        var.setValue(SIMPL::Defaults::DataContainerName);
        filter->setProperty("DataContainerName", var);
        DREAM3D_REQUIRE_EQUAL(propWasSet, true)
            filter->preflight();
        err = filter->getErrorCondition();
        DREAM3D_REQUIRE_EQUAL(err, -10018);


        // Reset The data container array with new instances for this sub test
        dca = DataContainerArray::New();
        vdc = DataContainer::New(SIMPL::Defaults::DataContainerName);
        dca->addDataContainer(vdc);
        filter->setDataContainerArray(dca);

        var.setValue(UnitTest::EnsembleInfoReaderTest::TestFileTxt); // should pass, .txt extension
        propWasSet = filter->setProperty("InputFile", var);
        var.setValue(SIMPL::Defaults::DataContainerName);
        filter->setProperty("DataContainerName", var);
        DREAM3D_REQUIRE_EQUAL(propWasSet, true)
            filter->preflight();
        DREAM3D_REQUIRED(filter->getErrorCondition(), >= , 0);

        // Reset The data container array with new instances for this sub test
        dca = DataContainerArray::New();
        vdc = DataContainer::New(SIMPL::Defaults::DataContainerName);
        dca->addDataContainer(vdc);
        filter->setDataContainerArray(dca);

        var.setValue(UnitTest::EnsembleInfoReaderTest::TestFileIni); // should pass, .ini extension
        propWasSet = filter->setProperty("InputFile", var);
        DREAM3D_REQUIRE_EQUAL(propWasSet, true)
            filter->preflight();
        err = filter->getErrorCondition();
        DREAM3D_REQUIRE_EQUAL(err, 0);
      }
      else
      {
        QString ss = QObject::tr("EnsembleInfoReaderTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
        DREAM3D_REQUIRE_EQUAL(0, 1)
      }
      return 1;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    int TestEnsembleInfoReader()
    {
      DataContainerArray::Pointer dca = DataContainerArray::New();
      DataContainer::Pointer vdc = DataContainer::New(SIMPL::Defaults::DataContainerName);

      // Now instantiate the EnsembleInfoReader Filter from the FilterManager
      QString filtName = "EnsembleInfoReader";
      FilterManager* fm = FilterManager::Instance();
      IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
      if (NULL != filterFactory.get())
      {
        // If we get this far, the Factory is good so creating the filter should not fail unless something has
        // horribly gone wrong in which case the system is going to come down quickly after this.
        AbstractFilter::Pointer filter = filterFactory->create();

        QVariant var;
        int err;
        bool propWasSet;

        dca->addDataContainer(vdc);
        filter->setDataContainerArray(dca);
        var.setValue(UnitTest::EnsembleInfoReaderTest::TestFileIni); // should pass
        propWasSet = filter->setProperty("InputFile", var);
        var.setValue(SIMPL::Defaults::DataContainerName);
        filter->setProperty("DataContainerName", var);
        DREAM3D_REQUIRE_EQUAL(propWasSet, true)
            filter->execute();
        DREAM3D_REQUIRED(filter->getErrorCondition(), >= , 0);

        // Reset The data container array with new instances for this sub test
        dca = DataContainerArray::New();
        vdc = DataContainer::New(SIMPL::Defaults::DataContainerName);
        dca->addDataContainer(vdc);
        filter->setDataContainerArray(dca);

        QString groups;
        groups = "[EnsembleInf]\nNumber_Phases = 2\n\n[1]\nCrystalStructure = Cubic_High\nPhaseType = PrimaryPhase\n\n[2]\nCrystalStructure = Hexagonal_High\nPhaseType = MatrixPhase\n";
        WriteTestFile(UnitTest::EnsembleInfoReaderTest::TestFileIni, groups);  //EnsembleInfo misspelled
        var.setValue(UnitTest::EnsembleInfoReaderTest::TestFileIni);
        propWasSet = filter->setProperty("InputFile", var);
        DREAM3D_REQUIRE_EQUAL(propWasSet, true)
            filter->execute();
        err = filter->getErrorCondition();
        DREAM3D_REQUIRE_EQUAL(err, -10003);

        // Reset The data container array with new instances for this sub test
        dca = DataContainerArray::New();
        vdc = DataContainer::New(SIMPL::Defaults::DataContainerName);
        dca->addDataContainer(vdc);
        filter->setDataContainerArray(dca);
        groups = "[EnsembleInfo]\nNumber_Phases = 2\n\n[1]\nCrystalStructure = Cubic_High\n\n\n[2]\nCrystalStructure = Hexagonal_High\nPhaseType = MatrixPhase\n";
        WriteTestFile(UnitTest::EnsembleInfoReaderTest::TestFileIni, groups); //PhaseType missing
        var.setValue(UnitTest::EnsembleInfoReaderTest::TestFileIni);
        propWasSet = filter->setProperty("InputFile", var);
        DREAM3D_REQUIRE_EQUAL(propWasSet, true)
            filter->execute();
        err = filter->getErrorCondition();
        DREAM3D_REQUIRE_EQUAL(err, -10009);

        // Reset The data container array with new instances for this sub test
        dca = DataContainerArray::New();
        vdc = DataContainer::New(SIMPL::Defaults::DataContainerName);
        dca->addDataContainer(vdc);
        filter->setDataContainerArray(dca);
        groups = "[EnsembleInfo]\nNumber_Phases = 2\n\n[1]\nCrystalStructure = Cubic_High\nPhaseType = PrimaryPhase\n\n[2]\n\nPhaseType = MatrixPhase\n";
        WriteTestFile(UnitTest::EnsembleInfoReaderTest::TestFileIni, groups); //CrystalStructure missing
        var.setValue(UnitTest::EnsembleInfoReaderTest::TestFileIni);
        propWasSet = filter->setProperty("InputFile", var);
        DREAM3D_REQUIRE_EQUAL(propWasSet, true)
            filter->execute();
        err = filter->getErrorCondition();
        DREAM3D_REQUIRE_EQUAL(err, -10008);

        // Reset The data container array with new instances for this sub test
        dca = DataContainerArray::New();
        vdc = DataContainer::New(SIMPL::Defaults::DataContainerName);
        dca->addDataContainer(vdc);
        filter->setDataContainerArray(dca);
        groups = "[EnsembleInfo]\nNumber_Phases = 2\n\n[1]\nCrystalStructure = Cubic_High\nPhaseType = PrimaryPase\n\n[2]\nCrystalStructure = Hexagonal_High\nPhaseType = MatrixPhase\n";
        WriteTestFile(UnitTest::EnsembleInfoReaderTest::TestFileIni, groups);  //PrimaryPhase is misspelled
        var.setValue(UnitTest::EnsembleInfoReaderTest::TestFileIni);
        propWasSet = filter->setProperty("InputFile", var);
        DREAM3D_REQUIRE_EQUAL(propWasSet, true)
            filter->execute();
        err = filter->getErrorCondition();
        DREAM3D_REQUIRE_EQUAL(err, -10007);

#if 0
        // Reset The data container array with new instances for this sub test
        dca = DataContainerArray::New();
        vdc = DataContainer::New(SIMPL::Defaults::DataContainerName);
        dca->addDataContainer(vdc);
        filter->setDataContainerArray(dca);
        groups = "[EnsembleInfo]\nNumber_Phases = 2\n\n[1]\nCrystalStructure = Cubic_High\nPhaseType = PrimaryPhase\n\n[3]\nCrystalStructure = Hexagonal_High\nPhaseType = MatrixPhase\n";
        WriteTestFile(UnitTest::EnsembleInfoReaderTest::TestFileIni, groups); //Group number is incorrect
        var.setValue(UnitTest::EnsembleInfoReaderTest::TestFileIni);
        propWasSet = filter->setProperty("InputFile", var);
        DREAM3D_REQUIRE_EQUAL(propWasSet, true)
            filter->execute();
        err = filter->getErrorCondition();
        DREAM3D_REQUIRE_EQUAL(err, -10005);
#endif

        // Reset The data container array with new instances for this sub test
        dca = DataContainerArray::New();
        vdc = DataContainer::New(SIMPL::Defaults::DataContainerName);
        dca->addDataContainer(vdc);
        filter->setDataContainerArray(dca);
        groups = "[EnsembleInfo]\nNumber_Phases = 2\n\n[1]\nCrystalStructure = Cubc\nPhaseType = PrimaryPhase\n\n[2]\nCrystalStructure = Hexagonal_High\nPhaseType = MatrixPhase\n";
        WriteTestFile(UnitTest::EnsembleInfoReaderTest::TestFileIni, groups); //Cubic is misspelled
        var.setValue(UnitTest::EnsembleInfoReaderTest::TestFileIni);
        propWasSet = filter->setProperty("InputFile", var);
        DREAM3D_REQUIRE_EQUAL(propWasSet, true)
            filter->execute();
        err = filter->getErrorCondition();
        DREAM3D_REQUIRE_EQUAL(err, -10006);

      }
      else
      {
        QString ss = QObject::tr("EnsembleInfoReaderTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
        DREAM3D_REQUIRE_EQUAL(0, 1)
      }
      return 1;
    }

    /**
  * @brief
*/
    void operator()()
    {
      int err = EXIT_SUCCESS;
      DREAM3D_REGISTER_TEST( TestFilterAvailability() );

      DREAM3D_REGISTER_TEST(TestEnsembleInfoWriter())
          DREAM3D_REGISTER_TEST(TestEnsembleInfoReader())

          DREAM3D_REGISTER_TEST( RemoveTestFiles() )
    }
  private:
    EnsembleInfoReaderTest(const EnsembleInfoReaderTest&); // Copy Constructor Not Implemented
    void operator=(const EnsembleInfoReaderTest&); // Operator '=' Not Implemented
};

