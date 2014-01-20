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


#include <QtCore/QCoreApplication>
#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QFile>

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
void generateQProperties(IFilterFactory::Pointer factory)
{
  AbstractFilter::Pointer filter = factory->create();
  QVector<FilterParameter::Pointer> parameters = filter->getFilterParameters();

  for(int i = 0; i < parameters.size(); i++)
  {
    std::cout << "   " << parameters[i]->getValueType().toStdString() << "  " << parameters[i]->getPropertyName().toStdString() << std::endl;


  }




}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoopOnFilters()
{
  FilterManager::Pointer fm = FilterManager::Instance();
  FilterManager::Collection factories = fm->getFactories();

  FilterManager::CollectionIterator i(factories);
  int count = 0;
  while (i.hasNext()) {
    i.next();
    std::cout << ++count << ": " << i.key().toStdString() << ": " << std::endl;
    IFilterFactory::Pointer factory = i.value();
    generateQProperties(factory);
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
  QCoreApplication::setApplicationName("FilterParameterTool");

  std::cout << "FilterParameterTool Starting. Version " << DREAM3DLib::Version::PackageComplete().toStdString() << std::endl;


  // Register all the filters including trying to load those from Plugins
  FilterManager::Pointer fm = FilterManager::Instance();
  DREAM3DPluginLoader::LoadPluginFilters(fm.get());


  // Send progress messages from PipelineBuilder to this object for display
  qRegisterMetaType<PipelineMessage>();

  int err = 0;

  LoopOnFilters();

  return 0;
}
