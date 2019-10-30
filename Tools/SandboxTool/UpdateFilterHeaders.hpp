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

    QString fName = fi.fileName();
    //    if(fi.baseName() != "DataArrayPathDisplay")
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

    // qDebug() << hFile;
    QVector<QString> outLines;
    QStringList list = contents.split(QRegExp("\\n"));
    QStringListIterator sourceLines(list);

    int32_t lineIndex = 0;

    bool hasSearchString = false;
    QString memoryInclude = "#include <memory>";
    int memoryIncludeIndex = 0;

    QString firstSIMPLInclude("#include ");
    int32_t firstSIMPLIncludeIndex = 0;

    QString sharedPtr = "std::shared_ptr<";
    bool sharedPtrUsed = false;

    while(sourceLines.hasNext())
    {
      QString line = sourceLines.next();

      if(line.startsWith(firstSIMPLInclude) && firstSIMPLIncludeIndex == 0)
      {
        firstSIMPLIncludeIndex = lineIndex;
      }

      if(line.trimmed() == memoryInclude)
      {
        memoryIncludeIndex = lineIndex;
      }

      if(line.contains(sharedPtr))
      {
        sharedPtrUsed = true;
      }

      lineIndex++;
      outLines.push_back(line);
    }

    if(sharedPtrUsed && memoryIncludeIndex == 0)
    {
      QString line = outLines[firstSIMPLIncludeIndex];
      line = "#include <memory>\n\n" + line;
      outLines[firstSIMPLIncludeIndex] = line;
      hasSearchString = true;
    }

    writeOutput(hasSearchString, outLines, hFile);

    //    if(hasSearchString && hFile.endsWith(".h"))
    //    {
    //      /*  *************************************** */
    //      // Update the .cpp file
    //      QString cppFile = hFile;
    //      cppFile.replace(".h", ".cpp");
    //      QFileInfo cppFi(cppFile);
    //      if(cppFi.exists())
    //      {
    //        {
    //          // Read the Source File
    //          QFile source(cppFile);
    //          source.open(QFile::ReadOnly);
    //          contents = source.readAll();
    //          source.close();
    //        }
    //        contents = contents + "\n" + definitionCode;
    //        outLines.clear();
    //        outLines.push_back(contents);
    //        outLines.push_back(QString("JUNK"));

    //        writeOutput(hasSearchString, outLines, cppFile);
    //      }
    //    }
  }
};
