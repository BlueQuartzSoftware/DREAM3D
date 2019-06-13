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

    QVector<QString> outLines;
    QStringList list = contents.split(QRegExp("\\n"));
    QStringListIterator sourceLines(list);

    int32_t lineIndex = 0;

    bool hasSearchString = false;
    QString dcaInclude = "#include \"SIMPLib/DataContainers/DataContainerArray.h\"";
    int dcaIncludeIndex = 0;

    QString dcaInstance = "DataContainerArray::Pointer";
    int32_t dcaInstanceIndex = 0;

    QString firstSIMPLInclude("#include \"SIMPLib/");
    int32_t firstSIMPLIncludeIndex = 0;

    QString firstSVWInclude("#include \"SVWidgetsLib/");
    int32_t firstSVWIncludeIndex = 0;

    while(sourceLines.hasNext())
    {
      QString line = sourceLines.next();

      if(line.trimmed() == dcaInclude)
      {
        dcaIncludeIndex = lineIndex;
      }

      if(line.contains(dcaInstance))
      {
        // line.replace(dcaInstance, "IDataArrayShPtrType");
        dcaInstanceIndex = lineIndex;
      }

      if(line.startsWith(firstSIMPLInclude) && lineIndex > firstSIMPLIncludeIndex)
      {
        firstSIMPLIncludeIndex = lineIndex;
      }
      if(line.startsWith(firstSVWInclude) && lineIndex > firstSVWIncludeIndex)
      {
        firstSVWIncludeIndex = lineIndex;
      }

      lineIndex++;
      outLines.push_back(line);
    }

    if(dcaInstanceIndex > 0)
    {
      if(firstSIMPLIncludeIndex == 0 && firstSVWIncludeIndex > 0)
      {
        firstSIMPLIncludeIndex = firstSVWIncludeIndex;
      }
      if(dcaIncludeIndex == 0 && firstSIMPLIncludeIndex < outLines.size())
      {
        QString line = outLines[firstSIMPLIncludeIndex];
        line = line + "\n" + dcaInclude + "\n";
        outLines[firstSIMPLIncludeIndex] = line;
        hasSearchString = true;
      }
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
