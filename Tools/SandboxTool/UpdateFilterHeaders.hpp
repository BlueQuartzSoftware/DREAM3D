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

    //    if (fi.baseName().compare("FindNumFeatures") != 0)
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

    bool didReplace = false;
    bool isFilter = false;
    QString searchString = "virtual const QString getSubGroupName() override;";

    QStringList outLines;
    QStringList list = contents.split(QRegExp("\\n"));
    QStringListIterator sourceLines(list);

    QString baseName = fi.baseName();
    QString cpyCtr = QString("%1(const %1&) = delete;").arg(baseName);
    QString mvCtr = QString("%1(%1&&) = delete;").arg(baseName);
    QString cpyAssign = QString("void operator=(const %1&) = delete;").arg(baseName);
    QString cpyAssignRepl = QString("    %1& operator=(const %1&) = delete; // Copy Assignment Not Implemented").arg(baseName);
    QString mvAssign = QString("    %1& operator=(%1&&) = delete;      // Move Assignment Not Implemented").arg(baseName);
    QString mvAssign1 = QString("void operator=(const %1&);").arg(baseName);

    while(sourceLines.hasNext())
    {
      QString line = sourceLines.next();
      if(line.contains(cpyCtr) && isFilter)
      {
        // outLines.push_back("  public:");
        outLines.push_back(line);
        // outLines.push_back(mvCtr);
        didReplace = true;
      }
      else if(line.contains(cpyAssign) && isFilter)
      {
        outLines.push_back(cpyAssignRepl);
        outLines.push_back(mvAssign);
        didReplace = true;
      }
      else if(line.contains(mvAssign1) && isFilter)
      {
        outLines.push_back(cpyAssignRepl);
        outLines.push_back(mvAssign);
        didReplace = true;
      }
      else if(line.contains("virtual const QString getCompiledLibraryName() const override"))
      {
        QString replaceString = line;
        replaceString = replaceString.replace("virtual ", "");
        outLines.push_back(replaceString);
        didReplace = true;
      }
      else if(line.contains("virtual const QString getBrandingString() const override"))
      {
        QString replaceString = line;
        replaceString = replaceString.replace("virtual ", "");
        outLines.push_back(replaceString);
        didReplace = true;
      }
      else if(line.contains("virtual const QString getFilterVersion() const override;"))
      {
        QString replaceString = line;
        replaceString = replaceString.replace("virtual ", "");
        outLines.push_back(replaceString);
        didReplace = true;
      }
      else if(line.contains("virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;"))
      {
        QString replaceString = line;
        replaceString = replaceString.replace("virtual ", "");
        outLines.push_back(replaceString);
        didReplace = true;
      }
      else if(line.contains("virtual const QString getGroupName() const override;"))
      {
        QString replaceString = line;
        replaceString = replaceString.replace("virtual ", "");
        outLines.push_back(replaceString);
        didReplace = true;
      }
      else if(line.contains("virtual const QString getSubGroupName() const override;"))
      {
        QString replaceString = line;
        replaceString = replaceString.replace("virtual ", "");
        outLines.push_back(replaceString);
        didReplace = true;
      }
      else if(line.contains("virtual const QUuid getUuid() override;"))
      {
        QString replaceString = line;
        replaceString = replaceString.replace("virtual ", "");
        outLines.push_back(replaceString);
        didReplace = true;
      }
      else if(line.contains("virtual const QString getHumanLabel() const override;"))
      {
        QString replaceString = line;
        replaceString = replaceString.replace("virtual ", "");
        outLines.push_back(replaceString);
        didReplace = true;
      }
      else if(line.contains("virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;"))
      {
        QString replaceString = line;
        replaceString = replaceString.replace("virtual ", "");
        outLines.push_back(replaceString);
        didReplace = true;
      }

      else if(line.contains("virtual void setupFilterParameters() override;"))
      {
        QString replaceString = line;
        replaceString = replaceString.replace("virtual ", "");
        outLines.push_back(replaceString);
        didReplace = true;
      }
      else if(line.contains("virtual void execute() override;"))
      {
        QString replaceString = line;
        replaceString = replaceString.replace("virtual ", "");
        outLines.push_back(replaceString);
        didReplace = true;
      }
      else if(line.contains("virtual void preflight() override;"))
      {
        QString replaceString = line;
        replaceString = replaceString.replace("virtual ", "");
        outLines.push_back(replaceString);
        didReplace = true;
      }
      else if(line.contains("virtual ~") && isFilter)
      {
        QString replaceString = line;
        replaceString = replaceString.replace("virtual ", "");
        replaceString = replaceString.replace(";", " override;");
        outLines.push_back(replaceString);
        didReplace = true;
      }
      else if(line.contains("SIMPL_STATIC_NEW_MACRO") && isFilter)
      {
        QString replaceString = line;
        replaceString = replaceString.replace("SIMPL_STATIC_NEW_MACRO", "SIMPL_FILTER_NEW_MACRO");
        outLines.push_back(replaceString);
        didReplace = true;
      }
      else if(line.contains(": public AbstractFilter"))
      {
        std::cout << "isFilter = True" << std::endl;
        isFilter = true;
        outLines.push_back(line);
      }
      else
      {
        outLines.push_back(line);
      }
    }

    if(didReplace)
    {
      std::cout << hFile.toStdString() << std::endl;
    }

    writeOutput(didReplace, outLines, hFile);
  }
};
