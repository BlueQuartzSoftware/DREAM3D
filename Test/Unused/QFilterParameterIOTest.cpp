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



#include <QtCore/QtDebug>
#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtCore/QSet>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/IOFilters/ReadH5Ebsd.h"
#include "SIMPLib/FilterParameters/QFilterParametersReader.h"
#include "SIMPLib/FilterParameters/QFilterParametersWriter.h"
#include "SIMPLib/IOFilters/DataContainerReader.h"

#include "SIMPLib/IOFilters/EbsdToH5Ebsd.h"

#include "UnitTestSupport.hpp"
#include "TestFileLocations.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  QFile::remove(UnitTest::QFilterParameterIOTest::TestFile);
#endif
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void writePipeline(FilterPipeline::Pointer pipeline)
{

  QFilterParametersWriter::Pointer writer = QFilterParametersWriter::New();
  writer->openFile(UnitTest::QFilterParameterIOTest::TestFile, QSettings::IniFormat);
  writer->setNumberOfFilters(pipeline->getFilterContainer().size());
  QVariant filterCount = pipeline->getFilterContainer().size();
  int count = pipeline->getFilterContainer().size();
  QSettings* prefs = writer->getPrefs();
  prefs->beginGroup(DREAM3D::Settings::PipelineBuilderGroup);
  prefs->setValue(DREAM3D::Settings::NumFilters, filterCount);
  prefs->setValue(DREAM3D::Settings::Version, SIMPLib::Version::Complete());
  prefs->endGroup();

  for (int i = 0; i < count; ++i)
  {
    AbstractFilter::Pointer filter = pipeline->getFilterContainer().at(i);
    filter->writeFilterParameters(writer.get(), i);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestWriteQSettingsBasedFile()
{
  DataContainerReader::Pointer dcr = DataContainerReader::New();
  dcr->setInputFile("foobar.dream3d");
  dcr->setReadVertexData(false);
  dcr->setReadEdgeData(false);
  dcr->setReadSurfaceData(false);
  dcr->setReadAllVertexArrays(false);
  dcr->setReadAllFaceArrays(false);
  dcr->setReadVolumeData(true);
  dcr->setReadAllCellArrays(true);
  dcr->setReadAllCellFeatureArrays(true);
  dcr->setReadAllCellEnsembleArrays(true);

  QSet<QString> cellArrays;
  cellArrays << DREAM3D::CellData::FeatureIds;
  QSet<QString> featureArrays;
  featureArrays << DREAM3D::FeatureData::Centroids;
  QSet<QString> ensembleArrays;
  ensembleArrays << DREAM3D::EnsembleData::CrystalStructures;
  QSet<QString> blank;
  dcr->setVolumeSelectedArrayNames(blank, blank, blank, cellArrays, featureArrays, ensembleArrays);

  FilterPipeline::Pointer pipeline = FilterPipeline::New();
  pipeline->pushBack(dcr);
  writePipeline(pipeline);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void readPipeline(QFilterParametersReader::Pointer paramsReader, FilterPipeline::Pointer pipeline)
{
  FilterManager::Pointer filtManager = FilterManager::Instance();
  QSettings* prefs = paramsReader->getPrefs();
  prefs->beginGroup(DREAM3D::Settings::PipelineBuilderGroup);
  bool ok = false;
  int filterCount = prefs->value(DREAM3D::Settings::NumFilters).toInt(&ok);
  prefs->endGroup();
  if (false == ok) {filterCount = 0;}

  for (int i = 0; i < filterCount; ++i)
  {
    QString gName = QString::number(i);

    // Open the group to get the name of the filter then close again.
    prefs->beginGroup(gName);
    QString filterName = prefs->value(DREAM3D::Settings::FilterName, "").toString();
    prefs->endGroup();
    //  qDebug() << filterName;

    IFilterFactory::Pointer factory = filtManager->getFactoryForFilter(filterName);
    DREAM3D_REQUIRE(NULL != factory) // We should know about all the filters
    AbstractFilter::Pointer filter = factory->create();

    if(NULL != filter.get())
    {
      filter->readFilterParameters(paramsReader.get(), i);
      pipeline->pushBack(filter);
    }
  }
  writePipeline(pipeline);
}


QString m_FilePrefix("Small_IN100_");
QString m_FileSuffix("");
QString m_FileExt("ang");


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QString> generateFileList(int start, int end, bool& hasMissingFiles, bool stackLowToHigh)
{
  int index = 0;
  QVector<QString> fileList;

  for (int i = 0; i < (end - start) + 1; ++i)
  {
    if (stackLowToHigh)
    {
      index = start + i;
    }
    else
    {
      index = end - i;
    }
    QString filename = QString("%1%2%3.%4").arg("Small_IN100_").arg(i).arg("").arg("ang");
    QString filePath = UnitTest::QFilterParameterIOTest::SmallIN100Folder + QDir::separator() + filename;
    filePath = QDir::toNativeSeparators(filePath);
    fileList.push_back(filePath);
  }
  return fileList;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void importSmallIN100()
{
  Observer obs;


  EbsdToH5Ebsd::Pointer filter = EbsdToH5Ebsd::New();
  filter->setOutputFile(UnitTest::QFilterParameterIOTest::SmallIn100_H5EBSD_FILE);
  filter->setZStartIndex(1);
  filter->setZEndIndex(118);
  filter->setZResolution(0.25);
  filter->setSampleTransformationAngle(180.0);
  filter->addObserver(&obs);
  QVector<float> transAxis(3, 0.0f);
  transAxis[0] = 0.0f;
  transAxis[1] = 1.0f;
  transAxis[2] = 0.0f;
  filter->setSampleTransformationAxis(transAxis);

  filter->setEulerTransformationAngle(90.0);
  transAxis[0] = 0.0f;
  transAxis[1] = 0.0f;
  transAxis[2] = 1.0f;
  filter->setEulerTransformationAxis(transAxis);

  filter->setRefFrameZDir( Ebsd::HightoLow );

  int start = filter->getZStartIndex();
  int end = filter->getZEndIndex();
  bool hasMissingFiles = false;

  // Now generate all the file names in the "Low to High" order because that is what the importer is expecting
  QVector<QString> fileList = generateFileList(start, end, hasMissingFiles, true);
  QVector<QString> realFileList;
  for(QVector<QString>::size_type i = 0; i < fileList.size(); ++i)
  {
    QString filePath = (fileList[i]);
    QFileInfo fi(filePath);
    if (fi.exists())
    {
      realFileList.push_back(fileList[i]);
    }
  }

  filter->setEbsdFileList(realFileList);


  // Run the preflight
  filter->preflight();
  if(filter->getErrorCondition() < 0)
  {
    QVector<PipelineMessage> msgs = filter->getPipelineMessages();
    for(qint32 i = 0; i < msgs.size(); ++i)
    {
      qDebug() << msgs[i].generateErrorString();
    }
    return;
  }

  // Run the Filter to import the data
  filter->execute();

  if(filter->getErrorCondition() < 0)
  {
    QVector<PipelineMessage> msgs = filter->getPipelineMessages();
    for(qint32 i = 0; i < msgs.size(); ++i)
    {
      qDebug() << msgs[i].generateErrorString();
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestReadQSettingsBasedFile()
{
  int err = 0;

  FilterManager::Pointer filtManager = FilterManager::Instance();
  // THIS IS A VERY IMPORTANT LINE: It will register all the known filters in the dream3d library. This
  // will NOT however get filters from plugins. We are going to have to figure out how to compile filters
  // into their own plugin and load the plugins from a command line.
  filtManager->RegisterKnownFilters(filtManager.get());
#if 0
  qDebug() << "Current Path: " << QDir::currentPath();
  // Read in the first Pipeline that converts the Small IN100 files to an .h5ebsd file
  // importSmallIN100();
  QDir cwd = QDir::current();
  cwd.cdUp();
  cwd.cd("Bin");
  qDebug() << "Changing working directory to " << cwd.absolutePath();
  QDir::setCurrent(cwd.absolutePath());
#endif

  FilterPipeline::Pointer pipeline = FilterPipeline::New();
  QFilterParametersReader::Pointer paramsReader = QFilterParametersReader::New();
  paramsReader->openFile(UnitTest::QFilterParameterIOTest::Prebuilt17, QSettings::IniFormat);
  readPipeline(paramsReader, pipeline);

  AbstractFilter::Pointer filter = pipeline->getFilterContainer().at(0);
  ReadH5Ebsd* readH5Ebsd = ReadH5Ebsd::SafePointerDownCast(filter.get());
  DREAM3D_REQUIRE(readH5Ebsd != NULL);
  readH5Ebsd->setInputFile(UnitTest::QFilterParameterIOTest::SmallIn100_H5EBSD_FILE);

  err = pipeline->preflightPipeline();
  DREAM3D_REQUIRE(err >= 0)

  pipeline->execute();
  err = pipeline->getErrorCondition();
  DREAM3D_REQUIRE(err >= 0)


}

// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  int err = EXIT_SUCCESS;

  DREAM3D_REGISTER_TEST( RemoveTestFiles() )

  DREAM3D_REGISTER_TEST( TestWriteQSettingsBasedFile() )

//  DREAM3D_REGISTER_TEST( TestReadQSettingsBasedFile() )

#if REMOVE_TEST_FILES
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
#endif

  PRINT_TEST_SUMMARY();

  return err;
}
