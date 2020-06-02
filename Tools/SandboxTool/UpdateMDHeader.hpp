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
    // qDebug() << msg;

    QString contents;
    QFileInfo fi(hFile);

    //    if(fi.baseName().compare("Contributing") != 0)
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

    bool hasSearchString = false;

    QString searchString = "# ";

    QVector<QString> outLines;
    QStringList list = contents.split(QRegExp("\\n"));
    QStringListIterator sourceLines(list);

    int32_t lineIndex = 0;
    hasSearchString = true;
    // First Pass is to analyze the header file
    while(sourceLines.hasNext())
    {
      QString line = sourceLines.next();

      if(line.startsWith(searchString))
      {
        line = line.replace("#", "").trimmed();
        line = line + "\n=============";
      }

      lineIndex++;
      outLines.push_back(line);
    }

    while(outLines.back().isEmpty())
    {
      outLines.pop_back();
    }

    if(hasSearchString)
    {
      writeOutput(hasSearchString, outLines, hFile);
    }
  }
};
