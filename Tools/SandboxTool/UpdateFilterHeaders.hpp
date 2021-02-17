#pragma once

#include <iostream>

#include <QtCore/QString>

#include "Sandbox.h"

class UpdateFilterHeaders : public Sandbox
{

public:
  UpdateFilterHeaders() = default;

  // -----------------------------------------------------------------------------
  bool positionInHeader(QString& contents, const QString& var, bool isPointer, QString arg)
  {

    // QStringList names;
    bool found = false;
    int lineNum = 1;
    QString searchString = var;

    QStringList list = contents.split(QRegExp("\\n"));
    QStringListIterator sourceLines(list);
    QStringList outLines;
    searchString = var;
    list = contents.split(QRegExp("\\n"));
    sourceLines.toFront(); // reset the iterator to the beginning
    lineNum = 1;

    QString privateSection = "private:";
    bool inPrivateSection = false;
    bool didReplace = false;
    bool isFilter = false;
    while(sourceLines.hasNext())
    {
      QString line = sourceLines.next();
      if(line.contains(": public AbstractFilter") && line.startsWith("class"))
      {
        isFilter = true;
      }
      else if(line.trimmed().simplified() == "public:" || line.trimmed().simplified() == "protected:")
      {
        inPrivateSection = false;
      }
      else if(line.trimmed().simplified() == "private:")
      {
        inPrivateSection = true;
      }
      else if(line.contains(searchString) && inPrivateSection)
      {
        int offset = line.indexOf(searchString);
        int size = searchString.size();
        if(line[offset + size] != " ")
        {
          outLines.push_back(line);
          continue;
        }
        int openBrace = line.indexOf("{");
        int closeBrace = line.lastIndexOf("}");
        bool isDataArrayPathClass = line.trimmed().startsWith("DataArrayPath");
        if(line.trimmed()[13] != " ")
        {
          isDataArrayPathClass = false;
        }
        if(isDataArrayPathClass && !arg.contains(",") && !arg.isEmpty())
        {
          arg = arg + ", \"\", \"\"";
        }
        // See if there is already a braced initializer, if there is, lets substitute the arg into it.
        if(openBrace > 0 && closeBrace > openBrace)
        {
          line = line.replace("{", "{" + arg);
          didReplace = true;
        }
      }
      outLines.push_back(line);
    }

    contents = outLines.join("\n");

    return didReplace && isFilter;
  }

  // -----------------------------------------------------------------------------
  bool fixInitializerList(QStringListIterator& sourceLines, QStringList& outLines, QString& hFileContents, const QString& cppFile)
  {
    bool isPointer = false;
    bool didReplace = false;

    // get through the initializer list
    while(sourceLines.hasNext())
    {
      QString line = sourceLines.next();
      if(line.contains("{"))
      {
        outLines.push_back(line);
        line = sourceLines.next();

        if(line.contains("}"))
        {
          outLines.back() = " = default;";
          didReplace = true;
        }
        else
        {
          outLines.push_back(line);
        }
        break;
      }
      int colonOffset = line.indexOf(":");
      int offset = line.indexOf("m_");
      int offset2 = line.indexOf("(");
      int offset3 = line.lastIndexOf(")");
      QString var = line.mid(offset, offset2 - offset);
      QString arg = line.mid(offset2 + 1, offset3 - offset2 - 1);
      if(line.contains("(nullptr)"))
      {
        isPointer = true;
      }
      else
      {
        isPointer = false;
      }
      didReplace = positionInHeader(hFileContents, var, isPointer, arg);
      if(!didReplace)
      {
        outLines.push_back(line);
      }
      //   qDebug() << "index: " << index << "   var:" << var;
    }
    return didReplace;
  }

#define SPECIFIC_CLASS 0
  // -----------------------------------------------------------------------------
  void operator()(const QString& cppFile)
  {

    QString contents;
    QFileInfo fi(cppFile);
    {
// Read the Source File
//
#if SPECIFIC_CLASS
      if(fi.baseName() != "TriangleDihedralAngleFilter")
      {
        return;
      }
#endif
      QFile source(cppFile);
      source.open(QFile::ReadOnly);
      contents = source.readAll();
      source.close();
    }

    QString hFile = cppFile;
    hFile.replace(".cpp", ".h");

    QString hFileContents;
    QFileInfo hfi(hFile);
    {
// Read the Source File
//
#if SPECIFIC_CLASS
      if(hfi.baseName() != "TriangleDihedralAngleFilter")
      {
        return;
      }
#endif
      QFile source(hFile);
      source.open(QFile::ReadOnly);
      hFileContents = source.readAll();
      source.close();
    }

    QStringList names;
    bool didReplace = false;

    QString searchString = fi.baseName() + "::" + fi.baseName();
    QStringList outLines;
    QStringList list = contents.split(QRegExp("\\n"));
    QStringListIterator sourceLines(list);

    int index = 0;
    while(sourceLines.hasNext())
    {
      QString line = sourceLines.next();
      if(line.contains(searchString))
      {
        outLines.push_back(line);
        // line = sourceLines.next();
        // outLines.push_back(line); // get the call to the superclass

        didReplace = fixInitializerList(sourceLines, outLines, hFileContents, cppFile);
      }
      else
      {
        outLines.push_back(line);
      }
    }

    writeOutput(didReplace, outLines, cppFile);
    writeOutput(didReplace, hFileContents.split(QRegExp("\\n")), hFile);
    index++;
  }
};
// asdfasdf
