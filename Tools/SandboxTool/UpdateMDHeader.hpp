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

    out << hFile << '\n';

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

    bool didReplace = false;
    bool isIOFilter = false;
    QString searchString = "=============";

    QVector<QString> outLines;
    QStringList list = contents.split(QRegExp("\\n"));
    QStringListIterator sourceLines(list);

    int32_t lineIndex = 0;

    // Get the first line
    QString zeroLine = sourceLines.next();
    if(zeroLine.startsWith("#"))
    {
      // The file is correctly formatted. Skip it and go on.
      return;
    }

    // Get the second line
    QString oneLine = sourceLines.next();
    if(oneLine.startsWith("===="))
    {
      // This doc needs updating
      zeroLine = "# " + zeroLine + " #";
      oneLine = "";
      outLines.push_back(zeroLine);
      outLines.push_back(oneLine);
      didReplace = true;
    }

    // First Pass is to analyze the header file
    while(sourceLines.hasNext())
    {
      QString line = sourceLines.next();

      outLines.push_back(line);
      lineIndex++;
    }

    if(didReplace && !isIOFilter)
    {
      std::cout << msg.toStdString() << std::endl;
      writeOutput(didReplace, outLines, hFile);
    }
  }
};
