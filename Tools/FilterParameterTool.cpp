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
QString quote(const QString& str)
{
  return QString("\"%1\"").arg(str);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void writeOutput(bool didReplace, QStringList &outLines, QString filename)
{
  if(didReplace == true)
  {
    QFileInfo fi2(filename);
#if 1
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
void checkConstructorForSetupFilterParameters(QStringListIterator &sourceLines, QStringList &outLines)
{

  // get through the initializer list
  while(sourceLines.hasNext())
  {
    QString line = sourceLines.next();
    outLines.push_back(line);
    if(line.contains("{") )
    {
      break;
    }
  }

  // Now walk the body of the constructor looking for the setupFilterParameters(); call
  bool foundFunction = false;
  while(sourceLines.hasNext())
  {
    QString line = sourceLines.next();

    if(line.contains("}"))
    {
      if (foundFunction == false )
      {
        outLines.push_back(QString("  setupFilterParameters();") );
      }
      outLines.push_back(line);
      break;
    }
    else if(line.contains("setupFilterParameters();"))
    {
      outLines.push_back(line);
      foundFunction = true;
    }
    else
    {
      outLines.push_back(line);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float positionInHeader(const QString hFile, const QString var, bool isPointer)
{
  QString contents;
  {
    // Read the Source File
    QFileInfo fi(hFile);
    //    if (fi.baseName().compare("EBSDSegmentFeatures") != 0)
    //    {
    //      return false;
    //    }

    QFile source(hFile);
    source.open(QFile::ReadOnly);
    contents = source.readAll();
    source.close();
  }


  QStringList names;
  bool found = false;
  int lineNum = 1;
  QString searchString = var;

  QStringList list = contents.split(QRegExp("\\n"));
  QStringListIterator sourceLines(list);
  //Look for raw pointers FIRST otherwise we get the wrong line.
  if(found == false && isPointer == true)
  {
    searchString = var;
    list = contents.split(QRegExp("\\n"));
    sourceLines.toFront(); // reset the iterator to the beginning

    lineNum = 1;
    while (sourceLines.hasNext())
    {
      QString line = sourceLines.next();
      if(line.contains(searchString) && line.contains("DEFINE_PTR_WEAKPTR_DATAARRAY") )
      {
        line = sourceLines.next();
        found = true;
        return (float)lineNum + 0.1f;
      }
      lineNum++;
    }
  }


  if(found == false && isPointer == true)
  {
    searchString = var;
    list = contents.split(QRegExp("\\n"));
    sourceLines.toFront(); // reset the iterator to the beginning

    lineNum = 1;
    while (sourceLines.hasNext())
    {
      QString line = sourceLines.next();
      if(line.contains(searchString) && line.contains("DEFINE_CREATED_DATAARRAY") )
      {
        line = sourceLines.next();
        found = true;
        return (float)lineNum + 0.1f;
      }
      lineNum++;
    }
  }

  searchString = var;
  list = contents.split(QRegExp("\\n"));
  sourceLines.toFront(); // reset the iterator to the beginning
  lineNum = 1;
  while (sourceLines.hasNext())
  {
    QString line = sourceLines.next();
    if(line.contains(searchString) )
    {
      int offset = line.indexOf(searchString);
      int size = searchString.size();
      // Make sure we find the whole word?
      if(line.at(offset + size).isLetterOrNumber())
      {
        continue;
      }
      if(line.contains("Q_PROPERTY") )
      {
        continue;
      }
      line = sourceLines.next();
      found = true;
      return (float)lineNum;
    }
    lineNum++;
  }

  // Variable was not found so it _might_ be embedded in a macro
  if(found == false && var.contains("ArrayName"))
  {
    int offset = var.indexOf("ArrayName");
    searchString = var.mid(0, offset); // extract out the variable name without the "ArrayName" on the end of it

    list = contents.split(QRegExp("\\n"));
    sourceLines.toFront(); // reset the iterator to the beginning

    lineNum = 1;
    while (sourceLines.hasNext())
    {
      QString line = sourceLines.next();
      if(line.contains(searchString) && line.contains("DEFINE_PTR_WEAKPTR_DATAARRAY") )
      {
        line = sourceLines.next();
        found = true;
        return (float)lineNum;
      }
      lineNum++;
    }
  }

  if(found == false) { lineNum = -1; }

  return (float)lineNum;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void fixInitializerList(QStringListIterator &sourceLines, QStringList &outLines, const QString& hFile, const QString& cppFile)
{
  bool isPointer = false;
  QMap<float, QString> orderedInitList;

  // get through the initializer list
  while(sourceLines.hasNext())
  {
    QString line = sourceLines.next();
    if(line.contains("{") )
    {
      break;
    }

    int offset = line.indexOf("m_");
    int offset2 = line.indexOf("(");
    QString var = line.mid(offset + 2, offset2 - offset - 2);
    if(line.contains("(NULL)") )
    {
      isPointer = true;
    }
    else
    {
      isPointer = false;
    }
    float index = positionInHeader(hFile, var, isPointer);
    orderedInitList[index] = line;
//    qDebug() << "index: " << index << "   line:" << line;
  }

//qDebug() << "--------------------------------";
  QMapIterator<float, QString> i(orderedInitList);
  while (i.hasNext())
  {
    i.next();
    QString line = i.value();
    if(line.trimmed().isEmpty()) { continue; } // Eat the blank line
    if(i.hasNext() && line.endsWith(",") == false)
    { line = line + ",";}
    if(i.hasNext() == false && line.endsWith(",") )
    { line = line.mid(0, line.size() -1 ); }
    outLines.push_back(line);
    qDebug() << "index: " << i.key() << "   line:" << line;
  }


  outLines.push_back("{");

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool fixFile( AbstractFilter::Pointer filter, const QString& hFile, const QString& cppFile)
{
  QString contents;
  {
    // Read the Source File
//    QFileInfo fi(cppFile);
//    if (fi.baseName().compare("SurfaceMeshToNonconformalVtk") != 0)
//    {
//      return false;
//    }

    QFile source(cppFile);
    source.open(QFile::ReadOnly);
    contents = source.readAll();
    source.close();
  }


  QStringList names;
  bool didReplace = false;

  QString searchString = filter->getNameOfClass() + "::" + filter->getNameOfClass();
  QStringList outLines;
  QStringList list = contents.split(QRegExp("\\n"));
  QStringListIterator sourceLines(list);

  int index = 0;
  while (sourceLines.hasNext())
  {
    QString line = sourceLines.next();
    if(line.contains(searchString) )
    {
      outLines.push_back(line);
      line = sourceLines.next();
      outLines.push_back(line); // get the call to the superclass

      fixInitializerList(sourceLines, outLines, hFile, cppFile);
      didReplace = true;
    }
    else
    {
      outLines.push_back(line);
    }
  }


  writeOutput(didReplace, outLines, cppFile);
  index++;

  return didReplace;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString findPath(const QString& groupName, const QString& filtName, const QString ext)
{
  QString prefix("/Users/mjackson/Workspace/DREAM3D_Rewrite/Source/");
  {
    QString path = prefix + "DREAM3DLib/" + groupName + "Filters/" + filtName + ext;
    QFileInfo fi(path);
    if(fi.exists() == true)
    {
      return path;
    }
  }

  prefix = prefix + "Plugins/";
  QStringList libs;
  libs << "DDDAnalysisToolbox" << "ImageImport" << "OrientationAnalysis" << "Processing" <<  "Reconstruction" << "Sampling" << "Statistics"  << "SurfaceMeshing" << "SyntheticBuilding";

  for (int i = 0; i < libs.size(); ++i)
  {
    QString path = prefix + libs.at(i) + "/" + libs.at(i) + "Filters/" + filtName + ext;
    //  std::cout << "****" << path.toStdString() << std::endl;

    QFileInfo fi(path);
    if(fi.exists() == true)
    {
      return path;
    }
  }
  return "NOT FOUND";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFilterParametersCode()
{

  FilterManager::Pointer fm = FilterManager::Instance();
  FilterManager::Collection factories = fm->getFactories();
  QMapIterator<QString, IFilterFactory::Pointer> iter(factories);
  // Loop on each filter
  while(iter.hasNext())
  {
    iter.next();
    IFilterFactory::Pointer factory = iter.value();
    AbstractFilter::Pointer filter = factory->create();

    QString cpp = findPath(filter->getGroupName(), filter->getNameOfClass(), ".cpp");
    QString h = findPath(filter->getGroupName(), filter->getNameOfClass(), ".h");

    fixFile(filter, h, cpp);
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Q_ASSERT(false); // We don't want anyone to run this program.
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

  GenerateFilterParametersCode();

  return 0;
}
