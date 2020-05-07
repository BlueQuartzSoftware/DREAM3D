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
#if 0
    QString cppFile = hFile;
    cppFile = cppFile.replace(".h", ".cpp");
    QFileInfo cfi(cppFile);
    if(!cfi.exists())
    {
      return;
    }
#endif
    // QString fName = fi.fileName();
    if(fi.baseName() != "ReadCtfData")
    {
      return;
    }
    qDebug() << hFile;
    // Read the Source File
    QFile source(hFile);
    source.open(QFile::ReadOnly);
    contents = source.readAll();
    source.close();

    QVector<QString> outLines;
    QStringList list = contents.split(QRegExp("\\n"));
    QStringListIterator sourceLines(list);

    int32_t lineIndex = 0;

    QString simplSearch = "#include \"SIMPLib/>";
    int lastIndex = 0;

    while(sourceLines.hasNext())
    {
      QString line = sourceLines.next();

      if(line.trimmed() == simplSearch)
      {
        std::cout << lineIndex << ": " << line.toStdString() << std::endl;
        if(lineIndex != lineIndex + 1)
        {
          std::cout << "Gap found at line " << lastIndex << std::endl;
        }
      }
      lineIndex++;
      outLines.push_back(line);
    }

    writeOutput(false, outLines, hFile);
  }
};
