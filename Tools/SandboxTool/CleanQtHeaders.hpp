#pragma once

#include <iostream>
#include <map>
#include <set>

#include <QtCore/QProcess>
#include <QtCore/QString>

#include "Sandbox.h"

#define K_START_ID 20

class CleanQtHeaders : public Sandbox
{

public:
  CleanQtHeaders() = default;

  void operator()(const QString& hFile)
  {
    QString contents;
    QFileInfo fi(hFile);

    //    if(fi.baseName().compare("AngReader") != 0)
    //    {
    //      return;
    //    }

    //  qDebug() << hFile;

    {
      // Read the Source File
      QFile source(hFile);
      source.open(QFile::ReadOnly);
      contents = source.readAll();
      source.close();
    }

    bool didReplace = false;

    QVector<QString> outLines;
    QStringList list = contents.split(QRegExp("\\n"));
    QStringListIterator sourceLines(list);

    int32_t lineIndex = 0;

    std::map<QString, int32_t> line2ClassMap;

    // First Pass is to analyze the header file
    while(sourceLines.hasNext())
    {
      //      if(lineIndex == 167)
      //      {
      //        std::cout << "here" << std::endl;
      //      }
      QString line = sourceLines.next();

      if(line.contains("<QtCore/QtDebug>"))
      {
        line = line.replace("<QtCore/QtDebug>", "<QtCore/QDebug>");
      }

      // Is this line including a QtHeader
      if(line.startsWith("#include <Qt"))
      {
        QString includeLine = line;

        includeLine = includeLine.replace("#include ", "");
        includeLine = includeLine.replace("<", "");
        includeLine = includeLine.replace(">", "");

        QStringList tokens = includeLine.split("/");
        if(tokens.size() == 2)
        {
          QString qtClass = tokens[1].trimmed();
          if(qtClass != "QDebug")
          {
            line2ClassMap.insert({qtClass, lineIndex});
          }
        }
      }
      else
      {
        QVector<QString> toRemove;
        for(const auto& pair : line2ClassMap)
        {
          if(line.contains(pair.first))
          {
            toRemove.push_back(pair.first);
          }
        }
        for(const auto& item : toRemove)
        {
          line2ClassMap.erase(item);
        }
      }

      outLines.push_back(line);
      lineIndex++;
    }

    std::set<int32_t> lines;
    if(!line2ClassMap.empty())
    {
      qDebug() << hFile << ": Qt Headers left over. " << line2ClassMap.size();
      for(const auto& pair : line2ClassMap)
      {
        qDebug() << "    " << pair.first;
        int32_t lineNum = pair.second;
        outLines[lineNum] = "//REMOVE_ME" + outLines[lineNum];
        lines.insert(lineNum);
      }
      didReplace = true;
    }

    QVector<QString> filteredOutLines;
    for(const auto& line : outLines)
    {
      if(!line.startsWith("//REMOVE_ME"))
      {
        filteredOutLines.push_back(line);
      }
    }
    outLines = filteredOutLines;
    if(didReplace)
    {
      writeOutput(didReplace, outLines, hFile);
    }
  }
};
