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


// C Includes
#include <stdlib.h>
#include <assert.h>

#ifdef _MSC_VER
#include <direct.h>
#else
#include <unistd.h>
#endif

// C++ Includes
#include <iostream>

// TCLAP Includes
#include <tclap/CmdLine.h>
#include <tclap/ValueArg.h>

// Boost includes
#include <boost/assert.hpp>

// Qt Includes
#include <QtCore/QtDebug>
#include <QtCore/QCoreApplication>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QStringListIterator>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QSettings>

// DREAM3DLib includes
#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/DREAM3DLibVersion.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/FilterFactory.hpp"
#include "DREAM3DLib/Common/FilterPipeline.h"
#include "DREAM3DLib/Plugin/IDREAM3DPlugin.h"
#include "DREAM3DLib/Plugin/DREAM3DPluginLoader.h"
#include "DREAM3DLib/FilterParameters/QFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/H5FilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/JsonFilterParametersReader.h"
#include "DREAM3DLib/Utilities/QMetaObjectUtilities.h"

#include "DREAM3DLib/Utilities/UnitTestSupport.hpp"

#include "PipelineRunnerTest.h"


class TestObserver : public QObject, public IObserver
{
    Q_OBJECT

  public:
    TestObserver(){}
    DREAM3D_TYPE_MACRO_SUPER(TestObserver, IObserver)

    virtual ~TestObserver(){}

  public slots:
    virtual void processPipelineMessage(const PipelineMessage& pm)
    {
      PipelineMessage msg = pm;
      QString filterHumanLabel = pm.getFilterHumanLabel();
      QString str;
      QTextStream ss(&str);
      if(msg.getType() == PipelineMessage::Error)
      {
        ss << msg.generateErrorString();
        std::cout << msg.getFilterHumanLabel().toStdString() << ": " << str.toStdString() << std::endl;
      }
      else if(msg.getType() == PipelineMessage::Warning)
      {
        ss << msg.generateWarningString();
        std::cout << msg.getFilterHumanLabel().toStdString() << ": " << str.toStdString() << std::endl;
      }
      else if(msg.getType() == PipelineMessage::StatusMessage)
      {
        ss << msg.generateStatusString();
        std::cout << msg.getFilterHumanLabel().toStdString() << ": " << str.toStdString() << std::endl;
      }
      else if(msg.getType() == PipelineMessage::ProgressValue)
      {
      //  ss << msg.getProgressValue() << "%";
      }
      else if(msg.getType() == PipelineMessage::StatusMessageAndProgressValue)
      {
      //  ss << msg.getProgressValue() << msg.generateStatusString();
      }
    }

  private:
    TestObserver(const TestObserver&); // Copy Constructor Not Implemented
    void operator=(const TestObserver&); // Operator '=' Not Implemented
};

#include "PipelineRunnerTest.moc"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void readPipeline(QFilterParametersReader::Pointer paramsReader, FilterPipeline::Pointer pipeline)
{
  FilterManager* filtManager = FilterManager::Instance();
  QSettings* prefs = paramsReader->getPrefs();
  prefs->beginGroup(DREAM3D::Settings::PipelineBuilderGroup);
  bool ok = false;
  int filterCount = prefs->value("Number_Filters").toInt(&ok);
  prefs->endGroup();
  if (false == ok) {filterCount = 0;}

  for (int i = 0; i < filterCount; ++i)
  {
    QString gName = QString::number(i);

    // Open the group to get the name of the filter then close again.
    prefs->beginGroup(gName);
    QString filterName = prefs->value("Filter_Name", "").toString();
    prefs->endGroup();
    //  qDebug() << filterName;

    IFilterFactory::Pointer factory = filtManager->getFactoryForFilter(filterName);
    AbstractFilter::Pointer filter = factory->create();

    if(NULL != filter.get())
    {
      filter->readFilterParameters(paramsReader.get(), i);
      pipeline->pushBack(filter);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExecutePipeline(const QString& pipelineFile)
{
  int err = EXIT_SUCCESS;

  // Sanity Check the filepath to make sure it exists, Report an error and bail if it does not
  QFileInfo fi(pipelineFile);
  std::cout << "#--------------Test Pipeline File: " << fi.absoluteFilePath().toStdString() << " ---------------------------#" << std::endl;
  if(fi.exists() == false)
  {
    std::cout << "The input file '" << pipelineFile.toStdString() << "' does not exist" << std::endl;
    err = EXIT_FAILURE;
  }
  DREAM3D_REQUIRE_EQUAL(err, EXIT_SUCCESS)

  QString ext = fi.completeSuffix();

  // Use the static method to read the Pipeline file and return a Filter Pipeline
  FilterPipeline::Pointer pipeline;
  if (ext == "ini" || ext == "txt")
  {
    pipeline = QFilterParametersReader::ReadPipelineFromFile(pipelineFile, QSettings::IniFormat);
  }
  else if (ext == "dream3d")
  {
    pipeline = H5FilterParametersReader::ReadPipelineFromFile(pipelineFile);
  }
  else if (ext == "json")
  {
    pipeline = JsonFilterParametersReader::ReadPipelineFromFile(pipelineFile);
  }

  if (NULL == pipeline.get())
  {
    std::cout << "An error occurred trying to read the pipeline file. Exiting now." << std::endl;
    err = EXIT_FAILURE;
  }
  DREAM3D_REQUIRE_EQUAL(err, EXIT_SUCCESS)


  TestObserver obs; // Create an Observer to report errors/progress from the executing pipeline
  pipeline->addMessageReceiver(&obs);
  // Preflight the pipeline
  err = pipeline->preflightPipeline();
  if (err < 0)
  {
    std::cout << "Errors preflighting the pipeline. Exiting Now." << std::endl;
  }
  DREAM3D_REQUIRE_EQUAL(err, EXIT_SUCCESS)

  // Now actually execute the pipeline
  pipeline->execute();
  err = pipeline->getErrorCondition();
  if (err < 0)
  {
    std::cout << "Error Condition of Pipeline: " << err << std::endl;
    err = EXIT_FAILURE;
  }
  DREAM3D_REQUIRE_EQUAL(err, EXIT_SUCCESS)

}

#define OVERWRITE_SOURCE_FILE 1
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void writeOutput(bool didReplace, QStringList& outLines, QString filename)
{
  if(didReplace == true)
  {
    QFileInfo fi2(filename);
#if OVERWRITE_SOURCE_FILE
    QFile hOut(filename);
#else
    QString tmpPath = "/tmp/" + fi2.fileName();
    QFile hOut(tmpPath);
#endif
    hOut.open(QFile::WriteOnly);
    QTextStream stream( &hOut );
    stream << outLines.join("\n");
    hOut.close();

    qDebug() << "Saved File " << fi2.absoluteFilePath();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AdjustOutputDirectory(const QString& pipelineFile)
{

  QString contents;

  // Read the Source File
  QFileInfo fi(pipelineFile);
  QFile source(pipelineFile);
  source.open(QFile::ReadOnly);
  contents = source.readAll();
  source.close();


  QString searchString = QString::fromLatin1("Data/Output/");
  QString replaceString = QString::fromLatin1("Data/zz_PipelineRunnerTest_Output/");
  QStringList outLines;
  QStringList list = contents.split(QRegExp("\\n"));
  QStringListIterator sourceLines(list);

  while (sourceLines.hasNext())
  {
    QString line = sourceLines.next();

    if( line.contains(QString("Data/")) == true && line.contains(searchString) == false )
    {
      line = line.replace(QString("Data/"), getDream3dDataDir() + "/Data/");
    }

    if(line.contains(searchString) )
    {
      line = line.replace(searchString, getTestTempDirectory());
    }

    outLines.push_back(line);

  }

  QString outFile = getTestTempDirectory() + fi.fileName();

  writeOutput(true, outLines, outFile);


  return outFile;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main (int argc, char*  argv[])
{

  QString pipelineFile("@DREAM3D_PIPELINE_FILE@");

  // Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("PipelineRunnerTest");


  // We need to change our working directory into the "DREAM3D_Data" directory because all the pipelines use relative paths
  QDir dataDir = QDir(getDream3dDataDir());
#ifdef _MSC_VER
  _chdir(dataDir.absolutePath().toLatin1().constData());
#else
  chdir(dataDir.absolutePath().toLatin1().constData());
#endif


  // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();
  DREAM3DPluginLoader::LoadPluginFilters(fm);

  // Send progress messages from PipelineBuilder to this object for display
  QMetaObjectUtilities::RegisterMetaTypes();


  int err = 0;
  // Read in the contents of the PipelineList file which contains all the Pipelines that we want
  // to execute
  QString contents;
  {
    // Read the Source File
    QFileInfo fi(getPipelineListFile());
    QFile source(getPipelineListFile());
    source.open(QFile::ReadOnly);
    contents = source.readAll();
    source.close();
  }
  // Split the file into tokens using the newline character
  QStringList list = contents.split(QRegExp("\\n"));
  QStringListIterator sourceLines(list);

  // Iterate over all the entries in the file and process each pipeline. Note that the order of the
  // pipelines will probably matter
  while (sourceLines.hasNext())
  {
    QString pipelineFile = sourceLines.next();
    pipelineFile = pipelineFile.trimmed();
    if(pipelineFile.isEmpty()) { continue; }
    try
    {
      QFileInfo fi(pipelineFile);

      pipelineFile = AdjustOutputDirectory(pipelineFile);

      DREAM3D::unittest::CurrentMethod = fi.fileName().toStdString();
      DREAM3D::unittest::numTests++;

      ExecutePipeline(pipelineFile);

      TestPassed(fi.fileName().toStdString());
      DREAM3D::unittest::CurrentMethod = "";
    }
    catch (TestException& e)
    {
      TestFailed(DREAM3D::unittest::CurrentMethod);
      std::cout << e.what() << std::endl;
      err = EXIT_FAILURE;
    }
  }

  return err;
}

