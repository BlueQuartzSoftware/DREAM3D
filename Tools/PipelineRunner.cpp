/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

// C Includes
#include <stdlib.h>
#include <assert.h>

// C++ Includes
#include <QtCore/QString>
#include <iostream>

// TCLAP Includes
#include <tclap/CmdLine.h>
#include <tclap/ValueArg.h>

// Boost includes
#include <boost/assert.hpp>

// Qt Includes
#include <QtCore/QtDebug>
#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QSettings>

// DREAM3DLib includes
#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/FilterFactory.hpp"
#include "DREAM3DLib/Common/FilterPipeline.h"
#include "DREAM3DLib/FilterParameters/QFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/QFilterParametersWriter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void readPipeline(QFilterParametersReader::Pointer paramsReader, FilterPipeline::Pointer pipeline)
{
  FilterManager::Pointer filtManager = FilterManager::Instance();
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
int main (int argc, char  *argv[])
{

  Observer obs;
  int err = 0;

  FilterManager::Pointer filtManager = FilterManager::Instance();
  // THIS IS A VERY IMPORTANT LINE: It will register all the known filters in the dream3d library. This
  // will NOT however get filters from plugins. We are going to have to figure out how to compile filters
  // into their own plugin and load the plugins from a command line.
  filtManager->RegisterKnownFilters(filtManager.get());

  qDebug() << "Current Path: " << QDir::currentPath();

  FilterPipeline::Pointer pipeline = FilterPipeline::New();
  QFilterParametersReader::Pointer paramsReader = QFilterParametersReader::New();
  paramsReader->openFile(argv[1]);
  readPipeline(paramsReader, pipeline);
  err = pipeline->preflightPipeline();

  pipeline->execute();
  err = pipeline->getErrorCondition();

}

