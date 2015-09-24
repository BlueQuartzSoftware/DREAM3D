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


#include "PMDirGenerator.h"

#include <iostream>

#include "moc_PMDirGenerator.cpp"
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PMDirGenerator::PMDirGenerator(QString outputDir, QString pathTemplate, QString dirName,
                               QString codeTemplateResourcePath, QTreeWidgetItem* wi,
                               QObject* parent) :
  QObject(parent),
  m_OutputDir(outputDir),
  m_PathTemplate(pathTemplate),
  m_DirName(dirName),
  m_CodeTemplateResourcePath(codeTemplateResourcePath),
  m_NameChangeable(false),
  m_DoesGenerateOutput(false),
  m_TreeWidgetItem(wi)
{

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PMDirGenerator::~PMDirGenerator()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PMDirGenerator::setOutputDir(QString v)
{
  m_OutputDir = v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PMDirGenerator::getPluginName()
{
  return m_PluginName;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PMDirGenerator::getOutputDir()
{
  return m_OutputDir;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PMDirGenerator::getPathTemplate()
{
  return m_PathTemplate;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PMDirGenerator::getDirName()
{
  return m_DirName;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PMDirGenerator::getCodeTemplateResourcePath()
{
  return m_CodeTemplateResourcePath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PMDirGenerator::setNameChangeable(bool v)
{
  m_NameChangeable = v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PMDirGenerator::isNameChangeable()
{
  return m_NameChangeable;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PMDirGenerator::setDoesGenerateOutput(bool v)
{
  m_DoesGenerateOutput = v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PMDirGenerator::doesGenerateOutput()
{
  return m_DoesGenerateOutput;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QTreeWidgetItem* PMDirGenerator::getTreeWidgetItem()
{
  return m_TreeWidgetItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PMDirGenerator::setDisplaySuffix(QString v)
{
  m_DisplaySuffix = v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PMDirGenerator::getDisplaySuffix()
{
  return m_DisplaySuffix;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PMDirGenerator::setPluginName(QString pluginName)
{
  m_PluginName = pluginName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PMDirGenerator::pluginNameChanged(const QString& pluginname)
{
//  qDebug() << "PMDirGenerator::pluginNameChanged" << "\n";
  if (isNameChangeable() == false)
  {
    return;
  }
  m_PluginName = cleanName(pluginname);

  if (NULL != m_TreeWidgetItem)
  {
    m_TreeWidgetItem->setText(0, m_PluginName + m_DisplaySuffix);
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PMDirGenerator::outputDirChanged(const QString& outputDir)
{
//  qDebug() << "PMDirGenerator::outputDirChanged" << "\n";
  m_OutputDir = outputDir;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PMDirGenerator::generateOutput()
{
//  qDebug() << "PMDirGenerator::generateOutput" << "\n";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PMDirGenerator::cleanName(QString name)
{
  //Remove all uses of "Plugin", "plugin", "Filter", and "filter"
  QRegExp rx("Filter|filter");
  name = name.replace(rx, "");
  //Remove all spaces and illegal characters from plugin name
  name = name.trimmed();
  name = name.remove(" ");
  name = name.remove(QRegExp("[^a-zA-Z_\\d\\s]"));
  return name;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PMDirGenerator::generateFileContents(QString replaceStr)
{
  return "";
}
