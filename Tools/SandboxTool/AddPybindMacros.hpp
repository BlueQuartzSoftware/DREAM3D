#pragma once

#include <QtCore/QString>

#include "Sandbox.h"

class AddPybindMacros : public Sandbox
{

public:
  AddPybindMacros() = default;

  void operator()(const QString& absPath)
  {
    QString filterName;
    QString contents;
    {
      // Read the Source File
      QFileInfo fi(absPath);
      if(fi.baseName().compare("DataContainerReader") != 0)
      {
        return;
      }
      filterName = fi.baseName();
      QFile source(absPath);
      source.open(QFile::ReadOnly);
      contents = source.readAll();
      source.close();
    }

    qDebug() << absPath;

    QStringList names;
    bool didReplace = false;

    QVector<int> lines(0);
    QList<QString> pybindLines;

    int qObjectMacroLine = 0;
    int currentLine = 0;
    QString searchString = "SIMPL_FILTER_PARAMETER";
    QString instanceString = "SIMPL_INSTANCE_PROPERTY";
    QStringList cppCodeLines;
    QVector<QString> outLines;
    QStringList list = contents.split(QRegExp("\\n"));
    QStringListIterator sourceLines(list);
    QStringList includes;

    while(sourceLines.hasNext())
    {
      QString line = sourceLines.next();
      QString line2 = line;

      if(line.contains(searchString))
      {
        line = line.trimmed();
        QStringList tokens = line.split("(");
        tokens = tokens[1].replace(")", "").trimmed().split(", ");
        QString paramType = tokens[0];
        QString paramVarName = tokens[1];
        QString pybindMacro;
        QTextStream out(&pybindMacro);
        out << "    PYB11_PROPERTY(" << paramType << " " << paramVarName << " READ get" << paramVarName << " WRITE set" << paramVarName << ")";
        pybindLines.append(pybindMacro);
        qDebug() << pybindMacro;
        didReplace = true;
      }

      if(line.contains("Q_OBJECT"))
      {
        qObjectMacroLine = currentLine + 1;
      }

      outLines.push_back(line2);
      currentLine++;
    }

    outLines.insert(qObjectMacroLine, pybindLines.join("\n"));

    writeOutput(didReplace, outLines, absPath);
  }
};
