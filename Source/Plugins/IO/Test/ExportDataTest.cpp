/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
 * All rights reserved.
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/Common/FilterPipeline.h"
#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/FilterFactory.hpp"
#include "DREAM3DLib/Plugin/IDREAM3DPlugin.h"
#include "DREAM3DLib/Plugin/DREAM3DPluginLoader.h"
#include "DREAM3DLib/Utilities/UnitTestSupport.hpp"
#include "DREAM3DLib/Utilities/QMetaObjectUtilities.h"
#include "DREAM3DLib/DataContainers/DataContainer.h"
#include "DREAM3DLib/CoreFilters/DataContainerReader.h"
#include "DREAM3DLib/CoreFilters/DataContainerWriter.h"

#include "TestFileLocations.h"
#include "GenerateFeatureIds.h"

#include <stdlib.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
	QFile::remove(UnitTest::ExportDataTest::TestFile);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestFilterAvailability()
{
  // Now instantiate the ExportData Filter from the FilterManager
  QString filtName = "ExportData";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
  if (NULL == filterFactory.get() )
  {
    std::stringstream ss;
    ss << "The ExportDataTest Requires the use of the " << filtName.toStdString() << " filter which is found in the IO Plugin";
    DREAM3D_TEST_THROW_EXCEPTION(ss.str())
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestExportDataWriter()
{

DataContainerArray::Pointer dca = DataContainerArray::New();

// A DataContainer that mimics some real data
DataContainer::Pointer m = DataContainer::New(DREAM3D::Defaults::DataContainerName);
dca->addDataContainer(m);

AttributeMatrix::Pointer attrMatrix = AttributeMatrix::New(QVector<size_t>(1, 20), DREAM3D::Defaults::AttributeMatrixName, DREAM3D::AttributeMatrixType::Generic);
m->addAttributeMatrix(DREAM3D::Defaults::AttributeMatrixName, attrMatrix);
int size = 20;
Int32ArrayType::Pointer intArray = Int32ArrayType::CreateArray(size, DREAM3D::CellData::CellPhases);
for (int i = 0; i < size; ++i)
{
	intArray->setValue(i, i + 20);
}
attrMatrix->addAttributeArray(DREAM3D::CellData::CellPhases, intArray);


Observer obs;
// Send progress messages from PipelineBuilder to this object for display

DataContainerWriter::Pointer writer = DataContainerWriter::New();
writer->setDataContainerArray(dca);
QString exportArrayFile = UnitTest::ExportDataTest::TestFile + QDir::separator() + "ExportTest.txt" ;
writer->setOutputFile(exportArrayFile);

// Since we are NOT using the Pipeline Object to execute the filter but instead we are directly executing the filter
// and we want to know about any error/warning/progress messages we need to connect the filter to our Observer object
// manually. Normally the Pipeline Object would do this for us. We are NOT using a Pipeline Object because using the
// Pipeline Object would over write the DataContainer Array that we have created with a blank one thus defeating the
// entire purpose of the test.
QObject::connect(writer.get(), SIGNAL(filterGeneratedMessage(const PipelineMessage&)),
	&obs, SLOT(processPipelineMessage(const PipelineMessage&)));

writer->execute();
int err = writer->getErrorCondition();

DREAM3D_REQUIRE_EQUAL(err, 0);


  // Now instantiate the EnsembleInfoReader Filter from the FilterManager
  QString filtName = "ExportData";
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
//		dca = DataContainerArray::New();
//		m = DataContainer::New(DREAM3D::Defaults::DataContainerName);
//		dca->addDataContainer(m);
		filter->setDataContainerArray(dca);


    var.setValue(2);
    propWasSet = filter->setProperty("Delimeter", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

		//var.setValue(".tst");
		//propWasSet = filter->setProperty("FileExtension", var);
		//DREAM3D_REQUIRE_EQUAL(propWasSet, true)

		var.setValue(10);
		propWasSet = filter->setProperty("MaxValPerLine", var);
		DREAM3D_REQUIRE_EQUAL(propWasSet, true)

		var.setValue(UnitTest::ExportDataTest::TestFile);
		propWasSet = filter->setProperty("OutputPath", var);
		DREAM3D_REQUIRE_EQUAL(propWasSet, true)

		DataArrayPath path = DataArrayPath(DREAM3D::Defaults::DataContainerName,
		DREAM3D::Defaults::AttributeMatrixName,
		DREAM3D::CellData::CellPhases);

		var.setValue(path);
		propWasSet = filter->setProperty("SelectedArrayPath", var);
		DREAM3D_REQUIRE_EQUAL(propWasSet, true)

		filter->execute();
		err = filter->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, 0);

    //DREAM3D_REQUIRED(filter->getErrorCondition(), >= , 0);


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
int TestExportDataReader()
{


	
//	DataContainerArray::Pointer dca = DataContainerArray::New();
//  DataContainer::Pointer vdc = DataContainer::New(DREAM3D::Defaults::DataContainerName);

  //// Now instantiate the EnsembleInfoReader Filter from the FilterManager
  //QString filtName = "EnsembleInfoReader";
  //FilterManager* fm = FilterManager::Instance();
  //IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
  //if (NULL != filterFactory.get())
  //{
  //  // If we get this far, the Factory is good so creating the filter should not fail unless something has
  //  // horribly gone wrong in which case the system is going to come down quickly after this.
  //  AbstractFilter::Pointer filter = filterFactory->create();

  //  QVariant var;
  //  int err;
  //  bool propWasSet;

  //  dca->addDataContainer(vdc);
  //  filter->setDataContainerArray(dca);
  //  var.setValue(UnitTest::EnsembleInfoReaderTest::TestFileIni); // should pass
  //  propWasSet = filter->setProperty("InputFile", var);
  //  DREAM3D_REQUIRE_EQUAL(propWasSet, true)
  //  filter->execute();
  //  DREAM3D_REQUIRED(filter->getErrorCondition(), >= , 0);


  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void loadFilterPlugins()
{
  // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();
  DREAM3DPluginLoader::LoadPluginFilters(fm);

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
  QCoreApplication::setApplicationName("ExportDataTest");

  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( loadFilterPlugins() );
  DREAM3D_REGISTER_TEST( TestFilterAvailability() );

  DREAM3D_REGISTER_TEST(TestExportDataWriter())
  DREAM3D_REGISTER_TEST(TestExportDataReader())

//  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
PRINT_TEST_SUMMARY();

  return err;
}

