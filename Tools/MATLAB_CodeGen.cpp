/* ============================================================================
 * Copyright (c) 2014 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2014 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <iostream>

#include <tclap/CmdLine.h>
#include <tclap/ValueArg.h>


#include <QtCore/QCoreApplication>
#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QMetaProperty>

// DREAM3DLib includes
#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/DREAM3DVersion.h"
#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/FilterFactory.hpp"
#include "DREAM3DLib/Plugin/DREAM3DPluginInterface.h"
#include "DREAM3DLib/Plugin/DREAM3DPluginLoader.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateCodeForFilter(const QString &outDir, AbstractFilter::Pointer filter)
{
  QString filterClassName = filter->getNameOfClass();

  // Generate our Output File Name for this filter. If we are going to generate a single file for everything then,
  // We need to think abit more about how this is setup. There is a potential for 2 plugins to clash with a filter's
  // C++ Class name, which would be interesting to say the least.

  QFile f(outDir + QDir::separator() + filter->getCompiledLibraryName() + "_" + filterClassName + ".m");
  f.open(QFile::WriteOnly);
  QTextStream out(&f);


  QVector<FilterParameter::Pointer> options = filter->getFilterParameters();

  out << "Filter_Class_Name=" << filterClassName << '\n';

  for (QVector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
  {
    FilterParameter* option = (*iter).get();
    if (option->getHumanLabel().compare("Required Information") == 0
        || option->getHumanLabel().compare("Created Information") == 0
        || option->getHumanLabel().compare("Optional Information") == 0
        || option->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0)
    { continue; }

    // Joe, This is the code you will need to modify to output something that is valid
    // MATLAB code and in some format that is easily used in MATLAB
    out << "Property=" << option->getPropertyName() << " Type=" << option->getWidgetType() << '\n';

  }

  f.close();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateMatlabCode(const QString &outDir)
{

  // Sanity check to make sure we have our output directory:
  QDir dir(outDir);
  dir.mkpath(".");


  // Get the FilterManager which will have a list of ALL of our
  FilterManager::Pointer fm = FilterManager::Instance();
  FilterManager::Collection factories = fm->getFactories();
  QMapIterator<QString, IFilterFactory::Pointer> iter(factories);
  // Loop on each filter
  while(iter.hasNext())
  {
    iter.next();
    IFilterFactory::Pointer factory = iter.value();
    AbstractFilter::Pointer filter = factory->create();

    GenerateCodeForFilter(outDir, filter);
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  // Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("MATLAB_CodeGen");

  std::cout << "MATLAB_CodeGen Starting. Version " << DREAM3DLib::Version::PackageComplete().toStdString() << std::endl;

  try
  {
    // Handle program options passed on command line.
    TCLAP::CmdLine cmd("PhToHDF5", ' ', DREAM3DLib::Version::Complete().toStdString());

    TCLAP::ValueArg<std::string> outputDirArg( "o", "outputDirectory", "Output Directory", true, "", "Output Directory");
    cmd.add(outputDirArg);

    // Parse the argv array.
    cmd.parse(argc, argv);
    if (argc == 1)
    {
      qDebug() << "PhToHDF5 program was not provided any arguments. Use the --help argument to show the help listing.";
      return EXIT_FAILURE;
    }


    QString outDir = QString::fromStdString(outputDirArg.getValue());


    // Register all the filters including trying to load those from Plugins
    FilterManager::Pointer fm = FilterManager::Instance();
    DREAM3DPluginLoader::LoadPluginFilters(fm.get());


    // Send progress messages from PipelineBuilder to this object for display
    qRegisterMetaType<PipelineMessage>();

    GenerateMatlabCode(outDir);
  }
  catch (TCLAP::ArgException &e) // catch any exceptions
  {
    std::cerr << " error: " << e.error() << " for arg " << e.argId();
    return EXIT_FAILURE;
  }
  return 0;
}
