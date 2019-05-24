#pragma once

#include <iostream>

#include <QtCore/QProcess>
#include <QtCore/QString>

#include "Sandbox.h"

#define K_START_ID 20

class UpdateMDHeader : public Sandbox
{

public:
  UpdateMDHeader() = default;

  void operator()(const QString& hFile)
  {
    QString msg;
    QTextStream out(&msg);

    out << hFile;
    qDebug() << msg;

    QString contents;
    QFileInfo fi(hFile);

    //     if (fi.baseName().compare("IObserver") != 0)
    //     {
    //       return;
    //     }

    {
      // Read the Source File
      QFile source(hFile);
      source.open(QFile::ReadOnly);
      contents = source.readAll();
      source.close();
    }

    bool hasInclude = false;
    bool hasSearchString = false;

    QString searchString = "DEFINE_IDATAARRAY_VARIABLE";
    QString includString = "#include \"SIMPLib";

    int includeIndex = 0;

    QVector<QString> outLines;
    QStringList list = contents.split(QRegExp("\\n"));
    QStringListIterator sourceLines(list);

    int32_t lineIndex = 0;

    // First Pass is to analyze the header file
    while(sourceLines.hasNext())
    {
      QString line = sourceLines.next();

      if(line.startsWith(includString))
      {
        includeIndex = lineIndex;
      }
      if(line.contains(searchString))
      {
        hasSearchString = true;
      }
      if(line.contains("#include \"SIMPLib/DataArrays/IDataArray.h\""))
      {
        hasInclude = true;
      }
      lineIndex++;
      outLines.push_back(line);
    }

    if(!hasInclude && hasSearchString)
    {
      QString line = outLines[includeIndex];
      line = line + "\n#include \"SIMPLib/DataArrays/IDataArray.h\"";
      outLines[includeIndex] = line;
    }
    qDebug() << "Last SIMPLib Include Directive is line " << includeIndex;

    if(!hasInclude && hasSearchString)
    {

      writeOutput(hasSearchString, outLines, hFile);
    }
  }
};
