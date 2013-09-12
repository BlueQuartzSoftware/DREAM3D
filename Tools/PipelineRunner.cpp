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
#include "QtGui/QApplication"
#include "QtCore/QSettings"

// DREAM3DLib includes
#include "DREAM3DLib/DREAM3DVersion.h"
#include "DREAM3DLib/Common/DataArray.hpp"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"

#include "FilterWidgets/FilterWidgetsLib.h"
#include "PipelineBuilder/FilterWidgetManager.h"
#include "PipelineBuilder/IFilterWidgetFactory.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main (int argc, char  *argv[])
{

  QuatF q = QuaternionMathF::New(0.565907, -0.24196, 0.106982, -0.7808710);
  float euler[3] = {0.0, 0.0, 0.0};


  OrientationMath::QuattoEuler(q, euler[0], euler[1], euler[2]);

//2.6014, 1.32595, 3.40947
  std::cout << "Quat:  " << q.x << ", " << q.y << ", " << q.z << ", " << q.w << std::endl;
  std::cout << "Euler: " << euler[0] << ", " << euler[1] << ", "  << euler[2] << std::endl;
}


#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main (int argc, char  *argv[])
{
  QApplication app(argc, argv);

  FilterWidgetsLib::RegisterKnownQFilterWidgets();




  FilterWidgetManager::Pointer wm = FilterWidgetManager::Instance();

  FilterWidgetManager::Collection allFactories = wm->getFactories();


  for(FilterWidgetManager::Collection::iterator iter = allFactories.begin(); iter != allFactories.end(); ++iter)
  {
    QString filterName = ((*iter).first);
    std::cout << "Writing " << filterName() << std::endl;
    IFilterWidgetFactory::Pointer wf = (*iter).second;
    if (NULL == wf) { return EXIT_FAILURE;}
    QFilterWidget* w = wf->createWidget();
    QString filePath = QString("/tmp/") + filterName + QString(".ini");
    QSettings prefs(filePath, QSettings::IniFormat, NULL);
    prefs.beginGroup(QString("FILTER_NUMBER"));
    w->writeOptions(prefs);
    prefs.endGroup();

  }


  return EXIT_SUCCESS;
}




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void readSettings(QSettings &prefs)
{

  //PipelineViewWidget viewWidget;

  prefs.beginGroup("PipelineBuilder");

  bool ok = false;
  int filterCount = prefs.value("Number_Filters").toInt(&ok);
  prefs.endGroup();

  if (false == ok) {filterCount = 0;}
  for (int i = 0; i < filterCount; ++i)
  {
    QString gName = QString::number(i);

    prefs.beginGroup(gName);

    QString filterName = prefs.value("Filter_Name", "").toString();

    std::cout << "Adding Filter " << filterName() << std::endl;
//    QFilterWidget* w = viewWidget.addFilter(filterName); // This will set the variable m_SelectedFilterWidget
//    if(w) {
//      w->blockSignals(true);
//      w->readOptions(prefs);
//      w->blockSignals(false);
//      //w->emitParametersChanged();
//    }
    prefs.endGroup();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main (int argc, char const *argv[])
{
   // This code is NOT READY to run AT ALL. IT was just a PLACE TO START
  BOOST_ASSERT(false);
  QString configFile;

  try
  {

   // Handle program options passed on command line.
    TCLAP::CmdLine cmd("PipelineRunner", ' ', DREAM3DLib::Version::Complete());

    TCLAP::ValueArg<std::string> inputFileArg( "c", "config", "The text file containing the pipeline information.", true, "", "Pipeline Config File");
    cmd.add(inputFileArg);


    // Parse the argv array.
    cmd.parse(argc, argv);
    if (argc == 1)
    {
      std::cout << "PipelineRunner program was not provided any arguments. Use the --help argument to show the help listing." << std::endl;
      return EXIT_FAILURE;
    }

    configFile = (inputFileArg.getValue());


  }
  catch (TCLAP::ArgException &e) // catch any exceptions
  {
    std::cerr << logTime() << " error: " << e.error() << " for arg " << e.argId() << std::endl;
    return EXIT_FAILURE;
  }


  // Create the QSettings Object
  QSettings prefs(configFile, QSettings::IniFormat, NULL);
  readSettings(prefs);

  return EXIT_SUCCESS;
}
#endif
