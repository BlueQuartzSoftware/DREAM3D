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
    bool isFilter = false;
    QString searchString = "virtual const QString getSubGroupName() override;";

    QVector<QString> outLines;
    QStringList list = contents.split(QRegExp("\\n"));
    QStringListIterator sourceLines(list);

    QString baseName = fi.baseName();
    QString cpyCtr = QString("%1(const %1&) = delete;").arg(baseName);
    int32_t cpyCtrLine = 0;

    QString mvCtr = QString("%1(%1&&) = delete;").arg(baseName);
    int32_t mvCtrLine = 0;

    QString cpyAssign = QString("%1& operator=(const %1&) = delete;").arg(baseName);
    int32_t cpyAssignLine = 0;

    QString mvAssign = QString("%1& operator=(%1&&) = delete;").arg(baseName);
    int32_t mvAssignLine = 0;

    QString cpyAssignOld = QString("void operator=(const %1&)").arg(baseName);
    int32_t cpyAssignOldLine = 0;

    QString cpyAssignRepl = QString("    %1& operator=(const %1&) = delete; // Copy Assignment Not Implemented").arg(baseName);
    QString mvAssign1 = QString("void operator=(const %1&);").arg(baseName);

    size_t lineIndex = 0;

    // First Pass is to analyze the header file
    while(sourceLines.hasNext())
    {
      QString line = sourceLines.next();
      if(line.contains(cpyCtr))
      {
        if(!outLines.last().contains("public:"))
        {
          outLines.push_back("  public:");
          lineIndex++;
          didReplace = true;
        }
        cpyCtrLine = lineIndex;
      }
      else if(line.contains(mvCtr))
      {
        mvCtrLine = lineIndex;
      }
      else if(line.contains(cpyAssign))
      {
        cpyAssignLine = lineIndex;
      }
      else if(line.contains(mvAssign))
      {
        mvAssignLine = lineIndex;
      }
      else if(line.contains(cpyAssignOld))
      {
        cpyAssignOldLine = lineIndex;
      }
      else if(line.contains("virtual ~") && !isFilter)
      {
        QString replaceString = line;
        replaceString = replaceString.replace("virtual ", "");
        // replaceString = replaceString.replace(";", " override;");
        // outLines.push_back(replaceString);
        didReplace = true;
      }
      else if(line.contains(": public AbstractFilter"))
      {
        // std::cout << "isFilter = True" << std::endl;
        isFilter = true;
        // outLines.push_back(line);
      }
      else
      {
        // outLines.push_back(line);
      }
      outLines.push_back(line);
      lineIndex++;
    }

    if(cpyCtrLine == 0)
    {
      std::cout << "#### " << hFile.toStdString() << std::endl;
      return;
    }

    if(mvCtrLine == 0)
    {
      QString line = outLines[cpyCtrLine];
      line = line + "\n" + "    " + mvCtr + " // Move Constructor Not Implemented";
      outLines[cpyCtrLine] = line;
      mvCtrLine = cpyCtrLine;
      didReplace = true;
    }
    if(cpyAssignLine == 0)
    {
      QString line = outLines[mvCtrLine];
      line = line + "\n" + "    " + cpyAssign + " // Copy Assignment Not Implemented";
      outLines[mvCtrLine] = line;
      cpyAssignLine = cpyCtrLine;
      didReplace = true;
    }
    if(mvAssignLine == 0)
    {
      QString line = outLines[cpyAssignLine];
      line = line + "\n" + "    " + mvAssign + " // Move Assignment Not Implemented";
      outLines[cpyAssignLine] = line;
      mvAssignLine = cpyCtrLine;
      didReplace = true;
    }
    if(cpyAssignOldLine != 0)
    {
      outLines[cpyAssignOldLine] = "";
      didReplace = true;
    }

    // didReplace = false;
    writeOutput(didReplace, outLines, hFile);

    if(didReplace)
    {
      // std::cout << hFile.toStdString() << std::endl;
      if(cpyCtrLine)
      {
        std::cout << "  cpyCtrLine: " << cpyCtrLine << std::endl;
      }
      if(mvCtrLine)
      {
        std::cout << "  mvCtrLine: " << mvCtrLine << std::endl;
      }
      if(cpyAssignLine)
      {
        std::cout << "  cpyAssignLine: " << cpyAssignLine << std::endl;
      }
      if(mvAssignLine)
      {
        std::cout << "  mvAssignLine: " << mvAssignLine << std::endl;
      }
      if(cpyAssignOldLine)
      {
        std::cout << "  cpyAssignOldLine: " << cpyAssignOldLine << std::endl;
      }
    }
  }
};
