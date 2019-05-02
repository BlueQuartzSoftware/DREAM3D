#pragma once

#include <iostream>

#include <QtCore/QProcess>
#include <QtCore/QString>

#include "Sandbox.h"

#define K_START_ID 20

class UpdateRenameIds : public Sandbox
{

public:
  UpdateRenameIds() = default;

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

    int existingEnumDef = 0;
    int lastIncludeLine = 0;
    int idIndex = 1;
    bool didReplace = false;
    bool isIOFilter = false;
    QString searchString = "";

    QVector<QString> outLines;
    QStringList list = contents.split(QRegExp("\\n"));
    QStringListIterator sourceLines(list);

    int32_t lineIndex = 0;

    // First Pass is to analyze the header file
    while(sourceLines.hasNext())
    {
      QString line = sourceLines.next();
      if(line.contains(searchString))
      {
        out << "    Line: " << lineIndex + 1 << '\n';
        QString idString = QString(", AttributeMatrixID%1);").arg(idIndex + K_START_ID);
        line = line.replace(");", idString);

        didReplace = true;
        idIndex++;
      }
#if 0
      if(line.contains("return \"IO\";"))
      {
        isIOFilter = true;
      }
      if(line.contains("return SIMPL::FilterGroups::IOFilters;"))
      {
        isIOFilter = true;
      }
      if(line.contains("return SIMPL::FilterSubGroups::InputFilters;"))
      {
        isIOFilter = true;
      }
#endif
      if(line.startsWith("#include"))
      {
        lastIncludeLine = lineIndex;
      }

      if(line.contains("enum createdPathID : RenameDataPath::DataID_t"))
      {
        existingEnumDef = lineIndex;
      }

      outLines.push_back(line);
      lineIndex++;
    }

    if(existingEnumDef == 0)
    {
      QString code;
      QTextStream c(&code);
      c << "\n\n/* Create Enumerations to allow the created Attribute Arrays to take part in renaming */\n";
      c << "enum createdPathID : RenameDataPath::DataID_t {\n";
      for(int i = 0; i < idIndex; i++)
      {
        c << "  AttributeMatrixID" << i + K_START_ID << " = " << i + K_START_ID;
        if(idIndex > 1)
        {
          c << ",";
        }
        c << '\n';
      }
      c << "};\n";
      outLines.insert(lastIncludeLine + 1, code);
    }
    else
    {
      QString code;
      QTextStream c(&code);

      for(int i = 1; i < idIndex; i++)
      {
        c << "  AttributeMatrixID" << i + K_START_ID << " = " << i + K_START_ID << ",\n";
      }
      outLines.insert(existingEnumDef + 2, code);
    }

    //    if(isIOFilter)
    //    {
    //      QString program = "/usr/bin/open";
    //      QStringList arguments;
    //      arguments << hFile;

    //      QProcess* myProcess = new QProcess(nullptr);
    //      myProcess->start(program, arguments);
    //    }

    if(didReplace && !isIOFilter)
    {
      std::cout << msg.toStdString() << std::endl;
      writeOutput(didReplace, outLines, hFile);
    }
  }
};
