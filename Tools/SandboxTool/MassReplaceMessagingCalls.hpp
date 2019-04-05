#pragma once

#include <QtCore/QString>

#include "Sandbox.h"

class MassReplaceMessagingCalls : public Sandbox
{

public:
  MassReplaceMessagingCalls() = default;

  void operator()(const QString& hFile)
  {

    QString contents;
    QFileInfo fi(hFile);

    //    if(fi.baseName().compare("CreateFEAInputFiles") != 0)
    //    {
    //      return;
    //    }

    {
      // Read the Source File
      QFile source(hFile);
      source.open(QFile::ReadOnly);
      contents = source.readAll();
      source.close();
    }

    bool didReplace = false;
    QString setterSearchString = "@@SET_WARNING_CONDITION@@";
    QString errCodeSearchString = "@@WARNING_CODE@@";
    QString placedErrCodeString = "@@getWarningCondition()@@";
    QString placedErrCodeToken = "getWarningCondition()";

    QStringList outLines;
    QStringList list = contents.split(QRegExp("\\n"));
    QStringListIterator sourceLines(list);

    QString baseName = fi.baseName();

    int lineCount = 0;
    bool readLine = true;
    while(sourceLines.hasNext())
    {
      QString line;
      if (readLine)
      {
        line = sourceLines.next();
        lineCount++;
      }
      readLine = true;
      int startLine = lineCount;
      if(line.contains(errCodeSearchString))
      {
        QString errCodeLine = line;
        QStringList errorCodeParts = errCodeLine.trimmed().split("@@", QString::SplitBehavior::SkipEmptyParts);
        QString errCode = errorCodeParts[2];

        bool found = false;
        int counter = 0;
        while(sourceLines.hasNext() && counter < 20 && !found && readLine)
        {
          line = sourceLines.next();
          lineCount++;
          if(line.contains(setterSearchString))
          {
            found = true;
            didReplace = true;
            QString errorFuncLine = line;
            QStringList errorFuncParts = errorFuncLine.trimmed().split("@@", QString::SplitBehavior::KeepEmptyParts);
            QString placedErrCode = errorFuncParts[3].trimmed();
            if(placedErrCode.contains(placedErrCodeToken) == false && placedErrCode != errCode)
            {
              qDebug() << fi.fileName() << ":" << startLine << " Warning code that was set does not match error code passed into notifyWarningCondition!";
            }

            placedErrCode.prepend("@@");
            placedErrCode.append("@@");
            line.replace(placedErrCode, errCode);

            line.replace(setterSearchString, "setWarningCondition");
          }
          else if (line.contains(errCodeSearchString))
          {
            readLine = false;
          }

          outLines.push_back(line);
          counter++;
        }

        if(!found)
        {
          qDebug() << fi.fileName() << ":" << startLine << "Did NOT find matching " << setterSearchString << " for " << errCodeSearchString;
        }
      }
      else if(line.contains(setterSearchString))
      {
        QString errorFuncLine = line;
        QStringList errorFuncParts = errorFuncLine.trimmed().split("@@", QString::SplitBehavior::KeepEmptyParts);
        QString errCode = errorFuncParts[3].trimmed();

        QString placedErrCode = errCode;
        placedErrCode.prepend("@@");
        placedErrCode.append("@@");
        line.replace(placedErrCode, errCode);

        line.replace(setterSearchString, "setWarningCondition");

        outLines.push_back(line);
      }
      else
      {
        outLines.push_back(line);
      }
    }

    writeOutput(didReplace, outLines, hFile);
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

    // QStringList names;
    bool found = false;
    int lineNum = 1;
    QString searchString = var;

    QStringList list = contents.split(QRegExp("\\n"));
    QStringListIterator sourceLines(list);
    // Look for raw pointers FIRST otherwise we get the wrong line.
    if(found == false && isPointer == true)
    {
      searchString = var;
      list = contents.split(QRegExp("\\n"));
      sourceLines.toFront(); // reset the iterator to the beginning

      lineNum = 1;
      while(sourceLines.hasNext())
      {
        QString line = sourceLines.next();
        if(line.contains(searchString) && line.contains("DEFINE_DATAARRAY_VARIABLE"))
        {
          int index = line.indexOf(var); // Verify we actually found the whole word
          if(line.at(index - 1).isSpace())
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
      while(sourceLines.hasNext())
      {
        QString line = sourceLines.next();
        if(line.contains(searchString) && line.contains("DEFINE_DATAARRAY_VARIABLE"))
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
    while(sourceLines.hasNext())
    {
      QString line = sourceLines.next();
      if(line.contains(searchString))
      {
        int offset = line.indexOf(searchString);
        int size = searchString.size();
        // Make sure we find the whole word?
        if(line.at(offset + size).isLetterOrNumber())
        {
          continue;
        }
        if(line.contains("Q_PROPERTY"))
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
      while(sourceLines.hasNext())
      {
        QString line = sourceLines.next();
        if(line.contains(searchString) && line.contains("DEFINE_DATAARRAY_VARIABLE"))
        {
          int index = line.indexOf(searchString); // Verify we actually found the whole word
          if(index > -1 && line.at(index - 1).isSpace())
          {
            line = sourceLines.next();
            found = true;
            return (float)lineNum;
          }
        }
        lineNum++;
      }
    }

    if(found == false)
    {
      lineNum = -1;
    }

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
      if(line.contains("{"))
      {
        break;
      }

      int offset = line.indexOf("m_");
      int offset2 = line.indexOf("(");
      QString var = line.mid(offset + 2, offset2 - offset - 2);
      if(line.contains("(nullptr)"))
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

    // qDebug() << "--------------------------------";
    QMapIterator<float, QString> i(orderedInitList);
    while(i.hasNext())
    {
      i.next();
      QString line = i.value();
      if(line.trimmed().isEmpty())
      {
        continue;
      } // Eat the blank line
      if(i.hasNext() && line.endsWith(",") == false)
      {
        line = line + ",";
      }
      if(i.hasNext() == false && line.endsWith(","))
      {
        line = line.mid(0, line.size() - 1);
      }
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

  static QSet<QString> qobjectClasses;
  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void CheckForQObjectSuperClass(const QString& hFile)
  {
    QString contents;
    QFileInfo fi(hFile);
    //  if (fi.baseName().compare("CropImageGeometry") != 0)
    //  {
    //    return false;
    //  }
    {
      // Read the Source File
      QFile source(hFile);
      source.open(QFile::ReadOnly);
      contents = source.readAll();
      source.close();
    }

    bool didReplace = false;

    QString searchString = "";

    QStringList outLines;
    QStringList list = contents.split(QRegExp("\\n"));
    QStringListIterator sourceLines(list);

    int index = 0;
    while(sourceLines.hasNext())
    {
      QString line = sourceLines.next();
      if(line.contains("public AbstractFilter"))
      {
        //"main -> parse -> execute;"
        if(!qobjectClasses.contains(fi.baseName()))
        {
          qobjectClasses.insert(fi.baseName());
          std::cout << "QObject -> " << fi.baseName().toStdString() << ";" << std::endl;
          break;
        }
      }

      else if(line.startsWith("class"))
      {
        QStringList tokens = line.split(" ");
        for(auto qsubclass : qobjectClasses)
        {
          // QString adjustQSubClass = qsubclass;
          if(tokens.contains(qsubclass) && qsubclass != fi.baseName())
          {
            if(!qobjectClasses.contains(fi.baseName()))
            {
              qobjectClasses.insert(fi.baseName());
              std::cout << qsubclass.trimmed().toStdString() << " -> " << fi.baseName().toStdString() << ";" << std::endl;
              break;
            }
          }
        }

        outLines.push_back(line);
      }
    }

    writeOutput(didReplace, outLines, hFile);
    index++;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  bool ReplaceIncludeGuard(const QString& hFile)
  {
    QString contents;
    QFileInfo fi(hFile);
    {
      // Read the Source File
      if(fi.suffix().compare("h") != 0)
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
    while(sourceLines.hasNext())
    {
      QString line = sourceLines.next();
      if(line.contains(searchString))
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
};
