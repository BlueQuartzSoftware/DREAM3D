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
    
    // Read the Source File
    QFileInfo fi(absPath);
    //      if(fi.baseName().compare("DataContainerReader") != 0)
    //      {
    //        return;
    //      }
    if(!fi.exists())
    {
      return;
    }
    QDir d(absPath);
    d.cdUp();
    qDebug() << d.dirName();
    if(!d.dirName().endsWith("Filters"))
    {
      return;
    }
      

    filterName = fi.baseName();
    QFile source(absPath);
    source.open(QFile::ReadOnly);
    contents = source.readAll();
    source.close();

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
    #ifdef Q_OS_WIN
        QStringList list = contents.split(QRegExp("\\r\\n"));
    #else
        QStringList list = contents.split(QRegExp("\\n"));
    #endif
    
    QStringListIterator sourceLines(list);
    QStringList includes;

    int classDeclLine = -1;
    QString classDeclStr = QString("class %1 : public").arg(filterName, 1);
    
    
    while(sourceLines.hasNext())
    {
      QString line = sourceLines.next();
      QString line2 = line;

      if(line.contains(searchString))
      {
        line = line.trimmed();
        QStringList tokens = line.split("(");
        tokens = tokens[1].replace(")", "").trimmed().split(",");
        QString paramType = tokens[0].trimmed();
        QString paramVarName = tokens[1].trimmed();
        QString pybindMacro;
        QTextStream out(&pybindMacro);
       // out << "    PYB11_PROPERTY(" << paramType << " " << paramVarName << " READ get" << paramVarName << " WRITE set" << paramVarName << ")";
        pybindLines.append(pybindMacro);
       // qDebug() << pybindMacro;
        didReplace = true;
      }

      if(line.contains("Q_OBJECT"))
      {
        qObjectMacroLine = currentLine + 1;
        QString str;
        QTextStream out(&str);
       // out << "    PYB11_CREATE_BINDINGS(" << fi.baseName() << " SUPERCLASS AbstractFilter)";
        pybindLines.append(str);
      }

      if(line.contains(classDeclStr))
      {
        classDeclLine = currentLine;
      }

      outLines.push_back(line2);
      currentLine++;
    }

    if(classDeclLine > 0)
    {
      QDir d(absPath);
      d.cdUp();
      d.cdUp();
      if(d.dirName().endsWith("Filters"))
      {
        return;
      }
      //qDebug() << d.dirName() << "_EXPORT";
      QString replace = QString("class %1_EXPORT %2 : public").arg(d.dirName(), 1).arg(filterName, 2);
      QString orig = outLines[classDeclLine];
      orig = orig.replace(classDeclStr, replace);
      qDebug() << orig;
      outLines[classDeclLine] = orig;
      didReplace = true;
      
      int includeLine = classDeclLine--;
      while(includeLine > -1)
      {
        QString outLine = outLines[includeLine];
        // Work backwards until you find the first blank line
        if(outLine.size() == 0)
        {
          outLine = QString("\n\n#include \"%1/%2DLLExport.h\"\n\n").arg(d.dirName(), 1).arg(d.dirName(), 2);
          qDebug() << "Add DLL Export Header at Line " << includeLine;
          outLines[includeLine] = outLine;
          includeLine = -1;
        }
        includeLine--;
      }
    }

    //outLines.insert(qObjectMacroLine, pybindLines.join("\n"));
    writeOutput(didReplace, outLines, absPath);
  }
};
