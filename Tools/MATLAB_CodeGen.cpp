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

#include <iostream>

#include <tclap/CmdLine.h>
#include <tclap/ValueArg.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QMetaProperty>
#include <QtCore/QString>

// DREAM3DLib includes
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/SIMPLibVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateCodeForFilter(const QString& outDir, AbstractFilter::Pointer filter)
{
  QString filterClassName = filter->getNameOfClass();

  // Generate our Output File Name for this filter. If we are going to generate a single file for everything then,
  // We need to think abit more about how this is setup. There is a potential for 2 plugins to clash with a filter's
  // C++ Class name, which would be interesting to say the least.

  QFile f(outDir + QDir::separator() + filterClassName + ".m");
  f.open(QFile::WriteOnly);
  QTextStream out(&f);

  QVector<FilterParameter::Pointer> options = filter->getFilterParameters();

  out << "function [ Filter_Parts ] = " << filterClassName << '\n';

  int i = 1;
  for(QVector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter)
  {
    SeparatorFilterParameter::Pointer sepFiltParam = SeparatorFilterParameter::New();
    FilterParameter* option = (*iter).get();
    if(option->getHumanLabel().compare("Required Information") == 0 || option->getHumanLabel().compare("Created Information") == 0 || option->getHumanLabel().compare("Optional Information") == 0 ||
       option->getWidgetType().compare(sepFiltParam->getWidgetType()) == 0)
    {
      continue;
    }

    // creates a working MATLAB m-file function per file per filter
    out << "Filter_Parts{1}{" << i << "} = '" << option->getPropertyName() << "';\n"
        << "Filter_Parts{2}{" << i << "} = '" << option->getWidgetType() << "';\n";
    ++i;
  }

  out << "end";

  f.close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateMatlabCode(const QString& outDir)
{

  // Sanity check to make sure we have our output directory:
  QDir dir(outDir);
  dir.mkpath(".");

  // Get the FilterManager which will have a list of ALL of our
  FilterManager* fm = FilterManager::Instance();
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
int main(int argc, char* argv[])
{
  // Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("MATLAB_CodeGen");

  std::cout << "MATLAB_CodeGen Starting. Version " << SIMPLib::Version::PackageComplete().toStdString() << std::endl;

  try
  {
    // Handle program options passed on command line.
    TCLAP::CmdLine cmd("PhToHDF5", ' ', SIMPLib::Version::Complete().toStdString());

    TCLAP::ValueArg<std::string> outputDirArg("o", "outputDirectory", "Output Directory", true, "", "Output Directory");
    cmd.add(outputDirArg);

    // Parse the argv array.
    cmd.parse(argc, argv);
    if(argc == 1)
    {
      qDebug() << "PhToHDF5 program was not provided any arguments. Use the --help argument to show the help listing.";
      return EXIT_FAILURE;
    }

    QString outDir = QString::fromStdString(outputDirArg.getValue());

    // Register all the filters including trying to load those from Plugins
    FilterManager* fm = FilterManager::Instance();
    SIMPLibPluginLoader::LoadPluginFilters(fm);

    // Send progress messages from PipelineBuilder to this object for display
    qRegisterMetaType<PipelineMessage>();

    GenerateMatlabCode(outDir);
  } catch(TCLAP::ArgException& e) // catch any exceptions
  {
    std::cerr << " error: " << e.error() << " for arg " << e.argId();
    return EXIT_FAILURE;
  }
  return 0;
}
