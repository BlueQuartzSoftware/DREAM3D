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


#include <QtCore/QCoreApplication>
#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QMetaProperty>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>
#include <QtCore/QUuid>

// DREAM3DLib includes
//#include "SIMPLib/SIMPLib.h"
//#include "SIMPLib/SIMPLibVersion.h"
//#include "SIMPLib/Plugin/PluginManager.h"
//#include "SIMPLib/Common/FilterManager.h"
//#include "SIMPLib/Common/FilterFactory.hpp"

//#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
//#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"

#include "Tools/ToolConfiguration.h"

#define OVERWRITE_SOURCE_FILE 1


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
    QTextStream stream( &hOut );
    for(qint32 i = 0; i < outLines.size() - 1; i++)
    {
      stream << outLines[i] << "\n";
    }
    hOut.close();

    qDebug() << "Saved File " << fi2.absoluteFilePath();
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void checkConstructorForSetupFilterParameters(QStringListIterator& sourceLines, QStringList& outLines)
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
    //        if (fi.baseName().compare("FillBadData") != 0)
    //        {
    //          return false;
    //        }

    QFile source(hFile);
    source.open(QFile::ReadOnly);
    contents = source.readAll();
    source.close();
  }


  //QStringList names;
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
      if(line.contains(searchString) && line.contains("DEFINE_DATAARRAY_VARIABLE") )
      {
        int index = line.indexOf(var); // Verify we actually found the whole word
        if(line.at(index - 1).isSpace() )
        {
          line = sourceLines.next();
          found = true;
          return (float)lineNum + 0.1f;
        }
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
      if(line.contains(searchString) && line.contains("DEFINE_DATAARRAY_VARIABLE") )
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
      if(line.contains(searchString) && line.contains("DEFINE_DATAARRAY_VARIABLE") )
      {
        int index = line.indexOf(searchString); // Verify we actually found the whole word
        if(index > -1 && line.at(index - 1).isSpace() )
        {
          line = sourceLines.next();
          found = true;
          return (float)lineNum;
        }
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
void fixInitializerList(QStringListIterator& sourceLines, QStringList& outLines, const QString& hFile, const QString& cppFile)
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
    if(line.contains("(nullptr)") )
    {
      isPointer = true;
    }
    else
    {
      isPointer = false;
    }
    float index = positionInHeader(hFile, var, isPointer);
    orderedInitList[index] = line;
    qDebug() << "index: " << index << "   var:" << var;
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
    { line = line.mid(0, line.size() - 1 ); }
    outLines.push_back(line);
    qDebug() << "index: " << i.key() << "   line:" << line;
  }
  outLines.push_back("{");
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool CorrectInitializerList( AbstractFilter::Pointer filter, const QString& hFile, const QString& cppFile)
{
  QString contents;
  {
    // Read the Source File
    QFileInfo fi(cppFile);
    //
    if (fi.baseName().compare("RegisterPointSets") != 0)
    {
      return false;
    }
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
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void fixFilterParameter(QStringListIterator& sourceLines, QStringList& outLines, const QString& searchString, const QString& line)
{

  int offset = line.indexOf(searchString);

  // Get the substring
  QString substring = line.mid(offset);

  // Split by commas. Hope no commas are within double quotes
  QStringList tokens = substring.split(",");

  // Get the 2nd argument
  QString second = tokens.at(1);
  second = second.replace("\"", "");
  second = second.trimmed();

  QString third = " \"\"";
  if(second.isEmpty() == false)
  {
    third = " get" + second + "()";
  }
  tokens[3] = third;

  QString final = line.left(offset);
  final = final + tokens.at(0);
  for(qint32 i = 1; i < tokens.size(); i++)
  {
    final = final + "," + tokens.at(i);
  }

  outLines.push_back(final);
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SplitFilterHeaderCodes( AbstractFilter::Pointer filter, const QString& hFile, const QString& cppFile)
{
  QString contents;
  {
    // Read the Source File
    QFileInfo fi(cppFile);
    //        if (fi.baseName().compare("CropImageGeometry") != 0)
    //        {
    //          return false;
    //        }

    QFile source(hFile);
    source.open(QFile::ReadOnly);
    contents = source.readAll();
    source.close();
  }


  QStringList names;
  bool didReplace = false;

  QString searchString = "virtual const QString getSubGroupName() {";
  QString replaceString = "    virtual const QString getSubGroupName();";
  QStringList outLines;
  QStringList list = contents.split(QRegExp("\\n"));
  QStringListIterator sourceLines(list);
  QString body;

  int index = 0;
  while (sourceLines.hasNext())
  {
    QString line = sourceLines.next();
    if(line.contains(searchString) )
    {
      outLines.push_back(replaceString);
      QStringList chomp = line.split("{");
      body = "{" + chomp.at(1);
      didReplace = true;
    }
    else
    {
      outLines.push_back(line);
    }
  }

  writeOutput(didReplace, outLines, hFile);
  index++;

  // Now update the .cpp file
  {
    QFile source(cppFile);
    source.open(QFile::ReadOnly);
    contents = source.readAll();
    source.close();
  }

  QFileInfo fi(cppFile);
  QString cName = fi.baseName();

  QTextStream out(&contents);
  out << "\n";
  out << "// -----------------------------------------------------------------------------\n";
  out << "//\n";
  out << "// -----------------------------------------------------------------------------\n";
  out << "const QString " << cName << "::getSubGroupName()\n";
  out << body << "\n\n";

  list = contents.split(QRegExp("\\n"));
  writeOutput(didReplace, list, cppFile);

  return didReplace;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ValidateParameterReader( AbstractFilter::Pointer filter, const QString& hFile, const QString& cppFile)
{
  QString contents;
  {
    // Read the Source File
    QFileInfo fi(cppFile);
    //    if (fi.baseName().compare("FindBoundingBoxFeatures") != 0)
    //    {
    //      return false;
    //    }

    QFile source(cppFile);
    source.open(QFile::ReadOnly);
    contents = source.readAll();
    source.close();
  }

  qDebug() << cppFile;

  QStringList names;
  bool didReplace = false;

  QString searchString = "readFilterParameters(AbstractFilterParametersReader* reader, int index)";
  QString endString = "reader->closeFilterGroup()";
  QStringList outLines;
  QStringList list = contents.split(QRegExp("\\n"));
  QStringListIterator sourceLines(list);
  QString body;

  int index = 0;
  while (sourceLines.hasNext())
  {
    QString line = sourceLines.next();
    if(line.contains(searchString) )
    {
      outLines.push_back(line);
      line = sourceLines.next();
      outLines.push_back(line); // Eat the line with the "{" on it
      while(true)
      {
        bool ok = true;
        line = sourceLines.next();
        if(line.contains(endString) ) { outLines.push_back(line); break; }
        else
        {

          QString origLine = line;
          line = line.trimmed();
          if(line.startsWith("set") )
          {
            int idx = line.indexOf('(');
            int sIdx = line.indexOf("set") + 3;
            QString prop = line.mid(sIdx, idx - sIdx);
            QString setProp = "set" + prop;
            QString quotedProp = "\"" + prop + "\"";
            QString getProp = "get" + prop;

            QString front("<<<<<<");
            if(line.contains(setProp) == false)
            {
              ok = false;
              front = front + "0 ";
            }
            if(line.contains(quotedProp) == false)
            {
              ok = false;
              front = front + "1 ";
            }
            if(line.contains(getProp) == false)
            {
              ok = false;
              front = front + "2 ";
            }
            if(ok)
            {
              outLines.push_back(origLine);
            }
            else
            {
              outLines.push_back(front + origLine);
            }

          }
          else
          {
            outLines.push_back(origLine);
          }
        }

      }




    }
    else
    {
      outLines.push_back(line);
    }
  }

  didReplace = true;
  writeOutput(didReplace, outLines, cppFile);
  index++;
  return didReplace;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFiltersWithMultipleDataArrayPaths(AbstractFilter::Pointer filter)
{
  //std::cout << "Filter: " << filter->getNameOfClass().toStdString() << std::endl;
  QVector<FilterParameter::Pointer> parameters = filter->getFilterParameters();
  int count = 0;
  foreach(FilterParameter::Pointer param, parameters)
  {
    if(param->getWidgetType() == FilterParameterWidgetType::DataArraySelectionWidget) { count++; }
  }
  if(count > 1)
  {
    std::cout << "| " << filter->getCompiledLibraryName().toStdString() << " | " << filter->getNameOfClass().toStdString() << " | " << count  << " | " << std::endl;
  }
}
#endif


#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
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

#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ReplaceIncludeGuard(const QString& hFile)
{
  QString contents;
  QFileInfo fi(hFile);
  {
    // Read the Source File
    if (fi.suffix().compare("h") != 0)
    {
      return false;
    }

    QFile source(hFile);
    source.open(QFile::ReadOnly);
    contents = source.readAll();
    source.close();
  }


  QStringList names;
  bool didReplace = false;

  QString searchString = "#ifndef";
  QString replaceString = "#ifndef _" + fi.baseName().toLower() + "_h_";
  QStringList outLines;
  QStringList list = contents.split(QRegExp("\\n"));
  QStringListIterator sourceLines(list);
  QString body;

  int index = 0;
  while (sourceLines.hasNext())
  {
    QString line = sourceLines.next();
    if(line.contains(searchString) )
    {
      outLines.push_back(replaceString);
      outLines.push_back("#define _" + fi.baseName().toLower() + "_h_");
      line = sourceLines.next(); // Eat the next line
      didReplace = true;
    }
    else
    {
      outLines.push_back(line);
    }
  }

  writeOutput(didReplace, outLines, hFile);
  index++;
  return didReplace;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReplaceLicenseText(QString absPath)
{
  QString contents;
  {
    // Read the Source File
    QFileInfo fi(absPath);
    //    if (fi.baseName().compare("AddFavoriteWidget") != 0)
    //    {
    //      return;
    //    }

    QFile source(absPath);
    source.open(QFile::ReadOnly);
    contents = source.readAll();
    source.close();
  }

  qDebug() << absPath;

  QStringList names;
  bool didReplace = false;

  QVector<int> lines(0);


  QString searchString = "/* ============================================================================";
  QString searchString2 = "* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */";
  QString searchString4 = " * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */";

  QString searchStrin3 = "Groeber";
  QVector<QString> outLines;
  QStringList list = contents.split(QRegExp("\\n"));
  QStringListIterator sourceLines(list);
  QMap<QString, int> lineToInclude;
  int index = 0;
  bool hasLicense = false;
  if(list.at(0).startsWith(searchString) == true)
  {
    hasLicense = true;
  }

  bool hasOldLicense = false;

  while (sourceLines.hasNext())
  {
    QString line = sourceLines.next();


    if(hasLicense == true && (line.startsWith(searchString2) || line.startsWith(searchString4)) )
    {
      hasLicense = false;

      QString license;
      QTextStream out(&license);

      out << "/* ============================================================================\n";
      out << "* Copyright (c) 2009-2016 BlueQuartz Software, LLC\n";
      out << "*\n";
      out << "* Redistribution and use in source and binary forms, with or without modification,\n";
      out << "* are permitted provided that the following conditions are met:\n";
      out << "*\n";
      out << "* Redistributions of source code must retain the above copyright notice, this\n";
      out << "* list of conditions and the following disclaimer.\n";
      out << "*\n";
      out << "* Redistributions in binary form must reproduce the above copyright notice, this\n";
      out << "* list of conditions and the following disclaimer in the documentation and/or\n";
      out << "* other materials provided with the distribution.\n";
      out << "*\n";
      out << "* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its\n";
      out << "* contributors may be used to endorse or promote products derived from this software\n";
      out << "* without specific prior written permission.\n";
      out << "*\n";
      out << "* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\"\n";
      out << "* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE\n";
      out << "* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n";
      out << "* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE\n";
      out << "* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL\n";
      out << "* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR\n";
      out << "* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER\n";
      out << "* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,\n";
      out << "* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE\n";
      out << "* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n";
      out << "*\n";
      out << "* The code contained herein was partially funded by the followig contracts:\n";
      out << "*    United States Air Force Prime Contract FA8650-07-D-5800\n";
      out << "*    United States Air Force Prime Contract FA8650-10-D-5210\n";
      out << "*    United States Prime Contract Navy N00173-07-C-2068\n";
      out << "*\n";
      out << "* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */\n";

      outLines.push_back(license);
      didReplace = true;
      continue;
    }


    if (hasLicense == true && line.startsWith(" *"))
    {
      if(line.contains(searchStrin3)) {
        hasOldLicense = true;
      }
      continue;
    }

    outLines.push_back(line);

    index++;
  }

  qDebug() << "Updating License: " << absPath;
  if (hasOldLicense) { writeOutput(didReplace, outLines, absPath); }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReplaceLicenseCodeRecursively(QDir currentDir)
{

  QStringList filters;
  filters.append("*.h");
 // filters.append("*.cpp");
  filters.append("*.hpp");
//  filters.append("*.in");

  qDebug() << currentDir;

  if(currentDir.dirName().compare("zRel") == 0 || currentDir.dirName().compare("Build") == 0)
  {
    return;
  }
  // Get a list of all the directories
  QFileInfoList dirList = currentDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
  if ( dirList.size() > 0 )
  {
    foreach(QFileInfo fi, dirList)
    {
      ReplaceLicenseCodeRecursively( QDir( fi.absoluteFilePath() ));   // Recursive call
    }
  }

  QFileInfoList itemList = currentDir.entryInfoList(filters);
  foreach(QFileInfo itemInfo, itemList)
  {
    QString itemFilePath = itemInfo.absoluteFilePath();
    //ReplaceLicenseText(itemFilePath);
    ReplaceIncludeGuard(itemFilePath);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReplaceMacros(QString absPath)
{
  QString filterName;
  QString contents;
  {
    // Read the Source File
    QFileInfo fi(absPath);
//        if (fi.baseName().compare("CombineAttributeArrays") != 0)
//        {
//          return;
//        }
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


  QString searchString = "SIMPL_FILTER_PARAMETER";
  QString replaceString  = "SIMPL_INSTANCE_PROPERTY_DECL";
  QString cppReplaceString = "SIMPL_INSTANCE_PROPERTY_DEF";

  QString instanceString = "SIMPL_INSTANCE_PROPERTY";
  QStringList cppCodeLines;
  QVector<QString> outLines;
  QStringList list = contents.split(QRegExp("\\n"));
  QStringListIterator sourceLines(list);
  QStringList includes;

  while (sourceLines.hasNext())
  {
    QString line = sourceLines.next();

    if(line.contains(searchString))
    {
      line.replace(searchString, replaceString);
      didReplace = true;
      QString cppCode = line;
      cppCode.replace(replaceString, cppReplaceString);
      cppCode.replace(cppReplaceString + "(", cppReplaceString + "(" + filterName + ", ");
      cppCode = cppCode.trimmed();
      cppCodeLines << cppCode;
    }
    else if(line.contains(instanceString))
    {
      line.replace(instanceString, replaceString);
      didReplace = true;
      QString cppCode = line;
      cppCode.replace(replaceString, cppReplaceString);
      cppCode.replace(cppReplaceString + "(", cppReplaceString + "(" + filterName + ", ");
      cppCode = cppCode.trimmed();
      cppCodeLines << cppCode;
    }

     outLines.push_back(line);
  }

  writeOutput(didReplace, outLines, absPath);

  if(cppCodeLines.isEmpty() == false)
  {
      absPath.replace(".h", ".cpp");
      QFile source(absPath);
      source.open(QFile::ReadOnly);
      contents = source.readAll();
      source.close();

      outLines.clear();
      lines.clear();
      QStringList list = contents.split(QRegExp("\\n"));
      list.append(cppCodeLines.join("\n"));
      writeOutput(didReplace, list, absPath);
  }


}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateMarkdownImageNames(QString absPath)
{
  QString filterName;
  QString contents;
  {
    // Read the Source File
    QFileInfo fi(absPath);
//        if (fi.baseName().compare("InitializeSyntheticVolume") != 0)
//        {
//          return;
//        }
    filterName = fi.baseName();
    QFile source(absPath);
    source.open(QFile::ReadOnly);
    contents = source.readAll();
    source.close();
  }

  //qDebug() << absPath;

  QStringList names;
  bool didReplace = false;

  QVector<int> lines(0);


  QString searchString = "![";

  QVector<QString> outLines;
  QStringList list = contents.split(QRegExp("\\n"));
  QStringListIterator sourceLines(list);
  QStringList includes;

  while (sourceLines.hasNext())
  {
    QString line = sourceLines.next();

    if(line.contains(searchString))
    {
     //hou qDebug() << line;
      QStringList imageLinks = line.split("![");
      foreach(QString chunk, imageLinks)
      {
        //qDebug() << chunk;
        
        int rightParenIdx = chunk.lastIndexOf(')');
        // Look for the last ')' character which _should_ be the end of the image tag
        if(rightParenIdx > -1)
        {
            // Now look for the '(' character which _should_ be the start of the tag.
            int leftParenIdx = chunk.lastIndexOf('(');
            if(leftParenIdx > -1)
            {
              QString path = chunk.mid(leftParenIdx + 1, rightParenIdx - leftParenIdx - 1);
              //qDebug() << path;
              
              QFileInfo fi2(absPath);
              QString imagePath = fi2.path() + QDir::separator() + path;
              //qDebug() << imagePath;
              QStringList filters;
              QFileInfo fi3(imagePath);
              QDir currentDir(fi3.path());
              QFileInfoList itemList = currentDir.entryInfoList(filters);
              foreach(QFileInfo itemInfo, itemList)
              {
//                qDebug() << itemInfo.fileName().toLower();
//                qDebug() << fi3.fileName().toLower();
                QString filename_lowercase = itemInfo.fileName().toLower();
                if(filename_lowercase.compare(fi3.fileName().toLower(), Qt::CaseSensitive) == 0)
                {
                  // the filenames are the same if converted to all lower case. Now lets see
                  // if they are the same if no conversion is performed.
                  if(itemInfo.fileName().compare(fi3.fileName(), Qt::CaseSensitive) != 0)
                  {
                    // The filenames are different so we need to correct this
                    line = line.replace(fi3.fileName(), itemInfo.fileName());
                    didReplace = true;
                    qDebug() << "Replacing filename .... ";
                  }
                }
                
              }
            }
        }
       
      }
    }
    outLines.push_back(line);
  }

  writeOutput(didReplace, outLines, absPath);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReplaceGrepSearchesRecursively(QDir currentDir)
{

  QStringList filters;
  //filters.append("*.cpp");
  filters.append("*.md");

  if(currentDir.dirName().compare("zRel") == 0 || currentDir.dirName().compare("Build") == 0)
  {
    return;
  }
  // Get a list of all the directories
  QFileInfoList dirList = currentDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
  if ( dirList.size() > 0 )
  {
    foreach(QFileInfo fi, dirList)
    {
      ReplaceGrepSearchesRecursively( QDir( fi.absoluteFilePath() ));   // Recursive call
    }
  }

  QFileInfoList itemList = currentDir.entryInfoList(filters);
  foreach(QFileInfo itemInfo, itemList)
  {
    QString itemFilePath = itemInfo.absoluteFilePath();
    // ReplaceText(itemFilePath);
    //    std::cout << "-------------------------------------" << std::endl;
    //    std::cout << itemFilePath.toStdString() << std::endl;
    //    AddIGeometryIncludes(itemFilePath);
    //   GroupIncludes(itemFilePath);
    // ReplaceText(itemFilePath);
    //  ReplaceMacros(itemFilePath);
    UpdateMarkdownImageNames(itemFilePath);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{

  //QUuid(uint l, ushort w1, ushort w2, uchar b1, uchar b2, uchar b3, uchar b4, uchar b5, uchar b6, uchar b7, uchar b8)
  uint l = 100;
  ushort w1 = 200;
  ushort w2 = 300;
  uchar b1 = 'S';
  uchar b2 = 'I';
  uchar b3 = 'M';
  uchar b4 = 'P';
  uchar b5 = 'L';
  uchar b6 = 'i';
  uchar b7 = 'b';
  uchar b8 = '1';
  QUuid uuid = QUuid(l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8);
  qDebug() << uuid;

  QString path1 = QString("%1/%2/%3").arg("DataContainer").arg("AttributeMatrix").arg("DataArray");
  QUuid p1 = QUuid::createUuidV5(uuid, path1);
  qDebug() << p1;

  path1 = QString("%1/%2/%3").arg("DataContainer").arg("AttributeMatrix").arg("DataArray1");
  p1 = QUuid::createUuidV5(uuid, path1);
  qDebug() << p1;



  Q_ASSERT(false); // We don't want anyone to run this program.
  // Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("FilterParameterTool");

  //std::cout << "FilterParameterTool Starting. Version " << SIMPLib::Version::PackageComplete().toStdString() << std::endl;

#if 0
  // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();
  SIMPLibPluginLoader::LoadPluginFilters(fm);


  // Send progress messages from PipelineBuilder to this object for display
  qRegisterMetaType<PipelineMessage>();
  GenerateFilterParametersCode();
//  GenerateMarkDownDocs();
  GenerateFilterParametersCode();
#else
  ReplaceGrepSearchesRecursively( QDir ( D3DTools::GetDREAM3DProjDir() + "/Source" ) );
  ReplaceGrepSearchesRecursively( QDir ( D3DTools::GetDREAM3DProjDir() + "/../DREAM3D_Plugins" ) );
  ReplaceGrepSearchesRecursively( QDir ( D3DTools::GetDREAM3DProjDir() + "/ExternalProjects/SIMPL" ) );
  ReplaceGrepSearchesRecursively( QDir ( D3DTools::GetDREAM3DProjDir() + "/ExternalProjects/SIMPLView" ) );
  ReplaceGrepSearchesRecursively( QDir ( D3DTools::GetDREAM3DProjDir() + "/ExternalProjects/Plugins" ) );

#endif
  return 0;
}
