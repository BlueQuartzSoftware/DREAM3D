/* ============================================================================
 * Copyright (c) 2016 BlueQuartz Softwae, LLC
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "GenerateSGPipelineFile.h"

// Qt Includes
#include <QtCore/QCommandLineOption>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QtDebug>

#include "OrientationLib/Texture/StatsGen.hpp"

int main(int argc, char* argv[])
{
  // Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QCoreApplication* app = new QCoreApplication(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("GenerateSGPipeline");
  QCoreApplication::setApplicationVersion(GenerateSGPipeline::Version::Major + "." + GenerateSGPipeline::Version::Minor + "." + GenerateSGPipeline::Version::Patch);

  QCommandLineParser parser;
  QString str;
  QTextStream ss(&str);
  ss << "Pipeline Runner (" << GenerateSGPipeline::Version::Major << "." << GenerateSGPipeline::Version::Minor << "." << GenerateSGPipeline::Version::Patch << "):  ";
  parser.setApplicationDescription(str);
  parser.addHelpOption();
  parser.addVersionOption();

  // A boolean option with a single name (-p)
  QCommandLineOption pipelineFileArg(QStringList() << "p"
                                                   << "pipeline",
                                     "Pipeline File as a JSON file.", "file");
  parser.addOption(pipelineFileArg);

  parser.addOption({"mu", "Value of Mu which is log(ESD)"});
  parser.addOption({"sigma", "Value of Sigma"});
  parser.addOption({"min", "Minimum log(ESD) value."});
  parser.addOption({"max", "Maximum log(ESD) value."});
  parser.addOption({"step", "Width of the bins"});
  parser.addOption({"esd", "Actual unitless Feature Equivelant Sphere Diameter you wish to have."});

  // Process the actual command line arguments given by the user
  parser.process(*app);

  return 0;
}
