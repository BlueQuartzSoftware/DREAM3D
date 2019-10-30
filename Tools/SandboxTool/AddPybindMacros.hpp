#pragma once

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtCore/QTextStream>

#include "Sandbox.h"

class AddPybindMacros : public Sandbox
{

public:
  AddPybindMacros() = default;

  void operator()(const QString& hFile)
  {
    QString msg;
    QTextStream out(&msg);
    out << hFile;

    QString contents;
    QFileInfo fi(hFile);

    if(fi.absoluteFilePath().contains("Wrapping"))
    {
      return;
    }
    if(fi.baseName() == ("SIMPLibSetGetMacros"))
    {
      return;
    }
    if(fi.baseName() == ("RemoveSIMPLMacros"))
    {
      return;
    }
    {
      // Read the Source File
      QFile source(hFile);
      source.open(QFile::ReadOnly);
      contents = source.readAll();
      source.close();
    }

    QString simplibInclude("#include \"SIMPLib/SIMPLib.h\"");
    int32_t simplibIncludeIndex = 0;

    QString pybindStart("PYB11_CREATE_BINDINGS");
    int32_t pybindStartIndex = 0;

    int32_t lastPybindMacroIndex = 0;

    QString _SIMPL_STATIC_NEW_MACRO("SIMPL_STATIC_NEW_MACRO");
    int32_t _SIMPL_STATIC_NEW_MACRO_Idx = 0;

    QString _SIMPL_FILTER_NEW_MACRO("SIMPL_FILTER_NEW_MACRO");
    int32_t _SIMPL_FILTER_NEW_MACRO_Idx = 0;

    QString _SIMPL_SHARED_POINTERS("SIMPL_SHARED_POINTERS");
    int32_t _SIMPL_SHARED_POINTERS_Idx = 0;

    QString _SIMPL_FILTER_PARAMETER("SIMPL_FILTER_PARAMETER");
    int32_t _SIMPL_FILTER_PARAMETER_Idx = 0;
    QString pybindGuard("#ifdef SIMPL_ENABLE_PYTHON");
    int32_t pybindGuardIndex = 0;

    bool hasSearchString = false;

    QString firstSIMPLInclude("#include \"SIMPLib/");
    int32_t firstSIMPLIncludeIndex = 1820966689;

    QVector<QString> outLines;
    QStringList list = contents.split(QRegExp("\\n"));
    QStringListIterator sourceLines(list);

    int32_t lineIndex = 0;

    while(sourceLines.hasNext())
    {
      QString line = sourceLines.next();
      QString compareLine = line.trimmed();

      if(compareLine == simplibInclude)
      {
        simplibIncludeIndex = lineIndex;
      }

      if(compareLine.startsWith(pybindStart))
      {
        pybindStartIndex = lineIndex;
      }
      if(compareLine.startsWith(pybindGuard))
      {
        pybindGuardIndex = lineIndex;
      }

      if(compareLine.startsWith("PYB11_"))
      {
        lastPybindMacroIndex = lineIndex;
      }

      if(compareLine.startsWith(_SIMPL_STATIC_NEW_MACRO))
      {
        _SIMPL_STATIC_NEW_MACRO_Idx = lineIndex;
        compareLine = compareLine.replace("SIMPL_STATIC_NEW_MACRO", "  PYB11_STATIC_NEW_MACRO");
        outLines[pybindStartIndex] = outLines[pybindStartIndex] + "\n" + compareLine;
      }
      if(compareLine.startsWith(_SIMPL_FILTER_NEW_MACRO))
      {
        _SIMPL_FILTER_NEW_MACRO_Idx = lineIndex;
        compareLine = compareLine.replace("SIMPL_FILTER_NEW_MACRO", "  PYB11_FILTER_NEW_MACRO");
        outLines[pybindStartIndex] = outLines[pybindStartIndex] + "\n" + compareLine;
      }
      if(compareLine.startsWith(_SIMPL_SHARED_POINTERS))
      {
        _SIMPL_SHARED_POINTERS_Idx = lineIndex;
        compareLine = compareLine.replace("SIMPL_SHARED_POINTERS", "  PYB11_SHARED_POINTERS");
        outLines[pybindStartIndex] = outLines[pybindStartIndex] + "\n" + compareLine;
      }
      if(compareLine.startsWith(_SIMPL_FILTER_PARAMETER))
      {
        _SIMPL_FILTER_PARAMETER_Idx = lineIndex;
        compareLine = compareLine.replace("SIMPL_FILTER_PARAMETER", "  PYB11_FILTER_PARAMETER");
        outLines[pybindStartIndex] = outLines[pybindStartIndex] + "\n" + compareLine;
      }

      if(compareLine.startsWith(firstSIMPLInclude) && lineIndex < firstSIMPLIncludeIndex)
      {
        firstSIMPLIncludeIndex = lineIndex;
      }

      lineIndex++;
      outLines.push_back(line);
    }

    if(pybindGuardIndex == 0 && pybindStartIndex > 0 && lastPybindMacroIndex > 0)
    {

      QString line;
      QTextStream out(&line);
      out << "\n"
          << "#ifdef SIMPL_ENABLE_PYTHON\n"
          << outLines.at(pybindStartIndex);

      // outLines[pybindStartIndex] = line;

      line.clear();
      out << outLines.at(lastPybindMacroIndex) << "\n"
          << "#endif\n\n";
      // outLines[lastPybindMacroIndex] = line;
      hasSearchString = true;

      line.clear();
      // out << simplibInclude << "\n" << outLines[firstSIMPLIncludeIndex];
      // outLines[firstSIMPLIncludeIndex] = line;
      // outLines.remove(simplibIncludeIndex);
    }

    writeOutput(hasSearchString, outLines, hFile);
  }
};
