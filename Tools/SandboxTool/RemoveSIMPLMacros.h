#pragma once

#include <iostream>

#include <QtCore/QProcess>
#include <QtCore/QString>

#include "Sandbox.h"

#define K_START_ID 20

class RemoveSIMPLMacros : public Sandbox
{

public:
  RemoveSIMPLMacros() = default;

  // -----------------------------------------------------------------------------
  QString check_SIMPL_FILTER_PARAMETER(const QString& inLine, QTextStream& privCodeOut, QTextStream& definitionCodeOut, bool& hasSearchString, const QFileInfo& fi)
  {
    QString searchString = "SIMPL_FILTER_PARAMETER";
    QString pubCode;
    QTextStream pubCodeOut(&pubCode);
    QString line = inLine;
    if(line.trimmed().startsWith("//"))
    {
      return inLine;
    }
    if(inLine.contains(searchString))
    {
      hasSearchString = true;

      line = line.replace(searchString, "");
      line = line.replace("(", "");
      line = line.replace(")", "");
      QStringList tokens = line.split(",");

      QString varType = tokens[0].trimmed();
      QString varName = tokens[1].trimmed();

      //        QString code;
      //        QTextStream out(&code);

      // out << "  private:\n";
      privCodeOut << "    " << varType << " m_" << varName << " = {};\n";
      // out << "\n";
      // out << "  public: \n";
      pubCodeOut << "    /**\n    * @brief Setter property for " << varName << "\n    */\n";
      pubCodeOut << "    void set" << varName << "(const " << varType << "& value); \n";
      pubCodeOut << "    /**\n    * @brief Getter property for " << varName << "\n    * @return Value of " << varName << "\n    */\n";
      pubCodeOut << "    " << varType << " get" << varName << "() const;\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "void " << fi.baseName() << "::set" << varName << "(const " << varType << "& value)\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  m_" << varName << " = value;\n";
      definitionCodeOut << "}\n\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "" << varType << " " << fi.baseName() << "::get" << varName << "() const\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  return m_" << varName << ";\n";
      definitionCodeOut << "}\n\n";

      line = pubCode;
    }
    return line;
  }

  // -----------------------------------------------------------------------------
  QString check_SIMPL_INSTANCE_PROPERTY(const QString& inLine, QTextStream& privCodeOut, QTextStream& definitionCodeOut, bool& hasSearchString, const QFileInfo& fi)
  {
    QString searchString = "SIMPL_INSTANCE_PROPERTY";
    QString pubCode;
    QTextStream pubCodeOut(&pubCode);
    QString line = inLine;
    if(line.trimmed().startsWith("//"))
    {
      return inLine;
    }
    if(inLine.contains(searchString))
    {
      hasSearchString = true;

      line = line.replace(searchString, "");
      line = line.replace("(", "");
      line = line.replace(")", "");
      QStringList tokens = line.split(",");

      QString varType = tokens[0].trimmed();
      QString varName = tokens[1].trimmed();

      if(varType.contains("CallbackType"))
      {
        varType = fi.baseName() + "::" + varType;
      }
      //        QString code;
      //        QTextStream out(&code);

      // out << "  private:\n";
      privCodeOut << "    " << varType << " m_" << varName << " = {};\n";
      // out << "\n";
      // out << "  public: \n";
      pubCodeOut << "    /**\n    * @brief Setter property for " << varName << "\n    */\n";
      pubCodeOut << "    void set" << varName << "(const " << varType << "& value); \n";
      pubCodeOut << "    /**\n    * @brief Getter property for " << varName << "\n    * @return Value of " << varName << "\n    */\n";
      pubCodeOut << "    " << varType << " get" << varName << "() const;\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "void " << fi.baseName() << "::set" << varName << "(const " << varType << "& value)\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  m_" << varName << " = value;\n";
      definitionCodeOut << "}\n\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "" << varType << " " << fi.baseName() << "::get" << varName << "() const\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  return m_" << varName << ";\n";
      definitionCodeOut << "}\n\n";

      line = pubCode;
    }
    return line;
  }

  // -----------------------------------------------------------------------------
  QString check_SIMPL_INSTANCE_STRING_PROPERTY(const QString& inLine, QTextStream& privCodeOut, QTextStream& definitionCodeOut, bool& hasSearchString, const QFileInfo& fi)
  {
    QString searchString = "SIMPL_INSTANCE_STRING_PROPERTY";
    QString pubCode;
    QTextStream pubCodeOut(&pubCode);
    QString line = inLine;
    if(line.trimmed().startsWith("//"))
    {
      return inLine;
    }
    if(inLine.contains(searchString))
    {
      hasSearchString = true;

      line = line.replace(searchString, "");
      QString needsOverride = "";
      if(line.contains("_OVERRIDE"))
      {
        needsOverride = " override ";
        line = line.replace("_OVERRIDE", "");
      }
      line = line.replace("(", "");
      line = line.replace(")", "");
      // QStringList tokens = line.split(",");

      QString varType = "QString";
      QString varName = line.trimmed();

      //        QString code;
      //        QTextStream out(&code);

      // out << "  private:\n";
      privCodeOut << "    " << varType << " m_" << varName << " = {};\n";
      // out << "\n";
      // out << "  public: \n";
      pubCodeOut << "    /**\n    * @brief Setter property for " << varName << "\n    */\n";
      pubCodeOut << "    void set" << varName << "(const " << varType << "& value) " << needsOverride << ";\n";
      pubCodeOut << "    /**\n    * @brief Getter property for " << varName << "\n    * @return Value of " << varName << "\n    */\n";
      pubCodeOut << "    " << varType << " get" << varName << "() const " << needsOverride << ";\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "void " << fi.baseName() << "::set" << varName << "(const " << varType << "& value)\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  m_" << varName << " = value;\n";
      definitionCodeOut << "}\n\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "" << varType << " " << fi.baseName() << "::get" << varName << "() const\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  return m_" << varName << ";\n";
      definitionCodeOut << "}\n\n";

      line = pubCode;
    }
    return line;
  }

  // -----------------------------------------------------------------------------
  QString check_SIMPL_SHARED_POINTERS(const QString& inLine, QTextStream& privCodeOut, QTextStream& definitionCodeOut, bool& hasSearchString, const QFileInfo& fi)
  {
    QString searchString = "SIMPL_SHARED_POINTERS";
    QString pubCode;
    QTextStream pubCodeOut(&pubCode);
    QString line = inLine;
    if(line.trimmed().startsWith("//"))
    {
      return inLine;
    }
    if(inLine.contains(searchString))
    {
      hasSearchString = true;

      line = line.replace(searchString, "");
      line = line.replace("(", "");
      line = line.replace(")", "");
      QStringList tokens = line.split(",");

      QString thisClass = tokens[0].trimmed();

      pubCodeOut << "    using Self = " << thisClass << ";\n";
      pubCodeOut << "    using Pointer = std::shared_ptr<Self>;\n";
      pubCodeOut << "    using ConstPointer = std::shared_ptr<const Self>;\n";
      pubCodeOut << "    using WeakPointer = std::weak_ptr<" << thisClass << ">;\n";
      pubCodeOut << "    using ConstWeakPointer = std::weak_ptr<" << thisClass << ">;\n";
      pubCodeOut << "    static Pointer NullPointer();\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << fi.baseName() << "::Pointer " << fi.baseName() << "::NullPointer()\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  return Pointer(static_cast<" << thisClass << "*>(nullptr));\n";
      definitionCodeOut << "}\n\n";

      line = pubCode;
    }
    return line;
  }

  // -----------------------------------------------------------------------------
  QString check_SIMPL_FILTER_NEW_MACRO(const QString& inLine, QTextStream& privCodeOut, QTextStream& definitionCodeOut, bool& hasSearchString, const QFileInfo& fi)
  {
    QString searchString = "SIMPL_FILTER_NEW_MACRO";
    QString pubCode;
    QTextStream pubCodeOut(&pubCode);
    QString line = inLine;
    if(line.trimmed().startsWith("//"))
    {
      return inLine;
    }
    if(inLine.contains(searchString))
    {
      hasSearchString = true;

      line = line.replace(searchString, "");
      line = line.replace("(", "");
      line = line.replace(")", "");

      QString thisClass = line.trimmed();

      pubCodeOut << "    static std::shared_ptr<" << thisClass << "> New();\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "std::shared_ptr<" << thisClass << "> " << fi.baseName() << "::New()\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  struct make_shared_enabler : public " << thisClass << "  \n";
      definitionCodeOut << "  {\n";
      definitionCodeOut << "  };\n";
      definitionCodeOut << "  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();\n";
      definitionCodeOut << "  val->setupFilterParameters();\n";
      definitionCodeOut << "  return val;\n";
      definitionCodeOut << "}\n\n";

      line = pubCode;
    }
    return line;
  }

  // -----------------------------------------------------------------------------
  QString check_SIMPL_STATIC_NEW_MACRO(const QString& inLine, QTextStream& privCodeOut, QTextStream& definitionCodeOut, bool& hasSearchString, const QFileInfo& fi)
  {
    QString searchString = "SIMPL_STATIC_NEW_MACRO";
    QString pubCode;
    QTextStream pubCodeOut(&pubCode);
    QString line = inLine;
    if(line.trimmed().startsWith("//"))
    {
      return inLine;
    }
    if(inLine.contains(searchString))
    {
      hasSearchString = true;

      line = line.replace(searchString, "");
      line = line.replace("(", "");
      line = line.replace(")", "");

      QString thisClass = line.trimmed();

      pubCodeOut << "    static Pointer New();\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << fi.baseName() << "::Pointer " << fi.baseName() << "::New()\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  Pointer sharedPtr (new (" << thisClass << "));   \n";
      definitionCodeOut << "  return sharedPtr;\n";
      definitionCodeOut << "}\n\n";

      line = pubCode;
    }
    return line;
  }

  // -----------------------------------------------------------------------------
  QString check_SIMPL_TYPE_MACRO_SUPER_OVERRIDE(const QString& inLine, QTextStream& privCodeOut, QTextStream& definitionCodeOut, bool& hasSearchString, const QFileInfo& fi)
  {
    QString searchString = "SIMPL_TYPE_MACRO_SUPER_OVERRIDE";
    QString pubCode;
    QTextStream pubCodeOut(&pubCode);
    QString line = inLine;
    if(line.trimmed().startsWith("//"))
    {
      return inLine;
    }
    if(inLine.contains(searchString))
    {
      hasSearchString = true;

      line = line.replace(searchString, "");
      line = line.replace("(", "");
      line = line.replace(")", "");
      QStringList tokens = line.split(",");

      QString className = tokens[0].trimmed();
      QString superClassName = tokens[1].trimmed();

      pubCodeOut << "    /**\n    * @brief Returns the name of the class for " << className << "\n    */\n";
      pubCodeOut << "    const QString getNameOfClass() const override;\n";
      pubCodeOut << "    /**\n    * @brief Returns the name of the class for " << className << "\n    */\n";
      pubCodeOut << "    static QString ClassName();\n";

      // pubCodeOut << "    /**\n    * @brief Getter property for " << varName << "\n    * @return Value of " << varName << "\n    */\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "const QString " << fi.baseName() << "::getNameOfClass() const\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  return QString(\"" << className << "\");\n";
      definitionCodeOut << "}\n\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "QString " << fi.baseName() << "::ClassName()\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  return QString(\"" << className << "\");\n";
      definitionCodeOut << "}\n\n";

      line = pubCode;
    }

    return line;
  }

  // -----------------------------------------------------------------------------
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

    bool hasSearchString = false;

    QString searchString = "SIMPL_FILTER_PARAMETER";
    int includeIndex = 0;

    QVector<QString> outLines;
    QStringList list = contents.split(QRegExp("\\n"));
    QStringListIterator sourceLines(list);

    int32_t lineIndex = 0;

    int32_t privLineIndex = 0;
    QString privCode;
    QTextStream privCodeOut(&privCode);

    QString definitionCode;
    QTextStream definitionCodeOut(&definitionCode);

    while(sourceLines.hasNext())
    {
      QString line = sourceLines.next();
      if(line.trimmed() == "private:")
      {
        privLineIndex = lineIndex;
        //  qDebug() << privLineIndex;
      }

      line = check_SIMPL_SHARED_POINTERS(line, privCodeOut, definitionCodeOut, hasSearchString, fi);
      line = check_SIMPL_FILTER_NEW_MACRO(line, privCodeOut, definitionCodeOut, hasSearchString, fi);
      line = check_SIMPL_STATIC_NEW_MACRO(line, privCodeOut, definitionCodeOut, hasSearchString, fi);
      line = check_SIMPL_STATIC_NEW_MACRO(line, privCodeOut, definitionCodeOut, hasSearchString, fi);
      line = check_SIMPL_TYPE_MACRO_SUPER_OVERRIDE(line, privCodeOut, definitionCodeOut, hasSearchString, fi);
      line = check_SIMPL_FILTER_PARAMETER(line, privCodeOut, definitionCodeOut, hasSearchString, fi);
      line = check_SIMPL_INSTANCE_PROPERTY(line, privCodeOut, definitionCodeOut, hasSearchString, fi);
      line = check_SIMPL_INSTANCE_STRING_PROPERTY(line, privCodeOut, definitionCodeOut, hasSearchString, fi);

      lineIndex++;
      outLines.push_back(line);
    }

    if(hasSearchString)
    {

      if(privLineIndex == 0)
      {
        // Work backwards from the end of the file to find the end of the class definition
        int32_t numLines = outLines.size() - 1;
        for(int32_t i = numLines; i >= 0; i--)
        {
          QString line = outLines[i].trimmed();
          if(line == "};")
          {
            privLineIndex = i - 1;
            outLines[privLineIndex] = outLines[privLineIndex] + "\n\n  private:";
            break;
          }
        }
      }
      // We still didn't find the end of the class def.
      if(privLineIndex == 0)
      {
        qDebug() << "Line containing the private: section was not found. End of Class definition was not found. BAILING OUT NOW.";
        qDebug() << "    " << fi.absoluteFilePath();
        return;
      }
      QString line = outLines[privLineIndex];
      line = line + "\n" + privCode;
      outLines[privLineIndex] = line;
    }

    writeOutput(hasSearchString, outLines, hFile);

    if(hasSearchString)
    {
      /*  *************************************** */
      // Update the .cpp file
      QString cppFile = hFile;
      cppFile.replace(".h", ".cpp");
      QFileInfo cppFi(cppFile);
      if(cppFi.exists())
      {
        {
          // Read the Source File
          QFile source(cppFile);
          source.open(QFile::ReadOnly);
          contents = source.readAll();
          source.close();
        }
        contents = contents + "\n" + definitionCode;
        outLines.clear();
        outLines.push_back(contents);
        outLines.push_back(QString("JUNK"));

        writeOutput(hasSearchString, outLines, cppFile);
      }
    }
  }
};
