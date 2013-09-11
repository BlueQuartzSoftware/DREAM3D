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
#include "PMFilterGenerator.h"

#include <iostream>


#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PMFilterGenerator::PMFilterGenerator(QString outputDir, QString pathTemplate,
                                     QString fileName,
                                     QString className,
                                     QString codeTemplateResourcePath,
                                     QTreeWidgetItem* wi,
                                     QObject* parent) :
PMFileGenerator(outputDir, pathTemplate, fileName, codeTemplateResourcePath, wi, parent),
m_ClassName(className)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PMFilterGenerator::~PMFilterGenerator()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PMFilterGenerator::pluginNameChanged(const QString &pluginName)
{
 // qDebug() << "PMFilterGenerator::pluginNameChanged" << "\n";
  // Just call the super class's implementation
  PMFileGenerator::pluginNameChanged(pluginName);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PMFilterGenerator::outputDirChanged(const QString &outputDir)
{
 // qDebug() << "PMFilterGenerator::outputDirChanged" << "\n";
  setOutputDir(outputDir);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PMFilterGenerator::generateOutput()
{
 // qDebug() << "PMFilterGenerator::generateOutput" << "\n";
  if (doesGenerateOutput() == false)
  {
    return;
  }

  //Get text field values from widget
  QString pluginName = getPluginName();
  QString pluginDir = getOutputDir();

  if (pluginName.isEmpty() == true || pluginDir.isEmpty() == true)
  {
      return;
  }
  QString classNameLowerCase = m_ClassName.toLower();

  //Open file
    QFile rfile(getCodeTemplateResourcePath());
    if ( rfile.open(QIODevice::ReadOnly | QIODevice::Text) ) {
      QTextStream in(&rfile);
      QString text = in.readAll();
      text.replace("@PluginName@", pluginName);
      text.replace("@ClassName@", m_ClassName);
      text.replace("@MD_FILE_NAME@", m_ClassName + ".md");
      text.replace("@ClassNameLowerCase@", classNameLowerCase);
      text.replace("@FilterGroup@", pluginName);
      text.replace("@FilterSubgroup@", pluginName);


      QString parentPath = getOutputDir() + QDir::separator() + getPathTemplate().replace("@PluginName@", getPluginName());
      parentPath = QDir::toNativeSeparators(parentPath);

      QDir dir(parentPath);
      dir.mkpath(parentPath);

      parentPath = parentPath + QDir::separator() + getFileName();
      //Write to file
      QFile f(parentPath);
      if ( f.open(QIODevice::WriteOnly | QIODevice::Text) ) {
        QTextStream out(&f);
        out << text;
      }

    }



}
