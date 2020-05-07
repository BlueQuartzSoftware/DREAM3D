#pragma once

#include <iostream>

#include <QtCore/QString>

#include "Sandbox.h"

class UpdateFilterHeaders : public Sandbox
{

public:
  UpdateFilterHeaders() = default;

  void operator()(const QString& hFile)
  {
    QString contents;
    QFileInfo fi(hFile);

    // Verify that there is a matching .cpp file
    QString cppFile = hFile;
    cppFile = cppFile.replace(".h", ".cpp");
    QFileInfo cfi(cppFile);
    if(!cfi.exists())
    {
      // std::cout << "Cpp File does not exist: " << cppFile.toStdString() << std::endl;
      return;
    }
    //    QString fName = fi.fileName();
    //    if(fi.baseName() != "MultiEmmpmFilter")
    //    {
    //      return;
    //    }

    // Read the Source File
    QFile source(hFile);
    source.open(QFile::ReadOnly);
    contents = source.readAll();
    source.close();

    // qDebug() << hFile;
    QVector<QString> outLines;
    QStringList list = contents.split(QRegExp("\\n"));
    QStringListIterator sourceLines(list);

    int32_t lineIndex = 0;

    QString memoryInclude = "#include <memory>";
    int memoryIncludeIndex = 0;

    while(sourceLines.hasNext())
    {
      QString line = sourceLines.next();

      if(line.trimmed() == memoryInclude)
      {
        memoryIncludeIndex = lineIndex;
        break;
      }
      lineIndex++;
      outLines.push_back(line);
    }

    // Now read the .cpp file
    if(memoryIncludeIndex > 0)
    {
      outLines.clear();
      bool updateCppFile = false;
      // Read the Source File
      QFile source(cppFile);
      source.open(QFile::ReadOnly);
      contents = source.readAll();
      source.close();

      list = contents.split(QRegExp("\\n"));
      sourceLines = QStringListIterator(list);

      while(sourceLines.hasNext())
      {
        QString line = sourceLines.next();

        if(line.trimmed() == memoryInclude)
        {
          updateCppFile = true;
          memoryIncludeIndex = lineIndex;
        }
        else
        {
          outLines.push_back(line);
        }

        lineIndex++;
      }
      if(updateCppFile)
      {
        // std::cout << "Updating: " << cppFile.toStdString() << std::endl;
        writeOutput(updateCppFile, outLines, cppFile);
      }
    }
  }
};
