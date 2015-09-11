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



#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/TSL/AngFields.h"
#include "EbsdLib/HKL/CtfFields.h"


#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/TestFilters/GenericExample.h"
#include "SIMPLib/TestFilters/MakeVolumeDataContainer.h"

#include "SIMPLib/IOFilters/DataContainerWriter.h"
#include "SIMPLib/FilterParameters/QFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/H5FilterParametersWriter.h"
#include "SIMPLib/FilterParameters/QFilterParametersReader.h"
#include "SIMPLib/FilterParameters/H5FilterParametersReader.h"


#include "UnitTestSupport.hpp"
#include "TestFileLocations.h"

namespace GenericFilterTest
{
  QString TestDir()
  {
    return UnitTest::TestTempDir + QString::fromLatin1("/GenericFilterTest");
  }

  QString TestTSLPipelineFile()
  {
    return TestDir() + QString::fromLatin1("/ReadH5Ebsd_AngFileExample.ini");
  }

  QString TestHKLPipelineFile()
  {
    return TestDir() + QString::fromLatin1("/ReadH5Ebsd_HklFileExample.ini");
  }

  QString DREAM3DFile()
  {
    return TestDir() + QString::fromLatin1("/SmallIN100.dream3d");
  }

  //// ****************** This file is a reference file from the DREAM3D Data Directory
  /// ******************** DO NOT FREAKING DELETE IT!!!!!!!!!!
  QString SmallIN100File()
  {
    return UnitTest::DataDir + QString::fromLatin1("/SmallIN100/SmallIN100.h5ebsd");
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  QFile::remove(GenericFilterTest::TestTSLPipelineFile());
  QFile::remove(GenericFilterTest::TestHKLPipelineFile());
  QFile::remove(GenericFilterTest::DREAM3DFile());
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestGenericFilter()
{

  Observer obs; // Npte that this variale will go out of scope at the end of this method. This is NOT the
  // way to add Observers into the FilterPipeline Object normally. We are careful here about the scope of
  // the variable.

  FilterPipeline::Pointer pipeline = FilterPipeline::New();
  pipeline->addMessageReceiver(&obs);

  // Insert a filter that simply creates an empty volume data container
  MakeVolumeDataContainer::Pointer f = MakeVolumeDataContainer::New();
  pipeline->pushBack(f);

  GenericExample::Pointer filter = GenericExample::New();
  pipeline->pushBack(filter);

  pipeline->preflightPipeline();
  int err = pipeline->getErrorCondition();

  DREAM3D_REQUIRED(err, >=, 0)

  pipeline->execute();
  err = pipeline->getErrorCondition();

  DREAM3D_REQUIRE(err >= 0)

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestTslReadH5Ebsd()
{

  Observer obs;
  // Send progress messages from PipelineBuilder to this object for display
  qRegisterMetaType<PipelineMessage>();

  // Create our Pipeline object
  FilterPipeline::Pointer pipeline = FilterPipeline::New();
  pipeline->addMessageReceiver(&obs);

  ReadH5Ebsd::Pointer reader = ReadH5Ebsd::New();
  AngFields angFields;
  QVector<QString> vArrayNames = angFields.getFieldNames();

  QSet<QString> arrays;
  for(int i = 0; i < vArrayNames.size(); i++)
  {
    arrays.insert(vArrayNames[i]);
  }

  reader->setInputFile(GenericFilterTest::SmallIN100File());
  reader->setSelectedArrayNames(arrays);
  reader->setRefFrameZDir(Ebsd::RefFrameZDir::HightoLow);
  reader->setZStartIndex(1);
  reader->setZEndIndex(117);
  reader->setUseTransformations(true);

  pipeline->pushBack(reader); // Push the H5EbsdReader into the Pipeline


  DataContainerWriter::Pointer dcWriter = DataContainerWriter::New();
  dcWriter->setOutputFile(GenericFilterTest::DREAM3DFile());
  pipeline->pushBack(dcWriter); // Push the DREAM3D writer into the Pipeline

// Now create a QSettings based writer to write the parameters to a .ini file
  QFilterParametersWriter::Pointer qWriter = QFilterParametersWriter::New();
  QString iniFile(GenericFilterTest::TestTSLPipelineFile());
  QFileInfo fi(iniFile);
  if (fi.exists() == true)
  {
    QFile(iniFile).remove();
  }
  qWriter->openFile(GenericFilterTest::TestTSLPipelineFile(), QSettings::IniFormat);
  // Write the Filter Parameters to the file
  int idx = reader->writeFilterParameters(qWriter.get(), 0);
  dcWriter->writeFilterParameters(qWriter.get(), idx);
  qWriter->closeFile();


// Now preflight and run the Pipeline
  int err = pipeline->preflightPipeline();
  if(err < 0)
  {
    std::cout << "Failed Preflight" << std::endl;
  }
  pipeline->run();
  err = pipeline->getErrorCondition();
  DREAM3D_REQUIRE(err >= 0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestHklReadH5Ebsd()
{

  Observer obs;
  // Send progress messages from PipelineBuilder to this object for display
  qRegisterMetaType<PipelineMessage>();

  // Create our Pipeline object
  FilterPipeline::Pointer pipeline = FilterPipeline::New();
  pipeline->addMessageReceiver(&obs);

  ReadH5Ebsd::Pointer reader = ReadH5Ebsd::New();
  CtfFields ctfFields;
  QVector<QString> vArrayNames = ctfFields.getFieldNames();

  QSet<QString> arrays;
  for(int i = 0; i < vArrayNames.size(); i++)
  {
    arrays.insert(vArrayNames[i]);
  }

  reader->setInputFile(GenericFilterTest::SmallIN100File());
  reader->setSelectedArrayNames(arrays);
  reader->setRefFrameZDir(Ebsd::RefFrameZDir::HightoLow);
  reader->setZStartIndex(1);
  reader->setZEndIndex(117);
  reader->setUseTransformations(true);

  pipeline->pushBack(reader); // Push the H5EbsdReader into the Pipeline


  DataContainerWriter::Pointer dcWriter = DataContainerWriter::New();
  dcWriter->setOutputFile(GenericFilterTest::DREAM3DFile());
  pipeline->pushBack(dcWriter); // Push the DREAM3D writer into the Pipeline

// Now create a QSettings based writer to write the parameters to a .ini file
  QFilterParametersWriter::Pointer qWriter = QFilterParametersWriter::New();
  QString iniFile(GenericFilterTest::TestHKLPipelineFile());
  QFileInfo fi(iniFile);
  if (fi.exists() == true)
  {
    QFile(iniFile).remove();
  }
  qWriter->openFile(GenericFilterTest::TestHKLPipelineFile(), QSettings::IniFormat);
  // Write the Filter Parameters to the file
  int idx = reader->writeFilterParameters(qWriter.get(), 0);
  dcWriter->writeFilterParameters(qWriter.get(), idx);
  qWriter->closeFile();


// Now preflight and run the Pipeline
//  int err = pipeline->preflightPipeline();
//  if(err < 0)
//  {
//    std::cout << "Failed Preflight" << std::endl;
//  }
//  pipeline->run();
//  err = pipeline->getErrorCondition();
//  DREAM3D_REQUIRE(err >= 0);
}

// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  int err = EXIT_SUCCESS;

  QDir dir(GenericFilterTest::TestDir());
  dir.mkpath(".");

#if REMOVE_TEST_FILES
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
#endif

  DREAM3D_REGISTER_TEST( TestTslReadH5Ebsd() )
  DREAM3D_REGISTER_TEST( TestHklReadH5Ebsd() )
  DREAM3D_REGISTER_TEST( TestGenericFilter() )

#if REMOVE_TEST_FILES
  //   DREAM3D_REGISTER_TEST( RemoveTestFiles() )
#endif

  PRINT_TEST_SUMMARY();
  return err;
}


