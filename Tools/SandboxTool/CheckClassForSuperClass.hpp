#pragma once

#include <iostream>
#include <set>

#include <QtCore/QString>

#include "Sandbox.h"
static std::set<QString> qobjectClasses;

class CheckClassForSuperClass : public Sandbox
{

public:
  CheckClassForSuperClass() = default;
  
  std::set<QString> getQObjectClasses() { return qobjectClasses; }
  
  void operator()(const QString& hFile)
  {
    QString contents;
    QFileInfo fi(hFile);
    //  if (fi.baseName().compare("CropImageGeometry") != 0)
    //  {
    //    return false;
    //  }
    {
      // Read the Source File
      QFile source(hFile);
      source.open(QFile::ReadOnly);
      contents = source.readAll();
      source.close();
    }

    // bool didReplace = false;
    bool isFilter = false;
    QString searchString = "";

    QStringList outLines;
    QStringList list = contents.split(QRegExp("\\n"));
    QStringListIterator sourceLines(list);

    int index = 0;
    while(sourceLines.hasNext())
    {
      QString line = sourceLines.next();
      if(line.contains(": public AbstractFilter"))
      {
        //"main -> parse -> execute;"
        if(qobjectClasses.find(fi.baseName()) == qobjectClasses.end())
        {
          qobjectClasses.insert(fi.baseName());
          std::cout << "AbstractFilter -> " << fi.baseName().toStdString() << ";" << std::endl;
        }
        outLines.push_back(line);
        isFilter = true;
      }

      else if(line.startsWith("class"))
      {
        QStringList tokens = line.split(" ");
        for(auto qsubclass : qobjectClasses)
        {
          // QString adjustQSubClass = qsubclass;
          if(tokens.contains(qsubclass) && qsubclass != fi.baseName())
          {
            if(qobjectClasses.find(fi.baseName()) == qobjectClasses.end())
            {
              qobjectClasses.insert(fi.baseName());
              std::cout << qsubclass.trimmed().toStdString() << " -> " << fi.baseName().toStdString() << ";" << std::endl;
              isFilter = true;
            }
          }
        }

        outLines.push_back(line);
      }
//      else if(line.contains("SIMPL_STATIC_NEW_MACRO") && isFilter)
//      {
//        QString replaceString = line;
//        replaceString = replaceString.replace("SIMPL_STATIC_NEW_MACRO", "SIMPL_FILTER_NEW_MACRO");
//        outLines.push_back(replaceString);
//        didReplace = true;
//      }
      else
      {
        outLines.push_back(line);
      }
    }

    //writeOutput(didReplace, outLines, hFile);
    index++;
  }
};
