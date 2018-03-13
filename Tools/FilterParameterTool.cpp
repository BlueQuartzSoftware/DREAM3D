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
#include <stdint.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QMetaProperty>
#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtCore/QUuid>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QSet>

// DREAM3DLib includes
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Plugin/PluginManager.h"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"

#include "DREAM3DToolsConfiguration.h"

#define OVERWRITE_SOURCE_FILE 1

QJsonDocument                 m_JsonDoc;


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString quote(const QString& str)
{
  return QString("\"%1\"").arg(str);
}

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
    QString tmpPath = "C:/temp/" + fi2.fileName();
    QFile hOut(tmpPath);
#endif
    hOut.open(QFile::WriteOnly);
    QTextStream stream(&hOut);
    stream << outLines.join("\n");
    hOut.close();

    qDebug() << "Saved File " << fi2.absoluteFilePath();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void writeOutput(bool didReplace, QVector<QString>& outLines, QString filename)
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
    QTextStream stream(&hOut);
    for(qint32 i = 0; i < outLines.size() - 1; i++)
    {
      stream << outLines[i] << "\n";
    }
    hOut.close();

    qDebug() << "Saved File " << fi2.absoluteFilePath();
  }
}


QString findPath(const QString& groupName, const QString& filtName, const QString ext)
{
  //  std::cout << groupName.toStdString() << "::" << filtName.toStdString() << std::endl;
  QString prefix = D3DTools::GetDREAM3DSourceDir() + "/";
  {
    QString path = D3DTools::GetSIMPLibDir() + "/" + groupName + "Filters/" + filtName + ext;
    QFileInfo fi(path);
    if(fi.exists() == true)
    {
      return path;
    }
  }

  PluginManager* pm = PluginManager::Instance();
  QStringList libs = pm->getPluginNames();

  prefix = D3DTools::GetSIMPLibPluginDir();


  for (int i = 0; i < libs.size(); ++i)
  {
    QString path = prefix + "/" + libs.at(i) + "/" + libs.at(i) + "Filters/" + filtName + ext;
    // std::cout << "    ****" << path.toStdString() << std::endl;

    QFileInfo fi(path);
    if(fi.exists() == true)
    {
      return path;
    }
  }


  prefix = D3DTools::GetDREAM3DProjParentDir() + "/DREAM3D_Plugins";
  for (int i = 0; i < libs.size(); ++i)
  {
    QString path = prefix + "/" + libs.at(i) + "/" + libs.at(i) + "Filters/" + filtName + ext;
    //  std::cout << "    ****" << path.toStdString() << std::endl;

    QFileInfo fi(path);
    if(fi.exists() == true)
    {
      return path;
    }
  }

  qDebug() << "Error Finding File for " << groupName << "/" << filtName << "/" << ext;
  return "NOT FOUND";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddPybindMacros(QString absPath)
{
  QString filterName;
  QString contents;
  {
    // Read the Source File
    QFileInfo fi(absPath);
    // if (fi.baseName().compare("DataContainerReader") != 0)
    // {
    //   return;
    // }
    filterName = fi.baseName();
    QFile source(absPath);
    source.open(QFile::ReadOnly);
    contents = source.readAll();
    source.close();
  }

  qDebug() << absPath;

  QStringList names;
  bool didReplace = false;

  QVector<int> lines(0);
  QList<QString> pybindLines;
  bool fileIsFilter = false;
  int qObjectMacroLine = 0;
  int currentLine = 0;
  QString searchString = "SIMPL_FILTER_PARAMETER";
  QString instanceString = "SIMPL_INSTANCE_PROPERTY";
  QStringList cppCodeLines;
  QVector<QString> outLines;
  QStringList list = contents.split(QRegExp("\\n"));
  QStringListIterator sourceLines(list);
  QStringList includes;
  QString superclass("FOO");
  while(sourceLines.hasNext())
  {
    QString line = sourceLines.next();
    QString line2 = line;
    
    if(line.contains(searchString))
    {
      line = line.trimmed();
      QStringList tokens = line.split("(");
      tokens = tokens[1].replace(")", "").trimmed().split(", ");
      QString paramType = tokens[0];
      QString paramVarName = tokens[1];
      QString pybindMacro;
      QTextStream out(&pybindMacro);
      out << "    PYB11_PROPERTY("<<paramType << " " << paramVarName << " READ get"<<paramVarName << " WRITE set"
      << paramVarName << ")";
      pybindLines.append(pybindMacro);
      qDebug() << pybindMacro;
      didReplace = true;
    }
    
    if(line.contains(": public AbstractFilter"))
    {
      fileIsFilter = true;
      superclass = "AbstractFilter";
    }
    if (line.contains(": public FileReader"))
    {
      fileIsFilter = true;
      superclass = "FileReader";
    }
    if( line.contains(": public FileWriter"))
    {
      fileIsFilter = true;
      superclass = "FileWriter";
    }
    if(line.contains(": public AlignSections"))
    {
      fileIsFilter = true;
      superclass = "AlignSections";
    }
    if(line.contains(": public EMMPMFilter"))
    {
      fileIsFilter = true;
      superclass = "EMMPMFilter";
    }
    if(line.contains(": public SegmentFeatures"))
    {
      fileIsFilter = true;
      superclass = "SegmentFeatures";
    }
    if(line.contains(": public GroupFeatures"))
    {
      fileIsFilter = true;
      superclass = "GroupFeatures";
    }
    if(line.contains(": public SampleSurfaceMesh"))
    {
      fileIsFilter = true;
      superclass = "SampleSurfaceMesh";
    }
    if(line.contains(": public SurfaceMeshFilter"))
    {
      fileIsFilter = true;
      superclass = "SurfaceMeshFilter";
    }

    
    if(line.contains("Q_OBJECT") && fileIsFilter)
    {
      line2 = line2 + "\n    PYB11_CREATE_BINDINGS(" + filterName + " SUPERCLASS " + superclass + ")";
      line = line + "\n    PYB11_CREATE_BINDINGS(" + filterName + " SUPERCLASS " + superclass + ")";

      qObjectMacroLine = currentLine + 2;
    }
    if(line.contains("PYB11_CREATE_BINDINGS"))
    {
      qObjectMacroLine = currentLine + 1;
    }
    
    outLines.push_back(line2);
    currentLine++;
  }

  outLines.insert(qObjectMacroLine, pybindLines.join("\n"));

  writeOutput(didReplace, outLines, absPath);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReplaceGrepSearchesRecursively(QDir currentDir)
{

  QStringList filters;
  // filters.append("*.cpp");
  filters.append("*.h");

  if(currentDir.dirName().compare("zRel") == 0 || currentDir.dirName().compare("Build") == 0)
  {
    return;
  }
  // Get a list of all the directories
  QFileInfoList dirList = currentDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
  if(dirList.size() > 0)
  {
    foreach(QFileInfo fi, dirList)
    {
      ReplaceGrepSearchesRecursively(QDir(fi.absoluteFilePath())); // Recursive call
    }
  }

  QFileInfoList itemList = currentDir.entryInfoList(filters);
  foreach(QFileInfo itemInfo, itemList)
  {
    QString headerFilePath = itemInfo.absoluteFilePath();
    AddPybindMacros(headerFilePath);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{

  Q_ASSERT(true); // We don't want anyone to run this program.
  // Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("FilterParameterTool");

// std::cout << "FilterParameterTool Starting. Version " << SIMPLib::Version::PackageComplete().toStdString() << std::endl;


  // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();
  SIMPLibPluginLoader::LoadPluginFilters(fm);

  // Send progress messages from PipelineBuilder to this object for display
  qRegisterMetaType<PipelineMessage>();


//  ReplaceGrepSearchesRecursively(QDir(D3DTools::GetDREAM3DProjDir() + "/ExternalProjects/SIMPL/Source/SIMPLib"));
  //ReplaceGrepSearchesRecursively(QDir(D3DTools::GetDREAM3DProjDir() + "/Source/Plugins"));
  ReplaceGrepSearchesRecursively(QDir(D3DTools::GetDREAM3DProjDir() + "/ExternalProjects/Plugins"));
  ReplaceGrepSearchesRecursively(QDir(D3DTools::GetDREAM3DProjDir() + "/../DREA3D_Plugins"));



  return 0;
}
